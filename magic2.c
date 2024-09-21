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
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"


extern char *target_name;

void spell_farsight( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    if (IS_AFFECTED(ch,AFF_BLIND))
    {
        send_to_char("Maybe it would help if you could see?\n\r",ch);
        return;
    }

    do_function(ch, &do_scan, target_name);
}


void spell_portal( int sn, int level, CHAR_DATA *ch, void *vo,int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   victim->in_room == NULL
    ||   !can_see_room(ch,victim->in_room)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   victim->level >= level
/*    ||   (!IS_NPC(victim) && victim->level >= LEVEL_HERO) */ /* NOT trust */
    ||   (IS_SET(victim->imm_flags,IMM_SUMMON))
    ||   (IS_NPC(victim) && saves_spell( level, victim,DAM_NONE) ))
/*    ||	(is_clan(victim) && !is_same_clan(ch,victim))) */
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char(ch,WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
	send_to_char("You lack the proper component for this spell.\n\r",ch);
	return;
    }

    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
     	act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
     	act("It flares brightly and vanishes!",ch,stone,NULL,TO_CHAR);
     	extract_obj(stone);
    }

    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 3;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room(portal,ch->in_room);

    act("$n opens a hole in the fabric of space and time, thus creating $p.",ch,portal,NULL,TO_ROOM);
    act("You create a hole in space and time then form it into $p.",ch,portal,NULL,TO_CHAR);
}

void spell_nexus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    CHAR_DATA *victim;
    OBJ_DATA *portal, *stone;
    ROOM_INDEX_DATA *to_room, *from_room;

    from_room = ch->in_room;

        if ( ( victim = get_char_world( ch, target_name ) ) == NULL
    ||   victim == ch
    ||   (to_room = victim->in_room) == NULL
    ||   !can_see_room(ch,to_room) || !can_see_room(ch,from_room)
    ||   IS_SET(to_room->room_flags, ROOM_SAFE)
    ||	 IS_SET(from_room->room_flags,ROOM_SAFE)
    ||   IS_SET(to_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(to_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(to_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(from_room->room_flags,ROOM_NO_RECALL)
    ||   victim->level >= level + 3
    ||   (!IS_NPC(victim) && victim->level >= LEVEL_HERO)  /* NOT trust */
    ||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
    ||   (IS_NPC(victim) && saves_spell( level, victim,DAM_NONE) )
    ||	 (is_clan(victim) && !is_same_clan(ch,victim)))
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    stone = get_eq_char(ch,WEAR_HOLD);
    if (!IS_IMMORTAL(ch)
    &&  (stone == NULL || stone->item_type != ITEM_WARP_STONE))
    {
        send_to_char("You lack the proper component for this spell.\n\r",ch);
        return;
    }

    if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
    {
        act("You draw upon the power of $p.",ch,stone,NULL,TO_CHAR);
        act("It flares brightly and vanishes!",ch,stone,NULL,TO_CHAR);
        extract_obj(stone);
    }

    /* portal one */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 1 + level / 10;
    portal->value[3] = to_room->vnum;

    obj_to_room(portal,from_room);

    act("$p rises up from the ground.",ch,portal,NULL,TO_ROOM);
    act("$p rises up before you.",ch,portal,NULL,TO_CHAR);

    /* no second portal if rooms are the same */
    if (to_room == from_room)
	return;

    /* portal two */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 1 + level/10;
    portal->value[3] = from_room->vnum;

    obj_to_room(portal,to_room);

    if (to_room->people != NULL)
    {
	act("$p rises up from the ground.",to_room->people,portal,NULL,TO_ROOM);
	act("$p rises up from the ground.",to_room->people,portal,NULL,TO_CHAR);
    }
}

void spell_elemental( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You already embrace the Elements.\n\r",ch);
	return;
    }
    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_FIRE;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_COLD;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_LIGHTNING;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_ACID;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = ch->level/2;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_ENERGY;
    affect_to_char( ch, &af );

    act("$n says a small prayer to the elements and is granted their protection.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You utter a small prayer to the elements and you feel their blessing within you.\n\r", ch );
    return;
}

void spell_fireshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You are already surrounded by burning flames.\n\r",ch);
	return;
    }
    af.where	 = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF2_FIRESHIELD;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_FIRE;
    affect_to_char( ch, &af );

    act("$n says a prayer to the elemental powers of fire.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You utter a prayer to the elemental powers of fire.\n\r", ch );
    return;
}

void spell_holyshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You are already surrounded by a Radiant Aura.\n\r",ch);
	return;
    }
    af.where	 = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = number_range(3, 8);
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF2_HOLYSHIELD;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_HOLY;
    affect_to_char( ch, &af );

    act("$n is granted a {WRadiant Aura{x by $s god.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You are blessed with a {WRadiant Aura{x by your god!\n\r", ch );
    return;
}

void spell_darkshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You are already surrounded by a Darkened Aura.\n\r",ch);
	return;
    }
    af.where	 = TO_AFFECTS2;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF2_DARKSHIELD;
    affect_to_char( ch, &af );

    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_NEGATIVE;
    affect_to_char( ch, &af );

    act("$n is granted a {DDark Aura{x by the darkness within $s's soul.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You extend forth the darkness within your soul, creating a {DDark Aura{x.\n\r", ch );
    return;
}

void spell_shockshield( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You already have a static charge.\n\r",ch);
	return;
    }
    af.where	 = TO_RESIST;
    af.type      = sn;
    af.level	 = level;
    af.duration  = 3;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = RES_LIGHTNING;
    affect_to_char( ch, &af );

    act("$n suddenly crackles and sparks with a strong static charge!",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You pull static energy from your surroundings and encase yourself in a static field.\n\r", ch );
    return;
}

void spell_mana( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    int mana, num1, roll, success = 0;

	if(IS_NPC(ch)) return;

	for(num1 = (get_attribute(ch,INTELLIGENCE)+ch->csabilities[CSABIL_OCCULT])*2;num1 != 0;num1--)
	{
		roll = number_range(1,10);
		if(roll >= 6) success++;
		else if(roll == 1) success--;
	}

	if(success > 0)
		mana = success*level;
	else
		mana = 100;

	ch->mana += mana;
	if(ch->mana > ch->max_mana) ch->mana = ch->max_mana;

    send_to_char( "You feel strange energies rushing through your body!\n\r", ch );
    return;
}

void spell_blood( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

    if(ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah"))
    {
    	if(ch->race == race_lookup("ghoul"))
    		send_to_char("You gag and spit out the blood out.. It's not vampiric vitae!\n\r",ch);
        if(ch->race == race_lookup("human"))
            send_to_char("You suddenly realize you're DRINKING BLOOD! And quickly spit it out.\n\r",ch);
        if(ch->race == race_lookup("dhampire"))
            send_to_char("You savor the taste of sweet blood then suddenly spit it out as you fight your vampiric nature.\n\r",ch);
        if(ch->race == race_lookup("garou"))
            send_to_char("While the taste of blood in combat is enjoyable.. this is pitiful. You spit it out.\n\r",ch);
        act("$n gags and spits out the blood.",ch,NULL,NULL,TO_ROOM);
        return;
	}
    ch->pblood += level*2;
    if(ch->pblood > ch->max_pblood)
        ch->pblood = ch->max_pblood;

    send_to_char( "You gulp down the blood with great pleasure.\n\r", ch );
    if (is_affected(ch, gsn_vamp_frenzy))
    {
      affect_strip(ch, gsn_vamp_frenzy);
      send_to_char("The sweet taste of blood satiates your frenzied hunger.\n\r", ch);
    }

    return;
}
void spell_vampire_blood( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{

    if(!IS_VAMP(ch))
    {
        if(ch->race == race_lookup("human"))
            send_to_char("You suddenly realize you're DRINKING BLOOD! And quickly spit it out.\n\r",ch);
        if(ch->race == race_lookup("dhampire"))
            send_to_char("You savor the taste of sweet blood then suddenly spit it out as you fight your vampiric nature.\n\r",ch);
        if(ch->race == race_lookup("garou"))
            send_to_char("While the taste of blood in combat is enjoyable.. this is pitiful. You spit it out.\n\r",ch);
        act("$n gags and spits out the blood.",ch,NULL,NULL,TO_ROOM);
        return;
	}

    ch->pblood += level;
    if(ch->pblood > ch->max_pblood)
        ch->pblood = ch->max_pblood;

    send_to_char( "You gulp down the blood with great pleasure.\n\r", ch );
    if (is_affected(ch, gsn_vamp_frenzy))
    {
      affect_strip(ch, gsn_vamp_frenzy);
      send_to_char("The sweet taste of {rVitae{x satiates your frenzied hunger.\n\r", ch);
    }
    return;
}

void spell_change_alignment( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    AFFECT_DATA af;

    if ( is_affected( victim, sn ))
    {
	if (victim == ch)
	  send_to_char("You've already been changed.\n\r",ch);
	else
	  act("$N's aura has already been altered.",ch,NULL,victim,TO_CHAR);
	return;
    }
    if (saves_spell(level , victim,DAM_OTHER))
	return;
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 2 * level;
    af.location  = APPLY_ALIGN;
    af.modifier  = number_range( 0, 2000 )-1000;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    send_to_char( "The energies of your soul seem to shift...\n\r", victim );
    act("$n's aura seems to shift...",victim,NULL,NULL,TO_ROOM);
    return;
}
void spell_zeal( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, sn ) )
    {
	  send_to_char("You already have the Zest of God within you.\n\r",ch);
	return;
    }
    if(level >= 20)
	spell_cure_light(skill_lookup("cure light"),level,ch,ch,TARGET_CHAR);
    if(level >= 40)
	spell_cure_light(skill_lookup("cure light"),level,ch,ch,TARGET_CHAR);
    if(level >= 60)
	spell_cure_light(skill_lookup("cure light"),level,ch,ch,TARGET_CHAR);
    if(level >= 80)
	spell_cure_light(skill_lookup("cure light"),level,ch,ch,TARGET_CHAR);
    if(level >= 100)
	spell_cure_light(skill_lookup("cure light"),level,ch,ch,TARGET_CHAR);


    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = level/4;
    af.modifier  = level/4;
    af.location  = APPLY_DAMROLL;
    if(level >= 80)
	af.bitvector = AFF_HASTE;
    else
	af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char( "You feel the fervor of God flowing through your veins!.\n\r", ch );
    return;
}

void spell_intervention( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
    ROOM_INDEX_DATA *location;
    AFFECT_DATA af;

    if ( is_affected( ch, sn ))
    {
	  send_to_char("You have had a devine intervention too recently to have another.\n\r",ch);
	return;
    }
    if ((location = get_room_index( ROOM_VNUM_TEMPLE)) == NULL)
    {
	send_to_char("You are completely lost.\n\r",ch);
	return;
    }
    affect_strip(ch,gsn_poison);
    affect_strip(ch,gsn_blindness);
    affect_strip(ch,gsn_sleep);
    affect_strip(ch,gsn_curse);
    affect_strip(ch,gsn_vamp_frenzy);
    affect_strip(ch,gsn_garou_frenzy);

    ch->hit = ch->max_hit;
    ch->mana = 0;
    ch->move = 0;
    if (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah") || ch->race == race_lookup("dhampire"))
	ch->pblood += 10;

    char_from_room(ch);
    char_to_room(ch,location);
    do_function(ch, &do_look, "auto");

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = 168;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char( "A great rumble fills the earth as the Hand of God sweeps from the heavens to deliver you to safety.\n\r", ch );
    act("The earth trembles and the skies shimmer as the Hand of God delivers $n from danger.",ch,NULL,NULL,TO_ROOM);
    return;
}

void spell_wrath_of_god( int sn, int level, CHAR_DATA *ch, void *vo,int target )
{
	CHAR_DATA *tmp;
    CHAR_DATA *victim = (CHAR_DATA *) vo;

    if(ch->level < victim->level)
	{
		send_to_char("I don't THINK so!\n\r",ch);
		tmp = victim;
		victim = ch;
		ch = tmp;
	}

	victim->hit = 1;
    damage( ch, victim, 15000, sn, DAM_FIRE ,TRUE);
    return;
}
