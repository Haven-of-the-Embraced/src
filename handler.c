/**************************************************************************r
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
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

/*
 * Local functions.
 */
void    affect_modify   args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void    obj_affect_to_char args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );

/* friend stuff -- for NPC's mostly */
bool is_friend(CHAR_DATA *ch,CHAR_DATA *victim)
{
    if (is_same_group(ch,victim))
    return TRUE;


    if (!IS_NPC(ch))
    return FALSE;

    if (!IS_NPC(victim))
    {
    if (IS_SET(ch->off_flags,ASSIST_PLAYERS))
        return TRUE;
    else
        return FALSE;
    }

    if (IS_AFFECTED(ch,AFF_CHARM))
    return FALSE;

    if (IS_SET(ch->off_flags,ASSIST_ALL))
    return TRUE;

    if (ch->group && ch->group == victim->group)
    return TRUE;

    if (IS_SET(ch->off_flags,ASSIST_VNUM)
    &&  ch->pIndexData == victim->pIndexData)
    return TRUE;

    if (IS_SET(ch->off_flags,ASSIST_RACE) && ch->race == victim->race)
    return TRUE;

    if (IS_SET(ch->off_flags,ASSIST_ALIGN)
    &&  !IS_SET(ch->act,ACT_NOALIGN) && !IS_SET(victim->act,ACT_NOALIGN)
    &&  ((IS_GOOD(ch) && IS_GOOD(victim))
    ||   (IS_EVIL(ch) && IS_EVIL(victim))
    ||   (IS_NEUTRAL(ch) && IS_NEUTRAL(victim))))
    return TRUE;

    return FALSE;
}

/* returns number of people on an object */
int count_users(OBJ_DATA *obj)
{
    CHAR_DATA *fch;
    int count = 0;

    if (obj->in_room == NULL)
    return 0;

    for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
    if (fch->on == obj)
        count++;

    return count;
}

/* returns material number */
int material_lookup (const char *name)
{
    return 0;
}

int weapon_lookup (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
    if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
    &&  !str_prefix(name,weapon_table[type].name))
        return type;
    }

    return -1;
}

int weapon_type (const char *name)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
    {
        if (LOWER(name[0]) == LOWER(weapon_table[type].name[0])
        &&  !str_prefix(name,weapon_table[type].name))
            return weapon_table[type].type;
    }

    return WEAPON_EXOTIC;
}


char *item_name(int item_type)
{
    int type;

    for (type = 0; item_table[type].name != NULL; type++)
    if (item_type == item_table[type].type)
        return item_table[type].name;
    return "none";
}

char *weapon_name( int weapon_type)
{
    int type;

    for (type = 0; weapon_table[type].name != NULL; type++)
        if (weapon_type == weapon_table[type].type)
            return weapon_table[type].name;
    return "exotic";
}

int attack_lookup  (const char *name)
{
    int att;

    for ( att = 0; attack_table[att].name != NULL; att++)
    {
    if (LOWER(name[0]) == LOWER(attack_table[att].name[0])
    &&  !str_prefix(name,attack_table[att].name))
        return att;
    }

    return 0;
}

/* returns a flag for wiznet */
long wiznet_lookup (const char *name)
{
    int flag;

    for (flag = 0; wiznet_table[flag].name != NULL; flag++)
    {
    if (LOWER(name[0]) == LOWER(wiznet_table[flag].name[0])
    && !str_prefix(name,wiznet_table[flag].name))
        return flag;
    }

    return -1;
}

/* returns class number */
int class_lookup (const char *name)
{
   int class;

   if ( !str_prefix( name, "mage" ) || !str_prefix( name, "cleric" ) || !str_prefix( name, "warrior" ) || !str_prefix( name, "thief" ))
    return -1;

   for ( class = 0; class < MAX_CLASS; class++)
   {
        if (LOWER(name[0]) == LOWER(class_table[class].name[0])
        &&  !str_prefix( name,class_table[class].name))
            return class;
   }

   return -1;
}

int classgroup_lookup (const char *name)
{
    if ( !str_prefix( name, "rogue" )) return 1;
    else if ( !str_prefix( name, "swordsman" )) return 2;
    else if ( !str_prefix( name, "clergy" )) return 3;
   return -1;
}


int roll_stats ()
{
   int roll;
   int adjust;


    roll = number_range(1, 100);
    adjust = 0;

    if(roll < 60) adjust += 1;
    roll = number_range(1, 100);
    if(roll < 50) adjust += 1;
    roll = number_range(1, 100);
    if(roll < 40) adjust += 1;
    roll = number_range(1, 100);
    if(roll < 30) adjust += 1;
    roll = number_range(1, 100);
    if(roll < 20) adjust += 1;
return adjust;
}

/* for immunity, vulnerabiltiy, and resistant
   the 'globals' (magic and weapons) may be overriden
   three other cases -- wood, silver, and iron -- are checked in fight.c */

int check_immune(CHAR_DATA *ch, int dam_type)
{
    int immune, def;
    int bit;

    immune = -1;
    def = IS_NORMAL;

    if (dam_type == DAM_NONE)
    return immune;

    if (dam_type <= 3 || dam_type == DAM_AGGREVATED)
    {
    if (IS_SET(ch->imm_flags,IMM_WEAPON))
        def = IS_IMMUNE;
    else if (IS_SET(ch->res_flags,RES_WEAPON))
        def = IS_RESISTANT;
    else if (IS_SET(ch->vuln_flags,VULN_WEAPON))
        def = IS_VULNERABLE;
    }
    else /* magical attack */
    {
    if (IS_SET(ch->imm_flags,IMM_MAGIC))
        def = IS_IMMUNE;
    else if (IS_SET(ch->res_flags,RES_MAGIC))
        def = IS_RESISTANT;
    else if (IS_SET(ch->vuln_flags,VULN_MAGIC))
        def = IS_VULNERABLE;
    }

    /* set bits to check -- VULN etc. must ALL be the same or this will fail */
    switch (dam_type)
    {
    case(DAM_BASH):     bit = IMM_BASH;     break;
    case(DAM_PIERCE):   bit = IMM_PIERCE;   break;
    case(DAM_SLASH):    bit = IMM_SLASH;    break;
    case(DAM_FIRE):     bit = IMM_FIRE;     break;
    case(DAM_COLD):     bit = IMM_COLD;     break;
    case(DAM_LIGHTNING):    bit = IMM_LIGHTNING;    break;
    case(DAM_ACID):     bit = IMM_ACID;     break;
    case(DAM_POISON):   bit = IMM_POISON;   break;
    case(DAM_NEGATIVE): bit = IMM_NEGATIVE; break;
    case(DAM_HOLY):     bit = IMM_HOLY;     break;
    case(DAM_ENERGY):   bit = IMM_ENERGY;   break;
    case(DAM_MENTAL):   bit = IMM_MENTAL;   break;
    case(DAM_DISEASE):  bit = IMM_DISEASE;  break;
    case(DAM_DROWNING): bit = IMM_DROWNING; break;
    case(DAM_LIGHT):    bit = IMM_LIGHT;    break;
    case(DAM_CHARM):    bit = IMM_CHARM;    break;
    case(DAM_SOUND):    bit = IMM_SOUND;    break;
    case(DAM_SUNLIGHT):     bit = IMM_SUNLIGHT;     break;
    default:        return def;
    }

    if (IS_SET(ch->imm_flags,bit))
    immune = IS_IMMUNE;
    else if (IS_SET(ch->res_flags,bit) && immune != IS_IMMUNE)
    immune = IS_RESISTANT;
    else if (IS_SET(ch->vuln_flags,bit))
    {
    if (immune == IS_IMMUNE)
        immune = IS_RESISTANT;
    else if (immune == IS_RESISTANT)
        immune = IS_NORMAL;
    else
        immune = IS_VULNERABLE;
    }

    if (immune == -1)
    return def;
    else
        return immune;
}



bool update_csstats(CHAR_DATA *ch)
{
    int i;

    for (i = 0; i <= MAX_CSATTRIBUTES; i ++)
        if(ch->csattributes[i] == 0 || ch->csattributes[i] > 10)
                ch->csattributes[i] = 1;

    if(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] == 0) ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]++;
    if(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] == 0) ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]++;
    if(ch->pcdata->csvirtues[CSVIRT_COURAGE] == 0) ch->pcdata->csvirtues[CSVIRT_COURAGE]++;

    if(ch->clan == clan_lookup("Nosferatu"))
        ch->csattributes[CSATTRIB_APPEARANCE] = 0;

    cskill_update(ch, FALSE);

    return TRUE;
}



/* checks mob format */
bool is_old_mob(CHAR_DATA *ch)
{
    if (ch->pIndexData == NULL)
    return FALSE;
    else if (ch->pIndexData->new_format)
    return FALSE;
    return TRUE;
}

/* for returning skill information */
int get_skill(CHAR_DATA *ch, int sn)
{
    int skill;

    if (sn == -1) /* shorthand for level based skills */
    {
    skill = ch->level * 5 / 2;
    }

    else if (sn < -1 || sn > MAX_SKILL)
    {
    bug("Bad sn %d in get_skill.",sn);
    skill = 0;
    }

    else if (!IS_NPC(ch))
    {
    if (ch->level < csskill_table[cskill_lookup(sn)].level)
        skill = 0;
    else
        skill = ch->pcdata->learned[sn];
    }

    else /* mobiles */
    {


        if (skill_table[sn].spell_fun != spell_null)
        skill = 40 + 2 * ch->level;

    else if (sn == gsn_sneak || sn == gsn_hide)
        skill = ch->level * 2 + 20;

        else if ((sn == gsn_dodge && IS_SET(ch->off_flags,OFF_DODGE))
    ||       (sn == gsn_parry && IS_SET(ch->off_flags,OFF_PARRY)))
        skill = ch->level * 2;

    else if (sn == gsn_shield_block)
        skill = 10 + 2 * ch->level;

    else if (sn == gsn_second_attack
    && (IS_SET(ch->act,ACT_WARRIOR) || IS_SET(ch->act,ACT_THIEF)))
        skill = 10 + 3 * ch->level;

    else if (sn == gsn_third_attack && IS_SET(ch->act,ACT_WARRIOR))
        skill = 4 * ch->level - 40;

    else if (sn == gsn_hand_to_hand)
        skill = 40 + 2 * ch->level;

    else if (sn == gsn_trip && IS_SET(ch->off_flags,OFF_TRIP))
        skill = 10 + 3 * ch->level;

    else if (sn == gsn_bash && IS_SET(ch->off_flags,OFF_BASH))
        skill = 10 + 3 * ch->level;

    else if (sn == gsn_disarm
         &&  (IS_SET(ch->off_flags,OFF_DISARM)
         ||   IS_SET(ch->act,ACT_WARRIOR)
         ||   IS_SET(ch->act,ACT_THIEF)))
        skill = 20 + 3 * ch->level;

    else if (sn == gsn_berserk && IS_SET(ch->off_flags,OFF_BERSERK))
        skill = 3 * ch->level;

    else if (sn == gsn_kick)
        skill = 10 + 3 * ch->level;

    else if (sn == gsn_ground)
        skill = 10 * ch->level;

    else if (sn == gsn_backstab && IS_SET(ch->act,ACT_THIEF))
        skill = 20 + 2 * ch->level;

    else if (sn == gsn_rescue)
        skill = 40 + ch->level;

    else if (sn == gsn_recall)
        skill = 40 + ch->level;

    else if (sn == gsn_sword
    ||  sn == gsn_dagger
    ||  sn == gsn_spear
    ||  sn == gsn_mace
    ||  sn == gsn_axe
    ||  sn == gsn_flail
    ||  sn == gsn_whip
    ||  sn == gsn_polearm
    ||  sn == gsn_lance)
        skill = 40 + 5 * ch->level / 2;

    else
       skill = 0;
    }

    if (ch->daze > 0)
    {
    if (skill_table[sn].spell_fun != spell_null)
        skill /= 2;
    else
        skill = 2 * skill / 3;
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
    skill = 9 * skill / 10;

    return URANGE(0,skill,100);
}

/* for returning weapon information */
int get_weapon_sn(CHAR_DATA *ch)
{
    OBJ_DATA *wield;
    int sn;

    wield = get_eq_char( ch, WEAR_WIELD );
    if (wield == NULL || wield->item_type != ITEM_WEAPON)
        sn = gsn_hand_to_hand;
    else switch (wield->value[0])
    {
        default :               sn = -1;                break;
        case(WEAPON_SWORD):     sn = gsn_sword;         break;
        case(WEAPON_DAGGER):    sn = gsn_dagger;        break;
        case(WEAPON_SPEAR):     sn = gsn_spear;         break;
        case(WEAPON_MACE):      sn = gsn_mace;          break;
        case(WEAPON_AXE):       sn = gsn_axe;           break;
        case(WEAPON_FLAIL):     sn = gsn_flail;         break;
        case(WEAPON_WHIP):      sn = gsn_whip;          break;
        case(WEAPON_POLEARM):   sn = gsn_polearm;       break;
        case(WEAPON_LANCE):   sn = gsn_lance;       break;
   }
   return sn;
}

int get_weapon_skill(CHAR_DATA *ch, int sn)
{
     int skill;

     /* -1 is exotic */
    if (IS_NPC(ch))
    {
    if (sn == -1)
        skill = 50 + (ch->csabilities[CSABIL_MELEE] * 7);
    else if (sn == gsn_hand_to_hand)
        skill = 40 + 2 * ch->level;
    else
        skill = 40 + 5 * ch->level / 2;
    }

    else
    {
    if (sn == -1)
        skill = 50 + (ch->csabilities[CSABIL_MELEE] * 7);
    else
        skill = ch->pcdata->learned[sn];
    }

    return URANGE(0,skill,100);
}


/* used to de-screw characters */
void reset_char(CHAR_DATA *ch)
{
     int loc,mod,stat;
     OBJ_DATA *obj;
     AFFECT_DATA *af;
     int i, oldbp, oldgen;
     bool fanged = FALSE;

     if (IS_NPC(ch))
    return;

    if (IS_AFFECTED(ch, AFF_FANGS))
      fanged = TRUE;

    /* do a FULL reset */
    for (loc = 0; loc < MAX_WEAR; loc++)
    {
        obj = get_eq_char(ch,loc);
        if (obj == NULL)
        continue;
        if (!obj->enchanted)
        for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;
            switch(af->location)
            {
                case APPLY_SEX: ch->sex     -= mod;
                        if (ch->sex < 0 || ch->sex >2)
                            ch->sex = IS_NPC(ch) ?
                            0 :
                            ch->pcdata->true_sex;
                                        break;
                case APPLY_MANA:    ch->max_mana    -= mod;     break;
                case APPLY_HIT: ch->max_hit -= mod;     break;
                case APPLY_MOVE:    ch->max_move    -= mod;     break;
            }
        }

            for ( af = obj->affected; af != NULL; af = af->next )
            {
                mod = af->modifier;
                switch(af->location)
                {
                    case APPLY_SEX:     ch->sex         -= mod;         break;
                    case APPLY_MANA:    ch->max_mana    -= mod;         break;
                    case APPLY_HIT:     ch->max_hit     -= mod;         break;
                    case APPLY_MOVE:    ch->max_move    -= mod;         break;
                }
            }
    }

    /*Resetting hp/mana/move modifiers by non-obj affects*/
    if ( ch->affected != NULL )
    {
      for ( af = ch->affected; af != NULL; af = af->next )
      {
        {
          mod = af->modifier;
          switch(af->location)
          {
            case APPLY_SEX:     ch->sex         -= mod;         break;
            case APPLY_MANA:    ch->max_mana    -= mod;         break;
            case APPLY_HIT:     ch->max_hit     -= mod;         break;
            case APPLY_MOVE:    ch->max_move    -= mod;         break;
          }
        }
      }
    }

    /* now reset the permanent stats */
    ch->pcdata->perm_hit    = ch->max_hit;
    ch->pcdata->perm_mana   = ch->max_mana;
    ch->pcdata->perm_move   = ch->max_move;
    ch->pcdata->last_level  = ch->played/3600;
    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
        {
        if (ch->sex > 0 && ch->sex < 3)
                ch->pcdata->true_sex    = ch->sex;
        else
            ch->pcdata->true_sex    = 0;
        }

    ch->affected_by = race_table[ch->race].aff;
    ch->imm_flags   = race_table[ch->race].imm;
    ch->res_flags   = race_table[ch->race].res;
    ch->vuln_flags  = race_table[ch->race].vuln;
    ch->form    = race_table[ch->race].form;
    ch->parts   = race_table[ch->race].parts;

    /* now restore the character to his/her true condition */
    for (stat = 0; stat < MAX_STATS; stat++)
    ch->mod_stat[stat] = 0;

    for (i = 0; i <= MAX_CSATTRIBUTES; i ++)
        ch->csattrib_mod[i] = 0;

    if (ch->pcdata->true_sex < 0 || ch->pcdata->true_sex > 2)
    ch->pcdata->true_sex = 0;
    ch->sex     = ch->pcdata->true_sex;
    ch->max_hit     = ch->pcdata->perm_hit;
    ch->max_mana    = ch->pcdata->perm_mana;
    ch->max_move    = ch->pcdata->perm_move;

    if (IS_VAMP(ch) && is_affected(ch, gsn_bloodofpotency))
    {
        oldgen = oldbp = 0;
        oldgen = ch->gen;
        oldbp = ch->max_pblood;
    }

    for (i = 0; i < 4; i++)
        ch->armor[i]    = 100;

    ch->hitroll     = 0;
    ch->damroll     = 0;
    ch->saving_throw    = 0;


    /* now start adding back the effects */
    for (loc = 0; loc < MAX_WEAR; loc++)
    {
        obj = get_eq_char(ch,loc);
        if (obj == NULL)
            continue;
    for (i = 0; i < 4; i++)
        ch->armor[i] -= apply_ac( obj, loc, i );

        if (!obj->enchanted)
        for ( af = obj->pIndexData->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;

            if ( af->location != APPLY_SPELL_AFFECT )
                affect_modify( ch, af, TRUE );
        }

        for ( af = obj->affected; af != NULL; af = af->next )
        {
            mod = af->modifier;

            if ( af->location == APPLY_SPELL_AFFECT )
                    affect_to_char ( ch, af );
            else
                affect_modify( ch, af, TRUE );

        }
    }

    /* now add back spell effects */
    for (af = ch->affected; af != NULL; af = af->next)
        affect_modify( ch, af, TRUE );

    /* make sure sex is RIGHT!!!! */
    if (ch->sex < 0 || ch->sex > 2)
    ch->sex = ch->pcdata->true_sex;

    if (IS_VAMP(ch) && is_affected(ch, gsn_bloodofpotency))
    {
        ch->gen = oldgen;
        ch->max_pblood = oldbp;
    }

    if (fanged == TRUE)
      SET_BIT(ch->affected_by, AFF_FANGS);
}


/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    if ( ch->desc != NULL && ch->desc->original != NULL )
    ch = ch->desc->original;

    if (ch->trust)
    return ch->trust;

    if ( IS_NPC(ch) && ch->level > LEVEL_HERO )
    return LEVEL_HERO;
    else
    return ch->level;
}

int IS_VAMP( CHAR_DATA *ch )
{
    if(ch->race == race_lookup("vampire"))
        return race_lookup("vampire");
    if(ch->race == race_lookup("methuselah"))
            return race_lookup("methuselah");
    if(ch->race == race_lookup("dhampire"))
            return race_lookup("dhampire");
    if(ch->race == race_lookup("ghoul"))
            return race_lookup("ghoul");
    else
    return 0;
}

bool has_blood( CHAR_DATA *ch )
{
	if(
	ch->race == race_lookup("bane") ||
  ch->race == race_lookup("centipede") ||
	ch->race == race_lookup("construct") ||
	ch->race == race_lookup("elemental") ||
	ch->race == race_lookup("insect") ||
  ch->race == race_lookup("ooze") ||
	ch->race == race_lookup("shadow") ||
	ch->race == race_lookup("spider") ||
	ch->race == race_lookup("spirit") ||
	ch->race == race_lookup("undead") ||
	ch->race == race_lookup("vegetation") ||
	ch->race == race_lookup("wraith") ||
  ch->race == race_lookup("placeholder") ||
	ch->race == race_lookup("unique")
    )
		return FALSE;

	else
		return TRUE;
}

bool breathes( CHAR_DATA *ch )
{
	if(
  ch->race == race_lookup("methuselah") ||
  ch->race == race_lookup("vampire") ||
	ch->race == race_lookup("bane") ||
  ch->race == race_lookup("centipede") ||
	ch->race == race_lookup("construct") ||
	ch->race == race_lookup("elemental") ||
	ch->race == race_lookup("insect") ||
  ch->race == race_lookup("ooze") ||
	ch->race == race_lookup("shadow") ||
	ch->race == race_lookup("spider") ||
	ch->race == race_lookup("spirit") ||
	ch->race == race_lookup("undead") ||
	ch->race == race_lookup("vegetation") ||
	ch->race == race_lookup("wraith") ||
  ch->race == race_lookup("placeholder") ||
	ch->race == race_lookup("unique")
    )
		return FALSE;

	else
		return TRUE;
}

bool is_natural( CHAR_DATA *ch )
{
  if (
    ch->race == race_lookup("dhampire") ||
    ch->race == race_lookup("garou") ||
  	ch->race == race_lookup("ghoul") ||
    ch->race == race_lookup("human") ||
    ch->race == race_lookup("methuselah") ||
  	ch->race == race_lookup("vampire") ||
  	ch->race == race_lookup("abomination") ||
  	ch->race == race_lookup("bane") ||
    ch->race == race_lookup("bygone") ||
  	ch->race == race_lookup("construct") ||
  	ch->race == race_lookup("demon") ||
  	ch->race == race_lookup("dragon") ||
  	ch->race == race_lookup("elemental") ||
  	ch->race == race_lookup("faerie") ||
  	ch->race == race_lookup("fera") ||
    ch->race == race_lookup("kinfolk") ||
    ch->race == race_lookup("vampire") ||
  	ch->race == race_lookup("kuei-jin") ||
    ch->race == race_lookup("mage") ||
  	ch->race == race_lookup("ooze") ||
  	ch->race == race_lookup("romani") ||
  	ch->race == race_lookup("shadow") ||
    ch->race == race_lookup("spirit") ||
  	ch->race == race_lookup("undead") ||
  	ch->race == race_lookup("vegetation") ||
  	ch->race == race_lookup("wraith") ||
  	ch->race == race_lookup("placeholder") ||
  	ch->race == race_lookup("unique")
    )
    return FALSE;

  else
    return TRUE;
}

/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) (current_time - ch->logon) ) / 72000;
}

/* command for retrieving stats */
int get_curr_stat( CHAR_DATA *ch, int stat )
{
    int max;

    return 25;
}

int get_attribute( CHAR_DATA *ch, int stat )
{

    if(ch->csattributes[stat]+ch->csattrib_mod[stat] > 10)
        return 10;

    if(ch->csattributes[stat]+ch->csattrib_mod[stat] < 0)
        return 0;

    return ch->csattributes[stat]+ch->csattrib_mod[stat];
}

int get_ability( CHAR_DATA *ch, int stat )
{
    return ch->csabilities[stat];
}

int get_ability2( CHAR_DATA *ch, int stat )
{
    if (IS_NPC(ch))
        return 1 + ch->level/30;

    return ch->pcdata->cssec_abil[stat];
}

int get_discipline( CHAR_DATA *ch, int disc)
{
    if (!IS_VAMP(ch))
        return 0;

    if (!IS_NPC(ch))
        return ch->pcdata->discipline[disc];

    return ch->level / 24;
}

/* command for returning max training score */
int get_max_train( CHAR_DATA *ch, int stat )
{
    return 25;
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
    return 1000;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
    return 5;

    if(!IS_NPC(ch))
        return (MAX_WEAR * 3) + (30*get_attribute(ch,DEXTERITY))+(ch->remorts);

    else
        return MAX_WEAR +  2 * get_curr_stat(ch,STAT_DEX) + ch->level;
}


/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
    return 10000000;

    if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET) )
    return 0;

    if(!IS_NPC(ch))
        return 10*(((get_attribute(ch,STRENGTH)+ch->potence)*100) + ch->remorts);
    else
        return str_app[get_curr_stat(ch,STAT_STR)].carry * 10 + ch->level * 25;
}



/*
 * See if a string is one of the names of an object.
 */

bool is_name ( char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH], part[MAX_INPUT_LENGTH];
    char *list, *string;

    /* fix crash on NULL namelist */
    if (namelist == NULL || namelist[0] == '\0')
        return FALSE;

    /* fixed to prevent is_name on "" returning TRUE */
    if (str[0] == '\0')
    return FALSE;

    string = str;
    /* we need ALL parts of string to match part of namelist */
    for ( ; ; )  /* start parsing string */
    {
    str = one_argument(str,part);

    if (part[0] == '\0' )
        return TRUE;

    /* check to see if this is part of namelist */
    list = namelist;
    for ( ; ; )  /* start parsing namelist */
    {
        list = one_argument(list,name);
        if (name[0] == '\0')  /* this name was not found */
        return FALSE;

        if (!str_prefix(string,name))
        return TRUE; /* full pattern match */

        if (!str_prefix(part,name))
        break;
    }
    }
}

bool is_exact_name(char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    if (namelist == NULL)
    return FALSE;

    for ( ; ; )
    {
    namelist = one_argument( namelist, name );
    if ( name[0] == '\0' )
        return FALSE;
    if ( !str_cmp( str, name ) )
        return TRUE;
    }
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd )
{
    OBJ_DATA *wield;
    int mod,i;

    mod = paf->modifier;

    if ( fAdd )
    {
    switch (paf->where)
    {
    case TO_AFFECTS:
        SET_BIT(ch->affected_by, paf->bitvector);
        break;
    case TO_AFFECTS2:
        SET_BIT(ch->affected2_by, paf->bitvector);
        break;
    case TO_IMMUNE:
        SET_BIT(ch->imm_flags,paf->bitvector);
        break;
    case TO_RESIST:
        SET_BIT(ch->res_flags,paf->bitvector);
        break;
    case TO_VULN:
        SET_BIT(ch->vuln_flags,paf->bitvector);
        break;
    }
    }
    else
    {
        switch (paf->where)
        {
        case TO_AFFECTS:
            REMOVE_BIT(ch->affected_by, paf->bitvector);
            break;
        case TO_AFFECTS2:
            REMOVE_BIT(ch->affected2_by, paf->bitvector);
            break;
        case TO_IMMUNE:
            REMOVE_BIT(ch->imm_flags,paf->bitvector);
            break;
        case TO_RESIST:
            REMOVE_BIT(ch->res_flags,paf->bitvector);
            break;
        case TO_VULN:
            REMOVE_BIT(ch->vuln_flags,paf->bitvector);
            break;
        }
    mod = 0 - mod;
    }

    switch ( paf->location )
    {
    default:
    bug( "Affect_modify: unknown location %d.", paf->location );
    return;

    case APPLY_NONE:                        break;
    case APPLY_STR:           ch->mod_stat[STAT_STR]    += mod; break;
    case APPLY_DEX:           ch->mod_stat[STAT_DEX]    += mod; break;
    case APPLY_INT:           ch->mod_stat[STAT_INT]    += mod; break;
    case APPLY_WIS:           ch->mod_stat[STAT_WIS]    += mod; break;
    case APPLY_CON:           ch->mod_stat[STAT_CON]    += mod; break;
    case APPLY_SEX:           ch->sex           += mod; break;
    case APPLY_CLASS:                       break;
    case APPLY_LEVEL:                       break;
    case APPLY_AGE:                     break;
    case APPLY_ALIGN:                                   break;
    case APPLY_GENERATION:    ch->gen           += mod; break;
    case APPLY_CS_STR:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_STRENGTH] += mod;} break;
    case APPLY_CS_DEX:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_DEXTERITY]  += mod;} break;
    case APPLY_CS_STA:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_STAMINA]  += mod;} break;
    case APPLY_CS_CHA:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_CHARISMA]  += mod;} break;
    case APPLY_CS_MAN:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_MANIPULATION]  += mod;} break;
    case APPLY_CS_APP:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_APPEARANCE]  += mod;} break;
    case APPLY_CS_PER:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_PERCEPTION]  += mod;} break;
    case APPLY_CS_INT:      if(!IS_NPC(ch)) ch->csattrib_mod[CSATTRIB_INTELLIGENCE]  += mod; break;
    case APPLY_CS_WIT:      if(!IS_NPC(ch)) {ch->csattrib_mod[CSATTRIB_WITS]  += mod;} break;
    case APPLY_HEIGHT:                      break;
    case APPLY_WEIGHT:                      break;
    case APPLY_MANA:          ch->max_mana      += mod; break;
    case APPLY_HIT:           ch->max_hit       += mod; break;
    case APPLY_MOVE:          ch->max_move      += mod; break;
    case APPLY_GOLD:                        break;
    case APPLY_EXP:                     break;

    case APPLY_MAX_BLOOD:   ch->max_pblood      += mod; break;

    case APPLY_AC:
        for (i = 0; i < 4; i ++)
            ch->armor[i] += mod;
        break;
    case APPLY_HITROLL:       ch->hitroll       += mod; break;
    case APPLY_DAMROLL:       ch->damroll       += mod; break;
    case APPLY_SAVES:   ch->saving_throw        += mod; break;
    case APPLY_SAVING_ROD:    ch->saving_throw      += mod; break;
    case APPLY_SAVING_PETRI:  ch->saving_throw      += mod; break;
    case APPLY_SAVING_BREATH: ch->saving_throw      += mod; break;
    case APPLY_SAVING_SPELL:  ch->saving_throw      += mod; break;
    case APPLY_SPELL_AFFECT:                    break;
    }

    /*
     * Check for weapon wielding.
     * Guard against recursion (for weapons with affects).
     */
    if ( !IS_NPC(ch) && ( wield = get_eq_char( ch, WEAR_WIELD ) ) != NULL
    &&   get_obj_weight(wield) > (get_attribute(ch, STRENGTH)*10*2))
    {
    static int depth;

    if ( depth == 0 )
    {
        depth++;
        act( "With an exaggerated sigh, you drop $p as it is too heavy for you to continuously wield.", ch, wield, NULL, TO_CHAR );
        act( "$n sighs heavily, then drops $p.", ch, wield, NULL, TO_ROOM );
        obj_from_char( wield );
        obj_to_room( wield, ch->in_room );
        depth--;
    }
    }

    return;
}


/* find an effect in an affect list */
AFFECT_DATA  *affect_find(AFFECT_DATA *paf, int sn)
{
    AFFECT_DATA *paf_find;

    for ( paf_find = paf; paf_find != NULL; paf_find = paf_find->next )
    {
        if ( paf_find->type == sn )
    return paf_find;
    }

    return NULL;
}

/* fix object affects when removing one */
void affect_check(CHAR_DATA *ch,int where,int vector)
{
    AFFECT_DATA *paf;
    OBJ_DATA *obj;

    if (where == TO_OBJECT || where == TO_WEAPON || vector == 0)
    return;

    for (paf = ch->affected; paf != NULL; paf = paf->next)
    if (paf->where == where && paf->bitvector == vector)
    {
        switch (where)
        {
            case TO_AFFECTS:
            SET_BIT(ch->affected_by,vector);
            break;
          case TO_AFFECTS2:
                SET_BIT(ch->affected2_by, vector);
                break;
            case TO_IMMUNE:
            SET_BIT(ch->imm_flags,vector);
            break;
            case TO_RESIST:
            SET_BIT(ch->res_flags,vector);
            break;
            case TO_VULN:
            SET_BIT(ch->vuln_flags,vector);
            break;
        }
        return;
    }

    for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    {
    if (obj->wear_loc == -1)
        continue;

            for (paf = obj->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:
                        SET_BIT(ch->affected_by,vector);
                        break;
                    case TO_AFFECTS2:
                        SET_BIT(ch->affected2_by,vector);
                        break;
                    case TO_IMMUNE:
                        SET_BIT(ch->imm_flags,vector);
                        break;
                    case TO_RESIST:
                        SET_BIT(ch->res_flags,vector);
                        break;
                    case TO_VULN:
                        SET_BIT(ch->vuln_flags,vector);

                }
                return;
            }

        if (obj->enchanted)
        continue;

        for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
            if (paf->where == where && paf->bitvector == vector)
            {
                switch (where)
                {
                    case TO_AFFECTS:
                        SET_BIT(ch->affected_by,vector);
                        break;
                    case TO_AFFECTS2:
                        SET_BIT(ch->affected2_by,vector);
                        break;
                    case TO_IMMUNE:
                        SET_BIT(ch->imm_flags,vector);
                        break;
                    case TO_RESIST:
                        SET_BIT(ch->res_flags,vector);
                        break;
                    case TO_VULN:
                        SET_BIT(ch->vuln_flags,vector);
                        break;
                }
                return;
            }
    }
}

/*
 * Give an affect to a char.
 */
void affect_to_char( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect();

    *paf_new        = *paf;

    VALIDATE(paf);  /* in case we missed it when we set up paf */
    paf_new->next   = ch->affected;
    ch->affected    = paf_new;

    affect_modify( ch, paf_new, TRUE );
    return;
}

/* give an affect to an object */
void affect_to_obj(OBJ_DATA *obj, AFFECT_DATA *paf)
{
    AFFECT_DATA *paf_new;

    paf_new = new_affect();

    *paf_new        = *paf;

    VALIDATE(paf);  /* in case we missed it when we set up paf */
    paf_new->next   = obj->affected;
    obj->affected   = paf_new;

    /* apply any affect vectors to the object's extra_flags */
    if (paf->bitvector)
        switch (paf->where)
        {
        case TO_OBJECT:
            SET_BIT(obj->extra_flags,paf->bitvector);
        break;
        case TO_WEAPON:
        if (obj->item_type == ITEM_WEAPON)
            SET_BIT(obj->value[4],paf->bitvector);
        break;
        }


    return;
}



/*
 * Remove an affect from a char.
 */
void affect_remove( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    int where;
    int vector;

    if ( ch->affected == NULL )
    {
    bug( "Affect_remove: no affect.", 0 );
    return;
    }

    affect_modify( ch, paf, FALSE );
    where = paf->where;
    vector = paf->bitvector;

    if ( paf == ch->affected )
    {
    ch->affected    = paf->next;
    }
    else
    {
    AFFECT_DATA *prev;

    for ( prev = ch->affected; prev != NULL; prev = prev->next )
    {
        if ( prev->next == paf )
        {
        prev->next = paf->next;
        break;
        }
    }

    if ( prev == NULL )
    {
        bug( "Affect_remove: cannot find paf.", 0 );
        return;
    }
    }

    free_affect(paf);

    affect_check(ch,where,vector);
    return;
}

void affect_remove_obj( OBJ_DATA *obj, AFFECT_DATA *paf)
{
    int where, vector;
    if ( obj->affected == NULL )
    {
        bug( "Affect_remove_object: no affect.", 0 );
        return;
    }

    if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_modify( obj->carried_by, paf, FALSE );

    where = paf->where;
    vector = paf->bitvector;

    /* remove flags from the object if needed */
    if (paf->bitvector)
    switch( paf->where)
        {
        case TO_OBJECT:
            REMOVE_BIT(obj->extra_flags,paf->bitvector);
            break;
        case TO_WEAPON:
            if (obj->item_type == ITEM_WEAPON)
                REMOVE_BIT(obj->value[4],paf->bitvector);
            break;
        }

    if ( paf == obj->affected )
    {
        obj->affected    = paf->next;
    }
    else
    {
        AFFECT_DATA *prev;

        for ( prev = obj->affected; prev != NULL; prev = prev->next )
        {
            if ( prev->next == paf )
            {
                prev->next = paf->next;
                break;
            }
        }

        if ( prev == NULL )
        {
            bug( "Affect_remove_object: cannot find paf.", 0 );
            return;
        }
    }

    free_affect(paf);

    if (obj->carried_by != NULL && obj->wear_loc != -1)
    affect_check(obj->carried_by,where,vector);
    return;
}



/*
 * Strip all affects of a given sn.
 */
void affect_strip( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
    paf_next = paf->next;
    if ( paf->type == sn )
        affect_remove( ch, paf );
    }

    return;
}



/*
 * Return true if a char is affected by a spell.
 */
bool is_affected( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
    if ( paf->type == sn )
        return TRUE;
    }

    return FALSE;
}

/*
 * Return true if a char is affected by a spell.
 */
int get_affect_level( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
    if ( paf->type == sn )
        return paf->level;
    }

    return 0;
}

/*
 * Return affect modifier.
 */
int get_affect_modifier( CHAR_DATA *ch, int sn )
{
    AFFECT_DATA *paf;

    for ( paf = ch->affected; paf != NULL; paf = paf->next )
    {
    if ( paf->type == sn )
        return paf->modifier;
    }

    return 0;
}

/*
 * Add or enhance an affect.
 */
void affect_join( CHAR_DATA *ch, AFFECT_DATA *paf )
{
    AFFECT_DATA *paf_old;
    bool found;

    found = FALSE;
    for ( paf_old = ch->affected; paf_old != NULL; paf_old = paf_old->next )
    {
    if ( paf_old->type == paf->type )
    {
        paf->level = (paf->level += paf_old->level) / 2;
        paf->duration += paf_old->duration;
        paf->modifier += paf_old->modifier;
        affect_remove( ch, paf_old );
        break;
    }
    }

    affect_to_char( ch, paf );
    return;
}

/*
 * Check if char has self induced shadowplay (+1 stealth/intimidation die)
 * Return 1 dice result if so.
*/
int stealth_int_shadowplay( CHAR_DATA *ch, int diff )
{
	if (is_affected(ch, gsn_shadowplay) && get_affect_level(ch, gsn_shadowplay) == 0)
		return godice(1, diff);
	else
		return 0;
}

/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    OBJ_DATA *obj;

    if ( ch->in_room == NULL )
    {
    bug( "Char_from_room: NULL.", 0 );
    return;
    }

    if ( !IS_NPC(ch) )
    --ch->in_room->area->nplayer;

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room->light > 0 )
    --ch->in_room->light;

    if (is_affected(ch, gsn_gift_lambentfire) && ch->in_room->light > 0)
    {
      ch->in_room->light -= 2;
      if (ch->in_room->light < 0)
        ch->in_room->light = 0;
    }

    if ( ch == ch->in_room->people )
    {
    ch->in_room->people = ch->next_in_room;
    }
    else
    {
    CHAR_DATA *prev;

    for ( prev = ch->in_room->people; prev; prev = prev->next_in_room )
    {
        if ( prev->next_in_room == ch )
        {
        prev->next_in_room = ch->next_in_room;
        break;
        }
    }

    if ( prev == NULL )
        bug( "Char_from_room: ch not found.", 0 );
    }

    ch->in_room      = NULL;
    ch->next_in_room = NULL;
    ch->on       = NULL;  /* sanity check! */
    return;
}



/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    OBJ_DATA *obj;

    if ( pRoomIndex == NULL )
    {
    ROOM_INDEX_DATA *room;

    bug( "Char_to_room: NULL.", 0 );

    if ((room = get_room_index(ROOM_VNUM_TEMPLE)) != NULL)
        char_to_room(ch,room);

    return;
    }

    ch->in_room     = pRoomIndex;
    ch->next_in_room    = pRoomIndex->people;
    pRoomIndex->people  = ch;

    if ( !IS_NPC(ch) )
    {
    if (ch->in_room->area->empty)
    {
        ch->in_room->area->empty = FALSE;
        ch->in_room->area->age = 0;
    }
    ++ch->in_room->area->nplayer;
    }

    if ( ( obj = get_eq_char( ch, WEAR_LIGHT ) ) != NULL
    &&   obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0 )
    ++ch->in_room->light;

    if (is_affected(ch, gsn_gift_lambentfire))
      ch->in_room->light += 2;

 /*   if (IS_AFFECTED(ch,AFF_PLAGUE))
    {
        AFFECT_DATA *af, plague;
        CHAR_DATA *vch;

        for ( af = ch->affected; af != NULL; af = af->next )
        {
            if (af->type == gsn_plague)
                break;
        }

        if (af == NULL)
        {
            REMOVE_BIT(ch->affected_by,AFF_PLAGUE);
            return;
        }

        if (af->level == 1)
            return;

    plague.where        = TO_AFFECTS;
        plague.type         = gsn_plague;
        plague.level        = af->level - 1;
        plague.duration     = number_range(1,2 * plague.level);
        plague.location     = APPLY_STR;
        plague.modifier     = -5;
        plague.bitvector    = AFF_PLAGUE;

        for ( vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
        {
            if (!saves_spell(plague.level - 2,vch,DAM_DISEASE)
        &&  !IS_IMMORTAL(vch) &&
                !IS_AFFECTED(vch,AFF_PLAGUE) && number_bits(6) == 0)
            {
                send_to_char("You feel hot and feverish.\n\r",vch);
                act("$n shivers and looks very ill.",vch,NULL,NULL,TO_ROOM);
                affect_join(vch,&plague);
            }
        }
    }
*/

    return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_content    = ch->carrying;
    ch->carrying     = obj;
    obj->carried_by  = ch;
    obj->in_room     = NULL;
    obj->in_obj      = NULL;
    ch->carry_number    += get_obj_number( obj );
    ch->carry_weight    += get_obj_weight( obj );
}



/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
    bug( "Obj_from_char: null ch.", 0 );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    unequip_char( ch, obj );

    if ( ch->carrying == obj )
    {
    ch->carrying = obj->next_content;
    }
    else
    {
    OBJ_DATA *prev;

    for ( prev = ch->carrying; prev != NULL; prev = prev->next_content )
    {
        if ( prev->next_content == obj )
        {
        prev->next_content = obj->next_content;
        break;
        }
    }

    if ( prev == NULL )
        bug( "Obj_from_char: obj not in list.", 0 );
    }

    obj->carried_by  = NULL;
    obj->next_content    = NULL;
    ch->carry_number    -= get_obj_number( obj );
    ch->carry_weight    -= get_obj_weight( obj );
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear, int type )
{
    if ( obj->item_type != ITEM_ARMOR )
    return 0;

    switch ( iWear )
    {
    case WEAR_TORSO: return 3 * obj->value[type];
    case WEAR_HEAD: return 2 * obj->value[type];
    case WEAR_LEGS: return 2 * obj->value[type];
    case WEAR_FEET: return     obj->value[type];
    case WEAR_HANDS:    return     obj->value[type];
    case WEAR_ARMS: return     obj->value[type];
    case WEAR_SHIELD:   return     obj->value[type];
    case WEAR_NECK_1:   return     obj->value[type];
    case WEAR_NECK_2:   return     obj->value[type];
    case WEAR_BODY:    return 2 * obj->value[type];
    case WEAR_WAIST:    return     obj->value[type];
    case WEAR_WRIST_L:  return     obj->value[type];
    case WEAR_WRIST_R:  return     obj->value[type];
    case WEAR_EAR_L:    return     obj->value[type];
    case WEAR_EAR_R:    return     obj->value[type];
    case WEAR_FACE: return     obj->value[type];
    case WEAR_HOLD: return     obj->value[type];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    if (ch == NULL)
    return NULL;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
    if ( obj->wear_loc == iWear )
        return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    char buf[MAX_STRING_LENGTH];
    int i;

    if ( get_eq_char( ch, iWear ) != NULL )
    {
    sprintf(buf, "Equip_char: already equipped (%d) (%s)", iWear, IS_NPC(ch) ? ch->short_descr : ch->name);
    bug(buf,0);
/*  bug( "Equip_char: already equipped (%d) (%s).", iWear, IS_NPC(ch) ? ch->short_descr : ch->name); */
    return;
    }

    for (i = 0; i < 4; i++)
        ch->armor[i]        -= apply_ac( obj, iWear,i );
    obj->wear_loc    = iWear;

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        if ( paf->location != APPLY_SPELL_AFFECT )
            affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    if ( paf->location == APPLY_SPELL_AFFECT )
            affect_to_char ( ch, paf );
    else
        affect_modify( ch, paf, TRUE );

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL )
    ++ch->in_room->light;

    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    AFFECT_DATA *paf = NULL;
    AFFECT_DATA *lpaf = NULL;
    AFFECT_DATA *lpaf_next = NULL;
    int i;
    char error[MSL];

    if ( obj->wear_loc == WEAR_NONE )
    {
      sprintf(error, "Unequip_char: %s (V#%d) in Room V#%d -- %s (V#%d) already unequipped.",
      ch->short_descr, IS_NPC(ch) ? ch->pIndexData->vnum : 0,
      ch->in_room->vnum, obj->short_descr, obj->pIndexData->vnum);
    bug( error, 0 );
    return;
    }

    for (i = 0; i < 4; i++)
        ch->armor[i]    += apply_ac( obj, obj->wear_loc,i );
    obj->wear_loc    = -1;

    if (!obj->enchanted)
    {
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        if ( paf->location == APPLY_SPELL_AFFECT )
        {
            for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
            {
            lpaf_next = lpaf->next;
            if ((lpaf->type == paf->type) &&
                (lpaf->level == paf->level) &&
                (lpaf->location == APPLY_SPELL_AFFECT))
            {
                affect_remove( ch, lpaf );
            lpaf_next = NULL;
            }
            }
        }
        else
        {
            affect_modify( ch, paf, FALSE );
        affect_check(ch,paf->where,paf->bitvector);
        }
     }
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    if ( paf->location == APPLY_SPELL_AFFECT )
    {
        bug ( "Norm-Apply: %d", 0 );
        for ( lpaf = ch->affected; lpaf != NULL; lpaf = lpaf_next )
        {
        lpaf_next = lpaf->next;
        if ((lpaf->type == paf->type) &&
            (lpaf->level == paf->level) &&
            (lpaf->location == APPLY_SPELL_AFFECT))
        {
            bug ( "location = %d", lpaf->location );
            bug ( "type = %d", lpaf->type );
            affect_remove( ch, lpaf );
            lpaf_next = NULL;
        }
        }
    }
    else
    {
        affect_modify( ch, paf, FALSE );
        affect_check(ch,paf->where,paf->bitvector);
    }

    if ( obj->item_type == ITEM_LIGHT
    &&   obj->value[2] != 0
    &&   ch->in_room != NULL
    &&   ch->in_room->light > 0 )
    --ch->in_room->light;

    return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int nMatch;

    nMatch = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
    if ( obj->pIndexData == pObjIndex )
        nMatch++;
    }

    return nMatch;
}



/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;
    CHAR_DATA *ch;

    if ( ( in_room = obj->in_room ) == NULL )
    {
    bug( "obj_from_room: NULL.", 0 );
    return;
    }

    for (ch = in_room->people; ch != NULL; ch = ch->next_in_room)
    if (ch->on == obj)
        ch->on = NULL;

    if ( obj == in_room->contents )
    {
    in_room->contents = obj->next_content;
    }
    else
    {
    OBJ_DATA *prev;

    for ( prev = in_room->contents; prev; prev = prev->next_content )
    {
        if ( prev->next_content == obj )
        {
        prev->next_content = obj->next_content;
        break;
        }
    }

    if ( prev == NULL )
    {
        bug( "Obj_from_room: obj not found.", 0 );
        return;
    }
    }

    if ( IS_SET(obj->extra_flags, ITEM_ILLUMINATE)
    &&   in_room != NULL )
    --in_room->light;

    obj->in_room      = NULL;
    obj->next_content = NULL;
    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
    obj->next_content       = pRoomIndex->contents;
    pRoomIndex->contents    = obj;
    obj->in_room        = pRoomIndex;
    obj->carried_by     = NULL;
    obj->in_obj         = NULL;

    if ( IS_SET(obj->extra_flags, ITEM_ILLUMINATE)
    &&   obj->in_room != NULL )
    ++obj->in_room->light;
    return;
}



/*
 * Move an object into an object.
 */
void obj_to_obj( OBJ_DATA *obj, OBJ_DATA *obj_to )
{
    obj->next_content       = obj_to->contains;
    obj_to->contains        = obj;
    obj->in_obj         = obj_to;
    obj->in_room        = NULL;
    obj->carried_by     = NULL;
    if (obj_to->pIndexData->vnum == OBJ_VNUM_PIT)
        obj->cost = 0;

    for ( ; obj_to != NULL; obj_to = obj_to->in_obj )
    {
    if ( obj_to->carried_by != NULL )
    {
        obj_to->carried_by->carry_number += get_obj_number( obj );
        obj_to->carried_by->carry_weight += get_obj_weight( obj )
        * WEIGHT_MULT(obj_to) / 100;
    }
    }

    return;
}



/*
 * Move an object out of an object.
 */
void obj_from_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_from;

    if ( ( obj_from = obj->in_obj ) == NULL )
    {
    bug( "Obj_from_obj: null obj_from.", 0 );
    return;
    }

    if ( obj == obj_from->contains )
    {
    obj_from->contains = obj->next_content;
    }
    else
    {
    OBJ_DATA *prev;

    for ( prev = obj_from->contains; prev; prev = prev->next_content )
    {
        if ( prev->next_content == obj )
        {
        prev->next_content = obj->next_content;
        break;
        }
    }

    if ( prev == NULL )
    {
        bug( "Obj_from_obj: obj not found.", 0 );
        return;
    }
    }

    obj->next_content = NULL;
    obj->in_obj       = NULL;

    for ( ; obj_from != NULL; obj_from = obj_from->in_obj )
    {
    if ( obj_from->carried_by != NULL )
    {
        obj_from->carried_by->carry_number -= get_obj_number( obj );
        obj_from->carried_by->carry_weight -= get_obj_weight( obj )
        * WEIGHT_MULT(obj_from) / 100;
    }
    }

    return;
}



/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
    OBJ_DATA *obj_content;
    OBJ_DATA *obj_next;

    if ( obj->in_room != NULL )
    obj_from_room( obj );
    else if ( obj->carried_by != NULL )
    obj_from_char( obj );
    else if ( obj->in_obj != NULL )
    obj_from_obj( obj );

    for ( obj_content = obj->contains; obj_content; obj_content = obj_next )
    {
    obj_next = obj_content->next_content;
    extract_obj( obj_content );
    }

    if ( object_list == obj )
    {
    object_list = obj->next;
    }
    else
    {
    OBJ_DATA *prev;

    for ( prev = object_list; prev != NULL; prev = prev->next )
    {
        if ( prev->next == obj )
        {
        prev->next = obj->next;
        break;
        }
    }

    if ( prev == NULL )
    {
        char buf[MSL];
        sprintf(buf, "Extract_obj: obj %d (%s) not found in object list", obj->pIndexData->vnum, obj->name);
        bug( buf, 0);
        return;
    }
    }

    --obj->pIndexData->count;
    free_obj(obj);
    return;
}



/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    /* doesn't seem to be necessary
    if ( ch->in_room == NULL )
    {
    bug( "Extract_char: NULL.", 0 );
    return;
    }
    */

    if(ch->mount != NULL)
        do_function(ch->mount, &do_dismount, "");


    nuke_pets(ch);
    ch->pet = NULL; /* just in case */

    if ( fPull )

    die_follower( ch );

    stop_fighting( ch, TRUE );

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
    obj_next = obj->next_content;
    if (obj->pIndexData->vnum != OBJ_VNUM_FETISH)
        extract_obj( obj );
    }

    if (ch->in_room != NULL)
        char_from_room( ch );

    /* Death room is set in the clan tabe now */
    if ( !fPull )
    {
        char_to_room(ch,get_room_index(ROOM_VNUM_ALTAR));
    return;
    }

    if ( IS_NPC(ch) )
    {
        --ch->pIndexData->count;
    }
    else
    {

    nuke_pets(ch);
    ch->pet = NULL;

    if ( ch->mount && ch->mount->mount == ch )
    {
        ch->mount->mount = NULL;

        if (ch->mount->riding)
        {
        act("You fall off of $N.", ch->mount, NULL, ch, TO_CHAR);
        act("$n falls off of $N.", ch->mount, NULL, ch, TO_ROOM);
        ch->mount->riding = FALSE;
        if (!IS_IMMORTAL(ch->mount))
            ch->mount->position = POS_SITTING;
        }
    }
    }
    if ( ch->desc != NULL && ch->desc->original != NULL )
    {
    do_function(ch, &do_return, "" );
    ch->desc = NULL;
    }

    for ( wch = char_list; wch != NULL; wch = wch->next )
    {
    if ( wch->reply == ch )
        wch->reply = NULL;
    if ( ch->mprog_target == wch )
        wch->mprog_target = NULL;
    }

    if ( ch == char_list )
    {
       char_list = ch->next;
    }
    else
    {
    CHAR_DATA *prev;

    for ( prev = char_list; prev != NULL; prev = prev->next )
    {
        if ( prev->next == ch )
        {
        prev->next = ch->next;
        break;
        }
    }

    if ( prev == NULL )
    {
        bug( "Extract_char: char not found.", 0 );
        return;
    }
    }

    if ( ch->desc != NULL )
    ch->desc->character = NULL;
    free_char( ch );
    return;
}



/*
 * Find a char in the room.
 */
 CHAR_DATA *get_char_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument )
 {
     char arg[MAX_INPUT_LENGTH];
     CHAR_DATA *rch;
     int number;
     int count;

     number = number_argument( argument, arg );
     count  = 0;
     if ( !str_cmp( arg, "self" ) )
 	return ch;

     if ( ch && room )
     {
 	bug( "get_char_room received multiple types (ch/room)", 0 );
 	return NULL;
     }

     if ( ch )
 	rch = ch->in_room->people;
     else
 	rch = room->people;

     for ( ; rch != NULL; rch = rch->next_in_room )
     {
 	if ( (ch && !can_see( ch, rch )) || !is_name( arg, rch->name ) )
 	    continue;
 	if ( ++count == number )
 	    return rch;
     }

     return NULL;
 }




/*
 * Find a char in the world.
 */
 CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
 {
     char arg[MAX_INPUT_LENGTH];
     CHAR_DATA *wch;
     int number;
     int count;

     if ( ch && ( wch = get_char_room( ch, NULL, argument ) ) != NULL )
 	return wch;

     number = number_argument( argument, arg );
     count  = 0;
     for ( wch = char_list; wch != NULL ; wch = wch->next )
     {
 	if ( wch->in_room == NULL || ( ch && !can_see( ch, wch ) )
 	||   !is_name( arg, wch->name ) )
 	    continue;
 	if ( ++count == number )
 	    return wch;
     }

     return NULL;
 }

CHAR_DATA * get_char_area( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ach;
    int number, count;

    if ((ach = get_char_room( ch, NULL, argument )) != NULL)
    return ach;

    number = number_argument( argument, arg );
    count = 0;
    for ( ach = char_list; ach != NULL; ach = ach->next )
    {
    if (ach->in_room->area != ch->in_room->area
    ||  !can_see( ch, ach ) || !is_name( arg, ach->name ))
        continue;
    if (++count == number)
        return ach;
    }
    return NULL;
}


/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
    if ( obj->pIndexData == pObjIndex )
        return obj;
    }

    return NULL;
}


/*
 * Find an obj in a list.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
    if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
    {
        if ( ++count == number )
        return obj;
    }
    }

    return NULL;
}

/*
 * Find an obj in a list even if hidden
 */
OBJ_DATA *get_obj_list2( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_content )
    {
    if ( is_name( arg, obj->name ) )
    {
        if ( ++count == number )
        return obj;
    }
    }

    return NULL;
}


/*
 * Find an obj in player's inventory.
 */
 OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument, CHAR_DATA *viewer )
 {
     char arg[MAX_INPUT_LENGTH];
     OBJ_DATA *obj;
     int number;
     int count;

     number = number_argument( argument, arg );
     count  = 0;
     for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
     {
         if ( obj->wear_loc == WEAR_NONE
         &&   ( viewer ? can_see_obj( viewer, obj ) : TRUE )
         &&   is_name( arg, obj->name ) )
         {
             if ( ++count == number )
                             return obj;
         }
     }

     return NULL;
 }

/* Find an object a player is carrying based on vnum and wear location  - Ugha*/
OBJ_DATA *get_carry_vnum(CHAR_DATA *ch, int vnum, int location, bool sight)
{
    OBJ_DATA *obj;

    for(obj = ch->carrying;obj != NULL; obj = obj->next_content)
    {
        if(sight && !can_see_obj(ch,obj))
            continue;
        if(obj->wear_loc == location && obj->pIndexData->vnum == vnum)
            return obj;
    }
    return NULL;
}


/*
 * Find an obj in player's equipment.
 */
 OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument, bool character )
 {
     char arg[MAX_INPUT_LENGTH];
     OBJ_DATA *obj;
     int number;
     int count;

     number = number_argument( argument, arg );
     count  = 0;
     for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
     {
         if ( obj->wear_loc != WEAR_NONE
                 &&  ( character ? can_see_obj( ch, obj ) : TRUE)
         &&   is_name( arg, obj->name ) )
         {
             if ( ++count == number )
                 return obj;
         }
     }

     return NULL;
 }



/*
 * Find an obj in the room or in inventory.
 */
 OBJ_DATA *get_obj_here( CHAR_DATA *ch, ROOM_INDEX_DATA *room, char *argument )
 {
     OBJ_DATA *obj;
     int number, count;
     char arg[MAX_INPUT_LENGTH];

     if ( ch && room )
     {
 	bug( "get_obj_here received a ch and a room",0);
 	return NULL;
     }

     number = number_argument( argument, arg );
     count = 0;

     if ( ch )
     {
 	obj = get_obj_list( ch, argument, ch->in_room->contents );
 	if ( obj != NULL )
 	    return obj;

 	if ( ( obj = get_obj_carry( ch, argument, ch ) ) != NULL )
 	    return obj;

 	if ( ( obj = get_obj_wear( ch, argument, TRUE ) ) != NULL )
 	    return obj;
     }
     else
     {
 	for ( obj = room->contents; obj; obj = obj->next_content )
 	{
 	    if ( !is_name( arg, obj->name ) )
 		continue;
 	    if ( ++count == number )
 		return obj;
 	}
     }

     return NULL;
 }


/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_hidden( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    obj = get_obj_list2( ch, argument, ch->in_room->contents );
    if ( obj != NULL )
    return obj;

    return NULL;
}


/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument, bool unseen )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( ( obj = get_obj_here( ch, NULL, argument ) ) != NULL )
    return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
    if ( obj->in_room != NULL &&
    (unseen || can_see_obj( ch, obj )) &&
    is_name( arg, obj->name ) )
    {
        if ( ++count == number )
        return obj;
    }
    }

    return NULL;
}

/*
 * Find an obj in obj_list regardless of where.
 */
OBJ_DATA *get_obj_anywhere( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = object_list; obj != NULL; obj = obj->next )
    {
        if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
        {
            if ( ++count == number )
            return obj;
        }
    }

    return NULL;
}

/* deduct cost from a character */

void deduct_cost(CHAR_DATA *ch, int cost)
{
    int silver = 0, gold = 0;

    silver = UMIN(ch->silver,cost);

    if (silver < cost)
    {
    gold = ((cost - silver + 99) / 100);
    silver = cost - 100 * gold;
    }

    ch->gold -= gold;
    ch->silver -= silver;

    if (ch->gold < 0)
    ch->gold = 0;
    if (ch->silver < 0)
    ch->silver = 0;
}
/*
 * Create a 'money' obj.
 */
OBJ_DATA *create_money( int gold, int silver )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if ( gold < 0 || silver < 0 || (gold == 0 && silver == 0) )
    {
    bug( "Create_money: zero or negative money.",UMIN(gold,silver));
    gold = UMAX(1,gold);
    silver = UMAX(1,silver);
    }

    if (gold == 0 && silver == 1)
    {
    obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
    }
    else if (gold == 1 && silver == 0)
    {
    obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE), 0 );
    }
    else if (silver == 0)
    {
        obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );
        sprintf( buf, obj->short_descr, gold );
        free_string( obj->short_descr );
        obj->short_descr        = str_dup( buf );
        obj->value[1]           = gold;
        obj->cost               = gold;
    obj->weight     = gold/100;
    }
    else if (gold == 0)
    {
        obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );
        sprintf( buf, obj->short_descr, silver );
        free_string( obj->short_descr );
        obj->short_descr        = str_dup( buf );
        obj->value[0]           = silver;
        obj->cost               = silver;
    obj->weight     = silver/1000;  //temporary 40 from 20
    }

    else
    {
    obj = create_object( get_obj_index( OBJ_VNUM_COINS ), 0 );
    sprintf( buf, obj->short_descr, silver, gold );
    free_string( obj->short_descr );
    obj->short_descr    = str_dup( buf );
    obj->value[0]       = silver;
    obj->value[1]       = gold;
    obj->cost       = 100 * gold + silver;
    obj->weight     = gold / 100 + silver / 1000;
    }

    return obj;
}



/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    int number;

    if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_MONEY
    ||  obj->item_type == ITEM_GEM || obj->item_type == ITEM_JEWELRY
    ||  obj->pIndexData->vnum == 9)
        number = 0;
    else
        number = 1;

    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        number += get_obj_number( obj );

    return number;
}


/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;
    OBJ_DATA *tobj;

    weight = obj->weight;
    for ( tobj = obj->contains; tobj != NULL; tobj = tobj->next_content )
    weight += get_obj_weight( tobj ) * WEIGHT_MULT(obj) / 100;

    return weight;
}

int get_true_weight(OBJ_DATA *obj)
{
    int weight;

    weight = obj->weight;
    for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
        weight += get_obj_weight( obj );

    return weight;
}

/*
 * True if room is dark.
 */
bool room_is_dark( ROOM_INDEX_DATA *pRoomIndex )
{
    if ( pRoomIndex->light > 0 )
    return FALSE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_DARK) )
    return TRUE;

    if ( pRoomIndex->sector_type == SECT_INSIDE
    ||   pRoomIndex->sector_type == SECT_CITY )
    return FALSE;

    if ( weather_info.sunlight == SUN_SET
    ||   weather_info.sunlight == SUN_DARK )
    return TRUE;

    return FALSE;
}


bool is_room_owner(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
    if (room->owner == NULL || room->owner[0] == '\0')
    return FALSE;

    return is_name(ch->name,room->owner);
}

/*
 * True if room is private.
 */
bool room_is_private( ROOM_INDEX_DATA *pRoomIndex )
{
    CHAR_DATA *rch;
    int count;


    if (pRoomIndex->owner != NULL && pRoomIndex->owner[0] != '\0')
    return TRUE;

    count = 0;
    for ( rch = pRoomIndex->people; rch != NULL; rch = rch->next_in_room )
    count++;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE)  && count >= 2 )
    return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_SOLITARY) && count >= 1 )
    return TRUE;

    if ( IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY) )
    return TRUE;

    return FALSE;
}

/* visibility on a room -- for entering and exits */
bool can_see_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    if (IS_SET(pRoomIndex->room_flags, ROOM_IMP_ONLY)
    &&  get_trust(ch) < MAX_LEVEL)
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags, ROOM_GODS_ONLY)
    &&  !IS_IMMORTAL(ch))
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags, ROOM_HEROES_ONLY)
    &&  !IS_IMMORTAL(ch))
    return FALSE;

        if (IS_SET(pRoomIndex->room_flags,ROOM_HUMAN_ONLY)
    && !IS_IMMORTAL(ch) && ch->race != race_lookup("human") )
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags,ROOM_VAMPIRE_ONLY)
    && !IS_IMMORTAL(ch) && ch->race != race_lookup("vampire") )
    return FALSE;
     if (IS_SET(pRoomIndex->room_flags,ROOM_GHOUL_ONLY)
    && !IS_IMMORTAL(ch) && ch->race != race_lookup("ghoul") )
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags,ROOM_GAROU_ONLY)
    && !IS_IMMORTAL(ch) && ch->race != race_lookup("garou") )
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags,ROOM_MAGE_ONLY)
    && !IS_IMMORTAL(ch) && ch->avatar == 0 )
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags,ROOM_NEWBIES_ONLY)
    &&  !IS_NEWBIE(ch) && !IS_IMMORTAL(ch))
    return FALSE;

    if (IS_SET(pRoomIndex->room_flags,ROOM_UMBRA)
    && !IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_IMMORTAL(ch))
    return FALSE;

    if (!IS_IMMORTAL(ch) && pRoomIndex->clan && ch->clan != pRoomIndex->clan)
    return FALSE;

    return TRUE;
}

bool unseen_check(CHAR_DATA *ch, CHAR_DATA *victim)
{
    /* Checks all types of Unseen/veil versus PCs reveal*/
    if (IS_AFFECTED2(victim, AFF2_VEIL) &&
        (IS_NPC(victim) || !is_affected(ch, gsn_reveal)))
       return FALSE;

        if ((IS_AFFECTED2(victim, AFF2_VEIL) || is_affected(victim, gsn_veil)) &&
            is_affected(ch, gsn_reveal) && !IS_NPC(ch) && !IS_NPC(victim))
        {
            int success;
            int diff;
            diff = 7;
            if (victim->pcdata->discipline[OBFUSCATE] > ch->pcdata->discipline[AUSPEX])
                return FALSE;

            if (ch->pcdata->discipline[AUSPEX] > victim->pcdata->discipline[OBFUSCATE])
                diff -= (ch->pcdata->discipline[AUSPEX] - victim->pcdata->discipline[OBFUSCATE]);

            success = godice(get_attribute(ch, PERCEPTION)+ch->csabilities[CSABIL_ALERTNESS], diff);

            if (ch->pcdata->discipline[AUSPEX] == victim->pcdata->discipline[OBFUSCATE])
                success -= godice(get_attribute(victim, MANIPULATION) + victim->csabilities[CSABIL_SUBTERFUGE], diff);

            if (success < 1)
                return FALSE;
            }

    if (is_affected(victim, gsn_unseen) && !IS_AFFECTED2(ch, AFF2_DETECT_UNSEEN) &&
        !is_affected(ch, gsn_reveal) && victim->fighting == NULL && !IS_NPC(ch) && !IS_NPC(victim))
    return FALSE;

    if (is_affected(victim, gsn_unseen) && !IS_AFFECTED2(ch, AFF2_DETECT_UNSEEN) &&
        is_affected(ch, gsn_reveal) && victim->fighting == NULL && !IS_NPC(ch) && !IS_NPC(victim))
        {
            int success;
            int diff;
            diff = 7;
            if (victim->pcdata->discipline[OBFUSCATE] > ch->pcdata->discipline[AUSPEX])
                return FALSE;

            if (ch->pcdata->discipline[AUSPEX] > victim->pcdata->discipline[OBFUSCATE])
                diff -= (ch->pcdata->discipline[AUSPEX] - victim->pcdata->discipline[OBFUSCATE]);

            success = godice(get_attribute(ch, PERCEPTION)+ch->csabilities[CSABIL_ALERTNESS], diff);

            if (ch->pcdata->discipline[AUSPEX] == victim->pcdata->discipline[OBFUSCATE])
                success -= godice(get_attribute(victim, MANIPULATION) + victim->csabilities[CSABIL_SUBTERFUGE], diff);

            if (success < 1)
                return FALSE;
            }
        return TRUE;
}

/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch == victim )
        return TRUE;

    if ( get_trust(ch) < victim->invis_level)
        return FALSE;

        if (is_affected(victim, gsn_trappedingauntlet) && !IS_IMMORTAL(ch))
        	return FALSE;

    if (IS_NPC(victim) && IS_AFFECTED2(victim, AFF2_UNSEEN) && !IS_IMMORTAL(ch))
        return FALSE;

    if (IS_NPC(ch) && IS_AFFECTED2(ch,  AFF2_DETECT_UNSEEN))
        return TRUE;

    if (get_trust(ch) < victim->incog_level && ch->in_room != victim->in_room)
        return FALSE;

    if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)) ||
        (IS_NPC(ch) && IS_IMMORTAL(ch)))
        return TRUE;

    if (IS_AFFECTED(ch, AFF_BLIND) )
        return FALSE;

    if (unseen_check(ch, victim) == FALSE)
        return FALSE;

    if (is_affected(victim,gsn_earthmeld))
        return FALSE;

    if (room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_INFRARED))
        return FALSE;

    if (IS_AFFECTED(victim, AFF_INVISIBLE) && !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;

    if (IS_AFFECTED(victim, AFF_HIDE) &&
        !IS_AFFECTED(ch, AFF_DETECT_HIDDEN) &&
        victim->fighting == NULL)
        return FALSE;

    if (ACT_LOOKING && IS_AFFECTED2(victim, AFF2_UMBRA) &&
        !IS_AFFECTED2(ch, AFF2_UMBRA) &&
        (is_affected(ch, gsn_spiritsight) || is_affected(ch, gsn_gift_pulseoftheinvisible)))
        return TRUE;

    if(IS_AFFECTED2(victim, AFF2_UMBRA) && !IS_AFFECTED2(ch, AFF2_UMBRA))
        return FALSE;

    if(IS_AFFECTED2(ch, AFF2_UMBRA) && !IS_AFFECTED2(victim, AFF2_UMBRA))
        return FALSE;


    return TRUE;
}

// Description macro function.

char *PERS(CHAR_DATA *ch, CHAR_DATA *looker, bool channel)
{

    if (can_see(looker, ch))
    {
        if (
                ((ch->changed > 0 ||
                IS_AFFECTED(ch, AFF_SHIFT) ||
                is_affected(ch, gsn_mask) ||
                is_affected(ch, gsn_shadowform) ||
                is_affected(ch, gsn_vicissitude_horrid) ||
                (!IS_NPC(ch) && ch->pcdata->shiftform >= CRINOS)) &&
                !channel &&
                (!IS_NPC(looker) && !IS_SET(looker->act2, PLR2_NOSHORTDESC))
                ) ||
                IS_NPC(ch) )
            return ch->short_descr;
            else
            return ch->name;
     } else {
            if (IS_IMMORTAL(ch)) {
                if (IS_SET(ch->comm, COMM_WIZINAME) && !IS_NULLSTR(ch->pcdata->wiziname))
                    return ch->pcdata->wiziname;
                else
                    return "An Immortal";
            } else
                return "someone";
            }
}

/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT) )
    return TRUE;

    if ( IS_SET(obj->extra_flags,ITEM_VIS_DEATH))
    return FALSE;

    if ( IS_AFFECTED( ch, AFF_BLIND ) && obj->item_type != ITEM_POTION)
    return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_INVIS)
    &&   !IS_AFFECTED(ch, AFF_DETECT_INVIS) )
        return FALSE;

    if ( IS_OBJ_STAT(obj,ITEM_HIDDEN))
        return FALSE;

    if (ACT_LOOKING && IS_SET(obj->extra_flags, ITEM_UMBRA) &&
        !IS_AFFECTED2(ch, AFF2_UMBRA) &&
        is_affected(ch, gsn_spiritsight) || is_affected(ch, gsn_gift_pulseoftheinvisible))
        return TRUE;

    if ( IS_SET(obj->extra_flags, ITEM_UMBRA) && !IS_AFFECTED2(ch, AFF2_UMBRA))
        return FALSE;

    if ( !IS_SET(obj->extra_flags, ITEM_UMBRA) && IS_AFFECTED2(ch, AFF2_UMBRA))
        return FALSE;

    if ( obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
    return TRUE;

    if ( IS_OBJ_STAT(obj,ITEM_GLOW) || (IS_OBJ_STAT(obj, ITEM_ILLUMINATE)))
    return TRUE;

    if ( room_is_dark( ch->in_room ) && !IS_AFFECTED(ch, AFF_DARK_VISION) )
    return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
    return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
    return TRUE;

    return FALSE;
}

/*Converts numerical hour to am/pm format*/
char *get_time_string(int hours)
{
    static char buf[MSL];
    bool pm = FALSE;
    buf[0] = '\0';

    if (hours > 12)
    {
        pm = TRUE;
        hours -= 12;
    }

    sprintf(buf, "%d%s", hours, pm ? "pm" : "am");
    return buf;
}

/*
 * Return ascii name of an affect location.
 */
char *affect_loc_name( int location )
{
    switch ( location )
    {
    case APPLY_NONE:        return "none";
    case APPLY_STR:     return "strength";
    case APPLY_DEX:     return "dexterity";
    case APPLY_INT:     return "intelligence";
    case APPLY_WIS:     return "wisdom";
    case APPLY_CON:     return "constitution";
    case APPLY_SEX:     return "sex";
    case APPLY_CLASS:       return "class";
    case APPLY_LEVEL:       return "level";
    case APPLY_AGE:     return "age";
    case APPLY_ALIGN:       return "alignment";
    case APPLY_GENERATION:  return "generation";
    case APPLY_CS_STR:      return "cs_strength";
    case APPLY_CS_DEX:      return "cs_dexterity";
    case APPLY_CS_STA:      return "cs_stamina";
    case APPLY_CS_CHA:      return "cs_charisma";
    case APPLY_CS_MAN:      return "cs_manipulation";
    case APPLY_CS_APP:      return "cs_appearance";
    case APPLY_CS_PER:      return "cs_perception";
    case APPLY_CS_INT:      return "cs_intelligence";
    case APPLY_CS_WIT:      return "cs_wits";

    case APPLY_MAX_BLOOD:   return "max_blood";

    case APPLY_MANA:        return "mana";
    case APPLY_HIT:     return "hp";
    case APPLY_MOVE:        return "moves";
    case APPLY_GOLD:        return "gold";
    case APPLY_EXP:     return "experience";
    case APPLY_AC:      return "armor class";
    case APPLY_HITROLL:     return "hit roll";
    case APPLY_DAMROLL:     return "damage roll";
    case APPLY_SAVES:       return "saves";
    case APPLY_SAVING_ROD:  return "save vs rod";
    case APPLY_SAVING_PETRI:    return "save vs petrification";
    case APPLY_SAVING_BREATH:   return "save vs breath";
    case APPLY_SAVING_SPELL:    return "save vs spell";
    case APPLY_SPELL_AFFECT:    return "none";
    }

    bug( "Affect_location_name: unknown location %d.", location );
    return "(unknown)";
}



/*
 * Return ascii name of an affect bit vector.
 */
char *affect_bit_name( int vector )
{
    static char buf[512];

    buf[0] = '\0';
    if ( vector & AFF_BLIND         ) strcat( buf, " blind"         );
    if ( vector & AFF_INVISIBLE     ) strcat( buf, " invisible"     );
    if ( vector & AFF_DETECT_EVIL   ) strcat( buf, " detect_evil"   );
    if ( vector & AFF_DETECT_INVIS  ) strcat( buf, " detect_invis"  );
    if ( vector & AFF_DETECT_MAGIC  ) strcat( buf, " detect_magic"  );
    if ( vector & AFF_DETECT_HIDDEN ) strcat( buf, " detect_hidden" );
    if ( vector & AFF_DETECT_GOOD   ) strcat( buf, " detect_good"   );
    if ( vector & AFF_SANCTUARY     ) strcat( buf, " sanctuary"     );
    if ( vector & AFF_FAERIE_FIRE   ) strcat( buf, " faerie_fire"   );
    if ( vector & AFF_INFRARED      ) strcat( buf, " infrared"      );
    if ( vector & AFF_CURSE         ) strcat( buf, " curse"         );
    if ( vector & AFF_SHIFT         ) strcat( buf, " shift"         );
    if ( vector & AFF_POISON        ) strcat( buf, " poison"        );
    if ( vector & AFF_PROTECT_EVIL  ) strcat( buf, " prot_evil"     );
    if ( vector & AFF_PROTECT_GOOD  ) strcat( buf, " prot_good"     );
    if ( vector & AFF_SNEAK         ) strcat( buf, " sneak"         );
    if ( vector & AFF_HIDE          ) strcat( buf, " hide"          );
    if ( vector & AFF_SLEEP         ) strcat( buf, " sleep"         );
    if ( vector & AFF_CHARM         ) strcat( buf, " charm"         );
    if ( vector & AFF_FLYING        ) strcat( buf, " flying"        );
    if ( vector & AFF_PASS_DOOR     ) strcat( buf, " pass_door"     );
    if ( vector & AFF_HASTE         ) strcat( buf, " haste"         );
    if ( vector & AFF_CALM          ) strcat( buf, " calm"          );
/*    if ( vector & AFF_PLAGUE      ) strcat( buf, " plague"        ); */
    if ( vector & AFF_WEAKEN        ) strcat( buf, " weaken"        );
    if ( vector & AFF_DARK_VISION   ) strcat( buf, " dark_vision"   );
    if ( vector & AFF_BERSERK       ) strcat( buf, " berserk"       );
    if ( vector & AFF_SWIM          ) strcat( buf, " swim"          );
    if ( vector & AFF_REGENERATION  ) strcat( buf, " regeneration"  );
    if ( vector & AFF_SLOW          ) strcat( buf, " slow"          );
    if ( vector & AFF_FANGS         ) strcat( buf, " fangs"         );
    if ( vector & AFF_XP_BOOST      ) strcat( buf, " xp_boost"      );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
 * Return ascii name of an affect bit vector.
 */
char *affect2_bit_name( int vector )
{
    static char buf[512];
    buf[0] = '\0';
    if (vector & AFF2_MIST) strcat(buf, " mist");
    if (vector & AFF2_VEIL) strcat(buf, " veil");
    if (vector & AFF2_VAMP_FRENZY) strcat(buf, " Vampire Frenzy");
    if (vector & AFF2_MAJESTY) strcat(buf, " majesty");
    if (vector & AFF2_QUIETUS_BLOODCURSE) strcat(buf, " bloodcurse");
    if (vector & AFF2_FIRESHIELD) strcat(buf, " fireshield");
    if (vector & AFF2_HOLYSHIELD) strcat(buf, " holyshield");
    if (vector & AFF2_DARKSHIELD) strcat(buf, " darkshield");
    if (vector & AFF2_DOUBLE_EXP) strcat(buf, " double_exp");
    if (vector & AFF2_UMBRA) strcat(buf, " umbra");
    if (vector & AFF2_UNSEEN) strcat(buf, " unseen");
    if (vector & AFF2_DETECT_UNSEEN) strcat(buf, " detect unseen");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_GLOW         ) strcat( buf, " glow"         );
    if ( extra_flags & ITEM_HUM          ) strcat( buf, " hum"          );
    if ( extra_flags & ITEM_DARK         ) strcat( buf, " dark"         );
    if ( extra_flags & ITEM_HIDDEN       ) strcat( buf, " hidden"         );
    if ( extra_flags & ITEM_ILLUMINATE   ) strcat( buf, " illuminate"   );
    if ( extra_flags & ITEM_INVIS        ) strcat( buf, " invis"        );
    if ( extra_flags & ITEM_MAGIC        ) strcat( buf, " magic"        );
    if ( extra_flags & ITEM_NODROP       ) strcat( buf, " nodrop"       );
    if ( extra_flags & ITEM_BLESS        ) strcat( buf, " bless"        );
    if ( extra_flags & ITEM_PAUSE_TIMER  ) strcat( buf, " pause-timer"    );
    if ( extra_flags & ITEM_REFLECTIVE   ) strcat( buf, " reflective"    );
    if ( extra_flags & ITEM_SCALING      ) strcat( buf, " scaling"      );
    if ( extra_flags & ITEM_NOREMOVE     ) strcat( buf, " noremove"     );
    if ( extra_flags & ITEM_INVENTORY    ) strcat( buf, " inventory"    );
    if ( extra_flags & ITEM_NOPURGE  ) strcat( buf, " nopurge"  );
    if ( extra_flags & ITEM_VIS_DEATH    ) strcat( buf, " vis_death"    );
    if ( extra_flags & ITEM_RARE         ) strcat( buf, " rare"     );
    if ( extra_flags & ITEM_ROT_DEATH    ) strcat( buf, " rot_death"    );
    if ( extra_flags & ITEM_NOLOCATE     ) strcat( buf, " no_locate"    );
    if ( extra_flags & ITEM_SELL_EXTRACT ) strcat( buf, " sell_extract" );
    if ( extra_flags & ITEM_BURN_PROOF   ) strcat( buf, " burn_proof"   );
    if ( extra_flags & ITEM_NOUNCURSE    ) strcat( buf, " no_uncurse"   );
    if ( extra_flags & ITEM_IS_STAKE     ) strcat( buf, " is_stake" );
    if ( extra_flags & ITEM_IS_PRESERVE  ) strcat( buf, " is_preserve"  );
    if ( extra_flags & ITEM_IS_ENHANCED  ) strcat( buf, " is_enhanced"  );
    if ( extra_flags & ITEM_BREW_POT     ) strcat( buf, " brew_pot" );
    if ( extra_flags & ITEM_UMBRA        ) strcat( buf, " umbra"    );
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/* return ascii name of an act vector */
char *act_bit_name( int act_flags )
{
    static char buf[512];

    buf[0] = '\0';

    if (IS_SET(act_flags,ACT_IS_NPC))
    {
    strcat(buf," npc");
        if (act_flags & ACT_SENTINEL    ) strcat(buf, " sentinel");
        if (act_flags & ACT_SCAVENGER   ) strcat(buf, " scavenger");
    if (act_flags & ACT_AGGRESSIVE  ) strcat(buf, " aggressive");
    if (act_flags & ACT_STAY_AREA   ) strcat(buf, " stay_area");
    if (act_flags & ACT_WIMPY   ) strcat(buf, " wimpy");
    if (act_flags & ACT_PET     ) strcat(buf, " pet");
    if (act_flags & ACT_MOUNT   ) strcat(buf, " mount");
    if (act_flags & ACT_TRAIN   ) strcat(buf, " train");
    if (act_flags & ACT_PRACTICE    ) strcat(buf, " practice");
    if (act_flags & ACT_UNDEAD  ) strcat(buf, " undead");
    if (act_flags & ACT_CLERIC  ) strcat(buf, " cleric");
    if (act_flags & ACT_MAGE    ) strcat(buf, " mage");
    if (act_flags & ACT_THIEF   ) strcat(buf, " thief");
    if (act_flags & ACT_WARRIOR ) strcat(buf, " warrior");
    if (act_flags & ACT_NOALIGN ) strcat(buf, " no_align");
    if (act_flags & ACT_NOPURGE ) strcat(buf, " no_purge");
    if (act_flags & ACT_IS_HEALER   ) strcat(buf, " healer");
    if (act_flags & ACT_IS_CHANGER  ) strcat(buf, " changer");
    if (act_flags & ACT_GAIN    ) strcat(buf, " skill_train");
    if (act_flags & ACT_UPDATE_ALWAYS) strcat(buf," update_always");
    if (act_flags & ACT_QUESTMOB) strcat(buf, " questmob");
    }
    else
    {
    strcat(buf," player");
    if (act_flags & PLR_AUTOASSIST  ) strcat(buf, " autoassist");
    if (act_flags & PLR_AUTOVAMP    ) strcat(buf, " autovamp");
    if (act_flags & PLR_AUTOEXIT    ) strcat(buf, " autoexit");
    if (act_flags & PLR_AUTOLOOT    ) strcat(buf, " autoloot");
    if (act_flags & PLR_AUTOSAC ) strcat(buf, " autosac");
    if (act_flags & PLR_AUTOGOLD    ) strcat(buf, " autogold");
    if (act_flags & PLR_AUTOSPLIT   ) strcat(buf, " autosplit");
    if (act_flags & PLR_CAMARILLA   ) strcat(buf, " camarilla");
    if (act_flags & PLR_SPEC    ) strcat(buf, " spectate");
    if (act_flags & PLR_HOLYLIGHT   ) strcat(buf, " holy_light");
    if (act_flags & PLR_CANLOOT ) strcat(buf, " loot_corpse");
    if (act_flags & PLR_NOSUMMON    ) strcat(buf, " no_summon");
    if (act_flags & PLR_NOFOLLOW    ) strcat(buf, " no_follow");
    if (act_flags & PLR_FREEZE  ) strcat(buf, " frozen");
    if (act_flags & PLR_COLOUR  ) strcat(buf, " colour");
    if (act_flags & PLR_THIEF   ) strcat(buf, " thief");
    if (act_flags & PLR_KILLER  ) strcat(buf, " killer");
    if (act_flags & PLR_IC      ) strcat(buf, " is_IC");
    if (act_flags & PLR_PUEBLO  ) strcat(buf, " pueblo");
    if (act_flags & PLR_NEWBIEHELP  ) strcat(buf, " newbiehelper");
    if (act_flags & PLR_PLAYTEST)  strcat(buf, " playtest");
    }
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *act2_bit_name( int act_flags )
{
    static char buf[512];

    buf[0] = '\0';

    if (IS_SET(act_flags,ACT2_IS_NPC))
    {
      strcat(buf," npc");
      if (act_flags & ACT2_INFLUENCE      ) strcat(buf, " influence"    );
      if (act_flags & ACT2_BLOOD_DOLL     ) strcat(buf, " blood_doll"   );
      if (act_flags & ACT2_RETAINER       ) strcat(buf, " retainer"     );
      if (act_flags & ACT2_ULTRA_MOB      ) strcat(buf, " ultra_mob"    );
    }
    else
    {
    strcat(buf," player");
    if (act_flags & PLR2_NEWBIE  ) strcat(buf, " newbie");
    if (act_flags & PLR2_PVP  ) strcat(buf, " pvp");
    if (act_flags & PLR2_QUESTOR  ) strcat(buf, " questor");
    if (act_flags & PLR2_LEADER  ) strcat(buf, " leader");
    if (act_flags & PLR2_AUTODOORS) strcat(buf, " autodoors");
    if (act_flags & PLR2_WHOLOCIC) strcat(buf, " wholocic");
    if (act_flags & PLR2_WHOLOCQUIET) strcat(buf, " wholocquiet");
    if (act_flags & PLR2_WHOLOCALWAYS) strcat(buf, " wholocalways");
    if (act_flags & PLR2_WHOSHOWLEVEL) strcat(buf, " whoshowlevel");
    if (act_flags & PLR2_IMMSHOWLOC) strcat(buf, " immshowloc");
    if (act_flags & PLR2_NOCONDITION) strcat(buf, " nocondition");
    if (act_flags & PLR2_NOSHORTDESC) strcat(buf, " noshortdesc");
    if (act_flags & PLR2_BATTLEPROMPT) strcat(buf, " battleprompt");
    if (act_flags & PLR2_DEBUG) strcat(buf, " debug");
    if (act_flags & PLR2_DEBUGMSG) strcat(buf, " debugmsg");
    if (act_flags & PLR2_RP_AVAILABLE) strcat(buf, " rpavailable");
    }
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *comm_bit_name(int comm_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (comm_flags & COMM_QUIET     ) strcat(buf, " quiet");
    if (comm_flags & COMM_DEAF      ) strcat(buf, " deaf");
    if (comm_flags & COMM_NOWIZ     ) strcat(buf, " no_wiz");
    if (comm_flags & COMM_NOIMP     ) strcat(buf, " no_imp");
    if (comm_flags & COMM_NOAUCTION ) strcat(buf, " no_auction");
    if (comm_flags & COMM_NOIC  ) strcat(buf, " no_ic");
    if (comm_flags & COMM_NOQUESTION    ) strcat(buf, " no_question");
    if (comm_flags & COMM_NOMUSIC   ) strcat(buf, " no_music");
    if (comm_flags & COMM_NOOOC ) strcat(buf, " no_ooc");
    if (comm_flags & COMM_COMPACT   ) strcat(buf, " compact");
    if (comm_flags & COMM_BRIEF     ) strcat(buf, " brief");
    if (comm_flags & COMM_PROMPT    ) strcat(buf, " prompt");
    if (comm_flags & COMM_COMBINE   ) strcat(buf, " combine");
    if (comm_flags & COMM_NOEMOTE   ) strcat(buf, " no_emote");
    if (comm_flags & COMM_NOSHOUT   ) strcat(buf, " no_shout");
    if (comm_flags & COMM_NOTELL    ) strcat(buf, " no_tell");
    if (comm_flags & COMM_NOCHANNELS    ) strcat(buf, " no_channels");
    if (comm_flags & COMM_NOANNOUNCE    ) strcat(buf, " no_announce");
    if (comm_flags & COMM_COMBAT_BRIEF  ) strcat(buf, " combat_brief");


    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *imm_bit_name(int imm_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (imm_flags & IMM_SUMMON      ) strcat(buf, " summon");
    if (imm_flags & IMM_CHARM       ) strcat(buf, " charm");
    if (imm_flags & IMM_MAGIC       ) strcat(buf, " magic");
    if (imm_flags & IMM_WEAPON      ) strcat(buf, " weapon");
    if (imm_flags & IMM_BASH        ) strcat(buf, " blunt");
    if (imm_flags & IMM_PIERCE      ) strcat(buf, " piercing");
    if (imm_flags & IMM_SLASH       ) strcat(buf, " slashing");
    if (imm_flags & IMM_FIRE        ) strcat(buf, " fire");
    if (imm_flags & IMM_SUNLIGHT        ) strcat(buf, " sunlight");
    if (imm_flags & IMM_COLD        ) strcat(buf, " cold");
    if (imm_flags & IMM_LIGHTNING   ) strcat(buf, " lightning");
    if (imm_flags & IMM_ACID        ) strcat(buf, " acid");
    if (imm_flags & IMM_POISON      ) strcat(buf, " poison");
    if (imm_flags & IMM_NEGATIVE    ) strcat(buf, " negative");
    if (imm_flags & IMM_HOLY        ) strcat(buf, " holy");
    if (imm_flags & IMM_ENERGY      ) strcat(buf, " energy");
    if (imm_flags & IMM_MENTAL      ) strcat(buf, " mental");
    if (imm_flags & IMM_DISEASE ) strcat(buf, " disease");
    if (imm_flags & IMM_DISARM) strcat(buf, " disarm");
    if (imm_flags & IMM_DROWNING    ) strcat(buf, " drowning");
    if (imm_flags & IMM_LIGHT       ) strcat(buf, " light");
    if (imm_flags & IMM_SOUND       ) strcat(buf, " sound");
    if (imm_flags & VULN_IRON       ) strcat(buf, " iron");
    if (imm_flags & VULN_WOOD       ) strcat(buf, " wood");
    if (imm_flags & VULN_SILVER ) strcat(buf, " silver");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *wear_bit_name(int wear_flags)
{
    static char buf[512];

    buf [0] = '\0';
    if (wear_flags & ITEM_TAKE      ) strcat(buf, " take");
    if (wear_flags & ITEM_WEAR_FINGER   ) strcat(buf, " finger");
    if (wear_flags & ITEM_WEAR_NECK ) strcat(buf, " neck");
    if (wear_flags & ITEM_WEAR_TORSO ) strcat(buf, " torso");
    if (wear_flags & ITEM_WEAR_HEAD ) strcat(buf, " head");
    if (wear_flags & ITEM_WEAR_LEGS ) strcat(buf, " legs");
    if (wear_flags & ITEM_WEAR_FEET ) strcat(buf, " feet");
    if (wear_flags & ITEM_WEAR_HANDS    ) strcat(buf, " hands");
    if (wear_flags & ITEM_WEAR_ARMS ) strcat(buf, " arms");
    if (wear_flags & ITEM_WEAR_SHIELD   ) strcat(buf, " shield");
    if (wear_flags & ITEM_WEAR_BODY    ) strcat(buf, " body");
    if (wear_flags & ITEM_WEAR_WAIST    ) strcat(buf, " waist");
    if (wear_flags & ITEM_WEAR_WRIST    ) strcat(buf, " wrist");
    if (wear_flags & ITEM_WIELD     ) strcat(buf, " wield");
    if (wear_flags & ITEM_HOLD      ) strcat(buf, " hold");
    if (wear_flags & ITEM_NO_SAC    ) strcat(buf, " nosac");
    if (wear_flags & ITEM_WEAR_FLOAT    ) strcat(buf, " float");
    if (wear_flags & ITEM_WEAR_EAR  ) strcat(buf, " ear");
    if (wear_flags & ITEM_WEAR_FACE ) strcat(buf, " face");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *form_bit_name(int form_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (form_flags & FORM_POISON    ) strcat(buf, " poison");
    else if (form_flags & FORM_EDIBLE   ) strcat(buf, " edible");
    if (form_flags & FORM_MAGICAL   ) strcat(buf, " magical");
    if (form_flags & FORM_INSTANT_DECAY ) strcat(buf, " instant_rot");
    if (form_flags & FORM_OTHER     ) strcat(buf, " other");
    if (form_flags & FORM_ANIMAL    ) strcat(buf, " animal");
    if (form_flags & FORM_SENTIENT  ) strcat(buf, " sentient");
    if (form_flags & FORM_UNDEAD    ) strcat(buf, " undead");
    if (form_flags & FORM_CONSTRUCT ) strcat(buf, " construct");
    if (form_flags & FORM_MIST      ) strcat(buf, " mist");
    if (form_flags & FORM_INTANGIBLE    ) strcat(buf, " intangible");
    if (form_flags & FORM_BIPED     ) strcat(buf, " biped");
    if (form_flags & FORM_CENTAUR   ) strcat(buf, " centaur");
    if (form_flags & FORM_INSECT    ) strcat(buf, " insect");
    if (form_flags & FORM_SPIDER    ) strcat(buf, " spider");
    if (form_flags & FORM_CRUSTACEAN    ) strcat(buf, " crustacean");
    if (form_flags & FORM_WORM      ) strcat(buf, " worm");
    if (form_flags & FORM_BLOB      ) strcat(buf, " blob");
    if (form_flags & FORM_MAMMAL    ) strcat(buf, " mammal");
    if (form_flags & FORM_BIRD      ) strcat(buf, " bird");
    if (form_flags & FORM_REPTILE   ) strcat(buf, " reptile");
    if (form_flags & FORM_SNAKE     ) strcat(buf, " snake");
    if (form_flags & FORM_DRAGON    ) strcat(buf, " dragon");
    if (form_flags & FORM_AMPHIBIAN ) strcat(buf, " amphibian");
    if (form_flags & FORM_FISH      ) strcat(buf, " fish");
    if (form_flags & FORM_COLD_BLOOD    ) strcat(buf, " cold_blooded");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *part_bit_name(int part_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (part_flags & PART_HEAD      ) strcat(buf, " head");
    if (part_flags & PART_ARMS      ) strcat(buf, " arms");
    if (part_flags & PART_LEGS      ) strcat(buf, " legs");
    if (part_flags & PART_HEART     ) strcat(buf, " heart");
    if (part_flags & PART_BRAINS    ) strcat(buf, " brains");
    if (part_flags & PART_GUTS      ) strcat(buf, " guts");
    if (part_flags & PART_HANDS     ) strcat(buf, " hands");
    if (part_flags & PART_FEET      ) strcat(buf, " feet");
    if (part_flags & PART_FINGERS   ) strcat(buf, " fingers");
    if (part_flags & PART_EAR       ) strcat(buf, " ears");
    if (part_flags & PART_EYE       ) strcat(buf, " eyes");
    if (part_flags & PART_LONG_TONGUE   ) strcat(buf, " long_tongue");
    if (part_flags & PART_EYESTALKS ) strcat(buf, " eyestalks");
    if (part_flags & PART_TENTACLES ) strcat(buf, " tentacles");
    if (part_flags & PART_FINS      ) strcat(buf, " fins");
    if (part_flags & PART_WINGS     ) strcat(buf, " wings");
    if (part_flags & PART_TAIL      ) strcat(buf, " tail");
    if (part_flags & PART_CLAWS     ) strcat(buf, " claws");
    if (part_flags & PART_FANGS     ) strcat(buf, " fangs");
    if (part_flags & PART_HORNS     ) strcat(buf, " horns");
    if (part_flags & PART_SCALES    ) strcat(buf, " scales");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *weapon_bit_name(int weapon_flags)
{
    static char buf[512];

    buf[0] = '\0';
    if (weapon_flags & WEAPON_FLAMING   ) strcat(buf, " flaming");
    if (weapon_flags & WEAPON_FROST ) strcat(buf, " frost");
    if (weapon_flags & WEAPON_VAMPIRIC  ) strcat(buf, " vampiric");
    if (weapon_flags & WEAPON_SHARP ) strcat(buf, " sharp");
    if (weapon_flags & WEAPON_VORPAL    ) strcat(buf, " vorpal");
    if (weapon_flags & WEAPON_TWO_HANDS ) strcat(buf, " two-handed");
    if (weapon_flags & WEAPON_SHOCKING  ) strcat(buf, " shocking");
    if (weapon_flags & WEAPON_POISON    ) strcat(buf, " poison");
    if (weapon_flags & WEAPON_SLOW      ) strcat(buf, " slow");
    if (weapon_flags & WEAPON_AGG_DAMAGE    ) strcat(buf, " agg_damage");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *light_bit_name( int light_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (light_flags & LIGHT_FLAMES        ) strcat(buf, " flames");
    if (light_flags & LIGHT_MYSTICAL      ) strcat(buf, " mystical");
    if (light_flags & LIGHT_INTENSE       ) strcat(buf, " intense");
    if (light_flags & LIGHT_WEAK          ) strcat(buf, " weak");
    if (light_flags & LIGHT_INTERMITTENT  ) strcat(buf, " intermittent");
    if (light_flags & LIGHT_PULSING       ) strcat(buf, " pulsing");
    if (light_flags & LIGHT_RECHARGE      ) strcat(buf, " recharge");

    return (buf[0] != '\0' ) ? buf+1 : "none";
}

char *cont_bit_name( int cont_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (cont_flags & CONT_CLOSEABLE ) strcat(buf, " closeable");
    if (cont_flags & CONT_PICKPROOF ) strcat(buf, " pickproof");
    if (cont_flags & CONT_CLOSED    ) strcat(buf, " closed");
    if (cont_flags & CONT_LOCKED    ) strcat(buf, " locked");
    if (cont_flags & CONT_PUT_ON    ) strcat(buf, " put_on");
    if (cont_flags & CONT_CAULDRON  ) strcat(buf, " cauldron");

    return (buf[0] != '\0' ) ? buf+1 : "none";
}


char *off_bit_name(int off_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (off_flags & OFF_AREA_ATTACK ) strcat(buf, " area attack");
    if (off_flags & OFF_BACKSTAB    ) strcat(buf, " backstab");
    if (off_flags & OFF_BASH        ) strcat(buf, " bash");
    if (off_flags & OFF_BERSERK     ) strcat(buf, " berserk");
    if (off_flags & OFF_DISARM      ) strcat(buf, " disarm");
    if (off_flags & OFF_DODGE       ) strcat(buf, " dodge");
    if (off_flags & OFF_FADE        ) strcat(buf, " fade");
    if (off_flags & OFF_FAST        ) strcat(buf, " fast");
    if (off_flags & OFF_KICK        ) strcat(buf, " kick");
    if (off_flags & OFF_KICK_DIRT   ) strcat(buf, " kick_dirt");
    if (off_flags & OFF_PARRY       ) strcat(buf, " parry");
    if (off_flags & OFF_RESCUE      ) strcat(buf, " rescue");
    if (off_flags & OFF_TAIL        ) strcat(buf, " tail");
    if (off_flags & OFF_TRIP        ) strcat(buf, " trip");
    if (off_flags & OFF_CRUSH       ) strcat(buf, " crush");
    if (off_flags & ASSIST_ALL      ) strcat(buf, " assist_all");
    if (off_flags & ASSIST_ALIGN    ) strcat(buf, " assist_align");
    if (off_flags & ASSIST_RACE     ) strcat(buf, " assist_race");
    if (off_flags & ASSIST_PLAYERS  ) strcat(buf, " assist_players");
    if (off_flags & ASSIST_GUARD    ) strcat(buf, " assist_guard");
    if (off_flags & ASSIST_VNUM     ) strcat(buf, " assist_vnum");
    if (off_flags & OFF_SECOND_ATTACK   ) strcat(buf, " second_attack");
    if (off_flags & OFF_THIRD_ATTACK    ) strcat(buf, " third_attack");
    if (off_flags & OFF_FOURTH_ATTACK   ) strcat(buf, " fourth_attack");
//    if (off_flags & OFF_ULTRA_MOB   ) strcat(buf, " ultra_mob");
    if (off_flags & OFF_SPHERE_ENTROPY  ) strcat(buf, " sphere_entropy");
    if (off_flags & OFF_SPHERE_FORCES   ) strcat(buf, " sphere_forces");
    if (off_flags & OFF_SPHERE_LIFE ) strcat(buf, " sphere_life");
    if (off_flags & OFF_SPHERE_MIND ) strcat(buf, " sphere_mind");
    if (off_flags & OFF_SPHERE_TIME ) strcat(buf, " sphere_time");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *trap_bit_name(int trap_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (trap_flags & TRAP_BASH      ) strcat(buf, " trap_bash");
    if (trap_flags & TRAP_PIERCE        ) strcat(buf, " trap_pierce");
    if (trap_flags & TRAP_SLASH     ) strcat(buf, " trap_slash");
    if (trap_flags & TRAP_FIRE      ) strcat(buf, " trap_fire");
    if (trap_flags & TRAP_COLD      ) strcat(buf, " trap_cold");
    if (trap_flags & TRAP_LIGHTNING     ) strcat(buf, " trap_lightning");
    if (trap_flags & TRAP_ACID      ) strcat(buf, " trap_acid");
    if (trap_flags & TRAP_POISON        ) strcat(buf, " trap_poison");
    if (trap_flags & TRAP_HOLY      ) strcat(buf, " trap_holy");
    if (trap_flags & TRAP_ENERGY        ) strcat(buf, " trap_energy");
    if (trap_flags & TRAP_MENTAL        ) strcat(buf, " trap_mental");
    if (trap_flags & TRAP_DROWNING      ) strcat(buf, " trap_drowning");
    if (trap_flags & TRAP_LIGHT     ) strcat(buf, " trap_light");
    if (trap_flags & TRAP_HARM      ) strcat(buf, " trap_harm");
    if (trap_flags & TRAP_SOUND     ) strcat(buf, " trap_sound");
    if (trap_flags & TRAP_SUNLIGHT      ) strcat(buf, " trap_sunlight");
    if (trap_flags & TRAP_BLIND     ) strcat(buf, " trap_blind");
    if (trap_flags & TRAP_FAERIE_FIRE   ) strcat(buf, " trap_faerie_fire");
    if (trap_flags & TRAP_CURSE     ) strcat(buf, " trap_curse");
    if (trap_flags & TRAP_SLEEP     ) strcat(buf, " trap_sleep");
    if (trap_flags & TRAP_CALM      ) strcat(buf, " trap_calm");
    if (trap_flags & TRAP_WEAKEN        ) strcat(buf, " trap_weaken");
    if (trap_flags & TRAP_SLOW      ) strcat(buf, " trap_slow");
    if (trap_flags & TRAP_DEATH     ) strcat(buf, " trap_death");


    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *trigger_bit_name(int trigger_flags)
{
    static char buf[512];

    buf[0] = '\0';

    if (trigger_flags & TRIGGER_NORTH       ) strcat(buf, " trigger_north");
    if (trigger_flags & TRIGGER_EAST        ) strcat(buf, " trigger_east");
    if (trigger_flags & TRIGGER_WEST        ) strcat(buf, " trigger_west");
    if (trigger_flags & TRIGGER_SOUTH       ) strcat(buf, " trigger_south");
    if (trigger_flags & TRIGGER_UP          ) strcat(buf, " trigger_up");
    if (trigger_flags & TRIGGER_DOWN        ) strcat(buf, " trigger_down");
    if (trigger_flags & TRIGGER_ALL     ) strcat(buf, " trigger_all");
    if (trigger_flags & TRIGGER_ENTER       ) strcat(buf, " trigger_enter");
    if (trigger_flags & TRIGGER_GET     ) strcat(buf, " trigger_get");
    if (trigger_flags & TRIGGER_PUT     ) strcat(buf, " trigger_put");
    if (trigger_flags & TRIGGER_DROP        ) strcat(buf, " trigger_drop");
    if (trigger_flags & TRIGGER_OPEN        ) strcat(buf, " trigger_open");


    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *focus_bit_name(int focus_flags)
{
    static char buf[512];
    buf[0] = '\0';
    if (focus_flags & FOCUS_SINGLE      ) strcat(buf, " focus_single");
    if (focus_flags & FOCUS_GROUP           ) strcat(buf, " focus_group");
    if (focus_flags & FOCUS_ROOM            ) strcat(buf, " focus_room");
    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
  Returns TRUE if weapon has requested bit
*/
bool *has_weapon_bit(int weapon_flags, char *weapon_bit)
{
    if (weapon_flags & WEAPON_FLAMING    && (!str_cmp(weapon_bit, "flaming"))    ) return TRUE;
    if (weapon_flags & WEAPON_FROST      && (!str_cmp(weapon_bit, "frost"))      ) return TRUE;
    if (weapon_flags & WEAPON_VAMPIRIC   && (!str_cmp(weapon_bit, "vampiric"))   ) return TRUE;
    if (weapon_flags & WEAPON_SHARP      && (!str_cmp(weapon_bit, "sharp"))      ) return TRUE;
    if (weapon_flags & WEAPON_VORPAL     && (!str_cmp(weapon_bit, "vorpol"))     ) return TRUE;
    if (weapon_flags & WEAPON_TWO_HANDS  && (!str_cmp(weapon_bit, "twohands"))   ) return TRUE;
    if (weapon_flags & WEAPON_SHOCKING   && (!str_cmp(weapon_bit, "shocking"))   ) return TRUE;
    if (weapon_flags & WEAPON_POISON     && (!str_cmp(weapon_bit, "poison"))     ) return TRUE;
    if (weapon_flags & WEAPON_SLOW       && (!str_cmp(weapon_bit, "slow"))       ) return TRUE;
    if (weapon_flags & WEAPON_AGG_DAMAGE && (!str_cmp(weapon_bit, "agg_damage")) ) return TRUE;

    return FALSE;
}

char *room_bit_name(int room_flags)
{
    static char buf[512];
    buf[0] = '\0';
    if ( room_flags & ROOM_DARK ) strcat(buf, " dark");
    if ( room_flags & ROOM_NO_MOB ) strcat(buf, " no_mob");
    if ( room_flags & ROOM_INDOORS ) strcat(buf, " indoors");
    if ( room_flags & ROOM_PRIVATE ) strcat(buf, " private");
    if ( room_flags & ROOM_SAFE ) strcat(buf, " safe");
    if ( room_flags & ROOM_SOLITARY ) strcat(buf, " solitary");
    if ( room_flags & ROOM_PET_SHOP ) strcat(buf, " pet_shop");
    if ( room_flags & ROOM_MOUNT_SHOP ) strcat(buf, " mount_shop");
    if ( room_flags & ROOM_NO_RECALL ) strcat(buf, " no_recall");
    if ( room_flags & ROOM_IMP_ONLY ) strcat(buf, " imp_only");
    if ( room_flags & ROOM_GODS_ONLY ) strcat(buf, " gods_only");
    if ( room_flags & ROOM_HEROES_ONLY ) strcat(buf, " heroes_only");
    if ( room_flags & ROOM_NEWBIES_ONLY ) strcat(buf, " newbies_only");
    if ( room_flags & ROOM_LAW ) strcat(buf, " law");
    if ( room_flags & ROOM_NOWHERE ) strcat(buf, " nowhere");
    if ( room_flags & ROOM_FORGE ) strcat(buf, " forge");
    if ( room_flags & ROOM_NO_QUIT ) strcat(buf, " no_quit");
    if ( room_flags & ROOM_UMBRA ) strcat(buf, " umbra");
    if ( room_flags & ROOM_BANK ) strcat(buf, " bank");
    if ( room_flags & ROOM_UNDERWATER ) strcat(buf, " underwater");
    if ( room_flags & ROOM_HUMAN_ONLY ) strcat(buf, " human_only");
    if ( room_flags & ROOM_GHOUL_ONLY ) strcat(buf, " ghoul_only");
    if ( room_flags & ROOM_VAMPIRE_ONLY ) strcat(buf, " vampire_only");
    if ( room_flags & ROOM_GAROU_ONLY ) strcat(buf, " garou_only");
    if ( room_flags & ROOM_MAGE_ONLY ) strcat(buf, " mage_only");
    if ( room_flags & ROOM_NOTELE ) strcat(buf, " no_teleport");

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

/*
  Returns TRUE if Room has requested bit
*/
bool *has_room_bit(int room_flags, char *room_bit)
{
    if (room_flags & ROOM_DARK          && (!str_cmp(room_bit, "dark"))         ) return TRUE;
    if (room_flags & ROOM_NO_MOB        && (!str_cmp(room_bit, "no_mob"))       ) return TRUE;
    if (room_flags & ROOM_INDOORS       && (!str_cmp(room_bit, "indoors"))      ) return TRUE;
    if (room_flags & ROOM_PRIVATE       && (!str_cmp(room_bit, "private"))      ) return TRUE;
    if (room_flags & ROOM_SAFE          && (!str_cmp(room_bit, "safe"))         ) return TRUE;
    if (room_flags & ROOM_SOLITARY      && (!str_cmp(room_bit, "solitary"))     ) return TRUE;
    if (room_flags & ROOM_PET_SHOP      && (!str_cmp(room_bit, "pet_shop"))     ) return TRUE;
    if (room_flags & ROOM_MOUNT_SHOP    && (!str_cmp(room_bit, "mount_shop"))   ) return TRUE;
    if (room_flags & ROOM_NO_RECALL     && (!str_cmp(room_bit, "no_recall"))    ) return TRUE;
    if (room_flags & ROOM_IMP_ONLY      && (!str_cmp(room_bit, "imp_only"))     ) return TRUE;
    if (room_flags & ROOM_GODS_ONLY     && (!str_cmp(room_bit, "gods_only"))    ) return TRUE;
    if (room_flags & ROOM_HEROES_ONLY   && (!str_cmp(room_bit, "heroes_only"))  ) return TRUE;
    if (room_flags & ROOM_NEWBIES_ONLY  && (!str_cmp(room_bit, "newbies_only")) ) return TRUE;
    if (room_flags & ROOM_LAW           && (!str_cmp(room_bit, "law"))          ) return TRUE;
    if (room_flags & ROOM_NOWHERE       && (!str_cmp(room_bit, "nowhere"))      ) return TRUE;
    if (room_flags & ROOM_FORGE         && (!str_cmp(room_bit, "forge"))        ) return TRUE;
    if (room_flags & ROOM_NO_QUIT       && (!str_cmp(room_bit, "no_quit"))      ) return TRUE;
    if (room_flags & ROOM_UMBRA         && (!str_cmp(room_bit, "umbra"))        ) return TRUE;
    if (room_flags & ROOM_BANK          && (!str_cmp(room_bit, "bank"))         ) return TRUE;
    if (room_flags & ROOM_UNDERWATER    && (!str_cmp(room_bit, "underwater"))   ) return TRUE;
    if (room_flags & ROOM_HUMAN_ONLY    && (!str_cmp(room_bit, "human_only"))   ) return TRUE;
    if (room_flags & ROOM_GHOUL_ONLY    && (!str_cmp(room_bit, "ghoul_only"))   ) return TRUE;
    if (room_flags & ROOM_VAMPIRE_ONLY  && (!str_cmp(room_bit, "vampire_only")) ) return TRUE;
    if (room_flags & ROOM_GAROU_ONLY    && (!str_cmp(room_bit, "garou_only"))   ) return TRUE;
    if (room_flags & ROOM_MAGE_ONLY     && (!str_cmp(room_bit, "mage_only"))    ) return TRUE;
    if (room_flags & ROOM_NOTELE        && (!str_cmp(room_bit, "no_teleport"))  ) return TRUE;

    return FALSE;
}

/*
  Returns TRUE if Room matches Clan input
*/
bool *room_clan_only(int clan_room, char *clan_input)
{
    if (clan_room == 1  && (!str_cmp(clan_input, "none"))               ) return TRUE;
    if (clan_room == 2  && (!str_cmp(clan_input, "malkavian"))          ) return TRUE;
    if (clan_room == 3  && (!str_cmp(clan_input, "assamite"))           ) return TRUE;
    if (clan_room == 4  && (!str_cmp(clan_input, "cappacocian"))        ) return TRUE;
    if (clan_room == 5  && (!str_cmp(clan_input, "gangrel"))            ) return TRUE;
    if (clan_room == 6  && (!str_cmp(clan_input, "nosferatu"))          ) return TRUE;
    if (clan_room == 7  && (!str_cmp(clan_input, "toreador"))           ) return TRUE;
    if (clan_room == 8  && (!str_cmp(clan_input, "brujah"))             ) return TRUE;
    if (clan_room == 9  && (!str_cmp(clan_input, "ventrue"))            ) return TRUE;
    if (clan_room == 10 && (!str_cmp(clan_input, "tremere"))            ) return TRUE;
    if (clan_room == 11 && (!str_cmp(clan_input, "lasombra"))           ) return TRUE;
    if (clan_room == 12 && (!str_cmp(clan_input, "tzimisce"))           ) return TRUE;
    if (clan_room == 13 && (!str_cmp(clan_input, "mage"))               ) return TRUE;
    if (clan_room == 14 && (!str_cmp(clan_input, "blackfury"))          ) return TRUE;
    if (clan_room == 15 && (!str_cmp(clan_input, "shadowlord"))         ) return TRUE;
    if (clan_room == 16 && (!str_cmp(clan_input, "fenrir"))             ) return TRUE;
    if (clan_room == 17 && (!str_cmp(clan_input, "silverfang"))         ) return TRUE;
    if (clan_room == 18 && (!str_cmp(clan_input, "wardersofmen"))       ) return TRUE;
    if (clan_room == 19 && (!str_cmp(clan_input, "childrenofgaia"))     ) return TRUE;
    if (clan_room == 20 && (!str_cmp(clan_input, "fianna"))             ) return TRUE;
    if (clan_room == 21 && (!str_cmp(clan_input, "bonegnawer"))         ) return TRUE;
    if (clan_room == 22 && (!str_cmp(clan_input, "akashicbrotherhood")) ) return TRUE;
    if (clan_room == 23 && (!str_cmp(clan_input, "dreamspeakers"))      ) return TRUE;
    if (clan_room == 24 && (!str_cmp(clan_input, "euthanatos"))         ) return TRUE;
    if (clan_room == 25 && (!str_cmp(clan_input, "verbena"))            ) return TRUE;
    if (clan_room == 26 && (!str_cmp(clan_input, "orderofhermes"))      ) return TRUE;
    if (clan_room == 27 && (!str_cmp(clan_input, "ravnos"))             ) return TRUE;
    if (clan_room == 28 && (!str_cmp(clan_input, "redtalon"))           ) return TRUE;
    if (clan_room == 29 && (!str_cmp(clan_input, "silentstrider"))      ) return TRUE;

    return FALSE;
}

char *match_length( char *text, int length )
{
    int l;

    l = strlen(text);

    while(l>length)
    {
        text[l] = '\0';
        l--;
    }

    while(l<length)
    {
        text[l] = ' ';
        l++;
    }
    text[l] = '\0';

    return text;
}
void restore_affects(CHAR_DATA *ch)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int i;
    obj = ch->carrying;

    for(i = 0; i < MAX_WEAR; i++)
    {
        if(obj == NULL)
            break;
        obj_next = obj->next_content;
        obj_affect_to_char(ch,obj,obj->wear_loc);
        obj = obj_next;
    }
    return;
}

void do_wipecs(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg [MAX_STRING_LENGTH];
    int i;

    argument = one_argument(argument,arg);

    if(arg[0] == '\0')
    send_to_char( "Sentax: wipecs <victim>\n\r", ch );

    if ( ( victim = get_char_world( ch, arg ) ) == NULL)
    {
    send_to_char( "Reset CS on whom?\n\r", ch );
    return;
    }

    if(IS_NPC(victim))
    {
    send_to_char( "Not on mobs!\n\r", ch );
    return;
    }

    victim->freebie = 15;
    victim->freebie += victim->remorts * 3;
    for(i = 0; i < 30; i++)
        victim->csabilities[i] = 0;

    for(i = 0; i < 30; i++)
        victim->pcdata->cssec_abil[i] = 0;

    for(i = 0; i < 9; i++)
        victim->csattributes[i] = 1;
    for(i = 0;i<MAX_CSBACK;i++)
        victim->pcdata->csbackgrounds[i] = 0;
    victim->pcdata->csvirtues[CSVIRT_CONSCIENCE] = 1;
    victim->pcdata->csvirtues[CSVIRT_SELF_CONTROL] = 1;
    victim->pcdata->csvirtues[CSVIRT_COURAGE] = 1;
    victim->csmax_willpower = 0;
    victim->cswillpower = 0;
    victim->pcdata->cshumanity = 0;
    victim->pcdata->progress = 0;
    victim->pcdata->stat[PHYSICAL] = 0;
    victim->pcdata->stat[SOCIAL] = 0;
    victim->pcdata->stat[MENTAL] = 0;
    victim->pcdata->stat[SKILLS] = 0;
    victim->pcdata->stat[TALENTS] = 0;
    victim->pcdata->stat[KNOWLEDGES] = 0;
    victim->dpoints = 5;
    send_to_char( "You reset their character sheet stats.\n\r", ch );
    send_to_char( "Your character sheet stats have been reset. Please log out of the mud and then back in.\n\r", victim );

    return;
}

void do_email(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );

    if(IS_NPC(ch)) return;

    if ( arg1[0] == '\0')
    {
        send_to_char("Email customization options are:\n\r\n\rwipe........... Erases all email information\n\r",ch);
        return;
    }


    if(!ch->pcdata->registered)
    {
        send_to_char("Your email address must be registered first. Use the 'register' command.\n\r",ch);
        return;
    }
    if(!str_cmp(arg1,"wipe"))
    {
        send_to_char("Your email information has been wiped. Use the 'register' command to enter your new address.\n\r",ch);
        ch->pcdata->email = NULL;
        ch->pcdata->registered = FALSE;
        ch->pcdata->reg_num = 0;
        return;
    }
    send_to_char("That is not a valid option.\n\r",ch);
    return;
}

void do_register(CHAR_DATA *ch, char *argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if(IS_NPC(ch)) return;
    /*
    sendch("Registration is currently disabled. If you need to recover a lost password,\n\r", ch);
    sendch("speak with an Immortal level107+. We apologize for any problems this may cause.\n\r", ch);
    return;
*/
    if(!ch->pcdata->registered)
    {
        if ( arg1[0] == '\0')
        {
            if(ch->pcdata->email == NULL)
            {
                send_to_char("You need to register a valid email address to fully access the mud.\n\rType register <address> to start the process.\n\r",ch);
                return;
            }
            sprintf(buf,"You need to enter your verification code that was sent to %s.\n\rIf you need your code resent type: register resend\n\r",ch->pcdata->email);
            send_to_char( buf, ch );
            return;
        }
        if(!str_cmp(arg1,"resend"))
        {
            if(ch->pcdata->email == NULL)
            {
                send_to_char("You need to register a valid email address to fully access the mud.\n\rType register <address> to start the process.\n\r",ch);
                return;
            }
            sprintf(buf,"The following is your Haven registration number for %s. Please log into the mud and type register <number>.\n\nRegistration number: %d",ch->name, ch->pcdata->reg_num);
            if(send_email("Haven Registration",ch->pcdata->email,buf) == FALSE) send_to_char("Email bug. Please contact the immortals at once.",ch);
            send_to_char("Registration number resent. Please check your email in a few minutes.\n\rNOTE: If you log out of the mud you will have to start the registration process again.\n\r",ch);
            return;
        }
        if(is_number(arg1) && ch->pcdata->reg_num != 0)
        {
            if(atoi(arg1) == ch->pcdata->reg_num)
            {
                send_to_char("Congratulations! Your email address is now registered! To see your email options type: email\n\r",ch);
                ch->pcdata->registered = TRUE;
            }
            else
                send_to_char("That is not the correct registration number. Please try again or contact an Immortal for help.\n\r",ch);
            return;
        }
        if(ch->pcdata->email == NULL)
        {
            int a = strlen(arg1);
            int b = a;
            for(;a > 0;a--)
            {
                if(arg1[a] == '@')
                    break;
            }
            for(;b > 0;b--)
            {
                if(arg1[b] == '.')
                    break;
            }
            if(!a || !b)
            {
                send_to_char("That is not a valid email address. Please register your FULL email address.\n\r",ch);
            }
            else
            {
                ch->pcdata->reg_num = number_range(100000,999999);
                ch->pcdata->email = str_dup(arg1);
                sprintf(buf,"\nThe following is your Haven registration number for %s. Please log into the mud and type register <number>.\n\nRegistration number: %d.\n",ch->name, ch->pcdata->reg_num);
                if(send_email("Haven Registration",ch->pcdata->email,buf) == FALSE) send_to_char("Email bug. Please contact the immortals at once.",ch);
                wrapln(ch, "Thank you. In a few moments you'll receive an email from admin@havenmud.net containing a six "
                            "digit code you are to enter into the mud to confirm your email address."
                            "To enter this code, type register <number> "
                            "{/NOTE: If you log out of the mud you will have to start the registration process again."
                            "If you do not receive an email within an hour, check your spam filters and then please "
                            "log out and start the process again.");
            }
            return;
        }
        do_function(ch, &do_register, "");
        return;
    }
    send_to_char("You are already registered. If you wish to change email addresses do so with\n\rthe email command by typing: email wipe\n\rNote: You'll have to complete the registration process again from scratch.\n\r",ch);
    return;
}

bool send_email(char *subject, char *address, char *body)
{
    char buf [MAX_STRING_LENGTH];
    sprintf(buf,"sendmail -f admin@havenmud.net -i %s", address);

    FILE *mailer = popen(buf, "w");
    if(!mailer) return FALSE;
    fprintf(mailer, "Subject: %s\n%s",subject, body);
    if(pclose(mailer) == -1) return FALSE;
    return TRUE;
}

/*
char space_buffer( int length )
{
    char spaces[MAX_STRING_LENGTH];
    int i;

    spaces[0] = 'a';
    i = 0;

    while(i != length)
    {
        spaces[i] = 'b';
        i++;
    }

    return spaces;
}
char *space_buffer( int length )
{
    char *buf;
    int i;

    buf = str_dup("");
    for(i = 0; i < length; i++)
        buf[i] = ' ';

    return buf;
}
*/

/* CENTER Function
 * Syntax: center( "argument", width, "fill" )
 *
 * This function will center "argument" in a string <width> characters
 * wide, using the "fill" character for padding on either end of the
 * string when centering. If "argument" is longer than <width>, the
 * "argument" is returned unmodified.
 *
 * Example usage:
 *      printf( "%s", center( "TEST", 10, "*" ) );
 * will return:
 *      ***TEST***
 *
 * If you don't want a fill character, and just want spaces, just set the
 * fill char to NULL, or " ".
 */
char *center( char *argument, int width, char *fill )
{
    char        buf[MSL];
    static char buf2[MSL];
    int         length;
    int         lead_chrs;
    int         trail_chrs;

    if( !fill )
        fill = " ";

    if( !argument )
    {
        sprintf( log_buf, "ERROR! Please note an imm if you see this message.\n\r"
                          "Please include EXACTLY what you did before you got this message.\n\r");
        return log_buf;
    }

    length = strlen_color( argument );

    if( length >= width )
        return argument;

    lead_chrs = (int)((width / 2) - (length / 2) + .5);
    memset( buf2, *fill, lead_chrs );
    buf2[lead_chrs] = '\0';
    strcat( buf2, argument);
    trail_chrs = width - lead_chrs - length;

    if( trail_chrs > 0 )
    {
        memset( buf, *fill, trail_chrs );
        buf[trail_chrs] = '\0';
        strcat(buf2, buf);
    }

    return buf2;
}

char *dots( int num, bool plus )
{
    if(plus)
    {
        switch ( num )
        {
            case 0:     return "OOOOO  ";
            case 1:     return "{W@{xOOOO  ";
            case 2:     return "{W@@{xOOO  ";
            case 3:     return "{W@@@{xOO  ";
            case 4:     return "{W@@@@{xO  ";
            case 5:     return "{W@@@@@{x  ";
            case 6:     return "{W@@@@@{x+1";
            case 7:     return "{W@@@@@{x+2";
            case 8:     return "{W@@@@@{x+3";
            case 9:     return "{W@@@@@{x+4";
            case 10:    return "{W@@@@@{x+5";
        }
    }
    switch ( num )
    {
    case 0:     return "OOOOO";
    case 1:     return "{W@{xOOOO";
    case 2:     return "{W@@{xOOO";
    case 3:     return "{W@@@{xOO";
    case 4:     return "{W@@@@{xO";
    case 5:     return "{W@@@@@{x";
    case 6:     return "{W@@@@@@{xOOOO";
    case 7:     return "{W@@@@@@@{xOOO";
    case 8:     return "{W@@@@@@@@{xOO";
    case 9:     return "{W@@@@@@@@@{xO";
    case 10:    return "{W@@@@@@@@@@{x";
    }

    bug( "Invalid dot rating: %d.", num );
    return "(unknown)";
}


char *dots2( int num, bool space)
{
    if(space)
    {
        switch ( num )
        {
            case 0:     return "O O O O O O O O O O";
            case 1:     return "{W@ {xO O O O O O O O O";
            case 2:     return "{W@ @ {xO O O O O O O O";
            case 3:     return "{W@ @ @ {xO O O O O O O";
            case 4:     return "{W@ @ @ @ {xO O O O O O";
            case 5:     return "{W@ @ @ @ @ {xO O O O O";
            case 6:     return "{W@ @ @ @ @ @ {xO O O O";
            case 7:     return "{W@ @ @ @ @ @ @ {xO O O";
            case 8:     return "{W@ @ @ @ @ @ @ @ {xO O";
            case 9:     return "{W@ @ @ @ @ @ @ @ @ {xO";
            case 10:    return "{W@ @ @ @ @ @ @ @ @ @{x";
        }
    }
    switch ( num )
    {
        case 0:     return "OOOOOOOOOO";
        case 1:     return "{W@{xOOOOOOOOO";
        case 2:     return "{W@@{xOOOOOOOO";
        case 3:     return "{W@@@{xOOOOOOO";
        case 4:     return "{W@@@@{xOOOOOO";
        case 5:     return "{W@@@@@{xOOOOO";
        case 6:     return "{W@@@@@@{xOOOO";
        case 7:     return "{W@@@@@@@{xOOO";
        case 8:     return "{W@@@@@@@@{xOO";
        case 9:     return "{W@@@@@@@@@{xO";
        case 10:    return "{W@@@@@@@@@@{x";
    }

    bug( "Invalid dot rating: %d.", num );
    return "(unknown)";
}

char *dots3( int num, bool space)
{

    if(space)
    {
        switch ( num )
        {
            case 0:     return "_ _ _ _ _ _ _ _ _ _";
            case 1:     return "{WX {x_ _ _ _ _ _ _ _ _";
            case 2:     return "{WX X{x _ _ _ _ _ _ _ _";
            case 3:     return "{WX X X {x_ _ _ _ _ _ _";
            case 4:     return "{WX X X X {x_ _ _ _ _ _";
            case 5:     return "{WX X X X X {x_ _ _ _ _";
            case 6:     return "{WX X X X X X{x _ _ _ _";
            case 7:     return "{WX X X X X X X {x_ _ _";
            case 8:     return "{WX X X X X X X X {x_ _";
            case 9:     return "{WX X X X X X X X X{x _";
            case 10:    return "{WX X X X X X X X X X{x";
        }
    }
    switch ( num )
    {
        case 0:     return "__________";
        case 1:     return "{WX{x_________";
        case 2:     return "{WXX{x________";
        case 3:     return "{WXXX{x_______";
        case 4:     return "{WXXXX{x______";
        case 5:     return "{WXXXXX{x_____";
        case 6:     return "{WXXXXXX{x____";
        case 7:     return "{WXXXXXXX{x___";
        case 8:     return "{WXXXXXXXX{x__";
        case 9:     return "{WXXXXXXXXX{x_";
        case 10:    return "{WXXXXXXXXXXX{x";
    }

    bug( "Invalid dot rating: %d.", num );
    return "(unknown)";
}


bool grudge_check(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if(victim && (ch->grudge == victim))
    return TRUE;
    else return FALSE;
}


bool grudge_update(CHAR_DATA *ch, CHAR_DATA *victim)
{
    if(ch->position == POS_FIGHTING && ch->fighting != NULL)
    {
        ch->grudge = ch->fighting;
        return TRUE;
    }
    else ch->grudge = NULL;
/*
    for(i = 0;i < 5;i++)
    {
        if(ch->grudge[i] == victim)
        {
            if(victim->position != POS_FIGHTING)
                ch->grudge[i] = NULL;
            return FALSE;
        }
        if(ch->grudge[i] == NULL)
        {
            ch->grudge[i] = victim;
            return TRUE;
        }
    }
*/
return FALSE;
}

void obj_affect_to_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    AFFECT_DATA *paf;
    int i;

    for (i = 0; i < 4; i++)
        ch->armor[i]        -= apply_ac( obj, iWear,i );
    obj->wear_loc    = iWear;

    if (!obj->enchanted)
    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
        if ( paf->location != APPLY_SPELL_AFFECT )
            affect_modify( ch, paf, TRUE );
    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    if ( paf->location == APPLY_SPELL_AFFECT )
            affect_to_char ( ch, paf );
    else
        affect_modify( ch, paf, TRUE );


    return;
}
bool is_clan_disc(CHAR_DATA *ch,int disc)
{
    int i;
    for(i = 0; i < 3; i++)
    {
        if(clandisc_table[ch->clan].clan_disc[i] == disc)
        return disc;
    }
    return FALSE;
}

int get_gauntlet(CHAR_DATA *ch)
{
    int gauntlet=0;

        if(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
        ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
        ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
        ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
        ||   IS_SET(ch->act,PLR_ARENA))
            gauntlet += 10;
        else if(ch->in_room->sector_type == SECT_INSIDE || ch->in_room->sector_type == SECT_CITY)
            gauntlet += 5;
        else if(ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM ||
        ch->in_room->sector_type == SECT_AIR || ch->in_room->sector_type == SECT_DESERT ||
        ch->in_room->sector_type == SECT_WATER_DROWN || ch->in_room->sector_type == SECT_HOT ||
        ch->in_room->sector_type == SECT_COLD)
            gauntlet += 4;
        else if(ch->in_room->sector_type == SECT_FIELD || ch->in_room->sector_type == SECT_HILLS ||
        ch->in_room->sector_type == SECT_MOUNTAIN)
            gauntlet += 3;
        else if(ch->in_room->sector_type == SECT_FOREST)
            gauntlet += 2;
        else gauntlet += 1;

    return gauntlet;
}

int quest_status(CHAR_DATA *ch, int quest)
{
    int i;

    if(IS_NPC(ch)) return ch->quest;

    for(i = 0;i < MAX_QUEST;i++)
        if(ch->pcdata->quest_id[i] == quest) return ch->pcdata->quest_step[i];

    return 0;
}

bool add_quest(CHAR_DATA *ch, int quest)
{
    int i;
    if(quest_status(ch,quest) != 0)
    {
        bug("add_quest: unable to assign quest", 0 );
        return FALSE;
    }

    for(i = 0;i < MAX_QUEST;i++)
    {
        if(ch->pcdata->quest_id[i] == 0)
        {
            ch->pcdata->quest_id[i] = quest;
            ch->pcdata->quest_step[i] = 1;
            ch->pcdata->quest_string[i] = str_dup("You have just started this quest.");
            ch->pcdata->quest_name[i] = str_dup("A quest");
            return TRUE;
        }
    }
    return FALSE;
}

/* Returns a string with returns inserted - Fallon */
char *wrapstr( CHAR_DATA *ch, const char *str )
{
    static char strwrap[MAX_STRING_LENGTH];
    int i;
    int count = 0;
    for ( i = 0; i < strlen(str); i++ )
    {
        count++;
        if ( count > 66 && str[i] == ' ' )
        {
            strwrap[i] = '\n';
            strwrap[i+1] = '\r';
            count = 0;
        }
        else
        {
            strwrap[i] = str[i];
        }
    }
    strwrap[i] = '\0';
    return strwrap;
}
/*
char *wordwrap( int lines, int length, const char *argument )
{
    static char strwrap[50][MAX_STRING_LENGTH];
    char org[2] = "  ";
    int i,b,n,r,count, word;
    count = n = b = 0;

    for(i = 0;i < 50;i++)
        strncpy(strwrap[i],org, MAX_STRING_LENGTH);

    for ( i = 0; i < strlen(argument); i++ )
    {
        count++;
        word = 0;
        if(argument[i] == ' ')
            for(r = i+1;argument[r] != ' ';r++)
                word++;

        if(count+word > length && argument[i] == ' ')
        {
            strwrap[n][b] = '\0';
            count = b = 0;
            n++;
        }
        else
        {
            strwrap[n][b] = argument[i];
            b++;
        }
    }
    if(lines > n)
    {
        n++;
        for(i = 0;i < length;i++)
            strwrap[n][i] = ' ';

        strwrap[n][i+1] = '\0';
        return strwrap[n];
    }
    else return strwrap[lines];
}
*/

#define C_BACK_WHITE    "[1;47m"
#define C_BACK_WHITE2   "[47m"
#define C_BACK_WHITE3   "[0;47m"
#define C_U_LINE    "[4m"
#define C_RESET "[4m"
#define C_BACK_BLACK    "[40m"

char *wordwrap(int desired_line, int length, char *argument, int alignment)
{
    char *str2;
    char string[MAX_STRING_LENGTH] = "\0", left[MAX_STRING_LENGTH] = "\0", right[MAX_STRING_LENGTH] = "\0";
    char buf[MAX_STRING_LENGTH];
    char word[200][MAX_INPUT_LENGTH];
    char sentence[200][MAX_INPUT_LENGTH];
    int i, words = 0, line = 0, left_pad, right_pad;

    for(i = 0;i < 200;i++)
        strcpy(sentence[i],"\0");

    str2 = string;

    for(i = 0;;i++)
    {
        argument = one_argument2(argument,word[i]);
        if(word[i][0] == '\0')  break;
        words++;
    }

    for(i = 0;i < words;i++)
    {
        if(strlen_color(str2)+strlen_color(word[i])+1 > length)
        {
            line++;
            strcpy(str2,"\0");
        }

        strcat(str2,word[i]);

        strcpy(sentence[line],str2);

        if(word[i+1]) strcat(str2," ");

    }
    strcpy(str2,sentence[desired_line]);

    if((i = length - strlen_color(str2)) < length)
    {
        switch(alignment)
        {
            case ALIGN_LEFT:
                            left_pad = 0;
                            right_pad = i;
                            break;
            case ALIGN_RIGHT:
                            left_pad = i;
                            right_pad = 0;
                            break;
            default:
                            left_pad = i/2;
                            right_pad = i/2;
                            if(left_pad+right_pad > i)
                                right_pad--;
                            break;
        }
        while(left_pad)
        {
            strcat(left," ");
            left_pad--;
        }
        while(right_pad)
        {
            strcat(right," ");
            right_pad--;
        }
        strcpy(buf,str2);
        strcpy(str2,left);
        strcat(str2,buf);
        strcat(str2,right);
    }

    while(strlen_color(str2) < length)
        strcat(str2," ");

    return str2;
}


/*
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int i;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if(!is_number(arg) || !is_number(arg2))
    {
        send_to_char("Syntax: Lines, length, string\n\r",ch);
        return;
    }
    for(i = 0;i < atoi(arg);i++)
    {
        send_to_char(format_str_len(wordwrap(i,atoi(arg2),argument), atoi(arg2),ALIGN_LEFT),ch);
        send_to_char("\n\r",ch);
    }
    return;

}
*/
char* wordwrap_str( const char* src, short num_columns, const char* nstr )
{
  /* to avoid buffer overruns use a special buffer */
  char str_buf[MSL+1];
  char *str = str_buf;

  /* return buffer */
  static char buf[MSL+1];
  char* buf_ptr = buf;

  short counter = 0; /* keep track of when to add new string */
  short nstr_len = strlen(nstr);

  /* check input */
  if ( !src || !nstr )
   return NULL;

  if ( num_columns <= 0 )
   num_columns = 75;

  /* copy up to max length of MSL */
  strncpy( str_buf, src, MSL );

  /* loop while we have string data */
  while ( *str != '\0' )
  {
    *buf_ptr = *str; /* copy character */

    /* insert new string? */
    if ( ++counter == num_columns )
    {
      short i;

      /* append new string to the end */
      strcat( buf, nstr );

      /* correct buf_ptr position */
      for ( i = 0; i < nstr_len; i++ )
       buf_ptr++;

      /* skip spaces after newlines to avoid bad looking output */

      if ( nstr[nstr_len-1] == '\n' )
      {
        while (isspace(*(++str))); /* while *(++str) is a space, loop again */
        str--; /* correct pointer */
      }

      counter = 0; /* reset counter */
    }

    buf_ptr++;
    str++;
  }

  return buf;
}

/* It should be noted that both strlen_color and strip_color make
 * exceptions for the special color code '{{', which is meant as
 * as escape code for placing literal opening braces into color
 * coded strings. So, in this sense, the two function deal with
 * the intended "visible string", rather than a mere "raw string
 * with no color codes"
 */
int strlen_color(const char *string)
{
    int length;

    if (!string || string[0] == '\0')
        return 0;

    length = 0;

    while (*string != '\0')             /* Loop through each character in the string. */
    {
        if (*string != '{')                 /* If the character's not the color code, count it */
        {
            string++;
            length++;
            continue;
        }
        else                                                /* The color code char's been found. */
        {
            if (*(++string) == '{')     /* If the color's a visible one, count it */
                length++;
        }

        string++;
    }

    return length;
}

char *strip_color(const char *string)
{
    char buffer[MAX_STRING_LENGTH];
    char *plain_string;;
    int pos = 0;
    char c;

    memset(buffer, '\0', MAX_STRING_LENGTH);

    plain_string = buffer;
    while (*string != '\0' && pos != (MAX_STRING_LENGTH - 1))
    {
        c = *string++;
        if (c == '{')
        {
            c = *string++;
            if (c == '{')
                buffer[pos++] = c;
            continue;
        }
        buffer[pos++] = c;
    }
    buffer[pos] = '\0';
    return (plain_string);
}

/* Lookup function to find the exact modifier in gsn at X location - Ugha*/
int get_aff_mod(CHAR_DATA *ch, int sn, int loc)
{
    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    for ( paf = ch->affected; paf != NULL; paf = paf_next )
    {
    paf_next = paf->next;
    if ( paf->type == sn  && paf->location == loc)
        return paf->modifier;
    }
    return 0;
}


char *FORMATF(const char *formatbuf, ...)
{
    static int i;
    static char buf[10][MSL*3];
    i++;
    i%=10;

    va_list args;
    va_start(args, formatbuf);
    vsnprintf(buf[i], MSL*3, formatbuf, args);
    va_end(args);

    return buf[i];
}

int c_str_len(const char *s)
{
    int len = 0; // visible string length (without colour codes)

    for( ; *s; s++ )
    {
        if( *s == '{' ) // skip the colour code
        {
            s++;        // move to the colour character
            switch(*s)
            {
                default:
                    break;

                case '-': // ~
                case '{': // `
                    len++;
                    break;

                case '/': // newline returns a -1
                    return -1;
                    break;
            }
            continue;
        }
        len++;  // normal character count that length
    }
    return len;
}
/**************************************************************************/
// prepend \r\n to what is to be sent
void println(const char *buf, CHAR_DATA *ch)
{
    sendch(buf, ch);
    sendch("\r\n", ch);
}
/**************************************************************************/
bool canwear_objaffect( CHAR_DATA *ch, OBJ_DATA *obj )
{
    int mod,i;
    AFFECT_DATA *paf;

    for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
    {
        mod = paf->modifier;

        if (paf->location == APPLY_MANA && ch->max_mana + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_HIT && ch->max_hit + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_MOVE && ch->max_move + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_MAX_BLOOD && ch->max_pblood + mod < 10)
            return FALSE;
        }

    for ( paf = obj->affected; paf != NULL; paf = paf->next )
    {
        mod = paf->modifier;

        if (paf->location == APPLY_MANA && ch->max_mana + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_HIT && ch->max_hit + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_MOVE && ch->max_move + mod < 10)
            return FALSE;
        else if (paf->location == APPLY_MAX_BLOOD && ch->max_pblood + mod < 10)
            return FALSE;
        }

    return TRUE;
    }

/*
pass_gauntlet:
Takes care of moving a Char and all of their contents to the Umbra one way or
the other.
*/
bool pass_gauntlet( CHAR_DATA *ch, bool show)
{
    OBJ_DATA *pObj;
    OBJ_DATA *pObjNext;
    OBJ_DATA *pContents;
    OBJ_DATA *pContentsNext;

    if (IS_AFFECTED2(ch, AFF2_UMBRA))
    {
        if (IS_SET(ch->in_room->room_flags, ROOM_UMBRA))
        {
            if (show)
                sendch("You cannot return to Reality, this place does not exist there!\n\r", ch);
            return FALSE;
        }
        if (show)
        {
            act( "You feel more solid as you pass into Physical Reality.",  ch, NULL, NULL, TO_CHAR    );
            act( "$n seems more solid and almost too real before disappearing completely.",  ch, NULL, NULL, TO_NOTVICT );
        }
        REMOVE_BIT(ch->affected2_by, AFF2_UMBRA);

        for ( pObj = ch->carrying; pObj != NULL; pObj = pObjNext )
        {
            pObjNext = pObj->next_content;
            REMOVE_BIT(pObj->extra_flags,ITEM_UMBRA);

            if (pObj->item_type == ITEM_CONTAINER ||
                pObj->item_type == ITEM_CORPSE_NPC ||
                pObj->item_type == ITEM_CORPSE_PC)
            {
                for ( pContents = pObj->contains; pContents != NULL; pContents = pContentsNext )
                {
                    pContentsNext = pContents->next_content;
                    REMOVE_BIT(pContents->extra_flags, ITEM_UMBRA);
                }
            }
        }
        if (show)
            act( "$n passes into this Reality as if stepping through a curtain.",  ch, NULL, NULL, TO_NOTVICT );
        return TRUE;
    } else {
        if (show) {
            act( "Reality passes through you as you step sideways into the Umbra.",  ch, NULL, NULL, TO_CHAR    );
            act( "$n seems ephemeral and hazy before simply blinking out of existence.",  ch, NULL, NULL, TO_NOTVICT );
        }
        SET_BIT(ch->affected2_by, AFF2_UMBRA);

        for ( pObj = ch->carrying; pObj != NULL; pObj = pObjNext )
        {
            pObjNext = pObj->next_content;
            SET_BIT(pObj->extra_flags,ITEM_UMBRA);

            if (pObj->item_type == ITEM_CONTAINER ||
                pObj->item_type == ITEM_CORPSE_NPC ||
                pObj->item_type == ITEM_CORPSE_PC)
            {
                for ( pContents = pObj->contains; pContents != NULL; pContents = pContentsNext )
                {
                    pContentsNext = pContents->next_content;
                    SET_BIT(pContents->extra_flags, ITEM_UMBRA);
                }
            }
        }
        if (show)
            act( "$n passes into the Umbra as if stepping through a curtain.",  ch, NULL, NULL, TO_NOTVICT );
        return TRUE;
    }
}
void update_moston ( void )
{
    DESCRIPTOR_DATA *d;
    int current;
    extern int most_players;
    extern int max_players;

    current = 0;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected != CON_PLAYING  )
            continue;
            current++;
    }

    if (current > most_players) most_players = current;
    if (current > max_players) max_players = current;
    save_config();

}
