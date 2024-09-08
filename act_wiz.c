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
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"
#include <stddef.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "addict.h"

/*
 * Local functions.
 */
ROOM_INDEX_DATA *   find_location   args( ( CHAR_DATA *ch, char *arg ) );
void restore_one            args ( (CHAR_DATA *ch, CHAR_DATA *victim, bool show) );
//void write_wizlist            args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );

void do_cheater(CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
char buf[MAX_STRING_LENGTH];


argument = one_argument( argument, arg1);

   if((victim = get_char_world(ch, arg1)) == NULL)
    {
        send_to_char("Who?\n\r",ch);
        return;
    }
    if(victim->cheater == 1)
      {
        send_to_char("Character is currently flagged as a cheater!\n\r",ch);
        send_to_char("Switching off cheater flag.\n\r",ch);
        victim->cheater = 0;
        return;
      }
   if(victim->cheater == 0)
      {
        send_to_char("Character is not flagged as a cheater.\n\r",ch);
        send_to_char("Character now flagged as a cheater!\n\r",ch);
        victim->cheater = 1;
        return;
      }
}

void do_tlookup(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int i;
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_INDEX_DATA *pObjIndex2;

    argument = one_argument(argument, arg); // Get the input value from the argument
    argument = one_argument(argument, arg1);

    if (arg[0] == '\0' && arg1[0] == '\0')
    {
        send_to_char("Usage: tlookup <table> <name>\r\nAvailable tables: brew, craft, liquid\r\n", ch);
        return;
    }


/* Begin Liquid table lookup */
    if (!str_prefix(arg, "liquid")) {
        if (arg1[0] == '\0') {
            sprintf(buf, "All liquids:\r\n          ");
            int counter = 0;
            for (i = 0; liq_table[i].liq_name != NULL; i++) {
                sprintf(buf + strlen(buf), "%-20s", liq_table[i].liq_name);
                counter++;
                if (counter == 3) {
                    strcat(buf, "\r\n");
                    send_to_char(buf, ch);
                    sprintf(buf, "          "); // Indentation for subsequent lines
                    counter = 0;
                }
            }
            if (counter != 0) {
                strcat(buf, "\r\n");
                send_to_char(buf, ch);
            }
            return;
        }

        for (i = 0; liq_table[i].liq_name != NULL; i++)
        {
            if (str_prefix(arg1, liq_table[i].liq_name) == 0)
            {
                // Match found, display the information

                    sprintf(buf,"%-10s : %-20s\r\n"
                                "%-10s : %-20s\r\n"
                                "%-10s : %-20d\r\n"
                                "%-10s : %-20d\r\n"
                                "%-10s : %-20d\r\n"
                                "%-10s : %-20d\r\n"
                                "%-10s : %-20d\r\n",
                                "Name", liq_table[i].liq_name,
                                "Color", liq_table[i].liq_color,
                                "Proof", liq_table[i].liq_affect[0],
                                "Fullness", liq_table[i].liq_affect[1],
                                "Thirst", liq_table[i].liq_affect[2],
                                "Food", liq_table[i].liq_affect[3],
                                "Size", liq_table[i].liq_affect[4]);

                    send_to_char(buf,ch);

                    return;
            }
        }

    // No match found
    send_to_char("No such drink exists.\r\n", ch);
    return;
    }
/* End Liquid table lookup */

/* Brew table */
    if ( !str_prefix( arg, "brew" ) )
    {
        if (arg1[0] == '\0') {
            sprintf(buf, "All potions:\r\n          ");
            int counter = 0;
            for (i = 0; brew_table[i].name != NULL; i++) {
                sprintf(buf + strlen(buf), "%-20s", brew_table[i].name);
                counter++;
                if (counter == 3) {
                    strcat(buf, "\r\n");
                    send_to_char(buf, ch);
                    sprintf(buf, "          "); // Indentation for subsequent lines
                    counter = 0;
                }
            }
            if (counter != 0) {
                strcat(buf, "\r\n");
                send_to_char(buf, ch);
            }
            return;
        }

        for (i = 0; brew_table[i].name != NULL; i++)
        {
            if (str_prefix(arg1, brew_table[i].name) == 0)
            {
                // Match found, display the information

            sprintf(buf, "%-15s : %-20s\r\n"
                         "%-15s : %-20s\r\n"
                         "%-15s : %-10d\r\n"
                         "%-15s : %-10d\r\n"
                         "%-15s : %-10d\r\n"
                         "%-15s : %-6d %-6d %-6d %-6d %-6d\r\n"
                         "%-15s : %-10s\r\n"
                         "%-15s : %-10s\r\n"
                         "%-15s : %-10s\r\n",
                         "Name", brew_table[i].name,
                         "Spell Number", skill_table[slot_lookup(brew_table[i].sn)].name,
                         "Min Level", brew_table[i].level,
                         "Potion Level", brew_table[i].plevel,
                         "Diff", brew_table[i].diff,
                         "Components", brew_table[i].component[0], brew_table[i].component[1], brew_table[i].component[2], brew_table[i].component[3], brew_table[i].component[4],
                         "Mage Only", brew_table[i].mage ? "TRUE" : "FALSE",
                         "Imm Only", brew_table[i].immonly ? "TRUE" : "FALSE",
                         "Color", brew_table[i].color);


                    send_to_char(buf,ch);

                    return;
            }

        }

    // No match found
    send_to_char("No such brew recipe exists.\r\n", ch);
    return;
    }

/* Craft table */
    if ( !str_prefix( arg, "craft" ) )
    {
        if (arg1[0] == '\0') {
            sprintf(buf, "All craftable items:\r\n          ");
            int counter = 0;
            for (i = 0; crafted_item_table[i].name != NULL; i++) {
                sprintf(buf + strlen(buf), "%-20s", crafted_item_table[i].name);
                counter++;
                if (counter == 3) {
                    strcat(buf, "\r\n");
                    send_to_char(buf, ch);
                    sprintf(buf, "          "); // Indentation for subsequent lines
                    counter = 0;
                }
            }
            if (counter != 0) {
                strcat(buf, "\r\n");
                send_to_char(buf, ch);
            }
            return;
        }

        for (i = 0; crafted_item_table[i].name != NULL; i++)
        {
            if (str_prefix(arg1, crafted_item_table[i].name) == 0)
            {
                // Match found, display the information
                    pObjIndex = get_obj_index( crafted_item_table[i].vnum );
                    pObjIndex2 = get_obj_index( crafted_item_table[i].resource_type );
                    sprintf(buf,"%-20s : %s\r\n"
                                "%-20s : %s\r\n"
                                "%-20s : [%5d] %s\r\n"
                                "%-20s : %s\r\n"
                                "%-20s : %d\r\n"
                                "%-20s : [%5d] %s\r\n"
                                "%-20s : %d %d %d %d %d\r\n"
                                "%-20s : %d %d %d %d %d\r\n"
                                "%-20s : %d %d %d %d %d %d %d %d %d %d\r\n",
                    
                    "Name", crafted_item_table[i].name,
                    "Type", crafted_item_table[i].type,
                    "VNUM", crafted_item_table[i].vnum, pObjIndex->short_descr,
                    "Skill", sec_abil_table[crafted_item_table[i].skill].name,
                    "Level", crafted_item_table[i].lvl,
                    "Resource Type", crafted_item_table[i].resource_type, pObjIndex2->short_descr,
                    "Components", crafted_item_table[i].comp[0],crafted_item_table[i].comp[1],crafted_item_table[i].comp[2],crafted_item_table[i].comp[3],crafted_item_table[i].comp[4],
                    "Socket Types", crafted_item_table[i].socket[0],crafted_item_table[i].socket[1],crafted_item_table[i].socket[2],crafted_item_table[i].socket[3],crafted_item_table[i].socket[4],
                    "Best Resource", crafted_item_table[i].best[0],crafted_item_table[i].best[1],crafted_item_table[i].best[2],crafted_item_table[i].best[3],crafted_item_table[i].best[4],crafted_item_table[i].best[5],crafted_item_table[i].best[6],crafted_item_table[i].best[7],crafted_item_table[i].best[8],crafted_item_table[i].best[9]);

                    send_to_char(buf,ch);

                    return;
            }

        }

    // No match found
    send_to_char("No such craft recipe exists.\r\n", ch);
    return;
    }

    else
        {
            send_to_char("No such table. Available options: brew, craft, liquid\r\n",ch);
            return;
        }
}

void do_qpmult(CHAR_DATA *ch,char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char buf [MAX_STRING_LENGTH];

argument = one_argument( argument, arg1);
if ( arg1[0] == '\0' )
{
 sprintf(buf, "Current rate of gain for global quest points is {W%d{Yx{x.\n\r",qpmult);
 send_to_char(buf,ch);
 return;
}
qpmult = atoi(arg1);

sprintf(buf,"Adjusting global QP multiper to {W%d{Yx{x.\n\r",qpmult);
send_to_char(buf,ch);
}

void do_xpmult(CHAR_DATA *ch,char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char buf [MAX_STRING_LENGTH];

argument = one_argument( argument, arg1);
if ( arg1[0] == '\0' )
{
 sprintf(buf,"Current rate of global XP gain is at {W%d{Yx{x.\n\r",xpmult);
 send_to_char(buf,ch);
 return;
}
xpmult = atoi(arg1);

sprintf(buf,"Adjusting global XP multiper to {W%d{Yx{x.\n\r",xpmult);
send_to_char(buf,ch);
}


void do_tag(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int tag;

    argument = one_argument( argument, arg1 );


       if ( arg1 == '\0' )
    {
    send_to_char( "Tag who?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if(ch->tagged == 1)
    {
        send_to_char("You are currently tagged!\n\r",ch);
        send_to_char("Syntax: tag <person> to tag someone else!\n\r",ch);
    }
   if(ch->tagged == 0)
        {
        send_to_char("You are not currently tagged.\n\r",ch);
        send_to_char("Syntax: tag <person>\n\r",ch);
        return;
        }


   if(IS_NPC(victim))

   {
   send_to_char("Do not try to break ma shit! Not on NPCS!\n\r",ch);
   return;
   }

   if (ch->tagged == 0)
   {
   send_to_char("You can't tag someone! You're not it!\n\r", ch);
   return;
   }

    if ( IS_AFFECTED(victim, AFF_FAERIE_FIRE) )
    {
    send_to_char("No tag backs! Either find a new victim or an immortal.\n\r",ch);
    return;
    }

   ch->tagged = 0;
   victim->tagged = 1;
   sprintf(buf,"You tag %s! They are IT!\n\r",victim->name);
   send_to_char(buf,ch);
   sprintf(buf,"%s has tagged you! You are it!\n\r",ch->name);
   send_to_char(buf,victim);

}
void do_wiznet( CHAR_DATA *ch, char *argument )
{
    int flag;
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->wiznet,WIZ_ON))
        {
            send_to_char("Signing off of Wiznet.\n\r",ch);
            REMOVE_BIT(ch->wiznet,WIZ_ON);
        }
        else
        {
            send_to_char("Welcome to Wiznet!\n\r",ch);
            SET_BIT(ch->wiznet,WIZ_ON);
        }
        return;
    }

    if (!str_prefix(argument,"on"))
    {
    send_to_char("Welcome to Wiznet!\n\r",ch);
    SET_BIT(ch->wiznet,WIZ_ON);
    return;
    }

    if (!str_prefix(argument,"off"))
    {
    send_to_char("Signing off of Wiznet.\n\r",ch);
    REMOVE_BIT(ch->wiznet,WIZ_ON);
    return;
    }

    /* show wiznet status */
    if (!str_prefix(argument,"status"))
    {
    buf[0] = '\0';

    if (!IS_SET(ch->wiznet,WIZ_ON))
        strcat(buf,"off ");

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
        if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
        {
        strcat(buf,wiznet_table[flag].name);
        strcat(buf," ");
        }

    strcat(buf,"\n\r");

    send_to_char("Wiznet status:\n\r",ch);
    send_to_char(buf,ch);
    return;
    }

    if (!str_prefix(argument,"show"))
    /* list of all wiznet options */
    {
    buf[0] = '\0';

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
        if (wiznet_table[flag].level <= get_trust(ch))
        {
            strcat(buf,wiznet_table[flag].name);
            strcat(buf," ");
        }
    }

    strcat(buf,"\n\r");

    send_to_char("Wiznet options available to you are:\n\r",ch);
    send_to_char(buf,ch);
    return;
    }

    flag = wiznet_lookup(argument);

    if (flag == -1 || get_trust(ch) < wiznet_table[flag].level)
    {
    send_to_char("No such option.\n\r",ch);
    return;
    }

    if (IS_SET(ch->wiznet,wiznet_table[flag].flag))
    {
    sprintf(buf,"You will no longer see %s on wiznet.\n\r",
            wiznet_table[flag].name);
    send_to_char(buf,ch);
    REMOVE_BIT(ch->wiznet,wiznet_table[flag].flag);
        return;
    }
    else
    {
        sprintf(buf,"You will now see %s on wiznet.\n\r",
        wiznet_table[flag].name);
    send_to_char(buf,ch);
        SET_BIT(ch->wiznet,wiznet_table[flag].flag);
    return;
    }

}

void wiznet(char *string, CHAR_DATA *ch, OBJ_DATA *obj,
        long flag, long flag_skip, int min_level)
{
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->connected == CON_PLAYING
    &&  IS_IMMORTAL(d->character)
    &&  IS_SET(d->character->wiznet,WIZ_ON)
    &&  (!flag || IS_SET(d->character->wiznet,flag))
    &&  (!flag_skip || !IS_SET(d->character->wiznet,flag_skip))
    &&  get_trust(d->character) >= min_level
    &&  d->character != ch)
        {
        if (IS_SET(d->character->wiznet,WIZ_PREFIX))
        send_to_char("--> ",d->character);
            act_new(string,d->character,obj,ch,TO_CHAR,POS_DEAD, FALSE);
        }
    }

    return;
}

//Added this to use later as a do_function();
void do_save_clans (CHAR_DATA *ch, char *argument)
{
    int i;
    bool found = FALSE;

    for (i = 1; clan_table[i].name != NULL; i++)
    {
        if (IS_SET (clan_table[i].flags, CLAN_CHANGED) )
        {
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        sendch ("No modified clans to save.\n\rClan file up to date.\n\r", ch);
        return;
    }

    save_clans(ch, argument);
    sendch ("Clans file saved.\n\r", ch);
    return;
}

void do_guild( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: guild <char> <cln name>\n\r",ch);
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't playing.\n\r", ch );
        return;
    }

    if (!str_prefix(arg2,"none"))
    {
    send_to_char("They are now clanless.\n\r",ch);
    send_to_char("You are now a member of no clan!\n\r",victim);
    victim->clan = 0;
    return;
    }

    if ((clan = clan_lookup(arg2)) == 0)
    {
    send_to_char("No such clan exists.\n\r",ch);
    return;
    }

//    if (clan_table[clan].independent)
//    {
//  sprintf(buf,"They are now a %s.\n\r",clan_table[clan].name);
//  send_to_char(buf,ch);
//  sprintf(buf,"You are now a %s.\n\r",clan_table[clan].name);
//  send_to_char(buf,victim);
//  }
//    else
//    {
    sprintf(buf,"They are now a member of clan %s.\n\r",
        capitalize(clan_table[clan].name));
    send_to_char(buf,ch);
    sprintf(buf,"You are now a member of clan %s.\n\r",
        capitalize(clan_table[clan].name));
//    }

    victim->clan = clan;
}

/* equips a character */
void do_outfit ( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int i,sn,vnum;

    if (ch->level > 5 || IS_NPC(ch))
    {
    send_to_char("You should search for equipment on your own.\n\r",ch);
    return;
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
    obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_LIGHT );
    }

    if ( ( obj = get_eq_char( ch, WEAR_TORSO ) ) == NULL )
    {
    obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
    obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_TORSO );
    }

    /* do the weapon thing */
    if ((obj = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
        sn = 0;
        vnum = OBJ_VNUM_SCHOOL_SWORD; /* just in case! */

        for (i = 0; weapon_table[i].name != NULL; i++)
        {
        if (ch->pcdata->learned[sn] <
        ch->pcdata->learned[*weapon_table[i].gsn])
        {
            sn = *weapon_table[i].gsn;
            vnum = weapon_table[i].vnum;
        }
        }

        obj = create_object(get_obj_index(vnum),0);
        obj_to_char(obj,ch);
        equip_char(ch,obj,WEAR_WIELD);
    }

    if (((obj = get_eq_char(ch,WEAR_WIELD)) == NULL
    ||   !IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
    &&  (obj = get_eq_char( ch, WEAR_SHIELD ) ) == NULL )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
    obj->cost = 0;
        obj_to_char( obj, ch );
        equip_char( ch, obj, WEAR_SHIELD );
    }

    send_to_char("The Immortals have taken pity on you and graced you with a set of equipment.\n\r",ch);
}


/* RT nochannels command, for those spammers */
void do_nochannels( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Nochannel whom?", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->comm, COMM_NOCHANNELS) )
    {
        REMOVE_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have restored your channel priviliges.\n\r",
              victim );
        send_to_char( "NOCHANNELS removed.\n\r", ch );
    sprintf(buf,"$N restores channels to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
        SET_BIT(victim->comm, COMM_NOCHANNELS);
        send_to_char( "The gods have revoked your channel priviliges.\n\r",
               victim );
        send_to_char( "NOCHANNELS set.\n\r", ch );
    sprintf(buf,"$N revokes %s's channels.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}


void do_smote(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }

    if (strstr(argument,ch->name) == NULL)
    {
    send_to_char("You must include your name in an smote.\n\r",ch);
    return;
    }

    send_to_char(argument,ch);
    send_to_char("\n\r",ch);

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
        if (vch->desc == NULL || vch == ch)
            continue;

        if ((letter = strstr(argument,vch->name)) == NULL)
        {
        send_to_char(argument,vch);
        send_to_char("\n\r",vch);
            continue;
        }

        strcpy(temp,argument);
        temp[strlen(argument) - strlen(letter)] = '\0';
        last[0] = '\0';
        name = vch->name;

        for (; *letter != '\0'; letter++)
        {
            if (*letter == '\'' && matches == strlen(vch->name))
            {
                strcat(temp,"r");
                continue;
            }

            if (*letter == 's' && matches == strlen(vch->name))
            {
                matches = 0;
                continue;
            }

            if (matches == strlen(vch->name))
            {
                matches = 0;
            }

            if (*letter == *name)
            {
                matches++;
                name++;
                if (matches == strlen(vch->name))
                {
                    strcat(temp,"you");
                    last[0] = '\0';
                    name = vch->name;
                    continue;
                }
                strncat(last,letter,1);
                continue;
            }

            matches = 0;
            strcat(temp,last);
            strncat(temp,letter,1);
            last[0] = '\0';
            name = vch->name;
        }

    send_to_char(temp,vch);
    send_to_char("\n\r",vch);
    }
    CHAR_DATA *mob, *mob_next;
    for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC) && mob != ch)
        mob->pcdata->room_last_pose = 0;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;


    return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC(ch) )
    {
    smash_tilde( argument );

    if (argument[0] == '\0')
    {
        sprintf(buf,"Your poofin is %s\n\r",ch->pcdata->bamfin);
        send_to_char(buf,ch);
        return;
    }

    if ( strstr(argument,ch->name) == NULL)
    {
        send_to_char("You must include your name.\n\r",ch);
        return;
    }

    free_string( ch->pcdata->bamfin );
    ch->pcdata->bamfin = str_dup( argument );

        sprintf(buf,"Your poofin is now %s\n\r",ch->pcdata->bamfin);
        send_to_char(buf,ch);
    }
    return;
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( !IS_NPC(ch) )
    {
        smash_tilde( argument );

        if (argument[0] == '\0')
        {
            sprintf(buf,"Your poofout is %s\n\r",ch->pcdata->bamfout);
            send_to_char(buf,ch);
            return;
        }

        if ( strstr(argument,ch->name) == NULL)
        {
            send_to_char("You must include your name.\n\r",ch);
            return;
        }

        free_string( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup( argument );

        sprintf(buf,"Your poofout is now %s\n\r",ch->pcdata->bamfout);
        send_to_char(buf,ch);
    }
    return;
}



void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Deny whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    SET_BIT(victim->act, PLR_DENY);
    send_to_char( "You are denied access!\n\r", victim );
    sprintf(buf,"$N denies access to %s",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    send_to_char( "OK.\n\r", ch );
    save_char_obj(victim);
    stop_fighting(victim,TRUE);
    do_function(victim, &do_quit, "" );

    return;
}



void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Disconnect whom?\n\r", ch );
    return;
    }

    if (is_number(arg))
    {
    int desc;

    desc = atoi(arg);
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->descriptor == desc )
            {
                close_socket( d );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
    }
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim->desc == NULL )
    {
    act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
    return;
    }

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d == victim->desc )
    {
        close_socket( d );
        send_to_char( "Ok.\n\r", ch );
        return;
    }
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}



void do_pardon( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( !str_cmp( arg2, "killer" ) )
    {
    if ( IS_SET(victim->act, PLR_KILLER) )
    {
        REMOVE_BIT( victim->act, PLR_KILLER );
        send_to_char( "Killer flag removed.\n\r", ch );
        send_to_char( "You are no longer a KILLER.\n\r", victim );
    }
    return;
    }

    if ( !str_cmp( arg2, "thief" ) )
    {
    if ( IS_SET(victim->act, PLR_THIEF) )
    {
        REMOVE_BIT( victim->act, PLR_THIEF );
        send_to_char( "Thief flag removed.\n\r", ch );
        send_to_char( "You are no longer a THIEF.\n\r", victim );
    }
    return;
    }

    send_to_char( "Syntax: pardon <character> <killer|thief>.\n\r", ch );
    return;
}



void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Global echo what?\n\r", ch );
    return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
    if ( d->connected == CON_PLAYING )
    {
        if (get_trust(d->character) >= get_trust(ch))
        send_to_char( "global> ",d->character);
        send_to_char( argument, d->character );
        send_to_char( "\n\r",   d->character );
    }
    }

    return;
}



void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Local echo what?\n\r", ch );

    return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
    if ( d->connected == CON_PLAYING
    &&   d->character->in_room == ch->in_room )
    {
            if (get_trust(d->character) >= get_trust(ch))
                send_to_char( "local> ",d->character);
        send_to_char( argument, d->character );
        send_to_char( "\n\r",   d->character );
    }
    }
        CHAR_DATA *mob, *mob_next;
    for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC) && mob != ch)
        mob->pcdata->room_last_pose = 0;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;

    return;
}

void do_zecho(CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0')
    {
    send_to_char("Zone echo what?\n\r",ch);
    return;
    }

    for (d = descriptor_list; d; d = d->next)
    {
    if (d->connected == CON_PLAYING
    &&  d->character->in_room != NULL && ch->in_room != NULL
    &&  d->character->in_room->area == ch->in_room->area)
    {
        if (get_trust(d->character) >= get_trust(ch))
        send_to_char("zone> ",d->character);
        send_to_char(argument,d->character);
        send_to_char("\n\r",d->character);
    }
    }
}

void do_pecho( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument(argument, arg);

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
    send_to_char("Personal echo what?\n\r", ch);
    return;
    }

    if  ( (victim = get_char_world(ch, arg) ) == NULL )
    {
    send_to_char("Target not found.\n\r",ch);
    return;
    }

    if (get_trust(victim) >= get_trust(ch) && get_trust(ch) != MAX_LEVEL)
        send_to_char( "personal> ",victim);

    send_to_char(argument,victim);
    send_to_char("\n\r",victim);
    send_to_char( "personal> ",ch);
    send_to_char(argument,ch);
    send_to_char("\n\r",ch);
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( is_number(arg) )
    return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
    return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg, FALSE ) ) != NULL )
    return obj->in_room;

    return NULL;
}



void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
    send_to_char( "Transfer whom (and where)?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING
        &&   d->character != ch
        &&   d->character->in_room != NULL
        &&   can_see( ch, d->character ) )
        {
        char buf[MAX_STRING_LENGTH];
        sprintf( buf, "%s %s", d->character->name, arg2 );
        do_function(ch, &do_transfer, buf );
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
        send_to_char( "No such location.\n\r", ch );
        return;
    }

    if ( !is_room_owner(ch,location) && room_is_private( location )
    &&  get_trust(ch) < MAX_LEVEL)
    {
        send_to_char( "That room is private right now.\n\r", ch );
        return;
    }
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim->in_room == NULL )
    {
    send_to_char( "They are in limbo.\n\r", ch );
    return;
    }

    if ( victim->fighting != NULL )
    stop_fighting( victim, TRUE );
    act( "$n disappears in a mushroom cloud.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );

    if( MOUNTED(victim) )
    {
        char_from_room( MOUNTED(victim) );
        char_to_room( MOUNTED(victim), location );
        send_to_char("Your rider is being transferred, and so are you.\n\r", MOUNTED(victim));
    }

    if (ch->pet != NULL)
    {
        char_from_room (ch->pet);
    char_to_room (ch->pet, location);
    }

    if ( ch != victim )
    act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_function(victim, &do_look, "auto" );
    send_to_char( "Ok.\n\r", ch );
}

void do_rat(CHAR_DATA *ch, char *argument)
{
    char arg1[MIL];
    char arg2[MIL];
    int upper;
    int lower;
    int i;
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1) || IS_NULLSTR(arg2) || IS_NULLSTR(argument))
    {
        sendch("Syntax: rat  <lowvnum> <highvnum> <command>\n\r", ch);
        sendch("This will perform <command> over the specified vnums.\n\r", ch);
        sendch("Syntax: rat players <command>\n\r", ch);
        sendch("This will perform <command> at every online player.\n\r", ch);
        return;
    }

    if (is_number(arg1) && is_number(arg2))
    {
    lower = atoi(arg1);
    upper = atoi(arg2);

    if (upper <= lower)
    {
        sendch("Upper must be larger value than lower.\n\r", ch);
        return;
    }
    if ((upper - lower) > 60)
    {
        wrapln(ch, "Range is greater than 60vnums, there is a high likelihood of that"
                   " causing your connection to spam off. Please work in smaller ranges.\n\r");
        return;
    }

    original = ch->in_room;
    on = ch->on;
    for (i = lower; i <= upper; i++)
    {
        if (ch == NULL)
        {
            break;
        }

        sprintf(arg, "%d", i);

            if ( ( location = find_location( ch, arg ) ) == NULL )
                continue;


        if (!is_room_owner(ch,location) && room_is_private( location )
    &&  get_trust(ch) < MAX_LEVEL)
        continue;

    char_from_room( ch );
    char_to_room( ch, location );
    interpret( ch, argument );

    }
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
    } else if (!str_cmp(arg1, "players"))
    {
        CHAR_DATA *dch;
        DESCRIPTOR_DATA *d;

        for (d = descriptor_list; d != NULL; d = d->next )
    {
        if (ch == NULL)
            break;
        if (d == NULL || (dch = d->character) == NULL || ch->level < dch->level)
            continue;
       location = dch->in_room;

        if (!is_room_owner(ch,location) && room_is_private( location )
            &&  get_trust(ch) < MAX_LEVEL)
        continue;

        char_from_room( ch );
        char_to_room( ch, location );
        interpret( ch, argument );

    }
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
}


void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    OBJ_DATA *on;
    CHAR_DATA *wch;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
    send_to_char( "At where what?\n\r", ch );
    return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
    send_to_char( "No such location.\n\r", ch );
    return;
    }

    if (!is_room_owner(ch,location) && room_is_private( location )
    &&  get_trust(ch) < MAX_LEVEL)
    {
    send_to_char( "That room is private right now.\n\r", ch );
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



void do_goto( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    int count = 0;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Goto where?\n\r", ch );
    return;
    }

    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
    send_to_char( "No such location.\n\r", ch );
    return;
    }

    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;

    if (!is_room_owner(ch,location) && room_is_private(location)
    &&  (count > 1 || get_trust(ch) < MAX_LEVEL))
    {
    send_to_char( "That room is private right now.\n\r", ch );
    return;
    }

    if ( ch->fighting != NULL )
    stop_fighting( ch, TRUE );

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
    if (get_trust(rch) >= ch->invis_level)
    {
        if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
        act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
        else
        act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
    }
    }

    char_from_room( ch );
    char_to_room( ch, location );

    if (ch->pet != NULL)
    {
        char_from_room (ch->pet);
    char_to_room (ch->pet, location);
    }

    if( MOUNTED(ch) )
    {
        char_from_room( MOUNTED(ch) );
        char_to_room( MOUNTED(ch), location );
        send_to_char("Your rider is a god, and did a goto. You went along for the ride.\n\r", MOUNTED(ch));
    }

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust(rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }

    do_function(ch, &do_look, "auto" );
    return;
}

void do_violate( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Goto where?\n\r", ch );
        return;
    }

    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
        send_to_char( "No such location.\n\r", ch );
        return;
    }

    if (!room_is_private( location ))
    {
        send_to_char( "That room isn't private, use goto.\n\r", ch );
        return;
    }

    if ( ch->fighting != NULL )
        stop_fighting( ch, TRUE );

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust(rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                act("$t",ch,ch->pcdata->bamfout,rch,TO_VICT);
            else
                act("$n leaves in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }

    char_from_room( ch );
    char_to_room( ch, location );


    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
    {
        if (get_trust(rch) >= ch->invis_level)
        {
            if (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                act("$t",ch,ch->pcdata->bamfin,rch,TO_VICT);
            else
                act("$n appears in a swirling mist.",ch,NULL,rch,TO_VICT);
        }
    }

    do_function(ch, &do_look, "auto" );
    return;
}

/* RT to replace the 3 stat commands */

void do_stat ( CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   char *string;
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *location;
   CHAR_DATA *victim;

   string = one_argument(argument, arg);
   if ( arg[0] == '\0')
   {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  stat <name>\n\r",ch);
    send_to_char("  stat obj <name>\n\r",ch);
    send_to_char("  stat mob <name>\n\r",ch);
    send_to_char("  stat room <number>\n\r",ch);
    return;
   }

   if (!str_cmp(arg,"room"))
   {
    do_function(ch, &do_rstat, string);
    return;
   }

   if (!str_cmp(arg,"obj"))
   {
    do_function(ch, &do_ostat, string);
    return;
   }

   if(!str_cmp(arg,"char"))
   {
    do_function(ch, &do_pstat, string);
    return;
   }


   if(!str_cmp(arg,"mob"))
   {
    do_function(ch, &do_mstat, string);
    return;
   }

   /* do it the old way */

  victim = get_char_world(ch,argument);
  if (victim != NULL)
  {
    if(!IS_NPC(victim))
    {
        do_function(ch, &do_pstat, argument);
        return;
    }
    do_function(ch, &do_mstat, argument);
    return;
  }

   obj = get_obj_world(ch,argument, FALSE);
   if (obj != NULL)
   {
     do_function(ch, &do_ostat, argument);
     return;
   }

  location = find_location(ch,argument);
  if (location != NULL)
  {
    do_function(ch, &do_rstat, argument);
    return;
  }

  send_to_char("Nothing by that name found anywhere.\n\r",ch);
}

void do_rstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj;
    CHAR_DATA *rch;
    int door;

    one_argument( argument, arg );
    location = ( arg[0] == '\0' ) ? ch->in_room : find_location( ch, arg );
    if ( location == NULL )
    {
        send_to_char( "No such location.\n\r", ch );
        return;
    }

    if (!is_room_owner(ch,location) && ch->in_room != location
    &&  room_is_private( location ) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char( "That room is private right now.\n\r", ch );
        return;
    }

    sprintf( buf, "Name: '%s'\n\rArea: '%s'\n\r",
        location->name,
        location->area->name );
    send_to_char( buf, ch );

    sprintf( buf,
        "Vnum: %d  Sector: %d  Light: %d  Healing: %d  Mana: %d\n\r",
        location->vnum,
        location->sector_type,
        location->light,
        location->heal_rate,
        location->mana_rate );
        send_to_char( buf, ch );

    sprintf( buf,
        "Room flags: %s\n\rDescription:\n\r%s",
        room_bit_name(location->room_flags),
        location->description );
        send_to_char( buf, ch );

    if ( location->extra_descr != NULL )
    {
        EXTRA_DESCR_DATA *ed;

        send_to_char( "Extra description keywords: '", ch );
        for ( ed = location->extra_descr; ed; ed = ed->next )
        {
            send_to_char( ed->keyword, ch );
            if ( ed->next != NULL )
            send_to_char( " ", ch );
        }
        send_to_char( "'.\n\r", ch );
    }

    send_to_char( "Characters:", ch );
    for ( rch = location->people; rch; rch = rch->next_in_room )
    {
        if (can_see(ch,rch))
            {
            send_to_char( " ", ch );
            one_argument( rch->name, buf );
            send_to_char( buf, ch );
        }
    }

    send_to_char( ".\n\rObjects:   ", ch );
    for ( obj = location->contents; obj; obj = obj->next_content )
    {
        send_to_char( " ", ch );
        one_argument( obj->name, buf );
        send_to_char( buf, ch );
    }
    send_to_char( ".\n\r", ch );

    for ( door = 0; door <= 5; door++ )
    {
        EXIT_DATA *pexit;

        if ( ( pexit = location->exit[door] ) != NULL )
        {
            sprintf( buf,
            "Door: %d.  To: %d.  Key: %d.  Exit flags: %d.\n\rKeyword: '%s'.  Description: %s",
            door, (pexit->u1.to_room == NULL ? -1 : pexit->u1.to_room->vnum),
                pexit->key, pexit->exit_info, pexit->keyword,
                pexit->description[0] != '\0' ? pexit->description : "(none).\n\r" );
            send_to_char( buf, ch );
        }
    }

    if ( location->rprogs )
     {
         int cnt = 0;
         PROG_LIST *list;

         sprintf(buf, "\n\rROOMPrograms for [%5d]:\n\r", location->vnum);
         send_to_char( buf, ch );

         for (list=location->rprogs; list; list=list->next)
         {
             if (cnt == 0)
             {
                 send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
                 send_to_char ( " ------ ---- ------- ------\n\r", ch );
             }

             sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
                 list->vnum,prog_type_to_name(list->trig_type),
                 list->trig_phrase);
             send_to_char( buf, ch );
             cnt++;
         }
     }

    return;
}



void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA *pObj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Stat what?\n\r", ch );
    return;
    }

    if ( ( obj = get_obj_anywhere(ch,argument) ) == NULL )
    {
    send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
    return;
    }

    pObj = obj->pIndexData;

    sprintf( buf, "Name(s): %s\n\r",
    obj->name );
    send_to_char( buf, ch );

    sprintf( buf, "Vnum: %d  Format: %s  Type: %s  Resets: %d\n\r",
    obj->pIndexData->vnum, obj->pIndexData->new_format ? "new" : "old",
    item_name(obj->item_type), obj->pIndexData->reset_num );
    send_to_char( buf, ch );

    sprintf( buf, "Short description: %s\n\rLong description: %s\n\r",
    obj->short_descr, obj->description );
    send_to_char( buf, ch );

    sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
    wear_bit_name(obj->wear_flags), extra_bit_name( obj->extra_flags ) );
    send_to_char( buf, ch );

    sprintf( buf, "Number: %d/%d  Weight: %d/%d/%d (10th pounds)\n\r",
    1,           get_obj_number( obj ),
    obj->weight, get_obj_weight( obj ),get_true_weight(obj) );
    send_to_char( buf, ch );

    sprintf(buf, "Size: %s\n\r", flag_string(size_flags, obj->size) );
    send_to_char(buf, ch);

    sprintf( buf, "Level: %d  Cost: %d  Condition: %d  Timer: %d\n\r",
    obj->level, obj->cost, obj->condition, obj->timer );
    send_to_char( buf, ch );

    sprintf( buf,
    "In room: %d  In object: %s  Carried by: %s  Wear_loc: %d\n\r",
    obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
    obj->in_obj     == NULL    ? "(none)" : obj->in_obj->short_descr,
    obj->carried_by == NULL    ? "(none)" :
        can_see(ch,obj->carried_by) ? obj->carried_by->name
                    : "someone",
    obj->wear_loc );
    send_to_char( buf, ch );

    sprintf( buf, "Values: %d %d %d %d %d\n\r",
    obj->value[0], obj->value[1], obj->value[2], obj->value[3],
    obj->value[4] );
    send_to_char( buf, ch );

    /* now give out vital statistics as per identify */

    switch ( obj->item_type )
    {
        case ITEM_SCROLL:
        case ITEM_POTION:
        case ITEM_PILL:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Spell Level         : %d  \n\rFirst Spell         : %s  \n\rSecond Spell        : %s  \n\rThird Spell         : %s  \n\rFourth Spell        : %s\n\r",
            obj->value[0],
            obj->value[1] >= 0 && obj->value[1] < MAX_SKILL ? skill_table[obj->value[1]].name : "{RInvalid Spell #{x",
            obj->value[2] >= 0 && obj->value[2] < MAX_SKILL ? skill_table[obj->value[2]].name : "{RInvalid Spell #{x",
            obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ? skill_table[obj->value[3]].name : "{RInvalid Spell #{x",
            obj->value[4] >= 0 && obj->value[4] < MAX_SKILL ? skill_table[obj->value[4]].name : "{RInvalid Spell #{x");
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;

        case ITEM_TRAP:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Damage Types        : %s  \n\rTriggers            : %s  \n\rTargets             : %s  \n\rCharges Remaining   : %d  \n\r{R**Currently Unused**{x: %d\n\r",
            flag_string( trap_flags, obj->value[0]),
            flag_string( trigger_flags, obj->value[1]),
            flag_string( focus_flags, obj->value[2]),
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
        break;

        case ITEM_WAND:
        case ITEM_STAFF:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Spell Level         : %d  \n\rMaximum Charges     : %d  \n\rCurrent Charges     : %d  \n\rSpell               : %s  \n\r{R**Currently Unused**{x: %d\n\r",
          obj->value[0],
          obj->value[1],
          obj->value[2],
          obj->value[3] >= 0 && obj->value[3] < MAX_SKILL ? skill_table[obj->value[3]].name : "{RInvalid Spell #{x",
          obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_DRINK_CON:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Liquid Max          : %d  \n\rLiquid Remaining    : %d  \n\rCurrent Liquid      : %s %s \n\rPoisoned            : %s  \n\r{R**Currently Unused**{x: %d\n\r",
          obj->value[0],
          obj->value[1],
          liq_table[obj->value[2]].liq_color,
          liq_table[obj->value[2]].liq_name,
          obj->value[3] != 0 ? "Yes" : "No",
          obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_WEAPON:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Weapon Type         : %s  \n\rD10 Damage Dice     : %d  \n\rStock Damage Dice   : %dd%d (Avg. %d) \n\rDamage Type         : %s  \n\rWeapon Flags        : %s\n\r",
          obj->value[0] == WEAPON_EXOTIC ? "exotic" : obj->value[0] == WEAPON_SWORD ? "sword" :
          obj->value[0] == WEAPON_DAGGER ? "dagger" : obj->value[0] == WEAPON_SPEAR ? "spear/staff" :
          obj->value[0] == WEAPON_MACE ? "mace/club" : obj->value[0] == WEAPON_AXE ? "axe" :
          obj->value[0] == WEAPON_FLAIL ? "flail" : obj->value[0] == WEAPON_WHIP ? "whip" :
          obj->value[0] == WEAPON_POLEARM ? "polearm" : obj->value[0] == WEAPON_LANCE ? "lance" :
          "unknown",
          obj->value[1] / 20,
          obj->value[1],obj->value[2], (1 + obj->value[2]) * obj->value[1] / 2,
          obj->value[3] > 0 && obj->value[3] < MAX_DAMAGE_MESSAGE ? attack_table[obj->value[3]].noun : "undefined",
          weapon_bit_name(obj->value[4]));
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_ARMOR:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Piercing Armor      : %d  \n\rBashing Armor       : %d  \n\rSlashing Armor      : %d  \n\rMagic Armor         : %d  \n\r{R**Currently Unused**{x: %d\n\r",
          obj->value[0],
          obj->value[1],
          obj->value[2],
          obj->value[3],
          obj->value[4]);
          send_to_char( buf, ch );
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_CONTAINER:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "v[0] Maximum Weight      : %5d.%d lbs  \n\rv[1] Container Flags     : %s  \n\rv[2] Key                 : %5d %s  \n\rv[3] Max Capacity        : %5d items \n\rv[4] Weight Multiplier   : %5d %\n\r",
          obj->value[0] / 10, obj->value[0] % 10,
          cont_bit_name(obj->value[1]),
          obj->value[2],
          get_obj_index(obj->value[2])
            ? get_obj_index(obj->value[2])->short_descr : "none",
          obj->value[3],
          obj->value[4]);
          send_to_char( buf, ch );
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_CORPSE_NPC:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Mob Vnum Killed     : %d%s  \n\r{RDefault Open Corpse {x: %d  \n\rBlood Left          : %d  \n\rPoisoned            : %s  \n\rBeneficial Affect   : %s\n\r",
            obj->value[0],
            obj->value[0] == 0 ? " {r[Imm Built Obj/Not Mob Kill]{x" : "",
            obj->value[1],
            obj->value[2],
            obj->value[3] == 1 ? "Yes" : "No",
            obj->value[4] == 0 ? "None" : skill_table[obj->value[4]].name);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

          case ITEM_CORPSE_PC:
            send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
            sprintf(buf, "{R**Currently Unused**{x: %d  \n\r{RDefault Open Corpse {x: %d  \n\rBlood Left          : %d  \n\rPoisoned            : %s  \n\rBeneficial Affect   : %s\n\r",
              obj->value[0],
              obj->value[1],
              obj->value[2],
              obj->value[3] == 1 ? "Yes" : "No",
              obj->value[4] == 0 ? "None" : skill_table[obj->value[4]].name);
            send_to_char(buf, ch);
            send_to_char("{c-----------------------------------------------------{x\n\r", ch);
            break;

          case ITEM_LIGHT:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Replenish Rate      : %3d %s  \n\rReplenish Chance    : %3d%  \n\rCurrent Light Hours : %3d %s  \n\rMax Light Hours     : %3d %s \n\rLight Flags         : %s\n\r",
            obj->value[0],
            obj->value[0] == 0 ? "[{yNo Replenish{x]" : "Per Update",
            obj->value[1],
            obj->value[2],
            obj->value[2] == -1 || obj->value[2] == 999 ? "[{MInfinite{x]" : "",
            obj->value[3],
            obj->value[3] == -1 || obj->value[3] == 999 ? "[{MInfinite{x]" : "",
            flag_string( light_flags, obj->value[4]));
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

          case ITEM_PORTAL:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Charges             : %d  \n\rExit Flags          : %s  \n\rPortal Flags        : %s  \n\rPortal To Room Vnum : %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            flag_string( exit_flags, obj->value[1]),
            flag_string( portal_flags , obj->value[2]),
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

          case ITEM_FURNITURE:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
              sprintf( buf, "Max People          : %d  \n\rMax Weight          : %d  \n\rFurniture Flags     : %s  \n\rHealing Rate Bonus  : %d%%  \n\rMana Regen Bonus    : %d%%  \n\r",
                  obj->value[0],
                  obj->value[1],
                  flag_string( furniture_flags, obj->value[2]),
                  obj->value[3],
                  obj->value[4] );
              send_to_char( buf, ch );
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
              break;

        case ITEM_FOOD:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Food Hours          : %d  \n\rFull Hours          : %d  \n\r{R**Currently Unused**{x: %d  \n\rPoisoned            : %s  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3] != 0 ? "Yes" : "No",
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_MONEY:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Silver Pieces                    : %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_FOUNTAIN:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Liquid Total        : %d  \n\rLiquid Left         : %d  \n\rLiquid Type         : %s  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            liq_table[obj->value[2]].liq_name,
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_TOOL:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Quality             : %d  \n\rTool Type           : %s  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            tool_table[obj->value[1]].tool_name,
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_CRAFTED:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Quality             : %d  \n\rCrafted Type        : %s  \n\rAugmentation 1      : %d  \n\rAugmentation 2      : %d  \n\rAugmentation 3      : %d\n\r",
            obj->value[0],
            crafted_table[obj->value[1]].crafted_name,
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_PLASM:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_LOCKPICK: 
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Charges             : %d  \n\rDifficulty Mod      : %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_BLASTPOWDER:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "Blast Charges       : %d  \n\rDamage Dice         : %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;

        case ITEM_TREASURE:
        case ITEM_TRASH:
        case ITEM_KEY:
        case ITEM_BOAT:
        case ITEM_PROTECT:
        case ITEM_MAP:
        case ITEM_WARP_STONE:
        case ITEM_RESOURCE:
        case ITEM_ROOM_KEY:
        case ITEM_GEM:
        case ITEM_JEWELRY:
        case ITEM_JUKEBOX:
          send_to_char("{c-----------------------{YVALUES{c------------------------{x\n\r", ch);
          sprintf(buf, "{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d  \n\r{R**Currently Unused**{x: %d\n\r",
            obj->value[0],
            obj->value[1],
            obj->value[2],
            obj->value[3],
            obj->value[4]);
          send_to_char(buf, ch);
          send_to_char("{c-----------------------------------------------------{x\n\r", ch);
          break;
    }

    if ( obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL )
    {
    EXTRA_DESCR_DATA *ed;

    send_to_char( "Extra description keywords: '", ch );

    for ( ed = obj->extra_descr; ed != NULL; ed = ed->next )
    {
        send_to_char( ed->keyword, ch );
        if ( ed->next != NULL )
            send_to_char( " ", ch );
    }

    for ( ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next )
    {
        send_to_char( ed->keyword, ch );
        if ( ed->next != NULL )
        send_to_char( " ", ch );
    }

    send_to_char( "'\n\r", ch );
    }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
    sprintf( buf, "Affects %s by %d, level %d",
        affect_loc_name( paf->location ), paf->modifier,paf->level );
    send_to_char(buf,ch);
    if ( paf->duration > -1)
        sprintf(buf,", %d hours.\n\r",paf->duration);
    else
        sprintf(buf,".\n\r");
    send_to_char( buf, ch );
    if (paf->bitvector)
    {
        switch(paf->where)
        {
        case TO_AFFECTS:
            sprintf(buf,"Adds %s affect.\n",
            affect_bit_name(paf->bitvector));
            break;
                case TO_WEAPON:
                    sprintf(buf,"Adds %s weapon flags.\n",
                        weapon_bit_name(paf->bitvector));
            break;
        case TO_OBJECT:
            sprintf(buf,"Adds %s object flag.\n",
            extra_bit_name(paf->bitvector));
            break;
        case TO_IMMUNE:
            sprintf(buf,"Adds immunity to %s.\n",
            imm_bit_name(paf->bitvector));
            break;
        case TO_RESIST:
            sprintf(buf,"Adds resistance to %s.\n\r",
            imm_bit_name(paf->bitvector));
            break;
        case TO_VULN:
            sprintf(buf,"Adds vulnerability to %s.\n\r",
            imm_bit_name(paf->bitvector));
            break;
        default:
            sprintf(buf,"Unknown bit %d: %d\n\r",
            paf->where,paf->bitvector);
            break;
        }
        send_to_char(buf,ch);
    }
    }

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
    sprintf( buf, "Affects %s by %d, level %d.\n\r",
        affect_loc_name( paf->location ), paf->modifier,paf->level );
    send_to_char( buf, ch );
        if (paf->bitvector)
        {
            switch(paf->where)
            {
                case TO_AFFECTS:
                    sprintf(buf,"Adds %s affect.\n",
                        affect_bit_name(paf->bitvector));
                    break;
                case TO_OBJECT:
                    sprintf(buf,"Adds %s object flag.\n",
                        extra_bit_name(paf->bitvector));
                    break;
                case TO_IMMUNE:
                    sprintf(buf,"Adds immunity to %s.\n",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_RESIST:
                    sprintf(buf,"Adds resistance to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                case TO_VULN:
                    sprintf(buf,"Adds vulnerability to %s.\n\r",
                        imm_bit_name(paf->bitvector));
                    break;
                default:
                    sprintf(buf,"Unknown bit %d: %d\n\r",
                        paf->where,paf->bitvector);
                    break;
            }
            send_to_char(buf,ch);
        }
    }

    if ( pObj && pObj->oprogs )
    {
        int cnt = 0;
        PROG_LIST *list;

        sprintf(buf, "\n\rOBJPrograms for [%5d]:\n\r", pObj->vnum);
        send_to_char( buf, ch );

        for (list = pObj->oprogs; list; list=list->next)
        {
            if (cnt == 0)
            {
                send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
                send_to_char ( " ------ ---- ------- ------\n\r", ch );
            }

            sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
                list->vnum,prog_type_to_name(list->trig_type),
                list->trig_phrase);
            send_to_char( buf, ch );
            cnt++;
        }
    }

    return;
}



void do_mstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    MOB_INDEX_DATA *pMob;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Stat whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    sprintf( buf, "Name: %s\n\r",
    victim->name);
    send_to_char( buf, ch );

    sprintf( buf,
    "Vnum: %d  Format: %s  Race: %s  Group: %d  Sex: %s  Room: %d\n\r",
    IS_NPC(victim) ? victim->pIndexData->vnum : 0,
    IS_NPC(victim) ? victim->pIndexData->new_format ? "new" : "old" : "pc",
    race_table[victim->race].name,
    IS_NPC(victim) ? victim->group : 0, sex_table[victim->sex].name,
    victim->in_room == NULL    ?        0 : victim->in_room->vnum
    );
    send_to_char( buf, ch );

    if (IS_NPC(victim))
    {
    sprintf(buf,"Count: %d  Killed: %d Maxload: %d\n\r",
        victim->pIndexData->count,victim->pIndexData->killed,victim->pIndexData->maxload);
    send_to_char(buf,ch);
    }

    sprintf( buf, "Hp: %d/%d  Mana: %d/%d  Move: %d/%d\n\r",
    victim->hit,         victim->max_hit,
    victim->mana,        victim->max_mana,
    victim->move,        victim->max_move );
    send_to_char( buf, ch );

    sprintf( buf, "Lv: %d  Gold: %ld  Silver: %ld  Exp: %d\n\r",
    victim->level, victim->gold, victim->silver, victim->exp );
    send_to_char( buf, ch );

    sprintf(buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
        GET_AC(victim,AC_PIERCE), GET_AC(victim,AC_BASH),
        GET_AC(victim,AC_SLASH),  GET_AC(victim,AC_EXOTIC));
    send_to_char(buf,ch);

/* position info on stat was removed due to a bug i can't bloody find! */
    sprintf( buf,
    "Hit: %d  Dam: %d  Saves: %d  Size: %s  Wimpy: %d  Freebies: %d\n\r",
    GET_HITROLL(victim), GET_DAMROLL(victim), victim->saving_throw,
    size_table[victim->size].name, victim->wimpy, victim->freebie);
    send_to_char( buf, ch );

    if (IS_NPC(victim) && victim->pIndexData->new_format)
    {
    sprintf(buf, "Damage: %dd%d  Message:  %s\n\r",
        victim->damage[DICE_NUMBER],victim->damage[DICE_TYPE],
        attack_table[victim->dam_type].noun);
    send_to_char(buf,ch);
    }
    sprintf( buf, "Fighting: %s\n\r",
    victim->fighting ? victim->fighting->name : "(none)" );
    send_to_char( buf, ch );

    if ( !IS_NPC(victim) )
    {
    sprintf( buf,
        "Thirst: %d  Hunger: %d  Full: %d  Drunk: %d  Blood: %d Rage: %d CPs: %d\n\r",
        victim->pcdata->condition[COND_THIRST],
        victim->pcdata->condition[COND_HUNGER],
        victim->pcdata->condition[COND_FULL],
        victim->pcdata->condition[COND_DRUNK],
        victim->pblood, victim->rage, victim->pcdata->points );
    send_to_char( buf, ch );
    }

    sprintf( buf, "Carry number: %d  Carry weight: %ld\n\r",
    victim->carry_number, get_carry_weight(victim) / 10 );
    send_to_char( buf, ch );


    if (!IS_NPC(victim))
    {
        sprintf( buf,
        "Age: %d  Played: %d  Last Level: %d  Timer: %d\n\r",
        get_age(victim),
        (int) (victim->played + current_time - victim->logon) / 3600,
        victim->pcdata->last_level,
        victim->timer );
        send_to_char( buf, ch );
    }

    sprintf(buf, "Act: %s\n\r",act_bit_name(victim->act));
    send_to_char(buf,ch);

    sprintf(buf, "Act2: %s\n\r", act2_bit_name(victim->act2));
    sendch(buf, ch);

    if (victim->comm)
    {
        sprintf(buf,"Comm: %s\n\r",comm_bit_name(victim->comm));
        send_to_char(buf,ch);
    }

    if (IS_NPC(victim) && victim->off_flags)
    {
        sprintf(buf, "Offense: %s\n\r",off_bit_name(victim->off_flags));
    send_to_char(buf,ch);
    }

    if (victim->imm_flags)
    {
    sprintf(buf, "Immune: %s\n\r",imm_bit_name(victim->imm_flags));
    send_to_char(buf,ch);
    }

    if (victim->res_flags)
    {
    sprintf(buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags));
    send_to_char(buf,ch);
    }

    if (victim->vuln_flags)
    {
    sprintf(buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
    send_to_char(buf,ch);
    }

    sprintf(buf, "Form: %s\n\rParts: %s\n\r",
    form_bit_name(victim->form), part_bit_name(victim->parts));
    send_to_char(buf,ch);

    if (victim->affected_by)
    {
    sprintf(buf, "Affected by %s\n\r",
        affect_bit_name(victim->affected_by));
    send_to_char(buf,ch);
    }

    if (victim->affected2_by)
    {
         sprintf(buf, "Also affected by %s\n\r",
            affect2_bit_name(victim->affected2_by) );
        send_to_char(buf,ch);
    }

    sprintf( buf, "Master: %s  Leader: %s  Pet: %s  Horse: %s\n\r",
    victim->master      ? victim->master->name   : "(none)",
    victim->leader      ? victim->leader->name   : "(none)",
    victim->pet         ? victim->pet->name      : "(none)",
    victim->mount       ? victim->mount->name    : "(none)");
    send_to_char( buf, ch );

    if (!IS_NPC(victim))
    {
    sprintf( buf, "Security: %d.\n\r", victim->pcdata->security );  /* OLC */
    send_to_char( buf, ch );                    /* OLC */
    }

    sprintf( buf, "Short description: %s\n\rLong  description: %s\n\r",
    victim->short_descr,
    victim->long_descr[0] != '\0' ? victim->long_descr : "(none)\n\r" );
    send_to_char( buf, ch );

    if (IS_NPC(victim))
        if ( (pMob = victim->pIndexData) != NULL && pMob->mprogs )
    {
        int cnt = 0;
        PROG_LIST *list;

        sprintf(buf, "\n\rMOBPrograms for [%5d]:\n\r", pMob->vnum);
        send_to_char( buf, ch );

        for (list = pMob->mprogs; list; list=list->next)
        {
            if (cnt == 0)
            {
                send_to_char ( " Number Vnum Trigger Phrase\n\r", ch );
                send_to_char ( " ------ ---- ------- ------\n\r", ch );
            }

            sprintf(buf, "[%5d] %4d %7s %s\n\r", cnt,
                list->vnum,prog_type_to_name(list->trig_type),
                list->trig_phrase);
            send_to_char( buf, ch );
            cnt++;
        }
    }

    if(victim->breed > 0 && victim->auspice > 0)
    {
        sprintf(buf,"Garou Information:\n\r Breed: %s  Auspice: %s  Tribe: %s  Renown: %d\n\r",
            victim->breed == BREED_LUPUS ? "Lupus" : victim->breed == BREED_METIS ? "Metis" :
            "Homid", victim->auspice == AUSPICE_RAGABASH ? "Ragabash" : victim->auspice == AUSPICE_THEURGE ?
            "Theurge" : victim->auspice == AUSPICE_PHILODOX ? "Philodox" : victim->auspice == AUSPICE_GALLIARD ?
            "Galliard" : "Ahroun", capitalize(clan_table[victim->clan].name), victim->renown);
        send_to_char(buf,ch);
    }

    if (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
    {
        if(!IS_NPC(victim))
        {
            sprintf(buf,"Vampire information:\n\r Clan: %s  Generation: %d(%d)  Sire: %s  Dpoints: %d  Childer: %d Blood: %d/%d\n\r",
            capitalize(clan_table[victim->clan].name),
            victim->gen,
            victim->pcdata->csgeneration,
            victim->sire,
            victim->dpoints,
            victim->childer,
            victim->pblood/10,
            victim->max_pblood/10);
            send_to_char(buf,ch);
        }
    }

    if (victim->race == race_lookup("ghoul"))
    {
        if(!IS_NPC(victim))
        {
            sprintf(buf,"Ghoul information:\n\r Generation: %d(%d)  Master: %s  Dpoints: %d  Blood: %d/%d\n\r",
            victim->gen,
            victim->pcdata->csgeneration,
            victim->vamp_master,
            victim->dpoints,
            victim->pblood/10,
            victim->max_pblood/10);
            send_to_char(buf,ch);
        }
    }
    if (victim->avatar > 0)
    {
        if(!IS_NPC(victim))
        {
            sprintf(buf,"Mage information:\n\r Tradition: %s  Mentor: %s  Apprentice: %s  Avatar: %d  Arete: %d  Quint: %d  Paradox: %d\n\r",
            capitalize(tradition_table[victim->tradition].name),
            victim->sire != NULL ? victim->sire : "none",
// new haven fix    victim->apprentice == "(null)" ?  "none" : victim->apprentice == NULL ? "none" : victim->apprentice,
            victim->apprentice == NULL ? "none" : victim->apprentice,
            victim->avatar,
            victim->arete,
            victim->quintessence,
            victim->paradox);
            send_to_char(buf,ch);
        }
    }
    if ( IS_NPC(victim) && victim->spec_fun != 0 )
    {
    sprintf(buf,"Mobile has special procedure %s.\n\r",
        spec_name(victim->spec_fun));
    send_to_char(buf,ch);
    }

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
    sprintf( buf,
        "Spell: '%s' modifies %s by %d for %d hours with bits %s%s, level %d.\n\r",
        skill_table[(int) paf->type].name,
        affect_loc_name( paf->location ),
        paf->modifier,
        paf->duration,
        paf->where == TO_RESIST ? "res_" : paf->where == TO_IMMUNE ? "imm_" : paf->where == TO_VULN ? "vuln_" : "",
        paf->where == TO_RESIST ? imm_bit_name(paf->bitvector) : paf->where == TO_VULN ? imm_bit_name(paf->bitvector) :
        paf->where == TO_IMMUNE ? imm_bit_name(paf->bitvector) :
        paf->where == TO_AFFECTS2 ? affect2_bit_name(paf->bitvector) : affect_bit_name( paf->bitvector ),
        paf->level
        );
    send_to_char( buf, ch );
    }
    send_to_char(" |<=-------------------------=Attributes=---------------------------=>|\n\r",ch);
    send_to_char(" |                                                                    |\n\r",ch);
    send_to_char(" |      PHYSICAL                SOCIAL                  MENTAL        |\n\r",ch);
    sprintf(buf, " | Strength_______%s Charisma_______%s Perception____%s|\n\r",dots(get_attribute(victim,STRENGTH),TRUE),dots(get_attribute(victim,CHARISMA),TRUE),dots(get_attribute(victim,PERCEPTION),TRUE));
    send_to_char(buf,ch);
    sprintf(buf, " | Dexterity______%s Manipulation___%s Intelligence__%s|\n\r",dots(get_attribute(victim,DEXTERITY),TRUE),dots(get_attribute(victim,MANIPULATION),TRUE),dots(get_attribute(victim,INTELLIGENCE),TRUE));
    send_to_char(buf,ch);
    sprintf(buf, " | Stamina________%s Appearance_____%s Wits__________%s|\n\r",dots(get_attribute(victim,STAMINA),TRUE),dots(get_attribute(victim,APPEARANCE),TRUE),dots(get_attribute(victim,WITS),TRUE));
    send_to_char(buf,ch);
    send_to_char(" |                                                                    |\n\r",ch);
    send_to_char(" |<=--------------------------=Abilities=---------------------------=>|\n\r",ch);
    send_to_char(" |                                                                    |\n\r",ch);
    send_to_char(" |       TALENTS               SKILLS                KNOWLEDGE        |\n\r",ch);
    sprintf(buf, " | Expression_____%s   Animal Ken_____%s   Academics______%s |\n\r",dots(victim->csabilities[CSABIL_EXPRESSION], FALSE),dots(victim->csabilities[CSABIL_ANIMAL_KEN], FALSE),dots(victim->csabilities[CSABIL_ACADEMICS], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Alertness______%s   Archery________%s   Seneschal______%s |\n\r",dots(victim->csabilities[CSABIL_ALERTNESS], FALSE),dots(victim->csabilities[CSABIL_ARCHERY], FALSE),dots(victim->csabilities[CSABIL_SENESCHAL], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Athletics______%s   Crafts_________%s   Investigation__%s |\n\r",dots(victim->csabilities[CSABIL_ATHLETICS], FALSE),dots(victim->csabilities[CSABIL_CRAFTS], FALSE),dots(victim->csabilities[CSABIL_INVESTIGATION], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Brawl__________%s   Etiquette______%s   Law____________%s |\n\r",dots(victim->csabilities[CSABIL_BRAWL], FALSE),dots(victim->csabilities[CSABIL_ETIQUETTE], FALSE),dots(victim->csabilities[CSABIL_LAW], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Dodge__________%s   Melee__________%s   Linguistics____%s |\n\r",dots(victim->csabilities[CSABIL_DODGE], FALSE),dots(victim->csabilities[CSABIL_MELEE], FALSE),dots(victim->csabilities[CSABIL_LINGUISTICS], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Empathy________%s   Performance____%s   Medicine_______%s |\n\r",dots(victim->csabilities[CSABIL_EMPATHY], FALSE),dots(victim->csabilities[CSABIL_PERFORMANCE], FALSE),dots(victim->csabilities[CSABIL_MEDICINE], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Intimidation___%s   Riding_________%s   Occult_________%s |\n\r",dots(victim->csabilities[CSABIL_INTIMIDATION], FALSE),dots(victim->csabilities[CSABIL_RIDING], FALSE),dots(victim->csabilities[CSABIL_OCCULT], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Leadership_____%s   Commerce_______%s   Politics_______%s |\n\r",dots(victim->csabilities[CSABIL_LEADERSHIP], FALSE),dots(victim->csabilities[CSABIL_COMMERCE], FALSE),dots(victim->csabilities[CSABIL_POLITICS], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Legerdemain____%s   Stealth________%s   Hearth Wisdom__%s |\n\r",dots(victim->csabilities[CSABIL_LEGERDEMAIN], FALSE),dots(victim->csabilities[CSABIL_STEALTH], FALSE),dots(victim->csabilities[CSABIL_HEARTHWISDOM], FALSE));
    send_to_char(buf,ch);
    sprintf(buf, " | Subterfuge_____%s   Survival_______%s   Theology_______%s |\n\r",dots(victim->csabilities[CSABIL_SUBTERFUGE], FALSE),dots(victim->csabilities[CSABIL_SURVIVAL], FALSE),dots(victim->csabilities[CSABIL_THEOLOGY], FALSE));
    send_to_char(buf,ch);
    send_to_char(" |                                                                    |\n\r",ch);
    send_to_char("<======================================================================>\n\r",ch);


    return;
}

void do_pstat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MSL];
    char buf3[MSL];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;
    int hometown;
    ROOM_INDEX_DATA *room;
    ROOM_INDEX_DATA *house;
    int i;
    int col = 1;
    BUFFER *buffer;
    buffer = new_buf();

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Stat whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }
    if(IS_NPC(victim))
    {
        send_to_char( "That isn't a player.\n\r",ch);
        return;
    }

    room = victim->in_room;
    hometown = victim->pcdata->hometown;
    house = get_room_index(victim->pcdata->home);
    sprintf(buf3, "%s", victim->pcdata->immtitle);

    send_to_char("{y                __________---------======---------__________{x\n\r",ch);
    sprintf(buf2, "%s {W%s {D[{W%3d{D]{x", !IS_NULLSTR(victim->pcdata->pretitle) ? victim->pcdata->pretitle : "", victim->name, victim->level);
    sprintf(buf, "{y[---------------! {W%s{y !---------------]{x\n\r", center(buf2, 40, " "));
    send_to_char(buf, ch);
    if (victim->cheater)
    {
      send_to_char("{y|  {R({DCheater!{R)  ({DCheater!{R)  ({DCheater!{R)  ({DCheater!{R)  ({DCheater!{R)  ({DCheater!{R)  {y|{x\n\r", ch);
    }
    sprintf(buf, "{y| {xIn Room:  %s (%5d)   {y|{x\n\r",
      room->vnum == NULL ? center("Unknown Room", 52, " ") : center(room->name, 52, " "),
      room->vnum == NULL ? 0 : room->vnum );
    send_to_char(buf, ch);
    sprintf(buf, "{y| {xCharacter Idle: %5d tick%s       %s {y|{x\n\r", victim->timer, victim->timer == 1 ? " " : "s",
      victim->timer >= 30 ? "{r--->{D[[[[ {wCHARACTER HAS VOIDED OUT{D ]]]]{x" : "                                      ");
    send_to_char(buf, ch);
    if(IS_IMMORTAL(victim))
    {
      send_to_char("{Y[-----------------------=====   Immortal Info  =====-----------------------]{x\n\r",ch);
        sprintf(buf, "{Y| {xSecurity: %d       Trust: %3d        Wizinvis: %3d         Incognito: %3d {Y|{x\n\r",
        victim->pcdata->security,        get_trust(victim),        victim->invis_level,
        victim->incog_level);
        send_to_char( buf, ch );
        sprintf(buf, "{Y| {xWiziname:  %s  {Y|{x\n\r", center(victim->pcdata->wiziname,60," "));
        send_to_char(buf, ch);
        sprintf(buf, "{Y| {xImmtitle:  %s  {Y|{x\n\r", center(victim->pcdata->immtitle,60," "));
        send_to_char(buf, ch);
    }

    if (!IS_IMMORTAL(victim))
    {
      send_to_char("{M[-----------------------=====   Mortal Trust   =====-----------------------]{x\n\r",ch);
      sprintf(buf, "{M| {xImmtalk: %s            ImmClass:  %d           Trusted Level: %4d {M|\n\r{x",
        IS_SET(victim->act, PLR_IMMTALK) ? "  [Yes]  " : "  (No)   ",
        victim->pcdata->immclass, get_trust(victim));
      send_to_char(buf, ch);
    }

    send_to_char("{W[-----------------------=====    Player Info   =====-----------------------]{x\n\r",ch);
    sprintf(buf, "{W| Race: %-12s  Sex(True): %-6s(%-6s)   Age: %6d(%7d hrs) |{x\n\r",
    race_table[victim->race].name, sex_table[victim->sex].name,  sex_table[victim->pcdata->true_sex].name,
    get_age(victim), (int) (victim->played + current_time - victim->logon) / 3600);
    send_to_char(buf, ch);

    sprintf(buf, "{W| {xRemorts: %4d    Freebies: %4d   Exp: %7d   Last Level: %7d hrs {W|{x\n\r",
    victim->remorts, victim->freebie, victim->exp, victim->pcdata->last_level);
    send_to_char( buf, ch );

    sprintf(buf, "{W| {xICMode: %s{x  RPAvail: %s{x          Quest Points: %5d   IC Hours: %5d {W|{x\n\r",
      IS_SET(victim->act, PLR_IC) ? "{gYes" : "{rNo ", IS_SET(victim->act2, PLR2_RP_AVAILABLE) ? "{gYes" : "{rNo ",
      victim->qpoints, victim->pcdata->IC_total/60);
    send_to_char(buf, ch);

    sprintf( buf, "{W| {xThirst: %3d          Hunger: %3d          Full: %3d          Drunk: %3d  {W|{x\n\r",
    victim->pcdata->condition[COND_THIRST],
    victim->pcdata->condition[COND_HUNGER],
    victim->pcdata->condition[COND_FULL],
    victim->pcdata->condition[COND_DRUNK]);
    send_to_char(buf, ch);
    sprintf( buf, "{W| {xItems Carried: %4d/%4d                   Items Weight: %7d/%7d {W|{x\n\r",
    victim->carry_number, can_carry_n(victim), get_carry_weight(victim) / 10, can_carry_w(victim)/10);
    send_to_char( buf, ch );

    send_to_char("{W+----------------------------      Society     ----------------------------+{x\n\r",ch);
    sprintf(buf, "{W| {xHome: <%5d> %-30s   Hometown: %-15s {W|{x\n\r",
        house ? house->vnum : 0, house ? house->name : "Unset", hometown_table[hometown].name);
    send_to_char(buf, ch);

    sprintf( buf, "{W| {xGold: %-15d  Silver: %-15d  In Bank: %-15d{W |\n\r",
    victim->gold, victim->silver, victim->pcdata->bank);
    send_to_char( buf, ch );

    send_to_char("{D+---------------------------      {WPVP Info      {D---------------------------+{x\n\r",ch);
    sprintf(buf, "{D| {xPVP Status:   %s        ", IS_SET(victim->act2, PLR2_PVP) ? "  {RActive{x  " : " {rInactive{x ");
    send_to_char(buf, ch);
    if (!IS_NPC(victim) && victim->pcdata->kill_target != NULL && str_cmp(victim->pcdata->kill_target, "null") &&
        str_cmp(victim->pcdata->kill_target, "(null)"))
      sprintf(buf2,"%s",center(victim->pcdata->kill_target, 20, " "));
    else
      sprintf(buf2, "     No Target      ");
    sprintf(buf, "         PK Target: {r%s {D|{x\n\r", buf2);
    send_to_char(buf,ch);

    send_to_char("{W+----------------------------      Combat      ----------------------------+{x\n\r",ch);
    sprintf( buf, "{W| {xLeader: %-15s  Pet: %-15s  Fighting: %-15s {W|{x\n\r",
    victim->leader ? victim->leader->name   : "Self",
    victim->pet ? victim->pet->name : "None",
    victim->fighting ? victim->fighting->name : "None" );
    send_to_char( buf, ch );

    sprintf( buf, "{W| {xHp: %4d/%4d(%4d)      Mana: %4d/%4d(%4d)     Move: %4d/%4d(%4d) {W|{x\n\r",
    victim->hit,         victim->max_hit,       victim->pcdata->perm_hit,
    victim->mana,        victim->max_mana,      victim->pcdata->perm_mana,
    victim->move,        victim->max_move,      victim->pcdata->perm_move
    );
    send_to_char( buf, ch );

    sprintf( buf, "{W| {xAggDamage: %5d              Wimpy: %5d                Stopped:  %3d  {W|{x\n\r",
    victim->agg_dam, victim->wimpy, victim->stopped);
    send_to_char(buf, ch);

    sprintf( buf, "{W| Hitroll [{DD10 Dice{x]: %5d [{D+%2d{x]       Damroll [{DDamage Mod{x]: %5d [{D+%3d{x] {W|{x\n\r",
      GET_HITROLL(victim), GET_HITROLL(victim)/200,
      GET_DAMROLL(victim), GET_DAMROLL(victim)/50);
    send_to_char(buf,ch);

    sprintf(buf,"{W| {x  Armor    :      Piercing       Bashing       Slashing        Magical   {W|{x\n\r");
    send_to_char(buf,ch);
    sprintf(buf,"{W| {x ROM[{DD10{x]  :      %5d[{D%2d{x]     %5d[{D%2d{x]      %5d[{D%2d{x]      %5d[{D%2d{x]  {W|{x\n\r",
        GET_AC(victim,AC_PIERCE), get_armor_diff(victim, victim, DAM_PIERCE),
        GET_AC(victim,AC_BASH), get_armor_diff(victim, victim, DAM_BASH),
        GET_AC(victim,AC_SLASH), get_armor_diff(victim, victim, DAM_SLASH),
        GET_AC(victim,AC_EXOTIC), get_armor_diff(victim, victim, DAM_ENERGY)
        );
    send_to_char(buf,ch);

    if(victim->pcdata->breed > 0 && victim->pcdata->auspice > 0)
    {
        col = 1;
        send_to_char("{g[-----------------------=====    Garou Info    =====-----------------------]{x\n\r",ch);
        sprintf(buf,"{g|{x Breed: %-8s            Auspice: %-12s    Tribe: %-13s {g|{x\n\r",
            victim->pcdata->breed == LUPUS ? "Lupus" : victim->pcdata->breed == METIS ? "Metis" :
            "Homid", victim->pcdata->auspice == RAGABASH ? "Ragabash" : victim->pcdata->auspice == THEURGE ?
            "Theurge" : victim->pcdata->auspice == PHILODOX ? "Philodox" : victim->pcdata->auspice == GALLIARD ?
            "Galliard" : "Ahroun", capitalize(clan_table[victim->clan].name));
        send_to_char(buf,ch);
        sprintf(buf, "{g| {xForm: %-7s                      Fur Color: %-25s  {g|{x\n\r",
          victim->pcdata->shiftform == LUPUS ? "Lupus" : victim->pcdata->shiftform == HISPO ? "Hispo" :
          victim->pcdata->shiftform == CRINOS ? "Crinos" : victim->pcdata->shiftform == GLABRO ? "Glabro" :
          victim->pcdata->shiftform == HOMID ? "Homid" : "ERROR!", victim->pcdata->garou_fur);
        send_to_char(buf, ch);
        sprintf(buf, "{g| {xPrimal-Urge: %2d           Rage(Temp): %2d (%2d)      Gnosis(Temp): %2d (%2d) {g|{x\n\r",
          victim->pcdata->primal_urge, victim->pcdata->rage[PERM], victim->pcdata->rage[TEMP], victim->pcdata->gnosis[PERM], victim->pcdata->gnosis[TEMP]);
        send_to_char(buf,ch);
        sprintf(buf2,"  Renown Rank: {G[{W%s{G]{g  ",
          victim->pcdata->rank == 1 ? "Cliath" : victim->pcdata->rank == 2 ? "Fostern" : victim->pcdata->rank == 3 ? "Adren" :
          victim->pcdata->rank == 4 ? "Athro" : victim->pcdata->rank == 5 ? "Elder" : "Pup");
        sprintf(buf, "{g+%s+{x\n\r", center(buf2, 74, "-"));
        send_to_char(buf, ch);
        sprintf(buf,"{g| {xGlory(Temp): %2d (%2d)      Honor(Temp): %2d (%2d)     Wisdom(Temp): %2d (%2d) {g|{x\n\r",
          victim->pcdata->renown[GLORY], victim->pcdata->renown[TEMP_GLORY],
          victim->pcdata->renown[HONOR], victim->pcdata->renown[TEMP_HONOR],
          victim->pcdata->renown[WISDOM], victim->pcdata->renown[TEMP_WISDOM]);
        send_to_char(buf, ch);
        send_to_char("{g+---------------------------    Gifts  Known    ---------------------------+{x\n\r",ch);
        for (i = 0; i < MAX_GIFT; i++)
        {
          if (victim->pcdata->gift[i] == 0)
            continue;
          else
          {
            if (col < 2)
              sprintf(buf, "{g|  {x%-32s  ", capitalize(gift_table[victim->pcdata->gift[i]].name));
            else if (col == 2)
            {
              sprintf(buf, "{g|  {x%-32s   {g|{x\n\r", capitalize(gift_table[victim->pcdata->gift[i]].name));
              col = 0;
            }

            add_buf(buffer, buf);
            col++;
          }
        }
        page_to_char(buf_string(buffer), ch);
        if (col == 2)
          send_to_char("{g|                                     |{x\n\r", ch);
    }

    if (!IS_NPC(victim) && victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
    {
      col = 1;
      send_to_char("{r[-----------------------=====   Vampire Info   =====-----------------------]{x\n\r",ch);
        sprintf(buf,"{r|{x Clan: %-15s      Generation: %2d(%2d)   Sire: %s    {r|{x\n\r",
        capitalize(clan_table[victim->clan].name),
        victim->gen,
        victim->pcdata->csgeneration,
        center(victim->sire, 15, " "));
        send_to_char(buf,ch);
        sprintf(buf,"{r|{x Fangs: %s           Childer: %2d              Blood Pool: %2d/%2d {r|{x\n\r",
          IS_AFFECTED(victim, AFF_FANGS) ? "Extended    " : "Concealed   ",
          victim->childer,
          victim->pblood/10,
          victim->max_pblood/10);
        send_to_char(buf,ch);
        send_to_char("{r+----------------------------    Disciplines   ----------------------------+{x",ch);
        for (i = 1; i < MAX_DISC; i++)
        {
          if (victim->pcdata->discipline[i] == 0)
            continue;
          else
          {
            sprintf(buf, " %s %-15s : %d   {r|{x", col == 1 ? "\n\r{r| {x" : "", disc_table[i].name, victim->pcdata->discipline[i]);
            send_to_char(buf, ch);
            col++;
            if (col > 3)
              col = 1;
          }
        }
        sprintf(buf, "%s\n\r", col == 2 ? "{r                        |                        |{x" : col == 3 ? "                        {r|{x" : "");
        send_to_char(buf, ch);
    }

    if (!IS_NPC(victim) && victim->race == race_lookup("ghoul"))
    {
      col = 1;
      send_to_char("{r[-----------------------=====    Ghoul Info    =====-----------------------]{x\n\r",ch);
      sprintf(buf, "{r|{x Domitor:   %s  ", center(victim->vamp_master, 17, " "));
      send_to_char(buf, ch);
      sprintf(buf,"        Domitor Clan: %s {r|{x\n\r", center(capitalize(clan_table[victim->clan].name), 20, " "));
      send_to_char(buf, ch);
      sprintf(buf, "{r|{x Domitor Gen: %s        ", center(victim->pcdata->csgeneration == 4 ? "Methuselah" : victim->pcdata->csgeneration == 5 ? "Fifth" :
      victim->pcdata->csgeneration == 6 ? "Sixth" : victim->pcdata->csgeneration == 7 ? "Seventh" :
      victim->pcdata->csgeneration == 8 ? "Eighth" : victim->pcdata->csgeneration == 9 ? "Ninth" :
      victim->pcdata->csgeneration == 10 ? "Tenth" : victim->pcdata->csgeneration == 11 ? "Eleventh" :
      victim->pcdata->csgeneration == 12 ? "Twelvth" : victim->pcdata->csgeneration == 13 ? "Thirteenth" : "Negligent",12," "));
      send_to_char(buf,ch);
      sprintf(buf,"     Blood Bond:     %s   {r|{x\n\r{r|{x Blood Pool:     %2d/%2d                 Age:             Unknown           {r|{x\n\r",
      center(victim->bonded == 0 ? "None" : victim->bonded == 1 ? "Level 1" : victim->bonded == 2 ? "Level 2" :
      victim->bonded == 3 ? "Level 3" : "Level 3",16," "),
        victim->pblood/10,
        victim->max_pblood/10);
      send_to_char(buf,ch);
      send_to_char("{r+----------------------------    Disciplines   ----------------------------+{x",ch);
      for (i = 1; i < MAX_DISC; i++)
      {
        if (victim->pcdata->discipline[i] == 0)
          continue;
        else
        {
          sprintf(buf, " %s %-15s : %d   {r|{x", col == 1 ? "\n\r{r| {x" : "", disc_table[i].name, victim->pcdata->discipline[i]);
          send_to_char(buf, ch);
          col++;
          if (col > 3)
            col = 1;
        }
      }
      sprintf(buf, "%s\n\r", col == 2 ? "{r                        |                        |{x" : col == 3 ? "                        {r|{x" : "");
      send_to_char(buf, ch);
    }

    if (!IS_NPC(victim) && victim->race == race_lookup("dhampire"))
    {
      col = 1;
      send_to_char("{r[-----------------------=====  Daywalker Info  =====-----------------------]{x\n\r",ch);
        sprintf(buf,"{r| {xClan: %-15s       Fangs: %s        Blood Pool: %2d/%2d{r |{x\n\r",
        capitalize(clan_table[victim->clan].name),
        IS_AFFECTED(victim, AFF_FANGS) ? "Extended    " : "Concealed   ",
        victim->pblood/10,
        victim->max_pblood/10
        );
      send_to_char(buf,ch);
      send_to_char("{r+----------------------------    Disciplines   ----------------------------+{x",ch);
      for (i = 1; i < MAX_DISC; i++)
      {
        if (victim->pcdata->discipline[i] == 0)
          continue;
        else
        {
          sprintf(buf, " %s %-15s : %d   {r|{x", col == 1 ? "\n\r{r| {x" : "", disc_table[i].name, victim->pcdata->discipline[i]);
          send_to_char(buf, ch);
          col++;
          if (col > 3)
            col = 1;
        }
      }
      sprintf(buf, "%s\n\r", col == 2 ? "{r                        |                        |{x" : col == 3 ? "                        {r|{x" : "");
      send_to_char(buf, ch);
    }

    if (victim->avatar > 0)
    {
      col = 1;
      send_to_char("{c[-----------------------=====    Mage Info     =====-----------------------]{x\n\r",ch);
      sprintf(buf, "{c|{x Tradition: %s      Rank:  %s           Avatar:   %d {c|{x\n\r", center(capitalize(tradition_table[victim->tradition].name), 16, " "),
        victim->rank == 1 ? "Apprentice" : victim->rank == 2 ? " Disciple " :
        victim->rank == 8 ? "  Master  " : victim->rank == 9 ? "  Mentor  " : "  Leader  ",
        victim->avatar);
      send_to_char(buf, ch);
      sprintf(buf, "{c| {xMentor:   %s         Apprentice:  %s     {c|{x\n\r", center(victim->sire ? victim->sire : "None", 18, " "),
        center(victim->apprentice ? "None" : victim->apprentice, 18, " "));
        send_to_char(buf,ch);
      sprintf(buf, "{c|{x Arete:   %2d              Quintessence:  %3d/%3d             Paradox: %3d {c|{x\n\r", victim->arete,
        victim->quintessence, victim->max_quintessence, victim->paradox);
      send_to_char(buf,ch);
      send_to_char("{c+---------------------------      Spheres       ---------------------------+{x",ch);
      for (i = 1; i <= MAX_SPHERE; i++)
      {
        if (victim->sphere[i] == 0)
          continue;
        else
        {
          sprintf(buf, " %s %-15s : %d   {c|{x", col == 1 ? "\n\r{c| {x" : "", capitalize(sphere_table[i].name), victim->sphere[i]);
          send_to_char(buf, ch);
          col++;
          if (col > 3)
            col = 1;
        }
      }
      sprintf(buf, "%s\n\r", col == 2 ? "{c                        |                        |{x" : col == 3 ? "                        {c|{x" : "");
      send_to_char(buf, ch);
    }

    if (!IS_NPC(victim) && victim->pcdata->playernotes != NULL)
    {
      send_to_char("{D[-------------------{m========    {WPlayer Notes    {m========{D-------------------]{x\n\r", ch);
      printf_to_char(ch,"%s", victim->pcdata->playernotes);
      send_to_char("{D+-------------------{m--------     {WEnd  Notes     {m--------{D-------------------+{x\n\r", ch);
    }

    send_to_char("{b+----------------------------     {B[ {WFlags {B]    {b----------------------------+{x\n\r",ch);
    sprintf(buf, "{b- {xAct: %s\n\r",act_bit_name(victim->act));
    send_to_char(buf,ch);

    sprintf(buf, "{b- {xAct2: %s\n\r", act2_bit_name(victim->act2));
    sendch(buf, ch);

    if (victim->comm)
    {
        sprintf(buf,"{b- {xComm: %s\n\r",comm_bit_name(victim->comm));
        send_to_char(buf,ch);
    }

    if (victim->imm_flags)
    {
    sprintf(buf, "{b- {xImmune: %s\n\r",imm_bit_name(victim->imm_flags));
    send_to_char(buf,ch);
    }

    if (victim->res_flags)
    {
    sprintf(buf, "{b- {xResist: %s\n\r", imm_bit_name(victim->res_flags));
    send_to_char(buf,ch);
    }

    if (victim->vuln_flags)
    {
    sprintf(buf, "{b- {xVulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
    send_to_char(buf,ch);
    }

    if (victim->affected_by)
    {
    sprintf(buf, "{b- {xAffected by %s\n\r",
        affect_bit_name(victim->affected_by));
    send_to_char(buf,ch);
    }

    if (victim->affected2_by)
    {
         sprintf(buf, "{b- {xAlso affected by %s\n\r",
            affect2_bit_name(victim->affected2_by) );
        send_to_char(buf,ch);
    }

    if(victim->affected != NULL)
      send_to_char("{y[-----------------------=====     Affects      =====-----------------------]{x\n\r",ch);

    for ( paf = victim->affected; paf != NULL; paf = paf->next )
    {
    if (paf->bitvector == 0)
      sprintf(buf2, "%s", "\n\r");
    else
      sprintf(buf2, "\n\r      Associated Bit [{y%s%s{x]\n\r",
        paf->where == TO_RESIST ? "res_" : paf->where == TO_IMMUNE ? "imm_" : paf->where == TO_VULN ? "vuln_" : "",
        paf->where == TO_RESIST ? imm_bit_name(paf->bitvector) : paf->where == TO_VULN ? imm_bit_name(paf->bitvector) :
        paf->where == TO_IMMUNE ? imm_bit_name(paf->bitvector) :
        paf->where == TO_AFFECTS ? affect_bit_name( paf->bitvector ) : affect2_bit_name( paf->bitvector ));
    sprintf( buf,
        " {y'{W%-18s{y'{W(Lvl-%3d){x: %18s (%4d) - %3d %s%s",
        skill_table[(int) paf->type].name,
        paf->level,
        affect_loc_name( paf->location ),
        paf->modifier,
        paf->duration,
        paf->duration == -1 ? "permanently" : paf->duration == 1 ? "tick " : "ticks",
        buf2
        );
    send_to_char( buf, ch );
    }

    return;
}


/* ofind and mfind replaced with vnum, vnum skill also added */

void do_vnum(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *string;

    string = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  vnum obj <name>\n\r",ch);
    send_to_char("  vnum mob <name>\n\r",ch);
    send_to_char("  vnum skill <skill or spell>\n\r",ch);
    return;
    }

    if (!str_cmp(arg,"obj"))
    {
    do_function(ch, &do_ofind, string);
    return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    {
    do_function(ch, &do_mfind, string);
    return;
    }

    if (!str_cmp(arg,"skill") || !str_cmp(arg,"spell"))
    {
    do_function (ch, &do_slookup, string);
    return;
    }
    /* do both */
    do_function(ch, &do_mfind, argument);
    do_function(ch, &do_ofind, argument);
}


void do_mfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Find whom?\n\r", ch );
    return;
    }

    fAll    = FALSE; /* !str_cmp( arg, "all" ); */
    found   = FALSE;
    nMatch  = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
    if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
    {
        nMatch++;
        if ( fAll || is_name( argument, pMobIndex->player_name ) )
        {
        found = TRUE;
        sprintf( buf, "[%5d] %s\n\r",
            pMobIndex->vnum, pMobIndex->short_descr );
        send_to_char( buf, ch );
        }
    }
    }

    if ( !found )
    send_to_char( "No mobiles by that name.\n\r", ch );

    return;
}



void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Find what?\n\r", ch );
    return;
    }

    fAll    = FALSE; /* !str_cmp( arg, "all" ); */
    found   = FALSE;
    nMatch  = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
    if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
    {
        nMatch++;
        if ( fAll || is_name( argument, pObjIndex->name ) )
        {
        found = TRUE;
        sprintf( buf, "[%5d] %s\n\r",
            pObjIndex->vnum, pObjIndex->short_descr );
        send_to_char( buf, ch );
        }
    }
    }

    if ( !found )
    send_to_char( "No objects by that name.\n\r", ch );

    return;
}

void do_mpfind( CHAR_DATA *ch, char *argument )
{
    extern int top_mob_index;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    PROG_LIST *pMobProg;
    int vnum;
    int mpvnum;
    int nMatch;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' || !is_number(arg))
    {
    send_to_char( "Syntax: mpfind <mprog vnum>\n\r", ch );
    return;
    }

    mpvnum = atoi(arg);
    if ( (get_prog_index(mpvnum, PRG_MPROG)) == NULL )
    {
        sendch("That is not a valid mprog vnum.\n\r", ch);
        return;
    }

    found   = FALSE;
    nMatch  = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_mob_index; vnum++ )
    {
        if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( pMobIndex->mprog_flags)
            {
                for (pMobProg = pMobIndex->mprogs; pMobProg != NULL; pMobProg = pMobProg->next)
                {
                    if (pMobProg->vnum == mpvnum)
                    {
                        found = TRUE;
                        sprintf(buf, "%s[%5d] %s\n\r", pMobIndex->count ? "*" : " ", pMobIndex->vnum,
                        pMobIndex->short_descr);
                        sendch(buf, ch);
                    }
                }
            }
        }
    }

    if ( !found )
    send_to_char( "No mobiles found with that mprog.\n\r", ch );

    return;
}


#define LEVEL_IS        0
#define LEVEL_ABOVE     1
#define LEVEL_BELOW     2
#define LEVEL_BETWEEN   3

void do_flagfind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    extern int top_room;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MIL];
    char arg5[MIL];
    BUFFER *buffer;
    AFFECT_DATA *paf;
    OBJ_INDEX_DATA *pObjIndex;
    MOB_INDEX_DATA *pMobIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;
    int nMatch;
    int count = 0;
    bool found;
    int i;
    int affflag;
    bool findflag;
    int tlevel, ulevel, llevel;
    const struct flag_type *table;
    bool foundlevel;
    bool dmgflag = FALSE;

    found   = FALSE;
    nMatch  = 0;
    buffer = new_buf();
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument (argument, arg5 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
        send_to_char( "\n\r", ch);
        send_to_char( "{y+=========================================================================+{x\n\r", ch);
        send_to_char( "{y| {WSyntax is   : {xflagfind <obj/mob/room> <type> <flag>                     {y|\n\r", ch );
        send_to_char( "{y| {RSyntax Note {W: {xSee '{chelp flagfind{x' for special syntax.                   {y|\n\r", ch);
        send_to_char( "{y+=========================================================================+{x\n\r", ch);
        send_to_char( "{y| {WObj types   : {xextra, wear, affect, apply, damage, type,                 {y|\n\r",ch);
        send_to_char( "{y| {W            : {xweapon, special, spell, size, level, cost                 {y|\n\r",ch);
        send_to_char( "{y+-------------------------------------------------------------------------+{x\n\r", ch);
        send_to_char( "{y| {WMob types   : {xlevel, race, special, shop, aff, aff2, act, act2,         {y|\n\r",ch);
        send_to_char( "{y| {W            : {xoff, res, vuln, imm, attr, abil, form, parts, size        {y|\n\r",ch);
        send_to_char( "{y+-------------------------------------------------------------------------+{x\n\r", ch);
        send_to_char( "{y| {WRoom types  : {xsector, room, clan, owner                                 {y|\n\r",ch);
        send_to_char( "{y+=========================================================================+{x\n\r", ch);
        send_to_char( "\n\r", ch);

        return;
    }

    if(!str_prefix(arg1,"mob"))
    {

        if (!str_prefix(arg2, "level"))
        {
            if (is_number(arg3))
            {
                tlevel = LEVEL_IS;
                llevel = atoi(arg3);
            } else if (!str_prefix(arg3, "above"))
            {
                tlevel = LEVEL_ABOVE;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs above which level?\n\r", ch);
                    return;
                } else
                    llevel = atoi(arg4);
            } else if (!str_prefix(arg3, "below"))
            {
                tlevel = LEVEL_BELOW;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs below which level?\n\r", ch);
                    return;
                } else
                    ulevel = atoi(arg4);
            } else if (!str_prefix(arg3, "between"))
            {
                tlevel = LEVEL_BETWEEN;
                if (!is_number(arg4) || !is_number(arg5))
                {
                    sendch("Search for mobs between which levels?\n\r", ch);
                    return;
                } else {
                    if (atoi(arg4) > atoi(arg5))
                    {
                        ulevel = atoi(arg4);
                        llevel = atoi(arg5);
                    } else {
                        ulevel = atoi(arg5);
                        llevel = atoi(arg4);
                    }
                }
            } else {
                sendch("Search for mobs of what level?\n\r", ch);
                return;
            }
        }

        findflag = TRUE;
        if(!str_cmp(arg2, "affect") || !str_cmp(arg2, "aff"))
            table = &affect_flags;
        else if (!str_cmp(arg2, "affect2") || !str_cmp(arg2, "aff2"))
            table = &affect2_flags;
        else if (!str_cmp(arg2, "act"))
            table = &act_flags;
        else if (!str_cmp(arg2, "act2"))
            table = &act2_flags;
        else if (!str_prefix(arg2, "immune"))
            table = &imm_flags;
        else if (!str_prefix(arg2, "resist"))
            table = &res_flags;
        else if (!str_prefix(arg2, "vulnerable"))
            table = &vuln_flags;
        else if (!str_prefix(arg2, "offense"))
            table = &off_flags;
        else if (!str_prefix(arg2, "attributes"))
            table = &attr_flags;
        else if (!str_prefix(arg2, "abilities"))
            table = &abil_flags;
        else if (!str_prefix(arg2, "form"))
            table = &form_flags;
        else if (!str_prefix(arg2, "parts"))
            table = &part_flags;
        else if (!str_prefix(arg2, "size"))
            table = &size_flags;
        else
            findflag = FALSE;

        if (findflag)
        {
            bool bitfound = FALSE;
            for (i = 0; table[i].name != NULL; i++)
                {
                    if (!str_prefix(arg3, table[i].name))
                    {
                        affflag = table[i].bit;
                        bitfound = TRUE;
                }
            }
            if (!bitfound)
                {sprintf(buf, "Bit '%s' not found.\n\r", arg3);
                sendch(buf, ch);
                return;
            }
        }


        for ( vnum = 0; nMatch < top_mob_index; vnum++ )
        {
            if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
            {
                nMatch++;
                if(!str_prefix(arg2, "race") && pMobIndex->race == race_lookup(arg3))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }
                if(!str_prefix(arg2, "shop") && pMobIndex->pShop != NULL)
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "special") && pMobIndex->spec_fun != NULL &&
                    !str_cmp(arg3, spec_name(pMobIndex->spec_fun)))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }

                if (!str_prefix(arg2, "level"))
                {
                    foundlevel = FALSE;
                    switch (tlevel)
                    {
                        case LEVEL_IS:
                            if (pMobIndex->level == llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_ABOVE:
                            if (pMobIndex->level > llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BELOW:
                            if (pMobIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BETWEEN:
                            if (pMobIndex->level > llevel && pMobIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        default: break;
                    }
                    if (foundlevel)
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] <Lvl %3d> %s\n\r",
                        pMobIndex->count ? "*" : " ", count, pMobIndex->vnum, pMobIndex->level, pMobIndex->short_descr );
                        add_buf(buffer,buf);
                        foundlevel = FALSE;
                    }
                }

                if(!str_prefix(arg2, "offense") && IS_SET(pMobIndex->off_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "immune") && IS_SET(pMobIndex->imm_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "resistant") && IS_SET(pMobIndex->res_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "vulnerable") && IS_SET(pMobIndex->vuln_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if((!str_cmp(arg2, "affect") || !str_cmp(arg2, "aff")) && IS_AFFECTED(pMobIndex, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }
                if((!str_cmp(arg2, "affect2") || !str_cmp(arg2, "aff2")) && IS_AFFECTED2(pMobIndex, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_cmp(arg2, "act") && IS_SET(pMobIndex->act, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_cmp(arg2, "act2") && IS_SET(pMobIndex->act2, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "attribute") && IS_SET(pMobIndex->attr_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "abilities") && IS_SET(pMobIndex->abil_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "form") && IS_SET(pMobIndex->form, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "parts") && IS_SET(pMobIndex->parts, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }
                if (!str_prefix(arg2, "size") && (pMobIndex->size == flag_value( size_flags, arg3)))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

            }
        }

    }
    if(!str_prefix(arg1,"obj"))
    {
        if (!str_prefix(arg2, "level"))
        {
            if (is_number(arg3))
            {
                tlevel = LEVEL_IS;
                llevel = atoi(arg3);
            } else if (!str_prefix(arg3, "above"))
            {
                tlevel = LEVEL_ABOVE;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs above which level?\n\r", ch);
                    return;
                } else
                    llevel = atoi(arg4);
            } else if (!str_prefix(arg3, "below"))
            {
                tlevel = LEVEL_BELOW;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs below which level?\n\r", ch);
                    return;
                } else
                    ulevel = atoi(arg4);
            } else if (!str_prefix(arg3, "between"))
            {
                tlevel = LEVEL_BETWEEN;
                if (!is_number(arg4) || !is_number(arg5))
                {
                    sendch("Search for objects between which levels?\n\r", ch);
                    return;
                } else {
                    if (atoi(arg4) > atoi(arg5))
                    {
                        ulevel = atoi(arg4);
                        llevel = atoi(arg5);
                    } else {
                        ulevel = atoi(arg5);
                        llevel = atoi(arg4);
                    }
                }
            } else {
                sendch("Search for objects of what level?\n\r", ch);
                return;
            }
        }

        if (!str_prefix(arg2, "cost"))
        {
            if (is_number(arg3))
            {
                tlevel = LEVEL_IS;
                llevel = atoi(arg3);
            } else if (!str_prefix(arg3, "above"))
            {
                tlevel = LEVEL_ABOVE;
                if (!is_number(arg4))
                {
                    sendch("Search for objects with a cost higher than what value? ({R*{xin silver)\n\r", ch);
                    return;
                } else
                    llevel = atoi(arg4);
            } else if (!str_prefix(arg3, "below"))
            {
                tlevel = LEVEL_BELOW;
                if (!is_number(arg4))
                {
                    sendch("Search for objects with a cost lower than what value? ({R*{xin silver)\n\r", ch);
                    return;
                } else
                    ulevel = atoi(arg4);
            } else if (!str_prefix(arg3, "between"))
            {
                tlevel = LEVEL_BETWEEN;
                if (!is_number(arg4) || !is_number(arg5))
                {
                    sendch("Search for objects with a cost between what values? ({R*{xin silver)\n\r", ch);
                    return;
                } else {
                    if (atoi(arg4) > atoi(arg5))
                    {
                        ulevel = atoi(arg4);
                        llevel = atoi(arg5);
                    } else {
                        ulevel = atoi(arg5);
                        llevel = atoi(arg4);
                    }
                }
            } else {
                sendch("Search for objects of what value? ({R*{xin silver)\n\r", ch);
                return;
            }
        }

        findflag = TRUE;
        if(!str_cmp(arg2, "extra"))
            table = &extra_flags;
        else if (!str_prefix(arg2, "size"))
            table = &size_flags;
        else if (!str_prefix(arg2, "wear"))
            table = &wear_flags;
        else if (!str_prefix(arg2, "type"))
            table = &type_flags;
        else if (!str_prefix(arg2, "apply"))
            table = &apply_flags;
        else if (!str_prefix(arg2, "weapon"))
            table = &weapon_class;
        else if (!str_prefix(arg2, "special"))
            table = &weapon_type2;
        else if (!str_prefix(arg2, "affect"))
            table = &affect_flags;
        else if (!str_prefix(arg2, "damage"))
            dmgflag = TRUE;
        else
            findflag = FALSE;

        if (findflag)
        {
            bool bitfound = FALSE;
            if (dmgflag)
                for (i = 0; attack_table[i].name != NULL; i++)
                {
                    if (is_exact_name(arg3, attack_table[i].name))
                        bitfound = TRUE;
                }
            else
                for (i = 0; table[i].name != NULL; i++)
                {
                    if (is_exact_name(arg3, table[i].name))
                    {
                        affflag = table[i].bit;
                        bitfound = TRUE;
                    }
                }
            if (!bitfound)
            {
                sprintf(buf, "{R**  {xBit name '{B%s{x' not found for [{g%s{x].  {R**{x\n\r", arg3, arg2);
                sendch(buf, ch);
                sprintf(buf, " ------------------\n\r|       Bits       |\n\r ------------------\n\r");
                sendch(buf, ch);
                if (dmgflag)
                    for (i = 0; attack_table[i].name != NULL; i++)
                    {
                        sprintf(buf, "(%2d) {B%s{x\n\r", i, attack_table[i].name);
                        sendch(buf, ch);
                    }
                else
                    for (i = 0; table[i].name != NULL; i++)
                    {
                        sprintf(buf, "(%2d) {B%s{x\n\r", i, table[i].name);
                        sendch(buf, ch);
                    }
                return;
            }
        }


        send_to_char("      <{YLvl{x> [ {gVnum{x] {Bx  #{x : Short Description\n\r", ch);
        send_to_char("      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r", ch);
        for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        {
            if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
            {
                nMatch++;
                if(!str_prefix(arg2,"extra"))
                {
                    if ( is_exact_name( arg3, extra_bit_name( pObjIndex->extra_flags ) ) )
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"wear"))
                {
                    if ( is_exact_name( arg3, wear_bit_name( pObjIndex->wear_flags ) ) )
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"affect"))
                {
                    for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
                    {
                        if(paf->bitvector && !str_prefix(affect_bit_name(paf->bitvector), arg3))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                                count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                    }
                }
                else if(!str_prefix(arg2,"apply"))
                {
                    for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
                    {
                        if(paf->location > APPLY_NONE && !str_prefix(flag_string( apply_flags, paf->location ), arg3))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "%s(%3d) [%5d] (%s: %3d) %s\n\r",
                            pObjIndex->count ? "*" : " ",
                            count,pObjIndex->vnum, flag_string( apply_flags, paf->location ), paf->modifier, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                    }
                }
                else if(!str_prefix(arg2,"damage"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && pObjIndex->value[3] < MAX_DAMAGE_MESSAGE && !str_prefix(attack_table[pObjIndex->value[3]].noun, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"type"))
                {
                    if(!str_prefix(item_name(pObjIndex->item_type),arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"weapon"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && !str_prefix(weapon_class[pObjIndex->value[0]].name, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"special"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && has_weapon_bit(pObjIndex->value[4], arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                            count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"spell"))
                {
                    if((pObjIndex->item_type == ITEM_SCROLL || pObjIndex->item_type == ITEM_POTION ||
                        pObjIndex->item_type == ITEM_PILL) &&
                        ((pObjIndex->value[1] >= 0 && pObjIndex->value[1] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[1]].name, arg3)) ||
                        (pObjIndex->value[2] >= 0 && pObjIndex->value[2] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[2]].name, arg3)) ||
                        (pObjIndex->value[3] >= 0 && pObjIndex->value[3] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[3]].name, arg3)) ||
                        (pObjIndex->value[4] >= 0 && pObjIndex->value[4] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[4]].name, arg3))))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                                count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                    if((pObjIndex->item_type == ITEM_WAND || pObjIndex->item_type == ITEM_STAFF) &&
                        (pObjIndex->value[3] >= 0 && pObjIndex->value[3] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[3]].name, arg3)))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                                count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                }
                else if (!str_prefix(arg2, "size") && (pObjIndex->size == flag_value( size_flags, arg3)))
                {
                    found = TRUE;
                    count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                        count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                    add_buf(buffer,buf);
                }
                else if (!str_prefix(arg2, "level"))
                {
                    foundlevel = FALSE;
                    switch (tlevel)
                    {
                        case LEVEL_IS:
                            if (pObjIndex->level == llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_ABOVE:
                            if (pObjIndex->level > llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BELOW:
                            if (pObjIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BETWEEN:
                            if (pObjIndex->level > llevel && pObjIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        default: break;
                    }
                    if (foundlevel)
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "(%3d) <{Y%3d{x> [{g%5d{x] {Bx %2d{x : %s\n\r",
                        count, pObjIndex->level, pObjIndex->vnum, pObjIndex->count, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                        foundlevel = FALSE;
                    }
                }
                else if (!str_prefix(arg2, "cost"))
                {
                    foundlevel = FALSE;
                    switch (tlevel)
                    {
                        case LEVEL_IS:
                            if (pObjIndex->cost == llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_ABOVE:
                            if (pObjIndex->cost > llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BELOW:
                            if (pObjIndex->cost < ulevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BETWEEN:
                            if (pObjIndex->cost > llevel && pObjIndex->cost < ulevel)
                                foundlevel = TRUE;
                            break;
                        default: break;
                    }
                    if (foundlevel)
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] $%10.10d : %s\n\r",
                        pObjIndex->count ? "*" : " ", count, pObjIndex->vnum, pObjIndex->cost, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                        foundlevel = FALSE;
                    }
                }
            }
        }
    }
/*Sengir added sector types for rooms to flagfind*/
    if (!str_prefix(arg1, "room"))
    {
        for (vnum = 0; nMatch < top_room; vnum++)
        {
            if ((pRoomIndex = get_room_index(vnum)) != NULL)
            {
                nMatch++;
                if(!str_prefix(arg2, "sector"))
                {
                    if (!str_cmp(arg3, flag_string(sector_flags,pRoomIndex->sector_type)))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "room"))
                {
                    if (has_room_bit(pRoomIndex->room_flags, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "clan"))
                {
                    if (room_clan_only(pRoomIndex->clan, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "owner"))
                {
                  if (!str_cmp(pRoomIndex->owner, arg3))
                  {
                    found = TRUE;
                    count++;
                    sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                    add_buf(buffer,buf);
                  }
                }

            }
        }
    }

    if ( !found )
        send_to_char( "Nothing with those flags.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    return;
}

void do_areaflagfind( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char arg4[MIL];
    char arg5[MIL];
    BUFFER *buffer;
    AFFECT_DATA *paf;
    AREA_DATA *pArea;
    OBJ_INDEX_DATA *pObjIndex;
    MOB_INDEX_DATA *pMobIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    int vnum;
    int nMatch;
    int minv, maxv;
    int count = 0;
    bool found;
    int i;
    int affflag;
    bool findflag;
    int tlevel, ulevel, llevel;
    const struct flag_type *table;
    bool foundlevel;

    found   = FALSE;
    nMatch  = 0;
    buffer = new_buf();
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
    argument = one_argument( argument, arg4 );
    argument = one_argument (argument, arg5 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0')
    {
      send_to_char( "\n\r", ch);
      send_to_char( "{y+=========================================================================+{x\n\r", ch);
      send_to_char( "{y| {WSyntax is   : {xareaflagfind <obj/mob/room> <type> <flag>                 {y|\n\r", ch );
      send_to_char( "{y| {RSyntax Note {W: {xSee '{chelp flagfind{x' for special syntax.                   {y|\n\r", ch);
      send_to_char( "{y+=========================================================================+{x\n\r", ch);
      send_to_char( "{y| {WObj types   : {xextra, wear, affect, damage, type, weapon, special, spell {y|\n\r",ch);
      send_to_char( "{y+-------------------------------------------------------------------------+{x\n\r", ch);
      send_to_char( "{y| {WMob types   : {xlevel, race, special, shop, aff, aff2, act, act2,         {y|\n\r",ch);
      send_to_char( "{y| {W            : {xoff, res, vuln, imm, attr, abil, form, parts, size        {y|\n\r",ch);
      send_to_char( "{y+-------------------------------------------------------------------------+{x\n\r", ch);
      send_to_char( "{y| {WRoom types  : {xsector, room, clan, owner                                 {y|\n\r",ch);
      send_to_char( "{y+=========================================================================+{x\n\r", ch);
      send_to_char( "\n\r", ch);
    }

    pArea = ch->in_room->area;

    if (pArea == NULL)
    {
        sendch("Error! Null area!\n\r", ch);
        return;
    }

    minv = pArea->min_vnum;
    maxv = pArea->max_vnum;

    if(!str_prefix(arg1,"mob"))
    {

        if (!str_prefix(arg2, "level"))
        {
            if (is_number(arg3))
            {
                tlevel = LEVEL_IS;
                llevel = atoi(arg3);
            } else if (!str_prefix(arg3, "above"))
            {
                tlevel = LEVEL_ABOVE;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs above which level?\n\r", ch);
                    return;
                } else
                    llevel = atoi(arg4);
            } else if (!str_prefix(arg3, "below"))
            {
                tlevel = LEVEL_BELOW;
                if (!is_number(arg4))
                {
                    sendch("Search for mobs below which level?\n\r", ch);
                    return;
                } else
                    ulevel = atoi(arg4);
            } else if (!str_prefix(arg3, "between"))
            {
                tlevel = LEVEL_BETWEEN;
                if (!is_number(arg4) || !is_number(arg5))
                {
                    sendch("Search for mobs between which levels?\n\r", ch);
                    return;
                } else {
                    if (atoi(arg4) > atoi(arg5))
                    {
                        ulevel = atoi(arg4);
                        llevel = atoi(arg5);
                    } else {
                        ulevel = atoi(arg5);
                        llevel = atoi(arg4);
                    }
                }
            } else {
                sendch("Search for mobs of what level?\n\r", ch);
                return;
            }
        }

        findflag = TRUE;
        if(!str_cmp(arg2, "affect") || !str_cmp(arg2, "aff"))
            table = &affect_flags;
        else if (!str_cmp(arg2, "affect2") || !str_cmp(arg2, "aff2"))
            table = &affect2_flags;
        else if (!str_cmp(arg2, "act"))
            table = &act_flags;
        else if (!str_cmp(arg2, "act2"))
            table = &act2_flags;
        else if (!str_prefix(arg2, "immune"))
            table = &imm_flags;
        else if (!str_prefix(arg2, "resist"))
            table = &res_flags;
        else if (!str_prefix(arg2, "vulnerable"))
            table = &vuln_flags;
        else if (!str_prefix(arg2, "offense"))
            table = &off_flags;
        else if (!str_prefix(arg2, "attributes"))
            table = &attr_flags;
        else if (!str_prefix(arg2, "abilities"))
            table = &abil_flags;
        else if (!str_prefix(arg2, "form"))
            table = &form_flags;
        else if (!str_prefix(arg2, "parts"))
            table = &part_flags;
        else if (!str_prefix(arg2, "size"))
            table = &size_flags;
        else
            findflag = FALSE;

        if (findflag)
        {
            bool bitfound = FALSE;
            for (i = 0; table[i].name != NULL; i++)
                {
                    if (!str_prefix(arg3, table[i].name))
                    {
                        affflag = table[i].bit;
                        bitfound = TRUE;
                }
            }
            if (!bitfound)
                {sprintf(buf, "Bit '%s' not found.\n\r", arg3);
                sendch(buf, ch);
                return;
            }
        }


        for ( vnum = minv; vnum < maxv; vnum++ )
        {
            if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
            {
                nMatch++;
                if(!str_prefix(arg2, "race") && pMobIndex->race == race_lookup(arg3))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }
                if(!str_prefix(arg2, "shop") && pMobIndex->pShop != NULL)
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "special") && pMobIndex->spec_fun != NULL &&
                    !str_cmp(arg3, spec_name(pMobIndex->spec_fun)))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                    add_buf(buffer,buf);
                }

                if (!str_prefix(arg2, "level"))
                {
                    foundlevel = FALSE;
                    switch (tlevel)
                    {
                        case LEVEL_IS:
                            if (pMobIndex->level == llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_ABOVE:
                            if (pMobIndex->level > llevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BELOW:
                            if (pMobIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        case LEVEL_BETWEEN:
                            if (pMobIndex->level > llevel && pMobIndex->level < ulevel)
                                foundlevel = TRUE;
                            break;
                        default: break;
                    }
                    if (foundlevel)
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        pMobIndex->count ? "*" : " ",count,pMobIndex->vnum, pMobIndex->short_descr );
                        add_buf(buffer,buf);
                        foundlevel = FALSE;
                    }
                }

                if(!str_prefix(arg2, "offense") && IS_SET(pMobIndex->off_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "immune") && IS_SET(pMobIndex->imm_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "resistant") && IS_SET(pMobIndex->res_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "vulnerable") && IS_SET(pMobIndex->vuln_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if((!str_cmp(arg2, "affect") || !str_cmp(arg2, "aff")) && IS_AFFECTED(pMobIndex, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }
                if((!str_cmp(arg2, "affect2") || !str_cmp(arg2, "aff2")) && IS_AFFECTED2(pMobIndex, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_cmp(arg2, "act") && IS_SET(pMobIndex->act, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_cmp(arg2, "act2") && IS_SET(pMobIndex->act2, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "attribute") && IS_SET(pMobIndex->attr_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "abilities") && IS_SET(pMobIndex->abil_flags, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "form") && IS_SET(pMobIndex->form, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if(!str_prefix(arg2, "parts") && IS_SET(pMobIndex->parts, affflag))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }

                if (!str_prefix(arg2, "size") && (pMobIndex->size == flag_value( size_flags, arg3)))
                {
                    found = TRUE;
                    count++;
                    sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                    pMobIndex->count ? "*" : " ",count, pMobIndex->vnum, pMobIndex->short_descr);
                    add_buf(buffer,buf);
                }
            }
        }

    }
    if(!str_prefix(arg1,"obj"))
    {
        for ( vnum = minv; vnum < maxv; vnum++ )
        {
            if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
            {
                nMatch++;
                if(!str_prefix(arg2,"extra"))
                {
                    if ( is_exact_name( arg3, extra_bit_name( pObjIndex->extra_flags ) ) )
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"wear"))
                {
                    if ( is_exact_name( arg3, wear_bit_name( pObjIndex->wear_flags ) ) )
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"affect"))
                {
                    for ( paf = pObjIndex->affected; paf != NULL; paf = paf->next )
                    {
                        if(paf->bitvector && !str_prefix(affect_bit_name(paf->bitvector), arg3))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                            get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                            count,pObjIndex->vnum, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                    }
                }
                else if(!str_prefix(arg2,"damage"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && pObjIndex->value[3] < MAX_DAMAGE_MESSAGE && !str_prefix(attack_table[pObjIndex->value[3]].noun, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"type"))
                {
                    if(!str_prefix(item_name(pObjIndex->item_type),arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"weapon"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && !str_prefix(weapon_class[pObjIndex->value[0]].name, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"special"))
                {
                    if(pObjIndex->item_type == ITEM_WEAPON && has_weapon_bit(pObjIndex->value[4], arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                        get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                        count,pObjIndex->vnum, pObjIndex->short_descr );
                        add_buf(buffer,buf);
                    }
                }
                else if(!str_prefix(arg2,"spell"))
                {
                    if((pObjIndex->item_type == ITEM_SCROLL || pObjIndex->item_type == ITEM_POTION ||
                        pObjIndex->item_type == ITEM_PILL) &&
                        ((pObjIndex->value[1] >= 0 && pObjIndex->value[1] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[1]].name, arg3)) ||
                        (pObjIndex->value[2] >= 0 && pObjIndex->value[2] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[2]].name, arg3)) ||
                        (pObjIndex->value[3] >= 0 && pObjIndex->value[3] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[3]].name, arg3)) ||
                        (pObjIndex->value[4] >= 0 && pObjIndex->value[4] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[4]].name, arg3))))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                            get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                            count,pObjIndex->vnum, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                    if((pObjIndex->item_type == ITEM_WAND || pObjIndex->item_type == ITEM_STAFF) &&
                        (pObjIndex->value[3] >= 0 && pObjIndex->value[3] < MAX_SKILL &&
                        !str_prefix(skill_table[pObjIndex->value[3]].name, arg3)))
                        {
                            found = TRUE;
                            count++;
                            sprintf( buf, "%s(%3d) [%5d] %s\n\r",
                            get_obj_world(ch,pObjIndex->name, TRUE) ? "*" : " ",
                            count,pObjIndex->vnum, pObjIndex->short_descr );
                            add_buf(buffer,buf);
                        }
                }
            }
        }
    }
/*Sengir added sector types for rooms to flagfind*/
    if (!str_prefix(arg1, "room"))
    {
        for (vnum = minv; vnum < maxv; vnum++)
        {
            if ((pRoomIndex = get_room_index(vnum)) != NULL)
            {
                nMatch++;
                if(!str_prefix(arg2, "sector"))
                {
                    if (!str_cmp(arg3, flag_string(sector_flags,pRoomIndex->sector_type)))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "room"))
                {
                    if (has_room_bit(pRoomIndex->room_flags, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "clan"))
                {
                    if (room_clan_only(pRoomIndex->clan, arg3))
                    {
                        found = TRUE;
                        count++;
                        sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                        add_buf(buffer,buf);
                    }
                }

                if(!str_prefix(arg2, "owner"))
                {
                  if (!str_cmp(pRoomIndex->owner, arg3))
                  {
                    found = TRUE;
                    count++;
                    sprintf( buf, " (%3d) [%5d] %s\n\r", count, pRoomIndex->vnum, pRoomIndex->name);
                    add_buf(buffer,buf);
                  }
                }

            }
        }
    }

    if ( !found )
        send_to_char( "Nothing with those flags.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    return;
}

void do_owhere(CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    BUFFER *buffer;
    OBJ_DATA *obj;
    OBJ_DATA *in_obj;
    bool found;
    int number = 0, max_found;

    found = FALSE;
    number = 0;
    max_found = 200;

    buffer = new_buf();

    if (argument[0] == '\0')
    {
    send_to_char("Find what?\n\r",ch);
    return;
    }

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( !can_see_obj( ch, obj ) || !is_name( argument, obj->name )
        ||   ch->level < obj->level)
            continue;

        found = TRUE;
        number++;

        for ( in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj )
            ;

        if ( in_obj->carried_by != NULL && can_see(ch,in_obj->carried_by)
    &&   in_obj->carried_by->in_room != NULL)
            sprintf( buf, "%3d) %s is carried by %s [Room %d]\n\r",
                number, obj->short_descr,PERS(in_obj->carried_by, ch, TRUE),
        in_obj->carried_by->in_room->vnum );
        else if (in_obj->in_room != NULL && can_see_room(ch,in_obj->in_room))
            sprintf( buf, "%3d) %s is in %s [Room %d]\n\r",
                number, obj->short_descr,in_obj->in_room->name,
        in_obj->in_room->vnum);
    else
            sprintf( buf, "%3d) %s is somewhere\n\r",number, obj->short_descr);

        buf[0] = UPPER(buf[0]);
        add_buf(buffer,buf);

        if (number >= max_found)
            break;
    }

    if ( !found )
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);
}


void do_mwhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    bool found;
    int count = 0;

    if ( argument[0] == '\0' )
    {
    DESCRIPTOR_DATA *d;

    /* show characters logged */

    buffer = new_buf();
    for (d = descriptor_list; d != NULL; d = d->next)
    {
        if (d->character != NULL && d->connected == CON_PLAYING
        &&  d->character->in_room != NULL && can_see(ch,d->character)
        &&  can_see_room(ch,d->character->in_room))
        {
        victim = d->character;
        count++;
        if (d->original != NULL)
            sprintf(buf,"%3d) %s (in the body of %s) is in %s [%d]\n\r",
            count, d->original->name,victim->short_descr,
            victim->in_room->name,victim->in_room->vnum);
        else
            sprintf(buf,"%3d) %s is in %s [%d]\n\r",
            count, victim->name,victim->in_room->name,
            victim->in_room->vnum);
        add_buf(buffer,buf);
        }
    }

        page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
    }

    found = FALSE;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {

    if (victim != NULL
    &&  victim->in_room != NULL
    &&  is_name( argument, victim->name )
    &&  can_see(ch,victim)
    &&  can_see_room(ch,victim->in_room))
    {
        found = TRUE;
        count++;
        sprintf( buf, "%3d) [%5d] %-28s [%5d] %s\n\r", count,
        IS_NPC(victim) ? victim->pIndexData->vnum : 0,
        IS_NPC(victim) ? victim->short_descr : victim->name,
        victim->in_room->vnum,
        victim->in_room->name );
        add_buf(buffer,buf);
    }
    }

    if ( !found )
    act( "You didn't find any $T.", ch, NULL, argument, TO_CHAR );
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);

    return;
}



void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}



void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d,*d_next;
    CHAR_DATA *vch;

    do_asave(ch, "changed");
    send_to_char("\n\r{YAreas saved successfully.  Proceeding to reboot.{x\n\r", ch);

    if (ch->invis_level < LEVEL_HERO)
    {
        sprintf( buf, "{yReboot by {M%s{y.{x\n\r\n\r{CWe should be right back up in half a minute.{x", ch->name );
        do_function(ch, &do_echo, buf );
    }

    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next )
    {
    d_next = d->next;
    vch = d->original ? d->original : d->character;
    if (vch != NULL)
        save_char_obj(vch);
        close_socket(d);
    }

    return;
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}

void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    DESCRIPTOR_DATA *d,*d_next;
    CHAR_DATA *vch;

    if (ch->invis_level < LEVEL_HERO)
    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf, TRUE);
    strcat( buf, "\n\r" );
    if (ch->invis_level < LEVEL_HERO)
    {
        do_function(ch, &do_echo, buf );
    }
    merc_down = TRUE;
    for ( d = descriptor_list; d != NULL; d = d_next)
    {
    d_next = d->next;
    vch = d->original ? d->original : d->character;
    if (vch != NULL)
        save_char_obj(vch);
    close_socket(d);
    }
    return;
}

void do_protect( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (argument[0] == '\0')
    {
    send_to_char("Protect whom from snooping?\n\r",ch);
    return;
    }

    if ((victim = get_char_world(ch,argument)) == NULL)
    {
    send_to_char("You can't find them.\n\r",ch);
    return;
    }

    if (IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
    act_new("$N is no longer snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    send_to_char("Your snoop-proofing was just removed.\n\r",victim);
    REMOVE_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
    else
    {
    act_new("$N is now snoop-proof.",ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    send_to_char("You are now immune to snooping.\n\r",victim);
    SET_BIT(victim->comm,COMM_SNOOP_PROOF);
    }
}



void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Snoop whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim->desc == NULL )
    {
    send_to_char( "No descriptor to snoop.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "Cancelling all snoops.\n\r", ch );
    wiznet("$N stops being such a snoop.",
        ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->snoop_by == ch->desc )
        d->snoop_by = NULL;
    }
    return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
    send_to_char( "Busy already.\n\r", ch );
    return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room
    &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That character is in a private room.\n\r",ch);
        return;
    }

    if ( get_trust( victim ) > get_trust( ch )
    ||   IS_SET(victim->comm,COMM_SNOOP_PROOF))
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if ( ch->desc != NULL )
    {
    for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
    {
        if ( d->character == victim || d->original == victim )
        {
        send_to_char( "No snoop loops.\n\r", ch );
        return;
        }
    }
    }

    victim->desc->snoop_by = ch->desc;
    sprintf(buf,"$N starts snooping on %s",
    (IS_NPC(ch) ? victim->short_descr : victim->name));
    wiznet(buf,ch,NULL,WIZ_SNOOPS,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Switch into whom?\n\r", ch );
    return;
    }

    if ( ch->desc == NULL )
    return;

    if ( ch->desc->original != NULL )
    {
    send_to_char( "You are already switched.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "Ok.\n\r", ch );
    return;
    }

    if (!IS_NPC(victim) && !IS_IMP(ch))
    {
    send_to_char("You can only switch into mobiles.\n\r",ch);
    return;
    }

    if (!is_room_owner(ch,victim->in_room) && ch->in_room != victim->in_room
    &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
    send_to_char("That character is in a private room.\n\r",ch);
    return;
    }

    if ( victim->desc != NULL )
    {
    send_to_char( "Character in use.\n\r", ch );
    return;
    }

    sprintf(buf,"$N switches into %s",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    if (victim->pcdata == NULL)
        victim->pcdata      = ch->pcdata;
    ch->desc            = NULL;
    /* change communications to match */
    if (IS_NPC(victim) && ch->prompt != NULL)
        victim->prompt = str_dup(ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    send_to_char( "Ok.\n\r", victim );
    return;
}



void do_return( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    if ( ch->desc == NULL )
    return;

    if ( ch->desc->original == NULL )
    {
        if(IS_IMMORTAL(ch)) send_to_char( "You aren't switched.\n\r", ch );
    return;
    }

    send_to_char(
"You return to your original body. Type replay to see any missed tells.\n\r",
    ch );
    if (IS_NPC(ch) && ch->prompt != NULL)
    {
    free_string(ch->prompt);
    ch->prompt = NULL;
    }

    sprintf(buf,"$N returns from %s.",ch->short_descr);
    wiznet(buf,ch->desc->original,0,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc                  = NULL;
    if (IS_NPC(ch))
        ch->pcdata                = NULL;
    return;
}

/* trust levels for load and clone */
bool obj_check (CHAR_DATA *ch, OBJ_DATA *obj)
{
    if (IS_TRUSTED(ch,GOD)
    || (IS_TRUSTED(ch,IMMORTAL) && obj->level <= 20 && obj->cost <= 1000)
    || (IS_TRUSTED(ch,DEMI)     && obj->level <= 10 && obj->cost <= 500)
    || (IS_TRUSTED(ch,ANGEL)    && obj->level <=  5 && obj->cost <= 250)
    || (IS_TRUSTED(ch,AVATAR)   && obj->level ==  0 && obj->cost <= 100))
    return TRUE;
    else
    return FALSE;
}

/* for clone, to insure that cloning goes many levels deep */
void recursive_clone(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *clone)
{
    OBJ_DATA *c_obj, *t_obj;


    for (c_obj = obj->contains; c_obj != NULL; c_obj = c_obj->next_content)
    {
    if (obj_check(ch,c_obj))
    {
        t_obj = create_object(c_obj->pIndexData,0);
        clone_object(c_obj,t_obj);
        obj_to_obj(t_obj,clone);
        recursive_clone(ch,c_obj,t_obj);
    }
    }
}

/* command that is similar to load */
void do_clone(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char *rest;
    CHAR_DATA *mob;
    OBJ_DATA  *obj;

    rest = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
    send_to_char("Clone what?\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"object"))
    {
    mob = NULL;
    obj = get_obj_here(ch,NULL, rest);
    if (obj == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    }
    else if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
    obj = NULL;
    mob = get_char_room(ch, NULL, rest);
    if (mob == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    }
    else /* find both */
    {
    mob = get_char_room(ch,NULL,argument);
    obj = get_obj_here(ch,NULL, argument);
    if (mob == NULL && obj == NULL)
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    }

    /* clone an object */
    if (obj != NULL)
    {
    OBJ_DATA *clone;

    if (!obj_check(ch,obj))
    {
        send_to_char(
        "Your powers are not great enough for such a task.\n\r",ch);
        return;
    }

    clone = create_object(obj->pIndexData,0);
    clone_object(obj,clone);
    if (obj->carried_by != NULL)
        obj_to_char(clone,ch);
    else
        obj_to_room(clone,ch->in_room);
    recursive_clone(ch,obj,clone);

    act("$n has created $p.",ch,clone,NULL,TO_ROOM);
    act("You clone $p.",ch,clone,NULL,TO_CHAR);
    wiznet("$N clones $p.",ch,clone,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    return;
    }
    else if (mob != NULL)
    {
    CHAR_DATA *clone;
    OBJ_DATA *new_obj;
    char buf[MAX_STRING_LENGTH];

    if (!IS_NPC(mob))
    {
        send_to_char("You can only clone mobiles.\n\r",ch);
        return;
    }

    if ((mob->level > 20 && !IS_TRUSTED(ch,GOD))
    ||  (mob->level > 10 && !IS_TRUSTED(ch,IMMORTAL))
    ||  (mob->level >  5 && !IS_TRUSTED(ch,DEMI))
    ||  (mob->level >  0 && !IS_TRUSTED(ch,ANGEL))
    ||  !IS_TRUSTED(ch,AVATAR))
    {
        send_to_char(
        "Your powers are not great enough for such a task.\n\r",ch);
        return;
    }

    clone = create_mobile(mob->pIndexData);
    clone_mobile(mob,clone);

    for (obj = mob->carrying; obj != NULL; obj = obj->next_content)
    {
        if (obj_check(ch,obj))
        {
        new_obj = create_object(obj->pIndexData,0);
        clone_object(obj,new_obj);
        recursive_clone(ch,obj,new_obj);
        obj_to_char(new_obj,clone);
        new_obj->wear_loc = obj->wear_loc;
        }
    }
    char_to_room(clone,ch->in_room);
        act("$n has created $N.",ch,NULL,clone,TO_ROOM);
        act("You clone $N.",ch,NULL,clone,TO_CHAR);
    sprintf(buf,"$N clones %s.",clone->short_descr);
    wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
        return;
    }
}

/* RT to replace the two load commands */

void do_load(CHAR_DATA *ch, char *argument )
{
   char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  load mob <vnum>\n\r",ch);
    send_to_char("  load obj <vnum> <level>\n\r",ch);
    return;
    }

    if (!str_cmp(arg,"mob") || !str_cmp(arg,"char"))
    {
    do_function(ch, &do_mload, argument);
    return;
    }

    if (!str_cmp(arg,"obj"))
    {
    do_function(ch, &do_oload, argument);
    return;
    }
    /* echo syntax */
    do_function(ch, &do_load, "");
}


void do_mload( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
    send_to_char( "Syntax: load mob <vnum>.\n\r", ch );
    return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
    send_to_char( "No mob has that vnum.\n\r", ch );
    return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    act( "$n has created $N!", ch, NULL, victim, TO_ROOM );
    sprintf(buf,"$N loads %s.",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH] ,arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA *obj;
    int level;

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number(arg1))
    {
    send_to_char( "Syntax: load obj <vnum> <level>.\n\r", ch );
    return;
    }

    level = get_trust(ch); /* default */

    if ( arg2[0] != '\0')  /* load with a level */
    {
    if (!is_number(arg2))
        {
      send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
      return;
    }
        level = atoi(arg2);
        if (level < 0 || level > get_trust(ch))
    {
      send_to_char( "Level must be be between 0 and your level.\n\r",ch);
      return;
    }
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
    send_to_char( "No object has that vnum.\n\r", ch );
    return;
    }

    obj = create_object( pObjIndex, level );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    obj_to_char( obj, ch );
    else
    obj_to_room( obj, ch->in_room );
    act( "$n has created $p!", ch, obj, NULL, TO_ROOM );
    wiznet("$N loads $p.",ch,obj,WIZ_LOAD,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}



void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    DESCRIPTOR_DATA *d;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    /* 'purge' */
    CHAR_DATA *vnext;
    OBJ_DATA  *obj_next;

    for ( victim = ch->in_room->people; victim != NULL; victim = vnext )
    {
        vnext = victim->next_in_room;
        if ( IS_NPC(victim) && !IS_SET(victim->act,ACT_NOPURGE)
        &&   victim != ch /* safety precaution */ )
        extract_char( victim, TRUE );
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if (!IS_OBJ_STAT(obj,ITEM_NOPURGE))
          extract_obj( obj );
    }

    act( "$n purges the room!", ch, NULL, NULL, TO_ROOM);
    send_to_char( "Ok.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( !IS_NPC(victim) )
    {

    if (ch == victim)
    {
      send_to_char("Ho ho ho.\n\r",ch);
      return;
    }

    if (get_trust(ch) <= get_trust(victim))
    {
      send_to_char("Maybe that wasn't a good idea...\n\r",ch);
      sprintf(buf,"%s tried to purge you!\n\r",ch->name);
      send_to_char(buf,victim);
      return;
    }

    act("$n disintegrates $N.",ch,0,victim,TO_NOTVICT);

        if (victim->level > 1)
        save_char_obj( victim );
        d = victim->desc;
        extract_char( victim, TRUE );
        if ( d != NULL )
          close_socket( d );

    return;
    }

    act( "$n purges $N.", ch, NULL, victim, TO_NOTVICT );
    extract_char( victim, TRUE );
    return;
}



void do_advance( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;
    int iLevel;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
    send_to_char( "Syntax: advance <char> <level>.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "That player is not here.\n\r", ch);
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( ( level = atoi( arg2 ) ) < 1 || level > MAX_LEVEL )
    {
    sprintf(buf,"Level must be 1 to %d.\n\r", MAX_LEVEL);
    send_to_char(buf, ch);
    return;
    }

    if ( level > get_trust( ch ) )
    {
    send_to_char( "Limited to your trust level.\n\r", ch );
    return;
    }

    if(level <= victim->level)
    {
        send_to_char("Cannot lower a player's level.\n\r",ch);
        return;
    }

    /*
     * Lower level:
     *   Reset to level 1.
     *   Then raise again.
     *   Currently, an imp can lower another imp.
     *   -- Swiftest
     */
    if ( level <= victim->level )
    {
/*        int temp_prac; */

    send_to_char( "Lowering a player's level!\n\r", ch );
    send_to_char( "**** OOOOHHHHHHHHHH  NNNNOOOO ****\n\r", victim );
/*  temp_prac = victim->practice; */
    victim->practice = 0;
    victim->train    = 0;
    victim->level    = 1;
    victim->exp      = exp_per_level(victim,victim->pcdata->points);
    victim->max_hit  = 10;
    victim->max_mana = 100;
    victim->max_move = 100;
    victim->hit      = victim->max_hit;
    victim->mana     = victim->max_mana;
    victim->move     = victim->max_move;
    victim->agg_dam  = 0;
    advance_level( victim, TRUE );
/*  victim->practice = temp_prac; */
    }
    else
    {
    send_to_char( "Raising a player's level!\n\r", ch );
    send_to_char( "**** OOOOHHHHHHHHHH  YYYYEEEESSS ****\n\r", victim );
    }

    for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
    victim->level += 1;
    advance_level( victim,TRUE);
    }
    sprintf(buf,"You are now level %d.\n\r",victim->level);
    send_to_char(buf,victim);
    victim->exp   = exp_per_level(victim,victim->pcdata->points)
          * UMAX( 1, victim->level );
    if (victim->trust <= victim->level)
        victim->trust = 0;
    save_char_obj(victim);
/*if(victim->level > 100)
        write_wizlist(ch, victim); */
    return;
}



void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
    send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "That player is not here.\n\r", ch);
    return;
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL )
    {
    sprintf(buf, "Level must be 0 (reset) or 1 to %d.\n\r",MAX_LEVEL);
    send_to_char(buf, ch);
    return;
    }

    if ( level > get_trust( ch ) )
    {
    send_to_char( "Limited to your trust.\n\r", ch );
    return;
    }

    victim->trust = level;
    return;
}

void restore_one(CHAR_DATA *ch, CHAR_DATA *victim, bool show) {
// Restores one char. Called recursively from do_restore to restore room/all

        affect_strip(victim,gsn_poison);
        affect_strip(victim,gsn_blindness);
        affect_strip(victim,gsn_sleep);
        affect_strip(victim,gsn_curse);
        affect_strip(victim,gsn_vamp_frenzy);
        affect_strip(victim,gsn_garou_frenzy);
        affect_strip(victim,gsn_slow);
        affect_strip(victim,gsn_torpor);
        affect_strip(victim,gsn_quietus_bloodcurse);
        affect_strip(victim,gsn_quietus_weakness);
        affect_strip(victim,gsn_regen);
        affect_strip(victim,gsn_forget);
        affect_strip(victim,gsn_laryngitis);
        affect_strip(victim,gsn_deafened);
        affect_strip(victim,gsn_botched_presence);
        affect_strip(victim,gsn_bleeding);
        affect_strip(victim,gsn_nocturne);
        affect_strip(victim,gsn_armsoftheabyss);
        affect_strip(victim,gsn_trappedingauntlet);
        affect_strip(victim,gsn_astrallylost);
        affect_strip(victim, gsn_weaken);

        if (is_affected(victim, gsn_shadowplay) && get_affect_level(victim, gsn_shadowplay) != 0)
          affect_strip(victim, gsn_shadowplay);
        if (is_affected(victim, gsn_glower) && get_affect_level(victim, gsn_glower) == 0)
          affect_strip(victim, gsn_glower);

        victim->agg_dam = 0;
        victim->hit     = victim->max_hit;
        victim->mana    = victim->max_mana;
        victim->move    = victim->max_move;
        victim->pblood  = victim->max_pblood;
        victim->quintessence = victim->max_quintessence;
        victim->paradox = 0;

        if (!IS_NPC(victim)) {
            if (victim->race == race_lookup("garou")) {
                    victim->pcdata->rage[TEMP] = victim->pcdata->rage[PERM];
                    victim->pcdata->gnosis[TEMP] = victim->pcdata->gnosis[PERM];
            }
            victim->cswillpower = victim->csmax_willpower;
        }

        update_pos( victim);
        if (victim->in_room != NULL && show)
                act("$n has restored you.",ch,NULL,victim,TO_VICT);
}

void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH], arg2[MIL];
    CHAR_DATA *victim;
    CHAR_DATA *vch;
    DESCRIPTOR_DATA *d;
    bool show;
    show = TRUE;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2);

    if (!str_cmp(arg2, "hide"))
        show = FALSE;

    if (arg[0] == '\0' || !str_cmp(arg,"room"))
    {
    /* cure room */
        for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
            if (IS_NPC(vch))
                continue;

            restore_one(ch, vch, show);
        }

        sprintf(buf,"$N restored room %d.",ch->in_room->vnum);
        wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
        send_to_char("Room restored.\n\r",ch);
        return;

    }
/*cindi changed the restore all to 105*/
    if ( get_trust(ch) >=  MAX_LEVEL - 6 && !str_cmp(arg,"all"))
    {
    /* cure all */

        for (d = descriptor_list; d != NULL; d = d->next)
        {
        victim = d->character;

        if (victim == NULL || IS_NPC(victim))
        continue;

        restore_one(ch, victim, show);
        }
    send_to_char("All active players restored.\n\r",ch);
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    restore_one(ch, victim, show);
    sprintf(buf,"$N restored %s",
    IS_NPC(victim) ? victim->short_descr : victim->name);
    wiznet(buf,ch,NULL,WIZ_RESTORE,WIZ_SECURE,get_trust(ch));
    send_to_char( "Ok.\n\r", ch );
    return;
}


void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Freeze whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
    REMOVE_BIT(victim->act, PLR_FREEZE);
    send_to_char( "You can play again.\n\r", victim );
    send_to_char( "FREEZE removed.\n\r", ch );
    sprintf(buf,"$N thaws %s.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
    SET_BIT(victim->act, PLR_FREEZE);
    send_to_char( "You can't do ANYthing!\n\r", victim );
    send_to_char( "FREEZE set.\n\r", ch );
    sprintf(buf,"$N puts %s in the deep freeze.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    save_char_obj( victim );

    return;
}



void do_log( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Log whom?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
    if ( fLogAll )
    {
        fLogAll = FALSE;
        send_to_char( "Log ALL off.\n\r", ch );
    }
    else
    {
        fLogAll = TRUE;
        send_to_char( "Log ALL on.\n\r", ch );
    }
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    /*
     * No level check, gods can log anyone.
     */
    if ( IS_SET(victim->act, PLR_LOG) )
    {
    REMOVE_BIT(victim->act, PLR_LOG);
    send_to_char( "LOG removed.\n\r", ch );
    }
    else
    {
    SET_BIT(victim->act, PLR_LOG);
    send_to_char( "LOG set.\n\r", ch );
    }

    return;
}

void do_debug( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Debug what?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "on" ) )
    {
    if ( Debug )
    {
        send_to_char( "Debug already on!\n\r", ch );
    }
    else
    {
        Debug = TRUE;
        send_to_char( "Debug on!\n\r", ch );
    }
    return;
    }

    if ( !str_cmp( arg, "off" ) )
    {
    if ( !Debug )
    {
        send_to_char( "Debug already off!\n\r", ch );
    }
    else
    {
        Debug = FALSE;
        send_to_char( "Debug off!\n\r", ch );
    }
    return;
    }

    return;
}


void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Noemote whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }


    if ( get_trust( victim ) >= get_trust( ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if ( IS_SET(victim->comm, COMM_NOEMOTE) )
    {
    REMOVE_BIT(victim->comm, COMM_NOEMOTE);
    send_to_char( "You can emote again.\n\r", victim );
    send_to_char( "NOEMOTE removed.\n\r", ch );
    sprintf(buf,"$N restores emotes to %s.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
    SET_BIT(victim->comm, COMM_NOEMOTE);
    send_to_char( "You can't emote!\n\r", victim );
    send_to_char( "NOEMOTE set.\n\r", ch );
    sprintf(buf,"$N revokes %s's emotes.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_noshout( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Noshout whom?\n\r",ch);
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if ( IS_SET(victim->comm, COMM_NOSHOUT) )
    {
    REMOVE_BIT(victim->comm, COMM_NOSHOUT);
    send_to_char( "You can shout again.\n\r", victim );
    send_to_char( "NOSHOUT removed.\n\r", ch );
    sprintf(buf,"$N restores shouts to %s.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
    SET_BIT(victim->comm, COMM_NOSHOUT);
    send_to_char( "You can't shout!\n\r", victim );
    send_to_char( "NOSHOUT set.\n\r", ch );
    sprintf(buf,"$N revokes %s's shouts.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Notell whom?", ch );
    return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
    send_to_char( "You failed.\n\r", ch );
    return;
    }

    if ( IS_SET(victim->comm, COMM_NOTELL) )
    {
    REMOVE_BIT(victim->comm, COMM_NOTELL);
    send_to_char( "You can tell again.\n\r", victim );
    send_to_char( "NOTELL removed.\n\r", ch );
    sprintf(buf,"$N restores tells to %s.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }
    else
    {
    SET_BIT(victim->comm, COMM_NOTELL);
    send_to_char( "You can't tell!\n\r", victim );
    send_to_char( "NOTELL set.\n\r", ch );
    sprintf(buf,"$N revokes %s's tells.",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    }

    return;
}



void do_peace( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;

    for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
    {
    if ( rch->fighting != NULL )
        stop_fighting( rch, TRUE );
    if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE))
        REMOVE_BIT(rch->act,ACT_AGGRESSIVE);
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    extern bool wizlock;
    wizlock = !wizlock;

    if ( wizlock )
    {
    wiznet("$N has wizlocked the game.",ch,NULL,0,0,0);
    send_to_char( "Game wizlocked.\n\r", ch );
    }
    else
    {
    wiznet("$N removes wizlock.",ch,NULL,0,0,0);
    send_to_char( "Game un-wizlocked.\n\r", ch );
    }

    return;
}

/* RT anti-newbie code */

void do_newlock( CHAR_DATA *ch, char *argument )
{
    extern bool newlock;
    newlock = !newlock;

    if ( newlock )
    {
    wiznet("$N locks out new characters.",ch,NULL,0,0,0);
        send_to_char( "New characters have been locked out.\n\r", ch );
    }
    else
    {
    wiznet("$N allows new characters back in.",ch,NULL,0,0,0);
        send_to_char( "Newlock removed.\n\r", ch );
    }

    return;
}


void do_slookup( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int sn;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Lookup which skill or spell?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[sn].name == NULL )
        break;
        sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
        sn, skill_table[sn].slot, skill_table[sn].name );
        send_to_char( buf, ch );
    }
    }
    else
    {
    if ( ( sn = skill_lookup( arg ) ) < 0 )
    {
        send_to_char( "No such skill or spell.\n\r", ch );
        return;
    }

    sprintf( buf, "Sn: %3d  Slot: %3d  Skill/spell: '%s'\n\r",
        sn, skill_table[sn].slot, skill_table[sn].name );
    send_to_char( buf, ch );
    }

    return;
}

/* RT set replaces sset, mset, oset, and rset */

void do_set( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if (arg[0] == '\0')
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  set mob   <name> <field> <value>\n\r",ch);
    send_to_char("  set obj   <name> <field> <value>\n\r",ch);
    send_to_char("  set room  <room> <field> <value>\n\r",ch);
      send_to_char("  set skill <name> <spell or skill> <value>\n\r",ch);
      send_to_char("  set time hour/day/month/year <value>\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"mobile") || !str_prefix(arg,"character"))
    {
    do_function(ch, &do_mset, argument);
    return;
    }

    if (!str_prefix(arg,"skill") || !str_prefix(arg,"spell"))
    {
    do_function(ch, &do_sset, argument);
    return;
    }

    if (!str_prefix(arg,"object"))
    {
    do_function(ch, &do_oset, argument);
    return;
    }

    if (!str_prefix(arg,"room"))
    {
    do_function(ch, &do_rset, argument);
    return;
    }
    if (!str_prefix(arg,"time"))
    {
    do_function(ch, &do_settime, argument);
    return;
    }
    if (!str_prefix(arg, "lock"))
    {
    do_function(ch, &do_lockset, argument);
    return;
    }
    /* echo syntax */
    do_function(ch, &do_set, "");
}

void do_lockset( CHAR_DATA *ch, char *argument)
{
char arg1 [MAX_INPUT_LENGTH];
char arg2 [MAX_INPUT_LENGTH];
char buf  [MAX_INPUT_LENGTH];
CHAR_DATA *victim;
int value;

argument = one_argument( argument, arg1 );
argument = one_argument( argument, arg2 );

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
    send_to_char("Syntax:\n\r set lock <name> <1 or 0>\n\r0 = unlocked\n\r1 = locked\n\r",ch);
    return;
    }

    value = atoi( arg2 );
    if ( value < 0 || value > 1 )
    {
    send_to_char( "Value range is 0 or 1.\n\r", ch );
    return;
    }

    if (value == 1)
    {
    victim->cslock = 1;
    sprintf(buf,"%s has had their CS locked.\n\r", victim->name);
    send_to_char(buf,ch);
    sprintf(buf,"%s has locked your CS.\n\r",ch->name);
    send_to_char(buf,ch);
    return;
    }

   if (value == 0)
   {
   victim->cslock = 0;
   sprintf(buf,"%s has had their CS unlocked.\n\r", victim->name);
   send_to_char(buf,ch);
   sprintf(buf,"%s has unlocked your CS.\n\r",ch->name);
   send_to_char(buf,victim);
   return;
   }
}

void do_sset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int value;
    int sn;
    bool fAll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
    send_to_char( "Syntax:\n\r",ch);
    send_to_char( "  set skill <name> <spell or skill> <value>\n\r", ch);
    send_to_char( "  set skill <name> all <value>\n\r",ch);
    send_to_char("   (use the name of the skill, not the number)\n\r",ch);
    return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_NPC(victim) )
    {
    send_to_char( "Not on NPC's.\n\r", ch );
    return;
    }

    fAll = !str_cmp( arg2, "all" );
    sn   = 0;
    if ( !fAll && ( sn = skill_lookup( arg2 ) ) < 0 )
    {
    send_to_char( "No such skill or spell.\n\r", ch );
    return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
    send_to_char( "Value must be numeric.\n\r", ch );
    return;
    }

    value = atoi( arg3 );
    if ( value < 0 || value > 100 )
    {
    send_to_char( "Value range is 0 to 100.\n\r", ch );
    return;
    }

    if ( fAll )
    {
    for ( sn = 0; sn < MAX_SKILL; sn++ )
    {
        if ( skill_table[sn].name != NULL )
        victim->pcdata->learned[sn] = value;
    }
    }
    else
    {
    victim->pcdata->learned[sn] = value;
    }

    return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    char buf[100];
    CHAR_DATA *victim;
    int value;
    //language stuff
 if ( !str_cmp( arg2, "learn" ) )
    {
        if (IS_NPC(ch))
                send_to_char("Not on mob's\n\r", ch);
        else
                victim->pcdata->learn = value;
        return;
    }
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  set char <name> <field> <value>\n\r",ch);
    send_to_char( "  Field being one of:\n\r",          ch );
    send_to_char( "    sex level race group gold silver hp mana move\n\r",    ch );
    send_to_char( "    thirst hunger drunk full home max_blood blood\n\r",  ch );
    send_to_char( "    security gen rank discipline drains embraced \n\r",    ch );
    send_to_char( "    childer rage remorts renown paradox quintessence\n\r",    ch );
    send_to_char( "    avatar arete freebie csstr csdex cssta cscha csman\n\r",    ch );
    send_to_char( "    csapp csper csint cswit agg gnosis primal-urge\n\r", ch );
    send_to_char( "    sphere background prenown trenown fur\n\r", ch);
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    /* clear zones for mobs */
    victim->zone = NULL;

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    /*
     * Set something.
     */

    if ( !str_prefix( arg2, "sex" ) )
    {
    if ( value < 0 || value > 2 )
    {
        send_to_char( "Sex range is 0 to 2.\n\r", ch );
        return;
    }
    victim->sex = value;
    if (!IS_NPC(victim))
        victim->pcdata->true_sex = value;
    return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
    if ( !IS_NPC(victim) )
    {
        send_to_char( "Not on PC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > MAX_LEVEL )
    {
        sprintf(buf, "Level range is 0 to %d.\n\r", MAX_LEVEL);
        send_to_char(buf, ch);
        return;
    }
    victim->level = value;
    return;
    }

    if(!str_prefix(arg2,"home"))
    {
        if(IS_NPC(victim))
        {
            send_to_char("You want to give a MOB a PLAYER home? Yeah, that's really going to happen.\n\r",ch);
            return;
        }
        victim->pcdata->home = value;
        send_to_char("Home set.\n\r",ch);
        return;
    }

    if ( !str_prefix( arg2, "gold" ) )
    {
    victim->gold = value;
    return;
    }
    if ( !str_prefix( arg2, "influence" ) )
    {
        if (!victim->in_room->area->domain)
        {
            sendch("They are not in a domain!\n\r", ch); return;
        } else if (!is_clan(victim))
        {
            sendch("They are not in a clan.\n\r", ch); return;
        } else if (!IS_SET(victim->act2, PLR2_PVP))
        {
            sendch("They are not active!\n\r", ch); return;
        }else {
            DOMAIN_DATA *dom;
            dom = victim->in_room->area->domain;
            dom->influence[victim->clan] += value;
            do_save_clans(ch, "");
            sendch("Influence set.\n\r", ch);
            return;
        }
    }
    if ( !str_prefix(arg2, "silver" ) )
    {
    victim->silver = value;
    return;
    }

    if ( !str_prefix( arg2, "max_hp" ) )
    {
    if ( value < -10 || value > 30000 )
    {
        send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
        return;
    }
    victim->max_hit = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_hit = value;
    return;
    }

    if ( !str_prefix( arg2, "hp" ) )
    {
    if ( value < -10 || value > 30000 )
    {
        send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
        return;
    }
    victim->hit = value;
    update_pos(victim);
    return;
    }


    if ( !str_prefix( arg2, "mana" ) )
    {
    if ( value < 0 || value > 30000 )
    {
        send_to_char( "Mana range is 0 to 30,000 mana points.\n\r", ch );
        return;
    }
    victim->max_mana = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_mana = value;
    return;
    }

    if ( !str_prefix( arg2, "move" ) )
    {
    if ( value < 0 || value > 30000 )
    {
        send_to_char( "Move range is 0 to 30,000 move points.\n\r", ch );
        return;
    }
    victim->max_move = value;
        if (!IS_NPC(victim))
            victim->pcdata->perm_move = value;
    return;
    }

    if ( !str_prefix( arg2, "thirst" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < -1 || value > 100 )
    {
        send_to_char( "Thirst range is -1 to 100.\n\r", ch );
        return;
    }

    victim->pcdata->condition[COND_THIRST] = value;
    return;
    }

    if ( !str_prefix( arg2, "drunk" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < -1 || value > 100 )
    {
        send_to_char( "Drunk range is -1 to 100.\n\r", ch );
        return;
    }

    victim->pcdata->condition[COND_DRUNK] = value;
    return;
    }

    if ( !str_prefix( arg2, "full" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < -1 || value > 100 )
    {
        send_to_char( "Full range is -1 to 100.\n\r", ch );
        return;
    }

    victim->pcdata->condition[COND_FULL] = value;
    return;
    }

    if ( !str_prefix( arg2, "hunger" ) )
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        if ( value < -1 || value > 100 )
        {
            send_to_char( "Full range is -1 to 100.\n\r", ch );
            return;
        }

        victim->pcdata->condition[COND_HUNGER] = value;
        return;
    }

    if (!str_prefix( arg2, "hometown"))
    {
    int hometown;
    hometown = get_hometown_num(arg3);

    if (IS_NPC(victim))
    {
    sendch("Not on NPCs!\n\r", ch);
    return;
    }

    if (hometown < 0)
    {
    sendch("Invalid hometown.\n\r", ch);
    return;
    }
    victim->pcdata->hometown = hometown;
    return;
    }

    if (!str_prefix( arg2, "race" ) )
    {
    int race;

    race = race_lookup(arg3);

    if ( race == 0)
    {
        send_to_char("That is not a valid race.\n\r",ch);
        return;
    }

    if (!IS_NPC(victim) && !race_table[race].pc_race)
    {
        send_to_char("That is not a valid player race.\n\r",ch);
        return;
    }

    victim->race = race;
    return;
    }

    if (!str_prefix(arg2,"group"))
    {
    if (!IS_NPC(victim))
    {
        send_to_char("Only on NPCs.\n\r",ch);
        return;
    }
    victim->group = value;
    return;
    }

    if ( !str_prefix( arg2, "blood" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Blood range is 0 to 1000.\n\r", ch);
        return;
    }
    victim->pblood = value;
    return;
    }
    if ( !str_prefix( arg2, "max_blood" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Maximum Blood blood pool range is 0 to 1000.\n\r", ch);
        return;
    }
    victim->max_pblood = value;
    return;
    }

    if ( !str_prefix( arg2, "dpoints" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 100 )
    {
        send_to_char("Dpoint range is 0 to 100.\n\r", ch);
        return;
    }
    victim->dpoints = value;
    return;
    }

	if ( !str_prefix (arg2, "breed" ) )
	{
	if (IS_NPC(victim))
	{
	sendch("Not on npcs unless you wanna crash us.\n\r", ch);
	return;
	}

	int desired;
	if (!str_prefix(arg3, "homid")) desired = HOMID;
    else if(!str_prefix(arg3, "metis")) desired = METIS;
    else if(!str_prefix(arg3, "lupus")) desired = LUPUS;
    else
    {
        send_to_char("That is not a breed.\n\r",ch);
        return;
    }
	victim->pcdata->breed = desired;
	return;
	}

	if ( !str_prefix (arg2, "auspice" ) )
	{
	if (IS_NPC(victim))
	{
	sendch("Not on npcs unless you wanna crash us.\n\r", ch);
	return;
	}

	int desired;
	if (!str_prefix(arg3, "ragabash")) desired = RAGABASH;
    else if(!str_prefix(arg3, "theurge")) desired = THEURGE;
    else if(!str_prefix(arg3, "philodox")) desired = PHILODOX;
	else if(!str_prefix(arg3, "galliard")) desired = GALLIARD;
	else if(!str_prefix(arg3, "ahroun")) desired = AHROUN;
    else
    {
        send_to_char("That is not an auspice.\n\r",ch);
        return;
    }
	victim->pcdata->auspice = desired;
	return;
	}
    if (!str_prefix (arg2, "fur"))
    {
        if (IS_NPC(victim))
        {
            send_to_char("Not on NPCs.\n\r", ch);
            return;
        }
            smash_tilde( argument );
            free_string( victim->pcdata->garou_fur );
            victim->pcdata->garou_fur = str_dup( argument );
            send_to_char("Fur color set.\n\r", ch);
            send_to_char("Your fur color has been changed.\n\r", victim);
            send_to_char("Will take effect on your next shift.\n\r", victim);
            return;
    }
    if ( !str_prefix ( arg2, "willpower" ))
    {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if( value < 1 || value > 10 )
        {
            send_to_char("Invalid Willpower range.\n\r",ch);
            return;
        }
        victim->csmax_willpower = value;
        return;
    }
        if ( !str_prefix ( arg2, "sphere" ) )
    {
        int i, disc = 99;

        if(IS_NPC(victim))
        {
            send_to_char("Not on NPCs.\n\r",ch);
            return;
        }

        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        for(i = 0;i <= MAX_SPHERE;i++)
        {
            if(!str_prefix ( arg3, sphere_table[i].name ))
                disc = i;
        }
        if(disc == 99)
        {
            send_to_char("No such Sphere.\n\r",ch);
            return;
        }
        if(!is_number(arg4) || (value = atoi(arg4)) < 0 || value > 5)
        {
            send_to_char("Invalid Sphere range. Valid range is 0 - 5\n\r",ch);
            return;
        }
        victim->sphere[disc] = value;
        return;
    }
    if ( !str_prefix ( arg2, "discipline" ) )
    {
        int i, disc = 99;

        if(IS_NPC(victim))
        {
            send_to_char("Not on NPCs.\n\r",ch);
            return;
        }

        if(!IS_VAMP(victim))
        {
            send_to_char("Target is not a Vampire.\n\r",ch);
            return;
        }

        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        for(i = 0;i < MAX_DISC;i++)
        {
            if(!str_prefix ( arg3, disc_table[i].name ))
                disc = i;
        }
        if(disc == 99)
        {
            send_to_char("No such Discipline.\n\r",ch);
            return;
        }
        if(!is_number(arg4) || (value = atoi(arg4)) < 0 || value > 5)
        {
            send_to_char("Invalid Discipline range. Valid range is 0 - 5\n\r",ch);
            return;
        }
        victim->pcdata->discipline[disc] = value;
        return;
    }
        if ( !str_prefix ( arg2, "backgrounds" ) )
    {
        int i, disc = 99;

        if(IS_NPC(victim))
        {
            send_to_char("Not on NPCs.\n\r",ch);
            return;
        }

        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        for(i = 0;i < MAX_CSBACK;i++)
        {
            if(!str_prefix ( arg3, csback_table[i].name ))
                disc = i;
        }
        if(disc == 99)
        {
            send_to_char("No such Background.\n\r",ch);
            return;
        }
        if(!is_number(arg4) || (value = atoi(arg4)) < 0 || value > 5)
        {
            send_to_char("Invalid background range. Valid range is 0 - 5\n\r",ch);
            return;
        }
        if (disc == CSBACK_GENERATION)
        {
            victim->gen = 10-value;
            victim->pcdata->csgeneration = 10-value;
            victim->max_pblood = 100+(value * 10);
        }
        if (disc == CSBACK_FOUNT)
            victim->max_quintessence = 100+(value*10);

        victim->pcdata->csbackgrounds[disc] = value;
        return;
    }

    if ( !str_prefix( arg2, "ichours" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 30000 )
    {
        send_to_char("IC Hour range is 0 to 30000.\n\r", ch);
        return;
    }
    victim->pcdata->IC_total = value*60;
    return;
    }

   if ( !str_prefix( arg2, "remorts" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Remort range is 0 to 1000.\n\r", ch);
        return;
    }
    victim->remorts = value;
    return;
    }

    if ( !str_prefix( arg2, "paradox" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Paradox range is 0 to 100.\n\r", ch);
        return;
    }
    victim->paradox = value;
    while(victim->paradox+victim->quintessence > victim->max_quintessence)
        victim->quintessence--;

    return;
    }

    if ( !str_prefix( arg2, "quintessence" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Quintessence range is 0 to 1000.\n\r", ch);
        return;
    }
    victim->max_quintessence = value;
    victim->quintessence = value;
    while(victim->paradox+victim->quintessence > victim->max_quintessence)
        victim->paradox--;

    return;
    }

    if ( !str_prefix( arg2, "avatar" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("Avatar range is 0 to 5.\n\r", ch);
        return;
    }
    victim->avatar = value;
    return;
    }

    if ( !str_prefix( arg2, "arete" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 10 )
    {
        send_to_char("Arete range is 0 to 10.\n\r", ch);
        return;
    }
    victim->arete = value;
    return;
    }


    if ( !str_prefix( arg2, "renown" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 1000 )
    {
        send_to_char("Renown range is 0 to 1000.\n\r", ch);
        return;
    }
    victim->renown = value;
    return;
    }

    if ( !str_prefix( arg2, "csstr" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Strength range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_STRENGTH] = value;
    return;
    }

    if ( !str_prefix( arg2, "csdex" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Dexterity range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_DEXTERITY] = value;
    return;
    }

    if ( !str_prefix( arg2, "cssta" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Stamina range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_STAMINA] = value;
    return;
    }

    if ( !str_prefix( arg2, "cscha" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Charisma range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_CHARISMA] = value;
    return;
    }

    if ( !str_prefix( arg2, "csman" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Manipulation range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_MANIPULATION] = value;
    return;
    }

    if ( !str_prefix( arg2, "csapp" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Appearance range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_APPEARANCE] = value;
    return;
    }

    if ( !str_prefix( arg2, "csper" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Perception range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_PERCEPTION] = value;
    return;
    }

    if ( !str_prefix( arg2, "csint" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Intelligence range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_INTELLIGENCE] = value;
    return;
    }

    if ( !str_prefix( arg2, "cswit" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 5 )
    {
        send_to_char("CS Wits range is 0 to 5.\n\r", ch);
        return;
    }
    victim->csattributes[CSATTRIB_WITS] = value;
    return;
    }


    if ( !str_prefix( arg2, "gen" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( !IS_IMMORTAL(victim) && (value < 5 || value > 10) )
    {
        send_to_char("Generation range is 5 to 10.\n\r", ch);
        return;
    }
    int newgen;
    victim->gen = value;
    victim->pcdata->csgeneration = value;
    newgen = 10-value;
    if (newgen > 5) newgen = 5;
    if (newgen < 0) newgen = 0;
    victim->pcdata->csbackgrounds[CSBACK_GENERATION] = newgen;
    victim->max_pblood = 100+(newgen * 10);

    return;
    }
    if ( !str_prefix( arg2, "rank" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 1 || value > 10 )
    {
        send_to_char("Rank range is 1 to 10.\n\r", ch);
        return;
    }
    victim->rank = value;
    return;
    }
    if ( !str_prefix( arg2, "grank" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 1 || value > 5 )
    {
        send_to_char("Rank range is 1 to 5.\n\r", ch);
        return;
    }
    victim->pcdata->rank = value;
    return;
    }
    if ( !str_prefix( arg2, "drains" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 100 )
    {
        send_to_char("Drains range is 0 to 100.\n\r", ch);
        return;
    }
    victim->drains = value;
    return;
    }

    if ( !str_prefix( arg2, "embraced" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 100 )
    {
        send_to_char("Embraced range is 0 to 100.\n\r", ch);
        return; }
    victim->embraced = value;
    return;
    }

    if ( !str_prefix( arg2, "childer" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( value < 0 || value > 100 )
    {
        send_to_char("childer range is 0 to 100.\n\r", ch);
        return;
    }
    victim->childer = value;
    return;
    }

    if ( !str_prefix( arg2, "rage" ) )
    {
    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

/* Sengir hax to set current garou rage
    if ( value < 0 || value > victim->pcdata->rage[TEMP] )
    {
        send_to_char("rage range is 0 to max rage.\n\r", ch);
        return;
    }
    victim->pcdata->rage[TEMP] = value;
    return;
*/

    if ( value < 0 || value > 100 )
    {
        send_to_char("{RRage{x range is 0 to 100.\n\r", ch);
        return;
    }
    victim->rage = value;
    return;
    }

    if ( !str_prefix( arg2, "immclass" ))
    {
        if (IS_NPC(victim))
        {
            sendch("Not on NPCs!\n\r", ch);
            return;
        }
        if (value < 0 || value > 3 )
        {
            send_to_char("Immclass range is 0 to 3.\n\r",ch);
            return;
        }
        victim->pcdata->immclass = value;
        return;
    }

    if ( !str_prefix( arg2, "freebie" ))
    {
        if (value < 0 || value > 1000 )
        {
            send_to_char("Freebies range is 0 to 1000.\n\r",ch);
            return;
        }
        victim->freebie = value;
        return;
    }

    if ( !str_cmp( arg2, "security" ) ) /* OLC */
    {
    if ( IS_NPC(ch) )
    {
        send_to_char( "Si, claro.\n\r", ch );
        return;
    }

        if ( IS_NPC( victim ) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

    if ( value > ch->pcdata->security || value < 0 )
    {
        if ( ch->pcdata->security != 0 )
        {
        sprintf( buf, "Valid security is 0-%d.\n\r",
            ch->pcdata->security );
        send_to_char( buf, ch );
        }
        else
        {
        send_to_char( "Valid security is 0 only.\n\r", ch );
        }
        return;
    }
    victim->pcdata->security = value;
    return;
    }

    if ( !str_prefix( arg2, "agg" ) )
    {
        if ( value < 0)
        {
            send_to_char("You cannot set agg damage below 0.\n\r",ch);
            return;
        }
        if ( value > victim->max_hit-1)
        {
            send_to_char("You cannot set agg damage equal to or higher than max hit points.\n\r",ch);
            return;
        }
        victim->agg_dam = value;
        return;
    }
    if ( !str_prefix( arg2, "gnosis" ) )
    {
        if ( value < 0)
        {
            send_to_char("You cannot set gnosis below 0.\n\r",ch);
            return;
        }
        if ( value > 10)
        {
            send_to_char("You cannot set gnosis greater than 10.\n\r",ch);
            return;
        }
        victim->pcdata->gnosis[PERM] = value;
        return;
    }
    if (!str_prefix(arg2, "prenown") )
    {
        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        if (IS_NULLSTR(arg3) || IS_NULLSTR(arg4))
        {
            sendch("Syntax:\n\r\n\r", ch);
            sendch("set char <char> prenown <glory/honor/wisdom> <1-5>    -- Sets Permenant Renown on char.\n\r", ch);
            return;
        }

        if(!is_number(arg4) || (value = atoi(arg4)) < 0 || value > 10)
        {
            send_to_char("Invalid Renown range. Valid range is 0 - 5\n\r",ch);
            return;
        }
        if ( !str_prefix(arg3, "glory"))
            victim->pcdata->renown[GLORY] = value;
        else if ( !str_prefix(arg3, "honor"))
            victim->pcdata->renown[HONOR] = value;
        else if ( !str_prefix(arg3, "wisdom"))
            victim->pcdata->renown[WISDOM] = value;
        else
            sendch("Unknown renown type.\n\r", ch);
		return;
    }
    if (!str_prefix(arg2, "trenown") )
    {
        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );

        if (IS_NULLSTR(arg3) || IS_NULLSTR(arg4))
        {
            sendch("Syntax:\n\r\n\r", ch);
            sendch("set char <char> prenown <glory/honor/wisdom> <1-5>    -- Sets Temporary Renown on char.\n\r", ch);
            return;
        }

        if(!is_number(arg4) || (value = atoi(arg4)) < 0 || value > 10)
        {
            send_to_char("Invalid Renown range. Valid range is 0 - 10\n\r",ch);
            return;
        }
        if ( !str_prefix(arg3, "glory"))
            victim->pcdata->renown[TEMP_GLORY] = value;
        else if ( !str_prefix(arg3, "honor"))
            victim->pcdata->renown[TEMP_HONOR] = value;
        else if ( !str_prefix(arg3, "wisdom"))
            victim->pcdata->renown[TEMP_WISDOM] = value;
        else
			sendch("Unknown renown type.\n\r", ch);
        return;
    }
    if ( !str_prefix( arg2, "primal-urge" ) )
    {
        if ( value < 0)
        {
            send_to_char("You cannot set primal-urge below 0.\n\r",ch);
            return;
        }
        if ( value > 5)
        {
            send_to_char("You cannot set primal-urge greater than 5.\n\r",ch);
            return;
        }
        victim->pcdata->primal_urge = value;
        return;
    }

    /*
     * Generate usage message.
     */
    do_function(ch, &do_mset, "" );
    return;
}

void do_settime( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int amount;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Sentax: set time hour/day/month/year <number>\n\r", ch );
        sprintf( buf,
            "It is %d o'clock on the day %d, the month %d and year %d.\n\r",
            time_info.hour, time_info.day, time_info.month, time_info.year);
        send_to_char(buf,ch);
        return;
    }
    amount   = atoi(arg2);
    if (!str_cmp(arg1,"hour"))
    {
        time_info.hour = amount;
        return;
    }
    if (!str_cmp(arg1,"day"))
    {
        time_info.day = amount;
        return;
    }
    if (!str_cmp(arg1,"month"))
    {
        time_info.month = amount;
        return;
    }
    if (!str_cmp(arg1,"year"))
    {
        time_info.year = amount;
        return;
    }

send_to_char("Sentax: set time hour/day/month/year <number>\n\r", ch );
return;
}


void do_string( CHAR_DATA *ch, char *argument )
{
    char type [MAX_INPUT_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    smash_tilde( argument );
    argument = one_argument( argument, type );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( type[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  string char <name> <field> <string>\n\r",ch);
    send_to_char("    fields: name short long desc title spec wiziname immtitle\n\r",ch);
    send_to_char("  string obj  <name> <field> <string>\n\r",ch);
    send_to_char("    fields: name short long extended\n\r",ch);
    return;
    }

    if (!str_prefix(type,"character") || !str_prefix(type,"mobile"))
    {
        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
        {
        send_to_char( "They aren't here.\n\r", ch );
        return;
        }

    /* clear zone for mobs */
    victim->zone = NULL;

    /* string something */

        if ( !str_prefix( arg2, "name" ) )
            {
            if ( !IS_NPC(victim) )
                {
                    send_to_char( "Not on PC's.\n\r", ch );
                    return;
                }
                free_string( victim->name );
                victim->name = str_dup( arg3 );
                return;
            }

        if (!str_prefix (arg2, "wiziname") && IS_IMMORTAL(victim))
        {
                free_string(victim->pcdata->wiziname);
                victim->pcdata->wiziname = str_dup ( arg3 );
                return;
        }

        if (!str_prefix (arg2, "immtitle") && IS_IMMORTAL(victim))
        {
                free_string(victim->pcdata->immtitle);
                victim->pcdata->immtitle = str_dup ( arg3 );
                return;
        }

        if ( !str_prefix( arg2, "description" ) )
        {
            free_string(victim->description);
            victim->description = str_dup(arg3);
            return;
        }

        if ( !str_prefix( arg2, "short" ) )
        {
        free_string( victim->short_descr );
        victim->short_descr = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "long" ) )
        {
        free_string( victim->long_descr );
        strcat(arg3,"\n\r");
        victim->long_descr = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "sire" ) )
        {
        if(IS_NPC(victim))
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }
        if(ch->level < (MAX_LEVEL - 2))
        {
            send_to_char( "This string type is reserved for higher levels.\n\r", ch );
            return;
        }
        free_string( victim->sire );
        strcat(arg3,"\n\r");
        victim->sire = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "target" ) )
        {
            if(IS_NPC(victim))
            {
                send_to_char( "Not on NPC's.\n\r", ch );
                return;
            }
            if(ch->level < MAX_LEVEL)
            {
                send_to_char( "This string type is reserved for higher levels.\n\r", ch );
                return;
            }
            free_string( victim->pcdata->kill_target );
            // strcat(arg3,"\n\r");
            victim->pcdata->kill_target = str_dup( arg3 );
            return;
        }


        if ( !str_prefix( arg2, "title" ) )
        {
        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

/*      if ( ch->level < victim->level)
        {
        send_to_char( "I don't THINK so!\n\r", ch );
        return;
        }
*/
        set_title( victim, arg3 );
        return;
        }

        if ( !str_prefix( arg2, "spec" ) )
        {
        if ( !IS_NPC(victim) )
        {
            send_to_char( "Not on PC's.\n\r", ch );
            return;
        }

        if ( ( victim->spec_fun = spec_lookup( arg3 ) ) == 0 )
        {
            send_to_char( "No such spec fun.\n\r", ch );
            return;
        }

        return;
        }
    }

    if (!str_prefix(type,"object"))
    {
        /* string an obj */

    if ( ( obj = get_obj_world( ch, arg1, FALSE) ) == NULL )
        {
        send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
        return;
        }

        if ( !str_prefix( arg2, "name" ) )
        {
        free_string( obj->name );
        obj->name = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "short" ) )
        {
        free_string( obj->short_descr );
        obj->short_descr = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "long" ) )
        {
        free_string( obj->description );
        obj->description = str_dup( arg3 );
        return;
        }

        if ( !str_prefix( arg2, "ed" ) || !str_prefix( arg2, "extended"))
        {
        EXTRA_DESCR_DATA *ed;

        argument = one_argument( argument, arg3 );
        if ( argument == NULL )
        {
            send_to_char( "Syntax: oset <object> ed <keyword> <string>\n\r",
            ch );
            return;
        }

        strcat(argument,"\n\r");

        ed = new_extra_descr();

        ed->keyword     = str_dup( arg3     );
        ed->description = str_dup( argument );
        ed->next        = obj->extra_descr;
        obj->extra_descr    = ed;
        return;
        }
    }


    /* echo bad use message */
    do_function(ch, &do_string, "");
}



void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
    send_to_char("Syntax:\n\r",ch);
    send_to_char("  set obj <object> <field> <value>\n\r",ch);
    send_to_char("  Field being one of:\n\r",               ch );
    send_to_char("    value0 value1 value2 value3 value4 (v1-v4)\n\r",  ch );
    send_to_char("    extra wear level weight cost timer\n\r",      ch );
    return;
    }

    if ( ( obj = get_obj_world( ch, arg1, FALSE ) ) == NULL )
    {
    send_to_char( "Nothing like that in heaven or earth.\n\r", ch );
    return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_cmp( arg2, "value0" ) || !str_cmp( arg2, "v0" ) )
    {
    obj->value[0] = UMIN(50,value);
    return;
    }

    if ( !str_cmp( arg2, "value1" ) || !str_cmp( arg2, "v1" ) )
    {
    obj->value[1] = value;
    return;
    }

    if ( !str_cmp( arg2, "value2" ) || !str_cmp( arg2, "v2" ) )
    {
    obj->value[2] = value;
    return;
    }

    if ( !str_cmp( arg2, "value3" ) || !str_cmp( arg2, "v3" ) )
    {
    obj->value[3] = value;
    return;
    }

    if ( !str_cmp( arg2, "value4" ) || !str_cmp( arg2, "v4" ) )
    {
    obj->value[4] = value;
    return;
    }

    if ( !str_prefix( arg2, "extra" ) )
    {
            if ( ( value = flag_value( extra_flags, argument ) ) != NO_FLAG )
        {
            TOGGLE_BIT(obj->extra_flags, value);

            send_to_char( "Extra flag toggled.\n\r", ch);
            return;
        } else {
                printf_to_char(ch, "Invalid bit name '%s'", argument);
                return;
        }
    }

    if ( !str_prefix( arg2, "wear" ) )
    {
    obj->wear_flags = value;
    return;
    }

    if ( !str_prefix( arg2, "level" ) )
    {
    obj->level = value;
    return;
    }

    if ( !str_prefix( arg2, "weight" ) )
    {
    obj->weight = value;
    return;
    }

    if ( !str_prefix( arg2, "cost" ) )
    {
    obj->cost = value;
    return;
    }

    if ( !str_prefix( arg2, "timer" ) )
    {
    obj->timer = value;
    return;
    }

    /*
     * Generate usage message.
     */
    do_function(ch, &do_oset, "" );
    return;
}



void do_rset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *location;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
    send_to_char( "Syntax:\n\r",ch);
    send_to_char( "  set room <location> <field> <value>\n\r",ch);
    send_to_char( "  Field being one of:\n\r",          ch );
    send_to_char( "    flags sector\n\r",               ch );
    return;
    }

    if ( ( location = find_location( ch, arg1 ) ) == NULL )
    {
    send_to_char( "No such location.\n\r", ch );
    return;
    }

    if (!is_room_owner(ch,location) && ch->in_room != location
    &&  room_is_private(location) && !IS_TRUSTED(ch,IMPLEMENTOR))
    {
        send_to_char("That room is private right now.\n\r",ch);
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg3 ) )
    {
    send_to_char( "Value must be numeric.\n\r", ch );
    return;
    }
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix( arg2, "flags" ) )
    {
    location->room_flags    = value;
    return;
    }

    if ( !str_prefix( arg2, "sector" ) )
    {
    location->sector_type   = value;
    return;
    }

    /*
     * Generate usage message.
     */
    do_function(ch, &do_rset, "");
    return;
}



void do_sockets( CHAR_DATA *ch, char *argument )
{
    char buf[2 * MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count   = 0;
    buf[0]  = '\0';

    one_argument(argument,arg);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d->character != NULL && can_see( ch, d->character )
    && (arg[0] == '\0' || is_name(arg,d->character->name)
               || (d->original && is_name(arg,d->original->name))))
    {
        count++;
        sprintf( buf + strlen(buf), "[%3d %2d] %s@%s\n\r",
        d->descriptor,
        d->connected,
        d->original  ? d->original->name  :
        d->character ? d->character->name : "(none)",
        d->host
        );
    }
    }
    if (count == 0)
    {
    send_to_char("No one by that name is connected.\n\r",ch);
    return;
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    strcat(buf,buf2);
    page_to_char( buf, ch );
    return;
}



/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
    send_to_char( "Force whom to do what?\n\r", ch );
    return;
    }

    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete") || !str_prefix(arg2,"mob"))
    {
    send_to_char("That will NOT be done.\n\r",ch);
    return;
    }

    sprintf( buf, "$n forces you to '%s'.", argument );

    if ( !str_cmp( arg, "all" ) )
    {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    if (get_trust(ch) < MAX_LEVEL - 3)
    {
        send_to_char("Not at your level!\n\r",ch);
        return;
    }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;

        if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch ) )
        {
        act( buf, ch, NULL, vch, TO_VICT );
        interpret( vch, argument );
        }
    }
    }
    else if (!str_cmp(arg,"players"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }

        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;

            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch )
        &&   vch->level < LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else if (!str_cmp(arg,"gods"))
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if (get_trust(ch) < MAX_LEVEL - 2)
        {
            send_to_char("Not at your level!\n\r",ch);
            return;
        }

        for ( vch = char_list; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;

            if ( !IS_NPC(vch) && get_trust( vch ) < get_trust( ch )
            &&   vch->level >= LEVEL_HERO)
            {
                act( buf, ch, NULL, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else
    {
    CHAR_DATA *victim;

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Aye aye, right away!\n\r", ch );
        return;
    }

        if (!is_room_owner(ch,victim->in_room)
    &&  ch->in_room != victim->in_room
        &&  room_is_private(victim->in_room) && !IS_TRUSTED(ch,IMPLEMENTOR))
        {
            send_to_char("That character is in a private room.\n\r",ch);
            return;
        }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "Do it yourself!\n\r", ch );
        return;
    }

    if ( !IS_NPC(victim) && get_trust(ch) < MAX_LEVEL -3)
    {
        send_to_char("Not at your level!\n\r",ch);
        return;
    }

    act( buf, ch, NULL, victim, TO_VICT );
    interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}



/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    /* take the default path */

      if ( ch->invis_level)
      {
      ch->invis_level = 0;
      act( "$n slowly fades into existence.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly fade back into existence.\n\r", ch );
      }
      else
      {
      ch->invis_level = get_trust(ch);
      act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
      send_to_char( "You slowly vanish into thin air.\n\r", ch );
      }
    else
    /* do the level thing */
    {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
    send_to_char("Invis level must be between 2 and your level.\n\r",ch);
        return;
      }
      else
      {
      ch->reply = NULL;
          ch->invis_level = level;
          act( "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You slowly vanish into thin air.\n\r", ch );
      }
    }

    return;
}


void do_incognito( CHAR_DATA *ch, char *argument )
{
    int level;
    char arg[MAX_STRING_LENGTH];

    /* RT code for taking a level argument */
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    /* take the default path */

      if ( ch->incog_level)
      {
          ch->incog_level = 0;
          act( "$n is no longer cloaked.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You are no longer cloaked.\n\r", ch );
      }
      else
      {
          ch->incog_level = get_trust(ch);
          act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You cloak your presence.\n\r", ch );
      }
    else
    /* do the level thing */
    {
      level = atoi(arg);
      if (level < 2 || level > get_trust(ch))
      {
        send_to_char("Incog level must be between 2 and your level.\n\r",ch);
        return;
      }
      else
      {
          ch->reply = NULL;
          ch->incog_level = level;
          act( "$n cloaks $s presence.", ch, NULL, NULL, TO_ROOM );
          send_to_char( "You cloak your presence.\n\r", ch );
      }
    }

    return;
}



void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
    return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* prefix command: it will put the string typed on each line typed */

void do_prefi (CHAR_DATA *ch, char *argument)
{
    send_to_char("You cannot abbreviate the prefix command.\r\n",ch);
    return;
}

void do_prefix (CHAR_DATA *ch, char *argument)
{
    char buf[MAX_INPUT_LENGTH];

    if (argument[0] == '\0')
    {
    if (ch->prefix[0] == '\0')
    {
        send_to_char("You have no prefix to clear.\r\n",ch);
        return;
    }

    send_to_char("Prefix removed.\r\n",ch);
    free_string(ch->prefix);
    ch->prefix = str_dup("");
    return;
    }

    if (ch->prefix[0] != '\0')
    {
    sprintf(buf,"Prefix changed to %s.\r\n",argument);
    free_string(ch->prefix);
    }
    else
    {
    sprintf(buf,"Prefix set to %s.\r\n",argument);
    }

    ch->prefix = str_dup(argument);
}


#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)


/*  Copyover - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@andreasen.org>
 *  http://www.andreasen.org
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
bool copyover_handler()
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next, *d_prev;
    char buf [100], buf2[100], buf3[MSL];
    extern int port,control; /* db.c */

    fp = fopen (COPYOVER_FILE, "w");

    if (!fp)
    {
        sprintf (buf3, "Could not write to copyover file: %s", COPYOVER_FILE);
        log_string( buf );
        perror ("do_copyover:fopen");
        return FALSE;
    }

    /* For each playing descriptor, save its state */
    for (d = descriptor_tsil; d ; d = d_prev)
    {
        CHAR_DATA * och = CH (d);
        d_prev = d->prev; /* We delete from the list , so need to save this */

        if (!d->character || d->connected > CON_PLAYING) /* drop those logging on */
        {
            write_to_descriptor (d->descriptor, "\n\rSorry, we are rebooting. Come back in a few minutes.\n\r", 0);
            close_socket (d); /* throw'em out */
        }
        else
        {
            fprintf (fp, "%d %s %s %d %s\n", d->descriptor, och->name, d->host, (int) (current_time - och->logon), och->leader != NULL ? och->leader->name : NULL);
            save_char_obj (och);
        }
    }

    fprintf (fp, "-1\n");
    fclose (fp);

    /* Close reserve and other always-open files and release other resources */
    fclose (fpReserve);

    /* exec - descriptors are inherited */
    sprintf (buf, "%d", port);
    sprintf (buf2, "%d", control);
    execl (EXE_FILE, "rom", buf, "copyover", buf2, (char *) NULL);

    /* Failed - sucessful exec will not return */
    perror ("do_copyover: execl");

    /* Here you might want to reopen fpReserve */
    fpReserve = fopen (NULL_FILE, "r");
    return FALSE;
}

void do_copyover (CHAR_DATA *ch, char * argument)
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    char buf [100], buf2[100], buf3[MSL];
    extern int port,control; /* db.c */

    if (!str_cmp(argument, "legend"))
    {
        sendch("\n\r{DLegend:{x\n\r", ch);
        sendch("{c[ * >{x - OOC Off/Quiet     {W[ * >{x - OOC On\n\r", ch);
        sendch("{GMatthew@123{x - Ready       {yYgolonac@123{x - Fighting\n\r", ch);
        sendch("{YZelan@123{x - Writing Note  {RMaat@123{x - In String Editor\n\r", ch);
        sendch("{DUgha@no-link{x - Linkdead\n\r", ch);
        return;
    }
    if (IS_NULLSTR(argument) || str_cmp(argument, "confirm"))
    {
        CHAR_DATA *wch;
        char        pcolor[MSL];
        char        bcolor[MSL];
        BUFFER      *output;

        output = new_buf();
        pcolor[0] = '\0';
        bcolor[0] = '\0';
        buf3[0]   = '\0';

        for (wch = char_list; wch; wch = wch->next )
        {
            if (IS_NPC(wch))
                continue;

    /* Color of player name.
     * Green    = Good to Go.
     * DkYellow = In a Fight.
     * BrYellow = Writing Note.
     * Red      = In Editor.
     * Grey     = Linkdead (Will drop in copyover)
     */
            sprintf(pcolor, "{G");
            if (wch->desc)
            {
                if (wch->fighting)
                    sprintf(pcolor, "{y");
                if (wch->pnote)
                    sprintf(pcolor, "{Y");
                if (wch->desc->pEdit)
                    sprintf(pcolor, "{R");
            } else
                sprintf(pcolor, "{D");
    /* Color of [] around stats.
     * dkCyan   = OOC off/Quiet
     * BrWhite  = OOC on
     */
            if (IS_SET(wch->comm, COMM_NOOOC) || IS_SET(wch->comm, COMM_QUIET))
                sprintf(bcolor, "{c");
            else
                sprintf(bcolor, "{W");

    /*Format it up.*/
            sprintf(buf3, "%s[ %s %3d %3d %3d %3d> {D<{w%4d{D> %s%s@%s{x\n\r",
            bcolor, (wch->pcdata->true_sex == 1) ? "M" : "F",
            wch->level,
            (int) wch->hit*100/(wch->max_hit==0?1:wch->max_hit),
            (int) wch->mana*100/(wch->max_mana==0?1:wch->max_mana),
            (int) wch->move*100/(wch->max_move==0?1:wch->max_move),
            wch->timer, pcolor, wch->name,
            (wch->desc != NULL) ? wch->desc->host : "no-link");

            add_buf(output, buf3);

            }
        sendch("\n\r{W*Type 'copyover legend' for color codes.*{x\n\r", ch);
        sendch("{D     L{x\n\r", ch);
        sendch("{D     E       M   M     I     N        H{x\n\r", ch);
        sendch("{D  S  V       A   O     D     A        O{x\n\r", ch);
        sendch("{D  E  E   H   N   V     L     M        S{x\n\r", ch);
        sendch("{D  X  L   P   A   E     E     E        T{x\n\r", ch);
        page_to_char(buf_string(output), ch);
        free_buf(output);
        return;
    }
    sprintf(buf3, "Copyover initialized by %s", ch->name);
    log_string(buf3);

    write_to_descriptor (ch->desc->descriptor, "Starting Copyover Procedures...\n\r", 0);


    write_to_descriptor (ch->desc->descriptor, "Saving commands, config and helps...\n\r", 0);
    /* autosave changed lists */
    do_function( NULL, &do_asave, "config" );
    do_function( NULL, &do_asave, "helps");
    do_function( NULL, &do_asave, "commands");
    do_function( NULL, &do_asave, "quests");
    write_to_descriptor (ch->desc->descriptor, "Proceeding to copyover...\n\r", 0);

    sprintf (buf, "\n\r\n\r *** Pull the Lever, Kronk! \n\r\n\r");

    for (d = descriptor_list; d ; d = d->next)
    {
        write_to_descriptor(d->descriptor, buf, 0);
    }


    if (!copyover_handler())
    {
        send_to_char ("Copyover FAILED!\n\r",ch);
    }
}

/* Recover from a copyover - load players */
void copyover_recover ()
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA   *ch;
    PC_DATA     *pcdata;
    FILE *fp;
    char name [100];
    char host[MSL];
    int desc;
    bool fOld;
    char buf[MSL];
    char leader[MSL];
    int logon;

    log_string("Copyover recovery initiated");

    fp = fopen (COPYOVER_FILE, "r");

    if (!fp) /* there are some descriptors open which will hang forever then ? */
    {
        perror ("copyover_recover:fopen");
        log_string ("Copyover file not found. Exitting.\n\r");
        exit (1);
    }

    unlink (COPYOVER_FILE); /* In case something crashes - doesn't prevent reading  */

    for (;;)
    {
        fscanf (fp, "%d %s %s %d %s\n", &desc, name, host, &logon, leader);
        if (desc == -1)
            break;

        /* Write something, and check if it goes error-free */
        if (!write_to_descriptor (desc, "\n\r *** WRONG LEEVEEEEEEERRRRRRRR!!\n\r",0))
        {
            close (desc); /* nope */
            continue;
        }

        d = new_descriptor();
        d->descriptor = desc;

        d->host = str_dup (host);
        d->next = descriptor_list;
        if (descriptor_list)
            descriptor_list->prev = d;
        if (descriptor_tsil == NULL)
            descriptor_tsil = d;
        descriptor_list = d;

        d->connected = CON_COPYOVER_RECOVER; /* -15, so close_socket frees the char */


        /* Now, find the pfile */

        fOld = load_char_obj (d, name);

        if (!fOld) /* Player file not found?! */
        {
            write_to_descriptor (desc, "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r", 0);
            close_socket (d);
        }
        else /* ok! */
        {

            /* Just In Case */
            if (!d->character->in_room)
                d->character->in_room = get_room_index (ROOM_VNUM_TEMPLE);

            /* Insert in the char_list */
            d->character->next = char_list;
            char_list = d->character;
            ch = d->character;
            pcdata = ch->pcdata;

            if (is_affected(ch, gsn_astralprojection)) {
                ch->in_room = get_room_index(ROOM_VNUM_TEMPLE);
                pass_gauntlet(ch, FALSE);
                sendch("{RYou were astrally projected during the copyover.\n\rYou've been returned to Recall.{x\n\r", ch);
                affect_strip(ch, gsn_astralprojection);
                affect_strip(ch, gsn_astrallylost);
            }

            ch->logon = (time_t) (current_time - logon);
            char_to_room (d->character, d->character->in_room);
            do_look (d->character, "auto");
            d->character->leadername = str_dup(leader);
            act ("$n materializes!", d->character, NULL, NULL, TO_ROOM);
            d->connected = CON_PLAYING;

            if (d->character->pet != NULL)
            {
                char_to_room(d->character->pet,d->character->in_room);
                act("$n materializes!.",d->character->pet,NULL,NULL,TO_ROOM);
            }
            sendch ("\n\r{DYour molecules have been restabilized.\n\r{wYou may feel violently ill. This is completely normal.{x\n\r", d->character);
        }
            extern bool slaughter;
    extern bool doubleexp;
    extern bool manualxp;
    extern bool spookums;
    if (slaughter)
        send_to_char("Time to get killing, {RSlaughterfest{x is running!\n\r", d->character);
    if (doubleexp || manualxp)
    {
        if (xpawardmult == 2)
            send_to_char("{CDoubleexp{x is turned on!{x\n\r", d->character);
        if (xpawardmult == 3)
            send_to_char("{CTripleexp{x is turned on!{x\n\r", d->character);
    }
    if (spookums)
        send_to_char("Spooky Time!\n\r", d->character);

    }
   fclose (fp);

    for (d = descriptor_list; d ; d = d->next)
    {
        ch = d->character;
        if (ch->leadername != NULL)
        {
            ch->leader = get_char_world(NULL, ch->leadername);
            ch->master = ch->leader;
        }
    }

}


void do_nuk( CHAR_DATA *ch, char *argument )
{
    send_to_char( "Type the WHOLE thing!\n\r", ch );
    return;
}

void do_nuke( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    char strsave[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Nuke whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( ch == victim )
    {
    send_to_char( "Suicide is a mortal sin.\n\r", ch );
    return;
    }

    if ( victim->level > ch->level )
    {
    send_to_char( "Your kidding, right?\n\r", ch );
    return;
    }

    act( "You nuke $M!",  ch, NULL, victim, TO_CHAR    );
    act( "You feel your body torn apart as $n nukes you!", ch, NULL, victim, TO_VICT    );
    act( "$n nukes $N!",  ch, NULL, victim, TO_NOTVICT );
    sprintf(buf,"$N nukes %s!",victim->name);
    wiznet(buf,ch,NULL,WIZ_PENALTIES,WIZ_SECURE,0);
    stop_fighting(victim,TRUE);
    do_function(victim, &do_quit, "");
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name ) );
    unlink(strsave);
    sprintf( strsave, "%s%s", BK_PLAYER_DIR, capitalize( victim->name ) );
    unlink(strsave);
    return;
}

void do_beep(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

 if ( ( victim = get_char_world( ch, arg ) ) == NULL )
 {
  send_to_char( "Beep whom?\n\r", ch );
  return;
 }

 sprintf( buf, "{R%s beeps at you!{<*{x\n\r", ch->name );
 send_to_char(buf,victim);
 sprintf( buf, "{RYou beep %s.{<*{x\n\r", victim->name );
 send_to_char(buf,ch);
 return;
}

void do_clearaff(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    while ( victim->affected )
       affect_remove( victim, victim->affected );
    send_to_char( "You feel less affected by things.\n\r", victim );
    sprintf( buf, "%s's affects have been cleared.\n\r", victim->name );
    send_to_char(buf,ch);

    if (IS_NPC(victim))
        return;

    if (victim->pcdata->shiftform != HOMID)
    {
      victim->pcdata->shiftform = HOMID;
      sprintf(buf, "%s has been shifted to Homid form.\n\r", victim->name);
      send_to_char(buf,ch);
    }

    return;
}

void do_roll(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    int num1;
    int num2;
    int result;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if(arg1 == NULL || arg2 == NULL)
    {
        send_to_char("Please enter TWO numbers.\n\r" ,ch);
        return;
    }

    if(!is_number( arg1 ) || !is_number( arg2 ))
    {
        send_to_char("Only numbers are valid.\n\r" ,ch);
        return;
    }

    num1   = atoi(arg1);
    num2   = atoi(arg2);
    if(num1 > 30000 || num2 > 30000)
    {
        send_to_char("Range too high!\n\r",ch);
        return;
    }

    send_to_char("Rolling numbers...\n\r" ,ch);

    result = number_range( num1, num2 );

    sprintf( buf, "The result is: %d", result );
    send_to_char(buf,ch);
    return;
}

void do_smite( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' )
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("Smite <victim> <y> <g>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char("Release your wrath upon whom?\n\r",ch);
        return;
    }

    if(IS_NPC(victim))
    {
        send_to_char("Not on NPCs!\n\r",ch);
        return;
    }

    if (victim->level > ch->level)
    {
        send_to_char("How can you smite someone who can crush you in one blow?\n\r",ch);
        return;
    }
    sprintf( buf, "\n\r"
"{C         _, .--.\n\r"
"        (  / (  '-.\n\r"
"        .-=-.    ) -.\n\r"
"       /   (  .' .   \\\n\r"
"       \\ ( ' ,_) ) \\_/\n\r"
"        (_ , {Y/\\{C  ,_/\n\r"
"          '--{Y\\ `\\{C--`\n\r"
"             {Y_\\ _\\\n\r"
"             `\\ \\ \n\r"
"              _\\_\\\n\r"
"              `\\\\\n\r"
"                \\\\\n\r"
"            {y-.'.{Y`\\{y.'.-\n\r");
    sprintf( buf2, "{WThe {RWrath{W of %s is unleashed upon %s.\n\r\n\rA bolt of lightning streaks out of the sky and strikes %s in the head.{x\n\r", ch->name, victim->name, victim->name);


    if(!str_cmp(arg2, "y") && !str_cmp(arg3, "g"))
    {


        for ( d = descriptor_list; d; d = d->next )
        {
            if ( d->connected == CON_PLAYING )
            {
                send_to_char( buf, d->character );
                send_to_char( buf2, d->character );
                send_to_char( "\n\r",   d->character );
            }
        }
    }
    else if(!str_cmp(arg2, "y"))
        {
            send_to_char( buf, victim );
            send_to_char( buf2, victim );
        }
    else if(!str_cmp(arg2, "g"))
        {
            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( buf2, d->character );
                }
            }
        }

    send_to_char( "{wYou are struck by the Lightning and knocked onto your rear!{x\n\r", victim );
    send_to_char( "\n\r{wYou smite them down with your power!{x\n\r", ch );
    victim->position = POS_SITTING;
    if ( ( obj = get_eq_char( victim, WEAR_FEET ) ) != NULL )
    {
        obj_from_char( obj );
        send_to_char("{WYou are blown out of your shoes and they land smouldering on the ground before you!{x\n\r",victim );
        act("{W$N is struck by $n's lightning so hard their shoes fly off their feet and land before you!{x", ch, NULL, victim, TO_NOTVICT );
        act("{W$N is struck by your lightning so hard their shoes fly off their feet!{x", ch, NULL, victim, TO_CHAR );
        obj_to_room( obj, victim->in_room );
    }
    return;
}

void do_vlist( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    MOB_INDEX_DATA *pMobIndex;
    OBJ_INDEX_DATA *pObjIndex;
    ROOM_INDEX_DATA *pRoomIndex;
    PROG_LIST  *prg;
    int tvnum;
    int bvnum;
    int vnum;
    int i;
    bool found;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if(!is_number(arg2) || !is_number(argument) || arg[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0')
    {
        send_to_char( "{WSyntax: {Yvlist <sub_command> <min_vnum> <max_vnum>{x\n\r", ch );
        sendch("\n\rValid Subcommands:\n\r", ch);
        sendch("obj, room, mob, mprog, oprog, rprog, showmp{R*{x, showop{R*{x, showrp{R*{x\n\r", ch);
        sendch("\n\r{R*{x - min and max vnum refer to the mob/room/obj, not program.\n\r", ch);
        return;
    }

    bvnum = atoi( arg2 );
    tvnum = atoi( argument );
    found = FALSE;

    if (!str_cmp(arg,"obj"))
    {
        for ( vnum = bvnum; vnum <= tvnum; vnum++ )
        {
            if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
            {
                    found = TRUE;
                    sprintf( buf, "{W[{G%5d{W] <{Blvl {M%3d{W> : {Ycnt{C%2d{W : {x%s\n\r",
                    pObjIndex->vnum, pObjIndex->level, pObjIndex->count, pObjIndex->short_descr );
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "{RNo {Cobjects {Rfound in that range.\n\r", ch );
        return;
    }

    if (!str_cmp(arg,"mob"))
    {
        for ( vnum = bvnum; vnum <= tvnum; vnum++ )
        {
            if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
            {
                    found = TRUE;
                    sprintf( buf, "{W[{G%5d{W] <{Blvl {M%3d{W> {W: {Ycnt{C%2d{W : {x%s\n\r", pMobIndex->vnum, pMobIndex->level, pMobIndex->count, pMobIndex->short_descr );
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "{RNo {Cmobs {Rfound in that range.\n\r", ch );
        return;
    }

    if (!str_cmp(arg,"room"))
    {
        for ( vnum = bvnum; vnum <= tvnum; vnum++ )
        {
            if ( ( pRoomIndex = get_room_index( vnum ) ) != NULL )
            {
                    found = TRUE;
                    sprintf( buf, "{D<{C%s{D> {W[{G%5d{W] {x%s\n\r",
                    pRoomIndex->sector_type == 0 ? "inside  " : pRoomIndex->sector_type == 1 ? "city    " : pRoomIndex->sector_type == 2 ? "field   " : pRoomIndex->sector_type == 3 ? "forest  " : pRoomIndex->sector_type == 4 ? "hills   " : pRoomIndex->sector_type == 5 ? "mountain" : pRoomIndex->sector_type == 6 ? "swim    " : pRoomIndex->sector_type == 7 ? "noswim  " : pRoomIndex->sector_type == 8 ? "unused  " : pRoomIndex->sector_type == 9 ? "air     " : pRoomIndex->sector_type == 10 ? "desert  " : pRoomIndex->sector_type == 11 ? "drown   " : pRoomIndex->sector_type == 12 ? "hot     " : pRoomIndex->sector_type == 13 ? "cold    " : pRoomIndex->sector_type == 14 ? "node    " : "{RERROR{x   ", pRoomIndex->vnum, pRoomIndex->name );
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "{RNo {Crooms {Rfound in that range.\n\r", ch );
        return;
    }

    if (!str_cmp(arg,"showmp"))
    {
        for ( vnum = bvnum; vnum <= tvnum; vnum++ )
        {
            if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL && pMobIndex->mprog_flags)
            {
                    found = TRUE;
                    sprintf( buf, "{W[{G%5d{W] {x%s has",
                    pMobIndex->vnum, pMobIndex->short_descr );
                    send_to_char( buf, ch );
                    for (prg = pMobIndex->mprogs; prg != NULL;prg = prg->next )
                    {
                        sprintf( buf, " {W[{C%d{W]{x",prg->vnum);
                        send_to_char( buf, ch );

                    }
                    send_to_char(".\n\r",ch);
            }
        }
        if(!found)
            send_to_char( "No mobs with mobprogs on them in that range.\n\r", ch );
        return;
    }

    if (!str_cmp(arg,"showop"))
    {
        for ( vnum = bvnum; vnum <= tvnum; vnum++ )
        {
            if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL && pObjIndex->oprog_flags)
            {
                    found = TRUE;
                    sprintf( buf, "{W[{G%5d{W] {x%s has",
                    pObjIndex->vnum, pObjIndex->short_descr );
                    send_to_char( buf, ch );
                    for (prg = pObjIndex->oprogs; prg != NULL;prg = prg->next )
                    {
                        sprintf( buf, " {W[{C%d{W]{x",prg->vnum);
                        send_to_char( buf, ch );

                    }
                    send_to_char(".\n\r",ch);
            }
        }
        if(!found)
            send_to_char( "No objs with objprogs on them in that range.\n\r", ch );
        return;
    }
        if (!str_cmp(arg,"showrp"))
        {
            for ( vnum = bvnum; vnum <= tvnum; vnum++ )
            {
                if ( ( pRoomIndex = get_room_index( vnum ) ) != NULL && pRoomIndex->rprog_flags)
                {
                        found = TRUE;
                        sprintf( buf, "{W[{G%5d{W] {x%s has",
                        pRoomIndex->vnum, pRoomIndex->name );
                        send_to_char( buf, ch );
                        for (prg = pRoomIndex->rprogs; prg != NULL;prg = prg->next )
                        {
                            sprintf( buf, " {W[{C%d{W]{x",prg->vnum);
                            send_to_char( buf, ch );

                        }
                        send_to_char(".\n\r",ch);
                }
            }
            if(!found)
                send_to_char( "No rooms with roomprogs on them in that range.\n\r", ch );
            return;
        }

    if (!str_cmp(arg,"mprog"))
    {
        for ( i = 0,vnum = bvnum; vnum <= tvnum; vnum++)
        {
            if ( ( get_prog_index( vnum, PRG_MPROG ) ) != NULL )
            {
                    found = TRUE;
                    i++;
                    sprintf( buf, "{W[%2d] {cVnum {W[{C%5d{W]{x\n\r",i,vnum);
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "No mobprogs found in that range.\n\r", ch );
        return;
    }
    if (!str_cmp(arg,"oprog"))
    {
        for ( i = 0,vnum = bvnum; vnum <= tvnum; vnum++)
        {
            if ( ( get_prog_index( vnum, PRG_OPROG ) ) != NULL )
            {
                    found = TRUE;
                    i++;
                    sprintf( buf, "{W[%2d] {cVnum {W[{C%5d{W]{x\n\r",i,vnum);
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "No objprogs found in that range.\n\r", ch );
        return;
    }
    if (!str_cmp(arg,"rprog"))
    {
        for ( i = 0,vnum = bvnum; vnum <= tvnum; vnum++)
        {
            if ( ( get_prog_index( vnum, PRG_RPROG ) ) != NULL )
            {
                    found = TRUE;
                    i++;
                    sprintf( buf, "{W[%2d] {cVnum {W[{C%5d{W]{x\n\r",i,vnum);
                    send_to_char( buf, ch );
            }
        }
        if(!found)
            send_to_char( "No roomprogs found in that range.\n\r", ch );
        return;
    }
    sprintf(buf, "do_vlist: subcommand '%s' not recognized.", arg);
    bug( buf, 0 );
    return;
}
/*
void read_wizlist( CHAR_DATA *ch)
{
    FILE *fp;
    int lvl[200];
    int count, i;
    char name[200] [MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];

    do_function(ch, &do_help, "wizlistintro");


    if ( ( fp = fopen( GOD_FILE, "r" ) ) == NULL )
        return;
    count = 1;
    lvl[0] = 1;
    for(count = 0; lvl[count] != 0; count++)
    {
        strcpy(name[count],fread_word(fp));
        lvl[count] = fread_number(fp);
        }
    fclose(fp);

    send_to_char("                                    GOD\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r\n\r                               Seraphim[109]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);

    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-1)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Cherubim[108]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-2)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Praetors[107]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-3)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Watchers[106]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-4)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Warders[105]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-5)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Veterans[104]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-6)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                              Developers[103]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-7)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                               Creators[102]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-8)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
    send_to_char("\n\r\n\r                           Novice Immortals[101]\n\r",ch);
    send_to_char("                              ---------------\n\r",ch);
    for(i = 0; i < count; i++)
    {
        if(lvl[i] == MAX_LEVEL-9)
        {
            sprintf( buf, "%s\n\r", center( name[i], 76, " "));
            send_to_char(buf,ch);
        }
    }
return;
}
void do_addwizlist(CHAR_DATA *ch, char *argument) {

    CHAR_DATA *victim;
    char    arg[MIL];
    one_argument(argument, arg);

    if ( arg[0] == '\0' )
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("addwizlist <immortal>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char("Add which Immortal to the wizlist?\n\r",ch);
        return;
    }

    if(IS_NPC(victim))
    {
        send_to_char("Not on NPCs!\n\r",ch);
        return;
    }
    if (victim->level < 101) {
        send_to_char("Not on Mortals!\n\r", ch);
        return;
    }
    write_wizlist(ch, victim);
    return;
}

void write_wizlist( CHAR_DATA *ch, CHAR_DATA *victim)
{
    FILE *fp;
    int lvl[200];
    int count, i;
    char name[200] [MAX_STRING_LENGTH];

    char buf[MAX_STRING_LENGTH];

    if ( ( fp = fopen( GOD_FILE, "w+" ) ) == NULL )
        return;

    lvl[0] = 1;
    for(count = 0; lvl[count] != 0; count ++)
    {
        strcpy(name[count],fread_word(fp));
        lvl[count] = fread_number(fp);
    }

    fprintf(fp,"%s %d",victim->name, victim->level);

    for(i = 0; i < count; i++)
        fprintf(fp,"%s %d", name[i], lvl[i]);

    sprintf(buf, "%s added to Wizlist!", victim->name);
    send_to_char(buf,ch);
    fclose(fp);
return;
}
*/


void do_lagem( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_STRING_LENGTH];
    char arg2 [MAX_STRING_LENGTH];
    int length;


    argument = one_argument(argument,arg);
    argument = one_argument(argument,arg2);

    if(arg[0] == '\0' || arg2[0] == '\0')
    send_to_char( "Sentax: lag <victim> <time>\n\r", ch );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL)
    {
    send_to_char( "Lag whom?\n\r", ch );
    return;
    }

    if(IS_NPC(victim))
    {
    send_to_char( "Not on mobs!\n\r", ch );
    return;
    }
    length = atoi(arg2);

    WAIT_STATE( victim, length );
    send_to_char( "You Laaaaaaaag them!\n\r", ch );

    return;
}

void do_setarena( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    int room;
    char arg [MAX_STRING_LENGTH];

    argument = one_argument(argument,arg);

    if(arg[0] == '\0')
    send_to_char( "Sentax: setarena <victim>\n\r", ch );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL)
    {
    send_to_char("Set whom to arena?\n\r", ch );
    return;
    }

    if(IS_NPC(victim))
    {
    send_to_char( "Not on mobs!\n\r", ch );
    return;
    }
    room = number_range(1,50);
    while (get_room_index( 26500+room ) == NULL)
        room = number_range(1,50);

    if (victim->in_room != NULL)
        char_from_room( victim );
    char_to_room( victim, get_room_index( 26500+room ) );
    if ( is_affected( victim, gsn_claws ) )
        do_function(victim, &do_claws, "");
    while ( victim->affected )
        affect_remove( victim, victim->affected );
    SET_BIT(victim->act, PLR_ARENA);
    af.where     = TO_AFFECTS;
    af.type      = gsn_arena;
    af.level     = 110;
    af.duration  = 10;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    ch->pblood      = 100;
    ch->rage        = 0;
    affect_to_char( victim, &af );
    send_to_char( "Arena set.\n\r", ch );
    die_follower(victim);
    send_to_char( "{BYou fill a rush of adrenaline as YOUR name is called to enter the {RArena!{x\n\r", victim );
    nuke_pets(ch);
    victim->affected_by = race_table[victim->race].aff;
    victim->position    = POS_STANDING;
    victim->hit     = victim->max_hit;
    victim->mana        = victim->max_mana;
    victim->move        = victim->max_move;
/*  restore_affects(ch); */
    announce(victim, ch, WIZ_LOAD);
    do_function(victim, &do_look, "auto" );
    act( "{BYou enter the {RARENA!{x",  ch, NULL, victim, TO_VICT    );
    act( "{B$N suddenly disappears as their name is called to enter the {RARENA!{x",  ch, NULL, victim, TO_ROOM );
    return;
}

void do_arename( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int room;
    extern bool arena;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    {
        send_to_char( "You cannot arena from a room you cannot recall from.\n\r", ch );
        return;
    }
    if(IS_SET(ch->act, PLR_ARENA))
    {
        send_to_char("You are already in the Arena.\n\r",ch);
        return;
    }

    if(arena)
    {
        send_to_char( "{BThe {RArena{B is now closed.{x\n\r", ch );
        return;
    }

    if(ch->gold < 500)
    {
        send_to_char( "You cannot afford the 500 gold to enter the Arena.\n\r", ch );
        return;
    }

    room = number_range(1,50);
    while (get_room_index( 26500+room ) == NULL)
        room = number_range(1,50);

    if (ch->in_room != NULL)
        char_from_room( ch );
    char_to_room( ch, get_room_index( 26500+room ) );
    SET_BIT(ch->act, PLR_ARENA);
    ch->gold -= 500;
    nuke_pets(ch);
    die_follower(ch);
    send_to_char( "You are charged 500 gold to enter the {RArena{x\n\r", ch );
    send_to_char( "{BYou fill a rush of adrenaline as YOUR name is called to enter the {RArena!{x\n\r", ch );
    if ( is_affected( ch, gsn_claws ) )
        do_function(ch, &do_claws, "");


    while ( ch->affected )
        affect_remove( ch, ch->affected );
    ch->affected_by = race_table[ch->race].aff;
    ch->position    = POS_STANDING;
    ch->hit     = ch->max_hit;
    ch->mana        = ch->max_mana;
    ch->move        = ch->max_move;
    ch->pblood      = ch->max_pblood;
    if(clan_table[ch->clan].clan_type == TYPE_TRADITION)
    {
        ch->quintessence = ch->max_quintessence;
        if(ch->paradox)
            ch->paradox = 0;
    }
    ch->rage        = 0;
    af.where     = TO_AFFECTS;
    af.type      = gsn_arena;
    af.level     = 110;
    af.duration  = 10;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    announce(ch, ch, WIZ_LOAD);
    do_function(ch, &do_look, "auto" );
    act( "{BYou enter the {RARENA!{x",  ch, NULL, ch, TO_VICT    );
    act( "{B$N suddenly disappears as their name is called to enter the {RARENA!{x",  ch, NULL, ch, TO_ROOM );
    return;

}

void do_spectate( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    int room;
    extern bool arena;

    if ( IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_AFFECTED(ch, AFF_CURSE))
    {
        send_to_char( "You cannot arena from a room you cannot recall from.\n\r", ch );
        return;
    }
    if(!arena)
    {
        send_to_char( "{BThe {RArena{B is now closed.{x\n\r", ch );
        return;
    }

    if(ch->gold < 25)
    {
        send_to_char( "You cannot afford the 25 gold to enter the Arena.\n\r", ch );
        return;
    }

    room = number_range(1,50);
    while (get_room_index( 26500+room ) == NULL)
        room = number_range(1,50);

    if (ch->in_room != NULL)
        char_from_room( ch );
    char_to_room( ch, get_room_index( 26500+room ) );
    SET_BIT(ch->act, PLR_SPEC);
    af.where     = TO_AFFECTS;
    af.type      = gsn_arena;
    af.level     = 110;
    af.duration  = 10;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    ch->gold -= 50;
    die_follower(ch);
    send_to_char( "You are charged 50 gold to enter the {RArena{x\n\r", ch );
    nuke_pets(ch);
    do_function(ch, &do_look, "auto" );
    act( "{BYou enter the {RARENA!{x",  ch, NULL, ch, TO_VICT    );
    send_to_char( "{GRemember, you are in the {RArena{G only to watch. Do not interfere!{x\n\r", ch );
    act( "{G$N suddenly disappears as they go to witness the {RARENA!{x",  ch, NULL, ch, TO_ROOM );
    return;
}
void do_gxp(CHAR_DATA *ch, char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char buf[MAX_INPUT_LENGTH];
int gxp;

argument = one_argument (argument, arg1);
gxp = atoi(arg1);
sprintf(buf,"Adding %d to global XP\n", gxp);
send_to_char(buf,ch);

global_xp += gxp;
}

void do_gqp(CHAR_DATA *ch, char *argument)
{
char arg1[MAX_INPUT_LENGTH];
char buf[MAX_INPUT_LENGTH];
int gqp;

argument = one_argument (argument, arg1);
gqp = atoi(arg1);
sprintf(buf,"Adding %d to global QP\n", gqp);
send_to_char(buf,ch);

global_qp += gqp;
}

void do_ipoint(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int ipoints;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );


    if((victim = get_char_world(ch, arg1)) == NULL)
    {
        send_to_char("Who?\n\r",ch);
        send_to_char("Syntax: ipoint <name> <value>\n\r Set value to 0 to view player's current immortal points\n\r",ch);
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char("Not on mobs!\n\r",ch);
        return;
    }

    if(!is_number(arg2))
    {
        send_to_char("That is an invalid value.\n\r",ch);
        send_to_char("Syntax: ipoint <name> <value>\n\r Set value to 0 to view player's current immortal points\n\r", ch);
        return;
    }
     ipoints = atoi(arg2);
    if(ipoints == 0)
    {
        sprintf(buf, "{W%s {xhas {Y%d {ximmortal points.\n\r", victim->name, victim->pcdata->ip);
        send_to_char(buf,ch);
        return;
    }
    if(ipoints > 0)
    {
        sprintf(buf, "You have been rewarded {Y%d {ximmortal points!\n\r", ipoints);
        send_to_char(buf,victim);
    }
    if(ipoints < 0)
    {
        sprintf(buf, "{R%d {ximmortal points have been removed from you!\n\r", abs(ipoints));
        send_to_char(buf,victim);
    }




    victim->pcdata->ip += ipoints;
    sprintf(buf, "{W%s {xnow has {Y%d {ximmortal points.\n\r", victim->name, victim->pcdata->ip);
    send_to_char(buf,ch);
    sprintf(buf, "You now have {Y%d {ximmortal points\n\r", victim->pcdata->ip);
    send_to_char(buf, victim);
    return;
}

void do_resetkill(CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg1[MAX_INPUT_LENGTH];
char buf[MAX_INPUT_LENGTH];

argument = one_argument( argument, arg1);

if((victim = get_char_world(ch, arg1)) == NULL)
{
	send_to_char("Who?\n\r",ch);
	send_to_char("Syntax: resetkill <target>\n\r",ch);
        return;
}
if(IS_NPC(victim))
{
	send_to_char("Not on mobs, what is wrong with you?\n\r",ch);
	return;
}
victim->currentkills = 0;

	sprintf(buf, "Your current kills have been reset.\n\r" );
	send_to_char(buf,victim);

        sprintf(buf, "You have reset the current kills of %s.\n\r", victim->name);
        send_to_char(buf,ch);
}

void do_qpoint(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int points;

    if ((IS_NPC(ch)) || (!IS_IMMORTAL(ch)))
    return;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if((victim = get_char_world(ch, arg1)) == NULL)
    {
        send_to_char("Who?\n\r",ch);
        send_to_char("Syntax: qpoint <name> <value>\n\r Set value to 0 to view player's current quest points.\n\r",ch);
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char("Not on mobs!\n\r",ch);
        return;
    }

    if(!is_number(arg2))
    {
        send_to_char("That is an invalid value.\n\r",ch);
        send_to_char("Syntax: qpoint <name> <value>\n\r Set value to 0 to view player's current quest points.\n\r",ch);
        return;
    }
    points = atoi(arg2);

    if(points > 30000 || points < -30000)
    {
        send_to_char("Valid values are -30000 to 30000.\n\r",ch);
        return;
    }

    if(victim->qpoints+points > MAX_QPOINTS)
    {
        sprintf(buf, "A player's quest points cannot exceed %d.\n\r", MAX_QPOINTS);
        send_to_char(buf,ch);
        return;
    }

/*Sengir added a check so you can't put a player negative*/
    if(victim->qpoints+points < 0)
    {
        send_to_char("That would leave the player with negative quest points!.\n\r", ch);
        return;
    }

    if(points == 0)
    {
        sprintf(buf, "{W%s {xhas {Y%d {xquest points.\n\r", victim->name, victim->qpoints);
        send_to_char(buf,ch);
        return;
    }

    if(points > 0)
    {
        victim->totalqpoints += points;
        sprintf(buf, "You have been rewarded {Y%d {xquest points!\n\r", points);
        send_to_char(buf,victim);
    }
    if(points < 0)
    {
        sprintf(buf, "{R%d {xquest points have been removed from you!\n\r", abs(points));
        send_to_char(buf,victim);
    }

    victim->qpoints += points;
    sprintf(buf, "{W%s {xnow has {Y%d {xquest points.\n\r", victim->name, victim->qpoints);
    send_to_char(buf,ch);
    sprintf(buf, "You now have {Y%d {xquest points.\n\r", victim->qpoints);
    send_to_char(buf, victim);
    return;
}

void do_frpoint(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int points;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if((victim = get_char_world(ch, arg1)) == NULL)
    {
        send_to_char("Who?\n\r",ch);
        send_to_char("Syntax: frpoint <name> <value>\n\r Set value to 0 to view player's current freebie points.\n\r",ch);
        return;
    }
    if(IS_NPC(victim))
    {
        send_to_char("Not on mobs!\n\r",ch);
        return;
    }

    if(!is_number(arg2))
    {
        send_to_char("That is an invalid value.\n\r",ch);
        send_to_char("Syntax: frpoint <name> <value>\n\r Set value to 0 to view player's current freebie points.\n\r",ch);
        return;
    }
    points = atoi(arg2);

    if(points > 30000 || points < -30000)
    {
        send_to_char("Valid values are -30000 to 30000.\n\r",ch);
        return;
    }

    if(victim->freebie+points > 30000)
    {
        send_to_char("A player's freebie points cannot exceed 30000.\n\r",ch);
        return;
    }

    if(victim->freebie+points < 0)
    {
        send_to_char("That would leave the player with negative freebie points!.\n\r", ch);
        return;
    }

    if(points == 0)
    {
        sprintf(buf, "{W%s {xhas {Y%d {xfreebie points.\n\r", victim->name, victim->freebie);
        send_to_char(buf,ch);
        return;
    }

    if(points > 0)
    {
        sprintf(buf, "You have been rewarded {Y%d {xfreebie points!\n\r", points);
        send_to_char(buf,victim);
    }
    if(points < 0)
    {
        sprintf(buf, "{R%d {xfreebie points have been removed from you!\n\r", abs(points));
        send_to_char(buf,victim);
    }

    victim->freebie += points;
    sprintf(buf, "{W%s {xnow has {Y%d {xfreebie points.\n\r", victim->name, victim->freebie);
    send_to_char(buf,ch);
    sprintf(buf, "You now have {Y%d {xfreebie points.\n\r", victim->freebie);
    send_to_char(buf, victim);
    return;
}
void do_globalpower( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    extern int most_players;
    extern bool slaughter;
    extern bool arena;
    extern bool nosun;
    extern bool doubleexp;
    extern bool manualxp;
    extern bool doubledam;
    extern bool spookums;
    extern bool resolver;
    extern bool newlock;
    extern bool wizlock;
    extern bool DEBUG_MESSAGES;


    int i;

    argument = one_argument( argument, arg );

    if(!str_cmp(arg, "arena"))
    {

        arena = !arena;

        if ( arena )
        {
            wiznet("$N has closed the Arena.",ch,NULL,0,0,0);
            send_to_char( "Arena Closed.\n\r", ch );
        }
        else
        {
            wiznet("$N has opened the Arena.",ch,NULL,0,0,0);
            send_to_char( "Arena Open.\n\r", ch );
        }
        return;
    }
    if(!str_cmp(arg, "debug"))
    {

        DEBUG_MESSAGES = !DEBUG_MESSAGES;
        sendch("Debug Messages toggled.", ch);
        return;
    }

    if(!str_cmp(arg,"nosun"))
    {

        if(nosun)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{YThe clouds slowly depart and the sun is free to shine once more.{x\n\r", d->character );
                }
            }
            nosun = FALSE;
        }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{DDark clouds swirl in the heavens, thickening and pulsing until they block out the sun!{x\n\r", d->character );
                }
            }
            nosun = TRUE;
        }
        return;
    }
    if(!str_cmp(arg,"doubleexp") || !str_cmp(arg, "doublexp") )
    {
        if(manualxp)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{G****[{?M{?u{?l{?t{?i{Y Exp Override has ended {y:-( {G]****{x\n\r", d->character );
                }
            }
            manualxp = FALSE;
            if (!doubleexp)
                xpawardmult = 1;
            else {
                if (xpstat <=3)
                    xpawardmult = 2;
                if (xpstat > 3)
                    xpawardmult = 3;
            }
        }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{G****{YIt's {RD{Go{Mu{Yb{Bl{Ce {RE{Gx{Mp{Y time!{G****{x\n\r", d->character );
                }
            }
            manualxp = TRUE;
            xpawardmult = 2;
        }
        return;
    }
    if(!str_cmp(arg,"tripleexp") || !str_cmp(arg, "triplexp") )
    {
        if(manualxp)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{G****[{?M{?u{?l{?t{?i{Y Exp Override has ended {y:-( {G]****{x\n\r", d->character );
                }
            }
            manualxp = FALSE;
            if (!doubleexp)
                xpawardmult = 1;
            else {
                if (xpstat <=3)
                    xpawardmult = 2;
                if (xpstat > 3)
                    xpawardmult = 3;
            }
        }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{R****{YIt's {RT{Gr{Mi{Yp{Bl{Ce {RE{Gx{Mp{Y time!{R****{x\n\r", d->character );
                }
            }
            manualxp = TRUE;
            xpawardmult = 3;
        }
        return;
    }
    if(!str_cmp(arg, "slaughter") || !str_cmp(arg,"slaughterfest"))
    {

        if(slaughter)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{DSlaughter-fest has ended. You can stop being all blood-lusty now.{x\n\r", d->character );
                }
            }
            slaughter = FALSE;
            nosun = FALSE;
            }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{YIt's {RSLAUGHTER-FEST!{Y Time! Get to {rKILLING!{x\n\r", d->character );
                }
            }
            slaughter = TRUE;
            nosun = TRUE;
        }
        return;
    }
    if(!str_cmp(arg,"spookums"))
    {

        if(spookums)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{YThe mystic spookiness abates, leaving the world back to its normal state.{x\n\r", d->character );
                }
            }
            spookums = FALSE;
        }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{DA dark spookiness settles across the land!{x\n\r", d->character );
                }
            }
            spookums = TRUE;
        }
        return;
    }
if(!str_cmp(arg,"resolver"))
    {

        if(resolver)
            resolver = FALSE;
        else
            resolver = TRUE;

        resolver ? sendch("Resolver is now on.\n\r", ch) : sendch("Resolver is now off.\n\r", ch);

        return;
    }

    if(!str_cmp(arg,"doubledam"))
    {

        if(doubledam)
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{DDouble Damage has ended. The frenzied rage within you subsides.{x\n\r", d->character );
                }
            }
            doubledam = FALSE;
        }
        else
        {

            for ( d = descriptor_list; d; d = d->next )
            {
                if ( d->connected == CON_PLAYING )
                {
                    send_to_char( "{rMurderous rage fills your mind as the Gods bless you with Double Damage!{x\n\r", d->character );
                }
            }
            doubledam = TRUE;
        }
        return;
    }
    if(!str_prefix(arg,"status"))
    {
        send_to_char("Global status:\n\r",ch);
        send_to_char("Manual Multi-xp is ", ch);
        manualxp ? send_to_char("{GON{x\n\r", ch) : send_to_char("{ROFF{x\n\r", ch);
        send_to_char("Slaughterfest is ", ch);
        slaughter ? send_to_char("{GON{x\n\r", ch) : send_to_char("{ROFF{x\n\r", ch);
        sendch("Double dam is ", ch);
        doubledam ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        send_to_char("Double exp is ", ch);
        doubleexp ? send_to_char("{GON{x\n\r", ch) : send_to_char("{ROFF{x\n\r", ch);
        sendch("Nosun is ", ch);
        nosun ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("Spookums is ", ch);
        spookums ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("Resolver is ", ch);
        resolver ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("Wizlock is ", ch);
        wizlock ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("Newlock is ", ch);
        newlock ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("Debug Messages are ", ch);
        DEBUG_MESSAGES ? sendch("{GON{x\n\r", ch) : sendch("{ROFF{x\n\r", ch);
        sendch("The Arena is ", ch);
        arena ? sendch("{RCLOSED{x\n\r", ch) : sendch("{GOPEN{x\n\r", ch);
        sprintf(buf, "Global XP: %d\n\r", global_xp);
        send_to_char(buf, ch);
        sprintf(buf, "Global QP: %d\n\r", global_qp);
        send_to_char(buf, ch);
        sprintf(buf, "XP Multiplier: %d\n\r", xpawardmult);
        send_to_char(buf, ch);
        send_to_char("\n\r{wValid options are: {Darena{w, {Ddoubleexp{w, {Dtripleexp{w, {Ddoubledam{w, {Dnosun{w,\n\r", ch);
        send_to_char("{Dslaughterfest{w, {Dspookums{w, {Dresolver{w, {Ddebug{w, {Dstatus{x\n\r",ch);

        return;
    }
            send_to_char("\n\r{wValid options are: {Darena{w, {Ddoubleexp{w, {Ddoubledam{w, {Dnosun{w,\n\r", ch);
        send_to_char("{Dslaughterfest{w, {Dspookums{w, {Dresolver{w, {Ddebug{w, {Dstatus{x\n\r",ch);
    return;
}
void do_portal( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *location;
    CHAR_DATA *rch;
    OBJ_DATA *portal;
    int count = 0;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Portal to where?\n\r", ch );
    return;
    }

    if ( ( location = find_location( ch, argument ) ) == NULL )
    {
    send_to_char( "No such location.\n\r", ch );
    return;
    }

    count = 0;
    for ( rch = location->people; rch != NULL; rch = rch->next_in_room )
        count++;

    for (rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room)
        act("$n waves $s hand and a shimmering portal to the beyond appears.",ch,NULL,rch,TO_VICT);

    for (rch = location->people; rch != NULL; rch = rch->next_in_room)
        act("A shimmering portal suddenly appears before you.",ch,NULL,rch,TO_VICT);

    send_to_char("You wave your hand and a portal suddenly appears.",ch);

    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 5;
    portal->value[3] = location->vnum;
    portal->level = 1;

    obj_to_room(portal,ch->in_room);

    /* portal two */
    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = 1;
    portal->value[3] = ch->in_room->vnum;
    portal->level = 1;

    obj_to_room(portal,location);

    return;
}

void do_obliterate( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int i=0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char("Syntax:\n\r",ch);
        send_to_char("obliterate <victim> <kill>\n\r",ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char("Destroy whom?\n\r",ch);
        return;
    }

    if (victim->level > ch->level)
    {
        send_to_char("How can you destroy someone so much more powerful then you?\n\r",ch);
        return;
    }


    for(i=1; i < 6; i++)
    {
        OBJ_DATA *obj;
        char *name;
        int vnum=0;

        switch(i)
        {
            case 1: vnum = OBJ_VNUM_GUTS;       break;
            case 2: vnum = OBJ_VNUM_SEVERED_HEAD;   break;
            case 3: vnum = OBJ_VNUM_TORN_HEART; break;
            case 4: vnum = OBJ_VNUM_SLICED_ARM; break;
            case 5: vnum = OBJ_VNUM_SLICED_LEG; break;
            case 6: vnum = OBJ_VNUM_BRAINS;     break;
        }
        obj = create_object( get_obj_index( vnum ), 0 );
        name        = IS_NPC(victim) ? victim->short_descr : victim->name;
        obj->timer  = number_range( 4, 7 );

        sprintf( buf, obj->short_descr, name );
        free_string( obj->short_descr );
        obj->short_descr = str_dup( buf );

        sprintf( buf, obj->description, name );
        free_string( obj->description );
        obj->description = str_dup( buf );

        if (obj->item_type == ITEM_FOOD)
        {
            if (IS_SET(victim->form,FORM_POISON))
                obj->value[3] = 1;
            else if (!IS_SET(victim->form,FORM_EDIBLE))
                obj->item_type = ITEM_TRASH;
        }

        obj_to_room( obj, victim->in_room );
    }

    act( "You point your finger at $M and utter in a cold, lifeless voice... '{RIt is time.{x'", ch, NULL, victim, TO_CHAR);
    act( "$N suddenly grabs $S chest and moans as if in great pain, then $S flesh begins to extend and contract as $S pulse goes wild, and then...\n\r{R*BOOM*{x $N explodes into a million pieces!{x",  ch, NULL, victim, TO_CHAR );
    act( "$n points $s finger at you and utters in a cold, lifeless voice... '{RIt is time.{x'", ch, NULL, victim, TO_VICT    );
    act( "You groan as a strong pain in your chest begins to grow until it overwhelms all thought, and then..\n\r{R*BOOM*{x You explode into a million pieces!{x",  ch, NULL, victim, TO_VICT );
    act( "$n points $s finger at $N and utters in a cold, lifeless voice... '{RIt is time.{x'", ch, NULL, victim, TO_NOTVICT);
    act( "$N suddenly grabs $S chest and moans as if in great pain, then $S flesh begins to extend and contract as $S pulse goes wild, and then...\n\r{R*BOOM*{x $N explodes into a million pieces!{x",  ch, NULL, victim, TO_NOTVICT );
    if(!str_cmp(arg2, "kill")) raw_kill( victim );

    sprintf(buf, "{R*{YAnnouncement{R*{w %s has been obliterated by the awesome powers of %s.{x\n\r", victim->name, ch->name);
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char( buf, d->character );
        }
    }

    return;
}

void do_salvation( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA af;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char("Give divine aid to whom?\n\r",ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char("Give divine aid to whom?\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_touch;
    af.level     = ch->level;
    af.duration  = 25;
    af.modifier  = -500;
    af.location  = APPLY_AC;
    af.bitvector = AFF_SANCTUARY;
    affect_to_char( victim, &af );

    af.modifier  = -20;
    af.location  = APPLY_SAVES;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.modifier  = 50;
    af.location  = APPLY_HITROLL;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );

    af.modifier  = 50;
    af.location  = APPLY_DAMROLL;
    af.bitvector = AFF_HASTE;
    affect_to_char( victim, &af );

    af.modifier  = 1000;
    af.location  = APPLY_HIT;
    af.bitvector = AFF_FLYING;
    affect_to_char( victim, &af );

    af.modifier  = 1000;
    af.location  = APPLY_MANA;
    af.bitvector = AFF_DARK_VISION;
    affect_to_char( victim, &af );

    af.modifier  = 1000;
    af.location  = APPLY_MOVE;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.where     = TO_IMMUNE;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = IMM_SUNLIGHT;
    affect_to_char( ch, &af );

    act( "You grant divine favor upon $N!", ch, NULL, victim, TO_CHAR);
    act( "$n grants divine salvation and favor upon you!", ch, NULL, victim, TO_VICT    );

    return;
}

// XP bonus command
void do_xp_boost( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA af;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char("Enhhance whose experience?\n\r",ch);
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char("Give an experience boost to who?\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type  = gsn_xp_boost;
    af.level     = ch->level;
    af.duration  = 25;
    af.modifier  =  0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_XP_BOOST;
    affect_to_char( victim, &af );


    act( "You grant bonus experience gains to $N!", ch, NULL, victim, TO_CHAR);
    act( "$n has granted you an experience boost!", ch, NULL, victim, TO_VICT    );

    return;
}


/*
void do_dice(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    int num1,num2, success, roll;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if(arg1 == NULL || arg2 == NULL)
    {
        send_to_char("Syntax: <number of dice> <show to room y/n> <difficulty>\n\r" ,ch);
        return;
    }

    if(!is_number( arg1 ))
    {
        send_to_char("Only numbers are valid.\n\r" ,ch);
        return;

    }
    if(!strcmp(arg2,"yes") && !strcmp(arg2,"no"))
    {
        send_to_char("Only yes or no answers please.\n\r",ch);
        return;
    }
    num1   = atoi(arg1);

    if(num1 > 25 || num1 < -25)
    {
        send_to_char("Too many dice!\n\r",ch);
        return;
    }
    num2 = atoi(arg3);

    if(num2 > 10 || num2 < 0)
    {
        send_to_char("Incorrect difficulty.",ch);
        return;
    }
    send_to_char("Rolling numbers...\n\r" ,ch);
    success = 0;
    while(num1 > 0)
    {
        sprintf(buf,"$n rolls %d\n\r",(roll = number_range(1,10)));
        if(!strcmp(arg2,"yes"))
            act( buf, ch, NULL, NULL, TO_ROOM );
        act( buf, ch, NULL, NULL, TO_CHAR );
        if(roll >= num2) success++;
        else if(roll == 1) success--;
        num1--;
    }
    if(success == 0)
    sprintf(buf, "Fail!\n\r");
    else if (success == -1)
    sprintf(buf, "BOTCH!\n\r");
    else sprintf(buf, "%d successes.\n\r", success);
    if(!strcmp(arg2, "yes"))
        act(buf, ch, NULL, NULL, TO_ROOM);
    act(buf, ch, NULL, NULL, TO_CHAR);
    return;
}
*/

void do_dice(CHAR_DATA *ch, char *argument)
{
    char arg [4] [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    int dice, diff, roll, success, botch, i;
    bool show = FALSE;
    bool private = FALSE;

    for(i = 0; i < 4;i++)
        argument = one_argument(argument, arg[i]);

    if(arg[0][0] == '\0' || !is_number(arg[0]) || (dice = atoi(arg[0])) == 0)
    {
        send_to_char("Syntax is: dice <number> <difficulty> <show> <private>\n\r"
        "<number> refers to the number of ten-sided dice to be used.\n\r"
        "<difficulty> refers to the result required for the dice roll to be a success.\n\r"
        "<show> is an optional tag to show the result of each dice instead of just the total.\n\r"
        "<private> is an optional tag to show the results to the entire room.\n\r"
        "The defaults are 6 difficulty with show and private disabled.\n\r"
        "Example: dice 8 6 show private\n\r",ch);
        return;
    }
    if(dice > 50)
    {
        send_to_char("The maximum number of dice you can roll is 50.\n\r",ch);
        return;
    }

    for(i = 0;i < 4;i++)
    {
        if(!str_cmp(arg[i], "show")) show = TRUE;
        if(!str_cmp(arg[i], "private")) private = TRUE;
    }
    if(is_number(arg[1])) diff = atoi(arg[1]);
    else diff = 6;

    if(!private)
    {
        send_to_char("Rolling... (Public)\n\r",ch);
        act("$n starts a diceroll...",ch, NULL, NULL, TO_ROOM);
    }
    else send_to_char("Rolling... (Private)\n\r",ch);

    success = botch = 0;
    for(i = 0;i < dice;i++)
    {
        if((roll = number_range(1,10)) >= diff) success++;
        if(roll == 1) botch++;
        sprintf(buf,"The roll is %d (%s)",roll, roll >= diff ? "success" : roll == 1 ? "botch" : "fail");
        if(show)
        {
            if(!private) act(buf,ch,NULL,NULL, TO_ROOM);
            act(buf,ch,NULL,NULL,TO_CHAR);
        }
    }
    if(success-botch >= 1)
        sprintf(buf,"{G%d Successes!{x",success-botch);
    else if(success-botch == 0)
        sprintf(buf,"{YNo successes. Fail.{x");
    else sprintf(buf,"{RBOTCH!{x");

    if(!private) act(buf,ch,NULL,NULL, TO_ROOM);
    act(buf,ch,NULL,NULL,TO_CHAR);
    return;
}

void do_immmask(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if ( argument[0] == '\0')
    {
        if (is_affected( ch, gsn_mask ))
        {
            send_to_char( "You return to your normal form.\n\r", ch );
            affect_strip(ch,gsn_mask);
        }
        else
            send_to_char("Assume a mask of whom?\n\r",ch);
        return;
    }
    if ((victim = get_char_room( ch, NULL, argument ))== NULL)
    {
        send_to_char( "Assume a mask of whom?\n\r", ch );
        return;
    }

    if(!IS_NPC(victim))
    {
        send_to_char("You may not assume the guise of a player.\n\r",ch);
        return;
    }

    if (is_affected( ch, gsn_vicissitude_horrid ) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("Return to your normal form first.\n\r",ch);
        return;
    }
    act( "You assume a mask of $N", ch, NULL, victim, TO_CHAR );
    sprintf(buf, "%s (%s)",victim->short_descr,ch->name);
        ch->short_descr = str_dup( buf );
    sprintf(buf, "%s%s",victim->short_descr,ch->pcdata->title);
            ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_mask;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
return;
}


void do_plead( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      send_to_char("What do you need help from the immortals with?\n\r",ch);
      return;
    }
   else
    {
        sprintf( buf, "{WYou plead to the immortals:'{Y*{R%s{Y*{x'\n\r", argument );
         send_to_char( buf, ch );
         for ( d = descriptor_list; d != NULL; d = d->next )
         {
           CHAR_DATA *victim;

           victim = d->original ? d->original : d->character;

           if ( d->connected == CON_PLAYING &&
                d->character != ch && IS_IMMORTAL(d->character))
                {
                    if(!IS_NPC(ch))
                        sprintf( buf, "{W%s pleads: '{Y*{R%s{Y*{<*{x'\n\r", ch->name,argument);
                    else
                        sprintf( buf, "{W%s pleads: '{Y*{R%s{Y*{<*{x'\n\r", ch->short_descr, argument);
                    send_to_char( buf, d->character );
                }
         }
     }
    WAIT_STATE(ch,10);
    return;
}

/** Function: do_pload
  * Descr   : Loads a player object into the mud, bringing them (and their
  *           pet) to you for easy modification.  Player must not be connected.
  *           Note: be sure to send them back when your done with them.
  * Returns : (void)
  * Syntax  : pload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_pload( CHAR_DATA *ch, char *argument )
{
  DESCRIPTOR_DATA d;
  bool isChar = FALSE;
  char name[MAX_INPUT_LENGTH];

  if (argument[0] == '\0')
  {
    send_to_char("Load who?\n\r", ch);
    return;
  }

  argument[0] = UPPER(argument[0]);
  argument = one_argument(argument, name);

  /* Dont want to load a second copy of a player who's allready online! */
  if ( get_char_world( ch, name ) != NULL )
  {
    send_to_char( "That person is allready connected!\n\r", ch );
    return;
  }

  isChar = load_char_obj(&d, name); /* char pfile exists? */

  if (!isChar)
  {
    send_to_char("Load Who? Are you sure? I cant seem to find them.\n\r", ch);
    return;
  }

  d.character->desc     = NULL;
  d.character->next     = char_list;
  char_list             = d.character;
  d.connected           = CON_PLAYING;
  reset_char(d.character);

  /* bring player to imm */
  if ( d.character->in_room != NULL )
  {
    char_to_room( d.character, ch->in_room); /* put in room imm is in */
  }
  act( "You have loaded $N.", ch, NULL, d.character, TO_CHAR);
  act( "$n has pulled $N from the pattern!",
        ch, NULL, d.character, TO_ROOM );

  if (d.character->pet != NULL)
   {
     char_to_room(d.character->pet,d.character->in_room);
     act("$n has entered the game.",d.character->pet,NULL,NULL,TO_ROOM);
   }

}

/** Function: do_punload
  * Descr   : Returns a player, previously 'ploaded' back to the void from
  *           whence they came.  This does not work if the player is actually
  *           connected.
  * Returns : (void)
  * Syntax  : punload (who)
  * Written : v1.0 12/97
  * Author  : Gary McNickle <gary@dharvest.com>
  */
void do_punload( CHAR_DATA *ch, char *argument )
{
  CHAR_DATA *victim;
  char who[MAX_INPUT_LENGTH];

  argument = one_argument(argument, who);

  if ( ( victim = get_char_world( ch, who ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

  /** Person is legitametly logged on... was not ploaded.
   */
  if (victim->desc != NULL)
  {
    send_to_char("I dont think that would be a good idea...\n\r", ch);
    return;
  }

  if (victim->was_in_room != NULL) /* return player and pet to orig room */
  {
    char_to_room(victim, victim->was_in_room);
    if (victim->pet != NULL)
      char_to_room(victim->pet, victim->was_in_room);
  }
    act( "You have unloaded $N.", ch, NULL, victim, TO_CHAR);
      act("$n has released $N back to the Pattern.",
       ch, NULL, victim, TO_ROOM);
  save_char_obj(victim);
  extract_char(victim, TRUE);


}
void do_forceinfluence(CHAR_DATA *ch, char *argument) {
    wiznet("Influence Tick.", NULL, NULL, WIZ_TICKS, 0, 0);
    influence_update  ( );
    sendch("Done.\n\r", ch);
    return;
}
void do_forcetick(CHAR_DATA *ch, char *argument) {
    pulse_point = 1;
    sendch("Done.\n\r", ch);
    return;
}

void do_forcebank(CHAR_DATA *ch, char *argument)
{
bank_update();
send_to_char("Banks updated.\n\r", ch);
return;
}

void do_makehelper (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg[MIL];
one_argument( argument, arg );
  if ( ( victim = get_char_world( ch, arg  ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

if (IS_SET(victim->act, PLR_NEWBIEHELP))
    {
    REMOVE_BIT(victim->act, PLR_NEWBIEHELP);
    send_to_char("Newbiehelper tag removed.\n\r", ch);
    send_to_char("You are no longer a newbie helper!\n\r", victim);
    }
    else {
    SET_BIT(victim->act, PLR_NEWBIEHELP);
    send_to_char("Newbiehelper tag set.\n\r", ch);
    send_to_char("You have been bestowed with the title Newbie Helper!\n\r", victim);
    }
    return;
    }

void do_playtest (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg[MIL];
one_argument( argument, arg );
  if ( ( victim = get_char_world( ch, arg  ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

if (IS_SET(victim->act, PLR_PLAYTEST))
    {
    REMOVE_BIT(victim->act, PLR_PLAYTEST);
    send_to_char("Play Tester tag removed.\n\r", ch);
    send_to_char("You are no longer a play tester.\n\r", victim);
    }
    else {
    SET_BIT(victim->act, PLR_PLAYTEST);
    send_to_char("Play Tester tag set.\n\r", ch);
    send_to_char("You have been set as a play tester!\n\r", victim);
    }
    return;
    }

void do_debugger (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg[MIL];
one_argument( argument, arg );
  if ( ( victim = get_char_world( ch, arg  ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

if (IS_SET(victim->act2, PLR2_DEBUG))
    {
    REMOVE_BIT(victim->act2, PLR2_DEBUG);
    send_to_char("They are no longer flagged as a Debugger.\n\r", ch);
    send_to_char("You are no longer a Debugger.\n\r", victim);
    }
    else {
    SET_BIT(victim->act2, PLR2_DEBUG);
    send_to_char("They are now flagged as a Debugger.\n\r", ch);
    send_to_char("You have been flagged as a Debugger!\n\r", victim);
    }
    return;
    }

void do_debugmsg (CHAR_DATA *ch, char *argument)
{

    if (!IS_SET(ch->act2, PLR2_DEBUG))
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }
    if (IS_SET(ch->act2, PLR2_DEBUGMSG))
        {
        REMOVE_BIT(ch->act2, PLR2_DEBUGMSG);
        send_to_char("You will no longer see debug messages.\n\r", ch);
        }
        else {
        SET_BIT(ch->act2, PLR2_DEBUGMSG);
        send_to_char("You will now see debug messages.\n\r", ch);
        }

    return;
    }

//Matthew - added to change all pfiles back to encrypted passwords.
void recrypt_pfiles( CHAR_DATA *ch, char *argument )
{
    DIR *dp;
    struct dirent *ep;
    DESCRIPTOR_DATA d;
    char    buf[MSL];
    bool isChar = FALSE;

    dp = opendir ("../player");
    if (dp != NULL)
    {
    while ( (ep = readdir (dp)) )
        {
        if ( ep->d_name[0] == '.' )
            continue;

    if ( get_char_world( ch, ep->d_name ) != NULL )
        continue;

    isChar = load_char_obj( &d, ep->d_name );
  if (!isChar)
  {
    bug( "recrypt_pfiles: pfile not found.", 0 );
    sprintf(buf, "Pfile name: %s", ep->d_name);
    bug (buf, 0);
    continue;
  }
    d.character->desc     = NULL;
    d.character->next     = char_list;
    char_list             = d.character;
    d.connected           = CON_PLAYING;
    reset_char(d.character);

    save_char_obj( d.character );
    extract_char( d.character, TRUE );
    }
    closedir( dp );
    }
    else
        bug("recrypt_pfiles: Could not read directory.",0);
  send_to_char("Pfiles changed.\n\r", ch);
  return;
}

void do_ticks (CHAR_DATA *ch, char *argument)
{

    char buf[MSL];
    send_to_char("Current Tick Times (seconds):\n\r", ch);
    sprintf(buf, "Pulse_area: %d\n\r", pulse_area/4);
    send_to_char(buf, ch);
    sprintf(buf, "Pulse_mobile: %d\n\r", pulse_mobile/4);
    send_to_char(buf, ch);
    sprintf(buf, "Pulse_violence: %d\n\r", pulse_violence/4);
    send_to_char(buf, ch);
    sprintf(buf, "Pulse_point: %d\n\r", pulse_point/4);
    send_to_char(buf, ch);
    sprintf(buf, "Pulse_music: %d\n\r", pulse_music/4);
    send_to_char(buf, ch);
    sprintf(buf, "Pulse_bank: %d\n\r", pulse_bank/4);
    send_to_char(buf, ch);

    return;
}

void do_allowadmintalk (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg[MIL];
char arg2[MIL];

    if (!ch->pcdata->admin)
             {
                 send_to_char("Huh?\n\r", ch);
                 return;
             }

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

  if ( ( victim = get_char_world( ch, arg  ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }
    if (IS_NPC(victim))
    {
        sendch("That is how the robot uprising starts... Better not.\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        if (!str_cmp(arg2, "shuttlecock"))
        {
            sendch("You remove your own admin privileges.\n\r", ch);
            victim->pcdata->admin = FALSE;
            return;
        } else {
            sendch("That does not seem like a good idea. To continue with this action,\n\r", ch);
            sendch("repeat the same command with the word 'shuttlecock' at the end.\n\r", ch);
            sendch("eg: allowadmintalk self shuttlecock\n\r", ch);
            return;
        }
    }

if (IS_ADMIN(victim))
    {
    victim->pcdata->admin = FALSE;
    send_to_char("They can no longer use Admintalk.\n\r", ch);
    }
    else {
    victim->pcdata->admin = TRUE;
    send_to_char("They may now use the Admintalk channel.\n\r", ch);
    send_to_char("You have been bestowed the Admintalk channel!\n\r", victim);
    }

    return;
    }

void do_allowimmtalk (CHAR_DATA *ch, char *argument)
{
CHAR_DATA *victim;
char arg[MIL];
one_argument( argument, arg );
  if ( ( victim = get_char_world( ch, arg  ) ) == NULL )
  {
    send_to_char( "They aren't here.\n\r", ch );
    return;
  }

if (IS_SET(victim->act, PLR_IMMTALK))
    {
    REMOVE_BIT(victim->act, PLR_IMMTALK);
    send_to_char("They can no longer use the Immtalk channel.\n\r", ch);
    }
    else {
    SET_BIT(victim->act, PLR_IMMTALK);
    send_to_char("They may now use the Immtalk channel.\n\r", ch);
    send_to_char("You have been bestowed the Immtalk channel!\n\r", victim);
    }
    return;
    }

void do_makeactive (CHAR_DATA *ch, char *argument)
{
    return;
}

void do_immtitle( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];

    if ( IS_NPC(ch) )
    return;

    if ( !IS_IMMORTAL(ch))
    { sendch("Huh?\n\r", ch); return; }

    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm, COMM_IMMTITLE))
        {
            sendch("Disabling Immtitle.\n\r", ch);
            REMOVE_BIT(ch->comm, COMM_IMMTITLE);
            send_to_char( "To see your current immtitle, type 'immtitle show'.\n\r", ch );
            return;
        } else {
            if (!IS_NULLSTR(ch->pcdata->immtitle))
            {
            sendch("Enabling Immtitle.\n\r", ch);
            SET_BIT(ch->comm, COMM_IMMTITLE);
            send_to_char( "To see your current immtitle, type 'immtitle show'.\n\r", ch );
            return;
            } else
            { sendch("You do not have an immtitle to show!\n\r", ch); return;}

            return;
        }
    }
    if (!str_prefix(argument, "show"))
    {
    sprintf(buf, "Your current immtitle is: %s\n\r", ch->pcdata->immtitle);
    sendch(buf, ch);
    send_to_char( "To clear your immtitle type 'immtitle clear'.\n\r", ch );
    return;
    }
    else if (!str_prefix(argument, "clear"))
    {
        ch->pcdata->immtitle = str_dup( "" );
        sendch("Your immtitle has been cleared.\n\r", ch);
        REMOVE_BIT(ch->comm, COMM_IMMTITLE);
        return;
    }

    if ( c_str_len(argument) > 14 )
    {
        sendch("Immtitle length limited to 14 visible characters. String truncated.\n\r", ch);
        argument[14] = '\0';
    }

    smash_tilde( argument );
    ch->pcdata->immtitle = str_dup(argument);
    send_to_char( "{WImmtitle set and enabled.{x\n\r", ch );
    SET_BIT(ch->comm, COMM_IMMTITLE);
    sprintf(buf, "Your Immtitle is now: %s\n\r", ch->pcdata->immtitle);
    sendch(buf, ch);
    return;
}

void do_pretitle( CHAR_DATA *ch, char *argument )
{
    char arg[MIL];
    CHAR_DATA *vic;

    if ( IS_NPC(ch) )
    return;

    argument = one_argument(argument, arg);

    if ( IS_NULLSTR(argument) || IS_NULLSTR(arg) || (vic = get_char_world(ch, arg)) == NULL  )
    {
    send_to_char( "Change whose pretitle to what?\n\r", ch );
    return;
    }

    if (IS_NPC(vic))
    {
        sendch("Not on NPCs.\n\r", ch);
        return;
    }

    if ( strlen(argument) > 60 )
    argument[60] = '\0';

    if (str_cmp(argument, "clear") && str_cmp(argument, "none"))
    {
    smash_tilde( argument );
    free_string( vic->pcdata->pretitle );
    vic->pcdata->pretitle = str_dup( argument );
    send_to_char( "Pretitle set.\n\r", ch );
    cprintf(vic, "Your pretitle on who has been changed to '%s' by an Immortal.\n\r", vic->pcdata->pretitle);
    } else {
    free_string( vic->pcdata->pretitle);
    vic->pcdata->pretitle = &str_empty[0];
    sendch("Pretitle cleared.\n\r", ch);
    sendch("Your pretitle has been cleared.\n\r", vic);
    }
}


void do_wiziname( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];

    if ( IS_NPC(ch) )
    return;

    if ( !IS_IMMORTAL(ch))
    { sendch("Huh?\n\r", ch); return; }

    if ( argument[0] == '\0' )
    {
        if (IS_SET(ch->comm, COMM_WIZINAME))
        {
            sendch("Disabling Wiziname.\n\r", ch);
            REMOVE_BIT(ch->comm, COMM_WIZINAME);
            send_to_char( "To see your current wiziname, type 'wiziname show'.\n\r", ch );
            return;
        } else {
            sendch("Enabling Wiziname.\n\r", ch);
            SET_BIT(ch->comm, COMM_WIZINAME);
            send_to_char( "To see your current wiziname, type 'wiziname show'.\n\r", ch );
            return;
        }
    }
    if (!str_prefix(argument, "show"))
    {
    sprintf(buf, "Your current wiziname is: %s\n\r", ch->pcdata->wiziname);
    sendch(buf, ch);
    send_to_char( "To clear your wiziname type 'wiziname clear'.\n\r", ch );
    return;
    }
    else if (!str_prefix(argument, "clear"))
    {
        ch->pcdata->wiziname = str_dup( "" );
        sendch("Your wiziname has been cleared.\n\r", ch);
        return;
    }

    if ( strlen(argument) > 60 )
    {
        sendch("Wiziname length limited to 60 chars. String truncated.\n\r", ch);
        argument[60] = '\0';
    }

    smash_tilde( argument );
    ch->pcdata->wiziname = str_dup(argument);
    send_to_char( "{WWiziname set and enabled.{x\n\r", ch );
    SET_BIT(ch->comm, COMM_WIZINAME);
    sprintf(buf, "Your Wiziname is now: %s\n\r", ch->pcdata->wiziname);
    sendch(buf, ch);
    return;
}

void do_auto_shutdown()
{
    DESCRIPTOR_DATA *d, *d_next;
    for (d = descriptor_list; d ; d = d_next)
    {
        d_next = d->next;
        write_to_descriptor(d->descriptor, "**** WARNING **** Crash Encountered - Emergency Copyover Commencing **** WARNING ****", 0);

    }

   do_function( NULL, &do_asave, "changed" );
   do_function( NULL, &do_asave, "config" );
   do_function( NULL, &do_asave, "helps");
   do_function( NULL, &do_asave, "commands");
   do_function( NULL, &do_asave, "quests");

   if (!copyover_handler())
   {
       log_string("Things are fucked.. Crashing anyway!");
       exit(1);
   }

}

void do_bslap( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    ROOM_INDEX_DATA *location;
    CHAR_DATA *victim;
    char buf[MSL];

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' || ( victim = get_char_world( ch, arg1  ) ) == NULL )
    {
    send_to_char( "Bitchslap whom across the realm?\n\r", ch );
    return;
    }

    if (IS_NPC(victim) || ch == victim)
    {
        sendch("Honestly?\n\r", ch);
        return;
    }

    location = get_random_room(victim);

    if ( victim->fighting != NULL )
    stop_fighting( victim, TRUE );

    sprintf(buf, "%s hauls off and {RB{rI{RT{rC{RH{rS{RL{rA{RP{rs{x %s half way across the realm!\n\r", ch->name, victim->name);

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character != victim && d->character != ch)
        {
            send_to_char( buf, d->character );
        }
    }

    act("You haul off and {RB{rI{RT{rC{RH{rS{RL{rA{RP{x $N across the realm!", ch, NULL, victim, TO_CHAR);
    char_from_room( victim );
    char_to_room( victim, location );
    act( "$n comes tumbling in from half way across the realm!", victim, NULL, NULL, TO_ROOM );

    if( MOUNTED(victim) )
    {
        char_from_room( MOUNTED(victim) );
        char_to_room( MOUNTED(victim), location );
    }

    if (ch->pet != NULL)
    {
        char_from_room (ch->pet);
        char_to_room (ch->pet, location);
    }

    act( "$n hauls off and {RB{rI{RT{rC{RH{rS{RL{rA{RP{rs{x you... you go flying across the realm!\n\r", ch, NULL, victim, TO_VICT );
    do_function(victim, &do_look, "auto" );

}
#define CHECK_STANDING 0
#define CHECK_RANGE    1
#define CHECK_AREA     2
void do_checklinks (CHAR_DATA *ch, char *argument)
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    EXIT_DATA *pExit;
    int door, areanum, lvnum, hvnum;
    int type;
    char arg1[MIL], arg2[MIL];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1) && IS_NULLSTR(arg2))
    {
        type = CHECK_STANDING;
    } else if (IS_NULLSTR(arg2)) {
        if ( is_number( arg1 ) )
            if ( (pArea = get_area_data(atoi(arg1))) != NULL )
                type = CHECK_AREA;
    } else {
        if (is_number(arg1) && is_number(arg2))
        type = CHECK_RANGE;
    }

    if (type == -1)
    {
        sendch("That is not a valid option. Please enter an area vnum, a vnum range, or nothing.\n\r", ch);
        return;
    }

    lvnum = hvnum = 0;
    switch (type)
    {
        case CHECK_STANDING:
            pRoom = ch->in_room;
            pArea = pRoom->area;
            break;
        case CHECK_RANGE:
            lvnum = atoi(arg1);
            hvnum = atoi(arg2);
            break;
        case CHECK_AREA:
            pArea = get_area_data(atoi(arg1));
            break;
        default:
            break;
    }

    if (lvnum == 0)
    {
        hvnum = pArea->max_vnum;
        lvnum = pArea->min_vnum;
    }
    output_crosslinks(ch, lvnum, hvnum);
    return;
}

void output_crosslinks(CHAR_DATA *ch, int lvnum, int hvnum)
{
    ROOM_INDEX_DATA *pRoom;
    EXIT_DATA *pExit;
    BUFFER *buffer;
    bool found;
    int door, vnum;
    buffer = new_buf();
    found = FALSE;
    int count = 0;
    char buf[MSL];
    char buf2[MSL];
    char linecolor[5], extcolor[5];

    if (lvnum > hvnum)
    { // If lowvnum is actually high, flip them.
        vnum = lvnum;
        lvnum = hvnum;
        hvnum = vnum;
    }

    for (vnum = lvnum; vnum <= hvnum; vnum++)
    {
        if ((pRoom = get_room_index(vnum)) == NULL)
            continue;

        for ( door = 0; door <= 5; door++ )
            if ( ( pExit = pRoom->exit[door] ) != NULL )
                if (pExit->u1.to_room->vnum > hvnum || pExit->u1.to_room->vnum < lvnum)
                {
                    if (count <= 0)
                        strcpy(linecolor, "{w");
                    else
                    {
                        strcpy(linecolor, "{D");
                    }
                    if (IS_SET(pExit->u1.to_room->area->area_flags, AREA_UNLINKED))
                        strcpy(extcolor, "{R");
                    else
                        strcpy(extcolor, "");

                    sprintf(buf2, "%s to", dir_name[door]);
                    sprintf( buf, "%s  %5d%s%s%-5d{x\n\r", linecolor, pRoom->vnum, center(buf2, 15, " "), extcolor, pExit->u1.to_room->vnum);
                    add_buf(buffer, buf);
                    count++;
                    if (count > 1) count = 0;
                    found = TRUE;
                }

    }

    if (found)
    {
        sendch("{c [{wVnum{c]    [{wExit{c]   [{wExternal{c]{x\n\r"
               "  --------------------------- \n\r\n\r", ch);
        page_to_char(buf_string(buffer),ch);
        free_buf(buffer);
    } else {
        sendch("Good Job! No Shit Links found.\n\r", ch);
    }
}

void do_checkresets (CHAR_DATA *ch, char *argument)
{
    AREA_DATA *pArea;
    ROOM_INDEX_DATA *pRoom;
    EXIT_DATA *pExit;
    int door, areanum, lvnum, hvnum;
    int type;
    char arg1[MIL], arg2[MIL];

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if (IS_NULLSTR(arg1) && IS_NULLSTR(arg2))
    {
        type = CHECK_STANDING;
    } else if (IS_NULLSTR(arg2)) {
        if ( is_number( arg1 ) )
            if ( (pArea = get_area_data(atoi(arg1))) != NULL )
                type = CHECK_AREA;
    } else {
        if (is_number(arg1) && is_number(arg2))
        type = CHECK_RANGE;
    }

    if (type == -1)
    {
        sendch("That is not a valid option. Please enter an area vnum, a vnum range, or nothing.\n\r", ch);
        return;
    }

    lvnum = hvnum = 0;
    switch (type)
    {
        case CHECK_STANDING:
            pRoom = ch->in_room;
            pArea = pRoom->area;
            break;
        case CHECK_RANGE:
            lvnum = atoi(arg1);
            hvnum = atoi(arg2);
            pArea = NULL;
            break;
        case CHECK_AREA:
            pArea = get_area_data(atoi(arg1));
            break;
        default:
            break;
    }

    if (pArea && lvnum == 0)
    {
        hvnum = pArea->max_vnum;
        lvnum = pArea->min_vnum;
        pRoom = get_room_index(pArea->min_vnum);
    }
    sendch("\n\r", ch);
    output_crossresets( ch, pArea, pRoom, lvnum, hvnum );
    return;
}

void output_crossresets( CHAR_DATA *ch, AREA_DATA *pArea, ROOM_INDEX_DATA *pRoom, int lvnum, int hvnum )
{
    RESET_DATA      *pReset;
    MOB_INDEX_DATA  *pMob = NULL;
    char        buf   [ MAX_STRING_LENGTH ];
    char        final [ MAX_STRING_LENGTH ];
    int         iReset = 0;
    int         vnum;

    final[0]  = '\0';

    send_to_char ("\n\rNow let's talk about shit resets...\n\r\n\r", ch );

    if (pArea != NULL)
    {
        lvnum = pArea->min_vnum;
        hvnum = pArea->max_vnum;
    }
    for ( vnum = lvnum; vnum <= hvnum; vnum++)
    {
        if ((pRoom = get_room_index(vnum)) == NULL )
            continue;

        for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
        {
        OBJ_INDEX_DATA  *pObj;
        MOB_INDEX_DATA  *pMobIndex;
        OBJ_INDEX_DATA  *pObjIndex;
        OBJ_INDEX_DATA  *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex;

        final[0] = '\0';

        switch ( pReset->command )
        {
        default:
            sprintf( buf, "Bad reset command: %c.", pReset->command );
            strcat( final, buf );
            break;

        case 'M':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                    sprintf( buf, "Load Mobile - Bad Mob %d\n\r", pReset->arg1 );
                    strcat( final, buf );
                    continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                    sprintf( buf, "Load Mobile - Bad Room %d\n\r", pReset->arg3 );
                    strcat( final, buf );
                    continue;
            }

                pMob = pMobIndex;
                if (pMobIndex->vnum < lvnum || pMobIndex->vnum > hvnum)
                {
                    sprintf( buf, "M[%5d] %-13.13s in room             R[%5d] %2d-%2d %-15.15s\n\r",
                               pReset->arg1, pMob->short_descr, pReset->arg3,
                               pReset->arg2, pReset->arg4, pRoomIndex->name );
                    strcat( final, buf );
                }

            /*
             * Check for pet shop.
             * -------------------
             */
            {
            ROOM_INDEX_DATA *pRoomIndexPrev;

            pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
            if ( pRoomIndexPrev
                && IS_SET( pRoomIndexPrev->room_flags, ROOM_PET_SHOP ) )
                        final[5] = 'P';
            }

            break;

        case 'O':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                    sprintf( buf, "Load Object - Bad Object %d\n\r",
                pReset->arg1 );
                    strcat( final, buf );
                    continue;
            }

                pObj       = pObjIndex;

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                    sprintf( buf, "Load Object - Bad Room %d\n\r", pReset->arg3 );
                    strcat( final, buf );
                    continue;
            }
                if (pObjIndex->vnum < lvnum || pObjIndex->vnum > hvnum)
                {
                    sprintf( buf, "O[%5d] %-13.13s in room             "
                                  "R[%5d]       %-15.15s{x\n\r",		//Added {x to clear color bleed
                                  pReset->arg1, pObj->short_descr,
                                  pReset->arg3, pRoomIndex->name );
                    strcat( final, buf );
                }

            break;

        case 'P':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                    sprintf( buf, "Put Object - Bad Object %d\n\r",
                        pReset->arg1 );
                    strcat( final, buf );
                    continue;
            }

                pObj       = pObjIndex;

            if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
            {
                    sprintf( buf, "Put Object - Bad To Object %d\n\r",
                        pReset->arg3 );
                    strcat( final, buf );
                    continue;
            }
                if (pMobIndex->vnum < lvnum || pMobIndex->vnum > hvnum)
                {
                    sprintf( buf,
                    "O[%5d] %-13.13s inside              O[%5d] %2d-%2d %-15.15s\n\r",
                    pReset->arg1,
                    pObj->short_descr,
                    pReset->arg3,
                    pReset->arg2,
                    pReset->arg4,
                    pObjToIndex->short_descr );
                    strcat( final, buf );
                }

            break;

        case 'G':
        case 'E':
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                    sprintf( buf, "Give/Equip Object - Bad Object %d\n\r",
                        pReset->arg1 );
                    strcat( final, buf );
                    continue;
            }

                pObj       = pObjIndex;

            if ( !pMob )
            {
                    sprintf( buf, "Give/Equip Object - No Previous Mobile\n\r" );
                    strcat( final, buf );
                    break;
            }
                if (pMobIndex->vnum < lvnum || pMobIndex->vnum > hvnum)
                {
                    if ( pMob->pShop )
                    {
                    sprintf( buf,
                    "O[%5d] %-13.13s in the inventory of S[%5d]       %-15.15s\n\r",
                    pReset->arg1,
                    pObj->short_descr,
                    pMob->vnum,
                    pMob->short_descr  );
                    }
                    else
                    sprintf( buf,
                    "O[%5d] %-13.13s %-19.19s M[%5d]       %-15.15s\n\r",
                    pReset->arg1,
                    pObj->short_descr,
                    (pReset->command == 'G') ?
                        flag_string( wear_loc_strings, WEAR_NONE )
                      : flag_string( wear_loc_strings, pReset->arg3 ),
                      pMob->vnum,
                      pMob->short_descr );
                    strcat( final, buf );
            }
            break;

        /*
         * Doors are set in rs_flags don't need to be displayed.
         * If you want to display them then uncomment the new_reset
         * line in the case 'D' in load_resets in db.c and here.
         */
        case 'D':
            break;
        /*
         * End Doors Comment.
         */
        case 'R':
            break;
        }
        send_to_char( final, ch );
        }
    }
    return;
}
