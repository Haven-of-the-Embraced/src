
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*   ROM 2.4 is copyright 1993-1998 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@hypercube.org)                *
*       Gabrielle Taylor (gtaylor@hypercube.org)               *
*       Brian Moore (zump@rom.org)                     *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/


#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "merc.h"
#include "interp.h"
#include "addict.h"

/*
 * Local functions.
 */
void    check_assist    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_dodge args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_block args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
 void check_killer    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_parry args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_shield_block     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    dam_message     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                            int dt, bool immune ) );
void    death_cry   args( ( CHAR_DATA *ch ) );
void    group_gain  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int xp_compute  args( ( CHAR_DATA *gch, CHAR_DATA *victim,
                int total_levels ) );
bool    is_safe     args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    is_aggsafe  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    make_corpse args( ( CHAR_DATA *ch ) );
void    one_hit     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    mob_hit     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    raw_kill    args( ( CHAR_DATA *victim ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    disarm      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_critical  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


/* experimental garou code */
int     get_armor_diff  args(( CHAR_DATA *ch, CHAR_DATA *victim, int dam_type));
void    d10_hit           args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int     d10_damdice     args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
int     d10_modifier    args( ( CHAR_DATA *ch ) );

extern bool DEBUG_MESSAGES;
/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj_next;
    bool room_trig = FALSE;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    ch_next = ch->next;

/* shapeshifting update placed here due to the fact that it's pretty easy and i don't
    have to do a major recode */

    if(!IS_NPC(ch) && ch->pcdata->shift_count)
    {
        ch->pcdata->shift_count--;
        if(ch->pcdata->shift_count == 0)
            do_function(ch, &do_shapeshift, "shift_count");
    }

    if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
        continue;

    if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
        multi_hit( ch, victim, TYPE_UNDEFINED );
    else
        stop_fighting( ch, FALSE );

    if ( ( victim = ch->fighting ) == NULL )
        continue;

    /*
     * Fun for the whole family!
     */
    check_assist(ch,victim);
    if ( IS_NPC( ch ) )
    {
        if(ch->race == race_lookup("vampire") && number_range(1,6) == 1)
        {
            act( "$n lunges forward and bites you, drawing forth your life essence!", ch, NULL, victim, TO_VICT );
            act( "$n lunges forward and bites $N, drawing forth their life essence!", ch, NULL, victim, TO_NOTVICT );
            if(!IS_NPC(victim) && IS_VAMP(victim))
                victim->pblood -= 2;
            damage( ch, victim, ch->level*2, gsn_magick, DAM_PIERCE, FALSE);
        }

        if ( HAS_TRIGGER_MOB( ch, TRIG_FIGHT ) )
        p_percent_trigger( ch, NULL, NULL, victim, NULL, NULL, TRIG_FIGHT );
        if ( HAS_TRIGGER_MOB( ch, TRIG_HPCNT ) )
        p_hprct_trigger( ch, victim );
        for ( obj = ch->carrying; obj; obj = obj_next )
        {
            obj_next = obj->next_content;

            if ( obj->wear_loc != WEAR_NONE && HAS_TRIGGER_OBJ( obj, TRIG_FIGHT ) )
            p_percent_trigger( NULL, obj, NULL, victim, NULL, NULL, TRIG_FIGHT );
        }

        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_FIGHT ) && room_trig == FALSE )
        {
            room_trig = TRUE;
            p_percent_trigger( NULL, NULL, ch->in_room, victim, NULL, NULL, TRIG_FIGHT );
        }
    }

    }

    return;
}

/* for auto assisting */
void check_assist(CHAR_DATA *ch,CHAR_DATA *victim)
{
    CHAR_DATA *rch, *rch_next;
    DOMAIN_DATA *dom;

    for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
    {
    rch_next = rch->next_in_room;

    if (is_affected(ch, gsn_earthmeld))
        continue;

    if (IS_AWAKE(rch) && rch->fighting == NULL && SAME_UMBRA(ch, rch))
    {
          if (RIDDEN(rch) == ch || MOUNTED(rch) == ch)
                multi_hit(rch,victim,TYPE_UNDEFINED);

        /* quick check for ASSIST_PLAYER */
        if (!IS_NPC(ch) && IS_NPC(rch)
        && IS_SET(rch->off_flags,ASSIST_PLAYERS)
        &&  rch->level + 6 > victim->level)
        {
        do_function(rch, &do_emote, "screams and attacks!");
        multi_hit(rch,victim,TYPE_UNDEFINED);
        continue;
        }

        /* Domain mobs assist domain ruler and players of allied clans.
         * -- DISABLED-- by player request. -matthew 2/10/19
        if (!IS_NPC(ch) && IS_NPC(rch) && IS_SET(ch->act2, PLR2_PVP) &&
        IS_SET(rch->act2, ACT2_INFLUENCE) &&
        (dom = rch->pIndexData->area->domain) != NULL &&
        (dom->clan == ch->clan || dom->relation[ch->clan] == RELATION_ALLIED))
        {
            do_function(rch, &do_emote, "screams and attacks!");
            multi_hit(rch, victim, TYPE_UNDEFINED);
            continue;
        }
        */

          /* PCs next */
        if (!IS_NPC(ch) || IS_AFFECTED(ch,AFF_CHARM))
        {
        if ( ( (!IS_NPC(rch) && IS_SET(rch->act,PLR_AUTOASSIST))
        ||     IS_AFFECTED(rch,AFF_CHARM))
        &&   is_same_group(ch,rch)
        &&   !is_safe(rch, victim)
        &&   !is_affected(rch,gsn_shadowform))
            multi_hit (rch,victim,TYPE_UNDEFINED);

        continue;
        }

        /* now check the NPC cases */

        if (IS_NPC(ch) && !IS_AFFECTED(ch,AFF_CHARM))

        {
        if ( (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALL))

        ||   (IS_NPC(rch) && rch->group && rch->group == ch->group)

        ||   (IS_NPC(rch) && rch->race == ch->race
           && IS_SET(rch->off_flags,ASSIST_RACE))

        ||   (rch->pIndexData == ch->pIndexData
           && IS_SET(rch->off_flags,ASSIST_VNUM)))

        {
            CHAR_DATA *vch;
            CHAR_DATA *target;
            int number;

            if (number_bits(1) == 0)
            continue;

            target = NULL;
            number = 0;
            for (vch = ch->in_room->people; vch; vch = vch->next)
            {
            if (can_see(rch,vch)
            &&  is_same_group(vch,victim)
            &&  number_range(0,number) == 0)
            {
                target = vch;
                number++;
            }
            }

            if (target != NULL)
            {
            do_function(rch, &do_emote, "screams and attacks!");
            multi_hit(rch,target,TYPE_UNDEFINED);
            }
        }
        }
    }
    }
}


/*
 * Do one group of attacks.
 */
void multi_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    int   chance, i, c, genbonus;
    int   stopclock;
    AFFECT_DATA af;

    /* decrement the wait */
    if (ch->desc == NULL)
    ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);
    if (ch->desc == NULL)
    ch->stopped = UMAX(0,ch->stopped - PULSE_VIOLENCE);
    if (ch->desc == NULL)
    ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE);

    if( is_affected(ch, gsn_sidesteptime))
      affect_strip(ch, gsn_sidesteptime);
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
      affect_strip( ch, gsn_unseen );
    if(IS_NPC(ch) || (!IS_NPC(ch) && ch->pcdata->discipline[OBFUSCATE] != 6))
    {
        REMOVE_BIT   ( ch->affected2_by, AFF2_VEIL       );
        affect_strip(ch, gsn_veil);
    }

//  Time 4 'Stop the Clock' -- If charges left, Decrement the affect before checking stunned.
    if (is_affected(ch, gsn_stoptheclock))
    {
      stopclock = get_affect_modifier(ch, gsn_stoptheclock);
      if (stopclock > 0)
      {
        af.where     = TO_AFFECTS;
        af.type      = gsn_stoptheclock;
        af.level     = get_affect_level(ch, gsn_stoptheclock);
        af.duration  = 0;
        af.location  = APPLY_NONE;
        af.modifier  = -1;
        af.bitvector = 0;
        affect_join(ch, &af);
        act("Your body seems to be frozen in time.", ch, NULL, victim, TO_CHAR);

        return;
      }
    }

    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
    return;


    if (IS_NPC(ch))
    {
    mob_hit(ch,victim,dt);
    return;
    }

    if (MOUNTED(ch))
    {
    if (!mount_success(ch, MOUNTED(ch), FALSE))
    send_to_char("You fall off your mount as you start your attacks!\n\r", ch);
    }

    one_hit( ch, victim, dt );

    if (ch->fighting != victim)
    return;

/* Replacement celerity for new buff-type celerity ability - Ugha*/
    if(!IS_NPC(ch) && is_affected(ch,gsn_celbuff) && dt != gsn_backstab)
        for(i = ch->pcdata->discipline[CELERITY];i > 0;i--)
        {
            if(number_range(1,65) <= (get_attribute(ch, DEXTERITY)*10))
                one_hit(ch,victim,dt);
        }


    /*Time Intrinsic. Acts like Celerity for Mages.
     */

    if(!IS_NPC(ch) && is_affected(ch,gsn_timealteration) > 0 && dt != gsn_backstab)
    {
        for(i = ch->sphere[SPHERE_TIME];i > 0;i--)
        {
            if(number_range(1, 100) <= (get_attribute(ch, DEXTERITY) *10))
                one_hit(ch,victim,dt);
        }
    }

    if(is_affected(ch,gsn_timealteration) && dt == gsn_backstab)
        one_hit(ch, victim, dt);

    if(!IS_NPC(ch) && is_affected(ch,gsn_rage) && dt != gsn_backstab)
    {
        for (i = 0; i < ch->pcdata->primal_urge; i++)
        {
            if ((godice(get_attribute(ch, DEXTERITY), 6)) > 0)
                one_hit(ch, victim, dt );
        }
    }

    if (ch->race == race_lookup("garou"))
    {
        CHAR_DATA *rch, *rch_next;
        for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
        {
            rch_next = rch->next_in_room;
            if ( !IS_NPC(rch) && rch->race == race_lookup("garou") && is_same_group(ch,rch) && rch != ch && dt != gsn_backstab)
            one_hit( ch, victim, dt );
        }
    }

     if (get_skill(ch,gsn_enhanced_speed) >= 1 && number_range(1,100) <= get_skill(ch,gsn_enhanced_speed) && number_range(1, 30) == 1)
    {
        act( "Your enhanced skill allows you to unleash a flurry of attacks!", ch, NULL, victim, TO_CHAR );
        act( "$n suddenly unleashes a flurry of attacks on you!", ch, NULL, victim, TO_VICT );
        act( "$n suddenly unleashes a flurry of attacks on $N", ch, NULL, victim, TO_ROOM );
        one_hit( ch, victim, dt );
        one_hit( ch, victim, dt );
        one_hit( ch, victim, dt );
        check_improve(ch,gsn_enhanced_speed,TRUE,6);
    }
    if (IS_AFFECTED(ch,AFF_HASTE))
    one_hit(ch,victim,dt);

    if ( ch->fighting != victim || dt == gsn_backstab )
    return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
    chance /= 2;

    if ( number_percent( ) < chance )
    {
    one_hit( ch, victim, dt );
    check_improve(ch,gsn_second_attack,TRUE,4);
    if ( ch->fighting != victim )
        return;
    }

    chance = get_skill(ch,gsn_third_attack)/3;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
    chance /= 4;

    if ( number_percent( ) < chance )
    {
    one_hit( ch, victim, dt );
    check_improve(ch,gsn_third_attack,TRUE,6);
    if ( ch->fighting != victim )
        return;
    }

    chance = get_skill(ch,gsn_fourth_attack)/4;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
    chance /= 6;

    if ( number_percent( ) < chance )
    {
    one_hit( ch, victim, dt );
    check_improve(ch,gsn_fourth_attack,TRUE,8);
    if ( ch->fighting != victim )
        return;
    }

    return;
}

/* procedure for all mobile attacks */
void mob_hit (CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    int chance,number, success;
    CHAR_DATA *vch, *vch_next;
    OBJ_DATA *obj;
    AFFECT_DATA af;

    if (IS_AFFECTED(ch, AFF_SLOW))
    {
      if (number_percent() < 75)
        one_hit(ch,victim,dt);
    }
    else
      one_hit(ch,victim,dt);

    if (ch->fighting != victim)
    return;

    /* Area attack -- BALLS nasty! */

    if (IS_SET(ch->off_flags,OFF_AREA_ATTACK))
    {
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
        vch_next = vch->next;
        if ((vch != victim && vch->fighting == ch))
        one_hit(ch,vch,dt);
    }
    }

    if (IS_AFFECTED(ch,AFF_HASTE)
    ||  (IS_SET(ch->off_flags,OFF_FAST) && !IS_AFFECTED(ch,AFF_SLOW)))
    one_hit(ch,victim,dt);
     if (IS_SET(ch->off_flags,OFF_SECOND_ATTACK))
    one_hit(ch,victim,dt);
      if (IS_SET(ch->off_flags,OFF_THIRD_ATTACK))
    one_hit(ch,victim,dt);
    if (IS_SET(ch->off_flags,OFF_FOURTH_ATTACK))
    one_hit(ch,victim,dt);

    if (ch->fighting != victim || dt == gsn_backstab)
    return;

    chance = get_skill(ch,gsn_second_attack)/2;

    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
    chance /= 2;

    if (number_percent() < chance)
    {
    one_hit(ch,victim,dt);
    if (ch->fighting != victim)
        return;
    }

    chance = get_skill(ch,gsn_third_attack)/4;

    if (IS_AFFECTED(ch,AFF_SLOW) && !IS_SET(ch->off_flags,OFF_FAST))
    chance = 0;

    if (number_percent() < chance)
    {
    one_hit(ch,victim,dt);
    if (ch->fighting != victim)
        return;
    }

    /* oh boy!  Fun stuff! */

    if (ch->wait > 0)
    return;

    if((success = number_range(0,ch->level/10)) != 0)
    {
        switch(number_range(1,5))
        {
            case (1): if (IS_SET(ch->off_flags,OFF_SPHERE_ENTROPY))
                        {
                            switch(number_range(1,3))
                            {
                                case (1):
                                        if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
                                            break;
                                        act( "$n snaps $s fingers and turns $N's weapon into dust!",  ch, NULL, victim, TO_NOTVICT );
                                        act( "$n snaps $s fingers and turns your weapon into dust!",  ch, NULL, victim, TO_VICT );
                                        extract_obj( obj );
                                        break;
                                case (2):
                                        act( "$n reaches out and touches $N who suddenly screams in agony!",  ch, NULL, victim, TO_NOTVICT );
                                        act( "You scream in agony as $n reaches out and touches you!",  ch, NULL, victim, TO_VICT );
                                        damage( ch, victim, success*800, gsn_magick, DAM_DISEASE, TRUE);
                                        break;
                                case (3):
                                        if(is_affected(victim,gsn_forget)) break;
                                        af.where     = TO_AFFECTS;
                                        af.type      = gsn_forget;
                                        af.level     = ch->level;
                                        af.duration  = ch->level;
                                        af.location  = APPLY_NONE;
                                        af.modifier  = 0;
                                        af.bitvector = 0;
                                        affect_to_char( victim, &af );
                                        act( "You suddenly feel very lightheaded!",  ch, NULL, victim, TO_VICT );
                                        break;
                            }
                        }
                        break;
            case (2): if (IS_SET(ch->off_flags,OFF_SPHERE_FORCES))
                        {
                            switch(number_range(1,3))
                            {
                                case (1):
                                        act( "$n's hair stands on end a moment before pointing at $N who gets a real charge out of it!",  ch, NULL, victim, TO_NOTVICT );
                                        act( "$n points at you and.. zzzzzZAP! You have an electrifying experience!",  ch, NULL, victim, TO_VICT    );
                                        damage( ch, victim, success*300, gsn_magick, DAM_LIGHTNING, TRUE);
                                        break;
                                case (2):
                                        act( "$n peers intently at $N who suddenly bursts into flame!",  ch, NULL, victim, TO_NOTVICT );
                                        act( "A wall of flame consumes you as a flash of heat engulfs your body and burns away at your skin!",  ch, NULL, victim, TO_VICT );
                                        damage( ch, victim, success*800, gsn_magick, DAM_FIRE, TRUE);
                                        break;
                                case (3):
                                        if(is_affected(ch,gsn_kineticshield)) break;
                                        af.where     = TO_AFFECTS;
                                        af.type      = gsn_kineticshield;
                                        af.level     = ch->level;
                                        af.duration  = ch->level;
                                        af.location  = APPLY_NONE;
                                        af.modifier  = 0;
                                        af.bitvector = 0;
                                        affect_to_char( ch, &af );
                                        act( "$n focuses as a shimmering bubble of energy surrounds $e.",  ch, NULL, victim, TO_ROOM );
                                        break;
                            }
                        }
                        break;
            case (3): if (IS_SET(ch->off_flags,OFF_SPHERE_LIFE))
                        {
                            ch->hit = UMIN( ch->hit + (success*100), ch->max_hit );
                            act("$n concentrates intently then suddenly seems to be feeling much better.",ch,NULL,victim,TO_ROOM);
                        }
                        break;
            case (4): /* Reserved for Mind */
                        break;
            case (5): if (IS_SET(ch->off_flags,OFF_SPHERE_TIME))
                        {
                            switch(number_range(1,3))
                            {
                                case (1):
                                        if(is_affected(ch,gsn_precognition)) break;
                                        af.where     = TO_AFFECTS;
                                        af.type      = gsn_precognition;
                                        af.level     = ch->level;
                                        af.duration  = ch->level;
                                        af.location  = APPLY_NONE;
                                        af.modifier  = 0;
                                        af.bitvector = 0;
                                        affect_to_char( ch, &af );

                                        act( "$n seems to be much more aware of their surroundings!",  ch, NULL, victim, TO_ROOM   );
                                        break;
                                case (2):
                                        if(is_affected(ch,gsn_timealteration)) break;
                                        af.where     = TO_AFFECTS;
                                        af.type      = gsn_timealteration;
                                        af.level     = ch->level;
                                        af.duration  = ch->level;
                                        af.location  = APPLY_NONE;
                                        af.modifier  = 0;
                                        af.bitvector = 0;
                                        affect_to_char( ch, &af );
                                        act( "$n suddenly seems to move much faster!",  ch, NULL, victim, TO_ROOM   );
                                        break;
                                case (3):
                                        WAIT_STATE(victim, PULSE_VIOLENCE);
                                        act( "$n waves a hand at $N who suddenly seems frozen!",  ch, NULL, victim, TO_NOTVICT    );
                                        act( "You suddenly find time rushing by too fast to perceive.",  ch, NULL, victim, TO_VICT );
                                        break;
                            }
                        }
                        break;
        }
    }
    number = number_range(0,2);

    if (number == 1 && IS_SET(ch->act,ACT_MAGE))
    {
    /*  { mob_cast_mage(ch,victim); return; } */ ;
    }

    if (number == 2 && IS_SET(ch->act,ACT_CLERIC))
    {
    /* { mob_cast_cleric(ch,victim); return; } */ ;
    }

    /* now for the skills */

    number = number_range(0,8);

    switch(number)
    {
    case (0) :
    if (IS_SET(ch->off_flags,OFF_BASH))
        do_function(ch, &do_bash, "");
    break;

    case (1) :
    if (IS_SET(ch->off_flags,OFF_BERSERK) && !IS_AFFECTED(ch,AFF_BERSERK))
        do_function(ch, &do_berserk, "");
    break;


    case (2) :
    if (IS_SET(ch->off_flags,OFF_DISARM)
    || (get_weapon_sn(ch) != gsn_hand_to_hand
    && (IS_SET(ch->act,ACT_WARRIOR)
    ||  IS_SET(ch->act,ACT_THIEF))))
        do_function(ch, &do_disarm, "");
    break;

    case (3) :
    if (IS_SET(ch->off_flags,OFF_KICK))
        do_function(ch, &do_kick, "");
    break;

    case (4) :
    if (IS_SET(ch->off_flags,OFF_KICK_DIRT))
        do_function(ch, &do_dirt, "");
    break;

    case (5) :
    if (IS_SET(ch->off_flags,OFF_TAIL))
    {
        /* do_function(ch, &do_tail, "") */ ;
    }
    break;

    case (6) :
    if (IS_SET(ch->off_flags,OFF_TRIP))
        do_function(ch, &do_trip, "");
    break;

    case (7) :
    if (IS_SET(ch->off_flags,OFF_CRUSH))
    {
        /* do_function(ch, &do_crush, "") */ ;
    }
    break;
    case (8) :
    if (IS_SET(ch->off_flags,OFF_BACKSTAB))
    {
        do_function(ch, &do_backstab, "");
    }
    }
}

/*
 * Hit one guy once.
 */
void one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
    OBJ_DATA *wield;
    int victim_ac;
    int thac0;
    int thac0_00;
    int thac0_32;
    int dam;
    int diceroll;
    int sn,skill;
    int dam_type;
    int genbonus;
    bool result;
    sn = -1;
    genbonus = 0;

    AFFECT_DATA af;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
    return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
    return;


    if (ch->stopped > 0)
    {
        return;
    }
    //d10_hit(ch,victim, dt);
    //return;

    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );

    // All Hits for Players get redirected to d10_hit for wod-style damage calculations.
    if(!IS_NPC(ch))
    {
        d10_hit(ch,victim, dt);
        return;
    }

    if ( dt == TYPE_UNDEFINED )
    {
    dt = TYPE_HIT;
    if ( wield != NULL && wield->item_type == ITEM_WEAPON )
        dt += wield->value[3];
    else if (is_affected(ch, gsn_claws))
        dt += 5;
    else
        dt += ch->dam_type;

    }

    if (dt < TYPE_HIT)
        if (wield != NULL)
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
    dam_type = DAM_BASH;

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = 20 + get_weapon_skill(ch,sn);

    /*
     * Calculate to-hit-armor-class-0 versus armor.
     */
    if ( IS_NPC(ch) )
    {
    thac0_00 = 20;
    thac0_32 = -4;   /* as good as a thief */
    if (IS_SET(ch->act,ACT_WARRIOR))
        thac0_32 = -10;
    else if (IS_SET(ch->act,ACT_THIEF))
        thac0_32 = -4;
    else if (IS_SET(ch->act,ACT_CLERIC))
        thac0_32 = 2;
    else if (IS_SET(ch->act,ACT_MAGE))
        thac0_32 = 6;
    }
    else
    {
    thac0_00 = 20;
    thac0_32 = -20;
    }
    thac0  = interpolate( ch->level, thac0_00, thac0_32 );

    if (thac0 < 0)
        thac0 = thac0/2;

    if (thac0 < -5)
        thac0 = -5 + (thac0 + 5) / 2;

    thac0 -= GET_HITROLL(ch) * skill/100;
    thac0 += 5 * (100 - skill) / 100;

    if (dt == gsn_backstab)
    thac0 -= 10 * (100 - get_skill(ch,gsn_backstab));

    switch(dam_type)
    {
    case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE)/10;   break;
    case(DAM_BASH):  victim_ac = GET_AC(victim,AC_BASH)/10;     break;
    case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH)/10;    break;
    default:     victim_ac = GET_AC(victim,AC_EXOTIC)/10;   break;
    };

    if (victim_ac < -15)
    victim_ac = (victim_ac + 15) / 5 - 15;

    if ( !can_see( ch, victim ) )
    victim_ac -= 4;

    if ( victim->position < POS_FIGHTING)
    victim_ac += 4;

    if (victim->position < POS_RESTING)
    victim_ac += 6;

    if (has_affect_modifier(victim, gsn_awe, ch->pIndexData->vnum))
      victim_ac -=10;

    /*
     * The moment of excitement!
     */
    while ( ( diceroll = number_bits( 5 ) ) >= 20 )
    ;

    if ( diceroll == 0
    || ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
    {
    /* Miss. */
    damage( ch, victim, 0, dt, dam_type, TRUE );
    tail_chain( );
    return;
    }

    /*
     * Hit.
     * Calc damage.
     */
    if ( IS_NPC(ch) && (!ch->pIndexData->new_format || wield == NULL))
    if (!ch->pIndexData->new_format)
    {
        dam = number_range( ch->level / 2, ch->level * 3 / 2 );
        if ( wield != NULL )
            dam += dam / 2;
    }
    else
        dam = dice(ch->damage[DICE_NUMBER],ch->damage[DICE_TYPE]);

    else
    {
    if (sn != -1 && number_percent() < 25)
        check_improve(ch,sn,TRUE,35);
    if ( wield != NULL )
    {
        if (wield->pIndexData->new_format)
        dam = dice(wield->value[1],wield->value[2]) * skill/100;
        else
            dam = number_range( wield->value[1] * skill/100,
                wield->value[2] * skill/100);

        if (get_eq_char(ch,WEAR_SHIELD) == NULL)  /* no shield = more */
        dam = dam * 11/10;

        //Two handed weapons potentially do double damage.
        if (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS))
            dam = number_range(dam, 2*dam/3);

    }
    else
        dam = number_range( 1 + 4 * skill/100, 2 * ch->level/3 * skill/100);
    }

    /*
     * Bonuses.
     */
    if ( get_skill(ch,gsn_enhanced_damage) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_enhanced_damage))
        {
            check_improve(ch,gsn_enhanced_damage,TRUE,6);
            dam += dam/5;
/*            dam += 2 * ( dam * diceroll/300); */
        }
    }

    if ( get_skill(ch,gsn_knight_training) > 0 )
    {
        diceroll = number_percent();
        if (diceroll <= get_skill(ch,gsn_knight_training))
        {
            check_improve(ch,gsn_knight_training,TRUE,8);
            dam += 3 * ( dam * diceroll/300);
        }
    }



    if(get_skill(ch, gsn_knight_training) && get_skill(ch,gsn_lance) > 1 && MOUNTED(ch))
    {
        if(get_skill(ch,gsn_lance) > number_range(1, 100) && wield->value[0] == WEAPON_POLEARM)
            dam += get_skill(ch,gsn_lance)/2;
    }

    if (MOUNTED(ch))
    {
        if(get_skill(ch, gsn_knight_training ) )
            dam += number_range(dam/8, dam);
        else
            dam += number_range(1, dam/8);
    }
     if (get_eq_char( ch, WEAR_WIELD ) == NULL && !IS_NPC(ch))
            dam += number_range( 5, ch->level/2+get_skill(ch,gsn_hand_to_hand)/10 );

    if ( !IS_AWAKE(victim) )
    dam *= 2;
     else if (victim->position < POS_FIGHTING)
    dam = dam * 3 / 2;

    if ( dt == gsn_backstab && wield != NULL)
    {
    dam *= 5;
    }

    dam += (GET_DAMROLL(ch)/2) * UMIN(100,skill) /100;

    if (!IS_NPC(ch))
        dam += get_attribute(ch, STRENGTH) * (dam / 5);

/*New Potence change, equivalent of STR for damage*/
    if (!IS_NPC(ch))
           dam += ((ch->pcdata->discipline[POTENCE])) * (dam / 5);
           // Newbie boost. May need adjusted - matthew
    if (!IS_NPC(ch) && IS_SET(ch->act2, PLR2_NEWBIE))
        dam = 3*dam/2;

    if ( dam <= 0 )
    dam = 1;

/* damage tweak for Renown skill */
    if ( number_percent( ) < get_skill(victim,gsn_renown))
    {
        dam -= dam/10;
        check_improve(victim,gsn_renown,TRUE,4);
    }
    /* vampire anti-bless junk */
    if (wield != NULL && IS_OBJ_STAT(wield,ITEM_BLESS) && (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah")))
        dam += dam/2;
    if ( dam > 0 && check_critical(ch,victim) ){
        dam += dam;
        }

    if (is_affected(ch, gsn_controlrandomness) && number_percent( ) < (2*ch->sphere[SPHERE_ENTROPY]))
    {
        dam += 2*dam/3;
        act("Fate smiles upon you and you critically strike $N!",ch,NULL,victim,TO_CHAR);
    }

     result = damage( ch, victim, dam, dt, dam_type, TRUE );

    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    {
    int dam;

    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
    {
        int level;
        AFFECT_DATA *poison, af;

        if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
        level = wield->level;
        else
        level = poison->level;

        if (!IS_AFFECTED(victim, AFF_POISON) && !saves_spell(level / 2,victim,DAM_POISON) && !is_affected(victim, gsn_poison))
        {
        send_to_char("You feel poison coursing through your veins.",
            victim);
        act("$n is poisoned by the venom on $p.",
            victim,wield,ch,TO_ROOM);

            af.where     = TO_AFFECTS;
            af.type      = gsn_poison;
            af.level     = level;
            af.duration  = level / 20 + 3;
            af.location  = APPLY_CS_STR;
            af.modifier  = -1;
            af.bitvector = AFF_POISON;
            affect_to_char( victim, &af );
        }

        /* weaken the poison if it's temporary */
        if (poison != NULL && number_percent() < 40)
        {
            poison->level = UMAX(0,poison->level - 2);
            poison->duration = UMAX(0,poison->duration - 1);

            if (poison->level == 0 && poison->duration == 0)
            {
            act("The poison on $p has worn off.",ch,wield,victim,TO_CHAR);
            affect_remove_obj(wield, poison);
            }
        }
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SLOW))
    {
        int level;
        AFFECT_DATA *slow, af;
        if ((slow = affect_find(wield->affected,gsn_slow)) == NULL)
        level = wield->level;
        else
        level = slow->level;
            if (!saves_spell(level / 2,victim,DAM_OTHER))
        {
        send_to_char("You feel slower...",          victim);
        act("$n is slowed by $p.",          victim,wield,ch,TO_ROOM);
        af.where     = TO_AFFECTS;
            af.type      = gsn_slow;
            af.level     = level * 3/4;
            af.duration  = level / 2;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = AFF_SLOW;
            affect_join( victim, &af );
        }
        /* weaken the poison if it's temporary */
        if (slow != NULL)
        {
            slow->level = UMAX(0,slow->level - 2);
            slow->duration = UMAX(0,slow->duration - 1);
                if (slow->level == 0 || slow->duration == 0)
            act("The taint on $p has worn off.",ch,wield,victim,TO_CHAR);
        }
    }
/* Bonus for being human... immune to agg damage - Ugha */
    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_AGG_DAMAGE) && victim->race != race_lookup("human"))
    {
        victim->agg_dam += 10;

    }
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
    {
        dam = number_range(5, wield->level / 2 + 10);
        act2("$p draws life from $n.",victim,wield,ch,TO_ROOM);
        act2("You feel $p drawing your life away.",victim,wield,ch,TO_CHAR);
        damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
        victim->mana -= dam/4;
        victim->move -= dam/2;
        ch->hit += dam/2;
        ch->mana += dam/8;
        ch->move += dam/4;
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_SHARP))
    {
      if (!is_affected(victim, gsn_bleeding))
      {
        if (number_range(1, 10) <= 2)
        af.where     = TO_AFFECTS;
        af.type      = gsn_bleeding;
        af.level     = ch->level / 40;
        af.duration  = ch->level / 40;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(victim, &af);
        act("$p slices into you, leaving a bleeding wound!", ch, wield, victim, TO_VICT);
      }
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
    {
        dam = number_range(1,wield->level / 4 + 1);
        act2("$n is burned by $p.",victim,wield,ch,TO_ROOM);
        act2("$p sears your flesh.",victim,wield,ch,TO_CHAR);
        fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_FIRE,FALSE);
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
    {
        dam = number_range(1,wield->level / 6 + 2);
        act2("$p freezes $n.",victim,wield,ch,TO_ROOM);
        act2("The cold touch of $p surrounds you with ice.",
        victim,wield,NULL,TO_CHAR);
        cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_COLD,FALSE);
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
    {
        dam = number_range(1,wield->level/5 + 2);
        act2("$n is struck by lightning from $p.",victim,wield,ch,TO_ROOM);
        act2("You are shocked by $p.",victim,wield,ch,TO_CHAR);
        shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
    }
    }
    tail_chain( );
    return;
}

/*
 * Inflict damage from a hit.
 */
bool damage(CHAR_DATA *ch,CHAR_DATA *victim,int dam,int dt,int dam_type,
        bool show)
{
    extern bool slaughter;
    extern bool doubledam;
    bool immune;
    DESCRIPTOR_DATA *d;
   char buf[MAX_STRING_LENGTH];


/*Sengir - check if victim is torped*/
    if (victim->position == POS_TORPOR)
    {
        send_to_char("They are in {RT{ro{Drp{ro{Rr{x!  Now is the time to {Gbehead{x the helpless!\n\r", ch);
        return FALSE;
    }

    if ( victim->position == POS_DEAD || victim->position == POS_TORPOR )
    return FALSE;
    affect_strip(victim,gsn_chant);     affect_strip(ch,gsn_chant);
    affect_strip(victim,gsn_meditation);    affect_strip(ch,gsn_meditation);
    if ( IS_AFFECTED2(ch, AFF2_MIST) && !IS_AFFECTED2(victim, AFF2_MIST))
        return FALSE;

    if(is_affected(ch,gsn_shadowform) && !is_affected(ch,gsn_shadowform))
        return FALSE;

    if(!is_affected(ch,gsn_shadowform) && is_affected(ch,gsn_shadowform))
        return FALSE;

    if ( !IS_AFFECTED2(ch, AFF2_MIST) && IS_AFFECTED2(victim, AFF2_MIST))
        return FALSE;

    if ( IS_SET(victim->act,PLR_SPEC) && !IS_NPC(victim))
        return FALSE;

        if(!IS_NPC(ch) && !IS_NPC(victim) && !IS_SET(victim->act,PLR_ARENA) &&
        victim != ch && (ch->in_room->area->domain == NULL ||
            !IS_SET(ch->act2, PLR2_PVP) || !IS_SET(victim->act2, PLR2_PVP)))
                {
        stop_fighting( ch, TRUE );
        stop_fighting( victim, TRUE );
        do_function(ch, &do_flee, "auto" );

        act( "$n seems about to attack $N and flees!", ch, NULL, victim, TO_ROOM );
        return FALSE;
    }

    if(IS_NPC(ch)) grudge_update(ch,victim);    if(IS_NPC(victim)) grudge_update(victim,ch);
    /* damage reduction */
    if(dt != gsn_fireshield && dt != gsn_holyshield && dt != gsn_darkshield && dt != gsn_gift_porcupine && dt != gsn_vicissitude_bonecraft)
    {
        if ( dam > 35)
        dam = (dam - 35)/2 + 35;
        if ( dam > 80)
        dam = (dam - 80)/2 + 80;
    }

    if(slaughter)
        dam *= 4;
    else if (doubledam)
        dam *= 2;

    /* vampire junk */
    if(!IS_NPC(ch) && ch->clan == clan_lookup("watcher") && victim->race == race_lookup("vampire"))
      dam += ch->level/2;

    if(!IS_NPC(ch) && ch->sphere[SPHERE_MATTER] > 0)
        dam += ch->sphere[SPHERE_MATTER] * (dam/8);

    if ( victim != ch )
    {
    /*
     * Certain attacks are forbidden.
     * Most other attacks are returned.
     */
    if ( is_safe( ch, victim ) )
        return FALSE;

    if ( victim->position > POS_STUNNED )
    {
        if ( victim->fighting == NULL )
        {
            if ( IS_NPC( victim ) && HAS_TRIGGER_MOB( victim, TRIG_KILL ) )
                p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_KILL );
        set_fighting( victim, ch );
        }
        if (victim->timer <= 4)
            victim->position = POS_FIGHTING;
    }

    if ( victim->position > POS_STUNNED )
    {
        if ( ch->fighting == NULL )
        set_fighting( ch, victim );
    }

    /*
     * More charm stuff.
     */
    if ( victim->master == ch )
        stop_follower( victim );
    }

    /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
    {
    affect_strip( ch, gsn_invis );
    affect_strip( ch, gsn_mass_invis );
    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
    act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    /*
     * Damage modifiers.
     */

    if ( dam > 1 && !IS_NPC(victim)
    &&   victim->pcdata->condition[COND_DRUNK]  > 10 )
    dam = 9 * dam / 10;

    if ( !IS_NPC(victim) && victim->pcdata->discipline[FORTITUDE] == 1)
    dam -= dam / 6;

    if ( !IS_NPC(victim) && victim->pcdata->discipline[FORTITUDE] == 2)
    dam -= dam / 5;

    if ( !IS_NPC(victim) && victim->pcdata->discipline[FORTITUDE] == 3)
    dam -= dam / 4;

    if ( !IS_NPC(victim) && victim->pcdata->discipline[FORTITUDE] == 4)
    dam -= dam / 3;

    if ( !IS_NPC(victim) && victim->pcdata->discipline[FORTITUDE] == 5)
    dam -= dam / 2;

    bool silver;

    if(!IS_NPC(victim) && victim->race == race_lookup("garou"))
    {

            OBJ_DATA *silver_weapon;
            if ((silver_weapon = get_eq_char(ch, WEAR_WIELD)) != NULL &&
                (IS_OBJ_STAT(silver_weapon,ITEM_SILVER)) &&
                (victim->pcdata->shiftform != ch->pcdata->breed ||
                 victim->pcdata->breed == METIS) )
                 {
                    silver = TRUE;
                    dam = dam*4;
                 }
    }

    if ( is_affected( victim, gsn_sanctus ) )
      dam -= dam/4;

    if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )
    dam /= 2;

    if (IS_NEWBIE(victim))
        dam = 2*dam/3;

    if (dam_type <= 3) dam = dam/2;

    immune = FALSE;


    /*
     * Check for parry, and dodge.
     */
    if ( dt >= TYPE_HIT && ch != victim)
    {
        if ( check_parry( ch, victim ) )
        return FALSE;
    if ( check_dodge( ch, victim ) )
        return FALSE;
    if ( check_block( ch, victim ) )
        return FALSE;
    if ( check_shield_block(ch,victim))
        return FALSE;

    }

/* new damcap code.. may cause major unbalance */

    if ( (!slaughter && !doubledam) && dam > 3000)
    {
    bug( "Damage: %d: more than 3000 points!", dam );
    dam = 3000;
    send_to_char("You've breeched the damcap!\n\r",ch);
    }

    switch(check_immune(victim,dam_type))
    {
    case(IS_IMMUNE):
        immune = TRUE;
        dam = 0;
        break;
    case(IS_RESISTANT):
        dam -= dam/5;
        break;
    case(IS_VULNERABLE):
        dam += dam/2;
        if(IS_NPC(victim)) victim->agg_dam += dam;
        else if (victim->race == race_lookup("garou"))
            break;
        victim->agg_dam += dam/20;
        break;
    }

    if (victim != ch && is_affected(victim, gsn_gift_haresleap) && number_percent() < 5*get_attribute(victim, DEXTERITY))
    {
        if (godice(get_attribute(victim, STRENGTH) + get_ability(victim, CSABIL_ATHLETICS), 7) > 0)
        {
            act("You leap back from $N's attack and escape harm!", victim, NULL, ch, TO_CHAR);
            act("$n leaps back out of range of $N's attack!", victim, NULL, ch, TO_NOTVICT);
            act("$n leaps back out of range of your attack!", victim, NULL, ch, TO_VICT);
            dam = 0;
        }
    }

    if(victim != ch && is_affected(victim,gsn_kineticshield) && number_range(1,100) > 50)
    {
        if(dam_type == DAM_PIERCE || dam_type == DAM_BASH || dam_type == DAM_SLASH)
        {
            act("{GYou deflect the blow with your {CKinetic Shield{G!{x",victim,NULL,ch,TO_CHAR);
            act("{G$N's blow is deflected completely by $n's {CKinetic Shield{G!{x",victim,NULL,ch,TO_NOTVICT);
            act("{GYou gasp in shock as $n's unnatural {CKinetic Shield{G completely deflects your blow!{x",victim,NULL,ch,TO_VICT);
            dam = 0;
        }
    }

    if(victim != ch && is_affected(victim,gsn_phantasm) && number_range(1,100) < get_affect_level(victim, gsn_phantasm) * 5)
    {
        act("$N is fooled by your phantasm and misses you completely!",victim,NULL,ch,TO_CHAR);
        act("$n is almost hit by $N, but just as the blow connects $n's image dissolves... $e was never there.",victim,NULL,ch,TO_NOTVICT);
        act("You almost hit $n, but just as the blow connects $s image dissolves... $e was never there.",victim,NULL,ch,TO_VICT);
        dam = 0;
    }

    if(dam < 0) dam = 0;

    if (show)
        dam_message( ch, victim, dam, dt, immune );

    if (dam == 0)
    return FALSE;

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( !IS_NPC(victim)
    &&   !IS_SET(victim->act,PLR_ARENA)
    &&   victim->level >= LEVEL_IMMORTAL
    &&   victim->hit < 1 )
    victim->hit = 1;
    /* if(dam_type == DAM_PIERCE || dam_type == DAM_BASH || dam_type == DAM_SLASH) */
    if(dt != gsn_fireshield && dt != gsn_holyshield && dt != gsn_darkshield && dt != gsn_gift_porcupine && dt != gsn_vicissitude_bonecraft)
    {
        /* fireshield */
        if(IS_AFFECTED2(victim, AFF2_FIRESHIELD) && !IS_AFFECTED2(ch, AFF2_FIRESHIELD) && !IS_SET(ch->imm_flags,IMM_FIRE) && !is_affected(ch,gsn_negationfield))
        {

            act2("{RYour fireshield sprays out and incinerates $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{R$n's fireshield sprays out and incinerates $N!{x",victim,NULL,ch,TO_NOTVICT);
            act2("{R$n's fireshield sprays out and incinerates you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_fireshield, DAM_FIRE, TRUE);
            fire_effect(ch,ch->level/2,dam/4,TARGET_CHAR);
        }
        /* holyshield */
        if(IS_AFFECTED2(victim, AFF2_HOLYSHIELD) && !IS_AFFECTED2(ch, AFF2_HOLYSHIELD) && !IS_SET(ch->imm_flags,IMM_HOLY) && !is_affected(ch,gsn_negationfield))
        {
            act2("{WYour Radiant Aura flashes out towards $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{W$n's Radiant Aura flashes out and hits $N!{x",victim,NULL,ch,TO_NOTVICT);
            act2("{W$n's Radiant Aura flashes out and hits you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam*2, gsn_holyshield, DAM_HOLY, TRUE);
        }
        /* darkshield */
        if(IS_AFFECTED2(victim, AFF2_DARKSHIELD) && !IS_AFFECTED2(ch, AFF2_DARKSHIELD) && !IS_SET(ch->imm_flags,IMM_NEGATIVE) && !is_affected(ch,gsn_negationfield))
        {

            act2("{DYour Dark Aura strikes out to envelope $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{D$n's Dark Aura strikes out and envelopes $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{D$n's Dark Aura strikes out and envelopes you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam*2, gsn_darkshield, DAM_NEGATIVE, TRUE);
        }
        if(is_affected(victim,gsn_gift_porcupine) && !is_affected(ch,gsn_gift_porcupine) && !is_affected(ch,gsn_negationfield))
        {
            act2("{YYou growl a laugh as $N is impaled on your spikes.{x",victim,NULL,ch,TO_CHAR);
            act2("{Y$n growls a cruel laugh as $N is impaled upon their spikes.{x",victim,NULL,ch,TO_NOTVICT);
            act2("{RYou scream in agony as $n's spikes pierce your flesh!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_gift_porcupine, DAM_PIERCE, TRUE);
        }
        /* bonecrafting */
        if(is_affected(victim,gsn_vicissitude_bonecraft) && !is_affected(ch,gsn_vicissitude_bonecraft) && !is_affected(ch,gsn_negationfield))
        {
            act2("{YYou feel the feather touch of raw flesh as $N is impaled upon your bone spikes.{x",victim,NULL,ch,TO_CHAR);
            act2("$n smiles slightly at the feel of $N impaled upon their bone spikes.{x",victim,NULL,ch,TO_NOTVICT);
            act2("{RYou feel a sharp pain as $n's bone spikes slice through your body.{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_vicissitude_bonecraft, DAM_PIERCE, TRUE);
        }

    }

    if((victim->race == race_lookup("methuselah") || victim->race == race_lookup("vampire")) && victim->hit <= 0 && !IS_IMMORTAL(victim) && !IS_SET(victim->act,PLR_ARENA))
    {
            victim->position = POS_TORPOR;
            victim->hit = -20;

                        if((number_range(1,100) < 50) && (!IS_NPC(victim)))
                        {
                                victim->perm_stat[number_range(0, 4)] -= 1;
                                update_csstats(victim);
                                send_to_char("Your struggle for life has permanently weakened you!\n\r",victim);
                        }

            stop_fighting( victim, TRUE );
            if(IS_NPC(victim)) do_function(victim, &do_visible, "" );
	    if(IS_NPC(ch)) grudge_update(ch,victim);
            if(IS_NPC(victim)) grudge_update(victim,ch);

            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( d->connected == CON_PLAYING &&
                d->character != victim &&
                is_same_clan(victim,d->character) && IS_VAMP(d->character) &&
                !IS_SET(d->character->comm,COMM_NOCLAN))
                {
                act_new("You sense that $n has entered Torpor!",victim,NULL,d->character,TO_VICT,POS_DEAD, TRUE);
                }
            }

            OBJ_DATA *obj, *obj_next;

            /* rot_torp - Ugha */

            for ( obj = victim->carrying; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;

                if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH))
                {

                    if(obj->wear_loc == WEAR_FLOAT)
                    {
                        if (obj->contains != NULL)
                        {

                            OBJ_DATA *in, *in_next;

                            act("$p evaporates,scattering its contents.",victim,obj,NULL,TO_ROOM);
                            for (in = obj->contains; in != NULL; in = in_next)
                            {
                                in_next = in->next_content;
                                obj_from_obj(in);
                                obj_to_room(in,victim->in_room);
                            }
                        }
                        else act("$p evaporates.",victim,obj,NULL,TO_ROOM);

                        extract_obj(obj);
                    }
                    else
                    {
                        obj->timer = number_range(5,10);
                        REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
                    }
                }
            }
    }
    update_pos( victim );
    if(IS_NPC(ch)) grudge_update(ch,victim);
    if(IS_NPC(victim)) grudge_update(victim,ch);

    switch( victim->position )
    {
    case POS_MORTAL:
    act( "$n is mortally wounded, and will die soon, if not aided.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char(
        "You are mortally wounded, and will die soon, if not aided.\n\r",
        victim );
    break;

    case POS_INCAP:
    act( "$n is incapacitated and will slowly die, if not aided.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char(
        "You are incapacitated and will slowly die, if not aided.\n\r",
        victim );
    break;

    case POS_STUNNED:
    act( "$n is stunned, but will probably recover.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char("You are stunned, but will probably recover.\n\r",
        victim );
    break;

    case POS_TORPOR:
    act( "$n falls to the floor as they enter {RT{ro{Drp{ro{Rr{x.",
        victim, NULL, NULL, TO_ROOM );
    if (IS_NPC(victim))
        act("'Behead' $n to send them into Final Death!", victim, NULL, NULL, TO_NEWBIEALL);
    send_to_char("You enter {RT{ro{Drp{ro{Rr{x.\n\r",
        victim );
    break;


    case POS_DEAD:
    act( "{rDeath has claimed $n!{x", victim, 0, 0, TO_ROOM );
        if (!IS_SET(victim->act,PLR_ARENA))
        {
            send_to_char( "{RYou take a mortal wound to the heart...\n\r",victim);
            send_to_char( "{RAs your life blood slips away, your last sight in this\n\r",victim);
            send_to_char( "{Rworld is of your enemy, gloating over your beaten corpse.\n\r",victim);
            send_to_char( "{RSuddenly, you feel your soul ripped away from your body...\n\r\n\r",victim);
            send_to_char( "{RAnd the Great God Zelan places you within a new whole and healthy body.{x\n\r\n\r", victim );
        }
    break;

    default:
    if ( dam > victim->max_hit / 4 )
        send_to_char( "That really did HURT!\n\r", victim );
    if ( victim->hit < victim->max_hit / 4 )
        send_to_char( "You sure are BLEEDING!\n\r", victim );
    break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
    stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
    kill_em(ch, victim);
    return TRUE;
    }

    if ( victim == ch )
    return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
    if ( number_range( 0, victim->wait ) == 0 )
    {
        do_function(victim, &do_recall, "" );
        return TRUE;
    }
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
    if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
    &&   victim->hit < victim->max_hit / 7)
    ||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
    &&     victim->master->in_room != victim->in_room ) )
    {
        do_function(victim, &do_flee, "" );
    }
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
    {
    do_function (victim, &do_flee, "" );
    }

    tail_chain( );
    return TRUE;
}

int get_armor_diff( CHAR_DATA *ch, CHAR_DATA *victim, int dam_type)
{
    int victim_ac, diff = 0;

    switch(dam_type)
    {
        case(DAM_PIERCE):victim_ac = GET_AC(victim,AC_PIERCE);  break;
        case(DAM_BASH):  victim_ac = GET_AC(victim,AC_BASH);        break;
        case(DAM_SLASH): victim_ac = GET_AC(victim,AC_SLASH);   break;
        default:     victim_ac = GET_AC(victim,AC_EXOTIC);  break;
    }

    diff += -(victim_ac/100);
    if (diff < 0)
        diff = -1;

	return diff/2;
}

// Calculate Damage Dicepool for combat hits. Moved here for use in other functions.

int d10_damdice( CHAR_DATA *ch, CHAR_DATA *victim)
{
    OBJ_DATA *wield;
    int dice = 0;
    int diceroll = 0;

    wield = get_eq_char( ch, WEAR_WIELD );
       if (wield)
    {
        //Weapon bonus computed as 1/20th of weapon dice.
     dice += (wield->value[1])/20;

        //Two handed weapons potentially add up to 2 dice.
    if (IS_WEAPON_STAT(wield,WEAPON_TWO_HANDS))
    dice += number_range(0, 2);

        // Sharp weapons potentially add up to 1 die
    if (IS_WEAPON_STAT(wield, WEAPON_SHARP))
        dice += number_range(0, 1);

    }
    // Race Bonuses
    // Garou bonus with claws or Chiropteran Marauder bone claws, +2 for claws.
    if (!wield && ( is_affected(ch, gsn_claws) || is_affected(ch, gsn_wingclaws)))
        dice += 2;
    // Razor Claws or Horrid Form gives +1
    if (!wield && (is_affected(ch, gsn_gift_razorclaws) || is_affected(ch, gsn_vicissitude_horrid)))
        dice += 1;

    //Vampire potence bonus
	if (!IS_NPC(ch) && IS_VAMP(ch))
			dice += ch->pcdata->discipline[POTENCE];

    //Shadow Lords Gift: Fatal Flaw Bonus if victim == your current saved quarry
    //Quarry mob vnum saved under aff->modifier
    if (IS_NPC(victim) && is_affected(ch, gsn_gift_fatalflaw) &&
    victim->pIndexData->vnum == get_affect_modifier(ch, gsn_gift_fatalflaw))
      dice++;

      //Mage Entropy 2
      if (is_affected(ch, gsn_controlrandomness) && number_percent( ) < get_affect_level(ch, gsn_controlrandomness) * 4)
      {
          dice += get_affect_level(ch, gsn_controlrandomness);
          act("Fate smiles upon you as your attack strikes towards a critical part of $N's defenses!",ch,NULL,victim,TO_CHAR);

      }

		/*
		 * Skill Bonuses.
		 */
		if ( get_skill(ch,gsn_enhanced_damage) > 0 )
		{
			diceroll = number_percent();
			if (diceroll <= get_skill(ch,gsn_enhanced_damage))
			{
				check_improve(ch,gsn_enhanced_damage,TRUE,6);
				dice++;
			}
		}

		if ( get_skill(ch,gsn_knight_training) > 0 )
		{
			diceroll = number_percent();
			if (diceroll <= get_skill(ch,gsn_knight_training))
			{
				check_improve(ch,gsn_knight_training,TRUE,6);
				dice++;
			}
		}

		if ( !IS_NPC(ch) && check_critical(ch,victim) )
			dice += get_attribute(ch, STRENGTH);

		dice += get_attribute(ch, STRENGTH);

    return dice;

}

// Find damage modifier for char. Moved here to be used in Other Functions.
int d10_modifier ( CHAR_DATA *ch)
{
    int sn;
    int skill;
    int modifier;

    if (!ch || ch == NULL)
    {
        bug("Get_Modifier: NULL ch.", 0 );
        return;
    }

    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = get_weapon_skill(ch,sn);

    modifier = 10 + ch->level/5;
    modifier += GET_DAMROLL(ch)/50;
    modifier =  modifier * (20 + skill) / 100;

    return UMAX(10, modifier);
}

/*
 * Hit one guy once as d10
 */ // bm_d10_hit -- d10 bookmark.
void d10_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    OBJ_DATA *wield;
    AFFECT_DATA af;
    bool immune = FALSE;
    extern bool slaughter;
    extern bool doubledam;


    int tohit = 0;
	int dice = 0;
    int soak = 0;
    int soakdice = 0;
    int armor = 0;
    int armordice = 0;
    int damsuccess = 0;
    int dam = 0;
    int dam_type;
    int strength_bonus = 0;
    int diff = 6;
    bool modified = FALSE;
    int sn, skill;
    bool result;
    int diceroll;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD ||
         victim->position == POS_TORPOR ||
         ch->in_room != victim->in_room )
        return;


  /* Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );
    if ( dt == TYPE_UNDEFINED )
    {
    dt = TYPE_HIT;
    if ( wield != NULL && wield->item_type == ITEM_WEAPON )
        dt += wield->value[3];
    else
        {
            if ( is_affected(ch, gsn_claws) || is_affected(ch, gsn_wingclaws))
                dt += attack_lookup("claws");
            else
                dt += ch->dam_type;
            }
    }


    if (dt < TYPE_HIT)
        if (wield != NULL)
            dam_type = attack_table[wield->value[3]].damage;
        else
            dam_type = attack_table[ch->dam_type].damage;
    else
        dam_type = attack_table[dt - TYPE_HIT].damage;

    if (dam_type == -1)
    dam_type = DAM_BASH;


    /* get the weapon skill */
    sn = get_weapon_sn(ch);
    skill = get_weapon_skill(ch,sn);

	if (!can_see(ch, victim))
		diff += 2;
    if (!can_see(victim, ch))
		diff -= 2;
    if ( victim->position < POS_FIGHTING)
		diff--;
	if ( victim->position < POS_RESTING)
		diff--;

    if (diff < 3)
        diff = 3;

    /* We have damage type and difficulty, now get the dicepool.*/
		if (wield)
			dice = get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_MELEE];
		else
			dice = get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_BRAWL];

		//Dicepool Modifiers.

    dice += GET_HITROLL(ch)/200;

	if (dt == gsn_waylay)
        {
            dice *= 2;
            diff = 4;
        }

  if (IS_AFFECTED(ch, AFF_SLOW))
    diff++;

  if (is_affected(victim, gsn_gift_lambentfire))
    diff++;

  if (is_affected(victim, gsn_gift_curseoftheaeolus) && get_affect_level(victim, gsn_gift_curseoftheaeolus) >= 0)
    dice--;

  if (IS_NPC(ch))
    if (has_affect_modifier(victim, gsn_awe, ch->pIndexData->vnum))
      diff++;

    if (is_affected(ch, gsn_hauntthesoul))
        diff++;

    if (diff > 10)
        diff = 10;

if (DEBUG_MESSAGES || IS_DEBUGGING(ch)){
	cprintf(ch, "hp{r%d{w df{c%d{x ", dice, diff);
	if (IS_NPC(ch)) cprintf(victim, "hp{r%d{w df{c%d{x ", dice, diff);}

		//Moment of excitement!
		tohit = godice(dice, diff);
		dice = 0; // clear dice so we can use them later.

    if (is_affected(ch, gsn_gift_exceptionalswordplay) 
        && get_affect_level(ch, gsn_gift_exceptionalswordplay) > 0
        && wield)
    {
        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_exceptionalswordplay;
        af.level     = get_affect_level(ch, gsn_gift_exceptionalswordplay);
        af.duration  = 0;
        af.location  = APPLY_NONE;
        af.modifier  = -1;
        af.bitvector = 0;
        affect_join( ch, &af );
        tohit++;

        if (get_affect_modifier(ch, gsn_gift_exceptionalswordplay) <= 0)
            affect_strip(ch, gsn_gift_exceptionalswordplay);
    }

	if (check_dodge(ch, victim) ||
      check_parry(ch, victim) ||
			check_shield_block(ch, victim) ||
			check_block(ch, victim) )
		tohit = 0;

	if (DEBUG_MESSAGES || IS_DEBUGGING(ch)){
		//Debug Message
		cprintf(ch, "t{R%d{x ", tohit);
		if (IS_NPC(ch)) cprintf(victim, "t{R%d{x ", tohit);}



    if(tohit < 1)
    { // Miss.
        d10_damage(ch, victim, 0, 0, dt, dam_type, DEFENSE_FULL, TRUE, TRUE);
        tail_chain( );
        return;
    }

    //Hit! Calculate dicepool for damage.
        //Exceptional rolls on hit add to damage dice.
    //if (tohit > 4)
    //	dice += (tohit - 4);

    //Garou rage regeneration. Placed here so it only fires on
    //successful hit.
    if (!IS_NPC(ch) && ch->race == race_lookup("garou"))
    {
        int hppercent;

        //Calc HP percent. more injured = less difficulty.
        hppercent = ch->hit*10 / ch->max_hit;
        if (hppercent > 6)
            hppercent = 6;
        if (hppercent < 2)
            hppercent = 2;
        // Cake in the code Ohno!
        //1% chance of rolling primal per hit, if less than full rage.
        // primal diff 4 + hpperc min/max 6/10
        if( ch->pcdata->rage[TEMP] < ch->pcdata->rage[PERM] && number_range(1, 200) == MAGIC_NUMBER &&
            godice(ch->pcdata->primal_urge,4+hppercent))
             {
            cprintf(ch,"You let loose a vicious howl as your bloodlust takes over!\n\r");
            act("$n lets loose a vicious howl as $s bloodlust takes over!", ch, NULL, NULL, TO_ROOM);
            ch->pcdata->rage[TEMP]++;
            if(ch->pcdata->rage[TEMP] > ch->pcdata->rage[PERM])
            ch->pcdata->rage[TEMP] = ch->pcdata->rage[PERM];
            }
    }



    //Increase skill?
    if (sn != -1 && number_percent() < 25)
        check_improve(ch,sn,TRUE,35);

        dice = d10_damdice(ch, victim);
        damsuccess += godice(dice, 5);

		if (DEBUG_MESSAGES || IS_DEBUGGING(ch))	{
            cprintf(ch, "dp{r%d{x ", dice);
            if (IS_NPC(ch)) cprintf(victim, "dp{r%d{x ", dice);
        }

	result = d10_damage(ch, victim, damsuccess, d10_modifier(ch), dt, dam_type, DEFENSE_FULL, TRUE, TRUE);

    /* but do we have a funky weapon? */
    if (result && wield != NULL)
    {
      int dam;

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_POISON))
      {
        int level;
        AFFECT_DATA *poison, af;

        if ((poison = affect_find(wield->affected,gsn_poison)) == NULL)
        level = wield->level;
        else
        level = poison->level;

        if (!IS_AFFECTED(victim, AFF_POISON) && !saves_spell(level / 2,victim,DAM_POISON) && !is_affected(victim, gsn_poison))
        {
            send_to_char("You feel poison coursing through your veins.", victim);
            act("$n is poisoned by the venom on $p.",victim,wield,ch,TO_ROOM);

            af.where     = TO_AFFECTS;
            af.type      = gsn_poison;
            af.level     = level;
            af.duration  = level / 20 + 3;
            af.location  = APPLY_CS_STR;
            af.modifier  = -1;
            af.bitvector = AFF_POISON;
            affect_to_char( victim, &af );
        }

        /* weaken the poison if it's temporary */
        if (poison != NULL && number_percent() < 40)
        {
            poison->level = UMAX(0,poison->level - 2);
            poison->duration = UMAX(0,poison->duration - 1);

            if (poison->level == 0 && poison->duration == 0)
            {
            act("The poison on $p has worn off.",ch,wield,victim,TO_CHAR);
            affect_remove_obj(wield, poison);
            }
        }
      }

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SLOW))
      {
        int level;
        AFFECT_DATA *slow, af;
        if ((slow = affect_find(wield->affected,gsn_slow)) == NULL)
        level = wield->level;
        else
        level = slow->level;
            if (!saves_spell(level / 2,victim,DAM_OTHER))
        {
        send_to_char("You feel slower...",          victim);
        act("$n is slowed by $p.",          victim,wield,ch,TO_ROOM);
        af.where     = TO_AFFECTS;
            af.type      = gsn_slow;
            af.level     = level * 3/4;
            af.duration  = level / 2;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = AFF_SLOW;
            affect_join( victim, &af );
        }
        /* weaken the poison if it's temporary */
        if (slow != NULL)
        {
            slow->level = UMAX(0,slow->level - 2);
            slow->duration = UMAX(0,slow->duration - 1);
                if (slow->level == 0 || slow->duration == 0)
            act("The taint on $p has worn off.",ch,wield,victim,TO_CHAR);
        }
      }

      if (ch->fighting == victim && (IS_WEAPON_STAT( wield, WEAPON_SHARP) ))
      {
        if (!is_affected(victim, gsn_bleeding) && tohit >= 4)
        {
          AFFECT_DATA af;
          af.where     = TO_AFFECTS;
          af.type      = gsn_bleeding;
          af.level     = tohit;
          af.duration  = 1 + tohit / 3;
          af.location  = APPLY_NONE;
          af.modifier  = 0;
          af.bitvector = 0;
          affect_to_char( victim, &af );
          act("Your attack slices open a bleeding wound!", ch, wield, victim, TO_CHAR);
          act("$p slices into you, leaving a bleeding wound!", ch, wield, victim, TO_VICT);
        }
      }

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
      {
        dam = number_range(5, wield->level / 2 + 10);
        act2("$p draws life from $n.",victim,wield,ch,TO_ROOM);
        act2("You feel $p drawing your life away.",victim,wield,ch,TO_CHAR);
        damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
        victim->mana -= dam/4;
        victim->move -= dam/2;
        ch->hit += dam/2;
        ch->mana += dam/8;
        ch->move += dam/4;
      }

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FLAMING))
      {
        dam = number_range(1,wield->level / 4 + 1);
        act2("$n is burned by $p.",victim,wield,ch,TO_ROOM);
        act2("$p sears your flesh.",victim,wield,ch,TO_CHAR);
        fire_effect( (void *) victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_FIRE,FALSE);
      }

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_FROST))
      {
        dam = number_range(1,wield->level / 6 + 2);
        act2("$p freezes $n.",victim,wield,ch,TO_ROOM);
        act2("The cold touch of $p surrounds you with ice.",
        victim,wield,NULL,TO_CHAR);
        cold_effect(victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_COLD,FALSE);
      }

      if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_SHOCKING))
      {
        dam = number_range(1,wield->level/5 + 2);
        act2("$n is struck by lightning from $p.",victim,wield,ch,TO_ROOM);
        act2("You are shocked by $p.",victim,wield,ch,TO_CHAR);
        shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
      }
    }

/* Claws chance to cause bleed */
    if (result && wield == NULL && ch->fighting == victim && ( is_affected(ch, gsn_claws) || is_affected(ch, gsn_wingclaws)))
    {
      if (!is_affected(victim, gsn_bleeding) && tohit >= 4)
      {
        AFFECT_DATA af;
        af.where     = TO_AFFECTS;
        af.type      = gsn_bleeding;
        af.level     = tohit;
        af.duration  = 1 + tohit / 3;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( victim, &af );
        act("Your slicing claw attack opens a bleeding wound on $N!", ch, NULL, victim, TO_CHAR);
        act("You are left with a bleeding wound from $n's claws!", ch, NULL, victim, TO_VICT);
      }
    }

    tail_chain( );
    return;
}


bool d10_damage(CHAR_DATA *ch, CHAR_DATA *victim, int damsuccess, int modifier, int dt, int dam_type, int defense, bool show, bool fatal)
{
    extern bool slaughter;
    extern bool doubledam;
    bool immune = FALSE;
    DESCRIPTOR_DATA *d;
	int soakdice, soakdiff, soak, armordice;
	int dam;
    char  buf[MSL];
	soakdice = soakdiff = soak = armordice = dam = 0;
    bool silver = FALSE;
    OBJ_DATA *wield = get_eq_char( ch, WEAR_WIELD );

    if (victim->position == POS_TORPOR)
    {
        send_to_char("They are in {RT{ro{Drp{ro{Rr{x!  Now is the time to {Gbehead{x the helpless!\n\r", ch);
        return FALSE;
    }

    if ( victim->position == POS_DEAD || victim->position == POS_TORPOR )
    return FALSE;
    affect_strip(victim,gsn_chant);     affect_strip(ch,gsn_chant);
    affect_strip(victim,gsn_meditation);    affect_strip(ch,gsn_meditation);
    if ( IS_AFFECTED2(ch, AFF2_MIST) && !IS_AFFECTED2(victim, AFF2_MIST))
        return FALSE;

    if(is_affected(ch,gsn_shadowform) && !is_affected(ch,gsn_shadowform))
        return FALSE;

    if(!is_affected(ch,gsn_shadowform) && is_affected(ch,gsn_shadowform))
        return FALSE;

    if ( !IS_AFFECTED2(ch, AFF2_MIST) && IS_AFFECTED2(victim, AFF2_MIST))
        return FALSE;

    if ( IS_SET(victim->act,PLR_SPEC) && !IS_NPC(victim))
        return FALSE;

        if(!IS_NPC(ch) && !IS_NPC(victim) && !IS_SET(victim->act,PLR_ARENA) &&
        victim != ch && (ch->in_room->area->domain == NULL ||
            !IS_SET(ch->act2, PLR2_PVP) || !IS_SET(victim->act2, PLR2_PVP)))
                {
        stop_fighting( ch, TRUE );
        stop_fighting( victim, TRUE );
        do_function(ch, &do_flee, "auto" );

        act( "$n seems about to attack $N and flees!", ch, NULL, victim, TO_ROOM );
        return FALSE;
    }

    if(IS_NPC(ch)) grudge_update(ch,victim);    if(IS_NPC(victim)) grudge_update(victim,ch);
//        if ( dam > 100)
//        dam = (dam - 100)/2 + 100;
        if ( dam > 300)
        dam = (dam - 300)/2 + 300;
        if ( dam > 500)
        dam = (dam - 500)/2 + 500;

	    if ( victim != ch )
    {
    /*
     * Certain attacks are forbidden.
     * Most other attacks are returned.
     */
    if ( is_safe( ch, victim ) )
        return FALSE;

    if ( victim->position > POS_STUNNED )
    {
        if ( victim->fighting == NULL )
        {
            if ( IS_NPC( victim ) && HAS_TRIGGER_MOB( victim, TRIG_KILL ) )
                p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_KILL );
        set_fighting( victim, ch );
        }
        if (victim->timer <= 4)
            victim->position = POS_FIGHTING;
    }

    if ( victim->position > POS_STUNNED )
    {
        if ( ch->fighting == NULL )
        set_fighting( ch, victim );
    }

    /*
     * More charm stuff.
     */
    if ( victim->master == ch )
        stop_follower( victim );
    }

   /*
     * Inviso attacks ... not.
     */
    if ( IS_AFFECTED(ch, AFF_INVISIBLE) )
    {
    affect_strip( ch, gsn_invis );
    affect_strip( ch, gsn_mass_invis );
    REMOVE_BIT( ch->affected_by, AFF_INVISIBLE );
    act( "$n fades into existence.", ch, NULL, NULL, TO_ROOM );
    }

    if(IS_AFFECTED(ch, AFF_HIDE))
    {
      affect_strip(ch, gsn_hide);
      affect_strip(ch, gsn_gift_blurofthemilkyeye);
      REMOVE_BIT(ch->affected_by, AFF_HIDE);
    }

    if (DEBUG_MESSAGES || IS_DEBUGGING(ch))	{
        cprintf(ch, "d{W%d{x ", damsuccess);
    if (IS_NPC(ch))
    cprintf(victim, "d{W%d{x ", damsuccess);
    }

    //Soak dice, subtracts successes from damage.
    soakdice = get_attribute(victim, STAMINA);

    if (dam_type > DAM_BASH && (victim->race == race_lookup("human") ||
            victim->race == race_lookup("ghoul")) )
        soakdice = 0;//humans can't soak lethal.
    if ((dam_type > DAM_SLASH || dt == attack_lookup("claws") ) && victim->race != race_lookup("garou"))
        soakdice = 0;// vamps can't soak agg.
    if (!IS_NPC(victim) && (dam_type > DAM_SLASH || dt == attack_lookup("claws") ) &&
        victim->race != race_lookup("garou") &&
        (victim->pcdata->shiftform == HOMID || victim->pcdata->shiftform == LUPUS) )
        soakdice = 0;// Garou can't soak agg in homid or lupus.
    if (IS_NPC(victim) && (wield) && (IS_WEAPON_STAT(wield,WEAPON_AGG_DAMAGE) || (affect_find(wield->affected,gsn_blood_agony) != NULL)))
        soakdice = 0;// No Stamina soak for Agg weapon

    if (!IS_NPC(victim))
    {
        if (IS_VAMP(victim)) //vamps add fortitude.
            soakdice += victim->pcdata->discipline[FORTITUDE];

    } else {// IS NPC
     if (IS_VAMP(victim) || victim->race == race_lookup("garou"))
        soakdice++;
    }

    // Armor, applied to physical damage and garou claws.
    if (dam_type <= DAM_SLASH || dt == attack_lookup("claws") )
        armordice += get_armor_diff(ch, victim, dam_type);

    if (is_affected(victim, gsn_skinoftheadder))
        soakdiff = 5;
    else
        soakdiff = 6;

    switch (defense) {
        case DEFENSE_NONE: soak = 0; break;
        case DEFENSE_SOAK: soak = godice(soakdice, soakdiff); break;
        case DEFENSE_ARMOR: soak = godice(armordice, soakdiff); break;
        case DEFENSE_FULL:  soak = godice(armordice+soakdice, soakdiff); break;
        default: soak = godice(armordice+soakdice, soakdiff); break;
    }

    if (soak < 0)
        soak = 0;

    damsuccess -= soak;

    //turn successes into real damage via the modifier.
    dam = damsuccess*modifier;
    // Bashing damage is halved.
    if (dam_type == DAM_BASH && IS_VAMP(victim))
        dam /= 2;

    //Newbie Bonus. Whee!
    if (IS_NEWBIE(ch))
        dam = 3*dam/2;

    //Protective Spells go here!
    if (IS_AFFECTED(victim, AFF_SANCTUARY))
        dam /= 2;
    if (IS_NEWBIE(victim))
        dam = 2*dam/3;

	if (dam < 1)
		dam = 0;

        switch(check_immune(victim,dam_type))
        {
            case(IS_IMMUNE):
                immune = TRUE;
                dam = 0;
                break;
            case(IS_RESISTANT):
                dam -= dam/2;
                break;
            case(IS_VULNERABLE):
                dam *= 2;
                break;
        }

    if(IS_SET(victim->vuln_flags, VULN_SILVER))
    {
        OBJ_DATA *silver_weapon;
        if (((silver_weapon = get_eq_char(ch, WEAR_WIELD)) != NULL && (IS_OBJ_STAT(silver_weapon,ITEM_SILVER)) || 
            (is_affected(ch, gsn_gift_silverclaws) && get_eq_char(ch, WEAR_WIELD) == NULL)) 
//            && (victim->pcdata->shiftform != ch->pcdata->breed || victim->pcdata->breed == METIS) 
            )
            {
                silver = TRUE;
                dam = dam*3/2;
            }
    }

if (DEBUG_MESSAGES || IS_DEBUGGING(ch))	{
        cprintf(ch, "M{D%d{x Ar{c%d{x sd{c%d{x s{c%d{x ", modifier, armordice, soakdice, soak, dam);
        if (IS_NPC(ch))
        cprintf(victim, "{xm{D%d Ar{c%d {xsd{c%d{x s{c%d{x ", modifier, armordice, soakdice, soak, dam);
}

    if(slaughter)
        dam *= 4;
    else if (doubledam)
        dam *= 2;

    if (show)
        dam_message( ch, victim, dam, dt, immune );

    if (silver)
    {
        act("$N screams in pain as the silver sears $S flesh!", ch, NULL, victim, TO_NOTVICT);
        act("You scream in pain as the silver sears your flesh!", ch, NULL, victim, TO_VICT);
        act("$N screams in pain as the silver sears $S flesh!", ch, NULL, victim, TO_CHAR);
        victim->agg_dam += dam / 10;
    }

		if (dam < 1)
			return FALSE;
    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */
    victim->hit -= dam;
    if ( victim->hit < 1 && ((!IS_NPC(victim)
    &&   !IS_SET(victim->act,PLR_ARENA)
    &&   victim->level >= LEVEL_IMMORTAL)
    || !fatal) )
    victim->hit = 1;

    if(dt != gsn_fireshield && dt != gsn_holyshield && dt != gsn_darkshield && dt != gsn_gift_porcupine && dt != gsn_vicissitude_bonecraft)
    {
        /* fireshield */
        if(IS_AFFECTED2(victim, AFF2_FIRESHIELD) && !IS_AFFECTED2(ch, AFF2_FIRESHIELD) && !IS_SET(ch->imm_flags,IMM_FIRE) && !is_affected(ch,gsn_negationfield))
        {

            act2("{RYour fireshield sprays out and incinerates $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{R$n's fireshield sprays out and incinerates $N!{x",victim,NULL,ch,TO_NOTVICT);
            act2("{R$n's fireshield sprays out and incinerates you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_fireshield, DAM_FIRE, TRUE);
            fire_effect(ch,ch->level/2,dam/4,TARGET_CHAR);
        }
        /* holyshield */
        if(IS_AFFECTED2(victim, AFF2_HOLYSHIELD) && !IS_AFFECTED2(ch, AFF2_HOLYSHIELD) && !IS_SET(ch->imm_flags,IMM_HOLY) && !is_affected(ch,gsn_negationfield))
        {
            act2("{WYour Radiant Aura flashes out towards $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{W$n's Radiant Aura flashes out and hits $N!{x",victim,NULL,ch,TO_NOTVICT);
            act2("{W$n's Radiant Aura flashes out and hits you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam*2, gsn_holyshield, DAM_HOLY, TRUE);
        }
        /* darkshield */
        if(IS_AFFECTED2(victim, AFF2_DARKSHIELD) && !IS_AFFECTED2(ch, AFF2_DARKSHIELD) && !IS_SET(ch->imm_flags,IMM_NEGATIVE) && !is_affected(ch,gsn_negationfield))
        {

            act2("{DYour Dark Aura strikes out to envelope $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{D$n's Dark Aura strikes out and envelopes $N!{x",victim,NULL,ch,TO_CHAR);
            act2("{D$n's Dark Aura strikes out and envelopes you!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam*2, gsn_darkshield, DAM_NEGATIVE, TRUE);
        }
        if(is_affected(victim,gsn_gift_porcupine) && !is_affected(ch,gsn_gift_porcupine) && !is_affected(ch,gsn_negationfield))
        {
            act2("{YYou growl a laugh as $N is impaled on your spikes.{x",victim,NULL,ch,TO_CHAR);
            act2("{Y$n growls a cruel laugh as $N is impaled upon their spikes.{x",victim,NULL,ch,TO_NOTVICT);
            act2("{RYou scream in agony as $n's spikes pierce your flesh!{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_gift_porcupine, DAM_PIERCE, TRUE);
        }
        /* bonecrafting */
        if(is_affected(victim,gsn_vicissitude_bonecraft) && !is_affected(ch,gsn_vicissitude_bonecraft) && !is_affected(ch,gsn_negationfield))
        {
            act2("{YYou feel the feather touch of raw flesh as $N is impaled upon your bone spikes.{x",victim,NULL,ch,TO_CHAR);
            act2("$n smiles slightly at the feel of $N impaled upon their bone spikes.{x",victim,NULL,ch,TO_NOTVICT);
            act2("{RYou feel a sharp pain as $n's bone spikes slice through your body.{x",victim,NULL,ch,TO_VICT);
            damage( victim, ch, dam, gsn_vicissitude_bonecraft, DAM_PIERCE, TRUE);
        }

    }

    if((victim->race == race_lookup("methuselah") || victim->race == race_lookup("vampire")) && victim->hit <= 0 && !IS_IMMORTAL(victim) && !IS_SET(victim->act,PLR_ARENA))
    {
            victim->position = POS_TORPOR;
            victim->hit = -20;

                        if((number_range(1,100) < 50) && (!IS_NPC(victim)))
                        {
                                victim->perm_stat[number_range(0, 4)] -= 1;
                                update_csstats(victim);
                                send_to_char("Your struggle for life has permanently weakened you!\n\r",victim);
                        }

            stop_fighting( victim, TRUE );
            if(IS_NPC(ch)) grudge_update(ch,victim);
            if(IS_NPC(victim)) grudge_update(victim,ch);

            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( d->connected == CON_PLAYING &&
                d->character != victim &&
                is_same_clan(victim,d->character) && IS_VAMP(d->character) &&
                !IS_SET(d->character->comm,COMM_NOCLAN))
                {
                act_new("You sense that $n has entered Torpor!",victim,NULL,d->character,TO_VICT,POS_DEAD, TRUE);
                }
            }

            OBJ_DATA *obj, *obj_next;

            /* rot_torp - Ugha */

            for ( obj = victim->carrying; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_content;

                if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH))
                {

                    if(obj->wear_loc == WEAR_FLOAT)
                    {
                        if (obj->contains != NULL)
                        {

                            OBJ_DATA *in, *in_next;

                            act("$p evaporates,scattering its contents.",victim,obj,NULL,TO_ROOM);
                            for (in = obj->contains; in != NULL; in = in_next)
                            {
                                in_next = in->next_content;
                                obj_from_obj(in);
                                obj_to_room(in,victim->in_room);
                            }
                        }
                        else act("$p evaporates.",victim,obj,NULL,TO_ROOM);

                        extract_obj(obj);
                    }
                    else
                    {
                        obj->timer = number_range(5,10);
                        REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
                    }
                }
            }
    }
    update_pos( victim );
    if(IS_NPC(ch)) grudge_update(ch,victim);
    if(IS_NPC(victim)) grudge_update(victim,ch);

    switch( victim->position )
    {
    case POS_MORTAL:
    act( "$n is mortally wounded, and will die soon, if not aided.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char(
        "You are mortally wounded, and will die soon, if not aided.\n\r",
        victim );
    break;

    case POS_INCAP:
    act( "$n is incapacitated and will slowly die, if not aided.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char(
        "You are incapacitated and will slowly die, if not aided.\n\r",
        victim );
    break;

    case POS_STUNNED:
    act( "$n is stunned, but will probably recover.",
        victim, NULL, NULL, TO_ROOM );
    send_to_char("You are stunned, but will probably recover.\n\r",
        victim );
    break;

    case POS_TORPOR:
    act( "$n falls to the floor as they enter {RT{ro{Drp{ro{Rr{x.",
        victim, NULL, NULL, TO_ROOM );
    if (IS_NPC(victim))
        act("'Behead' $n to send them into Final Death!", victim, NULL, NULL, TO_NEWBIEALL);
    send_to_char("You enter {RT{ro{Drp{ro{Rr{x.\n\r",
        victim );
    break;


    case POS_DEAD:
    act( "{rDeath has claimed $n!{x", victim, 0, 0, TO_ROOM );
        if (!IS_SET(victim->act,PLR_ARENA))
        {
            send_to_char( "{RYou take a mortal wound to the heart...\n\r",victim);
            send_to_char( "{RAs your life blood slips away, your last sight in this\n\r",victim);
            send_to_char( "{Rworld is of your enemy, gloating over your beaten corpse.\n\r",victim);
            send_to_char( "{RSuddenly, you feel your soul ripped away from your body...\n\r\n\r",victim);
            send_to_char( "{RAnd the Great God Zelan places you within a new whole and healthy body.{x\n\r\n\r", victim );
        }
    break;

    default:
    if ( dam > victim->max_hit / 4 )
        send_to_char( "That really did HURT!\n\r", victim );
    if ( victim->hit < victim->max_hit / 4 )
        send_to_char( "You sure are BLEEDING!\n\r", victim );
    break;
    }

    /*
     * Sleep spells and extremely wounded folks.
     */
    if ( !IS_AWAKE(victim) )
    stop_fighting( victim, FALSE );

    /*
     * Payoff for killing things.
     */
    if ( victim->position == POS_DEAD )
    {
    kill_em(ch, victim);
    return TRUE;
    }

    if ( victim == ch )
    return TRUE;

    /*
     * Take care of link dead people.
     */
    if ( !IS_NPC(victim) && victim->desc == NULL )
    {
    if ( number_range( 0, victim->wait ) == 0 )
    {
        do_function(victim, &do_recall, "" );
        return TRUE;
    }
    }

    /*
     * Wimp out?
     */
    if ( IS_NPC(victim) && dam > 0 && victim->wait < PULSE_VIOLENCE / 2)
    {
    if ( ( IS_SET(victim->act, ACT_WIMPY) && number_bits( 2 ) == 0
    &&   victim->hit < victim->max_hit / 7)
    ||   ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
    &&     victim->master->in_room != victim->in_room ) )
    {
        do_function(victim, &do_flee, "" );
    }
    }

    if ( !IS_NPC(victim)
    &&   victim->hit > 0
    &&   victim->hit <= victim->wimpy
    &&   victim->wait < PULSE_VIOLENCE / 2 )
    {
    do_function (victim, &do_flee, "" );
    }

    tail_chain( );
    return TRUE;




}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
    return TRUE;

    if (victim->fighting == ch || victim == ch)
    return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL)
    return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {

    /* safe room? */
    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
    {
        send_to_char("Not in this room.\n\r",ch);
        return TRUE;
    }

    if (victim->pIndexData->pShop != NULL)
    {
        send_to_char("The shopkeeper wouldn't like that.\n\r",ch);
        return TRUE;
    }

    /* no killing healers, trainers, etc */
    if (IS_SET(victim->act,ACT_IS_HEALER)
    ||  IS_SET(victim->act,ACT_IS_CHANGER))
    {
        send_to_char("I don't think Matthew would approve.\n\r",ch);
        return TRUE;
    }

    if (!IS_NPC(ch))
    {
        /* no pets */
        if (IS_SET(victim->act,ACT_PET))
        {
        act("But $N looks so cute and cuddly...",
            ch,NULL,victim,TO_CHAR);
        return TRUE;
        }

        /* no charmed creatures unless owner */
        if (IS_AFFECTED(victim,AFF_CHARM) && ch != victim->master)
        {
        send_to_char("You don't own that monster.\n\r",ch);
        return TRUE;
        }
    }
    }
    /* killing players */
    else
    {
    /* NPC doing the killing */
    if (IS_NPC(ch))
    {
        /* safe room check */
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        {
        send_to_char("Not in this room.\n\r",ch);
        return TRUE;
        }

        /* charmed mobs and pets cannot attack players while owned */
        if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
        &&  ch->master->fighting != victim)
        {
        send_to_char("Players are your friends!\n\r",ch);
        return TRUE;
        }
    }
    }
    return FALSE;
}

bool is_aggsafe(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if (victim->in_room == NULL || ch->in_room == NULL)
    return TRUE;

    if (victim->fighting == ch || victim == ch)
    return FALSE;

    /* NPC doing the killing */
    if (IS_NPC(ch))
    {
        if (!can_see(ch, victim) || !IS_AWAKE(ch))
        return TRUE;
        /* safe room check */
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        {
        return TRUE;
        }

        if (ch->pIndexData->pShop != NULL)
        {
            return TRUE;
        }

        if (IS_SET(ch->act,ACT_IS_HEALER)
        ||  IS_SET(ch->act,ACT_IS_CHANGER))
        {
            return TRUE;
        }
        /* charmed mobs and pets cannot attack players while owned */
        if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
        &&  ch->master->fighting != victim)
        {
        return TRUE;
        }
    }
    return FALSE;
}

bool is_safe_spell(CHAR_DATA *ch, CHAR_DATA *victim, bool area )
{
    if (victim->in_room == NULL || ch->in_room == NULL)
        return TRUE;

    if (victim == ch && area)
    return TRUE;

    if (victim->fighting == ch || victim == ch)
    return FALSE;

    if (IS_IMMORTAL(ch) && ch->level > LEVEL_IMMORTAL && !area)
    return FALSE;

    /* killing mobiles */
    if (IS_NPC(victim))
    {
    /* safe room? */
    if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        return TRUE;

    if (victim->pIndexData->pShop != NULL)
        return TRUE;

    /* no killing healers, trainers, etc */
    if ( IS_SET(victim->act,ACT_IS_HEALER)
    ||  IS_SET(victim->act,ACT_IS_CHANGER))
        return TRUE;

    if (!IS_NPC(ch))
    {
        /* no pets */
        if (IS_SET(victim->act,ACT_PET))
        return TRUE;

        /* no charmed creatures unless owner */
        if (IS_AFFECTED(victim,AFF_CHARM) && (area || ch != victim->master))
        return TRUE;

        /* legal kill? -- cannot hit mob fighting non-group member */
        if (victim->fighting != NULL && !is_same_group(ch,victim->fighting))
        return TRUE;
    }
    else
    {
        /* area effect spells do not hit other mobs */
        if (area && !is_same_group(victim,ch->fighting))
        return TRUE;
    }
    }
    /* killing players */
    else
    {
    if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
        return TRUE;

    /* NPC doing the killing */
    if (IS_NPC(ch))
    {
        /* charmed mobs and pets cannot attack players while owned */
        if (IS_AFFECTED(ch,AFF_CHARM) && ch->master != NULL
        &&  ch->master->fighting != victim)
        return TRUE;

        /* safe room? */
        if (IS_SET(victim->in_room->room_flags,ROOM_SAFE))
        return TRUE;

        /* legal kill? -- mobs only hit players grouped with opponent*/
        if (ch->fighting != NULL && !is_same_group(ch->fighting,victim))
        return TRUE;
    }

    /* player doing the killing */
    else
    {
        if (!is_clan(ch))
        return TRUE;

        if (IS_SET(victim->act,PLR_KILLER) || IS_SET(victim->act,PLR_THIEF))
        return FALSE;

        if (!is_clan(victim))
        return TRUE;

        if (ch->level > victim->level + 8)
        return TRUE;
    }

    }
    return FALSE;
}
/*
 * See if an attack justifies a KILLER flag.
 */


/*
 * Check for parry.
 */
bool check_parry( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
    return FALSE;

    if(!IS_NPC(victim)) chance = get_skill(victim,gsn_parry) / 6;
    else chance = get_skill(victim,gsn_parry) /2;

    if ( get_eq_char( victim, WEAR_WIELD ) == NULL )
    {
        if (IS_NPC(victim)) chance /= 2;
        else
        {
            if (get_skill(ch,gsn_hand_to_hand) <= 75) return FALSE;
            chance *= godice(get_attribute(victim,DEXTERITY)+victim->csabilities[CSABIL_BRAWL],6);
        }
    }
    else if(!IS_NPC(victim)) chance *= godice(get_attribute(victim,DEXTERITY)+victim->csabilities[CSABIL_MELEE],6);

    if (!can_see(ch,victim))
    chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
    return FALSE;

    if(!IS_SET(victim->comm,COMM_COMBAT_BRIEF))
        act( "You parry $n's attack.",  ch, NULL, victim, TO_VICT    );
    if(!IS_SET(ch->comm,COMM_COMBAT_BRIEF))
        act( "$N parries your attack.", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_parry,TRUE,6);
    return TRUE;
}

/*
 * Check for shield block.
 */
bool check_shield_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
        return FALSE;


    chance = get_skill(victim,gsn_shield_block) / 5 + 3;


    if ( get_eq_char( victim, WEAR_SHIELD ) == NULL )
        return FALSE;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;

    if(!IS_SET(victim->comm,COMM_COMBAT_BRIEF))
        act( "You block $n's attack with your shield.",  ch, NULL, victim,TO_VICT    );
    if(!IS_SET(ch->comm,COMM_COMBAT_BRIEF))
        act( "$N blocks your attack with a shield.", ch, NULL, victim,TO_CHAR    );
    check_improve(victim,gsn_shield_block,TRUE,6);
    return TRUE;
}


/*
 * Check for dodge.
 */
bool check_dodge( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
    return FALSE;

    if ( MOUNTED(victim) )
    return FALSE;

    if(!IS_NPC(victim))
    {
        chance = get_skill(victim,gsn_dodge) / 6;
        chance *= godice(get_attribute(victim,DEXTERITY)+victim->csabilities[CSABIL_DODGE],6);
    }
    else chance = get_skill(victim,gsn_dodge) / 2;

    if (!can_see(victim,ch))
    chance /= 2;

/*    if ( number_percent( ) >= chance + victim->level - ch->level ) */
    if ( number_percent( ) >= chance)
        return FALSE;
    if(!IS_SET(victim->comm,COMM_COMBAT_BRIEF))
        act( "You dodge $n's attack.", ch, NULL, victim, TO_VICT    );
    if(!IS_SET(ch->comm,COMM_COMBAT_BRIEF))
        act( "$N dodges your attack.", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_dodge,TRUE,6);
    return TRUE;
}

/* check for guardsman block */
bool check_block( CHAR_DATA *ch, CHAR_DATA *victim )
{
    int chance;

    if ( !IS_AWAKE(victim) )
    return FALSE;

    if (get_skill(victim,gsn_block) == 0) return FALSE;

    chance = get_skill(victim,gsn_block) / 2;

    if (!can_see(victim,ch))
    chance /= 2;

    if ( number_percent( ) >= chance + victim->level - ch->level )
        return FALSE;
    if(!IS_SET(victim->comm,COMM_COMBAT_BRIEF))
        act( "You manage to block $n's attack.", ch, NULL, victim, TO_VICT    );
    if(!IS_SET(ch->comm,COMM_COMBAT_BRIEF))
        act( "$N swiftly blocks your attack.", ch, NULL, victim, TO_CHAR    );
    check_improve(victim,gsn_block,TRUE,6);
    return TRUE;
}



/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    AFFECT_DATA af;
    int torporduration = 0;

/*Sengir removed     !IS_NPC(victim)    from line below, to let mobs torp*/
    if ( victim->hit == -20 && !IS_SET(victim->act,PLR_ARENA))
    {
            if(victim->race == race_lookup("methuselah") || victim->race == race_lookup("vampire"))
            {
            victim->position = POS_TORPOR;
            if (!IS_NPC(victim)) {
                torporduration = 31 - (victim->pcdata->cshumanity * 3);
                torporduration = UMAX(1, torporduration);
                torporduration = number_range(1, torporduration);
            }
            if (IS_NPC(victim))
                torporduration = victim->level / 20 + 5;

            if(!is_affected(victim, gsn_torpor))
            {

                af.where    = TO_AFFECTS;
                af.type     = gsn_torpor;
                af.level    = victim->level;
                af.duration = torporduration;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector    = 0;
                affect_to_char( victim, &af );

            }
        }

        else
            victim->position = POS_DEAD;
        return;
    }
/* Bonus for staying human, immunity to agg_damage - Ugha */
    if(victim->race == race_lookup("human")) victim->agg_dam = 0;
    if(victim->agg_dam >= victim->max_hit)
        victim->agg_dam = (victim->max_hit-1);
    if(victim->hit > (victim->max_hit - victim->agg_dam))
        victim->hit = (victim->max_hit - victim->agg_dam);


    if ( victim->hit > 0 )
    {
        if ( victim->position <= POS_STUNNED )
        victim->position = POS_STANDING;
    return;
    }

    if ( IS_NPC(victim) && victim->hit < 1 )
    {
    victim->position = POS_DEAD;
    return;
    }

    if ( victim->hit <= -11 )
    {
        victim->position = POS_DEAD;
        return;
    }

         if ( victim->hit <= -6 ) victim->position = POS_MORTAL;
    else if ( victim->hit <= -3 ) victim->position = POS_INCAP;
    else                          victim->position = POS_STUNNED;

    return;
}



/*
 * Start fights.
 */
void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
    affect_strip( ch, gsn_sleep );

    if (is_affected(ch, gsn_vampire_regen))
        affect_strip(ch, gsn_vampire_regen);

    ch->fighting = victim;
    ch->position = POS_FIGHTING;

    return;
}



/*
 * Stop fights.
 */
void stop_fighting( CHAR_DATA *ch, bool fBoth )
{
    CHAR_DATA *fch;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
    if ( fch == ch || ( fBoth && fch->fighting == ch ) )
    {
        fch->fighting   = NULL;
        fch->position   = IS_NPC(fch) ? fch->default_pos : POS_STANDING;
        update_pos( fch );
    }
    }

    return;
}



/*
 * Make a corpse out of a character.
 */
void make_corpse( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char mobname[MAX_STRING_LENGTH];
    OBJ_DATA *corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *money;
    OBJ_DATA *plasm;
    char *name;
    int blood = 0;

    one_argument(ch->name, mobname);


    if ( IS_NPC(ch))
    {
      name        = ch->short_descr;
      corpse      = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
      corpse->timer   = number_range( 3, 6 );
      corpse->value[0]    = ch->pIndexData->vnum;

      if (has_blood(ch))
        blood = (ch->level / 4) + 5;

        if( ch->race == race_lookup("dhampire") ||
            ch->race == race_lookup("garou") ||
            ch->race == race_lookup("vampire") ||
            ch->race == race_lookup("methuselah") ||
            ch->race == race_lookup("bygone") ||
            ch->race == race_lookup("dragon") ||
            ch->race == race_lookup("fera"))
          blood = blood * 3 / 2;

        if( ch->race == race_lookup("amphibian") ||
            ch->race == race_lookup("ape") ||
            ch->race == race_lookup("bat") ||
            ch->race == race_lookup("bear") ||
            ch->race == race_lookup("cat") ||
            ch->race == race_lookup("dog") ||
            ch->race == race_lookup("fish") ||
            ch->race == race_lookup("fox") ||
            ch->race == race_lookup("lizard") ||
            ch->race == race_lookup("marine mammal") ||
            ch->race == race_lookup("monkey") ||
            ch->race == race_lookup("pig") ||
            ch->race == race_lookup("rabbit") ||
            ch->race == race_lookup("rodent") ||
            ch->race == race_lookup("snake") ||
            ch->race == race_lookup("song bird") ||
            ch->race == race_lookup("water fowl") ||
            ch->race == race_lookup("wolf"))
          blood = blood / 2;

      corpse->value[2] = blood;

      if ( ch->gold > 0 )
      {
        money = create_money(ch->gold, ch->silver);
        if (IS_AFFECTED2(ch, AFF2_UMBRA))
            SET_BIT(money->extra_flags,ITEM_UMBRA);
        obj_to_obj(money, corpse);
        ch->gold = 0;
        ch->silver = 0;
      }
      corpse->cost = 0;
    }
    else
    {
    name        = ch->name;
    corpse      = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
    corpse->timer   = number_range( 25, 40 );
    REMOVE_BIT(ch->act,PLR_CANLOOT);
    if (!is_clan(ch))
        corpse->owner = str_dup(ch->name);
    else
    {
        corpse->owner = NULL;
        if (ch->gold > 1 || ch->silver > 1)
        {
        money = create_money(ch->gold / 2, ch->silver / 2);
        if (IS_AFFECTED2(ch, AFF2_UMBRA))
            SET_BIT(money->extra_flags, ITEM_UMBRA);
        obj_to_obj(money, corpse);
        ch->gold -= ch->gold/2;
        ch->silver -= ch->silver/2;
        }
    }

    corpse->cost = 0;
    }

    corpse->level = ch->level;

    sprintf( buf, corpse->short_descr, name );
    free_string( corpse->short_descr );
    corpse->short_descr = str_dup( buf );

    sprintf( buf, corpse->description, name );
    free_string( corpse->description );
    corpse->description = str_dup( buf );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
    bool floating = FALSE;

    obj_next = obj->next_content;
    if (obj->wear_loc == WEAR_FLOAT)
        floating = TRUE;
    obj_from_char( obj );
    if (obj->item_type == ITEM_PILL)
        obj->timer = number_range(500,1000);
    if (obj->item_type == ITEM_POTION)
        obj->timer = number_range(500,1000);
    if (obj->item_type == ITEM_SCROLL)
        obj->timer = number_range(1000,2500);
    if (IS_SET(obj->extra_flags,ITEM_ROT_DEATH) && !floating)
    {
        obj->timer = number_range(5,10);
        REMOVE_BIT(obj->extra_flags,ITEM_ROT_DEATH);
    }
    if(IS_AFFECTED2(ch, AFF2_UMBRA))
        SET_BIT(obj->extra_flags,ITEM_UMBRA);
    REMOVE_BIT(obj->extra_flags,ITEM_VIS_DEATH);

    if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
        extract_obj( obj );
    else if (floating)
    {
        if (IS_OBJ_STAT(obj,ITEM_ROT_DEATH)) /* get rid of it! */
        {
        if (obj->contains != NULL)
        {
            OBJ_DATA *in, *in_next;

            act("$p evaporates,scattering its contents.",
            ch,obj,NULL,TO_ROOM);
            for (in = obj->contains; in != NULL; in = in_next)
            {
            in_next = in->next_content;
            obj_from_obj(in);
            obj_to_room(in,ch->in_room);
            }
         }
         else
            act("$p evaporates.",
            ch,obj,NULL,TO_ROOM);
         extract_obj(obj);
        }
        else
        {
        act("$p falls to the floor.",ch,obj,NULL,TO_ROOM);
        obj_to_room(obj,ch->in_room);
        }
    }
    else
    {
            if( ch->race == race_lookup("MIB") ||
                ch->race == race_lookup("spirit") ||
                ch->race == race_lookup("wraith") ||
                ch->race == race_lookup("bane") )
            obj_to_room(obj, ch->in_room);
            else
            obj_to_obj( obj, corpse );
    }
    }
        if( ch->race == race_lookup("MIB") )
        {
          act("As it falls to the ground, $n's body melts away into nothingness.", ch, NULL, NULL, TO_ROOM);
          return;
        }
        if (ch->race == race_lookup("spirit") || ch->race == race_lookup("bane"))
        {
          act("After a brief flickering, $n fades from sight as if it had never existed.", ch, NULL, NULL, TO_ROOM);
          return;
        }
        if (ch->race == race_lookup("wraith"))
        {
          act("An unholy shriek bellows from $n, as $e is sucked into a briefly appearing void.", ch, NULL, NULL, TO_ROOM);
            if (number_range(1,10) > 9)
            {
                plasm = create_object(get_obj_index(OBJ_VNUM_PLASM), 1);
                if(IS_AFFECTED2(ch, AFF2_UMBRA))
                    SET_BIT(plasm->extra_flags,ITEM_UMBRA);
                plasm->value[0] = ch->level / 4 + 1;
                obj_to_room(plasm, ch->in_room);
              act("A piece of {Dplasm{x sloughs off of $n as $e vanishes into the void.", ch, NULL, NULL, TO_ROOM);
            }
          return;
        }

    if(str_cmp(mobname, "corpse"))
        obj_to_room( corpse, ch->in_room );
    else
        obj_to_room( corpse, get_room_index( 1 ));

    if(IS_AFFECTED2(ch, AFF2_UMBRA))
        SET_BIT(corpse->extra_flags,ITEM_UMBRA);
    return;
}



/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry( CHAR_DATA *ch )
{
    ROOM_INDEX_DATA *was_in_room;
    char *msg;
    int door;
    int vnum;

    vnum = 0;
    msg = "You hear $n's death cry.";

    switch ( number_bits(4))
    {
    case  0: msg  = "$n hits the ground ... DEAD.";         break;
    case  1:
    if (ch->material == 0)
    {
        msg  = "$n splatters blood on your armor.";
        break;
    }
    case  2:
    if (IS_SET(ch->parts,PART_GUTS))
    {
        msg = "$n spills $s guts all over the floor.";
        vnum = OBJ_VNUM_GUTS;
    }
    break;
    case  3:
    if (IS_SET(ch->parts,PART_HEAD))
    {
        msg  = "$n's severed head plops on the ground.";
        vnum = OBJ_VNUM_SEVERED_HEAD;
    }
    break;
    case  4:
    if (IS_SET(ch->parts,PART_HEART))
    {
        msg  = "$n's heart is torn from $s chest.";
        vnum = OBJ_VNUM_TORN_HEART;
    }
    break;
    case  5:
    if (IS_SET(ch->parts,PART_ARMS))
    {
        msg  = "$n's arm is sliced from $s dead body.";
        vnum = OBJ_VNUM_SLICED_ARM;
    }
    break;
    case  6:
    if (IS_SET(ch->parts,PART_LEGS))
    {
        msg  = "$n's leg is sliced from $s dead body.";
        vnum = OBJ_VNUM_SLICED_LEG;
    }
    break;
    case 7:
    if (IS_SET(ch->parts,PART_BRAINS))
    {
        msg = "$n's head is shattered, and $s brains splash all over you.";
        vnum = OBJ_VNUM_BRAINS;
    }
    }

    act( msg, ch, NULL, NULL, TO_ROOM );

    if ( vnum != 0 )
    {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    char *name;

    name        = IS_NPC(ch) ? ch->short_descr : ch->name;
    obj     = create_object( get_obj_index( vnum ), 0 );
    obj->timer  = number_range( 4, 7 );

    sprintf( buf, obj->short_descr, name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, obj->description, name );
    free_string( obj->description );
    obj->description = str_dup( buf );

    if (obj->item_type == ITEM_FOOD)
    {
        if (IS_SET(ch->form,FORM_POISON))
        obj->value[3] = 1;
        else if (!IS_SET(ch->form,FORM_EDIBLE))
        obj->item_type = ITEM_TRASH;
    }

    if(IS_AFFECTED2(ch, AFF2_UMBRA))
        SET_BIT(obj->extra_flags,ITEM_UMBRA);


    obj_to_room( obj, ch->in_room );
    }

    if ( IS_NPC(ch) )
    msg = "You hear something's death cry.";
    else
    msg = "You hear someone's death cry.";

    was_in_room = ch->in_room;
    for ( door = 0; door <= 5; door++ )
    {
    EXIT_DATA *pexit;

    if ( ( pexit = was_in_room->exit[door] ) != NULL
    &&   pexit->u1.to_room != NULL
    &&   pexit->u1.to_room != was_in_room )
    {
        ch->in_room = pexit->u1.to_room;
        act( msg, ch, NULL, NULL, TO_ROOM );
    }
    }
    ch->in_room = was_in_room;

    return;
}



void raw_kill( CHAR_DATA *victim )
{
    int i;

    stop_fighting( victim, TRUE );
    death_cry( victim );
    make_corpse( victim );

    if ( IS_NPC(victim) )
    {
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL-1)].killed++;
    extract_char( victim, TRUE );
    return;
    }
    extract_char( victim, FALSE );
    while ( victim->affected )
    affect_remove( victim, victim->affected );
    victim->affected_by = race_table[victim->race].aff;
    for (i = 0; i < 4; i++)
        victim->armor[i]= 100;
    victim->position    = POS_RESTING;
    victim->hit     = UMAX( 1, victim->hit  );
    victim->mana    = UMAX( 1, victim->mana );
    victim->move    = UMAX( 1, victim->move );
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    AFFECT_DATA af;
    OBJ_DATA *doubloon;
    OBJ_DATA *bead;
    int xp;
    int qp;
    int fakexp;
    int fakeqp;
    int members;
    extern bool doubleexp;
    extern bool manualxp;
    extern bool spookums;
    extern bool carnival;
    int group_levels;

    /*
     * Monsters don't get kill xp's or alignment changes.
     * P-killing doesn't help either.
     * Dying of mortal wounds or poison doesn't give xp to anyone!
     */
    if ( victim == ch )
    return;

    members = 0;
    group_levels = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if(gch == NULL) break;
        if (gch == ch->pet) continue;
        if ( is_same_group( gch, ch ) )
        {
            members++;
            group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
        }
    }

    if ( members == 0 )
    {
        bug( "Group_gain: members.", members );
        members = 1;
        group_levels = ch->level ;
    }

    lch = (ch->leader != NULL) ? ch->leader : ch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        if ( !is_same_group( gch, ch ) || IS_NPC(gch))
            continue;

        xp = xp_compute( gch, victim, group_levels );
        qp = number_range(1, xp/200);
        if (gch == lch && members > 1)
        qp *= 2;
        if (victim->level < gch->level || ch->leader != NULL)
        qp /= 2;
        if (victim->level + 20 < gch->level)
        qp = 0;

        if (!IS_NPC(gch) && gch->cswillpower < gch->csmax_willpower &&
        victim->level > gch->level &&
        ((gch == ch && number_percent() < (3 * (10-gch->cswillpower))) ||
        (gch != ch && number_percent() < (3 * (10-gch->cswillpower) / 2 ))) &&
        (godice(gch->pcdata->csvirtues[CSVIRT_COURAGE], 9) > 0))
        {
            gch->cswillpower++;
            sendch("Your resolve strengthens as yet another of your enemies is slain.\n\r", gch);
        }

        if(xp <= 0)
            xp = 1;

        gch->totalkills ++;
        gch->currentkills ++;
        // Adds QP to global QP from fights
        if (!manualxp && !doubleexp)
            global_qp += qp*qpmult;
        //Added line for people caught cheating.
        if (gch->cheater == 0)
            sprintf( buf, "You receive {g%d{x experience and {c%d{x quest points.\n\r", xp, qp );
        if (gch->cheater == 1)
        {
            fakexp = xp;
            fakeqp = qp;
            xp = 0;
            qp = 0;
            sprintf( buf, "You receive {g%d{x experience and {c%d{x quest points.\n\r", fakexp, fakeqp );
        }
        send_to_char( buf, gch );
        gain_exp( gch, xp );
        gch->qpoints += qp;
        gch->totalqpoints += qp;
        if (gch->qpoints > MAX_QPOINTS)
            gch->qpoints = MAX_QPOINTS;

        if (IS_SET(ch->act2, PLR2_QUESTOR)&&IS_NPC(victim))
        {
            if (ch->questmob == victim->pIndexData->vnum)
            {
                send_to_char("You have almost completed your QUEST!\n\r",ch);
                send_to_char("Return to the questmaster before your time runs out!\n\r",ch);
                ch->questmob = -1;
            }
        }

        if (spookums && number_percent() <= 20)
        {
            send_to_char("{D[       *****      {YTrick or Treat!      {D*****       ]{x\n\r", gch);
            if (number_percent() <= 15)
                send_to_char("{MTRICK!: {mWith a bright {Wflash{m of light, a shower of {Ysparks{m explode around you!{x\n\r", gch);
            else
            {
                if (number_percent() <= 20 && !IS_AFFECTED(gch, AFF_XP_BOOST))
                {
                    af.where     = TO_AFFECTS;
                    af.type  = gsn_xp_boost;
                    af.level     = 1;
                    af.duration  = 5;
                    af.modifier  =  0;
                    af.location  = APPLY_NONE;
                    af.bitvector = AFF_XP_BOOST;
                    affect_to_char( gch, &af );
                    send_to_char("{MTREAT!: {GYou feel a rush of mystic energy filling your bones!{x\n\r", gch);
                }
                else
                {
                    qp = number_range(1,5);
                    gch->qpoints += qp;
                    gch->totalqpoints += qp;
                    if (gch->qpoints > MAX_QPOINTS)
                        gch->qpoints = MAX_QPOINTS;
                    sprintf(buf, "{MTREAT!:  {GMystic energy fills you, as you gain %d quest points.{x\n\r", qp);
                    send_to_char(buf, gch);
                }
            }
        }

        if (carnival && number_percent() <= 15)
        {
            send_to_char("{D[    {M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*      {WThrow me something!     {M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*    {D]{x\n\r", gch);
            if (number_percent() <= 10)
            {
                send_to_char("{YA shiny doubloon lands by your feet, and you pick it up.  The jailer in Chester may be interested.{x\n\r", gch);
                if((doubloon = create_object(get_obj_index(31833),0)) == NULL)
                {
                    send_to_char("Error! Contact an imm at once to fix this missing item!\n\r",gch);
                    bug( "group_gain: Mardi Gras Doubloon [%d] = NULL", 31833);
                    return;
                }
                obj_to_char( doubloon, gch);
            }
            else
            {
                send_to_char("{YA multicolored bead flies past, and you snatch it out of the air!{x\n\r", gch);
                if((bead = create_object(get_obj_index(31832),0)) == NULL)
                {
                    send_to_char("Error! Contact an imm at once to fix this missing item!\n\r",gch);
                    bug( "group_gain: Mardi Gras Bead [%d] = NULL", 31832);
                    return;
                }
                obj_to_char( bead, gch);
            }
            send_to_char("{D[    {M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*      {M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*      {M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*{M*{G*{Y*    {D]{x\n\r", gch);
        }
    }
    return;
}



/*
 * Compute xp for a kill.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp,bxp, ixp,gloxp, xpcl;
    int level_range;
    int playerbonus;
    double dBonus;
    double axp, mxp;
    extern bool doubleexp;
    extern bool manualxp;
    DESCRIPTOR_DATA *d;

    level_range = victim->level - gch->level;

/*Sengir's exp testing, moving to curve later*/
    switch (level_range)
    {
        default:    base_exp =  1;  break;
        case -10:   base_exp =  3;  break;
        case -9:    base_exp =  6;  break;
        case -8:    base_exp =  12; break;
        case -7:    base_exp =  25; break;
        case -6:    base_exp =  50; break;
        case -5:    base_exp =  100; break;
        case -4:    base_exp =  120; break;
        case -3:    base_exp =  140; break;
        case -2:    base_exp =  160; break;
        case -1:    base_exp =  180;    break;
        case 0:     base_exp =  200;    break;
        case 1:     base_exp =  210;    break;
        case 2:     base_exp =  220;    break;
        case 3:     base_exp =  230;    break;
        case 4:     base_exp =  240;    break;
        case 5:     base_exp =  250;    break;
        case 6:     base_exp =  255;    break;
        case 7:     base_exp =  260;    break;
        case 8:     base_exp =  265;    break;
        case 9:     base_exp =  270;    break;
        case 10:    base_exp =  275;    break;
    }

    if (level_range > 10)
        base_exp = 275 + 4 * (level_range - 10);
    if (level_range > 20)
        base_exp = 275 + 2 * (level_range - 10);


    xp = base_exp;
    bxp = xp/2;

    /* randomize the rewards */
    xp = number_range (xp , xp * 5/4);

// Do I turn on double XP? Let's check.
// Rates check as well to lock in multiplier
    if (!manualxp)
    {

        if (xpstat == 0 && global_xp > 149999)
            {
            xpstat = 1;
            if (number_range(1,100) <= 25)
                {
                 doubleexp = TRUE;
                 xpawardmult = 2;
                 for ( d = descriptor_list; d; d = d->next )
                    {
                        if ( d->connected == CON_PLAYING )
                        {
                            send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                        }
                    }

                }
            }
        if (xpstat == 1 && global_xp > 199999)
            {
            xpstat = 2;
            if (number_range(1,100) <= 40)
                {
                 doubleexp = TRUE;
                 xpawardmult = 2;
                 for ( d = descriptor_list; d; d = d->next )
                    {
                        if ( d->connected == CON_PLAYING )
                        {
                            send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                        }
                    }

                }
            }
            if (xpstat == 2 && global_xp > 224999)
                {
                xpstat = 3;
                if (number_range(1,100) <= 55)
                    {
                     doubleexp = TRUE;
                     xpawardmult = 2;
                     for ( d = descriptor_list; d; d = d->next )
                        {
                            if ( d->connected == CON_PLAYING )
                            {
                                send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                            }
                        }

                    }
                }
        if (xpstat == 3 && global_xp > 249999)
            {
            xpstat = 4;
            if (number_range(1,100) <= 70)
                {
                 doubleexp = TRUE;
                 xpawardmult = 2;
                 for ( d = descriptor_list; d; d = d->next )
                    {
                        if ( d->connected == CON_PLAYING )
                        {
                            send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                        }
                    }

                }
            }
            if (xpstat == 4 && global_xp > 274999)
                {
                xpstat = 5;
                if (number_range(1,100) <= 85)
                    {
                     doubleexp = TRUE;
                     xpawardmult = 2;
                     for ( d = descriptor_list; d; d = d->next )
                        {
                            if ( d->connected == CON_PLAYING )
                            {
                                send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                            }
                        }

                    }
                }
        if (xpstat == 5 && global_xp > 300000)
            {
             doubleexp = TRUE;
             xpstat = 6;
             xpawardmult = 3;
             for ( d = descriptor_list; d; d = d->next )
                    {
                        if ( d->connected == CON_PLAYING )
                        {
                            send_to_char( "{R****{YIt's {RT{Gr{Mi{Yp{Bl{Ce {RE{Gx{Mp{Y time!{R****{x\n\r", d->character );
                        }
                    }

            }
    }

    if( IS_AFFECTED2(victim, AFF2_DOUBLE_EXP))
        xp = xp*2;
// Code to drain from global XP and award it.

   if (!manualxp && global_xp > 0 && doubleexp == TRUE)
       {
           global_xp -= xp;
       }

        xp = (xp * xpawardmult);

       xp = xp + (xp*2 / 8);

       // Returns double XP to its default state when it ends

        if (global_xp <= 0 && !manualxp)
       	{
       	if (doubleexp == TRUE)
       		{
       		            for ( d = descriptor_list; d; d = d->next )
                   {
                       if ( d->connected == CON_PLAYING )
                       {
                           if (xpawardmult == 2)
                               send_to_char( "{G****[{YDouble Exp has ended {y:-( {G]****{x\n\r", d->character );
                           if (xpawardmult == 3)
                               send_to_char( "{R****[{Ytriple Exp has ended {y:-( {R]****{x\n\r", d->character );
                       }
                   }
       		}

       	global_xp = 0;
       	doubleexp = FALSE;
       	xpstat = 0;
       	xpawardmult = 1;
       	}


/*Sengir altered extra xp if you are leader*/
    if ((gch->leader == NULL) && (level_range >= -10))
        xp += gch->csabilities[CSABIL_LEADERSHIP] * 5;

/*Sengir remort bonus xp code*/
    if (level_range >= -10)
        xp += gch->remorts / 2;

    /* standard exp cap */
    if(gch->wimpy == 0)
    {
      if(xp > 462+gch->remorts)
        xp = ((462+gch->remorts)*xpawardmult);
    }
    else if(xp > 462) xp = 462; //keep e'm from being pansies

    /*Sengir added deduction for being afkish in group, not contributing*/
    if (gch->timer > 10)
        xp -= xp * gch->timer / 12;

    /*Zelan added bonus XP spells and a newbie xp boost here */
    if ( IS_AFFECTED(gch, AFF_XP_BOOST))
    xp = bxp + xp;



    if ( IS_SET(gch->act2, PLR2_NEWBIE))
    xp =  bxp + xp;
       // Zelan steals XP here to add to global_xp

    if (!manualxp && doubleexp == FALSE)
    global_xp += xp/5;

    if (!IS_NPC(gch) && gch->pcdata->immclass > 0 )
    {
        switch (gch->pcdata->immclass)
        {
            case 1: mxp = (5 * (double) base_exp/100); break;
            case 2: mxp = (15 * (double) base_exp/100); break;
            case 3: mxp = (50 * (double) base_exp/100); break;
        }
        //cprintf(gch, "xp%d ixp%f", xp, ixp);
        mxp += 0.5;
        xp += (int) mxp;
    }

    axp = get_addict_bonus(gch, base_exp);
    xp += axp;

    if (IS_DEBUGGING(gch))
    {
        cprintf(gch, "xp%d base%d axp%f ixp%f\n\r", xp, base_exp, axp, mxp);
    }
    if(victim->leader != NULL)
        return xp/10;
    else
         return xp;

}


void dam_message( CHAR_DATA *ch, CHAR_DATA *victim,int dam,int dt,bool immune )
{
    char buf1[256], buf2[256], buf3[256];
    const char *vs;
    const char *vp;
    const char *attack;
    char punct;

    if (ch == NULL || victim == NULL)
    return;

     if ( dam ==   0 ) { vs = "{wmiss{x";   vp = "{wmisses{x";      }
    else if ( dam <=   4 ) { vs = "{xscratch";  vp = "{xscratches"; }
    else if ( dam <=   8 ) { vs = "{xgraze";    vp = "{xgrazes";        }
    else if ( dam <=  12 ) { vs = "{rhit{x";    vp = "{rhits{x";        }
    else if ( dam <=  16 ) { vs = "{rinjure{x"; vp = "{rinjures{x";     }
    else if ( dam <=  20 ) { vs = "{rwound{x";  vp = "{rwounds{x";      }
    else if ( dam <=  24 ) { vs = "{rmaul{x";       vp = "{rmauls{x";       }
    else if ( dam <=  28 ) { vs = "{rdecimate{x";   vp = "{rdecimates{x";   }
    else if ( dam <=  32 ) { vs = "{Rdevastate{x";  vp = "{Rdevastates{x";  }
    else if ( dam <=  36 ) { vs = "{Rmaim{x";   vp = "{Rmaims{x";       }
    else if ( dam <=  40 ) { vs = "{RMUTILATE{x";   vp = "{RMUTILATES{x";   }
    else if ( dam <=  44 ) { vs = "{RDISEMBOWEL{x"; vp = "{RDISEMBOWELS{x"; }
    else if ( dam <=  48 ) { vs = "{RDISMEMBER{x";  vp = "{RDISMEMBERS{x";  }
    else if ( dam <=  52 ) { vs = "{RMASSACRE{x";   vp = "{RMASSACRES{x";   }
    else if ( dam <=  56 ) { vs = "{RMANGLE{x"; vp = "{RMANGLES{x";     }
    else if ( dam <=  60 ) { vs = "{W*** DEMOLISH ***{x";
                 vp = "{W*** DEMOLISHES ***{x";         }
    else if ( dam <=  75 ) { vs = "{W*** DEVASTATE ***{x";
                 vp = "{W*** DEVASTATES ***{x";         }
    else if ( dam <= 100)  { vs = "{W=== OBLITERATE ==={x";
                 vp = "{W=== OBLITERATES ==={x";        }
    else if ( dam <= 125)  { vs = "{W>>> ANNIHILATE <<<{x";
                 vp = "{W>>> ANNIHILATES <<<{x";        }
    else if ( dam <= 150)  { vs = "{Y>>> ERADICATE <<<{x";
                 vp = "{Y>>> ERADICATES <<<{x";         }
    else if ( dam <= 200)  { vs = "{xdo {WCOLOSSAL{x damage to";
                             vp = "{xdoes {WCOLOSSAL{x damage to";      }
    else if ( dam <= 250)  { vs = "{xdo {YCOLOSSAL{x damage to";
                             vp = "{xdoes {YCOLOSSAL{x damage to";      }
    else if ( dam <= 275)  { vs = "{xdo {WUNSPEAKABLE{x things to";
                             vp = "{xdoes {WUNSPEAKABLE{x things to";   }
    else if ( dam <= 300)  { vs = "{xdo {YUNSPEAKABLE{x things to";
                             vp = "{xdoes {YUNSPEAKABLE{x things to";   }
    else if ( dam <= 800)  { vs = "{xdo {YU{WN{YS{WP{YE{WA{YK{WA{YB{WL{YE{x things to";
                             vp = "{xdoes {YU{WN{YS{WP{YE{WA{YK{WA{YB{WL{YE{x things to";       }
    else               { vs = "{xdo {DUN{wG{WO{wD{DLY{x things to";
                 vp = "{xdoes {DUN{wG{WO{wD{DLY{x things to"; }


    punct   = (dam <= 24) ? '.' : '!';

    if ( dt == TYPE_HIT )
    {
    if (ch  == victim)
    {

        sprintf( buf1, "$n %s $melf%c {R[{G%d{R]{x",vp,punct,dam);
        sprintf( buf2, "You %s yourself%c {R[{G%d{R]{x",vs,punct,dam);
    }
    else
    {
        sprintf( buf1, "$n %s $N%c {R[{G%d{R]{x",  vp, punct,dam );
        sprintf( buf2, "You %s $N%c {R[{G%d{R]{x", vs, punct,dam );
        sprintf( buf3, "$n %s you%c {R[{G%d{R]{x", vp, punct,dam );
    }
    }
    else
    {
    if ( dt >= 0 && dt < MAX_SKILL )
        attack  = skill_table[dt].noun_damage;
    else if ( dt >= TYPE_HIT
    && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
        attack  = attack_table[dt - TYPE_HIT].noun;
    else
    {
        bug( "Dam_message: bad dt %d.", dt );
        dt  = TYPE_HIT;
        attack  = attack_table[0].name;
    }

    if (immune)
    {
        if (ch == victim)
        {
        sprintf(buf1,"$n is unaffected by $s own %s.",attack);
        sprintf(buf2,"Luckily, you are immune to that.");
        }
        else
        {
            sprintf(buf1,"$N is unaffected by $n's %s!",attack);
            sprintf(buf2,"$N is unaffected by your %s!",attack);
            sprintf(buf3,"$n's %s is powerless against you.",attack);
        }
    }
    else
    {
        if (ch == victim)
        {
        sprintf( buf1, "$n's %s %s $m%c {R[{G%d{R]{x",attack,vp,punct,dam);
        sprintf( buf2, "Your %s %s you%c {R[{G%d{R]{x",attack,vp,punct,dam);
        }
        else
        {
            if (dt == TYPE_HIT + attack_lookup("claws"))
            {
                sprintf( buf1, "$n's %s %s $N%c {R[{G%d{R]{x",  attack, vs, punct, dam);
                sprintf( buf2, "Your %s %s $N%c {R[{G%d{R]{x",  attack, vs, punct, dam);
                sprintf( buf3, "$n's %s %s you%c {R[{G%d{R]{x", attack, vs, punct, dam);
            } else {
                sprintf( buf1, "$n's %s %s $N%c {R[{G%d{R]{x",  attack, vp, punct, dam);
                sprintf( buf2, "Your %s %s $N%c {R[{G%d{R]{x",  attack, vp, punct, dam);
                sprintf( buf3, "$n's %s %s you%c {R[{G%d{R]{x", attack, vp, punct, dam);
                }
        }
    }
    }

	if (ch == victim)
    {
    act(buf1,ch,NULL,NULL,TO_ROOM);
    act(buf2,ch,NULL,NULL,TO_CHAR);
    }
    else
    {
		if (dam > 1)
        act( buf1, ch, NULL, victim, TO_NOTVICT );
		if(dam > 1 || !IS_SET(ch->comm,COMM_COMBAT_BRIEF))
        act( buf2, ch, NULL, victim, TO_CHAR );
		if(dam > 1 || !IS_SET(victim->comm,COMM_COMBAT_BRIEF))
        act( buf3, ch, NULL, victim, TO_VICT );
    }
if (ch->maxdamage < dam)
   ch->maxdamage = dam;
    return;
}



/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm( CHAR_DATA *ch, CHAR_DATA *victim )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    return;

    if ( IS_OBJ_STAT(obj,ITEM_NOREMOVE))
    {
    act("$S weapon won't budge!",ch,NULL,victim,TO_CHAR);
    act("$n tries to disarm you, but your weapon won't budge!",
        ch,NULL,victim,TO_VICT);
    act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
    return;
    }

    act( "$n DISARMS you and sends your weapon flying!",
     ch, NULL, victim, TO_VICT    );
    act( "You disarm $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n disarms $N!",  ch, NULL, victim, TO_NOTVICT );

    obj_from_char( obj );
    if ( IS_OBJ_STAT(obj,ITEM_NODROP) || IS_OBJ_STAT(obj,ITEM_INVENTORY) )
    obj_to_char( obj, victim );
    else
    {
    obj_to_room( obj, victim->in_room );
    if(IS_AFFECTED2(victim, AFF2_UMBRA))
        SET_BIT(obj->extra_flags,ITEM_UMBRA);
    if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim,obj))
        get_obj(victim,obj,NULL);
    }

    return;
}

void do_berserk( CHAR_DATA *ch, char *argument)
{
    int berserksuccess = 0;
    int berserkdiff = 7;
    int hp_percent;
    int critical = 0;
    AFFECT_DATA af;

    if ((get_skill(ch,gsn_berserk)) == 0
    ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK)))
    {
    send_to_char("You turn red in the face, but nothing happens.\n\r",ch);
    return;
    }

    if (IS_AFFECTED(ch,AFF_BERSERK) || is_affected(ch,gsn_berserk)
    ||  is_affected(ch,skill_lookup("frenzy")))
    {
    send_to_char("You get a little madder.\n\r",ch);
    return;
    }

    if (IS_AFFECTED(ch,AFF_CALM))
    {
    send_to_char("You're feeling too mellow to berserk.\n\r",ch);
    return;
    }

    if (ch->move < ch->level / 5)
    {
        send_to_char("You are too tired to get angry.\n\r", ch);
        return;
    }

    ch->move -= ch->level / 5;

    /* lower hp = easier to get mad */
    hp_percent = 100 * ch->hit/ch->max_hit;
    if (hp_percent < 75)
      berserkdiff--;
    if (hp_percent < 35)
      berserkdiff--;

    berserksuccess = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_INTIMIDATION], berserkdiff);
    if (berserksuccess > 4)
      critical = berserksuccess * 5;

    if (berserksuccess < 0)
    {
      act("Your anger overwhelms you so completely that you forego your own sense of safety.",ch,NULL,NULL,TO_CHAR);
      WAIT_STATE(ch,4 * PULSE_VIOLENCE);

      af.where    = TO_AFFECTS;
      af.type     = gsn_berserk;
      af.level    = 0;
      af.duration = -(berserksuccess);
      af.modifier = berserksuccess*50;
      af.bitvector = AFF_BERSERK;
      af.location = APPLY_HITROLL;
      affect_to_char(ch,&af);

      af.modifier = -(berserksuccess)*100;
      af.location = APPLY_AC;
      af.bitvector = 0;
      affect_to_char(ch,&af);
      check_improve(ch,gsn_berserk,FALSE,6);
      return;
    }

    if (berserksuccess == 0)
    {
      act("Your pulse speeds up for a brief moment, but you fail to get angry enough.",ch,NULL,NULL,TO_CHAR);
      WAIT_STATE(ch,2 * PULSE_VIOLENCE);
      check_improve(ch,gsn_berserk,FALSE,6);
      return;
    }

    WAIT_STATE(ch,PULSE_VIOLENCE);
    send_to_char("Your pulse races as you are consumed by rage!\n\r",ch);
    act("$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_berserk,TRUE,6);

    af.where    = TO_AFFECTS;
    af.type     = gsn_berserk;
    af.level    = berserksuccess;
    af.duration = 10 + (berserksuccess * 5);
    af.modifier = critical + 10 + ch->level / 6 * berserksuccess;
    af.bitvector    = AFF_BERSERK;
    af.location = APPLY_HITROLL;
    affect_to_char(ch,&af);

    af.bitvector = 0;
    af.modifier = critical + 10 + ch->level / 4 * berserksuccess;
    af.location = APPLY_DAMROLL;
    affect_to_char(ch,&af);

    af.modifier = ch->level / 5 * 10;
    af.location = APPLY_AC;
    affect_to_char(ch,&af);

    gain_exp(ch, berserksuccess*4);
    return;
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *shield;
    int chance;
    int dicesuccess = 0, damagesuccess = 0;
    int mobdice = 2;
    int extradamage = 0;
    int precog = 0;
    one_argument(argument,arg);


    if ( (chance = get_skill(ch,gsn_bash)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH)))
    {
    send_to_char("Bashing? What's that?\n\r",ch);
    return;
    }

    if (!IS_NPC(ch) && ( shield = get_eq_char(ch, WEAR_SHIELD)) == NULL)
    {
        send_to_char("You must have a shield equipped in order to shield bash.\n\r", ch);
        return;
    }

    if (arg[0] == '\0')
    {
    victim = ch->fighting;
    if (victim == NULL)
    {
        send_to_char("But you aren't fighting anyone!\n\r",ch);
        return;
    }
    }

   else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't bash while riding!\n\r", ch);
        return;
    }

    if (victim == ch)
    {
    send_to_char("You try to bash your brains out, but fail.\n\r",ch);
    return;
    }

    if(ch->move < (ch->level / 30))
    {
        send_to_char("You are too tired to shield bash!\n\r", ch);
        return;
    }

    if (is_safe(ch,victim))
    return;

    if ( IS_NPC(victim) &&
    victim->fighting != NULL &&
    !is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
    act("But $N is your friend!",ch,NULL,victim,TO_CHAR);
    return;
    }

    ch->move -= (ch->level / 30);

 /*   if(is_affected(victim,gsn_precognition) && number_percent() > 50)
    {
    	act("Deftly moving out of the way, $N ducks from your shield bash attempt!", ch, NULL, victim, TO_CHAR);
    	act("Sensing the movement of $n's shield an instant before it actually happens, you deftly duck out of the way.", ch, NULL, victim, TO_VICT);
        act("$N ducks swiftly, avoiding $n's attempt to strike with their shield.", ch, NULL, victim, TO_NOTVICT);
    	return;
    }
*/

    if (IS_NPC(ch) )
    {
        if (!IS_SET(ch->off_flags, OFF_BASH))
            return;


        if (ch->race == race_lookup("garou"))
        {
            mobdice++;      /*Garou mobs gain +1 to attack*/
            extradamage = 2;
            if (number_range(1, 5) != 1)    /*80% chance shifted form, +1 */
                mobdice++;
        }

        if (IS_VAMP(ch))
        {
            mobdice++;      /*Vampire blooded get +1*/
            extradamage = 1;
            if (number_range(1, 3) != 1)    /*66% chance of Potence, +1*/               mobdice++;
        }

        if(is_affected(victim,gsn_precognition))
            mobdice -= get_affect_level(victim,gsn_precognition);
        if (mobdice < 0)
            mobdice = 0;

        dicesuccess = godice(mobdice, 6);

        if (dicesuccess < 0)
        {
            act("$n tries to bash your brains in but trips and falls as you deftly step away.", ch, NULL, victim, TO_VICT);
            act("$n trips and falls while attempting to bash $N over the head.", ch, NULL, victim, TO_NOTVICT);
            WAIT_STATE(ch, PULSE_VIOLENCE);
            ch->position = POS_RESTING;
            return;
        }

        if (dicesuccess == 0)
        {
            act("A gentle breeze caresses your cheek as $n very nearly bashes your brains in.", ch, NULL, victim, TO_VICT);
            act("$n attempts to bash $N over the head but misses by a hair.", ch, NULL, victim, TO_NOTVICT);
            return;
        }

        if (dicesuccess > 0)
        {
            act("$n bashes you over the head, knocking the sense out of you!", ch, NULL, victim, TO_VICT);
            act("$n bashes $N over the head and $N looks quite dazed.", ch, NULL, victim, TO_NOTVICT);

            extradamage += dicesuccess -1;
            damagesuccess = godice(mobdice + extradamage, 6);
            if (damagesuccess < 0)
                damagesuccess = 0;
            damage(ch, victim, damagesuccess * ch->level / 2, gsn_bash, DAM_BASH, TRUE);
            WAIT_STATE(victim, PULSE_VIOLENCE);
            return;
        }
    }

    if(is_affected(victim,gsn_precognition))
        precog = get_affect_level(victim,gsn_precognition);

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_MELEE], 6) - precog;

    if(dicesuccess < 0)
    {
        act("You swing $p at $N, but it slips out of your hand as you fail to connect!", ch, shield, victim, TO_CHAR);
        act("$n swings $p at you, but it slips out of $s hand.", ch, shield, victim, TO_VICT);
        act("$n tries to swing $p at $N, but it is knocked from $s hand!", ch, shield, victim, TO_NOTVICT);

        obj_from_char(shield);
        obj_to_room(shield,ch->in_room);
        return;
    }

    if(dicesuccess == 0)
    {
        act("$p glances off of $N.", ch, shield, victim, TO_CHAR);
        act("$n's swing with $p glances off of you.", ch, shield, victim, TO_VICT);
        act("$n tries to slam $p into $N, but it merely glances away.", ch, shield, victim, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE);
        return;
    }

    gain_exp(ch, dicesuccess*2);
    act("You swing $p, forcefully colliding with $N.", ch, shield, victim, TO_CHAR);
    act("$n smashes you with $p.", ch, shield, victim, TO_VICT);
    act("$n slams $p into $N.", ch, shield, victim, TO_NOTVICT);

    if (dicesuccess >= 5)
    {
        act("$p connects solidly with $N's head, causing a mild concussion.", ch, shield, victim, TO_CHAR);
        act("$n's $p connects with your head, rattling your brain.", ch, shield, victim, TO_VICT);
        act("$N appears to be a bit dazed after being hit with $p.", ch, shield, victim, TO_NOTVICT);
        DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
        WAIT_STATE(victim, PULSE_VIOLENCE);
    }

    WAIT_STATE(ch, 2*PULSE_VIOLENCE);
    damagesuccess = godice(get_attribute(ch,STRENGTH) + 1 + ch->pcdata->discipline[POTENCE], 6);
    if (damagesuccess < 0)
        damagesuccess = 0;

    damage(ch, victim, 3 * (damagesuccess * (ch->level + shield->level)) / 2, gsn_bash, DAM_BASH, TRUE);
    check_improve(ch,gsn_bash,TRUE,2);
    return;
}

void do_dirt( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int success, dice, diff, skill;
    int precog = 0;

    one_argument(argument,arg);

    if ( (skill = get_skill(ch,gsn_dirt)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT)))
    {
    send_to_char("You get your feet dirty.\n\r",ch);
    return;
    }

    if (arg[0] == '\0')
    {
    victim = ch->fighting;
    if (victim == NULL)
    {
        send_to_char("But you aren't fighting anyone!\n\r",ch);
        return;
    }
    }
    else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't kick dirt while riding!\n\r", ch);
        return;
    }

    if (IS_AFFECTED(victim,AFF_BLIND))
    {
    act("$E's already been blinded.",ch,NULL,victim,TO_CHAR);
    return;
    }

    if (victim == ch)
    {
    send_to_char("Very funny.\n\r",ch);
    return;
    }

    if (is_safe(ch,victim))
    return;

    if (IS_NPC(victim) &&
     victim->fighting != NULL &&
    !is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
    act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
    return;
    }
    dice = get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_BRAWL);
    diff = 6;

    /* modifiers */

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
    diff--;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
    diff += 2;

    /* level */
    if (victim->level > ch->level+10)
    diff++;

    /* skill */
    dice = (dice * skill) / 100;

    /* terrain */

    switch(ch->in_room->sector_type)
    {
    case(SECT_INSIDE):          diff += 1;   break;
    case(SECT_CITY):            diff += 1;   break;
    case(SECT_FIELD):           diff -= 1;   break;
    case(SECT_FOREST):          diff += 1;        break;
    case(SECT_HILLS):           diff += 1;        break;
    case(SECT_MOUNTAIN):        diff -= 1;   break;
    case(SECT_WATER_SWIM):      diff = -1;       break;
    case(SECT_WATER_NOSWIM):    diff = -1;       break;
    case(SECT_AIR):             diff = -1;  break;
    case(SECT_DESERT):          diff -= 2; break;
    case(SECT_WATER_DROWN):     diff = -1; break;
    case(SECT_HOT):                     break;
    case(SECT_COLD):                    break;
    }

    if (diff == -1)
    {
        sendch("There's no dirt here to kick!\n\r", ch);
        return;
    }

    if (diff > 10)
        diff = 10;

 /*   if(is_affected(victim,gsn_precognition) && number_percent() > 50)
    {
    	act("Your kicked dirt targeting is on point, but $N manages to dodge to the side!", ch, NULL, victim, TO_CHAR);
    	act("$n kicks some dirt directly at your face, but your foresight allowed you to dodge to the side with ease.", ch, NULL, victim, TO_VICT);
    	act("$N dodges quickly to the side, avoiding $n's kicked dirt.", ch, NULL, victim, TO_NOTVICT);
    	return;
    }
*/

    success = godice(dice, diff);
    if(is_affected(victim,gsn_precognition))
        precog = get_affect_level(victim,gsn_precognition);
    success -= precog;

    if (IS_DEBUGGING(ch)) {
        char buf[MSL];
        sprintf(buf, "Dice: %d(%d) Diff %d Success %d",
            dice, get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_BRAWL),
            diff, success);
        sendch(buf, ch);
    }

    /* now the attack */
    if (success > 0)
    {
        act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);
        damage(ch,victim,number_range(ch->level / 4, ch->level / 2),gsn_dirt,DAM_NONE,TRUE);
        check_improve(ch,gsn_dirt,TRUE,2);
        WAIT_STATE(ch,skill_table[gsn_dirt].beats);


        if (success < 3)
        {
            act("$n is momentarily stunned by the dirt kicked in $s eyes!",victim, NULL, NULL, TO_ROOM);
            STOPPED(victim, 2 * PULSE_VIOLENCE);

        } else {
            AFFECT_DATA af;
            act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);

            STOPPED(victim, PULSE_VIOLENCE);

            send_to_char("You can't see a thing!\n\r",victim);

            af.where    = TO_AFFECTS;
            af.type     = gsn_dirt;
            af.level    = ch->level;
            af.duration = 0;
            af.location = APPLY_HITROLL;
            af.modifier = -8 * success;
            af.bitvector    = AFF_BLIND;

            affect_to_char(victim,&af);
        }

		gain_exp( ch, success * 5);

    }
    else
    {
        act("$N avoids your kicked dirt!", ch, NULL, victim, TO_CHAR);
        act("$n kicks dirt towards you, but you deftly step aside.", ch, NULL, victim, TO_VICT);
        act("$n kicks dirt towards $N, who deftly steps out of the way.", ch, NULL, victim, TO_NOTVICT);
        damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
        check_improve(ch,gsn_dirt,FALSE,2);
        WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
}

void do_trip( CHAR_DATA *ch, char *argument )
{
	char arg[MAX_INPUT_LENGTH];
  char buf[MSL];
	CHAR_DATA *victim;
	int skill;
  int cost = 0;
	int tripdiff, tripsuccess, tripdamage, knockdown;
    int precog = 0;

	one_argument(argument,arg);

	if ( (skill = get_skill(ch,gsn_trip)) == 0
	||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP)))
	{
		send_to_char("You stumble for a moment before catching your balance.\n\r",ch);
		return;
	}

	if (arg[0] == '\0')
	{
		victim = ch->fighting;
		if (victim == NULL)
		{
			send_to_char("But you aren't fighting anyone!\n\r",ch);
			return;
		}
	}

	else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
	{
		send_to_char("You can't seem to find this person to trip.\n\r",ch);
		return;
	}

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't trip while riding!\n\r", ch);
        return;
    }

    if (is_safe(ch,victim))
	{
		send_to_char("You don't think that starting a fight here is a good idea.\n\r", ch);
		return;
	}

	if (IS_NPC(victim) && victim->fighting != NULL &&
	!is_same_group(ch,victim->fighting))
	{
		send_to_char("Kill stealing is not permitted.\n\r",ch);
		return;
	}

	if (IS_AFFECTED(victim,AFF_FLYING) || IS_AFFECTED(ch, AFF_FLYING))
	{
		act("Both $N and yourself must be standing on solid ground.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (victim->position < POS_FIGHTING)
	{
		act("$N has already been knocked down.",ch,NULL,victim,TO_CHAR);
		return;
	}

	if (victim == ch)
	{
		send_to_char("You fall flat on your face!\n\r",ch);
		WAIT_STATE(ch,2 * skill_table[gsn_trip].beats);
		act("$n trips over $s own feet!",ch,NULL,NULL,TO_ROOM);
		return;
	}

	if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
	{
		act("$N is your beloved master.",ch,NULL,victim,TO_CHAR);
		return;
	}

  if (!IS_NPC(ch))
    cost = (ch->level / 4) + (9 - get_attribute(ch, CSABIL_ATHLETICS));

	if (ch->move < cost + 1)
	{
		send_to_char("You are too tired to try to trip someone.\n\r",ch);
		return;
	}

	tripdiff = 7;

	if (ch->size < victim->size)
		tripdiff++;

	if (!can_see(victim,ch) || ch->stopped > 0 )
		tripdiff -= 2;

	if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE) || IS_SET(victim->off_flags, OFF_DODGE))
		tripdiff++;

	WAIT_STATE(ch, PULSE_VIOLENCE);

/*	if(is_affected(victim,gsn_precognition) && number_percent() > 50)
	{
		act("You sweep your legs quickly, but $N nimbly hops over the attack.", ch, NULL, victim, TO_CHAR);
		act("$n tries to sweep your legs out from under you, but you have a moment's warning before hopping over the attack.", ch, NULL, victim, TO_VICT);
		act("$N effortlessly hops over $n's attempted tripping maneuver.", ch, NULL, victim, TO_NOTVICT);
		return;
	}
*/
	if(is_affected(victim, gsn_gift_catfeet) && number_percent() > 45)
	{
		act("You sweep your legs quickly, but $N gracefully leaps to the side.", ch, NULL, victim, TO_CHAR);
		act("$n tries to sweep your legs out from under you, but you leap safely away with feline grace.", ch, NULL, victim, TO_VICT);
		act("$N effortlessly leaps away $n's attempted tripping maneuver, planting $S feet firmly.", ch, NULL, victim, TO_NOTVICT);
		return;
	}

	tripsuccess = godice(get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_BRAWL), tripdiff);
    if(is_affected(victim,gsn_precognition))
        precog = get_affect_level(victim,gsn_precognition);
    tripsuccess -= precog;

	if (tripsuccess < 0)
	{
		act("Even as dextrous as you are, you still manage to trip over your own two feet.", ch, NULL, victim, TO_CHAR);
		act("$n stumbles and faceplants unceremoniously.", ch, NULL, victim, TO_VICT);
		act("$n stumbles and faceplants unceremoniously.", ch, NULL, victim, TO_NOTVICT);
		ch->position = POS_RESTING;
		DAZE_STATE(ch, 6*PULSE_VIOLENCE);
		WAIT_STATE(ch, 8);
		return;
	}

	if (tripsuccess == 0)
	{
		act("You misjudge the distance between yourself and $N, missing your trip attempt.", ch, NULL, victim, TO_CHAR);
		act("$n tries to trip you, but misses from too far away.", ch, NULL, victim, TO_VICT);
		act("$n sweeps out with $s legs to try and trip $N, but misses from too far away.", ch, NULL, victim, TO_NOTVICT);
		WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
		check_improve(ch,gsn_trip,FALSE,3);
		return;
	}

	if (tripsuccess > 0)
	{
		act("You sweep your legs out in a powerful blow, connecting solidly with $N.",ch,NULL,victim,TO_CHAR);
		act("$n connects with a powerful blow, trying to topple you.",ch,NULL,victim,TO_VICT);
		act("$n connects with a solid blow, sweeping $s legs to collide with $N.",ch,NULL,victim,TO_NOTVICT);
		check_improve(ch,gsn_trip,TRUE,6);
    gain_exp(ch, 3*tripsuccess);

		tripdamage = godice(get_attribute(ch, STRENGTH) + get_discipline(ch, POTENCE), 6);
		damage(ch, victim, tripdamage*ch->level, gsn_trip, DAM_BASH, TRUE);

		if (IS_SET(victim->parts,PART_LEGS))
		{
			knockdown = godice(get_attribute(victim, DEXTERITY) + get_ability(victim, CSABIL_ATHLETICS), 8);
			if (knockdown < 1)
			{
				act("Your swift maneuver sweeps $N's legs out from under $M, and $E crashes to the ground.",ch,NULL,victim,TO_CHAR);
				act("$n's attack catches you by surprise, and you lose your balance.",ch,NULL,victim,TO_VICT);
				act("With a swift maneuver, $n sweeps $N's legs out from under $S.",ch,NULL,victim,TO_NOTVICT);
				WAIT_STATE(victim, PULSE_VIOLENCE * tripsuccess);
				WAIT_STATE(ch,skill_table[gsn_trip].beats);
				victim->position = POS_RESTING;
			}
		}
		else
		{
			act("Your opponent doesn't topple, having no legs to sweep.",ch,NULL,victim,TO_CHAR);
			act("You remain motionless against the blow.",ch,NULL,victim,TO_VICT);
			act("$n connects with a solid blow, but $N has no legs to be swept away.",ch,NULL,victim,TO_NOTVICT);
		}
		return;
	}
}



void do_kill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Kill whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }
  /* Allow player killing */
    if ( !IS_NPC(victim))
    {
            send_to_char( "You must MURDER a player.\n\r", ch );
            return;
    }

    if ( victim == ch )
    {
    send_to_char( "You hit yourself.  Ouch!\n\r", ch );
    multi_hit( ch, ch, TYPE_UNDEFINED );
    return;
    }

    if ( is_safe( ch, victim ) )
    return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
    act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if ( ch->position == POS_FIGHTING )
    {
    send_to_char( "You do the best you can!\n\r", ch );
    return;
    }

    if ( IS_AFFECTED2(ch, AFF2_MIST) && !IS_AFFECTED2(victim, AFF2_MIST))
    {
    send_to_char( "You cannot attack them in this form!\n\r", ch );
    return;
    }

    if(is_affected(ch,gsn_shadowform) && !is_affected(victim,gsn_shadowform))
    {
        send_to_char("You pass right through them.\n\r",ch);
        return;
    }
    if(!is_affected(ch,gsn_shadowform) && is_affected(victim,gsn_shadowform))
    {
        send_to_char("You pass right through them.\n\r",ch);
        return;
    }

    if ( !IS_AFFECTED2(ch, AFF2_MIST) && IS_AFFECTED2(victim, AFF2_MIST))
    {
    send_to_char( "You can barely see them, let alone attack them!\n\r", ch );
    return;
    }
    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}



void do_murde( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to MURDER, spell it out.\n\r", ch );
    return;
}


void do_murder( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Murder whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "You hit yourself.  Ouch!\n\r", ch );
    multi_hit( ch, ch, TYPE_UNDEFINED );
    return;
    }

    if ( is_safe( ch, victim ) )
    return;

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
    act( "$N is your beloved master.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if ( ch->position == POS_FIGHTING )
    {
    send_to_char( "You do the best you can!\n\r", ch );
    return;
    }

    if ( IS_AFFECTED2(ch, AFF2_MIST) && !IS_AFFECTED2(victim, AFF2_MIST))
    {
    send_to_char( "You cannot attack them in this form!\n\r", ch );
    return;
    }

    if(is_affected(ch,gsn_shadowform) && !is_affected(victim,gsn_shadowform))
    {
        send_to_char("You pass right through them.\n\r",ch);
        return;
    }
    if(!is_affected(ch,gsn_shadowform) && is_affected(victim,gsn_shadowform))
    {
        send_to_char("You pass right through them.\n\r",ch);
        return;
    }

    if ( IS_SET(victim->act,PLR_SPEC) && !IS_NPC(victim))
    {
    send_to_char( "You cannot attack that player!\n\r", ch );
    return;
    }

    if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
    {
        send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
        return;
    }

    if ( !IS_AFFECTED2(ch, AFF2_MIST) && IS_AFFECTED2(victim, AFF2_MIST))
    {
    send_to_char( "You can barely see them, let alone attack them!\n\r", ch );
    return;
    }
    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int check;
    int dice;
    int successes;
    int damsuccess;
    int damdice;
    int diff;
    int modifier;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Backstab whom?\n\r",ch);
        return;
    }

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't backstab while riding!\n\r", ch);
        return;
    }

    if (ch->fighting != NULL)
    {
    send_to_char("You're facing the wrong end.\n\r",ch);
    return;
    }

    else if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if (!IS_AFFECTED(ch,AFF_SNEAK) && !IS_AFFECTED(ch, AFF_HIDE) &&
        !IS_AFFECTED(ch, AFF_INVISIBLE) && !IS_AFFECTED2(ch, AFF2_UNSEEN) &&
        !IS_AFFECTED2(ch, AFF2_VEIL) && !IS_AFFECTED(victim, AFF_BLIND))
    {
        send_to_char("They will undoubtedly see you coming. Try being less visible.\n\r",ch);
        return;
    }

    if ( victim == ch )
    {
    send_to_char( "How can you sneak up on yourself?\n\r", ch );
    return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
     victim->fighting != NULL &&
    !is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
    send_to_char( "You need to wield a weapon to backstab.\n\r", ch );
    return;
    }
    if (obj->value[0] != WEAPON_DAGGER)
    {
        send_to_char( "With that weapon? YEAH RIGHT!\n\r", ch );
        return;
    }

    if (( victim->hit < victim->max_hit / 3) && victim->position != POS_SLEEPING)
    {
    act( "$N is hurt and suspicious ... you can't sneak up.",
        ch, NULL, victim, TO_CHAR );
    return;
    }
 /*Changing Backstab*/
    int skill = get_skill(ch, gsn_backstab);
    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if (!IS_AWAKE(victim))
        diff = 1;
    else
    {
        diff = UMIN( 10, (get_attribute(victim, PERCEPTION) + get_ability(victim, CSABIL_ALERTNESS)));

        if (skill >= 80)
            diff--;
        if (skill >= 90)
            diff--;
        if (diff < 7)
            diff = 7;
    }

    if (is_affected(ch,gsn_gift_scentofrunningwater))
        diff--;

    dice = get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_STEALTH);
    if (is_affected(ch,gsn_gift_silence))
        dice += 2;
    successes = godice(dice, diff);
    successes += stealth_int_shadowplay(ch,diff);
    damdice = d10_damdice(ch, victim) + successes;
    damsuccess = godice(damdice, 4);
    modifier = d10_modifier(ch);
    // Skill Percent increases modifier.
    modifier = (modifier * (50+skill))/100;

    if (IS_DEBUGGING(ch))
        cprintf(ch, "tohit:%d diff:%d success:%d dampool:%d damsuccess:%d\n\r", dice, diff, successes, damdice, damsuccess);

    if (successes < 3)
    {
        check_improve(ch,gsn_backstab,FALSE,2);
        act("$N seems to sense you coming and dodges your backstab!", ch, NULL, victim, TO_CHAR );
        d10_damage(ch, victim, 0, 0, gsn_backstab, DAM_PIERCE, DEFENSE_FULL, TRUE, TRUE);
        return;
    }

    check_improve(ch,gsn_backstab,TRUE,2);
    d10_damage(ch, victim, damsuccess, modifier, gsn_backstab, DAM_PIERCE, DEFENSE_SOAK, TRUE, TRUE);
    check = number_percent();
    if (SAME_ROOM(ch, victim) && (godice(dice, diff) > 1))
       {
        damsuccess = godice(damdice, 4);
        d10_damage(ch, victim, damsuccess, modifier, gsn_backstab, DAM_PIERCE, DEFENSE_SOAK, TRUE, TRUE);
       }

    if(SAME_ROOM(ch, victim) && (IS_AFFECTED(ch,AFF_HASTE) || is_affected(ch,gsn_timealteration) || is_affected(ch, gsn_rage) || is_affected(ch, gsn_celbuff)) && (godice(dice, diff) > 1))
    {
        damsuccess = godice(damdice, 4);
        d10_damage(ch, victim, damsuccess, modifier, gsn_backstab, DAM_PIERCE, DEFENSE_SOAK, TRUE, TRUE);
    }
    return;
}


void do_flee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    ROOM_INDEX_DATA *now_in;
    CHAR_DATA *victim;
    int attempt;

    if ( ( victim = ch->fighting ) == NULL && str_cmp( argument, "auto" ))
    {
        if ( ch->position == POS_FIGHTING )
            ch->position = POS_STANDING;
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
    }

    if ( is_affected( ch, gsn_vamp_frenzy ) || is_affected( ch, gsn_garou_frenzy ) || is_affected(ch, gsn_thaumaturgy_frenzy) )
    {
        send_to_char("Your frenzy prevents you from fleeing!\n\r",ch);
        return;
    }

    if (is_affected(ch,gsn_armsoftheabyss))
    {
        send_to_char("The shadowy tendrils holds you firmly, preventing you from fleeing!\n\r",ch);
        return;
    }

    if (is_affected(ch,gsn_gift_haltthecowardsflight))
    {
        send_to_char("You are too terrified to move swiftly enough to escape!\n\r",ch);
        return;
    }

    was_in = ch->in_room;
    for ( attempt = 0; attempt < 6; attempt++ )
    {
    EXIT_DATA *pexit;
    int door;

    door = number_door( );
    if ( ( pexit = was_in->exit[door] ) == 0
    ||   pexit->u1.to_room == NULL
    ||   IS_SET(pexit->exit_info, EX_CLOSED)
    ||   (get_skill(ch,gsn_retreat) < number_percent() &&  number_range(0,ch->daze) != 0)
    || ( IS_NPC(ch)
    &&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
        continue;

    move_char( ch, door, FALSE );
    if ( ( now_in = ch->in_room ) == was_in )
        continue;

    ch->in_room = was_in;
    act( "$n has fled!", ch, NULL, NULL, TO_ROOM );
    ch->in_room = now_in;

    if ( !IS_NPC(ch) )
        send_to_char( "You flee from combat!\n\r", ch );

    stop_fighting( ch, TRUE );
    return;
    }

    send_to_char( "PANIC! You couldn't escape!\n\r", ch );
    return;
}



void do_rescue( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *fch;
    int rescuesuccess = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Rescue whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "What about fleeing instead?\n\r", ch );
    return;
    }

    if ( !IS_NPC(ch) && IS_NPC(victim) )
    {
    send_to_char( "Doesn't need your help!\n\r", ch );
    return;
    }

    if ( ch->fighting == victim )
    {
    send_to_char( "Too late.\n\r", ch );
    return;
    }

    if ( ( fch = victim->fighting ) == NULL )
    {
    send_to_char( "That person is not fighting right now.\n\r", ch );
    return;
    }

    if(IS_AFFECTED2(ch,AFF2_MIST) || IS_AFFECTED2(victim,AFF2_MIST) || is_affected(ch,gsn_shadowform) || is_affected(victim,gsn_shadowform))
    {
        send_to_char("Your hand passes right through them...\n\r",ch);
        return;
    }

    if (ch->move < ch->level / 10)
    {
      send_to_char("You are too tired to attempt to rescue anyone.\n\r", ch);
      return;
    }

    ch->move -= ch->level / 10;
    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    rescuesuccess = godice(get_attribute(ch, WITS) + get_ability(ch, CSABIL_BRAWL), 7);

    if (rescuesuccess < 0)
    {
      act( "You stumble ungracefully while attempting to rescue $N!",  ch, NULL, victim, TO_CHAR    );
      act( "$n stumbles while trying to intervene between you and your attacker!", ch, NULL, victim, TO_VICT    );
      act( "$n stumbles awkwardly while trying to position $mself to rescue $N!",  ch, NULL, victim, TO_NOTVICT );
      WAIT_STATE( ch, skill_table[gsn_rescue].beats * 6);
      return;
    }

    if (rescuesuccess == 0)
    {
      act( "You fail to rescue $N from $S attacker!",  ch, NULL, victim, TO_CHAR    );
      act( "$n tries to intervene, but fails to rescue you from your attacker!", ch, NULL, victim, TO_VICT    );
      act( "$n tries to rescue $N from $S attacker, but fails!",  ch, NULL, victim, TO_NOTVICT );
      check_improve(ch,gsn_rescue,FALSE,1);
      return;
    }

    act( "You step in quickly to rescue $N from $S attacker!",  ch, NULL, victim, TO_CHAR    );
    act( "$n intervenes and rescues you from your attacker!", ch, NULL, victim, TO_VICT    );
    act( "$n moves quickly and rescues $N from $S attacker!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,3);
    gain_exp(ch, rescuesuccess * 2);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );

    if (rescuesuccess >= 4)
    {
      act( "Your swift intervention catches the enemy off-guard, allowing you a chance at a surprise attack!", ch, NULL, victim, TO_CHAR );
      multi_hit(ch, fch, TYPE_UNDEFINED);
    }
    return;
}

/*
 New Kick code by Sengir, based off CS
*/
void do_kick(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
//  char arg[MAX_INPUT_LENGTH];
    int dicesuccess = 0;
    int soaksuccess = 0;
    int mobdice = 2;    /*mob default kick pool*/
    int soakdice = 0;
    int damagesuccess = 0;
    int extradamage = 0;    /*Extra successes past 1st on attack added to damage*/
    int precog = 0;

    if (argument[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            if (!IS_NPC(ch))
                send_to_char("The air feels very threatened by your kick.  Now try for a real target.\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room(ch, NULL, argument)) == NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("Your target seems to be conspicuously absent.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("You can't kick yourself!  You aren't nearly limber enough.\n\r", ch);
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("Contrary to popular belief, a roundhouse kick to the shopkeeper's face will not magically lower the prices.\n\r", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        if (!IS_NPC(ch))
            send_to_char("You get the feeling that this might not be a good place to start a fight.\n\r", ch);
        return;
    }

    if (!IS_NPC(ch))
        ch->move -= 1;

 /*   if (is_affected(victim, gsn_precognition) && number_percent() > 50)
    {
        act("With uncanny judgement, $N deftly avoids your kick!", ch, NULL, victim, TO_CHAR);
        act("With a brief flash of insight, you swiftly react and dodge $n's incoming kick.", ch, NULL, victim, TO_VICT);
        act("Moving almost as if choreographed, $N twists to the side and dodges $n's kick.", ch, NULL, victim, TO_NOTVICT);
        return;
    }
*/
    if (IS_NPC(ch))
    {
        if (!IS_SET(ch->off_flags, OFF_KICK))
            return;

        WAIT_STATE(ch, 13);

        if (ch->race == race_lookup("garou"))
        {
            mobdice++;      /*Garou mobs gain +1 to attack*/
            if (number_range(1, 5) != 1)    /*80% chance shifted form, +1 */
                mobdice++;
        }

        if (IS_VAMP(ch))
        {
            mobdice++;      /*Vampire blooded get +1*/
            if (number_range(1, 3) != 1)    /*66% chance of Potence, +1*/               mobdice++;
        }

        if(is_affected(victim,gsn_precognition))
            precog = get_affect_level(victim,gsn_precognition);
        mobdice -= precog;

        dicesuccess = godice(mobdice, 6);

        if (dicesuccess < 0)
        {
            act("$n trips and falls as you sidestep a wild kick.", ch, NULL, victim, TO_VICT);
            act("$n trips and falls while attempting to kick $N.", ch, NULL, victim, TO_NOTVICT);
            WAIT_STATE(ch, PULSE_VIOLENCE);
            ch->position = POS_RESTING;
            return;
        }

        if (dicesuccess == 0)
        {
            act("$n's kick barely misses hitting you.", ch, NULL, victim, TO_VICT);
            act("$n attempts to kick at $N but misses.", ch, NULL, victim, TO_NOTVICT);
            return;
        }

        if (dicesuccess > 0)
        {
            act("You double over in pain as $n lashes out with a strong kick directly to your midsection.", ch, NULL, victim, TO_VICT);
            act("$n kicks $N directly in the stomach.", ch, NULL, victim, TO_NOTVICT);

            extradamage = dicesuccess -1;
            damagesuccess = godice(mobdice + extradamage, 6);
            if (damagesuccess < 0)
                damagesuccess = 0;
            damage(ch, victim, damagesuccess * ch->level / 2, gsn_kick, DAM_BASH, TRUE);
            return;
        }
    }

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 5);

    WAIT_STATE(ch, 12);

    if (dicesuccess < 0)
    {
        act("The momentum of a severely misjudged kick towards $N carries you around, \n\rcausing you to slip and fall.", ch, NULL, victim, TO_CHAR);
        act("$n misses you with a wild kick and slips to the ground.", ch, NULL, victim, TO_VICT);
        act("$n kicks out in $N's direction, but slips to the ground.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE);
        ch->position = POS_RESTING;
        return;
    }

    if (dicesuccess == 0)
    {
        act("With the tiniest overcompensation, your vicious kick misses $N by mere inches.", ch, NULL, victim, TO_CHAR);
        act("$n's kick comes within a few inches of connecting solidly with your body.", ch, NULL, victim, TO_VICT);
        act("$n's fierce kick nearly connects with $N.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    if (dicesuccess > 0)
    {
        act("Your kick connects directly with $N's midsection, leaving a nasty mark.", ch, NULL, victim, TO_CHAR);
        if (!IS_NPC(victim))
            act("$N's foot collides directly to your midsection.", ch, NULL, victim, TO_VICT);
        act("$n kicks out at $N, and connects with the blow.", ch, NULL, victim, TO_NOTVICT);
        if (dicesuccess > 4)
        {
            act("$N doubles over as you knock the wind out of $M!", ch, NULL, victim, TO_CHAR);
            act("$N doubles over as the wind is knocked out of $M.", ch, NULL, victim, TO_NOTVICT);
            if (!IS_NPC(victim))
                act("You feel the wind knocked out of you!", ch, NULL, victim, TO_VICT);
            WAIT_STATE(victim, PULSE_VIOLENCE);
        }

        gain_exp(ch, dicesuccess*2);
        check_improve(ch,gsn_kick,TRUE,2);
    }

    extradamage = dicesuccess - 1;
    damagesuccess = godice(get_attribute(ch, STRENGTH) + ch->pcdata->discipline[POTENCE] + extradamage, 6);

    if (IS_NPC(victim))
    {
        soakdice = 1;
        if (ch->race == race_lookup("garou") || IS_VAMP(victim))     /*bonus for vamp/garou, and 66% chance Fort or Crinos*/
        {
            if(number_range(1, 3) != 2)
                soakdice++;
            soakdice++;
        }

        if (victim->size > SIZE_LARGE)
            soakdice++;
        if (victim->size < SIZE_MEDIUM)
            soakdice--;
    }

    else
        soakdice = godice(get_attribute(ch, STAMINA) + ch->pcdata->discipline[FORTITUDE], 6);

    soaksuccess = godice(soakdice, 6);
    if (soaksuccess < 0)
        soaksuccess = 0;

    damagesuccess = damagesuccess - soaksuccess;
    if (damagesuccess < 0)
        damagesuccess = 0;

    damage(ch, victim, 2*(UMAX(15,damagesuccess * (ch->level * 2)))/3, gsn_kick, DAM_BASH, TRUE);

/*Sengir Adding in for second kick */
    if ((dicesuccess >= 4) && (get_skill(ch, gsn_second_kick) > 0))
    {
        act("With quick timing and incredible speed, you spin around and kick out at $N,", ch, NULL, victim, TO_CHAR);
        act("$n spins around, bringing $s heel directly at you,", ch, NULL, victim, TO_VICT);
        act("$n twists around, attempting another kick at $N,", ch, NULL, victim, TO_NOTVICT);

        dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 6);

        if (dicesuccess < 0)
        {
            act("but manage to turn too quickly, setting yourself off balance as you tumble to the ground.", ch, NULL, victim, TO_CHAR);
            act("and stumbles to the ground as the momentum carries $s through the turn.", ch, NULL, victim, TO_VICT);
            act("but falls to the ground, carried by the wild kick.", ch, NULL, victim, TO_NOTVICT);

            WAIT_STATE(ch, PULSE_VIOLENCE);
            ch->position = POS_RESTING;
            return;
        }

        if (dicesuccess == 0)
        {
            act("but miss with your follow through kick.", ch, NULL, victim, TO_CHAR);
            act("but misses by only a few inches on the follow through.", ch, NULL, victim, TO_VICT);
            act("but misses with the second kick.", ch, NULL, victim, TO_NOTVICT);
            return;
        }

        if (dicesuccess > 0)
        {
            act("colliding solidly once again with a devastating kick.", ch, NULL, victim, TO_CHAR);
            act("and collides directly with your body with the second kick.", ch, NULL, victim, TO_VICT);
            act("which connects as surely and forcefully as the first.", ch, NULL, victim, TO_NOTVICT);

            gain_exp(ch, dicesuccess);

            extradamage = dicesuccess - 1;
            damagesuccess = godice(get_attribute(ch, STRENGTH) + ch->pcdata->discipline[POTENCE] + extradamage, 6);

            soaksuccess = godice(soakdice, 6);
            if (soaksuccess < 0)
                soaksuccess = 0;

            damagesuccess = damagesuccess - soaksuccess;
            if (damagesuccess < 0)
                damagesuccess = 0;

            damage(ch, victim, 2*(UMAX(15,damagesuccess * ch->level))/3, gsn_kick, DAM_BASH, TRUE);

        }

    }

    return;
}


void do_disarm( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *wielded;
    int precog = 0;

    if (IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char( "Disarming is no longer allowed in the Arena.\n\r", ch );
        return;
    }

    if ((get_skill(ch,gsn_disarm)) == 0)
    {
    send_to_char( "You don't know the proper disarming techniques.\n\r", ch );
    return;
    }

    if ( wielded = get_eq_char( ch, WEAR_WIELD ) == NULL
    &&   ((get_skill(ch,gsn_hand_to_hand)) == 0
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
    send_to_char( "You must be wielding a weapon to disarm your opponent.\n\r", ch );
    return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
    {
    send_to_char( "Your opponent is not wielding a weapon.\n\r", ch );
    return;
    }

    if (!IS_NPC(ch) && ch->move <= ch->level / 4)
    {
      send_to_char("You are too tired to pull off that maneuver.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch))
      ch->move -= ch->level / 4;

	   int success, dice, diff;
	   success = dice = diff = 0;
	   // Disarm is rolled as a regular attack, but if the successes are greater than
	   // victim/s strength, they take no damage and the weapon goes flying.
	   dice = get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_MELEE);

    if(is_affected(victim,gsn_precognition))
        precog = get_affect_level(victim,gsn_precognition);
    dice -= precog;

     if (wielded == NULL && !IS_NPC(ch))
      success = godice(dice,8);
     else
	    success = godice(dice, 7);

     if (success < 0)
     {
       act("You strike at a perceived opening, only to find yourself disarmed by $N's quick maneuver!", ch, NULL, victim, TO_CHAR);
       act("With quick precision, you knock $n's weapon away, foiling $s attack!", ch, NULL, victim, TO_VICT);
       act("$n strikes out at $N's weapon, but has $s own weapon knocked away instead!", ch, NULL, victim, TO_NOTVICT);
		   disarm( victim, ch );
       return;
     }

    if (success == 0)
    {
      act("You try to strike at $N's weapon, but $E deftly moves away.", ch, NULL, victim, TO_CHAR);
      act("$n tries to strike at your weapon, but you react quickly and move to the side.", ch, NULL, victim, TO_VICT);
      act("With a quick movement, $n strikes at $N's weapon, but misses completely.", ch, NULL, victim, TO_NOTVICT);
  		WAIT_STATE(ch,skill_table[gsn_disarm].beats);
			check_improve(ch,gsn_disarm,FALSE,2);
      return;
    }

    if(is_affected(victim,gsn_precognition) && number_percent() > 50)
		{
    		act("You attempt to disarm $N, but $E twists to the side, holding $S weapon tightly!", ch, NULL, victim, TO_CHAR);
    		act("You sense $n striking in to try and disarm you, but you twist to the side and hold your weapon tightly.", ch, NULL, victim, TO_VICT);
    		act("$n strikes forward, but fails to disarm $N.", ch, NULL, victim, TO_NOTVICT);
    		return;
		}

		diff = get_attribute(victim, STRENGTH);

		if (diff > 6)
			diff = 6;
		if (diff < 1)
			diff = 1;

		if (success > diff)
		{
      if (IS_SET(victim->imm_flags, IMM_DISARM) || IS_SET(victim->act2, ACT2_ULTRA_MOB))
        {
          WAIT_STATE(ch,skill_table[gsn_disarm].beats);
          act("Your attack lands true, but $N manages to hold $S weapon tightly.",ch,NULL,victim,TO_CHAR);
          act("$n tries to disarm you, but you hold tightly to your weapon.",ch,NULL,victim,TO_VICT);
          act("$n tries to disarm $N, but seems to have failed.",ch,NULL,victim,TO_NOTVICT);
          return;
        }
			WAIT_STATE( ch, skill_table[gsn_disarm].beats );
      act("Sensing an opening, you strike out at $N's weapon!", ch, NULL, victim, TO_CHAR);
      act("$n strikes swiftly, aiming for your weapon!", ch, NULL, victim, TO_VICT);
      act("$n strikes out towards $N's weapon!", ch, NULL, victim, TO_NOTVICT);
			disarm( ch, victim );
			check_improve(ch,gsn_disarm,TRUE,6);
		}
    else
		{
			act("You miss your opening to strike at $N, but still manage to try for a connecting attack.", ch, NULL, victim, TO_CHAR);
			WAIT_STATE(ch,skill_table[gsn_disarm].beats);
			one_hit( ch, victim, TYPE_UNDEFINED );
			check_improve(ch,gsn_disarm,FALSE,2);
			return;
		}
    gain_exp(ch, success * 2);
}

void do_bite(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
//  char arg[MAX_INPUT_LENGTH];
    int dicesuccess = 0;
    int damagesuccess = 0;
    int poisondice = 0;
    int critical = 1;
    int precog = 0;

    if (IS_NPC(ch))
      return;

    if (argument[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            if (!IS_NPC(ch))
                send_to_char("Whom are you trying to bite?\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room(ch, NULL, argument)) == NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("Your target seems to be conspicuously absent.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("You bite yourself on the arm.\n\r", ch);
        return;
    }

    if (!IS_AFFECTED(ch, AFF_FANGS) && (ch->pcdata->shiftform < CRINOS) &&
      get_affect_level(ch, gsn_mutateform) != MUTATE_PANTHER &&
      get_affect_level(ch, gsn_mutateform) != MUTATE_BEAR &&
      get_affect_level(ch, gsn_shift) != SHIFT_WOLF &&
      get_affect_level(ch, gsn_shift) != SHIFT_BEAR &&
      !is_affected(ch, gsn_formofthecobra))
    {
      send_to_char("You don't have sharp enough teeth to bite effectively.\n\r", ch);
      return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("The shopkeeper would prefer to keep all limbs intact.\n\r", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        if (!IS_NPC(ch))
            send_to_char("You get the feeling that this might not be a good place to start a fight.\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && ch->move < ch->level / 4)
    {
      send_to_char("You are too tired to effectively bite someone.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch))
        ch->move -= ch->level / 4;

 /*   if (is_affected(victim, gsn_precognition) && number_percent() > 50)
    {
        act("Almost as if $E sees it coming, $N avoids your bite!", ch, NULL, victim, TO_CHAR);
        act("With a brief flash of insight, you swiftly react and dodge $n's ferocious bite.", ch, NULL, victim, TO_VICT);
        act("Without missing a beat, $N moves aside and dodges $n's bite.", ch, NULL, victim, TO_NOTVICT);
        return;
    }
*/

    if(is_affected(victim,gsn_precognition))
        precog = get_affect_level(victim,gsn_precognition);

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 5);
    dicesuccess -= precog;

    WAIT_STATE(ch, 12);

    if (dicesuccess < 0)
    {
        act("You lunge towards $N, but your bite comes up empty.", ch, NULL, victim, TO_CHAR);
        act("$n lunges towards you, baring $s teeth.", ch, NULL, victim, TO_VICT);
        act("You watch as $n tries to bite at $N.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE);
        return;
    }

    if (dicesuccess == 0)
    {
        act("You bite savagely at $N, but miss your target.", ch, NULL, victim, TO_CHAR);
        act("$n's teeth snap closed near your body.", ch, NULL, victim, TO_VICT);
        act("$n's bite misses $N.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    if (dicesuccess > 0)
    {
        act("Baring your teeth, you gnash violently upon $N with a powerful bite.", ch, NULL, victim, TO_CHAR);
        if (!IS_NPC(victim))
            act("$N bites down upon you, ripping into your body!", ch, NULL, victim, TO_VICT);
        act("$n lunges at $N, biting $M violently.", ch, NULL, victim, TO_NOTVICT);
        if (dicesuccess > 4)
        {
            act("With precision targeting, you bite down on a sensitive area!", ch, NULL, victim, TO_CHAR);
            if (!IS_NPC(victim))
                act("You flinch in pain as $n bites down hard!", ch, NULL, victim, TO_VICT);
            WAIT_STATE(victim, PULSE_VIOLENCE);
            critical = 1.5;
        }

        gain_exp(ch, dicesuccess*2);
    }

    damagesuccess = godice(get_attribute(ch, STRENGTH) + 1 + ch->pcdata->discipline[POTENCE], 6);

    if (damagesuccess < 0)
        damagesuccess = 0;

    d10_damage(ch, victim, damagesuccess, ch->level * 2 / 3 * critical, gsn_bite, DAM_PIERCE, DEFENSE_FULL, TRUE, TRUE);

    if (is_affected(ch, gsn_formofthecobra) && dicesuccess > 3)
    {
        poisondice = godice(7, 6);
        d10_damage(ch, victim, poisondice, ch->level/2, gsn_poison, DAM_POISON, DEFENSE_NONE, TRUE, TRUE);
    }
    return;
}

void do_shred(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
//  char arg[MAX_INPUT_LENGTH];
    int dicesuccess = 0;
    int damagesuccess = 0;

    if (IS_NPC(ch))
      return;

    if (argument[0] == '\0')
    {
        victim = ch->fighting;
        if (victim == NULL)
        {
            if (!IS_NPC(ch))
                send_to_char("Whom are you trying to shred with your claws?\n\r", ch);
            return;
        }
    }

    else if ((victim = get_char_room(ch, NULL, argument)) == NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("Your target seems to be conspicuously absent.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("Shredding yourself would do no good.\n\r", ch);
        return;
    }

    if (!is_affected(ch, gsn_claws) && !is_affected(ch, gsn_wingclaws))
    {
      send_to_char("You need to have sharp claws to shred an enemy.\n\r", ch);
      return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        if (!IS_NPC(ch))
            send_to_char("Shredding the shopkeeper would be bad for future endeavors.\n\r", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        if (!IS_NPC(ch))
            send_to_char("You get the feeling that this might not be a good place to start a fight.\n\r", ch);
        return;
    }

    if (!IS_NPC(ch) && ch->move < ch->level / 5)
    {
      send_to_char("You are too tired to shred your opponent.\n\r", ch);
      return;
    }

    if (!IS_NPC(ch))
        ch->move -= ch->level / 5;

    if (is_affected(victim, gsn_precognition) && number_percent() > 50)
    {
        act("$N moves out of the way of your shred attack, seconds before it connects!", ch, NULL, victim, TO_CHAR);
        act("With a brief flash of insight, you swiftly react and dodge $n's shredding claws.", ch, NULL, victim, TO_VICT);
        act("$N dodges away from $n's shred attack.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 6);

    WAIT_STATE(ch, 12);

    if (dicesuccess < 0)
    {
        act("You try to shred $N with your claws, but miss horribly!", ch, NULL, victim, TO_CHAR);
        act("$n tries to attack with $s claws, but misses completely.", ch, NULL, victim, TO_VICT);
        act("$n makes a lunge at $N, attempting to shred $M to bits, but misses by a mile.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, PULSE_VIOLENCE);
        return;
    }

    if (dicesuccess == 0)
    {
        act("You attack with your claws at $N, but miss completely.", ch, NULL, victim, TO_CHAR);
        act("$n tries to lunge at you with $s claws, but misses.", ch, NULL, victim, TO_VICT);
        act("$n's shredding misses $N.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    if (dicesuccess > 0)
    {
        act("With a ferocious attack, you shred into $N with your claws!", ch, NULL, victim, TO_CHAR);
        if (!IS_NPC(victim))
            act("$N's claws shred into your body!", ch, NULL, victim, TO_VICT);
        act("$n claws wildly at $N, shredding $M viciously.", ch, NULL, victim, TO_NOTVICT);
        gain_exp(ch, dicesuccess*2);
    }

    damagesuccess = godice(get_attribute(ch, STRENGTH) + 1 + ch->pcdata->discipline[POTENCE], 6);

    if (damagesuccess < 0)
        damagesuccess = 0;

    d10_damage(ch, victim, damagesuccess, ch->level * 2 / 3, gsn_shred, DAM_SLASH, DEFENSE_FULL, TRUE, TRUE);
    if (ch->fighting && dicesuccess > 4)
    {
        act("With quick thinking, you turn and shred a second time at $N!", ch, NULL, victim, TO_CHAR);
        if (!IS_NPC(victim))
            act("$n turns and slashes a second time at you!", ch, NULL, victim, TO_VICT);
        d10_damage(ch, victim, damagesuccess - 2, ch->level * 2 / 3, gsn_shred, DAM_SLASH, DEFENSE_FULL, TRUE, TRUE);

        WAIT_STATE(victim, PULSE_VIOLENCE);
    }
    return;
}

void do_surrender( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    if (IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char("You cannot surrender in the Arena!\n\r",ch);
        return;
    }
    if ( (mob = ch->fighting) == NULL )
    {
    send_to_char( "But you're not fighting!\n\r", ch );
    return;
    }
    act( "You surrender to $N!", ch, NULL, mob, TO_CHAR );
    act( "$n surrenders to you!", ch, NULL, mob, TO_VICT );
    act( "$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT );
    stop_fighting( ch, TRUE );

    if ( !IS_NPC( ch ) && IS_NPC( mob )
    &&   ( !HAS_TRIGGER_MOB( mob, TRIG_SURR )
        || !p_percent_trigger( mob, NULL, NULL, ch, NULL, NULL, TRIG_SURR ) ) )
    {
    act( "$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR );
    multi_hit( mob, ch, TYPE_UNDEFINED );
    }
}


void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}


void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Slay whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( ch == victim )
    {
    send_to_char( "Suicide is a mortal sin.\n\r", ch );
    return;
    }

    if ( !IS_NPC(victim) && victim->level >= get_trust(ch) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    act( "You slay $M in cold blood!",  ch, NULL, victim, TO_CHAR    );
    act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
    act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim );
    return;
}

void kill_em(CHAR_DATA *ch,CHAR_DATA *victim)
{
    char mobname[MAX_STRING_LENGTH];
    DOMAIN_DATA *dom;

    if (!ch || !victim)
        return;

    /* new arena code */
        if (!IS_NPC(victim) && IS_SET(victim->act,PLR_ARENA))
        {
                stop_fighting( victim, TRUE );
                death_cry( victim );
                if(victim != ch) announce(victim, ch, WIZ_MOBDEATHS);
                REMOVE_BIT(victim->act,PLR_ARENA);
                victim->position = POS_STANDING;
                act( "{BYou are victorious!{x\n\r{YThe room fills with the scent of wild flowers as an unseen presence\n\r takes the soul of $N to a better place.{x",  ch, NULL, victim, TO_CHAR    );
                act( "{RSorry, you are dead!{x\n\r\n\r{YYou are magically saved from death by the mercy of Ma'at.{x", ch, NULL, victim, TO_VICT    );
                act( "{YThe room fills with the scent of wild flowers as an unseen presence\n\r takes the soul of $N to a better place.{x",  ch, NULL, victim, TO_NOTVICT );
                if (victim->in_room != NULL)
                        char_from_room( victim );
                char_to_room( victim, get_room_index( ROOM_VNUM_ALTAR ) );

                if(is_affected( victim, gsn_vicissitude_horrid ))
                {
                    if (!IS_AFFECTED(victim, AFF_FANGS))
                    SET_BIT(victim->affected_by, AFF_FANGS);
                    do_function(victim, &do_shift, "");
                    do_function(victim, &do_fangs, "");
                }
                if(is_affected( victim, gsn_vicissitude_chiropteran ))
                {
                  if (!IS_AFFECTED(victim, AFF_FANGS))
                    SET_BIT(victim->affected_by, AFF_FANGS);
                  do_function(victim, &do_chiropteranmarauder, "");
                  do_function(victim, &do_fangs, "");
                  if (is_affected(victim, gsn_wingclaws))
                    do_function(victim, &do_wingclaws, "");
                }
                if ( IS_AFFECTED(victim, AFF_SHIFT))
                {       if (!IS_AFFECTED(victim, AFF_FANGS))
                        SET_BIT(victim->affected_by, AFF_FANGS);
                        do_function(ch, &do_shift, "");
                        do_function(ch, &do_fangs, "");
                }
                if ( is_affected( victim, gsn_claws ) )
                        do_function(victim, &do_claws, "");

/* new garou stuff */
                if(!IS_NPC(victim) && victim->pcdata->shiftform > 1)
                {
                    affect_strip(victim,gsn_change);
                    victim->pcdata->shiftform = 1;
                }

/* garou stuff */
                nuke_pets(ch);
                while ( victim->affected )
                        affect_remove( victim, victim->affected );
                victim->affected_by     = race_table[victim->race].aff;

                victim->position        = POS_RESTING;
                victim->hit             = victim->max_hit;
                victim->mana            = victim->max_mana;
                victim->move            = victim->max_move;
                return;
        }
        /*End Arena Death Code*/

    one_argument(victim->name, mobname);
        if(str_cmp(mobname, "corpse"))
        group_gain( ch, victim );

        if(IS_NPC(ch)) grudge_update(ch,victim);
        if(IS_NPC(victim)) grudge_update(victim,ch);

    if ( !IS_NPC(victim) )
    {
        sprintf( log_buf, "%s killed by %s at %d",
        victim->name,
        (IS_NPC(ch) ? ch->short_descr : ch->name),
        ch->in_room->vnum );
        log_string( log_buf );

    if(is_affected( victim, gsn_vicissitude_horrid ))
    {
        if (!IS_AFFECTED(victim, AFF_FANGS))
            SET_BIT(victim->affected_by, AFF_FANGS);
        do_function(victim, &do_horrid, "");
        do_function(victim, &do_fangs, "");
    }

    if(is_affected( victim, gsn_vicissitude_chiropteran ))
    {
      if (!IS_AFFECTED(victim, AFF_FANGS))
        SET_BIT(victim->affected_by, AFF_FANGS);
      do_function(victim, &do_chiropteranmarauder, "");
      do_function(victim, &do_fangs, "");
      if (is_affected(victim, gsn_wingclaws))
        do_function(victim, &do_wingclaws, "");
    }

    if ( IS_AFFECTED(victim, AFF_SHIFT))
    {   if (!IS_AFFECTED(victim, AFF_FANGS))
        SET_BIT(victim->affected_by, AFF_FANGS);
        do_function(victim, &do_shift, "");
        do_function(victim, &do_fangs, "");
    }

/* garou stuff */
    if(!IS_NPC(victim) && victim->race == race_lookup("ghoul"))
        victim->pblood = victim->max_pblood;

                victim->exp   = exp_per_level(victim,victim->pcdata->points) * UMAX( 1, victim->level );

      }

        sprintf( log_buf, "%s got toasted by %s at %s [room %d]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);

        if (IS_NPC(victim))
            wiznet(log_buf,NULL,NULL,WIZ_MOBDEATHS,0,0);
        else
          {
            wiznet(log_buf,NULL,NULL,WIZ_DEATHS,0,0);
            if(victim != ch) announce(victim, ch, WIZ_DEATHS);
      }
    if ( IS_NPC( victim ) && HAS_TRIGGER_MOB( victim, TRIG_DEATH) )
      {
        victim->position = POS_STANDING;
        p_percent_trigger( victim, NULL, NULL, ch, NULL, NULL, TRIG_DEATH );
    }
    if(!IS_NPC(victim) && victim->pcdata->shiftform > 1)
    {
        affect_strip(victim,gsn_change);
        victim->pcdata->shiftform = 1;
    }
    dom = ch->in_room->area->domain;
   if (!IS_NPC(ch) && ch->pcdata->influenced == 0 && dom != NULL &&
       (IS_NPC(victim) &&
       IS_SET(victim->act2, ACT2_INFLUENCE)) && IS_SET(ch->act2, PLR2_PVP) &&
       number_percent() < INFLUENCE_KILL_CHANCE &&
       victim->in_room->area->domain->change[ch->clan] < 6 )
    {
        victim->in_room->area->domain->change[ch->clan]++;
        ch->pcdata->influenced = INFLUENCE_TIMER;
        sendch("Your deeds have made you notorious in this domain.\n\r", ch);
        save_area(victim->in_room->area);
    } else if (!IS_NPC(ch) && !IS_NPC(victim) &&
        dom != NULL &&  IS_SET(ch->act2, PLR2_PVP) &&
        IS_SET(victim->act2, PLR2_PVP))
    {
        victim->in_room->area->domain->change[ch->clan] += 2;
        victim->in_room->area->domain->change[victim->clan] -= 4;
        ch->pcdata->influenced = INFLUENCE_TIMER;
        sendch("Your deeds have made you notorious in this domain.\n\r", ch);
        victim->pcdata->influenced = INFLUENCE_TIMER;
        sendch("Your humiliating failure causes you to lose sway over this domain!\n\r", victim);
        save_area(victim->in_room->area);
    }

    raw_kill( victim );

    if (!IS_NPC(ch))
        kill_triggers(ch);
    else if (SAME_ROOM(ch, ch->master))
        kill_triggers(ch->master);

    return;
}

void kill_triggers (CHAR_DATA *ch)
{
    OBJ_DATA *corpse;

    if ((corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL
    &&  corpse->item_type == ITEM_CORPSE_NPC && can_see_obj(ch,corpse))
    {
        OBJ_DATA *coins;

        corpse = get_obj_list( ch, "corpse", ch->in_room->contents );

        if ( IS_SET(ch->act, PLR_AUTOLOOT) &&
         corpse && corpse->contains) /* exists and not empty */
            {
        do_function(ch, &do_get, "all corpse");
        }

        if (IS_SET(ch->act,PLR_AUTOGOLD) &&
            corpse && corpse->contains  && /* exists and not empty */
        !IS_SET(ch->act,PLR_AUTOLOOT))
        {
        if ((coins = get_obj_list(ch,"gcash",corpse->contains))
             != NULL)
        {
            do_function(ch, &do_get, "all.gcash corpse");
            }
        }

        if (IS_SET(ch->act, PLR_AUTOSAC))
        {
            if (IS_SET(ch->act,PLR_AUTOLOOT) && corpse && corpse->contains)
            {
                return;
            }
            else
            {
                do_function(ch, &do_sacrifice, "corpse");
            }
        }
    }
}
void do_assassinate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int skill, dice, diff, success;
    int damdice, damsuccess, modifier;
    int resistdice, resistdiff, resistsuccess;

    one_argument( argument, arg );

    if ((skill = get_skill(ch,gsn_assassinate)) == 0)
    {
    send_to_char( "Huh?\n\r", ch );
    return;
    }

    if (arg[0] == '\0')
    {
        send_to_char("Assassinate whom?\n\r",ch);
        return;
    }

    if ((victim = get_char_room(ch,NULL,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if (ch->fighting != NULL)
    {
    send_to_char("You're facing the wrong end.\n\r",ch);
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "How can you sneak up on yourself?\n\r", ch );
    return;
    }

    if ( is_safe( ch, victim ) )
      return;

    if (IS_NPC(victim) &&
     victim->fighting != NULL &&
    !is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_WIELD ) ) == NULL)
    {
    send_to_char( "You need to wield a weapon to Assassinate.\n\r", ch );
    return;
    }
    if (obj->value[0] != WEAPON_DAGGER)
    {
        send_to_char( "With that weapon? YEAH RIGHT!\n\r", ch );
        return;
    }

    if (!IS_AFFECTED(ch,AFF_SNEAK) && !IS_AFFECTED(ch, AFF_HIDE) &&
        !IS_AFFECTED(ch, AFF_INVISIBLE) && !IS_AFFECTED2(ch, AFF2_UNSEEN) &&
        !IS_AFFECTED2(ch, AFF2_VEIL) && !IS_AFFECTED(victim, AFF_BLIND))
    {
        send_to_char("They will undoubtedly see you coming. Try being less visible.\n\r",ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_assassinate].beats );

    if ( victim->hit < 2 * victim->max_hit / 3 && victim->position != POS_SLEEPING)
    {
    act( "$N is hurt and suspicious ... you can't sneak up.",
        ch, NULL, victim, TO_CHAR );
    return;
    }
    // Roll to sneak up behind victim, resisted
    dice = get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_STEALTH);
    if (is_affected(ch,gsn_gift_silence))
        dice += 2;
    resistdice = get_attribute(victim, PERCEPTION) + get_ability(victim, CSABIL_ALERTNESS);
    diff = resistdiff = 6;

    if (!can_see(victim, ch))
    {
      diff -= 1;
      resistdiff += 1;
    }

    if (is_affected(ch,gsn_gift_scentofrunningwater))
        diff--;

    if (!IS_AWAKE(victim))
      resistdice = 0;

    success = godice(dice, diff);
    success += stealth_int_shadowplay(ch, diff);
    resistsuccess = godice(resistdice, resistdiff);

    if (IS_DEBUGGING(ch))
      cprintf(ch, "tohit:%d diff:%d resist:%d resdiff:%d succ:%d ressucc:%d\n\r", dice, diff,
    resistdice, resistdiff, success, resistsuccess);

    // Failure, or detected
    if (success < 1 || success < resistsuccess)
    {
      act( "You fail to sneak up on $N!", ch, NULL, victim, TO_CHAR);
      check_improve(ch,gsn_assassinate,FALSE,4);
      multi_hit(ch, victim, TYPE_UNDEFINED);
      return;
    }

    // Successful sneak, Now, do we hit the spine.
    dice = get_attribute(ch, INTELLIGENCE) + get_ability(ch, CSABIL_MEDICINE);
    dice = dice * skill / 100;
    diff = 12 - get_ability(ch, CSABIL_MEDICINE);
    success = godice(dice, diff);

    if (IS_DEBUGGING(ch))
      cprintf(ch, "attdice:%d attdiff:%d succ:%d\n\r", dice, diff, success);
    if (success > 0)
    {
      check_improve(ch, gsn_assassinate, TRUE, 4);
      damdice = d10_damdice(ch, victim);
      modifier = d10_modifier(ch) * ((skill + ch->level)/2)/10;
      diff = 6;
      damsuccess = godice(damdice, diff);

      if (IS_DEBUGGING(ch))
        cprintf(ch, "damdice:%d mod:%d damsucc:%d\n\r", damdice, modifier, damsuccess);

        if (d10_damage(ch, victim, damsuccess, modifier, gsn_assassinate, DAM_PIERCE, DEFENSE_SOAK, TRUE, TRUE))
      {
        act( "$N screams as you place your dagger in their spine!", ch, NULL, victim, TO_CHAR );
        act( "$n plants his dagger into your spine, your body spasms in pain.", ch, NULL, victim, TO_VICT );
        act( "$n stabs $N in the spine!", ch, NULL, victim, TO_NOTVICT );
      } else {
        check_improve(ch,gsn_assassinate,FALSE,4);
        do_function(ch, &do_backstab, arg);
     }
     return;
    } else {
      check_improve(ch,gsn_assassinate,FALSE,4);
      do_function(ch, &do_backstab, arg);
    }
}
void do_divine_strength(CHAR_DATA *ch, char *argument )
{
  AFFECT_DATA af;
	int divinesuccess;
	int divineskill = get_skill(ch, gsn_divine_strength);
	int focus = ch->level / 2;

	if (divineskill == 0)
	{
		send_to_char( "You have not devoted yourself enough to garner favor with a deity.\n\r", ch );
		return;
	}
	if ( is_affected( ch, gsn_divine_strength ))
	{
		send_to_char( "You have already received a blessing from your deity!\n\r", ch );
		return;
	}
  if (ch->position == POS_FIGHTING)
  {
    send_to_char("You cannot concentrate enough in combat to recite a proper prayer.\n\r", ch);
    return;
  }
	if (ch->cswillpower < 1)
	{
		send_to_char("You do not have the willpower required to focus a serious prayer.\n\r", ch);
		return;
	}
  if (is_affected(ch, gsn_laryngitis))
  {
    send_to_char("Your throat is still too soer to recite a proper prayer to your deity.\n\r", ch);
    return;
  }
	if (ch->move < focus)
	{
		send_to_char("You are too tired to offer up a proper prayer to your deity.\n\r", ch);
		return;
	}
	if (ch->mana < focus)
	{
		send_to_char("You do not have the inner reserve of energy to profess your faith.\n\r", ch);
		return;
	}

	ch->cswillpower--;
	ch->move -= focus;
	ch->mana -= focus;

	divinesuccess = godice((get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_THEOLOGY) * divineskill / 100), 6);
	act("Focusing on your devotion, you offer up a quick prayer to your deity above.", ch, NULL, NULL, TO_CHAR);
	act("$n offers up a quick prayer to the heavens above.", ch, NULL, NULL, TO_NOTVICT);
	WAIT_STATE( ch, skill_table[gsn_divine_strength].beats );

	if (divinesuccess < 0)
	{
		act("You feel a sense of dread and unease as you stumble through your prayer.", ch, NULL, NULL, TO_CHAR);
		act("$e looks around uneasily as $e stumbles through $s prayer.", ch, NULL, NULL, TO_NOTVICT);
		check_improve(ch,gsn_divine_strength,FALSE,4);

    af.where    = TO_AFFECTS;
    af.type     = gsn_curse;
    af.level    = ch->level + 15;
    af.duration = 5;
    af.modifier = ch->level/2;
    af.location = APPLY_HITROLL;
    af.bitvector    = 0;
    affect_to_char( ch, &af );
		return;
	}

	if (divinesuccess == 0)
	{
		act("After a moment of reflection, your prayer seems to have been unanswered.", ch, NULL, NULL, TO_CHAR);
		WAIT_STATE( ch, 5 );
		check_improve(ch,gsn_divine_strength,FALSE,4);
		return;
	}

	if (divinesuccess > 0)
	{
		act( "Smiling broadly, $n seems to be renewed with extra strength of purpose.",  ch, NULL, NULL, TO_ROOM );
		act( "A feeling of purpose and strength flows through your body as your deity grants your request for help.", ch, NULL, NULL, TO_CHAR);
		check_improve(ch,gsn_divine_strength,TRUE,4);
    gain_exp(ch, divinesuccess * (15 + get_ability(ch, CSABIL_THEOLOGY)));

    af.where    = TO_AFFECTS;
		af.type     = gsn_divine_strength;
		af.level    = ch->level;
		af.duration = (divinesuccess * 5) + 50;
		af.modifier = 1;
		af.location = APPLY_CS_STR;
		af.bitvector    = 0;
		affect_to_char( ch, &af );

		af.where    = TO_AFFECTS;
		af.type     = gsn_divine_strength;
		af.level    = ch->level;
		af.duration = (divinesuccess * 5) + 50;
		af.modifier = ch->level/2 + (divinesuccess * 5);
		af.location = APPLY_DAMROLL;
		af.bitvector    = 0;
		affect_to_char( ch, &af );

		if (divinesuccess > 4)
		{
			act("You feel the blessing of being your deity's chosen flow through your veins!", ch, NULL, NULL, TO_CHAR);

			af.where    = TO_AFFECTS;
			af.type     = gsn_divine_strength;
			af.level    = ch->level;
			af.duration = (divinesuccess * 5) + 50;
			af.modifier = ch->level/2 + (divinesuccess * 5);
			af.location = APPLY_HITROLL;
			af.bitvector    = 0;
			affect_to_char( ch, &af );

			ch->move = UMIN( ch->move + ch->level + (divinesuccess * (get_attribute(ch,STAMINA) + ch->csabilities[CSABIL_THEOLOGY])), ch->max_move );
		}
	}
	return;
}

bool check_critical(CHAR_DATA *ch, CHAR_DATA *victim)
{
        OBJ_DATA *obj;

        obj = get_eq_char(ch,WEAR_WIELD);

        if(IS_NPC(ch)) return FALSE;

        if (
             ( get_eq_char(ch,WEAR_WIELD) == NULL ) ||
             ( get_skill(ch,gsn_critical_strike)  <  1 ) ||
             ( get_weapon_skill(ch,get_weapon_sn(ch))  !=  100 ) ||
             ( number_range(0,100) > get_skill(ch,gsn_critical_strike)/6 ))
                return FALSE;

        /* Now, if it passed all the tests... */

        act2("$n {Rcritically{x strikes $N!",ch,NULL,victim,TO_NOTVICT);
        act2("Your skill with {Y$p{x allows you to {Rcritically{x strike $N!",ch,obj,victim,TO_CHAR);
        act2("$n {Rcritically{x strikes you!",ch,NULL,victim,TO_VICT);
        check_improve(ch,gsn_critical_strike,TRUE,8);
        return TRUE;
}

bool IN_LEVEL( CHAR_DATA *ch, CHAR_DATA *victim )
{
        return TRUE;
}

void do_ground( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

        if ( get_skill(ch, gsn_ground) == 0)
        {
                send_to_char(
                        "Your better off sticking to your own arts.\n\r", ch );
                return;
        }

        if ( ( victim = ch->fighting ) == NULL )
        {
                send_to_char( "You aren't fighting anyone.\n\r", ch );
                return;
        }

    if(is_affected(victim, gsn_gift_catfeet) && number_percent() > 45)
    {
    	act("You crash directly into $N, but $E tumbles backwards and upright immediately!", ch, NULL, victim, TO_CHAR);
    	act("With acrobatic grace that would make gymnasts jealous, you roll with $n's collision into you and hop back upright.", ch, NULL, victim, TO_VICT);
    	act("$n collides forcefully into $N, who simply tumbles backwards and pops upright again with apparent ease.", ch, NULL, victim, TO_NOTVICT);
    	return;
    }

        WAIT_STATE( ch, skill_table[gsn_ground].beats );

        act( "You crash into $N!",  ch, NULL, victim, TO_CHAR    );
        act( "$n crashes into you!", ch, NULL, victim, TO_VICT    );
        act( "$n crashes into $N!",  ch, NULL, victim, TO_NOTVICT );

        if ( get_skill(ch,gsn_ground) > number_percent()+20)
        {
                damage(ch,victim,number_range( 1, ch->level ), gsn_ground,DAM_BASH,TRUE);

                if(IS_AFFECTED(victim, AFF_FLYING))
                {
                        affect_strip(victim,gsn_fly);
                        REMOVE_BIT(victim->affected_by,AFF_FLYING);
                        WAIT_STATE(victim, PULSE_VIOLENCE);
                        act( "And knock $N out of the sky!",  ch, NULL, victim, TO_CHAR    );
                        act( "$n knocks you out of the sky!", ch, NULL, victim, TO_VICT    );
                        act( "$n knocks $N out of the sky!",  ch, NULL, victim, TO_NOTVICT );
                }

                check_improve(ch,gsn_ground,TRUE,2);
        }
        else
        {
                act( "And fall flat on your face!",  ch, NULL, victim, TO_CHAR    );
                act( "$n falls flat on $s face!", ch, NULL, victim, TO_VICT    );
                act( "$n falls flat on $s face!",  ch, NULL, victim, TO_NOTVICT );
                ch->position = POS_SITTING;
                damage( victim, ch, 10, gsn_ground,DAM_BASH,TRUE);
                check_improve(ch,gsn_ground,FALSE,2);
        }
        return;
}

void do_headbutt( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int dicesuccess = 0;
    int damagesuccess = 0;

        if (IS_NPC(ch))
                return;

        if (get_skill(ch,gsn_headbutt) == 0)
        {
                send_to_char("You'd just get a headache!\n\r", ch );
                return;
        }

        if (( victim = get_char_room( ch, NULL, argument ) ) == NULL && (victim = ch->fighting ) == NULL )
        {
                send_to_char( "There is no one here to butt heads with!\n\r", ch );
                return;
        }

        if (ch == victim)
        {
                send_to_char("Is that even physically possible?!\n\r", ch );
                return;
        }

    if(IS_AFFECTED2(ch,AFF2_MIST) || IS_AFFECTED2(victim,AFF2_MIST))
    {
        send_to_char("Your head passes right through!\n\r",ch);
        return;
    }

        if(victim->position == POS_SLEEPING || victim->position == POS_TORPOR)
        {
                send_to_char("They are already incapacitated!\n\r", ch );
                return;
        }

    if(ch->move < ch->level / 20)
    {
        send_to_char("You are too tired to headbutt.\n\r", ch);
        return;
    }

    ch->move -= (ch->level / 20);

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 6);

        WAIT_STATE( ch, skill_table[gsn_headbutt].beats );

    if(dicesuccess < 0)
    {
        act("$N nimbly sidesteps your headbutt, causing you to faceplant unceromoniously.", ch, NULL, victim, TO_CHAR);
        act("Moving with precise timing, you sidestep and avoid $n's attempt to introduce $s skull to your forehead.", ch, NULL, victim, TO_VICT);
        act("Throwing $mself forward, $n faceplants and lies on the ground, dazed.", ch, NULL, victim, TO_NOTVICT);
        ch->position = POS_RESTING;
        WAIT_STATE(ch, 4);
        damage(ch, ch, ch->level, gsn_headbutt, DAM_BASH, TRUE);
        check_improve(ch,gsn_headbutt,FALSE,5);
        return;
    }

    if(dicesuccess == 0)
    {
        act("You miss your headbutt!", ch, NULL, victim, TO_CHAR);
        act("$n tries to headbutt you and misses!", ch, NULL, victim, TO_VICT);
        act("$n tries to headbutt $N.", ch, NULL, victim, TO_NOTVICT);
        check_improve(ch,gsn_headbutt,FALSE,5);
        return;
    }

    damagesuccess = godice(get_attribute(ch, STRENGTH) + ch->pcdata->discipline[POTENCE], 6);
    if (damagesuccess < 0)
        damagesuccess = 0;

        act( "You slam your head into $N!", ch, NULL, victim, TO_CHAR );
        act( "$n slams $s head into yours!", ch, NULL, victim, TO_VICT );
        act( "$n slams $s head into $N's.", ch, NULL, victim, TO_ROOM );

    gain_exp(ch, dicesuccess);
    WAIT_STATE(victim, PULSE_VIOLENCE);

    damage(ch, victim, damagesuccess * ch->level / 2, gsn_headbutt, DAM_BASH, TRUE);

    if (IS_SET(victim->res_flags,RES_BASH))
        dicesuccess--;

    if(dicesuccess >= 4 && !IS_SET(victim->imm_flags,IMM_BASH))
    {
        act("$N stares at you for a second, dazed, before slumping to the ground.", ch, NULL, victim, TO_CHAR);
        act("You stare dizzily for just a moment, then the world goes black before your eyes.", ch, NULL, victim, TO_VICT);
        act("$N collapses in a crumpled heap, knocked out cold.", ch, NULL, victim, TO_NOTVICT);

        gain_exp(ch, dicesuccess);
        stop_fighting( victim, TRUE );

        af.where    = TO_AFFECTS;
        af.type     = gsn_headbutt;
        af.level    = ch->level;
        af.duration = dicesuccess - 2;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = AFF_SLEEP;
        affect_join(victim, &af);
        check_improve(ch,gsn_headbutt,TRUE,5);

        victim->position = POS_SLEEPING;
    }
    else  check_improve(ch,gsn_headbutt,TRUE,5);

    return;
}

void do_gouge( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int dicesuccess = 0;
    int damagesuccess = 0;
    int skill;
    int diff;

        if (IS_NPC(ch))
                return;

        skill = get_skill(ch,gsn_gouge);
        if (skill == 0)
        {
                send_to_char("Huh?\n\r", ch );
                return;
        }

        if (( victim = get_char_room( ch, NULL, argument ) ) == NULL && (victim = ch->fighting ) == NULL )
        {
                send_to_char( "Gouge whose eyes out?\n\r", ch );
                return;
        }

        if (ch == victim)
        {
                send_to_char("That seems like a terrible idea...\n\r", ch );
                return;
        }

    if(IS_AFFECTED2(ch,AFF2_MIST) || IS_AFFECTED2(victim,AFF2_MIST))
    {
        send_to_char("Your fingers would pass right through them!\n\r",ch);
        return;
    }

        if(IS_AFFECTED(victim, AFF_BLIND) || is_affected(victim, gsn_blindness) || is_affected(victim, gsn_gouge))
        {
                send_to_char("They are already blinded!\n\r", ch );
                return;
        }

    if (IS_NPC(victim) && !IS_SET(victim->parts, PART_EYE))
    {
        send_to_char("That creature does not seem to have eyes to gouge out.\n\r", ch);
        return;
    }

        if (skill > 90) diff = 7;
        if (skill < 90) diff = 8;
        if (skill < 70) diff = 9;
    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], diff);
    damagesuccess = godice(get_attribute(ch, STRENGTH) + ch->pcdata->discipline[POTENCE], 6);
        WAIT_STATE( ch, skill_table[gsn_gouge].beats );

    if(dicesuccess < 0)
    {
        act("$N avoids your attack, causing you to trip and fall!", ch, NULL, victim, TO_CHAR);
        act("Moving with precise timing, you sidestep and avoid $n's attempt to gouge your eyes out.", ch, NULL, victim, TO_VICT);
        act("$n leaps forward, fingers outstretched to gouge $N's eyes out, and trips!", ch, NULL, victim, TO_NOTVICT);
        ch->position = POS_RESTING;
        WAIT_STATE(ch, PULSE_VIOLENCE);
        damage(ch, ch, ch->level, gsn_trip, DAM_BASH, TRUE);
        check_improve(ch,gsn_gouge,FALSE,10);
        multi_hit(victim, ch, TYPE_UNDEFINED);
        return;
    }

    if(dicesuccess == 0 || damagesuccess < 1)
    {
        act("You miss their face by inches!", ch, NULL, victim, TO_CHAR);
        act("$n tries to gouge your eyes out and misses!", ch, NULL, victim, TO_VICT);
        act("$n tries to gouge $N's eyes out and missed by inches.", ch, NULL, victim, TO_NOTVICT);
        check_improve(ch,gsn_gouge,FALSE,5);
        multi_hit(victim, ch, TYPE_UNDEFINED);
        return;
    }

        act( "You rake your fingers across $N's eyes!", ch, NULL, victim, TO_CHAR );
        act( "$n rakes $s fingers across your eyes!", ch, NULL, victim, TO_VICT );
        act( "$n rakes $s fingers across $N's eyes!", ch, NULL, victim, TO_NOTVICT );

    gain_exp(ch, dicesuccess+damagesuccess);
    check_improve(ch,gsn_gouge,TRUE,6);
    WAIT_STATE(victim, PULSE_VIOLENCE);

    d10_damage(ch, victim, damagesuccess, d10_modifier(ch) * (20+skill) / 100, gsn_gouge, DAM_PIERCE, DEFENSE_NONE, TRUE, TRUE);

    if (skill > 90) diff = 2;
    if (skill < 90) diff = 3;
    if (skill < 70) diff = 4;
    if(damagesuccess > diff)
    {
        act("$N shrieks in agony as you gouge $S eyes out!", ch, NULL, victim, TO_CHAR);
        act("Your world becomes pain as $n tears your eyes from your skull!", ch, NULL, victim, TO_VICT);
        act("$N shrieks in agony as $n gouges their eyes out!", ch, NULL, victim, TO_NOTVICT);

        gain_exp(ch, dicesuccess);
        af.where    = TO_AFFECTS;
        af.type     = gsn_blindness;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = AFF_BLIND;
        affect_join(victim, &af);

    }

    return;
}

void do_blast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA  *obj;
    int dicesuccess;
    int damagesuccess;

        if (IS_NPC(ch))
                return;

        if (get_skill(ch,gsn_blast) == 0)
        {
                send_to_char("You have no idea how to do this!\n\r", ch );
                return;
        }

        if ( ( victim = get_char_room( ch, NULL, argument ) ) == NULL && (victim = ch->fighting ) == NULL)
        {
                send_to_char( "Noone here to blast!\n\r", ch );
                return;
        }

        if (ch == victim)
        {
                send_to_char("Blast yourself? Odd...\n\r", ch );
                return;
        }

    if ((obj = get_consumable(ch, ITEM_BLASTPOWDER)) == NULL)
    {
        send_to_char("You are not carrying any viable blasting powder.\n\r", ch);
        return;
    }

    if (obj->level > ch->level)
    {
      act("You cannot safely handle $p yet.", ch, obj, NULL, TO_CHAR);
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_blast].beats );

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_LEGERDEMAIN], 6);
    damagesuccess = godice(obj->value[1], 6);

        if(dicesuccess < 0)
        {
          act( "You cough and gag as your blast misfires, blinding yourself briefly.", ch, NULL, victim, TO_CHAR );
          act( "$n coughs and gags as smoke fills $s eyes.", ch, NULL, victim, TO_VICT );
          act( "$n coughs and gags as smoke fills $s eyes.", ch, NULL, victim, TO_NOTVICT );
          use_consumable(ch, obj, -2);
          WAIT_STATE(ch, PULSE_VIOLENCE);

        damage(ch,ch,damagesuccess * ch->level/2,gsn_blast,DAM_FIRE,TRUE);
        fire_effect(ch,ch->level,ch->level,TARGET_CHAR);
        return;
        }

        else if (dicesuccess == 0)
        {
          act( "Your blast misses $N!", ch, NULL, victim, TO_CHAR );
          act( "$n sends a blast of flame and smoke that misses you.", ch, NULL, victim, TO_VICT );
          act( "$n misses with $s blast of flame that was aimed at $N.", ch, NULL, victim, TO_ROOM );
      		check_improve(ch,gsn_blast,FALSE,4);
          use_consumable(ch, obj, -1);
      		return;
        }

    act("Using a bit of misdirection and legardemain, you send a blast of {Ds{wmok{De{x\n\rand {rf{yl{ra{ym{re{x directly at $N!", ch, NULL, victim, TO_CHAR);
    act("$n sends a blast of smoke and flame directly at you!", ch, NULL, victim, TO_VICT);
    act("$n sends a quick burst of smoke and flame towards $N.", ch, NULL, victim, TO_NOTVICT);

    d10_damage( ch, victim, damagesuccess, ch->level * 2, gsn_blast, DAM_FIRE, DEFENSE_NONE, TRUE, TRUE);
    fire_effect(victim, ch->level/2, number_range(1, ch->level+5), TARGET_CHAR);
	  check_improve(ch,gsn_blast,TRUE,8);
    use_consumable(ch, obj, -1);

    gain_exp(ch, dicesuccess*5);
    return;
}

void do_slip( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int dice, diff, success, resistdice, resist, skill, resistdiff;

    diff = resistdiff = 6;

        if (IS_NPC(ch))
                return;

        if ((skill = get_skill(ch,gsn_slip)) == 0)
        {
                send_to_char("You have no idea how to do this!\n\r", ch );
                return;
        }

        if(ch->position != POS_FIGHTING)
        {
                send_to_char("You're not fighting!\n\r", ch );
                return;
        }

        if ( ( victim = ch->fighting ) == NULL )
        {
                send_to_char( "Noone to slip away from!\n\r", ch );
                return;
        }

        dice = get_attribute(ch, WITS) + get_ability(ch, CSABIL_STEALTH);
        dice = dice * skill / 100;

        if (victim->fighting != ch)
        {
            diff--;
            resistdiff += 2;
        }

        if (is_affected(ch,gsn_gift_scentofrunningwater))
            diff--;

        success = godice(dice, diff);
        success += stealth_int_shadowplay(ch, diff);
        resistdice = get_attribute(victim, PERCEPTION) +
                 get_ability(victim, CSABIL_ALERTNESS);
        resist = godice(resistdice, resistdiff);

        if (IS_DEBUGGING(ch)) {
            char buf[MSL];

            sprintf(buf, "Dice %d Diff %d Resistdice %d Resistd %d Success %d Resist%d\n\r",
                dice, diff, resistdice, resistdiff, success, resist);
            sendch(buf, ch);
        }

        WAIT_STATE( ch, skill_table[gsn_slip].beats );

        if(success > resist)
        {
                act( "You manage to slip away from $N...", ch, NULL, victim, TO_CHAR );
                act( "$n suddenly slips away from you!", ch, NULL, victim, TO_VICT );
                act( "$n slips away from combat...", ch, NULL, victim, TO_ROOM );

                stop_fighting( ch, TRUE );
                if (!IS_AFFECTED (ch, AFF_HIDE))
                    do_function(ch, &do_hide, "");
                if(!is_affected(ch,gsn_sneak))
                    do_function(ch, &do_sneak, "");

                check_improve(ch,gsn_slip,TRUE,6);
        }

        else
        {
                act( "You fail to slip away from $N!", ch, NULL, victim, TO_CHAR );
                check_improve(ch,gsn_slip,FALSE,6);
        }

    return;
}
void do_warcry( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    CHAR_DATA *vict_next;
    CHAR_DATA *fch;
    int warcrysuccess, warcryskill;
    AFFECT_DATA af;

    if ( (warcryskill = get_skill(ch, gsn_warcry)) == 0)
    {
      send_to_char("You are better off sticking to your own arts.\n\r", ch );
      return;
    }

    if (is_affected(ch, gsn_laryngitis))
    {
      send_to_char("Your throat is too enflamed to shout your warcry.\n\r", ch);
      return;
    }

    if (ch->move < (ch->level/3) + 30)
    {
      send_to_char("You are too tired to shout right now.\n\r", ch);
      return;
    }

    WAIT_STATE( ch, skill_table[gsn_warcry].beats );
    warcrysuccess = godice(get_attribute(ch, MANIPULATION) + get_ability(ch, CSABIL_INTIMIDATION), 6);

    if (warcrysuccess < 0)
    {
      act( "You strain your throat while trying to shout!",  ch, NULL, NULL, TO_CHAR );
      af.where     = TO_AFFECTS;
      af.type      = gsn_laryngitis;
      af.level     = ch->level;
      af.duration  = -(warcrysuccess);
      af.location  = 0;
      af.modifier  = 0;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      return;
    }

    if (warcrysuccess == 0)
    {
      act( "You let out a squeak of a warcry!",  ch, NULL, NULL, TO_CHAR    );
      act( "$n attempts a mighty warcry, and succeeds only in letting out a tiny squeak.", ch, NULL, NULL, TO_ROOM    );
      check_improve(ch,gsn_warcry,FALSE,2);
      return;
    }

    act( "You bellow a loud warcry to challenge your foes!",  ch, NULL, NULL, TO_CHAR );
    act( "$n bellows a ferocious warcry!", ch, NULL, NULL, TO_NOTVICT );
    ch->move -= (ch->level / 3) + 30;
    check_improve(ch,gsn_warcry,TRUE,2);
    gain_exp(ch, warcrysuccess * 4);

    for ( victim = char_list; victim != NULL; victim = vict_next )
    {
      vict_next = victim->next;
      if(!IS_NPC(victim) || victim->in_room == NULL )
        continue;

      if ( victim->in_room == ch->in_room && SAME_UMBRA(ch, victim))
      {
        if ( victim != ch && !is_safe_spell(ch,victim,TRUE)
        && !is_affected(victim, gsn_deafened) && !IS_SET(victim->imm_flags,IMM_SOUND))
        {
/*       ** Taunt Removed until Figured out how to make it work properly **
          fch = victim->fighting;
          act("$N's focus shifts to you and $E attacks!", ch, NULL, victim, TO_CHAR);
          if (victim->fighting != NULL)
            stop_fighting(victim, TRUE);
          if (fch->fighting != NULL)
            stop_fighting(fch, TRUE);
          set_fighting(victim, ch);
          set_fighting(ch, victim);
*/
          if ((IS_SET(victim->res_flags, RES_SOUND) && warcrysuccess > 3 ) || warcrysuccess > 2 )
          {
            act( "$n's shout resonates in your ear, deafening you briefly!", ch, NULL, victim, TO_VICT );
            af.where     = TO_AFFECTS;
            af.type      = gsn_deafened;
            af.level     = warcrysuccess;
            af.duration  = 1;
            af.location  = APPLY_CS_PER;
            af.modifier  = -1;
            af.bitvector = 0;
            affect_to_char( victim, &af );
          }
//        Change up damage to higher success once Taunting is fixed
//          if (warcrysuccess > 2)
//          {
            damage( ch, victim, warcrysuccess * (ch->level/5) * warcryskill / 100, gsn_warcry, DAM_SOUND,TRUE);
//          }
        }
          continue;
      }
    }
    return;
}

void do_behead( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int dicesuccess = 0;

    if (IS_NPC(ch))
        return;

    one_argument( argument, arg );

    if (arg[0] == '\0')
    {
        send_to_char("Who deserves to lose their head?\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL)
    {
        send_to_char("That person isn't here!\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("Beheading yourself?  You've really lost it now.\n\r", ch);
        return;
    }

    if (is_safe(ch, victim))
    {
        send_to_char("No, not here.\n\r", ch);
        return;
    }

    if (ch->fighting)
    {
        send_to_char("You must stop fighting first.\n\r", ch);
        return;
    }

    if (victim->position != POS_TORPOR)
    {
        send_to_char("Your victim must be in Torpor to be beheaded.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim))
    {
        send_to_char("The Immortals wouldn't like that.\n\r", ch);
        return;
    }

    if (!IS_SET(victim->parts, PART_HEAD))
    {
        send_to_char("Your victim has no head to cut off!\n\r", ch);
        return;
    }

    WAIT_STATE(ch, 6);
    dicesuccess = godice(get_attribute(ch, STRENGTH) + 1, 5);

    if (dicesuccess < 0)
    {
        act("You swing a blade down to slice off $N's head, but instead miss and slice your leg.", ch, NULL, victim, TO_CHAR);
        act("$n swings a blade down towards $N, but misses completely; instead slicing $s own leg.", ch, NULL, victim, TO_ROOM);
        damage(ch, ch, ch->level, gsn_behead, DAM_SLASH, TRUE);
        WAIT_STATE(ch, 6);
        return;
    }

    if (dicesuccess == 0)
    {
        act("You swing the blade down and it bites into $N's neck, but not enough to sever it.", ch, NULL, victim, TO_CHAR);
        act("$n swings $s blade down into $N's neck, which remains firmly attached.", ch, NULL, victim, TO_ROOM);
        WAIT_STATE(ch, 4);
        return;
    }


    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    char *name;

    name        = IS_NPC(victim) ? victim->short_descr : victim->name;
    obj     = create_object( get_obj_index( OBJ_VNUM_SEVERED_HEAD ) , 0 );
    obj->timer  = number_range( 7, 12 );

    sprintf( buf, obj->short_descr, name );
    free_string( obj->short_descr );
    obj->short_descr = str_dup( buf );

    sprintf( buf, obj->description, name);
    free_string( obj->description );
    obj->description = str_dup( buf );

    if (obj->item_type == ITEM_FOOD)
    {
        if (IS_SET(victim->form, FORM_POISON))
            obj->value[3] = 1;
        else if (!IS_SET(ch->form, FORM_EDIBLE))
            obj->item_type = ITEM_TRASH;
    }

    if (IS_AFFECTED2(ch, AFF2_UMBRA))
        SET_BIT(obj->extra_flags, ITEM_UMBRA);

    obj_to_room( obj, ch->in_room);

    REMOVE_BIT(victim->parts, PART_HEAD);

    WAIT_STATE(ch, 6);

    act("With a mighty swing, you sever $N's head from $S body!", ch, NULL, victim, TO_CHAR);
    act("$n slices $N's head from $S body!", ch, NULL, victim, TO_NOTVICT);
    act("You feel a sharp pain in your neck, and then...blissful nothingness.", ch, NULL, victim, TO_VICT);
    act("The severed head rolls away from the body, coming to rest by your foot.", ch, NULL, victim, TO_CHAR);
    act("The severed head rolls away from the body and comes to rest by $n's feet.", ch, NULL, victim, TO_NOTVICT);

    if(ch->race == race_lookup("ghoul"))
    {
        act("Giving in to your addiction, you scramble to the headless corpse and begin lapping up the sweet vitae.", ch, NULL, victim, TO_CHAR);
        act("$n scrambles to the corpse, lapping up the blood flowing from the severed neck.", ch, NULL, victim, TO_NOTVICT);
        ch->pblood += (victim->level / 4) + 10;
        if(ch->pblood > ch->max_pblood)
            ch->pblood = ch->max_pblood;
    }


    gain_exp(ch, dicesuccess * 5);
    kill_em(ch, victim);
}

void do_allies(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    AFFECT_DATA af;
    char buf[MSL];
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

    if (is_affected(ch, gsn_botched_allies) || is_affected(ch, gsn_allies)) {
        send_to_char("You cannot call upon your allies again so soon!\n\r", ch);
        return;
    }

    if (ch->pcdata->csbackgrounds[CSBACK_ALLIES] < 1)
    {
        send_to_char( "You do not have any allies to call upon for aid!\n\r", ch );
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot summon another ally!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_ALLIES)) == NULL )
    {
        send_to_char( "Error: please infrorm the Coders!\n\r", ch );
        return;
    }

    dicesuccess = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_LEADERSHIP], 7);

    WAIT_STATE(ch, 10);

    if (dicesuccess < 0)
    {
        send_to_char("Your allies are offended that you only ever call in times of crisis.\n\rThey refuse to respond to your missive.\n\r", ch);
        af.where     = TO_AFFECTS;
        af.type      = gsn_botched_allies;
        af.level     = ch->level;
        af.duration  = number_range(10, 25-ch->pcdata->csbackgrounds[CSBACK_ALLIES]*2);
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );
        return;
    }

    if (dicesuccess == 0)
    {
        act("Your call for aid goes unanswered.", ch, NULL, NULL, TO_CHAR);
        af.where     = TO_AFFECTS;
        af.type      = gsn_allies;
        af.level     = ch->level;
        af.bitvector = 0;
        af.duration  = number_range(0, 10-ch->pcdata->csbackgrounds[CSBACK_ALLIES]);
        af.location  = 0;
        af.modifier  = 0;
        affect_to_char( ch, &af );
        return;
    }


    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );
    add_follower( mob, ch );

    mob->leader = ch;
    mob->level  = (ch->level);
    mob->max_hit = (ch->max_hit/3) + (dicesuccess * 200);
    mob->hitroll = ch->hitroll/2 + (dicesuccess * 5);
    mob->damroll = ch->damroll;
    mob->hit = mob->max_hit;
    mob->damage[DICE_NUMBER] = ch->level/2;
    mob->damage[DICE_TYPE] = 2+(dicesuccess/2);
    mob->armor[0] -= ch->level*dicesuccess;
    mob->armor[1] -= ch->level*dicesuccess;
    mob->armor[2] -= ch->level*dicesuccess;
    mob->armor[3] -= ch->level*dicesuccess;
    ch->pet = mob;




    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if (dicesuccess == 1) {
        sprintf(buf, "an acquaintance", ch->name);
        mob->short_descr = str_dup(buf);
        sprintf(buf, "An acquaintance of %s stands here, hoping he can be useful in some way.\n\r", ch->name);
        mob->long_descr = str_dup(buf);
        mob->name = str_dup("acquaintance ally allies");
    }
    if (dicesuccess > 1) {
        SET_BIT(mob->off_flags, OFF_SECOND_ATTACK);
        sprintf(buf, "an associate", ch->name);
        mob->short_descr = str_dup(buf);
        sprintf(buf, "An associate of %s stands here, watching his ally's back.\n\r", ch->name);
        mob->long_descr = str_dup(buf);
        mob->name = str_dup("associate ally allies");
    }
    if (dicesuccess > 2) {
        SET_BIT(mob->off_flags, OFF_AREA_ATTACK);
        sprintf(buf, "a group of associates", ch->name);
        mob->short_descr = str_dup(buf);
        sprintf(buf, "A group of %s's associates loiter in the area.\n\r", ch->name);
        mob->long_descr = str_dup(buf);
        mob->name = str_dup("group associates loitering ally allies");
    }
    if (dicesuccess > 3) {
        SET_BIT(mob->off_flags, OFF_THIRD_ATTACK);
        sprintf(buf, "a group of loyal allies", ch->name);
        mob->short_descr = str_dup(buf);
        sprintf(buf, "A group of %s's loyal supporters and allies loiter in the area.\n\r", ch->name);
        mob->long_descr = str_dup(buf);
        mob->name = str_dup("group loyal supporters loitering ally allies");
    }
    act("$n calls out for aid and $N comes to $s aid.", ch, NULL, mob, TO_NOTVICT);
    act("You call out for aid and $N comes to your aid.", ch, NULL, mob, TO_CHAR);

    gain_exp(ch, dicesuccess * ch->pcdata->csbackgrounds[CSBACK_ALLIES]);
    return;
}

// Matthew's PVP code. becomeactive flags you PLR2_PVP and allows you to kill other
// active players.
void do_becomeactive (CHAR_DATA *ch, char * argument)
{
    char arg[MIL];
    char buf[MSL];
    one_argument( argument, arg);

    if(ch->clan <= 0)
    {
        send_to_char("You must be in a clan, tribe, or tradition to become active.\n\r", ch);
        return;
    }

    if (IS_SET(ch->act2, PLR2_PVP))
    {
        send_to_char("You are already active.\n\r", ch);
        return;
    }

    if (str_cmp(arg, "confirm"))
    {
        send_to_char("Becoming active means that you will be subject to the Player Killing\n\r", ch);
        send_to_char("rules and could be killed, losing equipment and stats or suffering from\n\r", ch);
        send_to_char("other consequences. {RThis cannot be undone!{x\n\r", ch);
        send_to_char("\n\r", ch);
        send_to_char("If you wish to continue type 'becomeactive confirm'.\n\r", ch);
        wiznet("$N is considering becoming active.",ch,NULL,WIZ_PKILL,0,0);
        return;
    } else {
        send_to_char("You are now set as 'active'. You can be killed by other players and gain\n\r", ch);
        send_to_char("influence over domains.\n\r", ch);
        SET_BIT(ch->act2, PLR2_PVP);
        wiznet("$N is now active.",ch,NULL,WIZ_PKILL,WIZ_SECURE,0);
        return;
    }
}

int get_modifier (CHAR_DATA *ch, int type)
{
    int modifier;

    switch (type)
    {
        case MODTYPE_VERYWEAK:
            modifier = ( ch->remorts/3 + GET_DAMROLL(ch)/20);
            modifier /= 8;
            break;
        case MODTYPE_WEAK:
            modifier = ( ch->remorts/3 + GET_DAMROLL(ch)/20);
            modifier /= 4;
            break;
        case MODTYPE_WEAKMELEE:
            modifier = ( ch->remorts/3 + GET_DAMROLL(ch)/20 + ch->level/3);
            modifier /= 4;
            break;
        case MODTYPE_MELEE:
            modifier = ( ch->remorts/3) + GET_DAMROLL(ch)/10 + ch->level/3;
            modifier /= 4;
            break;
        case MODTYPE_STRONGMELEE:
            modifier = ( (ch->remorts/3) + GET_DAMROLL(ch)/10 + (ch->level/3));
            modifier /= 2;
            break;
        case MODTYPE_STRONG:
            modifier = ( (ch->remorts/3) + (GET_DAMROLL(ch)/10) + (ch->level/3));
            modifier = 3*modifier/2;
            break;
        case MODTYPE_VERYSTRONG:
            modifier = ( (2*ch->remorts/3) + (GET_DAMROLL(ch)/5) + (ch->level/2));
            break;
        case MODTYPE_GODLY:
            modifier = ( (ch->remorts) + (GET_DAMROLL(ch)/4) + (ch->level/2));
            break;

        default:
            modifier = ( (2*ch->remorts/3) + (GET_DAMROLL(ch)/5) + (ch->level/3));
            break;
    }
    return modifier;
}
