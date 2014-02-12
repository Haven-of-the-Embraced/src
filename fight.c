
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

//#define DEBUG_MESSAGES

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"

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
void    make_corpse args( ( CHAR_DATA *ch ) );
void    one_hit     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    mob_hit     args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    raw_kill    args( ( CHAR_DATA *victim ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    disarm      args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    check_critical  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );


/* experimental garou code */
char    *slash_dam_noun args((int dam,bool plural));
int     get_armor_diff  args(( CHAR_DATA *ch, CHAR_DATA *victim, int dam_type));
void    garou_hit           args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
bool    garou_damage        args( ( CHAR_DATA *ch,CHAR_DATA *victim,int claw_dam,int bite_dam,int claw_dam_type,int bite_dam_type ) );
/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */
void violence_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *victim;

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
    //if(!IS_NPC(ch) && ch->pcdata->shiftform > HOMID)
    //{
    //    ch->hit += (ch->max_hit/600+1) * get_attribute(ch,STAMINA);
    //    if(ch->hit > ch->max_hit) ch->hit = ch->max_hit;
    // }

    //if(!IS_NPC(ch) && ch->pcdata->shiftform > CRINOS)
    //{
    //   ch->move += (ch->max_move/300+1) * get_attribute(ch,STAMINA);
    //    if(ch->move > ch->max_move) ch->move = ch->max_move;
    //}
    //if (!IS_NPC(ch) && ch->race == race_lookup("garou") && ch->pcdata->shiftform != CRINOS)
    //{
    //    ch->mana += (ch->max_mana/1200+1) * get_attribute(ch, STAMINA);
    //    if (ch->mana > ch->max_mana) ch->mana = ch->max_mana;
    //}

    if ( ( victim = ch->fighting ) == NULL || ch->in_room == NULL )
        continue;

    if ( IS_AWAKE(ch) && ch->in_room == victim->in_room )
    {
        multi_hit( ch, victim, TYPE_UNDEFINED );
      //  if(!IS_NPC(ch) && ch->race == race_lookup("garou") &&
      //  ch->pcdata->shiftform == CRINOS && is_affected(ch, gsn_rage))
      //  {
      //      ch->hit += (ch->max_hit/1000+1)*get_attribute(ch, STAMINA);
      //      if(ch->hit > ch->max_hit) ch->hit = ch->max_hit;
      //  }
    }
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

        if ( HAS_TRIGGER( ch, TRIG_FIGHT ) )
        mp_percent_trigger( ch, victim, NULL, NULL, TRIG_FIGHT );
        if ( HAS_TRIGGER( ch, TRIG_HPCNT ) )
        mp_hprct_trigger( ch, victim );
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

    if (IS_AWAKE(rch) && rch->fighting == NULL)
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

        /* Domain mobs assist domain ruler and players of allied clans. */
        if (!IS_NPC(ch) && IS_NPC(rch) && IS_SET(ch->act2, PLR2_PVP) &&
        IS_SET(rch->act2, ACT2_INFLUENCE) &&
        (dom = rch->pIndexData->area->domain) != NULL &&
        (dom->clan == ch->clan || dom->relation[ch->clan] == RELATION_ALLIED))
        {
            do_function(rch, &do_emote, "screams and attacks!");
            multi_hit(rch, victim, TYPE_UNDEFINED);
            continue;
        }

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

        ||   (IS_NPC(rch) && IS_SET(rch->off_flags,ASSIST_ALIGN)
           &&   ((IS_GOOD(rch)    && IS_GOOD(ch))
             ||  (IS_EVIL(rch)    && IS_EVIL(ch))
             ||  (IS_NEUTRAL(rch) && IS_NEUTRAL(ch))))

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
    int     chance, i, c, genbonus;

    /* decrement the wait */
    if (ch->desc == NULL)
    ch->wait = UMAX(0,ch->wait - PULSE_VIOLENCE);

    if (ch->desc == NULL)
    ch->daze = UMAX(0,ch->daze - PULSE_VIOLENCE);

    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );
    if(ch->obfuscate != 6) REMOVE_BIT   ( ch->affected2_by, AFF2_VEIL       );

    /* no attacks for stunnies -- just a check */
    if (ch->position < POS_RESTING)
    return;

    if (ch->stopped > 0)
    {
        ch->stopped--;
        return;
    }
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

   /* DISABLED 9/27/13 DUE TO BEING OVERPOWERED
   
   if (!IS_NPC(ch) && IS_VAMP(ch) && (ch->gen < 10) && dt != gsn_backstab)
    { // Vampire Generation buff. same as celerity/2

    genbonus = 10 - ch->gen;
        for(i = genbonus/2;i > 0;i--)
        {
                one_hit(ch,victim,dt);
        }

    }*/
/* Replacement celerity for new buff-type celerity ability - Ugha*/
    if(!IS_NPC(ch) && is_affected(ch,gsn_celbuff) && dt != gsn_backstab)
        for(i = ch->pcdata->discipline[CELERITY];i > 0;i--)
        {
            if(number_range(1,65) <= (get_attribute(ch, DEXTERITY)*10))
                one_hit(ch,victim,dt);
        }


    /*Time Intrinsic. Acts like Celerity for Mages.
     */

    if(!IS_NPC(ch) && ch->sphere[SPHERE_TIME] > 0 && dt != gsn_backstab)
    {
        for(i = ch->sphere[SPHERE_TIME];i > 0;i--)
        {
            if((i >= 3 && number_range(1, 80) <= (get_attribute(ch,DEXTERITY)*10)) ||
               (i < 3 && number_range(1, 200) <= (get_attribute(ch, DEXTERITY) *10)))
                one_hit(ch,victim,dt);
        }
    }

    if(is_affected(ch,gsn_timealteration))
        one_hit( ch, victim, dt );
    if(is_affected(ch,gsn_timealteration) && dt != gsn_backstab)
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
        check_improve(ch,gsn_enhanced_speed,TRUE,1);
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
    check_improve(ch,gsn_second_attack,TRUE,5);
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
    check_improve(ch,gsn_fourth_attack,TRUE,6);
    if ( ch->fighting != victim )
        return;
    }

    chance = get_skill(ch,gsn_fifth_attack)/4;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
    chance /= 8;

    if ( number_percent( ) < chance )
    {
    one_hit( ch, victim, dt );
    check_improve(ch,gsn_fifth_attack,TRUE,6);
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
                                        victim->stopped = success;
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


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
    return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
    return;


    //garou_hit(ch,victim, dt);
    //return;
	
    /*
     * Figure out the type of damage message.
     */
    wield = get_eq_char( ch, WEAR_WIELD );

/*  if(!IS_NPC(ch) && wield == NULL && ch->pcdata->shiftform >= CRINOS) */
    if(!IS_NPC(ch) && ch->race == race_lookup("garou"))
    {
        garou_hit(ch,victim, dt);
        return;
    }

    if ( dt == TYPE_UNDEFINED )
    {
    dt = TYPE_HIT;
    if ( wield != NULL && wield->item_type == ITEM_WEAPON )
        dt += wield->value[3];
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
    thac0_00 = class_table[ch->class].thac0_00;
    thac0_32 = class_table[ch->class].thac0_32;
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
    if (sn != -1)
        check_improve(ch,sn,TRUE,2);
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
    if (!IS_NPC(ch) && IS_WEAPON_STAT(wield, WEAPON_SHARP))
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
            check_improve(ch,gsn_knight_training,TRUE,6);
            dam += 3 * ( dam * diceroll/300);
        }
    }



    if(ch->class == class_lookup("knight") && get_skill(ch,gsn_lance) > 1 && MOUNTED(ch))
    {
        if(get_skill(ch,gsn_lance) > number_range(1, 100) && wield->value[0] == WEAPON_POLEARM)
            dam += get_skill(ch,gsn_lance)/2;
    }

    if (MOUNTED(ch))
    {
        if(ch->class == class_lookup("knight") || ch->class == class_lookup("crusader"))
            dam += number_range(dam/8, dam);
        else
            dam += number_range(1, dam/8);
    }
     if ( (get_eq_char( ch, WEAR_WIELD ) == NULL || wield->pIndexData->vnum == OBJ_VNUM_CLAWS) && !IS_NPC(ch))
     {
        if(ch->class == class_lookup("monk"))
            dam += number_range( 5, ch->level+get_skill(ch,gsn_hand_to_hand)/5 );
        else
            dam += number_range( 5, ch->level/2+get_skill(ch,gsn_hand_to_hand)/10 );
     }

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

 /*  DISABLED 9/27/13 DUE TO BEING OVERPOWERED
 
 if (!IS_NPC(ch) && IS_VAMP(ch))
    { // Vampire Generation buff. same as potence or strength.
        if (ch->gen > 12 || (ch->gen < 10) )
            genbonus = 10 - ch->gen;
        else
            genbonus = 0;

        dam += genbonus * (dam / 8);
    }
*/
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
        check_improve(victim,gsn_renown,TRUE,3);
    }
    /* vampire anti-bless junk */
    if (wield != NULL && IS_OBJ_STAT(wield,ITEM_BLESS) && (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah")))
        dam += dam/2;
    if ( dam > 0 && check_critical(ch,victim) ){
        dam += dam;
        }

    if (is_affected(ch, gsn_controlrandomness) && number_percent( ) < 5)
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

        if (!saves_spell(level / 2,victim,DAM_POISON))
        {
        send_to_char("You feel poison coursing through your veins.",
            victim);
        act("$n is poisoned by the venom on $p.",
            victim,wield,ch,TO_ROOM);

            af.where     = TO_AFFECTS;
            af.type      = gsn_poison;
            af.level     = level;
            af.duration  = level / 2 + 1;
            af.location  = APPLY_STR;
            af.modifier  = -1;
            af.bitvector = AFF_POISON;
            affect_join( victim, &af );
        }

        /* weaken the poison if it's temporary */
        if (poison != NULL)
        {
            poison->level = UMAX(0,poison->level - 2);
            poison->duration = UMAX(0,poison->duration - 1);

            if (poison->level == 0 || poison->duration == 0)
            act("The poison on $p has worn off.",ch,wield,victim,TO_CHAR);
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
/*
        int level;

        AFFECT_DATA *agg_damage, af;

        if ((agg_damage = affect_find(wield->affected,gsn_agg_damage)) == NULL)
        level = wield->level;
        else
        level = agg_damage->level;

        send_to_char("You scream at the searing pain of your wounds!\n\r",
            victim);
        act2("$n screams in pain at the touch of $p.",
            victim,wield,NULL,TO_ROOM);

            af.where     = TO_AFFECTS;
            af.type      = gsn_agg_damage;
            af.level     = level * 3/4;
            af.duration  = 1;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = 0;
            affect_join( victim, &af );
*/
    }
        if (ch->fighting == victim && IS_WEAPON_STAT(wield,WEAPON_VAMPIRIC))
    {
        dam = number_range(5, wield->level / 2 + 10);
        act2("$p draws life from $n.",victim,wield,ch,TO_ROOM);
        act2("You feel $p drawing your life away.",victim,wield,ch,TO_CHAR);
        damage(ch,victim,dam,0,DAM_NEGATIVE,FALSE);
        victim->mana -= dam/4;
        victim->move -= dam/2;
/*      ch->alignment = UMAX(-1000,ch->alignment - 1); */
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
        act2("You are shocked by $p.",victim,ch,NULL,TO_CHAR);
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
    affect_strip(victim,gsn_vampire_regen); affect_strip(ch,gsn_vampire_regen);
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
        victim != ch && (ch->in_room->area->domain != NULL ||
            !IS_SET(ch->act2, PLR2_PVP) || !IS_SET(victim->act2, PLR2_PVP)))
                {
        stop_fighting( ch, TRUE );
        stop_fighting( victim, TRUE );
        do_function(ch, &do_flee, "auto" );

        act( "$n seems about to attack $N and flees!", ch, NULL, victim, TO_ROOM );
        return FALSE;
    }

/*  if ( !IS_NPC(victim) && !IS_NPC(ch) && ch != victim)
    {
        if(!IS_SET(ch->act,PLR_IC) || !IS_SET(victim->act,PLR_IC))
            return FALSE;
        if(!IN_LEVEL(ch, victim))           return FALSE;   } */
    /*
     * Stop up any residual loopholes.
     */
/*
    if ( dam > 2000 && dt >= TYPE_HIT)
    {
    bug( "Damage: %d: more than 2000 points!", dam );
    dam = 2000;
    send_to_char("You've breeched the damcap!\n\r",ch);
    if (!IS_IMMORTAL(ch))
    {
        OBJ_DATA *obj;
        obj = get_eq_char( ch, WEAR_WIELD );
        send_to_char("You really shouldn't cheat.\n\r",ch);
        if (obj != NULL)
            extract_obj(obj);
    }

    }
*/
    if(IS_NPC(ch)) grudge_update(ch,victim);    if(IS_NPC(victim)) grudge_update(victim,ch);
    /* damage reduction */
        if ( dam > 35)
        dam = (dam - 35)/2 + 35;
        if ( dam > 80)
        dam = (dam - 80)/2 + 80;
        
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
            if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
                mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL );
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

 //   if (!IS_NPC(victim) && IS_VAMP(victim) && victim->gen < 10)
  //      dam -= dam / victim->gen;


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
     //   if (!silver && (victim->pcdata->shiftform > HOMID &&
     //       victim->pcdata->shiftform < LUPUS) )
     //   dam -= dam / 2*UMAX(4, 10 - (godice(get_attribute(victim, STAMINA), 8)));
    }


    /*
    dam -= get_attribute(victim,STAMINA)*8;
        dam -= (godice(get_attribute(victim,STAMINA), 8)*8);
*/



     if(victim->sphere[SPHERE_ENTROPY] > 0)
    {
        if(dam_type == DAM_PIERCE || dam_type == DAM_BASH || dam_type == DAM_SLASH)
        {
					#ifdef DEBUG_MESSAGES	
			//Debug Message
            sprintf( buf, "B4E: %d ", dam);
            send_to_char( buf, victim );
			#endif
            dam -= (dam/100)*(6*ch->sphere[SPHERE_ENTROPY]);
				#ifdef DEBUG_MESSAGES	
			//Debug Message
            sprintf( buf, "AE: %d\n\r", dam);
            send_to_char( buf, victim );
			#endif
        }
    } 

  if(victim->sphere[SPHERE_FORCES] > 0)
    {
        if(dam_type != DAM_PIERCE && dam_type != DAM_BASH && dam_type != DAM_SLASH)
		#ifdef DEBUG_MESSAGES	
			//Debug Message
		cprintf(ch, "B4F: %d ", dam);
		#endif
            dam -= (dam/100)*(4*ch->sphere[SPHERE_FORCES]);
			#ifdef DEBUG_MESSAGES	
			//Debug Message
		cprintf(ch, "AF: %d\n\r", dam);
		#endif
    } 

    if ( is_affected( victim, gsn_sanctus ) )
      dam -= dam/4;

    if ( dam > 1 && IS_AFFECTED(victim, AFF_SANCTUARY) )
    dam /= 2;

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

    if ( !slaughter && dam > 3000)
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
    
    if (is_affected(victim, gsn_gift_haresleap) && number_percent() < 5*get_attribute(victim, DEXTERITY))
    {
        if (godice(get_attribute(victim, STRENGTH) + get_ability(victim, CSABIL_ATHLETICS), 7) > 0)
        {
            act("You leap back from $N's attack and escape harm!", victim, NULL, ch, TO_CHAR);
            act("$n leaps back out of range of $N's attack!", victim, NULL, ch, TO_NOTVICT);
            act("$n leaps back out of range of your attack!", victim, NULL, ch, TO_VICT);
            dam = 0;
        }
    }

    if(is_affected(victim,gsn_kineticshield) && number_range(1,100) > 50)
    {
        if(dam_type == DAM_PIERCE || dam_type == DAM_BASH || dam_type == DAM_SLASH)
        {
            act("{GYou deflect the blow with your {CKinetic Shield{G!{x",victim,NULL,ch,TO_CHAR);
            act("{G$N's blow is deflected completely by $n's {CKinetic Shield{G!{x",victim,NULL,ch,TO_NOTVICT);
            act("{GYou gasp in shock as $n's unnatural {CKinetic Shield{G completely deflects your blow!{x",victim,NULL,ch,TO_VICT);
            dam = 0;
        }
    }

    if (show)
        dam_message( ch, victim, dam, dt, immune );

    if(dam < 0) dam = 0;


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
            send_to_char( "{RAnd the Great God Ugha places you within a new whole and healthy body.{x\n\r\n\r", victim );
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
/*
    if((diff = (victim->level - ch->level)/20) < 1)
        diff = 0;
*/
    diff += -(victim_ac/100);
    if (diff < 0)
        diff = -1;

    /*if(victim_ac < -500) diff += 5;
    else if(victim_ac < -400) diff += 4;
    else if(victim_ac < -300) diff += 3;
    else if(victim_ac < -200) diff += 2;
    else if(victim_ac < -100) diff += 1;
    else if(0 < victim_ac < 100 ) diff += 0;
    else if(victim_ac > 100) diff -= 1;
    else if(victim_ac > 200) diff -= 2;

    if(diff > 10) diff = 10;
    if(diff < 0) diff = 0;*/

	    return diff/2;
}

/*
 * Hit one guy once as a garou
 */ // bm_garou_hit -- garou bookmark.
void garou_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    OBJ_DATA *wield;
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
    int diff = 5;
    bool modified = FALSE;
    int sn, skill;
	int modifier;
	bool result;
	int diceroll;


    /* just in case */
    if (victim == ch || ch == NULL || victim == NULL)
        return;

    /*
     * Can't beat a dead char!
     * Guard against weird room-leavings.
     */
    if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
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
            if (ch->race == race_lookup("garou") &&
                is_affected(ch, gsn_claws) )
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
    if (diff < 4)
        diff = 4;

    /* We have damage type and difficulty, now get the dicepool.*/
    if (IS_NPC(ch))
	{
	dice = 5;
	if (ch->level > 20) dice++;
	if (ch->level > 40) dice++;
	if (ch->level > 60) dice++;
	if (ch->level > 80) dice++;
	if (ch->level > 100) dice++;
	
	}
	 else
	 {// IS player	
		if (wield)
			dice = get_attribute(ch,DEXTERITY)+ch->pcdata->csabilities[CSABIL_MELEE];
		else
			dice = get_attribute(ch,DEXTERITY)+ch->pcdata->csabilities[CSABIL_BRAWL];
    }    
		//Dicepool Modifiers.
        
    dice += GET_HITROLL(ch)/200;

	if (dt == gsn_backstab || dt == gsn_waylay)
		dice *= 2;

		#ifdef DEBUG_MESSAGES	
//Debug Message
	cprintf(ch, "hp{r%d{x ", dice);
	if (IS_NPC(ch)) cprintf(victim, "hp{r%d{x ", dice);
#endif
			
				
		//Moment of excitement!
		tohit = godice(dice, diff);
		dice = 0; // clear dice so we can use them later.
		if (IS_NPC(ch))
			modifier = 10 + ch->level/10;
		else{
            modifier = 5;
			modifier +=  5 + UMIN(ch->remorts/5, 20) + ch->level/5;
		}
    modifier += GET_DAMROLL(ch)/50;
	modifier =  modifier * (20 + skill) / 100;

	if (check_parry(ch, victim) ||
			check_block(ch, victim) ||
			check_shield_block(ch, victim) )
		tohit = 0;

	#ifdef DEBUG_MESSAGES	
		//Debug Message
		cprintf(ch, "t{R%d{x ", tohit);
		if (IS_NPC(ch)) cprintf(victim, "t{R%d{x ", tohit);
	#endif



		if(tohit < 1) 
		{ // Miss.
			d10_damage(ch, victim, 0, modifier, dt, dam_type, TRUE);
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
			if( ch->pcdata->rage[TEMP] < ch->pcdata->rage[PERM] && number_range(1, 200) == 42 &&
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
		if (sn != -1 && number_percent() > 95 )
			check_improve(ch,sn,TRUE,1);

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
		
		if (!wield && ch->race == race_lookup("garou") && is_affected(ch, gsn_claws))
            dice += 2;
			
		if (!IS_NPC(ch) && IS_VAMP(ch))
		{
			dice += ch->pcdata->discipline[POTENCE];
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
			

		if (IS_NPC(ch))
			dice += 3 +  ch->level / 30;
		else
			dice += get_attribute(ch, STRENGTH);
			
        damsuccess += godice(dice, 5);
		#ifdef DEBUG_MESSAGES	
//Debug Message
	cprintf(ch, "dp{r%d{x ", dice);
	if (IS_NPC(ch)) cprintf(victim, "dp{r%d{x ", dice);
#endif

	result = d10_damage(ch, victim, damsuccess, modifier, dt, dam_type, TRUE);
	
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

        if (!saves_spell(level / 2,victim,DAM_POISON))
        {
        send_to_char("You feel poison coursing through your veins.",
            victim);
        act("$n is poisoned by the venom on $p.",
            victim,wield,ch,TO_ROOM);

            af.where     = TO_AFFECTS;
            af.type      = gsn_poison;
            af.level     = level;
            af.duration  = level / 2 + 1;
            af.location  = APPLY_STR;
            af.modifier  = -1;
            af.bitvector = AFF_POISON;
            affect_join( victim, &af );
        }

        /* weaken the poison if it's temporary */
        if (poison != NULL)
        {
            poison->level = UMAX(0,poison->level - 2);
            poison->duration = UMAX(0,poison->duration - 1);

            if (poison->level == 0 || poison->duration == 0)
            act("The poison on $p has worn off.",ch,wield,victim,TO_CHAR);
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
        act2("You are shocked by $p.",victim,ch,NULL,TO_CHAR);
        shock_effect(victim,wield->level/2,dam,TARGET_CHAR);
        damage(ch,victim,dam,0,DAM_LIGHTNING,FALSE);
    }
    }		

	
    tail_chain( );
    return;
}


bool d10_damage(CHAR_DATA *ch, CHAR_DATA *victim, int damsuccess, int modifier, int dt, int dam_type, bool show)
{
    extern bool slaughter;
    extern bool doubledam;
    bool immune = FALSE;
    DESCRIPTOR_DATA *d;
	int soakdice, soak, armordice;
	int dam;
    char  buf[MSL];
	soakdice = soak = armordice = dam = 0;

    if (victim->position == POS_TORPOR)
    {
        send_to_char("They are in {RT{ro{Drp{ro{Rr{x!  Now is the time to {Gbehead{x the helpless!\n\r", ch);
        return FALSE;
    }

    if ( victim->position == POS_DEAD || victim->position == POS_TORPOR )
    return FALSE;
    affect_strip(victim,gsn_chant);     affect_strip(ch,gsn_chant);
    affect_strip(victim,gsn_meditation);    affect_strip(ch,gsn_meditation);
    affect_strip(victim,gsn_vampire_regen); affect_strip(ch,gsn_vampire_regen);
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
        victim != ch && (ch->in_room->area->domain != NULL ||
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
            if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_KILL ) )
                mp_percent_trigger( victim, ch, NULL, NULL, TRIG_KILL );
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
	
	
        if (ch != victim)
        {
            if (check_dodge( ch, victim ))
                return FALSE;
        }
		#ifdef DEBUG_MESSAGES	

       cprintf(ch, "d{W%d{x ", damsuccess);
        if (IS_NPC(ch))
        cprintf(victim, "d{W%d{x ", damsuccess);
#endif
		
		//Soak dice, subtracts successes from damage.
        if (!IS_NPC(victim))
        {
        soakdice = get_attribute(victim, STAMINA);
        if (IS_VAMP(victim)) //vamps add fortitude.
            soakdice += victim->pcdata->discipline[FORTITUDE];

        } else {// IS NPC
            soakdice = victim->level/50;
         if (IS_VAMP(victim) || victim->race == race_lookup("garou"))
            soakdice++;
        }
		
        if (dam_type > DAM_BASH && (victim->race == race_lookup("human") ||
                victim->race == race_lookup("ghoul")) )
            soakdice = 0;//humans can't soak lethal.
        if ((dam_type > DAM_SLASH || dt == attack_lookup("claws") ) && victim->race != race_lookup("garou"))
            soakdice = 0;// vamps can't soak agg.
	    if (!IS_NPC(victim) && (dam_type > DAM_SLASH || dt == attack_lookup("claws") ) && 
			victim->race != race_lookup("garou") && 
			(victim->pcdata->shiftform == HOMID || victim->pcdata->shiftform == LUPUS) )
			soakdice = 0;// Garou can't soak agg in homid or lupus.

		// Armor, applied to physical damage and garou claws.
		if (dam_type <= DAM_SLASH || dt == attack_lookup("claws") )
			armordice += get_armor_diff(ch, victim, dam_type);
			
		soak = godice(armordice+soakdice, 6);

			
		if (soak < 0)
			soak = 0;
			
		damsuccess -= soak;
		
        //turn successes into real damage via the modifier.
		dam = damsuccess*modifier;
	// Bashing damage is halved.
		if (dam_type == DAM_BASH && IS_VAMP(victim))
			dam /= 2;
		
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
#ifdef DEBUG_MESSAGES  
        /*Debug Messages.*/ 
        cprintf(ch, "M{D%d{x Ar{c%d{x sd{c%d{x s{c%d{x ", modifier, armordice, soakdice, soak, dam);
        if (IS_NPC(ch))
        cprintf(victim, "{xm{D%d Ar{c%d {xsd{c%d{x s{c%d{x ", modifier, armordice, soakdice, soak, dam);
#endif
	/* damage reduction */
    //    if ( dam > 5)
    //    dam = (dam - 5)/2 + 5;
    //    if ( dam > 35)
    //    dam = (dam-35)/2 + 35;
    //    if ( dam > 30)
    //    dam = (dam - 30)/2 + 30;

    if(slaughter)
        dam *= 4;
    else if (doubledam)
        dam *= 2;
			
    if (show)
        dam_message( ch, victim, dam, dt, immune );
		
		if (dam < 1)
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
            send_to_char( "{RAnd the Great God Ugha places you within a new whole and healthy body.{x\n\r\n\r", victim );
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



char *slash_dam_noun(int dam, bool plural)
{
    if(dam <= 20) { if(plural) return "barely scratches"; else return "barely scratch"; }
    else if(dam <= 40) { if(plural) return "nips"; else return "nip"; }
    else if(dam <= 60) { if(plural) return "bruises"; else return "bruise"; }
    else if(dam <= 80) { if(plural) return "scrapes"; else return "scrape"; }
    else if(dam <= 100) { if(plural)  return "slices"; else return "slice"; }
    else if(dam <= 120) { if(plural) return "cuts"; else return "cut"; }
    else if(dam <= 150) { if(plural) return "punctures"; else return "puncture"; }
    else if(dam <= 175) { if(plural) return "tears"; else return "tear"; }
    else if(dam <= 200) { if(plural) return "rips"; else return "rip"; }
    else if(dam <= 250) { if(plural) return "gashes"; else return "gash"; }
    else if(dam <= 300) { if(plural) return "rakes"; else return "rake"; }
    else if(dam <= 350) { if(plural) return "carves"; else return "carve"; }
    else if(dam <= 400) { if(plural) return "cleaves"; else return "cleave"; }
    else if(dam <= 450) { if(plural) return "lacerates"; else return "lacerate"; }
    else if(dam <= 500) { if(plural) return "lances"; else return "lance"; }
    else if(dam <= 550) { if(plural) return "rends"; else return "rend"; }
    else if(dam <= 600) { if(plural) return "severs"; else return "sever"; }
    else if(dam <= 650) { if(plural) return "hacks"; else return "hack"; }
    else if(dam <= 800) { if(plural) return "shreds"; else return "shred"; }
    else { if(plural) return "savages"; else return "savage"; }

    return "ERROR";
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
    if (IS_SET(victim->act,ACT_TRAIN)
    ||  IS_SET(victim->act,ACT_PRACTICE)
    ||  IS_SET(victim->act,ACT_IS_HEALER)
    ||  IS_SET(victim->act,ACT_IS_CHANGER))
    {
        send_to_char("I don't think Ugha would approve.\n\r",ch);
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
    /* player doing the killing */
/*  else
    {
        if (!is_clan(ch))
        {
        send_to_char("Join a clan if you want to kill players.\n\r",ch);
        return TRUE;
        }

        if (IS_SET(victim->act,PLR_KILLER) || IS_SET(victim->act,PLR_THIEF))
        return FALSE;

        if (!is_clan(victim))
        {
        send_to_char("They aren't in a clan, leave them alone.\n\r",ch);
        return TRUE;
        }

        if (ch->level > victim->level + 8)
        {
        send_to_char("Pick on someone your own size.\n\r",ch);
        return TRUE;
        }
    } */
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
    if (IS_SET(victim->act,ACT_TRAIN)
    ||  IS_SET(victim->act,ACT_PRACTICE)
    ||  IS_SET(victim->act,ACT_IS_HEALER)
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
            chance *= godice(get_attribute(victim,DEXTERITY)+victim->pcdata->csabilities[CSABIL_BRAWL],6);
        }
    }
    else if(!IS_NPC(victim)) chance *= godice(get_attribute(victim,DEXTERITY)+victim->pcdata->csabilities[CSABIL_MELEE],6);

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
        chance *= godice(get_attribute(victim,DEXTERITY)+victim->pcdata->csabilities[CSABIL_DODGE],6);
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
            if (!IS_NPC(victim))
                torporduration = 40 - (victim->pcdata->cshumanity * 3);
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
    if ( ch->fighting != NULL )
    {
 //   bug( "Set_fighting: already fighting", 0 );
 //   sprintf( buf, "BUG: You are already fighting %s! Please inform coders!\n\r", ch->fighting->name );
 //   send_to_char( buf, ch );
    return;
    }

    if ( IS_AFFECTED(ch, AFF_SLEEP) )
    affect_strip( ch, gsn_sleep );

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
    OBJ_DATA *fetish=NULL;
    OBJ_DATA *money;
    char *name;

    one_argument(ch->name, mobname);


    if ( IS_NPC(ch))
    {
    name        = ch->short_descr;
    corpse      = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);
    corpse->timer   = number_range( 3, 6 );
    corpse->value[0]    = ch->pIndexData->vnum;
    if ( ch->gold > 0 )
    {
        money = create_money(ch->gold, ch->silver);
        if (IS_AFFECTED2(ch, AFF2_UMBRA))
            SET_BIT(money->extra_flags,ITEM_UMBRA);
        obj_to_obj(money, corpse);
/*      obj_to_obj( create_money( ch->gold, ch->silver ), corpse ); */
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
/*      obj_to_obj(create_money(ch->gold / 2, ch->silver/2), corpse);*/
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
        if (obj->pIndexData->vnum == OBJ_VNUM_FETISH)
            fetish = obj;
        else
            if( ch->race == race_lookup("MIB") ||
                ch->race == race_lookup("spirit") ||
                ch->race == race_lookup("wraith") ||
                ch->race == race_lookup("bane") )
            obj_to_room(obj, ch->in_room);
            else
            obj_to_obj( obj, corpse );
    }
    }

        if( ch->race == race_lookup("MIB") ||
            ch->race == race_lookup("spirit") ||
            ch->race == race_lookup("wraith") ||
            ch->race == race_lookup("bane") )
            return;

    if(fetish != NULL)
        obj_to_char(fetish,ch);
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
/*  save_char_obj( victim ); we're stable enough to not need this :) */
    return;
}



void group_gain( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *gch;
    CHAR_DATA *lch;
    int xp;
    int qp;
    int members;
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
    qp = number_range(1, xp/100);
    if (gch == lch && members > 1)
    qp *= 2;
    if (victim->level < gch->level || ch->leader != NULL)
    qp /= 2;
    if (victim->level + 20 < gch->level)
    qp = 0;
   /* if (IS_SET(victim->off_flags, ASSIST_GUARD))
    {
        xp = 5;
    }*/
    if(xp <= 0) xp = 1;
    sprintf( buf, "You receive {g%d{x experience and {c%d{x quest points.\n\r", xp, qp );
    send_to_char( buf, gch );
    gain_exp( gch, xp );
    gch->qpoints += qp;
            if (IS_SET(ch->act2, PLR2_QUESTOR)&&IS_NPC(victim))
        {
            if (ch->questmob == victim->pIndexData->vnum)
            {
        send_to_char("You have almost completed your QUEST!\n\r",ch);
                send_to_char("Return to the questmaster before your time runs out!\n\r",ch);
                ch->questmob = -1;
            }
        }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->wear_loc == WEAR_NONE )
        continue;

        if ( ( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL)    && IS_EVIL(ch)    )
        ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD)    && IS_GOOD(ch)    )
        ||   ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch) ) )
        {
        act( "You are zapped by $p.", ch, obj, NULL, TO_CHAR );
        act( "$n is zapped by $p.",   ch, obj, NULL, TO_ROOM );
        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
        if(IS_AFFECTED2(ch, AFF2_UMBRA))
            SET_BIT(obj->extra_flags,ITEM_UMBRA);
        }
    }
    }

    return;
}



/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute( CHAR_DATA *gch, CHAR_DATA *victim, int total_levels )
{
    int xp,base_exp,bxp;
    int level_range;
    int playerbonus;
    extern bool doubleexp;

    level_range = victim->level - gch->level;

    /* compute the base exp -  Sengir testing out new xp below
    switch (level_range)
    {
    default :   base_exp =   1;     break;
    case -9 :   base_exp =   3;     break;
    case -8 :   base_exp =   4;     break;
    case -7 :   base_exp =   7;     break;
    case -6 :   base_exp =   11;        break;
    case -5 :   base_exp =  13;     break;
    case -4 :   base_exp =  24;     break;
    case -3 :   base_exp =  35;     break;
    case -2 :   base_exp =  46;     break;
    case -1 :   base_exp =  57;     break;
    case  0 :   base_exp =  178;        break;
    case  1 :   base_exp =  79;     break;
    case  2 :   base_exp = 90;      break;
    case  3 :   base_exp = 101;     break;
    case  4 :   base_exp = 112;     break;
    case  5 :   base_exp = 123;     break;
    case  6 :   base_exp = 134;     break;
    case  7 :   base_exp = 145;     break;
    case  8 :   base_exp = 156;     break;
    case  9 :   base_exp = 167;     break;
    case  10 :  base_exp = 178;     break;
    }
*/

/*Sengir's exp testing, moving to curve later*/
    switch (level_range)
    {
        default:    base_exp =  1;  break;
        case -9:    base_exp =  7;  break;
        case -8:    base_exp =  15; break;
        case -7:    base_exp =  30; break;
        case -6:    base_exp =  45; break;
        case -5:    base_exp =  50; break;
        case -4:    base_exp =  65; break;
        case -3:    base_exp =  80; break;
        case -2:    base_exp =  95; break;
        case -1:    base_exp =  110;    break;
        case 0:     base_exp =  125;    break;
        case 1:     base_exp =  135;    break;
        case 2:     base_exp =  145;    break;
        case 3:     base_exp =  155;    break;
        case 4:     base_exp =  165;    break;
        case 5:     base_exp =  175;    break;
        case 6:     base_exp =  185;    break;
        case 7:     base_exp =  195;    break;
        case 8:     base_exp =  205;    break;
        case 9:     base_exp =  215;    break;
        case 10:    base_exp =  225;    break;
    }


/* curve test.. failed horribly
    switch (level_range)
    {
    default :   base_exp =   1;     break;
    case -9 :   base_exp =   3;     break;
    case -8 :   base_exp =   5;     break;
    case -7 :   base_exp =   7;     break;
    case -6 :   base_exp =   11;    break;
    case -5 :   base_exp =  20;     break;
    case -4 :   base_exp =  35;     break;
    case -3 :   base_exp =  65;     break;
    case -2 :   base_exp =  110;    break;
    case -1 :   base_exp =  145;    break;
    case  0 :   base_exp =  175;    break;
    case  1 :   base_exp =  180;    break;
    case  2 :   base_exp = 185;     break;
    case  3 :   base_exp = 190;     break;
    case  4 :   base_exp = 195;     break;
    case  5 :   base_exp = 200;     break;
    case  6 :   base_exp = 195;     break;
    case  7 :   base_exp = 190;     break;
    case  8 :   base_exp = 185;     break;
    case  9 :   base_exp = 180;     break;
    case  10 :  base_exp = 175;     break;
    }
*/
    if (level_range > 10)
    base_exp = 225 + 6 * (level_range - 10);



/* more curve test code :(
    if (level_range > 10)
    base_exp = 150;
*/


    xp = base_exp;
    bxp = xp/2;
    /* reduce for playing time  :  why????-Sengir

    {
    * compute quarter-hours per level *
    time_per_level = 4 *
             (gch->played + (int) (current_time - gch->logon))/3600
             / gch->level;

    time_per_level = URANGE(2,time_per_level,12);
    if (gch->level < 15)  * make it a curve *
        time_per_level = UMAX(time_per_level,(15 - gch->level));
    xp = xp * time_per_level / 12;
    }
*/
    int cuplayers;
    /* randomize the rewards */
    xp = number_range (xp , xp * 5/4);
    /* Reward for more players being online */
   //Broken! xp = xp + (xp*(cuplayers*2) / 100);
    /* adjust for grouping */
/*    xp = xp * gch->level/( UMAX(1,total_levels -1) ); */

    if( IS_AFFECTED2(victim, AFF2_DOUBLE_EXP))
        xp = xp*2;





/* pack hunting code
    if (gch->race == race_lookup("garou"))
    {
        int garoucount = 0;
        CHAR_DATA *rch, *rch_next;
        for ( rch = gch->in_room->people; rch != NULL; rch = rch_next )
        {
            rch_next = rch->next_in_room;
            if ( !IS_NPC(rch) && rch->race == race_lookup("garou") && is_same_group(gch,rch) && rch != gch)
            garoucount += 10;
        }
        if(garoucount > 0) garoucount += 10;
        xp += garoucount;
    }
*/
    /* exp caps put in for new exp code */
/*    if(gch->level > 75 && xp > 200)
    xp = 200;
    else if(gch->level > 50 && xp > 250)
        xp = 250;
    else if(gch->level > 30 && xp > 300)
        xp = 300;
    else if(gch->level > 15 && xp > 350)
        xp = 350;
    else if (xp > 400)
    xp = 400;
*/
/*Sengir altered extra xp if you are leader*/
    if ((gch->leader == NULL) && (level_range >= -5))
        xp += gch->pcdata->csabilities[CSABIL_LEADERSHIP] * 5;

/*Sengir remort bonus xp code*/
    if (level_range >= -5)
        xp += gch->remorts / 2;

    if(gch->sphere[SPHERE_MIND] > 0  && level_range > -5)
        xp += 4*gch->sphere[SPHERE_MIND];

    /* standard exp cap for all levels.. try it out */
    if(gch->wimpy == 0)
    {
        if(gch->sphere[SPHERE_MIND] > 0 && xp > (370+gch->remorts+gch->sphere[SPHERE_MIND]*4)) xp = (370+gch->remorts+gch->sphere[SPHERE_MIND]*4);
        if(xp > 370+gch->remorts) xp = 370+gch->remorts; //get out there and get some!
    }
    else if(xp > 350) xp = 350; //keep e'm from being pansies

    /*Sengir added deduction for being afkish in group, not contributing*/
    if (gch->timer > 3)
        xp -= xp * gch->timer / 12;
    /*Zelan added bonus XP spells and a newbie xp boost here */
    if ( IS_AFFECTED(gch, AFF_XP_BOOST))
    xp = bxp + xp;

    if ( IS_SET(gch->act2, PLR2_NEWBIE))
    xp =  bxp + xp;

    if(victim->leader != NULL)
        return 0;
    else
    {
        if(doubleexp) return xp*2;
        else return xp;
    }

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
    int chance, hp_percent;

    if ((chance = get_skill(ch,gsn_berserk)) == 0
    ||  (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BERSERK))
    ||  (!IS_NPC(ch)
    &&   ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
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

    if (ch->mana < ch->level / 2)
    {
    send_to_char("You can't get up enough energy.\n\r",ch);
    return;
    }

    if (ch->move < ch->level / 5)
    {
        send_to_char("You are too tired to get angry.\n\r", ch);
        return;
    }

    /* modifiers */

    /* fighting */
    if (ch->position == POS_FIGHTING)
    chance += 10;

    /* damage -- below 50% of hp helps, above hurts */
    hp_percent = 100 * ch->hit/ch->max_hit;
    chance += 25 - hp_percent/2;

    if (number_percent() < chance)
    {
    AFFECT_DATA af;

    WAIT_STATE(ch,PULSE_VIOLENCE);
    ch->mana -= ch->level / 2;
    ch->move -= ch->level / 5;

    /* heal a little damage */
    ch->hit += ch->level * 2;
    ch->hit = UMIN(ch->hit,ch->max_hit);

    send_to_char("Your pulse races as you are consumed by rage!\n\r",ch);
    act("$n gets a wild look in $s eyes.",ch,NULL,NULL,TO_ROOM);
    check_improve(ch,gsn_berserk,TRUE,2);

    af.where    = TO_AFFECTS;
    af.type     = gsn_berserk;
    af.level    = ch->level;
    af.duration = number_fuzzy(ch->level / 8);
    af.modifier = UMAX(1,ch->level/5);
    af.bitvector    = AFF_BERSERK;

    af.location = APPLY_HITROLL;
    affect_to_char(ch,&af);

    af.location = APPLY_DAMROLL;
    affect_to_char(ch,&af);

    af.modifier = UMAX(10,10 * (ch->level/5));
    af.location = APPLY_AC;
    affect_to_char(ch,&af);
    }

    else
    {
    WAIT_STATE(ch,2 * PULSE_VIOLENCE);
    ch->mana -= ch->level / 4;
    ch->move -= ch->level / 5;

    send_to_char("Your pulse speeds up, but nothing happens.\n\r",ch);
    check_improve(ch,gsn_berserk,FALSE,2);
    }
}

void do_bash( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *shield;
    int chance;
    int dicesuccess = 0, damagesuccess = 0;

    one_argument(argument,arg);

    if (IS_NPC(ch))
        return;

    if ( (chance = get_skill(ch,gsn_bash)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_BASH))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_bash].skill_level[ch->class]))
    {
    send_to_char("Bashing? What's that?\n\r",ch);
    return;
    }

    if (( shield = get_eq_char(ch, WEAR_SHIELD)) == NULL)
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

   else if ((victim = get_char_room(ch,arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't bash while riding!\n\r", ch);
        return;
    }
/*
    if (victim->position < POS_STUNNED)
    {
    act("$N is already stunned.",ch,NULL,victim,TO_CHAR);
    return;
    }
*/
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

    if(is_affected(victim,gsn_precognition) && number_percent() > 50)
    {
        send_to_char("They avoid your shield bash!\n\r",ch);
        return;
    }


    WAIT_STATE(ch, 12);
    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->pcdata->csabilities[CSABIL_MELEE], 6);

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
        WAIT_STATE(ch, 6);
        return;
    }

    gain_exp(ch, dicesuccess*2);
    act("You swing $p, forcefully colliding with $N.", ch, shield, victim, TO_CHAR);
    act("$n smashes you with $p.", ch, shield, victim, TO_VICT);
    act("$n slams $p into $N.", ch, shield, victim, TO_NOTVICT);

    if (dicesuccess >= 4)
    {
        act("$p connects solidly with $N's head, causing a mild concussion.", ch, shield, victim, TO_CHAR);
        act("$n's $p connects with your head, rattling your brain.", ch, shield, victim, TO_VICT);
        act("$N appears to be a bit dazed after being hit with $p.", ch, shield, victim, TO_NOTVICT);
        DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
    }

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
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_dirt)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_KICK_DIRT))
    ||   (!IS_NPC(ch)
    &&    ch->level < skill_table[gsn_dirt].skill_level[ch->class]))
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
    else if ((victim = get_char_room(ch,arg)) == NULL)
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

    if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
    {
        send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
        return;
    }

/*    if ( !IS_NPC(victim) && (!IS_SET(ch->act,PLR_IC) || !IS_SET(victim->act,PLR_IC)))
    {
        send_to_char( "You and your victim must both be in IC mode to fight.\n\r", ch );
        return;
    }
*/
    if (IS_AFFECTED(ch,AFF_CHARM) && ch->master == victim)
    {
    act("But $N is such a good friend!",ch,NULL,victim,TO_CHAR);
    return;
    }

    /* modifiers */

    /* dexterity */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= 2 * get_curr_stat(victim,STAT_DEX);

    /* speed  */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
    chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
    chance -= 25;

    /* level */
    chance += (ch->level - victim->level) * 2;

    /* sloppy hack to prevent false zeroes */
    if (chance % 5 == 0)
    chance += 1;

    /* terrain */

    switch(ch->in_room->sector_type)
    {
    case(SECT_INSIDE):      chance -= 20;   break;
    case(SECT_CITY):        chance -= 10;   break;
    case(SECT_FIELD):       chance +=  5;   break;
    case(SECT_FOREST):              break;
    case(SECT_HILLS):               break;
    case(SECT_MOUNTAIN):        chance -= 10;   break;
    case(SECT_WATER_SWIM):      chance  =  0;   break;
    case(SECT_WATER_NOSWIM):    chance  =  0;   break;
    case(SECT_AIR):         chance  =  0;   break;
    case(SECT_DESERT):      chance += 10;   break;
    case(SECT_WATER_DROWN): chance  =  0;   break;
    case(SECT_HOT):             break;
    case(SECT_COLD):                break;
    }

    if (chance == 0)
    {
    send_to_char("There isn't any dirt to kick.\n\r",ch);
    return;
    }

    if(is_affected(victim,gsn_precognition) && number_percent() > 50)
    {
        send_to_char("They avoid your kicked dirt!\n\r",ch);
        return;
    }

    /* now the attack */
    if (number_percent() < chance)
    {
    AFFECT_DATA af;
    act("$n is blinded by the dirt in $s eyes!",victim,NULL,NULL,TO_ROOM);
    act("$n kicks dirt in your eyes!",ch,NULL,victim,TO_VICT);

    damage(ch,victim,number_range(ch->level / 4, ch->level / 2),gsn_dirt,DAM_NONE,FALSE);
    send_to_char("You can't see a thing!\n\r",victim);
    check_improve(ch,gsn_dirt,TRUE,2);
    WAIT_STATE(ch,skill_table[gsn_dirt].beats);

    af.where    = TO_AFFECTS;
    af.type     = gsn_dirt;
    af.level    = ch->level;
    af.duration = 0;
    af.location = APPLY_HITROLL;
    af.modifier = -4;
    af.bitvector    = AFF_BLIND;

    affect_to_char(victim,&af);
    }
    else
    {
    damage(ch,victim,0,gsn_dirt,DAM_NONE,TRUE);
    check_improve(ch,gsn_dirt,FALSE,2);
    WAIT_STATE(ch,skill_table[gsn_dirt].beats);
    }
}

void do_trip( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int chance;

    one_argument(argument,arg);

    if ( (chance = get_skill(ch,gsn_trip)) == 0
    ||   (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_TRIP))
    ||   (!IS_NPC(ch)
      && ch->level < skill_table[gsn_trip].skill_level[ch->class]))
    {
    send_to_char("Tripping?  What's that?\n\r",ch);
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

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
    send_to_char("They aren't here.\n\r",ch);
    return;
    }

    if ( MOUNTED(ch) )
    {
        send_to_char("You can't trip while riding!\n\r", ch);
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

    if (IS_AFFECTED(victim,AFF_FLYING))
    {
    act("$S feet aren't on the ground.",ch,NULL,victim,TO_CHAR);
    return;
    }

    if (victim->position < POS_FIGHTING)
    {
    act("$N is already down.",ch,NULL,victim,TO_CHAR);
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

    /* modifiers */

    /* size */
    if (ch->size < victim->size)
        chance += (ch->size - victim->size) * 10;  /* bigger = harder to trip */

    /* dex */
    chance += get_curr_stat(ch,STAT_DEX);
    chance -= get_curr_stat(victim,STAT_DEX) * 3 / 2;

    /* speed */
    if (IS_SET(ch->off_flags,OFF_FAST) || IS_AFFECTED(ch,AFF_HASTE))
    chance += 10;
    if (IS_SET(victim->off_flags,OFF_FAST) || IS_AFFECTED(victim,AFF_HASTE))
    chance -= 20;

    /* level */
    chance += (ch->level - victim->level) * 2;

    if(is_affected(victim,gsn_precognition) && number_percent() > 50)
    {
        send_to_char("They avoid your trip!\n\r",ch);
        return;
    }

    if(is_affected(victim, gsn_gift_catfeet) && number_percent() > 45)
    {
        send_to_char("You swipe your foot out to trip them, but a nimble sidestep causes you to miss!\n\r", ch);
        return;
    }

    /* now the attack */
    if (number_percent() < chance)
    {
    act("$n trips you and you go down!",ch,NULL,victim,TO_VICT);
    act("You trip $N and $S goes down!",ch,NULL,victim,TO_CHAR);
    act("$n trips $N, sending $M to the ground.",ch,NULL,victim,TO_NOTVICT);
    check_improve(ch,gsn_trip,TRUE,1);

    DAZE_STATE(victim,2 * PULSE_VIOLENCE);
        WAIT_STATE(ch,skill_table[gsn_trip].beats);
    victim->position = POS_RESTING;
    damage(ch,victim,number_range(2, ch->level / 3 + victim->size),gsn_trip, DAM_BASH,TRUE);
    }
    else
    {
    damage(ch,victim,0,gsn_trip,DAM_BASH,TRUE);
    WAIT_STATE(ch,skill_table[gsn_trip].beats*2/3);
    check_improve(ch,gsn_trip,FALSE,1);
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

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
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

/*    if ( victim->fighting != NULL &&
    !is_same_group(ch,victim->fighting))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }
*/
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
/* Don't need this. Murder makes it useless.
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

    if ( !IS_NPC(victim) && (!IS_SET(ch->act,PLR_IC) || !IS_SET(victim->act,PLR_IC)))
    {
    send_to_char( "You and your victim must both be in IC mode to fight.\n\r", ch );
    return;
    } */
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

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
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

/*
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

    if (IS_AFFECTED(ch,AFF_CHARM) || (IS_NPC(ch) && IS_SET(ch->act,ACT_PET)))
    return;

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "Suicide is a mortal sin.\n\r", ch );
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

    if ( !IS_AFFECTED2(ch, AFF2_MIST) && IS_AFFECTED2(victim, AFF2_MIST))
    {
    send_to_char( "You can barely see them, let alone attack them!\n\r", ch );
    return;
    }

    if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
    {
        send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
        return;
    }

    if ( !IS_NPC(victim) && (!IS_SET(ch->act,PLR_IC) ||
!IS_SET(victim->act,PLR_IC)))
    {
        send_to_char( "You and your victim must both be in IC mode to fight.\n\r, ch );
        return;
    }

    WAIT_STATE( ch, 1 * PULSE_VIOLENCE );
    if (IS_NPC(ch))
    check_killer( ch, victim );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

*/

void do_backstab( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int check;
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

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if (!IS_AFFECTED(ch,AFF_SNEAK))
    {
        send_to_char("How do you expect to sneak up on them if your not sneaking?\n\r",ch);
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

    WAIT_STATE( ch, skill_table[gsn_backstab].beats );
    if ( number_percent( ) < get_skill(ch,gsn_backstab)
    || ( get_skill(ch,gsn_backstab) >= 2 && !IS_AWAKE(victim) ) )
    {
    check_improve(ch,gsn_backstab,TRUE,1);

    one_hit( ch, victim, gsn_backstab );
    check = number_percent();
    if (!IS_AFFECTED(ch,AFF_HASTE))
       {
       if (check < 40)
          one_hit( ch, victim, gsn_backstab );
       }

    if (IS_AFFECTED(ch,AFF_HASTE))
       {
       if (check < 80)
          one_hit( ch, victim, gsn_backstab );
       }
        if(is_affected(ch,gsn_timealteration))
        one_hit( ch, victim, gsn_backstab );
    }
    else
    {
    check_improve(ch,gsn_backstab,FALSE,1);
    damage( ch, victim, 0, gsn_backstab,DAM_NONE,TRUE);
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

    if ( is_affected( ch, gsn_vamp_frenzy ) || is_affected( ch, gsn_garou_frenzy ) || is_affected(ch, gsn_thaumaturgy_frenzy) || is_affected(ch,gsn_armsoftheabyss))
    {
        send_to_char("Your frenzy prevents you from fleeing!\n\r",ch);
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
    {
        send_to_char( "You flee from combat!\n\r", ch );
    if( (ch->class == 2)
        && (number_percent() < 3*(ch->level/2) ) )
        send_to_char( "You snuck away safely.\n\r", ch);
    else
        {
        send_to_char( "You lost 10 exp.\n\r", ch);
        gain_exp( ch, -10 );
        if (ch->race == race_lookup("garou") && ch->pcdata->rank == 0)
        {
            send_to_char("The shame and humiliation of fleeing from a fight only adds fuel to your anger!\n\r", ch);
            ch->rage += 3;
        }
        }
    }

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

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Rescue whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
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

/*    if ( IS_NPC(fch) && !is_same_group(ch,victim))
    {
        send_to_char("Kill stealing is not permitted.\n\r",ch);
        return;
    }
*/
    WAIT_STATE( ch, skill_table[gsn_rescue].beats );
    if ( number_percent( ) > get_skill(ch,gsn_rescue))
    {
    send_to_char( "You fail the rescue.\n\r", ch );
    check_improve(ch,gsn_rescue,FALSE,1);
    return;
    }

    act( "You rescue $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n rescues you!", ch, NULL, victim, TO_VICT    );
    act( "$n rescues $N!",  ch, NULL, victim, TO_NOTVICT );
    check_improve(ch,gsn_rescue,TRUE,1);

    stop_fighting( fch, FALSE );
    stop_fighting( victim, FALSE );

    set_fighting( ch, fch );
    set_fighting( fch, ch );
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
/*  bool wp = FALSE; */


/*  one_argument( argument, arg );
    if (!str_prefix(arg,"wp"))
    {
        if (IS_NPC(ch))
            return;

        if(ch->pcdata->cswillpower <= 0)
        {
            send_to_char("You do not have enough {WWillpower{x to spend.\n\r", ch);
            return;
        }

        wp = TRUE;

    }
*/
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

    else if ((victim = get_char_room(ch, argument)) == NULL)
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

/*  if (wp)
        ch->pcdata->cswillpower--;
*/
    if (is_affected(victim, gsn_precognition) && number_percent() > 50)
    {
        act("With uncanny judgement, $N deftly avoids your kick!", ch, NULL, victim, TO_CHAR);
        act("With a brief flash of insight, you swiftly react and dodge $n's incoming kick.", ch, NULL, victim, TO_VICT);
        act("Moving almost as if choreographed, $N twists to the side and dodges $n's kick.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

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

        dicesuccess = godice(mobdice, 6);

        if (dicesuccess < 0)
        {
            act("$n trips and falls as you sidestep a wild kick.", ch, NULL, victim, TO_VICT);
            act("$n trips and falls while attempting to kick $N.", ch, NULL, victim, TO_NOTVICT);
            ch->stopped = 1;
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

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->pcdata->csabilities[CSABIL_BRAWL], 5);
/*  if (wp)
    {
        send_to_char("You spent {WWillpower{x!  Extra success!\n\r", ch);
        dicesuccess++;
    }
*/
    WAIT_STATE(ch, 12);

    if (dicesuccess < 0)
    {
        act("The momentum of a severely misjudged kick towards $N carries you around, \n\rcausing you to slip and fall.", ch, NULL, victim, TO_CHAR);
        act("$n misses you with a wild kick and slips to the ground.", ch, NULL, victim, TO_VICT);
        act("$n kicks out in $N's direction, but slips to the ground.", ch, NULL, victim, TO_NOTVICT);
        ch->stopped = 2;
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
            victim->stopped = 2;
        }

        gain_exp(ch, dicesuccess*2);
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

        dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->pcdata->csabilities[CSABIL_BRAWL], 6);

        if (dicesuccess < 0)
        {
            act("but manage to turn too quickly, setting yourself off balance as you tumble to the ground.", ch, NULL, victim, TO_CHAR);
            act("and stumbles to the ground as the momentum carries $s through the turn.", ch, NULL, victim, TO_VICT);
            act("but falls to the ground, carried by the wild kick.", ch, NULL, victim, TO_NOTVICT);

            ch->stopped = 3;
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
    int chance,hth,ch_weapon,vict_weapon,ch_vict_weapon;

    hth = 0;

    if (IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char( "Disarming is no longer allowed in the Arena.\n\r", ch );
        return;
    }

    if ((chance = get_skill(ch,gsn_disarm)) == 0)
    {
    send_to_char( "You don't know how to disarm opponents.\n\r", ch );
    return;
    }

    if ( get_eq_char( ch, WEAR_WIELD ) == NULL
    &&   ((hth = get_skill(ch,gsn_hand_to_hand)) == 0
    ||    (IS_NPC(ch) && !IS_SET(ch->off_flags,OFF_DISARM))))
    {
    send_to_char( "You must wield a weapon to disarm.\n\r", ch );
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

    /* find weapon skills */
    ch_weapon = get_weapon_skill(ch,get_weapon_sn(ch));
    vict_weapon = get_weapon_skill(victim,get_weapon_sn(victim));
    ch_vict_weapon = get_weapon_skill(ch,get_weapon_sn(victim));

	if (IS_NPC(ch))
	{
		/* modifiers */
		/* skill */
		if ( get_eq_char(ch,WEAR_WIELD) == NULL)
		chance = chance * hth/150;
		else
		chance = chance * ch_weapon/100;

		chance += (ch_vict_weapon/2 - vict_weapon) / 2;

		/* dex vs. strength */
		chance += get_curr_stat(ch,STAT_DEX);
		chance -= 2 * get_curr_stat(victim,STAT_STR);

		/* level */
		chance += (ch->level - victim->level);
		chance /= 2;
		
		
		if(is_affected(victim,gsn_precognition) && number_percent() > 50)
		{
			send_to_char("They avoid your attempt to disarm!\n\r",ch);
			return;
		}

		/* and now the attack */
		if (number_percent() < chance)
		{
			WAIT_STATE( ch, skill_table[gsn_disarm].beats );
		disarm( ch, victim );
		check_improve(ch,gsn_disarm,TRUE,1);
		}
		else
		{
		WAIT_STATE(ch,skill_table[gsn_disarm].beats);
		act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
		act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
		act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
		check_improve(ch,gsn_disarm,FALSE,1);
		}
		return;
	} else 
		{ // CH is pc.
		int success, dice, diff;
		success = dice = diff = 0;
		// Disarm is rolled as a regular attack, but if the successes are greater than
		// victim/s strength, they take no damage and the weapon goes flying.
		dice = get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_MELEE);
		success = godice(dice, 7);
		
		if (success > 0)
		{
			if (IS_NPC(victim))
				diff = (victim->level/12);
			else
				diff = get_attribute(ch, STRENGTH);
				
			if (diff > 6)
				diff = 6;
			if (diff < 1)
				diff = 1;
				
			if (success > diff)
			{
				WAIT_STATE( ch, skill_table[gsn_disarm].beats );
				disarm( ch, victim );
				check_improve(ch,gsn_disarm,TRUE,1);
			} else
				{
				act("You fail to disarm $N, hitting $M instead.", ch, NULL, victim, TO_CHAR);
				WAIT_STATE(ch,skill_table[gsn_disarm].beats);
				one_hit( ch, victim, TYPE_UNDEFINED );
				check_improve(ch,gsn_disarm,FALSE,1);
				return;
				}
		} else {
			WAIT_STATE(ch,skill_table[gsn_disarm].beats);
			act("You fail to disarm $N.",ch,NULL,victim,TO_CHAR);
			act("$n tries to disarm you, but fails.",ch,NULL,victim,TO_VICT);
			act("$n tries to disarm $N, but fails.",ch,NULL,victim,TO_NOTVICT);
			check_improve(ch,gsn_disarm,FALSE,1);
		}
		
	}
		

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
    &&   ( !HAS_TRIGGER( mob, TRIG_SURR )
        || !mp_percent_trigger( mob, ch, NULL, NULL, TRIG_SURR ) ) )
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

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
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
    OBJ_DATA *corpse;
    char mobname[MAX_STRING_LENGTH];
    DOMAIN_DATA *dom;

    if (!ch || !victim)
        return;

    /* new arena code */
        if (IS_SET(victim->act,PLR_ARENA))
        {
                stop_fighting( victim, TRUE );
                death_cry( victim );
                if(victim != ch) announce(victim, ch, WIZ_MOBDEATHS);
                REMOVE_BIT(victim->act,PLR_ARENA);
                victim->position = POS_STANDING;
                act( "{BYou are victorious!{x\n\r{YThe room fills with the scent of wild flowers as an unseen presence\n\r takes the soul of $N to a better place.{x",  ch, NULL, victim, TO_CHAR    );
                act( "{RSorry, you are dead!{x\n\r\n\r{YYou are magically saved from death by the mercy of Jasmine.{x", ch, NULL, victim, TO_VICT    );
                act( "{YThe room fills with the scent of wild flowers as an unseen presence\n\r takes the soul of $N to a better place.{x",  ch, NULL, victim, TO_NOTVICT );
                if (victim->in_room != NULL)
                        char_from_room( victim );
                char_to_room( victim, get_room_index( ROOM_VNUM_ALTAR ) );
                if ( IS_AFFECTED(victim, AFF_SHIFT))
                {       if (!IS_AFFECTED(victim, AFF_FANGS))
                        SET_BIT(victim->affected_by, AFF_FANGS);
                        do_function(ch, &do_shift, "");
                        do_function(ch, &do_fangs, "");
                }
                if(is_affected( victim, gsn_vicissitude_horrid ))
                {
                    if (!IS_AFFECTED(victim, AFF_FANGS))
                    SET_BIT(victim->affected_by, AFF_FANGS);
                    do_function(victim, &do_shift, "");
                    do_function(victim, &do_fangs, "");
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
        if ( is_affected( victim, gsn_change ) )
            do_function(victim, &do_changeform, "homid");
        victim->rage = 0;
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

    if ( IS_AFFECTED(victim, AFF_SHIFT))
    {   if (!IS_AFFECTED(victim, AFF_FANGS))
        SET_BIT(victim->affected_by, AFF_FANGS);
        do_function(victim, &do_shift, "");
        do_function(victim, &do_fangs, "");
    }

    if(is_affected( victim, gsn_vicissitude_horrid ))
    {
        if (!IS_AFFECTED(victim, AFF_FANGS))
            SET_BIT(victim->affected_by, AFF_FANGS);
        do_function(victim, &do_horrid, "");
        do_function(victim, &do_fangs, "");
    }
/* garou stuff */
    if (victim->changed > 0)
        do_function(victim, &do_changeform, "homid");
    victim->rage = 0;
    if(!IS_NPC(victim) && victim->race == race_lookup("ghoul"))
        victim->pblood = victim->max_pblood;

        /*
         * Dying penalty:
         * all the way back to previous level.
         */
/*      if ( victim->exp > exp_per_level(victim,victim->pcdata->points)
                   * victim->level )
    gain_exp( victim, (2 * (exp_per_level(victim,victim->pcdata->points)
                     * victim->level - victim->exp)/3) + 50 );
*/
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
    if ( IS_NPC( victim ) && HAS_TRIGGER( victim, TRIG_DEATH) )
      {
        victim->position = POS_STANDING;
        mp_percent_trigger( victim, ch, NULL, NULL, TRIG_DEATH );
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

    if (!IS_NPC(ch)
    &&  (corpse = get_obj_list(ch,"corpse",ch->in_room->contents)) != NULL
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
    return;
}
void do_assassinate( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int chance;
    int dam;

    one_argument( argument, arg );

    if (get_skill(ch,gsn_assassinate) == 0)
    {
    send_to_char( "Huh?\n\r", ch );
    return;
    }

    if (arg[0] == '\0')
    {
        send_to_char("Assassinate whom?\n\r",ch);
        return;
    }

    chance = number_range(1, 100);

    if (!IS_AFFECTED(ch,AFF_SNEAK))
    {
        send_to_char("How do you expect to sneak up on them if your not sneaking?\n\r",ch);
        return;
    }

    if (ch->fighting != NULL)
    {
    send_to_char("You're facing the wrong end.\n\r",ch);
    return;
    }

    else if ((victim = get_char_room(ch,arg)) == NULL)
    {
        send_to_char("They aren't here.\n\r",ch);
        return;
    }

    if (victim->fighting != NULL)
    {
        send_to_char("They are too defensive as it is, wait til they aren't fighting!\n\r", ch);
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


    WAIT_STATE( ch, skill_table[gsn_assassinate].beats );
    if ( victim->hit < victim->max_hit)
    {
    act( "$N is hurt and suspicious ... you can't sneak up.",
        ch, NULL, victim, TO_CHAR );
    return;
    }
    dam = number_range(1, ch->level);
    dam += get_curr_stat(ch,STAT_DEX);
    dam += get_curr_stat(ch,STAT_STR);


    if ( number_percent( ) < get_skill(ch,gsn_assassinate)
    || ( get_skill(ch,gsn_assassinate) >= 2 && !IS_AWAKE(victim) ) )
    {
    if(chance <= 10+get_curr_stat(ch,STAT_DEX))
        {
        act( "$N screams as you place your dagger in their spine!", ch, NULL, victim, TO_CHAR );
        act( "$n plants his dagger into your spine, your body spasms in pain.", ch, NULL, victim, TO_VICT );
        act( "$n stabs $N in the spine!", ch, NULL, victim, TO_NOTVICT );
        damage( ch, victim, (ch->level +dam)*100, gsn_assassinate,DAM_PIERCE,TRUE);
        ch->move -= 50;
        return;
    }
/*  if(chance == 1 && !IS_NPC(victim) && (victim->race != race_lookup("vampire") || victim->race != race_lookup("methuselah")))
    {
                act( "$N screams as you place your dagger in their spine!", ch, NULL, victim, TO_CHAR );
                act( "$n plants his dagger into your spine, your body spasms in pain.", ch, NULL, victim, TO_VICT );
                act( "$n stabs $N in the spine!", ch, NULL, victim, TO_NOTVICT );
        return;
    }
*/
    check_improve(ch,gsn_assassinate,TRUE,1);
    do_function(ch, &do_backstab, victim->name);
/*  multi_hit( ch, victim, gsn_assassinate ); */
    }
    else
    {
    check_improve(ch,gsn_assassinate,FALSE,1);
    damage( ch, victim, 0, gsn_assassinate,DAM_NONE,TRUE);
    }

    return;
}
void do_divine_strength(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if (get_skill(ch,gsn_divine_strength) == 0)
    {
        send_to_char( "You are not holy enough!\n\r", ch );
        return;
    }
    if ( is_affected( ch, gsn_divine_strength ))
    {
        send_to_char( "You are already blessed by your God!\n\r", ch );
        return;
    }
    if ( get_skill(ch,gsn_divine_strength) > number_percent())
    {
        act( "$n invokes the power of $s God!",  ch, NULL, NULL, TO_ROOM );
        send_to_char( "You pray to your God to give you strength to smite your foes!\n\r", ch );
        check_improve(ch,gsn_divine_strength,TRUE,1);

        af.where    = TO_AFFECTS;
        af.type     = gsn_divine_strength;
        af.level    = ch->level;
        af.duration = ch->level/2;
        af.modifier = ch->level/2+5;
        af.location = APPLY_DAMROLL;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_divine_strength;
        af.level    = ch->level;
        af.duration = ch->level/2;
        af.modifier = 1;
        af.location = APPLY_CS_STR;
        af.bitvector    = 0;
        affect_to_char( ch, &af );
    }
    else
    {
        act( "$n fails to invoke the power of $s God!",  ch, NULL, NULL, TO_ROOM );
        send_to_char( "You fail to summon a Divine Blessing for Strength!\n\r", ch );
        check_improve(ch,gsn_divine_strength,FALSE,1);
    }
return;
}

/* void do_divine_strength(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if (get_skill(ch,gsn_divine_strength) == 0)
    {
    send_to_char( "Huh?\n\r", ch );
    return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
    send_to_char( "You aren't fighting anyone.\n\r", ch );
    return;
    }
        if ( get_skill(ch,gsn_divine_strength) > number_percent())
    {
        act( "$n invokes the power of their God!",  ch, NULL, NULL, TO_ROOM );
        send_to_char( "You pray to your God to give you strength to smite your foes.\n\r", ch );
        check_improve(ch,gsn_divine_strength,TRUE,1);
        do_function(ch, &do_disarm, "");
        do_function(ch, &do_kick, "");
        do_function(ch, &do_kick, "");
        do_function(ch, &do_trip, "");
        do_function(ch, &do_trip, "");
    }
    else
    {
        act( "$n fails to invoke the power of their God!",  ch, NULL, NULL, TO_ROOM );
        send_to_char( "You fail to summon a Divine Blessing for Strength!\n\r", ch );
        check_improve(ch,gsn_assassinate,FALSE,1);
    }
return;
} */
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
        check_improve(ch,gsn_critical_strike,TRUE,6);
        return TRUE;
}

bool IN_LEVEL( CHAR_DATA *ch, CHAR_DATA *victim )
{
/*
                if(IS_IMMORTAL(ch))
                        return TRUE;
                if (IS_SET(victim->act,PLR_ARENA))
                        return TRUE;
                if(victim->level < ch->level-20)
                        return FALSE;
                if(victim->level > ch->level+20)
                        return FALSE;
*/
        return TRUE;
}

void do_ground( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

        if ( !IS_NPC(ch)
        &&   ch->level < skill_table[gsn_ground].skill_level[ch->class] )
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
        send_to_char("You crash into them, but they simply tumble backwards and land upright again!\n\r", ch);
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

        if ( ( victim = get_char_room( ch, argument ) ) == NULL )
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

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->pcdata->csabilities[CSABIL_BRAWL], 6);

        WAIT_STATE( ch, skill_table[gsn_headbutt].beats );

    if(dicesuccess < 0)
    {
        act("$N nimbly sidesteps your headbutt, causing you to faceplant unceromoniously.", ch, NULL, victim, TO_CHAR);
        act("Moving with precise timing, you sidestep and avoid $n's attempt to introduce $s skull to your forehead.", ch, NULL, victim, TO_VICT);
        act("Throwing $mself forward, $n faceplants and lies on the ground, dazed.", ch, NULL, victim, TO_NOTVICT);
        ch->position = POS_RESTING;
        WAIT_STATE(ch, 4);
        damage(ch, ch, ch->level, gsn_headbutt, DAM_BASH, TRUE);
        check_improve(ch,gsn_headbutt,FALSE,1);
        return;
    }

    if(dicesuccess == 0)
    {
        act("You miss your headbutt!", ch, NULL, victim, TO_CHAR);
        act("$n tries to headbutt you and misses!", ch, NULL, victim, TO_VICT);
        act("$n tries to headbutt $N.", ch, NULL, victim, TO_NOTVICT);
        check_improve(ch,gsn_headbutt,FALSE,1);
        return;
    }

    damagesuccess = godice(get_attribute(ch, STRENGTH) + ch->pcdata->discipline[POTENCE], 6);
    if (damagesuccess < 0)
        damagesuccess = 0;

        act( "You slam your head into $N!", ch, NULL, victim, TO_CHAR );
        act( "$n slams $s head into yours!", ch, NULL, victim, TO_VICT );
        act( "$n slams $s head into $N's.", ch, NULL, victim, TO_ROOM );

    gain_exp(ch, dicesuccess);

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
        check_improve(ch,gsn_headbutt,FALSE,5);

        victim->position = POS_SLEEPING;
    }
    else  check_improve(ch,gsn_headbutt,TRUE,2);

    return;
}

void do_blast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
/*    int chance; */
    int dicesuccess;

        if (IS_NPC(ch))
                return;

        if (get_skill(ch,gsn_blast) == 0)
        {
                send_to_char("You have no idea how to do this!\n\r", ch );
                return;
        }

        if ( ( victim = get_char_room( ch, argument ) ) == NULL )
        {
                send_to_char( "Noone here to blast!\n\r", ch );
                return;
        }

        if (ch == victim)
        {
                send_to_char("Blast yourself? Odd...\n\r", ch );
                return;
        }

    if (ch->mana < ch->level / 2)
    {
        send_to_char("You don't have the energy to spend on that right now.\n\r", ch);
        return;
    }
    ch->mana -= ch->level / 2;
/*
        chance = number_percent();
        if(get_curr_stat(ch,STAT_INT) > get_curr_stat(victim,STAT_INT))
                chance += (get_curr_stat(ch,STAT_INT) - get_curr_stat(ch,STAT_INT));
        if(ch->level >= victim->level)
                chance += (ch->level - victim->level)/2;
        else
                chance -= (victim->level - ch->level);
*/


        WAIT_STATE( ch, skill_table[gsn_blast].beats );

    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->pcdata->csabilities[CSABIL_SUBTERFUGE], 6);

        if(dicesuccess < 0)
        {

                act( "You cough and gag as your blast misfires, blinding yourself briefly.", ch, NULL, victim, TO_CHAR );
                act( "$n coughs and gags as smoke fills $s eyes.", ch, NULL, victim, TO_VICT );
                act( "$n coughs and gags as smoke fills $s eyes.", ch, NULL, victim, TO_NOTVICT );

        damage(ch,ch,ch->level/2,gsn_blast,DAM_FIRE,TRUE);
        fire_effect(ch,ch->level,ch->level,TARGET_CHAR);
        return;

/*                damage(ch,victim,number_range( ch->level+1, ch->level*3 + 3 ), gsn_blast,DAM_FIRE,TRUE);
                fire_effect(victim,ch->level/2,number_range(1, ch->level+5),TARGET_CHAR);
                check_improve(ch,gsn_blast,TRUE,2);
*/
        }

        else if (dicesuccess == 0)
        {

                act( "Your blast misses $N!", ch, NULL, victim, TO_CHAR );
                act( "$n sends a blast of flame and smoke that misses you.", ch, NULL, victim, TO_VICT );
                act( "$n misses with $s blast of flame that was aimed at $N.", ch, NULL, victim, TO_ROOM );
        return;
        }

    act("Using a bit of misdirection and legardemain, you send a blast of {Ds{wmok{De{x\n\rand {rf{yl{ra{ym{re{x directly at $N!", ch, NULL, victim, TO_CHAR);
    act("$n sends a blast of smoke and flame directly at you!", ch, NULL, victim, TO_VICT);
    act("$n sends a quick burst of smoke and flame towards $N.", ch, NULL, victim, TO_NOTVICT);

    damage(ch, victim, dicesuccess * ch->level * 3 / 5, gsn_blast, DAM_FIRE, TRUE);
    fire_effect(victim, ch->level/2, number_range(1, ch->level+5), TARGET_CHAR);

    gain_exp(ch, dicesuccess);
    return;
}

void do_slip( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    int chance;

        if (IS_NPC(ch))
                return;

        if (get_skill(ch,gsn_slip) == 0)
        {
                send_to_char("You have no idea how to do this!\n\r", ch );
                return;
        }

        if(ch->position != POS_FIGHTING)
        {
                send_to_char("Your not fighting!\n\r", ch );
                return;
        }

        if ( ( victim = ch->fighting ) == NULL )
        {
                send_to_char( "Noone to slip away from!\n\r", ch );
                return;
        }

        chance = number_percent();
        if(get_curr_stat(ch,STAT_DEX) > get_curr_stat(victim,STAT_INT))
                chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(ch,STAT_INT));
        if(get_curr_stat(ch,STAT_DEX) > get_curr_stat(victim,STAT_WIS))
                chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(ch,STAT_WIS));
        if(get_curr_stat(ch,STAT_DEX) > get_curr_stat(victim,STAT_DEX))
                chance += (get_curr_stat(ch,STAT_DEX) - get_curr_stat(ch,STAT_DEX));
        if(ch->level >= victim->level)
                chance += (ch->level - victim->level)/2;
        else
                chance -= (victim->level - ch->level);

        WAIT_STATE( ch, skill_table[gsn_slip].beats );

        if(chance <= get_skill(ch,gsn_slip))
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

        if ( !IS_NPC(ch)
        &&   ch->level < skill_table[gsn_warcry].skill_level[ch->class] )
        {
                send_to_char("You are better off sticking to your own arts.\n\r", ch );
                return;
        }

        WAIT_STATE( ch, skill_table[gsn_warcry].beats );
        if ( get_skill(ch,gsn_warcry) > number_percent())
        {

                act( "You bellow a loud warcry!",  ch, NULL, NULL, TO_CHAR    );
                ch->move -= ch->level / 2;
                check_improve(ch,gsn_warcry,TRUE,2);
                for ( victim = char_list; victim != NULL; victim = vict_next )
                {
                        vict_next       = victim->next;
                        if(!IS_NPC(victim))
                            continue;
                        if ( victim->in_room == NULL )
                                continue;
                        if ( victim->in_room == ch->in_room )
                        {
                                if ( victim != ch && !is_safe_spell(ch,victim,TRUE))
                                {
                                        damage( ch,victim,ch->level, gsn_warcry, DAM_SOUND,TRUE);
                                        act( "$n bellows a loud warcry that hurts your ears!", ch, NULL, victim, TO_ROOM    );
                                }
                                continue;
                        }
                }
        }
        else
        {
                act( "You let out a squeak of a warcry!",  ch, NULL, NULL, TO_CHAR    );
                act( "$n attempts a mighty warcry, and succeeds only in letting out a tiny squeak.", ch, NULL, NULL, TO_ROOM    );
                check_improve(ch,gsn_warcry,FALSE,2);
        }
        return;
}

/* Sengir with Celerity, had to have here to use one_hit() properly*/
/* Disabled due to players wanting a buffing ability. Left here because its good work. - Ugha
void do_celerity(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int celattack;

    if (IS_NPC(ch))
        return;

    if (ch->fighting == NULL )
    {
        send_to_char("You aren't fighting anyone!\n\r", ch);
        return;
    }

    victim = ch->fighting;

    if (!IS_VAMP(ch))
    {
        send_to_char("You do not have Kindred blood in your veins!\n\r", ch);
        return;
    }

    if (ch->pcdata->discipline[CELERITY] == 0)
    {
        send_to_char("You have not begun studying the metahuman speed of Celerity.\n\r", ch);
        return;
    }

    if (ch->pblood < (ch->pcdata->discipline[CELERITY] * 2) + 2)
    {
        send_to_char("You are too low on vampiric vitae to fuel that Discipline!\n\r", ch);
        return;
    }

    ch->pblood -= ch->pcdata->discipline[CELERITY] * 2;

    act("Drawing upon the latent power of your {rVi{Rt{rae{x, you lash out and attack $N repeatedly.", ch, NULL, victim, TO_CHAR);
    act("$n suddenly lashes out at you with a flurry of swift attacks.", ch, NULL, victim, TO_VICT);
    act("$n strikes out swiftly at $N, attacking repeatedly.", ch, NULL, victim, TO_NOTVICT);

    WAIT_STATE(ch, 12);

    for(celattack = 1; celattack <= ch->pcdata->discipline[CELERITY]; celattack++)
    {
        one_hit( ch, victim, TYPE_UNDEFINED );
    }

    return;
}
*/

/*Sengir adding new beheading code*/
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
        send_to_char("Whom deserves to lose their head?\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg)) == NULL)
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

    name        = IS_NPC(ch) ? victim->short_descr : victim->name;
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

    REMOVE_BIT(ch->parts, PART_HEAD);

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

    dicesuccess = godice(get_attribute(ch, CHARISMA) + ch->pcdata->csabilities[CSABIL_LEADERSHIP], 7);

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
  //af.duration  = (dicesuccess * ch->pcdata->csbackgrounds[CSBACK_ALLIES]) + 10; // Old duration code
  // Removed it - should be replaced with an auto equipment drop and allies dismiss, not just charm wearing off
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
