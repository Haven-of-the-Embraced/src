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
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Based on MERC 2.2 MOBprograms by N'Atas-ha.                            *
 *  Written and adapted to ROM 2.4 by                                      *
 *          Markku Nylander (markku.nylander@uta.fi)                       *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "mob_cmds.h"

DECLARE_DO_FUN( do_look 	);
extern ROOM_INDEX_DATA *find_location( CHAR_DATA *, char * );

/*
 * MOBcommand table.
 */
const	struct	mob_cmd_type	mob_cmd_table	[] =
{
	{	"asound", 	do_mpasound	},
	{	"gecho",	do_mpgecho	},
	{	"zecho",	do_mpzecho	},
	{	"kill",		do_mpkill	},
	{	"assist",	do_mpassist	},
	{	"junk",		do_mpjunk	},
	{	"echo",		do_mpecho	},
	{	"echoaround",	do_mpechoaround	},
	{	"echoat",	do_mpechoat	},
	{	"mload",	do_mpmload	},
	{	"oload",	do_mpoload	},
	{	"purge",	do_mppurge	},
	{	"goto",		do_mpgoto	},
	{	"at",		do_mpat		},
	{	"transfer",	do_mptransfer	},
	{	"gtransfer",	do_mpgtransfer	},
	{	"otransfer",	do_mpotransfer	},
	{	"force",	do_mpforce	},
	{	"gforce",	do_mpgforce	},
	{	"vforce",	do_mpvforce	},
	{	"cast",		do_mpcast	},
	{	"damage",	do_mpdamage	},
	{	"remember",	do_mpremember	},
	{	"forget",	do_mpforget	},
	{	"delay",	do_mpdelay	},
	{	"cancel",	do_mpcancel	},
	{	"call",		do_mpcall	},
	{	"flee",		do_mpflee	},
	{	"remove",	do_mpremove	},
	{	"quest",	do_mpquest	},
    {   "qpoint",   do_mpqpoint },
	{	"",		0		}
};

/*
 * OBJcommand table.
 */
const	struct	obj_cmd_type	obj_cmd_table	[] =
{
    {	"gecho",       	do_opgecho	},
    {	"zecho",	do_opzecho	},
    {	"echo",		do_opecho	},
    {	"echoaround",	do_opechoaround	},
    {	"echoat",	do_opechoat	},
    {	"mload",	do_opmload	},
    {	"oload",	do_opoload	},
    {	"purge",	do_oppurge	},
    {	"goto",		do_opgoto	},
    {	"transfer",	do_optransfer	},
    {	"gtransfer",	do_opgtransfer	},
    {	"otransfer",	do_opotransfer	},
    {	"force",	do_opforce	},
    {	"gforce",	do_opgforce	},
    {	"vforce",	do_opvforce	},
    {	"damage",	do_opdamage	},
    {	"remember",	do_opremember	},
    {	"forget",	do_opforget	},
    {	"delay",	do_opdelay	},
    {	"cancel",	do_opcancel	},
    {	"call",		do_opcall	},
    {	"remove",	do_opremove	},
    {	"",		0		}
};

/*
 * ROOMcommand table.
 */
const	struct  room_cmd_type	room_cmd_table	[] =
{
    {	"asound",	do_rpasound	},
    {	"gecho",	do_rpgecho	},
    {	"zecho",	do_rpzecho	},
    {	"echo",		do_rpecho 	},
    {	"echoaround",	do_rpechoaround },
    {	"echoat",	do_rpechoat 	},
    {	"mload",	do_rpmload 	},
    {	"oload",	do_rpoload 	},
    {	"purge",	do_rppurge 	},
    {	"transfer",	do_rptransfer 	},
    {	"gtransfer",	do_rpgtransfer 	},
    {	"otransfer",	do_rpotransfer 	},
    {	"force",	do_rpforce 	},
    {	"gforce",	do_rpgforce 	},
    {	"vforce",	do_rpvforce 	},
    {	"damage",       do_rpdamage 	},
    {	"remember",	do_rpremember 	},
    {	"forget",	do_rpforget 	},
    {	"delay",	do_rpdelay 	},
    {	"cancel",	do_rpcancel 	},
    {	"call",		do_rpcall 	},
    {	"remove",	do_rpremove 	},
    {	"",		0 		},
};

char *prog_type_to_name( int type )
{
    switch ( type )
    {
    case TRIG_ACT:             	return "ACT";
    case TRIG_SPEECH:          	return "SPEECH";
    case TRIG_RANDOM:          	return "RANDOM";
    case TRIG_FIGHT:           	return "FIGHT";
    case TRIG_HPCNT:           	return "HPCNT";
    case TRIG_DEATH:           	return "DEATH";
    case TRIG_ENTRY:           	return "ENTRY";
    case TRIG_GREET:           	return "GREET";
    case TRIG_GRALL:        	return "GRALL";
    case TRIG_GIVE:            	return "GIVE";
    case TRIG_BRIBE:           	return "BRIBE";
    case TRIG_KILL:	      		return "KILL";
    case TRIG_DELAY:           	return "DELAY";
    case TRIG_SURR:	      		return "SURRENDER";
    case TRIG_EXIT:	      		return "EXIT";
    case TRIG_EXALL:	      	return "EXALL";
    case TRIG_GET:				return "GET";
    case TRIG_DROP:				return "DROP";
    case TRIG_SIT:				return "SIT";
	case TRIG_SYSTIME:			return "SYSTIME";
	case TRIG_MUDTIME: 			return "MUDTIME";
    default:                  	return "ERROR";
    }
}

/*
 * MOBprog section
 * Mob command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void mob_interpret( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;

	if (ch->desc != NULL)
	{
		sprintf( buf, "Mob_interpret: '%s' tried executing '%s' through mob_interpret.", ch->name, argument );
	    bug( buf, 0 );
		do_huh(ch, "");
		return;
	}
    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; mob_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == mob_cmd_table[cmd].name[0]
	&&   !str_prefix( command, mob_cmd_table[cmd].name ) )
	{
	    (*mob_cmd_table[cmd].do_fun) ( ch, argument );
	    tail_chain( );
	    return;
	}
    }
    sprintf( buf, "Mob_interpret: invalid cmd from mob %d: '%s'",
	IS_NPC(ch) ? ch->pIndexData->vnum : 0, command );
    bug( buf, 0 );
}

/*
 * Displays MOBprogram triggers of a mobile
 *
 * Syntax: mpstat [name]
 */
void do_mpstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *mprg;
    CHAR_DATA   *victim;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Mpstat whom?\n\r", ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "No such creature.\n\r", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "That is not a mobile.\n\r", ch);
	return;
    }

    sprintf( arg, "Mobile #%-6d [%s]\n\r",
	victim->pIndexData->vnum, victim->short_descr );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	victim->mprog_delay,
	victim->mprog_target == NULL
		? "No target" : victim->mprog_target->name );
    send_to_char( arg, ch );

    if ( !victim->pIndexData->mprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, mprg = victim->pIndexData->mprogs; mprg != NULL;
	 mprg = mprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( mprg->trig_type ),
	      mprg->vnum,
	      mprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given MOBprogram
 *
 * Syntax: mpdump [vnum]
 */
void do_mpdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *mprg;

   one_argument( argument, buf );
   if ( ( mprg = get_prog_index( atoi(buf), PRG_MPROG) ) == NULL )
   {
	send_to_char( "No such MOBprogram.\n\r", ch );
	return;
   }
   page_to_char( mprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: mob gecho [string]
 */
void do_mpgecho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "MpGEcho: missing argument from vnum %d",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Mob echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: mob zecho [string]
 */
void do_mpzecho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "MpZEcho: missing argument from vnum %d",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ch->in_room == NULL )
	return;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == ch->in_room->area )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Mob echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all the rooms aroud the mobile
 *
 * Syntax: mob asound [string]
 */
void do_mpasound( CHAR_DATA *ch, char *argument )
{

    ROOM_INDEX_DATA *was_in_room;
    int              door;

    if ( argument[0] == '\0' )
	return;

    was_in_room = ch->in_room;
    for ( door = 0; door < 6; door++ )
    {
    	EXIT_DATA       *pexit;

      	if ( ( pexit = was_in_room->exit[door] ) != NULL
	  &&   pexit->u1.to_room != NULL
	  &&   pexit->u1.to_room != was_in_room )
      	{
	    ch->in_room = pexit->u1.to_room;
//	    MOBtrigger  = FALSE;
            MOBtrigger = TRUE;
	    act( argument, ch, NULL, NULL, TO_ROOM );
//	    MOBtrigger  = TRUE;
	}
    }
    ch->in_room = was_in_room;
    return;

}

/*
 * Lets the mobile kill any player or mobile without murder
 *
 * Syntax: mob kill [victim]
 */
void do_mpkill( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch || IS_NPC(victim) || ch->position == POS_FIGHTING )
	return;

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	bug( "MpKill - Charmed mob attacking master from vnum %d.",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}

/*
 * Lets the mobile assist another mob or player
 *
 * Syntax: mob assist [character]
 */
void do_mpassist( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch || ch->fighting != NULL || victim->fighting == NULL )
	return;

    multi_hit( ch, victim->fighting, TYPE_UNDEFINED );
    return;
}


/*
 * Lets the mobile destroy an object in its inventory
 * it can also destroy a worn object and it can destroy
 * items using all.xxxxx or just plain all of them
 *
 * Syntax: mob junk [item]
 */

void do_mpjunk( CHAR_DATA *ch, char *argument )
{
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    one_argument( argument, arg );

    if ( arg[0] == '\0')
	return;

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
    	if ( ( obj = get_obj_wear( ch, arg, TRUE ) ) != NULL )
      	{
      	    unequip_char( ch, obj );
	    extract_obj( obj );
    	    return;
      	}
      	if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
	    return;
	extract_obj( obj );
    }
    else
      	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
      	{
            obj_next = obj->next_content;
	    if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
            {
          	if ( obj->wear_loc != WEAR_NONE)
	    	unequip_char( ch, obj );
          	extract_obj( obj );
            }
      	}

    return;

}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: mob echoaround [victim] [string]
 */

void do_mpechoaround( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    act( argument, ch, NULL, victim, TO_NOTVICT );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: mob echoat [victim] [string]
 */
void do_mpechoat( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    act( argument, ch, NULL, victim, TO_VICT );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: mpecho [string]
 */
void do_mpecho( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
	return;
    act( argument, ch, NULL, NULL, TO_ROOM );
}

/*
 * Lets the mobile load another mobile.
 *
 * Syntax: mob mload [vnum]
 */
void do_mpmload( CHAR_DATA *ch, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( ch->in_room == NULL || arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Mpmload: bad mob index (%d) from mob %d",
	    vnum, IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    return;
}

/*
 * Lets the mobile load an object
 *
 * Syntax: mob oload [vnum] [level] {R}
 */
void do_mpoload( CHAR_DATA *ch, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;
    bool            fToroom = FALSE, fWear = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        bug( "Mpoload - Bad syntax from vnum %d.",
	    IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
        return;
    }

    if ( arg2[0] == '\0' )
    {
	level = get_trust( ch );
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    bug( "Mpoload - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
        }
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    bug( "Mpoload - Bad level from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
	}
    }

    /*
     * Added 3rd argument
     * omitted - load to mobile's inventory
     * 'R'     - load to room
     * 'W'     - load to mobile and force wear
     */
    if ( arg3[0] == 'R' || arg3[0] == 'r' )
	fToroom = TRUE;
    else if ( arg3[0] == 'W' || arg3[0] == 'w' )
	fWear = TRUE;

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Mpoload - Bad vnum arg from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    obj = create_object( pObjIndex, level );
    if ( (fWear || !fToroom) && CAN_WEAR(obj, ITEM_TAKE) )
    {
	obj_to_char( obj, ch );
	if ( fWear )
	    wear_obj( ch, obj, TRUE );
    }
    else
    {
	obj_to_room( obj, ch->in_room );
    }

    return;
}

/*
 * Lets the mobile purge all objects and other npcs in the room,
 * or purge a specified object or mob in the room. The mobile cannot
 * purge itself for safety reasons.
 *
 * syntax mob purge {target}
 */
void do_mppurge( CHAR_DATA *ch, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim ) && victim != ch
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !IS_SET(obj->extra_flags, ITEM_NOPURGE) )
		extract_obj( obj );
	}

	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( ch, NULL, arg ) ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    bug( "Mppurge - Bad argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Mppurge - Purging a PC from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    extract_char( victim, TRUE );
    return;
}


/*
 * Lets the mobile goto any location it wishes that is not private.
 *
 * Syntax: mob goto [location]
 */
void do_mpgoto( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "Mpgoto - No argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpgoto - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ch->fighting != NULL )
	stop_fighting( ch, TRUE );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}

/*
 * Lets the mobile do a command at another location.
 *
 * Syntax: mob at [location] [commands]
 */
void do_mpat( CHAR_DATA *ch, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    CHAR_DATA       *wch;
    OBJ_DATA 	    *on;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpat - Bad argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	bug( "Mpat - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    original = ch->in_room;
    on = ch->on;
    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );
	    ch->on = on;
	    break;
	}
    }

    return;
}

/*
 * Lets the mobile transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: mob transfer [target|'all'] [location]
 */
void do_mptransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mptransfer - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
            sprintf( buf, "%s %s", victim->name, arg2 );
	    do_mptransfer( ch, buf );
	}
	return;
    }

        if ( !str_cmp( arg1, "players" ) )
    {
	CHAR_DATA *victim_next;

	for ( victim = ch->in_room->people; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( !IS_NPC(victim) )
	    {
		sprintf( buf, "%s %s", victim->name, arg2 );
		do_mptransfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    bug( "Mptransfer - No such location from vnum %d.",
	        IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the mobile transfer all chars in same group as the victim.
 *
 * Syntax: mob gtransfer [victim] [location]
 */
void do_mpgtransfer( CHAR_DATA *ch, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Mpgtransfer - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( (who = get_char_room( ch, NULL, arg1 )) == NULL )
	return;

    for ( victim = ch->in_room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
	    sprintf( buf, "%s %s", victim->name, arg2 );
	    do_mptransfer( ch, buf );
    	}
    }
    return;
}

/*
 * Lets the mobile force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the mobile.
 *
 * Syntax: mob force [victim] [commands]
 */
void do_mpforce( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Mpforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == ch->in_room
		&& get_trust( vch ) < get_trust( ch )
		&& !IS_IMMORTAL(vch)
		&& can_see( ch, vch ) )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	    return;

	if ( victim == ch )
	    return;

	if(IS_IMMORTAL(victim))
	    return;

	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the mobile force a group something. Must be mortal level.
 *
 * Syntax: mob gforce [victim] [commands]
 */
void do_mpgforce( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "MpGforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    if ( victim == ch )
	return;

    if(IS_IMMORTAL(victim))
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) && !IS_IMMORTAL(vch))
        {
	    interpret( vch, argument );
	}
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something (except ch)
 *
 * Syntax: mob vforce [vnum] [commands]
 */
void do_mpvforce( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "MpVforce - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "MpVforce - Non-number argument vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	&&   ch != victim && victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}


/*
 * Lets the mobile cast spells --
 * Beware: this does only crude checking on the target validity
 * and does not account for mana etc., so you should do all the
 * necessary checking in your mob program before issuing this cmd!
 *
 * Syntax: mob cast [spell] {target}
 */

void do_mpcast( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    OBJ_DATA *obj;
    void *victim = NULL;
    char spell[ MAX_INPUT_LENGTH ],
	 target[ MAX_INPUT_LENGTH ];
    int sn;

    argument = one_argument( argument, spell );
               one_argument( argument, target );

    if ( spell[0] == '\0' )
    {
	bug( "MpCast - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    {
	bug( "MpCast - No such spell from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    vch = get_char_room( ch, NULL, target );
    obj = get_obj_here( ch, NULL, target );
    switch ( skill_table[sn].target )
    {
	default: return;
	case TAR_IGNORE:
	    break;
	case TAR_CHAR_OFFENSIVE:
	    if ( vch == NULL || vch == ch )
		return;
	    victim = ( void * ) vch;
	    break;
	case TAR_CHAR_DEFENSIVE:
	    victim = vch == NULL ? ( void *) ch : (void *) vch; break;
	case TAR_CHAR_SELF:
	    victim = ( void *) ch; break;
	case TAR_OBJ_CHAR_DEF:
	case TAR_OBJ_CHAR_OFF:
	case TAR_OBJ_INV:
	    if ( obj == NULL )
		return;
	    victim = ( void * ) obj;
    }
    (*skill_table[sn].spell_fun)( sn, ch->level, ch, victim,
	skill_table[sn].target );
    return;
}

/*
 * Lets mob cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: mob damage [victim] [min] [max] {kill}
 */
void do_mpdamage( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "MpDamage - Bad syntax from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if( !str_cmp( target, "all" ) )
	fAll = TRUE;
    else if( ( victim = get_char_room( ch, NULL, target ) ) == NULL )
	return;

    if ( is_number( min ) )
	low = atoi( min );
    else
    {
	bug( "MpDamage - Bad damage min vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "MpDamage - Bad damage max vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if ( target[0] != '\0' )
	fKill = TRUE;
    if ( fAll )
    {
	for( victim = ch->in_room->people; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( victim != ch )
    		damage( ch, victim,
		    fKill ?
		    number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
	        TYPE_UNDEFINED, DAM_NONE, FALSE );
	}
    }
    else
    	damage( ch, victim,
	    fKill ?
	    number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
        TYPE_UNDEFINED, DAM_NONE, FALSE );
    return;
}

/*
 * Lets the mobile to remember a target. The target can be referred to
 * with $q and $Q codes in MOBprograms. See also "mob forget".
 *
 * Syntax: mob remember [victim]
 */
void do_mpremember( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	ch->mprog_target = get_char_world( ch, arg );
    else
	bug( "MpRemember: missing argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
}

/*
 * Reverse of "mob remember".
 *
 * Syntax: mob forget
 */
void do_mpforget( CHAR_DATA *ch, char *argument )
{
    ch->mprog_target = NULL;
}

/*
 * Sets a delay for MOBprogram execution. When the delay time expires,
 * the mobile is checked for a MObprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_MOBILE
 *
 * Syntax: mob delay [pulses]
 */
void do_mpdelay( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "MpDelay: invalid arg from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    ch->mprog_delay = atoi( arg );
}

/*
 * Reverse of "mob delay", deactivates the timer.
 *
 * Syntax: mob cancel
 */
void do_mpcancel( CHAR_DATA *ch, char *argument )
{
   ch->mprog_delay = -1;
}
/*
 * Lets the mobile to call another MOBprogram withing a MOBprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: mob call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_mpcall( CHAR_DATA *ch, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void *, int );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "MpCall: missing arguments from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_MPROG ) ) == NULL )
    {
	bug( "MpCall: invalid prog from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( ch, NULL, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( ch, NULL, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( ch, NULL, arg );
program_flow( prg->vnum, prg->code, ch, NULL, NULL, vch, (void *)obj1, (void *)obj2, 1 );
}

/*
 * Forces the mobile to flee.
 *
 * Syntax: mob flee
 *
 */
void do_mpflee( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *was_in;
    EXIT_DATA *pexit;
    int door, attempt;

    if ( ch->fighting != NULL )
	return;

    if ( (was_in = ch->in_room) == NULL )
	return;

    for ( attempt = 0; attempt < 6; attempt++ )
    {
        door = number_door( );
        if ( ( pexit = was_in->exit[door] ) == 0
        ||   pexit->u1.to_room == NULL
        ||   IS_SET(pexit->exit_info, EX_CLOSED)
        || ( IS_NPC(ch)
        &&   IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) ) )
            continue;

        move_char( ch, door, FALSE );
        if ( ch->in_room != was_in )
	    return;
    }
}

/*
 * Lets the mobile to transfer an object. The object must be in the same
 * room with the mobile.
 *
 * Syntax: mob otransfer [item name] [location]
 */
void do_mpotransfer( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "MpOTransfer - Missing argument from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    one_argument( argument, buf );
    if ( ( location = find_location( ch, buf ) ) == NULL )
    {
	bug( "MpOTransfer - No such location from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    if ( (obj = get_obj_here( ch, NULL, arg )) == NULL )
	return;
    if ( obj->carried_by == NULL )
	obj_from_room( obj );
    else
    {
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char( ch, obj );
	obj_from_char( obj );
    }
    obj_to_room( obj, location );
}

/*
 * Lets the mobile to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: mob remove [victim] [object vnum|'all']
 */
void do_mpremove( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "MpRemove: Invalid object from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj );
	     obj_from_char( obj );
	     extract_obj( obj );
	}
    }
}

/*
 * OBJprog section
 */
void do_obj( OBJ_DATA *obj, char *argument )
{
    /*
     * Security check!
     */
    if ( obj->level < MAX_LEVEL )
	return;
    obj_interpret( obj, argument );
}
/*
 * Obj command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void obj_interpret( OBJ_DATA *obj, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;

    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; obj_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == obj_cmd_table[cmd].name[0]
	&&   !str_prefix( command, obj_cmd_table[cmd].name ) )
	{
	    (*obj_cmd_table[cmd].obj_fun) ( obj, argument );
	    tail_chain( );
	    return;
	}
    }
    sprintf( buf, "Obj_interpret: invalid cmd from obj %d: '%s'",
	obj->pIndexData->vnum, command );
    bug( buf, 0 );
}

/*
 * Displays OBJprogram triggers of an object
 *
 * Syntax: opstat [name]
 */
void do_opstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *oprg;
    OBJ_DATA   *obj;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "Opstat what?\n\r", ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg, FALSE ) ) == NULL )
    {
	send_to_char( "No such object.\n\r", ch );
	return;
    }

    sprintf( arg, "Object #%-6d [%s]\n\r",
	obj->pIndexData->vnum, obj->short_descr );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	obj->oprog_delay,
	obj->oprog_target == NULL
		? "No target" : obj->oprog_target->name );
    send_to_char( arg, ch );

    if ( !obj->pIndexData->oprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, oprg = obj->pIndexData->oprogs; oprg != NULL;
	 oprg = oprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( oprg->trig_type ),
	      oprg->vnum,
	      oprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given OBJprogram
 *
 * Syntax: opdump [vnum]
 */
void do_opdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *oprg;

   one_argument( argument, buf );
   if ( ( oprg = get_prog_index( atoi(buf), PRG_OPROG ) ) == NULL )
   {
	send_to_char( "No such OBJprogram.\n\r", ch );
	return;
   }
   page_to_char( oprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: obj gecho [string]
 */
void do_opgecho( OBJ_DATA *obj, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "OpGEcho: missing argument from vnum %d",
	    obj->pIndexData->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Obj echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the mob
 *
 * Syntax: obj zecho [string]
 */
void do_opzecho( OBJ_DATA *obj, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "OpZEcho: missing argument from vnum %d",
	    obj->pIndexData->vnum );
	return;
    }

    if ( obj->in_room == NULL && (obj->carried_by == NULL || obj->carried_by->in_room == NULL) )
	return;

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   ( (obj->in_room && d->character->in_room->area == obj->in_room->area)
	|| (obj->carried_by && d->character->in_room->area == obj->carried_by->in_room->area) ) )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Obj echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the message to everyone in the room other than the mob and victim
 *
 * Syntax: obj echoaround [victim] [string]
 */

void do_opechoaround( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    if ( obj->in_room && obj->in_room->people )
	vch = obj->in_room->people;
    else if ( obj->carried_by && obj->carried_by->in_room )
	vch = obj->carried_by->in_room->people;
    else
	vch = NULL;

    for ( ; vch; vch = vch->next_in_room )
    {
	if ( vch == victim )
	    continue;
	send_to_char( argument, vch );
    }
}

/*
 * Prints the message to only the victim
 *
 * Syntax: obj echoat [victim] [string]
 */
void do_opechoat( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    act( argument, obj->carried_by?obj->carried_by:obj->in_room->people, obj, victim, TO_VICT );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: obj echo [string]
 */
void do_opecho( OBJ_DATA *obj, char *argument )
{
    if ( argument[0] == '\0' )
	return;

    if ( !obj->carried_by && !obj->in_room->people )
	return;

    act( argument, obj->carried_by?obj->carried_by:obj->in_room->people, NULL, NULL, TO_ROOM );
    act( argument, obj->carried_by?obj->carried_by:obj->in_room->people, NULL, NULL, TO_CHAR );
}

/*
 * Lets the object load a mobile.
 *
 * Syntax: obj mload [vnum]
 */
void do_opmload( OBJ_DATA *obj, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( (obj->in_room == NULL
      && (obj->carried_by == NULL || obj->carried_by->in_room == NULL))
      || arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Opmload: bad mob index (%d) from obj %d",
	    vnum, obj->pIndexData->vnum );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, obj->in_room?obj->in_room:obj->carried_by->in_room );
    return;
}

/*
 * Lets the object load another object
 *
 * Syntax: obj oload [vnum] [level]
 */
void do_opoload( OBJ_DATA *obj, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *nobj;
    int             level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
        bug( "Opoload - Bad syntax from vnum %d.",
	    obj->pIndexData->vnum );
        return;
    }

    if ( arg2[0] == '\0' )
    {
	level = obj->level;
    }
    else
    {
	/*
	 * New feature from Alander.
	 */
        if ( !is_number( arg2 ) )
        {
	    bug( "Opoload - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	    return;
        }
	level = atoi( arg2 );
	if ( level < 0 || level > obj->level )
	{
	    bug( "Opoload - Bad level from vnum %d.",
		obj->pIndexData->vnum );
	    return;
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Opoload - Bad vnum arg from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    nobj = create_object( pObjIndex, level );
    obj_to_room( nobj, obj->in_room?obj->in_room:obj->carried_by->in_room );

    return;
}

/*
 * Lets the object purge all other objects and npcs in the room,
 * or purge a specified object or mob in the room. The object cannot
 * purge itself for safety reasons.
 *
 * syntax obj purge {target}
 */
void do_oppurge( OBJ_DATA *obj, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *vobj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by && obj->carried_by->in_room )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;

	for ( ; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim )
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	if ( obj->in_room )
	    vobj = obj->in_room->contents;
	else
	    vobj = obj->carried_by->in_room->contents;

	for ( ; vobj != NULL; vobj = obj_next )
	{
	    obj_next = vobj->next_content;
	    if ( !IS_SET(vobj->extra_flags, ITEM_NOPURGE) && vobj != obj )
		extract_obj( vobj );
	}

	return;
    }

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
    {
	if ( ( vobj = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) )
	{
	    extract_obj( vobj );
	}
	else if ( obj->carried_by && (vobj = get_obj_carry( obj->carried_by, arg, NULL )) != NULL)
	{
	    extract_obj( vobj );
	}
	else if ( obj->carried_by && (vobj = get_obj_wear( obj->carried_by, arg, FALSE )) != NULL )
	{
	    unequip_char( vobj->carried_by, vobj );
	    extract_obj( vobj );
	}
	else
	{
	    bug( "Oppurge - Bad argument from vnum %d.",
		obj->pIndexData->vnum );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Oppurge - Purging a PC from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    extract_char( victim, TRUE );
    return;
}


/*
 * Lets the object goto any location it wishes that is not private.
 *
 * Syntax: obj goto [location]
 */
void do_opgoto( OBJ_DATA *obj, char *argument )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    OBJ_DATA *dobj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "Opgoto - No argument from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( is_number(arg) )
	location = get_room_index( atoi( arg ) );
    else if ( (victim = get_char_world( NULL, arg )) != NULL )
	location = victim->in_room;
    else if ( ( dobj = get_obj_world( NULL, arg, FALSE )) != NULL )
	location = dobj->in_room;
    else
    {
	bug( "Opgoto - No such location from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( obj->in_room != NULL )
	obj_from_room( obj );
    else if ( obj->carried_by != NULL )
	obj_from_char( obj );
    obj_to_room( obj, location );

    return;
}

/*
 * Lets the object transfer people.  The 'all' argument transfers
 *  everyone in the current room to the specified location
 *
 * Syntax: obj transfer [target|'all'] [location]
 */
void do_optransfer( OBJ_DATA *obj, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;
    OBJ_DATA	    *dobj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Optransfer - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;
	for ( ; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( !IS_NPC(victim) )
	    {
		sprintf( buf, "%s %s", victim->name, arg2 );
		do_optransfer( obj, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = obj->in_room?obj->in_room:obj->carried_by->in_room;
    }
    else
    {
	if ( is_number(arg2))
	    location = get_room_index( atoi(arg2) );
	else if ( (victim = get_char_world( NULL, arg2 )) != NULL )
	    location = victim->in_room;
	else if ( ( dobj = get_obj_world( NULL, arg2, FALSE )) != NULL )
	    location = dobj->in_room;
	else
	{
	    bug( "Optransfer - No such location from vnum %d.",
	        obj->pIndexData->vnum );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( NULL, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the object transfer all chars in same group as the victim.
 *
 * Syntax: obj gtransfer [victim] [location]
 */
void do_opgtransfer( OBJ_DATA *obj, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Opgtransfer - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( (who = get_char_room( NULL, (obj->in_room)?obj->in_room:obj->carried_by->in_room, arg1 )) == NULL )
	return;

    if ( obj->in_room && obj->in_room->people )
	victim = obj->in_room->people;
    else if ( obj->carried_by && obj->carried_by->in_room)
	victim = obj->carried_by->in_room->people;
    else
	victim = NULL;

    for ( ; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
	    sprintf( buf, "%s %s", victim->name, arg2 );
	    do_optransfer( obj, buf );
    	}
    }
    return;
}

/*
 * Lets the object force someone to do something. Must be mortal level
 * and the all argument only affects those in the room with the object.
 *
 * Syntax: obj force [victim] [commands]
 */
void do_opforce( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Opforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !obj->in_room && !obj->carried_by )
	return;
    if ( obj->in_room && !obj->in_room->people )
	return;
    if ( obj->carried_by && !obj->carried_by->in_room )
	return;

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( ((obj->in_room && vch->in_room == obj->in_room)
		|| (obj->carried_by && vch->in_room == obj->carried_by->in_room))
		&& get_trust( vch ) < obj->level )
	    {
		interpret( vch, argument );
	    }
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( NULL, (obj->in_room)?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	    return;

	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the object force a group something. Must be mortal level.
 *
 * Syntax: obj gforce [victim] [commands]
 */
void do_opgforce( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "OpGforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) )
        {
	    interpret( vch, argument );
	}
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something
 *
 * Syntax: obj vforce [vnum] [commands]
 */
void do_opvforce( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "OpVforce - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "OpVforce - Non-number argument vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	&& victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}

/*
 * Lets obj cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: obj damage [victim] [min] [max] {kill}
 */
void do_opdamage( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "OpDamage - Bad syntax from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if( !str_cmp( target, "all" ) )
	fAll = TRUE;
    else if( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, target ) ) == NULL )
	return;

    if ( is_number( min ) )
	low = atoi( min );
    else
    {
	bug( "OpDamage - Bad damage min vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "OpDamage - Bad damage max vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if ( target[0] != '\0' )
	fKill = TRUE;
    if ( fAll )
    {
	if ( obj->in_room && obj->in_room->people )
	    victim = obj->in_room->people;
	else if ( obj->carried_by )
	    victim = obj->carried_by->in_room->people;
	else
	    victim = NULL;

	for( ; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( obj->carried_by && victim != obj->carried_by )
    		damage( victim, victim,
		    fKill ?
		    number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
	        TYPE_UNDEFINED, DAM_NONE, FALSE );
	}
    }
    else
    	damage( victim, victim,
	    fKill ?
	    number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
        TYPE_UNDEFINED, DAM_NONE, FALSE );
    return;
}

/*
 * Lets the object to remember a target. The target can be referred to
 * with $q and $Q codes in OBJprograms. See also "obj forget".
 *
 * Syntax: obj remember [victim]
 */
void do_opremember( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	obj->oprog_target = get_char_world( NULL, arg );
    else
	bug( "OpRemember: missing argument from vnum %d.",
		obj->pIndexData->vnum );
}

/*
 * Reverse of "obj remember".
 *
 * Syntax: obj forget
 */
void do_opforget( OBJ_DATA *obj, char *argument )
{
    obj->oprog_target = NULL;
}

/*
 * Sets a delay for OBJprogram execution. When the delay time expires,
 * the object is checked for a OBJprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_TICK
 *
 * Syntax: obj delay [pulses]
 */
void do_opdelay( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "OpDelay: invalid arg from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    obj->oprog_delay = atoi( arg );
}

/*
 * Reverse of "obj delay", deactivates the timer.
 *
 * Syntax: obj cancel
 */
void do_opcancel( OBJ_DATA *obj, char *argument )
{
   obj->oprog_delay = -1;
}
/*
 * Lets the object to call another OBJprogram withing a OBJprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the same room with the
 * mobile.
 *
 * Syntax: obj call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_opcall( OBJ_DATA *obj, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void *, int );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "OpCall: missing arguments from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_OPROG ) ) == NULL )
    {
	bug( "OpCall: invalid prog from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg );
    program_flow( prg->vnum, prg->code, NULL, obj, NULL, vch, (void *)obj1, (void *)obj2, 1 );
}

/*
 * Lets the object to transfer an object. The object must be in the same
 * room with the object.
 *
 * Syntax: obj otransfer [item name] [location]
 */
void do_opotransfer( OBJ_DATA *obj, char *argument )
{
    OBJ_DATA *obj1, *dobj;
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "OpOTransfer - Missing argument from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    one_argument( argument, buf );
    if ( is_number( buf ) )
	location = get_room_index( atoi(buf) );
    else if ( (victim = get_char_world( NULL, buf )) != NULL )
	location = victim->in_room;
    else if ( ( dobj = get_obj_world( NULL, arg, FALSE )) != NULL )
	location = dobj->in_room;
    else
    {
	bug( "OpOTransfer - No such location from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }

    if ( (obj1 = get_obj_here( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg )) == NULL )
	return;
    if ( obj1->carried_by == NULL )
	obj_from_room( obj1 );
    else
    {
	if ( obj1->wear_loc != WEAR_NONE )
	    unequip_char( obj1->carried_by, obj1 );
	obj_from_char( obj1 );
    }
    obj_to_room( obj1, location );
}

/*
 * Lets the object to strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: obj remove [victim] [object vnum|'all']
 */
void do_opremove( OBJ_DATA *obj, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj1, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, obj->in_room?obj->in_room:obj->carried_by->in_room, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "OpRemove: Invalid object from vnum %d.",
		obj->pIndexData->vnum );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj1 = victim->carrying; obj1; obj1 = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj1->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj1 );
	     obj_from_char( obj1 );
	     extract_obj( obj1 );
	}
    }
}

/*
 * ROOMprog section
 */

void do_room( ROOM_INDEX_DATA *room, char *argument )
{
    room_interpret( room, argument );
}

/*
 * Room command interpreter. Implemented separately for security and speed
 * reasons. A trivial hack of interpret()
 */
void room_interpret( ROOM_INDEX_DATA *room, char *argument )
{
    char buf[MAX_STRING_LENGTH], command[MAX_INPUT_LENGTH];
    int cmd;

    argument = one_argument( argument, command );

    /*
     * Look for command in command table.
     */
    for ( cmd = 0; room_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == room_cmd_table[cmd].name[0]
	&&   !str_prefix( command, room_cmd_table[cmd].name ) )
	{
	    (*room_cmd_table[cmd].room_fun) ( room, argument );
	    tail_chain( );
	    return;
	}
    }
    sprintf( buf, "Room_interpret: invalid cmd from room %d: '%s'",
	room->vnum, command );
    bug( buf, 0 );
}

/*
 * Displays ROOMprogram triggers of a room
 *
 * Syntax: rpstat [name]
 */
void do_rpstat( CHAR_DATA *ch, char *argument )
{
    char        arg[ MAX_STRING_LENGTH  ];
    PROG_LIST  *rprg;
    ROOM_INDEX_DATA   *room;
    int i;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
	room = ch->in_room;
    else if ( !is_number( arg ) )
    {
	send_to_char( "You must provide a number.\n\r", ch );
	return;
    }
    else if ( ( room = get_room_index( atoi(arg) ) ) == NULL )
    {
	send_to_char( "No such room.\n\r", ch );
	return;
    }

    sprintf( arg, "Room #%-6d [%s]\n\r",
	room->vnum, room->name );
    send_to_char( arg, ch );

    sprintf( arg, "Delay   %-6d [%s]\n\r",
	room->rprog_delay,
	room->rprog_target == NULL
		? "No target" : room->rprog_target->name );
    send_to_char( arg, ch );

    if ( !room->rprog_flags )
    {
	send_to_char( "[No programs set]\n\r", ch);
	return;
    }

    for ( i = 0, rprg = room->rprogs; rprg != NULL;
	 rprg = rprg->next )

    {
	sprintf( arg, "[%2d] Trigger [%-8s] Program [%4d] Phrase [%s]\n\r",
	      ++i,
	      prog_type_to_name( rprg->trig_type ),
	      rprg->vnum,
	      rprg->trig_phrase );
	send_to_char( arg, ch );
    }

    return;

}

/*
 * Displays the source code of a given ROOMprogram
 *
 * Syntax: rpdump [vnum]
 */
void do_rpdump( CHAR_DATA *ch, char *argument )
{
   char buf[ MAX_INPUT_LENGTH ];
   PROG_CODE *rprg;

   one_argument( argument, buf );
   if ( ( rprg = get_prog_index( atoi(buf), PRG_RPROG ) ) == NULL )
   {
	send_to_char( "No such ROOMprogram.\n\r", ch );
	return;
   }
   page_to_char( rprg->code, ch );
}

/*
 * Prints the argument to all active players in the game
 *
 * Syntax: room gecho [string]
 */
void do_rpgecho( ROOM_INDEX_DATA *room, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "RpGEcho: missing argument from vnum %d",
	    room->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Room echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all players in the same area as the room
 *
 * Syntax: room zecho [string]
 */
void do_rpzecho( ROOM_INDEX_DATA *room, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
	bug( "RpZEcho: missing argument from vnum %d",
	    room->vnum );
	return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
	if ( d->connected == CON_PLAYING
	&&   d->character->in_room != NULL
	&&   d->character->in_room->area == room->area )
 	{
	    if ( IS_IMMORTAL(d->character) )
		send_to_char( "Room echo> ", d->character );
	    send_to_char( argument, d->character );
	    send_to_char( "\n\r", d->character );
	}
    }
}

/*
 * Prints the argument to all the rooms aroud the room
 *
 * Syntax: room asound [string]
 */
void do_rpasound( ROOM_INDEX_DATA *room, char *argument )
{
    int              door;

    if ( argument[0] == '\0' )
	return;

    for ( door = 0; door < 6; door++ )
    {
    	EXIT_DATA       *pexit;

      	if ( ( pexit = room->exit[door] ) != NULL
	  &&   pexit->u1.to_room != NULL
	  &&   pexit->u1.to_room != room
	  &&   pexit->u1.to_room->people != NULL )
      	{
	    act( argument, pexit->u1.to_room->people, NULL, NULL, TO_ROOM );
	    act( argument, pexit->u1.to_room->people, NULL, NULL, TO_CHAR );
	}
    }
    return;

}

/*
 * Prints the message to everyone in the room other than the victim
 *
 * Syntax: room echoaround [victim] [string]
 */

void do_rpechoaround( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
	return;

    if ( ( victim=get_char_room( NULL, room, arg ) ) == NULL )
	return;

    act( argument, victim, NULL, victim, TO_NOTVICT );
}

/*
 * Prints the message to only the victim
 *
 * Syntax: room echoat [victim] [string]
 */
void do_rpechoat( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
	return;

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    act( argument, victim, NULL, NULL, TO_CHAR );
}

/*
 * Prints the message to the room at large
 *
 * Syntax: rpecho [string]
 */
void do_rpecho( ROOM_INDEX_DATA *room, char *argument )
{
    if ( argument[0] == '\0' )
	return;

    if ( !room->people )
	return;

    act( argument, room->people, NULL, NULL, TO_ROOM );
    act( argument, room->people, NULL, NULL, TO_CHAR );
}

/*
 * Lets the room load a mobile.
 *
 * Syntax: room mload [vnum]
 */
void do_rpmload( ROOM_INDEX_DATA *room, char *argument )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;
    int vnum;

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
	return;

    vnum = atoi(arg);
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	sprintf( arg, "Rpmload: bad mob index (%d) from room %d",
	    vnum, room->vnum );
	bug( arg, 0 );
	return;
    }
    victim = create_mobile( pMobIndex );
    char_to_room( victim, room );
    return;
}

/*
 * Lets the room load an object
 *
 * Syntax: room oload [vnum] [level]
 */
void do_rpoload( ROOM_INDEX_DATA *room, char *argument )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
               one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) || arg2[0] == '\0'
      || !is_number( arg2 ) )
    {
        bug( "Rpoload - Bad syntax from vnum %d.",
	    room->vnum );
        return;
    }

    level = atoi( arg2 );
    if ( level < 0 || level > LEVEL_IMMORTAL )
    {
	bug( "Rpoload - Bad level from vnum %d.", room->vnum );
	return;
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	bug( "Rpoload - Bad vnum arg from vnum %d.", room->vnum );
	return;
    }

    obj = create_object( pObjIndex, level );
    obj_to_room( obj, room );

    return;
}

/*
 * Lets the room purge all objects npcs in the room,
 * or purge a specified object or mob in the room.
 *
 * syntax room purge {target}
 */
void do_rppurge( ROOM_INDEX_DATA *room, char *argument )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        CHAR_DATA *vnext;
        OBJ_DATA  *obj_next;

	for ( victim = room->people; victim != NULL; victim = vnext )
	{
	    vnext = victim->next_in_room;
	    if ( IS_NPC( victim )
	    &&   !IS_SET(victim->act, ACT_NOPURGE) )
		extract_char( victim, TRUE );
	}

	for ( obj = room->contents; obj != NULL; obj = obj_next )
	{
	    obj_next = obj->next_content;
	    if ( !IS_SET(obj->extra_flags, ITEM_NOPURGE) )
		extract_obj( obj );
	}

	return;
    }

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
    {
	if ( ( obj = get_obj_here( NULL, room, arg ) ) )
	{
	    extract_obj( obj );
	}
	else
	{
	    bug( "Rppurge - Bad argument from vnum %d.", room->vnum );
	}
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	bug( "Rppurge - Purging a PC from vnum %d.", room->vnum );
	return;
    }
    extract_char( victim, TRUE );
    return;
}

/*
 * Lets the room transfer people.  The 'all' argument transfers
 *  everyone in the room to the specified location
 *
 * Syntax: room transfer [target|'all'] [location]
 */
void do_rptransfer( ROOM_INDEX_DATA *room, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;
    OBJ_DATA	    *tobj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Rptransfer - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
	CHAR_DATA *victim_next;

	for ( victim = room->people; victim != NULL; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
	    if ( !IS_NPC(victim) )
	    {
		sprintf( buf, "%s %s", victim->name, arg2 );
		do_rptransfer( room, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = room;
    }
    else
    {
	if ( is_number(arg2))
	    location = get_room_index( atoi(arg2) );
	else if ( (victim = get_char_world( NULL, arg2 )) != NULL )
	    location = victim->in_room;
	else if ( ( tobj = get_obj_world( NULL, arg2, FALSE )) != NULL )
	    location = tobj->in_room;
	else
	{
	    bug( "Rptransfer - No such location from vnum %d.", room->vnum );
	    return;
	}

	if ( room_is_private( location ) )
	    return;
    }

    if ( ( victim = get_char_world( NULL, arg1 ) ) == NULL )
	return;

    if ( victim->in_room == NULL )
	return;

    if ( victim->fighting != NULL )
	stop_fighting( victim, TRUE );
    char_from_room( victim );
    char_to_room( victim, location );
    do_look( victim, "auto" );

    return;
}

/*
 * Lets the room transfer all chars in same group as the victim.
 *
 * Syntax: room gtransfer [victim] [location]
 */
void do_rpgtransfer( ROOM_INDEX_DATA *room, char *argument )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char	     buf[MAX_STRING_LENGTH];
    CHAR_DATA       *who, *victim, *victim_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	bug( "Rpgtransfer - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( (who = get_char_room( NULL, room, arg1 )) == NULL )
	return;

    for ( victim = room->people; victim; victim = victim_next )
    {
    	victim_next = victim->next_in_room;
    	if( is_same_group( who,victim ) )
    	{
	    sprintf( buf, "%s %s", victim->name, arg2 );
	    do_rptransfer( room, buf );
    	}
    }
    return;
}

/*
 * Lets the room force someone to do something. Must be mortal level
 * and the all argument only affects those in the room.
 *
 * Syntax: room force [victim] [commands]
 */
void do_rpforce( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "Rpforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

	for ( vch = char_list; vch != NULL; vch = vch_next )
	{
	    vch_next = vch->next;

	    if ( vch->in_room == room && !IS_IMMORTAL( vch ) )
		interpret( vch, argument );
	}
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	    return;

	interpret( victim, argument );
    }

    return;
}

/*
 * Lets the room force a group something. Must be mortal level.
 *
 * Syntax: room gforce [victim] [commands]
 */
void do_rpgforce( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim, *vch, *vch_next;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "RpGforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    for ( vch = victim->in_room->people; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next_in_room;

	if ( is_same_group(victim,vch) )
	    interpret( vch, argument );
    }
    return;
}

/*
 * Forces all mobiles of certain vnum to do something
 *
 * Syntax: room vforce [vnum] [commands]
 */
void do_rpvforce( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim, *victim_next;
    char arg[ MAX_INPUT_LENGTH ];
    int vnum;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	bug( "RpVforce - Bad syntax from vnum %d.", room->vnum );
	return;
    }

    if ( !is_number( arg ) )
    {
	bug( "RpVforce - Non-number argument vnum %d.", room->vnum );
	return;
    }

    vnum = atoi( arg );

    for ( victim = char_list; victim; victim = victim_next )
    {
	victim_next = victim->next;
	if ( IS_NPC(victim) && victim->pIndexData->vnum == vnum
	  && victim->fighting == NULL )
	    interpret( victim, argument );
    }
    return;
}

/*
 * Lets room cause unconditional damage to someone. Nasty, use with caution.
 * Also, this is silent, you must show your own damage message...
 *
 * Syntax: room damage [victim] [min] [max] {kill}
 */
void do_rpdamage( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim = NULL, *victim_next;
    char target[ MAX_INPUT_LENGTH ],
	 min[ MAX_INPUT_LENGTH ],
	 max[ MAX_INPUT_LENGTH ];
    int low, high;
    bool fAll = FALSE, fKill = FALSE;

    argument = one_argument( argument, target );
    argument = one_argument( argument, min );
    argument = one_argument( argument, max );

    if ( target[0] == '\0' )
    {
	bug( "RpDamage - Bad syntax from vnum %d.", room->vnum );
	return;
    }
    if( !str_cmp( target, "all" ) )
	fAll = TRUE;
    else if( ( victim = get_char_room( NULL, room, target ) ) == NULL )
	return;

    if ( is_number( min ) )
	low = atoi( min );
    else
    {
	bug( "RpDamage - Bad damage min vnum %d.", room->vnum );
	return;
    }
    if ( is_number( max ) )
	high = atoi( max );
    else
    {
	bug( "RpDamage - Bad damage max vnum %d.", room->vnum );
	return;
    }
    one_argument( argument, target );

    /*
     * If kill parameter is omitted, this command is "safe" and will not
     * kill the victim.
     */

    if ( target[0] != '\0' )
	fKill = TRUE;
    if ( fAll )
    {
	for( victim = room->people; victim; victim = victim_next )
	{
	    victim_next = victim->next_in_room;
    	    damage( victim, victim,
	      fKill ?
	      number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
	      TYPE_UNDEFINED, DAM_NONE, FALSE );
	}
    }
    else
    	damage( victim, victim,
	  fKill ?
	  number_range(low,high) : UMIN(victim->hit,number_range(low,high)),
          TYPE_UNDEFINED, DAM_NONE, FALSE );
    return;
}

/*
 * Lets the room remember a target. The target can be referred to
 * with $q and $Q codes in ROOMprograms. See also "room forget".
 *
 * Syntax: room remember [victim]
 */
void do_rpremember( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    one_argument( argument, arg );
    if ( arg[0] != '\0' )
	room->rprog_target = get_char_world( NULL, arg );
    else
	bug( "RpRemember: missing argument from vnum %d.", room->vnum );
}

/*
 * Reverse of "room remember".
 *
 * Syntax: room forget
 */
void do_rpforget( ROOM_INDEX_DATA *room, char *argument )
{
    room->rprog_target = NULL;
}

/*
 * Sets a delay for ROOMprogram execution. When the delay time expires,
 * the room is checked for a ROOMprogram with DELAY trigger, and if
 * one is found, it is executed. Delay is counted in PULSE_AREA
 *
 * Syntax: room delay [pulses]
 */
void do_rpdelay( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];

    one_argument( argument, arg );
    if ( !is_number( arg ) )
    {
	bug( "RpDelay: invalid arg from vnum %d.", room->vnum );
	return;
    }
    room->rprog_delay = atoi( arg );
}

/*
 * Reverse of "room delay", deactivates the timer.
 *
 * Syntax: room cancel
 */
void do_rpcancel( ROOM_INDEX_DATA *room, char *argument )
{
   room->rprog_delay = -1;
}
/*
 * Lets the room call another ROOMprogram within a ROOMprogram.
 * This is a crude way to implement subroutines/functions. Beware of
 * nested loops and unwanted triggerings... Stack usage might be a problem.
 * Characters and objects referred to must be in the room.
 *
 * Syntax: room call [vnum] [victim|'null'] [object1|'null'] [object2|'null']
 *
 */
void do_rpcall( ROOM_INDEX_DATA *room, char *argument )
{
    char arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *vch;
    OBJ_DATA *obj1, *obj2;
    PROG_CODE *prg;
    extern void program_flow( sh_int, char *, CHAR_DATA *, OBJ_DATA *, ROOM_INDEX_DATA *, CHAR_DATA *, const void *, const void *, int );

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "RpCall: missing arguments from vnum %d.", room->vnum );
	return;
    }
    if ( ( prg = get_prog_index( atoi(arg), PRG_RPROG ) ) == NULL )
    {
	bug( "RpCall: invalid prog from vnum %d.", room->vnum );
	return;
    }
    vch = NULL;
    obj1 = obj2 = NULL;
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
        vch = get_char_room( NULL, room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj1 = get_obj_here( NULL, room, arg );
    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    	obj2 = get_obj_here( NULL, room, arg );
    program_flow( prg->vnum, prg->code, NULL, NULL, room, vch, (void *)obj1, (void *)obj2, 1 );
}

/*
 * Lets the room transfer an object. The object must be in the room.
 *
 * Syntax: room otransfer [item name] [location]
 */
void do_rpotransfer( ROOM_INDEX_DATA *room, char *argument )
{
    OBJ_DATA *obj, *tobj;
    ROOM_INDEX_DATA *location;
    char arg[ MAX_INPUT_LENGTH ];
    char buf[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	bug( "RpOTransfer - Missing argument from vnum %d.", room->vnum );
	return;
    }
    one_argument( argument, buf );

    if ( is_number( buf ) )
	location = get_room_index( atoi(buf) );
    else if ( (victim = get_char_world( NULL, buf )) != NULL )
	location = victim->in_room;
    else if ( ( tobj = get_obj_world( NULL, arg, FALSE )) != NULL )
	location = tobj->in_room;
    else
    {
	bug( "RpOTransfer - No such location from vnum %d.", room->vnum );
	return;
    }

    if ( (obj = get_obj_here( NULL, room, arg )) == NULL )
	return;

    if ( obj->carried_by == NULL )
	obj_from_room( obj );
    else
    {
	if ( obj->wear_loc != WEAR_NONE )
	    unequip_char( obj->carried_by, obj );
	obj_from_char( obj );
    }
    obj_to_room( obj, location );
}

/*
 * Lets the room strip an object or all objects from the victim.
 * Useful for removing e.g. quest objects from a character.
 *
 * Syntax: room remove [victim] [object vnum|'all']
 */
void do_rpremove( ROOM_INDEX_DATA *room, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj, *obj_next;
    sh_int vnum = 0;
    bool fAll = FALSE;
    char arg[ MAX_INPUT_LENGTH ];

    argument = one_argument( argument, arg );
    if ( ( victim = get_char_room( NULL, room, arg ) ) == NULL )
	return;

    one_argument( argument, arg );
    if ( !str_cmp( arg, "all" ) )
	fAll = TRUE;
    else if ( !is_number( arg ) )
    {
	bug ( "RpRemove: Invalid object from vnum %d.", room->vnum );
	return;
    }
    else
	vnum = atoi( arg );

    for ( obj = victim->carrying; obj; obj = obj_next )
    {
	obj_next = obj->next_content;
	if ( fAll || obj->pIndexData->vnum == vnum )
	{
	     unequip_char( victim, obj );
	     obj_from_char( obj );
	     extract_obj( obj );
	}
    }
}

void do_mpqpoint(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    char log_buf[MAX_STRING_LENGTH];
    int points;


    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if((victim = get_char_room(ch, NULL, arg1)) == NULL)
        return;

    if(IS_NPC(victim))
        return;

    if(!is_number(arg2))
    {
        bug ( "MpQpoint: Invalid QP amount from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
        return;
    }
    points = atoi(arg2);

    if(points > 30000 || points < 0)
    {
        bug ( "MpQpoint: Invalid QP amount from vnum %d.",
		IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
        return;
    }

    if(victim->qpoints+points > MAX_QPOINTS)
        points = MAX_QPOINTS - victim->qpoints;



    if(points > 0)
    {
        sprintf(buf, "You have been rewarded {Y%d {xquest points!\n\r", points);
        send_to_char(buf,victim);
    }

    victim->qpoints += points;
		victim->totalqpoints += points;
    sprintf(log_buf, "%s was awarded %d Qpoints by mob %d (Now has %d)",victim->name, points, IS_NPC(ch) ? ch->pIndexData->vnum : 0, victim->qpoints);
    wiznet(log_buf,ch,NULL,WIZ_SECURE,0,0);
    log_string(log_buf);
    return;
}


/* quest command used to handle quest data */
void do_mpquest( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *victim;
	char arg[3][MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int i;

	for(i = 0;i < 3;i++)
		argument = one_argument(argument,arg[i]);

	if(arg[0][0] == '\0')
	{
		bug("MpQuest: argument is not valid from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
	}

	if ( ( victim = get_char_room( ch, NULL, arg[1] ) ) == NULL)
	{
		bug("MpQuest: victim not valid from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
	}

	if(!str_cmp(arg[0],"define"))
	{
		if(!is_number(arg[2]))
		{
			bug("MpQuest: argument is not a number from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
			return;
		}
		ch->quest = atoi(arg[2]);
		return;
	}

	if(ch->quest == 0)
	{
		bug("MpQuest: quest id not defined from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
		return;
	}

	if(IS_NPC(victim))
	{
		bug("MpQuest: Victim is a mob.",0);
		return;
	}

	if(!str_cmp(arg[0],"assign"))
	{
		if(quest_status(victim,ch->quest))
		{
			bug("MpQuest: Player already assigned quest.",0);
			return;
		}

		if(!add_quest(victim,ch->quest))
		{
			bug("MpQuest: unable to assign quest from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
			return;
		}

        send_to_char("    {C[[--          {GYou have been assigned a new Quest.           {C--]]{x\n\r", victim);
		return;
	}
	if(!str_cmp(arg[0],"step"))
	{
		if(!quest_status(victim,ch->quest)) return;

		if(!is_number(arg[2])) return;

		for(i = 0;i < MAX_QUEST;i++)
		{
			if(victim->pcdata->quest_id[i] == ch->quest)
			{
				victim->pcdata->quest_step[i] = atoi(arg[2]);
                if (atoi(arg[2]) == 100)
                    send_to_char("    {C[[--              {GYou have completed a Quest!               {C--]]{x\n\r", victim);
                else
                    send_to_char("    {C[[--      {GYour Quest Log has been updated.  Type {yqlog{G.      {C--]]{x\n\r", victim);
				return;
			}
		}
		return;
	}
	if(!str_cmp(arg[0],"string"))
	{
		for(i = 0;i < MAX_QUEST;i++)
		{
			if(victim->pcdata->quest_id[i] == ch->quest)
			{
				sprintf(buf,"%s %s",capitalize(arg[2]),argument);
				victim->pcdata->quest_string[i] = str_dup(buf);
				return;
			}
		}
		return;
	}

	if(!str_cmp(arg[0],"name"))
	{
		for(i = 0;i < MAX_QUEST;i++)
		{
			if(victim->pcdata->quest_id[i] == ch->quest)
			{
				sprintf(buf,"%s %s",capitalize(arg[2]),argument);
				victim->pcdata->quest_name[i] = str_dup(buf);
				return;
			}
		}
		return;
	}

	if(!str_cmp(arg[0],"complete"))
	{
		for(i = 0;i < MAX_QUEST;i++)
		{
			if(victim->pcdata->quest_id[i] == ch->quest)
			{
				victim->pcdata->quest_step[i] = 100;
				victim->pcdata->quest_string[i] = str_dup("You've completed this quest.");
				sprintf(buf,"{R*{YYou've completed {g%s! {YCongratulations!{R*{x\n\r",victim->pcdata->quest_name[i]);
				send_to_char(buf,victim);
				return;
			}
		}
		return;
	}

	bug("MpQuest: invalid command from vnum %d", IS_NPC(ch) ? ch->pIndexData->vnum : 0 );
	return;
}
