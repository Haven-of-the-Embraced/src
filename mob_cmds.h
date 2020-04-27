/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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


struct	mob_cmd_type
{
    char * const	name;
    DO_FUN *		do_fun;
};

struct obj_cmd_type
{
    char * const	name;
    OBJ_FUN *		obj_fun;
};

struct room_cmd_type
{
    char * const	name;
    ROOM_FUN *		room_fun;
};

/* the command table itself */
extern	const	struct	mob_cmd_type	mob_cmd_table	[];

/*
 * MOBcommand functions.
 * Defined in mob_cmds.c
 */
DECLARE_DO_FUN(	do_mpasound	);
DECLARE_DO_FUN(	do_mpgecho	);
DECLARE_DO_FUN(	do_mpzecho	);
DECLARE_DO_FUN(	do_mpkill	);
DECLARE_DO_FUN(	do_mpassist	);
DECLARE_DO_FUN(	do_mpjunk	);
DECLARE_DO_FUN(	do_mpechoaround	);
DECLARE_DO_FUN(	do_mpecho	);
DECLARE_DO_FUN(	do_mpechoat	);
DECLARE_DO_FUN(	do_mpmload	);
DECLARE_DO_FUN(	do_mpoload	);
DECLARE_DO_FUN(	do_mppurge	);
DECLARE_DO_FUN(	do_mpgoto	);
DECLARE_DO_FUN(	do_mpat		);
DECLARE_DO_FUN(	do_mptransfer	);
DECLARE_DO_FUN(	do_mpgtransfer	);
DECLARE_DO_FUN(	do_mpforce	);
DECLARE_DO_FUN(	do_mpgforce	);
DECLARE_DO_FUN(	do_mpvforce	);
DECLARE_DO_FUN(	do_mpcast	);
DECLARE_DO_FUN(	do_mpdamage	);
DECLARE_DO_FUN(	do_mpremember	);
DECLARE_DO_FUN(	do_mpforget	);
DECLARE_DO_FUN(	do_mpdelay	);
DECLARE_DO_FUN(	do_mpcancel	);
DECLARE_DO_FUN(	do_mpcall	);
DECLARE_DO_FUN(	do_mpflee	);
DECLARE_DO_FUN(	do_mpotransfer	);
DECLARE_DO_FUN(	do_mpremove	);
DECLARE_DO_FUN( do_mpqpoint     );
DECLARE_DO_FUN(	do_mpquest	);

/*
 * OBJcommand functions
 * Defined in mob_cmds.c
 */
DECLARE_OBJ_FUN( do_opgecho	 );
DECLARE_OBJ_FUN( do_opzecho	 );
DECLARE_OBJ_FUN( do_opecho	 );
DECLARE_OBJ_FUN( do_opechoaround );
DECLARE_OBJ_FUN( do_opechoat	 );
DECLARE_OBJ_FUN( do_opmload	 );
DECLARE_OBJ_FUN( do_opoload	 );
DECLARE_OBJ_FUN( do_oppurge	 );
DECLARE_OBJ_FUN( do_opgoto	 );
DECLARE_OBJ_FUN( do_optransfer	 );
DECLARE_OBJ_FUN( do_opgtransfer	 );
DECLARE_OBJ_FUN( do_opotransfer	 );
DECLARE_OBJ_FUN( do_opforce	 );
DECLARE_OBJ_FUN( do_opgforce	 );
DECLARE_OBJ_FUN( do_opvforce	 );
DECLARE_OBJ_FUN( do_opdamage	 );
DECLARE_OBJ_FUN( do_opremember	 );
DECLARE_OBJ_FUN( do_opforget	 );
DECLARE_OBJ_FUN( do_opdelay	 );
DECLARE_OBJ_FUN( do_opcancel	 );
DECLARE_OBJ_FUN( do_opcall	 );
DECLARE_OBJ_FUN( do_opremove	 );

/*
 * ROOMcommand functions
 * Defined in mob_cmds.c
 */
DECLARE_ROOM_FUN( do_rpasound	 );
DECLARE_ROOM_FUN( do_rpgecho	 );
DECLARE_ROOM_FUN( do_rpzecho	 );
DECLARE_ROOM_FUN( do_rpecho	 );
DECLARE_ROOM_FUN( do_rpechoaround);
DECLARE_ROOM_FUN( do_rpechoat	 );
DECLARE_ROOM_FUN( do_rpmload	 );
DECLARE_ROOM_FUN( do_rpoload	 );
DECLARE_ROOM_FUN( do_rppurge	 );
DECLARE_ROOM_FUN( do_rptransfer	 );
DECLARE_ROOM_FUN( do_rpgtransfer );
DECLARE_ROOM_FUN( do_rpotransfer );
DECLARE_ROOM_FUN( do_rpforce	 );
DECLARE_ROOM_FUN( do_rpgforce	 );
DECLARE_ROOM_FUN( do_rpvforce	 );
DECLARE_ROOM_FUN( do_rpdamage	 );
DECLARE_ROOM_FUN( do_rpremember	 );
DECLARE_ROOM_FUN( do_rpforget	 );
DECLARE_ROOM_FUN( do_rpdelay	 );
DECLARE_ROOM_FUN( do_rpcancel	 );
DECLARE_ROOM_FUN( do_rpcall	 );
DECLARE_ROOM_FUN( do_rpremove	 );
