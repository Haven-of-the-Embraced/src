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
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif


/* int rename(const char *oldfname, const char *newfname); viene en stdio.h */

char *print_flags(int flag)
{
    int count, pos = 0;
    static char buf[52];


    for (count = 0; count < 32;  count++)
    {
        if (IS_SET(flag,1<<count))
        {
            if (count < 26)
                buf[pos] = 'A' + count;
            else
                buf[pos] = 'a' + (count - 26);
            pos++;
        }
    }

    if (pos == 0)
    {
        buf[pos] = '0';
        pos++;
    }

    buf[pos] = '\0';

    return buf;
}


/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST    100
static  OBJ_DATA *  rgObjNest   [MAX_NEST];



/*
 * Local functions.
 */
void    fwrite_char args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fwrite_obj  args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
                FILE *fp, int iNest ) );
void    fwrite_pet  args( ( CHAR_DATA *pet, FILE *fp) );
void    fwrite_mount args( ( CHAR_DATA *pet, FILE *fp) );
void    fread_char  args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_pet   args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_mount args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_obj   args( ( CHAR_DATA *ch,  FILE *fp ) );



/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    FILE *fp;

    if ( IS_NPC(ch) )
    return;

    if ( ch->desc != NULL && ch->desc->original != NULL )
    ch = ch->desc->original;

#if defined(unix)
    /* create god log */
    if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
    {
    fclose(fpReserve);
    sprintf(strsave, "%s%s",GOD_DIR, capitalize(ch->name));
    if ((fp = fopen(strsave,"w")) == NULL)
    {
        bug("Save_char_obj: fopen",0);
        perror(strsave);
    }

    fprintf(fp,"Lev %2d Trust %2d  %s%s\n",
        ch->level, get_trust(ch), ch->name, ch->pcdata->title);
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    }
#endif

    fclose( fpReserve );
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
    if ( ( fp = fopen( TEMP_FILE, "w" ) ) == NULL )
    {
    bug( "Save_char_obj: fopen", 0 );
    perror( strsave );
    }
    else
    {
    fwrite_char( ch, fp );
    if ( ch->carrying != NULL )
        fwrite_obj( ch, ch->carrying, fp, 0 );
    /* save the pets */
    if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
        fwrite_pet(ch->pet,fp);
    fprintf( fp, "#END\n" );
        if (ch->mount && ch->mount->in_room == ch->in_room)
            fwrite_mount(ch->mount, fp);
        fprintf( fp, "#END\n" );

    }
    fclose( fp );
    rename(TEMP_FILE,strsave);
    sprintf(buf, "cp %s %s", strsave, BK_PLAYER_DIR);
    system(buf);
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    AFFECT_DATA *paf;
    int sn, gn, pos;

    fprintf( fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER" );
    fprintf( fp, "Name %s~\n",  ch->name        );
    if(!IS_NPC(ch))
    {
        fprintf( fp, "ICtotal %ld\n",ch->pcdata->IC_total);
        fprintf( fp, "ICrank %ld\n",ch->pcdata->IC_rank);
        fprintf( fp, "ICgoal %ld\n",ch->pcdata->IC_goal);
        fprintf (fp, "Influ %d\n", ch->pcdata->influenced);

        if(ch->pcdata->registered)
        {
            fprintf( fp, "Registered %d\n",ch->pcdata->registered);
            fprintf( fp, "Email %s~\n",ch->pcdata->email);
        }
    }
    if (ch->desc)
      fprintf( fp, "Hostname %s~\n", ch->desc->host);

    fprintf( fp, "Vers %d\n",   ch->version > 0 ? ch->version : 6);
    fprintf( fp, "Home %d\n", ch->pcdata->home );
    fprintf( fp, "Hometown %d\n", ch->pcdata->hometown);
    fprintf( fp, "Admin %d\n",ch->pcdata->admin);
    fprintf( fp, "qpoints %d\n", ch->qpoints );
    fprintf( fp, "qpointstot %d\n", ch->totalqpoints);
    fprintf( fp, "pblood %d\n", ch->pblood );
    fprintf( fp, "unlockp %d\n", ch->unlocksspent);
    fprintf( fp, "totalkills %d\n", ch->totalkills);
    fprintf( fp, "currentkills %d\n", ch->currentkills);
    fprintf( fp, "maxdamage %d\n", ch->maxdamage);
    fprintf( fp, "cheater %d\n", ch->cheater);
    fprintf( fp, "cslock %d\n", ch->cslock);
    fprintf( fp, "csdemeanor %s~\n", ch->pcdata->csdemeanor);
    fprintf( fp, "csnature %s~\n", ch->pcdata->csnature);
    fprintf( fp, "ip %d\n", ch->pcdata->ip);
    fprintf( fp, "tagged %d\n", ch->tagged);
        fprintf( fp, "max_pblood %d\n", ch->max_pblood );
        fprintf( fp, "gen %d\n", ch->gen );

//    fprintf( fp, "max_pblood %d\n", ch->max_pblood );
//    fprintf( fp, "gen %d\n", is_affected(ch,gsn_bloodofpotency) ? ch->gen+1 : ch->gen );
// 3/25/12 Matthew - Banking system
    if (ch->pcdata->bank != 0)
 fprintf( fp, "Bank  %d\n", ch->pcdata->bank     );
    fprintf( fp, "Sire %s~\n", ch->sire );
    fprintf( fp, "Apprentice %s~\n", ch->apprentice );
    fprintf( fp, "Vamp_Master %s~\n", ch->vamp_master );
    fprintf( fp, "Agg %d\n", ch->agg );
    fprintf( fp, "Immun %s\n",     print_flags( ch->imm_flags) );
    fprintf( fp, "Resis %s\n",     print_flags( ch->res_flags) );
    fprintf( fp, "Vulne %s\n",    print_flags( ch->vuln_flags) );
    fprintf( fp, "Dpoints %d\n", ch->dpoints );
    fprintf( fp, "Remorts %d\n", ch->remorts );
    fprintf( fp, "Bonded %d\n", ch->bonded );
    fprintf( fp, "Inhumanity %d\n", ch->inhumanity );
    fprintf( fp, "Rage %d\n", ch->rage );
    fprintf( fp, "Changed %d\n", ch->changed );
    fprintf( fp, "Shift %s~\n", ch->shift );
    fprintf( fp, "Rank %d\n", ch->rank );
    fprintf( fp, "Tradition %d\n", ch->tradition );
    fprintf( fp, "Arete %d\n", ch->arete );
    fprintf( fp, "Avatar %d\n", ch->avatar );
    fprintf( fp, "Paradox %d\n", ch->paradox );
    fprintf( fp, "Quintessence %d\n", ch->quintessence );
    fprintf( fp, "Chimerstry %d\n", ch->chimerstry ) ;
    fprintf( fp, "Maxquint %d\n", ch->max_quintessence );
    fprintf( fp, "Foci %d\n", ch->foci );
    fprintf( fp, "Spheres %d %d %d %d %d %d %d %d %d\n",
    ch->sphere[1], ch->sphere[2], ch->sphere[3], ch->sphere[4], ch->sphere[5], ch->sphere[6], ch->sphere[7],
    ch->sphere[8], ch->sphere[9]);
//    fprintf( fp, "Drains %d\n", ch->drains );
    fprintf( fp, "Embraced %d\n", ch->embraced );
    fprintf( fp, "Childer %d\n", ch->childer );
    fprintf( fp, "Breed %d\n", ch->breed );
    fprintf( fp, "Auspice %d\n", ch->auspice );
    fprintf( fp, "Gift1 %s~\n", ch->gift1 );
    fprintf( fp, "Gift2 %s~\n", ch->gift2 );
    fprintf( fp, "Gift3 %s~\n", ch->gift3 );
    fprintf( fp, "Gift4 %s~\n", ch->gift4 );
    fprintf( fp, "Gift5 %s~\n", ch->gift5 );
    if(ch->pcdata->rank > 0)
    {
        fprintf( fp, "grenown %d %d %d %d %d %d\n", ch->pcdata->renown[GLORY], ch->pcdata->renown[TEMP_GLORY],
        ch->pcdata->renown[HONOR], ch->pcdata->renown[TEMP_HONOR], ch->pcdata->renown[WISDOM], ch->pcdata->renown[TEMP_WISDOM]);
        fprintf( fp, "primal %d\n", ch->pcdata->primal_urge);
        fprintf( fp, "shiftform %d\n", ch->pcdata->shiftform);
        fprintf( fp, "grage %d %d\n", ch->pcdata->rage[PERM], ch->pcdata->rage[TEMP]);
        fprintf( fp, "gnosis %d %d\n", ch->pcdata->gnosis[PERM], ch->pcdata->gnosis[TEMP]);
        fprintf( fp, "grank %d\n", ch->pcdata->rank);
        fprintf( fp, "TBA %d %d %d\n", ch->pcdata->tribe, ch->pcdata->breed, ch->pcdata->auspice);
        fprintf( fp, "fur %s~\n", ch->pcdata->garou_fur);
            fprintf( fp, "Newgifts %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->gift[0],  ch->pcdata->gift[1],  ch->pcdata->gift[2],
    ch->pcdata->gift[3],  ch->pcdata->gift[4],  ch->pcdata->gift[5],
    ch->pcdata->gift[6],  ch->pcdata->gift[7],  ch->pcdata->gift[8],
    ch->pcdata->gift[9],  ch->pcdata->gift[10], ch->pcdata->gift[11],
    ch->pcdata->gift[12], ch->pcdata->gift[13], ch->pcdata->gift[14],
    ch->pcdata->gift[15], ch->pcdata->gift[16], ch->pcdata->gift[17],
    ch->pcdata->gift[18], ch->pcdata->gift[19], ch->pcdata->gift[20],
    ch->pcdata->gift[21], ch->pcdata->gift[22], ch->pcdata->gift[23],
    ch->pcdata->gift[24], ch->pcdata->gift[25], ch->pcdata->gift[26],
    ch->pcdata->gift[27], ch->pcdata->gift[28], ch->pcdata->gift[29],
        ch->pcdata->gift[30],  ch->pcdata->gift[31],  ch->pcdata->gift[32],
    ch->pcdata->gift[33],  ch->pcdata->gift[34],  ch->pcdata->gift[35],
    ch->pcdata->gift[36],  ch->pcdata->gift[37],  ch->pcdata->gift[38],
    ch->pcdata->gift[39],  ch->pcdata->gift[40], ch->pcdata->gift[41],
    ch->pcdata->gift[42], ch->pcdata->gift[43], ch->pcdata->gift[44],
    ch->pcdata->gift[45], ch->pcdata->gift[46], ch->pcdata->gift[47],
    ch->pcdata->gift[48], ch->pcdata->gift[49], ch->pcdata->gift[40],
    ch->pcdata->gift[51], ch->pcdata->gift[52], ch->pcdata->gift[53],
    ch->pcdata->gift[54], ch->pcdata->gift[55], ch->pcdata->gift[56],
    ch->pcdata->gift[57], ch->pcdata->gift[58], ch->pcdata->gift[59],
        ch->pcdata->gift[60],  ch->pcdata->gift[61],  ch->pcdata->gift[62],
    ch->pcdata->gift[63],  ch->pcdata->gift[64],  ch->pcdata->gift[65],
    ch->pcdata->gift[66],  ch->pcdata->gift[67],  ch->pcdata->gift[68],
    ch->pcdata->gift[69],  ch->pcdata->gift[70], ch->pcdata->gift[71],
    ch->pcdata->gift[72], ch->pcdata->gift[73], ch->pcdata->gift[74],
    ch->pcdata->gift[75], ch->pcdata->gift[76], ch->pcdata->gift[77],
    ch->pcdata->gift[78], ch->pcdata->gift[79], ch->pcdata->gift[80],
    ch->pcdata->gift[81], ch->pcdata->gift[82], ch->pcdata->gift[83],
    ch->pcdata->gift[84], ch->pcdata->gift[85], ch->pcdata->gift[86],
    ch->pcdata->gift[87], ch->pcdata->gift[88], ch->pcdata->gift[89]);
    }
    fprintf( fp, "Renown %d\n", ch->renown );
    fprintf( fp, "freebie %d\n", ch->freebie );
    fprintf( fp, "progress %d\n", ch->pcdata->progress );
    if(ch->pcdata->progress < 6)
    {
        fprintf( fp, "primary %d\n", ch->pcdata->primary );
        fprintf( fp, "secondary %d\n", ch->pcdata->secondary );
        fprintf( fp, "tertiary %d\n", ch->pcdata->tertiary );
        fprintf( fp, "cscreate %d %d %d %d %d %d\n", ch->pcdata->stat[PHYSICAL], ch->pcdata->stat[SOCIAL], ch->pcdata->stat[MENTAL], ch->pcdata->stat[SKILLS], ch->pcdata->stat[TALENTS], ch->pcdata->stat[KNOWLEDGES]);
    }

//    fprintf( fp, "CSname %s~\n",    ch->pcdata->csname);
//    fprintf( fp, "CSchronicle %s~\n",   ch->pcdata->cschronicle);
//    fprintf( fp, "CSclan %s~\n",    ch->pcdata->csclan);
//    fprintf( fp, "CSconcept %s~\n", ch->pcdata->csconcept);
//    fprintf( fp, "CSsect %s~\n",    ch->pcdata->cssect);
//    fprintf( fp, "CSsire %s~\n",    ch->pcdata->cssire);
    fprintf( fp, "CSgeneration %d\n", ch->pcdata->csgeneration );
//    fprintf( fp, "CShaven %s~\n",   ch->pcdata->cshaven);

    if (ch->nextquest != 0)
        fprintf( fp, "QuestNext %d\n",  ch->nextquest   );
    else if (ch->countdown != 0)
        fprintf( fp, "QuestNext %d\n",  30              );
    fprintf( fp, "CSattributes %d %d %d %d %d %d %d %d %d\n",
    ch->csattributes[CSATTRIB_STRENGTH],    ch->csattributes[CSATTRIB_DEXTERITY],
    ch->csattributes[CSATTRIB_STAMINA],     ch->csattributes[CSATTRIB_CHARISMA],
    ch->csattributes[CSATTRIB_MANIPULATION],    ch->csattributes[CSATTRIB_APPEARANCE],
    ch->csattributes[CSATTRIB_PERCEPTION],  ch->csattributes[CSATTRIB_INTELLIGENCE],
    ch->csattributes[CSATTRIB_WITS]);

    fprintf( fp, "CSattrib_mod %d %d %d %d %d %d %d %d %d\n",
    ch->csattrib_mod[CSATTRIB_STRENGTH],    ch->csattrib_mod[CSATTRIB_DEXTERITY],
    ch->csattrib_mod[CSATTRIB_STAMINA],     ch->csattrib_mod[CSATTRIB_CHARISMA],
    ch->csattrib_mod[CSATTRIB_MANIPULATION],    ch->csattrib_mod[CSATTRIB_APPEARANCE],
    ch->csattrib_mod[CSATTRIB_PERCEPTION],  ch->csattrib_mod[CSATTRIB_INTELLIGENCE],
    ch->csattrib_mod[CSATTRIB_WITS]);

    fprintf( fp, "CSabilities %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->csabilities[0], ch->csabilities[1], ch->csabilities[2],
    ch->csabilities[3], ch->csabilities[4], ch->csabilities[5],
    ch->csabilities[6], ch->csabilities[7], ch->csabilities[8],
    ch->csabilities[9], ch->csabilities[10],    ch->csabilities[11],
    ch->csabilities[12],    ch->csabilities[13],    ch->csabilities[14],
    ch->csabilities[15],    ch->csabilities[16],    ch->csabilities[17],
    ch->csabilities[18],    ch->csabilities[19],    ch->csabilities[20],
    ch->csabilities[21],    ch->csabilities[22],    ch->csabilities[23],
    ch->csabilities[24],    ch->csabilities[25],    ch->csabilities[26],
    ch->csabilities[27],    ch->csabilities[28],    ch->csabilities[29]);

    fprintf( fp, "CSsec_abil %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->cssec_abil[0],  ch->pcdata->cssec_abil[1],  ch->pcdata->cssec_abil[2],
    ch->pcdata->cssec_abil[3],  ch->pcdata->cssec_abil[4],  ch->pcdata->cssec_abil[5],
    ch->pcdata->cssec_abil[6],  ch->pcdata->cssec_abil[7],  ch->pcdata->cssec_abil[8],
    ch->pcdata->cssec_abil[9],  ch->pcdata->cssec_abil[10], ch->pcdata->cssec_abil[11],
    ch->pcdata->cssec_abil[12], ch->pcdata->cssec_abil[13], ch->pcdata->cssec_abil[14],
    ch->pcdata->cssec_abil[15], ch->pcdata->cssec_abil[16], ch->pcdata->cssec_abil[17],
    ch->pcdata->cssec_abil[18], ch->pcdata->cssec_abil[19], ch->pcdata->cssec_abil[20],
    ch->pcdata->cssec_abil[21], ch->pcdata->cssec_abil[22], ch->pcdata->cssec_abil[23],
    ch->pcdata->cssec_abil[24], ch->pcdata->cssec_abil[25], ch->pcdata->cssec_abil[26],
    ch->pcdata->cssec_abil[27], ch->pcdata->cssec_abil[28], ch->pcdata->cssec_abil[29]);

// Why the hell doesn't this work? - Ugha

    if(IS_VAMP(ch))
    {
/*
        int i;

        fprintf( fp, "Disciplines");
        for(i = 0; i < MAX_DISC;i++);
            fprintf( fp, " %d", ch->pcdata->discipline[i]);
        fprintf( fp, "\n");
*/
        fprintf( fp, "Disciplines %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        ch->pcdata->discipline[0], ch->pcdata->discipline[1], ch->pcdata->discipline[2],
        ch->pcdata->discipline[3], ch->pcdata->discipline[4], ch->pcdata->discipline[5],
        ch->pcdata->discipline[6], ch->pcdata->discipline[7], ch->pcdata->discipline[8],
        ch->pcdata->discipline[9], ch->pcdata->discipline[10], ch->pcdata->discipline[11],
        ch->pcdata->discipline[12], ch->pcdata->discipline[13], ch->pcdata->discipline[14],
        ch->pcdata->discipline[15], ch->pcdata->discipline[16], ch->pcdata->discipline[17]);
    }

    fprintf( fp, "CSbackgrounds %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
    ch->pcdata->csbackgrounds[0],   ch->pcdata->csbackgrounds[1],   ch->pcdata->csbackgrounds[2],
    ch->pcdata->csbackgrounds[3],   ch->pcdata->csbackgrounds[4],   ch->pcdata->csbackgrounds[5],
    ch->pcdata->csbackgrounds[6],   ch->pcdata->csbackgrounds[7],   ch->pcdata->csbackgrounds[8],
    ch->pcdata->csbackgrounds[9],   ch->pcdata->csbackgrounds[10],  ch->pcdata->csbackgrounds[11],
    ch->pcdata->csbackgrounds[12],  ch->pcdata->csbackgrounds[13],  ch->pcdata->csbackgrounds[14],
    ch->pcdata->csbackgrounds[15],  ch->pcdata->csbackgrounds[16],  ch->pcdata->csbackgrounds[17],
    ch->pcdata->csbackgrounds[18]);

    fprintf( fp, "CSvirtues %d %d %d\n",
    ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], ch->pcdata->csvirtues[CSVIRT_COURAGE]);

    fprintf( fp, "CShumanity %d\n", ch->pcdata->cshumanity );
    fprintf( fp, "CSmaxwillpower %d\n", ch->csmax_willpower );
    fprintf( fp, "CSwillpower %d\n", ch->cswillpower );
/*
    fprintf( fp, "CSmaxbloodpool %d\n", ch->pcdata->csmax_bloodpool );
    fprintf( fp, "CSbloodpool %d\n", ch->pcdata->csbloodpool );
    fprintf( fp, "CShealth %d\n", ch->pcdata->cshealth );
    fprintf( fp, "CSexp %d\n", ch->pcdata->csexp );
*/
    fprintf( fp, "Id   %ld\n", ch->id           );
    fprintf( fp, "LogO %ld\n",  current_time        );
    fprintf( fp, "kill_target %s~\n",   ch->pcdata->kill_target);
    if (!IS_NULLSTR(ch->short_descr))
        fprintf( fp, "ShD  %s~\n",  ch->short_descr );
    if( !IS_NULLSTR(ch->long_descr))
    fprintf( fp, "LnD  %s~\n",  ch->long_descr  );
    if (!IS_NULLSTR(ch->description))
        fprintf( fp, "Desc %s~\n",  ch->description );
    if (!IS_NULLSTR(ch->pcdata->playernotes))
        fprintf( fp, "Plrnote %s~\n", ch->pcdata->playernotes);
    if (ch->prompt != NULL || !str_cmp(ch->prompt,"<%hhp %mm %vmv> "))
        fprintf( fp, "Prom %s~\n",      ch->prompt      );
    fprintf( fp, "Race %s~\n", race_table[ch->race].name );
    if (ch->clan)
        fprintf( fp, "Clan %s~\n",clan_table[ch->clan].name);
    fprintf( fp, "Sex  %d\n",   ch->sex         );
    fprintf( fp, "Cla  %d\n",   ch->class       );
    fprintf( fp, "Levl %d\n",   ch->level       );
    if (!IS_NULLSTR(ch->pcdata->wiziname))
    fprintf( fp, "Wizin  %s~\n", ch->pcdata->wiziname);
    if (!IS_NULLSTR(ch->pcdata->pretitle))
    fprintf( fp, "Pretitle  %s~\n", ch->pcdata->pretitle);
    if (!IS_NULLSTR(ch->pcdata->immtitle))
    fprintf( fp, "Immtitle  %s~\n", ch->pcdata->immtitle);
    if (ch->trust != 0)
    fprintf( fp, "Tru  %d\n",   ch->trust   );
    fprintf( fp, "Sec  %d\n",    ch->pcdata->security   );  /* OLC */
    fprintf( fp, "Iclass %d\n", ch->pcdata->immclass);
    fprintf( fp, "Plyd %d\n",
    ch->played + (int) (current_time - ch->logon)   );
    fprintf( fp, "Not  %ld %ld %ld %ld %ld\n",
    ch->pcdata->last_note,ch->pcdata->last_idea,ch->pcdata->last_penalty,
    ch->pcdata->last_news,ch->pcdata->last_changes  );
    fprintf( fp, "Scro %d\n",   ch->lines       );
    fprintf( fp, "Sect %d\n",   ch->pcdata->sect        );
    fprintf( fp, "Room %d\n",
        (  ch->in_room == get_room_index( ROOM_VNUM_LIMBO )
        && ch->was_in_room != NULL )
            ? ch->was_in_room->vnum
            : ch->in_room == NULL ? 3001 : ch->in_room->vnum );

    fprintf( fp, "HMV  %d %d %d %d %d %d\n",
    ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move, ch->max_move );
    fprintf( fp, "Agg_dam %d\n", ch->agg_dam);
    if (ch->gold > 0)
      fprintf( fp, "Gold %ld\n",    ch->gold        );
    else
      fprintf( fp, "Gold %d\n", 0           );
    if (ch->silver > 0)
    fprintf( fp, "Silv %ld\n",ch->silver        );
    else
    fprintf( fp, "Silv %d\n",0          );
    fprintf( fp, "Exp  %d\n",   ch->exp         );
    if (ch->act != 0)
    fprintf( fp, "Act  %s\n",   print_flags(ch->act));
    if (ch->act2 != 0)
    fprintf( fp, "Act2 %s\n",   print_flags(ch->act2));
    if (ch->affected_by != 0)
    fprintf( fp, "AfBy %s\n",   print_flags(ch->affected_by));
    if (ch->affected2_by != 0)
    fprintf( fp, "Af2By %s\n",   print_flags(ch->affected2_by));
    fprintf( fp, "Comm %s\n",       print_flags(ch->comm));
    if (ch->wiznet)
        fprintf( fp, "Wizn %s\n",   print_flags(ch->wiznet));
    if (ch->invis_level)
    fprintf( fp, "Invi %d\n",   ch->invis_level );
    if (ch->incog_level)
    fprintf(fp,"Inco %d\n",ch->incog_level);
    fprintf( fp, "Pos  %d\n",
    ch->position == POS_FIGHTING ? POS_STANDING : ch->position );
    if (ch->practice != 0)
        fprintf( fp, "Prac %d\n",   ch->practice    );
    if (ch->train != 0)
    fprintf( fp, "Trai %d\n",   ch->train   );
    if (ch->saving_throw != 0)
    fprintf( fp, "Save  %d\n",  ch->saving_throw);
    fprintf( fp, "Alig  %d\n",  ch->alignment       );
    if (ch->hitroll != 0)
    fprintf( fp, "Hit   %d\n",  ch->hitroll );
    if (ch->damroll != 0)
    fprintf( fp, "Dam   %d\n",  ch->damroll );
    fprintf( fp, "ACs %d %d %d %d\n",
    ch->armor[0],ch->armor[1],ch->armor[2],ch->armor[3]);
    if (ch->wimpy !=0 )
    fprintf( fp, "Wimp  %d\n",  ch->wimpy   );
    fprintf( fp, "Attr %d %d %d %d %d\n",
    ch->perm_stat[STAT_STR],
    ch->perm_stat[STAT_INT],
    ch->perm_stat[STAT_WIS],
    ch->perm_stat[STAT_DEX],
    ch->perm_stat[STAT_CON] );

    fprintf (fp, "AMod %d %d %d %d %d\n",
    ch->mod_stat[STAT_STR],
    ch->mod_stat[STAT_INT],
    ch->mod_stat[STAT_WIS],
    ch->mod_stat[STAT_DEX],
    ch->mod_stat[STAT_CON] );

    if ( IS_NPC(ch) )
    {
    fprintf( fp, "Vnum %d\n",   ch->pIndexData->vnum    );
    }
    else
    {
    fprintf( fp, "Pass %s~\n",  ch->pcdata->pwd     );
    if (ch->pcdata->bamfin[0] != '\0')
        fprintf( fp, "Bin  %s~\n",  ch->pcdata->bamfin);
    if (ch->pcdata->bamfout[0] != '\0')
        fprintf( fp, "Bout %s~\n",  ch->pcdata->bamfout);
    fprintf( fp, "Titl %s~\n",  ch->pcdata->title   );
        fprintf( fp, "Pnts %d\n",       ch->pcdata->points      );
    fprintf( fp, "TSex %d\n",   ch->pcdata->true_sex    );
    fprintf( fp, "LLev %d\n",   ch->pcdata->last_level  );
    fprintf( fp, "HMVP %d %d %d\n", ch->pcdata->perm_hit,
                           ch->pcdata->perm_mana,
                           ch->pcdata->perm_move);
    fprintf( fp, "Cnd  %d %d %d %d\n",
        ch->pcdata->condition[0],
        ch->pcdata->condition[1],
        ch->pcdata->condition[2],
        ch->pcdata->condition[3] );

    /* write alias */
        for (pos = 0; pos < MAX_ALIAS; pos++)
    {
        if (ch->pcdata->alias[pos] == NULL
        ||  ch->pcdata->alias_sub[pos] == NULL)
        break;

        fprintf(fp,"Alias %s %s~\n",ch->pcdata->alias[pos],
            ch->pcdata->alias_sub[pos]);
    }

    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0 )
        {
        fprintf( fp, "Sk %d '%s'\n",
            ch->pcdata->learned[sn], skill_table[sn].name );
        }
    }

    for ( gn = 0; gn < MAX_GROUP; gn++ )
        {
            if ( group_table[gn].name != NULL && ch->pcdata->group_known[gn])
            {
                fprintf( fp, "Gr '%s'\n",group_table[gn].name);
            }
        }
    }

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
    if (paf->type < 0 || paf->type>= MAX_SKILL)
        continue;

    fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
        skill_table[paf->type].name,
        paf->where,
        paf->level,
        paf->duration,
        paf->modifier,
        paf->location,
        paf->bitvector
        );
    }

    fprintf( fp, "End\n\n" );
    return;
}

/* write a pet */
void fwrite_pet( CHAR_DATA *pet, FILE *fp)
{
    AFFECT_DATA *paf;

    fprintf(fp,"#PET\n");

    fprintf(fp,"Vnum %d\n",pet->pIndexData->vnum);

    fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
        fprintf(fp,"ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
        fprintf(fp,"LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
        fprintf(fp,"Desc %s~\n", pet->description);
    if (pet->race != pet->pIndexData->race)
        fprintf(fp,"Race %s~\n", race_table[pet->race].name);
    if (pet->clan)
        fprintf( fp, "Clan %s~\n",clan_table[pet->clan].name);
    fprintf(fp,"Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
        fprintf(fp,"Levl %d\n", pet->level);
    fprintf(fp, "HMV  %d %d %d %d %d %d\n",
        pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
    if (pet->gold > 0)
        fprintf(fp,"Gold %ld\n",pet->gold);
    if (pet->silver > 0)
    fprintf(fp,"Silv %ld\n",pet->silver);
    if (pet->exp > 0)
        fprintf(fp, "Exp  %d\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
        fprintf(fp, "Act  %s\n", print_flags(pet->act));
    if (pet->affected_by != pet->pIndexData->affected_by)
        fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
    if (pet->affected2_by != pet->pIndexData->affected2_by)
        fprintf(fp, "Af2By %s\n", print_flags(pet->affected2_by));
    if (pet->comm != 0)
        fprintf(fp, "Comm %s\n", print_flags(pet->comm));
    fprintf(fp,"Pos  %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);
    if (pet->saving_throw != 0)
        fprintf(fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != pet->pIndexData->alignment)
        fprintf(fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
        fprintf(fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
        fprintf(fp, "Dam  %d\n", pet->damroll);
    fprintf(fp, "ACs  %d %d %d %d\n",
        pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
    fprintf(fp, "Attr %d %d %d %d %d\n",
        pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
        pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
        pet->perm_stat[STAT_CON]);
    fprintf(fp, "AMod %d %d %d %d %d\n",
        pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
        pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
        pet->mod_stat[STAT_CON]);

    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
        if (paf->type < 0 || paf->type >= MAX_SKILL)
            continue;

        fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where, paf->level, paf->duration, paf->modifier,paf->location,
            paf->bitvector);
    }

    fprintf(fp,"End\n");
    return;
}

/* write a mount */
void fwrite_mount( CHAR_DATA *pet, FILE *fp)
{
    AFFECT_DATA *paf;

    fprintf(fp,"#MOUNT\n");

    fprintf(fp,"Vnum %d\n",pet->pIndexData->vnum);

    fprintf(fp,"Name %s~\n", pet->name);
    fprintf(fp,"LogO %ld\n", current_time);
    if (pet->short_descr != pet->pIndexData->short_descr)
        fprintf(fp,"ShD  %s~\n", pet->short_descr);
    if (pet->long_descr != pet->pIndexData->long_descr)
        fprintf(fp,"LnD  %s~\n", pet->long_descr);
    if (pet->description != pet->pIndexData->description)
        fprintf(fp,"Desc %s~\n", pet->description);
    if (pet->race != pet->pIndexData->race)
        fprintf(fp,"Race %s~\n", race_table[pet->race].name);
    if (pet->clan)
        fprintf( fp, "Clan %s~\n",clan_table[pet->clan].name);
    fprintf(fp,"Sex  %d\n", pet->sex);
    if (pet->level != pet->pIndexData->level)
    fprintf(fp,"Levl %d\n", pet->level);
    fprintf(fp, "HMV  %d %d %d %d %d %d\n",
        pet->hit, pet->max_hit, pet->mana, pet->max_mana, pet->move, pet->max_move);
    if (pet->gold > 0)
        fprintf(fp,"Gold %ld\n",pet->gold);
    if (pet->silver > 0)
        fprintf(fp,"Silv %ld\n",pet->silver);
    if (pet->exp > 0)
        fprintf(fp, "Exp  %d\n", pet->exp);
    if (pet->act != pet->pIndexData->act)
        fprintf(fp, "Act  %s\n", print_flags(pet->act));
    if (pet->affected_by != pet->pIndexData->affected_by)
        fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
    if (pet->comm != 0)
        fprintf(fp, "Comm %s\n", print_flags(pet->comm));
    fprintf(fp,"Pos  %d\n", pet->position = POS_FIGHTING ? POS_STANDING : pet->position);
    if (pet->saving_throw != 0)
        fprintf(fp, "Save %d\n", pet->saving_throw);
    if (pet->alignment != pet->pIndexData->alignment)
        fprintf(fp, "Alig %d\n", pet->alignment);
    if (pet->hitroll != pet->pIndexData->hitroll)
        fprintf(fp, "Hit  %d\n", pet->hitroll);
    if (pet->damroll != pet->pIndexData->damage[DICE_BONUS])
        fprintf(fp, "Dam  %d\n", pet->damroll);
    fprintf(fp, "ACs  %d %d %d %d\n",
        pet->armor[0],pet->armor[1],pet->armor[2],pet->armor[3]);
    fprintf(fp, "Attr %d %d %d %d %d\n",
        pet->perm_stat[STAT_STR], pet->perm_stat[STAT_INT],
        pet->perm_stat[STAT_WIS], pet->perm_stat[STAT_DEX],
        pet->perm_stat[STAT_CON]);
    fprintf(fp, "AMod %d %d %d %d %d\n",
        pet->mod_stat[STAT_STR], pet->mod_stat[STAT_INT],
        pet->mod_stat[STAT_WIS], pet->mod_stat[STAT_DEX],
        pet->mod_stat[STAT_CON]);

    for ( paf = pet->affected; paf != NULL; paf = paf->next )
    {
        if (paf->type < 0 || paf->type >= MAX_SKILL)
            continue;

        fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where, paf->level, paf->duration, paf->modifier,paf->location,
            paf->bitvector);
    }

    fprintf(fp,"End\n");
    return;
}





/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    EXTRA_DESCR_DATA *ed;
    AFFECT_DATA *paf;

    /*
     * Slick recursion to write lists backwards,
     *   so loading them will load in forwards order.
     */
    if ( obj->next_content != NULL )
    fwrite_obj( ch, obj->next_content, fp, iNest );

    /*
     * Castrate storage characters.
     */
/* Cut out as part of limited repop no restriction code. Test stuff really.
    if ( (ch->level < obj->level - 50 && obj->item_type != ITEM_CONTAINER)
    ||   obj->item_type == ITEM_KEY
    ||   (obj->item_type == ITEM_MAP && !obj->value[0]))
*/
    if (obj->item_type == ITEM_KEY || (obj->item_type == ITEM_MAP && !obj->value[0]))
    return;

    fprintf( fp, "#O\n" );
    fprintf( fp, "Vnum %d\n",   obj->pIndexData->vnum        );
    if (!obj->pIndexData->new_format)
    fprintf( fp, "Oldstyle\n");
    if (obj->enchanted)
    fprintf( fp,"Enchanted\n");
    fprintf( fp, "Nest %d\n",   iNest            );

    /* these data are only used if they do not match the defaults */

    if ( obj->name != obj->pIndexData->name)
        fprintf( fp, "Name %s~\n",  obj->name            );
    if ( obj->short_descr != obj->pIndexData->short_descr)
        fprintf( fp, "ShD  %s~\n",  obj->short_descr         );
    if ( obj->description != obj->pIndexData->description)
        fprintf( fp, "Desc %s~\n",  obj->description         );
    if ( obj->extra_flags != obj->pIndexData->extra_flags)
        fprintf( fp, "ExtF %d\n",   obj->extra_flags         );
    if ( obj->wear_flags != obj->pIndexData->wear_flags)
        fprintf( fp, "WeaF %d\n",   obj->wear_flags          );
    if ( obj->item_type != obj->pIndexData->item_type)
        fprintf( fp, "Ityp %d\n",   obj->item_type           );
    if ( obj->weight != obj->pIndexData->weight)
        fprintf( fp, "Wt   %d\n",   obj->weight          );
    if ( obj->condition != obj->pIndexData->condition)
    fprintf( fp, "Cond %d\n",   obj->condition           );

    /* variable data */

    fprintf( fp, "Wear %d\n",   obj->wear_loc                );
    if (obj->level != obj->pIndexData->level)
        fprintf( fp, "Lev  %d\n",   obj->level           );
    if (obj->timer != 0)
        fprintf( fp, "Time %d\n",   obj->timer       );
    fprintf( fp, "Cost %d\n",   obj->cost            );
    if (obj->value[0] != obj->pIndexData->value[0]
    ||  obj->value[1] != obj->pIndexData->value[1]
    ||  obj->value[2] != obj->pIndexData->value[2]
    ||  obj->value[3] != obj->pIndexData->value[3]
    ||  obj->value[4] != obj->pIndexData->value[4])
        fprintf( fp, "Val  %d %d %d %d %d\n",
        obj->value[0], obj->value[1], obj->value[2], obj->value[3],
        obj->value[4]        );

    switch ( obj->item_type )
    {
    case ITEM_POTION:
    case ITEM_SCROLL:
    case ITEM_PILL:
    if ( obj->value[1] > 0 )
    {
        fprintf( fp, "Spell 1 '%s'\n",
        skill_table[obj->value[1]].name );
    }

    if ( obj->value[2] > 0 )
    {
        fprintf( fp, "Spell 2 '%s'\n",
        skill_table[obj->value[2]].name );
    }

    if ( obj->value[3] > 0 )
    {
        fprintf( fp, "Spell 3 '%s'\n",
        skill_table[obj->value[3]].name );
    }

    break;

    case ITEM_STAFF:
    case ITEM_WAND:
    if ( obj->value[3] > 0 )
    {
        fprintf( fp, "Spell 3 '%s'\n",
        skill_table[obj->value[3]].name );
    }

    break;
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
    if (paf->type < 0 || paf->type >= MAX_SKILL || paf->duration < 0)
        continue;
        fprintf( fp, "Affc '%s' %3d %3d %3d %3d %3d %10d\n",
            skill_table[paf->type].name,
            paf->where,
            paf->level,
            paf->duration,
            paf->modifier,
            paf->location,
            paf->bitvector
            );
    }

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
    fprintf( fp, "ExDe %s~ %s~\n",
        ed->keyword, ed->description );
    }

    fprintf( fp, "End\n\n" );

    if ( obj->contains != NULL )
    fwrite_obj( ch, obj->contains, fp, iNest + 1 );

    return;
}



/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj( DESCRIPTOR_DATA *d, char *name )
{
    char strsave[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *ch;
    FILE *fp;
    bool found;
    int stat;

    ch = new_char();
    ch->pcdata = new_pcdata();

    d->character            = ch;
    ch->pcdata->character   = ch;
    ch->desc                = d;
    ch->name                = str_dup( name );
    ch->id              = get_pc_id();
    ch->race                = race_lookup("human");
    ch->act             = 0;
    ch->act2            = 0;
    ch->comm                = COMM_COMBINE
                    | COMM_PROMPT;
    ch->prompt              = str_dup( "{c<%hhp %mm %vmv %wwillpower %Xtnl> [Exits: %e]{x " );
    ch->pcdata->confirm_delete      = FALSE;
    ch->pcdata->pwd         = str_dup( "" );
    ch->pcdata->bamfin          = str_dup( "" );
    ch->pcdata->bamfout         = str_dup( "" );
    ch->pcdata->title           = str_dup( "" );
    ch->pcdata->wiziname        = str_dup( "" );
    ch->pcdata->immtitle        = str_dup( "" );
    ch->pcdata->pretitle        = str_dup( "" );
    ch->pcdata->last_rpnote            = current_time;
    ch->pcdata->hometown 		= 0;
    for (stat =0; stat < MAX_STATS; stat++)
    ch->perm_stat[stat]     = 13;
    ch->pcdata->condition[COND_THIRST]  = 48;
    ch->pcdata->condition[COND_FULL]    = 48;
    ch->pcdata->condition[COND_HUNGER]  = 48;
    ch->pcdata->last_pose = 5;
    ch->pcdata->room_last_pose = 5;
    ch->pcdata->security        = 0;    /* OLC */
    ch->pcdata->immclass = 0;
    ch->pcdata->sect        = 0;
    ch->clan = 0;
    ch->roleplaying = 0;
    ch->bloodtick = 0;
    ch->airtick = 0;
    ch->pcdata->influenced = 0;
    ch->pcdata->csgeneration = 0;
    ch->max_quintessence   = 100;
    for (stat = 0; stat < MAX_GIFT; stat ++)
        ch->pcdata->gift[stat] = FALSE;
    for (stat = 0; stat < MAX_CSBACK; stat ++)
     ch->pcdata->csbackgrounds[stat] = 0;

    found = FALSE;
    fclose( fpReserve );

    #if defined(unix)
    /* decompress if .gz file exists */
    sprintf( strsave, "%s%s%s", PLAYER_DIR, capitalize(name),".gz");
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
    fclose(fp);
    sprintf(buf,"gzip -dfq %s",strsave);
    system(buf);
    }
    #endif

    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( name ) );
    /* sprintf( strsave, "%s%s", BK_PLAYER_DIR, capitalize( name ) ); */
    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
    int iNest;

    for ( iNest = 0; iNest < MAX_NEST; iNest++ )
        rgObjNest[iNest] = NULL;

    found = TRUE;
    for ( ; ; )
    {
        char letter;
        char *word;

        letter = fread_letter( fp );
        if ( letter == '*' )
        {
        fread_to_eol( fp );
        continue;
        }

        if ( letter != '#' )
        {
        bug( "Load_char_obj: # not found.", 0 );
        break;
        }

        word = fread_word( fp );
        if      ( !str_cmp( word, "PLAYER" ) ) fread_char ( ch, fp );
        else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
        else if ( !str_cmp( word, "O"      ) ) fread_obj  ( ch, fp );
        else if ( !str_cmp( word, "PET"    ) ) fread_pet  ( ch, fp );
        else if ( !str_cmp( word, "MOUNT"  ) ) fread_mount( ch, fp );
        else if ( !str_cmp( word, "END"    ) ) break;
        else
        {
        bug( "Load_char_obj: bad section.", 0 );
        break;
        }
    }
    fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );


    /* initialize race */
    if (found)
    {
    int i;

    if (ch->race == 0)
        ch->race = race_lookup("human");

    ch->dam_type = 17; /*punch */

    ch->affected_by = ch->affected_by|race_table[ch->race].aff;
    ch->imm_flags   = ch->imm_flags | race_table[ch->race].imm;
    ch->res_flags   = ch->res_flags | race_table[ch->race].res;
    ch->vuln_flags  = ch->vuln_flags | race_table[ch->race].vuln;
    ch->form    = race_table[ch->race].form;
    ch->parts   = race_table[ch->race].parts;
    }

    return found;
}



/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )                    \
                if ( !str_cmp( word, literal ) )    \
                {                   \
                    field  = value;         \
                    fMatch = TRUE;          \
                    break;              \
                }

/* provided to free strings */
#if defined(KEYS)
#undef KEYS
#endif

#define KEYS( literal, field, value )                   \
                if ( !str_cmp( word, literal ) )    \
                {                   \
                    free_string(field);         \
                    field  = value;         \
                    fMatch = TRUE;          \
                    break;              \
                }

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int count = 0;
    int lastlogoff = current_time;
    int percent;

    sprintf(buf,"Loading %s.",ch->name);
    log_string(buf);

    for ( ; ; )
    {
    word   = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

    case 'A':
        KEY( "Act",     ch->act,        fread_flag( fp ) );
        KEY( "Act2", ch->act2, fread_flag( fp ) );
        KEY( "AffectedBy",  ch->affected_by,    fread_flag( fp ) );
        KEY( "Affected2By", ch->affected2_by,   fread_flag( fp ) );
        KEY( "AfBy",    ch->affected_by,    fread_flag( fp ) );
        KEY( "Af2By",   ch->affected2_by,   fread_flag( fp ) );
        KEY( "Admin",      ch->pcdata->admin,         fread_number( fp ) );
        KEY( "Agg", ch->agg,    fread_number( fp ) );
        KEY( "Agg_dam", ch->agg_dam,    fread_number( fp ) );
        KEY( "Alignment",   ch->alignment,      fread_number( fp ) );
        KEY( "Alig",    ch->alignment,      fread_number( fp ) );
        KEY( "Animalism",   ch->animalism,      fread_number( fp ) );
        KEY( "Arete",   ch->arete,      fread_number( fp ) );
        KEY( "Auspex",  ch->auspex,     fread_number( fp ) );
        KEY( "Auspice", ch->auspice,        fread_number( fp ) );
        KEY( "Apprentice",  ch->apprentice,         fread_string( fp ) );
        if (!str_cmp(word, "Aget"))
        {
            // silently ignore.
            fread_number(fp);
            fMatch = TRUE;
        }
        if (!str_cmp( word, "Alia"))
        {
        if (count >= MAX_ALIAS)
        {
            fread_to_eol(fp);
            fMatch = TRUE;
            break;
        }

        ch->pcdata->alias[count]    = str_dup(fread_word(fp));
        ch->pcdata->alias_sub[count]    = str_dup(fread_word(fp));
        count++;
        fMatch = TRUE;
        break;
        }

            if (!str_cmp( word, "Alias"))
            {
                if (count >= MAX_ALIAS)
                {
                    fread_to_eol(fp);
                    fMatch = TRUE;
                    break;
                }

                ch->pcdata->alias[count]        = str_dup(fread_word(fp));
                ch->pcdata->alias_sub[count]    = fread_string(fp);
                count++;
                fMatch = TRUE;
                break;
            }

        if (!str_cmp( word, "AC") || !str_cmp(word,"Armor"))
        {
        fread_to_eol(fp);
        fMatch = TRUE;
        break;
        }

        if (!str_cmp(word,"ACs"))
        {
        int i;

        for (i = 0; i < 4; i++)
            ch->armor[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

        if (!str_cmp(word, "AffD"))
        {
        AFFECT_DATA *paf;
        int sn;

        paf = new_affect();

        sn = skill_lookup(fread_word(fp));
        if (sn < 0)
            bug("Fread_char: unknown skill.",0);
        else
            paf->type = sn;

        paf->level  = fread_number( fp );
        paf->duration   = fread_number( fp );
        paf->modifier   = fread_number( fp );
        paf->location   = fread_number( fp );
        paf->bitvector  = fread_number( fp );
        paf->next   = ch->affected;
        ch->affected    = paf;
        fMatch = TRUE;
        break;
        }

            if (!str_cmp(word, "Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                    paf->type = sn;

                paf->where  = fread_number(fp);
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                paf->next       = ch->affected;
                ch->affected    = paf;
                fMatch = TRUE;
                break;
            }

        if ( !str_cmp( word, "AttrMod"  ) || !str_cmp(word,"AMod"))
        {
        int stat;
        for (stat = 0; stat < MAX_STATS; stat ++)
           ch->mod_stat[stat] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

        if ( !str_cmp( word, "AttrPerm" ) || !str_cmp(word,"Attr"))
        {
        int stat;

        for (stat = 0; stat < MAX_STATS; stat++)
            ch->perm_stat[stat] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        KEY( "Avatar",  ch->avatar,     fread_number( fp ) );
        break;

    case 'B':
        KEY( "Bamfin",  ch->pcdata->bamfin, fread_string( fp ) );
        KEY( "Bamfout", ch->pcdata->bamfout,    fread_string( fp ) );
        KEY( "Bank",        ch->pcdata->bank,       fread_number( fp ) );
        KEY( "Bin",     ch->pcdata->bamfin, fread_string( fp ) );
        KEY( "Bonded",  ch->bonded,     fread_number( fp ) );
        KEY( "Bout",    ch->pcdata->bamfout,    fread_string( fp ) );
        KEY( "Breed",   ch->breed,  fread_number( fp ) );
        break;

    case 'C':
        KEY( "Changed", ch->changed,        fread_number( fp ) );
        KEY( "Childer", ch->childer,        fread_number( fp ) );
        KEY( "Class",   ch->class,      fread_number( fp ) );
        KEY( "Cla",     ch->class,      fread_number( fp ) );
        KEY( "Chimerstry", ch->chimerstry, fread_number (fp ) );
        KEY( "currentkills",   ch->currentkills,   fread_number ( fp ) );
        KEY( "cslock", ch->cslock,      fread_number( fp ) );
        KEY( "csdemeanor",  ch->pcdata->csdemeanor, fread_string( fp ) );
        KEY( "csnature",    ch->pcdata->csnature,   fread_string( fp ) );
        KEY( "cheater", ch->cheater, fread_number (fp) );

                if ( !str_cmp( word, "Clan" ) ) {
                    char *tmp = fread_string(fp);
                    ch->clan = clan_lookup(tmp);
                    free_string(tmp);
                    fMatch = TRUE;
                    break;
                }
                // clan below was replaced with above because mem leak
       // KEY( "Clan",    ch->clan,   clan_lookup(fread_string(fp)));

        if ( !str_cmp( word, "Condition" ) || !str_cmp(word,"Cond"))
        {
        ch->pcdata->condition[0] = fread_number( fp );
        ch->pcdata->condition[1] = fread_number( fp );
        ch->pcdata->condition[2] = fread_number( fp );
        fMatch = TRUE;
        break;
        }
            if (!str_cmp(word,"Cnd"))
            {
                ch->pcdata->condition[0] = fread_number( fp );
                ch->pcdata->condition[1] = fread_number( fp );
                ch->pcdata->condition[2] = fread_number( fp );
        ch->pcdata->condition[3] = fread_number( fp );
                fMatch = TRUE;
                break;
            }
        KEY( "Celerity",    ch->celerity,       fread_number( fp ) );
        KEY("Comm",     ch->comm,       fread_flag( fp ) );

        if( !str_cmp( word, "Create"))
        {
            ch->pcdata->stat[PHYSICAL] = fread_number(fp);
            ch->pcdata->stat[SOCIAL] = fread_number(fp);
            ch->pcdata->stat[MENTAL] = fread_number(fp);
            ch->pcdata->stat[SKILLS] = 0;
            ch->pcdata->stat[TALENTS] = 0;
            ch->pcdata->stat[KNOWLEDGES] = 0;
            fMatch = TRUE;
            break;
        }

        if( !str_cmp( word, "csCreate"))
        {
            ch->pcdata->stat[PHYSICAL] = fread_number(fp);
            ch->pcdata->stat[SOCIAL] = fread_number(fp);
            ch->pcdata->stat[MENTAL] = fread_number(fp);
            ch->pcdata->stat[SKILLS] = fread_number(fp);
            ch->pcdata->stat[TALENTS] = fread_number(fp);
            ch->pcdata->stat[KNOWLEDGES] = fread_number(fp);
            fMatch = TRUE;
            break;
        }


//        KEY( "CSname",  ch->pcdata->csname, fread_string( fp ) );
//        KEY( "CSchronicle", ch->pcdata->cschronicle,    fread_string( fp ) );
//        KEY( "CSclan",  ch->pcdata->csclan, fread_string( fp ) );
//       KEY( "CSnature",    ch->pcdata->csnature,   fread_string( fp ) );
//        KEY( "CSdemeanor",  ch->pcdata->csdemeanor, fread_string( fp ) );
//        KEY( "CSconcept",   ch->pcdata->csconcept,  fread_string( fp ) );
//        KEY( "CSsect",  ch->pcdata->cssect, fread_string( fp ) );
//        KEY( "CSsire",  ch->pcdata->cssire, fread_string( fp ) );
        KEY( "CSgeneration",    ch->pcdata->csgeneration,       fread_number( fp ) );
//        KEY( "CShaven", ch->pcdata->cshaven,    fread_string( fp ) );
      if ( !str_cmp( word, "CSattributes"  ))
        {
        int i;
        for (i = 0; i <= MAX_CSATTRIBUTES; i ++)
           ch->csattributes[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

        if ( !str_cmp( word, "CSattrib_mod"  ))
        {
        int i;
        for (i = 0; i <= MAX_CSATTRIBUTES; i ++)
           ch->csattrib_mod[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

        if ( !str_cmp( word, "CSabilities"  ))
        {
        int i;
        for (i = 0; i <= MAX_CSABIL; i ++)
           ch->csabilities[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

        if ( !str_cmp( word, "CSsec_abil"  ))
        {
        int i;
        for (i = 0; i <= MAX_CSABIL; i ++)
           ch->pcdata->cssec_abil[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }

/*
        if ( !str_cmp( word, "CSdisciplines"  ))
        {
        int i;
        for (i = 1; i <= 20; i ++)
           ch->pcdata->csdisciplines[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
*/
        if ( !str_cmp( word, "CSbackgrounds"  ))
        {
        int i;
        for (i = 0; i < MAX_CSBACK; i ++)
           ch->pcdata->csbackgrounds[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }


        if ( !str_cmp( word, "CSvirtues"  ))
        {
            ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] = fread_number(fp);
            ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] = fread_number(fp);
            ch->pcdata->csvirtues[CSVIRT_COURAGE] = fread_number(fp);

        fMatch = TRUE;
        break;
        }

        KEY( "CShumanity",  ch->pcdata->cshumanity,     fread_number( fp ) );
        KEY( "CSmaxwillpower",  ch->csmax_willpower,        fread_number( fp ) );
        KEY( "CSwillpower", ch->cswillpower,        fread_number( fp ) );
/*
        KEY( "CSmaxbloodpool",  ch->pcdata->csmax_bloodpool,        fread_number( fp ) );
        KEY( "CSbloodpool", ch->pcdata->csbloodpool,        fread_number( fp ) );
        KEY( "CShealth",    ch->pcdata->cshealth,       fread_number( fp ) );
        KEY( "CSexp",   ch->pcdata->csexp,      fread_number( fp ) );
*/
        break;


    case 'D':
        KEY( "Damroll", ch->damroll,        fread_number( fp ) );
        KEY( "Dam",     ch->damroll,        fread_number( fp ) );
        KEY( "Description", ch->description,    fread_string( fp ) );
        KEY( "Desc",    ch->description,    fread_string( fp ) );

        if( !str_cmp( word, "Disciplines" ))
        {

            int i;
            for (i = 0;i < MAX_DISC;i++)
                ch->pcdata->discipline[i] = fread_number(fp);

            fMatch = TRUE;
            break;
        }

        KEY( "Domination",  ch->domination,     fread_number( fp ) );
        KEY( "Dpoints", ch->dpoints,        fread_number( fp ) );


        break;

    case 'E':
        KEY( "Embraced",    ch->embraced,       fread_number( fp ) );
        if ( !str_cmp( word, "End" ) )
        {
            /* adjust hp mana move up  -- here for speed's sake */
            percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

        percent = UMIN(percent,100);

            if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON))
            /*&&  !IS_AFFECTED(ch,AFF_PLAGUE))*/
            {
                ch->hit += (ch->max_hit - ch->hit) * percent / 100;
                ch->mana    += (ch->max_mana - ch->mana) * percent / 100;
                ch->move    += (ch->max_move - ch->move)* percent / 100;
            }
        return;
        }
        KEY( "Exp",     ch->exp,        fread_number( fp ) );
        KEY( "Email",       ch->pcdata->email,      fread_string( fp ) );
        break;

    case 'F':
        KEY( "freebie", ch->freebie,        fread_number( fp ) );
        KEY( "fur",     ch->pcdata->garou_fur,      fread_string( fp ) );
        KEY( "Fortitude",   ch->fortitude,      fread_number( fp ) );
        KEY( "Foci",    ch->foci,       fread_number( fp ) );
        break;

    case 'G':
        KEY( "gen", ch->gen,        fread_number( fp ) );
        if ( !str_cmp( word, "grenown"  ))
        {
            ch->pcdata->renown[GLORY] = fread_number(fp);
            ch->pcdata->renown[TEMP_GLORY] = fread_number(fp);
            ch->pcdata->renown[HONOR] = fread_number(fp);
            ch->pcdata->renown[TEMP_HONOR] = fread_number(fp);
            ch->pcdata->renown[WISDOM] = fread_number(fp);
            ch->pcdata->renown[TEMP_WISDOM] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        if ( !str_cmp( word, "grage"  ))
        {
            ch->pcdata->rage[PERM] = fread_number(fp);
            ch->pcdata->rage[TEMP] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        KEY( "grank",   ch->pcdata->rank,       fread_number( fp ) );
        if ( !str_cmp( word, "gnosis"  ))
        {
            ch->pcdata->gnosis[PERM] = fread_number(fp);
            ch->pcdata->gnosis[TEMP] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        KEY( "Gift1",   ch->gift1,      fread_string( fp ) );
        KEY( "Gift2",   ch->gift2,      fread_string( fp ) );
        KEY( "Gift3",   ch->gift3,      fread_string( fp ) );
        KEY( "Gift4",   ch->gift4,      fread_string( fp ) );
        KEY( "Gift5",   ch->gift5,      fread_string( fp ) );
        KEY( "Gold",    ch->gold,       fread_number( fp ) );

            if ( !str_cmp( word, "Group" )  || !str_cmp(word,"Gr"))
            {
                char *temp;
                // Quietly ignore groups. Just in case old pfiles show up.
                temp = fread_word( fp ) ;
                fMatch = TRUE;
            }
        break;

    case 'H':
        KEY( "Home",    ch->pcdata->home,   fread_number( fp ) );
	KEY( "Hometown",ch->pcdata->hometown, fread_number(fp) );
        KEY( "Hitroll", ch->hitroll,        fread_number( fp ) );
        KEY( "Hit",     ch->hitroll,        fread_number( fp ) );

        if ( !str_cmp( word, "HpManaMove" ) || !str_cmp(word,"HMV"))
        {
        ch->hit     = fread_number( fp );
        ch->max_hit = fread_number( fp );
        ch->mana    = fread_number( fp );
        ch->max_mana    = fread_number( fp );
        ch->move    = fread_number( fp );
        ch->max_move    = fread_number( fp );
        fMatch = TRUE;
        break;
        }

            if ( !str_cmp( word, "HpManaMovePerm" ) || !str_cmp(word,"HMVP"))
            {
                ch->pcdata->perm_hit    = fread_number( fp );
                ch->pcdata->perm_mana   = fread_number( fp );
                ch->pcdata->perm_move   = fread_number( fp );
                fMatch = TRUE;
                break;
            }

        if (!str_cmp(word, "Hostname"))
        {
          fread_to_eol(fp);
          fMatch = TRUE;
        }

        break;

    case 'I':
        KEY( "ICtotal",     ch->pcdata->IC_total,           fread_number( fp ) );
        KEY( "ICrank",      ch->pcdata->IC_rank,            fread_number( fp ) );
        KEY( "ICgoal",      ch->pcdata->IC_goal,            fread_number( fp ) );
        KEY( "Influ",       ch->pcdata->influenced,           fread_number( fp ) );
        KEY( "Id",      ch->id,         fread_number( fp ) );
        KEY( "Inhumanity",  ch->inhumanity,     fread_number( fp ) );
        KEY( "InvisLevel",  ch->invis_level,    fread_number( fp ) );
        KEY( "Inco",    ch->incog_level,    fread_number( fp ) );
        KEY( "Invi",    ch->invis_level,    fread_number( fp ) );
        KEYS( "Immtitle",   ch->pcdata->immtitle,    fread_string(fp) );
        KEY( "Immnote",    ch->pcdata->last_rpnote,  fread_number( fp ) );
        KEY( "Iclass", ch->pcdata->immclass, fread_number( fp) );
        KEY( "Immun",     ch->imm_flags,        fread_flag( fp ) );
        KEY( "Ip", ch->pcdata->ip,    fread_number( fp) );

        break;

    case 'K':
        KEY( "kill_target", ch->pcdata->kill_target,            fread_string( fp ) );
        break;

    case 'L':
        KEY( "LastLevel",   ch->pcdata->last_level, fread_number( fp ) );
        KEY( "LLev",    ch->pcdata->last_level, fread_number( fp ) );
        KEY( "Level",   ch->level,      fread_number( fp ) );
        KEY( "Lev",     ch->level,      fread_number( fp ) );
        KEY( "Levl",    ch->level,      fread_number( fp ) );
        KEY( "LogO",    lastlogoff,     fread_number( fp ) );
        KEY( "LongDescr",   ch->long_descr,     fread_string( fp ) );
        KEY( "LnD",     ch->long_descr,     fread_string( fp ) );
        break;

    case 'M':
        KEY( "max_pblood",       ch->max_pblood,      fread_number( fp ) );
        KEY( "Maxquint",       ch->max_quintessence, fread_number( fp ) );
        KEY( "Mortis",  ch->mortis,     fread_number( fp ) );
        KEY( "maxdamage",   ch->maxdamage,   fread_number ( fp ) );
        break;

    case 'N':
        KEYS( "Name",   ch->name,       fread_string( fp ) );
        KEY( "Note",    ch->pcdata->last_note,  fread_number( fp ) );
        if ( !str_cmp( word, "Newgifts"  ))
        {
        int i;
        for (i = 0; i < MAX_GIFT; i ++)
           ch->pcdata->gift[i] = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        if (!str_cmp(word,"Not"))
        {
        ch->pcdata->last_note           = fread_number(fp);
        ch->pcdata->last_idea           = fread_number(fp);
        ch->pcdata->last_penalty        = fread_number(fp);
        ch->pcdata->last_news           = fread_number(fp);
        ch->pcdata->last_changes        = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        break;

    case 'O':
        KEY( "Obfuscate",       ch->obfuscate,      fread_number( fp ) );
        KEY( "Obtenebration",       ch->obtenebration,      fread_number( fp ) );
        break;

    case 'P':
        KEY( "Paradox",       ch->paradox,      fread_number( fp ) );
        KEY( "Password",    ch->pcdata->pwd,    fread_string( fp ) );
        KEY( "Pass",    ch->pcdata->pwd,    fread_string( fp ) );
        KEY( "pblood",       ch->pblood,      fread_number( fp ) );
        KEY( "Played",  ch->played,     fread_number( fp ) );
        KEY( "Plyd",    ch->played,     fread_number( fp ) );
        KEY( "Points",  ch->pcdata->points, fread_number( fp ) );
        KEY( "Pnts",    ch->pcdata->points, fread_number( fp ) );
        KEY( "Position",    ch->position,       fread_number( fp ) );
        KEY( "Pos",     ch->position,       fread_number( fp ) );
        KEY( "Potence", ch->potence,        fread_number( fp ) );
        KEY( "Practice",    ch->practice,       fread_number( fp ) );
        KEY( "Prac",    ch->practice,       fread_number( fp ) );
        KEY( "Presence",    ch->presence,       fread_number( fp ) );
        KEYS( "Pretitle",   ch->pcdata->pretitle,    fread_string(fp) );
          KEYS( "Prompt",      ch->prompt,             fread_string( fp ) );
        KEY( "Prom",    ch->prompt,     fread_string( fp ) );
        KEY( "Protean", ch->protean,        fread_number( fp ) );
        KEY( "primary", ch->pcdata->primary,        fread_number( fp ) );
        KEY( "primal",  ch->pcdata->primal_urge,    fread_number( fp ) );
        KEY( "progress",    ch->pcdata->progress,   fread_number( fp ) );
        KEY( "Plrnote",    ch->pcdata->playernotes,    fread_string( fp ) );

        break;

    case 'Q':
        KEY( "qpoints", ch->qpoints,        fread_number( fp ) );
        KEY( "qpointstot", ch->totalqpoints,        fread_number( fp ) );
        KEY( "Quietus", ch->quietus,        fread_number( fp ) );
        KEY( "QuestNext",   ch->nextquest,          fread_number( fp ) );
        KEY( "Quintessence",    ch->quintessence,       fread_number( fp ) );
        break;

    case 'R':
        KEY( "Registered",      ch->pcdata->registered,         fread_number( fp ) );
        KEY( "Race",        ch->race,
                race_lookup(fread_string( fp )) );
        KEY( "Rage",    ch->rage,       fread_number( fp ) );
        KEY( "Rank",    ch->rank,       fread_number( fp ) );
        KEY( "Remorts", ch->remorts,        fread_number( fp ) );
        KEY( "Renown",  ch->renown, fread_number( fp ) );
                KEY( "Resis",     ch->res_flags,        fread_flag( fp ) );

        if ( !str_cmp( word, "Room" ) )
        {
        ch->in_room = get_room_index( fread_number( fp ) );
        if ( ch->in_room == NULL )
            ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
        fMatch = TRUE;
        break;
        }

        break;

    case 'S':
        KEY( "SavingThrow", ch->saving_throw,   fread_number( fp ) );
        KEY( "Save",    ch->saving_throw,   fread_number( fp ) );
        KEY( "Scro",    ch->lines,      fread_number( fp ) );
        KEY( "Sect",        ch->pcdata->sect,       fread_number( fp ) );
        KEY( "Sex",     ch->sex,        fread_number( fp ) );
        KEY( "shiftform",       ch->pcdata->shiftform,      fread_number( fp ) );
        KEY( "ShortDescr",  ch->short_descr,    fread_string( fp ) );
        KEY( "ShD",     ch->short_descr,    fread_string( fp ) );
        KEY( "Shift",   ch->shift,          fread_string( fp ) );
        KEY( "Sec",         ch->pcdata->security,   fread_number( fp ) );   /* OLC */
        KEY( "secondary",         ch->pcdata->secondary,    fread_number( fp ) );
        KEY( "Silv",        ch->silver,             fread_number( fp ) );
        KEY( "Sire",    ch->sire,           fread_string( fp ) );


        if ( !str_cmp( word, "Skill" ) || !str_cmp(word,"Sk"))
        {
        int sn;
        int value;
        char *temp;

        value = fread_number( fp );
        temp = fread_word( fp ) ;
        sn = skill_lookup(temp);
        if (sn == skill_lookup("knight's training"))
            fread_to_eol( fp );
        /* sn    = skill_lookup( fread_word( fp ) ); */
        if ( sn < 0 )
        {
            fprintf(stderr,"%s",temp);
            bug( "Fread_char: unknown skill. ", 0 );
        }
        else
            ch->pcdata->learned[sn] = value;
        fMatch = TRUE;
        }
        if (!str_cmp(word, "Spheres"))
        {
        ch->sphere[1]   = fread_number( fp );
        ch->sphere[2]   = fread_number( fp );
        ch->sphere[3]   = fread_number( fp );
        ch->sphere[4]   = fread_number( fp );
        ch->sphere[5]   = fread_number( fp );
        ch->sphere[6]   = fread_number( fp );
        ch->sphere[7]   = fread_number( fp );
        ch->sphere[8]   = fread_number( fp );
        ch->sphere[9]   = fread_number( fp );
        fMatch = TRUE;
        break;
        }

        break;

    case 'T':
        if ( !str_cmp( word, "TBA"  ))
        {
            ch->pcdata->tribe = fread_number(fp);
            ch->pcdata->breed = fread_number(fp);
            ch->pcdata->auspice = fread_number(fp);
        fMatch = TRUE;
        break;
        }
        KEY( "Thaumaturgy", ch->thaumaturgy,        fread_number( fp ) );
        KEY( "tertiary",    ch->pcdata->tertiary,       fread_number( fp ) );
        KEY( "Tradition",   ch->tradition,      fread_number( fp ) );

            KEY( "TrueSex",     ch->pcdata->true_sex,   fread_number( fp ) );
        KEY( "TSex",    ch->pcdata->true_sex,   fread_number( fp ) );
        KEY( "Trai",    ch->train,      fread_number( fp ) );
        KEY( "Trust",   ch->trust,      fread_number( fp ) );
        KEY( "Tru",     ch->trust,      fread_number( fp ) );
        KEY( "totalkills",   ch->totalkills,   fread_number ( fp ) );
        KEY( "tagged", ch->tagged, fread_number ( fp ) );

        if ( !str_cmp( word, "Title" )  || !str_cmp( word, "Titl"))
        {
        ch->pcdata->title = fread_string( fp );
            if (ch->pcdata->title[0] != '.' && ch->pcdata->title[0] != ','
        &&  ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
        {
            sprintf( buf, " %s", ch->pcdata->title );
            free_string( ch->pcdata->title );
            ch->pcdata->title = str_dup( buf );
        }
        fMatch = TRUE;
        break;
        }

        break;

    case 'V':
        KEY( "Vamp_Master", ch->vamp_master,            fread_string( fp ) );
        KEY( "Version",     ch->version,        fread_number ( fp ) );
        KEY( "Vers",    ch->version,        fread_number ( fp ) );
        KEY( "Vicissitude", ch->vicissitude,        fread_number( fp ) );
                KEY( "Vulne",     ch->vuln_flags,        fread_flag( fp ) );
        if ( !str_cmp( word, "Vnum" ) )
        {
        ch->pIndexData = get_mob_index( fread_number( fp ) );
        fMatch = TRUE;
        break;
        }
        break;

    case 'W':
        KEY( "Wimpy",   ch->wimpy,      fread_number( fp ) );
        KEY( "Wimp",    ch->wimpy,      fread_number( fp ) );
        KEY( "Wizn",    ch->wiznet,     fread_flag( fp ) );
        KEYS( "Wizin",   ch->pcdata->wiziname,    fread_string(fp) );
        break;

    case 'U':
            KEY( "Unlockp",   ch->unlocksspent,   fread_number ( fp ) );
            break;

    }

    if ( !fMatch )
    {
        sprintf(buf, "Fread_char: no match for '%s'.", word);
        bug( buf, 0 );
        fread_to_eol( fp );
    }
    }
}

/* load a pet from the forgotten reaches */
void fread_pet( CHAR_DATA *ch, FILE *fp )
{
    char *word;
    CHAR_DATA *pet;
    bool fMatch;
    int lastlogoff = current_time;
    int percent;

    /* first entry had BETTER be the vnum or we barf */
    word = feof(fp) ? "END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {
        int vnum;

        vnum = fread_number(fp);
        if (get_mob_index(vnum) == NULL)
    {
            bug("Fread_pet: bad vnum %d.",vnum);
        pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
    }
        else
            pet = create_mobile(get_mob_index(vnum));
    }
    else
    {
        bug("Fread_pet: no vnum in file.",0);
        pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
    }

    for ( ; ; )
    {
        word    = feof(fp) ? "END" : fread_word(fp);
        fMatch = FALSE;

        switch (UPPER(word[0]))
        {
        case '*':
            fMatch = TRUE;
            fread_to_eol(fp);
            break;

        case 'A':
            KEY( "Act",     pet->act,       fread_flag(fp));
            KEY( "AfBy",    pet->affected_by,   fread_flag(fp));
            KEY( "Alig",    pet->alignment,     fread_number(fp));

            if (!str_cmp(word,"ACs"))
            {
                int i;

                for (i = 0; i < 4; i++)
                    pet->armor[i] = fread_number(fp);
                fMatch = TRUE;
                break;
            }

            if (!str_cmp(word,"AffD"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;

                paf->level  = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next   = pet->affected;
                pet->affected   = paf;
                fMatch      = TRUE;
                break;
            }

            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;

        paf->where  = fread_number(fp);
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
            }

            if (!str_cmp(word,"AMod"))
            {
                int stat;

                for (stat = 0; stat < MAX_STATS; stat++)
                    pet->mod_stat[stat] = fread_number(fp);
                fMatch = TRUE;
                break;
            }

            if (!str_cmp(word,"Attr"))
            {
                 int stat;

                 for (stat = 0; stat < MAX_STATS; stat++)
                     pet->perm_stat[stat] = fread_number(fp);
                 fMatch = TRUE;
                 break;
            }
            break;

         case 'C':
             KEY( "Clan",       pet->clan,       clan_lookup(fread_string(fp)));
             KEY( "Comm",   pet->comm,      fread_flag(fp));
             break;

         case 'D':
             KEY( "Dam",    pet->damroll,       fread_number(fp));
             KEY( "Desc",   pet->description,   fread_string(fp));
             break;

         case 'E':
             if (!str_cmp(word,"End"))
         {
        pet->leader = ch;
        pet->master = ch;
        ch->pet = pet;
            /* adjust hp mana move up  -- here for speed's sake */
            percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

            if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON))
/*          &&  !IS_AFFECTED(ch,AFF_PLAGUE))*/
            {
            percent = UMIN(percent,100);
                pet->hit    += (pet->max_hit - pet->hit) * percent / 100;
                pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
                pet->move   += (pet->max_move - pet->move)* percent / 100;
            }
                return;
         }
             KEY( "Exp",    pet->exp,       fread_number(fp));
             break;

         case 'G':
             KEY( "Gold",   pet->gold,      fread_number(fp));
             break;

         case 'H':
             KEY( "Hit",    pet->hitroll,       fread_number(fp));

             if (!str_cmp(word,"HMV"))
             {
                pet->hit    = fread_number(fp);
                pet->max_hit    = fread_number(fp);
                pet->mana   = fread_number(fp);
                pet->max_mana   = fread_number(fp);
                pet->move   = fread_number(fp);
                pet->max_move   = fread_number(fp);
                fMatch = TRUE;
                break;
             }
             break;

        case 'L':
             KEY( "Levl",   pet->level,     fread_number(fp));
             KEY( "LnD",    pet->long_descr,    fread_string(fp));
         KEY( "LogO",   lastlogoff,     fread_number(fp));
             break;

        case 'N':
             KEY( "Name",   pet->name,      fread_string(fp));
             break;

        case 'P':
             KEY( "Pos",    pet->position,      fread_number(fp));
             break;

    case 'R':
            KEY( "Race",    pet->race, race_lookup(fread_string(fp)));
            break;

        case 'S' :
            KEY( "Save",    pet->saving_throw,  fread_number(fp));
            KEY( "Sex",     pet->sex,       fread_number(fp));
            KEY( "ShD",     pet->short_descr,   fread_string(fp));
            KEY( "Silv",        pet->silver,            fread_number( fp ) );
            break;

        if ( !fMatch )
        {
            bug("Fread_pet: no match.",0);
            fread_to_eol(fp);
        }

        }
    }
}

/* load a mount from the forgotten reaches */
void fread_mount( CHAR_DATA *ch, FILE *fp )
{
    char *word;
    CHAR_DATA *pet;
    bool fMatch;
    int lastlogoff = current_time;
    int percent;

    /* first entry had BETTER be the vnum or we barf */
    word = feof(fp) ? "END" : fread_word(fp);
    if (!str_cmp(word,"Vnum"))
    {
        int vnum;

        vnum = fread_number(fp);
        if (get_mob_index(vnum) == NULL)
    {
            bug("Fread_mount: bad vnum %d.",vnum);
        pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
    }
        else
            pet = create_mobile(get_mob_index(vnum));
    }
    else
    {
        bug("Fread_mount: no vnum in file.",0);
        pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
    }

    for ( ; ; )
    {
        word    = feof(fp) ? "END" : fread_word(fp);
        fMatch = FALSE;

        switch (UPPER(word[0]))
        {
        case '*':
            fMatch = TRUE;
            fread_to_eol(fp);
            break;

        case 'A':
            KEY( "Act",         pet->act,               fread_flag(fp));
            KEY( "AfBy",        pet->affected_by,       fread_flag(fp));
            KEY( "Alig",    pet->alignment,     fread_number(fp));

            if (!str_cmp(word,"ACs"))
            {
                int i;

                for (i = 0; i < 4; i++)
                    pet->armor[i] = fread_number(fp);
                fMatch = TRUE;
                break;
            }

            if (!str_cmp(word,"AffD"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;

                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
       }

            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_char: unknown skill.",0);
                else
                   paf->type = sn;

                paf->where      = fread_number(fp);
                paf->level      = fread_number(fp);
                paf->duration   = fread_number(fp);
                paf->modifier   = fread_number(fp);
                paf->location   = fread_number(fp);
                paf->bitvector  = fread_number(fp);
                paf->next       = pet->affected;
                pet->affected   = paf;
                fMatch          = TRUE;
                break;
        }

            if (!str_cmp(word,"AMod"))
            {
                int stat;

                for (stat = 0; stat < MAX_STATS; stat++)
                    pet->mod_stat[stat] = fread_number(fp);
                fMatch = TRUE;
                break;
            }

            if (!str_cmp(word,"Attr"))
            {
                 int stat;

                 for (stat = 0; stat < MAX_STATS; stat++)
                     pet->perm_stat[stat] = fread_number(fp);
                 fMatch = TRUE;
                 break;
            }
            break;

         case 'C':
             KEY( "Clan",       pet->clan,       clan_lookup(fread_string(fp)));
         KEY( "Comm",   pet->comm,      fread_number(fp));
             break;

         case 'D':
             KEY( "Dam",    pet->damroll,       fread_number(fp));
             KEY( "Desc",   pet->description,   fread_string(fp));
             break;

         case 'E':
             if (!str_cmp(word,"End"))
         {
                ch->mount = pet;
                pet->mount = ch;
                /* adjust hp mana move up  -- here for speed's sake */
                percent = (current_time - lastlogoff) * 25 / ( 2 * 60 * 60);

                if (percent > 0 && !IS_AFFECTED(ch,AFF_POISON)
                &&  !IS_AFFECTED(ch,AFF_PLAGUE))
                {
                    percent = UMIN(percent,100);
                    pet->hit    += (pet->max_hit - pet->hit) * percent / 100;
                    pet->mana   += (pet->max_mana - pet->mana) * percent / 100;
                    pet->move   += (pet->max_move - pet->move)* percent / 100;
                }

                return;
         }
             KEY( "Exp",    pet->exp,       fread_number(fp));
             break;

         case 'G':
             KEY( "Gold",   pet->gold,      fread_number(fp));
             break;

         case 'H':
             KEY( "Hit",    pet->hitroll,       fread_number(fp));

             if (!str_cmp(word,"HMV"))
             {
                pet->hit    = fread_number(fp);
                pet->max_hit    = fread_number(fp);
                pet->mana   = fread_number(fp);
                pet->max_mana   = fread_number(fp);
                pet->move   = fread_number(fp);
                pet->max_move   = fread_number(fp);
                fMatch = TRUE;
                break;
             }
             break;

        case 'L':
             KEY( "Levl",   pet->level,     fread_number(fp));
             KEY( "LnD",    pet->long_descr,    fread_string(fp));
             KEY( "LogO",       lastlogoff,             fread_number(fp));
             break;

        case 'N':
             KEY( "Name",   pet->name,      fread_string(fp));
             break;

        case 'P':
             KEY( "Pos",    pet->position,      fread_number(fp));
             break;

    case 'R':
            KEY( "Race",    pet->race, race_lookup(fread_string(fp)));
            break;

        case 'S' :
            KEY( "Save",    pet->saving_throw,  fread_number(fp));
            KEY( "Sex",     pet->sex,       fread_number(fp));
            KEY( "ShD",     pet->short_descr,   fread_string(fp));
            KEY( "Silv",        pet->silver,            fread_number( fp ) );
            break;

        if ( !fMatch )
        {
            bug("Fread_mount: no match.",0);
            fread_to_eol(fp);
        }

        }
    }

}


extern  OBJ_DATA    *obj_free;

void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    OBJ_DATA *obj;
    char *word;
    int iNest;
    bool fMatch;
    bool fNest;
    bool fVnum;
    bool first;
    bool new_format;  /* to prevent errors */
    bool make_new;    /* update object */
    int ic;
    fVnum = FALSE;
    obj = NULL;
    first = TRUE;  /* used to counter fp offset */
    new_format = FALSE;
    make_new = FALSE;

    word   = feof( fp ) ? "End" : fread_word( fp );
    if (!str_cmp(word,"Vnum" ))
    {
        int vnum;
    first = FALSE;  /* fp will be in right place */

        vnum = fread_number( fp );
        if (  get_obj_index( vnum )  == NULL )
    {
            bug( "Fread_obj: bad vnum %d.", vnum );
    }
        else
    {
        obj = create_object(get_obj_index(vnum),-1);
        new_format = TRUE;
    }

    }

    if (obj == NULL)  /* either not found or old style */
    {
        obj = new_obj();
        obj->name       = str_dup( "" );
        obj->short_descr    = str_dup( "" );
        obj->description    = str_dup( "" );
    }

    fNest       = FALSE;
    fVnum       = TRUE;
    iNest       = 0;

    for ( ; ; )
    {
    if (first)
        first = FALSE;
    else
        word   = feof( fp ) ? "End" : fread_word( fp );
    fMatch = FALSE;

    switch ( UPPER(word[0]) )
    {
    case '*':
        fMatch = TRUE;
        fread_to_eol( fp );
        break;

    case 'A':
        if (!str_cmp(word,"AffD"))
        {
        AFFECT_DATA *paf;
        int sn;

        paf = new_affect();

        sn = skill_lookup(fread_word(fp));
        if (sn < 0)
            bug("Fread_obj: unknown skill.",0);
        else
            paf->type = sn;

        paf->level  = fread_number( fp );
        paf->duration   = fread_number( fp );
        paf->modifier   = fread_number( fp );
        paf->location   = fread_number( fp );
        paf->bitvector  = fread_number( fp );
        paf->next   = obj->affected;
        obj->affected   = paf;
        fMatch      = TRUE;
        break;
        }
            if (!str_cmp(word,"Affc"))
            {
                AFFECT_DATA *paf;
                int sn;

                paf = new_affect();

                sn = skill_lookup(fread_word(fp));
                if (sn < 0)
                    bug("Fread_obj: unknown skill.",0);
                else
                    paf->type = sn;

        paf->where  = fread_number( fp );
                paf->level      = fread_number( fp );
                paf->duration   = fread_number( fp );
                paf->modifier   = fread_number( fp );
                paf->location   = fread_number( fp );
                paf->bitvector  = fread_number( fp );
                if (paf->duration > 0)
                {
                    paf->next       = obj->affected;
                    obj->affected   = paf;

                  }
                fMatch          = TRUE;
                break;
            }
        break;

    case 'C':
        KEY( "Cond",    obj->condition,     fread_number( fp ) );
        KEY( "Cost",    obj->cost,      fread_number( fp ) );
        break;

    case 'D':
        KEY( "Description", obj->description,   fread_string( fp ) );
        KEY( "Desc",    obj->description,   fread_string( fp ) );
        break;

    case 'E':

        if ( !str_cmp( word, "Enchanted"))
        {
        obj->enchanted = TRUE;
        fMatch  = TRUE;
        break;
        }

        KEY( "ExtraFlags",  obj->extra_flags,   fread_number( fp ) );
        KEY( "ExtF",    obj->extra_flags,   fread_number( fp ) );

        if ( !str_cmp( word, "ExtraDescr" ) || !str_cmp(word,"ExDe"))
        {
        EXTRA_DESCR_DATA *ed;

        ed = new_extra_descr();

        ed->keyword     = fread_string( fp );
        ed->description     = fread_string( fp );
        ed->next        = obj->extra_descr;
        obj->extra_descr    = ed;
        fMatch = TRUE;
        }

        if ( !str_cmp( word, "End" ) )
        {
        if ( !fNest || ( fVnum && obj->pIndexData == NULL ) )
        {
            bug( "Fread_obj: incomplete object.", 0 );
            free_obj(obj);
            return;
        }
        else
        {
            if ( !fVnum )
            {
            free_obj( obj );
            obj = create_object( get_obj_index( OBJ_VNUM_DUMMY ), 0 );
            }
            if (!new_format)
            {
                obj->next   = object_list;
                object_list = obj;
                obj->pIndexData->count++;
            }

            if (!obj->pIndexData->new_format
            && obj->item_type == ITEM_ARMOR
            &&  obj->value[1] == 0)
            {
            obj->value[1] = obj->value[0];
            obj->value[2] = obj->value[0];
            }
            if (make_new)
            {
            int wear;

            wear = obj->wear_loc;
            extract_obj(obj);

            obj = create_object(obj->pIndexData,0);
            obj->wear_loc = wear;
            }
            if ( iNest == 0 || rgObjNest[iNest] == NULL )
            obj_to_char( obj, ch );
            else
            obj_to_obj( obj, rgObjNest[iNest-1] );
            return;
        }
        }
        break;

    case 'I':
        KEY( "ItemType",    obj->item_type,     fread_number( fp ) );
        KEY( "Ityp",    obj->item_type,     fread_number( fp ) );
        break;

    case 'L':
        KEY( "Level",   obj->level,     fread_number( fp ) );
        KEY( "Lev",     obj->level,     fread_number( fp ) );
        break;

    case 'N':
        KEY( "Name",    obj->name,      fread_string( fp ) );

        if ( !str_cmp( word, "Nest" ) )
        {
        iNest = fread_number( fp );
        if ( iNest < 0 || iNest >= MAX_NEST )
        {
            bug( "Fread_obj: bad nest %d.", iNest );
        }
        else
        {
            rgObjNest[iNest] = obj;
            fNest = TRUE;
        }
        fMatch = TRUE;
        }
        break;

    case 'O':
        if ( !str_cmp( word,"Oldstyle" ) )
        {
        if (obj->pIndexData != NULL && obj->pIndexData->new_format)
            make_new = TRUE;
        fMatch = TRUE;
        }
        break;


    case 'S':
        KEY( "ShortDescr",  obj->short_descr,   fread_string( fp ) );
        KEY( "ShD",     obj->short_descr,   fread_string( fp ) );

        if ( !str_cmp( word, "Spell" ) )
        {
        int iValue;
        int sn;

        iValue = fread_number( fp );
        sn     = skill_lookup( fread_word( fp ) );
        if ( iValue < 0 || iValue > 3 )
        {
            bug( "Fread_obj: bad iValue %d.", iValue );
        }
        else if ( sn < 0 )
        {
            bug( "Fread_obj: unknown skill.", 0 );
        }
        else
        {
            obj->value[iValue] = sn;
        }
        fMatch = TRUE;
        break;
        }

        break;

    case 'T':
        KEY( "Timer",   obj->timer,     fread_number( fp ) );
        KEY( "Time",    obj->timer,     fread_number( fp ) );
        break;

    case 'V':
        if ( !str_cmp( word, "Values" ) || !str_cmp(word,"Vals"))
        {
        obj->value[0]   = fread_number( fp );
        obj->value[1]   = fread_number( fp );
        obj->value[2]   = fread_number( fp );
        obj->value[3]   = fread_number( fp );
        if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
           obj->value[0] = obj->pIndexData->value[0];

        switch (obj->item_type) {

          case ITEM_LIGHT:
          case ITEM_SCROLL:
          case ITEM_WAND:
          case ITEM_STAFF:
          case ITEM_POTION:
          case ITEM_DRINK_CON:
          case ITEM_FOUNTAIN:
          case ITEM_PILL:
          case ITEM_PORTAL:
          case ITEM_TRAP:
          case ITEM_PLASM:
          case ITEM_LOCKPICK:
          case ITEM_BLASTPOWDER:
            break;
          default:
            for (ic=0;ic<4;ic++)
              obj->value[ic] = obj->pIndexData->value[ic];
            }

        fMatch      = TRUE;
        break;
        }

        if ( !str_cmp( word, "Val" ) )
        {
        obj->value[0]   = fread_number( fp );
        obj->value[1]   = fread_number( fp );
        obj->value[2]   = fread_number( fp );
        obj->value[3]   = fread_number( fp );
        obj->value[4]   = fread_number( fp );
        fMatch = TRUE;
        break;
        }

        if ( !str_cmp( word, "Vnum" ) )
        {
        int vnum;

        vnum = fread_number( fp );
        if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL )
            bug( "Fread_obj: bad vnum %d.", vnum );
        else
            fVnum = TRUE;
        fMatch = TRUE;
        break;
        }
        break;

    case 'W':
        KEY( "WearFlags",   obj->wear_flags,    fread_number( fp ) );
        KEY( "WeaF",    obj->wear_flags,    fread_number( fp ) );
        KEY( "WearLoc", obj->wear_loc,      fread_number( fp ) );
        KEY( "Wear",    obj->wear_loc,      fread_number( fp ) );
        KEY( "Weight",  obj->weight,        fread_number( fp ) );
        KEY( "Wt",      obj->weight,        fread_number( fp ) );
        break;

    }

    if ( !fMatch )
    {
        bug( "Fread_obj: no match.", 0 );
        fread_to_eol( fp );
    }
    }
}
