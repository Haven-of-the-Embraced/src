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
*   ROM 2.4 is copyright 1993-1995 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@pacinfo.com)                  *
*       Gabrielle Taylor (gtaylor@pacinfo.com)             *
*       Brian Moore (rom@rom.efn.org)                  *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  MOBprograms for ROM 2.4 v0.98g (C) M.Nylander 1996                     *
 *  Based on MERC 2.2 MOBprograms concept by N'Atas-ha.                    *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *  This code may be copied and distributed as per the ROM license.        *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <ctype.h>
#include "merc.h"
#include "tables.h"
#include "lookup.h"
#include "recycle.h"

SLEEP_DATA *first_sleep = NULL;

extern int flag_lookup( const char *word, const struct flag_type *flag_table );

/*
 * These defines correspond to the entries in fn_keyword[] table.
 * If you add a new if_check, you must also add a #define here.
 */
#define CHK_RAND    (0)
#define CHK_MOBHERE     (1)
#define CHK_OBJHERE     (2)
#define CHK_MOBEXISTS   (3)
#define CHK_OBJEXISTS   (4)
#define CHK_PEOPLE      (5)
#define CHK_PLAYERS     (6)
#define CHK_MOBS        (7)
#define CHK_CLONES      (8)
#define CHK_ORDER       (9)
#define CHK_HOUR        (10)
#define CHK_ISPC        (11)
#define CHK_ISNPC       (12)
#define CHK_ISGOOD      (13)
#define CHK_ISEVIL      (14)
#define CHK_ISNEUTRAL   (15)
#define CHK_ISIMMORT    (16)
#define CHK_ISCHARM     (17)
#define CHK_ISFOLLOW    (18)
#define CHK_ISACTIVE    (19)
#define CHK_ISDELAY     (20)
#define CHK_ISVISIBLE   (21)
#define CHK_HASTARGET   (22)
#define CHK_ISTARGET    (23)
#define CHK_EXISTS      (24)
#define CHK_AFFECTED    (25)
#define CHK_ACT         (26)
#define CHK_OFF         (27)
#define CHK_IMM         (28)
#define CHK_CARRIES     (29)
#define CHK_WEARS       (30)
#define CHK_HAS         (31)
#define CHK_USES        (32)
#define CHK_NAME        (33)
#define CHK_POS         (34)
#define CHK_CLAN        (35)
#define CHK_RACE        (36)
#define CHK_CLASS       (37)
#define CHK_OBJTYPE     (38)
#define CHK_VNUM        (39)
#define CHK_HPCNT       (40)
#define CHK_ROOM        (41)
#define CHK_SEX         (42)
#define CHK_LEVEL       (43)
#define CHK_ALIGN       (44)
#define CHK_MONEY       (45)
#define CHK_OBJVAL0     (46)
#define CHK_OBJVAL1     (47)
#define CHK_OBJVAL2     (48)
#define CHK_OBJVAL3     (49)
#define CHK_OBJVAL4     (50)
#define CHK_GRPSIZE     (51)
#define CHK_QUEST_STEP  (52)
#define CHK_REMORT      (53)

/*
 * These defines correspond to the entries in fn_evals[] table.
 */
#define EVAL_EQ            0
#define EVAL_GE            1
#define EVAL_LE            2
#define EVAL_GT            3
#define EVAL_LT            4
#define EVAL_NE            5

/*
 * if-check keywords:
 */
const char * fn_keyword[] =
{
    "rand",     /* if rand 30       - if random number < 30 */
    "mobhere",      /* if mobhere dog  - is there a 'dog' here */
    "objhere",      /* if objhere bottle    - is there a 'bottle' here */
            /* if mobhere 1233  - is there mob vnum 1233 here */
            /* if objhere 1233  - is there obj vnum 1233 here */
    "mobexists",    /* if mobexists dog    - is there a dog somewhere */
    "objexists",    /* if objexists sword   - is there a sword somewhere */

    "people",       /* if people > 4    - does room contain > 4 people */
    "players",      /* if players > 1   - does room contain > 1 pcs */
    "mobs",     /* if mobs > 2      - does room contain > 2 mobiles */
    "clones",       /* if clones > 3    - are there > 3 mobs of same vnum here */
    "order",        /* if order == 0    - is mob the first in room */
    "hour",     /* if hour > 11     - is the time > 11 o'clock */


    "ispc",     /* if ispc $n       - is $n a pc */
    "isnpc",        /* if isnpc $n      - is $n a mobile */
    "isgood",       /* if isgood $n     - is $n good */
    "isevil",       /* if isevil $n     - is $n evil */
    "isneutral",    /* if isneutral $n  - is $n neutral */
    "isimmort",     /* if isimmort $n   - is $n immortal */
    "ischarm",      /* if ischarm $n    - is $n charmed */
    "isfollow",     /* if isfollow $n   - is $n following someone */
    "isactive",     /* if isactive $n   - is $n's position > SLEEPING */
    "isdelay",      /* if isdelay $i    - does $i have mobprog pending */
    "isvisible",    /* if isvisible $n  - can mob see $n */
    "hastarget",    /* if hastarget $i  - does $i have a valid target */
    "istarget",     /* if istarget $n   - is $n mob's target */
    "exists",       /* if exists $n     - does $n exist somewhere */

    "affected",     /* if affected $n blind - is $n affected by blind */
    "act",      /* if act $i sentinel   - is $i flagged sentinel */
    "off",              /* if off $i berserk    - is $i flagged berserk */
    "imm",              /* if imm $i fire   - is $i immune to fire */
    "carries",      /* if carries $n sword  - does $n have a 'sword' */
            /* if carries $n 1233   - does $n have obj vnum 1233 */
    "wears",        /* if wears $n lantern  - is $n wearing a 'lantern' */
            /* if wears $n 1233 - is $n wearing obj vnum 1233 */
    "has",          /* if has $n weapon - does $n have obj of type weapon */
    "uses",     /* if uses $n armor - is $n wearing obj of type armor */
    "name",     /* if name $n puff  - is $n's name 'puff' */
    "pos",      /* if pos $n standing   - is $n standing */
    "clan",     /* if clan $n 'whatever'- does $n belong to clan 'whatever' */
    "race",     /* if race $n dragon    - is $n of 'dragon' race */
    "class",        /* if class $n mage - is $n's class 'mage' */
    "objtype",      /* if objtype $p scroll - is $p a scroll */

    "vnum",     /* if vnum $i == 1233   - virtual number check */
    "hpcnt",        /* if hpcnt $i > 30 - hit point percent check */
    "room",     /* if room $i == 1233   - room virtual number */
    "sex",      /* if sex $i == 0   - sex check */
    "level",        /* if level $n < 5  - level check */
    "align",        /* if align $n < -1000  - alignment check */
    "money",        /* if money $n > 2000  - Money check (total, gold + silver)*/
    "objval0",      /* if objval0 > 1000    - object value[] checks 0..4 */
    "objval1",
    "objval2",
    "objval3",
    "objval4",
    "grpsize",      /* if grpsize $n > 6    - group size check */
    "quest_step", /* if quest_step $n == 100  - quest status check */
    "remort", /* if remort $n >= 50     - check remorts on char*/
    "\n"        /* Table terminator */
};

const char *fn_evals[] =
{
    "==",
    ">=",
    "<=",
    ">",
    "<",
    "!=",
    "\n"
};

/*
 * Return a valid keyword from a keyword table
 */
int keyword_lookup( const char **table, char *keyword )
{
    register int i;
    for( i = 0; table[i][0] != '\n'; i++ )
        if( !str_cmp( table[i], keyword ) )
            return( i );
    return -1;
}

/*
 * Perform numeric evaluation.
 * Called by cmd_eval()
 */
int num_eval( int lval, int oper, int rval )
{
    switch( oper )
    {
        case EVAL_EQ:
             return ( lval == rval );
        case EVAL_GE:
             return ( lval >= rval );
        case EVAL_LE:
             return ( lval <= rval );
        case EVAL_NE:
             return ( lval != rval );
        case EVAL_GT:
             return ( lval > rval );
        case EVAL_LT:
             return ( lval < rval );
        default:
             bug( "num_eval: invalid oper", 0 );
             return 0;
    }
}

/*
 * ---------------------------------------------------------------------
 * UTILITY FUNCTIONS USED BY CMD_EVAL()
 * ----------------------------------------------------------------------
 */

/*
 * Get a random PC in the room (for $r parameter)
 */
 CHAR_DATA *get_random_char( CHAR_DATA *mob, OBJ_DATA *obj, ROOM_INDEX_DATA *room )
 {
     CHAR_DATA *vch, *victim = NULL;
     int now = 0, highest = 0;

     if ( (mob && obj) || (mob && room) || (obj && room) )
     {
 	bug( "get_random_char received multiple prog types",0);
 	return NULL;
     }

     if ( mob )
 	vch = mob->in_room->people;
     else if ( obj )
     {
 	if ( obj->in_room )
 	    vch = obj->in_room->people;
 	else
 	    vch = obj->carried_by->in_room->people;
     }
     else
 	vch = room->people;

     for( ; vch; vch = vch->next_in_room )
     {
         if ( mob
 	&&   mob != vch
        &&   !IS_NPC( vch )
        &&   can_see( mob, vch )
        &&   ( now = number_percent() ) > highest )
        {
            victim = vch;
            highest = now;
        }
        else if ( (now = number_percent()) > highest )
     	{
    	    victim = vch;
    	    highest = now;
    	}
        }
    return victim;
}

/*
 * How many other players / mobs are there in the room
 * iFlag: 0: all, 1: players, 2: mobiles 3: mobs w/ same vnum 4: same group
 */
 int count_people_room( CHAR_DATA *mob, OBJ_DATA *obj, ROOM_INDEX_DATA *room, int iFlag )
 {
     CHAR_DATA *vch;
     int count;

     if ( (mob && obj) || (mob && room) || (obj && room) )
     {
 	bug( "count_people_room received multiple prog types",0);
 	return 0;
     }

     if ( mob )
 	vch = mob->in_room->people;
     else if ( obj )
     {
 	if ( obj->in_room )
 	    vch = obj->in_room->people;
 	else
 	    vch = obj->carried_by->in_room->people;
     }
     else
 	vch = room->people;

     for ( count = 0; vch; vch = vch->next_in_room )
     {
 	if ( mob )
 	{
    if ( mob != vch
    &&   (iFlag == 0
      || (iFlag == 1 && !IS_NPC( vch ))
      || (iFlag == 2 && IS_NPC( vch ))
      || (iFlag == 3 && IS_NPC( mob ) && IS_NPC( vch )
         && mob->pIndexData->vnum == vch->pIndexData->vnum )
      || (iFlag == 4 && is_same_group( mob, vch )) )
    && can_see( mob, vch ) )
        count++;
    }
    else if ( obj || room )
    {
        if ( iFlag == 0
        || (iFlag == 1 && !IS_NPC( vch ))
        || (iFlag == 2 && IS_NPC( vch )))
        count++;
    }
    }

    return ( count );
}

/*
 * Get the order of a mob in the room. Useful when several mobs in
 * a room have the same trigger and you want only the first of them
 * to act
 */
 int get_order( CHAR_DATA *ch, OBJ_DATA *obj )
 {
     CHAR_DATA *vch;
     OBJ_DATA *vobj;
     int i;

     if ( ch && obj )
     {
 	bug( "get_order received multiple prog types",0);
 	return 0;
     }

     if ( ch && !IS_NPC(ch) )
 	return 0;

     if ( ch )
     {
 	vch = ch->in_room->people;
 	vobj = NULL;
     }
     else
     {
 	vch = NULL;
 	if ( obj->in_room )
 	    vobj = obj->in_room->contents;
 	else if ( obj->carried_by->in_room->contents )
 	    vobj = obj->carried_by->in_room->contents;
 	else
 	    vobj = NULL;
     }

     if ( ch )
 	for ( i = 0; vch; vch = vch->next_in_room )
 	{
    if ( vch == ch )
        return i;

    if ( IS_NPC(vch)
    &&   vch->pIndexData->vnum == ch->pIndexData->vnum )
        i++;
    }
    else
    for ( i = 0; vobj; vobj = vobj->next_content )
    {
        if ( vobj == obj )
        return i;

        if ( vobj->pIndexData->vnum == obj->pIndexData->vnum )
        i++;
    }

    return 0;
}

/*
 * Check if ch has a given item or item type
 * vnum: item vnum or -1
 * item_type: item type or -1
 * fWear: TRUE: item must be worn, FALSE: don't care
 */
bool has_item( CHAR_DATA *ch, sh_int vnum, sh_int item_type, bool fWear )
{
    OBJ_DATA *obj;
    for ( obj = ch->carrying; obj; obj = obj->next_content )
    if ( ( vnum < 0 || obj->pIndexData->vnum == vnum )
    &&   ( item_type < 0 || obj->pIndexData->item_type == item_type )
    &&   ( !fWear || obj->wear_loc != WEAR_NONE ) )
        return TRUE;
    return FALSE;
}

/*
 * Check if there's a mob with given vnum in the room
 */
 bool get_mob_vnum_room( CHAR_DATA *ch, OBJ_DATA *obj, ROOM_INDEX_DATA *room, sh_int vnum )
 {
     CHAR_DATA *mob;

     if ( (ch && obj) || (ch && room) || (obj && room) )
     {
 	bug( "get_mob_vnum_room received multiple prog types",0);
 	return FALSE;
     }

     if ( ch )
 	mob = ch->in_room->people;
     else if ( obj )
     {
 	if ( obj->in_room )
 	    mob = obj->in_room->people;
 	else
 	    mob = obj->carried_by->in_room->people;
     }
     else
 	mob = room->people;

     for ( ; mob; mob = mob->next_in_room )
    if ( IS_NPC( mob ) && mob->pIndexData->vnum == vnum )
        return TRUE;
    return FALSE;
}

/*
 * Check if there's an object with given vnum in the room
 */
 bool get_obj_vnum_room( CHAR_DATA *ch, OBJ_DATA *obj, ROOM_INDEX_DATA *room, sh_int vnum )
 {
     OBJ_DATA *vobj;

     if ( (ch && obj) || (ch && room) || (obj && room) )
     {
 	bug( "get_obj_vnum_room received multiple prog types",0);
 	return FALSE;
     }

     if ( ch )
 	vobj = ch->in_room->contents;
     else if ( obj )
     {
 	if ( obj->in_room )
 	    vobj = obj->in_room->contents;
 	else
 	    vobj = obj->carried_by->in_room->contents;
     }
     else
 	vobj = room->contents;

     for ( ; vobj; vobj = vobj->next_content )
 	if ( vobj->pIndexData->vnum == vnum )
        return TRUE;
    return FALSE;
}

/* ---------------------------------------------------------------------
 * CMD_EVAL
 * This monster evaluates an if/or/and statement
 * There are five kinds of statement:
 * 1) keyword and value (no $-code)     if random 30
 * 2) keyword, comparison and value     if people > 2
 * 3) keyword and actor                 if isnpc $n
 * 4) keyword, actor and value          if carries $n sword
 * 5) keyword, actor, comparison and value  if level $n >= 10
 *
 *----------------------------------------------------------------------
 */
int cmd_eval_mob( sh_int vnum, char *line, int check,
    CHAR_DATA *mob, CHAR_DATA *ch,
    const void *arg1, const void *arg2, CHAR_DATA *rch )
{
    CHAR_DATA *lval_char = mob;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    OBJ_DATA  *lval_obj = NULL;

    char *original, buf[MAX_INPUT_LENGTH], code;
    int lval = 0, oper = 0, rval = -1;

    original = line;
    line = one_argument( line, buf );
    if ( buf[0] == '\0' || mob == NULL )
    return FALSE;

    /*
     * If this mobile has no target, let's assume our victim is the one
     */
    if ( mob->mprog_target == NULL )
    mob->mprog_target = ch;

    switch ( check )
    {
    /*
     * Case 1: keyword and value
     */
    case CHK_RAND:
        return( atoi( buf ) < number_percent() );
        case CHK_MOBHERE:
    	    if ( is_number( buf ) )
    		return( get_mob_vnum_room( mob, NULL, NULL, atoi(buf) ) );
    	    else
    		return( (bool) (get_char_room( mob, NULL, buf) != NULL) );
    	case CHK_OBJHERE:
    	    if ( is_number( buf ) )
    		return( get_obj_vnum_room( mob, NULL, NULL, atoi(buf) ) );
    	    else
    		return( (bool) (get_obj_here( mob, NULL, buf) != NULL) );
            case CHK_MOBEXISTS:
    	    return( (bool) (get_char_world( mob, buf) != NULL) );
    	case CHK_OBJEXISTS:
    	    return( (bool) (get_obj_world( mob, buf, FALSE) != NULL) );
    	/*
    	 * Case 2 begins here: We sneakily use rval to indicate need
    	 * 		       for numeric eval...
    	 */
    	case CHK_PEOPLE:
    	    rval = count_people_room( mob, NULL, NULL, 0 ); break;
    	case CHK_PLAYERS:
    	    rval = count_people_room( mob, NULL, NULL, 1 ); break;
    	case CHK_MOBS:
    	    rval = count_people_room( mob, NULL, NULL, 2 ); break;
    	case CHK_CLONES:
    	    rval = count_people_room( mob, NULL, NULL, 3 ); break;
    	case CHK_ORDER:
    	    rval = get_order( mob, NULL ); break;
    	case CHK_HOUR:
        rval = time_info.hour; break;
    default:;
    }

    /*
     * Case 2 continued: evaluate expression
     */
    if ( rval >= 0 )
    {
    if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
    {
	    sprintf( buf, "Cmd_eval_mob: prog %d syntax error(2) '%s'",
        vnum, original );
        bug( buf, 0 );
        return FALSE;
    }
    one_argument( line, buf );
    lval = rval;
    rval = atoi( buf );
    return( num_eval( lval, oper, rval ) );
    }

    /*
     * Case 3,4,5: Grab actors from $* codes
     */
    if ( buf[0] != '$' || buf[1] == '\0' )
    {
	sprintf( buf, "Cmd_eval_mob: prog %d syntax error(3) '%s'",
        vnum, original );
    bug( buf, 0 );
        return FALSE;
    }
    else
        code = buf[1];
    switch( code )
    {
        case 'i':
            lval_char = mob; break;
        case 'n':
            lval_char = ch; break;
        case 't':
            lval_char = vch; break;
        case 'r':
            lval_char = rch == NULL ? get_random_char( mob, NULL, NULL ) : rch ; break;
        case 'o':
            lval_obj = obj1; break;
        case 'p':
            lval_obj = obj2; break;
    case 'q':
        lval_char = mob->mprog_target; break;
    default:
	    sprintf( buf, "Cmd_eval_mob: prog %d syntax error(4) '%s'",
        vnum, original );
        bug( buf, 0 );
        return FALSE;
    }
    /*
     * From now on, we need an actor, so if none was found, bail out
     */
    if ( lval_char == NULL && lval_obj == NULL )
        return FALSE;

    /*
     * Case 3: Keyword, comparison and value
     */
    switch( check )
    {
    case CHK_ISPC:
            return( lval_char != NULL && !IS_NPC( lval_char ) );
        case CHK_ISNPC:
            return( lval_char != NULL && IS_NPC( lval_char ) );
        case CHK_ISGOOD:
            return( lval_char != NULL && IS_GOOD( lval_char ) );
        case CHK_ISEVIL:
            return( lval_char != NULL && IS_EVIL( lval_char ) );
        case CHK_ISNEUTRAL:
            return( lval_char != NULL && IS_NEUTRAL( lval_char ) );
    case CHK_ISIMMORT:
            return( lval_char != NULL && IS_IMMORTAL( lval_char ) );
        case CHK_ISCHARM: /* A relic from MERC 2.2 MOBprograms */
            return( lval_char != NULL && IS_AFFECTED( lval_char, AFF_CHARM ) );
        case CHK_ISFOLLOW:
            return( lval_char != NULL && lval_char->master != NULL
         && lval_char->master->in_room == lval_char->in_room );
    case CHK_ISACTIVE:
        return( lval_char != NULL && lval_char->position > POS_SLEEPING );
    case CHK_ISDELAY:
        return( lval_char != NULL && lval_char->mprog_delay > 0 );
    case CHK_ISVISIBLE:
            switch( code )
            {
                default :
                case 'i':
                case 'n':
                case 't':
                case 'r':
        case 'q':
                return( lval_char != NULL && can_see( mob, lval_char ) );
        case 'o':
        case 'p':
                return( lval_obj != NULL && can_see_obj( mob, lval_obj ) );
        }
    case CHK_HASTARGET:
        return( lval_char != NULL && lval_char->mprog_target != NULL
        &&  lval_char->in_room == lval_char->mprog_target->in_room );
    case CHK_ISTARGET:
        return( lval_char != NULL && mob->mprog_target == lval_char );
    default:;
     }

     /*
      * Case 4: Keyword, actor and value
      */
     line = one_argument( line, buf );
     switch( check )
     {
    case CHK_AFFECTED:
        return( lval_char != NULL
        &&  IS_SET(lval_char->affected_by, flag_lookup(buf, affect_flags)) );
    case CHK_ACT:
        return( lval_char != NULL
        &&  IS_SET(lval_char->act, flag_lookup(buf, act_flags)) );
    case CHK_IMM:
        return( lval_char != NULL
        &&  IS_SET(lval_char->imm_flags, flag_lookup(buf, imm_flags)) );
    case CHK_OFF:
        return( lval_char != NULL
        &&  IS_SET(lval_char->off_flags, flag_lookup(buf, off_flags)) );
    case CHK_CARRIES:
        if ( is_number( buf ) )
        return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, FALSE ) );
        else
        return( lval_char != NULL && (get_obj_carry( lval_char, buf, lval_char ) != NULL) );
    case CHK_WEARS:
        if ( is_number( buf ) )
        return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, TRUE ) );
        else
		return( lval_char != NULL && (get_obj_wear( lval_char, buf, TRUE ) != NULL) );
    case CHK_HAS:
        return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), FALSE ) );
    case CHK_USES:
        return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), TRUE ) );
    case CHK_NAME:
            switch( code )
            {
                default :
                case 'i':
                case 'n':
                case 't':
                case 'r':
        case 'q':
            return( lval_char != NULL && is_name( buf, lval_char->name ) );
        case 'o':
        case 'p':
            return( lval_obj != NULL && is_name( buf, lval_obj->name ) );
        }
    case CHK_POS:
        return( lval_char != NULL && lval_char->position == position_lookup( buf ) );
    case CHK_CLAN:
        return( lval_char != NULL && lval_char->clan == clan_lookup( buf ) );
    case CHK_RACE:
        return( lval_char != NULL && lval_char->race == race_lookup( buf ) );
    case CHK_CLASS:
        return( lval_char != NULL && lval_char->class == class_lookup( buf ) );
    case CHK_OBJTYPE:
        return( lval_obj != NULL && lval_obj->item_type == item_lookup( buf ) );
    default:;
    }

    /*
     * Case 5: Keyword, actor, comparison and value
     */
    if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
    {
	sprintf( buf, "Cmd_eval_mob: prog %d syntax error(5): '%s'",
        vnum, original );
    bug( buf, 0 );
    return FALSE;
    }
    one_argument( line, buf );
    rval = atoi( buf );

    switch( check )
    {
    case CHK_VNUM:
        switch( code )
            {
                default :
                case 'i':
                case 'n':
                case 't':
                case 'r':
        case 'q':
                    if( lval_char != NULL && IS_NPC( lval_char ) )
                        lval = lval_char->pIndexData->vnum;
                    break;
                case 'o':
                case 'p':
                     if ( lval_obj != NULL )
                        lval = lval_obj->pIndexData->vnum;
            }
            break;
    case CHK_HPCNT:
        if ( lval_char != NULL ) lval = (lval_char->hit * 100)/(UMAX(1,lval_char->max_hit)); break;
    case CHK_ROOM:
        if ( lval_char != NULL && lval_char->in_room != NULL )
        lval = lval_char->in_room->vnum; break;
        case CHK_SEX:
        if ( lval_char != NULL ) lval = lval_char->sex; break;
        case CHK_LEVEL:
            if ( lval_char != NULL ) lval = lval_char->level; break;
    case CHK_ALIGN:
            if ( lval_char != NULL ) lval = lval_char->alignment; break;
    case CHK_MONEY:  /* Money is converted to silver... Cindi was here too */
        if ( lval_char != NULL )
        lval =( lval_char->gold*100) + lval_char->silver; break;
        case CHK_OBJVAL0:
            if ( lval_obj != NULL ) lval = lval_obj->value[0]; break;
        case CHK_OBJVAL1:
            if ( lval_obj != NULL ) lval = lval_obj->value[1]; break;
        case CHK_OBJVAL2:
            if ( lval_obj != NULL ) lval = lval_obj->value[2]; break;
        case CHK_OBJVAL3:
            if ( lval_obj != NULL ) lval = lval_obj->value[3]; break;
    case CHK_OBJVAL4:
        if ( lval_obj != NULL ) lval = lval_obj->value[4]; break;
        case CHK_GRPSIZE:
    	    if( lval_char != NULL ) lval = count_people_room( lval_char, NULL, NULL, 4 ); break;
    /* quest log quest step comparison */
    case CHK_QUEST_STEP:
        if( lval_char != NULL ) lval = quest_status(lval_char, mob->quest); break;
    case CHK_REMORT:
        if (lval_char != NULL ) lval = lval_char->remorts; break;
    default:
            return FALSE;
    }
    return( num_eval( lval, oper, rval ) );
}

int cmd_eval_obj( sh_int vnum, char *line, int check,
	OBJ_DATA *obj, CHAR_DATA *ch,
	const void *arg1, const void *arg2, CHAR_DATA *rch )
{
    CHAR_DATA *lval_char = NULL;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    OBJ_DATA *lval_obj = obj;

    char *original, buf[MAX_INPUT_LENGTH], code;
    int lval = 0, oper = 0, rval = -1;

    original = line;
    line = one_argument( line, buf );
    if ( buf[0] == '\0' || obj == NULL )
	return FALSE;

    /*
     * If this object has no target, let's assume our victim is the one
     */
    if ( obj->oprog_target == NULL )
	obj->oprog_target = ch;

    switch ( check )
    {
	/*
	 * Case 1: keyword and value
	 */
	case CHK_RAND:
	    return( atoi( buf ) < number_percent() );
	case CHK_MOBHERE:
	    if ( is_number( buf ) )
		return( get_mob_vnum_room( NULL, obj, NULL,  atoi(buf) ) );
	    else
		return( (bool) (get_char_room( NULL, (obj->in_room?obj->in_room:obj->carried_by->in_room), buf) != NULL) );
	case CHK_OBJHERE:
	    if ( is_number( buf ) )
		return( get_obj_vnum_room( NULL, obj, NULL,  atoi(buf) ) );
	    else
		return( (bool) (get_obj_here( NULL, (obj->in_room?obj->in_room:obj->carried_by->in_room), buf) != NULL) );
        case CHK_MOBEXISTS:
	    return( (bool) (get_char_world( NULL, buf) != NULL) );
	case CHK_OBJEXISTS:
	    return( (bool) (get_obj_world( NULL, buf, FALSE) != NULL) );
	/*
	 * Case 2 begins here: We sneakily use rval to indicate need
	 * 		       for numeric eval...
	 */
	case CHK_PEOPLE:
	    rval = count_people_room( NULL, obj, NULL, 0 ); break;
	case CHK_PLAYERS:
	    rval = count_people_room( NULL, obj, NULL, 1 ); break;
	case CHK_MOBS:
	    rval = count_people_room( NULL, obj, NULL, 2 ); break;
	case CHK_CLONES:
	    bug( "cmd_eval_obj: received CHK_CLONES.",0); break;
	case CHK_ORDER:
	    rval = get_order( NULL, obj ); break;
	case CHK_HOUR:
	    rval = time_info.hour; break;
	default:;
    }

    /*
     * Case 2 continued: evaluate expression
     */
    if ( rval >= 0 )
    {
	if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
	{
	    sprintf( buf, "Cmd_eval_obj: prog %d syntax error(2) '%s'",
		vnum, original );
	    bug( buf, 0 );
	    return FALSE;
	}
	one_argument( line, buf );
	lval = rval;
	rval = atoi( buf );
	return( num_eval( lval, oper, rval ) );
    }

    /*
     * Case 3,4,5: Grab actors from $* codes
     */
    if ( buf[0] != '$' || buf[1] == '\0' )
    {
	sprintf( buf, "Cmd_eval_obj: prog %d syntax error(3) '%s'",
		vnum, original );
	bug( buf, 0 );
        return FALSE;
    }
    else
        code = buf[1];
    switch( code )
    {
    	case 'i':
            lval_obj = obj; break;
        case 'n':
            lval_char = ch; break;
        case 't':
            lval_char = vch; break;
        case 'r':
            lval_char = rch == NULL ? get_random_char( NULL, obj, NULL ) : rch ; break;
        case 'o':
            lval_obj = obj1; break;
        case 'p':
            lval_obj = obj2; break;
	case 'q':
	    lval_char = obj->oprog_target; break;
	default:
	    sprintf( buf, "Cmd_eval_obj: prog %d syntax error(4) '%s'",
		vnum, original );
	    bug( buf, 0 );
	    return FALSE;
    }
    /*
     * From now on, we need an actor, so if none was found, bail out
     */
    if ( lval_char == NULL && lval_obj == NULL )
    	return FALSE;

    /*
     * Case 3: Keyword, comparison and value
     */
    switch( check )
    {
	case CHK_ISPC:
            return( lval_char != NULL && !IS_NPC( lval_char ) );
        case CHK_ISNPC:
            return( lval_char != NULL && IS_NPC( lval_char ) );
        case CHK_ISGOOD:
            return( lval_char != NULL && IS_GOOD( lval_char ) );
        case CHK_ISEVIL:
            return( lval_char != NULL && IS_EVIL( lval_char ) );
        case CHK_ISNEUTRAL:
            return( lval_char != NULL && IS_NEUTRAL( lval_char ) );
	case CHK_ISIMMORT:
            return( lval_char != NULL && IS_IMMORTAL( lval_char ) );
        case CHK_ISCHARM: /* A relic from MERC 2.2 MOBprograms */
            return( lval_char != NULL && IS_AFFECTED( lval_char, AFF_CHARM ) );
        case CHK_ISFOLLOW:
            return( lval_char != NULL && lval_char->master != NULL
		 && lval_char->master->in_room == lval_char->in_room );
	case CHK_ISACTIVE:
	    return( lval_char != NULL && lval_char->position > POS_SLEEPING );
	case CHK_ISDELAY:
	    return( lval_char != NULL && lval_char->mprog_delay > 0 );
	case CHK_HASTARGET:
	    return( lval_char != NULL && lval_char->mprog_target != NULL
		&&  lval_char->in_room == lval_char->mprog_target->in_room );
	case CHK_ISTARGET:
	    return( lval_char != NULL && obj->oprog_target == lval_char );
	default:;
     }

     /*
      * Case 4: Keyword, actor and value
      */
     line = one_argument( line, buf );
     switch( check )
     {
	case CHK_AFFECTED:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->affected_by, flag_lookup(buf, affect_flags)) );
	case CHK_ACT:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->act, flag_lookup(buf, act_flags)) );
	case CHK_IMM:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->imm_flags, flag_lookup(buf, imm_flags)) );
	case CHK_OFF:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->off_flags, flag_lookup(buf, off_flags)) );
	case CHK_CARRIES:
	    if ( is_number( buf ) )
		return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, FALSE ) );
	    else
		return( lval_char != NULL && (get_obj_carry( lval_char, buf, lval_char ) != NULL) );
	case CHK_WEARS:
	    if ( is_number( buf ) )
		return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, TRUE ) );
	    else
		return( lval_char != NULL && (get_obj_wear( lval_char, buf, FALSE ) != NULL) );
	case CHK_HAS:
	    return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), FALSE ) );
	case CHK_USES:
	    return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), TRUE ) );
	case CHK_NAME:
            switch( code )
            {
                default :
                case 'n':
                case 't':
                case 'r':
		case 'q':
		    return( lval_char != NULL && is_name( buf, lval_char->name ) );
		case 'i':
		case 'o':
		case 'p':
		    return( lval_obj != NULL && is_name( buf, lval_obj->name ) );
	    }
	case CHK_POS:
	    return( lval_char != NULL && lval_char->position == position_lookup( buf ) );
	case CHK_CLAN:
	    return( lval_char != NULL && lval_char->clan == clan_lookup( buf ) );
	case CHK_RACE:
	    return( lval_char != NULL && lval_char->race == race_lookup( buf ) );
	case CHK_OBJTYPE:
	    return( lval_obj != NULL && lval_obj->item_type == item_lookup( buf ) );
	default:;
    }

    /*
     * Case 5: Keyword, actor, comparison and value
     */
    if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
    {
	sprintf( buf, "Cmd_eval_obj: prog %d syntax error(5): '%s'",
		vnum, original );
	bug( buf, 0 );
	return FALSE;
    }
    one_argument( line, buf );
    rval = atoi( buf );

    switch( check )
    {
	case CHK_VNUM:
	    switch( code )
            {
                default :
                case 'n':
                case 't':
                case 'r':
		case 'q':
                    if( lval_char != NULL && IS_NPC( lval_char ) )
                        lval = lval_char->pIndexData->vnum;
                    break;
		case 'i':
                case 'o':
                case 'p':
                     if ( lval_obj != NULL )
                        lval = lval_obj->pIndexData->vnum;
            }
            break;
	case CHK_HPCNT:
	    if ( lval_char != NULL ) lval = (lval_char->hit * 100)/(UMAX(1,lval_char->max_hit)); break;
	case CHK_ROOM:
	    if ( lval_char != NULL && lval_char->in_room != NULL )
		lval = lval_char->in_room->vnum;
	    else if ( lval_obj != NULL && (lval_obj->in_room != NULL || lval_obj->carried_by != NULL ))
		lval = lval_obj->in_room?lval_obj->in_room->vnum:lval_obj->carried_by->in_room->vnum;
	    break;
        case CHK_SEX:
	    if ( lval_char != NULL ) lval = lval_char->sex; break;
        case CHK_LEVEL:
            if ( lval_char != NULL ) lval = lval_char->level; break;
	case CHK_ALIGN:
            if ( lval_char != NULL ) lval = lval_char->alignment; break;
	case CHK_MONEY:  /* Money is converted to silver... */
	    if ( lval_char != NULL )
		lval = lval_char->gold + (lval_char->silver * 100); break;
	case CHK_OBJVAL0:
            if ( lval_obj != NULL ) lval = lval_obj->value[0]; break;
        case CHK_OBJVAL1:
            if ( lval_obj != NULL ) lval = lval_obj->value[1]; break;
        case CHK_OBJVAL2:
            if ( lval_obj != NULL ) lval = lval_obj->value[2]; break;
        case CHK_OBJVAL3:
            if ( lval_obj != NULL ) lval = lval_obj->value[3]; break;
	case CHK_OBJVAL4:
	    if ( lval_obj != NULL ) lval = lval_obj->value[4]; break;
	case CHK_GRPSIZE:
	    if( lval_char != NULL ) lval = count_people_room( lval_char, NULL, NULL, 4 ); break;
	default:
            return FALSE;
    }
    return( num_eval( lval, oper, rval ) );
}

int cmd_eval_room( sh_int vnum, char *line, int check,
	ROOM_INDEX_DATA *room, CHAR_DATA *ch,
	const void *arg1, const void *arg2, CHAR_DATA *rch )
{
    CHAR_DATA *lval_char = NULL;
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    OBJ_DATA *lval_obj = NULL;

    char *original, buf[MAX_INPUT_LENGTH], code;
    int lval = 0, oper = 0, rval = -1;

    original = line;
    line = one_argument( line, buf );
    if ( buf[0] == '\0' || room == NULL )
	return FALSE;

    /*
     * If this room has no target, let's assume our victim is the one
     */
    if ( room->rprog_target == NULL )
	room->rprog_target = ch;

    switch ( check )
    {
	/*
	 * Case 1: keyword and value
	 */
	case CHK_RAND:
	    return( atoi( buf ) < number_percent() );
	case CHK_MOBHERE:
	    if ( is_number( buf ) )
		return( get_mob_vnum_room( NULL, NULL, room,  atoi(buf) ) );
	    else
		return( (bool) (get_char_room( NULL, room, buf) != NULL) );
	case CHK_OBJHERE:
	    if ( is_number( buf ) )
		return( get_obj_vnum_room( NULL, NULL, room,  atoi(buf) ) );
	    else
		return( (bool) (get_obj_here( NULL, room, buf) != NULL) );
        case CHK_MOBEXISTS:
	    return( (bool) (get_char_world( NULL, buf) != NULL) );
	case CHK_OBJEXISTS:
	    return( (bool) (get_obj_world( NULL, buf, FALSE) != NULL) );
	/*
	 * Case 2 begins here: We sneakily use rval to indicate need
	 * 		       for numeric eval...
	 */
	case CHK_PEOPLE:
	    rval = count_people_room( NULL, NULL, room, 0 ); break;
	case CHK_PLAYERS:
	    rval = count_people_room( NULL, NULL, room, 1 ); break;
	case CHK_MOBS:
	    rval = count_people_room( NULL, NULL, room, 2 ); break;
	case CHK_CLONES:
	    bug( "Cmd_eval_room: received CHK_CLONES.",0); break;
	case CHK_ORDER:
	    bug( "Cmd_eval_room: received CHK_ORDER.",0); break;
	case CHK_HOUR:
	    rval = time_info.hour; break;
	default:;
    }

    /*
     * Case 2 continued: evaluate expression
     */
    if ( rval >= 0 )
    {
	if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
	{
	    sprintf( buf, "Cmd_eval_room: prog %d syntax error(2) '%s'",
		vnum, original );
	    bug( buf, 0 );
	    return FALSE;
	}
	one_argument( line, buf );
	lval = rval;
	rval = atoi( buf );
	return( num_eval( lval, oper, rval ) );
    }

    /*
     * Case 3,4,5: Grab actors from $* codes
     */
    if ( buf[0] != '$' || buf[1] == '\0' )
    {
	sprintf( buf, "Cmd_eval_room: prog %d syntax error(3) '%s'",
		vnum, original );
	bug( buf, 0 );
        return FALSE;
    }
    else
        code = buf[1];
    switch( code )
    {
    	case 'i':
            bug( "Cmd_eval_room: received code case 'i'.",0); break;
        case 'n':
            lval_char = ch; break;
        case 't':
            lval_char = vch; break;
        case 'r':
            lval_char = rch == NULL ? get_random_char( NULL, NULL, room ) : rch ; break;
        case 'o':
            lval_obj = obj1; break;
        case 'p':
            lval_obj = obj2; break;
	case 'q':
	    lval_char = room->rprog_target; break;
	default:
	    sprintf( buf, "Cmd_eval_room: prog %d syntax error(4) '%s'",
		vnum, original );
	    bug( buf, 0 );
	    return FALSE;
    }
    /*
     * From now on, we need an actor, so if none was found, bail out
     */
    if ( lval_char == NULL && lval_obj == NULL )
    	return FALSE;

    /*
     * Case 3: Keyword, comparison and value
     */
    switch( check )
    {
	case CHK_ISPC:
            return( lval_char != NULL && !IS_NPC( lval_char ) );
        case CHK_ISNPC:
            return( lval_char != NULL && IS_NPC( lval_char ) );
        case CHK_ISGOOD:
            return( lval_char != NULL && IS_GOOD( lval_char ) );
        case CHK_ISEVIL:
            return( lval_char != NULL && IS_EVIL( lval_char ) );
        case CHK_ISNEUTRAL:
            return( lval_char != NULL && IS_NEUTRAL( lval_char ) );
	case CHK_ISIMMORT:
            return( lval_char != NULL && IS_IMMORTAL( lval_char ) );
        case CHK_ISCHARM: /* A relic from MERC 2.2 MOBprograms */
            return( lval_char != NULL && IS_AFFECTED( lval_char, AFF_CHARM ) );
        case CHK_ISFOLLOW:
            return( lval_char != NULL && lval_char->master != NULL
		 && lval_char->master->in_room == lval_char->in_room );
	case CHK_ISACTIVE:
	    return( lval_char != NULL && lval_char->position > POS_SLEEPING );
	case CHK_ISDELAY:
	    return( lval_char != NULL && lval_char->mprog_delay > 0 );
	case CHK_HASTARGET:
	    return( lval_char != NULL && lval_char->mprog_target != NULL
		&&  lval_char->in_room == lval_char->mprog_target->in_room );
	case CHK_ISTARGET:
	    return( lval_char != NULL && room->rprog_target == lval_char );
	default:;
     }

     /*
      * Case 4: Keyword, actor and value
      */
     line = one_argument( line, buf );
     switch( check )
     {
	case CHK_AFFECTED:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->affected_by, flag_lookup(buf, affect_flags)) );
	case CHK_ACT:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->act, flag_lookup(buf, act_flags)) );
	case CHK_IMM:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->imm_flags, flag_lookup(buf, imm_flags)) );
	case CHK_OFF:
	    return( lval_char != NULL
		&&  IS_SET(lval_char->off_flags, flag_lookup(buf, off_flags)) );
	case CHK_CARRIES:
	    if ( is_number( buf ) )
		return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, FALSE ) );
	    else
		return( lval_char != NULL && (get_obj_carry( lval_char, buf, lval_char ) != NULL) );
	case CHK_WEARS:
	    if ( is_number( buf ) )
		return( lval_char != NULL && has_item( lval_char, atoi(buf), -1, TRUE ) );
	    else
		return( lval_char != NULL && (get_obj_wear( lval_char, buf, FALSE ) != NULL) );
	case CHK_HAS:
	    return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), FALSE ) );
	case CHK_USES:
	    return( lval_char != NULL && has_item( lval_char, -1, item_lookup(buf), TRUE ) );
	case CHK_NAME:
            switch( code )
            {
                default :
                case 'n':
                case 't':
                case 'r':
		case 'q':
		    return( lval_char != NULL && is_name( buf, lval_char->name ) );
		case 'i':
			return FALSE;
		case 'o':
		case 'p':
		    return( lval_obj != NULL && is_name( buf, lval_obj->name ) );
	    }
	case CHK_POS:
	    return( lval_char != NULL && lval_char->position == position_lookup( buf ) );
	case CHK_CLAN:
	    return( lval_char != NULL && lval_char->clan == clan_lookup( buf ) );
	case CHK_RACE:
	    return( lval_char != NULL && lval_char->race == race_lookup( buf ) );
	case CHK_OBJTYPE:
	    return( lval_obj != NULL && lval_obj->item_type == item_lookup( buf ) );
	default:;
    }

    /*
     * Case 5: Keyword, actor, comparison and value
     */
    if ( (oper = keyword_lookup( fn_evals, buf )) < 0 )
    {
	sprintf( buf, "Cmd_eval_room: prog %d syntax error(5): '%s'",
		vnum, original );
	bug( buf, 0 );
	return FALSE;
    }
    one_argument( line, buf );
    rval = atoi( buf );

    switch( check )
    {
	case CHK_VNUM:
	    switch( code )
            {
                default :
                case 'n':
                case 't':
                case 'r':
		case 'q':
                    if( lval_char != NULL && IS_NPC( lval_char ) )
                        lval = lval_char->pIndexData->vnum;
                    break;
		case 'i':
			return FALSE;
                case 'o':
                case 'p':
                     if ( lval_obj != NULL )
                        lval = lval_obj->pIndexData->vnum;
            }
            break;
	case CHK_HPCNT:
	    if ( lval_char != NULL ) lval = (lval_char->hit * 100)/(UMAX(1,lval_char->max_hit)); break;
	case CHK_ROOM:
	    if ( lval_char != NULL && lval_char->in_room != NULL )
		lval = lval_char->in_room->vnum;
	    else if ( lval_obj != NULL && (lval_obj->in_room != NULL || lval_obj->carried_by != NULL ) )
		lval = lval_obj->in_room?lval_obj->in_room->vnum:lval_obj->carried_by->in_room->vnum;
	    break;
        case CHK_SEX:
	    if ( lval_char != NULL ) lval = lval_char->sex; break;
        case CHK_LEVEL:
            if ( lval_char != NULL ) lval = lval_char->level; break;
	case CHK_ALIGN:
            if ( lval_char != NULL ) lval = lval_char->alignment; break;
	case CHK_MONEY:  /* Money is converted to silver... */
	    if ( lval_char != NULL )
		lval = lval_char->gold + (lval_char->silver * 100); break;
	case CHK_OBJVAL0:
            if ( lval_obj != NULL ) lval = lval_obj->value[0]; break;
        case CHK_OBJVAL1:
            if ( lval_obj != NULL ) lval = lval_obj->value[1]; break;
        case CHK_OBJVAL2:
            if ( lval_obj != NULL ) lval = lval_obj->value[2]; break;
        case CHK_OBJVAL3:
            if ( lval_obj != NULL ) lval = lval_obj->value[3]; break;
	case CHK_OBJVAL4:
	    if ( lval_obj != NULL ) lval = lval_obj->value[4]; break;
	case CHK_GRPSIZE:
	    if( lval_char != NULL ) lval = count_people_room( lval_char, NULL, NULL, 4 ); break;
	default:
            return FALSE;
    }
    return( num_eval( lval, oper, rval ) );
}

/*
 * ------------------------------------------------------------------------
 * EXPAND_ARG
 * These are hacks of act() in comm.c. I've added some safety guards,
 * so that missing or invalid $-codes do not crash the server
 * ------------------------------------------------------------------------
 */
void expand_arg_mob( char *buf,
    const char *format,
    CHAR_DATA *mob, CHAR_DATA *ch,
    const void *arg1, const void *arg2, CHAR_DATA *rch )
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
    const char *someone = "someone";
    const char *something = "something";
    const char *someones = "someone's";

    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return;

    point   = buf;
    str     = format;
    while ( *str != '\0' )
    {
        if ( *str != '$' )
        {
            *point++ = *str++;
            continue;
        }
        ++str;

        switch ( *str )
        {
            default:  bug( "Expand_arg_mob: bad code %d.", *str );
                          i = " <@@@> ";                        break;
            case 'i':
        one_argument( mob->name, fname );
		i = fname;                         		break;
            case 'I': i = mob->short_descr;                     break;
            case 'n':
        i = someone;
        if ( ch != NULL && can_see( mob, ch ) )
        {
                    one_argument( ch->name, fname );
            i = capitalize(fname);
		}						break;
            case 'N':
            i = (ch != NULL && can_see( mob, ch ) )
        ? ( IS_NPC( ch ) ? ch->short_descr : ch->name )
		: someone;                         		break;
            case 't':
        i = someone;
        if ( vch != NULL && can_see( mob, vch ) )
        {
                     one_argument( vch->name, fname );
             i = capitalize(fname);
		}						break;
            case 'T':
            i = (vch != NULL && can_see( mob, vch ))
        ? ( IS_NPC( vch ) ? vch->short_descr : vch->name )
		: someone;                         		break;
            case 'r':
        if ( rch == NULL )
		    rch = get_random_char( mob, NULL, NULL );
        i = someone;
        if( rch != NULL && can_see( mob, rch ) )
        {
                    one_argument( rch->name, fname );
            i = capitalize(fname);
		} 						break;
            case 'R':
        if ( rch == NULL )
		    rch = get_random_char( mob, NULL, NULL );
        i  = ( rch != NULL && can_see( mob, rch ) )
        ? ( IS_NPC( ch ) ? ch->short_descr : ch->name )
		:someone;					break;
        case 'q':
        i = someone;
        if ( mob->mprog_target != NULL && can_see( mob, mob->mprog_target ) )
            {
            one_argument( mob->mprog_target->name, fname );
            i = capitalize( fname );
		} 						break;
        case 'Q':
            i = (mob->mprog_target != NULL && can_see( mob, mob->mprog_target ))
        ? ( IS_NPC( mob->mprog_target ) ? mob->mprog_target->short_descr : mob->mprog_target->name )
		: someone;                         		break;
            case 'j': i = he_she  [URANGE(0, mob->sex, 2)];     break;
            case 'e':
            i = (ch != NULL && can_see( mob, ch ))
        ? he_she  [URANGE(0, ch->sex, 2)]
		: someone;					break;
            case 'E':
            i = (vch != NULL && can_see( mob, vch ))
        ? he_she  [URANGE(0, vch->sex, 2)]
		: someone;					break;
            case 'J':
        i = (rch != NULL && can_see( mob, rch ))
        ? he_she  [URANGE(0, rch->sex, 2)]
		: someone;					break;
        case 'X':
        i = (mob->mprog_target != NULL && can_see( mob, mob->mprog_target))
        ? he_she  [URANGE(0, mob->mprog_target->sex, 2)]
		: someone;					break;
            case 'k': i = him_her [URANGE(0, mob->sex, 2)];	break;
            case 'm':
            i = (ch != NULL && can_see( mob, ch ))
        ? him_her [URANGE(0, ch  ->sex, 2)]
		: someone;        				break;
            case 'M':
            i = (vch != NULL && can_see( mob, vch ))
        ? him_her [URANGE(0, vch ->sex, 2)]
		: someone;					break;
            case 'K':
        if ( rch == NULL )
		    rch = get_random_char( mob, NULL, NULL );
        i = (rch != NULL && can_see( mob, rch ))
        ? him_her [URANGE(0, rch ->sex, 2)]
		: someone;					break;
            case 'Y':
	    	i = (mob->mprog_target != NULL && can_see( mob, mob->mprog_target ))
		? him_her [URANGE(0, mob->mprog_target->sex, 2)]
		: someone;					break;
            case 'l': i = his_her [URANGE(0, mob ->sex, 2)];    break;
            case 's':
	    	i = (ch != NULL && can_see( mob, ch ))
		? his_her [URANGE(0, ch ->sex, 2)]
		: someones;					break;
            case 'S':
	    	i = (vch != NULL && can_see( mob, vch ))
		? his_her [URANGE(0, vch ->sex, 2)]
		: someones;					break;
            case 'L':
		if ( rch == NULL )
		    rch = get_random_char( mob, NULL, NULL );
		i = ( rch != NULL && can_see( mob, rch ) )
		? his_her [URANGE(0, rch ->sex, 2)]
		: someones;					break;
            case 'Z':
	    	i = (mob->mprog_target != NULL && can_see( mob, mob->mprog_target ))
		? his_her [URANGE(0, mob->mprog_target->sex, 2)]
		: someones;					break;
	    case 'o':
		i = something;
		if ( obj1 != NULL && can_see_obj( mob, obj1 ) )
		{
            	    one_argument( obj1->name, fname );
                    i = fname;
		} 						break;
            case 'O':
                i = (obj1 != NULL && can_see_obj( mob, obj1 ))
                ? obj1->short_descr
                : something;					break;
            case 'p':
		i = something;
		if ( obj2 != NULL && can_see_obj( mob, obj2 ) )
		{
            	    one_argument( obj2->name, fname );
            	    i = fname;
		} 						break;
            case 'P':
            	i = (obj2 != NULL && can_see_obj( mob, obj2 ))
                ? obj2->short_descr
                : something;					break;
        }

        ++str;
        while ( ( *point = *i ) != '\0' )
            ++point, ++i;

    }
    *point = '\0';

    return;
}

void expand_arg_other( char *buf,
	const char *format,
	OBJ_DATA *obj, ROOM_INDEX_DATA *room, CHAR_DATA *ch,
	const void *arg1, const void *arg2, CHAR_DATA *rch )
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
    const char *someone = "someone";
    const char *something = "something";
    const char *someones = "someone's";

    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *vch = (CHAR_DATA *) arg2;
    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    if ( obj && room )
    {
	bug( "expand_arg_other received a obj and a room",0);
	return;
    }

/*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return;

    point   = buf;
    str     = format;
    while ( *str != '\0' )
    {
    	if ( *str != '$' )
        {
            *point++ = *str++;
            continue;
        }
        ++str;

        switch ( *str )
        {
            default:  bug( "Expand_arg: bad code %d.", *str );
                          i = " <@@@> ";                        break;
            case 'i':
		if ( obj )
		{
		    one_argument( obj->name, fname );
		    i = fname;
		}
		else
		{
		    bug( "Expand_arg_other: room had an \"i\" case.",0);
		    i = " <@@@> ";
		}						break;
            case 'I':
		if ( obj )
		    i = obj->short_descr;
		else
		{
		    bug( "Expand_arg_other: room had an \"I\" case.",0);
		    i = " <@@@> ";
		}						break;
            case 'n':
		i = someone;
		if ( ch != NULL )
		{
            	    one_argument( ch->name, fname );
		    i = capitalize(fname);
		}						break;
            case 'N':
	    	i = (ch != NULL )
		? ( IS_NPC( ch ) ? ch->short_descr : ch->name )
		: someone;                         		break;
            case 't':
		i = someone;
		if ( vch != NULL )
		{
            	     one_argument( vch->name, fname );
		     i = capitalize(fname);
		}						break;
            case 'T':
	    	i = (vch != NULL )
		? ( IS_NPC( vch ) ? vch->short_descr : vch->name )
		: someone;                         		break;
            case 'r':
		if ( rch == NULL && obj )
		    rch = get_random_char( NULL, obj, NULL );
		else if ( rch == NULL && room )
		    rch = get_random_char( NULL, NULL, room );
		i = someone;
		if( rch != NULL )
		{
                    one_argument( rch->name, fname );
		    i = capitalize(fname);
		} 						break;
            case 'R':
		if ( rch == NULL && obj )
		    rch = get_random_char( NULL, obj, NULL );
		else if ( rch == NULL && room )
		    rch = get_random_char( NULL, NULL, room );
		i  = ( rch != NULL )
		? ( IS_NPC( ch ) ? ch->short_descr : ch->name )
		:someone;					break;
	    case 'q':
		i = someone;
		if ( obj && obj->oprog_target != NULL )
	        {
		    one_argument( obj->oprog_target->name, fname );
		    i = capitalize( fname );
		}
		else if ( room && room->rprog_target != NULL )
		{
		    one_argument( room->rprog_target->name, fname );
		    i = capitalize( fname );
		} 						break;
	    case 'Q':
	    	i = (obj && obj->oprog_target != NULL)
		? ( IS_NPC( obj->oprog_target ) ?
		obj->oprog_target->short_descr : obj->oprog_target->name )
		: (room && room->rprog_target != NULL)
		? ( IS_NPC( room->rprog_target ) ?
		room->rprog_target->short_descr : room->rprog_target->name )
		: someone;					break;
            case 'j':
		bug( "Expand_arg_other: Obj/room received case 'j'",0);
		i = " <@@@> ";     break;
            case 'e':
	    	i = (ch != NULL )
		? he_she  [URANGE(0, ch->sex, 2)]
		: someone;					break;
            case 'E':
	    	i = (vch != NULL )
		? he_she  [URANGE(0, vch->sex, 2)]
		: someone;					break;
            case 'J':
		i = (rch != NULL )
		? he_she  [URANGE(0, rch->sex, 2)]
		: someone;					break;
	    case 'X':
		i = (obj && obj->oprog_target != NULL )
		? he_she  [URANGE(0, obj->oprog_target->sex, 2)]
		: (room && room->rprog_target != NULL )
		? he_she  [URANGE(0, room->rprog_target->sex, 2)]
		: someone;					break;
            case 'k':
		bug( "Expand_arg_other: received case 'k'.",0);
		i = " <@@@> ";					break;
            case 'm':
	    	i = (ch != NULL )
		? him_her [URANGE(0, ch->sex, 2)]
		: someone;        				break;
            case 'M':
	    	i = (vch != NULL )
		? him_her [URANGE(0, vch->sex, 2)]
		: someone;					break;
            case 'K':
		if ( obj && rch == NULL )
		    rch = get_random_char( NULL, obj, NULL );
		else if ( room && rch == NULL )
		    rch = get_random_char( NULL, NULL, room );
		i = (rch != NULL)
		? him_her [URANGE(0, rch ->sex, 2)]
		: someone;					break;
            case 'Y':
	    	i = (obj && obj->oprog_target != NULL)
		? him_her [URANGE(0, obj->oprog_target->sex, 2)]
		: (room && room->rprog_target != NULL)
		? him_her [URANGE(0, room->rprog_target->sex, 2)]
		: someone;					break;
            case 'l':
		bug( "Expand_arg_other: received case 'l'.",0);
		i = " <@@@> ";					break;
            case 's':
	    	i = (ch != NULL )
		? his_her [URANGE(0, ch ->sex, 2)]
		: someones;					break;
            case 'S':
	    	i = (vch != NULL )
		? his_her [URANGE(0, vch ->sex, 2)]
		: someones;					break;
            case 'L':
		if ( obj && rch == NULL )
		    rch = get_random_char( NULL, obj, NULL );
		else if ( room && rch == NULL )
		    rch = get_random_char( NULL, NULL, room );
		i = ( rch != NULL )
		? his_her [URANGE(0, rch ->sex, 2)]
		: someones;					break;
            case 'Z':
	    	i = (obj && obj->oprog_target != NULL)
		? his_her [URANGE(0, obj->oprog_target->sex, 2)]
		: (room && room->rprog_target != NULL)
		? his_her [URANGE(0, room->rprog_target->sex, 2)]
		: someones;					break;
	    case 'o':
		i = something;
		if ( obj1 != NULL )
		{
            	    one_argument( obj1->name, fname );
                    i = fname;
		} 						break;
            case 'O':
                i = (obj1 != NULL)
                ? obj1->short_descr
                : something;					break;
            case 'p':
		i = something;
		if ( obj2 != NULL )
		{
            	    one_argument( obj2->name, fname );
            	    i = fname;
		} 						break;
            case 'P':
            	i = (obj2 != NULL)
                ? obj2->short_descr
                : something;					break;
        }

        ++str;
        while ( ( *point = *i ) != '\0' )
            ++point, ++i;

    }
    *point = '\0';

    return;
}

/*
 * ------------------------------------------------------------------------
 *  PROGRAM_FLOW
 *  This is the program driver. It parses the mob program code lines
 *  and passes "executable" commands to interpret()
 *  Lines beginning with 'mob' are passed to mob_interpret() to handle
 *  special mob commands (in mob_cmds.c)
 *-------------------------------------------------------------------------
 */

#define MAX_NESTED_LEVEL 12 /* Maximum nested if-else-endif's (stack size) */
#define BEGIN_BLOCK       0 /* Flag: Begin of if-else-endif block */
#define IN_BLOCK         -1 /* Flag: Executable statements */
#define END_BLOCK        -2 /* Flag: End of if-else-endif block */
#define MAX_CALL_LEVEL    5 /* Maximum nested calls */

void program_flow(
        sh_int pvnum,  /* For diagnostic purposes */
	char *source,  /* the actual MOBprog code */
	CHAR_DATA *mob,
	OBJ_DATA *obj,
	ROOM_INDEX_DATA *room,
	CHAR_DATA *ch, const void *arg1, const void *arg2,
    int numlines )

{
    CHAR_DATA *rch = NULL;
    char *code, *line;
    char buf[MAX_STRING_LENGTH];
    char control[MAX_INPUT_LENGTH], data[MAX_STRING_LENGTH];
    char bugbuf[MAX_STRING_LENGTH];

    static int call_level; /* Keep track of nested "mpcall"s */

    int level, eval, check;
    int state[MAX_NESTED_LEVEL], /* Block state (BEGIN,IN,END) */
    cond[MAX_NESTED_LEVEL];  /* Boolean value based on the last if-check */
    int count = 0;
    PROG_CODE *prog = get_mprog_by_vnum(pvnum);

    sh_int mvnum = 0,ovnum = 0,rvnum = 0;

    if ( (mob && obj) || (mob && room) || (obj && room) )
    {
	bug( "PROGs: program_flow received multiple prog types.", 0 );
	return;
    }

    if ( mob )
    {
	    mvnum = mob->pIndexData->vnum;
        if ((prog = get_prog_index(pvnum, PRG_MPROG)) == NULL)
            {
                sprintf(bugbuf, "PROGs: program_flow unable to find mob prog index (%d) for mob %d", pvnum, mvnum);
                bug(bugbuf, 0);
                return;
            }
    } else if ( obj ) {
	    ovnum = obj->pIndexData->vnum;
        if ((prog = get_prog_index(pvnum, PRG_OPROG)) == NULL)
            {
                sprintf(bugbuf, "PROGs: program_flow unable to find mob prog index (%d) for mob %d", pvnum, ovnum);
                bug(bugbuf, 0);
                return;
            }
    } else if ( room ) {
	    rvnum = room->vnum;
        if ((prog = get_prog_index(pvnum, PRG_RPROG)) == NULL)
            {
                sprintf(bugbuf, "PROGs: program_flow unable to find mob prog index (%d) for mob %d", pvnum, rvnum);
                bug(bugbuf, 0);
                return;
            }
    } else
    {
	bug( "PROGs: program_flow did not receive a prog type.", 0 );
	return;
    }

    if( ++call_level > MAX_CALL_LEVEL )
    {
	if ( mob )
	    sprintf( bugbuf, "Progs: MAX_CALL_LEVEL exceeded, vnum %d, mprog vnum %d",
	      mvnum, pvnum );
	else if ( obj )
	    sprintf( bugbuf, "Progs: MAX_CALL_LEVEL exceeded, vnum %d oprog vnum %d.",
	      ovnum, pvnum );
	else
	    sprintf( bugbuf, "Progs: MAX_CALL_LEVEL exceeded, vnum %d rprog vnum %d.",
	      rvnum, pvnum );
	bug( bugbuf, 0 );
    call_level--;
    return;
    }

    /*
     * Reset "stack"
     */
    for ( level = 0; level < MAX_NESTED_LEVEL; level++ )
    {
        state[level] = IN_BLOCK;
        cond[level]  = TRUE;
    }
    level = 0;

    code = source;
    /*
     * Parse the Prog code
     */
    while ( *code )
    {
    bool first_arg = TRUE;
    char *b = buf, *c = control, *d = data;
    /*
     * Get a command line. We sneakily get both the control word
     * (if/and/or) and the rest of the line in one pass.
     */
    while( isspace( *code ) && *code ) code++;
    while ( *code )
    {
        if ( *code == '\n' || *code == '\r' )
        break;
        else if ( isspace(*code) )
        {
        if ( first_arg )
            first_arg = FALSE;
        else
            *d++ = *code;
        }
        else
        {
        if ( first_arg )
           *c++ = *code;
        else
           *d++ = *code;
        }
        *b++ = *code++;
    }
    *b = *c = *d = '\0';

    if(++count < numlines)
    {
        if ( !str_cmp( control, "if" ) )
        {
            state[level] = BEGIN_BLOCK;
            state[++level] = END_BLOCK;
        }
        else if ( !str_cmp( control, "endif" ) )
        {
            state[level] = IN_BLOCK;
            state[--level] = END_BLOCK;
        }

        continue;
    }


    if ( buf[0] == '\0' )
        break;
    if ( buf[0] == '*' ) /* Comment */
        continue;

        line = data;
    /*
     * Match control words
     */
    if ( !str_cmp( control, "sleep" ) )
    {
        int timer;
        SLEEP_DATA *sd;

        line = one_argument( line, control );
        if(control[0] == '\0')
            timer = 4;
        else if(is_number(control) && atoi(control) > 0)
            timer = atoi(control);
        else
        {
            sprintf( buf, "Mobprog: argument NaN, using 4 as default. mob %d prog %d",mvnum, pvnum );
            bug( buf, 0 );
            timer = 4;
        }

        sd = new_sleep_data();
        sd->ch = ch;
        sd->mob = mob;
        sd->obj = obj;
        sd->room = room;
        sd->line = count + 1;
        sd->vnum = prog->vnum;
        sd->timer = timer;
        sd->prog = prog;

        sd->next = first_sleep;
        if(first_sleep)
            first_sleep->prev = sd;
        first_sleep = sd;
        call_level--;
        return;
    }

    if ( !str_cmp( control, "if" ) )
    {
        if ( state[level] == BEGIN_BLOCK )
        {
		if ( mob )
        sprintf( buf, "Mobprog: misplaced if statement, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: misplaced if statement, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: misplaced if statement, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        state[level] = BEGIN_BLOCK;
            if ( ++level >= MAX_NESTED_LEVEL )
            {
		if ( mob )
        sprintf( buf, "Mobprog: Max nested level exceeded, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: Max nested level exceeded, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: Max nested level exceeded, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        if ( level && cond[level-1] == FALSE )
        {
        cond[level] = FALSE;
        continue;
        }
        line = one_argument( line, control );
	    if ( mob && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
        {
		cond[level] = cmd_eval_mob( pvnum, line, check, mob, ch, arg1, arg2, rch );
        }
	    else if ( obj && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		cond[level] = cmd_eval_obj( pvnum, line, check, obj, ch, arg1, arg2, rch );
	    }
	    else if ( room && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		cond[level] = cmd_eval_room( pvnum, line, check, room, ch, arg1, arg2, rch );
	    }
        else
        {
		if ( mob )
        sprintf( buf, "Mobprog: invalid if_check (if), mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: invalid if_check (if), obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: invalid if_check (if), room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        state[level] = END_BLOCK;
        }
    else if ( !str_cmp( control, "or" ) )
    {
        if ( !level || state[level-1] != BEGIN_BLOCK )
        {
		if ( mob )
        sprintf( buf, "Mobprog: or without if, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: or without if, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: or without if, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        if ( level && cond[level-1] == FALSE ) continue;
        line = one_argument( line, control );
	    if ( mob && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
        {
		eval = cmd_eval_mob( pvnum, line, check, mob, ch, arg1, arg2, rch );
        }
	    else if ( obj && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		eval = cmd_eval_obj( pvnum, line, check, obj, ch, arg1, arg2, rch );
	    }
	    else if ( room && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		eval = cmd_eval_room( pvnum, line, check, room, ch, arg1, arg2, rch );
	    }
        else
            {
		if ( mob )
        sprintf( buf, "Mobprog: invalid if_check (or), mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: invalid if_check (or), obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: invalid if_check (or), room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
            }
            cond[level] = (eval == TRUE) ? TRUE : cond[level];
        }
    else if ( !str_cmp( control, "and" ) )
    {
        if ( !level || state[level-1] != BEGIN_BLOCK )
        {
		if ( mob )
        sprintf( buf, "Mobprog: and without if, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: and without if, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: and without if, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        if ( level && cond[level-1] == FALSE ) continue;
        line = one_argument( line, control );
	    if ( mob && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
        {
		eval = cmd_eval_mob( pvnum, line, check, mob, ch, arg1, arg2, rch );
        }
	    else if ( obj && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		eval = cmd_eval_obj( pvnum, line, check, obj, ch, arg1, arg2, rch );
	    }
	    else if ( room && ( check = keyword_lookup( fn_keyword, control ) ) >= 0 )
	    {
		eval = cmd_eval_room( pvnum, line, check, room, ch, arg1, arg2, rch );
	    }
        else
        {
		if ( mob )
        sprintf( buf, "Mobprog: invalid if_check (and), mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: invalid if_check (and), obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: invalid if_check (and), room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        cond[level] = (cond[level] == TRUE) && (eval == TRUE) ? TRUE : FALSE;
        }
    else if ( !str_cmp( control, "endif" ) )
    {
        if ( !level || state[level-1] != BEGIN_BLOCK )
        {
		if ( mob )
        sprintf( buf, "Mobprog: endif without if, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: endif without if, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: endif without if, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        cond[level] = TRUE;
        state[level] = IN_BLOCK;
            state[--level] = END_BLOCK;
        }
    else if ( !str_cmp( control, "else" ) )
    {
        if ( !level || state[level-1] != BEGIN_BLOCK )
        {
		if ( mob )
        sprintf( buf, "Mobprog: else without if, mob %d prog %d",
            mvnum, pvnum );
		else if ( obj )
		    sprintf( buf, "Objprog: else without if, obj %d prog %d",
			ovnum, pvnum );
		else
		    sprintf( buf, "Roomprog: else without if, room %d prog %d",
			rvnum, pvnum );
        bug( buf, 0 );
        call_level--;
        return;
        }
        if ( level && cond[level-1] == FALSE ) continue;
            state[level] = IN_BLOCK;
            cond[level] = (cond[level] == TRUE) ? FALSE : TRUE;
        }
        else if ( cond[level] == TRUE
    && ( !str_cmp( control, "break" ) || !str_cmp( control, "end" ) ) )
    {
        call_level--;
            return;
    }
    else if ( (!level || cond[level] == TRUE) && buf[0] != '\0' )
    {
        state[level] = IN_BLOCK;

	    if ( mob )
		expand_arg_mob( data, buf, mob, ch, arg1, arg2, rch );
	    else if ( obj )
		expand_arg_other( data, buf, obj, NULL, ch, arg1, arg2, rch );
	    else
		expand_arg_other( data, buf, NULL, room, ch, arg1, arg2, rch );

        if ( !str_cmp( control, "mob" ) )
        {
        /*
         * Found a mob restricted command, pass it to mob interpreter
         */
        line = one_argument( data, control );
		if ( !mob )
		    bug( "mob command in non MOBprog",0);
		else
        mob_interpret( mob, line );
        }
	    else if ( !str_cmp( control, "obj" ) )
	    {
		/*
		 * Found an obj restricted command, pass it to obj interpreter
		 */
		line = one_argument( data, control );
		if ( !obj )
		    bug( "obj command in non OBJprog",0);
        else
		    obj_interpret( obj, line );
	    }
	    else if ( !str_cmp( control, "room" ) )
        {
        /*
		 * Found a room restricted command, pass it to room interpreter
		 */
		line = one_argument( data, control );
		if ( !room )
		    bug( "room command in non ROOMprog", 0 );
		else
		    room_interpret( room, line );
	    }
	    else
	    {
		/*
         * Found a normal mud command, pass it to interpreter
         */
		if ( !mob )
		    bug( "Normal MUD command in non-MOBprog, prog vnum %d", pvnum );
		else
        interpret( mob, data );
        }
    }
    }
    call_level--;
}

/*
 * ---------------------------------------------------------------------
 * Trigger handlers. These are called from various parts of the code
 * when an event is triggered.
 * ---------------------------------------------------------------------
 */

/*
 * A general purpose string trigger. Matches argument to a string trigger
 * phrase.
 */
void p_act_trigger(
	char *argument, CHAR_DATA *mob, OBJ_DATA *obj,
	ROOM_INDEX_DATA *room, CHAR_DATA *ch,
    const void *arg1, const void *arg2, int type )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;

    if ( (mob && obj) || (mob && room) || (obj && room) )
    {
	bug( "Multiple program types in ACT trigger.", 0 );
	return;
    }

    if ( mob )
        {
    	for ( prg = mob->pIndexData->mprogs; prg != NULL; prg = prg->next )
    	{
    	    if ( prg->trig_type == type
    	      &&  strstr( argument, prg->trig_phrase ) != NULL )
    	    {
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->ch == ch && test->vnum == prg->vnum)
                        return;

    		    program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, arg1, arg2, 1 );
    		          break;
    	    }
    	}
        }
        else if ( obj )
        {
    	for ( prg = obj->pIndexData->oprogs; prg != NULL; prg = prg->next )
    	{
    	    if ( prg->trig_type == type
    	      && strstr( argument, prg->trig_phrase ) != NULL )
    	    {
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->obj == obj && test->vnum == prg->vnum)
                        return;
    		program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, arg1, arg2, 1 );
    		break;
    	    }
    	}
        }
        else if ( room )
        {
    	for ( prg = room->rprogs; prg != NULL; prg = prg->next )
    	{
    	    if ( prg->trig_type == type
    	      && strstr( argument, prg->trig_phrase ) != NULL )
    	    {
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->room == room && test->vnum == prg->vnum)
                        return;
    		program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, arg1, arg2, 1 );
    		break;
    	    }
    	}
        }
        else
    	bug( "ACT trigger with no program type.", 0 );

        return;
    }

/*
 * A general purpose percentage trigger. Checks if a random percentage
 * number is less than trigger phrase
 */
bool p_percent_trigger( CHAR_DATA *mob, OBJ_DATA *obj, ROOM_INDEX_DATA *room,
	CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;

    if ( (mob && obj) || (mob && room) || (obj && room) )
    {
	bug( "Multiple program types in PERCENT trigger.", 0 );
	return ( FALSE );
    }

    if ( mob )
    {
        for ( prg = mob->pIndexData->mprogs; prg != NULL; prg = prg->next )
        {
            if ( prg->trig_type == type
        &&   number_percent() < atoi( prg->trig_phrase ) )
            {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->ch == ch && test->vnum == prg->vnum)
                    return ( TRUE );

            program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, arg1, arg2, 1 );
            return ( TRUE );
            }
        }
    }else if ( obj )
    {
	for ( prg = obj->pIndexData->oprogs; prg != NULL; prg = prg->next )
	{
	    if ( prg->trig_type == type
	      && number_percent() < atoi( prg->trig_phrase ) )
	    {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->obj == obj && test->vnum == prg->vnum)
                    return ( TRUE );

		program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, arg1, arg2, 1 );
		return ( TRUE );
	    }
	}
    }
    else if ( room )
    {
	for ( prg = room->rprogs; prg != NULL; prg = prg->next )
	{
	    if ( prg->trig_type == type
	      && number_percent() < atoi( prg->trig_phrase ) )
	    {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->room == room && test->vnum == prg->vnum)
                    return ( TRUE );

		program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, arg1, arg2, 1 );
		return ( TRUE );
	    }
	}
    }
    else
	bug( "PERCENT trigger missing program type.", 0 );

    return ( FALSE );
}

void p_bribe_trigger( CHAR_DATA *mob, CHAR_DATA *ch, int amount )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;

    /*
     * Original MERC 2.2 MOBprograms used to create a money object
     * and give it to the mobile. WFT was that? Funcs in act_obj()
     * handle it just fine.
     */
    for ( prg = mob->pIndexData->mprogs; prg; prg = prg->next )
    {
    if ( prg->trig_type == TRIG_BRIBE
    &&   amount >= atoi( prg->trig_phrase ) )
    {
        for(test = first_sleep; test != NULL; test = test->next)
            if(test->ch == ch && test->vnum == prg->vnum)
                return;

        program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, NULL, NULL, 1 );
        break;
    }
    }
    return;
}

bool p_exit_trigger( CHAR_DATA *ch, int dir, int type )
{
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *room;
    PROG_LIST   *prg;
    SLEEP_DATA *test;

    if ( type == PRG_MPROG )
    {
    for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
    {
    if ( IS_NPC( mob )
    &&   ( HAS_TRIGGER_MOB(mob, TRIG_EXIT) || HAS_TRIGGER_MOB(mob, TRIG_EXALL) ) )
    {
        for ( prg = mob->pIndexData->mprogs; prg; prg = prg->next )
        {
        /*
         * Exit trigger works only if the mobile is not busy
         * (fighting etc.). If you want to be sure all players
         * are caught, use ExAll trigger
         */
        if ( prg->trig_type == TRIG_EXIT
        &&  dir == atoi( prg->trig_phrase )
        &&  mob->position == mob->pIndexData->default_pos
        &&  can_see( mob, ch ) )
        {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->ch == ch && test->vnum == prg->vnum)
                    return ( TRUE );

            program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, NULL, NULL, 1 );
            return TRUE;
        }
        else
        if ( prg->trig_type == TRIG_EXALL
        &&   dir == atoi( prg->trig_phrase ) )
        {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->ch == ch && test->vnum == prg->vnum)
                    return ( TRUE );

            program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, NULL, NULL, 1 );
            return TRUE;
        }
        }
    }
    }
    }
    else if ( type == PRG_OPROG )
    {
	for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
	    if ( HAS_TRIGGER_OBJ( obj, TRIG_EXALL ) )
	    {
		for ( prg = obj->pIndexData->oprogs; prg; prg = prg->next )
		{
		    if ( prg->trig_type == TRIG_EXALL
		      && dir == atoi( prg->trig_phrase ) )
		    {
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->obj == obj && test->vnum == prg->vnum)
                        return ( TRUE );
			program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, NULL, NULL, 1 );
			return TRUE;
		    }
		}
	    }
	}

	for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
	{
	    for ( obj = mob->carrying; obj; obj = obj->next_content )
	    {
		if ( HAS_TRIGGER_OBJ( obj, TRIG_EXALL ) )
		{
		    for ( prg = obj->pIndexData->oprogs; prg; prg = prg->next )
		    {
			if ( prg->trig_type == TRIG_EXALL
			  && dir == atoi( prg->trig_phrase ) )
			{
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->obj == obj && test->vnum == prg->vnum)
                        return ( TRUE );

			    program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, NULL, NULL, 1 );
			    return TRUE;
			}
		    }
		}
	    }
	}
    }
    else if ( type == PRG_RPROG )
    {
	room = ch->in_room;

	if ( HAS_TRIGGER_ROOM( room, TRIG_EXALL ) )
	{
	    for ( prg = room->rprogs; prg; prg = prg->next )
	    {
		if ( prg->trig_type == TRIG_EXALL
		    && dir == atoi( prg->trig_phrase ) )
		{
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->room == room && test->vnum == prg->vnum)
                    return ( TRUE );
		    program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, NULL, NULL, 1 );
		    return TRUE;
		}
	    }
	}
    }

    return FALSE;
}

void p_give_trigger( CHAR_DATA *mob, OBJ_DATA *obj, ROOM_INDEX_DATA *room,
			CHAR_DATA *ch, OBJ_DATA *dropped, int type )
{

    char        buf[MAX_INPUT_LENGTH], *p;
    PROG_LIST  *prg;
    SLEEP_DATA *test;

    if ( (mob && obj) || (mob && room) || (obj && room) )
    {
	bug( "Multiple program types in GIVE trigger.", 0 );
	return;
    }

    if ( mob )
    {
    for ( prg = mob->pIndexData->mprogs; prg; prg = prg->next )
    if ( prg->trig_type == TRIG_GIVE )
    {
        p = prg->trig_phrase;
        /*
         * Vnum argument
         */
        if ( is_number( p ) )
        {
        if ( dropped->pIndexData->vnum == atoi(p) )
        {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->ch == ch && test->vnum == prg->vnum)
                    return;

            program_flow(prg->vnum, prg->code, mob, NULL, NULL, ch, (void *) dropped, NULL, 1);
            return;
        }
        }
        /*
	 	 * Dropped object name argument, e.g. 'sword'
         */
        else
        {
            while( *p )
            {
            p = one_argument( p, buf );

			if ( is_name( buf, dropped->name )
            ||   !str_cmp( "all", buf ) )
            {
                for(test = first_sleep; test != NULL; test = test->next)
                    if(test->ch == ch && test->vnum == prg->vnum)
                        return;

                        program_flow(prg->vnum, prg->code, mob, NULL, NULL, ch, (void *) dropped, NULL, 1);
                return;
            }
        }
        }
    }
}
else if ( obj )
{
for ( prg = obj->pIndexData->oprogs; prg; prg = prg->next )
    if ( prg->trig_type == type )
    {
        for(test = first_sleep; test != NULL; test = test->next)
            if(test->obj == obj && test->vnum == prg->vnum)
                return ( TRUE );
    program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, (void *) obj, NULL, 1);
    return;
    }
}
else if ( room )
{
for ( prg = room->rprogs; prg; prg = prg->next )
    if ( prg->trig_type == type )
    {
    p = prg->trig_phrase;
    /*
     * Vnum argument
     */
    if ( is_number( p ) )
    {
        if ( dropped->pIndexData->vnum == atoi(p) )
        {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->room == room && test->vnum == prg->vnum)
                    return ( TRUE );
        program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, (void *) dropped, NULL, 1);
        return;
        }
    }
    /*
     * Dropped object name argument, e.g. 'sword'
     */
    else
    {
        while( *p )
        {
        p = one_argument( p, buf );

        if ( is_name( buf, dropped->name )
          || !str_cmp( "all", buf ) )
        {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->room == room && test->vnum == prg->vnum)
                    return ( TRUE );
            program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, (void *) dropped, NULL, 1);
            return;
        }
        }
    }
    }
}
}

void p_greet_trigger( CHAR_DATA *ch, int type )
{
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *room;
    DOMAIN_DATA *domain;

    if ( type == PRG_MPROG )
    {
    for ( mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room )
    {
    if ( IS_NPC( mob )
    && ( HAS_TRIGGER_MOB(mob, TRIG_GREET) || HAS_TRIGGER_MOB(mob,TRIG_GRALL) || IS_SET(mob->act2, ACT2_INFLUENCE)) )
    {
        /*
         * Domain Influence Mob aggressive added here.
         * Mob will attack players if mob's domain is at war with ch's clan.
         */
         if (IS_SET(mob->act2, ACT2_INFLUENCE) && IS_SET(ch->act2, PLR2_PVP) &&
         (domain = mob->pIndexData->area->domain) != NULL &&
         domain->relation[ch->clan] == RELATION_WARRING)
         {
             act("%n lets loose a wild warcry and attacks $N!", mob, NULL, ch, TO_NOTVICT);
             act("$n lets loose a wild warcry and attacks you!", mob, NULL, ch, TO_VICT);
             multi_hit( mob, ch, TYPE_UNDEFINED );
             return;
         }
        /*
         * Greet trigger works only if the mobile is not busy
         * (fighting etc.). If you want to catch all players, use
         * GrAll trigger
         */
        if ( HAS_TRIGGER_MOB( mob,TRIG_GREET )
        &&   mob->position == mob->pIndexData->default_pos
        &&   can_see( mob, ch ) )
        p_percent_trigger( mob, NULL, NULL, ch, NULL, NULL, TRIG_GREET );
        else
        if ( HAS_TRIGGER_MOB( mob, TRIG_GRALL ) )
        p_percent_trigger( mob, NULL, NULL, ch, NULL, NULL, TRIG_GRALL );
    }
    }
    }
    else if ( type == PRG_OPROG )
    {
	for ( obj = ch->in_room->contents; obj != NULL; obj = obj->next_content )
	{
	    if ( HAS_TRIGGER_OBJ( obj, TRIG_GRALL ) )
	    {
		p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_GRALL );
        return;
        }
    }

    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        for ( obj = mob->carrying; obj; obj = obj->next_content )
        {
            if ( HAS_TRIGGER_OBJ( obj, TRIG_GRALL ) )
            {
                p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_GRALL );
                return;
            }
        }
    }
}
        else if ( type == PRG_RPROG )
        {
        room = ch->in_room;

        if ( HAS_TRIGGER_ROOM( room, TRIG_GRALL ) )
            p_percent_trigger( NULL, NULL, room, ch, NULL, NULL, TRIG_GRALL );
        }

return;
}

void p_hprct_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;

    for ( prg = mob->pIndexData->mprogs; prg != NULL; prg = prg->next )
    if ( ( prg->trig_type == TRIG_HPCNT )
    && ( (100 * mob->hit / mob->max_hit) < atoi( prg->trig_phrase ) ) )
    {
        for(test = first_sleep; test != NULL; test = test->next)
            if(test->ch == ch && test->vnum == prg->vnum)
                return;

        program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, NULL, NULL, 1 );
        break;
    }
}

void p_talk_trigger( CHAR_DATA *mob, CHAR_DATA *ch )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;

    for ( prg = mob->pIndexData->mprogs; prg != NULL; prg = prg->next )
    if ( prg->trig_type == TRIG_TALK )
    {
        for(test = first_sleep; test != NULL; test = test->next)
            if(test->ch == ch && test->vnum == prg->vnum)
                return;

        program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, NULL, NULL, 1 );
        break;
    }
}

PROG_CODE *get_mprog_by_vnum(int vnum)
{
    PROG_CODE *temp;
    for(temp = mprog_list; temp != NULL; temp = temp->next)
        if(temp->vnum == vnum)
            return temp;
    return NULL;
}

/*
 * Checks whether the number provided as trig_phrase refers to the current
 * time. Could be systime or gametime.
 */
bool p_time_trigger( CHAR_DATA *mob, OBJ_DATA *obj, ROOM_INDEX_DATA *room,
	CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    PROG_LIST *prg;
    SLEEP_DATA *test;
    int time;

    if ( (mob && obj) || (mob && room) || (obj && room) )
    {
	bug( "Multiple program types in TIME trigger.", 0 );
	return ( FALSE );
    }

    if (type == TRIG_SYSTIME)
    {
        struct tm * timeinfo;
        timeinfo = localtime (&current_time);
        time = timeinfo->tm_hour * 100;
        time += timeinfo->tm_min;

        if (time < 0 || time > 2359)
        {
            char buf[MSL];
            sprintf(buf, "TIME trigger computed time as (%d) from localtime(&current_time).", time);
            bug(buf, 0);
            return FALSE;
        }
    } else if (type == TRIG_MUDTIME)
    {
        time = time_info.hour;

        if (time < 0 || time > 23)
        {
            char buf[MSL];
            sprintf(buf, "TIME trigger computed time as (%d) from time_info.hour.", time);
            bug(buf, 0);
            return FALSE;
        }
    } else
    {
        char buf[MSL];
        sprintf(buf, "TIME trigger received bad type %s (%d).", prog_type_to_name(type), type);
        bug(buf, 0);
        return FALSE;
    }

    if ( mob )
    {
        for ( prg = mob->pIndexData->mprogs; prg != NULL; prg = prg->next )
        {
            if ( prg->trig_type == type && time == atoi( prg->trig_phrase ) )
            {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->ch == ch && test->vnum == prg->vnum)
                    return ( TRUE );

            program_flow( prg->vnum, prg->code, mob, NULL, NULL, ch, arg1, arg2, 1 );
            return ( TRUE );
            }
        }
    }else if ( obj )
    {
	for ( prg = obj->pIndexData->oprogs; prg != NULL; prg = prg->next )
	{
        if ( prg->trig_type == type && time == atoi( prg->trig_phrase ) )
	    {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->obj == obj && test->vnum == prg->vnum)
                    return ( TRUE );

		program_flow( prg->vnum, prg->code, NULL, obj, NULL, ch, arg1, arg2, 1 );
		return ( TRUE );
	    }
	}
    }
    else if ( room )
    {
	for ( prg = room->rprogs; prg != NULL; prg = prg->next )
	{
        if ( prg->trig_type == type && time == atoi( prg->trig_phrase ) )
	    {
            for(test = first_sleep; test != NULL; test = test->next)
                if(test->room == room && test->vnum == prg->vnum)
                    return ( TRUE );

		program_flow( prg->vnum, prg->code, NULL, NULL, room, ch, arg1, arg2, 1 );
		return ( TRUE );
	    }
	}
    }
    else
	bug( "TIME trigger missing program type.", 0 );

    return ( FALSE );
}
