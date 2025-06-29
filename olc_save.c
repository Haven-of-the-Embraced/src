/**************************************************************************
 *  File: olc_save.c                                                       *
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
/* OLC_SAVE.C
 * This takes care of saving all the .are information.
 * Notes:
 * -If a good syntax checker is used for setting vnum ranges of areas
 *  then it would become possible to just cycle through vnums instead
 *  of using the iHash stuff and checking that the room or reset or
 *  mob etc is part of that area.
 */

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

#define DIF(a,b) (~((~a)|(b)))

/*
 *  Verbose writes reset data in plain english into the comments
 *  section of the resets.  It makes areas considerably larger but
 *  may aid in debugging.
 */

/* #define VERBOSE */

/*****************************************************************************
 Name:      fix_string
 Purpose:   Returns a string without \r and ~.
 ****************************************************************************/
char *fix_string( const char *str )
{
    static char strfix[MAX_STRING_LENGTH * 2];
    int i;
    int o;

    if ( str == NULL )
        return '\0';

    for ( o = i = 0; str[i+o] != '\0'; i++ )
    {
        if (str[i+o] == '\r' || str[i+o] == '~')
            o++;
        strfix[i] = str[i+o];
    }
    strfix[i] = '\0';
    return strfix;
}



/*****************************************************************************
 Name:      save_area_list
 Purpose:   Saves the listing of files to be loaded at startup.
 Called by: do_asave(olc_save.c).
 ****************************************************************************/
void save_area_list()
{
    FILE *fp;
    AREA_DATA *pArea;
    extern HELP_AREA * had_list;
    HELP_AREA * ha;

    if ( ( fp = fopen( AREA_LIST, "w" ) ) == NULL )
    {
    bug( "Save_area_list: fopen", 0 );
    perror( AREA_LIST );
    }
    else
    {

    for( pArea = area_first; pArea; pArea = pArea->next )
    {
        fprintf( fp, "%s\n", pArea->file_name );
    }

    fprintf( fp, "$\n" );
    fclose( fp );
    }

    return;
}


/*
 * ROM OLC
 * Used in save_mobile and save_object below.  Writes
 * flags on the form fread_flag reads.
 *
 * buf[] must hold at least 32+1 characters.
 *
 * -- Hugin
 */
char *fwrite_flag( long flags, char buf[] )
{
    char offset;
    char *cp;

    buf[0] = '\0';

    if ( flags == 0 )
    {
    strcpy( buf, "0" );
    return buf;
    }

    /* 32 -- number of bits in a long */

    for ( offset = 0, cp = buf; offset < 32; offset++ )
    if ( flags & ( (long)1 << offset ) )
    {
        if ( offset <= 'Z' - 'A' )
        *(cp++) = 'A' + offset;
        else
        *(cp++) = 'a' + offset - ( 'Z' - 'A' + 1 );
    }

    *cp = '\0';

    return buf;
}

void save_mobprogs( FILE *fp, AREA_DATA *pArea )
{
    PROG_CODE *pMprog;
        int i;

        fprintf(fp, "#MOBPROGS\n");

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pMprog = get_prog_index(i, PRG_MPROG) ) != NULL)
        {
                  fprintf(fp, "#%d\n", i);
                  fprintf(fp, "%s~\n", fix_string(pMprog->code));
        }
        }

        fprintf(fp,"#0\n\n");
        return;
}
void save_objprogs( FILE *fp, AREA_DATA *pArea )
{
	PROG_CODE *pOprog;
        int i;

        fprintf(fp, "#OBJPROGS\n");

	for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pOprog = get_prog_index(i, PRG_OPROG) ) != NULL)
		{
		          fprintf(fp, "#%d\n", i);
		          fprintf(fp, "%s~\n", fix_string(pOprog->code));
		}
        }

        fprintf(fp,"#0\n\n");
        return;
}

void save_roomprogs( FILE *fp, AREA_DATA *pArea )
{
	PROG_CODE *pRprog;
        int i;

        fprintf(fp, "#ROOMPROGS\n");

	for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
        {
          if ( (pRprog = get_prog_index(i,PRG_RPROG) ) != NULL)
		{
		          fprintf(fp, "#%d\n", i);
		          fprintf(fp, "%s~\n", fix_string(pRprog->code));
		}
        }

        fprintf(fp,"#0\n\n");
        return;
}

/*****************************************************************************
 Name:      save_mobile
 Purpose:   Save one mobile to file, new format -- Hugin
 Called by: save_mobiles (below).
 ****************************************************************************/
void save_mobile( FILE *fp, MOB_INDEX_DATA *pMobIndex )
{
    sh_int race = pMobIndex->race;
    PROG_LIST *pMprog;
    char buf[MAX_STRING_LENGTH];
    long temp;

    fprintf( fp, "#%d\n",   pMobIndex->vnum );
    fprintf( fp, "%s~\n",   pMobIndex->player_name );
    fprintf( fp, "%s~\n",   pMobIndex->short_descr );
    fprintf( fp, "%s~\n",   fix_string( pMobIndex->long_descr ) );
    fprintf( fp, "%s~\n",   fix_string( pMobIndex->description) );
    fprintf( fp, "%s~\n",   race_table[race].name );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->act,        buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->act2,       buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->affected_by,    buf ) );
    fprintf( fp, "%d %d\n", pMobIndex->alignment , pMobIndex->group);
    fprintf( fp, "%d ",     pMobIndex->level );
    fprintf( fp, "%d ",     pMobIndex->hitroll );
    fprintf( fp, "%dd%d+%d ",   pMobIndex->hit[DICE_NUMBER],
                pMobIndex->hit[DICE_TYPE],
                pMobIndex->hit[DICE_BONUS] );
    fprintf( fp, "%dd%d+%d ",   pMobIndex->mana[DICE_NUMBER],
                pMobIndex->mana[DICE_TYPE],
                pMobIndex->mana[DICE_BONUS] );
    fprintf( fp, "%dd%d+%d ",   pMobIndex->damage[DICE_NUMBER],
                pMobIndex->damage[DICE_TYPE],
                pMobIndex->damage[DICE_BONUS] );
    fprintf( fp, "%s\n",    attack_table[pMobIndex->dam_type].name );
    fprintf( fp, "%d %d %d %d\n",
                pMobIndex->ac[AC_PIERCE] / 10,
                pMobIndex->ac[AC_BASH]   / 10,
                pMobIndex->ac[AC_SLASH]  / 10,
                pMobIndex->ac[AC_EXOTIC] / 10 );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->attr_flags, buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->abil_flags, buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->off_flags,  buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->imm_flags,  buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->res_flags,  buf ) );
    fprintf( fp, "%s\n",    fwrite_flag( pMobIndex->vuln_flags, buf ) );
    fprintf( fp, "%s %s %s %ld\n",
                position_table[pMobIndex->start_pos].short_name,
                position_table[pMobIndex->default_pos].short_name,
                sex_table[pMobIndex->sex].name,
                pMobIndex->wealth );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->form,  buf ) );
    fprintf( fp, "%s ",     fwrite_flag( pMobIndex->parts, buf ) );

    fprintf( fp, "%s ",     size_table[pMobIndex->size].name );
    fprintf( fp, "%s\n",    IS_NULLSTR(pMobIndex->material) ? pMobIndex->material : "unknown" );
    fprintf( fp, "%d\n ", pMobIndex->maxload);

    if ((temp = DIF(race_table[race].act,pMobIndex->act)))
        fprintf( fp, "F act %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].aff,pMobIndex->affected_by)))
        fprintf( fp, "F aff %s\n", fwrite_flag(temp, buf) );

    if (pMobIndex->affected2_by)
        fprintf( fp, "F af2 %s\n", fwrite_flag(pMobIndex->affected2_by, buf) );

    if ((temp = DIF(race_table[race].off,pMobIndex->off_flags)))
        fprintf( fp, "F off %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].imm,pMobIndex->imm_flags)))
        fprintf( fp, "F imm %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].res,pMobIndex->res_flags)))
        fprintf( fp, "F res %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].vuln,pMobIndex->vuln_flags)))
        fprintf( fp, "F vul %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].form,pMobIndex->form)))
        fprintf( fp, "F for %s\n", fwrite_flag(temp, buf) );

    if ((temp = DIF(race_table[race].parts,pMobIndex->parts)))
        fprintf( fp, "F par %s\n", fwrite_flag(temp, buf) );

    for (pMprog = pMobIndex->mprogs; pMprog; pMprog = pMprog->next)
    {
        fprintf(fp, "M %s %d %s~\n",
        prog_type_to_name(pMprog->trig_type), pMprog->vnum,
                pMprog->trig_phrase);
    }

    return;
}


/*****************************************************************************
 Name:      save_mobiles
 Purpose:   Save #MOBILES secion of an area file.
 Called by: save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_mobiles( FILE *fp, AREA_DATA *pArea )
{
    int i;
    MOB_INDEX_DATA *pMob;

    fprintf( fp, "#MOBILES\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
    if ( (pMob = get_mob_index( i )) && pMob->delete == FALSE)
        save_mobile( fp, pMob );
    }

    fprintf( fp, "#0\n\n\n\n" );
    return;
}





/*****************************************************************************
 Name:      save_object
 Purpose:   Save one object to file.
                new ROM format saving -- Hugin
 Called by: save_objects (below).
 ****************************************************************************/
void save_object( FILE *fp, OBJ_INDEX_DATA *pObjIndex )
{
    char letter;
    AFFECT_DATA *pAf;
    EXTRA_DESCR_DATA *pEd;
    char buf[MAX_STRING_LENGTH];
    PROG_LIST *pOprog;

    fprintf( fp, "#%d\n",    pObjIndex->vnum );
    fprintf( fp, "%s~\n",    pObjIndex->name );
    fprintf( fp, "%s~\n",    pObjIndex->short_descr );
    fprintf( fp, "%s~\n",    fix_string( pObjIndex->description ) );
    fprintf( fp, "%s~\n",    pObjIndex->material );
    fprintf( fp, "%d\n",     pObjIndex->timer );
    fprintf( fp, "%s ",      item_name(pObjIndex->item_type));
    fprintf( fp, "%s ",      fwrite_flag( pObjIndex->extra_flags, buf ) );
    fprintf( fp, "%s\n",     fwrite_flag( pObjIndex->wear_flags,  buf ) );
    fprintf( fp, "%d %d %d %d\n",     pObjIndex->size, 0, 0, 0 );

/*
 *  Using fwrite_flag to write most values gives a strange
 *  looking area file, consider making a case for each
 *  item type later.
 */

    switch ( pObjIndex->item_type )
    {
        default:
        fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[0], buf ) );
        fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[1], buf ) );
        fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[2], buf ) );
        fprintf( fp, "%s ",  fwrite_flag( pObjIndex->value[3], buf ) );
        fprintf( fp, "%s\n", fwrite_flag( pObjIndex->value[4], buf ) );
        break;

        case ITEM_DRINK_CON:
        case ITEM_FOUNTAIN:
            fprintf( fp, "%d %d '%s' %d %d\n",
                     pObjIndex->value[0],
                     pObjIndex->value[1],
                     liq_table[pObjIndex->value[2]].liq_name,
             pObjIndex->value[3],
             pObjIndex->value[4]);
            break;

        case ITEM_CONTAINER:
            fprintf( fp, "%d %s %d %d %d\n",
                     pObjIndex->value[0],
                     fwrite_flag( pObjIndex->value[1], buf ),
                     pObjIndex->value[2],
                     pObjIndex->value[3],
                     pObjIndex->value[4]);
            break;

        case ITEM_WEAPON:
            fprintf( fp, "%s %d %d %s %s\n",
                     weapon_name(pObjIndex->value[0]),
                     pObjIndex->value[1],
                     pObjIndex->value[2],
                     attack_table[pObjIndex->value[3]].name,
                     fwrite_flag( pObjIndex->value[4], buf ) );
            break;

        case ITEM_PILL:
        case ITEM_POTION:
        case ITEM_SCROLL:
        fprintf( fp, "%d '%s' '%s' '%s' '%s'\n",
             pObjIndex->value[0] > 0 ? /* no negative numbers */
             pObjIndex->value[0]
             : 0,
             pObjIndex->value[1] != -1 ?
             skill_table[pObjIndex->value[1]].name
             : "",
             pObjIndex->value[2] != -1 ?
             skill_table[pObjIndex->value[2]].name
             : "",
             pObjIndex->value[3] != -1 ?
             skill_table[pObjIndex->value[3]].name
             : "",
             pObjIndex->value[4] != -1 ?
             skill_table[pObjIndex->value[4]].name
             : "");
        break;

        case ITEM_TOOL:
            fprintf( fp, "%d '%s' %d %d %d\n",
             pObjIndex->value[0],
             tool_table[pObjIndex->value[1]].tool_name,
             pObjIndex->value[2],
             pObjIndex->value[3],
             pObjIndex->value[4]);
            break;

        case ITEM_CRAFTED:
            fprintf( fp, "%d '%s' %d %d %d\n",
             pObjIndex->value[0],
             crafted_table[pObjIndex->value[1]].crafted_name,
             pObjIndex->value[2],
             pObjIndex->value[3],
             pObjIndex->value[4]);
            break;


        case ITEM_STAFF:
        case ITEM_WAND:
        fprintf( fp, "%d %d %d '%s' %d\n",
                    pObjIndex->value[0],
                    pObjIndex->value[1],
                    pObjIndex->value[2],
                    pObjIndex->value[3] != -1 ?
                        skill_table[pObjIndex->value[3]].name :
                        "",
                    pObjIndex->value[4] );
        break;
    }

    fprintf( fp, "%d ", pObjIndex->level );
    fprintf( fp, "%d ", pObjIndex->weight );
    fprintf( fp, "%d ", pObjIndex->cost );

         if ( pObjIndex->condition >  90 ) letter = 'P';
    else if ( pObjIndex->condition >  75 ) letter = 'G';
    else if ( pObjIndex->condition >  50 ) letter = 'A';
    else if ( pObjIndex->condition >  25 ) letter = 'W';
    else if ( pObjIndex->condition >  10 ) letter = 'D';
    else if ( pObjIndex->condition >   0 ) letter = 'B';
    else                                   letter = 'R';

    fprintf( fp, "%c\n", letter );

    for( pAf = pObjIndex->affected; pAf; pAf = pAf->next )
    {
    if (pAf->where == TO_OBJECT || pAf->bitvector == 0)
            fprintf( fp, "A\n%d %d\n",  pAf->location, pAf->modifier );
    else
    {
        fprintf( fp, "F\n" );

        switch(pAf->where)
        {
            case TO_AFFECTS:
                fprintf( fp, "A " );
                break;
            case TO_IMMUNE:
                fprintf( fp, "I " );
                break;
            case TO_RESIST:
                fprintf( fp, "R " );
                break;
            case TO_VULN:
                fprintf( fp, "V " );
                break;
            default:
                bug( "olc_save: Invalid Affect->where", 0);
                break;
        }

        fprintf( fp, "%d %d %s\n", pAf->location, pAf->modifier,
                fwrite_flag( pAf->bitvector, buf ) );
    }
    }

    for( pEd = pObjIndex->extra_descr; pEd; pEd = pEd->next )
    {
        fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
         fix_string( pEd->description ) );
    }
    for (pOprog = pObjIndex->oprogs; pOprog; pOprog = pOprog->next)
    {
        fprintf(fp, "O %s %d %s~\n",
        prog_type_to_name(pOprog->trig_type), pOprog->vnum,
                pOprog->trig_phrase);
    }

    return;
}




/*****************************************************************************
 Name:      save_objects
 Purpose:   Save #OBJECTS section of an area file.
 Called by: save_area(olc_save.c).
 Notes:         Changed for ROM OLC.
 ****************************************************************************/
void save_objects( FILE *fp, AREA_DATA *pArea )
{
    int i;
    OBJ_INDEX_DATA *pObj;

    fprintf( fp, "#OBJECTS\n" );

    for( i = pArea->min_vnum; i <= pArea->max_vnum; i++ )
    {
    if ( (pObj = get_obj_index( i )) && pObj->delete == FALSE)
        save_object( fp, pObj );
    }

    fprintf( fp, "#0\n\n\n\n" );
    return;
}





/*****************************************************************************
 Name:      save_rooms
 Purpose:   Save #ROOMS section of an area file.
 Called by: save_area(olc_save.c).
 ****************************************************************************/
void save_rooms( FILE *fp, AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoomIndex;
    EXTRA_DESCR_DATA *pEd;
    EXIT_DATA *pExit;
    int iHash;
    int door;
    PROG_LIST *pRprog;

    fprintf( fp, "#ROOMS\n" );
    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->area == pArea )
            {
                fprintf( fp, "#%d\n",       pRoomIndex->vnum );
                fprintf( fp, "%s~\n",       pRoomIndex->name );
                fprintf( fp, "%s~\n",       fix_string( pRoomIndex->description ) );
        fprintf( fp, "0 " );
                fprintf( fp, "%d ",     pRoomIndex->room_flags );
                fprintf( fp, "%d\n",        pRoomIndex->sector_type );

                for ( pEd = pRoomIndex->extra_descr; pEd;
                      pEd = pEd->next )
                {
                    fprintf( fp, "E\n%s~\n%s~\n", pEd->keyword,
                                                  fix_string( pEd->description ) );
                }
                for( door = 0; door < MAX_DIR; door++ ) /* I hate this! */
                {
                    if ( ( pExit = pRoomIndex->exit[door] )
                          && pExit->u1.to_room )
                    {
            int locks = 0;

            /* HACK : TO PREVENT EX_LOCKED etc without EX_ISDOOR
               to stop booting the mud */
            if ( IS_SET(pExit->rs_flags, EX_CLOSED)
            ||   IS_SET(pExit->rs_flags, EX_LOCKED)
            ||   IS_SET(pExit->rs_flags, EX_HIDDEN)
            ||   IS_SET(pExit->rs_flags, EX_LIFT)
            ||   IS_SET(pExit->rs_flags, EX_PICKPROOF)
            ||   IS_SET(pExit->rs_flags, EX_NOPASS)
            ||   IS_SET(pExit->rs_flags, EX_EASY)
            ||   IS_SET(pExit->rs_flags, EX_HARD)
            ||   IS_SET(pExit->rs_flags, EX_INFURIATING)
            ||   IS_SET(pExit->rs_flags, EX_NOCLOSE)
            ||   IS_SET(pExit->rs_flags, EX_NOLOCK) )
                SET_BIT(pExit->rs_flags, EX_ISDOOR);
            else
                REMOVE_BIT(pExit->rs_flags, EX_ISDOOR);

            /* THIS SUCKS but it's backwards compatible */
            /* NOTE THAT EX_NOCLOSE NOLOCK etc aren't being saved */

            /* Added lift and hidden, previous coder is right,
               this DOES suck... big time.  - Ugha*/
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 1;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 2;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 3;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 4;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 5;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 6;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ))
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ))
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 7;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ))
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( !IS_SET( pExit->rs_flags, EX_HIDDEN )))
                locks = 8;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 9;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ))
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 10;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ))
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 11;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN ))
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS )))
                locks = 12;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ) )
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 13;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( !IS_SET( pExit->rs_flags, EX_NOPASS ) )
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 14;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( !IS_SET( pExit->rs_flags, EX_PICKPROOF ))
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ))
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 15;
            if ( IS_SET( pExit->rs_flags,  EX_ISDOOR )
            && ( IS_SET( pExit->rs_flags,  EX_NOPASS ))
            && ( IS_SET( pExit->rs_flags,  EX_PICKPROOF ))
            && ( !IS_SET( pExit->rs_flags, EX_LIFT ))
            && ( IS_SET( pExit->rs_flags,  EX_HIDDEN )))
                locks = 16;

                        fprintf( fp, "D%d\n",      pExit->orig_door );
                        fprintf( fp, "%s~\n",      fix_string( pExit->description ) );
                        fprintf( fp, "%s~\n",      pExit->keyword );
                        fprintf( fp, "%d %d %d\n", locks,
                                                   pExit->key,
                                                   pExit->u1.to_room->vnum );
                    }
                }
        if (pRoomIndex->mana_rate != 100 || pRoomIndex->heal_rate != 100)
         fprintf ( fp, "M %d H %d\n",pRoomIndex->mana_rate,
                                     pRoomIndex->heal_rate);
        if (pRoomIndex->clan > 0)
         fprintf ( fp, "C %s~\n" , clan_table[pRoomIndex->clan].name );

        if (!IS_NULLSTR(pRoomIndex->owner))
         fprintf ( fp, "O %s~\n" , pRoomIndex->owner );
         for (pRprog = pRoomIndex->rprogs; pRprog; pRprog = pRprog->next)
         {
         	 fprintf(fp, "R %s %d %s~\n",
         	 prog_type_to_name(pRprog->trig_type), pRprog->vnum,
         	 pRprog->trig_phrase);
         }
        fprintf( fp, "S\n" );
            }
        }
    }
    fprintf( fp, "#0\n\n\n\n" );
    return;
}



/*****************************************************************************
 Name:      save_specials
 Purpose:   Save #SPECIALS section of area file.
 Called by: save_area(olc_save.c).
 ****************************************************************************/
void save_specials( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    MOB_INDEX_DATA *pMobIndex;

    fprintf( fp, "#SPECIALS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->spec_fun )
            {
#if defined( VERBOSE )
                fprintf( fp, "M %d %s Load to: %s\n", pMobIndex->vnum,
                                                      spec_name( pMobIndex->spec_fun ),
                                                      pMobIndex->short_descr );
#else
                fprintf( fp, "M %d %s\n", pMobIndex->vnum,
                              spec_name( pMobIndex->spec_fun ) );
#endif
            }
        }
    }

    fprintf( fp, "S\n\n\n\n" );
    return;
}



/*
 * This function is obsolete.  It it not needed but has been left here
 * for historical reasons.  It is used currently for the same reason.
 *
 * I don't think it's obsolete in ROM -- Hugin.
 */
void save_door_resets( FILE *fp, AREA_DATA *pArea )
{
    int iHash;
    ROOM_INDEX_DATA *pRoomIndex;
    EXIT_DATA *pExit;
    int door;

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoomIndex = room_index_hash[iHash]; pRoomIndex; pRoomIndex = pRoomIndex->next )
        {
            if ( pRoomIndex->area == pArea )
            {
                for( door = 0; door < MAX_DIR; door++ )
                {
                    if ( ( pExit = pRoomIndex->exit[door] )
                          && pExit->u1.to_room
                          && ( IS_SET( pExit->rs_flags, EX_CLOSED )
                          || IS_SET( pExit->rs_flags, EX_LOCKED ) ) )
#if defined( VERBOSE )
            fprintf( fp, "D 0 %d %d %d The %s door of %s is %s\n",
                pRoomIndex->vnum,
                pExit->orig_door,
                IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1,
                dir_name[ pExit->orig_door ],
                pRoomIndex->name,
                IS_SET( pExit->rs_flags, EX_LOCKED) ? "closed and locked"
                    : "closed" );
#endif
#if !defined( VERBOSE )
            fprintf( fp, "D 0 %d %d %d\n",
                pRoomIndex->vnum,
                pExit->orig_door,
                IS_SET( pExit->rs_flags, EX_LOCKED) ? 2 : 1 );
#endif
        }
        }
    }
    }
    return;
}




/*****************************************************************************
 Name:      save_resets
 Purpose:   Saves the #RESETS section of an area file.
 Called by: save_area(olc_save.c)
 ****************************************************************************/
void save_resets( FILE *fp, AREA_DATA *pArea )
{
    RESET_DATA *pReset;
    MOB_INDEX_DATA *pLastMob = NULL;
    OBJ_INDEX_DATA *pLastObj;
    ROOM_INDEX_DATA *pRoom;
    char buf[MAX_STRING_LENGTH];
    int iHash;

    fprintf( fp, "#RESETS\n" );

    save_door_resets( fp, pArea );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pRoom = room_index_hash[iHash]; pRoom; pRoom = pRoom->next )
        {
            if ( pRoom->area == pArea )
        {
    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
    switch ( pReset->command )
    {
    default:
        bug( "Save_resets: bad command %c.", pReset->command );
        break;

#if defined( VERBOSE )
    case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
        fprintf( fp, "M 0 %d %d %d %d Load %s\n",
            pReset->arg1,
                pReset->arg2,
                pReset->arg3,
        pReset->arg4,
                pLastMob->short_descr );
            break;

    case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
        fprintf( fp, "O 0 %d 0 %d %s loaded to %s\n",
            pReset->arg1,
                pReset->arg3,
                capitalize(pLastObj->short_descr),
                pRoom->name );
            break;

    case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
        fprintf( fp, "P 0 %d %d %d %d %s put inside %s\n",
            pReset->arg1,
            pReset->arg2,
                pReset->arg3,
                pReset->arg4,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastObj->short_descr );
            break;

    case 'G':
        fprintf( fp, "G 0 %d 0 %s is given to %s\n",
            pReset->arg1,
            capitalize(get_obj_index( pReset->arg1 )->short_descr),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

    case 'E':
        fprintf( fp, "E 0 %d 0 %d %s is loaded %s of %s\n",
            pReset->arg1,
                pReset->arg3,
                capitalize(get_obj_index( pReset->arg1 )->short_descr),
                flag_string( wear_loc_strings, pReset->arg3 ),
                pLastMob ? pLastMob->short_descr : "!NO_MOB!" );
            if ( !pLastMob )
            {
                sprintf( buf, "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

    case 'D':
            break;

    case 'R':
            pRoom = get_room_index( pReset->arg1 );
        fprintf( fp, "R 0 %d %d Randomize %s\n",
            pReset->arg1,
                pReset->arg2,
                pRoom->name );
            break;
            }
#endif
#if !defined( VERBOSE )
    case 'M':
            pLastMob = get_mob_index( pReset->arg1 );
        fprintf( fp, "M 0 %d %d %d %d\n",
            pReset->arg1,
                pReset->arg2,
                pReset->arg3,
                pReset->arg4 );
            break;

    case 'O':
            pLastObj = get_obj_index( pReset->arg1 );
            pRoom = get_room_index( pReset->arg3 );
        fprintf( fp, "O 0 %d 0 %d\n",
            pReset->arg1,
                pReset->arg3 );
            break;

    case 'P':
            pLastObj = get_obj_index( pReset->arg1 );
        fprintf( fp, "P 0 %d %d %d %d\n",
            pReset->arg1,
            pReset->arg2,
                pReset->arg3,
                pReset->arg4 );
            break;

    case 'G':
        fprintf( fp, "G 0 %d 0\n", pReset->arg1 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

    case 'E':
        fprintf( fp, "E 0 %d 0 %d\n",
            pReset->arg1,
                pReset->arg3 );
            if ( !pLastMob )
            {
                sprintf( buf,
                    "Save_resets: !NO_MOB! in [%s]", pArea->file_name );
                bug( buf, 0 );
            }
            break;

    case 'D':
            break;

    case 'R':
            pRoom = get_room_index( pReset->arg1 );
        fprintf( fp, "R 0 %d %d\n",
            pReset->arg1,
                pReset->arg2 );
            break;
            }
#endif
        }
        }   /* End if correct area */
    }   /* End for pRoom */
    }   /* End for iHash */
    fprintf( fp, "S\n\n\n\n" );
    return;
}



/*****************************************************************************
 Name:      save_shops
 Purpose:   Saves the #SHOPS section of an area file.
 Called by: save_area(olc_save.c)
 ****************************************************************************/
void save_shops( FILE *fp, AREA_DATA *pArea )
{
    SHOP_DATA *pShopIndex;
    MOB_INDEX_DATA *pMobIndex;
    int iTrade;
    int iHash;

    fprintf( fp, "#SHOPS\n" );

    for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
    {
        for( pMobIndex = mob_index_hash[iHash]; pMobIndex; pMobIndex = pMobIndex->next )
        {
            if ( pMobIndex && pMobIndex->area == pArea && pMobIndex->pShop )
            {
                pShopIndex = pMobIndex->pShop;

                fprintf( fp, "%d ", pShopIndex->keeper );
                for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
                {
                    if ( pShopIndex->buy_type[iTrade] != 0 )
                    {
                       fprintf( fp, "%d ", pShopIndex->buy_type[iTrade] );
                    }
                    else
                       fprintf( fp, "0 ");
                }
                fprintf( fp, "%d %d ", pShopIndex->profit_buy, pShopIndex->profit_sell );
                fprintf( fp, "%d %d\n", pShopIndex->open_hour, pShopIndex->close_hour );
            }
        }
    }

    fprintf( fp, "0\n\n\n\n" );
    return;
}
void save_clans(CHAR_DATA *ch, char *argument)
{
    FILE *fp;
    int i;
    int value;
    char buf[MSL];

    sprintf (buf, CLAN_FILE);
    if ( !( fp = fopen(buf, "w" ) ) )
    {
        log_string( "Open_guild: fopen" );
         return;
    }

    for (i = 1; i < MAX_CLAN; i++)
    {
        if ( !IS_SET(clan_table[i].flags, CLAN_DELETED)
                && (clan_table[i].name != NULL))
        {
            fprintf(fp, "\nClan\t%s~\n", clan_table[i].name);

            fprintf(fp, "Who\t%s~\n", clan_table[i].who_name);
            fprintf(fp, "Type\t%d\n", clan_table[i].clan_type);
            fprintf(fp, "Bank\t%d\n", clan_table[i].bank);
            fprintf(fp, "Hall\t%d\n", clan_table[i].clanhall);
            fprintf(fp, "Bonus\t%d\n", clan_table[i].bonus);
                 if (clan_table[i].banker == NULL
                    || clan_table[i].banker[0] == '\0')
                clan_table[i].banker = str_dup("None");
            fprintf(fp, "Banker\t%s~\n",  clan_table[i].banker);

            fprintf(fp, "Leader\t%s~\n", clan_table[i].leader);


            /* Remove "changed" bit before writing flags to file */
            if (IS_SET(clan_table[i].flags, CLAN_CHANGED))
                REMOVE_BIT(clan_table[i].flags, CLAN_CHANGED);

        fprintf( fp, "Flags %s\n",  fwrite_flag( clan_table[i].flags, buf ) );
        fprintf(fp, "\n");
        }
    }

    fprintf(fp, "\nEnd\n");
    fclose(fp);
    return;
}

/*
void save_helps( FILE *fp, HELP_AREA *ha )
{
    HELP_DATA *help = ha->first;

    fprintf( fp, "#HELPS\n" );

    for ( ; help; help = help->next_area )
    {
        fprintf( fp, "%d %s~\n", help->level, help->keyword );
        fprintf( fp, "%s~\n\n", fix_string( help->text ) );
    }

    fprintf( fp, "-1 $~\n\n" );

    ha->changed = FALSE;

    return;
}*/
/* disabled due to the fact it erases area files - Ugha
void save_other_helps( CHAR_DATA *ch )
{
    extern HELP_AREA * had_list;
    HELP_AREA *ha;
    FILE *fp;

    for ( ha = had_list; ha; ha = ha->next )
        if ( ha->changed == TRUE )
        {
            fp = fopen( ha->filename, "w" );

            if ( !fp )
            {
                perror( ha->filename );
                return;
            }

            save_helps( fp, ha );

            if (ch)
                printf_to_char( ch, "%s\n\r", ha->filename );

            fprintf( fp, "#$\n" );
            fclose( fp );
        }

    return;
}
*/
/*****************************************************************************
 Name:      save_area
 Purpose:   Save an area, note that this format is new.
 Called by: do_asave(olc_save.c).
 ****************************************************************************/
void save_area( AREA_DATA *pArea )
{
    FILE *fp;
    char areaLoc[MIL];
    fclose( fpReserve );
    sprintf(areaLoc, "%s%s", AREA_DIR, pArea->file_name);
    if ( !( fp = fopen( areaLoc, "w" ) ) )
    {
    bug( "Open_area: fopen", 0 );
    perror( pArea->file_name );
    }

    fprintf( fp, "#AREADATA\n" );
    fprintf( fp, "Name %s~\n",        pArea->name );
    fprintf( fp, "Builders %s~\n",        fix_string( pArea->builders ) );
    fprintf( fp, "VNUMs %d %d\n",      pArea->min_vnum, pArea->max_vnum );
    fprintf( fp, "Credits %s~\n",    pArea->credits );
    fprintf( fp, "Security %d\n",         pArea->security );
	fprintf( fp, "Flags %d\n", pArea->area_flags );
    fprintf( fp, "Lvls %d %d\n",  pArea->low_range, pArea->high_range);
    if (pArea->domain != NULL)
    {
        fprintf(fp, "Clan %d\n", pArea->domain->clan);
        fprintf( fp, "Tax %d\n", pArea->domain->tax);
        fprintf( fp, "Infl %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        pArea->domain->influence[0], pArea->domain->influence[1], pArea->domain->influence[2],
        pArea->domain->influence[3], pArea->domain->influence[4],
        pArea->domain->influence[5], pArea->domain->influence[6],
        pArea->domain->influence[7], pArea->domain->influence[8],
        pArea->domain->influence[9], pArea->domain->influence[10],
        pArea->domain->influence[11], pArea->domain->influence[12],
        pArea->domain->influence[13], pArea->domain->influence[14],
        pArea->domain->influence[15], pArea->domain->influence[16],
        pArea->domain->influence[17], pArea->domain->influence[18],
        pArea->domain->influence[19], pArea->domain->influence[20],
        pArea->domain->influence[21], pArea->domain->influence[22],
        pArea->domain->influence[23], pArea->domain->influence[24],
        pArea->domain->influence[25], pArea->domain->influence[26] );
        fprintf( fp, "Relation %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        pArea->domain->relation[0], pArea->domain->relation[1], pArea->domain->relation[2],
        pArea->domain->relation[3], pArea->domain->relation[4],
        pArea->domain->relation[5], pArea->domain->relation[6],
        pArea->domain->relation[7], pArea->domain->relation[8],
        pArea->domain->relation[9], pArea->domain->relation[10],
        pArea->domain->relation[11], pArea->domain->relation[12],
        pArea->domain->relation[13], pArea->domain->relation[14],
        pArea->domain->relation[15], pArea->domain->relation[16],
        pArea->domain->relation[17], pArea->domain->relation[18],
        pArea->domain->relation[19], pArea->domain->relation[20],
        pArea->domain->relation[21], pArea->domain->relation[22],
        pArea->domain->relation[23], pArea->domain->relation[24],
        pArea->domain->relation[25], pArea->domain->relation[26]);
        }
    fprintf( fp, "End\n\n\n\n" );

    save_mobiles( fp, pArea );
    save_objects( fp, pArea );
    save_rooms( fp, pArea );
    save_specials( fp, pArea );
    save_resets( fp, pArea );
    save_shops( fp, pArea );
    save_mobprogs( fp, pArea );
    save_objprogs( fp, pArea );
    save_roomprogs( fp, pArea );
/*
    if ( pArea->helps && pArea->helps->first )
    save_helps( fp, pArea->helps );
*/
    fprintf( fp, "#$\n" );

    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*****************************************************************************
 Name:      do_asave
 Purpose:   Entry point for saving area data.
 Called by: interpreter(interp.c)
 ****************************************************************************/
void do_asave( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    AREA_DATA *pArea;
    FILE *fp;
    int value, sec;

    fp = NULL;

    if ( !ch )       /* Do an autosave */
    sec = 9;
    else if ( !IS_NPC(ch) )
        sec = ch->pcdata->security;
    else
        sec = 0;

/*    {
    save_area_list();
    for( pArea = area_first; pArea; pArea = pArea->next )
    {
        save_area( pArea );
        REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
    }
    return;
    } */

    smash_tilde( argument );
    strcpy( arg1, argument );

    if ( arg1[0] == '\0' )
    {
    if (ch)
    {
        send_to_char( "Syntax:\n\r", ch );
        send_to_char( "  asave <vnum>   - saves a particular area\n\r", ch );
        send_to_char( "  asave list     - saves the area.lst file\n\r", ch );
        send_to_char( "  asave area     - saves the area being edited\n\r", ch );
        send_to_char( "  asave changed  - saves all changed zones\n\r", ch );
        send_to_char( "  asave world    - saves the world! (db dump)\n\r",  ch );
        send_to_char( "  asave helps    - saves the help files\n\r",    ch );
        send_to_char( "  asave commands - saves the command table\n\r", ch);
        send_to_char( "  asave config   - saves configuration file\n\r", ch);
        send_to_char( "  asave quests   - saves quest item table\n\r", ch);
        send_to_char( "\n\r", ch );
    }

    return;
    }

    /* Snarf the value (which need not be numeric). */
    value = atoi( arg1 );
    if ( !( pArea = get_area_data( value ) ) && is_number( arg1 ) )
    {
    if (ch)
        send_to_char( "That area does not exist.\n\r", ch );
    return;
    }

    /* Save area of given vnum. */
    /* ------------------------ */
    if ( is_number( arg1 ) )
    {
    if ( ch && !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "You are not a builder for this area.\n\r", ch );
        return;
    }

    save_area_list();
    save_area( pArea );

    send_to_char("Area saved.\n\r", ch);
    return;
    }

    /* Save the world, only authorized areas. */
    /* -------------------------------------- */
    if ( !str_cmp( "world", arg1 ) )
    {
    save_clans (ch, argument);
    save_area_list();
    for( pArea = area_first; pArea; pArea = pArea->next )
    {
        /* Builder must be assigned this area. */
        if ( ch && !IS_BUILDER( ch, pArea ) )
        continue;

        REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
        REMOVE_BIT( pArea->area_flags, AREA_ADDED );
        REMOVE_BIT( pArea->area_flags, AREA_LOADING);
        save_area( pArea );
    }

    if ( ch )
        send_to_char( "You saved the world.\n\r", ch );

//  save_other_helps( NULL );

    return;
    }

    /* Save changed areas, only authorized areas. */
    /* ------------------------------------------ */
    if ( !str_cmp( "changed", arg1 ) )
    {
    char buf[MAX_INPUT_LENGTH];

    save_area_list();

    if ( ch )
        send_to_char( "Saved zones:\n\r", ch );
    else
        log_string( "Autosave: Zones..." );

    sprintf( buf, "None.\n\r" );

    for( pArea = area_first; pArea; pArea = pArea->next )
    {
        /* Builder must be assigned this area. */
        if ( ch && !IS_BUILDER( ch, pArea ) )
        continue;

        /* Save changed areas. */
        if ( IS_SET(pArea->area_flags, AREA_CHANGED) )
        {
            REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
            REMOVE_BIT( pArea->area_flags, AREA_ADDED );
            REMOVE_BIT( pArea->area_flags, AREA_LOADING);
            save_area( pArea );

            if ( ch )
            {
                sprintf( buf, "%24s - '%s'\n\r", pArea->name, pArea->file_name );
                send_to_char( buf, ch );
            }
            else {
                sprintf( buf, "%s - '%s'", pArea->name, pArea->file_name );
                log_string( buf );
            }
        }
    }

//  save_other_helps( ch );

    if ( !str_cmp( buf, "None.\n\r" ) )
        {
                if ( ch )
            send_to_char( buf, ch );
        else
            log_string( "None." );
        }
       char buf1[MIL];
        int i;

        for (i = 1; clan_lookup(clan_table[i].name) != 0; i++)
        {
            if (clan_lookup (clan_table[i].name) != 0
                && IS_SET(clan_table[i].flags, CLAN_CHANGED)
                && !IS_SET(clan_table[i].flags, CLAN_DELETED) )
            {
                save_clans(ch, argument);
                sprintf (buf1, "\n\rClan '%s' saved.", clan_table[i].name);
                if (ch)
                {
                    sendch (buf1, ch);
                    sendch ("\n\r", ch);
                }
                else
                    log_string(buf1);
            }
        } //Note: make sure return is after this last bracket here
    return;
    }

    /* Save the area.lst file. */
    /* ----------------------- */
    if ( !str_cmp( arg1, "list" ) )
    {
    save_area_list();
    return;
    }

    if (!str_prefix( arg1, "commands") )
    {
            save_cmd_table();
            if (ch)
            send_to_char("Command table saved.\n\r", ch);
            else
        log_string("Autosave: Command Table");
            return;
        }

    if (!str_prefix( arg1, "config") )
    {
        save_config( );
        if (ch)
        sendch("Config file saved.", ch);
        else
        log_string("Autosave: Config");
        return;
    }
    if (!str_prefix( arg1, "quests") )
    {
        save_qitem_table();
        if (ch)
            sendch("QItem Table Saved.\n\r", ch);
        else
            log_string("Autosave: Quests");
        return;
    }
    /* Save area being edited, if authorized. */
    /* -------------------------------------- */
    if ( !str_cmp( arg1, "area" ) )
    {
    if ( !ch || !ch->desc )
        return;

    /* Is character currently editing. */
    if ( ch->desc->editor == ED_NONE )
    {
        send_to_char( "You are not editing an area, "
        "therefore an area vnum is required.\n\r", ch );
        return;
    }

    /* Find the area to save. */
    switch (ch->desc->editor)
    {
        case ED_AREA:
        pArea = (AREA_DATA *)ch->desc->pEdit;
        break;
        case ED_ROOM:
        pArea = ch->in_room->area;
        break;
        case ED_OBJECT:
        pArea = ( (OBJ_INDEX_DATA *)ch->desc->pEdit )->area;
        break;
        case ED_MOBILE:
        pArea = ( (MOB_INDEX_DATA *)ch->desc->pEdit )->area;
        break;
        case ED_HELP:
        send_to_char( "Saving area : ", ch );
//          save_other_helps( ch );
            return;
        default:
        pArea = ch->in_room->area;
        break;
    }

    if ( !IS_BUILDER( ch, pArea ) )
    {
        send_to_char( "You are not a builder for this area.\n\r", ch );
        return;
    }

    save_area_list();
    save_area( pArea );
    REMOVE_BIT( pArea->area_flags, AREA_CHANGED );
    send_to_char( "Area saved.\n\r", ch );
    return;
    }
     /* Save Help File */
    if(!str_prefix(arg1, "helps"))
    {
        save_helps();
        if (ch)
        send_to_char( "Helps Saved.\n\r", ch);
    else
        log_string("Autosave: Helps");
        return;
    }
    /* Show correct syntax. */
    /* -------------------- */
    if (ch)
    do_asave( ch, "" );

    return;
}

void save_helps()
{
    HELP_DATA * pHelp;
    FILE * fp;

    if (! (fp = fopen( HELP_FILE, "w") ) )
    {
        bug( "Open_help: fopen", 0);
        perror(HELP_FILE);
     }

        for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
        {
            if(pHelp->delete)
                continue;

            fprintf(fp, "%d %s~\n\n%s~\n",
                    pHelp->level, pHelp->keyword, fix_string(pHelp->text));
        }

        fprintf(fp,"0 $~\n\n#$\n");
        fclose(fp);
        return;
}

void save_qitem_table()
{
    FILE *fp;
    QITEM_DATA *qitem;

    if( (fp = fopen(QUEST_FILE,"w+") ) != NULL )
    {
        for(qitem = qitem_list; qitem ; qitem = qitem->next )
        {

            if (qitem->delete == TRUE)
                continue;

            fprintf(fp, "Name %s~\n",       qitem->name );
            fprintf(fp, "Ivnum %d\n",       qitem->qobjvnum );
            fprintf(fp, "Placed %d\n",      qitem->place );
            fprintf(fp, "Room %d\n",        qitem->roomvnum );
            fprintf(fp, "Mob %d\n",         qitem->mobvnum );
            fprintf(fp, "Obj %d\n",         qitem->objvnum );
            fprintf(fp, "Notify %d\n",      qitem->notify );
            fprintf(fp, "Notified %s~\n",   qitem->notified );
            fprintf(fp, "Found %d\n",       qitem->found    );
            fprintf(fp, "Foundby %s~\n",    qitem->foundby);
            fprintf(fp, "End\n\n");
        }
    }
    fprintf(fp, "$\n");
    fclose(fp);
}
void load_quest_list()
{
    FILE *fp;
    QITEM_DATA *qitem;
    char *word;
    if((fp = fopen(QUEST_FILE, "r" )) == NULL)
    {   bug("Load_quest_list: Quest File not found!", 0);
        return;
    }
    for( ;; )
    {
        word = feof (fp) ? "End" : fread_word (fp);
        if(word[0] == '$' )
            return;
        switch (UPPER(word[0] ))
        {
            case 'E':
                if(!str_cmp(word, "End" ) )
                    break;
                break;
            case 'F':
                if (!str_cmp(word, "Found"))
                {
                    qitem->found = fread_number(fp);
                    break;
                }
                if (!str_cmp(word, "Foundby"))
                {
                    qitem->foundby = fread_string(fp);
                    break;
                }
                break;
            case 'I':
                if(!str_cmp(word, "Ivnum"))
                {
                    qitem->qobjvnum = fread_number(fp);
                    break;
                }
                break;
            case 'N':
                if(!str_cmp(word, "Name" ) )
                {   qitem = new_qitem();
                    qitem->name = fread_string(fp);
                    break;
                }
                if(!str_cmp(word, "Notify" ) )
                {
                    qitem->notify = fread_number(fp);
                    break;
                }
                if (!str_cmp(word, "Notified"))
                {
                    qitem->notified = fread_string(fp);
                    break;
                }
                break;
            case 'M':
                if(!str_cmp(word, "Mob"))
                {
                    qitem->mobvnum = fread_number(fp);
                    break;
                }
                break;
            case 'O':
                if(!str_cmp(word, "Obj"))
                {
                    qitem->objvnum = fread_number(fp);
                    break;
                }
                break;
            case 'P':
                if(!str_cmp(word, "Placed"))
                {
                    qitem->place = fread_number(fp);
                    break;
                }
                break;
            case 'R':
                if(!str_cmp(word, "Room"))
                {
                    qitem->roomvnum = fread_number(fp);
                    break;
                }
                break;

        }//End of switch
    }//End of for

    file_close(fp);
}
