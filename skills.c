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
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
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
#include "magic.h"
#include "recycle.h"
#include "tables.h"

/* RT spells and skills show the players spells (or skills) */

void do_spells(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char arg[MAX_INPUT_LENGTH];
    char spell_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
    char spell_columns[LEVEL_HERO + 1];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO, mana;
    bool fAll = FALSE, found = FALSE;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
      return;

    if (argument[0] != '\0')
    {
	fAll = TRUE;

	if (str_prefix(argument,"all"))
	{
	    argument = one_argument(argument,arg);
	    if (!is_number(arg))
	    {
		send_to_char("Arguments must be numerical or all.\n\r",ch);
		return;
	    }
	    max_lev = atoi(arg);

	    if (max_lev < 1 || max_lev > LEVEL_HERO)
	    {
		sprintf(buf,"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		send_to_char(buf,ch);
		return;
	    }

	    if (argument[0] != '\0')
	    {
		argument = one_argument(argument,arg);
		if (!is_number(arg))
		{
		    send_to_char("Arguments must be numerical or all.\n\r",ch);
		    return;
		}
		min_lev = max_lev;
		max_lev = atoi(arg);

		if (max_lev < 1 || max_lev > LEVEL_HERO)
		{
		    sprintf(buf,
			"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		    send_to_char(buf,ch);
		    return;
		}

		if (min_lev > max_lev)
		{
		    send_to_char("That would be silly.\n\r",ch);
		    return;
		}
	    }
	}
    }


    /* initialize data */
    for (level = 0; level < LEVEL_HERO + 1; level++)
    {
        spell_columns[level] = 0;
        spell_list[level][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        int csn;
        if (skill_table[sn].name == NULL )
	    break;

        if ( (csn = cskill_lookup(sn)) < 0)
            continue;

	if ((level = csskill_table[csn].level) < LEVEL_HERO + 1
	&&  (fAll || level <= ch->level)
	&&  level >= min_lev && level <= max_lev
	&&  skill_table[sn].spell_fun != spell_null
	&&  ch->pcdata->learned[sn] > 0)
        {
	    found = TRUE;
	    level = csskill_table[csn].level;
	    if (ch->level < level)
	    	sprintf(buf,"%-18s   n/a      ", skill_table[sn].name);
	    else
	    {
		mana = UMAX(skill_table[sn].min_mana,
		    100/(2 + ch->level - level));
	        sprintf(buf,"%-18s  %3d mana  ",skill_table[sn].name,mana);
	    }

	    if (spell_list[level][0] == '\0')
          	sprintf(spell_list[level],"\n\rLevel %2d: %s",level,buf);
	    else /* append */
	    {
          	if ( ++spell_columns[level] % 1 == 0)
		    strcat(spell_list[level],"\n\r          ");
          	strcat(spell_list[level],buf);
	    }
	}
    }

    /* return results */

    if (!found)
    {
      	send_to_char("No prayers found.\n\r",ch);
      	return;
    }

    buffer = new_buf();
    for (level = 0; level < LEVEL_HERO + 1; level++)
      	if (spell_list[level][0] != '\0')
	    add_buf(buffer,spell_list[level]);
    add_buf(buffer,"\n\r");
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

void do_skills(CHAR_DATA *ch, char *argument)
{
    BUFFER *buffer;
    char arg[MAX_INPUT_LENGTH];
    char skill_list[LEVEL_HERO + 1][MAX_STRING_LENGTH];
    char skill_columns[LEVEL_HERO + 1];
    int sn, level, min_lev = 1, max_lev = LEVEL_HERO;
    bool fAll = FALSE, found = FALSE;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch))
      return;

    if (argument[0] != '\0')
    {
	fAll = TRUE;

	if (str_prefix(argument,"all"))
	{
	    argument = one_argument(argument,arg);
	    if (!is_number(arg))
	    {
		send_to_char("Arguments must be numerical or all.\n\r",ch);
		return;
	    }
	    max_lev = atoi(arg);

	    if (max_lev < 1 || max_lev > LEVEL_HERO)
	    {
		sprintf(buf,"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		send_to_char(buf,ch);
		return;
	    }

	    if (argument[0] != '\0')
	    {
		argument = one_argument(argument,arg);
		if (!is_number(arg))
		{
		    send_to_char("Arguments must be numerical or all.\n\r",ch);
		    return;
		}
		min_lev = max_lev;
		max_lev = atoi(arg);

		if (max_lev < 1 || max_lev > LEVEL_HERO)
		{
		    sprintf(buf,
			"Levels must be between 1 and %d.\n\r",LEVEL_HERO);
		    send_to_char(buf,ch);
		    return;
		}

		if (min_lev > max_lev)
		{
		    send_to_char("That would be silly.\n\r",ch);
		    return;
		}
	    }
	}
    }


    /* initialize data */
    for (level = 0; level < LEVEL_HERO + 1; level++)
    {
        skill_columns[level] = 0;
        skill_list[level][0] = '\0';
    }

    for (sn = 0; sn < MAX_SKILL; sn++)
    {
        if (skill_table[sn].name == NULL )
	    break;

	if ((level = csskill_table[cskill_lookup(sn)].level) < LEVEL_HERO + 1
	&&  (fAll || level <= ch->level)
	&&  level >= min_lev && level <= max_lev
	&&  ch->pcdata->learned[sn] > 0)
        {
	    found = TRUE;
	    level = csskill_table[cskill_lookup(sn)].level;
	    if (ch->level < level)
	    	sprintf(buf,"%-18s n/a      ", skill_table[sn].name);
	    else
	    	sprintf(buf,"%-18s %3d%%      ",skill_table[sn].name,
		    ch->pcdata->learned[sn]);

	    if (skill_list[level][0] == '\0')
          	sprintf(skill_list[level],"\n\r{WLevel %2d: {x%s",level,buf);
	    else /* append */
	    {
          	if ( ++skill_columns[level] % 2 == 0)
		    strcat(skill_list[level],"\n\r          ");
          	strcat(skill_list[level],buf);
	    }
	}
    }

    /* return results */

    if (!found)
    {
      	send_to_char("No skills found.\n\r",ch);
      	return;
    }

    buffer = new_buf();
    for (level = 0; level < LEVEL_HERO + 1; level++)
      	if (skill_list[level][0] != '\0')
	    add_buf(buffer,skill_list[level]);
    add_buf(buffer,"\n\r");
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}

int exp_per_level(CHAR_DATA *ch, int points)
{
    int expl,mod;

    if (IS_NPC(ch))
	return 1000;

    expl = 1000;
    mod = 100;
    if (ch->race == race_lookup("vampire"))
        mod = 150;
    if (ch->race == race_lookup("ghoul"))
        mod = 125;
    if (ch->race == race_lookup("garou"))
        mod = 200;
    if (ch->race == race_lookup("human"))
        mod = 100;

	if((expl * mod/100)+(100*ch->remorts) > 10000)
		return 10000;
	else
    	return (expl * mod/100)+(100*ch->remorts);
}

/* checks for skill improvement */
void check_improve( CHAR_DATA *ch, int sn, bool success, int multiplier )
{
    int chance;
    char buf[100];
    int skill;

    if (IS_NPC(ch))
	return;

    if (ch->level < csskill_table[cskill_lookup(sn)].level
    ||  ch->pcdata->learned[sn] == 0
    ||  ch->pcdata->learned[sn] == 100)
	return;  /* skill is not known */

    if (sn == gsn_whip || sn == gsn_dagger) skill = CSABIL_LIGHT;
    if (sn == gsn_axe || sn == gsn_sword) skill = CSABIL_HEAVY;
    if (sn == gsn_flail || sn == gsn_mace) skill = CSABIL_BLUNT;
    if (sn == gsn_lance || sn == gsn_spear || sn == gsn_polearm) skill = CSABIL_POLEARM;

    if (sn == gsn_whip || sn == gsn_dagger || sn == gsn_axe || sn == gsn_sword ||
        sn == gsn_flail || sn == gsn_mace || sn == gsn_lance ||
        sn == gsn_spear || sn == gsn_polearm)  {
    switch (ch->pcdata->cssec_abil[skill])
    {
        case 0: if (ch->pcdata->learned[sn] >= 50) return; break;
        case 1: if (ch->pcdata->learned[sn] >= 60) return; break;
        case 2: if (ch->pcdata->learned[sn] >= 70) return; break;
        case 3: if (ch->pcdata->learned[sn] >= 80) return; break;
        case 4: if (ch->pcdata->learned[sn] >= 90) return; break;
    }
    }



    /* check to see if the character has a chance to learn */
    chance = 10 * 50;
    chance /= (		multiplier
		*	2
		*	4);
    chance += ch->level;

    if (number_range(1,1000) > chance)
	return;

    /* now that the character has a CHANCE to learn, see if they really have */

    if (success)
    {
	chance = URANGE(5,100 - ch->pcdata->learned[sn], 95);
	if (number_percent() < chance)
	{
	    ch->pcdata->learned[sn]++;
	    sprintf(buf,"You have become better at %s! (%d)\n\r",
		    skill_table[sn].name,ch->pcdata->learned[sn]);
	    send_to_char(buf,ch);
	    gain_exp(ch, ch->pcdata->learned[sn]);

	}
    }

    else
    {
	chance = URANGE(5,ch->pcdata->learned[sn]/2,30);
	if (number_percent() < chance)
	{
	    ch->pcdata->learned[sn] += number_range(1,3);
	    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],100);
	    sprintf(buf,
		"You learn from your mistakes, and your %s skill improves. (%d)\n\r",
		skill_table[sn].name, ch->pcdata->learned[sn]);
	    send_to_char(buf,ch);
	    gain_exp(ch, ch->pcdata->learned[sn]);
	}
    }
}

#define STAT 0
#define DOT 1
#define TYPE 2
#define ATTR 0
#define ABIL 1
#define SECA 2
void do_skillmap(CHAR_DATA *ch, char *argument)
{
    int i;
    BUFFER *output;
    char buf[MSL];
    char primary[MSL];
    char secondary[MSL];
    bool has_secondary;

    has_secondary = TRUE;
    output = new_buf();
    sprintf(buf, "%9s        %-3s %-17s   %-17s\n\r", "Name", "Lvl", "Primary       Dot", "Secondary     Dot");
    add_buf(output, buf);
    for (i = 0; csskill_table[i].name != NULL; i++)
    {
        switch (csskill_table[i].primary[TYPE])
        {
            case ABIL:
                if (csskill_table[i].primary[STAT] == CSABIL_NONE)
                    continue;
                else
                    sprintf(primary, "%s", ability_table[csskill_table[i].primary[STAT]].name);
                break;
            case ATTR:
                sprintf(primary, "%s", attribute_table[csskill_table[i].primary[STAT]].name);
                break;
            case SECA:
                sprintf(primary, "%s", sec_abil_table[csskill_table[i].primary[STAT]].name);
                break;
            default:
                bug("skills_list: Invalid primary cs type.", 0);
                return;
        }

        switch (csskill_table[i].secondary[TYPE])
        {

            case ABIL:
                if (csskill_table[i].secondary[STAT] == CSABIL_NONE)
                    has_secondary = FALSE;
                else
                {
                    has_secondary = TRUE;
                    sprintf(secondary, "%s", ability_table[csskill_table[i].secondary[STAT]].name);
                }

                break;
            case ATTR:
                sprintf(secondary, "%s", attribute_table[csskill_table[i].secondary[STAT]].name);
                has_secondary = TRUE;
                break;
            case SECA:
                sprintf(secondary, "%s", sec_abil_table[csskill_table[i].secondary[STAT]].name);
                has_secondary = TRUE;
                break;
            default:
                bug("skills_list: Invalid primary cs type.", 0);
                return;
        }

        if (has_secondary)
        {
            sprintf(buf, "%s%16s %-3d %-15s %-2d  %-15s %-2d{x\n\r", ch->pcdata->learned[*csskill_table[i].gsn] ? "" : "{c" , csskill_table[i].name, csskill_table[i].level,
                                                        primary, csskill_table[i].primary[DOT],
                                                        secondary, csskill_table[i].secondary[DOT]);
        }
        else
        {
            sprintf(buf, "%s%16s %-3d %-15s %-2d{x\n\r", ch->pcdata->learned[*csskill_table[i].gsn] ? "" : "{c" , csskill_table[i].name, csskill_table[i].level,
                                                        primary, csskill_table[i].primary[DOT]);
        }
        add_buf(output, buf);
    }

    page_to_char(buf_string(output), ch);

}
