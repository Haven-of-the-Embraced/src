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
#include "merc.h"
#include "interp.h"
#include "music.h"
#include "recycle.h"
#include "tables.h"

/*
 * Local functions.
 */
int hit_gain    args( ( CHAR_DATA *ch ) );
int mana_gain   args( ( CHAR_DATA *ch ) );
int move_gain   args( ( CHAR_DATA *ch ) );
void    mobile_update   args( ( void ) );
void    weather_update  args( ( void ) );
void    char_update args( ( void ) );
void    obj_update  args( ( void ) );
void    aggr_update args( ( void ) );
void    bank_offline_update args( ( void ) );
void    bank_update args( (void) );
void    quest_update    args( ( void ) ); /* Vassago - quest.c */
void    underwater_update    args( ( void ) ); /* Drowning stuff - drown.c */

/* used for saving */

int save_number = 0;

// Moved to global scope - Matthew
int     pulse_area;
int     pulse_mobile;
int     pulse_violence;
int     pulse_point;
int     pulse_music;
int     pulse_influence;
int     pulse_influence_fade;
// Affects pulse for over time effects
int pulse_affects;
int pulse_regen;
//int     pulse_bank_offline;
int     pulse_bank;
int     pulse_weather;
int pulse_underwater;



/*
 * Advancement stuff.
 */
void advance_level( CHAR_DATA *ch, bool hide )
{
    char buf[MAX_STRING_LENGTH];
    int add_hp;
    int add_mana;
    int add_move;
    int add_prac;

    ch->pcdata->last_level =
    ( ch->played + (int) (current_time - ch->logon) ) / 3600;

    add_hp      = dice(8, 3) + 6;
    add_mana    = dice(4, 3) + 4;
    add_move    = dice(4, 3) + 4;

    add_hp  = UMAX(  2, add_hp   );
    add_mana    = UMAX(  2, add_mana );
    add_move    = UMAX(  6, add_move );

    if(ch->sphere[SPHERE_LIFE] > 0)
    {
        add_hp += ch->sphere[SPHERE_LIFE]*2;
        add_move += ch->sphere[SPHERE_LIFE];
    }
    ch->max_hit     += add_hp;
    ch->max_mana    += add_mana;
    ch->max_move    += add_move;

    ch->pcdata->perm_hit    += add_hp;
    ch->pcdata->perm_mana   += add_mana;
    ch->pcdata->perm_move   += add_move;

    if (!hide)
        cprintf(ch,
        "You gain {R%d{r/%d{x hit point%s, {M%d{m/%d{x mana, and {G%d{g/%d{x movement.\n\r",
        add_hp, ch->max_hit, add_hp == 1 ? "" : "s", add_mana, ch->max_mana, add_move, ch->max_move);
    return;
}



void gain_exp( CHAR_DATA *ch, int gain )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) || ch->level >= LEVEL_HERO || ch->act == PLR_ARENA)
    return;

    ch->exp = UMAX( exp_per_level(ch,ch->pcdata->points), ch->exp + gain );
    while ( ch->level < LEVEL_HERO && ch->exp >=
    exp_per_level(ch,ch->pcdata->points) * (ch->level+1) )
    {
    send_to_char( "{YYou raise a level!!  {x", ch );
    ch->level += 1;
    if(ch->level == 50 || ch->level == 100)
    announce(ch, NULL, WIZ_LEVELS);

    sprintf(buf,"%s gained level %d",ch->name,ch->level);
    log_string(buf);
    sprintf(buf,"{r$N {yhas just attained level {b%d{x!",ch->level);
    wiznet(buf,ch,NULL,WIZ_LEVELS,0,0);
    advance_level(ch,FALSE);
    save_char_obj(ch);
    }

    return;
}



/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;

    if(!IS_NPC(ch) && (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")))
    return 0;

    if ( IS_NPC(ch) )
    {
    gain =  5 + ch->level;
    if (IS_AFFECTED(ch,AFF_REGENERATION))
        gain *= 2;

    switch(ch->position)
    {
        default :       gain /= 2;          break;
        case POS_SLEEPING:  gain = 3 * gain/2;      break;
        case POS_RESTING:                   break;
        case POS_FIGHTING:  gain /= 3;          break;
    }


    }
    else
    {
 //   gain = UMAX(5, get_attribute(ch, STAMINA) * 5 + ch->level/2);
    gain = UMAX(5, (get_attribute(ch, STAMINA) * 2) * ch->max_hit/100);

    gain += (ch->pcdata->condition[COND_HUNGER] + ch->pcdata->condition[COND_THIRST])/2;

    number = number_percent();
    if (number < get_skill(ch,gsn_fast_healing))
    {
        gain += number * gain / 100;
        if (ch->hit < ch->max_hit)
        check_improve(ch,gsn_fast_healing,TRUE,8);
    }

    /*  doubling it to make up for agg */
    gain += gain;

    switch ( ch->position )
    {
        default:        gain /= 4;          break;
        case POS_SLEEPING:                  break;
        case POS_RESTING:   gain /= 2;          break;
        case POS_FIGHTING:  gain /= 6;          break;
    }

    }

    gain = gain * ch->in_room->heal_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[3] / 100;

    if ( IS_AFFECTED(ch, AFF_POISON) )
    gain /= 4;

    if (IS_AFFECTED(ch,AFF_HASTE))
    gain /=2;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
    gain /=2;


        if( get_affect_level(ch, gsn_bandage) >= 2)
            gain += gain;
        if( get_affect_level(ch, gsn_bandage) >= 5 && ch->agg_dam > 0)
            ch->agg_dam--;

    return UMIN(gain, ch->max_hit - ch->hit);
}



int mana_gain( CHAR_DATA *ch )
{
    int gain;
    int number;

    if (ch->in_room == NULL)
    return 0;

    if ( IS_NPC(ch) )
    {
    gain = 5 + ch->level;
    switch (ch->position)
    {
        default:        gain /= 2;      break;
        case POS_SLEEPING:  gain = 3 * gain/2;  break;
        case POS_RESTING:               break;
        case POS_FIGHTING:  gain /= 3;      break;
        }
    }
    else
    {
/* Sengir changing mana gain to CS based
    gain = (get_curr_stat(ch,STAT_WIS)
          + get_curr_stat(ch,STAT_INT) + ch->level) / 2;
   ----See below for new calcs--- */
    gain = (((get_attribute(ch, INTELLIGENCE) * 10) + ch->level ) / 2) + 5;
    number = number_percent();
    if (number < get_skill(ch,gsn_meditation))
    {
        gain += number * gain / 100;
        if (ch->mana < ch->max_mana)
            check_improve(ch,gsn_meditation,TRUE,8);
    }
    if (is_affected( ch, gsn_chant))
        gain += (ch->level / 10) + number_range(number_range(1, get_skill(ch,gsn_chant)), 100);

    if (!class_table[ch->class].fMana)
        gain /= 2;

    switch ( ch->position )
    {
        default:        gain = gain;                break;
        case POS_SLEEPING:  gain += gain / 2;       break;
        case POS_RESTING:   gain += gain / 5;       break;
        case POS_FIGHTING:  gain /= 4;          break;
    }

    if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
        gain /= 2;

    if ( ch->pcdata->condition[COND_THIRST] == 0 )
        gain /= 2;

    }

    gain = gain * ch->in_room->mana_rate / 100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[4] / 100;

    if ( IS_AFFECTED( ch, AFF_POISON ) )
    gain /= 4;

/*    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8; */

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        gain /=2 ;

    return UMIN(gain, ch->max_mana - ch->mana);
}



int move_gain( CHAR_DATA *ch )
{
    int gain;

    if (ch->in_room == NULL)
    return 0;

    if ( IS_NPC(ch) )
    {
    gain = ch->level;
    }
    else
    {
    gain = UMAX( 20, ch->level );

    switch ( ch->position )
    {
    case POS_SLEEPING: gain += get_attribute(ch, STAMINA) * 5;      break;
    case POS_RESTING:  gain += (get_attribute(ch, STAMINA) * 5) / 2;  break;
    }

    if ( ch->pcdata->condition[COND_HUNGER]   == 0 )
        gain /= 2;

    if ( ch->pcdata->condition[COND_THIRST] == 0 )
        gain /= 2;
    }

    gain = gain * ch->in_room->heal_rate/100;

    if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[3] / 100;

    if ( IS_AFFECTED(ch, AFF_POISON) )
    gain /= 4;

/*    if (IS_AFFECTED(ch, AFF_PLAGUE))
        gain /= 8; */

    if (IS_AFFECTED(ch,AFF_HASTE) || IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        gain /=2 ;

    return UMIN(gain, ch->max_move - ch->move);
}



void gain_condition( CHAR_DATA *ch, int iCond, int value )
{
    int condition;

    if ( value == 0 || IS_NPC(ch) || ch->level >= LEVEL_IMMORTAL)
    return;

    condition = ch->pcdata->condition[iCond];
    if (condition == -1)
        return;
    ch->pcdata->condition[iCond]    = URANGE( 0, condition + value, 48 );

    if ( ch->pcdata->condition[iCond] == 0 )
    {
    switch ( iCond )
    {
    case COND_HUNGER:
        if(!IS_SET(ch->act,PLR_IC))
        {
            if(!IS_NEWBIE(ch)) ch->agg_dam++;
			if (!IS_SET(ch->act2, PLR2_NOCONDITION))
            send_to_char( "Pangs of hunger rumble through your stomach.\n\r",  ch );
        }
        break;

    case COND_THIRST:
        if(!IS_SET(ch->act,PLR_IC))
        {
            if(!IS_NEWBIE(ch)) ch->agg_dam++;
			if (!IS_SET(ch->act2, PLR2_NOCONDITION))
            send_to_char( "Your parched lips are in need of refreshment.\n\r", ch );
        }
        break;

    case COND_DRUNK:
        if ( condition != 0 && !IS_SET(ch->act2, PLR2_NOCONDITION))
        send_to_char( "You are sober.\n\r", ch );
        break;
    }
    }

    return;
}



/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    EXIT_DATA *pexit;
    int door;

    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    ch_next = ch->next;

    if ( !IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch,AFF_CHARM))
        continue;

    if (ch->in_room->area->empty && !IS_SET(ch->act,ACT_UPDATE_ALWAYS))
        continue;

    /* Examine call for special procedure */
    if ( ch->spec_fun != 0 )
    {
        if ( (*ch->spec_fun) ( ch ) )
        continue;
    }

    if (ch->pIndexData->pShop != NULL) /* give him some gold */
        if ((ch->gold * 100 + ch->silver) < ch->pIndexData->wealth)
        {
        ch->gold += ch->pIndexData->wealth * number_range(1,20)/5000000;
        ch->silver += ch->pIndexData->wealth * number_range(1,20)/50000;
        }

    /*
     * Check triggers only if mobile still in default position
     */
    if ( ch->position == ch->pIndexData->default_pos )
    {
        /* Delay */
        if ( HAS_TRIGGER( ch, TRIG_DELAY)
        &&   ch->mprog_delay > 0 )
        {
        if ( --ch->mprog_delay <= 0 )
        {
            mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_DELAY );
            continue;
        }
        }
        if ( HAS_TRIGGER( ch, TRIG_RANDOM) )
        {
        if( mp_percent_trigger( ch, NULL, NULL, NULL, TRIG_RANDOM ) )
        continue;
        }
    }

    /* That's all for sleeping / busy monster, and empty zones */
    if ( ch->position != POS_STANDING )
        continue;

    /* Scavenge */
    if ( IS_SET(ch->act, ACT_SCAVENGER)
    &&   ch->in_room->contents != NULL
    &&   number_bits( 6 ) == 0 )
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_best;
        int max;

        max         = 1;
        obj_best    = 0;
        for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
        {
        if ( CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj) && !count_users(obj)
             && obj->cost > max  && obj->cost > 0)
        {
            obj_best    = obj;
            max         = obj->cost;
        }
        }

        if ( obj_best )
        {
        obj_from_room( obj_best );
        obj_to_char( obj_best, ch );
        act( "$n gets $p.", ch, obj_best, NULL, TO_ROOM );
        }
    }

    /* Wander */
    if ( !IS_SET(ch->act, ACT_SENTINEL)
    && number_bits(3) == 0
    && ( door = number_bits( 5 ) ) <= 5
    && ( pexit = ch->in_room->exit[door] ) != NULL
    &&   pexit->u1.to_room != NULL
    &&   !IS_SET(pexit->exit_info, EX_CLOSED)
    &&   !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB)
    && ( !IS_SET(ch->act, ACT_STAY_AREA)
    ||   pexit->u1.to_room->area == ch->in_room->area )
    && ( !IS_SET(ch->act, ACT_OUTDOORS)
    ||   !IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS))
    && ( !IS_SET(ch->act, ACT_INDOORS)
    ||   IS_SET(pexit->u1.to_room->room_flags,ROOM_INDOORS)))
    {
        move_char( ch, door, FALSE );
    }
    }

    return;
}



/*
 * Update the weather.
 */
void weather_update( void )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int diff;

    buf[0] = '\0';

    switch ( ++time_info.hour )
    {
    case  5:
    weather_info.sunlight = SUN_LIGHT;
    strcat( buf, "A soft amber glow gradually spreads its light across the eastern sky, foretelling the coming dawn.\n\r" );
    break;

    case  6:
    weather_info.sunlight = SUN_RISE;
    strcat( buf, "Cresting on the horizion, the sun makes its daily appearance, and chases away the terrors of the previous night.\n\r" );
    break;

   /* Jocelyn and Sengir Noontime message.*/

    case 12:
        weather_info.sunlight = HIGH_NOON;
        strcat( buf, "Reaching its apex, the sun radiates its heat and light across the land.\n\r" );
        break;


    case 19:
    weather_info.sunlight = SUN_SET;
    strcat( buf, "Slowly sinking from the sky, the sun dips below the western horizion, giving way to the night.\n\r" );
    break;

    case 20:
    weather_info.sunlight = SUN_DARK;
    strcat( buf, "An inky blackness menacingly creeps across the sky, bathing the land in eerie darkness.\n\r" );
    break;

    case 24:
    time_info.hour = 0;
    time_info.day++;
    time_info.moon_count++;
    if(time_info.phase == 0)
        strcat(buf,"Stars sparkle in the moonless sky above.\n\r");
    sprintf(buf, "The %s moon shines down upon the land.\n\r", time_info.phase == 1 ? "crescent" :
        time_info.phase == 2 ? "half" : time_info.phase == 3 ? "gibbous" : "full");
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   IS_OUTSIDE(d->character)
        &&   IS_AWAKE(d->character)
        && !IS_NPC(d->character)
        && d->character->pcdata->rank > 0)
        {
            d->character->pcdata->rage[TEMP]++;
            if(d->character->pcdata->auspice-1 == time_info.phase || d->character->pcdata->rage[TEMP] > d->character->pcdata->rage[PERM])
                d->character->pcdata->rage[TEMP] = d->character->pcdata->rage[PERM];
        }
    }
    break;
    }
/*
    if ( time_info.day   >= 35 )
    {
    time_info.day = 0;
    time_info.month++;
    }
*/
    if ( (time_info.month == 0 && time_info.day > 30) ||
        (time_info.month == 1 && time_info.day > 27 ) ||
        (time_info.month == 2 && time_info.day > 30 ) ||
        (time_info.month == 3 && time_info.day > 29 ) ||
        (time_info.month == 4 && time_info.day > 30 ) ||
        (time_info.month == 5 && time_info.day > 29 ) ||
        (time_info.month == 6 && time_info.day > 30 ) ||
        (time_info.month == 7 && time_info.day > 30 ) ||
        (time_info.month == 8 && time_info.day > 29 ) ||
        (time_info.month == 9 && time_info.day > 30 ) ||
        (time_info.month == 10 && time_info.day > 29 ) ||
        (time_info.month == 11 && time_info.day > 30 ))
    {
        time_info.day = 0;
        time_info.month++;
    }

    if ( time_info.month >= 12 )
    {
    time_info.month = 0;
    time_info.year++;
    }

    if ( time_info.moon_count == 3)
    {
        time_info.moon_count = 0;
        if(time_info.phase == 9) time_info.phase = 0;
        else
            time_info.phase++;
    }
    /*
     * Weather change.
     */
    if ( time_info.month >= 9 && time_info.month <= 16 )
    diff = weather_info.mmhg >  985 ? -2 : 2;
    else
    diff = weather_info.mmhg > 1015 ? -2 : 2;

    weather_info.change   += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
    weather_info.change    = UMAX(weather_info.change, -12);
    weather_info.change    = UMIN(weather_info.change,  12);

    weather_info.mmhg += weather_info.change;
    weather_info.mmhg  = UMAX(weather_info.mmhg,  960);
    weather_info.mmhg  = UMIN(weather_info.mmhg, 1040);

    switch ( weather_info.sky )
    {
    default:
    bug( "Weather_update: bad sky %d.", weather_info.sky );
    weather_info.sky = SKY_CLOUDLESS;
    break;

    case SKY_CLOUDLESS:
    if ( weather_info.mmhg <  990
    || ( weather_info.mmhg < 1010 && number_bits( 2 ) == 0 ) )
    {
        strcat( buf, "Dark rain clouds begin to roll in and block out the sun.\n\r" );
        weather_info.sky = SKY_CLOUDY;
    }
    break;

    case SKY_CLOUDY:
    if ( weather_info.mmhg <  970
    || ( weather_info.mmhg <  990 && number_bits( 2 ) == 0 ) )
    {
        strcat( buf, "Droplets of water begin bathing the land and washing away the dust and grime.\n\r" );
        weather_info.sky = SKY_RAINING;
    }

    if ( weather_info.mmhg > 1030 && number_bits( 2 ) == 0 )
    {
        strcat( buf, "As quickly as they appeared, the storm clouds abate.\n\r" );
        weather_info.sky = SKY_CLOUDLESS;
    }
    break;

    case SKY_RAINING:
    if ( weather_info.mmhg <  970 && number_bits( 2 ) == 0 )
    {
        strcat( buf, "Illuminating the sky, a jagged bolt of lightning strikes the earth.  Moments later a loud clap of thunder resonates through the sky.\n\r" );
        weather_info.sky = SKY_LIGHTNING;
    }

    if ( weather_info.mmhg > 1030
    || ( weather_info.mmhg > 1010 && number_bits( 2 ) == 0 ) )
    {
        strcat( buf, "The rainstorm lessens, allowing the ripples in the water puddles to slowly smooth out.\n\r" );
        weather_info.sky = SKY_CLOUDY;
    }
    break;

    case SKY_LIGHTNING:
    if ( weather_info.mmhg > 1010
    || ( weather_info.mmhg >  990 && number_bits( 2 ) == 0 ) )
    {
        strcat( buf, "With a brief flash, the lightning stops and the thunder rumbles off into the distance.\n\r" );
        weather_info.sky = SKY_RAINING;
        break;
    }
    break;
    }

    if ( buf[0] != '\0' )
    {
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   IS_OUTSIDE(d->character)
        &&   IS_AWAKE(d->character)
        &&   !IS_SET(d->character->act, PLR_IC))
        send_to_char( buf, d->character );
    }

/*  for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && IS_OUTSIDE(d->character) )
        {
            if ((weather_info.sky == SKY_LIGHTNING) && (number_percent() <= 10) )
            {
                send_to_char("You are struck by a bolt of {Ylightning{x!\n\r", d->character);
                damage(d->character, d->character, ch->level/2, sn, DAM_LIGHTNING, TRUE );
            }
        }
    }
*/
    }

    return;
}



/*
 * Update all chars, including mobs.
*/
void char_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_quit;
    char buf[MAX_STRING_LENGTH];
    extern bool nosun;
    extern bool doubleexp;
    int         qpgain;
    int         ichours;
    int         bqp;
    int         qpaward;
    ch_quit = NULL;

    /* update save counter */
    save_number++;

    if (save_number > 29)
    save_number = 0;

    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    ch_next = ch->next;

        if ( ch->timer > 45 && !IS_IMMORTAL(ch))
            ch_quit = ch;

    if ( ch->position >= POS_STUNNED )
    {
            /* check to see if we need to go home */
            if (IS_NPC(ch) && ch->zone != NULL && ch->zone != ch->in_room->area
            &&  ch->fighting == NULL
        && !IS_AFFECTED(ch,AFF_CHARM) && number_percent() < 60)
            {
                act("$n wanders on home.",ch,NULL,NULL,TO_ROOM);
                extract_char(ch,TRUE);
                continue;
            }
        if(IS_NPC(ch) && RIDDEN(ch) && get_char_room( ch, ch->mount->name ) == NULL)
            {
                act("$n takes this chance to bolt for freedom.",ch,NULL,NULL,TO_ROOM);
                extract_char(ch,TRUE);
                continue;
            }
        if(IS_NPC(ch) && ch->position != ch->default_pos)
        do_function(ch, &do_wake, "");

        if (ch->race != race_lookup("garou")) {
            if ( ch->hit  < ch->max_hit )
            ch->hit  += hit_gain(ch);
            else
            ch->hit = ch->max_hit;

            if ( ch->mana < ch->max_mana )
            ch->mana += mana_gain(ch);
            else
            ch->mana = ch->max_mana;

            if ( ch->move < ch->max_move )
            ch->move += move_gain(ch);
            else
            ch->move = ch->max_move;
        }
    }

    if ( ch->position == POS_STUNNED )
        update_pos( ch );

    if ( !IS_NPC(ch) /*&& ch->level < LEVEL_IMMORTAL*/ )
    {
        OBJ_DATA *obj;

        if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
        &&   obj->item_type == ITEM_LIGHT && !IS_IMMORTAL(ch)
        &&   obj->value[2] > 0 )
        {
        if ( --obj->value[2] == 0 && ch->in_room != NULL )
        {
            --ch->in_room->light;
            act( "$p casts its last bit of illumination before dimming to nothingness.", ch, obj, NULL, TO_ROOM );
            act( "$p flickers and sputters before finally going out.", ch, obj, NULL, TO_CHAR );
            extract_obj( obj );
        }
        else if ( obj->value[2] <= 5 && ch->in_room != NULL)
            act("$p flickers briefly, struggling to continue illuminating.",ch,obj,NULL,TO_CHAR);
        }

        //if (IS_IMMORTAL(ch))
        //ch->timer = 0;

        if ( ++ch->timer >= 30 && !IS_IMMORTAL(ch))
        {
        if ( ch->was_in_room == NULL && ch->in_room != NULL )
        {
            ch->was_in_room = ch->in_room;
            if ( ch->fighting != NULL )
            stop_fighting( ch, TRUE );
            act( "$n disappears, leaving a gaping void in $s wake.",
            ch, NULL, NULL, TO_ROOM );
            send_to_char( "With an overwhelming feeling of wanderlust, you decide to visit the Immortals in Limbo.\n\r", ch );
            if(IS_SET(ch->act,PLR_IC))
            {
                REMOVE_BIT(ch->act,PLR_IC);
                send_to_char("You are no longer {D({CIC{D){x!  Quit trying to break the buffer!\n\r",ch);
                wiznet("{D$N has voided while {W({CIC{W){D!{x",ch,NULL,WIZ_LINKS,0,0);
                ch->qpoints -= 10;
                send_to_char("You have just lost {R10{x quest points! This is a warning!\n\r",ch);
                send_to_char("Chronic voiding while IC will be met with further warnings and\n\r", ch);
                send_to_char("potential flagging for being excluded from the IC qp gain system.\n\r", ch);


            }
                wiznet("$N disappears into the void.",ch,NULL,WIZ_LINKS,0,0);

            if (ch->level > 1)
                save_char_obj( ch );
            char_from_room( ch );
            char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
        }
    }

/* New ifcheck code added by Sengir, if player is IC they no longer
   get hungrier/thirstier or lose fullness */
		if (!IS_SET(ch->act, PLR_IC) && !IS_IMMORTAL(ch))
		{
			if (ch->race == race_lookup("vampire"))
			gain_condition( ch, COND_FULL, -8 );
			else
		    gain_condition( ch, COND_FULL, -4 );
		}

		if(!IS_SET(ch->act, PLR_IC) && number_range(1, 3) == 2)
            {
            gain_condition( ch, COND_DRUNK,  -1 );
            gain_condition( ch, COND_THIRST, -1 );
            gain_condition( ch, COND_HUNGER, ch->size > SIZE_MEDIUM ? -2 : -1);
        }
        if(!IS_NPC(ch))
        {
            if(ch->race == race_lookup("garou"))
                ch->agg_dam -= 2*get_attribute(ch,STAMINA);
            else
                ch->agg_dam -= (get_attribute(ch,STAMINA)+ch->fortitude);
        }
        if(ch->hit > (ch->max_hit-ch->agg_dam)) ch->hit = (ch->max_hit - ch->agg_dam);
        if(ch->agg_dam < 0) ch->agg_dam = 0;
        if(ch->agg_dam == ch->max_hit) ch->agg_dam = (ch->max_hit-1);
    }

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
        paf_next    = paf->next;
        if ( paf->duration > 0 )
        {
        paf->duration--;
            }
        else if ( paf->duration < 0 )
        ;
        else
        {
        if ( paf_next == NULL
        ||   paf_next->type != paf->type
        ||   paf_next->duration > 0 )
        {
            if ( paf->type == gsn_chimaera)
            {
                act("$n begins to fade away... then winks out of existence.", ch, NULL, NULL, TO_ROOM);
                extract_char(ch, TRUE);
                break;
            }

            if ( paf->type > 0 && skill_table[paf->type].msg_off )
            {
            send_to_char( skill_table[paf->type].msg_off, ch );
            send_to_char( "\n\r", ch );
            }
        }

        affect_remove( ch, paf );
        }

        if (!ch) // Guard against null ch being returned from chimaera wearing off.
            continue;

if (ch->qpoints > 30000)
    ch->qpoints = 30000;
    }

    if(!IS_NPC(ch) && ch->in_room != NULL && !IS_IMMORTAL(ch) && ch->in_room->sector_type == SECT_WATER_DROWN && (ch->race != race_lookup("vampire") ||
    ch->race != race_lookup("methuselah")))
    {
        act( "$n chokes and gurgles.",ch, NULL, NULL, TO_ROOM );
        send_to_char( "You are drowning!\n\r",ch);
        damage(ch,ch,ch->max_hit/20,0,DAM_DROWNING,FALSE);
    }

    if(!IS_NPC(ch) && ch->in_room != NULL && ch->in_room->sector_type == SECT_HOT && !IS_IMMORTAL(ch))
    {
        act( "$n swelters under the intense heat.",ch, NULL, NULL, TO_ROOM );
        send_to_char( "You swelter under the intense heat!\n\r",ch);
        damage(ch,ch,ch->max_hit/50,0,DAM_FIRE,FALSE);
    }

    if(!IS_NPC(ch) && ch->in_room != NULL && ch->in_room->sector_type == SECT_COLD && !IS_IMMORTAL(ch))
    {
        act( "$n shivers from the cold.",ch, NULL, NULL, TO_ROOM );
        send_to_char( "You shiver from the intense cold.\n\r",ch);
        damage(ch,ch,ch->max_hit/50,0,DAM_COLD,FALSE);
    }

    if (is_affected(ch, gsn_agg_damage) && ch != NULL)
    {
        AFFECT_DATA *agg_damage;

        agg_damage = affect_find(ch->affected,gsn_agg_damage);

        if (agg_damage != NULL)
        {
            act( "$n screams in pain and cluches their wounds!", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You scream in pain from your injuries!\n\r", ch );
          if(ch->position != POS_TORPOR)
            damage(ch,ch,agg_damage->level,gsn_agg_damage, DAM_NEGATIVE,FALSE);
        }
    }
/*Sengir -  Characters gain WP every DAY*/
    if (!IS_NPC(ch))
    {
        if (ch->cswillpower < ch->csmax_willpower)
        {
            if ((ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")) && time_info.hour == 18)
            {
                ch->cswillpower++;
                send_to_char("The evening's transition to nighttime fills you with resolve and determination.\n\r", ch);
            }

            if (ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah") && time_info.hour == 6)
            {
                ch->cswillpower++;
                send_to_char("The new day fortifies your resolve and determination, ready to face what lies ahead.\n\r", ch);
            }
        }
    }


/*  New Mage Code */
    if(ch->race == race_lookup("mage") || clan_table[ch->clan].clan_type == TYPE_TRADITION)
    {
        if (ch->paradox > 0)
            if (number_range(1, 3) == 1)
                ch->paradox--;
        if (ch->quintessence+ch->paradox < ch->max_quintessence)
            ch->quintessence += ch->avatar*(3 + ch->pcdata->csbackgrounds[CSBACK_FOUNT]);
        if (ch->in_room->sector_type == SECT_NODE)
        {
            send_to_char("{MYour body siphons off a little of the mystical energy permeating the room.{x\n\r", ch);
            ch->quintessence += ch->avatar*6;
        }
        if(ch->quintessence+ch->paradox > ch->max_quintessence) ch->quintessence -= ((ch->quintessence+ch->paradox) - ch->max_quintessence);
    }

    if(ch->race == race_lookup("garou") && ch->changed != CHANGED_CRINOS && ch->rage > 0)
    {
        ch->rage -= 2;
        if(ch->rage < 0) ch->rage = 0;
    }
// Garou gnosis regen per tick.

    if(!IS_NPC(ch) && ch->race == race_lookup("garou"))
    {
        int diff, success, gain;

        if (IS_AFFECTED2(ch, AFF2_UMBRA))
            diff = 4;
        else
            diff = 4 + get_gauntlet(ch);

       if (is_affected(ch, gsn_meditation) && ch->pcdata->gnosis[TEMP] < ch->pcdata->gnosis[PERM])
       {
            success = godice(get_attribute(ch, WITS) + ch->pcdata->cssec_abil[CSABIL_ENIGMAS], diff);

            if (success > 0)
            {

                if (success > 1)
                    gain = success / 2;
                else
                    gain = 1;

                ch->pcdata->gnosis[TEMP] += UMIN(gain, ch->pcdata->gnosis[PERM] - ch->pcdata->gnosis[TEMP]);

                if (ch->pcdata->gnosis[TEMP] == ch->pcdata->gnosis[PERM])
                    sendch("Your spiritual reserves are fully replenished!\n\r", ch);
                else
                    sendch("You feel refreshed and at one with Mother Gaia.\n\r", ch);

            }
        }
    }


    if ((ch->played/3600) > 15 && IS_SET(ch->act2, PLR2_NEWBIE))
    {
        REMOVE_BIT(ch->act2, PLR2_NEWBIE);
    }
    if (!IS_NPC(ch) && ch->pcdata->influenced > 0)
    {
        ch->pcdata->influenced--;
        if (ch->pcdata->influenced < 0)
            ch->pcdata->influenced = 0;
    }

    if(IS_SET(ch->act,PLR_IC))
    {
        if(!IS_NPC(ch))
        {

            ch->pcdata->IC_total++;
            ch->pcdata->last_pose++;
            ch->pcdata->room_last_pose++;
            ch->roleplaying++;
            if (ch->pcdata->influenced == 0 && ch->roleplaying > INFLUENCE_TIMER)
            {
            AREA_DATA *pArea;
            pArea = ch->in_room->area;
            if (pArea->domain != NULL && IS_SET(ch->act2, PLR2_PVP) &&
            number_percent() < 15 && pArea->domain->change[ch->clan] < 5)
            {
            pArea->domain->change[ch->clan]++;
            ch->pcdata->influenced = INFLUENCE_TIMER;
            save_area(pArea);
            sendch("Your deeds have made you notorious in this domain.\n\r", ch);
        }
    }   /* add rank gain and goal gain here */
        }
    if(ch->qpoints < 30000)
    {
        ichours = ch->pcdata->IC_total/60;

            qpgain = number_range(ichours, ichours/2 + 8);
        if (ichours >= 100)
            qpgain = number_range(ichours/2, ichours/4 + 8);
        if (ichours > 300)
            qpgain = number_range(ichours/4, ichours/8 + 8);
        if (ichours > 600)
            qpgain = number_range(ichours/8, ichours/16 + 8);

       switch (ch->pcdata->room_last_pose) {
           case 0: case 1:
           case 2: case 3: qpgain = 3*qpgain/2;break;
           case 4: case 5: break;
           case 6: case 7: qpgain = qpgain/2;break;
           default: qpgain = 4;break;
       }
        // Doing math ahead of time so variables match
        qpaward = UMIN(200+ch->remorts, qpgain);

       if (ch->pcdata->last_pose < 8)
        {

        // Line for this into gxp as well
             global_xp += qpaward*xpmult;
             ch->qpoints += qpaward;
        }

       if (doubleexp)
           {
           qpaward = qpaward*2;
           global_xp -= qpaward*2;

            if (global_qp > 40000)
              {
                 qpaward = qpaward*2;
                 global_qp -= qpaward;
              }
           ch->qpoints += qpaward;
       }


       if (ch->qpoints > 30000)
            ch->qpoints = 30000;
        }


/* New experience code added by Sengir to help out the hardcore RP'ers, Revised by Matthew
   Calculation: exp random 50-rphrs+100 max 370+remorts*2 */

   ichours = (ch->played + (int) (current_time - ch->logon)) / 3600;
                gain_exp( ch, UMIN( (370 + ch->remorts)*2, number_range( ichours/8, (ichours/6 + ch->roleplaying))));
        if (doubleexp)
            gain_exp( ch, UMIN( (370 + ch->remorts)*2, number_range( ichours/8, (ichours/6 + ch->roleplaying))*xpawardmult));
    }

    if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
    {
        if(ch->pblood <= 5 && ch->position != POS_TORPOR && !IS_IMMORTAL(ch) && !IS_NPC(ch))
        {
            vamp_frenzy( gsn_vamp_frenzy, ch->level, ch, ch,TARGET_CHAR);
/* disabled to reduce lag... maybe - Ugha
            if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
                send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/frenzy.wav>",ch,TO_ROOM);
*/
        }

        if(ch->pblood != 0 && !IS_IMMORTAL(ch) && !is_affected(ch,gsn_earthmeld) && !IS_SET(ch->act,PLR_IC) && (!is_safe))
        {
            if(number_range(1,6) % 2 == 0)
            ch->pblood--;
        }
        if(ch->pblood  < 0) ch->pblood = 0;
        if(ch->pblood == 0 && ch->position != POS_TORPOR && !IS_IMMORTAL(ch) && !IS_NPC(ch))
        {
            send_to_char( "You are DYING from lack of blood!\n\r", ch );
            act( "$n writhes from lack of blood!", ch, NULL, NULL, TO_ROOM );
            damage( ch, ch, ch->max_hit/30, 0, DAM_FIRE ,FALSE);
        }
        if(ch->pblood < 0) ch->pblood = 0;
        if(ch->pblood < ch->max_pblood && IS_IMMORTAL(ch)) ch->pblood = ch->max_pblood;

        if(ch->position != POS_TORPOR && ch->in_room && !room_is_dark( ch->in_room ) &&
        IS_OUTSIDE(ch) && time_info.hour < 19 && time_info.hour > 5 && !IS_IMMORTAL(ch)
        && !IS_NPC(ch) && !IS_SET(ch->imm_flags,IMM_SUNLIGHT) && !is_affected(ch,gsn_earthmeld)
        && !nosun && !IS_SET(ch->act,PLR_IC))
        {
            if ( (weather_info.sky == SKY_CLOUDY) || (weather_info.sky == SKY_RAINING) )
            {
                act( "$n flinches in pain as a few stray beams of sunlight cause $s skin to smoulder.", ch, NULL, NULL, TO_ROOM);
                send_to_char("You flinch reflexively as a few beams of sunlight pass through the clouds and caress your skin.\n\r", ch);
                damage( ch, ch, ch->max_hit/(30+(ch->pcdata->discipline[FORTITUDE]*5)), 0, DAM_SUNLIGHT, FALSE);
                if(is_affected(ch,gsn_shadowform))
                    damage( ch, ch, ch->max_hit/(8+(ch->pcdata->discipline[FORTITUDE]*2)), 0, DAM_SUNLIGHT, FALSE);
            }
            else
            {
                act( "$n screams in agony as $s skin smoulders beneath the blazing sun.", ch, NULL, NULL, TO_ROOM );
                send_to_char( "You scream in pain as your skin smoulders beneath the blazing sun.\n\r", ch );
                damage( ch, ch, ch->max_hit/(15+(ch->pcdata->discipline[FORTITUDE]*5)), 0, DAM_SUNLIGHT, FALSE);
                if(is_affected(ch,gsn_shadowform))
                    damage( ch, ch, ch->max_hit/(8+(ch->pcdata->discipline[FORTITUDE]*2)), 0, DAM_SUNLIGHT, FALSE);
/* disabled to reduce lag... maybe - Ugha
                if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
                    send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/sunlight.wav>",ch,TO_ROOM);
*/
            }
        }
    }
    else if(ch->race == race_lookup("ghoul") && !IS_NPC(ch) && !IS_SET(ch->act,PLR_IC))
    {
/* Sengir temp hack to lower blood loss */
        if(number_range(1,6) == 3)
            ch->pblood--;
        if(ch->pblood < 0)
        {
            ch->pblood = 0;
            send_to_char("Your body begins going into withdrawals from the lack of vitae.\n\r", ch);
            if(number_range(1,24) == 1)
            {
                send_to_char("Thoughts of your master's sweet {Rvitae{x flash briefly in your mind, but the\n\r intensity of the longing isn't nearly what you recall it once being.\n\r", ch);
                ch->bonded--;
                if(ch->bonded < 0)
                    ch->bonded = 0;
            }

            if (ch->bonded == 0)
            {
                send_to_char("\n\r{WThe last of your master's {Rvitae{W burns out of your system, and your body violently shakes from lack of your preferred addiction.\n\rYou no longer feel enthralled to your Domitor, you are free.\n\rBut the longing for {Rvitae{W remains...{x\n\r", ch);
                if(IS_AFFECTED(ch,AFF_SHIFT))
                    do_function(ch, &do_shift,"");
                while( ch->affected )
                    affect_remove( ch, ch->affected );

                ch->race = race_lookup("human");
                ch->affected_by = race_table[ch->race].aff;
                ch->max_pblood = 100;
                ch->pblood = 100;
                ch->exp = exp_per_level(ch, ch->pcdata->points) * UMAX(1, ch->level);

            }

/*          damage(ch,ch,ch->max_hit/2,0,DAM_FIRE,FALSE);
            send_to_char( "You are DYING from lack of nourishment! You MUST fulfill your addiction!\n\r", ch );
            act( "$n screams in agony from lack of nourishment.", ch, NULL, NULL, TO_ROOM );
*/
        }
    }
    else if(ch->race == race_lookup("dhampire") && ch->pblood < ch->max_pblood)
    {
        ch->pblood++;
/* New dhampire blood regen by Sengir */
        if (ch->pcdata->condition[COND_HUNGER] != 0)
            ch->pblood++;
        if (ch->pcdata->condition[COND_THIRST] != 0)
            ch->pblood++;
        if (ch->position == POS_SLEEPING)
            ch->pblood++;

        if(ch->pblood > ch->max_pblood) ch->pblood = ch->max_pblood;
    }
    else if ( ch->position == POS_INCAP && number_range(0,1) == 0)
    {
        damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE);
    }
    else if ( ch->position == POS_MORTAL )
    {
        damage( ch, ch, 1, TYPE_UNDEFINED, DAM_NONE,FALSE);
    }
    }

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;

    if (ch->desc != NULL && ch->desc->descriptor % 30 == save_number)
    {
        save_char_obj(ch);
    }

        if (ch == ch_quit)
    {
            do_function(ch, &do_quit, "" );
    }
    }

    return;
}




/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA *paf, *paf_next;

    for ( obj = object_list; obj != NULL; obj = obj_next )
    {
    CHAR_DATA *rch;
    char *message;

    obj_next = obj->next;

    /* go through affects and decrement */
        for ( paf = obj->affected; paf != NULL; paf = paf_next )
        {
            paf_next    = paf->next;
            if ( paf->duration > 0 )
            {
                paf->duration--;
                if (number_range(0,4) == 0 && paf->level > 0)
                  paf->level--;  /* spell strength fades with time */
            }
            else if ( paf->duration < 0 )
                ;
            else
            {
                if ( paf_next == NULL
                ||   paf_next->type != paf->type
                ||   paf_next->duration > 0 )
                {
                    if ( paf->type > 0 && skill_table[paf->type].msg_obj )
                    {
            if (obj->carried_by != NULL)
            {
                rch = obj->carried_by;
                act(skill_table[paf->type].msg_obj,
                rch,obj,NULL,TO_CHAR);
            }
            if (obj->in_room != NULL
            && obj->in_room->people != NULL)
            {
                rch = obj->in_room->people;
                act(skill_table[paf->type].msg_obj,
                rch,obj,NULL,TO_ALL);
            }
                    }
                }

                affect_remove_obj( obj, paf );
            }
        }


    if ( obj->timer <= 0 || --obj->timer > 0 )
        continue;

    switch ( obj->item_type )
    {
    default:              message = "$p crumbles into dust.";  break;
    case ITEM_FOUNTAIN:   message = "$p dries up.";         break;
    case ITEM_CORPSE_NPC: message = "$p decays into dust."; break;
    case ITEM_CORPSE_PC:  message = "$p decays into dust."; break;
    case ITEM_FOOD:       message = "$p decomposes.";   break;
    case ITEM_POTION:     message = "$p has evaporated from disuse.";
                                break;
    case ITEM_PORTAL:     message = "$p fades out of existence."; break;
    case ITEM_CONTAINER:
        if (CAN_WEAR(obj,ITEM_WEAR_FLOAT))
        if (obj->contains)
            message =
        "$p flickers and vanishes, spilling its contents on the floor.";
        else
            message = "$p flickers and vanishes.";
        else
        message = "$p crumbles into dust.";
        break;
    }

    if ( obj->carried_by != NULL )
    {
        if (IS_NPC(obj->carried_by)
        &&  obj->carried_by->pIndexData->pShop != NULL)
        obj->carried_by->silver += obj->cost/5;
        else
        {
            act( message, obj->carried_by, obj, NULL, TO_CHAR );
        if ( obj->wear_loc == WEAR_FLOAT)
            act(message,obj->carried_by,obj,NULL,TO_ROOM);
        }
    }
    else if ( obj->in_room != NULL
    &&      ( rch = obj->in_room->people ) != NULL )
    {
        if (! (obj->in_obj && obj->in_obj->pIndexData->vnum == OBJ_VNUM_PIT
               && !CAN_WEAR(obj->in_obj,ITEM_TAKE)))
        {
            act( message, rch, obj, NULL, TO_ROOM );
            act( message, rch, obj, NULL, TO_CHAR );
        }
    }

        if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
    &&  obj->contains)
    {   /* save the contents */
            OBJ_DATA *t_obj, *next_obj;

        for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
        {
        next_obj = t_obj->next_content;
        obj_from_obj(t_obj);

        if (obj->in_obj) /* in another object */
            obj_to_obj(t_obj,obj->in_obj);

        else if (obj->carried_by)  /* carried */
            if (obj->wear_loc == WEAR_FLOAT)
            if (obj->carried_by->in_room == NULL)
                extract_obj(t_obj);
            else
                obj_to_room(t_obj,obj->carried_by->in_room);
            else
                obj_to_char(t_obj,obj->carried_by);

        else if (obj->in_room == NULL)  /* destroy it */
            extract_obj(t_obj);

        else /* to a room */
            obj_to_room(t_obj,obj->in_room);
        }
    }

    extract_obj( obj );
    }

    return;
}
void influence_fade(void)
{
    AREA_DATA *pArea;
    DOMAIN_DATA *dom;
    int i;

        for ( pArea = area_first; pArea; pArea = pArea->next )
        {
        if ((dom = pArea->domain) == NULL)
            continue;
        for (i = 0; i < MAX_CLAN;i++)
        {
            if (dom->change[i] > 0 || dom->influence[i] <= 0)
                continue;
            if (number_percent() < 5)
                dom->influence[i]--;
        }
        save_area( pArea );
    }
    return;
}


void influence_update(void)
{
    AREA_DATA *pArea;
    int         i;
    int         change;

       for ( pArea = area_first; pArea; pArea = pArea->next )
        {
        if (!pArea->domain)
            continue;
        for (i = 0; i < MAX_CLAN;i++)
        {
            change = pArea->domain->change[i];

            if (change == 0)
                continue;

            if (change > 6 || change < -6)
                change = change > 0 ? 6 : -6;
            pArea->domain->influence[i] += change;
            pArea->domain->change[i] = 0;
            if (pArea->domain->influence[i] > 100)
                pArea->domain->influence[i] = 100;
            if (pArea->domain->influence[i] < -100)
                pArea->domain->influence[i] = -100;
        }
            save_area(pArea);
        }
    }





/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    CHAR_DATA *victim;




    for ( wch = char_list; wch != NULL; wch = wch_next )
    {
    wch_next = wch->next;
    if ( IS_NPC(wch)
    ||   wch->level >= LEVEL_IMMORTAL
    ||   wch->in_room == NULL
    ||   wch->in_room->area->empty)
        continue;

    for ( ch = wch->in_room->people; ch != NULL; ch = ch_next )
    {
        int count;

        ch_next = ch->next_in_room;

        if ( !IS_NPC(ch)
        ||   (!IS_SET(ch->act, ACT_AGGRESSIVE) && !grudge_check(ch,wch))
        ||   IS_SET(ch->in_room->room_flags,ROOM_SAFE)
        ||   IS_AFFECTED(ch,AFF_CALM)
        ||   ch->fighting != NULL
        ||   IS_AFFECTED(ch, AFF_CHARM)
        ||   !IS_AWAKE(ch)
        ||   ( IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch) )
        ||   !can_see( ch, wch )
        ||   number_bits(1) == 0)
        continue;


        /*
         * Ok we have a 'wch' player character and a 'ch' npc aggressor.
         * Now make the aggressor fight a RANDOM pc victim in the room,
         *   giving each 'vch' an equal chance of selection.
         */
        count   = 0;
        victim  = NULL;
        for ( vch = wch->in_room->people; vch != NULL; vch = vch_next )
        {
        vch_next = vch->next_in_room;

        if ( !IS_NPC(vch)
        &&   vch->level < LEVEL_IMMORTAL
        &&   !(ch->pIndexData->area->domain != NULL && is_clan(vch) &&
             IS_SET(ch->act2, PLR2_PVP) && ch->pIndexData->area->domain->clan == vch->clan)
/*      &&   ch->level >= vch->level - 5 */
        &&   ( !IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch) )
        &&   can_see( ch, vch )
        &&   vch->position > POS_TORPOR
        &&   !IS_AFFECTED2(vch, AFF2_MAJESTY)
        &&   !is_affected(vch, gsn_shadowform)
        &&   !IS_AFFECTED2(vch, AFF2_MIST)
        &&   !is_affected(vch,gsn_sidesteptime)
        &&   (!(IS_VAMP(ch) && (vch->pcdata->csbackgrounds[CSBACK_STATUS]*10 > number_range(1, 100)))
        ||   !(ch->race == race_lookup("human") && (vch->pcdata->csbackgrounds[CSBACK_FAME]*10 > number_range(1, 100)))
        ||   !(ch->race == race_lookup("garou") && (vch->pcdata->csbackgrounds[CSBACK_PUREBREED]*10 > number_range(1, 100)))))

        {
            if ( number_range( 0, count ) == 0 )
            victim = vch;
            count++;
        }
        }

        if ( victim == NULL )
        continue;
    if(grudge_check(ch,victim))
    {
        if (victim->agg > 0)
        {
            victim->agg--;
            continue;
        }
        else if(ch->hit < ch->max_hit/10)
        {
            act( "$n shrieks loudly at the sight of $N!", ch, NULL, victim, TO_NOTVICT );
            act( "$n shrieks loudly at the sight of you!", ch, NULL, victim, TO_VICT );
            do_function(ch, &do_flee, "auto");
            victim->agg = PULSE_PER_SECOND;
            continue;
        }
        else
            do_function(ch, &do_say, "Now you die, pig!!" );
    }

    if( IS_AFFECTED(victim, AFF_SNEAK) && !IS_SET(ch->res_flags,RES_SOUND) && !IS_SET(ch->imm_flags,IMM_SOUND) && !IS_SET(ch->vuln_flags,VULN_SOUND))
    {
        if (victim->agg > 0)
        {
            victim->agg--;
            continue;
        }
    }
        affect_strip(victim,gsn_chant);
        affect_strip(victim,gsn_meditation);
            if (RIDDEN(ch))
        {
        if (!mount_success(RIDDEN(wch), ch, FALSE))
        {
            send_to_char("Your mount escapes your control!\n\r", RIDDEN(ch));
            multi_hit( ch, victim, TYPE_UNDEFINED );
        }
        else
        {
            send_to_char("You manage to keep your mount under control.\n\r", RIDDEN(ch));
        }
        }
        else
            multi_hit( ch, victim, TYPE_UNDEFINED );
        multi_hit( ch, victim, TYPE_UNDEFINED );
    }
    }

    return;
}


void sleep_update(void)
{
        SLEEP_DATA *temp = first_sleep, *temp_next;

        for( ; temp != NULL; temp = temp_next)
        {
            bool delete = FALSE;

            temp_next = temp->next;

            /* checks to make sure the mob still exists*/
            if(!temp->mob)
                delete = TRUE;
            /*checks to make sure the character is still in the same room as the mob*/
            else if(temp->mob && temp->ch && temp->mob->in_room != temp->ch->in_room)
                delete = TRUE;
            if(delete)
            {
                /* some slick linked list manipulation */
                if(temp->prev)
                    temp->prev->next = temp->next;
                if(temp->next)
                    temp->next->prev = temp->prev;
                if( temp == first_sleep && (temp->next == NULL || temp->prev == NULL) )
                    first_sleep = temp->next;
                free_sleep_data(temp);
                continue;
            }

        if(--temp->timer <= 0)
        {
            program_flow(temp->vnum, temp->prog->code, temp->mob, temp->ch, NULL, NULL, temp->line);

            /* more slick linked list manipulation */
            if(temp->prev)
                temp->prev->next = temp->next;
            if(temp->next)
                temp->next->prev = temp->prev;
            if( temp == first_sleep && (temp->next == NULL || temp->prev == NULL) )
                first_sleep = temp->next;
            free_sleep_data(temp);
        }
    }
}



/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler( void )
{
CHAR_DATA   *ch;

    if ( --pulse_area     <= 0 )
    {
    pulse_area  = PULSE_AREA;
    /*number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );*/
    area_update ( );
    quest_update     ( );
    save_config();
    }

    if ( --pulse_music    <= 0 )
    {
    pulse_music = PULSE_MUSIC;
    song_update();
    }

    if ( --pulse_mobile   <= 0 )
    {
    pulse_mobile    = PULSE_MOBILE;
    mobile_update   ( );
    }

    if ( --pulse_violence <= 0 )
    {
    pulse_violence  = PULSE_VIOLENCE;
    violence_update ( );

    }
    if ( --pulse_affects <= 0 )
    {
    pulse_affects = PULSE_AFFECTS;
    affects_update ( );
    }

    if ( --pulse_regen <= 0)
    {
        pulse_regen = PULSE_REGEN;
        regen_update();
    }

    if ( --pulse_underwater    <= 0 )
    {
        pulse_underwater     = PULSE_UNDERWATER;
        underwater_update    ( );
    }

    if (--pulse_influence_fade <= 0 )
    {
    pulse_influence_fade = PULSE_INFLUENCEFADE;
    influence_fade ( );
    }

    if (--pulse_influence <= 0 )
    {
    pulse_influence = PULSE_INFLUENCECHANGE;
    influence_update  ( );
    }


    if ( --pulse_bank     <= 0 )
    {
    pulse_bank  = PULSE_BANK;
    bank_update ( );
    }

    if ( --pulse_point    <= 0 )
    {
    wiznet("{WTICK{R!{x",NULL,NULL,WIZ_TICKS,0,0);
    pulse_point     = PULSE_TICK;
    char_update ( );
    obj_update  ( );
    }

    if (--pulse_weather   <= 0 )
    {
        pulse_weather   = PULSE_WEATHER;
        wiznet("{DTOCK{R!{x", NULL, NULL, WIZ_TICKS, 0, 0);
        weather_update  ( );
    }

    sleep_update ();
    aggr_update( );
    tail_chain( );
    return;
}

//Matt's banking code.

void bank_offline_update(void)
{
return;
};
/*
DESCRIPTOR_DATA d;
CHAR_DATA   *ch;
char buf[MSL];
struct dirent *pfile;
FILE *fp;
bool isChar = FALSE;

sprintf(buf, "%s", PLAYER_DIR);
while( pfile != NULL )
{
pfile = readdir(buf);
if ( pfile.d_name[0] == '.' )
continue;
strcat(buf,pfile->d_name);

if ( get_char_world( ch, buf ) != NULL )
continue;

isChar = load_char_obj(&d, buf);
//Pfile exists?
if (!isChar)
{
   bug( "bank_update: pfile not found.", 0 );
    continue;
  }
d.character->desc     = NULL;
ch            = d.character;
reset_char(ch);
int interest;

if (ch->pcdata->bank <= 0 && ch->pcdata->csbackgrounds[CSBACK_RESOURCES] <= 0)
    continue;

if (ch->pcdata->csbackgrounds[CSBACK_RESOURCES] > 0) {
    interest = (ch->pcdata->csbackgrounds[CSBACK_RESOURCES] * ch->pcdata->csbackgrounds[CSBACK_RESOURCES]) * 2;
    interest += (ch->pcdata->csbackgrounds[CSBACK_STATUS] * ch->pcdata->csbackgrounds[CSBACK_INFLUENCE]);
    interest *= 50;
    interest = number_range(2*interest/3,3*interest/2);*
    interest = (5+ch->pcdata->csbackgrounds[CSBACK_STATUS]) * interest/10;
    if (interest > 1000)
    interest = 8*interest/10;
    if (interest > 3000)
    interest = 9*interest/10;
    if (interest > 4000)
    interest = 7*interest/10;
    if (interest > 5000)
    interest = 7*interest/10;
    if (interest > 6000)
    interest = 7*interest/10;
}


if (ch->pcdata->bank > 0)
{
    interest = (ch->pcdata->bank*1)/10000;
    if (interest > 1000)
    interest = 8*interest/10;
    if (interest > 3000)
    interest = 9*interest/10;
    if (interest > 4000)
    interest = 7*interest/10;
    if (interest > 5000)
    interest = 7*interest/10;
    if (interest > 6000)
    interest = 7*interest/10;
}
save_char_obj(ch);
break;
}
return;
}
};*/

void bank_update( void )
{
int interest;
DESCRIPTOR_DATA *d;
CHAR_DATA   *ch;


    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    ch = d->character;
        if ( d->connected == CON_PLAYING && !IS_NPC(d->character))
        {

if (ch->pcdata->bank <= 0 && ch->pcdata->csbackgrounds[CSBACK_RESOURCES] <= 0)
    continue;

if (ch->pcdata->csbackgrounds[CSBACK_RESOURCES] > 0) {
    interest = (ch->pcdata->csbackgrounds[CSBACK_RESOURCES] * ch->pcdata->csbackgrounds[CSBACK_RESOURCES]) * 2;
    interest += (ch->pcdata->csbackgrounds[CSBACK_STATUS] * ch->pcdata->csbackgrounds[CSBACK_INFLUENCE]);
    interest *= 10;
    interest = number_range(2*interest/3,3*interest/2);
    interest = (5+ch->pcdata->csbackgrounds[CSBACK_STATUS]) * interest/10;
    if (interest > 100)
    interest = 9*interest/10;
    if (interest > 200)
    interest = 8*interest/10;
    if (interest > 300)
    interest = 7*interest/10;
    if (interest > 1000)
    interest = 7*interest/10;
    if (interest > 2000)
    interest = 6*interest/10;
    ch->pcdata->bank += interest;
}


if (ch->pcdata->bank > 0)
{
    interest = (ch->pcdata->bank)/10000;
    if (interest > 500)
    interest = 9*interest/10;
    if (interest > 1000)
    interest = 8*interest/10;
    if (interest > 2000)
    interest = 7*interest/10;
    if (interest > 3000)
    interest = 6*interest/10;
        if (interest > 4000)
        interest = 5*interest/10;
        ch->pcdata->bank += (interest+1);
    }}
break;
}
return;
};
