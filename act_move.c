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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
char *  const   dir_name    []      =
{
    "north", "east", "south", "west", "up", "down"
};

const   sh_int  rev_dir     []      =
{
    2, 3, 0, 1, 5, 4
};

/*
#define SECT_INSIDE             0
#define SECT_CITY               1
#define SECT_FIELD              2
#define SECT_FOREST             3
#define SECT_HILLS              4
#define SECT_MOUNTAIN           5
#define SECT_WATER_SWIM         6
#define SECT_WATER_NOSWIM       7
#define SECT_UNUSED             8
#define SECT_AIR                9
#define SECT_DESERT             10
#define SECT_WATER_DROWN        11
#define SECT_HOT                12
#define SECT_COLD               13
#define SECT_NODE               14
#define SECT_MAX                15*/

const   sh_int  movement_loss   [SECT_MAX]  =
{ //I  C  F  F  H  M  S  N  U  A  D  D  H  C  N
    2, 2, 2, 2, 2, 3, 2, 2, 2, 4, 3, 4, 2, 2, 1
};



/*
 * Local functions.
 */
int find_door   args( ( CHAR_DATA *ch, char *arg ) );
bool    has_key     args( ( CHAR_DATA *ch, int key ) );



void move_char( CHAR_DATA *ch, int door, bool follow )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    OBJ_DATA *reflective;
    char buf[MAX_STRING_LENGTH];
    int stealth = 0, perception = 0;

    if ( door < 0 || door > 5 )
    {
    bug( "Do_move: bad door %d.", door );
    return;
    }
    if ( MOUNTED(ch) )
    {
    if (MOUNTED(ch)->position < POS_FIGHTING)
    {
        send_to_char("Your mount must be standing.\n\r", ch);
            return;
      }
    if (!mount_success(ch, MOUNTED(ch), FALSE))
    {
            send_to_char("Your mount subbornly refuses to go that way.\n\r", ch);
            return;
      }
    }

    /*
     * Exit trigger, if activated, bail out. Only PCs are triggered.
     */
     if ( !IS_NPC(ch)
       && (p_exit_trigger( ch, door, PRG_MPROG )
       ||  p_exit_trigger( ch, door, PRG_OPROG )
       ||  p_exit_trigger( ch, door, PRG_RPROG )) )
 	return;

    in_room = ch->in_room;
    if ( ( pexit   = in_room->exit[door] ) == NULL
    ||   ( to_room = pexit->u1.to_room   ) == NULL
    ||   !can_see_room(ch,pexit->u1.to_room))
    {
    send_to_char( "Alas, you cannot go that way.\n\r", ch );
    return;
    }
    /*if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );*/
    if(IS_AFFECTED2(ch, AFF2_VEIL) && (IS_NPC(ch) ||
            (!IS_NPC(ch) && ch->pcdata->discipline[OBFUSCATE] != 6)))
    {
    REMOVE_BIT(ch->affected2_by, AFF2_VEIL);
    affect_strip(ch, gsn_veil);
    }
    if (IS_SET(pexit->exit_info, EX_CLOSED)
    &&  (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->exit_info,EX_NOPASS))
    &&   !IS_IMMORTAL(ch)
    && !IS_SET(pexit->exit_info, EX_HIDDEN)
    && !IS_SET(pexit->exit_info, EX_LIFT))
    {
    act( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
    return;
    }

    if (IS_SET(pexit->exit_info, EX_HIDDEN)
    && IS_SET(pexit->exit_info, EX_CLOSED)
    && !IS_IMMORTAL(ch))
    {
    send_to_char( "Alas, you cannot go that way.\n\r", ch );
    return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM)
    &&   ch->master != NULL
    &&   in_room == ch->master->in_room )
    {
    send_to_char( "What?  And leave your beloved master?\n\r", ch );
    return;
    }

    if ( !is_room_owner(ch,to_room) && room_is_private( to_room ) )
    {
    send_to_char( "That room is private right now.\n\r", ch );
    return;
    }

    if ( !IS_NPC(ch) )
    {
    int move;


    if ( in_room->sector_type == SECT_AIR
    ||   to_room->sector_type == SECT_AIR )
    {
        if ( !IS_AFFECTED(ch, AFF_FLYING) && !IS_IMMORTAL(ch))
        {
        if ( MOUNTED(ch) )
          {
            if( !IS_AFFECTED(MOUNTED(ch), AFF_FLYING) )
            {
                send_to_char( "Your mount can't fly.\n\r", ch );
                return;
            }
            }
        else
        {
            send_to_char( "You can't fly.\n\r", ch );
            return;
        }
        }
    }

    if ( (in_room->sector_type == SECT_WATER_NOSWIM
        ||    to_room->sector_type == SECT_WATER_NOSWIM )
        &&    MOUNTED(ch))
        {
            sprintf( buf,"You can't take your mount there.\n\r");
            send_to_char(buf, ch);
            return;
        }

    if (( in_room->sector_type == SECT_WATER_NOSWIM
    ||    to_room->sector_type == SECT_WATER_NOSWIM)
    &&    !IS_AFFECTED(ch,AFF_FLYING))
    {
        OBJ_DATA *obj;
        bool found;

        /*
         * Look for a boat.
         */
        found = FALSE;

        if (IS_IMMORTAL(ch))
        found = TRUE;

        for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        {
        if ( obj->item_type == ITEM_BOAT )
        {
            found = TRUE;
            break;
        }
        }
        if ( !found )
        {
        send_to_char( "You need a boat to go there.\n\r", ch );
        return;
        }
    }
    if((in_room->sector_type == SECT_WATER_DROWN
    || to_room->sector_type == SECT_WATER_DROWN)
    && (ch->race != race_lookup("vampire") || ch->race != race_lookup("methuselah")))
        send_to_char("You struggle to hold your breath as you dive into the deep water.\n\r",ch);

    move = movement_loss[UMIN(SECT_MAX-1, in_room->sector_type)]
         + movement_loss[UMIN(SECT_MAX-1, to_room->sector_type)]
         ;

        move /= 2;  /* i.e. the average */


    if ( !MOUNTED(ch) )
    {
        /* conditional effects */
        if (IS_AFFECTED(ch,AFF_FLYING) || IS_AFFECTED(ch,AFF_HASTE))
        move /= 2;

        if (is_affected(ch, gsn_gift_snowrunning))
            move /= 2;

        if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        move *= 2;

        if ( ch->move < move )
        {
        send_to_char( "You are too exhausted to move another step.\n\r", ch );
        return;
        }
    }
    else
    {
            if (IS_AFFECTED( MOUNTED(ch), AFF_FLYING)
          || IS_AFFECTED( MOUNTED(ch), AFF_HASTE))
                move /= 2;

    if (IS_AFFECTED(ch,AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
        move *= 2;

            if (IS_AFFECTED( MOUNTED(ch), AFF_SLOW) || ch->in_room->sector_type == SECT_COLD)
                move *= 2;

        if ( MOUNTED(ch)->move < move )
        {
        send_to_char( "Your mount is too exhausted.\n\r", ch );
        return;
            }
        }

    WAIT_STATE( ch, 1 );

    if (!MOUNTED(ch))
            ch->move -= move;
        else
            MOUNTED(ch)->move -= move;
    }

    if (RIDDEN(ch))
    {
    CHAR_DATA *rch;
    rch = RIDDEN(ch);

    if (!mount_success(rch, ch, FALSE))
    {
        act( "Your mount escapes your control, and leaves $T.", rch, NULL, dir_name[door], TO_CHAR );
        if (RIDDEN(ch))
        ch = RIDDEN(ch);
    }
    else
    {
        send_to_char("You steady your mount.\n\r", rch);
        return;
    }
    }

    if ( !IS_AFFECTED(ch, AFF_SNEAK) && ch->invis_level < LEVEL_HERO)
    {
        if (MOUNTED(ch))
        {
            if( !IS_AFFECTED(MOUNTED(ch), AFF_FLYING) )
                sprintf(buf, "$n leaves $T, riding on %s.", MOUNTED(ch)->short_descr);
            else
                sprintf(buf, "$n soars $T, on %s.", MOUNTED(ch)->short_descr);
            act( buf, ch, NULL, dir_name[door], TO_ROOM );
        }
        else
        {
            act( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM );
        }
    }

    ch->agg = PULSE_PER_SECOND;
    char_from_room( ch );
    char_to_room( ch, to_room );
    if ( !IS_AFFECTED(ch, AFF_SNEAK)
      &&   ch->invis_level < LEVEL_HERO)
    {
    if(!MOUNTED(ch))
    {
        act( "$n approaches.", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        if ( !IS_AFFECTED(MOUNTED(ch), AFF_FLYING) )
        act( "$n has arrived, riding on $N.", ch, NULL, MOUNTED(ch), TO_ROOM );
        else
        act( "$n soars in, riding on $N.", ch, NULL, MOUNTED(ch), TO_ROOM );
        }
    }

    if (MOUNTED(ch))
    {
        char_from_room( MOUNTED(ch) );
        char_to_room( MOUNTED(ch), to_room );
    }
    do_function(ch, &do_look, "auto" );

    if (HAS_REFLECTION(ch))
    {
    	if(IS_AFFECTED(ch, AFF_SNEAK) || IS_AFFECTED(ch, AFF2_UNSEEN) || IS_AFFECTED(ch,AFF_HIDE))
    		stealth = godice(get_attribute(ch,WITS)+ch->csabilities[CSABIL_STEALTH],6);
        stealth += stealth_int_shadowplay(ch, 6);

    	for (reflective = ch->in_room->contents; reflective != NULL; reflective = reflective->next_content)
    	{
    		if (IS_REFLECTIVE(reflective) && (SAME_UMBRA_OBJ(ch, reflective)))
    		{
    			for (fch = char_list; fch != NULL; fch = fch_next)
    			{
    				fch_next = fch->next;
    				if (fch->in_room == NULL || fch->in_room != ch->in_room
    					|| !SAME_UMBRA(ch, fch) || !can_see(fch, ch)
    					|| !can_see_obj(fch, reflective))
    					continue;

    				perception = godice(get_attribute(fch,PERCEPTION)+fch->csabilities[CSABIL_ALERTNESS], 7);
    				if (perception >= stealth)
    				{
    					if (fch == ch)
    						act("{WYou spot your own image reflecting in $p as you enter the room.{x", fch, reflective, ch, TO_CHAR);
    					else
    						act("{W$N's image reflects briefly in $p as $E enters the room.{x", fch, reflective, ch, TO_CHAR);
    				}
    			}
    		}
    	}
    }

    if (in_room == to_room) /* no circular follows */
    return;

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
    fch_next = fch->next_in_room;

    if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
    &&   fch->position < POS_STANDING)
        do_function(fch, &do_stand, "");

    if ( fch->master == ch && SAME_UMBRA(fch, ch) &&
        fch->position == POS_STANDING && can_see_room(fch,to_room))
    {

        if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
        &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
        {
        act("You can't bring $N into the city.",
            ch,NULL,fch,TO_CHAR);
        act("You aren't allowed in the city.",
            fch,NULL,NULL,TO_CHAR);
        continue;
        }

        act( "You follow $N.", fch, NULL, ch, TO_CHAR );
        move_char( fch, door, TRUE );
    }
    }

    /*
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if ( IS_NPC( ch ) && HAS_TRIGGER_MOB( ch, TRIG_ENTRY ) )
    p_percent_trigger( ch, NULL, NULL, NULL, NULL, NULL, TRIG_ENTRY );
    if ( !IS_NPC( ch ) )
    {
    	p_greet_trigger( ch, PRG_MPROG );
	    p_greet_trigger( ch, PRG_OPROG );
	    p_greet_trigger( ch, PRG_RPROG );
    }

    return;
}



void do_north( CHAR_DATA *ch, char *argument )
{
   if ( IS_AFFECTED2 (ch, AFF2_IGNIS_FATUUS))
   {
       move_char( ch, DIR_SOUTH, FALSE);
   return;
   }
    move_char( ch, DIR_NORTH, FALSE );
    return;
}



void do_east( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_EAST, FALSE );
    return;
}



void do_south( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_SOUTH, FALSE );
    return;
}



void do_west( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_WEST, FALSE );
    return;
}



void do_up( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_UP, FALSE );
    return;
}



void do_down( CHAR_DATA *ch, char *argument )
{
    move_char( ch, DIR_DOWN, FALSE );
    return;
}



int find_door( CHAR_DATA *ch, char *arg )
{
    EXIT_DATA *pexit;
    int door = -1;

    if ( !str_cmp( arg, "n" ) || !str_cmp( arg, "north" ) )
    {
         if((pexit = ch->in_room->exit[0]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 0;
    }
    else if ( !str_cmp( arg, "e" ) || !str_cmp( arg, "east"  ) )
    {
         if((pexit = ch->in_room->exit[1]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 1;
    }
    else if ( !str_cmp( arg, "s" ) || !str_cmp( arg, "south" ) )
    {
         if((pexit = ch->in_room->exit[2]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 2;
    }
    else if ( !str_cmp( arg, "w" ) || !str_cmp( arg, "west"  ) )
    {
         if((pexit = ch->in_room->exit[3]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 3;
    }
    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) )
    {
         if((pexit = ch->in_room->exit[4]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 4;
    }
    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) )
    {
         if((pexit = ch->in_room->exit[5]) != NULL && !str_cmp(pexit->keyword, ""))
            door = 5;
    }
    else
    {
    for ( door = 0; door <= 5; door++ )
    {
        if ( ( pexit = ch->in_room->exit[door] ) != NULL
        &&   IS_SET(pexit->exit_info, EX_ISDOOR)
        &&   pexit->keyword != NULL
        &&   is_name( arg, pexit->keyword ) )
        return door;
    }
    //act( "I see no $T here.", ch, NULL, arg, TO_CHAR );
    return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
    act( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
    return -1;
    }

    if ( !IS_SET(pexit->exit_info, EX_ISDOOR) )
    {
    send_to_char( "You can't do that.\n\r", ch );
    return -1;
    }

    return door;
}



void do_open( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Open what?\n\r", ch );
    return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'open door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
        { send_to_char( "It's already open.\n\r",      ch ); return; }
    if (  IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's locked.\n\r",            ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_LIFT) )
        { send_to_char( "You can't do that.\n\r",      ch ); return; }

    if( IS_SET(pexit->exit_info, EX_HIDDEN) )
    {
        act("You reveal a hidden passage!",ch,NULL,NULL,TO_CHAR);
        act( "$n reveals a hidden passage!", ch, NULL, NULL, TO_ROOM );
    }
    REMOVE_BIT(pexit->exit_info, EX_HIDDEN);
    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
    act( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
    send_to_char( "Ok.\n\r", ch );

    /* open the other side */
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
    &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
    &&   pexit_rev->u1.to_room == ch->in_room )
    {
        CHAR_DATA *rch;

        REMOVE_BIT( pexit_rev->exit_info, EX_HIDDEN );
        REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
        act( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
    }

    return;
    }


    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
    {
    /* open portal */
    if (obj->item_type == ITEM_PORTAL)
    {
        if (!IS_SET(obj->value[1], EX_ISDOOR) || IS_SET(obj->value[1], EX_LIFT))
        {
        send_to_char("You can't do that.\n\r",ch);
        return;
        }

        if (!IS_SET(obj->value[1], EX_CLOSED))
        {
        send_to_char("It's already open.\n\r",ch);
        return;
        }

        if (IS_SET(obj->value[1], EX_LOCKED))
        {
        send_to_char("It's locked.\n\r",ch);
        return;
        }

        REMOVE_BIT(obj->value[1], EX_CLOSED);
        act("You open $p.",ch,obj,NULL,TO_CHAR);
        act("$n opens $p.",ch,obj,NULL,TO_ROOM);
        return;
    }

    /* 'open object' */
    if ( obj->item_type != ITEM_CONTAINER)
        { send_to_char( "That's not a container.\n\r", ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
        { send_to_char( "It's already open.\n\r",      ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
        { send_to_char( "You can't do that.\n\r",      ch ); return; }
    if ( IS_SET(obj->value[1], CONT_LOCKED) )
        { send_to_char( "It's locked.\n\r",            ch ); return; }

    REMOVE_BIT(obj->value[1], CONT_CLOSED);
    act("You open $p.",ch,obj,NULL,TO_CHAR);
    act( "$n opens $p.", ch, obj, NULL, TO_ROOM );
    return;
    }
}

void do_lift( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Lift what?\n\r", ch );
    return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'lift door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
        { send_to_char( "It's already open.\n\r",      ch ); return; }
    if (  IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's locked.\n\r",            ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_LIFT) )
        { send_to_char( "You can't do that.\n\r",      ch ); return; }


    if( IS_SET(pexit->exit_info, EX_HIDDEN) )
    {
        act("You reveal a hidden passage!",ch,NULL,NULL,TO_CHAR);
        act( "$n reveals a hidden passage!", ch, NULL, NULL, TO_ROOM );
    }

    REMOVE_BIT(pexit->exit_info, EX_HIDDEN);
    REMOVE_BIT(pexit->exit_info, EX_CLOSED);
    act( "$n lifts the $d.", ch, NULL, pexit->keyword, TO_ROOM );
    send_to_char( "Ok.\n\r", ch );

    /* open the other side */
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
    &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
    &&   pexit_rev->u1.to_room == ch->in_room )
    {
        CHAR_DATA *rch;

        REMOVE_BIT( pexit_rev->exit_info, EX_HIDDEN);
        REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
        act( "The $d is lifted from the other side.", rch, NULL, pexit_rev->keyword, TO_CHAR );
        return;
    }
    }


        if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
        {
        /* open portal */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET(obj->value[1], EX_ISDOOR) || !IS_SET(obj->value[1], EX_LIFT))
            {
            send_to_char("You can't do that.\n\r",ch);
            return;
            }

            if (!IS_SET(obj->value[1], EX_CLOSED))
            {
            send_to_char("It's already open.\n\r",ch);
            return;
            }

            if (IS_SET(obj->value[1], EX_LOCKED))
            {
            send_to_char("It's locked.\n\r",ch);
            return;
            }

            REMOVE_BIT(obj->value[1], EX_CLOSED);
            act("You lift the $p.",ch,obj,NULL,TO_CHAR);
            act("$n lifts the $p.",ch,obj,NULL,TO_ROOM);
            return;
        }
            send_to_char( "Lift what?.\n\r", ch );
            return;
        }

}

void do_close( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Close what?\n\r", ch );
    return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'close door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit   = ch->in_room->exit[door];
    if ( IS_SET(pexit->exit_info, EX_CLOSED) )
        { send_to_char( "It's already closed.\n\r",    ch ); return; }

    SET_BIT(pexit->exit_info, EX_CLOSED);
    act( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
    send_to_char( "Ok.\n\r", ch );

    /* close the other side */
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
    &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
    &&   pexit_rev->u1.to_room == ch->in_room )
    {
        CHAR_DATA *rch;

        SET_BIT( pexit_rev->exit_info, EX_CLOSED );
        for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
        act( "The $d closes.", rch, NULL, pexit_rev->keyword, TO_CHAR );
        return;
    }
    }

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
    {
    /* portal stuff */
    if (obj->item_type == ITEM_PORTAL)
    {

        if (!IS_SET(obj->value[1],EX_ISDOOR)
        ||   IS_SET(obj->value[1],EX_NOCLOSE))
        {
        send_to_char("You can't do that.\n\r",ch);
        return;
        }

        if (IS_SET(obj->value[1],EX_CLOSED))
        {
        send_to_char("It's already closed.\n\r",ch);
        return;
        }

        SET_BIT(obj->value[1],EX_CLOSED);
        act("You close $p.",ch,obj,NULL,TO_CHAR);
        act("$n closes $p.",ch,obj,NULL,TO_ROOM);
        return;
    }

    /* 'close object' */
    if ( obj->item_type != ITEM_CONTAINER )
        { send_to_char( "That's not a container.\n\r", ch ); return; }
    if ( IS_SET(obj->value[1], CONT_CLOSED) )
        { send_to_char( "It's already closed.\n\r",    ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSEABLE) )
        { send_to_char( "You can't do that.\n\r",      ch ); return; }

    SET_BIT(obj->value[1], CONT_CLOSED);
    act("You close $p.",ch,obj,NULL,TO_CHAR);
    act( "$n closes $p.", ch, obj, NULL, TO_ROOM );
    return;
    }

}



bool has_key( CHAR_DATA *ch, int key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
    if ( obj->pIndexData->vnum == key )
        return TRUE;
    }

    return FALSE;
}



void do_lock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Lock what?\n\r", ch );
    return;
    }

    if (MOUNTED(ch))
    {
        send_to_char("You can't reach the lock from your mount.\n\r", ch);
        return;
    }

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'lock door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit   = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
        { send_to_char( "It's not closed.\n\r",        ch ); return; }
    if ( pexit->key < 0 )
        { send_to_char( "It can't be locked.\n\r",     ch ); return; }
    if ( !has_key( ch, pexit->key) )
        { send_to_char( "You lack the key.\n\r",       ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's already locked.\n\r",    ch ); return; }

    SET_BIT(pexit->exit_info, EX_LOCKED);
    send_to_char( "*Click*\n\r", ch );
    act( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

    /* lock the other side */
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
    &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0
    &&   pexit_rev->u1.to_room == ch->in_room )
    {
        SET_BIT( pexit_rev->exit_info, EX_LOCKED );
        return;
    }
    }

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
    {
    /* portal stuff */
    if (obj->item_type == ITEM_PORTAL)
    {
        if (!IS_SET(obj->value[1],EX_ISDOOR)
        ||  IS_SET(obj->value[1],EX_NOCLOSE))
        {
        send_to_char("You can't do that.\n\r",ch);
        return;
        }
        if (!IS_SET(obj->value[1],EX_CLOSED))
        {
        send_to_char("It's not closed.\n\r",ch);
        return;
        }

        if (obj->value[4] < 0 || IS_SET(obj->value[1],EX_NOLOCK))
        {
        send_to_char("It can't be locked.\n\r",ch);
        return;
        }

        if (!has_key(ch,obj->value[4]))
        {
        send_to_char("You lack the key.\n\r",ch);
        return;
        }

        if (IS_SET(obj->value[1],EX_LOCKED))
        {
        send_to_char("It's already locked.\n\r",ch);
        return;
        }

        SET_BIT(obj->value[1],EX_LOCKED);
        act("You lock $p.",ch,obj,NULL,TO_CHAR);
        act("$n locks $p.",ch,obj,NULL,TO_ROOM);
        return;
    }

    /* 'lock object' */
    if ( obj->item_type != ITEM_CONTAINER )
        { send_to_char( "That's not a container.\n\r", ch ); return; }
    if ( !IS_SET(obj->value[1], CONT_CLOSED) )
        { send_to_char( "It's not closed.\n\r",        ch ); return; }
    if ( obj->value[2] < 0 )
        { send_to_char( "It can't be locked.\n\r",     ch ); return; }
    if ( !has_key( ch, obj->value[2] ) )
        { send_to_char( "You lack the key.\n\r",       ch ); return; }
    if ( IS_SET(obj->value[1], CONT_LOCKED) )
        { send_to_char( "It's already locked.\n\r",    ch ); return; }

    SET_BIT(obj->value[1], CONT_LOCKED);
    act("You lock $p.",ch,obj,NULL,TO_CHAR);
    act( "$n locks $p.", ch, obj, NULL, TO_ROOM );
    return;
    }

}



void do_unlock( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Unlock what?\n\r", ch );
    return;
    }

    if (MOUNTED(ch))
    {
        send_to_char("You can't reach the lock from your mount.\n\r", ch);
        return;
    }


    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'unlock door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) )
        { send_to_char( "It's not closed.\n\r",        ch ); return; }
    if ( pexit->key < 0 )
        { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
    if ( !has_key( ch, pexit->key) &&
        (!is_affected(ch, gsn_gift_artisanscommand) ||
        get_affect_level(ch, gsn_gift_artisanscommand) < 0)
        )
        { send_to_char( "You lack the key.\n\r",       ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
    if (IS_SET(pexit->exit_info, EX_PICKPROOF) && !has_key(ch, pexit->key) &&
        is_affected(ch, gsn_gift_artisanscommand))
    {
      send_to_char("The spirits cannot seem to unlock the exit.\n\r", ch);
      return;
    }

    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    if (is_affected(ch, gsn_gift_artisanscommand))
      send_to_char("You ask the spirits of the lock to open, and they comply willingly.\n\r", ch);
    send_to_char( "*Click*\n\r", ch );
    act( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

    /* unlock the other side */
    if ( ( to_room   = pexit->u1.to_room            ) != NULL
    &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
    &&   pexit_rev->u1.to_room == ch->in_room )
    {
        REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
        return;
    }
    }


        if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
        {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET(obj->value[1],EX_ISDOOR))
            {
            send_to_char("You can't do that.\n\r",ch);
            return;
            }

            if (!IS_SET(obj->value[1],EX_CLOSED))
            {
            send_to_char("It's not closed.\n\r",ch);
            return;
            }

            if (obj->value[4] < 0)
            {
            send_to_char("It can't be unlocked.\n\r",ch);
            return;
            }

            if (!has_key(ch,obj->value[4]))
            {
            send_to_char("You lack the key.\n\r",ch);
            return;
            }

            if (!IS_SET(obj->value[1],EX_LOCKED))
            {
            send_to_char("It's already unlocked.\n\r",ch);
            return;
            }

            REMOVE_BIT(obj->value[1],EX_LOCKED);
            act("You unlock $p.",ch,obj,NULL,TO_CHAR);
            act("$n unlocks $p.",ch,obj,NULL,TO_ROOM);
            return;
        }

        /* 'unlock object' */
        if ( obj->item_type != ITEM_CONTAINER )
            { send_to_char( "That's not a container.\n\r", ch ); return; }
        if ( !IS_SET(obj->value[1], CONT_CLOSED) )
            { send_to_char( "It's not closed.\n\r",        ch ); return; }
        if ( obj->value[2] < 0 )
            { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
        if ( !has_key( ch, obj->value[2] ) &&
            (!is_affected(ch, gsn_gift_artisanscommand) ||
            get_affect_level(ch, gsn_gift_artisanscommand) < 0)
            )
            { send_to_char( "You lack the key.\n\r",       ch ); return; }
        if ( !IS_SET(obj->value[1], CONT_LOCKED) )
            { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
        if (IS_SET(obj->value[1], CONT_PICKPROOF) && !has_key(ch, obj->value[2]) &&
            is_affected(ch, gsn_gift_artisanscommand))
        {
          send_to_char("The spirits cannot seem to unlock this container.\n\r", ch);
          return;
        }

        REMOVE_BIT(obj->value[1], CONT_LOCKED);
        if (is_affected(ch, gsn_gift_artisanscommand))
          send_to_char("You ask the spirits of technology to unlock the container, and they comply.\n\r", ch);
        else
          act("You unlock $p.",ch,obj,NULL,TO_CHAR);
        act( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
        return;
        }

}



void do_pick( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj, *lockpick;
    int door;
    int pickdiff = 6;
    int picksuccess;

    one_argument( argument, arg );
    if(IS_NPC(ch)) return;

    if ( arg[0] == '\0' )
    {
      send_to_char( "Pick what?\n\r", ch );
      return;
    }

    if (MOUNTED(ch))
    {
        send_to_char("You can't pick locks while mounted.\n\r", ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_SHIFT))
    {
      send_to_char("You cannot properly hold a lockpick in an altered form.\n\r", ch);
      return;
    }

    if ((lockpick = get_consumable(ch, ITEM_LOCKPICK)) == NULL)
    {
        send_to_char("You do not have any lockpicks readily available.\n\r", ch);
        return;
    }

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
      if ( IS_NPC(gch) && IS_AWAKE(gch) && ch->level + 5 < gch->level )
      {
        act( "$N is standing too close to the lock.", ch, NULL, gch, TO_CHAR );
        return;
      }
    }

    WAIT_STATE( ch, skill_table[gsn_pick_lock].beats );
    pickdiff = pickdiff + lockpick->value[1];

    if ( ( door = find_door( ch, arg ) ) >= 0 )
    {
    /* 'pick door' */
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;

    pexit = ch->in_room->exit[door];
    if ( !IS_SET(pexit->exit_info, EX_CLOSED) && !IS_IMMORTAL(ch))
        { send_to_char( "It's not closed.\n\r",        ch ); return; }
    if ( pexit->key < 0 && !IS_IMMORTAL(ch))
        { send_to_char( "It can't be picked.\n\r",     ch ); return; }
    if ( !IS_SET(pexit->exit_info, EX_LOCKED) )
        { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_PICKPROOF) && !IS_IMMORTAL(ch))
        { send_to_char( "It can't be picked.\n\r",             ch ); return; }
    if ( IS_SET(pexit->exit_info, EX_EASY) )
      pickdiff--;
    if ( IS_SET(pexit->exit_info, EX_HARD) )
      pickdiff++;
    if ( IS_SET(pexit->exit_info, EX_INFURIATING) )
      pickdiff = pickdiff + 2;

    picksuccess = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_LEGERDEMAIN],pickdiff);

    if ( picksuccess < 0 )
    {
      send_to_char( "You break your lockpick, causing the lock to be unpickable.\n\r", ch);
      SET_BIT(pexit->exit_info, EX_PICKPROOF);
      extract_obj(lockpick);
      return;
    }

    if ( picksuccess == 0 )
    {
      send_to_char( "You failed.\n\r", ch);
      check_improve(ch,gsn_pick_lock,FALSE,4);
      use_consumable(ch, lockpick, -2);
      return;
    }

    REMOVE_BIT(pexit->exit_info, EX_LOCKED);
    send_to_char( "*Click*\n\r", ch );
    act( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
    check_improve(ch,gsn_pick_lock,TRUE,4);
    use_consumable(ch, lockpick, -1);
    gain_exp(ch, picksuccess);

    /* pick the other side */
      if ( ( to_room   = pexit->u1.to_room            ) != NULL
      &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
      &&   pexit_rev->u1.to_room == ch->in_room )
      {
        REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
        return;
      }
    }

        if ( ( obj = get_obj_here( ch, NULL, arg ) ) != NULL )
        {
        /* portal stuff */
        if (obj->item_type == ITEM_PORTAL)
        {
            if (!IS_SET(obj->value[1],EX_ISDOOR))
            {
              send_to_char("You can't do that.\n\r",ch);
              return;
            }

            if (!IS_SET(obj->value[1],EX_CLOSED))
            {
              send_to_char("It's not closed.\n\r",ch);
              return;
            }

            if (obj->value[4] < 0)
            {
              send_to_char("It can't be unlocked.\n\r",ch);
              return;
            }

            if (IS_SET(obj->value[1],EX_PICKPROOF))
            {
              send_to_char("You failed.\n\r",ch);
              return;
            }

            picksuccess = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_LEGERDEMAIN],pickdiff);

            if (picksuccess < 0)
            {
              act("Your fumbling causes you to break your lockpick.",ch,obj,NULL,TO_CHAR);
              extract_obj(lockpick);
              return;
            }

            if (picksuccess == 0)
            {
              act("You fail to pick the lock on $p.", ch, obj, NULL, TO_CHAR);
              use_consumable(ch, lockpick, -2);
              return;
            }

            REMOVE_BIT(obj->value[1],EX_LOCKED);
            act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
            act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
            check_improve(ch,gsn_pick_lock,TRUE,4);
            use_consumable(ch, lockpick, -1);
            gain_exp(ch, picksuccess);
            return;
        }

        /* 'pick object' */
        if ( obj->item_type != ITEM_CONTAINER )
            { send_to_char( "That's not a container.\n\r", ch ); return; }
        if ( !IS_SET(obj->value[1], CONT_CLOSED) )
            { send_to_char( "It's not closed.\n\r",        ch ); return; }
        if ( obj->value[2] < 0 )
            { send_to_char( "It can't be unlocked.\n\r",   ch ); return; }
        if ( !IS_SET(obj->value[1], CONT_LOCKED) )
            { send_to_char( "It's already unlocked.\n\r",  ch ); return; }
        if ( IS_SET(obj->value[1], CONT_PICKPROOF) )
            { send_to_char( "You failed.\n\r",             ch ); return; }

        picksuccess = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_LEGERDEMAIN],pickdiff);

        if (picksuccess < 0)
        {
          act("Your fumbling causes you to break your lockpick.",ch,obj,NULL,TO_CHAR);
          extract_obj(lockpick);
          return;
        }

        if (picksuccess == 0)
        {
          act("You fail to pick the lock on $p.", ch, obj, NULL, TO_CHAR);
          use_consumable(ch, lockpick, -2);
          return;
        }

        REMOVE_BIT(obj->value[1], CONT_LOCKED);
            act("You pick the lock on $p.",ch,obj,NULL,TO_CHAR);
            act("$n picks the lock on $p.",ch,obj,NULL,TO_ROOM);
        check_improve(ch,gsn_pick_lock,TRUE,4);
        use_consumable(ch, lockpick, -1);
        gain_exp(ch, picksuccess);
        return;
        }
}


void do_stand( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (argument[0] != '\0')
    {
    if (ch->position == POS_FIGHTING)
    {
        send_to_char("Maybe you should finish fighting first?\n\r",ch);
        return;
    }
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (obj == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    if (obj->item_type != ITEM_FURNITURE
    ||  (!IS_SET(obj->value[2],STAND_AT)
    &&   !IS_SET(obj->value[2],STAND_ON)
    &&   !IS_SET(obj->value[2],STAND_IN)))
    {
        send_to_char("You can't seem to find a place to stand.\n\r",ch);
        return;
    }
    if (ch->on != obj && count_users(obj) >= obj->value[0])
    {
        act_new("There's no room to stand on $p.",
        ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        return;
    }
    ch->on = obj;
    if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	   p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
    if ( IS_AFFECTED(ch, AFF_SLEEP) )
        { send_to_char( "You can't wake up!\n\r", ch ); return; }

    if (obj == NULL)
    {
        send_to_char( "You wake and stand up.\n\r", ch );
        act( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
        ch->on = NULL;
    }
    else if (IS_SET(obj->value[2],STAND_AT))
    {
       act_new("You wake and stand at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
       act("$n wakes and stands at $p.",ch,obj,NULL,TO_ROOM);
    }
    else if (IS_SET(obj->value[2],STAND_ON))
    {
        act_new("You wake and stand on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        act("$n wakes and stands on $p.",ch,obj,NULL,TO_ROOM);
    }
    else
    {
        act_new("You wake and stand in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        act("$n wakes and stands in $p.",ch,obj,NULL,TO_ROOM);
    }
    ch->position = POS_STANDING;
    do_function(ch, &do_look, "auto");
    break;


    case POS_RESTING: case POS_SITTING:
    if (obj == NULL)
    {
        send_to_char( "You stand up.\n\r", ch );
        act( "$n stands up.", ch, NULL, NULL, TO_ROOM );
        ch->on = NULL;
    }
    else if (IS_SET(obj->value[2],STAND_AT))
    {
        act("You stand at $p.",ch,obj,NULL,TO_CHAR);
        act("$n stands at $p.",ch,obj,NULL,TO_ROOM);
    }
    else if (IS_SET(obj->value[2],STAND_ON))
    {
        act("You stand on $p.",ch,obj,NULL,TO_CHAR);
        act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
    }
    else
    {
        act("You stand in $p.",ch,obj,NULL,TO_CHAR);
        act("$n stands on $p.",ch,obj,NULL,TO_ROOM);
    }
    ch->position = POS_STANDING;
    break;

    case POS_STANDING:
    send_to_char( "You are already standing.\n\r", ch );
    break;

    case POS_FIGHTING:
    send_to_char( "You are already fighting!\n\r", ch );
    break;
    }

    if(is_affected( ch, gsn_chant ))
    {
        send_to_char( "You stop your chanting.\n\r", ch );
        act( "$n stops chanting to $s God.",  ch, NULL, NULL, TO_ROOM );
        affect_strip(ch,gsn_chant);
    }
        affect_strip(ch,gsn_meditation);
    return;
}



void do_rest( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch))
    {
        send_to_char("You can't rest while mounted.\n\r", ch);
        return;
    }

    if (RIDDEN(ch))
    {
        send_to_char("You can't rest while being ridden.\n\r", ch);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
    send_to_char("You are already fighting!\n\r",ch);
    return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if (argument[0] != '\0')
    {
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (obj == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    }
    else obj = ch->on;

    if (obj != NULL)
    {
        if (obj->item_type != ITEM_FURNITURE
        ||  (!IS_SET(obj->value[2],REST_ON)
        &&   !IS_SET(obj->value[2],REST_IN)
        &&   !IS_SET(obj->value[2],REST_AT)))
        {
        send_to_char("You can't rest on that.\n\r",ch);
        return;
        }

        if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
        {
        act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        return;
        }

    ch->on = obj;
    if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
	   p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
    }

    switch ( ch->position )
    {
    case POS_SLEEPING:
    if (IS_AFFECTED(ch,AFF_SLEEP))
    {
        send_to_char("You can't wake up!\n\r",ch);
        return;
    }

    if (obj == NULL)
    {
        send_to_char( "You wake up and start resting.\n\r", ch );
        act ("$n wakes up and starts resting.",ch,NULL,NULL,TO_ROOM);
    }
    else if (IS_SET(obj->value[2],REST_AT))
    {
        act_new("You wake up and rest at $p.",
            ch,obj,NULL,TO_CHAR,POS_SLEEPING, FALSE);
        act("$n wakes up and rests at $p.",ch,obj,NULL,TO_ROOM);
    }
        else if (IS_SET(obj->value[2],REST_ON))
        {
            act_new("You wake up and rest on $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING, FALSE);
            act("$n wakes up and rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
            act_new("You wake up and rest in $p.",
                    ch,obj,NULL,TO_CHAR,POS_SLEEPING, FALSE);
            act("$n wakes up and rests in $p.",ch,obj,NULL,TO_ROOM);
        }
    ch->position = POS_RESTING;
    break;

    case POS_RESTING:
    send_to_char( "You are already resting.\n\r", ch );
    break;

    case POS_STANDING:
    if (obj == NULL)
    {
        send_to_char( "You rest.\n\r", ch );
        act( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
    }
        else if (IS_SET(obj->value[2],REST_AT))
        {
        act("You sit down at $p and rest.",ch,obj,NULL,TO_CHAR);
        act("$n sits down at $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
        act("You sit on $p and rest.",ch,obj,NULL,TO_CHAR);
        act("$n sits on $p and rests.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
        act("You rest in $p.",ch,obj,NULL,TO_CHAR);
        act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
        }
    ch->position = POS_RESTING;
    break;

    case POS_SITTING:
    if (obj == NULL)
    {
        send_to_char("You rest.\n\r",ch);
        act("$n rests.",ch,NULL,NULL,TO_ROOM);
    }
        else if (IS_SET(obj->value[2],REST_AT))
        {
        act("You rest at $p.",ch,obj,NULL,TO_CHAR);
        act("$n rests at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],REST_ON))
        {
        act("You rest on $p.",ch,obj,NULL,TO_CHAR);
        act("$n rests on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
        act("You rest in $p.",ch,obj,NULL,TO_CHAR);
        act("$n rests in $p.",ch,obj,NULL,TO_ROOM);
    }
    ch->position = POS_RESTING;
    break;
    }


    return;
}


void do_sit (CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch))
    {
        send_to_char("You can't sit while mounted.\n\r", ch);
        return;
    }

    if (RIDDEN(ch))
    {
        send_to_char("You can't sit while being ridden.\n\r", ch);
        return;
    }


    if (ch->position == POS_FIGHTING)
    {
    send_to_char("Maybe you should finish this fight first?\n\r",ch);
    return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if (argument[0] != '\0')
    {
    obj = get_obj_list(ch,argument,ch->in_room->contents);
    if (obj == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    }
    else obj = ch->on;

    if (obj != NULL)
    {
    if (obj->item_type != ITEM_FURNITURE
    ||  (!IS_SET(obj->value[2],SIT_ON)
    &&   !IS_SET(obj->value[2],SIT_IN)
    &&   !IS_SET(obj->value[2],SIT_AT)))
    {
        send_to_char("You can't sit on that.\n\r",ch);
        return;
    }

    if (obj != NULL && ch->on != obj && count_users(obj) >= obj->value[0])
    {
        act_new("There's no more room on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        return;
    }

    ch->on = obj;
    if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
       p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
    }
    switch (ch->position)
    {
    case POS_SLEEPING:
        if (IS_AFFECTED(ch,AFF_SLEEP))
        {
        send_to_char("You can't wake up!\n\r",ch);
        return;
        }

            if (obj == NULL)
            {
                send_to_char( "You wake and sit up.\n\r", ch );
                act( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
            }
            else if (IS_SET(obj->value[2],SIT_AT))
            {
                act_new("You wake and sit at $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
                act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else if (IS_SET(obj->value[2],SIT_ON))
            {
                act_new("You wake and sit on $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
                act("$n wakes and sits at $p.",ch,obj,NULL,TO_ROOM);
            }
            else
            {
                act_new("You wake and sit in $p.",ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
                act("$n wakes and sits in $p.",ch,obj,NULL,TO_ROOM);
            }

        ch->position = POS_SITTING;
        break;
    case POS_RESTING:
        if (obj == NULL)
        send_to_char("You stop resting.\n\r",ch);
        else if (IS_SET(obj->value[2],SIT_AT))
        {
        act("You sit at $p.",ch,obj,NULL,TO_CHAR);
        act("$n sits at $p.",ch,obj,NULL,TO_ROOM);
        }

        else if (IS_SET(obj->value[2],SIT_ON))
        {
        act("You sit on $p.",ch,obj,NULL,TO_CHAR);
        act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
        }
        ch->position = POS_SITTING;
        break;
    case POS_SITTING:
        send_to_char("You are already sitting down.\n\r",ch);
        break;
    case POS_STANDING:
        if (obj == NULL)
            {
        send_to_char("You sit down.\n\r",ch);
                act("$n sits down on the ground.",ch,NULL,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],SIT_AT))
        {
        act("You sit down at $p.",ch,obj,NULL,TO_CHAR);
        act("$n sits down at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],SIT_ON))
        {
        act("You sit on $p.",ch,obj,NULL,TO_CHAR);
        act("$n sits on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
        act("You sit down in $p.",ch,obj,NULL,TO_CHAR);
        act("$n sits down in $p.",ch,obj,NULL,TO_ROOM);
        }
            ch->position = POS_SITTING;
            break;
    }
    return;
}


void do_sleep( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;

    if (MOUNTED(ch))
    {
        send_to_char("You can't sleep while mounted.\n\r", ch);
        return;
    }

    if (RIDDEN(ch))
    {
        send_to_char("You can't sleep while being ridden.\n\r", ch);
        return;
    }


    switch ( ch->position )
    {
    case POS_SLEEPING:
    send_to_char( "You are already sleeping.\n\r", ch );
    break;

    case POS_RESTING:
    case POS_SITTING:
    case POS_STANDING:
    if (argument[0] == '\0' && ch->on == NULL)
    {
        send_to_char( "You go to sleep.\n\r", ch );
        act( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
        ch->position = POS_SLEEPING;
    }
    else  /* find an object and sleep on it */
    {
        if (argument[0] == '\0')
        obj = ch->on;
        else
            obj = get_obj_list( ch, argument,  ch->in_room->contents );

        if (obj == NULL)
        {
        send_to_char("You don't see that here.\n\r",ch);
        return;
        }
        if (obj->item_type != ITEM_FURNITURE
        ||  (!IS_SET(obj->value[2],SLEEP_ON)
        &&   !IS_SET(obj->value[2],SLEEP_IN)
        &&   !IS_SET(obj->value[2],SLEEP_AT)))
        {
        send_to_char("You can't sleep on that!\n\r",ch);
        return;
        }

        if (ch->on != obj && count_users(obj) >= obj->value[0])
        {
        act_new("There is no room on $p for you.",
            ch,obj,NULL,TO_CHAR,POS_DEAD, FALSE);
        return;
        }

        ch->on = obj;
        if ( HAS_TRIGGER_OBJ( obj, TRIG_SIT ) )
           p_percent_trigger( NULL, obj, NULL, ch, NULL, NULL, TRIG_SIT );
        if (IS_SET(obj->value[2],SLEEP_AT))
        {
        act("You go to sleep at $p.",ch,obj,NULL,TO_CHAR);
        act("$n goes to sleep at $p.",ch,obj,NULL,TO_ROOM);
        }
        else if (IS_SET(obj->value[2],SLEEP_ON))
        {
            act("You go to sleep on $p.",ch,obj,NULL,TO_CHAR);
            act("$n goes to sleep on $p.",ch,obj,NULL,TO_ROOM);
        }
        else
        {
        act("You go to sleep in $p.",ch,obj,NULL,TO_CHAR);
        act("$n goes to sleep in $p.",ch,obj,NULL,TO_ROOM);
        }
        ch->position = POS_SLEEPING;
    }
    break;

    case POS_FIGHTING:
    send_to_char( "You are already fighting!\n\r", ch );
    break;
    }

    return;
}



void do_wake( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    { do_function(ch, &do_stand, ""); return; }

    if ( !IS_AWAKE(ch) )
    { send_to_char( "You are asleep yourself!\n\r",       ch ); return; }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    { send_to_char( "They aren't here.\n\r",              ch ); return; }

    if ( IS_AWAKE(victim) )
    { act( "$N is already awake.", ch, NULL, victim, TO_CHAR ); return; }

    if ( IS_AFFECTED(victim, AFF_SLEEP) )
    { act( "You can't wake $M!",   ch, NULL, victim, TO_CHAR );  return; }

    act_new( "$n wakes you.", ch, NULL, victim, TO_VICT,POS_SLEEPING, FALSE );
    do_function(victim, &do_stand, "");
    return;
}



void do_sneak( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int success;
    int diff = 6;

    if (is_affected(ch, gsn_gift_scentofrunningwater))
        diff--;

    if (MOUNTED(ch))
    {
        send_to_char("You can't sneak while riding.\n\r", ch);
        return;
    }

    if(is_affected(ch,gsn_sneak))
    {
        send_to_char("You stop sneaking around.\n\r",ch);
        affect_strip( ch, gsn_sneak );
        return;
    }

    if(IS_AFFECTED(ch,AFF_SNEAK))
    {
        send_to_char("You are already moving as silently as you can.\n\r",ch);
        return;
    }

    send_to_char( "Using all of your skill and poise, you attempt to move silently.\n\r", ch );

    if(!IS_NPC(ch))
    {
      int silence = 0;
      if (is_affected(ch,gsn_gift_silence))
        silence += 2;
      success = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_STEALTH]+silence,6);
      success += stealth_int_shadowplay(ch,6);
        if(success == 0)
        {
            send_to_char("Your own feet betray your position as you attempt to be silent.\n\r",ch);
            WAIT_STATE(ch, 12);
            return;
        }

        if(success < 0)
        {
            if((ch->move -= 100) < 0) ch->move = 0;
            send_to_char("Being overly cautious, you trip and stumble over your own two feet.\n\r",ch);
            WAIT_STATE(ch, 12);
            return;
        }
    }
    else success=1;

    if ( number_percent( ) < get_skill(ch,gsn_sneak))
    {
    check_improve(ch,gsn_sneak,TRUE,3);
    af.where     = TO_AFFECTS;
    af.type      = gsn_sneak;
    af.level     = ch->level;
    af.duration  = 10*success;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    send_to_char("You feel much stealthier.\n\r", ch);
    affect_to_char( ch, &af );
    WAIT_STATE(ch, 3);
    return;
    }
    else
    check_improve(ch,gsn_sneak,FALSE,3);
    send_to_char("Despite your best attempt, you can't seem to sneak about!\n\r", ch);
    WAIT_STATE(ch, 6);

    return;
}

/* void do_hide (CHAR_DATA * ch, char *argument)
{
  AFFECT_DATA af;

if (argument[0] == '\0')
    {
        send_to_char( "You attempt to hide.\n\r", ch );

        if ( IS_AFFECTED(ch, AFF_HIDE) )
            return;

        if ( number_percent( ) < get_skill(ch,gsn_hide))
        {

            af.where     = TO_AFFECTS;
            af.type      = gsn_hide;
            af.level     = ch->level;
            af.duration  = ch->level + 12;
            af.location  = APPLY_NONE;
            af.modifier  = 0;
            af.bitvector = AFF_HIDE;
            affect_to_char( ch, &af );
            send_to_char( "You are now hidden!.\n\r", ch );
            check_improve(ch,gsn_hide,TRUE,3);
            return;
        }
        else
        {
            check_improve(ch,gsn_hide,FALSE,3);
            return;
        }
}*/


void do_hide (CHAR_DATA * ch, char *argument)
{
  AFFECT_DATA af;

    if (MOUNTED(ch))
    {
        send_to_char("You can't hide while riding.\n\r", ch);
        return;
    }

  send_to_char ("You attempt to hide.\n\r", ch);
  affect_strip (ch, gsn_hide);

  if (IS_AFFECTED (ch, AFF_HIDE))
    return;

  if (number_percent () < get_skill (ch, gsn_hide))
    {
      check_improve (ch, gsn_hide, TRUE, 3);

      af.where = TO_AFFECTS;
      af.type = gsn_hide;
      af.level = ch->level;
      af.duration = ch->level;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = AFF_HIDE;
      affect_to_char (ch, &af);
    }
  else
    check_improve (ch, gsn_hide, FALSE, 3);

  return;
}

/*
void do_hide( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You attempt to hide.\n\r", ch );

    if ( IS_AFFECTED(ch, AFF_HIDE) )
    REMOVE_BIT(ch->affected_by, AFF_HIDE);

    if ( number_percent( ) < get_skill(ch,gsn_hide))
    {
    SET_BIT(ch->affected_by, AFF_HIDE);
    check_improve(ch,gsn_hide,TRUE,3);
    }
    else
    check_improve(ch,gsn_hide,FALSE,3);

    return;
}
*/


/*
 * Contributed by Alander.
 */
void do_visible( CHAR_DATA *ch, char *argument )
{
    affect_strip ( ch, gsn_invis            );
    affect_strip ( ch, gsn_mass_invis           );
    affect_strip ( ch, gsn_sneak            );
    affect_strip ( ch, gsn_hide         );
    affect_strip ( ch, gsn_cloakshadow          );
    affect_strip ( ch, gsn_cloak_the_gathering      );
    affect_strip ( ch, gsn_unseen               );
    affect_strip ( ch, gsn_veil);

    REMOVE_BIT   ( ch->affected_by, AFF_HIDE        );
    REMOVE_BIT   ( ch->affected_by, AFF_INVISIBLE   );
    REMOVE_BIT   ( ch->affected_by, AFF_SNEAK       );
    REMOVE_BIT   ( ch->affected2_by, AFF2_VEIL      );
    REMOVE_BIT   ( ch->affected2_by, AFF2_UNSEEN    );

    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_recall( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int vnum;

    if (IS_NPC(ch) )
    {
    send_to_char("Only players can recall.\n\r",ch);
    return;
    }

    act( "$n calls out to the heavens for safe transportation!", ch, 0, 0, TO_ROOM );

    if (!IS_NPC(ch) && ch->pcdata->hometown && ch->pcdata->hometown < MAX_HOMETOWN)
	    vnum = get_hometown(ch->pcdata->hometown);
    else
	    vnum = ROOM_VNUM_TEMPLE;

    if ( ( location = get_room_index( vnum ) ) == NULL )
    {
    send_to_char( "You are completely lost.\n\r", ch );
    return;
    }

    if(!IS_NPC(ch) && argument[0] != '\0' && !str_prefix(argument, "home"))
    {
        OBJ_DATA *obj;
        OBJ_INDEX_DATA *pObjIndex;

        if(ch->pcdata->home == 0)
        {
            send_to_char("You don't have a home!\n\r",ch);
            return;
        }

        if ( ( pObjIndex = get_obj_index( ch->pcdata->home ) ) == NULL )
        {
            send_to_char( "Can't find your key! Contact an Immortal.\n\r", ch );
            return;
        }

        obj = create_object( pObjIndex, ch->level );
        if ( (ch->silver + 100 * ch->gold) < obj->cost )
        {
            send_to_char( "You can't afford to recall home.\n\r", ch );
            return;
        }

        deduct_cost(ch,obj->cost);
        location = get_room_index(ch->pcdata->home);
        extract_obj(obj);
    }

     if ( !IS_NPC(ch) && !IS_NEWBIE(ch) && (ch->silver + (100 * ch->gold)) < RECALL_COST*100 )
        {
            send_to_char( "You can't afford to recall.\n\r", ch );
            return;
        }

    if ( ch->in_room == location )
    return;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    {
    send_to_char( "Twisted laughter fills your head as your prayer for help goes unanswered.  The Mad Hatter has forsaken you.\n\r", ch );
    return;
    }
    if (is_affected( ch, gsn_arena ))
    {
    send_to_char( "You cannot recall yet. Wait for the {RArena{x timer.\n\r", ch );
    return;
    }
    if ( ( victim = ch->fighting ) != NULL )
    {
    int lose,skill;

    skill = get_skill(ch,gsn_recall);

    if ( number_percent() < 80 * skill / 100 )
    {
        check_improve(ch,gsn_recall,FALSE,1);
        WAIT_STATE( ch, 4 );
        sprintf( buf, "You failed!.\n\r");
        send_to_char( buf, ch );
        return;
    }

    lose = (ch->desc != NULL) ? 25 : 50;
    gain_exp( ch, 0 - lose );
    check_improve(ch,gsn_recall,TRUE,1);
    sprintf( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
    send_to_char( buf, ch );
    stop_fighting( ch, TRUE );

    }

    ch->move /= 2;
    if (!IS_NEWBIE(ch))
        deduct_cost(ch,RECALL_COST*100);
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );
    act( "$n vanishes mysteriously and swiftly.", ch, NULL, NULL, TO_ROOM );
    REMOVE_BIT(ch->act,PLR_ARENA);
    REMOVE_BIT(ch->act,PLR_SPEC);
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n mysteriously appears in the room, looking briefly dazed.", ch, NULL, NULL, TO_ROOM );
    do_function(ch, &do_look, "auto" );

    if (ch->pet != NULL)
    {
        char_from_room(ch->pet);
	char_to_room(ch->pet, location);
    }
    if (ch->mount != NULL)
    {
        char_from_room(ch->mount);
	char_to_room(ch->mount, location);
    }

    return;
}



void do_train( CHAR_DATA *ch, char *argument )
{
    return;
}


void do_knock( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *was_in;
    EXIT_DATA *pexit;
    int door;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch)) return;


    if (!IS_SET(ch->act,PLR_IC))
    {
        send_to_char( "You must be IC to use this command!\n\r", ch );
    return;
    }

    if ( arg1[0] == '\0' )
    {
    send_to_char("Knock in which direction?\n\r", ch );
    return;
    }

         if ( !str_prefix( arg1, "n" ) || !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "e" ) || !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "s" ) || !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "w" ) || !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "u" ) || !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "d" ) || !str_prefix( arg1, "down"  ) ) door = 5;
    else
    {
    send_to_char( "You can only knock in directions.\n\r", ch );
    return;
    }

    if ( (pexit = ch->in_room->exit[door] ) == NULL )
    {
    send_to_char( "There is no door in that direction.\n\r", ch );
    return;
    }

    to_room = ch->in_room->exit[door]->u1.to_room;
    was_in = ch->in_room;

    char_from_room( ch );
    char_to_room( ch, to_room );

    act( "You hear a knock on the door.", ch, NULL, NULL, TO_ROOM );
    act( "You knock on the door.", ch, NULL, NULL, TO_CHAR );

    char_from_room( ch );
    char_to_room( ch, was_in );
}

void do_meditate(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int diff;
    int skill;

	if (IS_NPC(ch))
		return;
    skill = get_skill(ch, gsn_meditation);

    if (skill < 50)
    {
        send_to_char("You have not been trained in the art of stilling your mind.\n\r", ch);
        return;
    }

    if ( is_affected( ch, gsn_meditation ))
    {
        send_to_char( "Be still, you are already meditating.\n\r", ch );
        return;
    }
    if(ch->position > POS_SITTING)
    {
        send_to_char( "You cannot do this standing!\n\r", ch );
        return;
    }
    diff = 8;
    if (skill >= 65) diff --;
    if (skill >= 80) diff --;
    if (skill == 100) diff --;
    if(godice(get_attribute(ch, STAMINA) + ch->pcdata->cssec_abil[CSABIL_MEDITATION], 8) < 1 )
    {
        send_to_char( "You fail to still your mind.\n\r", ch );
        WAIT_STATE( ch, 36 );
        return;
    }

    act( "$n assumes a comfortable position and begins to meditate.",  ch, NULL, NULL, TO_ROOM );
    send_to_char( "You assume a comfortable position and begin to meditate.\n\r", ch );

    af.where    = TO_AFFECTS;
    af.type     = gsn_meditation;
    af.level    = ch->level;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector    = 0;
    affect_to_char( ch, &af );
return;
}

void do_hometown (CHAR_DATA *ch, char *argument)
{
    int hn;
    char arg1[MSL];
    char arg2[MSL];
    AFFECT_DATA af;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1))
    {
      sendch("                    ____________\n\r", ch);
      sendch(" +==================  HOMETOWN  =================+\n\r", ch);
      sendch(" +   Allows you to set your city {CRecall{x point.   +\n\r", ch);
      sendch(" +      Check the list for current cities.       +\n\r", ch);
      sendch(" +-----------------------------------------------+\n\r", ch);
      sendch(" + Syntax:          hometown <city>              +\n\r", ch);
      sendch(" +                  hometown list                +\n\r", ch);
      sendch(" +-----------------------------------------------+\n\r", ch);
      sendch(" + Cost  :          {Y250 Quest Points{x             +\n\r", ch);
      if (is_affected(ch, gsn_hometown_change))
      {
        sendch(" +-----------------------------------------------+\n\r", ch);
        sendch(" +   {R*{x You have changed hometown recently,       +\n\r +       and cannot do so again so soon.   {R*{x     +\n\r", ch);
      }
      sendch(" +===============================================+\n\r", ch);
      return;
    }

/*    if (ch->pcdata->hometown > 0)
    {
        sendch("Your hometown has already been set! See an Immortal if this is an error.\n\r", ch);
        return;
    }
*/
    if (!str_prefix(arg1, "list"))
    {
        int i, col;
        col = 0;
        sendch("               {y______________________{x\n\r", ch);
        sendch(" +{D================{y!!!  CITIES  !!!{D==============={x+\n\r", ch);
        sendch(" +             {y``````````````````````{x            +\n\r", ch);
        sendch(" +  You may select from the following Hometowns. +\n\r", ch);
        sendch(" +-----------------------------------------------+", ch);
        for (i = 1; i < MAX_HOMETOWN; i++)
        {
          if (col == 0)
            sendch("\n\r", ch);
          cprintf(ch, " {G%s{x ", center(hometown_table[i].name, 10, " "));
          if (++col > 3)
            col = 0;
        }

        sendch("\n\r +{D============={y=====HHHHHHHHHHHH====={D============{x+\n\r", ch);
        return;
    }

    if ( (hn = get_hometown_num(arg1)) < 0 || hn > MAX_HOMETOWN )
    {
        sendch("That is not a valid hometown. Type 'hometown list' to see choices.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_hometown_change))
    {
      send_to_char("You have already changed hometown recently.  Wait a while before doing it again.\n\r", ch);
      return;
    }

    if (ch->qpoints < 250)
    {
      send_to_char("It costs {Y250 Quest Points{x to be able to change your Hometown location.\n\r", ch);
      return;
    }

    if (!str_cmp(arg2, "confirm"))
    {
        ch->qpoints -= 250;
        sendch(" +-----------------------------------------------+\n\r", ch);
        cprintf(ch, " +    Setting new hometown to:   {y%10s{x      +\n\r +    {Y250QP{x deducted.    {g%6dQP{x remaining.     +\n\r", hometown_table[hn].name, ch->qpoints);
        sendch(" +-----------------------------------------------+\n\r", ch);
        ch->pcdata->hometown = hn;

        af.where      = TO_AFFECTS;
        af.type       = gsn_hometown_change;
        af.level      = 0;
        af.duration   = 500;
        af.modifier   = 0;
        af.location   = APPLY_NONE;
        af.bitvector  = 0;
        affect_to_char( ch, &af );
        return;
    }
    else
    {
        cprintf(ch, "To set your recall location to %s, type 'hometown %s confirm'\n\rRemember, you will be charged {Y250QP{x for this change.\n\r",
          hometown_table[hn].name, hometown_table[hn].name);
        return;
    }

}

/* random room generation procedure */
ROOM_INDEX_DATA  *get_random_room(CHAR_DATA *ch)
{
    ROOM_INDEX_DATA *room;

    for ( ; ; )
    {
        room = get_room_index( number_range( 0, 65535 ) );
        if ( room != NULL )
        if ( can_see_room(ch,room)
	&&   !room_is_private(room)
        &&  !IS_SET(room->area->area_flags, AREA_NOTELE)
        &&  !IS_SET(room->area->area_flags, AREA_UNLINKED)
        &&   !IS_SET(room->room_flags, ROOM_PRIVATE)
        &&   !IS_SET(room->room_flags, ROOM_UMBRA)
        &&   !IS_SET(room->room_flags, ROOM_SOLITARY)
	&&   !IS_SET(room->room_flags, ROOM_SAFE)
	&&   (IS_NPC(ch) || IS_SET(ch->act,ACT_AGGRESSIVE)
	||   !IS_SET(room->room_flags,ROOM_LAW)))
            break;
    }

    return room;
}

/* RT Enter portals */
void do_enter( CHAR_DATA *ch, char *argument)
{
    ROOM_INDEX_DATA *location;

    if ( ch->fighting != NULL )
	return;

    /* nifty portal stuff */
    if (argument[0] != '\0')
    {
        ROOM_INDEX_DATA *old_room;
	OBJ_DATA *portal;
	CHAR_DATA *fch, *fch_next;

        old_room = ch->in_room;

	portal = get_obj_list( ch, argument,  ch->in_room->contents );

	if (portal == NULL)
	{
	    send_to_char("You don't see that here.\n\r",ch);
	    return;
	}

	if (portal->item_type != ITEM_PORTAL
        ||  (IS_SET(portal->value[1],EX_CLOSED) && !IS_TRUSTED(ch,ANGEL)))
	{
	    send_to_char("You can't seem to find a way in.\n\r",ch);
	    return;
	}

	if (!IS_TRUSTED(ch,ANGEL) && !IS_SET(portal->value[2],GATE_NOCURSE)
	&&  (IS_AFFECTED(ch,AFF_CURSE)
	||   IS_SET(old_room->room_flags,ROOM_NO_RECALL)))
	{
	    send_to_char("Something prevents you from leaving...\n\r",ch);
	    return;
	}

  if (IS_SET(portal->value[2], GATE_UMBRA) && is_affected(ch, gsn_astralprojection))
  {
      act("Your astral form refuses to cross through $p.  You must follow your silver cord to your waiting body.", ch, portal, NULL, TO_CHAR);
      act("$n's form wavers for a moment as it refuses to cross into $p.", ch, portal, NULL, TO_NOTVICT);
      return;
  }

  if (IS_SET(portal->value[2],GATE_RANDOM) || portal->value[3] == -1)
	{
	    location = get_random_room(ch);
	    portal->value[3] = location->vnum; /* for record keeping :) */
	}
	else if (IS_SET(portal->value[2],GATE_BUGGY) && (number_percent() < 5))
	    location = get_random_room(ch);
	else
	    location = get_room_index(portal->value[3]);

	if (location == NULL
	||  (location == old_room && !IS_SET(portal->value[2], GATE_UMBRA))
	||  !can_see_room(ch,location))
	{
	   act("$p doesn't seem to go anywhere.",ch,portal,NULL,TO_CHAR);
	   return;
	}

    if (portal->level <= 100 && room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR))
	{
	   act("$p doesn't seem to go anywhere.",ch,portal,NULL,TO_CHAR);
	   return;
	}

        if (IS_NPC(ch) && IS_SET(ch->act,ACT_AGGRESSIVE)
        &&  IS_SET(location->room_flags,ROOM_LAW))
        {
            send_to_char("Something prevents you from leaving...\n\r",ch);
            return;
        }

	act("$n steps into $p.",ch,portal,NULL,TO_ROOM);

	if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
	    act("You enter $p.",ch,portal,NULL,TO_CHAR);
	else
	    act("You walk through $p and find yourself somewhere else...",
	        ch,portal,NULL,TO_CHAR);

    char_from_room(ch);
    char_to_room(ch, location);

    if (IS_SET(portal->value[2], GATE_UMBRA) && !pass_gauntlet(ch, TRUE))
    {
	    char_from_room(ch);
	    char_to_room(ch, old_room);
        sendch("It is too dangerous to continue onward.\n\r", ch);
        return;
    }

	if (IS_SET(portal->value[2],GATE_GOWITH)) /* take the gate along */
	{
	    obj_from_room(portal);
	    obj_to_room(portal,location);
	}


	if (IS_SET(portal->value[2],GATE_NORMAL_EXIT))
	    act("$n has arrived.",ch,portal,NULL,TO_ROOM);
	else
	    act("$n has arrived through $p.",ch,portal,NULL,TO_ROOM);

	do_function(ch, &do_look, "auto");

	/* charges */
	if (portal->value[0] > 0)
	{
	    portal->value[0]--;
	    if (portal->value[0] == 0)
		portal->value[0] = -1;
	}

	/* protect against circular follows */
	if (old_room == location && !IS_SET(portal->value[2], GATE_UMBRA))
	    return;

    	for ( fch = old_room->people; fch != NULL; fch = fch_next )
    	{
            fch_next = fch->next_in_room;

            if (portal == NULL || portal->value[0] == -1)
	    /* no following through dead portals */
                continue;

            if ( fch->master == ch && IS_AFFECTED(fch,AFF_CHARM)
            &&   fch->position < POS_STANDING)
            	do_function(fch, &do_stand, "");

            if ( fch->master == ch && fch->position == POS_STANDING)
            {

                if (IS_SET(ch->in_room->room_flags,ROOM_LAW)
                &&  (IS_NPC(fch) && IS_SET(fch->act,ACT_AGGRESSIVE)))
                {
                    act("You can't bring $N into the city.",
                    	ch,NULL,fch,TO_CHAR);
                    act("You aren't allowed in the city.",
                    	fch,NULL,NULL,TO_CHAR);
                    continue;
            	}

            	act( "You follow $N.", fch, NULL, ch, TO_CHAR );
		do_function(fch, &do_enter, argument);
            }
    	}

 	if (portal != NULL && portal->value[0] == -1)
	{
	    act("$p fades out of existence.",ch,portal,NULL,TO_CHAR);
	    if (ch->in_room == old_room)
		act("$p fades out of existence.",ch,portal,NULL,TO_ROOM);
	    else if (old_room->people != NULL)
	    {
		act("$p fades out of existence.",
		    old_room->people,portal,NULL,TO_CHAR);
		act("$p fades out of existence.",
		    old_room->people,portal,NULL,TO_ROOM);
	    }
	    extract_obj(portal);
	}


	/*
	 * If someone is following the char, these triggers get activated
	 * for the followers before the char, but it's safer this way...
	 */
	if ( IS_NPC( ch ) && HAS_TRIGGER_MOB( ch, TRIG_ENTRY ) )
	    p_percent_trigger( ch, NULL, NULL, NULL, NULL, NULL, TRIG_ENTRY );
	if ( !IS_NPC( ch ) )
    {
        p_greet_trigger( ch, PRG_MPROG );
        p_greet_trigger( ch, PRG_OPROG );
        p_greet_trigger( ch, PRG_RPROG );
    }


	return;
    }

    send_to_char("Nope, can't do it.\n\r",ch);
    return;
}
