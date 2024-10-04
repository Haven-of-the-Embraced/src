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
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "addict.h"

ROOM_INDEX_DATA *       find_location   args( ( CHAR_DATA *ch, char *arg ) );

int cuplayers;
char *  const   where_name  [] =
{
    "<used as light>     ",
    "<worn on finger>    ",
    "<worn on finger>    ",
    "<worn around neck>  ",
    "<worn around neck>  ",
    "<worn on torso>     ",
    "<worn on head>      ",
    "<worn on legs>      ",
    "<worn on feet>      ",
    "<worn on hands>     ",
    "<worn on arms>      ",
    "<worn as shield>    ",
    "<worn about body>   ",
    "<worn about waist>  ",
    "<worn around wrist> ",
    "<worn around wrist> ",
    "<wielded>           ",
    "<held>              ",
    "<floating nearby>   ",
    "<worn in ear>       ",
    "<worn in ear>       ",
    "<worn on face>      ",
};


/* for  keeping track of the player count */
int max_on = 0;

/* extern for whether the action is a 'look' */
bool ACT_LOOKING = FALSE;

/*
 * Local functions.
 */
char *  format_obj_to_char  args( ( OBJ_DATA *obj, CHAR_DATA *ch,
                    bool fShort ) );
void    show_list_to_char   args( ( OBJ_DATA *list, CHAR_DATA *ch,
                    bool fShort, bool fShowNothing ) );
void    show_char_to_char_0 args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char_1 args( ( CHAR_DATA *victim, CHAR_DATA *ch ) );
void    show_char_to_char   args( ( CHAR_DATA *list, CHAR_DATA *ch ) );
bool    check_blind     args( ( CHAR_DATA *ch ) );
bool    vampsightcheck      args( ( CHAR_DATA *ch ) );



char *format_obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch, bool fShort )
{
    static char buf[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if ((fShort && (obj->short_descr == NULL || obj->short_descr[0] == '\0'))
    ||  (obj->description == NULL || obj->description[0] == '\0'))
    return buf;

    if ( IS_IMMORTAL(ch) && IS_OBJ_STAT(obj, ITEM_UMBRA) && !IS_AFFECTED2(ch, AFF2_UMBRA)      )   strcat( buf, "(Umbra) "   );
    if ((is_affected(ch, gsn_spiritsight) || is_affected(ch, gsn_gift_pulseoftheinvisible))
        && (IS_OBJ_STAT(obj, ITEM_UMBRA) && !SAME_UMBRA_OBJ(ch, obj))) strcat(buf, "(Indistinct) ");
    if ( IS_OBJ_STAT(obj, ITEM_INVIS)     )   strcat( buf, "(Invis) "     );
    if ( IS_AFFECTED(ch, AFF_DETECT_MAGIC)
         && IS_OBJ_STAT(obj, ITEM_MAGIC)  )   strcat( buf, "(Magical) "   );
    if ((ch->race == race_lookup("garou") || is_affected(ch, gsn_spiritsight)) && 
        affect_find(obj->affected,skill_lookup("fetish")) != NULL)
        strcat( buf, "(Spiritual) ");
    if ( IS_OBJ_STAT(obj, ITEM_ILLUMINATE) )    strcat( buf, "(Illuminating) ");
    if ( IS_OBJ_STAT(obj, ITEM_GLOW)      )   strcat( buf, "(Glowing) "   );
    if ( IS_OBJ_STAT(obj, ITEM_HUM)       )   strcat( buf, "(Humming) "   );
    if ( is_affected(ch, gsn_gift_sensesilver) && IS_OBJ_STAT(obj, ITEM_SILVER))
      strcat( buf, "(Silver) "    );

    if ( fShort )
    {
    if ( obj->short_descr != NULL )
        strcat( buf, obj->short_descr );
    }
    else
    {
    if ( obj->description != NULL)
        strcat( buf, obj->description );
    }

    return buf;
}


/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char( OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    char **prgpstrShow;
    int *prgnShow;
    char *pstrShow;
    OBJ_DATA *obj;
    int nShow;
    int iShow;
    int count;
    bool fCombine;

    if ( ch->desc == NULL )
    return;

    /*
     * Alloc space for output lines.
     */
    output = new_buf();

    count = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    count++;
    prgpstrShow = alloc_mem( count * sizeof(char *) );
    prgnShow    = alloc_mem( count * sizeof(int)    );
    nShow   = 0;

    /*
     * Format the list of objects.
     */
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
    if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ))
    {
        pstrShow = format_obj_to_char( obj, ch, fShort );

        fCombine = FALSE;

        if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
        {
        /*
         * Look for duplicates, case sensitive.
         * Matches tend to be near end so run loop backwords.
         */
        for ( iShow = nShow - 1; iShow >= 0; iShow-- )
        {
            if ( !strcmp( prgpstrShow[iShow], pstrShow ) )
            {
            prgnShow[iShow]++;
            fCombine = TRUE;
            break;
            }
        }
        }

        /*
         * Couldn't combine, or didn't want to.
         */
        if ( !fCombine )
        {
        prgpstrShow [nShow] = str_dup( pstrShow );
        prgnShow    [nShow] = 1;
        nShow++;
        }
    }
    }

    /*
     * Output the formatted list.
     */
    for ( iShow = 0; iShow < nShow; iShow++ )
    {
    if (prgpstrShow[iShow][0] == '\0')
    {
        free_string(prgpstrShow[iShow]);
        continue;
    }

    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
    {
        if ( prgnShow[iShow] != 1 )
        {
        sprintf( buf, "(%2d) ", prgnShow[iShow] );
        add_buf(output,buf);
        }
        else
        {
        add_buf(output,"     ");
        }
    }
    add_buf(output,prgpstrShow[iShow]);
    add_buf(output,"\n\r");
    free_string( prgpstrShow[iShow] );
    }

    if ( fShowNothing && nShow == 0 )
    {
    if ( IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE) )
        send_to_char( "     ", ch );
    send_to_char( "Nothing.\n\r", ch );
    }
    page_to_char(buf_string(output),ch);

    /*
     * Clean up.
     */
    free_buf(output);
    free_mem( prgpstrShow, count * sizeof(char *) );
    free_mem( prgnShow,    count * sizeof(int)    );

    return;
}



void show_char_to_char_0( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH],message[MAX_STRING_LENGTH];

    buf[0] = '\0';

    if (IS_SET(victim->comm,COMM_AFK )) strcat( buf, "{x[{RAFK{x]{y ");
    if ( victim->invis_level >= LEVEL_HERO ) strcat( buf, "{x(Wizi){y ");
    if (IS_NPC(victim) && IS_SET(victim->act, ACT_QUESTMOB) && IS_IMMORTAL(ch))
      strcat( buf, "{W*{D!{GQ{gu{ce{Cst{cM{go{Gb{D!{W*{y "        );

    if (IS_NPC(victim) && IS_SET(victim->act2, ACT2_INFLUENCE) &&
        !IS_NPC(ch) && is_affected(ch, gsn_notoriety))
            strcat(buf, "{R({rInfluence{R){y ");
    if ( IS_IMMORTAL(ch) && IS_AFFECTED2(victim, AFF2_UMBRA) ) strcat( buf, "{x({mUmbra{x){y ");
    if ( (is_affected(ch, gsn_spiritsight) || is_affected(ch, gsn_gift_pulseoftheinvisible))
        && !IS_AFFECTED2(ch, AFF2_UMBRA) && IS_AFFECTED2(victim, AFF2_UMBRA)) strcat(buf, "{x({mIndistinct{x){y ");
    if ( RIDDEN(victim) )
    {
        if ( ch != RIDDEN(victim) )
            strcat( buf, "(Ridden) " );
        else
        strcat( buf, "(Your mount) " );
    }
    if(is_affected(ch, gsn_gift_sensetheunnatural) && is_supernatural(victim)) strcat( buf, "{G({MS{G){y ");
    if(is_affected(ch,gsn_gift_sensewyrm))
    {
        if(IS_EVIL(victim) || victim->race == race_lookup("methuselah") || victim->race == race_lookup("vampire"))
            strcat( buf, "{x*{RTaint{x*{y " );
        else if(victim->race == race_lookup("ghoul") || victim->race == race_lookup("dhampire"))
            strcat( buf, "{x*{MTaint{x*{y " );
    }
    if ( IS_AFFECTED(victim, AFF_INVISIBLE)   ) strcat( buf, "{x({CInvis{x){y "      );
    if ( IS_AFFECTED(victim, AFF_HIDE)        ) strcat( buf, "{x({yHide{x){y "       );
    if ( IS_AFFECTED(victim, AFF_CHARM)       ) strcat( buf, "{x({BCharmed{x){y "    );
    if ( IS_AFFECTED2(victim, AFF2_MAJESTY)   ) strcat( buf, "{x({RM{Da{rj{Res{rt{Di{Rc{x){y " );
    if ( is_affected(ch, gsn_timesense) && is_affected(victim, gsn_sidesteptime))
      strcat( buf, "{x{D[{YTemporal{D]{y "   );
    if ( is_affected(ch, gsn_timesense) && is_affected(victim, gsn_stoptheclock))
      strcat( buf, "{x{Y[{DStopped{Y]{y "   );
    if ( is_affected(ch, gsn_timesense) &&
      (IS_AFFECTED(victim, AFF_HASTE) || IS_SET(victim->off_flags, OFF_FAST) ||
        is_affected(victim, gsn_celbuff) || is_affected(victim, gsn_rage) ||
        is_affected(victim, gsn_timealteration)) ) strcat( buf, "{x{D({YAccelerated{D){y "   );
    if ( is_affected(ch, gsn_timesense) &&
        (is_affected(victim, gsn_slow) || IS_AFFECTED(victim, AFF_SLOW)  ))
        strcat( buf, "{x{Y({DDecelerated{Y){y "   );
    if(is_affected(ch, gsn_telepathy) && IS_SET(victim->form,FORM_SENTIENT)) strcat( buf, "{x{w[{G::{w]{y " );
    if ( IS_AFFECTED(victim, AFF_PASS_DOOR)   ) strcat( buf, "{x({cTranslucent{x){y ");
    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) ) strcat( buf, "{x({MPink Aura{x){y "  );
    if ( IS_AFFECTED(victim, AFF_SANCTUARY)   ) strcat( buf, "{x({wWhite Aura{x){y " );
    if ( is_affected(victim, gsn_unseen))      strcat( buf, "{x({DUnseen{x){y ");
    if(IS_AFFECTED(victim, AFF_FANGS)       )   strcat( buf, "{x({rFangs{x){y "  );
    if ( is_affected(victim, gsn_gift_eyesofthecat))    strcat( buf, "{x@{GEyes{x@{y ");
    if ( is_affected(victim, gsn_gleam))    strcat( buf, "{x@{REyes{x@{y ");
/*Players able to see those hovering above the ground - Sengir*/
    if (IS_AFFECTED(victim, AFF_FLYING))    strcat( buf, "{x({GAloft{x){y " );
    if(IS_SET(victim->imm_flags,IMM_SUNLIGHT) && (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah")))    strcat( buf, "{x({DShadowy{x){y "  );
    if(IS_SET(victim->act,PLR_IC))      strcat( buf, "{x{D({CIC{D){x{y "    );
    if(IS_SET(victim->act,PLR_SPEC) && !IS_NPC(victim))     strcat( buf, "{x({GSpec{x){y "  );
    if(!IS_NPC(victim) && IS_SET(victim->act,PLR_ARENA))       strcat( buf, "{x({RArena{x){y " );
    if ( IS_AFFECTED2(victim, AFF2_FIRESHIELD)   ) strcat( buf, "{x({YGodly Aura{x){y " );
    if ( IS_AFFECTED2(victim, AFF2_HOLYSHIELD)   ) strcat( buf, "{x({WRadiant Aura{x){y " );
    if ( IS_AFFECTED2(victim, AFF2_DARKSHIELD)   ) strcat( buf, "{x({DDark Aura{x){y " );
    if ( is_affected(victim,gsn_shockshield)    ) strcat( buf, "{x({CS{Wt{cat{Wi{Cc{x){y " );
    if (is_affected(victim, gsn_gift_lambentfire))  strcat(buf, "{x{D{{{WLuminous{D}{y ");
/*Testing for imms to see Sanctus Lucis by Sengir*/
    if ( is_affected(victim,gsn_sanctus) && IS_IMMORTAL(ch))    strcat( buf, "{x({WH{wol{Wy{x){y ");
    if (is_affected(ch, gsn_gift_sensesilver) && has_silver(victim))
      strcat( buf, "{W[{wSilver{W>{y " );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER ) )
                        strcat( buf, "{x({RKILLER{x) "     );
    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF  ) )
                        strcat( buf, "(THIEF) "      );
    if (IS_NPC(victim) &&ch->questmob > 0 && victim->pIndexData->vnum == ch->questmob)
        strcat( buf, "[TARGET] ");
    strcat( buf, "{y");
    if ( victim->position == victim->start_pos && victim->long_descr[0] != '\0' )
    {
    strcat( buf, victim->long_descr );
    send_to_char( buf, ch );
    return;
    }

    strcat( buf, PERS( victim, ch, FALSE ) );
    if ( !IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF)
    &&   victim->position == POS_STANDING && ch->on == NULL )
    if ( !IS_AFFECTED(victim, AFF_SHIFT) && victim->changed == 0 && !is_affected( victim, gsn_mask )
    && is_affected(victim,gsn_shadowform) && is_affected(victim,gsn_vicissitude_horrid))
    strcat( buf, victim->pcdata->title );

    switch ( victim->position )
    {
    case POS_TORPOR:     strcat( buf, "{y is mortally wounded." ); break;
    case POS_DEAD:     strcat( buf, "{y is DEAD!!" );              break;
    case POS_MORTAL:   strcat( buf, "{y is mortally wounded." );   break;
    case POS_INCAP:    strcat( buf, "{y is incapacitated." );      break;
    case POS_STUNNED:  strcat( buf, "{y is lying here stunned." ); break;
    case POS_SLEEPING:
    if (victim->on != NULL)
    {
        if (IS_SET(victim->on->value[2],SLEEP_AT))
        {
        sprintf(message,"{y is sleeping at %s{y.",
            victim->on->short_descr);
        strcat(buf,message);
        }
        else if (IS_SET(victim->on->value[2],SLEEP_ON))
        {
        sprintf(message,"{y is sleeping on %s{y.",
            victim->on->short_descr);
        strcat(buf,message);
        }
        else
        {
        sprintf(message, "{y is sleeping in %s{y.",
            victim->on->short_descr);
        strcat(buf,message);
        }
    }
    else
        strcat(buf,"{y is slumbering peacefully.");
    break;
    case POS_RESTING:
        if (victim->on != NULL)
    {
            if (IS_SET(victim->on->value[2],REST_AT))
            {
                sprintf(message,"{y is resting at %s{y.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],REST_ON))
            {
                sprintf(message,"{y is resting on %s{y.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, "{y is resting in %s{y.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
    }
        else
        strcat( buf, "{y is relaxing comfortably{y." );
    break;
    case POS_SITTING:
        if (victim->on != NULL)
        {
            if (IS_SET(victim->on->value[2],SIT_AT))
            {
                sprintf(message,"{y is sitting at %s{y.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else if (IS_SET(victim->on->value[2],SIT_ON))
            {
                sprintf(message," is sitting on %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
            else
            {
                sprintf(message, " is sitting in %s.",
                    victim->on->short_descr);
                strcat(buf,message);
            }
        }
        else
        strcat(buf, " is sitting quietly.");
    break;
    case POS_STANDING:
    if (victim->on != NULL)
    {
        if (IS_SET(victim->on->value[2],STAND_AT))
        {
        sprintf(message," is standing at %s.",
            victim->on->short_descr);
        strcat(buf,message);
        }
        else if (IS_SET(victim->on->value[2],STAND_ON))
        {
        sprintf(message," is standing on %s.",
           victim->on->short_descr);
        strcat(buf,message);
        }
        else
        {
        sprintf(message," is standing in %s.",
            victim->on->short_descr);
        strcat(buf,message);
        }
    }
    else if ( MOUNTED(victim) )
    {
        strcat( buf, " is here, riding " );
        strcat( buf, MOUNTED(victim)->short_descr );
        strcat( buf, ".");
    }
    else
    {
        strcat( buf, "{y is loitering in the area." );
    }
    break;
    case POS_FIGHTING:
    strcat( buf, "{y is engaged in an epic battle to the death with " );
    if ( victim->fighting == NULL )
        strcat( buf, "{ythin air??" );
    else if ( victim->fighting == ch )
        strcat( buf, "{yYOU!" );
    else if ( victim->in_room == victim->fighting->in_room )
    {
        strcat( buf, PERS( victim->fighting, ch, FALSE ) );
        strcat( buf, "." );
    }
    else
        strcat( buf, "{ysomeone who left??" );
    break;
    }

    strcat( buf, "{x\n\r" );
    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}



void show_char_to_char_1( CHAR_DATA *victim, CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    int iWear;
    int percent;
    bool found;

    if ( can_see( victim, ch ) )
    {
    if (ch == victim)
        act( "$n looks at $mself.",ch,NULL,NULL,TO_ROOM);
    else
    {
        act( "$n looks at you.", ch, NULL, victim, TO_VICT    );
        act( "$n looks at $N.",  ch, NULL, victim, TO_NOTVICT );
    }
    }

    if ( victim->description[0] != '\0' )
    {
    send_to_char( victim->description, ch );
    send_to_char( "\n\r", ch);
    }
    else
    {
    act( "You see nothing special about $M.\n\r", ch, NULL, victim, TO_CHAR );
    }

     if (!IS_NPC(victim))
    {
        send_to_char("{WAppearance_____", ch);
        if (get_attribute(victim, APPEARANCE) == 0)
        {
            send_to_char("{DX{wOOOO{x\n\r", ch);
            act("Not even $S own mother could love a face like that.", ch, NULL, victim, TO_CHAR);
        }
        if (get_attribute(victim, APPEARANCE) == 1)
        {
            send_to_char("@{wOOOO{x\n\r", ch);
                        act("$E looks like $E got hit one too many times with the ugly stick.", ch, NULL, victim, TO_CHAR);
        }
        if (get_attribute(victim, APPEARANCE) == 2)
        {
            send_to_char("@@{wOOO{x\n\r", ch);
                        act("$E is kinda funny lookin, but not altogether unattractive.", ch, NULL, victim, TO_CHAR);
        }
        if (get_attribute(victim, APPEARANCE) == 3)
        {
            send_to_char("@@@{wOO{x\n\r", ch);
                        act("$E likely turns heads any place $E goes.", ch, NULL, victim, TO_CHAR);
        }
        if (get_attribute(victim, APPEARANCE) == 4)
        {
            send_to_char("@@@@{wO{x\n\r", ch);
                        act("The bards sing praise of $S beauty, immortalizing it in song.", ch, NULL, victim, TO_CHAR);
        }
        if (get_attribute(victim, APPEARANCE) == 5)
        {
            send_to_char("@@@@@{x\n\r", ch);
                        act("$E could have anyone, anytime, anywhere.", ch, NULL, victim, TO_CHAR);
        }
        send_to_char("\n\r", ch);
    }

    if (IS_AFFECTED(victim,AFF_FANGS))
    act( "A pair of inch long ({rFangs{x) protrude from behind $N's upper lip!", ch, NULL, victim, TO_CHAR );

    if (IS_AFFECTED2(victim, AFF2_MAJESTY))
    act( "$N exudes an air of {Brespect{x, {Yfear{x, and {Mdevotion{x from all who gaze upon $M.", ch, NULL, victim, TO_CHAR);

    if (is_affected(ch, gsn_gift_scentofthetrueform))
    {
      sprintf(buf,"You detect the unmistakable scent of a %s coming from %s.\n\r",
            race_table[victim->race].name, victim->short_descr);
      send_to_char(buf,ch);
    }

    if ( victim->max_hit > 0 )
    percent = ( 100 * victim->hit ) / victim->max_hit;
    else
    percent = -1;

    strcpy( buf, PERS(victim, ch, FALSE) );

    if (percent >= 100)
    strcat( buf, " appears to be perfectly healthy.\n\r");
    else if (percent >= 90)
    strcat( buf, " has a few minor cuts.\n\r");
    else if (percent >= 75)
    strcat( buf," is suffering from several wounds and bruises.\n\r");
    else if (percent >=  50)
    strcat( buf, " has quite a few injuries.\n\r");
    else if (percent >= 30)
    strcat( buf, " has been battered and bruised.\n\r");
    else if (percent >= 15)
    strcat ( buf, " needs serious medical attention soon.\n\r");
    else if (percent >= 0 )
    strcat (buf, " is in critical condition.\n\r");
    else
    strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char( "\n\r", ch);
    send_to_char( buf, ch );

    if (IS_AFFECTED(victim,AFF_CHARM) && victim->master == ch)
    {
      act("{B****  {xThis is your {w({BCharmed{w){x follower, and $E",ch,NULL,victim,TO_CHAR);
      sprintf(buf,"     will follow you for an additional %d hour%s. {B****{x\n\r",
            get_affect_duration(victim, gsn_charm_person), get_affect_duration(victim, gsn_charm_person) == 1 ? "" : "s");
      send_to_char(buf,ch);
    }

    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
    if ( ( obj = get_eq_char( victim, iWear ) ) != NULL
    &&   can_see_obj( ch, obj ) )
    {
        if ( !found )
        {
        send_to_char( "\n\r", ch );
        act( "$N is using:", ch, NULL, victim, TO_CHAR );
        found = TRUE;
        }
        send_to_char( where_name[iWear], ch );
        send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
        send_to_char( "\n\r", ch );
    }
    }

    if ( victim != ch
    &&   !IS_NPC(ch)
    &&   number_percent( ) < get_skill(ch,gsn_peek))
    {
    send_to_char( "\n\rYou peek at the inventory:\n\r", ch );
    check_improve(ch,gsn_peek,TRUE,4);
    show_list_to_char( victim->carrying, ch, TRUE, TRUE );
    }

    return;
}



void show_char_to_char( CHAR_DATA *list, CHAR_DATA *ch )
{
    CHAR_DATA *rch;

    for ( rch = list; rch != NULL; rch = rch->next_in_room )
    {
    if ( rch == ch || ( RIDDEN(rch) && rch->in_room == RIDDEN(rch)->in_room && RIDDEN(rch) != ch ) )
        continue;

    if ( get_trust(ch) < rch->invis_level)
        continue;

    if ( can_see( ch, rch ) )
    {
        show_char_to_char_0( rch, ch );
          if( MOUNTED(rch) && (rch->in_room == MOUNTED(rch)->in_room) )
                show_char_to_char_0( MOUNTED(rch), ch );
    }
    else if ( room_is_dark( ch->in_room ) && IS_AFFECTED(rch, AFF_INFRARED )
              && SAME_UMBRA(ch, rch))
    {
        send_to_char( "{yA pair of {rglowing red eyes{y lurks in the darkness.{x\n\r", ch );
    }
    }

    return;
}



bool check_blind( CHAR_DATA *ch )
{

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_HOLYLIGHT))
    return TRUE;

    if ( IS_AFFECTED(ch, AFF_BLIND) && !is_affected(ch, gsn_gift_scentofsight))
    {
    send_to_char( "You can't see a thing!\n\r", ch );
    return FALSE;
    }

    return TRUE;
}

/* changes your scroll */
void do_scroll(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    int lines;

    one_argument(argument,arg);

    if (arg[0] == '\0')
    {
    if (ch->lines == 0)
        send_to_char("You do not page long messages.\n\r",ch);
    else
    {
        sprintf(buf,"You currently display %d lines per page.\n\r",
            ch->lines + 2);
        send_to_char(buf,ch);
    }
    return;
    }

    if (!is_number(arg))
    {
    send_to_char("You must provide a number.\n\r",ch);
    return;
    }

    lines = atoi(arg);

    if (lines == 0)
    {
        send_to_char("Paging disabled.\n\r",ch);
        ch->lines = 0;
        return;
    }

    if (lines < 10 || lines > 100)
    {
    send_to_char("You must provide a reasonable number.\n\r",ch);
    return;
    }

    sprintf(buf,"Scroll set to %d lines.\n\r",lines);
    send_to_char(buf,ch);
    ch->lines = lines - 2;
}

/* RT does socials */
void do_socials(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int iSocial;
    int col;

    col = 0;

    for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
    {
    sprintf(buf,"%-12s",social_table[iSocial].name);
    send_to_char(buf,ch);
    if (++col % 6 == 0)
        send_to_char("\n\r",ch);
    }

    if ( col % 6 != 0)
    send_to_char("\n\r",ch);
    return;
}



/* RT Commands to replace news, motd, imotd, etc from ROM */

void do_motd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "motd");
}

void do_imotd(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "imotd");
}

void do_rules(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "rules");
}

void do_story(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "story");
}

void do_wizlist(CHAR_DATA *ch, char *argument)
{
    do_function(ch, &do_help, "wizlist");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

void do_autolist(CHAR_DATA *ch, char *argument)
{
    /* lists most player flags */
    char buf[MSL];

    if (IS_NPC(ch))
      return;

    sprintf(buf, "   {WCharacter Options:  {D%s{x\n\r", ch->name);
    send_to_char(buf, ch);
    send_to_char("   {WCommand    Status   Current Affect{x\n\r",ch);
    send_to_char("W{Y----<    COMBAT    >---------------------------------------------------------{xW\n\r",ch);

    send_to_char("autoassist     ",ch);
    if (IS_SET(ch->act,PLR_AUTOASSIST))
        send_to_char("{G* ON{x  {gWill automatically assist your party in a fight.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rWill not automatically assist your party in a fight.{x\n\r",ch);

    send_to_char("autogold       ",ch);
    if (IS_SET(ch->act,PLR_AUTOGOLD))
      send_to_char("{G* ON{x  {gWill automatically loot gold from corpses.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rWill not automatically loot gold from corpses.{x\n\r",ch);

    send_to_char("autoloot       ",ch);
    if (IS_SET(ch->act,PLR_AUTOLOOT))
        send_to_char("{G* ON{x  {gWill automatically loot items from corpses.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rWill not automatically loot items from corpses.{x\n\r",ch);

    send_to_char("autosac        ",ch);
    if (IS_SET(ch->act,PLR_AUTOSAC))
      send_to_char("{G* ON{x  {gWill automatically sacrifice a corpse upon killing mob.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rWill not automatically sacrifice corpse upon killing of mob.{x\n\r",ch);

    send_to_char("autosplit      ",ch);
    if (IS_SET(ch->act,PLR_AUTOSPLIT))
      send_to_char("{G* ON{x  {gWill automatically split looted gold evenly with party.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rWill not automatically split looted gold with party.{x\n\r",ch);

    send_to_char("combat         ",ch);
    if (IS_SET(ch->comm,COMM_COMBAT_BRIEF))
      send_to_char("{G* ON{x  {gPrevents extra combat descriptions.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rCombat descriptions will show.{x\n\r",ch);

    send_to_char("battleprompt   ",ch);
    if (IS_SET(ch->act2,PLR2_BATTLEPROMPT))
      send_to_char("{G* ON{x  {gShows info on who is tanking in group.{x\n\r",ch);
    else
      send_to_char("{RX OFF{x {rNo group tank info shown.{x\n\r",ch);

    send_to_char("W{y----<  NON-COMBAT  >---------------------------------------------------------{xW\n\r",ch);

    send_to_char("brief          ",ch);
    if (IS_SET(ch->comm, COMM_BRIEF))
    send_to_char("{G* ON{x  {gRoom descriptions hidden.{x\n\r",ch);
else
    send_to_char("{RX OFF{x {rFull room descriptions shown.{x\n\r",ch);

    send_to_char("autoexit       ",ch);
    if (IS_SET(ch->act,PLR_AUTOEXIT))
        send_to_char("{G* ON{x  {gObvious exits are displayed in each room.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rNo exits displayed in each room.{x\n\r",ch);

    send_to_char("colour         ",ch);
    if (IS_SET(ch->act,PLR_COLOUR))
        send_to_char("{G* ON{x  {gMUD is displayed with ANSI color.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rMUD will not show color.{x\n\r",ch);

    send_to_char("compact        ",ch);
    if (IS_SET(ch->comm,COMM_COMPACT))
        send_to_char("{G* ON{x  {gShows a space between the display and prompt.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rNo space between display and prompt.{x\n\r",ch);

    send_to_char("prompt         ",ch);
    if (IS_SET(ch->comm,COMM_PROMPT))
        send_to_char("{G* ON{x  {gShows your active prompt.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rNo prompt shown.{x\n\r",ch);

    send_to_char("combine        ",ch);
    if (IS_SET(ch->comm,COMM_COMBINE))
        send_to_char("{G* ON{x  {gStacked items in inventory.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rItems separated in inventory.{x\n\r",ch);

    send_to_char("noshort        ",ch);
    if (IS_SET(ch->act2,PLR2_NOSHORTDESC))
        send_to_char("{G* ON{x  {gShow Player name instead of short descriptions.{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rShow Player short description rather than name.{x\n\r",ch);

    send_to_char("noloot         ",ch);
    if (!IS_SET(ch->act,PLR_CANLOOT))
    send_to_char("{G* ON{x  {gYour corpse is safe from thieves (and other players).{x\n\r",ch);
    else
        send_to_char("{RX OFF{x {rYour corpse may be looted by other players.{x\n\r",ch);

    send_to_char("nosummon       ",ch);
    if (IS_SET(ch->act,PLR_NOSUMMON))
    send_to_char("{G* ON{x  {gYou cannot be summoned.{x\n\r",ch);
    else
    send_to_char("{RX OFF{x {rYou can be summoned.{x\n\r",ch);

    send_to_char("nofollow       ",ch);
    if (IS_SET(ch->act,PLR_NOFOLLOW))
    send_to_char("{G* ON{x  {gYou do not welcome followers.{x\n\r",ch);
    else
    send_to_char("{RX OFF{x {rYou accept followers.{x\n\r",ch);

    send_to_char("icmode         ",ch);
    if (IS_SET(ch->act,PLR_IC))
    send_to_char("{G* ON{x  {gYou are in IC mode.{x\n\r", ch);
    else
    send_to_char("{RX OFF{x {rYou are in OOC mode.{x\n\r", ch);

    send_to_char("rpavailable    ",ch);
    if (IS_SET(ch->act2,PLR2_RP_AVAILABLE))
    send_to_char("{G* ON{x  {gYou are displaying available for RP.{x\n\r", ch);
    else
    send_to_char("{RX OFF{x {rYou are not showing available for RP.{x\n\r", ch);

    send_to_char("M------------------------------------------------------------------------------M\n\r",ch);
}

void do_autoassist(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOASSIST))
    {
      send_to_char("Autoassist removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOASSIST);
    }
    else
    {
      send_to_char("You will now assist when needed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOASSIST);
    }
}

void do_autoexit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOEXIT))
    {
      send_to_char("Exits will no longer be displayed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOEXIT);
    }
    else
    {
      send_to_char("Exits will now be displayed.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOEXIT);
    }
}

void do_autogold(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOGOLD))
    {
      send_to_char("Autogold removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOGOLD);
    }
    else
    {
      send_to_char("Automatic gold looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOGOLD);
    }
}

void do_autoloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOLOOT))
    {
      send_to_char("Autolooting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOLOOT);
    }
    else
    {
      send_to_char("Automatic corpse looting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOLOOT);
    }
}

void do_autosac(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOSAC))
    {
      send_to_char("Autosacrificing removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSAC);
    }
    else
    {
      send_to_char("Automatic corpse sacrificing set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSAC);
    }
}

void do_autosplit(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_AUTOSPLIT))
    {
      send_to_char("Autosplitting removed.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_AUTOSPLIT);
    }
    else
    {
      send_to_char("Automatic gold splitting set.\n\r",ch);
      SET_BIT(ch->act,PLR_AUTOSPLIT);
    }
}

void do_brief(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_BRIEF))
    {
      send_to_char("Full descriptions activated.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_BRIEF);
    }
    else
    {
      send_to_char("Short descriptions activated.\n\r",ch);
      SET_BIT(ch->comm,COMM_BRIEF);
    }
}

void do_compact(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMPACT))
    {
      send_to_char("Compact mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMPACT);
    }
    else
    {
      send_to_char("Compact mode set.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMPACT);
    }
}

void do_combat(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBAT_BRIEF))
    {
      send_to_char("Brief Combat mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMBAT_BRIEF);
    }
    else
    {
      send_to_char("Brief Combat mode set.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMBAT_BRIEF);
    }
}

void do_prompt(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];

   if ( argument[0] == '\0' )
   {
    if (IS_SET(ch->comm,COMM_PROMPT))
    {
            send_to_char("You will no longer see prompts.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_PROMPT);
        }
        else
        {
            send_to_char("You will now see prompts.\n\r",ch);
            SET_BIT(ch->comm,COMM_PROMPT);
        }
       return;
   }

	else if ( !strcmp( argument, "show") )
    {
		sprintf(buf,"Your current prompt displays as: %s\n\r", ch->prompt);
		send_to_char(buf, ch);
		return;
    }

	else if ( !strcmp( argument, "raw"))
	{
        if (IS_NPC(ch))
            return;
		send_to_char("Your 'raw code' prompt setup is: ", ch);
		write_to_buffer( ch->desc, ch->prompt, 0);
		send_to_char("\n\r", ch);
		return;
	}
  if( !strcmp( argument, "all" ) )
      strcpy( buf, "{c<%hhp %mm %vmv>{x " );
   else
   {
      if ( strlen(argument) > 120 )
      {
          argument[100] = '\0';
          send_to_char("Prompt too long! Truncated.\n\r", ch);
      }
      strcpy( buf, argument );
      smash_tilde( buf );
      if (str_suffix("%c",buf))
    strcat(buf," ");

   }

   free_string( ch->prompt );
   ch->prompt = str_dup( buf );
   sprintf(buf,"Prompt set to %s\n\r",ch->prompt );
   send_to_char(buf,ch);
   return;
}

void do_combine(CHAR_DATA *ch, char *argument)
{
    if (IS_SET(ch->comm,COMM_COMBINE))
    {
      send_to_char("Long inventory selected.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_COMBINE);
    }
    else
    {
      send_to_char("Combined inventory selected.\n\r",ch);
      SET_BIT(ch->comm,COMM_COMBINE);
    }
}

void do_noloot(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_CANLOOT))
    {
      send_to_char("Your corpse is now safe from thieves.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_CANLOOT);
    }
    else
    {
      send_to_char("Your corpse may now be looted.\n\r",ch);
      SET_BIT(ch->act,PLR_CANLOOT);
    }
}

void do_nofollow(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_NOFOLLOW))
    {
      send_to_char("You now accept followers.\n\r",ch);
      REMOVE_BIT(ch->act,PLR_NOFOLLOW);
    }
    else
    {
      send_to_char("You no longer accept followers.\n\r",ch);
      SET_BIT(ch->act,PLR_NOFOLLOW);
      die_follower( ch );
    }
}

void do_ic(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (IS_SET(ch->act,PLR_IC))
    {
    if ( ch->position == POS_FIGHTING )
    {
        send_to_char("Not while your fighting you don't!\n\r",ch);
        WAIT_STATE(ch, 48);
        return;
    }
      send_to_char("{WYou switch to {C({DOOC{C) {Wmode.{x\n\r",ch);
      REMOVE_BIT(ch->act,PLR_IC);
      ch->pcdata->last_pose = 10;
      ch->pcdata->room_last_pose = 10;
    }
    else
    {
      send_to_char("{x{WYou have opted to go into {D({CIC{D) {Wmode for a roleplaying session.{x\n\r",ch);
        send_to_char("{RWarning{r! {WIdling while {D({CIC{D) {Wis illegal and will result in deletion of your character.\n\r{RAll {Wforms of triggers are illegal while {D({CIC{D){W. Disable all triggers {Gnow{W.{x\n\r",ch);
        SET_BIT(ch->act,PLR_IC);
        ch->pcdata->last_pose = 2;
        ch->pcdata->room_last_pose = 2;
    }
    return;
}

void do_rpavailable(CHAR_DATA *ch, char *argument)
{
  if (IS_NPC(ch))
    return;

    if (!IS_SET(ch->act2,PLR2_RP_AVAILABLE))
    {
      send_to_char("You now show as being available and looking for RP.\n\r",ch);
      SET_BIT(ch->act2,PLR2_RP_AVAILABLE);
    }
    else
    {
      send_to_char("You no longer show being available and looking for RP.\n\r",ch);
      REMOVE_BIT(ch->act2,PLR2_RP_AVAILABLE);
    }
  return;
}

void do_noshortdesc(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (!IS_SET(ch->act2,PLR2_NOSHORTDESC))
    {
      send_to_char("Player short descriptions will no longer be displayed.\n\r",ch);
      SET_BIT(ch->act2,PLR2_NOSHORTDESC);
    }
    else
    {
      send_to_char("Player short descriptions will now be displayed.\n\r",ch);
      REMOVE_BIT(ch->act2,PLR2_NOSHORTDESC);
    }
}

void do_battleprompt(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
      return;

    if (!IS_SET(ch->act2,PLR2_BATTLEPROMPT))
    {
      send_to_char("Battle prompt will now be displayed when in combat.\n\r",ch);
      SET_BIT(ch->act2,PLR2_BATTLEPROMPT);
    }
    else
    {
      send_to_char("Battle prompt will no longer be displayed in combat.\n\r",ch);
      REMOVE_BIT(ch->act2,PLR2_BATTLEPROMPT);
    }
}


void do_nosummon(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
    {
      if (IS_SET(ch->imm_flags,IMM_SUMMON))
      {
    send_to_char("You are no longer immune to summon.\n\r",ch);
    REMOVE_BIT(ch->imm_flags,IMM_SUMMON);
      }
      else
      {
    send_to_char("You are now immune to summoning.\n\r",ch);
    SET_BIT(ch->imm_flags,IMM_SUMMON);
      }
    }
    else
    {
      if (IS_SET(ch->act,PLR_NOSUMMON))
      {
        send_to_char("You are no longer immune to summon.\n\r",ch);
        REMOVE_BIT(ch->act,PLR_NOSUMMON);
      }
      else
      {
        send_to_char("You are now immune to summoning.\n\r",ch);
        SET_BIT(ch->act,PLR_NOSUMMON);
      }
    }
}

void do_look( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char *pdesc;
    int door;
    int number,count;
    bool nextline = FALSE;

    if ( ch->desc == NULL )
    return;

    if ( ch->position < POS_SLEEPING )
    {
    send_to_char( "You can't see anything but stars!\n\r", ch );
    return;
    }

    if ( ch->position == POS_SLEEPING )
    {
    send_to_char( "You can't see anything, you're sleeping!\n\r", ch );
    return;
    }

    if ( !check_blind( ch ) )
    return;

    if ( !IS_NPC(ch)
    &&   !IS_SET(ch->act, PLR_HOLYLIGHT)
    &&   room_is_dark( ch->in_room )
    && !IS_AFFECTED(ch, AFF_DARK_VISION))
    {
    send_to_char( "It is {Dpitch black{x ... \n\r", ch );
    show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
    show_char_to_char( ch->in_room->people, ch );
    return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    number = number_argument(arg1,arg3);
    count = 0;
    ACT_LOOKING = TRUE;

    if ( arg1[0] == '\0' || !str_cmp( arg1, "auto" ) )
    {
    /* 'look' or 'look auto' */
    if(IS_AFFECTED2(ch,AFF2_UMBRA))
      send_to_char("{W+============================={m[      Umbra      ]{x==============================+{x\n\r", ch);
    else
      send_to_char("{W+==============================================================================+{x\n\r", ch);
    if ( (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act,PLR_HOLYLIGHT)))
    ||   IS_BUILDER(ch, ch->in_room->area) )
        sprintf(buf,"{W|Room %6d| ",ch->in_room->vnum);
    else
      sprintf(buf,"{W| ");
    send_to_char(buf,ch);

    sprintf(buf,"%s{x\n\r", ch->in_room->name);
    send_to_char(buf,ch);

    if(ch->avatar > 0 && is_affected(ch, gsn_spiritsight))
    {
        sprintf(buf,"{W| [{yGauntlet{W: %d] {x",get_gauntlet(ch));
        send_to_char(buf,ch);
        nextline = TRUE;
    }
    	if(ch->clan && ch->in_room->area->domain)
	{
		sprintf(buf,"{W| [{rInfluence{W: {D%d{x]",ch->in_room->area->domain->influence[ch->clan]);
		send_to_char(buf,ch);
    nextline = TRUE;
	}

    if (nextline)
      send_to_char("\n\r", ch);

    if((ch->avatar > 0) && (ch->in_room->sector_type == SECT_NODE) && is_affected(ch, gsn_primesense))
      send_to_char("{W| {MYou feel the pull and tug of raw Quintessential energy gathering in the room.{x\n\r", ch);
    send_to_char("{W+==============================================================================+{x\n\r", ch);

    if ( arg1[0] == '\0'
    || ( !IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF) ) )
    {
        send_to_char( "  ",ch);

        if(IS_AFFECTED2(ch, AFF2_UMBRA) && (pdesc = get_extra_descr("umbra_desc",ch->in_room->extra_descr)) != NULL)
            send_to_char(pdesc,ch);
        else
            send_to_char( ch->in_room->description, ch );
/*Extra descriptions seen by players with Auspex 1 active - Sengir*/
        if(!IS_AFFECTED2(ch, AFF2_UMBRA) &&
        (is_affected(ch, gsn_gift_heightenedsenses) || is_affected(ch, gsn_reveal)) &&
        (pdesc = get_extra_descr("sense_room_desc",ch->in_room->extra_descr)) != NULL)
        {
            send_to_char("\n\r{WYour extraordinary senses detect something about this room:{x\n\r", ch);
            send_to_char(pdesc,ch);
        }
    }

        if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT) )
    {
        send_to_char("\n\r",ch);
            do_function(ch, &do_exits, "auto" );
    }
    if(!IS_NPC(ch) && ch->pcdata->cssec_abil[CSABIL_PROSPECTING] == 5 && get_obj_hidden( ch, "vein" ) && number_range(1,5) == 5)
    {
        send_to_char("{YYou sense that there may be something worth searching for here.{x\n\r",ch);
        return;
    }
    send_to_char("{y",ch);
    show_list_to_char( ch->in_room->contents, ch, FALSE, FALSE );
    show_char_to_char( ch->in_room->people,   ch );
    send_to_char("{x",ch);
    return;
    }

    if ( !str_cmp( arg1, "i" ) || !str_cmp(arg1, "in")  || !str_cmp(arg1,"on"))
    {
    /* 'look in' */
    if ( arg2[0] == '\0' )
    {
        send_to_char( "Look in what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here( ch, NULL, arg2 ) ) == NULL )
    {
        send_to_char( "You do not see that here.\n\r", ch );
        return;
    }

    switch ( obj->item_type )
    {
    default:
        send_to_char( "That is not a container.\n\r", ch );
        break;

    case ITEM_DRINK_CON:
        if ( obj->value[1] <= 0 )
        {
        send_to_char( "It is empty.\n\r", ch );
        break;
        }

        sprintf( buf, "It's %sfilled with  a %s liquid.\n\r",
        obj->value[1] <     obj->value[0] / 4
            ? "less than half-" :
        obj->value[1] < 3 * obj->value[0] / 4
            ? "about half-"     : "more than half-",
        liq_table[obj->value[2]].liq_color
        );

        send_to_char( buf, ch );
        break;

    case ITEM_CONTAINER:
    case ITEM_CORPSE_NPC:
    case ITEM_CORPSE_PC:
        if ( IS_SET(obj->value[1], CONT_CLOSED) )
        {
        send_to_char( "It is closed.\n\r", ch );
        break;
        }

        act( "$p holds:", ch, obj, NULL, TO_CHAR );
        show_list_to_char( obj->contains, ch, TRUE, TRUE );
        break;
    }
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) != NULL )
    {
    show_char_to_char_1( victim, ch );
    return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
    if ( can_see_obj( ch, obj ) )
    {  /* player can see object */
        pdesc = get_extra_descr( arg3, obj->extra_descr );
        if ( pdesc != NULL )
            {
                if (++count == number)
            {
            send_to_char( pdesc, ch );
            if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
              act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
            }
            else continue;
            }

        pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
        if ( pdesc != NULL )
            {
                if (++count == number)
            {
            send_to_char( pdesc, ch );
            if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
              act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
            }
        else continue;
            }

        if ( is_name( arg3, obj->name ) )
            if (++count == number)
            {
                send_to_char( obj->description, ch );
                send_to_char( "\n\r",ch);
                if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
                  act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
          }
      }
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
    {
    if ( can_see_obj( ch, obj ) )
    {
        pdesc = get_extra_descr( arg3, obj->extra_descr );
        if ( pdesc != NULL )
            if (++count == number)
            {
            send_to_char( pdesc, ch );
            if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
              act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
            }

        pdesc = get_extra_descr( arg3, obj->pIndexData->extra_descr );
        if ( pdesc != NULL )
            if (++count == number)
            {
            send_to_char( pdesc, ch );
            if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
              act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
            }

        if ( is_name( arg3, obj->name ) )
        if (++count == number)
        {
            send_to_char( obj->description, ch );
            send_to_char("\n\r",ch);
            if(HAS_REFLECTION(ch) && IS_REFLECTIVE(obj))
              act("{WYou catch a glimpse of your reflection in $p.{x", ch, obj, NULL, TO_CHAR);
            return;
        }
    }
    }

    pdesc = get_extra_descr(arg3,ch->in_room->extra_descr);
    if (pdesc != NULL)
    {
    if (++count == number)
    {
        send_to_char(pdesc,ch);
        return;
    }
    }

    if (count > 0 && count != number)
    {
        if (count == 1)
            sprintf(buf,"You only see one %s here.\n\r",arg3);
        else
            sprintf(buf,"You only see %d of those here.\n\r",count);

        send_to_char(buf,ch);
        return;
    }

         if ( !str_cmp( arg1, "n" ) || !str_cmp( arg1, "north" ) ) door = 0;
    else if ( !str_cmp( arg1, "e" ) || !str_cmp( arg1, "east"  ) ) door = 1;
    else if ( !str_cmp( arg1, "s" ) || !str_cmp( arg1, "south" ) ) door = 2;
    else if ( !str_cmp( arg1, "w" ) || !str_cmp( arg1, "west"  ) ) door = 3;
    else if ( !str_cmp( arg1, "u" ) || !str_cmp( arg1, "up"    ) ) door = 4;
    else if ( !str_cmp( arg1, "d" ) || !str_cmp( arg1, "down"  ) ) door = 5;
    else
    {
    send_to_char( "You do not see that here.\n\r", ch );
    return;
    }

    /* 'look direction' */
    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
    send_to_char( "Nothing special there.\n\r", ch );
    return;
    }

    if ( pexit->description != NULL && pexit->description[0] != '\0' )
    send_to_char( pexit->description, ch );
    else
    send_to_char( "Nothing special there.\n\r", ch );

    if ( pexit->keyword    != NULL
    &&   pexit->keyword[0] != '\0'
    &&   pexit->keyword[0] != ' ' )
    {
    if ( IS_SET(pexit->exit_info, EX_CLOSED) )
    {
        act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
    }
    else if ( IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
        act( "The $d is open.",   ch, NULL, pexit->keyword, TO_CHAR );
    }
    }

    return;
}

void do_examine( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char condition[3];
    OBJ_DATA *obj;
    AFFECT_DATA *paf;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	     send_to_char( "Examine what?\n\r", ch );
	     return;
    }

    if (!str_prefix(arg, "room"))
    {
      ROOM_INDEX_DATA *room;
      room = ch->in_room;
      send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
      sprintf( buf," |-- %s --|\n\r", center(room->name, 70, " "));
  	  send_to_char( buf, ch );
      if ((ch->in_room->sector_type == SECT_NODE) && IS_AFFECTED(ch, AFF_DETECT_MAGIC))
        send_to_char(" |         {Y*****  Mystical Energy  *****{x ebbs and flows freely here.          |\n\r", ch);
      send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
      send_to_char(" |  The surrounding area is ", ch);
       switch (room->sector_type)
       {
         case SECT_INSIDE:
              send_to_char("inside a structure.                               | \n\r", ch);
              break;
         case SECT_CITY:
              send_to_char("within the city limits.                           | \n\r", ch);
              break;
         case SECT_FIELD:
              send_to_char("an open field.                                    | \n\r", ch);
              break;
         case SECT_FOREST:
              send_to_char("covered by trees.                                 | \n\r", ch);
              break;
         case SECT_HILLS:
              send_to_char("full of rolling hillsides.                        | \n\r", ch);
              break;
         case SECT_WATER_SWIM:
         case SECT_WATER_NOSWIM:
         case SECT_WATER_DROWN:
              send_to_char("covered by water.                                 | \n\r", ch);
              break;
         case SECT_AIR:
              send_to_char("wide open sky.                                    | \n\r", ch);
              break;
         case SECT_DESERT:
              send_to_char("dry, sandy and barren.                            | \n\r", ch);
              break;
         case SECT_HOT:
              send_to_char("hot and sweltering.                               | \n\r", ch);
              break;
         case SECT_COLD:
              send_to_char("cold and freezing.                                | \n\r", ch);
              break;
         default:
              send_to_char("indescribable.                                    | \n\r", ch);
              break;
       }

      if (has_room_bit(room->room_flags, "dark"))
        send_to_char(" |  There doesn't seem to be any natural lighting.                            |\n\r", ch);
      if (has_room_bit(room->room_flags, "indoors"))
        send_to_char(" |  The area is sheltered from the outside world.                             |\n\r", ch);
      if (has_room_bit(room->room_flags, "bank"))
        send_to_char(" |  Bank transactions are possible here.                                      |\n\r", ch);
      if (has_room_bit(room->room_flags, "underwater"))
        send_to_char(" |  The entire area is submerged.                                             |\n\r", ch);
      if (has_room_bit(room->room_flags, "forge"))
        send_to_char(" |  You can forge materials here.                                             |\n\r", ch);
      send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
      return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
      EXIT_DATA *pexit;
      pexit = ch->in_room->exit[door];

      send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
      send_to_char(" |-----------------------=====  Exit Information  =====-----------------------|  \n\r",ch);

      if ( IS_SET(pexit->exit_info, EX_CLOSED))
         send_to_char( " |   That exit appears to be closed.                                          |\n\r", ch );
      if ( IS_SET(pexit->exit_info, EX_LOCKED))
         send_to_char( " |   The exit has been locked.                                                |\n\r",  ch );
      if ( IS_SET(pexit->exit_info, EX_PICKPROOF))
         send_to_char( " |   The lock cannot be picked.                                               |\n\r", ch );
      if ( IS_SET(pexit->exit_info, EX_EASY))
         send_to_char( " |   The lock should be child's play to pick.                                 |\n\r", ch );
      if ( IS_SET(pexit->exit_info, EX_HARD))
         send_to_char( " |   This lock might be a little difficult to pick.                           |\n\r", ch );
      if ( IS_SET(pexit->exit_info, EX_INFURIATING))
         send_to_char( " |   This lock will take the highest skill to pick.                           |\n\r", ch );

      send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
      return;
    }

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
    {
	int cond = obj->condition;
	sprintf( condition, "%s", cond == 0 ? "{D" : cond <= 15 ? "{r" : cond <= 45 ? "{y" : cond < 100 ? "{g" : "{W");

	send_to_char( condition, ch );
	send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------()\n\r",ch);
   	sprintf( buf,"%s | --{x Level %3d  %s::{x  %s\n\r", condition, obj->level, condition, obj->short_descr);
	send_to_char( buf, ch );
    if (IS_OBJ_STAT(obj, ITEM_MAGIC) && IS_AFFECTED(ch, AFF_DETECT_MAGIC))
    {
        sprintf(buf, "%s | --{x Mystical {YEnergy{x permeates %s.\n\r", condition, obj->short_descr);
        send_to_char( buf, ch );
    }
    send_to_char( condition, ch );
	send_to_char(" |-----------------------==============================-----------------------|{x\n\r",ch);
	sprintf( buf,"%s | --{x Keywords: %s\n\r", condition, obj->name );
	send_to_char( buf, ch );

	sprintf( buf,"%s | --{x %s\n\r", condition, obj->description );
	send_to_char( buf, ch );

	sprintf( buf,"%s | --{x This item weighs approximately %d.%d pounds.\n\r", condition, obj->weight / 10, obj->weight % 10);
	send_to_char( buf, ch );

	send_to_char( condition, ch );
	send_to_char(" |-----------------------==============================-----------------------|\n\r",ch);
    sprintf( buf, "");
    if (obj->item_type == ITEM_CONTAINER)
    sprintf( buf, "%s | --{x Contains: {Y%d/%d {xmax items, combined weight of {y%d.%d/%d.%d{x lbs\n\r", condition,
            get_obj_number(obj), obj->value[3], get_obj_weight(obj) / 10, get_obj_weight(obj) % 10,
            obj->value[0] / 10, obj->value[0] % 10);
    if (obj->item_type == ITEM_LIGHT)
    sprintf( buf,"%s | --{x This light will remain lit for %s\n\r", condition, obj->value[2] > 75 ? "an extremely long time." :
            obj->value[2] > 50 ? "quite a while." : obj->value[2] > 25 ? "a bit longer." :
            obj->value[2] > 10 ? "just a little longer." : "only a short time longer.");
    if (obj->item_type == ITEM_FOOD)
    if (obj->timer <= 0)
      sprintf( buf, "%s | --{x This food appears to have been preserved.\n\r", condition);
    else
      sprintf( buf, "%s | --{x This food %s\n\r", condition, obj->timer >= 10 ? "is fresh." : "will spoil soon.");
    if (obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC)
        sprintf( buf,"%s | --{x Factoring in any contents inside, the total weight is approximately %5d pounds.\n\r", condition, get_obj_weight(obj) / 10);
    if (obj->item_type == ITEM_DRINK_CON)
        if ( obj->value[1] <= 0 )
            sprintf( buf, "%s | --{x It is empty.\n\r", condition);
        else
            sprintf( buf, "%s | --{x It's %sfilled with  a %s liquid.\n\r", condition,
                obj->value[1] < obj->value[0] / 4 ? "less than half-" :
                obj->value[1] < 3 * obj->value[0] / 4 ? "about half-" : "more than half-",
                liq_table[obj->value[2]].liq_color);
    if (obj->item_type == ITEM_TOOL)
        sprintf( buf, "%s | --{x This can be used as a %s.\n\r", condition, tool_table[obj->value[1]].tool_name);
    if (obj->item_type == ITEM_BLASTPOWDER)
        sprintf( buf, "%s | --{x This %s powder has %s.\n\r", condition,
            obj->value[1] < 3 ? "weak" : obj->value[1] < 5 ? "average" :
            obj->value[1] < 8 ? "potent" : "devastating",
            obj->value[0] < 2 ? "a single charge left" :
            obj->value[0] < 5 ? "very few charges left" :
            "quite a few charges left");
    if (obj->item_type == ITEM_WEAPON)
        sprintf( buf, "%s | --{x This would make a useful %s.\n\r", condition,
        obj->value[0] == WEAPON_EXOTIC ? "improvised weapon" : obj->value[0] == WEAPON_SWORD ? "sword" :
        obj->value[0] == WEAPON_DAGGER ? "dagger" : obj->value[0] == WEAPON_SPEAR ? "spear/staff" :
        obj->value[0] == WEAPON_MACE ? "mace/club" : obj->value[0] == WEAPON_AXE ? "axe" :
        obj->value[0] == WEAPON_FLAIL ? "flail" : obj->value[0] == WEAPON_WHIP ? "whip" :
        obj->value[0] == WEAPON_POLEARM ? "polearm" : obj->value[0] == WEAPON_LANCE ? "lance" :
        "unknown");
    if (obj->item_type == ITEM_PLASM)
        sprintf( buf, "%s | --{x There appear to be %2d unit%s usable.\n\r", condition, obj->value[0], obj->value[0] > 1 ? "s" : "");
    send_to_char( buf, ch );
    if ((ch->race == race_lookup("garou") || is_affected(ch, gsn_spiritsight)) && 
        affect_find(obj->affected,skill_lookup("fetish")) != NULL)
    {
        sprintf( buf, "%s |-----------------------======{mSpiritual  Essence%s======-----------------------|\n\r", 
            condition, condition);
        send_to_char( buf, ch );
        sprintf(buf, "%s | --{x The {mSpirit{x within this Fetish adds the following:\n\r", condition);
        send_to_char( buf, ch );
        for ( paf = obj->affected; paf != NULL; paf = paf->next )
        {
            if (paf->type == gsn_fetish)
            {
                sprintf( buf, "%s |    {x'%s (%d)' -> %3d hours\n\r", condition,
                affect_loc_name( paf->location ), paf->modifier, paf->duration );
                if (str_cmp(affect_loc_name( paf->location ), "none") && paf->modifier == 0)
                    send_to_char("",ch);
                else
                    send_to_char( buf, ch );
                if (paf->bitvector)
                {
                    switch(paf->where)
                    {
                        case TO_AFFECTS:
                            sprintf(buf,"%s |{x    +[{M%s{x]\n", condition,
                            affect_bit_name(paf->bitvector));
                            break;
                        case TO_WEAPON:
                            sprintf(buf,"%s |{x    +[{M%s{x]\n", condition,
                            weapon_bit_name(paf->bitvector));
                            break;
                        case TO_OBJECT:
                            sprintf(buf,"%s |{x    +[{M%s{x]\n", condition,
                            extra_bit_name(paf->bitvector));
                            break;
                        case TO_IMMUNE:
                            sprintf(buf,"%s |{x    Immunity to [{M%s{x]\n", condition,
                            imm_bit_name(paf->bitvector));
                            break;
                        case TO_RESIST:
                            sprintf(buf,"%s |{x    Resistance to [{M%s{x]\n\r", condition,
                            imm_bit_name(paf->bitvector));
                            break;
                        case TO_VULN:
                            sprintf(buf,"%s |{x    Vulnerability to [{M%s{x]\n\r", condition,
                            imm_bit_name(paf->bitvector));
                        break;
                        default:
                            sprintf(buf,"%s |{x    Unknown bit [{M%d: %d{x]\n\r", condition,
                            paf->where,paf->bitvector);
                        break;
                    }
                    send_to_char(buf,ch);
                }
            }
        }
    }
    send_to_char( condition, ch );
	send_to_char("()-----------------------=======ooooOOOOOOOOoooo=======-----------------------(){x\n\r",ch);
    }

    else
	send_to_char( "You don't see that item here.\n\r", ch);

    return;
}


/*
 * Thanks to Zrin for auto-exit part.
 */
void do_exits( CHAR_DATA *ch, char *argument )
{
    extern char * const dir_name[];
    char buf[MAX_STRING_LENGTH], buf2[MAX_STRING_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool fAuto;
    int door;

    fAuto  = !str_cmp( argument, "auto" );

    if ( !check_blind( ch ) )
    return;

    if (fAuto)
    sprintf(buf,"[Exits:");
    else if (IS_IMMORTAL(ch))
    sprintf(buf,"Obvious exits from room %d:\n\r",ch->in_room->vnum);
    else
    sprintf(buf,"Obvious exits:\n\r");

    found = FALSE;
    for ( door = 0; door <= 5; door++ )
    {
    if ( (pexit = ch->in_room->exit[door]) != NULL &&
         pexit->u1.to_room != NULL && can_see_room(ch,pexit->u1.to_room))
    {
        if(IS_SET(pexit->exit_info, EX_HIDDEN) && !IS_IMMORTAL(ch))
        continue;

        found = TRUE;
        if ( fAuto )
        {
        strcat( buf, " " );
        if ( IS_SET(pexit->exit_info, EX_HIDDEN))
            strcat( buf, "*" );
        else
            if ( IS_SET(pexit->exit_info, EX_CLOSED))
                strcat( buf, "<" );
        if (IS_SET(pexit->u1.to_room->room_flags, ROOM_UMBRA))
            sprintf(buf2, "{m%s{x",dir_name[door]);
        else
            sprintf(buf2, "{C%s{x",dir_name[door]);

        strcat( buf, buf2);
        if ( IS_SET(pexit->exit_info, EX_HIDDEN))
            strcat( buf, "*" );
        else
            if ( IS_SET(pexit->exit_info, EX_CLOSED))
                strcat( buf, ">" );
        }
        else
        {
        sprintf( buf + strlen(buf), "%-5s - %s",
            capitalize( dir_name[door] ),
            room_is_dark( pexit->u1.to_room )
            ?  "Too dark to tell"
            : pexit->u1.to_room->name
            );
        if (IS_IMMORTAL(ch))
            sprintf(buf + strlen(buf),
            " (room %d)\n\r",pexit->u1.to_room->vnum);
        else
            sprintf(buf + strlen(buf), "\n\r");
        }
    }
    }

    if ( !found )
    strcat( buf, fAuto ? " none" : "None.\n\r" );

    if ( fAuto )
    strcat( buf, "]\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_worth( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
    {
    sprintf(buf,"You have %ld gold and %ld silver.\n\r",
        ch->gold,ch->silver);
    send_to_char(buf,ch);
    return;
    }

    sprintf(buf,
    "You are currently carrying %ld {Ygold {xand %ld {Wsilver{x.\n\rYou have accumulated %d {gexperience {xduring your travels, and require\n\r%d {Gexperience {xto reach the next level.\n\r",
    ch->gold, ch->silver,ch->exp,
    (ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp);

    send_to_char(buf,ch);

    return;
}


void do_score( CHAR_DATA *ch, char *argument )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	BUFFER *output;
	output = new_buf();
	int hours = (ch->played + (int) (current_time - ch->logon) ) / 3600;
  OBJ_DATA *wield;
  wield = get_eq_char( ch, WEAR_WIELD );
  int basedamage = get_attribute(ch, STRENGTH);
  int baseattack = get_attribute(ch, DEXTERITY);

  //Basic Attack Roll Calculations for D10
  if (wield)
    baseattack += ch->csabilities[CSABIL_MELEE];
  else
    baseattack += ch->csabilities[CSABIL_BRAWL];
  baseattack += GET_HITROLL(ch) / 200;
  //Basic Damage Calculations for D10
  if (wield)
    basedamage += (wield->value[1])/20;
  if (!wield && (is_affected(ch, gsn_claws) || is_affected(ch, gsn_wingclaws)))
    basedamage += 2;
  if (!wield && (is_affected(ch, gsn_gift_razorclaws) || is_affected(ch, gsn_vicissitude_horrid)))
    basedamage += 1;
  if (!IS_NPC(ch) && IS_VAMP(ch))
    basedamage += ch->pcdata->discipline[POTENCE];
  if (!IS_NPC(ch) && IS_MAGE(ch) && ch->sphere[SPHERE_MATTER] > 0)
    basedamage += 1+ch->sphere[SPHERE_MATTER]/2;

	add_buf(output, "\n\r{w                          __, _, ____,__  _,____,_,  _,{x\n\r");
	add_buf(output, "{w                         (-|__| (-/_|(-\\  /(-|_,(-|\\ | {x\n\r");
	add_buf(output, "{D     |      _____________/{w_|  |,_/  |, _\\/  _|__,_| \\|,{D\\_____________      |{x\n\r");
	sprintf(buf2,"{W%s{D",ch->name);
	sprintf(buf,"{D     |\\____/  %s  \\____/|\n\r",center(buf2,53,"_"));
	add_buf(output,buf);
	sprintf(buf,"{D     |  _____/   {wLevel: {D%3d{w    Exp TNL: {D%8d{w    Remorts: {D%4d{D  \\_____  |{x\n\r",(int) ch->level,IS_NPC(ch) ? 0 : ((ch->level + 1) * exp_per_level(ch,ch->pcdata->points) - ch->exp),ch->remorts);
	add_buf(output,buf);
	sprintf(buf,"{D     | |{w  HP:   {g%5d{x/{g%5d{w    Agg Damage: {r%5d{w    Play Hours:   {D%5d {D | |{x\n\r",(int) ch->hit,(int) ch->max_hit,(int) ch->agg_dam,hours);
	add_buf(output,buf);
	sprintf(buf,"{D     |/ {w  Mana: {c%5d{x/{c%5d{w    Hitroll:    {c%5d{w    Hours IC:     {c%5d   {D\\|{x\n\r",(int) ch->mana, (int)ch->max_mana, GET_HITROLL(ch), IS_NPC(ch) ? 0 : ch->pcdata->IC_total/60);
	add_buf(output,buf);
	sprintf(buf,"{D     |  {w  Move: {y%5d{x/{y%5d{w    Damroll:    {c%5d{w    Freebies:     {c%5d    {D|{x\n\r", (int) ch->move,(int) ch->max_move, GET_DAMROLL(ch), ch->freebie);
	add_buf(output,buf);
	sprintf(buf,"{D     |{w    Armor:     {wPierce:{D%5d{w  Bash:{D%5d{w  Slash:{D%5d{w  Magic:{D%5d    {D|{x\n\r", GET_AC(ch,AC_PIERCE),GET_AC(ch,AC_BASH),GET_AC(ch,AC_SLASH),GET_AC(ch,AC_EXOTIC));
	add_buf(output,buf);
	add_buf(output,"{D     |    {wGold:      Silver:   In Bank:     Carried:     Weight:           {D|{x\n\r");
	sprintf(buf,"{D     |    {y%-9d{x",ch->gold);
	add_buf(output,buf);
	sprintf(buf,"  {w%-9ld{x",ch->silver);
	add_buf(output,buf);
	sprintf(buf," {y%-9d{x",ch->pcdata->bank);
	add_buf(output,buf);
	sprintf(buf,"  {c%5hd{x/{D%-4d   {c%7ld{w/{D%-7d{D     {D|{x\n\r",ch->carry_number,can_carry_n(ch),get_carry_weight(ch)/10, can_carry_w(ch)/10);
	add_buf(output,buf);

	if(!IS_NPC(ch))
	{
		/* Not sure where to put this. We can make it a line of Alert style data that we can add to in the future if needed. */
		if(IS_SET(ch->act2, PLR2_PVP))
		{
			sprintf(buf,"{D     |{w %s {D|{x\n\r",center("{wYour character is PVP active.{x",67, " "));
			add_buf(output,buf);
		}

    /*D10 Relevant Info*/
		add_buf(output,"     {D|>--------------------------{WD10 Information{D--------------------------<|{x\n\r");
    sprintf(buf,"{D     |{w    Armor Dice:     {wPierce:{D%3d{w   Bash:{D%3d{w   Slash:{D%3d{w   Other:{D%3d    {D|{x\n\r", get_armor_diff(ch, ch, DAM_PIERCE), get_armor_diff(ch, ch, DAM_BASH), get_armor_diff(ch, ch, DAM_SLASH), get_armor_diff(ch, ch, DAM_ENERGY));
    add_buf(output, buf);
	  sprintf(buf,"{D     |  {w  Attack Roll Dice: {c%2d                   {xAttack Damage Dice: {c%2d    {D|{x\n\r", baseattack, basedamage);
    add_buf(output, buf);
		if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
		{
			add_buf(output,"     {D|>------------------------------{WVampire{D------------------------------<|{x\n\r");
			sprintf(buf2,"{wYou have been a %s member of Clan %s for %d years.{x",ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
				capitalize(clan_table[ch->clan].name), get_age(ch));
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output,buf);
			sprintf(buf2,"{w%s leads your Clan. There is {Y%d{w in the Clan bank.{x",capitalize(clan_table[ch->clan].leader),clan_table[ch->clan].bank);
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output,buf);
		}

		/* Garou stuff. Seems like more info is needed. */
		if(ch->race == race_lookup("garou"))
		{
			add_buf(output,"     {D|>-------------------------------{WGarou{D-------------------------------<|{x\n\r");
			if(ch->pcdata->breed == 0 && ch->pcdata->auspice == 0)
				add_buf(output,"          {rFinish Garou creation in order to see your Garou stats.{x\n\r");
			else
			{
				sprintf(buf2,"{wYou are a %d year old %s %s %s of Tribe %s.{x",get_age(ch),ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
					ch->pcdata->breed == LUPUS ? "Lupus" : ch->pcdata->breed == METIS ? "Metis" : "Homid",
					ch->pcdata->auspice == RAGABASH ? "Ragabash" : ch->pcdata->auspice == THEURGE ?
					"Theurge" : ch->pcdata->auspice == PHILODOX ? "Philodox" : ch->pcdata->auspice == GALLIARD ?
					"Galliard" : "Ahroun",  capitalize(clan_table[ch->clan].name));
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
				add_buf(output,buf);
				sprintf(buf2,"{w%s leads your Tribe. There is {Y%d{w in the tribe bank.{x",capitalize(clan_table[ch->clan].leader),clan_table[ch->clan].bank);
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
				add_buf(output,buf);
			}
		}

		if (ch->race == race_lookup("ghoul"))
		{
			add_buf(output,"     {D|>-------------------------------{WGhoul{D-------------------------------<|\n\r");
			sprintf(buf2, "{wYou are a %d year old %s Ghoul in service to clan %s.{x", get_age(ch), ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
				capitalize(clan_table[ch->clan].name));
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output, buf);
			sprintf(buf2, "{wYou have {r%d{w of your master's Vitae left in your system.{x",ch->pblood/10);
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output, buf);
		}
        if (ch->race == race_lookup("human") && ch->avatar > 0)
        {
			add_buf(output,"     {D|>--------------------------------{WMage{D-------------------------------<|{x\n\r");
			sprintf(buf2, "{wYou are a %d year old %s Mage of the %s.{x", get_age(ch),
                ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female", capitalize(clan_table[ch->clan].name));
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output, buf);
				sprintf(buf2,"{wAvatar: {W%d  {wArete: {W%d  {wQuintessence: {C%d  {wParadox: {R%d{x ",ch->avatar,ch->arete,
					ch->quintessence,ch->paradox);
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
                add_buf(output, buf);
                sprintf(buf2,"{wRank: {D%s  {wMentor: {D%s{w  Apprentice: {D%s {wLeader: {D%s {x",
                    ch->rank == 1 ? "Apprentice" : ch->rank == 2 ? "Disciple" : ch->rank == 8 ? "Master" : ch->rank == 9 ? "Mentor" : "Leader",
                    ch->sire != NULL ? ch->sire : "none", ch->apprentice == NULL ? "None" : ch->apprentice,
                    capitalize(clan_table[ch->clan].leader)
                    );
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
                add_buf(output, buf);
                sprintf(buf2,"{wYour Tradition has {Y%d{w in the bank.{x",clan_table[ch->clan].bank);
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
				add_buf(output,buf);
        }

		if (ch->race == race_lookup("human") && ch->avatar == 0)
		{
			add_buf(output,"     {D|>-------------------------------{WHuman{D-------------------------------<|{x\n\r");
			sprintf(buf2,"{wYou are a %d year old %s human.{x",get_age(ch),ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female");
			sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
			add_buf(output, buf);
		}

		if (IS_IMMORTAL(ch) || (!IS_NPC(ch) && ch->pcdata->ip > 0))
		{
			add_buf(output,"     {D|>------------------------------{WImmortal{D-----------------------------<|{x\n\r");
			if(IS_IMMORTAL(ch))
			{
				sprintf(buf2,"{wWizinvis: {C%d  {wIncog: {C%d  {wTrust: {C%d  {wHolylight: {C%s{x ",ch->invis_level,ch->incog_level,
					get_trust(ch),IS_SET(ch->act,PLR_HOLYLIGHT) ? "On" : "Off");
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
				add_buf(output,buf);
				if(!IS_NPC(ch))
				{
					sprintf(buf2,"{wWiziname: {C%s  {wWiziname is set to %s{x",IS_SET(ch->comm, COMM_WIZINAME) ? "On" : "Off",
						!IS_NULLSTR(ch->pcdata->wiziname) ? ch->pcdata->wiziname : "Unset");
					sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
					add_buf(output,buf);
				}
			}
			if(!IS_NPC(ch) && ch->pcdata->ip > 0)
			{
				sprintf(buf2,"{wImmortal Points: {C%d{x",ch->pcdata->ip);
				sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
				add_buf(output,buf);
			}
		}

		add_buf(output,"     {D|>----------------------------{WAchievement{D----------------------------<|{x\n\r");

		sprintf(buf2,"{wKills: {y%d  {wCurrent Kills: {y%d  {wTop Dam: {y%d{x", ch->totalkills,ch->currentkills,ch->maxdamage);
		sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
		add_buf(output,buf);

    sprintf(buf2,"{wQpoints: {y%d  {wTotal Qpoints: {y%d{x", ch->qpoints, ch->totalqpoints);
		sprintf(buf,"{D     |%s{D|{x\n\r",center(buf2,69," "));
		add_buf(output,buf);
	}
	/* End of NPC check block */

	add_buf(output,"{D     |           ______________________________________________            |{x\n\r");
	add_buf(output,"{D     |/\\________/    {wUse CS, CS2, CS3 and BG for more info{D     \\_________/\\|{x\n\r");
    page_to_char(buf_string(output),ch);
    free_buf(output);
}

void do_clans (CHAR_DATA *ch, char *argument)
{
        char buf[MAX_INPUT_LENGTH], buf1[MIL];
    int i, col = 0;

    buf1[0] = '\0';
    sendch("================== Active Clans ==================\n\r", ch);

    for (i = 0; i < MAX_CLAN; i++)
    {
        if (clan_table[i].who_name != NULL
                && clan_table[i].who_name[0] != '\0')
        {
                sprintf (buf, "{c%-27.27s %s{x",
                        clan_table[i].who_name,
                        clan_table[i].leader);
                sendch (buf, ch);

                if (++col % 1 == 0)
                        sendch ("\n\r", ch);
        }
    }
    if (col % 1 != 0)
        sendch  ("\n\r", ch);
    return;
}

void do_affects(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA *paf, *paf_last = NULL;
    AFFECT_DATA *fatalflawaf;
    MOB_INDEX_DATA *qMob;
    char buf[MAX_STRING_LENGTH], buf2 [MSL];
    bool specialaffect = FALSE;
    bool awed = FALSE;
    int quarry = 0;

    if ( ch->affected != NULL )
    {
    send_to_char( "\n\r------------<<<<<<<<<<<<============================>>>>>>>>>>>>------------\n\r", ch );
    send_to_char( "|                     -<=     Current Affects      =>-                     |\n\r", ch );
    send_to_char( "|                                                                          |\n\r", ch );
    send_to_char( "| Name                          : Modifies           Amount      Duration  |\n\r", ch );
    send_to_char( "----------------------------------------------------------------------------\n\r", ch );
    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
        if (paf->type == gsn_awe)
        {
          awed = TRUE;
          continue;
        }
        if (paf->type == gsn_gift_fatalflaw)
        {
          specialaffect = TRUE;
          fatalflawaf = paf;
          quarry = fatalflawaf->modifier;
          qMob = get_mob_index(quarry);
          continue;
        }
        if (paf_last != NULL && paf->type == paf_last->type)
            sprintf( buf, "                                ");
        else
            sprintf( buf, "  %-30s", skill_table[paf->type].name );

        send_to_char( buf, ch );

        sprintf( buf,
            ": %-17s  %5d    ",
            affect_loc_name( paf->location ),
            paf->modifier);
        send_to_char( buf, ch );
        if ( paf->duration == -1 )
            sprintf( buf, "   permanently" );
        else
            sprintf( buf, "   %3d hour%s", paf->duration, paf->duration == 1 ? " " : "s" );
        send_to_char( buf, ch );

        send_to_char( "\n\r", ch );
        paf_last = paf;
    }

    if (awed)
    {
      send_to_char( "|------------------+=======[     {RAWED BY YOU{x   ]=======+-------------------|\n\r", ch );
      send_to_char( "|                        Target                        |   Time Remaining  |\n\r", ch );
      for ( paf = ch->affected; paf != NULL; paf = paf->next )
      {
        if (paf->type == gsn_awe)
        {
          quarry = paf->modifier;
          qMob = get_mob_index(quarry);
          sprintf(buf2, "{Y%d hour%s{x", paf->duration, paf->duration != 1 ? "s" : "");
          sprintf(buf, "| {g%s{x |", center(capitalize(qMob->short_descr), 52, " "));
          send_to_char(buf, ch);
          sprintf(buf, "%s|\n\r", center(buf2, 19, " "));
          send_to_char(buf, ch);
        }
      }
    }

    if (specialaffect)
    {
      send_to_char( "|------------------+=======[  {yGIFT: FATAL FLAW{x ]=======+-------------------|\n\r", ch );
      send_to_char( "|                   Current Opponent                   |   Time Remaining  |\n\r", ch );
      sprintf(buf2, "{Y%d hour%s{x", fatalflawaf->duration, fatalflawaf->duration != 1 ? "s" : "");
      sprintf(buf, "| {g%s{x |", center(capitalize(qMob->short_descr), 52, " "));
      send_to_char(buf, ch);
      sprintf(buf, "%s|\n\r", center(buf2, 19, " "));
      send_to_char(buf, ch);
      if (get_affect_level(ch, gsn_gift_fatalflaw) == 0)
      {
        send_to_char( "|------------------+-----------------------------------+-------------------|\n\r", ch );
        sprintf(buf2, "{x You refuse to believe that %s is perfect, {R", qMob->short_descr);
        sprintf(buf, "|{R %s{x |\n\r| {R************ {xand cannot focus on any new opponent right now. {R*********** {x|\n\r", center(buf2, 72, "*"));
        send_to_char(buf, ch);
      }
    }
    send_to_char( "------------<<<<<<<<<<<<============================>>>>>>>>>>>>------------\n\r", ch );
    }
    else
    send_to_char("You are not affected by any spells.\n\r",ch);

    return;
}



char *  const   day_name    [] =
{
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday", "Sunday"
};

char *  const   month_name  [] =
{
    "January", "Febuary", "March", "April",
    "May", "June", "July", "August", "September",
    "October", "November", "December", "error", "error",
    "error", "error", "error"
};

void do_resettime( CHAR_DATA *ch, char *argument )
{
    send_to_char("The World is Reset.\n\r", ch );
    time_info.hour = 0;
    time_info.day = 0;
    time_info.month = 0;
    time_info.year = 0;
    return;
}


 void do_time( CHAR_DATA *ch, char *argument )
{
    extern char str_boot_time[];
    char buf[MAX_STRING_LENGTH];
    char *suf;
    int day;

    if(time_info.hour < 0 || time_info.day < 0 || time_info.month < 0 || time_info.year < 0 )
    {
        send_to_char("Time error! Auto-resetting the time...\n\r", ch );
        send_to_char("The World is Reset.\n\r", ch );
        time_info.hour = 0;
        time_info.day = 0;
        time_info.month = 0;
        time_info.year = 0;
    }

    day     = time_info.day + 1;

         if ( day > 4 && day <  20 ) suf = "th";
    else if ( day % 10 ==  1       ) suf = "st";
    else if ( day % 10 ==  2       ) suf = "nd";
    else if ( day % 10 ==  3       ) suf = "rd";
    else                             suf = "th";

    sprintf( buf,
    "It is %d %s, %s, the %d%s day of %s, %d AD.\n\r",
    (time_info.hour % 12 == 0) ? 12 : time_info.hour %12,
    time_info.hour >= 12 ? "pm" : "am",
    day_name[day % 7],
    day, suf,
    month_name[time_info.month], time_info.year);
    send_to_char(buf,ch);
    sprintf(buf,"Haven was started on %sThe current system time is %s",
    str_boot_time,
    (char *) ctime( &current_time )
    );
    send_to_char( buf, ch );
    if(IS_OUTSIDE(ch))
    {
        if(weather_info.sunlight == SUN_SET || weather_info.sunlight == SUN_DARK)
        {
            switch(time_info.phase)
            {
                case 0:
                        send_to_char("The Moon is New.\n\r",ch);
                        send_to_char ("{D         _..._\n\r", ch);
                        send_to_char ("{D       .:::::::.\n\r", ch);
                        send_to_char ("{D      :::::::::::\n\r", ch);
                        send_to_char ("{D      :::::::::::\n\r", ch);
                        send_to_char ("{D      `:::::::::'\n\r", ch);
                        send_to_char ("{D        `':::''{x\n\r\n\r", ch);
                        break;
                case 1:
                        send_to_char("The Crescent Moon shimmers mysteriously.\n\r",ch);
                        send_to_char ("{D        {W_..._\n\r", ch);
                        send_to_char ("{D      .::::{W. `.\n\r", ch);
                        send_to_char ("{D     :::::::{W.  :\n\r", ch);
                        send_to_char ("{D     :::::::{W:  :\n\r", ch);
                        send_to_char ("{D     `::::::{W' .'\n\r", ch);
                        send_to_char ("{D      `'::{W'-'{x\n\r\n\r", ch);
                        break;
                case 2:
                        send_to_char("The Half Moon glimmers overhead.\n\r",ch);
                        send_to_char ("{x        _.{W.._\n\r", ch);
                        send_to_char ("{x      .:::{W:  `.\n\r", ch);
                        send_to_char ("{x     :::::{W:    :\n\r", ch);
                        send_to_char ("{x     :::::{W:    :\n\r", ch);
                        send_to_char ("{x     `::::{W:   .'\n\r", ch);
                        send_to_char ("{x       `':{W:.-'{x\n\r\n\r", ch);
                        break;
                case 3:
                        send_to_char("The Gibbous Moon winks from the shadows.\n\r",ch);
                        send_to_char ("{x        _{W..._\n\r", ch);
                        send_to_char ("{x      .::{W'   `.\n\r", ch);
                        send_to_char ("{x     ::{W:       :\n\r", ch);
                        send_to_char ("{x     ::{W:       :\n\r", ch);
                        send_to_char ("{x     `::{W.     .'\n\r", ch);
                        send_to_char ("{x      `':{W..-'{x\n\r\n\r", ch);
                        break;
                case 4:
                        send_to_char("The Full Moon fills the heavens with it's brilliance.\n\r",ch);
                        send_to_char ("{W        _..._\n\r", ch);
                        send_to_char ("{W      .'     `.\n\r", ch);
                        send_to_char ("{W     :         :\n\r", ch);
                        send_to_char ("{W     :         :\n\r", ch);
                        send_to_char ("{W     `.       .'\n\r", ch);
                        send_to_char ("{W       `-...-' {x\n\r\n\r", ch);
                        break;
                case 5:
                        send_to_char("The Gibbous Moon winks from the shadows.\n\r",ch);
                        send_to_char ("{W        _{D..._\n\r", ch);
                        send_to_char ("{W      .'   {D`::.\n\r", ch);
                        send_to_char ("{W     :       {D:::\n\r", ch);
                        send_to_char ("{W     :       {D:::\n\r", ch);
                        send_to_char ("{W     `.     {D.::'\n\r", ch);
                        send_to_char ("{W       `-{D..:''{x\n\r\n\r", ch);
                        break;
                case 6:
                        send_to_char("The Half Moon beams down from overhead.\n\r",ch);
                        send_to_char ("{W        _.{D.._\n\r", ch);
                        send_to_char ("{W      .'  {D::::.\n\r", ch);
                        send_to_char ("{W     :    {D::::::\n\r", ch);
                        send_to_char ("{W     :    {D::::::\n\r", ch);
                        send_to_char ("{W     `.   {D:::::'\n\r", ch);
                        send_to_char ("{W       `-.{D::''{x\n\r\n\r", ch);
                        break;
                case 7:
                        send_to_char("The Crescent Moon hangs in the sky like a beautiful ornament.\n\r",ch);
                        send_to_char ("{W        _..._{D\n\r", ch);
                        send_to_char ("{W      .' .{D::::.\n\r", ch);
                        send_to_char ("{W     :  :{D:::::::\n\r", ch);
                        send_to_char ("{W     :  :{D:::::::\n\r", ch);
                        send_to_char ("{W     `. '{D::::::'\n\r", ch);
                        send_to_char ("{W       `-.::{D''{x\n\r\n\r", ch);
                        break;
                default: send_to_char("Error.\n\r",ch);
                        break;
            }
        }
    }
    return;
}



void do_weather( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    static char * const sky_look[4] =
    {
    "{Wcloudless{x",
    "{Dcloudy{x",
    "rainy",
    "lit by {Wflashes {xof {Ylightning{x"
    };

    if ( !IS_OUTSIDE(ch) )
    {
    send_to_char( "You can't see the weather indoors.\n\r", ch );
    return;
    }

    sprintf( buf, "The sky is %s and %s.\n\r",
    sky_look[weather_info.sky],
    weather_info.change >= 0
    ? "a warm southerly breeze blows"
    : "a cold northern gust blows"
    );
    send_to_char( buf, ch );
    return;
}

void do_help( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;
    char buf[MSL];

    output = new_buf();

    if ( argument[0] == '\0' )
    argument = "summary";

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
    argument = one_argument(argument,argone);
    if (argall[0] != '\0')
        strcat(argall," ");
    strcat(argall,argone);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

    if (level > get_trust( ch ) )
        continue;

    if ( is_name( argall, pHelp->keyword ) )
    {
        /* add seperator if found */
        if (found)
        add_buf(output,
    "\n\r======================================================================\n\r\n\r");
        if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
        {
            add_buf(output, "{W**********************************************************************{x\n\r");
            sprintf(buf, "{DKeywords:{c %s{x\n\r", pHelp->keyword);
            add_buf(output,buf);
            add_buf(output,"{W**********************************************************************{x\n\r\n\r");
        }

        /*
         * Strip leading '.' to allow initial blanks.
         */
        if ( pHelp->text[0] == '.' )
        add_buf(output,pHelp->text+1);
        else
        add_buf(output,pHelp->text);
        found = TRUE;
        /* small hack :) */
        if (ch->desc != NULL && ch->desc->connected != CON_PLAYING
        &&              ch->desc->connected != CON_GEN_GROUPS)
        break;
    }
    }

    if (!found){
        send_to_char( "No help on that word. This will be logged so the helpfile can be added.\n\r", ch );
        append_file( ch, NOHELPS_FILE, argall, TRUE );}
    else
    page_to_char(buf_string(output),ch);
    free_buf(output);
}


void do_helplist( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUFFER *output;
    bool found = FALSE;
    char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
    int level;
    char buf[MSL];

    output = new_buf();

    if ( argument[0] == '\0' ) {
      send_to_char("You must give some word or partial word to search for. eg. 'helplist ki'.\n\r", ch);
      return;
    }

    /* this parts handles help a b so that it returns help 'a b' */
    argall[0] = '\0';
    while (argument[0] != '\0' )
    {
    argument = one_argument(argument,argone);
    if (argall[0] != '\0')
        strcat(argall," ");
    strcat(argall,argone);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
    {
        level = (pHelp->level < 0) ? -1 * pHelp->level - 1 : pHelp->level;

    if (level > get_trust( ch ) )
        continue;

      if ( is_name( argall, pHelp->keyword ) )
      {
          /* add seperator if found */
          if (found)
          add_buf(output,"");
          if ( pHelp->level >= 0 && str_cmp( argall, "imotd" ) )
          {
              sprintf(buf, "{DKeywords:{c %s{x\n\r", pHelp->keyword);
              add_buf(output,buf);
          }

          found = TRUE;
      }
    }

    if (!found){
        send_to_char( "No helps were found for that search string.n\r", ch );
        return;
      } else
    page_to_char(buf_string(output),ch);
    free_buf(output);
}



/* whois command */
void do_whois (CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    BUFFER *buffer;
    buffer = new_buf();
    (char *) ctime( &current_time );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Whom do you want information on?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if(IS_NPC(victim))
    {
        send_to_char( "That isn't a player.\n\r",ch);
        return;
    }

    if (victim->level > ch->level)
    {
        send_to_char( "That won't work on those higher level than you.\n\r",ch);
        return;
    }

    sprintf(buf, "{g /========================[    {GWhois{g     ]========================\\{x\n\r");
    add_buf(buffer, buf);
    sprintf(buf, "{g/==================================================================\\{x\n\r");
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Name           :  %s\n\r", victim->name);
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Race           :  %s\n\r", race_table[victim->race].name);
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Affiliation    :  %s\n\r", capitalize(clan_table[victim->clan].name));
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Remorts        :  %d\n\r", victim->remorts);
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Creation ID    :  %d\n\r", victim->id);
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Create Date    :  %s", ctime(&victim->id));
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    Total Logged In:  %d hours\n\r", (int) (victim->played + current_time - victim->logon) / 3600);
    add_buf(buffer, buf);
    sprintf(buf, "{g||{x    This Session   :  %d hours\n\r", (int) (current_time - victim->logon) / 3600);
    add_buf(buffer, buf);

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->character != NULL && can_see( ch, d->character )
        && is_name(arg,d->character->name) || (d->original && is_name(arg,d->original->name)))
        {
            sprintf( buf, "{g||{x    Socket         :  [%3d %2d] %s@%s\n\r",
            d->descriptor, d->connected,
            d->original  ? d->original->name  :
            d->character ? d->character->name : "(none)",
            d->host
            );
        }
    }
    add_buf(buffer, buf);
    sprintf(buf, "{g\\==================================================================/{x\n\r");
    add_buf(buffer, buf);
    sprintf(buf, "{g \\================================================================/{x\n\r");
    add_buf(buffer, buf);
    page_to_char(buf_string(buffer), ch);
    return; 
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
void do_who( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    char ilist[2*MSL];
    char plist[2*MSL];
    char tags[MSL];
    /*char award[MAX_STRING_LENGTH]; */
    BUFFER *output;
    DESCRIPTOR_DATA *d;

    extern bool doubleexp;
    extern bool manualxp;
    extern bool slaughter;
    extern bool doubledam;
    extern bool nosun;
    extern bool spookums;
    int iRace;
    int iClan;
    int iLevelLower;
    int iLevelUpper;
    int nNumber;
    int nMatch;
    int vMatch;
    int bootsecs;
    int uphours, upmins;
    extern int most_players;
    extern int max_players;
    bool rgfRace[MAX_PC_RACE];
    bool rgfClan[top_clan];
    bool fClanRestrict = FALSE;
    bool fClan = FALSE;
    bool fRaceRestrict = FALSE;
    bool fImmortalOnly = FALSE;
    char *format;

    /*
     * Set default arguments.
     */
    iLevelLower    = 0;
    iLevelUpper    = MAX_LEVEL;
    for ( iRace = 0; iRace < MAX_PC_RACE; iRace++ )
        rgfRace[iRace] = FALSE;
    for (iClan = 0; iClan < MAX_CLAN; iClan++)
    rgfClan[iClan] = FALSE;

    /*
     * Parse arguments.
     */
    nNumber = 0;

    /* hack to prevent players from seeing guild and clan on who */
     fClanRestrict = FALSE;
     rgfClan[iClan] = FALSE;


    /*
     * Now show matching chars.
     */
    output = new_buf();
    ilist[0] = '\0';
    plist[0] = '\0';
    nMatch = 0;
    vMatch = 0;
    buf[0] = '\0';
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *wch;

        /*
         * Check for match against restrictions.
         * Don't use trust as that exposes trusted mortals.
         */
        if ( d->connected != CON_PLAYING  )
            continue;
        if (!can_see( ch, d->character ))
                { nMatch++; continue;}

        wch   = ( d->original != NULL ) ? d->original : d->character;

    if (!can_see(ch,wch))
        continue;

        if ( wch->level < iLevelLower
        ||   wch->level > iLevelUpper
        || ( fImmortalOnly  && wch->level < LEVEL_IMMORTAL )
        || ( fRaceRestrict && !rgfRace[wch->race])
    || ( fClan && !is_clan(wch))
    || ( fClanRestrict && !rgfClan[wch->clan]))
            continue;

        vMatch++;
        nMatch++;

        // Format it up!
       sprintf(tags, "{w |                |");
        if (IS_SET(wch->act2, PLR2_NEWBIE)) { sprintf(tags, "{w |   {D--{WNewbie{D--   {w|"); }
        if (IS_SET(wch->act, PLR_NEWBIEHELP)) { sprintf(tags, "{w |  {B<{w-{D[{WHelper{D]{w-{B>{w  |"); }
        if (IS_IMMORTAL(wch) && !IS_NULLSTR(wch->pcdata->immtitle) &&
            IS_SET(wch->comm, COMM_IMMTITLE) ) {
            if (c_str_len(wch->pcdata->immtitle) > 10)
                sprintf(tags, "{w | {w%-14s {w|", wch->pcdata->immtitle);
            else
                sprintf(tags, "{w | {w   %-10s  {w|", wch->pcdata->immtitle);

            }

        if (IS_SET(wch->comm, COMM_AFK) && wch->timer < 1000)
                sprintf(tags, "{w |  {D**{GAFK{D** {r({w%3d{r) {w|", wch->timer);
        else if (IS_SET(wch->comm, COMM_AFK) && wch->timer >= 1000)
            sprintf(tags, "{w |  {D**{GAFK{D** {r({w%4d{r){w|", wch->timer);
        if (wch->timer >= 18 && wch->timer < 1000)
                sprintf(tags, "{w | {D**{RIDLE{D** {r({w%3d{r) {w|", wch->timer);
        else if (wch->timer >= 1000)
            sprintf(tags, "{w | {D**{RIDLE{D** {r({w%4d{r){w|", wch->timer);

        if (IS_SET(wch->act2, PLR2_RP_AVAILABLE) &&
            !IS_SET(wch->comm, COMM_AFK) && wch->timer < 18)
          sprintf(tags, "{w |{y[{MLooking For RP{y]{x|");
        if (IS_SET(wch->act, PLR_IC)) {sprintf(tags, "{w | {w({CIn-Character{w){w |");}
        if (IS_SET(wch->comm, COMM_QUIET)) {sprintf(tags, "{w |  {c<{yQuiet Mode{c>{w  |");}


        if (IS_IMMORTAL(ch) || IS_SET(ch->act2, PLR2_WHOSHOWLEVEL))
        {
            sprintf(buf2, "{w[{m%3d{w]{x", wch->level);
            strcat(buf, buf2);
        }

        if (wch->incog_level >= LEVEL_HERO || wch->invis_level >= LEVEL_HERO)
           {if (wch->incog_level >= LEVEL_HERO)
                sprintf(buf2, " {w({mInc%d{w){x", wch->incog_level);
            if (wch->invis_level >= LEVEL_HERO)
                sprintf(buf2, " {w({cWiz%d{w){x", wch->invis_level);
            strcat(buf, buf2); }

        if (!IS_NULLSTR(wch->pcdata->pretitle))
            {sprintf(buf2, " {w%s", wch->pcdata->pretitle); strcat(buf, buf2);}

        sprintf(buf2, "%s%s{x%s%s%s%s", IS_IMMORTAL(ch) ? (IS_SET(wch->act, PLR_IMMTALK) ? " {y" : " {w") : " {w",
		 wch->name, wch->pcdata->title,
            IS_IMMORTAL(ch) ? (wch->cheater ? " {R({DCheater{R){x" : "") : "",
                IS_IMMORTAL(ch) ? (IS_AFFECTED2(wch, AFF2_UMBRA) ? " {D({mUmbra{D){x" : "") : "",
                IS_SET(wch->act, PLR_ARENA) ? " {w({RArena{w){x" : "",
                IS_SET(wch->act, PLR_SPEC)  ? " {w({rSpec{w){x" : "");
        strcat(buf, buf2);

        if ((IS_IMMORTAL(ch) && IS_SET(ch->act2, PLR2_IMMSHOWLOC)) || IS_SET(wch->act2, PLR2_WHOLOCALWAYS) ||
            (IS_SET(wch->act, PLR_IC) && IS_SET(wch->act2, PLR2_WHOLOCIC)) ||
            (IS_SET(wch->comm, COMM_QUIET) && IS_SET(wch->act2, PLR2_WHOLOCQUIET))) {
                sprintf(buf2, " {w({DAt:{c%s{w){x", wch->in_room->area->name); strcat(buf, buf2);}


    sprintf(buf3,"%s %s{x\n\r",tags, buf);
    if (IS_IMMORTAL(wch))
        strcat(ilist, buf3);
    else
        strcat(plist, buf3);
    buf[0] = '\0';
    }
    cuplayers = plist;

    update_moston();
    bootsecs = (int) (current_time - boot_time);
    upmins   = bootsecs / 60;
    uphours  = upmins / 60;
    upmins   = upmins % 60;

    if (ilist[0] == '\0')
        sprintf(ilist, "{w |    {D< {rNone{D >{w    |\r\n");
    if (plist[0] == '\0')
        sprintf(plist, "{w |    {D< {rNone{D >{w    |\r\n");


    add_buf(output,"{D\n\r                     |     |                        |      |\r\n");
    add_buf(output,"{D                     |\\   /|                        |\\    /|\r\n");
    add_buf(output,"{D                     | | | |  ___   |       |  ___  | |  | |\r\n");
    add_buf(output,"{D                     | |_| | / _  \\ |\\     /| / __\\ |  \\ \\ |\r\n");
    add_buf(output,"{D                     |  _  || /_|  || |   | | | __> | | \\  |\r\n");
    add_buf(output,"{D                    / /  |_| \\___/\\| \\ \\ / /  \\___/ |_|  \\ \\\r\n");
    add_buf(output,"{D                   /_/                \\ v /               \\_\\\r\n");
    add_buf(output,"{w<================={D/{w===================={D\\ /{w==================={D\\{w=================>\r\n");
    add_buf(output,"{w |{D---{WImmortals{D----{w|                     {Dv{w                                     \r\n");
    add_buf(output, ilist);
    add_buf(output,"{w |{D----{wPlayers{D-----{w|{x\n\r");
    add_buf(output, plist);
      sprintf(buf2," {w+----------------+---------------------+--------------+----------------------+\n\r");
    add_buf(output,buf2);
    sprintf( buf2, "{w |__{gPlayer{w_{gStats{w__|    {DPlaying Now{w:%2d   |__{cMUD{w__{cStats{w__|    {DUptime{w:%4d{Dh{w%2d{Dm{W   |{x\n\r", vMatch, uphours, upmins, uphours > 99 ? "" : " " );
    add_buf(output,buf2);
    sprintf(buf2, "{w |  {DMost Online{w:%2d    {DSince Reboot{w:%2d   |  {DGXp{w: %-6d      {DXP Mod{w:   %d{Dx{w      |{x\n\r", max_players, most_players, global_xp, xpawardmult);
    add_buf(output, buf2);
    add_buf(output, "{w<==============================================================================>{x\n\r");

    if(manualxp)
    {
      if (xpawardmult == 2)
          sprintf( buf2, " =====   The Immortals have granted a boon.  {CDouble XP Bonus{x is ACTIVE!   ===== \n");
      if (xpawardmult == 3)
          sprintf( buf2, " =====   The Immortals have granted a boon.  {CTriple XP Bonus{x is ACTIVE!   ===== \n");
      add_buf(output,buf2);
    }
    else if (doubleexp)
    {
      if (xpawardmult == 2)
        sprintf( buf2, " =====                  {CGlobal Double XP Bonus{x is ACTIVE!                 ===== \n");
      if (xpawardmult == 3)
        sprintf( buf2, " =====                  {CGlobal Triple XP Bonus{x is ACTIVE!                 ===== \n");
      add_buf(output,buf2);
    }

    if (slaughter)
    {
      sprintf( buf2, " =====               {RGet to killing.  It's a SLAUGHTERFEST!{x               ===== \n");
      add_buf(output,buf2);
    }
    if (doubledam)
    {
      sprintf( buf2, " =====                       {RDOUBLE DAMAGE is ACTIVE!{x                     ===== \n");
      add_buf(output,buf2);
    }
    if (nosun)
    {
      sprintf( buf2, " =====                   {DDark clouds blout out the sun.{x                   ===== \n");
      add_buf(output,buf2);
    }
    if (spookums)
    {
      sprintf( buf2, " =====             {GThe land is covered by an eerie {Dspookiness{G.{x            ===== \n");
      add_buf(output,buf2);
    }
    if (slaughter || doubledam || nosun || doubleexp || manualxp || spookums)
    {
      add_buf(output, "{w<==============================================================================>{x\n\r");
    }
    page_to_char( buf_string(output), ch );
    free_buf(output);
    return;
}

void do_whoconfig (CHAR_DATA *ch, char *argument)
{
    char arg1[MIL];
    char arg2[MIL];
    char buf[MSL];

    if (IS_NPC(ch))
        return;

    if (IS_NULLSTR(argument))
    {
        sendch("Whoconfig Settings:\n\r\n\r", ch);
        buf[0] = '\0';
        if (IS_SET(ch->act2, PLR2_WHOLOCIC))
            sprintf(buf, "{ywhen IC {x");
        else buf[0] = '\0';

        if (IS_SET(ch->act2, PLR2_WHOLOCQUIET))
            strcat(buf, "{ywhen Quiet {x");
        if (IS_SET(ch->act2, PLR2_WHOLOCALWAYS))
            sprintf(buf, "{GAlways {x");
        if (!IS_SET(ch->act2, PLR2_WHOLOCIC) &&
            !IS_SET(ch->act2, PLR2_WHOLOCQUIET) &&
            !IS_SET(ch->act2, PLR2_WHOLOCALWAYS))
            sprintf(buf, "{RNever {x");

        cprintf(ch, "%-25s%s\n\r", "{DWhoformat:{x", "{Gdefault{x");
        cprintf(ch, "%-25s%s\n\r", "{DLevels Shown:{x", (IS_SET(ch->act2, PLR2_WHOSHOWLEVEL) ? "{GSHOWN{x" : "{RHIDDEN{x"));
        cprintf(ch, "%-25s%s\n\r", "{DLocation Shown:{x", buf);
        cprintf(ch, "%-25s%s\n\r", "{DPretitle:{x",
            !IS_NULLSTR(ch->pcdata->pretitle) ? ch->pcdata->pretitle : "{R(None Set){x" );
        if (IS_IMMORTAL(ch)) {
            cprintf(ch, "%-25s%s\n\r", "{DImmtitle:{x",
            !IS_NULLSTR(ch->pcdata->immtitle) ? ch->pcdata->immtitle : "{R(None Set){x" );
            cprintf(ch, "%-25s%s\n\r", "{DPlr Location Shown:{x", (IS_SET(ch->act2, PLR2_IMMSHOWLOC) ? "{GALWAYS{x" : "Per Player Settings"));
        }

        sendch("\n\r", ch);
        sendch("{DSyntax:\n\r", ch);
        sendch("{cwhoconfig location {D<ic/quiet/always>    - Toggles location display.{x\n\r", ch);
        sendch("{cwhoconfig levels                        {D- Show/Hide levels on Who.{x\n\r", ch);
        if (IS_IMMORTAL(ch))
       {sendch("{cwhoconfig immshowlocation               {D- Show/Hide player location at all times.{x\n\r", ch);}
        sendch("\n\r", ch);
        sendch("{DFor players interested in having a pretitle set on their character (For example: \n\r", ch);
        sendch("Lord, Voivode, Prince) please submit your reasoning in a '{cnote to immortal{D'. {x\n\r", ch);
        return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (!str_prefix(arg1, "levels"))
    {
        TOGGLE_BIT(ch->act2, PLR2_WHOSHOWLEVEL);
        if (IS_SET(ch->act2, PLR2_WHOSHOWLEVEL))
            sendch("Toggled. Current level config: SHOW\n\r", ch);
        else
            sendch("Toggled. Current level config: HIDE\n\r", ch);
        return;
    }
    if (!str_prefix(arg1, "immshowlocation"))
    {
        if (!IS_IMMORTAL(ch))
        {sendch("This command is for Immortals. Hence being called 'immshowlocation'. Get it?\n\r", ch);
        return; }
        TOGGLE_BIT(ch->act2, PLR2_IMMSHOWLOC);
        if (IS_SET(ch->act2, PLR2_IMMSHOWLOC))
            sendch("Toggled. Current plr location config: SHOW\n\r", ch);
        else
            sendch("Toggled. Current plr location config: Per Player Settings\n\r", ch);
        return;
    }
    if (!str_prefix(arg1, "location"))
    {
        if (!str_cmp(arg2, "ic"))
            TOGGLE_BIT(ch->act2, PLR2_WHOLOCIC);
        else if (!str_cmp(arg2, "quiet"))
            TOGGLE_BIT(ch->act2, PLR2_WHOLOCQUIET);
        else if (!str_cmp(arg2, "always"))
            TOGGLE_BIT(ch->act2, PLR2_WHOLOCALWAYS);
        else {
            cprintf(ch, "Unknown location setting '%s'\n\r", arg2);
            return;
        }
        buf[0] = '\0';
        if (IS_SET(ch->act2, PLR2_WHOLOCIC))
            strcat(buf, "IC ");
        if (IS_SET(ch->act2, PLR2_WHOLOCQUIET))
            strcat(buf, "QUIET ");
        if (IS_SET(ch->act2, PLR2_WHOLOCALWAYS))
            strcat(buf, "ALWAYS ");
        if (IS_NULLSTR(buf))
            strcat(buf, "none");

        cprintf(ch, "Toggled. Current location config: %s\n\r", buf);

    } else {
        cprintf(ch, "Unknown whoconfig setting '%s'\n\r", arg1);
        return;
    }
}

void do_count ( CHAR_DATA *ch, char *argument )
{
    int count;
    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];

    count = 0;

    for ( d = descriptor_list; d != NULL; d = d->next )
        if ( d->connected == CON_PLAYING && can_see( ch, d->character ) )
        count++;

    max_on = UMAX(count,max_on);

    if (max_on == count)
        sprintf(buf,"There are %d characters on, the most so far today.\n\r",
        count);
    else
    sprintf(buf,"There are %d characters on, the most on today was %d.\n\r",
        count,max_on);

    send_to_char(buf,ch);
}

void do_inventory( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You are currently carrying in your inventory:\n\r", ch );
    show_list_to_char( ch->carrying, ch, TRUE, TRUE );
    return;
}



void do_equipment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int iWear;
    bool found;

    send_to_char( "You are using:\n\r", ch );
    found = FALSE;
    for ( iWear = 0; iWear < MAX_WEAR; iWear++ )
    {
    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
        continue;

    send_to_char( where_name[iWear], ch );
    if ( can_see_obj( ch, obj ) )
    {
        send_to_char( format_obj_to_char( obj, ch, TRUE ), ch );
        send_to_char( "\n\r", ch );
    }
    else
    {
        send_to_char( "something.\n\r", ch );
    }
    found = TRUE;
    }

    if ( !found )
    send_to_char( "Nothing.\n\r", ch );

    return;
}



void do_compare( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    int value1;
    int value2;
    char *msg;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( arg1[0] == '\0' )
    {
    send_to_char( "Compare what to what?\n\r", ch );
    return;
    }

    if ( ( obj1 = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    if (arg2[0] == '\0')
    {
    for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
    {
        if (obj2->wear_loc != WEAR_NONE
        &&  can_see_obj(ch,obj2)
        &&  obj1->item_type == obj2->item_type
        &&  (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0 )
        break;
    }

    if (obj2 == NULL)
    {
        send_to_char("You aren't wearing anything comparable.\n\r",ch);
        return;
    }
    }

    else if ( (obj2 = get_obj_carry(ch,arg2,ch) ) == NULL )
    {
    send_to_char("You do not have that item.\n\r",ch);
    return;
    }

    msg     = NULL;
    value1  = 0;
    value2  = 0;

    if ( obj1 == obj2 )
    {
    msg = "You compare $p to itself.  It looks about the same.";
    }
    else if ( obj1->item_type != obj2->item_type )
    {
    msg = "You can't compare $p and $P.";
    }
    else
    {
    switch ( obj1->item_type )
    {
    default:
        msg = "You can't compare $p and $P.";
        break;

    case ITEM_ARMOR:
        value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
        value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
        break;

    case ITEM_WEAPON:
        if (obj1->pIndexData->new_format)
        value1 = (1 + obj1->value[2]) * obj1->value[1];
        else
            value1 = obj1->value[1] + obj1->value[2];

        if (obj2->pIndexData->new_format)
        value2 = (1 + obj2->value[2]) * obj2->value[1];
        else
            value2 = obj2->value[1] + obj2->value[2];
        break;
    }
    }

    if ( msg == NULL )
    {
         if ( value1 == value2 ) msg = "$p and $P look about the same.";
    else if ( value1  > value2 ) msg = "$p looks better than $P.";
    else                         msg = "$p looks worse than $P.";
    }

    act( msg, ch, obj1, obj2, TO_CHAR );
    return;
}



void do_credits( CHAR_DATA *ch, char *argument )
{
    do_function(ch, &do_help, "diku" );
    return;
}



void do_where( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Players near you:\n\r", ch );
    found = FALSE;
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        && ( victim = d->character ) != NULL
        &&   !IS_NPC(victim)
        &&   victim->in_room != NULL
        &&   !IS_SET(victim->in_room->room_flags,ROOM_NOWHERE)
        &&   (is_room_owner(ch,victim->in_room)
        ||    !room_is_private(victim->in_room))
        &&   victim->in_room->area == ch->in_room->area
        &&   can_see( ch, victim ) )
        {
        found = TRUE;
        sprintf( buf, "%-28s %s\n\r",
            victim->name, victim->in_room->name );
        send_to_char( buf, ch );
        }
    }
    if ( !found )
        send_to_char( "None\n\r", ch );
    }
    else
    {
    found = FALSE;
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
        if ( victim->in_room != NULL
        &&   victim->in_room->area == ch->in_room->area
        &&   !IS_AFFECTED(victim, AFF_HIDE)
        &&   !IS_SET(victim->act, ACT_QUESTMOB)
/*      &&   !IS_AFFECTED(victim, AFF_SNEAK) */
        &&   can_see( ch, victim )
        &&   is_name( arg, victim->name ) )
        {
        found = TRUE;
        sprintf( buf, "%-28s %s\n\r",
            PERS(victim, ch, FALSE), victim->in_room->name );
        send_to_char( buf, ch );
        break;
        }
    }
    if ( !found )
        act( "You didn't find any $T.", ch, NULL, arg, TO_CHAR );
    }

    return;
}




void do_consider( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *msg;
    int diff, parmor, varmor;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Whom do you wish to consider?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "Your target is not here to consider.\n\r", ch );
    return;
    }

    if (is_safe(ch,victim))
    {
    send_to_char("It would not be wise to attack your target at this moment.\n\r",ch);
    return;
    }

    diff = victim->level - ch->level;

         if ( diff <=  -5 ) msg = "{D";
    else if ( diff <=   5 ) msg = "{G";
    else if ( diff <=  15 ) msg = "{Y";
    else                    msg = "{R";

    parmor = (GET_AC(ch, AC_BASH) + GET_AC(ch, AC_PIERCE) + GET_AC(ch, AC_SLASH) + GET_AC(ch, AC_EXOTIC)) / 4;
    varmor = (GET_AC(victim, AC_BASH) + GET_AC(victim, AC_PIERCE) + GET_AC(victim, AC_SLASH) + GET_AC(victim, AC_EXOTIC)) / 4;

    send_to_char("                          ( ( ( {M[Consider]{x ) ) ) \n\r", ch);
    sprintf(buf, "__________________________( ( < %sLevel %4d {x> ) )__________________________\n\r", msg, victim->level);
    send_to_char(buf, ch);
    sprintf(buf, "  %s \n\r",  center(capitalize(victim->short_descr),70," "));
    send_to_char(buf, ch);
        if ( diff <=  -5 ) msg = "  It is probably not worth your time to kill $N.";
    else if ( diff <=   5 ) msg = "  $N would be a good match for you.";
    else if ( diff <=  15 ) msg = "  $N might provide a bit of a challenge.";
    else                    msg = "  Only those sure of themselves should pick this fight.";
    act( msg, ch, NULL, victim, TO_CHAR );

    if (parmor < varmor - 200)
      msg = "  You are generally better armored than $N.";
    if (parmor >= varmor - 200 && parmor <= varmor + 200)
      msg = "  You and $N appear to be reasonably matched in total armor.";
    if (parmor > varmor + 200)
      msg = "  $N is wearing much better armor than you are.";
    act(msg, ch, NULL, victim, TO_CHAR);

    if (is_affected(ch, gsn_telepathy) && IS_SET(victim->form, FORM_SENTIENT) && IS_NPC(victim))
    {
        send_to_char("{c|-------------------------( (   {CTelepathy{c    ) )-------------------------|{x \n\r", ch);
        sprintf( buf, "{c| {xTrace thoughts coming from %s's mind:\n\r{c| {x", victim->short_descr);
        send_to_char( buf, ch );
        if (IS_SET(victim->act,ACT_SENTINEL))
            send_to_char(" [ Immobility  ]", ch);
        else
            send_to_char("                ", ch);
        if (IS_SET(victim->act,ACT_SCAVENGER))
            send_to_char("   [  Hoarding   ]", ch);
        else
            send_to_char("                  ", ch);
        if (IS_SET(victim->act,ACT_AGGRESSIVE))
            send_to_char("  [    Anger    ]", ch);
        else
            send_to_char("                 ", ch);
        if (IS_SET(victim->act,ACT_WIMPY))
            send_to_char("   [  Cowardice  ]", ch);
        else
            send_to_char("                  ", ch);
        send_to_char("\n\r{c| {x", ch);
        if (IS_SET(victim->act,ACT_CLERIC))
            send_to_char(" [  Devotion   ]", ch);
        else
            send_to_char("                ", ch);
        if (IS_SET(victim->act,ACT_MAGE))
            send_to_char("   [Enlightenment]", ch);
        else
            send_to_char("                  ", ch);
        if (IS_SET(victim->act,ACT_THIEF))
            send_to_char("  [  Duplicity  ]", ch);
        else
            send_to_char("                 ", ch);
        if (IS_SET(victim->act,ACT_WARRIOR))
            send_to_char("   [    Valor    ]", ch);
        else
            send_to_char("                  ", ch);
        send_to_char("\n\r{c| {x", ch);
        if (IS_SET(victim->act,ACT_IS_HEALER))
            send_to_char(" [ Assistance  ]", ch);
        else
            send_to_char("                ", ch);
        if (IS_SET(victim->act,ACT_IS_CHANGER))
            send_to_char("   [   Avarice   ]", ch);
        else
            send_to_char("                  ", ch);
        send_to_char("\n\r{c| {x", ch);
        if (IS_SET(victim->act2,ACT2_INFLUENCE))
            send_to_char(" [ Persuasion  ]", ch);
        else
            send_to_char("                ", ch);
        if (IS_SET(victim->act2,ACT2_BLOOD_DOLL))
            send_to_char("   [   Desire    ]", ch);
        else
            send_to_char("                  ", ch);
        if (IS_SET(victim->act2,ACT2_RETAINER))
            send_to_char("  [ Employment  ]", ch);
        else
            send_to_char("                 ", ch);
        if (IS_SET(victim->act2,ACT2_SOCIALITE))
            send_to_char(" [ Aristocracy ]", ch);
        else
            send_to_char("                ", ch);
        send_to_char("{x\n\r", ch);
    }

    if (is_affected(ch, gsn_gift_namethespirit) && is_umbral_denizen(victim))
    {
        send_to_char("{M|-------------------------( ( {mUmbral Denizen{M ) )-------------------------|{x \n\r", ch);
        msg = name_the_spirit(victim);
        send_to_char(msg, ch);
        sprintf( buf, "{m--> {x%s\n\r", victim->name);
        send_to_char( buf, ch );
    }

    send_to_char("--------------------------( ( ( ---------- ) ) )-------------------------- \n\r", ch);

    return;
}



void set_title( CHAR_DATA *ch, char *title )
{
    char buf[MAX_STRING_LENGTH];

    if ( IS_NPC(ch) )
    {
    bug( "Set_title: NPC.", 0 );
    return;
    }

    if ( title[0] != '.' && title[0] != ',' && title[0] != '!' && title[0] != '?' )
    {
    buf[0] = ' ';
    strcpy( buf+1, title );
    }
    else
    {
    strcpy( buf, title );
    }

    free_string( ch->pcdata->title );
    ch->pcdata->title = str_dup( buf );
    return;
}



void do_title( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
    return;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Change your title to what?\n\r", ch );
    return;
    }

    if ( strlen(argument) > 60 )
    argument[60] = '\0';

    smash_tilde( argument );
    set_title( ch, argument );
    send_to_char( "{WTitle set.{x\n\r", ch );
}

void do_description( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char args[MIL];
    char arg1[MIL];
    strcpy( args, argument );
    one_argument(args, arg1);

    if (!str_cmp(arg1, "edit"))
    {
        string_append( ch, &ch->description );
        return;
    }

    if ( argument[0] != '\0' )
    {
    buf[0] = '\0';
    smash_tilde( argument );

        if (argument[0] == '-')
        {
            int len;
            bool found = FALSE;

            if (ch->description == NULL || ch->description[0] == '\0')
            {
                send_to_char("No lines left to remove.\n\r",ch);
                return;
            }

        strcpy(buf,ch->description);

            for (len = strlen(buf); len > 0; len--)
            {
                if (buf[len] == '\r')
                {
                    if (!found)  /* back it up */
                    {
                        if (len > 0)
                            len--;
                        found = TRUE;
                    }
                    else /* found the second one */
                    {
                        buf[len + 1] = '\0';
            free_string(ch->description);
            ch->description = str_dup(buf);
            send_to_char( "Your description is:\n\r", ch );
            send_to_char( ch->description ? ch->description :
                "(None).\n\r", ch );
                        return;
                    }
                }
            }
            buf[0] = '\0';
        free_string(ch->description);
        ch->description = str_dup(buf);
        send_to_char("Description cleared.\n\r",ch);
        return;
        }
    if ( argument[0] == '+' )
    {
        if ( ch->description != NULL )
        strcat( buf, ch->description );
        argument++;
        while ( isspace(*argument) )
        argument++;
    }


        if (strlen( ch->description ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ))
    {
        send_to_char( "Description too long.\n\r", ch );
        return;
    }

    strcat( buf, argument );
    strcat( buf, "\n\r" );
    free_string( ch->description );
    ch->description = str_dup( buf );
    }

    send_to_char( "Your description is:\n\r", ch );
    send_to_char( ch->description ? ch->description : "(None).\n\r", ch );
    return;
}

void do_playernotes( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char args[MIL];
    char arg1[MIL];
    char arg2[MIL];
    CHAR_DATA *victim;
    one_argument(argument, arg1);
    one_argument(argument, arg2);

    if ((victim = get_char_world(ch, arg1)) == NULL)
    {
        sendch("Change the IC playernotes on whom?\n\r", ch);
        return;
    }

        cprintf(ch, "Editing playernotes on %s.", victim->name);
        string_append( ch, &victim->pcdata->playernotes );
        return;

}

void do_report( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];

    sprintf( buf,
    "You say '{wI have %d/%d hp %d/%d mana %d/%d mv %d xp.{x'\n\r",
    ch->hit,  ch->max_hit,
    ch->mana, ch->max_mana,
    ch->move, ch->max_move,
    ch->exp   );

    send_to_char( buf, ch );

    sprintf( buf, "$n says '{wI have %d/%d hp %d/%d mana %d/%d mv %d xp.{x'",
    ch->hit,  ch->max_hit,
    ch->mana, ch->max_mana,
    ch->move, ch->max_move,
    ch->exp   );

    act( buf, ch, NULL, NULL, TO_ROOM );

    /* Vamp stuff */
    if(ch->race == race_lookup("methuselah"))
    {
        sprintf( buf,
            "You say '{wI am a Methuselah of the clan %s and I have stalked this plane for %d of your years.{x'\n\r",
            capitalize(clan_table[ch->clan].name),
        get_age(ch)  );

        send_to_char( buf, ch );

        sprintf( buf,
            "$n says '{wI am a Methuselah of the clan %s and I have stalked this plane for %d of your years.{x'\n\r",
            capitalize(clan_table[ch->clan].name),
        get_age(ch)  );

        act( buf, ch, NULL, NULL, TO_ROOM );

    }
    if(ch->race == race_lookup("vampire"))
    {
        sprintf( buf,
            "You say '{wI am a %dth generation Kindred of the clan %s. I have been the childe of %s for %d years.{x'\n\r",
            ch->gen,
            capitalize(clan_table[ch->clan].name),
            ch->sire,
            get_age(ch)  );

        send_to_char( buf, ch );

        sprintf( buf,
            "$n says '{wI am a %dth generation Kindred of the clan %s. I have been the childe of %s for %d years.{x'\n\r",
            ch->gen,
            capitalize(clan_table[ch->clan].name),
            ch->sire,
            get_age(ch)  );

        act( buf, ch, NULL, NULL, TO_ROOM );

    }
    if(ch->race == race_lookup("dhampire"))
    {
        sprintf( buf,
            "You say '{wI am a Daywalker. My quest to purge the earth of evil has lasted for %d years.{x'\n\r",
            get_age(ch)  );

        send_to_char( buf, ch );

        sprintf( buf,
            "$n says '{wI am a Daywalker. My quest to purge the earth of evil has lasted for %d years.{x'\n\r",
            get_age(ch)  );

        act( buf, ch, NULL, NULL, TO_ROOM );
    }
    if(ch->race == race_lookup("garou"))
    {
        sprintf( buf,
            "You say '{wI am proud to be one of Gaia's Children. I have spent %d years in my quest to rid the world of the Wyrm.{x'\n\r",
            get_age(ch)  );

        send_to_char( buf, ch );

        sprintf( buf,
            "$n says '{wI am proud to be one of Gaia's Children. I have spent %d years in my quest to rid the world of the Wyrm.{x'\n\r",
            get_age(ch)  );

        act( buf, ch, NULL, NULL, TO_ROOM );
    }
    if(ch->race == race_lookup("ghoul"))
    {
        sprintf(buf, "You say '{wI have had the honor of being the loyal servant of %s for %d years.{x'\n\r",ch->vamp_master, get_age(ch));
        send_to_char(buf,ch);
        sprintf(buf, "$n says '{wI have had the honor of being the loyal servant of %s for %d years.{x'\n\r",ch->vamp_master, get_age(ch));
        act(buf,ch,NULL,NULL,TO_ROOM);
    }
    if(ch->race == race_lookup("human"))
    {
        if(clan_table[ch->clan].clan_type == TYPE_TRADITION)
        {
            sprintf(buf, "You say '{wI am of the Enlightened order of Mages. I have served the %s tradition for %d years. Beware my power.{x\n\r",
            capitalize(tradition_table[ch->tradition].name), get_age(ch));
            send_to_char(buf,ch);
            sprintf(buf, "$n says '{wI am of the Enlightened order of Mages. I have served the %s tradition for %d years. Beware my power.{x\n\r",
            capitalize(tradition_table[ch->tradition].name), get_age(ch));
            act(buf,ch,NULL,NULL,TO_ROOM);
            return;

        }
        else
        {
            send_to_char( "You say '{wI am human.{x'\n\r", ch );
            act( "$n says '{wI am human.{x'", ch, NULL, NULL, TO_ROOM );
        }
    }


    return;
}


// Practice is now completely unnecessary.
void do_practice( CHAR_DATA *ch, char *argument )
{ return; };

/*
 * 'Wimpy' originally by Dionysos.
 */
void do_wimpy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int wimpy;

    one_argument( argument, arg );

    if( !strcmp( arg, "show" ) )
    {
      send_to_char("{y                                | WIMPY |{x\n\r", ch);
      send_to_char("{yW______________________________________________________________________________W{x\n\r", ch);
      if (ch->wimpy == 0)
        sprintf( buf, "{y|     {xYou will never flee automatically.  Set your Wimpy above 0 to do so.     {y|{x\n\r");
      else
        sprintf( buf, "{y|    {xYou will automatically try to flee when you drop to %5d hit points.     {y|{x\n\r", ch->wimpy );
      send_to_char( buf, ch );
      send_to_char("{yW------------------------------------------------------------------------------W{x\n\r", ch);
      return;
    }

    if ( arg[0] == '\0' )
    wimpy = ch->max_hit / 5;
    else
    wimpy = atoi( arg );

    if ( wimpy < 0 )
    {
    send_to_char( "Your courage exceeds your wisdom.\n\r", ch );
    return;
    }

    if ( wimpy > ch->max_hit/2 )
    {
    send_to_char( "Such cowardice ill becomes you.  Choose a lower value.\n\r", ch );
    return;
    }

    ch->wimpy   = wimpy;
    sprintf( buf, "Wimpy is now set to %d hit points.\n\r", wimpy );
    send_to_char( buf, ch );
    return;
}



void do_password( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
    return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Syntax: password <old> <new>.\n\r", ch );
    return;
    }

    if ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) &&
        strcmp(arg1, ch->pcdata->pwd )  )
    {
    WAIT_STATE( ch, 40 );
    send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
    return;
    }

    if ( strlen(arg2) < 5 )
    {
    send_to_char(
        "New password must be at least five characters long.\n\r", ch );
    return;
    }

    /*
     * No tilde allowed because of player file format.
     */
    pwdnew = str_dup( arg2 );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
    if ( *p == '~' )
    {
        send_to_char(
        "New password not acceptable, try again.\n\r", ch );
        return;
    }
    }

    free_string( ch->pcdata->pwd );
    ch->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( ch );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_invite( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if(ch->clan != clan_lookup("watcher") && clan_table[ch->clan].clan_type != TYPE_TRADITION)
    {
        send_to_char( "You are not in a guild!\n\r",ch);
        return;
    }
    if(ch->rank < 9)
    {
        send_to_char( "You are not high enough rank to invite someone!\n\r",ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char( "Syntax: invite <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here to invite.\n\r", ch );
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char( "You cannot invite this creature!\n\r", ch );
        return;
    }
    if(victim->race != race_lookup("human") || victim->embraced >= 1)
    {
        send_to_char( "You can only awaken the Sleepers with no ties to the supernatural!\n\r", ch );
        return;
    }
        if(victim->race != race_lookup("human") && ch->clan == clan_lookup("mage"))
        {
            send_to_char("Only a Sleeper may be Awakened.\n\r",ch);
            return;
        }
    if(victim->clan != 0)
    {
        send_to_char( "They are already in a guild!\n\r",ch);
        return;
    }
        /*NEW MAGE CODE */
    if(ch->clan == clan_lookup("mage") || clan_table[ch->clan].clan_type == TYPE_TRADITION)
    {
        if(arg2[0] == '\0')
        {
            send_to_char("Which Tradition?\n\r",ch);
            return;
        }
        if(victim->tradition == 0)
        {
            if((victim->tradition = tradition_lookup(arg2)) == 0)
            {
                send_to_char("There is no Tradition of that name.\n\r",ch);
                return;
            }
            if(victim->avatar == 0) victim->avatar = 1;
            if(victim->arete == 0) victim->arete = 1;
            if(victim->dpoints == 0) victim->dpoints = 5;
            victim->quintessence = 10;
            victim->sphere[tradition_table[victim->tradition].sphere]++;
            victim->sire = str_dup( ch->name );
            victim->pcdata->sect = sect_lookup("mage");
            ch->apprentice = str_dup( victim->name );
            send_to_char("You gasp as you Awaken to a world of possibilities.\n\r",victim);
            send_to_char("Reality isn't as you understood it and you now see how it can be manipulated to your Will.\n\r",victim);
        }
    }
    victim->clan = ch->clan;
    victim->rank = 1;

    if(victim->clan == clan_lookup("watcher"))
    {
        victim->pcdata->learned[skill_lookup("sanctus lucis")] = 50;
        victim->pcdata->learned[skill_lookup("scry")] = 50;
        ch->embraced = 0;
    }
    sprintf(buf,"They are now a member of the %s Guild.\n\r",
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,ch);
    sprintf(buf,"You are now a member of the %s Guild.\n\r",
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,victim);

    return;
}
void do_rank( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int rank;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if(ch->clan <= 12)
    {
        send_to_char( "You are not in a guild or tribe!\n\r",ch);
        return;
    }
    if(ch->clan == clan_lookup("mage"))
    {
        send_to_char("Use Promote.\n\r",ch);
        return;
    }
    if(ch->rank < 9)
    {
        send_to_char( "You are not high enough rank to set someone's rank!\n\r",ch);
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char( "Syntax: rank <char> <rank>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here to rank.\n\r", ch );
        return;
    }
    if(victim->clan != ch->clan)
    {
        if(ch->race == race_lookup("garou"))
            send_to_char( "They are not a member of your tribe!\n\r", ch );
        else
            send_to_char( "They are not a member of your guild!\n\r", ch );
        return;
    }
    rank = atoi( arg2 );
    if(rank > ch->rank)
    {
        send_to_char( "You cannot set someone to a higher rank then yourself!\n\r", ch );
        return;
    }
    if(victim->rank > ch->rank)
    {
        send_to_char( "You cannot set someone higher rank then yourself!\n\r", ch );
        return;
    }
    if(rank > 10 || rank < 1)
    {
        send_to_char( "Minimum rank is 1, Maximum is 10.\n\r", ch );
        return;
    }

    victim->rank = rank;
    sprintf(buf,"They are now rank %d of %s.\n\r", rank,
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,ch);
    sprintf(buf,"You are now rank %d of %s.\n\r", rank,
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,victim);

    return;
}

void do_promote( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if(ch->avatar < 0)
    {
        send_to_char( "Your Avatar has not been Awakened!\n\r",ch);
        return;
    }
    if(ch->rank < 10)
    {
        send_to_char( "Only the leader of a Tradition may promote or demote another.\n\r",ch);
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char( "Syntax: promote <char> <rank>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here to promote.\n\r", ch );
        return;
    }

    if(IS_NPC(victim) || IS_NPC(ch)) return;

    if(victim->tradition != ch->tradition)
    {
            send_to_char( "They are not a member of your Tradition\n\r", ch );
        return;
    }
    if(!str_prefix( arg2, "mentor" ))
    {
        if(victim->rank < 8)
        {
            send_to_char("Only those of the rank Master may attain the honor of becoming a Mentor.\n\r",ch);
            return;
        }
        if(victim->pcdata->IC_total < (50*60))
        {
            send_to_char("They have not yet spent the time needed to gain the experience of a Mentor.\n\r",ch);
            return;
        }

        if(victim->rank == 9)
        {
            send_to_char("That person is already a Mentor.\n\r",ch);
            return;
        }
        victim->rank = 9;
        send_to_char("You have attained the honor of becoming a Mentor of your Tradition.\n\r",victim);
        send_to_char("You bestow the honor of Mentor.\n\r",ch);
    }
    else if(!str_prefix( arg2, "disciple" ))
    {
        if(victim->rank == 2)
        {
            send_to_char("They are already a Disciple.\n\r",ch);
            return;
        }
        if(victim->rank > 2) send_to_char("You are being demoted to the rank of Disciple.\n\r",victim);
        if(victim->rank < 2)
        {
            if(victim->pcdata->IC_total < (20*60))
            {
                send_to_char("They have not attended enough lessons to attain the rank of Disciple.\n\r",ch);
                return;
            }
            send_to_char("You are promoted to the rank of Disciple.\n\r",victim);
        }
        send_to_char("You assign them the rank of Disciple.\n\r",ch);
        victim->rank = 2;
        victim->apprentice = NULL;
        return;
    }
    else if(!str_prefix( arg2, "master"))
    {
        bool master = FALSE;
        int i;
        if(victim->rank < 2 || victim->rank >= 8)
        {
            send_to_char("You may only promote Disciples into Masters.\n\r",ch);
            return;
        }
        if(victim->pcdata->IC_total < (40*60))
        {
            send_to_char("They have not spent the time needed to become Masters.\n\r",ch);
            return;
        }

        for(i = 9; i != 0; i--)
            if(victim->sphere[i] == 5) master = TRUE;
        if(master)
        {
            send_to_char("You attain the title of Master!\n\r",victim);
            send_to_char("You promote them to the title of Master.\n\r",ch);
            victim->rank = 8;
        }
        else
        {
            send_to_char("They have yet to attain the required Spheres.\n\r",ch);
            return;
        }
    }
    else if(!str_prefix( arg2, "leader"))
    {
        if(victim->rank < 8)
        {
            send_to_char("You may only pass on your leadership to a Master of your Tradition.\n\r",ch);
            return;
        }
        if(victim == ch)
        {
            if(ch->apprentice == NULL)
            {
                send_to_char("You are already leader of your Tradition.\n\r",ch);
                return;
            }
            else
            {
                send_to_char("You are now ready to seek out another Apprentice.\n\r",ch);
                ch->apprentice = NULL;
                return;
            }
            return;
        }
        sprintf(buf,"You pass on your leadership of %s to %s. You are demoted to the title of Master.\n\r",capitalize(tradition_table[ch->tradition].name), capitalize(victim->name));
        send_to_char(buf,ch);
        sprintf(buf,"%s passes on the leadership of %s to it. You now lead the Tradition. May you use your power wisely.\n\r",capitalize(ch->name), capitalize(tradition_table[ch->tradition].name));
        send_to_char(buf,ch);
        ch->rank = 8;
        victim->rank = 10;
        return;
    }
    else
        send_to_char("That is not a valid choice.\n\r",ch);
    return;
}



void do_expel( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if(ch->clan <= 12)
    {
        send_to_char( "You are not in a guild!\n\r",ch);
        return;
    }
    if(ch->rank < 9)
    {
        send_to_char( "You are not high enough rank to expel someone!\n\r",ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char( "Syntax: expel <char>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here to expel.\n\r", ch );
        return;
    }
    if(ch->rank < victim->rank)
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }

    if(victim->clan != ch->clan)
    {
        if(ch->race == race_lookup("garou"))
            send_to_char( "They are not a member of your tribe!\n\r", ch );
        else
            send_to_char( "They are not a member of your guild!\n\r", ch );
        return;
    }
    victim->rank = 1;
    victim->pcdata->learned[skill_lookup("sanctus lucis")] = 0;
    victim->pcdata->learned[skill_lookup("scry")] = 0;
    victim->renown = 0;
    victim->tradition = 0;
    victim->arete = 0;
    victim->avatar = 0;
    victim->paradox = 0;
    victim->quintessence = 0;
    victim->sphere[SPHERE_PRIME] = 0;
    victim->sphere[SPHERE_SPIRIT] = 0;
    victim->sphere[SPHERE_TIME] = 0;
    victim->sphere[SPHERE_MIND] = 0;
    victim->sphere[SPHERE_MATTER] = 0;
    victim->sphere[SPHERE_LIFE] = 0;
    victim->sphere[SPHERE_FORCES] = 0;
    victim->sphere[SPHERE_CORRESPONDENCE] = 0;
    victim->sphere[SPHERE_ENTROPY] = 0;
    sprintf(buf,"You have expelled %s from %s\n\r", victim->name,
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,ch);
    sprintf(buf,"You are no longer a member of %s!\n\r",
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,victim);
    victim->clan = 0;
    return;
}
void do_induct( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if(ch->clan <= 13 || ch->race != race_lookup("garou"))
    {
        send_to_char( "You are not of a Tribe!\n\r",ch);
        return;
    }
    if(ch->rank < 9)
    {
        if(ch->sex == SEX_MALE)
            send_to_char( "You are not the Alpha Male!\n\r",ch);
        else
            send_to_char( "You are not the Alpha Female!\n\r",ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char( "Syntax: induct <char>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "That Garou isn't here to induct.\n\r", ch );
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char( "You cannot induct this creature!\n\r", ch );
        return;
    }
    if(victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
    {
        send_to_char( "How dare you consider inducting that Wyrm spawn!\n\r", ch );
        return;
    }
    if(victim->race != race_lookup("garou"))
    {
        send_to_char("They are not Garou!\n\r",ch);
        return;
    }
    if(victim->clan != 0)
    {
        send_to_char( "They are already in a tribe!\n\r",ch);
        return;
    }
    victim->clan = ch->clan;
    victim->rank = 1;
    sprintf(buf,"They are now a member of the %s tribe.\n\r",
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,ch);
    sprintf(buf,"You are welcomed as if family, into %s!\n\r",
        capitalize(clan_table[victim->clan].name));
    send_to_char(buf,victim);

    return;
}

bool vampsightcheck(CHAR_DATA *ch)
{
    int chance;

    chance = number_range(1, 100);
    chance += ch->level;

    if (ch->clan == clan_lookup("watcher") && chance >= 90)
        return TRUE;
    else
        return FALSE;
}


void do_fixme(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch)) return;

    ch->exp = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
    send_to_char("Your fixed!\n\r", ch);
    return;
}

void do_gwho(CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if(ch->clan == 0)
    {
        send_to_char("+----------------------------------------+\n\r", ch);
        send_to_char("| You are not in a {RClan{y/{GTribe{y/{CTradition{x. |\n\r", ch);
        send_to_char("+----------------------------------------+\n\r", ch);
        return;
    }

    send_to_char(" ______________________________________________ \n\r", ch);
    if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah") || ch->race == race_lookup("ghoul"))
    {
      sprintf( buf, ":%s:\n\r", center("{RClan Members{x", 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ":{y%s{x:\n\r", center(capitalize(clan_table[ch->clan].name), 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ": Leader - %-12s   Banker - %-12s:\n\r", capitalize(clan_table[ch->clan].leader), capitalize(clan_table[ch->clan].banker));
      send_to_char(buf, ch);
      send_to_char(":----------------------------------------------:\n\r", ch);
      sprintf( buf, ":%s:\n\r", center("{YCurrently Online{x", 46, "_"));
      send_to_char(buf, ch);
      send_to_char(": Lvl | Name           || Generation           :\n\r", ch);
    }
    else if(ch->race == race_lookup("garou"))
    {
      sprintf( buf, ":%s:\n\r", center("{GTribe Members{x", 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ":{y%s{x:\n\r", center(capitalize(tribe_table[ch->pcdata->tribe].name), 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ": Leader - %-12s   Banker - %-12s:\n\r", capitalize(clan_table[ch->clan].leader), capitalize(clan_table[ch->clan].banker));
      send_to_char(buf, ch);
      send_to_char(":----------------------------------------------:\n\r", ch);
      sprintf( buf, ":%s:\n\r", center("{YCurrently Online{x", 46, "_"));
      send_to_char(buf, ch);
      send_to_char(": Lvl | Name           || Rank                 :\n\r", ch);    }
    else if(ch->clan == clan_lookup("watcher"))
    {
        sprintf( buf, "Your silent brethren of the {W%s{x sect of the Inquisition{x:\n\r", capitalize(clan_table[ch->clan].name));
        send_to_char(buf,ch);
        send_to_char("[Lvl  | Name   | Rank | Hit Points |  Mana   | Movement ]\n\r", ch);
    }
    else
    {
      sprintf( buf, ":%s:\n\r", center("{CTradition Members{x", 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ":{y%s{x:\n\r", center(capitalize(tradition_table[ch->tradition].name), 46, " "));
      send_to_char(buf,ch);
      sprintf( buf, ": Leader - %-12s   Banker - %-12s:\n\r", capitalize(clan_table[ch->clan].leader), capitalize(clan_table[ch->clan].banker));
      send_to_char(buf, ch);
      send_to_char(":----------------------------------------------:\n\r", ch);
      sprintf( buf, ":%s:\n\r", center("{YCurrently Online{x", 46, "_"));
      send_to_char(buf, ch);
      send_to_char(": Lvl | Name           || Rank                 :\n\r", ch);
    }
    send_to_char(":==============================================:\n\r", ch);

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        CHAR_DATA *victim;
        victim = d->original ? d->original : d->character;
        if ( d->connected == CON_PLAYING && ch->clan == victim->clan && !IS_IMMORTAL(victim))
        {
            if(victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
              sprintf( buf, ": %s%d | %-15s|| %s           :\n\r",
                victim->level < 10 ? "  " : victim->level < 100 ? " " : "",
                victim->level,
                victim->name,
                victim->pcdata->csgeneration == 1 ? "First     " : victim->pcdata->csgeneration == 2 ? "Second    " : victim->pcdata->csgeneration == 3 ? "Third     " :
                victim->pcdata->csgeneration == 4 ? "Fourth    " : victim->pcdata->csgeneration == 5 ? "Fifth     " : victim->pcdata->csgeneration == 6 ? "Sixth     " : victim->pcdata->csgeneration == 7 ? "Seventh   " :
                victim->pcdata->csgeneration == 8 ? "Eighth    " : victim->pcdata->csgeneration == 9 ? "Ninth     " : victim->pcdata->csgeneration == 10 ? "Tenth     " : victim->pcdata->csgeneration == 11 ? "Eleventh  " :
                victim->pcdata->csgeneration == 12 ? "Twelfth   " : victim->pcdata->csgeneration == 13 ? "Thirteenth" : "Unknown   ");
            else if(victim->race == race_lookup("ghoul"))
              sprintf( buf, ": %s%d | %-15s|| %s           :\n\r",
                victim->level < 10 ? "  " : victim->level < 100 ? " " : "",
                victim->level,
                victim->name,
                "{rGhoul{x     ");
            else if(victim->race == race_lookup("garou"))
              sprintf( buf, ": %s%d | %-15s|| %s         :\n\r",
                victim->level < 10 ? "  " : victim->level < 100 ? " " : "",
                victim->level,
                victim->name,
                victim->pcdata->rank == 1 ? "Cliath      " : victim->pcdata->rank == 2 ? "Fostern     " : victim->pcdata->rank == 3 ? "Adren       " :
                victim->pcdata->rank == 4 ? "Athro       " : victim->pcdata->rank == 5 ? "Elder       " : "Pup         ");
            else if(victim->clan == clan_lookup("watcher"))
                sprintf( buf, "[%d%s | %s | %s | %d/%d | %d/%d | %d/%d]\n\r",
                    victim->level,
                    victim->level < 100 ? " " : "",
                    victim->name,
                    victim->rank == 1 ? "Init" : victim-> rank == 2 ? "Aclyt" : victim->rank == 7 ? "Counc" : victim->rank == 8 ? "ProFi" : victim->rank == 9 ? "SupCn" : victim->rank == 10 ? "GrInq" : "Disciple",
                    victim->hit, victim->max_hit,
                    victim->mana, victim->max_mana,
                    victim->move, victim->max_move);
            else
                sprintf( buf, ": %s%d | %-15s|| %s         :\n\r",
                    victim->level < 10 ? "  " : victim->level < 100 ? " " : "",
                    victim->level,
                    victim->name,
                    victim->rank == 1 ? "Apprentice  " : victim->rank == 2 ? "Disciple    " : victim->rank == 8 ? "Master      " : victim->rank == 9 ? "Mentor      " : "Leader      ");
            send_to_char( buf, ch);
        }
    }
    send_to_char(" TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT \n\r", ch);
    return;
}

void do_sect(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int sect;

    if (IS_NPC(ch))
        return;

    argument = one_argument( argument, arg );

    if(arg[0] == '\0')
    {
        send_to_char("Join which sect?\n\r",ch);
        return;
    }

    if(!str_prefix( arg, "none" ))

    {

        send_to_char("You abandon all sects.\n\r",ch);

        ch->pcdata->sect = 0;

        return;

    }

    if(ch->clan <= 0)
    {
        send_to_char("You aren't in a clan!\n\r",ch);
        return;
    }


    if ((sect = sect_lookup(arg)) == 0)
    {
        send_to_char("There is no such sect\n\r",ch);
        return;
    }

    if(sect_table[sect].vamp && ch->race != race_lookup("vampire") && ch->race != race_lookup("methuselah") && ch->race != race_lookup("dhampire"))
    {
        send_to_char("You cannot join that sect.\n\r",ch);
        return;
    }
    if(!str_cmp(sect_table[sect].name, "Tribe") && ch->race != race_lookup("garou") && ch->race != race_lookup("dhampire"))
    {
        send_to_char("You are not one of Gaia's Warriors.\n\r",ch);
        return;
    }

    if(!str_cmp(sect_table[sect].name, "Immortal") && !IS_IMMORTAL(ch))
    {
        send_to_char("You cannot join that sect.\n\r",ch);
        return;
    }

    if (ch->pcdata->sect == sect)
    {
        sprintf(buf,"You are already a member of the %s.\n\r",sect_table[sect].name);
        send_to_char(buf,ch);
    }
    else
    {
        sprintf(buf,"You now a member of the %s.\n\r",sect_table[sect].name);
        send_to_char(buf,ch);
        ch->pcdata->sect = sect;
    }
return;
}
void do_glance( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int percent;

    argument = one_argument( argument, arg );

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char("Who???\n\r",ch);
    return;
    }

    if ( can_see( victim, ch ) )
    {
    if (ch == victim)
        act( "$n glances at $mself.",ch,NULL,NULL,TO_ROOM);
    else
    {
        act( "$n glances at you.", ch, NULL, victim, TO_VICT    );
        act( "$n glances at $N.",  ch, NULL, victim, TO_NOTVICT );
    }
    }

    if ( victim->description[0] != '\0' )
    {
    send_to_char( victim->description, ch );
    }
    else
    {
    act( "You see nothing special about $M.", ch, NULL, victim, TO_CHAR );
    }

    send_to_char("\n\r", ch);

        if (!IS_NPC(victim))
        {
                send_to_char("{WAppearance_____", ch);
                if (get_attribute(victim, APPEARANCE) == 0)
                {
                        send_to_char("{DX{wOOOO{x\n\r", ch);
                        act("Not even $S own mother could love a face like that.", ch, NULL, victim, TO_CHAR);
                }
                if (get_attribute(victim, APPEARANCE) == 1)
                {
                        send_to_char("@{wOOOO{x\n\r", ch);
                        act("$E looks like $E got hit one too many times with the ugly stick.", ch, NULL, victim, TO_CHAR);
                }
                if (get_attribute(victim, APPEARANCE) == 2)
                {
                        send_to_char("@@{wOOO{x\n\r", ch);
                        act("$E is kinda funny lookin, but not altogether unattractive.", ch, NULL, victim, TO_CHAR);
                }
                if (get_attribute(victim, APPEARANCE) == 3)
                {
                        send_to_char("@@@{wOO{x\n\r", ch);
                        act("$E likely turns heads any place $E goes.", ch, NULL, victim, TO_CHAR);
                }
                if (get_attribute(victim, APPEARANCE) == 4)
                {
                        send_to_char("@@@@{wO{x\n\r", ch);
                        act("The bards sing praise of $S beauty, immortalizing it in song.", ch, NULL, victim, TO_CHAR);
                }
                if (get_attribute(victim, APPEARANCE) == 5)
                {
                        send_to_char("@@@@@{x\n\r", ch);
                        act("$E could have anyone, anytime, anywhere.", ch, NULL, victim, TO_CHAR);
                }
                send_to_char("\n\r", ch);
    }



    if ( MOUNTED(victim) )
    {
        sprintf( buf, "%s is riding %s.\n\r", victim->name, MOUNTED(victim)->short_descr);
        send_to_char( buf, ch);
    }
    if ( RIDDEN(victim) )
    {
        sprintf( buf, "%s is being ridden by %s.\n\r", victim->short_descr, RIDDEN(victim)->name );
        send_to_char( buf, ch);
    }

    if (IS_AFFECTED(victim,AFF_FANGS))
    act( "$N has inch long fangs protruding from their lips!", ch, NULL, victim, TO_CHAR );


    if ( victim->max_hit > 0 )
    percent = ( 100 * victim->hit ) / victim->max_hit;
    else
    percent = -1;

    strcpy( buf, PERS(victim, ch, FALSE) );

    if (percent >= 100)
    strcat( buf, " is in excellent condition.\n\r");
    else if (percent >= 90)
    strcat( buf, " has a few scratches.\n\r");
    else if (percent >= 75)
    strcat( buf," has some small wounds and bruises.\n\r");
    else if (percent >=  50)
    strcat( buf, " has quite a few wounds.\n\r");
    else if (percent >= 30)
    strcat( buf, " has some big nasty wounds and scratches.\n\r");
    else if (percent >= 15)
    strcat ( buf, " looks pretty hurt.\n\r");
    else if (percent >= 0 )
    strcat (buf, " is in awful condition.\n\r");
    else
    strcat(buf, " is bleeding to death.\n\r");

    buf[0] = UPPER(buf[0]);
    send_to_char( buf, ch );
    return;
}

void do_qlog(CHAR_DATA *ch, char *argument)
{
    int i,quests, line;
    char buf[50][MAX_STRING_LENGTH];
    char temp_buf[MAX_STRING_LENGTH];
//  char arg1[MAX_INPUT_LENGTH];

    quests = 0;
    line = -1;

    if(IS_NPC(ch))
    {
        sprintf(temp_buf,"My quest number is %d!",ch->quest);
        do_function(ch, &do_say, temp_buf);
        return;
    }

    for(i = 0;i < MAX_QUEST;i++)
        if(ch->pcdata->quest_id[i] != 0) quests++;


    if(quests > 10)
    {
        send_to_char("You have too many quests. Contact Ugha and bug him to death to finish the page code.\n\r",ch);
        return;
    }


    for(i = 0;i < 50;i++)
    {
        if(i < 25)
            sprintf(buf[i]," |||                                   |");
        else
            sprintf(buf[i],"                                    ||| \n\r");
    }

    for(i = 0;quests;i++)
    {
        if(!quests) break;
        quests--;
        if(i < 5)
        {
            sprintf(buf[++line]," |||  {Y%s{x   |",wordwrap(0, 30, ch->pcdata->quest_name[quests],ALIGN_CENTER));
            sprintf(temp_buf,"[{g%d%% Complete{x]",ch->pcdata->quest_step[quests]);
            sprintf(buf[++line]," |||  %s   |",wordwrap(0,30,temp_buf,ALIGN_CENTER));
            sprintf(buf[++line]," |||  {W%s{x   |",wordwrap(0,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));
            sprintf(buf[++line]," |||  {W%s{x   |",wordwrap(1,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));
            sprintf(buf[++line]," |||  {W%s{x   |",wordwrap(2,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));
        }
        else
        {
            sprintf(buf[++line],"   {Y%s{x   ||| \n\r",wordwrap(0,30,ch->pcdata->quest_name[quests],ALIGN_CENTER));
            sprintf(temp_buf,"[{g%d%% Complete{x]",ch->pcdata->quest_step[quests]);
            sprintf(buf[++line],"   %s   ||| \n\r",wordwrap(0,30,temp_buf,ALIGN_CENTER));
            sprintf(buf[++line],"   {W%s{x   ||| \n\r",wordwrap(0,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));
            sprintf(buf[++line],"   {W%s{x   ||| \n\r",wordwrap(1,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));
            sprintf(buf[++line],"   {W%s{x   ||| \n\r",wordwrap(2,30,ch->pcdata->quest_string[quests], ALIGN_CENTER));

        }
    }

//  sprintf(temp_buf,"%s's Quest Journal    Page %d of %d",ch->name,

    sprintf(temp_buf,"%s's Quest Journal. Page 1 (of 1)",ch->name);
    sprintf(temp_buf,"%s\n\r",wordwrap(0,78,temp_buf,ALIGN_CENTER));
    send_to_char(temp_buf,ch);
    send_to_char("   ____________________________________ _____________________________________ \n\r",ch);
    send_to_char("  /|                                   V                                    | \\\n\r",ch);
    send_to_char(" |||                                  \\|/                                   ||| \n\r",ch);
    for(i = 0;i < 25;i++)
    {
        strcat(buf[i],buf[i+25]);
        send_to_char(buf[i],ch);
    }
    send_to_char(" |||                                  \\|/                                   ||| \n\r",ch);
    send_to_char(" |_____________________________________^______________________________________| \n\r",ch);
    return;
}

void do_contact(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    if(IS_NPC(ch)) return;

    if(argument[0] == '\0')
    {
        send_to_char("This command is used to send an email to the Admins. Abuse of this command results in nasty consequences.\n\r",ch);
        send_to_char("Syntax: contact <message>\n\r",ch);
        return;
    }

    if(!ch->pcdata->registered)
    {
        send_to_char("You must have a valid email account registered in Haven to use this command.\n\r",ch);
        return;
    }

    if(ch->remorts < 5)
    {
        send_to_char("You must have at least five remorts to use this command.\n\r",ch);
        return;
    }

    sprintf(buf,"%s (%s)\n\n-----\n\n%s\n",ch->name,ch->pcdata->email,argument);
    if(send_email("Contact","admin@havenmud.net",buf) == FALSE) send_to_char("Email bug. Please contact the immortals at once.",ch);
    else send_to_char("Your message has been sent to the Admins.\n\rThey will either log on to reply or reply via email to your registered email address.\n\rRemember: Abuse of this command results in nasty consequences you don't want to face.\n\r",ch);
    return;
}

void do_clanbank (CHAR_DATA *ch, char *argument )
{
    char command[MIL];
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int amnt;

    if (IS_NPC(ch))
        return;

    if (!is_clan(ch))
    {
    send_to_char("You aren't in a clan.\n\r",ch);
    return;
    }

    if (!IS_BANKER(ch, ch->clan))
    {
        sendch("You are not authorized to use your clan's bank!\n\r", ch);
        return;
    }

    argument = one_argument(argument, command);
    if ( command[0] == '\0' )
    {
    sprintf( buf, "Syntax: clanbank  <balance/deposit/withdraw>\n\r" );
    send_to_char( buf, ch );
    return;
    }

    if (!str_cmp(command, "balance"))
    {
    char buf[MAX_STRING_LENGTH];
    sprintf( buf, "Your clan has {y%d{x gold in the bank.\n\r", clan_table[ch->clan].bank );
    send_to_char( buf, ch );
    return;
    }

    if (!str_cmp(command, "deposit"))
    {
        if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
        {
        sprintf( buf, "But you are not in a bank.\n\r" );
        send_to_char( buf, ch );
        return;
        }

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
        sprintf( buf, "How many coins do you wish to deposit?\n\r" );
        send_to_char( buf, ch );
        return;
        }

        amnt = atoi( arg );
        if ( amnt <= 0 )
        {
         sprintf( buf, "No, no I don't think so. Try withdraw.\n\r" );
           send_to_char( buf, ch );
          return;
        }
        if ( amnt >= (ch->gold + 1) )
        {
        sprintf( buf, "%s, you do not have {y%d{x coins.\n\r", ch->name, amnt );
        send_to_char( buf, ch );
        return;
        }

        clan_table[ch->clan].bank += amnt;
        ch->gold -= amnt;
        sprintf( buf, "%s, your clan's account now contains: {y%d{x coins, after depositing {y%d{x coins.\n\r", ch->name, clan_table[ch->clan].bank, amnt );
        send_to_char(buf, ch );
        SET_BIT( clan_table[ch->clan].flags, CLAN_CHANGED );
        save_clans (ch, argument);
        return;
    }
    if (!str_cmp(command, "withdraw"))
    {

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
    {
    sprintf( buf, "But you are not in a bank.\n\r" );
    send_to_char( buf, ch );
    return;
    }

    argument = one_argument(argument, arg);\
    if ( arg[0] == '\0' )
    {
     sprintf( buf, "How many coins do you wish to withdraw?\n\r" );
     send_to_char( buf, ch );
     return;
    }

    amnt = atoi( arg );
    if ( amnt <= 0 )
    {
     sprintf( buf, "No, no I don't think so. Try deposit.\n\r" );
     send_to_char( buf, ch );
     return;
    }
    if ( amnt >= (clan_table[ch->clan].bank + 1) )
    {
    sprintf( buf, "%s, your clan does not have {y%d{x coins in the bank.\n\r", ch->name, amnt );
    send_to_char(buf, ch );
    return;
    }

    ch->gold += amnt;
    clan_table[ch->clan].bank -= amnt;
    sprintf( buf, "%s, your clan's account now contains: {y%d{x coins, after withdrawing {y%d{x coins.\n\r", ch->name, clan_table[ch->clan].bank, amnt );
    send_to_char( buf, ch );
        SET_BIT( clan_table[ch->clan].flags, CLAN_CHANGED );
        save_clans (ch, argument);
    return;
    }
}

void do_bank (CHAR_DATA *ch, char *argument )
{
    char command[MIL];
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int amnt;
    if (IS_NPC(ch))
        return;

    argument = one_argument(argument, command);
    if ( command[0] == '\0' )
    {
    sprintf( buf, "Syntax: bank  <balance/deposit/withdraw>\n\r" );
    send_to_char( buf, ch );
    return;
    }

    if (str_cmp(command, "balance") && str_cmp(command, "deposit") && str_cmp(command, "withdraw"))
    {
        send_to_char("That is not a valid bank command.\n\r", ch);
        return;
    }

    if (!str_cmp(command, "balance"))
    {
    char buf[MAX_STRING_LENGTH];
    sprintf( buf, "You have {y%d{x gold in the bank.\n\r", ch->pcdata->bank );
    send_to_char( buf, ch );
    return;
    }

    if (!str_cmp(command, "deposit"))
    {
        if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
        {
        sprintf( buf, "But you are not in a bank.\n\r" );
        send_to_char( buf, ch );
        return;
        }

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
        {
        sprintf( buf, "How many coins do you wish to deposit?\n\r" );
        send_to_char( buf, ch );
        return;
        }

        amnt = atoi( arg );
        if ( amnt <= 0 )
        {
         sprintf( buf, "No, no I don't think so. Try withdraw.\n\r" );
          send_to_char( buf, ch );
          return;
        }
        if ( amnt >= (ch->gold + 1) )
        {
        sprintf( buf, "%s, you do not have {y%d{x coins.\n\r", ch->name, amnt );
        send_to_char( buf, ch );
        return;
        }

        ch->pcdata->bank += amnt;
        ch->gold -= amnt;
        sprintf( buf, "%s, your account now contains: {y%d{x coins, after depositing {y%d{x coins.\n\r", ch->name, ch->pcdata->bank, amnt );
        send_to_char(buf, ch );
        return;
    }
    if (!str_cmp(command, "withdraw"))
    {

    if (!IS_SET(ch->in_room->room_flags, ROOM_BANK))
    {
    sprintf( buf, "But you are not in a bank.\n\r" );
    send_to_char( buf, ch );
    return;
    }

    argument = one_argument(argument, arg);

    if ( arg[0] == '\0' )
    {
     sprintf( buf, "How many coins do you wish to withdraw?\n\r" );
     send_to_char( buf, ch );
     return;
    }

    amnt = atoi( arg );
    if ( amnt <= 0 )
    {
     sprintf( buf, "No, no I don't think so. Try deposit.\n\r" );
     send_to_char( buf, ch );
     return;
    }
    if ( amnt >= (ch->pcdata->bank + 1) )
    {
    sprintf( buf, "%s, you do not have {y%d{x coins in the bank.\n\r", ch->name, amnt );
    send_to_char(buf, ch );
    return;
    }

    ch->gold += amnt;
    ch->pcdata->bank -= amnt;
    sprintf( buf, "%s, your account now contains: {y%d{x coins, after withdrawing {y%d{x coins.\n\r", ch->name, ch->pcdata->bank, amnt );
    send_to_char( buf, ch );
    return;
    }
}



// Matthew - Added so IMMs can change passwords on pfiles.
void do_pnewpass( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA d;
    bool isChar = FALSE;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *pArg;
    char *pwdnew;
    char *p;
    char cEnd;

    if ( IS_NPC(ch) )
    return;

    /*
     * Can't use one_argument here because it smashes case.
     * So we just steal all its code.  Bleagh.
     */
    pArg = arg1;
    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *pArg++ = *argument++;
    }
    *pArg = '\0';

    pArg = arg2;
    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *pArg++ = *argument++;
    }
    *pArg = '\0';

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Syntax: pnewpass <name> <password>.\n\r", ch );
    return;
    }

    if ( strlen(arg2) < 5 )
    {
    send_to_char(
        "New password must be at least five characters long.\n\r", ch );
    return;
    }

      /* Dont want to load a second copy of a player who's allready online! */
  if ( get_char_world( ch, arg1 ) != NULL )
  {
    send_to_char( "That person is connected. They can reset their own password.\n\r", ch );
    return;
  }
    isChar = load_char_obj(&d, arg1);

  if (!isChar)
  {
    send_to_char("Change whose password?\n\r", ch);
    return;
  }

    d.character->desc     = NULL;
    d.character->next     = char_list;
    char_list             = d.character;
    d.connected           = CON_PLAYING;
    reset_char(d.character);
    char_to_room(d.character,get_room_index(ROOM_VNUM_ALTAR));
      /*
     * No tilde allowed because of player file format.
     */
    pwdnew = str_dup( arg2 );
    for ( p = pwdnew; *p != '\0'; p++ )
    {
    if ( *p == '~' )
    {
        send_to_char(
        "New password not acceptable, try again.\n\r", ch );
        return;
    }
    }

    free_string( d.character->pcdata->pwd );
    d.character->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( d.character );
    extract_char( d.character, TRUE );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_checkhelps (CHAR_DATA *ch, char *argument)
{
    char buf[MSL], buf2[MSL];
    char arg1[MIL];

    BUFFER *output;

    output= new_buf();

    // now display the specified number of lines of the log file.
    argument = one_argument( argument, arg1 );
    if (IS_NULLSTR(arg1))
    {
        sprintf(buf, "tail -n 10 " NOHELPS_FILE );
        add_buf(output, "\r\nYou can select the number of loglines, type checkhelp <number of lines>\n\r");

    }
    else if (is_number ( arg1 ))
    {
        int value = atoi(arg1);
        if (value<1 || value>20000)
        {
            add_buf(output,"\r\n`RNumber of lines to tail must be between 1 and 20000.`x\r\n");
            sprintf(buf, "tail -n 10 " NOHELPS_FILE);
        }
        else
        {
            sprintf(buf, "tail -n %d " NOHELPS_FILE " | head -n 40", value);
        }
    }
    else
    {
        add_buf(output, "\r\n`RThe only parameter for this command must be a "
            "numeric value\r\nfor the number of lines of the error "
            "log you wish to see.`x\r\n");
        sprintf(buf, "tail -n 10 " NOHELPS_FILE );
    }

    add_buf(output,get_piperesult(buf));

    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}

void do_checktypos (CHAR_DATA *ch, char *argument)
{
    char buf[MSL], buf2[MSL];
    char arg1[MIL];

    BUFFER *output;

    output= new_buf();

    // now display the specified number of lines of the log file.
    argument = one_argument( argument, arg1 );
    if (IS_NULLSTR(arg1))
    {
        sprintf(buf, "tail -n 10 " TYPO_FILE );
        add_buf(output, "\r\nYou can select the number of loglines, type checktypos <number of lines>\n\r");

    }
    else if (is_number ( arg1 ))
    {
        int value = atoi(arg1);
        if (value<1 || value>20000)
        {
            add_buf(output,"\r\n`RNumber of lines to tail must be between 1 and 20000.`x\r\n");
            sprintf(buf, "tail -n 10 " TYPO_FILE);
        }
        else
        {
            sprintf(buf, "tail -n %d " TYPO_FILE " | head -n 40", value);
        }
    }
    else
    {
        add_buf(output, "\r\n`RThe only parameter for this command must be a "
            "numeric value\r\nfor the number of lines of the error "
            "log you wish to see.`x\r\n");
        sprintf(buf, "tail -n 10 " TYPO_FILE );
    }

    add_buf(output,get_piperesult(buf));

    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}

void do_checkbugs (CHAR_DATA *ch, char *argument)
{
    char buf[MSL], buf2[MSL];
    char arg1[MIL];

    BUFFER *output;

    output= new_buf();

    // now display the specified number of lines of the log file.
    argument = one_argument( argument, arg1 );
    if (IS_NULLSTR(arg1))
    {
        sprintf(buf, "tail -n 10 " BUG_FILE );
        add_buf(output, "\r\nYou can select the number of loglines, type checkbugs <number of lines>\n\r");

    }
    else if (is_number ( arg1 ))
    {
        int value = atoi(arg1);
        if (value<1 || value>20000)
        {
            add_buf(output,"\r\n`RNumber of lines to tail must be between 1 and 20000.`x\r\n");
            sprintf(buf, "tail -n 10 " BUG_FILE);
        }
        else
        {
            sprintf(buf, "tail -n %d " BUG_FILE " | head -n 40", value);
        }
    }
    else
    {
        add_buf(output, "\r\n`RThe only parameter for this command must be a "
            "numeric value\r\nfor the number of lines of the error "
            "log you wish to see.`x\r\n");
        sprintf(buf, "tail -n 10 " BUG_FILE );
    }

    add_buf(output,get_piperesult(buf));

    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}

void do_systemlog (CHAR_DATA *ch, char *argument)
{
    char buf[MSL], buf2[MSL];
    char arg1[MIL];

    BUFFER *output;

    output= new_buf();

    // now display the specified number of lines of the log file.
    argument = one_argument( argument, arg1 );
    if (IS_NULLSTR(arg1))
    {
        sprintf(buf, "tail -n 10 " SYSTEM_LOG );
        add_buf(output, "\r\nYou can select the number of loglines, type systemlog <number of lines>\n\r");

    }
    else if (is_number ( arg1 ))
    {
        int value = atoi(arg1);
        if (value<1 || value>20000)
        {
            add_buf(output,"\r\n`RNumber of lines to tail must be between 1 and 20000.`x\r\n");
            sprintf(buf, "tail -n 10 " SYSTEM_LOG);
        }
        else
        {
            sprintf(buf, "tail -n %d " SYSTEM_LOG " | head -n 40", value);
        }
    }
    else
    {
        add_buf(output, "\r\n`RThe only parameter for this command must be a "
            "numeric value\r\nfor the number of lines of the error "
            "log you wish to see.`x\r\n");
        sprintf(buf, "tail -n 10 " SYSTEM_LOG );
    }

    add_buf(output,get_piperesult(buf));

    page_to_char(buf_string(output), ch);
    free_buf(output);

    return;
}

void do_dinfo (CHAR_DATA *ch, char *argument)
{
    AREA_DATA *area;
    DOMAIN_DATA *dom;
    char outbuf[MSL];
    int i;

    if (IS_NULLSTR(argument) || !is_number(argument))
    {
        cprintf( ch, "[%3s] %-27s (%-10s)\n\r", "Num", "Area Name", "Ruling Clan" );

        for (area = area_first ; area != NULL; area = area->next)
        {
            if ((dom = area->domain) == NULL)
                continue;

            cprintf( ch, "[%3d] %-27s (%-10s)\n\r", dom->vnum,
            area->name, capitalize(clan_table[dom->clan].name));
        }
        sendch("\n\rFor more information on a specific domain, type 'dinfo <vnum>'.\n\r", ch);
        return;
    }
    if (is_number(argument))
    {
        bool found = FALSE;
        for (area = area_first; area != NULL ; area = area->next)
        {
            if (area->domain == NULL)
                continue;

                dom = area->domain;

            if (dom->vnum == atoi(argument))
            {
                found = TRUE;
                break;
            }
        }
        if (!found)
        {sendch("Domain not found.\n\r", ch);return;}
        else {
        char relation[100];
        cprintf(ch, "Domain Data for Area:%s\n\r", area->name);
        cprintf(ch,  "Domain Vnum:         {D[{w%d{D]{x\n\r", area->domain->vnum);
        cprintf(ch, "Ruling Clan:         {D[{R%s{D]{x\n\r",
        capitalize(clan_table[area->domain->clan].name));
        cprintf(ch, "Economy Tax:     [%d]\n\r", area->domain->tax);
        cprintf( ch, "%s\n\r", "Clan Influence:");
        int i, col;
        for (i = 1; i < MAX_CLAN; i++)
            {
                if (dom->influence[i] == 0)
                continue;
                cprintf( ch, "{D: %-10s {w%3d {c(%3d){x ", capitalize(clan_table[i].name), dom->influence[i], dom->change[i]);
                 col++;
                if (col > 1 )
                    {
                    send_to_char("\n\r", ch);
                    col = 0;
                    }
                }
            sendch("\n\rClan Relations:\n\r", ch);
       for (i = 1; i < MAX_CLAN ; i++ )
      {
          if (dom->relation[i] == RELATION_NONE)
            continue;
            switch (dom->relation[i])
                {
                    case RELATION_ALLIED: sprintf(relation, "Ally");break;
                    case RELATION_WARRING: sprintf(relation, "Warring");break;
                    default: sprintf(relation, "Neutral");break;
                }
          cprintf(ch, "%12s %s\n\r", clan_table[i].name, relation);
      }

            return;
        }

    }
}

void do_addict (CHAR_DATA *ch, char *argument)
{
    PC_DATA *pcdata;
    int count, mins, secs, hours;
    double bonus;
    char buf[MSL];
    BUFFER *output;
    output = new_buf();
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;
    bool first, last;
    first = last = FALSE;
    char buf2[MSL];



    add_buf(output, "-=============================-  Addict Data  -=============================-\n\r");
    add_buf(output, "Legend:          {G^^^{D = {wFirst{x                     {rvvv{D = {wLast{x\n\r");
    add_buf(output, "                 {Y>>>{D = {wNext{x                      {y---{D = {wNone{x\n\r");
    if (IS_IMMORTAL(ch))
    {
        add_buf(output, "                 {W!!!{D = {wImmortal{x\n\r");
        add_buf(output, "\n\r{D[POS][Play Time][Bonus ] Name            - Login Time{x\n\r");

        for ( d = descriptor_list; d; d = d->next )
        {
            if (!d->character || d->connected > CON_PLAYING)
                continue;

            vch = d->character;
            secs = ((int)current_time - vch->logon);
            mins = secs / 60;
            hours = mins / 60;

            if (hours)
            {
                if (!IS_IMMORTAL(vch))
                {
                    bonus = get_addict_mult(vch);
                } else {
                    bonus = 13.37;
                }

                mins = mins % 60;
                sprintf(buf, "{D[{w%5d{mh{w%2.2d{mm{D][{W%5.2f{w%%{D]{w %-15s{D - {y%s{x", hours, mins,bonus, vch->name, ctime(&vch->logon));
            } else {
            sprintf(buf, "{D[{m%8dm{D][{W%5d{w%%{D]{w %-15s{D - {y%s{x", mins, 0, vch->name, ctime(&vch->logon));
            }
        add_buf(output, buf);
        }

    } else {
        add_buf(output, "\n\r{D[Play Time][Bonus ] - Login Time{x\n\r");
        secs = ((int)current_time - ch->logon);
        mins = secs / 60;
        hours = mins / 60;

        if (hours)
        {
            bonus = get_addict_mult(ch);
            mins = mins % 60;
            sprintf(buf, "{D[{w%5d{mh{w%2.2d{mm{D][{W%5.2f{w%%{D] - {y%s{x", hours, mins, bonus, ctime(&ch->logon));
        } else {
            sprintf(buf, "{D[{w%8d{mm{D][{w------{D] - {y%s{x", mins, ctime(&ch->logon));
        }
    add_buf(output, buf);

    }

    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
}
