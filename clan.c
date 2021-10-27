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
*   ROM 2.4 is copyright 1993-1996 Russ Taylor                 *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@efn.org)                  *
*       Gabrielle Taylor                           *
*       Brian Moore (zump@rom.org)                     *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/

/***************************************************************************
*       This file is for clan related commands.  Included here is:         *
*                 The JOIN command.                                        *
*                 The immortal CLEADER command.                            *
*                 The clan leader ACCEPT command.                          *
*                 The clan leader LONER command.                           *
*       Clans can be found in the clan table in tables.c                   *
*                             -Blizzard (blizzard_imp@hotmail.com)         *
***************************************************************************/

/***************************************************************************
*    Highly Modified clan system modded by Bojack of Dragonball Arena.     *
*   Added to this file is:                         *
*       top_clan                           *
*       bit names                          *
*       loading clans                          *
*       is_clan                            *
*       is_same_clan                           *
*       clan_lookup                        *
*       player_rank                        *
*       player_clan                        *
*   Nothing else was modified, I purposely made it around the      *
*   clan system written by Blizzard.                   *
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
#include <stdlib.h>
#include "merc.h"

#include "db.h" // For top_clan and added into do_memory

int top_clan;

char *clan_bit_name( int clan_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( clan_flags & CLAN_INDEPENDENT  ) strcat( buf, " independent");
    if ( clan_flags & CLAN_CHANGED  ) strcat( buf, " changed");
    if ( clan_flags & CLAN_DELETED  ) strcat( buf, " deleted");
    if ( clan_flags & CLAN_IMMORTAL ) strcat( buf, " immortal");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

void load_clans(void)
{
    FILE *fp;
    char *string;
    int count = 0;
    int i;
    bool fMatch = FALSE;

    for (i = 0; i < MAX_CLAN; i++) //MAX_CLAN; i++)
    {
    clan_table[i].name = "";
    clan_table[i].who_name = "";
    clan_table[i].leader = "";
    clan_table[i].flags = 0;
    clan_table[i].clan_type = 0;
    clan_table[i].bank = 0;
    clan_table[i].bonus = 0;
    clan_table[i].clanhall = ROOM_VNUM_TEMPLE;
    clan_table[i].banker = "";
    }

    if (!(fp = fopen(CLAN_FILE, "r")) )
    {
    log_string("Error: guild.dat file not found!");
    exit(1);
    }

    for (;;)
    {
    string = feof(fp) ? "End" : fread_word(fp);

    if (!str_cmp(string, "End"))
        break;

    switch (UPPER(string[0]))
    {
          case 'C':
          top_clan++;
              count++;
              clan_table[count].name = fread_string(fp);
              fMatch = TRUE;
              break;
          case 'B':
            if (!str_cmp(string, "Bank"))
            {
                clan_table[count].bank = fread_number(fp);
                fMatch = TRUE;
                break;
            }
            if (!str_cmp(string, "Bonus"))
            {
                clan_table[count].bonus = fread_number(fp);
                fMatch = TRUE;
                break;
            }
            if (!str_cmp(string, "Banker"))
            {
                clan_table[count].banker = fread_string(fp);
                fMatch = TRUE;
                break;
            }

          case 'H':
              if (!str_cmp(string, "Hall"))
              {
                  clan_table[count].clanhall = fread_number(fp);
                  fMatch = TRUE;
                  break;
              }

          case 'F':
              clan_table[count].flags  = fread_flag( fp );
              fMatch = TRUE;
              break;
          case 'T':
              clan_table[count].clan_type = fread_number(fp);
              fMatch = TRUE;
              break;

          case 'L':
          clan_table[count].leader = fread_string(fp);
          fMatch = TRUE;
          break;

      case 'W':
          clan_table[count].who_name = fread_string(fp);
          fMatch = TRUE;
          break;
      }         /* end of switch */
    }               /* end of while (!feof) */

    if (!fMatch)
    {
    log_string("Fread_clans: no match.");
    fread_to_eol(fp);
    }
    fclose(fp);
    return;
}

bool is_clan(CHAR_DATA * ch)
{
    if (ch)
        return ch->clan;
    else
    {
        log_string("is_clan: NULL ch passed to is_clan");
        return FALSE;
    }
}

bool is_same_clan(CHAR_DATA * ch, CHAR_DATA * victim)
{
    if (IS_SET(clan_table[ch->clan].flags, CLAN_INDEPENDENT))
    return FALSE;
    else
    return (ch->clan == victim->clan);
}

int clan_lookup(const char *name)
{
    int clan;

    for (clan = 0; clan < MAX_CLAN; clan++)
    {
    if (!str_prefix(name, clan_table[clan].name))
        return clan;
    }

    return 0;
}

char *player_clan(CHAR_DATA * ch)
{
    if (ch->clan == 0)
    return '\0';
    return clan_table[ch->clan].name;
}


/* Commands */
void do_cleader (CHAR_DATA *ch, char *argument) {
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int i;

    one_argument( argument, arg );

    if (arg[0] == '\0') {
        sendch( "Syntax: cleader <name>\n\r", ch );
        return;
    }

    if ((victim = get_char_world(ch, arg)) == NULL ) {
        sendch( "They aren't here.\n\r", ch );
        return;
    }

    if (victim->level > ch->level) {
        sendch( "You failed.\n\r", ch );
        return;
    }

    if (IS_SET(victim->act2, PLR2_LEADER)) {
        REMOVE_BIT(victim->act2, PLR2_LEADER);
        sendch( "The gods have revoked your leadership priviliges.\n\r", victim );
        sendch( "Leadership removed.\n\r", ch );
        free_string(clan_table[victim->clan].leader);
        clan_table[victim->clan].leader = str_dup("None");
        //sprintf(buf,"$N takes leadership away from %s",victim->name);
    }
    else {
        if (!is_clan(victim)) {
            sendch ("That person must be clanned first.\n\r", ch);
            return;
        }

        free_string(clan_table[victim->clan].leader);
        SET_BIT(victim->act2, PLR2_LEADER);
        clan_table[victim->clan].leader = str_dup(victim->name);
        sendch( "The gods have made you a clan leader.\n\r", victim );
        sendch( "Leadership set.\n\r", ch );
        //sprintf(buf,"$N make %s a clan leader.",victim->name);
    }
    save_clans(ch, argument);
    return;
}

void do_banker (CHAR_DATA *ch, char *argument)
{
    char name[MIL];
    CLAN_DATA *pClan;
    char buf[MSL];
    one_argument( argument, name );

    if (!is_clan(ch))
    {
        sendch("You are not in a clan!\n\r", ch);
        return;
    }
    if ( !IS_IMMORTAL(ch) && str_cmp(clan_table[ch->clan].leader, ch->name))
    {
        sendch("You are not the leader of your clan!\n\r", ch);
        return;
    }
    pClan = get_clan_data(clan_table[ch->clan].name);
    if (pClan == NULL)
    {
        sendch("BUG: NULL Clan. See a coder about this!\n\r", ch);
        return;
    }

    if ( name[0] == '\0' )
    {
    send_to_char( "Syntax:  banker [$name]  -toggles banker\n\r", ch );
    send_to_char( "Syntax:  banker All      -allows everyone\n\r", ch );
    return;
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
    save_clans(ch, argument);
    return;
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
    save_clans(ch, argument);
    return;
    }

}

void do_declare( CHAR_DATA *ch, char *argument )
{
    DOMAIN_DATA *dom;
    char    arg1[MIL];
    char    arg2[MIL];
    int     clan;
    int     relation;
    char    buf[MSL];
    DESCRIPTOR_DATA *d;
    CHAR_DATA       *dch;
    if (IS_NPC(ch))
        return;
    if (!is_clan(ch))
    {
        sendch("You are not in a clan!\n\r", ch);
        return;
    }
    if (str_cmp(clan_table[ch->clan].leader, ch->name))
    {
        sendch("Only the leader of a clan may declare the relations of a domain.\n\r", ch);
        return;
    }
    if( (dom = ch->in_room->area->domain) == NULL || dom->clan != ch->clan)
    {
        sendch("This is not your domain!\n\r", ch);
        return;
    }
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1) || IS_NULLSTR(arg2))
    {
        sendch("Syntax: declare <relation> <clan>\n\r", ch);
        sendch("Relations are 'ally', 'war', or 'neutral'.\n\r", ch);
        return;
    }
    if ((clan = clan_lookup(arg2)) == 0)
        {
            sendch("That is not a valid clan!\n\r", ch);
            return;
        }
    if (clan == ch->clan)
    {
        sendch("You cannot declare against your own clan!\n\r", ch);
        return;
    }

    if (!str_cmp(arg1, "ally"))
        relation = RELATION_ALLIED;
    else if (!str_cmp(arg1, "war"))
        relation = RELATION_WARRING;
    else if (!str_cmp(arg1, "neutral"))
        relation = RELATION_NONE;
    else {
        sendch("That is not a valid domain relation. Please choose either\n\r", ch);
        sendch("'ally', 'war', or 'neutral' to declare.\n\r", ch);
        return;
    }
    if (dom->relation[clan] == relation)
    {
        sendch("There would be no point to that.. It's already declared!\n\r", ch);
        return;
    }

    dom->relation[clan] = relation;
    switch (relation)
    {
        case RELATION_NONE:
            sprintf(buf, "You have declared %s neutral for members of clan %s!\n\r", ch->in_room->area->name, clan_table[clan].name);
            break;
        case RELATION_ALLIED:
            sprintf(buf, "You have declared %s allied with clan %s!\n\r",ch->in_room->area->name, clan_table[clan].name);
            break;
        case RELATION_WARRING:
            sprintf(buf, "You have declared %s at war with clan %s!\n\r",ch->in_room->area->name, clan_table[clan].name);
            break;
        }
    sendch(buf, ch);

    switch (relation)
    {
        case RELATION_NONE:
            sprintf(buf, "%s has declared %s neutral for members of clan %s!\n\r", ch->name, ch->in_room->area->name, clan_table[clan].name);
            break;
        case RELATION_ALLIED:
            sprintf(buf, "%s has declared %s allied with clan %s!\n\r", ch->name, ch->in_room->area->name, clan_table[clan].name);
            break;
        case RELATION_WARRING:
            sprintf(buf, "%s has declared %s at war with clan %s!\n\r", ch->name, ch->in_room->area->name, clan_table[clan].name);
            break;
        }
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        dch = d->character;
        if (dch->clan != ch->clan || dch == ch)
            continue;

        sendch(buf, dch);
    }
    save_area(ch->in_room->area);
    return;

}

void do_tax (CHAR_DATA *ch, char *argument)
{
    DOMAIN_DATA *dom;
    int tax;

    if (IS_NPC(ch))
        return;
    if (!is_clan(ch))
    { sendch("Huh?", ch);return;}
    if (!IS_LEADER(ch))
    {sendch("Only the leader of a clan may set a tax on their domain.\n\r", ch); return;}
    if ( (dom = ch->in_room->area->domain) == NULL)
        {sendch("This is not your domain!\n\r", ch); return;}
    if (dom->clan != ch->clan)
    {sendch("This is not your domain!\n\r", ch); return;}
    if (IS_NULLSTR(argument) || !is_number( argument ) || (tax = atoi(argument)) > 100 || tax < 0 )
    {
        sendch("Syntax: tax <percent> - Percent must be a number between 1 and 100.\n\r", ch);
        printf_to_char(ch, "Current Tax: %d%% markup.\n\r", dom->tax);
        return;
    }
    printf_to_char(ch, "Setting tax percentage to %d%% for this domain.\n\r", tax);
    dom->tax = tax;
    save_area( ch->in_room->area );
    return;
}

void do_claim (CHAR_DATA *ch, char *argument)
{
    DOMAIN_DATA *dom;
    char    buf[MSL];
    char    arg[MIL];

    if (IS_NPC(ch))
        return;
    if (!is_clan(ch))
    {
        sendch("You are not in a clan!\n\r", ch);
        return;
    }
    if ( (dom = ch->in_room->area->domain) == NULL)
    {
        sendch("This area cannot be claimed as a domain.\n\r", ch);
        return;
    }

    if (dom->clan == ch->clan)
    {
      sendch("Your clan already rules over this domain!\n\r", ch);
      return;
    }
    
    argument = one_argument(argument, arg);

    if (str_cmp(arg, "confirm"))
    {
     if (dom->clan == 0)
     {
         sendch("This domain is currently unclaimed by any clan.\n\r", ch);
         cprintf(ch, "Your clan's current influence in this domain is %d%%.\n\r", dom->influence[ch->clan]);
         return;
     } else {
         cprintf(ch, "This domain is currently claimed by the %s clan. They have %d%% influence over the domain.\n\r",
         clan_table[dom->clan].name, dom->influence[dom->clan]);
         cprintf(ch, "Your clan currently has %d%% influence in the area. You will need at least %d%% influence to takeover this domain.\n\r",
         dom->influence[ch->clan], dom->influence[dom->clan] + 30);
         return;
     }
    }

    if (dom->clan == 0 && dom->influence[ch->clan] < 49)
    {
        sendch("Your clan does not have enough influence in this domain to claim it.\n\r", ch);
        return;
    } else if (dom->clan > 1 && dom->influence[ch->clan] < 50 &&
                dom->influence[ch->clan] + 30 < dom->influence[dom->clan] )
    {
        sendch("Your clan does not have enough influence in this domain to oust the ruling clan.\n\r", ch);
        return;
    }
    int i;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *fch;
    for (i = 0 ; i < MAX_CLAN ; i ++ )
    {
        dom->relation == RELATION_NONE;
        if (i == ch->clan)
            continue;
        dom->influence[i] = 0;
        dom->change[i] = 0;
    }
    if (dom->clan < 0)
        cprintf(ch, "Your clan wrests this domain from the harsh rule of the %s clan!\n\r", clan_table[dom->clan].name);
    else
        sendch("Your clan moves in and claims this domain as their own.\n\r", ch);

    if (dom->tax > 0)
        dom->tax = 0;

    for (d = descriptor_list; d != NULL ; d = d->next)
    {
        fch = d->character;
        if (fch == NULL || fch == ch || !is_clan(fch))
            continue;
        if (dom->clan > 0)
        {
        if (fch->clan == dom->clan)
            cprintf(fch, "The %s clan has moved in and claimed %s from your clan's rule!\n\r", clan_table[ch->clan].name, ch->in_room->area->name);
        }
        if (fch->clan == ch->clan)
            cprintf(fch, "%s has claimed the %s domain in the name of %s!\n\r", ch->name, clan_table[ch->clan].name);
    }

    dom->clan = ch->clan;
    save_area(ch->in_room->area);
    return;
}
