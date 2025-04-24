/***************************************************************************
 *  File: olc_act.c                                                        *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/



#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "recycle.h"
#include "lookup.h"

#define HEDIT( fun )            bool fun( CHAR_DATA *ch, char *argument )
#define CEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )


char * prog_type_to_name ( int type );

#define ALT_FLAGVALUE_SET( _blargh, _table, _arg )      \
    {                           \
        int blah = flag_value( _table, _arg );      \
        _blargh = (blah == NO_FLAG) ? 0 : blah;     \
    }

#define ALT_FLAGVALUE_TOGGLE( _blargh, _table, _arg )       \
    {                           \
        int blah = flag_value( _table, _arg );      \
        _blargh ^= (blah == NO_FLAG) ? 0 : blah;    \
    }

/* Return TRUE if area changed, FALSE if not. */
#define REDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define OEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define MEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )
#define AEDIT( fun )        bool fun( CHAR_DATA *ch, char *argument )



struct olc_help_type
{
    char *command;
    const void *structure;
    char *desc;
};



bool show_version( CHAR_DATA *ch, char *argument )
{
    send_to_char( VERSION, ch );
    send_to_char( "\n\r", ch );
    send_to_char( AUTHOR, ch );
    send_to_char( "\n\r", ch );
    send_to_char( DATE, ch );
    send_to_char( "\n\r", ch );
    send_to_char( CREDITS, ch );
    send_to_char( "\n\r", ch );

    return FALSE;
}

/*
 * This table contains help commands and a brief description of each.
 * ------------------------------------------------------------------
 */
const struct olc_help_type help_table[] =
{
    {   "area",     area_flags,  "Area attributes."      },
    {   "room",     room_flags,  "Room attributes."      },
    {   "sector",   sector_flags,    "Sector types, terrain."    },
    {   "exit",     exit_flags,  "Exit types."           },
    {   "type",     type_flags,  "Types of objects."         },
    {   "clantype", clantype_flags, "Types of clans." },
    {   "extra",    extra_flags,     "Object attributes."        },
    {   "wear",     wear_flags,  "Where to wear object."     },
    {   "spec",     spec_table,  "Available special programs."   },
    {   "sex",      sex_flags,   "Sexes."            },
    {   "act",      act_flags,   "Mobile attributes."        },
    {   "act2",     act2_flags,  "Mobile extended attributes."  },
    {   "affect",   affect_flags,    "Mobile affects."       },
    {   "aff2", affect2_flags,   "Mobile affects2."      },
    {   "wear-loc", wear_loc_flags,  "Where mobile wears object."    },
    {   "spells",   skill_table,     "Names of current spells."      },
    {   "container",    container_flags, "Container status."         },
    {   "attrflags",    attr_flags,      "Mob template attribute modifiers." },
    {   "abilflags",    abil_flags,     "Mob template ability modifiers." },

/* ROM specific bits: */

    {   "armor",    ac_type,     "Ac for different attacks."     },
    {   "apply",    apply_flags,     "Apply flags"           },
    {   "form",     form_flags,  "Mobile body form."             },
    {   "part",     part_flags,  "Mobile body parts."        },
    {   "imm",      imm_flags,   "Mobile immunity."      },
    {   "res",      res_flags,   "Mobile resistance."            },
    {   "vuln",     vuln_flags,  "Mobile vulnerability."     },
    {   "off",      off_flags,   "Mobile offensive behaviour."   },
    {   "size",     size_flags,  "Mobile / Object size."          },
    {   "position",     position_flags,  "Mobile positions."             },
    {   "wclass",       weapon_class,    "Weapon class."                 },
    {   "wtype",        weapon_type2,    "Special weapon type."          },
    {   "portal",   portal_flags,    "Portal types."         },
    {   "furniture",    furniture_flags, "Furniture types."      },
    {   "liquid",   liq_table,   "Liquid types."         },
    {   "apptype",  apply_types,     "Apply types."          },
    {   "weapon",   attack_table,    "Weapon types."         },
    {   "mprog",    mprog_flags,     "MobProgram flags."         },
    {	"oprog",	oprog_flags,	 "ObjProgram flags."		 },
    {	"rprog",	rprog_flags,	 "RoomProgram flags."		 },
    {   "tool",     tool_table,     "Tool types."        },
        {"clan",    clan_flags, "Clan flags."},
    {   "crafted",      crafted_table,      "Crafted item types."        },
    {   "trap", trap_flags,  "Trap flags."       },
    {   "light", light_flags,   "Special Light Flags."},
    {   NULL,       NULL,        NULL                }
};



/*****************************************************************************
 Name:      show_flag_cmds
 Purpose:   Displays settable flags and stats.
 Called by: show_help(olc_act.c).
 ****************************************************************************/
void show_flag_cmds( CHAR_DATA *ch, const struct flag_type *flag_table )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  flag;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
    if ( flag_table[flag].settable )
    {
        sprintf( buf, "%-19.18s", flag_table[flag].name );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
        strcat( buf1, "\n\r" );
    }
    }

    if ( col % 4 != 0 )
    strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}


/*****************************************************************************
 Name:      show_skill_cmds
 Purpose:   Displays all skill functions.
        Does remove those damn immortal commands from the list.
        Could be improved by:
        (1) Adding a check for a particular class.
        (2) Adding a check for a level range.
 Called by: show_help(olc_act.c).
 ****************************************************************************/
void show_skill_cmds( CHAR_DATA *ch, int tar )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH*2 ];
    int  sn;
    int  col;

    buf1[0] = '\0';
    col = 0;
    for (sn = 0; sn < MAX_SKILL; sn++)
    {
    if ( !skill_table[sn].name )
        break;

    if ( !str_cmp( skill_table[sn].name, "reserved" )
      || skill_table[sn].spell_fun == spell_null )
        continue;

    if ( tar == -1 || skill_table[sn].target == tar )
    {
        sprintf( buf, "%-19.18s", skill_table[sn].name );
        strcat( buf1, buf );
        if ( ++col % 4 == 0 )
        strcat( buf1, "\n\r" );
    }
    }

    if ( col % 4 != 0 )
    strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:      show_spec_cmds
 Purpose:   Displays settable special functions.
 Called by: show_help(olc_act.c).
 ****************************************************************************/
void show_spec_cmds( CHAR_DATA *ch )
{
    char buf  [ MAX_STRING_LENGTH ];
    char buf1 [ MAX_STRING_LENGTH ];
    int  spec;
    int  col;

    buf1[0] = '\0';
    col = 0;
    send_to_char( "Preceed special functions with 'spec_'\n\r\n\r", ch );
    for (spec = 0; spec_table[spec].function != NULL; spec++)
    {
    sprintf( buf, "%-19.18s", &spec_table[spec].name[5] );
    strcat( buf1, buf );
    if ( ++col % 4 == 0 )
        strcat( buf1, "\n\r" );
    }

    if ( col % 4 != 0 )
    strcat( buf1, "\n\r" );

    send_to_char( buf1, ch );
    return;
}



/*****************************************************************************
 Name:      show_help
 Purpose:   Displays help for many tables used in OLC.
 Called by: olc interpreters.
 ****************************************************************************/
bool show_help( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char spell[MAX_INPUT_LENGTH];
    int cnt;

    argument = one_argument( argument, arg );
    one_argument( argument, spell );

    /*
     * Display syntax.
     */
    if ( arg[0] == '\0' )
    {
    send_to_char( "Syntax:  ? [command]\n\r\n\r", ch );
    send_to_char( "[command]  [description]\n\r", ch );
    for (cnt = 0; help_table[cnt].command != NULL; cnt++)
    {
        sprintf( buf, "%-10.10s -%s\n\r",
            capitalize( help_table[cnt].command ),
        help_table[cnt].desc );
        send_to_char( buf, ch );
    }
    return FALSE;
    }

    /*
     * Find the command, show changeable data.
     * ---------------------------------------
     */
    for (cnt = 0; help_table[cnt].command != NULL; cnt++)
    {
        if (  arg[0] == help_table[cnt].command[0]
          && !str_prefix( arg, help_table[cnt].command ) )
    {
        if ( help_table[cnt].structure == spec_table )
        {
        show_spec_cmds( ch );
        return FALSE;
        }
        else
        if ( help_table[cnt].structure == liq_table )
        {
            show_liqlist( ch );
            return FALSE;
        }
        else
        if ( help_table[cnt].structure == tool_table )
        {
            show_toollist( ch );
            return FALSE;
        }
        else
        if ( help_table[cnt].structure == crafted_table )
        {
            show_craftedlist( ch );
            return FALSE;
        }
        else
        if ( help_table[cnt].structure == attack_table )
        {
            show_damlist( ch );
            return FALSE;
        }
        else
        if ( help_table[cnt].structure == skill_table )
        {

        if ( spell[0] == '\0' )
        {
            send_to_char( "Syntax:  ? spells "
                "[ignore/attack/defend/self/object/all]\n\r", ch );
            return FALSE;
        }

        if ( !str_prefix( spell, "all" ) )
            show_skill_cmds( ch, -1 );
        else if ( !str_prefix( spell, "ignore" ) )
            show_skill_cmds( ch, TAR_IGNORE );
        else if ( !str_prefix( spell, "attack" ) )
            show_skill_cmds( ch, TAR_CHAR_OFFENSIVE );
        else if ( !str_prefix( spell, "defend" ) )
            show_skill_cmds( ch, TAR_CHAR_DEFENSIVE );
        else if ( !str_prefix( spell, "self" ) )
            show_skill_cmds( ch, TAR_CHAR_SELF );
        else if ( !str_prefix( spell, "object" ) )
            show_skill_cmds( ch, TAR_OBJ_INV );
        else
            send_to_char( "Syntax:  ? spell "
                "[ignore/attack/defend/self/object/all]\n\r", ch );

        return FALSE;
        }
        else
        {
        show_flag_cmds( ch, help_table[cnt].structure );
        return FALSE;
        }
    }
    }

    show_help( ch, "" );
    return FALSE;
}

REDIT( redit_rlist )
{
    ROOM_INDEX_DATA *pRoomIndex;
    AREA_DATA       *pArea;
    char        buf  [ MAX_STRING_LENGTH   ];
    BUFFER      *buf1;
    char        arg  [ MAX_INPUT_LENGTH    ];
    bool found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );

    pArea = ch->in_room->area;
    buf1=new_buf();
/*    buf1[0] = '\0'; */
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
    if ( ( pRoomIndex = get_room_index( vnum ) ) )
    {
        found = TRUE;
        sprintf( buf, "[%5d] %-17.16s",
            vnum, capitalize( pRoomIndex->name ) );
        add_buf( buf1, buf );
        if ( ++col % 3 == 0 )
            add_buf( buf1, "\n\r" );
    }
    }

    if ( !found )
    {
    send_to_char( "Room(s) not found in this area.\n\r", ch);
    return FALSE;
    }

    if ( col % 3 != 0 )
    add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}

REDIT( redit_mlist )
{
    MOB_INDEX_DATA  *pMobIndex;
    AREA_DATA       *pArea;
    char        buf  [ MAX_STRING_LENGTH   ];
    BUFFER      *buf1;
    char        arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Syntax:  mlist <all/name>\n\r", ch );
    return FALSE;
    }

    buf1=new_buf();
    pArea = ch->in_room->area;
/*    buf1[0] = '\0'; */
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
    if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
    {
        if ( fAll || is_name( arg, pMobIndex->player_name ) )
        {
        found = TRUE;
        sprintf( buf, "[%5d] %-17.16s",
            pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
        add_buf( buf1, buf );
        if ( ++col % 3 == 0 )
            add_buf( buf1, "\n\r" );
        }
    }
    }

    if ( !found )
    {
    send_to_char( "Mobile(s) not found in this area.\n\r", ch);
    return FALSE;
    }

    if ( col % 3 != 0 )
    add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}



REDIT( redit_olist )
{
    OBJ_INDEX_DATA  *pObjIndex;
    AREA_DATA       *pArea;
    char        buf  [ MAX_STRING_LENGTH   ];
    BUFFER      *buf1;
    char        arg  [ MAX_INPUT_LENGTH    ];
    bool fAll, found;
    int vnum;
    int  col = 0;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Syntax:  olist <all/name/item_type>\n\r", ch );
    return FALSE;
    }

    pArea = ch->in_room->area;
    buf1=new_buf();
/*    buf1[0] = '\0'; */
    fAll    = !str_cmp( arg, "all" );
    found   = FALSE;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
    {
    if ( ( pObjIndex = get_obj_index( vnum ) ) )
    {
        if ( fAll || is_name( arg, pObjIndex->name )
        || flag_value( type_flags, arg ) == pObjIndex->item_type )
        {
        found = TRUE;
        sprintf( buf, "[%5d] %-17.16s",
            pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
        add_buf( buf1, buf );
        if ( ++col % 3 == 0 )
            add_buf( buf1, "\n\r" );
        }
    }
    }

    if ( !found )
    {
    send_to_char( "Object(s) not found in this area.\n\r", ch);
    return FALSE;
    }

    if ( col % 3 != 0 )
    add_buf( buf1, "\n\r" );

    page_to_char( buf_string(buf1), ch );
    free_buf(buf1);
    return FALSE;
}



REDIT( redit_mshow )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  mshow <vnum>\n\r", ch );
    return FALSE;
    }

    if ( !is_number( argument ) )
    {
       send_to_char( "REdit: Enter a number.\n\r", ch);
       return FALSE;
    }

    if ( is_number( argument ) )
    {
    value = atoi( argument );
    if ( !( pMob = get_mob_index( value ) ))
    {
        send_to_char( "REdit:  That mobile does not exist.\n\r", ch );
        return FALSE;
    }

    ch->desc->pEdit = (void *)pMob;
    }

    medit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
}

REDIT( redit_copy )
{
    ROOM_INDEX_DATA	*pRoom;
    ROOM_INDEX_DATA	*pRoom2; /* Room to copy */
    int vnum;

    if ( argument[0] == '\0' )
    {
      send_to_char("Syntax: copy <vnum> \n\r",ch);
      return FALSE;
    }

    if ( !is_number(argument) )
    {
      send_to_char("REdit: You must enter a number (vnum).\n\r",ch);
      return FALSE;
    }
    else /* argument is a number */
    {
      vnum = atoi(argument);
      if( !( pRoom2 = get_room_index(vnum) ) )
      {
	send_to_char("REdit: That room does not exist.\n\r",ch);
	return FALSE;
      }
    }

    EDIT_ROOM(ch, pRoom);

    free_string( pRoom->description );
    pRoom->description = str_dup( pRoom2->description );

    free_string( pRoom->name );
    pRoom->name = str_dup( pRoom2->name );

    /* sector flags */
    pRoom->sector_type = pRoom2->sector_type;

    /* room flags */
    pRoom->room_flags = pRoom2->room_flags;

    pRoom->heal_rate = pRoom2->heal_rate;
    pRoom->mana_rate = pRoom2->mana_rate;

    pRoom->clan = pRoom2->clan;

    free_string( pRoom->owner );
    pRoom->owner = str_dup( pRoom2->owner );

    pRoom->extra_descr = pRoom2->extra_descr;

    send_to_char( "Room info copied.", ch );
    return TRUE;
}


REDIT( redit_oshow )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  oshow <vnum>\n\r", ch );
    return FALSE;
    }

    if ( !is_number( argument ) )
    {
       send_to_char( "REdit: Enter a number.\n\r", ch);
       return FALSE;
    }

    if ( is_number( argument ) )
    {
    value = atoi( argument );
    if ( !( pObj = get_obj_index( value ) ))
    {
        send_to_char( "REdit:  That object does not exist.\n\r", ch );
        return FALSE;
    }

    ch->desc->pEdit = (void *)pObj;
    }

    oedit_show( ch, argument );
    ch->desc->pEdit = (void *)ch->in_room;
    return FALSE;
}



/*****************************************************************************
 Name:      check_range( lower vnum, upper vnum )
 Purpose:   Ensures the range spans only one area.
 Called by: aedit_vnum(olc_act.c).
 ****************************************************************************/
bool check_range( int lower, int upper )
{
    AREA_DATA *pArea;
    int cnt = 0;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
    /*
     * lower < area < upper
     */
        if ( ( lower <= pArea->min_vnum && pArea->min_vnum <= upper )
    ||   ( lower <= pArea->max_vnum && pArea->max_vnum <= upper ) )
        ++cnt;

    if ( cnt > 1 )
        return FALSE;
    }
    return TRUE;
}
AEDIT( aedit_level )
{
    AREA_DATA *pArea;
    char lower[MAX_INPUT_LENGTH];
    char upper[MAX_INPUT_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( !is_number( lower ) || lower[0] == '\0'
    || !is_number( upper ) || upper[0] == '\0' )
    {
    send_to_char( "Syntax:  level [#xlower] [#xupper]\n\r", ch );
    return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
    send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
    return FALSE;
    }
    if (ilower < 1 || iupper > 100)
    {
        sendch("AEdit:  Level range must be between levels 1 and 100.\n\r", ch);
        return FALSE;
    }

    pArea->low_range = ilower;
    send_to_char( "Lower level set.\n\r", ch );

    pArea->high_range = iupper;
    send_to_char( "Upper level set.\n\r", ch );

    return TRUE;
}



AEDIT( aedit_llevel )
{
    AREA_DATA *pArea;
    char lower[MAX_INPUT_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, lower );

    if ( !is_number( lower ) || lower[0] == '\0' )
    {
    send_to_char( "Syntax:  llevel [#xlower]\n\r", ch );
    return FALSE;
    }

    if ( pArea->high_range != 0 && ( ilower = atoi( lower ) ) > ( iupper = pArea->high_range ) )
    {
    send_to_char( "AEdit:  Value must be less than the high_range.\n\r", ch );
    return FALSE;
    }

    if (ilower < 1)
    {
        sendch("AEdit:  Value must be greater than 0.\n\r", ch);
        return FALSE;
    }

    pArea->low_range = ilower;
    send_to_char( "Lower level set.\n\r", ch );
    return TRUE;
}


AEDIT( aedit_hlevel )
{
    AREA_DATA *pArea;
    char upper[MAX_INPUT_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, upper );

    if ( !is_number( upper ) || upper[0] == '\0' )
    {
    send_to_char( "Syntax:  hlevel [#xupper]\n\r", ch );
    return FALSE;
    }

    if ( ( ilower = pArea->low_range ) != 0 && ilower > ( iupper = atoi( upper ) ) )
    {
    send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
    return FALSE;
    }
    if (iupper > 100)
    {
        sendch("AEdit:  Value must be less than 100.\n\r", ch);
        return FALSE;
    }

    pArea->high_range = iupper;
    send_to_char( "Upper level set.\n\r", ch );

    return TRUE;
}



AREA_DATA *get_vnum_area( int vnum )
{
    AREA_DATA *pArea;

    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
        if ( vnum >= pArea->min_vnum
          && vnum <= pArea->max_vnum )
            return pArea;
    }

    return 0;
}



/*
 * Area Editor Functions.
 */
AEDIT( aedit_show )
{
    AREA_DATA *pArea;
    char buf  [MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    sprintf( buf, "Name:     [%5d] %s\n\r", pArea->vnum, pArea->name );
    send_to_char( buf, ch );

#if 0  /* ROM OLC */
    sprintf( buf, "Recall:   [%5d] %s\n\r", pArea->recall,
    get_room_index( pArea->recall )
    ? get_room_index( pArea->recall )->name : "none" );
    send_to_char( buf, ch );
#endif /* ROM */

    sprintf( buf, "File:     %s\n\r", pArea->file_name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnums:    [%d-%d]\n\r", pArea->min_vnum, pArea->max_vnum );
    send_to_char( buf, ch );

    cprintf(ch, "Levels:   [%d-%d]\n\r", pArea->low_range, pArea->high_range);

    sprintf( buf, "Age:      [%d]\n\r", pArea->age );
    send_to_char( buf, ch );

    sprintf( buf, "Players:  [%d]\n\r", pArea->nplayer );
    send_to_char( buf, ch );

    sprintf( buf, "Security: [%d]\n\r", pArea->security );
    send_to_char( buf, ch );

    sprintf( buf, "Builders: [%s]\n\r", pArea->builders );
    send_to_char( buf, ch );

    sprintf( buf, "Credits : [%s]\n\r", pArea->credits );
    send_to_char( buf, ch );

     sprintf( buf, "Flags:    [%s]\n\r", flag_string( area_flags, pArea->area_flags ) );
    send_to_char( buf, ch );
    if (pArea->domain == NULL)
        send_to_char("Domain Data: None\n\r", ch);
        else
        {
        send_to_char("Domain Data:\n\r", ch);
        cprintf(ch,  "Domain Vnum:    {D[{w%d{D]{x\n\r", pArea->domain->vnum);
        sprintf(buf, "Ruling Clan:    {D[{R%s{D]{x\n\r", capitalize(clan_table[pArea->domain->clan].name));
        send_to_char(buf, ch);
        sprintf(buf, "Economy Tax:     [%d]\n\r", pArea->domain->tax);
        sendch(buf, ch);
        sprintf( buf, "%s\n\r", "Clan Influence:");
        send_to_char( buf, ch );
        int i, col;
        for (i = 1; i < MAX_CLAN; i++)
        {
            if (pArea->domain->influence[i] == 0)
            continue;

            sprintf( buf, "{w%3d {D: %-20s ", pArea->domain->influence[i], capitalize(clan_table[i].name));
            send_to_char(buf, ch);
            col++;
            if (col > 1 )
            {
                send_to_char("\n\r", ch);
                col = 0;
            }
        }
    }


    return FALSE;
}



AEDIT( aedit_reset )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    reset_area( pArea );
    send_to_char( "Area reset.\n\r", ch );

    return FALSE;
}



AEDIT( aedit_create )
{
    AREA_DATA *pArea;

    pArea               =   new_area();
    area_last->next     =   pArea;
    area_last       =   pArea;  /* Thanks, Walker. */
    ch->desc->pEdit     =   (void *)pArea;

    SET_BIT( pArea->area_flags, AREA_ADDED );
    send_to_char( "Area Created.\n\r", ch );
    return FALSE;
}



AEDIT( aedit_name )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:   name [$name]\n\r", ch );
    return FALSE;
    }

    free_string( pArea->name );
    pArea->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_credits )
{
    AREA_DATA *pArea;

    EDIT_AREA(ch, pArea);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:   credits [$credits]\n\r", ch );
    return FALSE;
    }

    free_string( pArea->credits );
    pArea->credits = str_dup( argument );

    send_to_char( "Credits set.\n\r", ch );
    return TRUE;
}


AEDIT( aedit_file )
{
    AREA_DATA *pArea;
    char file[MAX_STRING_LENGTH];
    int i, length;

    EDIT_AREA(ch, pArea);

    one_argument( argument, file ); /* Forces Lowercase */

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  filename [$file]\n\r", ch );
    return FALSE;
    }

    /*
     * Simple Syntax Check.
     */
     length = strlen(file);

     if (length < 4 || length > 16)
     {
         sendch("Filenames must be between 4 and 16 characters, without the '.are'.", ch);
         return FALSE;
     }
    /*
     * Allow only letters and numbers.
     */
    for ( i = 0; i < length; i++ )
    {
    if ( !isalnum( file[i] ) )
    {
        send_to_char( "Only letters and numbers are valid.\n\r", ch );
        return FALSE;
    }
    }

    free_string( pArea->file_name );
    strcat( file, ".are" );
    pArea->file_name = str_dup( file );

    send_to_char( "Filename set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_age )
{
    AREA_DATA *pArea;
    char age[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, age );

    if ( !is_number( age ) || age[0] == '\0' )
    {
    send_to_char( "Syntax:  age [#xage]\n\r", ch );
    return FALSE;
    }

    pArea->age = atoi( age );

    send_to_char( "Age set.\n\r", ch );
    return TRUE;
}


#if 0 /* ROM OLC */
AEDIT( aedit_recall )
{
    AREA_DATA *pArea;
    char room[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, room );

    if ( !is_number( argument ) || argument[0] == '\0' )
    {
    send_to_char( "Syntax:  recall [#xrvnum]\n\r", ch );
    return FALSE;
    }

    value = atoi( room );

    if ( !get_room_index( value ) )
    {
    send_to_char( "AEdit:  Room vnum does not exist.\n\r", ch );
    return FALSE;
    }

    pArea->recall = value;

    send_to_char( "Recall set.\n\r", ch );
    return TRUE;
}
#endif /* ROM OLC */


AEDIT( aedit_security )
{
    AREA_DATA *pArea;
    char sec[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int  value;

    EDIT_AREA(ch, pArea);

    one_argument( argument, sec );

    if ( !is_number( sec ) || sec[0] == '\0' )
    {
    send_to_char( "Syntax:  security [#xlevel]\n\r", ch );
    return FALSE;
    }

    value = atoi( sec );

    if ( value > ch->pcdata->security || value < 0 )
    {
    if ( ch->pcdata->security != 0 )
    {
        sprintf( buf, "Security is 0-%d.\n\r", ch->pcdata->security );
        send_to_char( buf, ch );
    }
    else
        send_to_char( "Security is 0 only.\n\r", ch );
    return FALSE;
    }

    pArea->security = value;

    send_to_char( "Security set.\n\r", ch );
    return TRUE;
}



AEDIT( aedit_builder )
{
    AREA_DATA *pArea;
    char name[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    EDIT_AREA(ch, pArea);

    one_argument( argument, name );

    if ( name[0] == '\0' )
    {
    send_to_char( "Syntax:  builder [$name]  -toggles builder\n\r", ch );
    send_to_char( "Syntax:  builder All      -allows everyone\n\r", ch );
    return FALSE;
    }

    name[0] = UPPER( name[0] );

    if ( strstr( pArea->builders, name ) != '\0' )
    {
    pArea->builders = string_replace( pArea->builders, name, "\0" );
    pArea->builders = string_unpad( pArea->builders );

    if ( pArea->builders[0] == '\0' )
    {
        free_string( pArea->builders );
        pArea->builders = str_dup( "None" );
    }
    send_to_char( "Builder removed.\n\r", ch );
    return TRUE;
    }
    else
    {
    buf[0] = '\0';
    if ( strstr( pArea->builders, "None" ) != '\0' )
    {
        pArea->builders = string_replace( pArea->builders, "None", "\0" );
        pArea->builders = string_unpad( pArea->builders );
    }

    if (pArea->builders[0] != '\0' )
    {
        strcat( buf, pArea->builders );
        strcat( buf, " " );
    }
    strcat( buf, name );
    free_string( pArea->builders );
    pArea->builders = string_proper( str_dup( buf ) );

    send_to_char( "Builder added.\n\r", ch );
    send_to_char( pArea->builders,ch);
    return TRUE;
    }

    return FALSE;
}



AEDIT( aedit_vnum )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    argument = one_argument( argument, lower );
    one_argument( argument, upper );

    if ( !is_number( lower ) || lower[0] == '\0'
    || !is_number( upper ) || upper[0] == '\0' )
    {
    send_to_char( "Syntax:  vnum [#xlower] [#xupper]\n\r", ch );
    return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = atoi( upper ) ) )
    {
    send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
    return FALSE;
    }

    if ( !check_range( atoi( lower ), atoi( upper ) ) )
    {
    send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
    return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
    send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
    return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
    send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
    return TRUE;    /* The lower value has been set. */
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}



AEDIT( aedit_lvnum )
{
    AREA_DATA *pArea;
    char lower[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, lower );

    if ( !is_number( lower ) || lower[0] == '\0' )
    {
    send_to_char( "Syntax:  min_vnum [#xlower]\n\r", ch );
    return FALSE;
    }

    if ( ( ilower = atoi( lower ) ) > ( iupper = pArea->max_vnum ) )
    {
    send_to_char( "AEdit:  Value must be less than the max_vnum.\n\r", ch );
    return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
    send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
    return FALSE;
    }

    if ( get_vnum_area( ilower )
    && get_vnum_area( ilower ) != pArea )
    {
    send_to_char( "AEdit:  Lower vnum already assigned.\n\r", ch );
    return FALSE;
    }

    pArea->min_vnum = ilower;
    send_to_char( "Lower vnum set.\n\r", ch );
    return TRUE;
}

AEDIT( aedit_clan )
{
    AREA_DATA *pArea;
    EDIT_AREA(ch, pArea);
    char    arg1[MIL];
    char    arg2[MIL];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (clan_lookup(arg1) == 0 && pArea->domain != NULL)
    {
        if (str_cmp(arg2, "confirm"))
            {
                sendch("Setting that would remove the domain for this area and delete all\n\r", ch);
                sendch("clans data associated with this domain.\n\r", ch);
                sendch("BE SURE YOU KNOW WHAT YOU ARE DOING!!\n\r\n\r", ch);
                sendch("To complete this action type 'clan none confirm'\n\r", ch);
                return FALSE;
            } else {
                sendch("Removing clan and domain info from this area.\n\r", ch);
                pArea->domain = NULL;
                free_domain(pArea->domain);
                return;
            }
    } else {
        if (pArea->domain == NULL) {
            pArea->domain = new_domain();
        }
    pArea->domain->clan = clan_lookup(arg1);
    send_to_char ( "Clan set.\n\r", ch);
    return TRUE;
    }
}

AEDIT( aedit_uvnum )
{
    AREA_DATA *pArea;
    char upper[MAX_STRING_LENGTH];
    int  ilower;
    int  iupper;

    EDIT_AREA(ch, pArea);

    one_argument( argument, upper );

    if ( !is_number( upper ) || upper[0] == '\0' )
    {
    send_to_char( "Syntax:  max_vnum [#xupper]\n\r", ch );
    return FALSE;
    }

    if ( ( ilower = pArea->min_vnum ) > ( iupper = atoi( upper ) ) )
    {
    send_to_char( "AEdit:  Upper must be larger then lower.\n\r", ch );
    return FALSE;
    }

    if ( !check_range( ilower, iupper ) )
    {
    send_to_char( "AEdit:  Range must include only this area.\n\r", ch );
    return FALSE;
    }

    if ( get_vnum_area( iupper )
    && get_vnum_area( iupper ) != pArea )
    {
    send_to_char( "AEdit:  Upper vnum already assigned.\n\r", ch );
    return FALSE;
    }

    pArea->max_vnum = iupper;
    send_to_char( "Upper vnum set.\n\r", ch );

    return TRUE;
}



/*
 * Room Editor Functions.
 */
REDIT( redit_show )
{
    ROOM_INDEX_DATA *pRoom;
    char        buf  [MAX_STRING_LENGTH];
    char        buf1 [2*MAX_STRING_LENGTH];
    OBJ_DATA        *obj;
    CHAR_DATA       *rch;
    PROG_LIST		*list;
    int         door;
    bool        fcnt;

    EDIT_ROOM(ch, pRoom);

    buf1[0] = '\0';

    sprintf( buf, "Description:\n\r%s", pRoom->description );
    strcat( buf1, buf );

    sprintf( buf, "Name:       [%s]\n\rArea:       [%5d] %s\n\r",
        pRoom->name, pRoom->area->vnum, pRoom->area->name );
    strcat( buf1, buf );

    sprintf( buf, "Vnum:       [%5d]\n\rSector:     [%s]\n\r",
        pRoom->vnum, flag_string( sector_flags, pRoom->sector_type ) );
    strcat( buf1, buf );

    sprintf( buf, "Room flags: [%s]\n\r",
        flag_string( room_flags, pRoom->room_flags ) );
    strcat( buf1, buf );

    if ( pRoom->heal_rate != 100 || pRoom->mana_rate != 100 )
    {
    sprintf( buf, "Health rec: [%d]\n\rMana rec  : [%d]\n\r",
        pRoom->heal_rate , pRoom->mana_rate );
    strcat( buf1, buf );
    }

    if ( pRoom->clan > 0 )
    {
    sprintf( buf, "Clan      : [%d] %s\n\r",
        pRoom->clan,
        clan_table[pRoom->clan].name );
    strcat( buf1, buf );
    }

    if ( !IS_NULLSTR(pRoom->owner) )
    {
    sprintf( buf, "Owner     : [%s]\n\r", pRoom->owner );
    strcat( buf1, buf );
    }

    if ( pRoom->extra_descr )
    {
    EXTRA_DESCR_DATA *ed;

    strcat( buf1, "Desc Kwds:  [" );
    for ( ed = pRoom->extra_descr; ed; ed = ed->next )
    {
        strcat( buf1, ed->keyword );
        if ( ed->next )
        strcat( buf1, " " );
    }
    strcat( buf1, "]\n\r" );
    }

    strcat( buf1, "Characters: [" );
    fcnt = FALSE;
    for ( rch = pRoom->people; rch; rch = rch->next_in_room )
    {
    one_argument( rch->name, buf );
    strcat( buf1, buf );
    strcat( buf1, " " );
    fcnt = TRUE;
    }

    if ( fcnt )
    {
    int end;

    end = strlen(buf1) - 1;
    buf1[end] = ']';
    strcat( buf1, "\n\r" );
    }
    else
    strcat( buf1, "none]\n\r" );

    strcat( buf1, "Objects:    [" );
    fcnt = FALSE;
    for ( obj = pRoom->contents; obj; obj = obj->next_content )
    {
    one_argument( obj->name, buf );
    strcat( buf1, buf );
    strcat( buf1, " " );
    fcnt = TRUE;
    }

    if ( fcnt )
    {
    int end;

    end = strlen(buf1) - 1;
    buf1[end] = ']';
    strcat( buf1, "\n\r" );
    }
    else
    strcat( buf1, "none]\n\r" );

    strcat( buf1, "\n\r  V=====[    Exit Information    ]=====V\n\r");

    for ( door = 0; door < MAX_DIR; door++ )
    {
    EXIT_DATA *pexit;
    EXIT_DATA *revexit;
    ROOM_INDEX_DATA *revroom;

    if ( ( pexit = pRoom->exit[door] ) )
    {
        char word[MAX_INPUT_LENGTH];
        char reset_state[MAX_STRING_LENGTH];
        char *state;
        int i, length;
        bool oneway = FALSE;

        revroom = get_room_index(pexit->u1.to_room->vnum);
        if ( ( revexit = revroom->exit[rev_dir[door]]) == NULL ||
              revexit->u1.to_room->vnum != pRoom->vnum)
          oneway = TRUE;

        sprintf( buf, "> %-5s %sto [%5d]   Key: [%5d] \n\r",
        capitalize(dir_name[door]),
        oneway == TRUE ? "{Y[1Way]{x " : "{y[2Way]{x ",
        pexit->u1.to_room ? pexit->u1.to_room->vnum : 0,      /* ROM OLC */
        pexit->key );
        strcat( buf1, buf );

        /*
         * Format up the exit info.
         * Capitalize all flags that are not part of the reset info.
         */
        strcpy( reset_state, flag_string( exit_flags, pexit->rs_flags ) );
        state = flag_string( exit_flags, pexit->exit_info );
        strcat( buf1, "   Flags: [" );
        for (; ;)
        {
        state = one_argument( state, word );

        if ( word[0] == '\0' )
        {
            int end;

            end = strlen(buf1) - 1;
            buf1[end] = ']';
            strcat( buf1, "\n\r" );
            break;
        }

        if ( str_infix( word, reset_state ) )
        {
            length = strlen(word);
            for (i = 0; i < length; i++)
            word[i] = UPPER(word[i]);
        }
        strcat( buf1, word );
        strcat( buf1, " " );
        }

        if ( pexit->keyword && pexit->keyword[0] != '\0' )
        {
        sprintf( buf, "Kwds: [%s]\n\r", pexit->keyword );
        strcat( buf1, buf );
        }
        if ( pexit->description && pexit->description[0] != '\0' )
        {
        sprintf( buf, "%s", pexit->description );
        strcat( buf1, buf );
        }
    }
    }

    send_to_char( buf1, ch );

    if ( pRoom->rprogs )
     {
     int cnt;

     sprintf(buf, "\n\rROOMPrograms for [%5d]:\n\r", pRoom->vnum);
     send_to_char( buf, ch );

     for (cnt=0, list=pRoom->rprogs; list; list=list->next)
     {
         if (cnt ==0)
         {
             send_to_char ( "  [P#]   Vnum  TRIGGER  'Phrase'\n\r", ch );
             send_to_char ( "  ----  -----  -------  --------\n\r", ch );
         }

         sprintf(buf, "[%2d] %5d %7.7s '%s'\n\r", cnt,
             list->vnum,prog_type_to_name(list->trig_type),
             list->trig_phrase);
         send_to_char( buf, ch );
         cnt++;
     }
     }
    return FALSE;
}




/* Local function. */
bool change_exit( CHAR_DATA *ch, char *argument, int door )
{
    ROOM_INDEX_DATA *pRoom;
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char buf[MSL];
    int  value;

    EDIT_ROOM(ch, pRoom);

    /*
     * Set the exit flags, needs full argument.
     * ----------------------------------------
     */
    if ( ( value = flag_value( exit_flags, argument ) ) != NO_FLAG )
    {
    ROOM_INDEX_DATA *pToRoom;
    sh_int rev;                                    /* ROM OLC */

    if ( !pRoom->exit[door] )
    {
        send_to_char("Exit does not exist.\n\r",ch);
        return FALSE;
    }

    /*
     * This room.
     */
    TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
    /* Don't toggle exit_info because it can be changed by players. */
    pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;

    /*
     * Connected room.
     */
    pToRoom = pRoom->exit[door]->u1.to_room;     /* ROM OLC */
    rev = rev_dir[door];

    if (pToRoom->exit[rev] != NULL)
    {
        pToRoom->exit[rev]->rs_flags = pRoom->exit[door]->rs_flags;
        pToRoom->exit[rev]->exit_info = pRoom->exit[door]->exit_info;
    }

    send_to_char( "Exit flag toggled.\n\r", ch );
    return TRUE;
    }

    /*
     * Now parse the arguments.
     */
    argument = one_argument( argument, command );
    one_argument( argument, arg );

    if ( command[0] == '\0' && argument[0] == '\0' )    /* Move command. */
    {
    move_char( ch, door, TRUE );                    /* ROM OLC */
    return FALSE;
    }

    if ( command[0] == '?' )
    {
    do_help( ch, "EXIT" );
    return FALSE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
    ROOM_INDEX_DATA *pToRoom;
    sh_int rev;                                     /* ROM OLC */

    if ( !pRoom->exit[door] )
    {
        send_to_char( "REdit:  Cannot delete a null exit.\n\r", ch );
        return FALSE;
    }

    /*
     * Remove ToRoom Exit.
     */
    rev = rev_dir[door];
    pToRoom = pRoom->exit[door]->u1.to_room;       /* ROM OLC */

    if ( pToRoom->exit[rev] )
    {
        if (pToRoom->exit[rev]->u1.to_room == pRoom)
        {
            send_to_char("Unlinking two-way exit.\n\r", ch);
            free_exit( pToRoom->exit[rev] );
            pToRoom->exit[rev] = NULL;
        } else {
            sprintf(buf, "{rWARNING{w: Reverse exit unchanged. Exit from %d does not link here. (Links to %d).\n\r", pToRoom->vnum, pToRoom->exit[rev]->u1.to_room->vnum);
            sendch(buf, ch);
        }

    }

    /*
     * Remove this exit.
     */
    free_exit( pRoom->exit[door] );
    pRoom->exit[door] = NULL;

    send_to_char( "Exit unlinked.\n\r", ch );
    return TRUE;
    }

    if ( !str_cmp( command, "link" ) )
    {
    EXIT_DATA *pExit;
    ROOM_INDEX_DATA *toRoom;

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
        send_to_char( "Syntax:  [direction] link [vnum]\n\r", ch );
        return FALSE;
    }

    value = atoi( arg );

    if ( ! (toRoom = get_room_index( value )) )
    {
        send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
        return FALSE;
    }

    if ( !IS_BUILDER( ch, toRoom->area ) )
    {
        send_to_char( "REdit:  Cannot link to that area.\n\r", ch );
        return FALSE;
    }

    if ( toRoom->exit[rev_dir[door]] )
    {
        send_to_char( "REdit:  Remote side's exit already exists.\n\r", ch );
        return FALSE;
    }

    if ( !pRoom->exit[door] )
        pRoom->exit[door] = new_exit();

    pRoom->exit[door]->u1.to_room = toRoom;
    pRoom->exit[door]->orig_door = door;

    door                    = rev_dir[door];
    pExit                   = new_exit();
    pExit->u1.to_room       = pRoom;
    pExit->orig_door    = door;
    toRoom->exit[door]       = pExit;

    send_to_char( "Two-way link established.\n\r", ch );
    return TRUE;
    }

    if ( !str_cmp( command, "dig" ) )
    {
    char buf[MAX_STRING_LENGTH];

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
        send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
        return FALSE;
    }

    redit_create( ch, arg );
    sprintf( buf, "link %s", arg );
    change_exit( ch, buf, door);
    return TRUE;
    }

    if ( !str_cmp( command, "room" ) )
    {
    ROOM_INDEX_DATA *toRoom;

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
        send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
        return FALSE;
    }

    value = atoi( arg );

    if ( !(toRoom = get_room_index( value )) )
    {
        send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
        return FALSE;
    }

    if ( !pRoom->exit[door] )
        pRoom->exit[door] = new_exit();

    pRoom->exit[door]->u1.to_room = toRoom;    /* ROM OLC */
    pRoom->exit[door]->orig_door = door;

    send_to_char( "One-way link established.\n\r", ch );
    return TRUE;
    }

    if ( !str_cmp( command, "key" ) )
    {
    OBJ_INDEX_DATA *key;

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
        send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
        return FALSE;
    }

    if ( !pRoom->exit[door] )
    {
        send_to_char("Salida no existe.\n\r",ch);
        return FALSE;
    }

    value = atoi( arg );

    if ( !(key = get_obj_index( value )) )
    {
        send_to_char( "REdit:  Key doesn't exist.\n\r", ch );
        return FALSE;
    }

    if ( key->item_type != ITEM_KEY )
    {
        send_to_char( "REdit:  Object is not a key.\n\r", ch );
        return FALSE;
    }

    pRoom->exit[door]->key = value;

    send_to_char( "Exit key set.\n\r", ch );
    return TRUE;
    }

    if ( !str_cmp( command, "name" ) )
    {
    if ( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
        send_to_char( "         [direction] name none\n\r", ch );
        return FALSE;
    }

    if ( !pRoom->exit[door] )
       {
        send_to_char("Exit does not exist.\n\r",ch);
        return FALSE;
       }

    free_string( pRoom->exit[door]->keyword );

    if (str_cmp(arg,"none"))
        pRoom->exit[door]->keyword = str_dup( arg );
    else
        pRoom->exit[door]->keyword = str_dup( "" );

    send_to_char( "Exit name set.\n\r", ch );
    return TRUE;
    }

    if ( !str_prefix( command, "description" ) )
    {
    if ( arg[0] == '\0' )
    {
       if ( !pRoom->exit[door] )
       {
        send_to_char("Exit does not exist.\n\r",ch);
        return FALSE;
       }

        string_append( ch, &pRoom->exit[door]->description );
        return TRUE;
    }

    send_to_char( "Syntax:  [direction] desc\n\r", ch );
    return FALSE;
    }

    return FALSE;
}



REDIT( redit_north )
{
    if ( change_exit( ch, argument, DIR_NORTH ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_south )
{
    if ( change_exit( ch, argument, DIR_SOUTH ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_east )
{
    if ( change_exit( ch, argument, DIR_EAST ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_west )
{
    if ( change_exit( ch, argument, DIR_WEST ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_up )
{
    if ( change_exit( ch, argument, DIR_UP ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_down )
{
    if ( change_exit( ch, argument, DIR_DOWN ) )
    return TRUE;

    return FALSE;
}



REDIT( redit_ed )
{
    ROOM_INDEX_DATA *pRoom;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' || keyword[0] == '\0' )
    {
    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
    send_to_char( "         ed edit [keyword]\n\r", ch );
    send_to_char( "         ed delete [keyword]\n\r", ch );
    send_to_char( "         ed format [keyword]\n\r", ch );
    return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
        return FALSE;
    }

    ed          =   new_extra_descr();
    ed->keyword     =   str_dup( keyword );
    ed->description     =   str_dup( "" );
    ed->next        =   pRoom->extra_descr;
    pRoom->extra_descr  =   ed;

    string_append( ch, &ed->description );

    return TRUE;
    }


    if ( !str_cmp( command, "edit" ) )
    {
    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pRoom->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
    }

    if ( !ed )
    {
        send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
        return FALSE;
    }

    string_append( ch, &ed->description );

    return TRUE;
    }


    if ( !str_cmp( command, "delete" ) )
    {
    EXTRA_DESCR_DATA *ped = NULL;

    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pRoom->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
        ped = ed;
    }

    if ( !ed )
    {
        send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
        return FALSE;
    }

    if ( !ped )
        pRoom->extra_descr = ed->next;
    else
        ped->next = ed->next;

    free_extra_descr( ed );

    send_to_char( "Extra description deleted.\n\r", ch );
    return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pRoom->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
    }

    if ( !ed )
    {
        send_to_char( "REdit:  Extra description keyword not found.\n\r", ch );
        return FALSE;
    }

    ed->description = format_string( ed->description );

    send_to_char( "Extra description formatted.\n\r", ch );
    return TRUE;
    }

    redit_ed( ch, "" );
    return FALSE;
}



REDIT( redit_create )
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    int value;
    int iHash;

    EDIT_ROOM(ch, pRoom);

    value = atoi( argument );

    if ( argument[0] == '\0' || value <= 0 )
    {
    send_to_char( "Syntax:  create [vnum > 0]\n\r", ch );
    return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
    send_to_char( "REdit:  That vnum is not assigned an area.\n\r", ch );
    return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
    send_to_char( "REdit:  Vnum in an area you cannot build in.\n\r", ch );
    return FALSE;
    }

    if ( get_room_index( value ) )
    {
    send_to_char( "REdit:  Room vnum already exists.\n\r", ch );
    return FALSE;
    }

    pRoom           = new_room_index();
    pRoom->area         = pArea;
    pRoom->vnum         = value;

    if ( value > top_vnum_room )
        top_vnum_room = value;

    iHash           = value % MAX_KEY_HASH;
    pRoom->next         = room_index_hash[iHash];
    room_index_hash[iHash]  = pRoom;
    ch->desc->pEdit     = (void *)pRoom;

    send_to_char( "Room created.\n\r", ch );
    return TRUE;
}



REDIT( redit_name )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  name [name]\n\r", ch );
    return FALSE;
    }

    free_string( pRoom->name );
    pRoom->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch );
    return TRUE;
}



REDIT( redit_desc )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
    string_append( ch, &pRoom->description );
    return TRUE;
    }

    send_to_char( "Syntax:  desc\n\r", ch );
    return FALSE;
}

REDIT( redit_heal )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (is_number(argument))
       {
          pRoom->heal_rate = atoi ( argument );
          send_to_char ( "Heal rate set.\n\r", ch);
          return TRUE;
       }

    send_to_char ( "Syntax : heal <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_mana )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if (is_number(argument))
       {
          pRoom->mana_rate = atoi ( argument );
          send_to_char ( "Mana rate set.\n\r", ch);
          return TRUE;
       }

    send_to_char ( "Syntax : mana <#xnumber>\n\r", ch);
    return FALSE;
}

REDIT( redit_clan )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    pRoom->clan = clan_lookup(argument);

    send_to_char ( "Clan set.\n\r", ch);
    return TRUE;
}

REDIT( redit_format )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    pRoom->description = format_string( pRoom->description );

    send_to_char( "String formatted.\n\r", ch );
    return TRUE;
}



REDIT( redit_mreset )
{
    ROOM_INDEX_DATA *pRoom;
    MOB_INDEX_DATA  *pMobIndex;
    CHAR_DATA       *newmob;
    char        arg [ MAX_INPUT_LENGTH ];
    char        arg2 [ MAX_INPUT_LENGTH ];

    RESET_DATA      *pReset;
    char        output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' || !is_number( arg ) )
    {
    send_to_char ( "Syntax:  mreset <vnum> <max #x> <mix #x>\n\r", ch );
    return FALSE;
    }

    if ( !( pMobIndex = get_mob_index( atoi( arg ) ) ) )
    {
    send_to_char( "REdit: No mobile has that vnum.\n\r", ch );
    return FALSE;
    }

    if ( pMobIndex->area != pRoom->area )
    {
    send_to_char( "REdit: No such mobile in this area.\n\r", ch );
    return FALSE;
    }

    /*
     * Create the mobile reset.
     */
    pReset              = new_reset_data();
    pReset->command = 'M';
    pReset->arg1    = pMobIndex->vnum;
    pReset->arg2    = is_number( arg2 ) ? atoi( arg2 ) : MAX_MOB;
    pReset->arg3    = pRoom->vnum;
    pReset->arg4    = is_number( argument ) ? atoi (argument) : 1;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    /*
     * Create the mobile.
     */
    newmob = create_mobile( pMobIndex );
    char_to_room( newmob, pRoom );

    sprintf( output, "%s (%d) has been loaded and added to resets.\n\r"
    "There will be a maximum of %d loaded to this room.\n\r",
    capitalize( pMobIndex->short_descr ),
    pMobIndex->vnum,
    pReset->arg2 );
    send_to_char( output, ch );
    act( "$n has created $N!", ch, NULL, newmob, TO_ROOM );
    return TRUE;
}



struct wear_type
{
    int wear_loc;
    int wear_bit;
};



const struct wear_type wear_table[] =
{
    {   WEAR_NONE,  ITEM_TAKE       },
    {   WEAR_LIGHT, ITEM_LIGHT      },
    {   WEAR_FINGER_L,  ITEM_WEAR_FINGER    },
    {   WEAR_FINGER_R,  ITEM_WEAR_FINGER    },
    {   WEAR_NECK_1,    ITEM_WEAR_NECK      },
    {   WEAR_NECK_2,    ITEM_WEAR_NECK      },
    {   WEAR_TORSO,  ITEM_WEAR_TORSO      },
    {   WEAR_HEAD,  ITEM_WEAR_HEAD      },
    {   WEAR_LEGS,  ITEM_WEAR_LEGS      },
    {   WEAR_FEET,  ITEM_WEAR_FEET      },
    {   WEAR_HANDS, ITEM_WEAR_HANDS     },
    {   WEAR_ARMS,  ITEM_WEAR_ARMS      },
    {   WEAR_SHIELD,    ITEM_WEAR_SHIELD    },
    {   WEAR_BODY, ITEM_WEAR_BODY     },
    {   WEAR_WAIST, ITEM_WEAR_WAIST     },
    {   WEAR_WRIST_L,   ITEM_WEAR_WRIST     },
    {   WEAR_WRIST_R,   ITEM_WEAR_WRIST     },
    {   WEAR_WIELD, ITEM_WIELD      },
    {   WEAR_HOLD,  ITEM_HOLD       },
    {   WEAR_EAR_L, ITEM_WEAR_EAR       },
    {   WEAR_EAR_R, ITEM_WEAR_EAR       },
    {   WEAR_FACE,  ITEM_WEAR_FACE      },
    {   WEAR_BACK,  ITEM_WEAR_BACK      },
    {   NO_FLAG,    NO_FLAG         }
};



/*****************************************************************************
 Name:      wear_loc
 Purpose:   Returns the location of the bit that matches the count.
        1 = first match, 2 = second match etc.
 Called by: oedit_reset(olc_act.c).
 ****************************************************************************/
int wear_loc(int bits, int count)
{
    int flag;

    for (flag = 0; wear_table[flag].wear_bit != NO_FLAG; flag++)
    {
        if ( IS_SET(bits, wear_table[flag].wear_bit) && --count < 1)
            return wear_table[flag].wear_loc;
    }

    return NO_FLAG;
}



/*****************************************************************************
 Name:      wear_bit
 Purpose:   Converts a wear_loc into a bit.
 Called by: redit_oreset(olc_act.c).
 ****************************************************************************/
int wear_bit(int loc)
{
    int flag;

    for (flag = 0; wear_table[flag].wear_loc != NO_FLAG; flag++)
    {
        if ( loc == wear_table[flag].wear_loc )
            return wear_table[flag].wear_bit;
    }

    return 0;
}



REDIT( redit_oreset )
{
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA  *pObjIndex;
    OBJ_DATA        *newobj;
    OBJ_DATA        *to_obj;
    CHAR_DATA       *to_mob;
    char        arg1 [ MAX_INPUT_LENGTH ];
    char        arg2 [ MAX_INPUT_LENGTH ];
    int         olevel = 0;

    RESET_DATA      *pReset;
    char        output [ MAX_STRING_LENGTH ];

    EDIT_ROOM(ch, pRoom);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
    send_to_char ( "Syntax:  oreset <vnum> <args>\n\r", ch );
    send_to_char ( "        -no_args               = into room\n\r", ch );
    send_to_char ( "        -<obj_name>            = into obj\n\r", ch );
    send_to_char ( "        -<mob_name> <wear_loc> = into mob\n\r", ch );
    return FALSE;
    }

    if ( !( pObjIndex = get_obj_index( atoi( arg1 ) ) ) )
    {
    send_to_char( "REdit: No object has that vnum.\n\r", ch );
    return FALSE;
    }

    if ( pObjIndex->area != pRoom->area )
    {
    send_to_char( "REdit: No such object in this area.\n\r", ch );
    return FALSE;
    }

    /*
     * Load into room.
     */
    if ( arg2[0] == '\0' )
    {
    pReset      = new_reset_data();
    pReset->command = 'O';
    pReset->arg1    = pObjIndex->vnum;
    pReset->arg2    = 0;
    pReset->arg3    = pRoom->vnum;
    pReset->arg4    = 0;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
    obj_to_room( newobj, pRoom );

    sprintf( output, "%s (%d) has been loaded and added to resets.\n\r",
        capitalize( pObjIndex->short_descr ),
        pObjIndex->vnum );
    send_to_char( output, ch );
    }
    else
    /*
     * Load into object's inventory.
     */
    if ( argument[0] == '\0'
    && ( ( to_obj = get_obj_list( ch, arg2, pRoom->contents ) ) != NULL ) )
    {
    pReset      = new_reset_data();
    pReset->command = 'P';
    pReset->arg1    = pObjIndex->vnum;
    pReset->arg2    = 0;
    pReset->arg3    = to_obj->pIndexData->vnum;
    pReset->arg4    = 1;
    add_reset( pRoom, pReset, 0/* Last slot*/ );

    newobj = create_object( pObjIndex, number_fuzzy( olevel ) );
    newobj->cost = 0;
    obj_to_obj( newobj, to_obj );

    sprintf( output, "%s (%d) has been loaded into "
        "%s (%d) and added to resets.\n\r",
        capitalize( newobj->short_descr ),
        newobj->pIndexData->vnum,
        to_obj->short_descr,
        to_obj->pIndexData->vnum );
    send_to_char( output, ch );
    }
    else
    /*
     * Load into mobile's inventory.
     */
    if ( ( to_mob = get_char_room( ch, NULL, arg2 ) ) != NULL )
    {
    int wear_loc;

    /*
     * Make sure the location on mobile is valid.
     */
    if ( (wear_loc = flag_value( wear_loc_flags, argument )) == NO_FLAG )
    {
        send_to_char( "REdit: Invalid wear_loc.  '? wear-loc'\n\r", ch );
        return FALSE;
    }

    /*
     * Disallow loading a sword(WEAR_WIELD) into WEAR_HEAD.
     */
    if ( !IS_SET( pObjIndex->wear_flags, wear_bit(wear_loc) ) )
    {
        sprintf( output,
            "%s (%d) has wear flags: [%s]\n\r",
            capitalize( pObjIndex->short_descr ),
            pObjIndex->vnum,
        flag_string( wear_flags, pObjIndex->wear_flags ) );
        send_to_char( output, ch );
        return FALSE;
    }

    /*
     * Can't load into same position.
     */
    if ( get_eq_char( to_mob, wear_loc ) )
    {
        send_to_char( "REdit:  Object already equipped.\n\r", ch );
        return FALSE;
    }

    pReset      = new_reset_data();
    pReset->arg1    = pObjIndex->vnum;
    pReset->arg2    = wear_loc;
    if ( pReset->arg2 == WEAR_NONE )
        pReset->command = 'G';
    else
        pReset->command = 'E';
    pReset->arg3    = wear_loc;

    add_reset( pRoom, pReset, 0/* Last slot*/ );

    olevel  = URANGE( 0, to_mob->level - 2, LEVEL_HERO );
        newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

    if ( to_mob->pIndexData->pShop )    /* Shop-keeper? */
    {
        switch ( pObjIndex->item_type )
        {
        default:        olevel = 0;             break;
        case ITEM_PILL: olevel = number_range(  0, 10 );    break;
        case ITEM_POTION:   olevel = number_range(  0, 10 );    break;
        case ITEM_SCROLL:   olevel = number_range(  5, 15 );    break;
        case ITEM_WAND: olevel = number_range( 10, 20 );    break;
        case ITEM_STAFF:    olevel = number_range( 15, 25 );    break;
        case ITEM_ARMOR:    olevel = number_range(  5, 15 );    break;
        case ITEM_WEAPON:   if ( pReset->command == 'G' )
                        olevel = number_range( 5, 15 );
                else
                    olevel = number_fuzzy( olevel );
        break;
        }

        newobj = create_object( pObjIndex, olevel );
        if ( pReset->arg2 == WEAR_NONE )
        SET_BIT( newobj->extra_flags, ITEM_INVENTORY );
    }
    else
        newobj = create_object( pObjIndex, number_fuzzy( olevel ) );

    obj_to_char( newobj, to_mob );
    if ( pReset->command == 'E' )
        equip_char( to_mob, newobj, pReset->arg3 );

    sprintf( output, "%s (%d) has been loaded "
        "%s of %s (%d) and added to resets.\n\r",
        capitalize( pObjIndex->short_descr ),
        pObjIndex->vnum,
        flag_string( wear_loc_strings, pReset->arg3 ),
        to_mob->short_descr,
        to_mob->pIndexData->vnum );
    send_to_char( output, ch );
    }
    else    /* Display Syntax */
    {
    send_to_char( "REdit:  That mobile isn't here.\n\r", ch );
    return FALSE;
    }

    act( "$n has created $p!", ch, newobj, NULL, TO_ROOM );
    return TRUE;
}



/*
 * Object Editor Functions.
 */
void show_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *obj )
{
    char buf[MAX_STRING_LENGTH];

    switch( obj->item_type )
    {
    default:    /* No values. */
        break;

    case ITEM_LIGHT:
      send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
      sprintf(buf, "v[0] Replenish Rate      : %3d %s  \n\rv[1] Replenish Chance    : %3d%  \n\rv[2] Current Light Hours : %3d %s  \n\rv[3] Max Light Hours     : %3d %s \n\rv[4] Light Flags         : %s\n\r",
        obj->value[0],
        obj->value[0] == 0 ? "[{yNo Replenish{x]" : "Per Update",
        obj->value[1],
        obj->value[2],
        obj->value[2] == -1 || obj->value[2] == 999 ? "[{MInfinite{x]" : "",
        obj->value[3],
        obj->value[3] == -1 || obj->value[3] == 999 ? "[{MInfinite{x]" : "",
        flag_string( light_flags, obj->value[4]));
      send_to_char( buf, ch );
      send_to_char("{c-----------------------------------------------------{x\n\r", ch);
      break;

    case ITEM_WAND:
    case ITEM_STAFF:
            sprintf( buf,
        "[v0] Level:          [%d]\n\r"
        "[v1] Charges Total:  [%d]\n\r"
        "[v2] Charges Left:   [%d]\n\r"
        "[v3] Spell:          %s\n\r",
        obj->value[0],
        obj->value[1],
        obj->value[2],
        obj->value[3] != -1 ? skill_table[obj->value[3]].name
                            : "none" );
        send_to_char( buf, ch );
        break;

    case ITEM_PORTAL:
        sprintf( buf,
            "[v0] Charges:        [%d]\n\r"
            "[v1] Exit Flags:     %s\n\r"
            "[v2] Portal Flags:   %s\n\r"
            "[v3] Goes to (vnum): [%d]\n\r",
            obj->value[0],
            flag_string( exit_flags, obj->value[1]),
            flag_string( portal_flags , obj->value[2]),
            obj->value[3] );
        send_to_char( buf, ch);
        break;

    case ITEM_FURNITURE:
        sprintf( buf,
            "[v0] Max people:      [%d]\n\r"
            "[v1] Max weight:      [%d]\n\r"
            "[v2] Furniture Flags: %s\n\r"
            "[v3] Heal bonus:      [%d]\n\r"
            "[v4] Mana bonus:      [%d]\n\r",
            obj->value[0],
            obj->value[1],
            flag_string( furniture_flags, obj->value[2]),
            obj->value[3],
            obj->value[4] );
        send_to_char( buf, ch );
        break;

    case ITEM_TRAP:
        sprintf( buf,
            "[v0] Damage flags:     %s\n\r"
            "[v1] Trigger flags:    %s\n\r"
            "[v2] Target flags:     %s\n\r"
            "[v3] Trap Charges:    [%d]\n\r",
            flag_string( trap_flags, obj->value[0]),
            flag_string( trigger_flags, obj->value[1]),
            flag_string( focus_flags, obj->value[2]),
            obj->value[3]);
        send_to_char( buf, ch );
        break;

    case ITEM_SCROLL:
    case ITEM_POTION:
    case ITEM_PILL:
            sprintf( buf,
        "[v0] Level:  [%d]\n\r"
        "[v1] Spell:  %s\n\r"
        "[v2] Spell:  %s\n\r"
        "[v3] Spell:  %s\n\r"
        "[v4] Spell:  %s\n\r",
        obj->value[0],
        obj->value[1] != -1 ? skill_table[obj->value[1]].name
                            : "none",
        obj->value[2] != -1 ? skill_table[obj->value[2]].name
                                    : "none",
        obj->value[3] != -1 ? skill_table[obj->value[3]].name
                            : "none",
        obj->value[4] != -1 ? skill_table[obj->value[4]].name
                            : "none" );
        send_to_char( buf, ch );
        break;

/* ARMOR for ROM */

        case ITEM_ARMOR:
        sprintf( buf,
        "[v0] Ac pierce       [%d]\n\r"
        "[v1] Ac bash         [%d]\n\r"
        "[v2] Ac slash        [%d]\n\r"
        "[v3] Ac exotic       [%d]\n\r",
        obj->value[0],
        obj->value[1],
        obj->value[2],
        obj->value[3] );
        send_to_char( buf, ch );
        break;

/* WEAPON changed in ROM: */
/* I had to split the output here, I have no idea why, but it helped -- Hugin */
/* It somehow fixed a bug in showing scroll/pill/potions too ?! */
    case ITEM_WEAPON:
            sprintf( buf, "[v0] Weapon class:   %s\n\r",
             flag_string( weapon_class, obj->value[0] ) );
        send_to_char( buf, ch );
        sprintf( buf, "[v1] Number of dice: [%d]\n\r", obj->value[1] );
        send_to_char( buf, ch );
        sprintf( buf, "[v2] Type of dice:   [%d]\n\r", obj->value[2] );
        send_to_char( buf, ch );
        sprintf( buf, "[v3] Type:           %s\n\r",
            attack_table[obj->value[3]].name );
        send_to_char( buf, ch );
        sprintf( buf, "[v4] Special type:   %s\n\r",
             flag_string( weapon_type2,  obj->value[4] ) );
        send_to_char( buf, ch );
        break;

    case ITEM_CONTAINER:
      send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
      sprintf(buf,
        "v[0] Maximum Weight      : %5d.%d lbs  \n\rv[1] Container Flags     : %s  \n\rv[2] Key                 : %5d %s  \n\rv[3] Max Capacity        : %5d items \n\rv[4] Weight Multiplier   : %5d % \n\r",
        obj->value[0] / 10, obj->value[0] % 10,
        flag_string( container_flags, obj->value[1] ),
        obj->value[2],
        get_obj_index(obj->value[2])
          ? get_obj_index(obj->value[2])->short_descr : "none",
        obj->value[3],
        obj->value[4] );
      send_to_char( buf, ch );
      send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;

    case ITEM_DRINK_CON:
        sprintf( buf,
            "[v0] Liquid Total: [%d]\n\r"
            "[v1] Liquid Left:  [%d]\n\r"
            "[v2] Liquid:       %s\n\r"
            "[v3] Poisoned:     %s\n\r",
            obj->value[0],
            obj->value[1],
            liq_table[obj->value[2]].liq_name,
            obj->value[3] != 0 ? "Yes" : "No" );
        send_to_char( buf, ch );
        break;

    case ITEM_FOUNTAIN:
        sprintf( buf,
            "[v0] Liquid Total: [%d]\n\r"
            "[v1] Liquid Left:  [%d]\n\r"
            "[v2] Liquid:       %s\n\r",
            obj->value[0],
            obj->value[1],
            liq_table[obj->value[2]].liq_name );
        send_to_char( buf,ch );
        break;

    case ITEM_FOOD:
        sprintf( buf,
        "[v0] Food hours: [%d]\n\r"
        "[v1] Full hours: [%d]\n\r"
        "[v3] Poisoned:   %s\n\r",
        obj->value[0],
        obj->value[1],
        obj->value[3] != 0 ? "Yes" : "No" );
        send_to_char( buf, ch );
        break;

    case ITEM_MONEY:
            sprintf( buf, "[v0] Gold:   [%d]\n\r", obj->value[0] );
        send_to_char( buf, ch );
        break;

    case ITEM_TOOL:
        sprintf( buf,
        "[v0] Quality: [%d]\n\r"
        "[v1] Type:    %s\n\r",
        obj->value[0],
        tool_table[obj->value[1]].tool_name );
        send_to_char( buf, ch );
        break;

    case ITEM_CRAFTED:
        sprintf( buf,
        "[v0] Quality: [%d]\n\r"
        "[v1] Type:    %s\n\r"
        "[v2] Socket1: %d\n\r"
        "[v3] Socket2: %d\n\r"
        "[v4] Socket3: %d\n\r",
        obj->value[0],
        crafted_table[obj->value[1]].crafted_name,
        obj->value[2],
        obj->value[3],
        obj->value[4]);
        send_to_char( buf, ch );
        break;

      case ITEM_PLASM:
        send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
        sprintf(buf, "v[0] Total Plasm         : %3d Pieces\n\r",
          obj->value[0]);
        send_to_char( buf, ch );
        send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;

      case ITEM_LOCKPICK:
        send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
        sprintf(buf,
          "v[0] Durability          : %3d \n\r"
          "v[1] Difficulty          : %3d Adjustment\n\r",
          obj->value[0],
          obj->value[1]);
        send_to_char( buf, ch );
        send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;

      case ITEM_BLASTPOWDER:
        send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
        sprintf(buf,
          "v[0] Charges             : %2d Charges\n\r"
          "v[1] Damage Dice         : %2d Dice\n\r",
          obj->value[0],
          obj->value[1]);
        send_to_char( buf, ch );
        send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;
    }

    return;
}



bool set_obj_values( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, int value_num, char *argument)
{
    int set_value = atoi(argument);

    switch( pObj->item_type )
    {
        default:
            break;

        case ITEM_LIGHT:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_LIGHT" );
                return FALSE;
            case 0:
              if (set_value < 0 || set_value > 10)
              {
                send_to_char("Light Replenish Value is between 0 [No Replenish] and 10.\n\r", ch);
                break;
              }
              send_to_char( "LIGHT REPLENISH RATE SET.\n\r\n\r", ch );
              pObj->value[0] = atoi( argument );
              break;
            case 1:
              if (set_value < 0 || set_value > 100)
              {
                send_to_char("Light Replenish Chance (%) is between 0 [No Chance] and 100.\n\r", ch);
                break;
              }
              send_to_char( "LIGHT REPLENISH CHANCE SET.\n\r\n\r", ch );
              pObj->value[1] = atoi( argument );
              break;
            case 2:
              if (set_value < -1 || set_value > pObj->value[3] || set_value == 0)
              {
                send_to_char("Current Light Hours can be set to:  -1 [Infinite], or between 1 and Maximum Light Hours.\n\r", ch);
                break;
              }
              send_to_char( "HOURS OF LIGHT SET.\n\r\n\r", ch );
              pObj->value[2] = atoi( argument );
              break;
            case 3:
              if (set_value < -1 || set_value > 999 || set_value == 0)
              {
                send_to_char("Maximum Light Hours can be set to:  -1 [Infinite], or between 1 and 999.\n\r", ch);
                break;
              }
              send_to_char( "MAXIMUM LIGHT HOURS SET.\n\r\n\r", ch );
              pObj->value[3] = atoi( argument );
              break;
            case 4:
              ALT_FLAGVALUE_TOGGLE( pObj->value[4], light_flags, argument );
              break;
        }
            break;

        case ITEM_WAND:
        case ITEM_STAFF:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_STAFF_WAND" );
                return FALSE;
            case 0:
                send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "TOTAL NUMBER OF CHARGES SET.\n\r\n\r", ch );
                pObj->value[1] = atoi( argument );
                break;
            case 2:
                send_to_char( "CURRENT NUMBER OF CHARGES SET.\n\r\n\r", ch );
                pObj->value[2] = atoi( argument );
                break;
            case 3:
                send_to_char( "SPELL TYPE SET.\n\r", ch );
                pObj->value[3] = skill_lookup( argument );
                break;
        }
            break;

        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_SCROLL_POTION_PILL" );
                return FALSE;
            case 0:
                send_to_char( "SPELL LEVEL SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "SPELL TYPE 1 SET.\n\r\n\r", ch );
                pObj->value[1] = skill_lookup( argument );
                break;
            case 2:
                send_to_char( "SPELL TYPE 2 SET.\n\r\n\r", ch );
                pObj->value[2] = skill_lookup( argument );
                break;
            case 3:
                send_to_char( "SPELL TYPE 3 SET.\n\r\n\r", ch );
                pObj->value[3] = skill_lookup( argument );
                break;
            case 4:
                send_to_char( "SPELL TYPE 4 SET.\n\r\n\r", ch );
                pObj->value[4] = skill_lookup( argument );
                break;
        }
        break;

/* ARMOR for ROM: */

        case ITEM_ARMOR:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_ARMOR" );
            return FALSE;
            case 0:
            send_to_char( "AC PIERCE SET.\n\r\n\r", ch );
            pObj->value[0] = atoi( argument );
            break;
            case 1:
            send_to_char( "AC BASH SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
            case 2:
            send_to_char( "AC SLASH SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
            case 3:
            send_to_char( "AC EXOTIC SET.\n\r\n\r", ch );
            pObj->value[3] = atoi( argument );
            break;
        }
        break;

/* WEAPONS changed in ROM */

        case ITEM_WEAPON:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_WEAPON" );
                return FALSE;
            case 0:
            send_to_char( "WEAPON CLASS SET.\n\r\n\r", ch );
            ALT_FLAGVALUE_SET( pObj->value[0], weapon_class, argument );
            break;
            case 1:
                send_to_char( "NUMBER OF DICE SET.\n\r\n\r", ch );
                pObj->value[1] = atoi( argument );
                break;
            case 2:
                send_to_char( "TYPE OF DICE SET.\n\r\n\r", ch );
                pObj->value[2] = atoi( argument );
                break;
            case 3:
                send_to_char( "WEAPON TYPE SET.\n\r\n\r", ch );
                pObj->value[3] = attack_lookup( argument );
                break;
            case 4:
                    send_to_char( "SPECIAL WEAPON TYPE TOGGLED.\n\r\n\r", ch );
            ALT_FLAGVALUE_TOGGLE( pObj->value[4], weapon_type2, argument );
            break;
        }
            break;

    case ITEM_PORTAL:
        switch ( value_num )
        {
            default:
                do_help(ch, "ITEM_PORTAL" );
                return FALSE;

            case 0:
                send_to_char( "CHARGES SET.\n\r\n\r", ch);
                pObj->value[0] = atoi ( argument );
                break;
            case 1:
                send_to_char( "EXIT FLAGS SET.\n\r\n\r", ch);
            ALT_FLAGVALUE_SET( pObj->value[1], exit_flags, argument );
                break;
            case 2:
                send_to_char( "PORTAL FLAGS SET.\n\r\n\r", ch);
            ALT_FLAGVALUE_SET( pObj->value[2], portal_flags, argument );
                break;
            case 3:
                send_to_char( "EXIT VNUM SET.\n\r\n\r", ch);
                pObj->value[3] = atoi ( argument );
                break;
       }
       break;

    case ITEM_FURNITURE:
        switch ( value_num )
        {
            default:
                do_help( ch, "ITEM_FURNITURE" );
                return FALSE;

            case 0:
                send_to_char( "NUMBER OF PEOPLE SET.\n\r\n\r", ch);
                pObj->value[0] = atoi ( argument );
                break;
            case 1:
                send_to_char( "MAX WEIGHT SET.\n\r\n\r", ch);
                pObj->value[1] = atoi ( argument );
                break;
            case 2:
                send_to_char( "FURNITURE FLAGS TOGGLED.\n\r\n\r", ch);
            ALT_FLAGVALUE_TOGGLE( pObj->value[2], furniture_flags, argument );
                break;
            case 3:
                send_to_char( "HEAL BONUS SET.\n\r\n\r", ch);
                pObj->value[3] = atoi ( argument );
                break;
            case 4:
                send_to_char( "MANA BONUS SET.\n\r\n\r", ch);
                pObj->value[4] = atoi ( argument );
                break;
        }
        break;

    case ITEM_TRAP:
        switch ( value_num )
        {
            default:
                do_help( ch, "ITEM_TRAP" );
                return FALSE;

            case 0:
                send_to_char( "DAMAGE TYPE SET.\n\r\n\r", ch);
            ALT_FLAGVALUE_TOGGLE( pObj->value[0], trap_flags, argument );
                break;
            case 1:
                send_to_char( "TRAP TRIGGER SET.\n\r\n\r", ch);
            ALT_FLAGVALUE_TOGGLE( pObj->value[1], trigger_flags, argument );
                break;
            case 2:
                send_to_char( "TRAP TARGET SET.\n\r\n\r", ch);
            ALT_FLAGVALUE_TOGGLE( pObj->value[2], focus_flags, argument );
                break;
            case 3:
                send_to_char( "TRAP CHARGES SET.\n\r\n\r", ch);
                pObj->value[3] = atoi ( argument );
                break;
        }
        break;


        case ITEM_CONTAINER:
        switch ( value_num )
        {
        int value;

        default:
            do_help( ch, "ITEM_CONTAINER" );
                return FALSE;
        case 0:
                send_to_char( "WEIGHT CAPACITY SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
        case 1:
                if ( ( value = flag_value( container_flags, argument ) ) != NO_FLAG )
            TOGGLE_BIT(pObj->value[1], value);
            else
            {
            do_help ( ch, "ITEM_CONTAINER" );
            return FALSE;
            }
                send_to_char( "CONTAINER TYPE SET.\n\r\n\r", ch );
                break;
        case 2:
            if ( atoi(argument) != 0 )
            {
            if ( !get_obj_index( atoi( argument ) ) )
            {
                send_to_char( "THERE IS NO SUCH ITEM.\n\r\n\r", ch );
                return FALSE;
            }

            if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
            {
                send_to_char( "THAT ITEM IS NOT A KEY.\n\r\n\r", ch );
                return FALSE;
            }
            }
            send_to_char( "CONTAINER KEY SET.\n\r\n\r", ch );
            pObj->value[2] = atoi( argument );
            break;
        case 3:
            send_to_char( "CONTAINER MAX WEIGHT SET.\n\r", ch);
            pObj->value[3] = atoi( argument );
            break;
        case 4:
            send_to_char( "WEIGHT MULTIPLIER SET.\n\r\n\r", ch );
            pObj->value[4] = atoi ( argument );
            break;
        }
        break;

    case ITEM_DRINK_CON:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_DRINK" );
/* OLC          do_help( ch, "liquids" );    */
                return FALSE;
            case 0:
                send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
                pObj->value[1] = atoi( argument );
                break;
            case 2:
                send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
                pObj->value[2] = ( liq_lookup(argument) != -1 ?
                               liq_lookup(argument) : 0 );
                break;
            case 3:
                send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
                pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
                break;
        }
            break;

    case ITEM_FOUNTAIN:
        switch (value_num)
        {
            default:
            do_help( ch, "ITEM_FOUNTAIN" );
/* OLC          do_help( ch, "liquids" );    */
                return FALSE;
            case 0:
                send_to_char( "MAXIMUM AMOUT OF LIQUID HOURS SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "CURRENT AMOUNT OF LIQUID HOURS SET.\n\r\n\r", ch );
                pObj->value[1] = atoi( argument );
                break;
            case 2:
                send_to_char( "LIQUID TYPE SET.\n\r\n\r", ch );
                pObj->value[2] = ( liq_lookup( argument ) != -1 ?
                               liq_lookup( argument ) : 0 );
                break;
            }
    break;

    case ITEM_FOOD:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_FOOD" );
                return FALSE;
            case 0:
                send_to_char( "HOURS OF FOOD SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "HOURS OF FULL SET.\n\r\n\r", ch );
                pObj->value[1] = atoi( argument );
                break;
            case 3:
                send_to_char( "POISON VALUE TOGGLED.\n\r\n\r", ch );
                pObj->value[3] = ( pObj->value[3] == 0 ) ? 1 : 0;
                break;
        }
            break;

    case ITEM_MONEY:
        switch ( value_num )
        {
            default:
            do_help( ch, "ITEM_MONEY" );
                return FALSE;
            case 0:
                send_to_char( "GOLD AMOUNT SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
        case 1:
            send_to_char( "SILVER AMOUNT SET.\n\r\n\r", ch );
            pObj->value[1] = atoi( argument );
            break;
        }
            break;

    case ITEM_TOOL:
        switch (value_num)
        {
            default:
            do_help( ch, "ITEM_TOOL" );
                return FALSE;
            case 0:
                send_to_char( "TOOL QUALITY SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "TOOL TYPE SET.\n\r\n\r", ch );
                pObj->value[1] = ( tool_lookup( argument ) != -1 ?
                               tool_lookup( argument ) : 0 );
                break;
         }
         break;
    case ITEM_CRAFTED:
        switch (value_num)
        {
            default:
            do_help( ch, "ITEM_CRAFTED" );
                return FALSE;
            case 0:
                send_to_char( "CRAFTED QUALITY SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 1:
                send_to_char( "CRAFTED TYPE SET.\n\r\n\r", ch );
                pObj->value[1] = ( crafted_lookup( argument ) != -1 ?
                               crafted_lookup( argument ) : 0 );
                break;
            case 2:
                send_to_char( "SOCKET1 SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 3:
                send_to_char( "SOCKET2 SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;
            case 4:
                send_to_char( "SOCKET3 SET.\n\r\n\r", ch );
                pObj->value[0] = atoi( argument );
                break;

        }
        break;

    case ITEM_PLASM:
      switch (value_num)
      {
        default:
          do_help( ch, "ITEM_PLASM" );
          return FALSE;
        case 0:
          if (set_value < 0 || set_value > 10)
          {
            send_to_char("Plasm Pieces is between 0 and 10.\n\r", ch);
            break;
          }
          send_to_char( "PLASM PIECES SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
       }
       break;

   case ITEM_LOCKPICK:
     switch (value_num)
     {
       default:
         do_help( ch, "ITEM_LOCKPICK" );
         return FALSE;
       case 0:
        if (set_value < 0 || set_value > 10)
        {
          send_to_char("Lockpick Durability is between 0 and 10.\n\r", ch);
          break;
        }
         send_to_char( "LOCKPICK DURABILITY SET.\n\r\n\r", ch );
         pObj->value[0] = atoi( argument );
         break;
       case 1:
        if (set_value < -1 || set_value > 1)
        {
          send_to_char("Lockpick Difficulty is between -1 and 1.\n\r", ch);
          break;
        }
        send_to_char( "LOCKPICK DIFFICULTY SET.\n\r\n\r", ch );
        pObj->value[1] = atoi( argument );
        break;
      }
      break;

    case ITEM_BLASTPOWDER:
      switch (value_num)
      {
        default:
          do_help( ch, "ITEM_BLASTPOWDER" );
          return FALSE;
        case 0:
         if (set_value < 0 || set_value > 10)
         {
           send_to_char("Blast Powder Charges is between 0 and 10.\n\r", ch);
           break;
         }
         send_to_char( "BLASTPOWDER CHARGES SET.\n\r\n\r", ch );
          pObj->value[0] = atoi( argument );
          break;
        case 1:
          if (set_value < 3 || set_value > 10)
          {
           send_to_char("Blast Powder Damage is between 3 and 10.\n\r", ch);
           break;
          }
          send_to_char( "BLASTPOWDER DAMAGE SET.\n\r\n\r", ch );
           pObj->value[1] = atoi( argument );
          break;
         }
         break;

    }

    show_obj_values( ch, pObj );

    return TRUE;
}



OEDIT( oedit_show )
{
    OBJ_INDEX_DATA *pObj;
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *paf;
    PROG_LIST *list;
    int cnt;

    EDIT_OBJ(ch, pObj);

    sprintf( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
    pObj->name,
    !pObj->area ? -1        : pObj->area->vnum,
    !pObj->area ? "No Area" : pObj->area->name );
    send_to_char( buf, ch );


    sprintf( buf, "Vnum:        [%5d]\n\rType:        [%s]\n\r",
    pObj->vnum,
    flag_string( type_flags, pObj->item_type ) );
    send_to_char( buf, ch );

    sprintf( buf, "Level:       [%5d]\n\r", pObj->level );
    send_to_char( buf, ch );

    sprintf( buf, "Wear flags:  [%s]\n\r",
    flag_string( wear_flags, pObj->wear_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Extra flags: [%s]\n\r",
    flag_string( extra_flags, pObj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Material:    [%s]\n\r",                /* ROM */
    pObj->material );
    send_to_char( buf, ch );

    sprintf( buf, "Condition:   [%5d]\n\r",               /* ROM */
    pObj->condition );
    send_to_char( buf, ch );

    sprintf( buf, "Timer:       [%5d]\n\r",               /* UGHA */
    pObj->timer );
    send_to_char( buf, ch );

    sprintf( buf, "Size:        [%5s]\n\r",
    flag_string( size_flags, pObj->size ) );
    send_to_char( buf, ch );

    sprintf( buf, "Weight:      [%5d.%d]\n\rCost:        [%5d]\n\r",
    pObj->weight / 10, pObj->weight % 10, pObj->cost );
    send_to_char( buf, ch );

    if ( pObj->extra_descr )
    {
    EXTRA_DESCR_DATA *ed;

    send_to_char( "Ex desc kwd: ", ch );

    for ( ed = pObj->extra_descr; ed; ed = ed->next )
    {
        send_to_char( "[", ch );
        send_to_char( ed->keyword, ch );
        send_to_char( "]", ch );
    }

    send_to_char( "\n\r", ch );
    }

    sprintf( buf, "Short desc:  %s\n\rLong desc:\n\r     %s\n\r",
    pObj->short_descr, pObj->description );
    send_to_char( buf, ch );

    for ( cnt = 0, paf = pObj->affected; paf; paf = paf->next )
    {
    if ( cnt == 0 )
    {
        send_to_char( "Number Modifier Affects Bits\n\r", ch );
        send_to_char( "------ -------- ------- ----\n\r", ch );
    }
    sprintf( buf, "[%4d] %-8d %-7s %s\n\r", cnt,
        paf->modifier,
        flag_string( apply_flags, paf->location ), affect_bit_name(paf->bitvector));
    send_to_char( buf, ch );
    cnt++;
    }

    show_obj_values( ch, pObj );

    if ( pObj->oprogs )
    {
	int cnt;

	sprintf(buf, "\n\rOBJPrograms for [%5d]:\n\r", pObj->vnum);
	send_to_char( buf, ch );

	for (cnt=0, list=pObj->oprogs; list; list=list->next)
	{
		if (cnt ==0)
		{
			send_to_char ( " [P#]   Vnum  TRIGGER  'Phrase'\n\r", ch );
			send_to_char ( " ----  -----  -------  --------\n\r", ch );
		}

		sprintf(buf, " [%2d]  %5d  %7.7s  '%s'\n\r", cnt,
			list->vnum,prog_type_to_name(list->trig_type),
			list->trig_phrase);
		send_to_char( buf, ch );
		cnt++;
	}
    }
    return FALSE;
}


/*
 * oedit_copy function thanks to Zanthras of Mystical Realities MUD.
 */
OEDIT( oedit_copy )
{
    OBJ_INDEX_DATA *pObj;
    OBJ_INDEX_DATA *pObj2; /* The object to copy */
    int vnum, i;

    if ( argument[0] == '\0' )
    {
      send_to_char("Syntax: copy <vnum> \n\r",ch);
      return FALSE;
    }

    if ( !is_number(argument) )
    {
      send_to_char("OEdit: You must enter a number (vnum).\n\r",ch);
      return FALSE;
    }
    else /* argument is a number */
    {
      vnum = atoi(argument);
      if( !( pObj2 = get_obj_index(vnum) ) )
      {
	send_to_char("OEdit: That object does not exist.\n\r",ch);
	return FALSE;
      }
    }

    EDIT_OBJ(ch, pObj);

    free_string( pObj->name );
    pObj->name = str_dup( pObj2->name );

    pObj->item_type = pObj2->item_type;

    pObj->level = pObj2->level;

    pObj->wear_flags  = pObj2->wear_flags;
    pObj->extra_flags = pObj2->extra_flags;

    free_string( pObj->material );
    pObj->material = str_dup( pObj2->material );

    pObj->condition = pObj2->condition;

    pObj->weight = pObj2->weight;
    pObj->cost   = pObj2->cost;

    pObj->extra_descr = pObj2->extra_descr;

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( pObj2->short_descr );

    free_string( pObj->description );
    pObj->description = str_dup( pObj2->description );

    pObj->affected = pObj2->affected;

    for (i = 0; i < 5; i++)
    {
      pObj->value[i] = pObj2->value[i];
    }

    send_to_char( "Object info copied.", ch );
    return TRUE;
}


/*
 * Need to issue warning if flag isn't valid. -- does so now -- Hugin.
 */
OEDIT( oedit_addaffect )
{
    int value;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, loc );
    one_argument( argument, mod );

    if ( loc[0] == '\0' || mod[0] == '\0' || !is_number( mod ) )
    {
    send_to_char( "Syntax:  addaffect [location] [#xmod]\n\r", ch );
    return FALSE;
    }

    if ( ( value = flag_value( apply_flags, loc ) ) == NO_FLAG ) /* Hugin */
    {
        send_to_char( "Valid affects are:\n\r", ch );
    show_help( ch, "apply" );
    return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   atoi( mod );
    pAf->where      =   TO_OBJECT;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   0;
    pAf->level      =   pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Affect added.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_addapply )
{
    int value,bv,typ;
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    char loc[MAX_STRING_LENGTH];
    char mod[MAX_STRING_LENGTH];
    char type[MAX_STRING_LENGTH];
    char bvector[MAX_STRING_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, type );
    argument = one_argument( argument, loc );
    argument = one_argument( argument, mod );
    argument = one_argument( argument, bvector );

    if ( type[0] == '\0' || ( typ = flag_value( apply_types, type ) ) == NO_FLAG )
    {
        send_to_char( "Invalid apply type. Valid apply types are:\n\r", ch);
        show_help( ch, "apptype" );
        return FALSE;
    }

    if ( loc[0] == '\0' || ( value = flag_value( apply_flags, loc ) ) == NO_FLAG )
    {
        send_to_char( "Valid applys are:\n\r", ch );
    show_help( ch, "apply" );
    return FALSE;
    }

    if ( bvector[0] == '\0' || ( bv = flag_value( bitvector_type[typ].table, bvector ) ) == NO_FLAG )
    {
        send_to_char( "Invalid bitvector type.\n\r", ch );
    send_to_char( "Valid bitvector types are:\n\r", ch );
    show_help( ch, bitvector_type[typ].help );
        return FALSE;
    }

    if ( mod[0] == '\0' || !is_number( mod ) )
    {
    send_to_char( "Syntax:  addapply [type] [location] [#xmod] [bitvector]\n\r", ch );
    return FALSE;
    }

    pAf             =   new_affect();
    pAf->location   =   value;
    pAf->modifier   =   atoi( mod );
    pAf->where      =   apply_types[typ].bit;
    pAf->type       =   -1;
    pAf->duration   =   -1;
    pAf->bitvector  =   bv;
    pAf->level      =   pObj->level;
    pAf->next       =   pObj->affected;
    pObj->affected  =   pAf;

    send_to_char( "Apply added.\n\r", ch);
    return TRUE;
}

/*
 * My thanks to Hans Hvidsten Birkeland and Noam Krendel(Walker)
 * for really teaching me how to manipulate pointers.
 */
OEDIT( oedit_delaffect )
{
    OBJ_INDEX_DATA *pObj;
    AFFECT_DATA *pAf;
    AFFECT_DATA *pAf_next;
    char affect[MAX_STRING_LENGTH];
    int  value;
    int  cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, affect );

    if ( !is_number( affect ) || affect[0] == '\0' )
    {
    send_to_char( "Syntax:  delaffect [#xaffect]\n\r", ch );
    return FALSE;
    }

    value = atoi( affect );

    if ( value < 0 )
    {
    send_to_char( "Only non-negative affect-numbers allowed.\n\r", ch );
    return FALSE;
    }

    if ( !( pAf = pObj->affected ) )
    {
    send_to_char( "OEdit:  Non-existant affect.\n\r", ch );
    return FALSE;
    }

    if( value == 0 )    /* First case: Remove first affect */
    {
    pAf = pObj->affected;
    pObj->affected = pAf->next;
    free_affect( pAf );
    }
    else        /* Affect to remove is not the first */
    {
    while ( ( pAf_next = pAf->next ) && ( ++cnt < value ) )
         pAf = pAf_next;

    if( pAf_next )      /* See if it's the next affect */
    {
        pAf->next = pAf_next->next;
        free_affect( pAf_next );
    }
    else                                 /* Doesn't exist */
    {
         send_to_char( "No such affect.\n\r", ch );
         return FALSE;
    }
    }

    send_to_char( "Affect removed.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_name )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  name [string]\n\r", ch );
    return FALSE;
    }

    free_string( pObj->name );
    pObj->name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_short )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  short [string]\n\r", ch );
    return FALSE;
    }

    free_string( pObj->short_descr );
    pObj->short_descr = str_dup( argument );
    pObj->short_descr[0] = LOWER( pObj->short_descr[0] );

    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_long )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  long [string]\n\r", ch );
    return FALSE;
    }

    free_string( pObj->description );
    pObj->description = str_dup( argument );
    pObj->description[0] = UPPER( pObj->description[0] );

    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}



bool set_value( CHAR_DATA *ch, OBJ_INDEX_DATA *pObj, char *argument, int value )
{
    if ( argument[0] == '\0' )
    {
    set_obj_values( ch, pObj, -1, "" );     /* '\0' changed to "" -- Hugin */
    return FALSE;
    }

    if ( set_obj_values( ch, pObj, value, argument ) )
    return TRUE;

    return FALSE;
}



/*****************************************************************************
 Name:      oedit_values
 Purpose:   Finds the object and sets its value.
 Called by: The four valueX functions below. (now five -- Hugin )
 ****************************************************************************/
bool oedit_values( CHAR_DATA *ch, char *argument, int value )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( set_value( ch, pObj, argument, value ) )
        return TRUE;

    return FALSE;
}


OEDIT( oedit_value0 )
{
    if ( oedit_values( ch, argument, 0 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value1 )
{
    if ( oedit_values( ch, argument, 1 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value2 )
{
    if ( oedit_values( ch, argument, 2 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value3 )
{
    if ( oedit_values( ch, argument, 3 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_value4 )
{
    if ( oedit_values( ch, argument, 4 ) )
        return TRUE;

    return FALSE;
}



OEDIT( oedit_weight )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  weight [number]\n\r", ch );
    return FALSE;
    }

    pObj->weight = atoi( argument );

    send_to_char( "Weight set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_cost )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  cost [number]\n\r", ch );
    return FALSE;
    }

    pObj->cost = atoi( argument );

    send_to_char( "Cost set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_create )
{
    OBJ_INDEX_DATA *pObj;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
    send_to_char( "Syntax:  oedit create [vnum]\n\r", ch );
    return FALSE;
    }

    pArea = get_vnum_area( value );
    if ( !pArea )
    {
    send_to_char( "OEdit:  That vnum is not assigned an area.\n\r", ch );
    return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
    send_to_char( "OEdit:  Vnum in an area you cannot build in.\n\r", ch );
    return FALSE;
    }

    if ( get_obj_index( value ) )
    {
    send_to_char( "OEdit:  Object vnum already exists.\n\r", ch );
    return FALSE;
    }

    pObj            = new_obj_index();
    pObj->vnum          = value;
    pObj->area          = pArea;

    if ( value > top_vnum_obj )
    top_vnum_obj = value;

    iHash           = value % MAX_KEY_HASH;
    pObj->next          = obj_index_hash[iHash];
    obj_index_hash[iHash]   = pObj;
    ch->desc->pEdit     = (void *)pObj;

    send_to_char( "Object Created.\n\r", ch );
    return TRUE;
}



OEDIT( oedit_ed )
{
    OBJ_INDEX_DATA *pObj;
    EXTRA_DESCR_DATA *ed;
    char command[MAX_INPUT_LENGTH];
    char keyword[MAX_INPUT_LENGTH];

    EDIT_OBJ(ch, pObj);

    argument = one_argument( argument, command );
    one_argument( argument, keyword );

    if ( command[0] == '\0' )
    {
    send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
    send_to_char( "         ed delete [keyword]\n\r", ch );
    send_to_char( "         ed edit [keyword]\n\r", ch );
    send_to_char( "         ed format [keyword]\n\r", ch );
    return FALSE;
    }

    if ( !str_cmp( command, "add" ) )
    {
    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed add [keyword]\n\r", ch );
        return FALSE;
    }

    ed                  =   new_extra_descr();
    ed->keyword         =   str_dup( keyword );
    ed->next            =   pObj->extra_descr;
    pObj->extra_descr   =   ed;

    string_append( ch, &ed->description );

    return TRUE;
    }

    if ( !str_cmp( command, "edit" ) )
    {
    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed edit [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pObj->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
    }

    if ( !ed )
    {
        send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
        return FALSE;
    }

    string_append( ch, &ed->description );

    return TRUE;
    }

    if ( !str_cmp( command, "delete" ) )
    {
    EXTRA_DESCR_DATA *ped = NULL;

    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed delete [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pObj->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
        ped = ed;
    }

    if ( !ed )
    {
        send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
        return FALSE;
    }

    if ( !ped )
        pObj->extra_descr = ed->next;
    else
        ped->next = ed->next;

    free_extra_descr( ed );

    send_to_char( "Extra description deleted.\n\r", ch );
    return TRUE;
    }


    if ( !str_cmp( command, "format" ) )
    {
    EXTRA_DESCR_DATA *ped = NULL;

    if ( keyword[0] == '\0' )
    {
        send_to_char( "Syntax:  ed format [keyword]\n\r", ch );
        return FALSE;
    }

    for ( ed = pObj->extra_descr; ed; ed = ed->next )
    {
        if ( is_name( keyword, ed->keyword ) )
        break;
        ped = ed;
    }

    if ( !ed )
    {
                send_to_char( "OEdit:  Extra description keyword not found.\n\r", ch );
                return FALSE;
    }

    ed->description = format_string( ed->description );

    send_to_char( "Extra description formatted.\n\r", ch );
    return TRUE;
    }

    oedit_ed( ch, "" );
    return FALSE;
}





/* ROM object functions : */

OEDIT( oedit_extra )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_OBJ(ch, pObj);

    if ( ( value = flag_value( extra_flags, argument ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pObj->extra_flags, value);

        send_to_char( "Extra flag toggled.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax:  extra [flag]\n\r"
          "Type '? extra' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_wear )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

     if ( argument[0] != '\0' )
    {
    EDIT_OBJ(ch, pObj);

    if ( ( value = flag_value( wear_flags, argument ) ) != NO_FLAG )
    {
        TOGGLE_BIT(pObj->wear_flags, value);

        send_to_char( "Wear flag toggled.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax:  wear [flag]\n\r"
          "Type '? wear' for a list of flags.\n\r", ch );
    return FALSE;
}


OEDIT( oedit_type )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_OBJ(ch, pObj);

    if ( ( value = flag_value( type_flags, argument ) ) != NO_FLAG )
    {
        pObj->item_type = value;

        send_to_char( "Type set.\n\r", ch);

        /*
         * Clear the values.
         */
        pObj->value[0] = 0;
        pObj->value[1] = 0;
        pObj->value[2] = 0;
        pObj->value[3] = 0;
        pObj->value[4] = 0;     /* ROM */

        return TRUE;
    }
    }

    send_to_char( "Syntax:  type [flag]\n\r"
          "Type '? type' for a list of flags.\n\r", ch );
    return FALSE;
}


OEDIT( oedit_size )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_OBJ(ch, pObj);

    if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
    {
        pObj->size = value;

        send_to_char( "Size set.\n\r", ch);

        return TRUE;
    }
    }

    send_to_char( "Syntax:  size [flag]\n\r"
          "Type '? size' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT( oedit_material )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  material [string]\n\r", ch );
    return FALSE;
    }

    free_string( pObj->material );
    pObj->material = str_dup( argument );

    send_to_char( "Material set.\n\r", ch);
    return TRUE;
}

OEDIT( oedit_level )
{
    OBJ_INDEX_DATA *pObj;

    EDIT_OBJ(ch, pObj);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  level [number]\n\r", ch );
    return FALSE;
    }

    pObj->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}



OEDIT( oedit_condition )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0'
    && ( value = atoi (argument ) ) >= 0
    && ( value <= 100 ) )
    {
    EDIT_OBJ( ch, pObj );

    pObj->condition = value;
    send_to_char( "Condition set.\n\r", ch );

    return TRUE;
    }

    send_to_char( "Syntax:  condition [number]\n\r"
          "Where number can range from 0 (ruined) to 100 (perfect).\n\r",
          ch );
    return FALSE;
}


OEDIT( oedit_timer )
{
    OBJ_INDEX_DATA *pObj;
    int value;

    if ( argument[0] != '\0'
    && ( value = atoi (argument ) ) >= -1
    && ( value <= 30000 ) )
    {
    EDIT_OBJ( ch, pObj );

    pObj->timer = value;
    send_to_char( "Timer set.\n\r", ch );

    return TRUE;
    }

    send_to_char( "Syntax:  timer [number]\n\r"
          "Where number can range from -1 (forever) to 30000 (20.8 real days).\n\r",ch );
    return FALSE;
}




/*
 * Mobile Editor Functions.
 */
MEDIT( medit_show )
{
    MOB_INDEX_DATA *pMob;
    char buf[MAX_STRING_LENGTH];
    PROG_LIST *list;

    EDIT_MOB(ch, pMob);

    sprintf( buf, "Name:        [%s]\n\rArea:        [%5d] %s\n\r",
    pMob->player_name,
    !pMob->area ? -1        : pMob->area->vnum,
    !pMob->area ? "No Area" : pMob->area->name );
    send_to_char( buf, ch );

    sprintf( buf, "Act:         [%s]\n\r",
    flag_string( act_flags, pMob->act ) );
    send_to_char( buf, ch );

    sprintf( buf, "Act2:         [%s]\n\r",
    flag_string( act2_flags, pMob->act2 ) );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum:        [%5d] Sex:   [%s]   Race: [%s]\n\r",
    pMob->vnum,
    pMob->sex == SEX_MALE    ? "male   " :
    pMob->sex == SEX_FEMALE  ? "female " :
    pMob->sex == 3           ? "random " : "neutral",
    race_table[pMob->race].name );
    send_to_char( buf, ch );

    sprintf( buf,
          "Level:       [%2d]    Align: [%4d]      Hitroll: [%2d] Dam Type:    [%s]\n\r",
    pMob->level,    pMob->alignment,
    pMob->hitroll,  attack_table[pMob->dam_type].name );
    send_to_char( buf, ch );

    sprintf( buf, "Group:       [%3d -> %s]\n\r", mob_group_table[pMob->group].groupnumber, mob_group_table[pMob->group].name);
    send_to_char( buf, ch );

    sprintf( buf, "Hit dice:    [%2dd%-3d+%4d] ",
         pMob->hit[DICE_NUMBER],
         pMob->hit[DICE_TYPE],
         pMob->hit[DICE_BONUS] );
    send_to_char( buf, ch );

    sprintf( buf, "Damage dice: [%2dd%-3d+%4d] ",
         pMob->damage[DICE_NUMBER],
         pMob->damage[DICE_TYPE],
         pMob->damage[DICE_BONUS] );
    send_to_char( buf, ch );

    sprintf( buf, "Mana dice:   [%2dd%-3d+%4d]\n\r",
         pMob->mana[DICE_NUMBER],
         pMob->mana[DICE_TYPE],
         pMob->mana[DICE_BONUS] );
    send_to_char( buf, ch );
    printf_to_char(ch, "Max Load:    [%d]%s\n\r", pMob->maxload, pMob->maxload < 0 ? " (Use Resets MxA Limit)" : "" );

/* ROM values end */

    sprintf( buf, "Affected by: [%s]\n\r",
    flag_string( affect_flags, pMob->affected_by ) );
    send_to_char( buf, ch );

    sprintf( buf, "Affected2 by: [%s]\n\r",
    flag_string( affect2_flags, pMob->affected2_by ) );
    send_to_char( buf, ch );

/* ROM values: */

    sprintf( buf, "Armor:       [pierce: %d  bash: %d  slash: %d  magic: %d]\n\r",
    pMob->ac[AC_PIERCE], pMob->ac[AC_BASH],
    pMob->ac[AC_SLASH], pMob->ac[AC_EXOTIC] );
    send_to_char( buf, ch );

    sprintf( buf, "Form:        [%s]\n\r",
    flag_string( form_flags, pMob->form ) );
    send_to_char( buf, ch );

    sprintf( buf, "Parts:       [%s]\n\r",
    flag_string( part_flags, pMob->parts ) );
    send_to_char( buf, ch );

    sprintf( buf, "Imm:         [%s]\n\r",
    flag_string( imm_flags, pMob->imm_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Res:         [%s]\n\r",
    flag_string( res_flags, pMob->res_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Vuln:        [%s]\n\r",
    flag_string( vuln_flags, pMob->vuln_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Off:         [%s]\n\r",
    flag_string( off_flags,  pMob->off_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Size:        [%s]\n\r",
    flag_string( size_flags, pMob->size ) );
    send_to_char( buf, ch );

    sprintf( buf, "Material:    [%s]\n\r",
        pMob->material );
    send_to_char( buf, ch );

    sprintf( buf, "Start pos.   [%s]\n\r",
    flag_string( position_flags, pMob->start_pos ) );
    send_to_char( buf, ch );

    sprintf( buf, "Default pos  [%s]\n\r",
    flag_string( position_flags, pMob->default_pos ) );
    send_to_char( buf, ch );

    sprintf( buf, "Wealth:      [%5ld]\n\r",
    pMob->wealth );
    send_to_char( buf, ch );

    cprintf(ch, "Attr:        [%s]\n\r",
    flag_string( attr_flags, pMob->attr_flags ) );

    cprintf(ch, "Abil:        [%s]\n\r",
    flag_string( abil_flags, pMob->abil_flags ) );

/* ROM values end */

    if ( pMob->spec_fun )
    {
    sprintf( buf, "Spec fun:    [%s]\n\r",  spec_name( pMob->spec_fun ) );
    send_to_char( buf, ch );
    }

    sprintf( buf, "Short descr: %s\n\rLong descr:\n\r%s",
    pMob->short_descr,
    pMob->long_descr );
    send_to_char( buf, ch );

    sprintf( buf, "Description:\n\r%s", pMob->description );
    send_to_char( buf, ch );

    if ( pMob->pShop )
    {
    SHOP_DATA *pShop;
    int iTrade;

    pShop = pMob->pShop;

    sprintf( buf,
      "Shop data for [%5d]:\n\r"
      "  Markup for purchaser: %d%%\n\r"
      "  Markdown for seller:  %d%%\n\r",
        pShop->keeper, pShop->profit_buy, pShop->profit_sell );
    send_to_char( buf, ch );
    sprintf( buf, "  Hours: %d to %d.\n\r",
        pShop->open_hour, pShop->close_hour );
    send_to_char( buf, ch );

    for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
    {
        if ( pShop->buy_type[iTrade] != 0 )
        {
        if ( iTrade == 0 ) {
            send_to_char( "  Number Trades Type\n\r", ch );
            send_to_char( "  ------ -----------\n\r", ch );
        }
        sprintf( buf, "  [%4d] %s\n\r", iTrade,
            flag_string( type_flags, pShop->buy_type[iTrade] ) );
        send_to_char( buf, ch );
        }
    }
    }

    if ( pMob->mprogs )
    {
    int cnt;

    sprintf(buf, "\n\rMOBPrograms for [%5d]:\n\r", pMob->vnum);
    send_to_char( buf, ch );

    for (cnt=0, list=pMob->mprogs; list; list=list->next)
    {
        if (cnt ==0)
        {
            send_to_char ( "  [P#]   Vnum  TRIGGER  'Phrase'\n\r", ch );
            send_to_char ( "  ----  -----  -------  --------\n\r", ch );
        }

        sprintf(buf, "  [%2d]  %5d  %7.7s  '%s'\n\r", cnt,
            list->vnum,prog_type_to_name(list->trig_type),
            list->trig_phrase);
        send_to_char( buf, ch );
        cnt++;
    }
    }

    return FALSE;
}



MEDIT( medit_create )
{
    MOB_INDEX_DATA *pMob;
    AREA_DATA *pArea;
    int  value;
    int  iHash;

    value = atoi( argument );
    if ( argument[0] == '\0' || value == 0 )
    {
    send_to_char( "Syntax:  medit create [vnum]\n\r", ch );
    return FALSE;
    }

    pArea = get_vnum_area( value );

    if ( !pArea )
    {
    send_to_char( "MEdit:  That vnum is not assigned an area.\n\r", ch );
    return FALSE;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
    send_to_char( "MEdit:  Vnum in an area you cannot build in.\n\r", ch );
    return FALSE;
    }

    if ( get_mob_index( value ) )
    {
    send_to_char( "MEdit:  Mobile vnum already exists.\n\r", ch );
    return FALSE;
    }

    pMob            = new_mob_index();
    pMob->vnum          = value;
    pMob->area          = pArea;

    if ( value > top_vnum_mob )
    top_vnum_mob = value;

    pMob->act           = ACT_IS_NPC;
    pMob->act2          = ACT2_IS_NPC;
    iHash           = value % MAX_KEY_HASH;
    pMob->next          = mob_index_hash[iHash];
    mob_index_hash[iHash]   = pMob;
    ch->desc->pEdit     = (void *)pMob;

    send_to_char( "Mobile Created.\n\r", ch );
    return TRUE;
}



MEDIT( medit_spec )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  spec [special function]\n\r", ch );
    return FALSE;
    }


    if ( !str_cmp( argument, "none" ) )
    {
        pMob->spec_fun = NULL;

        send_to_char( "Spec removed.\n\r", ch);
        return TRUE;
    }

    if ( spec_lookup( argument ) )
    {
    pMob->spec_fun = spec_lookup( argument );
    send_to_char( "Spec set.\n\r", ch);
    return TRUE;
    }

    send_to_char( "MEdit: No such special function.\n\r", ch );
    return FALSE;
}

MEDIT( medit_damtype )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  damtype [damage message]\n\r", ch );
    send_to_char( "For a list of damage messages, enter '? weapon'.\n\r", ch );
    return FALSE;
    }

    pMob->dam_type = attack_lookup(argument);
    send_to_char( "Damage type set.\n\r", ch);
    return TRUE;
}


MEDIT( medit_align )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    send_to_char( "Alignment no loger used, left unset.\n\r", ch);
    return FALSE;
}



MEDIT( medit_level )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  level [number]\n\r", ch );
    return FALSE;
    }

    pMob->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}
MEDIT( medit_maxload )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  maxload [number]\n\r", ch );
    return FALSE;
    }

    pMob->maxload = atoi( argument );
    if (pMob->maxload < -1)
        pMob->maxload = -1;

    send_to_char( "Maxload set.\n\r", ch);
    return TRUE;
}


MEDIT( medit_desc )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    string_append( ch, &pMob->description );
    return TRUE;
    }

    send_to_char( "Syntax:  desc    - line edit\n\r", ch );
    return FALSE;
}




MEDIT( medit_long )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  long [string]\n\r", ch );
    return FALSE;
    }

    free_string( pMob->long_descr );
    strcat( argument, "\n\r" );
    pMob->long_descr = str_dup( argument );
    pMob->long_descr[0] = UPPER( pMob->long_descr[0]  );

    send_to_char( "Long description set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_short )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  short [string]\n\r", ch );
    return FALSE;
    }

    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( argument );

    send_to_char( "Short description set.\n\r", ch);
    return TRUE;
}



MEDIT( medit_name )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  name [string]\n\r", ch );
    return FALSE;
    }

    free_string( pMob->player_name );
    pMob->player_name = str_dup( argument );

    send_to_char( "Name set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_shop )
{
    MOB_INDEX_DATA *pMob;
    char command[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];

    argument = one_argument( argument, command );
    argument = one_argument( argument, arg1 );

    EDIT_MOB(ch, pMob);

    if ( command[0] == '\0' )
    {
    send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
    send_to_char( "         shop profit [#xbuying%] [#xselling%]\n\r", ch );
    send_to_char( "         shop type [#x0-4] [item type]\n\r", ch );
    send_to_char( "         shop assign\n\r", ch );
    send_to_char( "         shop remove\n\r", ch );
    return FALSE;
    }


    if ( !str_cmp( command, "hours" ) )
    {
    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Syntax:  shop hours [#xopening] [#xclosing]\n\r", ch );
        return FALSE;
    }

    if ( !pMob->pShop )
    {
        send_to_char( "MEdit:  You must first create a shop. (shop assign).\n\r", ch );
        return FALSE;
    }

    pMob->pShop->open_hour = atoi( arg1 );
    pMob->pShop->close_hour = atoi( argument );

    send_to_char( "Shop hours set.\n\r", ch);
    return TRUE;
    }


    if ( !str_cmp( command, "profit" ) )
    {
    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' || !is_number( argument ) )
    {
        send_to_char( "Syntax:  shop profit [#xbuying%] [#xselling%]\n\r", ch );
        return FALSE;
    }

    if ( !pMob->pShop )
    {
        send_to_char( "MEdit:  You must first create a shop. (shop assign).\n\r", ch );
        return FALSE;
    }

    pMob->pShop->profit_buy     = atoi( arg1 );
    pMob->pShop->profit_sell    = atoi( argument );

    send_to_char( "Shop profit set.\n\r", ch);
    return TRUE;
    }


    if ( !str_cmp( command, "type" ) )
    {
    char buf[MAX_INPUT_LENGTH];
    int value;

    if ( arg1[0] == '\0' || !is_number( arg1 )
    || argument[0] == '\0' )
    {
        send_to_char( "Syntax:  shop type [#x0-4] [item type]\n\r", ch );
        return FALSE;
    }

    if ( atoi( arg1 ) >= MAX_TRADE )
    {
        sprintf( buf, "MEdit:  May sell %d items max.\n\r", MAX_TRADE );
        send_to_char( buf, ch );
        return FALSE;
    }

    if ( !pMob->pShop )
    {
        send_to_char( "MEdit:  You must first create a shop. (shop assign).\n\r", ch );
        return FALSE;
    }

    if ( ( value = flag_value( type_flags, argument ) ) == NO_FLAG )
    {
        send_to_char( "MEdit:  That type of item is not known.\n\r", ch );
        return FALSE;
    }

    pMob->pShop->buy_type[atoi( arg1 )] = value;

    send_to_char( "Shop type set.\n\r", ch);
    return TRUE;
    }

    /* shop assign && shop delete by Phoenix */

    if ( !str_prefix(command, "assign") )
    {
        if ( pMob->pShop )
        {
            send_to_char("Mob already has a shop assigned to it.\n\r", ch);
            return FALSE;
    }

    pMob->pShop     = new_shop();
    if ( !shop_first )
            shop_first  = pMob->pShop;
    if ( shop_last )
        shop_last->next = pMob->pShop;
    shop_last       = pMob->pShop;

    pMob->pShop->keeper = pMob->vnum;

    send_to_char("New shop assigned to mobile.\n\r", ch);
    return TRUE;
    }

    if ( !str_prefix(command, "remove") )
    {
    SHOP_DATA *pShop;

    pShop       = pMob->pShop;
    pMob->pShop = NULL;

    if ( pShop == shop_first )
    {
        if ( !pShop->next )
        {
            shop_first = NULL;
            shop_last = NULL;
        }
        else
            shop_first = pShop->next;
    }
    else
    {
        SHOP_DATA *ipShop;

        for ( ipShop = shop_first; ipShop; ipShop = ipShop->next )
        {
            if ( ipShop->next == pShop )
            {
                if ( !pShop->next )
                {
                    shop_last = ipShop;
                    shop_last->next = NULL;
                }
                else
                    ipShop->next = pShop->next;
            }
        }
    }

    free_shop(pShop);

    send_to_char("Mobile is no longer a shopkeeper.\n\r", ch);
    return TRUE;
    }

    medit_shop( ch, "" );
    return FALSE;
}


/*
 * medit_copy function thanks to Zanthras of Mystical Realities MUD.
 * Thanks to Ivan for what there is of the incomplete mobprog part.
 * Hopefully it will be finished in a later release of this snippet.
 */
MEDIT( medit_copy )
{
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMob2; /* The mob to copy */
    int vnum;

    /* PROG_LIST *list; */ /* Used for the mob prog for loop */

    if ( argument[0] == '\0' )
    {
      send_to_char("Syntax: copy <vnum> \n\r",ch);
      return FALSE;
    }

    if ( !is_number(argument) )
    {
      send_to_char("MEdit: You must enter a number (vnum).\n\r",ch);
      return FALSE;
    }
    else /* argument is a number */
    {
      vnum = atoi(argument);
      if( !( pMob2 = get_mob_index(vnum) ) )
      {
	send_to_char("MEdit: That mob does not exist.\n\r",ch);
	return FALSE;
      }
    }

    EDIT_MOB(ch, pMob);

    free_string( pMob->player_name );
    pMob->player_name = str_dup( pMob2->player_name );

    pMob->new_format = pMob2->new_format;
    pMob->act = pMob2->act;
    pMob->sex = pMob2->sex;

    pMob->race = pMob2->race;

    pMob->level = pMob2->level;

    pMob->alignment = pMob2->alignment;

    pMob->hitroll = pMob2->hitroll;

    pMob->dam_type = pMob2->dam_type;

    pMob->group = pMob2->group;

    pMob->hit[DICE_NUMBER] = pMob2->hit[DICE_NUMBER];
    pMob->hit[DICE_TYPE]   = pMob2->hit[DICE_TYPE];
    pMob->hit[DICE_BONUS]  = pMob2->hit[DICE_BONUS];

    pMob->damage[DICE_NUMBER] = pMob2->damage[DICE_NUMBER];
    pMob->damage[DICE_TYPE]   = pMob2->damage[DICE_TYPE];
    pMob->damage[DICE_BONUS]  = pMob2->damage[DICE_BONUS];

    pMob->mana[DICE_NUMBER] = pMob2->mana[DICE_NUMBER];
    pMob->mana[DICE_TYPE]   = pMob2->mana[DICE_TYPE];
    pMob->mana[DICE_BONUS]  = pMob2->mana[DICE_BONUS];

    pMob->affected_by = pMob2->affected_by;

    pMob->ac[AC_PIERCE] = pMob2->ac[AC_PIERCE];
    pMob->ac[AC_BASH]   = pMob2->ac[AC_BASH];
    pMob->ac[AC_SLASH]  = pMob2->ac[AC_SLASH];
    pMob->ac[AC_EXOTIC] = pMob2->ac[AC_EXOTIC];


    pMob->form  = pMob2->form;
    pMob->parts = pMob2->parts;

    pMob->imm_flags  = pMob2->imm_flags;
    pMob->res_flags  = pMob2->res_flags;
    pMob->vuln_flags = pMob2->vuln_flags;
    pMob->off_flags  = pMob2->off_flags;
    pMob->attr_flags = pMob2->attr_flags;
    pMob->abil_flags = pMob2->abil_flags;

    pMob->size     = pMob2->size;

    free_string( pMob->material );
    pMob->material = str_dup( pMob2->material );

    pMob->start_pos   = pMob2->start_pos;
    pMob->default_pos = pMob2->default_pos;

    pMob->wealth = pMob2->wealth;

    pMob->spec_fun = pMob2->spec_fun;

    free_string( pMob->short_descr );
    pMob->short_descr = str_dup( pMob2->short_descr );

    free_string( pMob->long_descr );
    pMob->long_descr = str_dup( pMob2->long_descr   );

    free_string( pMob->description );
    pMob->description = str_dup( pMob2->description );

    /* Hopefully get the shop data to copy later
     * This is the fields here if you get it copying send me
     * a copy and I'll add it to the snippet with credit to
     * you of course :) same with the mobprogs for loop :)
     */

/*
    if ( pMob->pShop )
    {
	SHOP_DATA *pShop, *pShop2;

	pShop =  pMob->pShop;
	pShop2 = pMob2->pShop;

	pShop->profit_buy = pShop2->profit_buy;
	pShop->profit_sell = pShop2->profit_sell;

	pShop->open_hour = pShop2->open_hour;
	pShop->close_hour = pShop2->close_hour;

	pShop->buy_type[iTrade] = pShop2->buy_type[iTrade];
    }
*/
/*  for loop thanks to Ivan, still needs work though

    for (list = pMob->mprogs; list; list = list->next )
    {
      PROG_LIST *newp = new_mprog();
      newp->trig_type = list->trig_type;

      free_string( newp->trig_phrase );
      newp->trig_phrase = str_dup( list->trig_phrase );

      newp->vnum = list->vnum;

      free_string( newp->code )
      newp->code = str_dup( list->code );

      pMob->mprogs = newp;
    }
*/

    send_to_char( "Mob info copied.", ch );
    return FALSE;
}


/* ROM medit functions: */


MEDIT( medit_sex )          /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( sex_flags, argument ) ) != NO_FLAG )
    {
        pMob->sex = value;

        send_to_char( "Sex set.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax: sex [sex]\n\r"
          "Type '? sex' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_act )          /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( act_flags, argument ) ) != NO_FLAG )
    {
        pMob->act ^= value;
        SET_BIT( pMob->act, ACT_IS_NPC );

        send_to_char( "Act flag toggled.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax: act [flag]\n\r"
          "Type '? act' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_act2 )          /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;
    char buf[MSL];

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( act2_flags, argument ) ) != NO_FLAG )
    {
        pMob->act2 ^= value;
        SET_BIT( pMob->act2, ACT2_IS_NPC );

        send_to_char( "Act2 flag toggled.\n\r", ch);
        sprintf(buf, "Flagged: %s\n\r", act2_bit_name(pMob->act2));
        sendch(buf, ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax: act2 [flag]\n\r"
          "Type '? act2' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_affect )      /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( affect_flags, argument ) ) != NO_FLAG )
    {
        pMob->affected_by ^= value;

        send_to_char( "Affect flag toggled.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax: affect [flag]\n\r"
          "Type '? affect' for a list of flags.\n\r", ch );
    return FALSE;
}
MEDIT( medit_affect2 )      /* Moved out of medit() due to naming conflicts -- Hugin */
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( affect2_flags, argument ) ) != NO_FLAG )
    {
        pMob->affected2_by ^= value;

        send_to_char( "Affect2 flag toggled.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax: affect [flag]\n\r"
          "Type '? affect' for a list of flags.\n\r", ch );
    return FALSE;
}



MEDIT( medit_ac )
{
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int pierce, bash, slash, exotic;

    do   /* So that I can use break and send the syntax in one place */
    {
    if ( argument[0] == '\0' )  break;

    EDIT_MOB(ch, pMob);
    argument = one_argument( argument, arg );

    if ( !is_number( arg ) )  break;
    pierce = atoi( arg );
    argument = one_argument( argument, arg );

    if ( arg[0] != '\0' )
    {
        if ( !is_number( arg ) )  break;
        bash = atoi( arg );
        argument = one_argument( argument, arg );
    }
    else
        bash = pMob->ac[AC_BASH];

    if ( arg[0] != '\0' )
    {
        if ( !is_number( arg ) )  break;
        slash = atoi( arg );
        argument = one_argument( argument, arg );
    }
    else
        slash = pMob->ac[AC_SLASH];

    if ( arg[0] != '\0' )
    {
        if ( !is_number( arg ) )  break;
        exotic = atoi( arg );
    }
    else
        exotic = pMob->ac[AC_EXOTIC];

    pMob->ac[AC_PIERCE] = pierce;
    pMob->ac[AC_BASH]   = bash;
    pMob->ac[AC_SLASH]  = slash;
    pMob->ac[AC_EXOTIC] = exotic;

    send_to_char( "Ac set.\n\r", ch );
    return TRUE;
    } while ( FALSE );    /* Just do it once.. */

    send_to_char( "Syntax:  ac [ac-pierce [ac-bash [ac-slash [ac-exotic]]]]\n\r"
          "help MOB_AC  gives a list of reasonable ac-values.\n\r", ch );
    return FALSE;
}

MEDIT( medit_form )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( form_flags, argument ) ) != NO_FLAG )
    {
        pMob->form ^= value;
        send_to_char( "Form toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: form [flags]\n\r"
          "Type '? form' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_part )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( part_flags, argument ) ) != NO_FLAG )
    {
        pMob->parts ^= value;
        send_to_char( "Parts toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: part [flags]\n\r"
          "Type '? part' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_imm )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( imm_flags, argument ) ) != NO_FLAG )
    {
        pMob->imm_flags ^= value;
        send_to_char( "Immunity toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: imm [flags]\n\r"
          "Type '? imm' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_res )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( res_flags, argument ) ) != NO_FLAG )
    {
        pMob->res_flags ^= value;
        send_to_char( "Resistance toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: res [flags]\n\r"
          "Type '? res' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_vuln )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( vuln_flags, argument ) ) != NO_FLAG )
    {
        pMob->vuln_flags ^= value;
        send_to_char( "Vulnerability toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: vuln [flags]\n\r"
          "Type '? vuln' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_material )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  material [string]\n\r", ch );
    return FALSE;
    }

    free_string( pMob->material );
    pMob->material = str_dup( argument );

    send_to_char( "Material set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_off )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( off_flags, argument ) ) != NO_FLAG )
    {
        pMob->off_flags ^= value;
        send_to_char( "Offensive behaviour toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: off [flags]\n\r"
          "Type '? off' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_size )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( size_flags, argument ) ) != NO_FLAG )
    {
        pMob->size = value;
        send_to_char( "Size set.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: size [size]\n\r"
          "Type '? size' for a list of sizes.\n\r", ch );
    return FALSE;
}

MEDIT( medit_hitdice )
{
    static char syntax[] = "Syntax:  hitdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 )
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    pMob->hit[DICE_NUMBER] = atoi( num   );
    pMob->hit[DICE_TYPE]   = atoi( type  );
    pMob->hit[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Hitdice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_manadice )
{
    static char syntax[] = "Syntax:  manadice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 )
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    pMob->mana[DICE_NUMBER] = atoi( num   );
    pMob->mana[DICE_TYPE]   = atoi( type  );
    pMob->mana[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Manadice set.\n\r", ch );
    return TRUE;
}

MEDIT( medit_damdice )
{
    static char syntax[] = "Syntax:  damdice <number> d <type> + <bonus>\n\r";
    char *num, *type, *bonus, *cp;
    MOB_INDEX_DATA *pMob;

    EDIT_MOB( ch, pMob );

    if ( argument[0] == '\0' )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    num = cp = argument;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) )  *(cp++) = '\0';

    type = cp;

    while ( isdigit( *cp ) ) ++cp;
    while ( *cp != '\0' && !isdigit( *cp ) ) *(cp++) = '\0';

    bonus = cp;

    while ( isdigit( *cp ) ) ++cp;
    if ( *cp != '\0' ) *cp = '\0';

    if ( !( is_number( num ) && is_number( type ) && is_number( bonus ) ) )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    if ( ( !is_number( num   ) || atoi( num   ) < 1 )
    ||   ( !is_number( type  ) || atoi( type  ) < 1 )
    ||   ( !is_number( bonus ) || atoi( bonus ) < 0 ) )
    {
    send_to_char( syntax, ch );
    return FALSE;
    }

    pMob->damage[DICE_NUMBER] = atoi( num   );
    pMob->damage[DICE_TYPE]   = atoi( type  );
    pMob->damage[DICE_BONUS]  = atoi( bonus );

    send_to_char( "Damdice set.\n\r", ch );
    return TRUE;
}


MEDIT( medit_race )
{
    MOB_INDEX_DATA *pMob;
    int race;

    if ( argument[0] != '\0'
    && ( race = race_lookup( argument ) ) != 0 )
    {
    EDIT_MOB( ch, pMob );

    pMob->race = race;
    pMob->act     |= race_table[race].act;
    pMob->affected_by |= race_table[race].aff;
    pMob->off_flags   |= race_table[race].off;
    pMob->imm_flags   |= race_table[race].imm;
    pMob->res_flags   |= race_table[race].res;
    pMob->vuln_flags  |= race_table[race].vuln;
    pMob->form        |= race_table[race].form;
    pMob->parts       |= race_table[race].parts;

    send_to_char( "Race set.\n\r", ch );
    return TRUE;
    }

    if ( argument[0] == '?' )
    {
    char buf[MAX_STRING_LENGTH];

    send_to_char( "Available races are:", ch );

    for ( race = 0; race_table[race].name != NULL; race++ )
    {
        if ( ( race % 3 ) == 0 )
        send_to_char( "\n\r", ch );
        sprintf( buf, " %-15s", race_table[race].name );
        send_to_char( buf, ch );
    }

    send_to_char( "\n\r", ch );
    return FALSE;
    }

    send_to_char( "Syntax:  race [race]\n\r"
          "Type 'race ?' for a list of races.\n\r", ch );
    return FALSE;
}


MEDIT( medit_position )
{
    MOB_INDEX_DATA *pMob;
    char arg[MAX_INPUT_LENGTH];
    int value;

    argument = one_argument( argument, arg );

    switch ( arg[0] )
    {
    default:
    break;

    case 'S':
    case 's':
    if ( str_prefix( arg, "start" ) )
        break;

    if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
        break;

    EDIT_MOB( ch, pMob );

    pMob->start_pos = value;
    send_to_char( "Start position set.\n\r", ch );
    return TRUE;

    case 'D':
    case 'd':
    if ( str_prefix( arg, "default" ) )
        break;

    if ( ( value = flag_value( position_flags, argument ) ) == NO_FLAG )
        break;

    EDIT_MOB( ch, pMob );

    pMob->default_pos = value;
    send_to_char( "Default position set.\n\r", ch );
    return TRUE;
    }

    send_to_char( "Syntax:  position [start/default] [position]\n\r"
          "Type '? position' for a list of positions.\n\r", ch );
    return FALSE;
}


MEDIT( medit_gold )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  wealth [number]\n\r", ch );
    return FALSE;
    }

    pMob->wealth = atoi( argument );

    send_to_char( "Wealth set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_hitroll )
{
    MOB_INDEX_DATA *pMob;

    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  hitroll [number]\n\r", ch );
    return FALSE;
    }

    pMob->hitroll = atoi( argument );

    send_to_char( "Hitroll set.\n\r", ch);
    return TRUE;
}

MEDIT( medit_attrflags )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( attr_flags, argument ) ) != NO_FLAG )
    {
        pMob->attr_flags ^= value;
        send_to_char( "Attribute Modifier toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: attrflags [flags]\n\r"
          "Type '? attrflags' for a list of flags.\n\r", ch );
    return FALSE;
}

MEDIT( medit_abilflags )
{
    MOB_INDEX_DATA *pMob;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_MOB( ch, pMob );

    if ( ( value = flag_value( abil_flags, argument ) ) != NO_FLAG )
    {
        pMob->abil_flags ^= value;
        send_to_char( "Ability Modifier toggled.\n\r", ch );
        return TRUE;
    }
    }

    send_to_char( "Syntax: abilflags [flags]\n\r"
          "Type '? abilflags' for a list of flags.\n\r", ch );
    return FALSE;
}

void show_liqlist(CHAR_DATA *ch)
{
    int liq;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
    {
    if ( (liq % 21) == 0 )
        add_buf(buffer,"Name                 Color          Proof Full Thirst Food Ssize\n\r");

    sprintf(buf, "%-20s %-14s %5d %4d %6d %4d %5d\n\r",
        liq_table[liq].liq_name,liq_table[liq].liq_color,
        liq_table[liq].liq_affect[0],liq_table[liq].liq_affect[1],
        liq_table[liq].liq_affect[2],liq_table[liq].liq_affect[3],
        liq_table[liq].liq_affect[4] );
    add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}

void show_toollist(CHAR_DATA *ch)
{
    int tool;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for ( tool = 0; tool_table[tool].tool_name != NULL; tool++)
    {
    if ( (tool % 21) == 0 )
        add_buf(buffer,"Name\n\r");

    sprintf(buf, "%-20s\n\r",
        tool_table[tool].tool_name);
    add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}

void show_craftedlist(CHAR_DATA *ch)
{
    int crafted;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for ( crafted = 0; crafted_table[crafted].crafted_name != NULL; crafted++)
    {
    if ( (crafted % 21) == 0 )
        add_buf(buffer,"Name\n\r");

    sprintf(buf, "%-20s\n\r",
        crafted_table[crafted].crafted_name);
    add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}


void show_damlist(CHAR_DATA *ch)
{
    int att;
    BUFFER *buffer;
    char buf[MAX_STRING_LENGTH];

    buffer = new_buf();

    for ( att = 0; attack_table[att].name != NULL; att++)
    {
    if ( (att % 21) == 0 )
        add_buf(buffer,"Name                 Noun\n\r");

    sprintf(buf, "%-20s %-20s\n\r",
        attack_table[att].name,attack_table[att].noun );
    add_buf(buffer,buf);
    }

    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);

return;
}

MEDIT( medit_group )
{
    MOB_INDEX_DATA *pMob;
    MOB_INDEX_DATA *pMTemp;
    char arg[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int temp;
    BUFFER *buffer;
    bool found = FALSE;
    int gt;
    EDIT_MOB(ch, pMob);

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: group <#>\n\r", ch);
        send_to_char( "        group show <#>\n\r", ch);
        send_to_char( "        group list\n\r", ch);
        return FALSE;
    }

    if (is_number(argument))
    {
        gt = atoi(argument);
        if (gt >= MAX_MOBGROUP)
        {
            sprintf(buf, "Group # must be between [ {Y0 {D- {Y%d {x].\n\r", MAX_MOBGROUP - 1);
            send_to_char(buf, ch);
            return FALSE;
        }

        if (mob_group_table[gt].raceonly)
            if (pMob->race != race_lookup(mob_group_table[gt].race1)
                && pMob->race != race_lookup(mob_group_table[gt].race2)
                && pMob->race != race_lookup(mob_group_table[gt].race3))
            {
                sprintf(buf, "Group [{y%s{x] can only be assigned to these races:\n\r     '%s' '%s' '%s'\n\r", 
                    mob_group_table[gt].name, mob_group_table[gt].race1, mob_group_table[gt].race2, mob_group_table[gt].race3);
                send_to_char(buf, ch);
                return FALSE;
            }
        
        pMob->group = atoi(argument);
        sprintf(buf, "Group Set:  [ {Y%3d{x ] -> %20s \n\r", mob_group_table[pMob->group].groupnumber,
            mob_group_table[pMob->group].name);
        send_to_char(buf, ch);
        return TRUE;
    }

    if ( !strcmp( argument, "list" ))
    {
        send_to_char("[ {Y # {x ] ->           Group Name        [ {Y # {x ] ->           Group Name        \n\r", ch);
        send_to_char("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\r", ch);
        for (temp = 0; temp < MAX_MOBGROUP; temp++)
        {
            sprintf(buf, "[ {Y%3d{x ] -> %20s        %s", mob_group_table[temp].groupnumber, mob_group_table[temp].name,
                temp % 2 != 0 ? "\n\r" : "");
            send_to_char(buf, ch);
        }
        send_to_char( "\n\r\n\r{WTo set group: {cgroup #{x\n\r", ch);
        return FALSE;
    }

    argument = one_argument( argument, arg );

    if ( !strcmp( arg, "show" ) && is_number( argument ) )
    {

        if (atoi(argument) == 0)
        {
            send_to_char( "Are you crazy?\n\r", ch);
            return FALSE;
        }

        buffer = new_buf ();

        for (temp = 0; temp < 65536; temp++)
        {
            pMTemp = get_mob_index(temp);
            if ( pMTemp && ( pMTemp->group == atoi(argument) ) )
            {
            found = TRUE;
            sprintf( buf, "[%5d]:  %s\n\r", pMTemp->vnum, pMTemp->short_descr );
            add_buf( buffer, buf );
            }
        }

        if (found)
        {
            send_to_char("[ Vnum]:  Mob\n\r-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n\r", ch);
            page_to_char( buf_string(buffer), ch );
        }
        else
            send_to_char( "There are no mobs in that group.\n\r", ch );

        free_buf( buffer );
        return FALSE;
    }

    return FALSE;
}

REDIT( redit_owner )
{
    ROOM_INDEX_DATA *pRoom;

    EDIT_ROOM(ch, pRoom);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  owner [owner]\n\r", ch );
    send_to_char( "         owner none\n\r", ch );
    return FALSE;
    }

    free_string( pRoom->owner );
    if (!str_cmp(argument, "none"))
        pRoom->owner = str_dup("");
    else
    pRoom->owner = str_dup( argument );

    send_to_char( "Owner set.\n\r", ch );
    return TRUE;
}

MEDIT ( medit_addmprog )
{
  int value;
  MOB_INDEX_DATA *pMob;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_MOB(ch, pMob);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addmprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (mprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "mprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num) , PRG_MPROG) ) == NULL)
  {
        send_to_char("No such MOBProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_mprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pMob->mprog_flags,value);
  list->next            = pMob->mprogs;
  pMob->mprogs          = list;

  send_to_char( "Mprog Added.\n\r",ch);
  return TRUE;
}

MEDIT ( medit_delmprog )
{
    MOB_INDEX_DATA *pMob;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char mprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_MOB(ch, pMob);

    one_argument( argument, mprog );
    if (!is_number( mprog ) || mprog[0] == '\0' )
    {
       send_to_char("Syntax:  delmprog [#mprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( mprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative mprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pMob->mprogs) )
    {
        send_to_char("MEdit:  Non existant mprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
    REMOVE_BIT(pMob->mprog_flags, pMob->mprogs->trig_type);
        list = pMob->mprogs;
        pMob->mprogs = list->next;
        free_mprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
        REMOVE_BIT(pMob->mprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_mprog(list_next);
        }
        else
        {
                send_to_char("No such mprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Mprog removed.\n\r", ch);
    return TRUE;
}

REDIT( redit_room )
{
    ROOM_INDEX_DATA *room;
    int value;

    EDIT_ROOM(ch, room);

    if ( (value = flag_value( room_flags, argument )) == NO_FLAG )
    {
        send_to_char( "Sintaxis: room [flags]\n\r", ch );
        return FALSE;
    }

        TOGGLE_BIT(room->room_flags, value);
    send_to_char( "Room flags toggled.\n\r", ch );
        return TRUE;
}

REDIT( redit_sector )
{
    ROOM_INDEX_DATA *room;
    int value;

    EDIT_ROOM(ch, room);

    if ( (value = flag_value( sector_flags, argument )) == NO_FLAG )
    {
        send_to_char( "Syntax: sector [type]\n\r", ch );
        return FALSE;
    }

    room->sector_type = value;
    send_to_char( "Sector type set.\n\r", ch );

    return TRUE;
}
 /* Help Editor - kermit 1/98 */
 HEDIT (hedit_make)
 {
     HELP_DATA *pHelp;

     if (argument[0] == '\0')
     {
         send_to_char("Syntax: mpedit make [keyword(s)]\n\r",ch);
         return FALSE;
     }

     pHelp                        = new_help();
     pHelp->keyword = str_dup(argument);
     pHelp->next                  = help_first;
     help_first                    = pHelp;
     ch->desc->pEdit               = (void *)pHelp;

     send_to_char("New Help Entry Created.\n\r",ch);
     return TRUE;
 }

HEDIT( hedit_show)
{
    HELP_DATA *pHelp;
    char buf[MSL];

    EDIT_HELP(ch,pHelp);

    if(pHelp->delete)  {
        send_to_char("\n\nTHIS HELP IS MARKED FOR DELETION!\n\r",ch);
        return FALSE;
    }

    sprintf(buf, "Level:       [%d]\n\r"
                "Keywords: %s\n\r"
                "\n\r%s\n\r",
            pHelp->level, pHelp->keyword, pHelp->text);
    send_to_char(buf,ch);

    return FALSE;
}

HEDIT( hedit_desc)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if (argument[0] =='\0')
    {
       string_append(ch, &pHelp->text);
       return TRUE;
    }

    send_to_char(" Syntax: desc\n\r",ch);
    return FALSE;
}

HEDIT( hedit_keywords)
{
    HELP_DATA *pHelp;
    EDIT_HELP(ch, pHelp);

    if(argument[0] == '\0')
    {
        send_to_char(" Syntax: keywords [keywords]\n\r",ch);
        return FALSE;
    }

    pHelp->keyword = str_dup(argument);
    send_to_char( "Keyword(s) Set.\n\r", ch);
    return TRUE;
}

HEDIT(hedit_level)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch, pHelp);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  level [number]\n\r", ch );
    return FALSE;
    }

    pHelp->level = atoi( argument );

    send_to_char( "Level set.\n\r", ch);
    return TRUE;
}

HEDIT( hedit_delete)
{
    HELP_DATA *pHelp;

    EDIT_HELP(ch,pHelp);

    if(!pHelp->delete) {
        pHelp->delete = TRUE;
        send_to_char("YOU HAVE MARKED THIS HELP DELETION!\n\r",ch);
        return TRUE;
    }

    pHelp->delete = FALSE;
    send_to_char("YOU HAVE UNMARKED THIS HELP FOR DELETION!\n\r",ch);
    return TRUE;
}
// Clan editor functions
CEDIT( cedit_flags )
{
    CLAN_DATA *pClan;
    long value = 0;

    if ( argument[0] != '\0' )
    {
        EDIT_CLAN( ch, pClan );

        if ( ( value = flag_value( clan_flags, argument ) ) != NO_FLAG )
        {
                pClan->flags ^= value;
                sendch( "Clan flag(s) toggled.\n\r", ch);
                return TRUE;
        }
    }

    sendch( "Syntax: flag [flag ID]\n\r"
            "Type '? clan' for a list of valid flags.\n\r", ch );
    return FALSE;
}

CEDIT( cedit_create )
{
    int i;

    if (IS_NULLSTR (argument) )
    {
        sendch ("Syntax: create <name>\n\r", ch);
        return FALSE;
    }

    if ((clan_lookup(argument)) != 0)
    {
    sendch ("That clan already exists.\n\r", ch);
    return FALSE;
    }

    // just loop through and find an open spot
    for (i = 1; i < MAX_CLAN; i++)
    {
        if (clan_lookup(clan_table[i].name) == 0)
        break;
    }

    if (i <= MAX_CLAN) /* open slot */
    {
        CLAN_DATA *pClan;
        int x;

        clan_table[i].name = str_dup(capitalize(argument));
        clan_table[i].who_name = str_dup("Empty");
        clan_table[i].leader = str_dup("Empty");
        clan_table[i].banker = str_dup("None");
        clan_table[i].bonus = 0;
        clan_table[i].bank = 0;
        clan_table[i].clan_type = 0;
        clan_table[i].clanhall = ROOM_VNUM_TEMPLE;


        pClan = &clan_table[i]; /* return new clan data */
        ch->desc->pEdit = pClan;
        top_clan++;

        sendch("Clan created.\n\r", ch);
        return TRUE;
    }
    sendch("No room to create a new clan. Increase MAX_CLAN!\n\r", ch);
    return FALSE;
}

CEDIT( cedit_delete )
{
    CLAN_DATA *pClan;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *pch;
    int i, j, x, location;
    bool found = FALSE;

    EDIT_CLAN (ch, pClan);

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->editor == ED_CLAN && pClan == d->pEdit)
                edit_done (d->character);
    }

    // Set clan location
    for (i = 0; i < MAX_CLAN; i++)
    {
        if (clan_table[i].name == pClan->name)
        {
                found = TRUE;
                location = i;
                break;
        }
    }

    if (!found)
    {
        sendch ("Clan has already been deleted.\n\r", ch);
        return FALSE;
    }

    // Incase the clan we delete is still being used by players
    for (pch = char_list; pch != NULL; pch = pch->next)
    {
        if (clan_table[pch->clan].name == pClan->name)
        {
            pch->clan = 0;
        }
    }

    // Shift the clans up
    for (i = location; i < MAX_CLAN; ++i)
    {
        x = i + 1;
        clan_table[i] = clan_table[x];
        clan_table[i].name = clan_table[x].name;
        clan_table[i].who_name = clan_table[x].who_name;
        clan_table[i].leader = clan_table[x].leader;
        clan_table[i].bank = clan_table[x].bank;
        clan_table[i].banker = clan_table[x].banker;
        clan_table[i].flags = clan_table[x].flags;
    }

    clan_table[top_clan] = clan_table[0]; // Set last clan to null
    top_clan--; // Change null from being accessed

    save_clans(ch, argument); //Updating file
    sendch ("Clan deleted.\n\r", ch);
    return TRUE;
}

CEDIT( cedit_list )
{
    char buf[MAX_INPUT_LENGTH], buf1[MIL];
    int i, col = 0;

    buf1[0] = '\0';
    sendch("================== Active Clans ==================\n\r", ch);

    for (i = 1; i < MAX_CLAN; i++)
    {
        if (clan_table[i].name != NULL
                && clan_table[i].name[0] != '\0')
        {
                sprintf (buf, "{c%-27.27s %s{x",
                        clan_table[i].name,
                        clan_table[i].leader);
                sendch (buf, ch);

                if (++col % 1 == 0)
                        sendch ("\n\r", ch);
        }
    }
    if (col % 1 != 0)
        sendch  ("\n\r", ch);
    return FALSE;
}

CEDIT( cedit_show )
{
    CLAN_DATA *pClan;
    char buf[MAX_STRING_LENGTH];
    char buf1[2 * MAX_STRING_LENGTH];
    int i;

    EDIT_CLAN(ch, pClan);

    buf1[0] = '\0';

    sprintf(buf, "Name:        [%s]\n\r", pClan->name);
    strcat (buf1, buf);

    if (!str_cmp(pClan->who_name, "Empty") )
    {
        sprintf (buf, "Who Name:    [%s]\n\r", pClan->who_name);
        strcat (buf1, buf);
    } else {
        sprintf (buf, "Who Name:    %s\n\r", pClan->who_name);
        strcat (buf1, buf);
    }

    sprintf(buf, "Leader:      [%s]\n\r", pClan->leader);
    strcat(buf1, buf);
    sprintf(buf, "Clanhall:    [%d]\n\r", pClan->clanhall);
    strcat(buf1, buf);
    sprintf(buf, "Banker:      [%s]\n\r", pClan->banker);
    strcat (buf1, buf);
    sprintf(buf, "Balance:     [%d]\n\r", pClan->bank);
    strcat (buf1, buf);
    sprintf(buf, "Dam Bonus:   [%d]\n\r", pClan->bonus);
    strcat (buf1, buf);
    switch (pClan->clan_type)
    {

        case TYPE_NONE:     sprintf(buf, "Type:        [%s]\n\r", "None");break;
        case TYPE_CLAN:     sprintf(buf, "Type:        [%s]\n\r", "Clan");break;
        case TYPE_TRIBE:     sprintf(buf, "Type:        [%s]\n\r", "Tribe");break;
        case TYPE_TRADITION:     sprintf(buf, "Type:        [%s]\n\r", "Tradition");break;
        default: sprintf(buf, "Type:        [%s]\n\r", "Unknown");break;
    }
    strcat(buf1, buf);

    sprintf(buf, "Flag:        [%s]\n\r", flag_string( clan_flags, pClan->flags ) );
    strcat (buf1, buf);

    sendch (buf1, ch);
    return FALSE;
}

CEDIT( cedit_name )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if (argument[0] == '\0')
    {
        sendch("Syntax: name <name of clan>\n\r", ch);
        return FALSE;
    }

    if (argument[0] != '\0')
    {
        int i;

        for (i = 1; clan_table[i].name != 0; i++)
        {
            if (clan_table[i].name[0] == argument[0]
                && clan_table[i].name[0] != pClan->name[0])
            {
                sendch("That clan already exists.\n\r", ch);
                return FALSE;
            }
        }

    }

    if (pClan->name[0] != '\0')
        free_string(pClan->name);

        pClan->name = str_dup(argument);

    sendch("Name set.\n\r", ch);
    return TRUE;
}

CEDIT( cedit_who_name )
{
    CLAN_DATA *pClan;
    char buf[MSL];
    int i;

    EDIT_CLAN(ch, pClan);

    if (argument[0] == '\0')
    {
        sendch("Syntax: who <name>\n\r", ch);
        return FALSE;
    }

    if (pClan->who_name[0] != '\0')
        free_string(pClan->who_name);

        i = strlen(argument);
        if (argument[i-2] != '{' && argument[i-1] == '{')
                argument[i-1] = '\0';

        if (argument[i-2] != '{' || argument[i-1] != 'x')
                strcat (argument, "{x");

        sprintf (buf, "%s", argument);
        pClan->who_name = str_dup(buf);

    sendch("Who name set.\n\r", ch);
    return TRUE;
}


CEDIT( cedit_leader )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if (argument[0] == '\0')
    {
        sendch("Syntax: leader <name>\n\r", ch);
        return FALSE;
    }

    if (pClan->leader[0] != '\0')
        free_string(pClan->leader);

        pClan->leader = str_dup(argument);

    sendch("Leader's name set.\n\r", ch);
    return TRUE;
}
CEDIT( cedit_banker )
{
    CLAN_DATA *pClan;
    char name[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    EDIT_CLAN(ch, pClan);

    one_argument( argument, name );

    if ( name[0] == '\0' )
    {
    send_to_char( "Syntax:  banker [$name]  -toggles banker\n\r", ch );
    send_to_char( "Syntax:  banker All      -allows everyone\n\r", ch );
    return FALSE;
    }

    name[0] = UPPER( name[0] );

    if ( strstr( pClan->banker, name ) != '\0' )
    {
    pClan->banker = string_replace( pClan->banker, name, "\0" );
    pClan->banker = string_unpad( pClan->banker );

    if ( pClan->banker[0] == '\0' )
    {
        free_string( pClan->banker );
        pClan->banker = str_dup( "None" );
    }
    send_to_char( "Banker removed.\n\r", ch );
    return TRUE;
    }
    else
    {
    buf[0] = '\0';
    if ( strstr( pClan->banker, "None" ) != '\0' )
    {
        pClan->banker = string_replace( pClan->banker, "None", "\0" );
        pClan->banker = string_unpad( pClan->banker );
    }

    if (pClan->banker[0] != '\0' )
    {
        strcat( buf, pClan->banker );
        strcat( buf, " " );
    }
    strcat( buf, name );
    free_string( pClan->banker );
    pClan->banker = string_proper( str_dup( buf ) );

    send_to_char( "Banker added.\n\r", ch );
    send_to_char( pClan->banker,ch);
    return TRUE;
    }

    return FALSE;
}
CEDIT( cedit_balance )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  balance [number]\n\r", ch );
    return FALSE;
    }

    pClan->bank = atoi( argument );

    send_to_char( "Bank balance set.\n\r", ch);
    return TRUE;
}

CEDIT( cedit_bonus )
{
    CLAN_DATA *pClan;

    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  bonus [number]\n\r", ch );
    return FALSE;
    }

    pClan->bonus = atoi( argument );

    send_to_char( "Bonus multiplier set.\n\r", ch);
    return TRUE;
}
CEDIT( cedit_clanhall )
{
    CLAN_DATA *pClan;
    int value;
    ROOM_INDEX_DATA *toRoom;
    EDIT_CLAN(ch, pClan);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  clanhall [number]\n\r", ch );
    return FALSE;
    }

    value = atoi( argument );

    if ( ! (toRoom = get_room_index( value )) )
    {
        send_to_char( "CEdit:  Cannot set to nonexistant room.\n\r", ch );
        return FALSE;
    }
    pClan->clanhall = value;
    send_to_char( "Clanhall set.\n\r", ch);
    return TRUE;
}
CEDIT( cedit_type )      /* Moved out of oedit() due to naming conflicts -- Hugin */
{
    CLAN_DATA *pClan;
    int value;

    if ( argument[0] != '\0' )
    {
    EDIT_CLAN(ch, pClan);

    if ( ( value = flag_value( clantype_flags, argument ) ) != NO_FLAG )
    {
        pClan->clan_type = value;

        send_to_char( "Type set.\n\r", ch);
        return TRUE;
    }
    }

    send_to_char( "Syntax:  type [flag]\n\r"
          "Type '? clantype' for a list of flags.\n\r", ch );
    return FALSE;
}

OEDIT ( oedit_addoprog )
{
  int value;
  OBJ_INDEX_DATA *pObj;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_OBJ(ch, pObj);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addoprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (oprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "oprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_OPROG ) ) == NULL)
  {
        send_to_char("No such OBJProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_oprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pObj->oprog_flags,value);
  list->next            = pObj->oprogs;
  pObj->oprogs          = list;

  send_to_char( "Oprog Added.\n\r",ch);
  return TRUE;
}

OEDIT ( oedit_deloprog )
{
    OBJ_INDEX_DATA *pObj;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char oprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_OBJ(ch, pObj);

    one_argument( argument, oprog );
    if (!is_number( oprog ) || oprog[0] == '\0' )
    {
       send_to_char("Syntax:  deloprog [#oprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( oprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative oprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pObj->oprogs) )
    {
        send_to_char("OEdit:  Non existant oprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pObj->oprog_flags, pObj->oprogs->trig_type);
        list = pObj->oprogs;
        pObj->oprogs = list->next;
        free_oprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pObj->oprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_oprog(list_next);
        }
        else
        {
                send_to_char("No such oprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Oprog removed.\n\r", ch);
    return TRUE;
}

REDIT ( redit_addrprog )
{
  int value;
  ROOM_INDEX_DATA *pRoom;
  PROG_LIST *list;
  PROG_CODE *code;
  char trigger[MAX_STRING_LENGTH];
  char phrase[MAX_STRING_LENGTH];
  char num[MAX_STRING_LENGTH];

  EDIT_ROOM(ch, pRoom);
  argument=one_argument(argument, num);
  argument=one_argument(argument, trigger);
  argument=one_argument(argument, phrase);

  if (!is_number(num) || trigger[0] =='\0' || phrase[0] =='\0' )
  {
        send_to_char("Syntax:   addrprog [vnum] [trigger] [phrase]\n\r",ch);
        return FALSE;
  }

  if ( (value = flag_value (rprog_flags, trigger) ) == NO_FLAG)
  {
        send_to_char("Valid flags are:\n\r",ch);
        show_help( ch, "rprog");
        return FALSE;
  }

  if ( ( code =get_prog_index (atoi(num), PRG_RPROG ) ) == NULL)
  {
        send_to_char("No such ROOMProgram.\n\r",ch);
        return FALSE;
  }

  list                  = new_rprog();
  list->vnum            = atoi(num);
  list->trig_type       = value;
  list->trig_phrase     = str_dup(phrase);
  list->code            = code->code;
  SET_BIT(pRoom->rprog_flags,value);
  list->next            = pRoom->rprogs;
  pRoom->rprogs          = list;

  send_to_char( "Rprog Added.\n\r",ch);
  return TRUE;
}

REDIT ( redit_delrprog )
{
    ROOM_INDEX_DATA *pRoom;
    PROG_LIST *list;
    PROG_LIST *list_next;
    char rprog[MAX_STRING_LENGTH];
    int value;
    int cnt = 0;

    EDIT_ROOM(ch, pRoom);

    one_argument( argument, rprog );
    if (!is_number( rprog ) || rprog[0] == '\0' )
    {
       send_to_char("Syntax:  delrprog [#rprog]\n\r",ch);
       return FALSE;
    }

    value = atoi ( rprog );

    if ( value < 0 )
    {
        send_to_char("Only non-negative rprog-numbers allowed.\n\r",ch);
        return FALSE;
    }

    if ( !(list= pRoom->rprogs) )
    {
        send_to_char("REdit:  Non existant rprog.\n\r",ch);
        return FALSE;
    }

    if ( value == 0 )
    {
	REMOVE_BIT(pRoom->rprog_flags, pRoom->rprogs->trig_type);
        list = pRoom->rprogs;
        pRoom->rprogs = list->next;
        free_rprog( list );
    }
    else
    {
        while ( (list_next = list->next) && (++cnt < value ) )
                list = list_next;

        if ( list_next )
        {
		REMOVE_BIT(pRoom->rprog_flags, list_next->trig_type);
                list->next = list_next->next;
                free_rprog(list_next);
        }
        else
        {
                send_to_char("No such rprog.\n\r",ch);
                return FALSE;
        }
    }

    send_to_char("Rprog removed.\n\r", ch);
    return TRUE;
}
