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
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "tables.h"

int flag_lookup (const char *name, const struct flag_type *flag_table)
{
    int flag;

    for (flag = 0; flag_table[flag].name != NULL; flag++)
    {
    if (LOWER(name[0]) == LOWER(flag_table[flag].name[0])
    &&  !str_prefix(name,flag_table[flag].name))
        return flag_table[flag].bit;
    }

    return NO_FLAG;
}

int sect_lookup(const char *name)
{
    int sect;

    for (sect = 0; sect < MAX_SECT; sect++)
    {
    if (LOWER(name[0]) == LOWER(sect_table[sect].name[0])
    &&  !str_prefix(name,sect_table[sect].name))
        return sect;
    }

    return 0;
}
int tradition_lookup(const char *name)
{
    int trad;

    for (trad = 0; trad < MAX_TRADITION; trad++)
    {
    if (LOWER(name[0]) == LOWER(tradition_table[trad].name[0])
    &&  !str_prefix(name,tradition_table[trad].name))
        return trad;
    }

    return 0;
}

int background_lookup(const char *name)
{
    int back;

    for (back = 0; back < MAX_CSBACK; back++)
    {
    if (LOWER(name[0]) == LOWER(csback_table[back].name[0])
    &&  !str_cmp(name,csback_table[back].name))
        return back;
    }

    return -1;
}


int rote_lookup(const char *name)
{
    int rote;

    for (rote = 0; rote < MAX_ROTE; rote++)
    {
    if (LOWER(name[0]) == LOWER(rote_table[rote].name[0])
    &&  !str_prefix(name,rote_table[rote].name))
        return rote;
    }

    return 0;
}


int position_lookup (const char *name)
{
   int pos;

   for (pos = 0; position_table[pos].name != NULL; pos++)
   {
    if (LOWER(name[0]) == LOWER(position_table[pos].name[0])
    &&  !str_prefix(name,position_table[pos].name))
        return pos;
   }

   return -1;
}

int sex_lookup (const char *name)
{
   int sex;

   for (sex = 0; sex_table[sex].name != NULL; sex++)
   {
    if (LOWER(name[0]) == LOWER(sex_table[sex].name[0])
    &&  !str_prefix(name,sex_table[sex].name))
        return sex;
   }

   return -1;
}

int size_lookup (const char *name)
{
   int size;

   for ( size = 0; size_table[size].name != NULL; size++)
   {
        if (LOWER(name[0]) == LOWER(size_table[size].name[0])
        &&  !str_prefix( name,size_table[size].name))
            return size;
   }

   return -1;
}

/* returns race number */
int race_lookup (const char *name)
{
   int race;

   for ( race = 0; race_table[race].name != NULL; race++)
   {
    if (LOWER(name[0]) == LOWER(race_table[race].name[0])
    &&  !str_prefix( name,race_table[race].name))
        return race;
  }

   return 0;
}

int item_lookup(const char *name)
{
    int type;

    for (type = 0; item_table[type].name != NULL; type++)
    {
        if (LOWER(name[0]) == LOWER(item_table[type].name[0])
        &&  !str_prefix(name,item_table[type].name))
            return item_table[type].type;
    }

    return -1;
}

int liq_lookup (const char *name)
{
    int liq;

    for ( liq = 0; liq_table[liq].liq_name != NULL; liq++)
    {
    if (LOWER(name[0]) == LOWER(liq_table[liq].liq_name[0])
    && !str_prefix(name,liq_table[liq].liq_name))
        return liq;
    }

    return -1;
}

/* direct copy of liq_lookup above. Yes, I'm lazy. So sue me. */
int tool_lookup (const char *name)
{
    int tool;

    for ( tool = 0; tool_table[tool].tool_name != NULL; tool++)
    {
    if (LOWER(name[0]) == LOWER(tool_table[tool].tool_name[0])
    && !str_prefix(name,tool_table[tool].tool_name))
        return tool;
    }

    return -1;
}

int crafted_lookup (const char *name)
{
    int crafted;

    for ( crafted = 0; crafted_table[crafted].crafted_name != NULL; crafted++)
    {
    if (LOWER(name[0]) == LOWER(crafted_table[crafted].crafted_name[0])
    && !str_prefix(name,crafted_table[crafted].crafted_name))
        return crafted;
    }

    return -1;
}



HELP_DATA * help_lookup( char *keyword )
{
    HELP_DATA *pHelp;
    char temp[MIL], argall[MIL];

    argall[0] = '\0';

    while (keyword[0] != '\0' )
    {
        keyword = one_argument(keyword, temp);
        if (argall[0] != '\0')
            strcat(argall," ");
        strcat(argall, temp);
    }

    for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
        if ( is_name( argall, pHelp->keyword ) )
            return pHelp;

    return NULL;
}

HELP_AREA * had_lookup( char *arg )
{
    HELP_AREA * temp;
    extern HELP_AREA * had_list;

    for ( temp = had_list; temp; temp = temp->next )
        if ( !str_cmp( arg, temp->filename ) )
            return temp;

    return NULL;

}

int brew_lookup(const char *name)
{
    int brew;

    for (brew = 0; brew < MAX_BREW; brew++)
    {
    if (LOWER(name[0]) == LOWER(brew_table[brew].name[0])
    &&  !str_prefix(name,brew_table[brew].name))
        return brew;
    }

    return 0;
}

int get_hometown_num (const char *name)
{
    int hometown;

    for (hometown = 0; hometown < MAX_HOMETOWN; hometown++)
    {
        if (LOWER(name[0]) == LOWER(hometown_table[hometown].name[0])
        &&  !str_prefix(name,hometown_table[hometown].name))
            return hometown;
    }

    return -1;
};

int get_hometown( int num)
{
    if (num >= 0 && num < MAX_HOMETOWN &&
            hometown_table[num].vnum > 0)
        return hometown_table[num].vnum;
    else
        return ROOM_VNUM_TEMPLE;
};


int like_lookup(CHAR_DATA *mob, CHAR_DATA *ch)
{
    int i;

    if(!IS_NPC(mob) || IS_NPC(ch))
        return -1;

    for (i = 0;i < 100;i++)
    {
        if(mob->memory_name[i] == NULL)
            break;
        if(!str_cmp(mob->memory_name[i],ch->name))
            return i;
    }
    return -1;
}

int get_like(CHAR_DATA *mob, CHAR_DATA *ch)
{
    int index;
    if(!IS_NPC(mob) || IS_NPC(ch))
        return 0;

    if((index = like_lookup(mob,ch)) == -1)
        return 0;

    if(mob->memory_name[index] != NULL)
        return mob->memory_like[index];

    return 0;
}

void add_like(CHAR_DATA *mob, CHAR_DATA *ch, int amount)
{
    int i, list = 0;

    if(!IS_NPC(mob) || IS_NPC(ch))
        return;

    if(!get_like(mob,ch))
    {
        for(i = 98;i >= 0;i--)
        {
            if(mob->memory_name[i] != NULL)
            {
                mob->memory_name[i+1] = mob->memory_name[i];
                mob->memory_like[i+1] = mob->memory_like[i];
            }
        }
    }
    else
    {
        list = like_lookup(mob,ch);
        if(list == -1)
            list = 0;
    }

    if(mob->memory_name[list] == NULL)
        mob->memory_name[list] = str_dup(ch->name);
    if(mob->memory_like[list] == 0)
    {
        mob->memory_like[list] = 35;
        mob->memory_like[list] += number_range(-10,10);
        mob->memory_like[list] += (get_attribute(ch,APPEARANCE)*5);
        if(is_affected(ch, gsn_majesty))
            mob->memory_like[list] += 10;
        if(is_affected(ch, gsn_awe))
            mob->memory_like[list] += 5;
        if(!IS_SET(mob->act, ACT_AGGRESSIVE))
            mob->memory_like[list] -= 20;
    }
    mob->memory_like[list] += amount;

    if(mob->memory_like[list] < 1)
        mob->memory_like[list] = 1;
    if(mob->memory_like[list] > 100)
        mob->memory_like[list] = 100;

    return;
}

