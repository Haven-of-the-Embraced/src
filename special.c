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
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(   spec_breath_any         );
DECLARE_SPEC_FUN(   spec_breath_acid        );
DECLARE_SPEC_FUN(   spec_breath_fire        );
DECLARE_SPEC_FUN(   spec_breath_frost       );
DECLARE_SPEC_FUN(   spec_breath_gas         );
DECLARE_SPEC_FUN(   spec_breath_lightning   );
DECLARE_SPEC_FUN(   spec_cast_adept         );
DECLARE_SPEC_FUN(   spec_cast_cleric        );
DECLARE_SPEC_FUN(   spec_cast_judge         );
DECLARE_SPEC_FUN(   spec_cast_mage          );
DECLARE_SPEC_FUN(   spec_cast_undead        );
DECLARE_SPEC_FUN(   spec_cast_lightning     );
DECLARE_SPEC_FUN(   spec_cast_acid          );
DECLARE_SPEC_FUN(   spec_cast_cold          );
DECLARE_SPEC_FUN(   spec_cast_fire          );
DECLARE_SPEC_FUN(   spec_executioner        );
DECLARE_SPEC_FUN(   spec_fido               );
DECLARE_SPEC_FUN(   spec_guard              );
DECLARE_SPEC_FUN(   spec_janitor            );
DECLARE_SPEC_FUN(   spec_mayor              );
DECLARE_SPEC_FUN(   spec_poison             );
DECLARE_SPEC_FUN(   spec_thief              );
DECLARE_SPEC_FUN(   spec_nasty              );
DECLARE_SPEC_FUN(   spec_troll_member       );
DECLARE_SPEC_FUN(   spec_ogre_member        );
DECLARE_SPEC_FUN(   spec_patrolman          );
DECLARE_SPEC_FUN(   spec_stake              );
DECLARE_SPEC_FUN(   spec_bo_dog             );
DECLARE_SPEC_FUN(   spec_lag                );
/* DECLARE_SPEC_FUN(   spec_assamite           );
DECLARE_SPEC_FUN(   spec_baali              ); */
DECLARE_SPEC_FUN(   spec_brujah             );
/* DECLARE_SPEC_FUN(   spec_cappadocian        );
DECLARE_SPEC_FUN(   spec_followersofset     );
DECLARE_SPEC_FUN(   spec_gangrel            );
DECLARE_SPEC_FUN(   spec_gargoyle           );*/
DECLARE_SPEC_FUN(   spec_lasombra           );
/* DECLARE_SPEC_FUN(   spec_malkavian          );
DECLARE_SPEC_FUN(   spec_nosferatu          );
DECLARE_SPEC_FUN(   spec_ravnos             );
DECLARE_SPEC_FUN(   spec_salubri            );
DECLARE_SPEC_FUN(   spec_toreador           );
DECLARE_SPEC_FUN(   spec_tremere            );
DECLARE_SPEC_FUN(   spec_tzimisce           );
DECLARE_SPEC_FUN(   spec_ventrue            );
DECLARE_SPEC_FUN(   spec_animalism          );
DECLARE_SPEC_FUN(   spec_auspex             ); */
DECLARE_SPEC_FUN(   spec_celerity           );
/* DECLARE_SPEC_FUN(   spec_chimerstry         );
DECLARE_SPEC_FUN(   spec_daimoinon          );
DECLARE_SPEC_FUN(   spec_dementation        ); */
DECLARE_SPEC_FUN(   spec_dominate           );
/* DECLARE_SPEC_FUN(   spec_flight             );
DECLARE_SPEC_FUN(   spec_fortitude          );
DECLARE_SPEC_FUN(   spec_mortis             );
DECLARE_SPEC_FUN(   spec_obeah              );
DECLARE_SPEC_FUN(   spec_obfuscate          ); */
DECLARE_SPEC_FUN(   spec_obtenebration      );
DECLARE_SPEC_FUN(   spec_potence            );
DECLARE_SPEC_FUN(   spec_presence           );
/* DECLARE_SPEC_FUN(   spec_protean            );
DECLARE_SPEC_FUN(   spec_quietus            ); */
DECLARE_SPEC_FUN(   spec_serpentis          );
/* DECLARE_SPEC_FUN(   spec_thaum_regovitae    );
DECLARE_SPEC_FUN(   spec_valeran            );
DECLARE_SPEC_FUN(   spec_vicissitude        ); */
DECLARE_SPEC_FUN(   spec_command            );
DECLARE_SPEC_FUN(   spec_mesmerize          );
DECLARE_SPEC_FUN(   spec_forgetfulmind      );
DECLARE_SPEC_FUN(   spec_shadowplay         );
DECLARE_SPEC_FUN(   spec_nocturne           );
DECLARE_SPEC_FUN(   spec_armsoftheabyss     );
DECLARE_SPEC_FUN(   spec_awe                );
DECLARE_SPEC_FUN(   spec_dreadgaze          );
DECLARE_SPEC_FUN(   spec_eyesoftheserpent   );
DECLARE_SPEC_FUN(   spec_evil_eye           );
DECLARE_SPEC_FUN(   spec_questmaster        );
DECLARE_SPEC_FUN(   spec_jarjar             );

/* the function table */
const   struct  spec_type    spec_table[] =
{
// Breath Weapons
    {   "spec_breath_any",        spec_breath_any       },
    {   "spec_breath_acid",       spec_breath_acid      },
    {   "spec_breath_fire",       spec_breath_fire      },
    {   "spec_breath_frost",      spec_breath_frost     },
    {   "spec_breath_gas",        spec_breath_gas       },
    {   "spec_breath_lightning",  spec_breath_lightning },
// Casting Groups
    {   "spec_cast_adept",        spec_cast_adept       },
    {   "spec_cast_cleric",       spec_cast_cleric      },
    {   "spec_cast_judge",        spec_cast_judge       },
    {   "spec_cast_mage",         spec_cast_mage        },
    {   "spec_cast_undead",       spec_cast_undead      },
    {   "spec_cast_lightning",    spec_cast_lightning   },
    {   "spec_cast_acid",         spec_cast_acid        },
    {   "spec_cast_cold",         spec_cast_cold        },
    {   "spec_cast_fire",         spec_cast_fire        },
// Specialty Progs
    {   "spec_executioner",       spec_executioner      },
    {   "spec_fido",              spec_fido             },
    {   "spec_guard",             spec_guard            },
    {   "spec_janitor",           spec_janitor          },
    {   "spec_mayor",             spec_mayor            },
    {   "spec_poison",            spec_poison           },
    {   "spec_thief",             spec_thief            },
    {   "spec_nasty",             spec_nasty            },
    {   "spec_troll_member",      spec_troll_member     },
    {   "spec_ogre_member",       spec_ogre_member      },
    {   "spec_patrolman",         spec_patrolman        },
    {   "spec_stake",             spec_stake            },
    {   "spec_bo_dog",            spec_bo_dog           },
    {   "spec_lag",               spec_lag              },
// Clans
/*    {   "spec_assamite",          spec_assamite         },
    {   "spec_baali",             spec_baali            }, */
    {   "spec_brujah",            spec_brujah           },
/*    {   "spec_cappadocian",       spec_cappadocian      },
    {   "spec_followersofset",    spec_followersofset   },
    {   "spec_gangrel",           spec_gangrel          },
    {   "spec_gargoyle",          spec_gargoyle         },*/
    {   "spec_lasombra",          spec_lasombra         },
/*    {   "spec_malkavian",         spec_malkavian        },
    {   "spec_nosferatu",         spec_nosferatu        },
    {   "spec_ravnos",            spec_ravnos           },
    {   "spec_salubri",           spec_salubri          },
    {   "spec_toreador",          spec_toreador         },
    {   "spec_tremere",           spec_tremere          },
    {   "spec_tzimisce",          spec_tzimisce         },
    {   "spec_ventrue",           spec_ventrue          },*/
// Disciplines
/*    {   "spec_animalism",         spec_animalism        },
    {   "spec_auspex",            spec_auspex           },  */
    {   "spec_celerity",          spec_celerity         },
/*    {   "spec_chimerstry",        spec_chimerstry       },
    {   "spec_daimoinon",         spec_daimoinon        },
    {   "spec_dementation",       spec_dementation      }, */
    {   "spec_dominate",          spec_dominate         },
/*    {   "spec_flight",            spec_flight           },
    {   "spec_fortitude",         spec_fortitude        },
    {   "spec_mortis",            spec_mortis           },
    {   "spec_obeah",             spec_obeah            },
    {   "spec_obfuscate",         spec_obfuscate        },*/
    {   "spec_obtenebration",     spec_obtenebration    },
    {   "spec_potence",           spec_potence          },
    {   "spec_presence",          spec_presence          },
/*    {   "spec_protean",           spec_protean          },
    {   "spec_quietus",           spec_quietus          },*/
    {   "spec_serpentis",         spec_serpentis        },
/*    {   "spec_thaum_regovitae",   spec_thaum_regovitae  },
    {   "spec_valeran",           spec_valeran          },
    {   "spec_vicissitude",       spec_vicissitude      },
*/
// Individual Discipline Powers
    {   "spec_command",           spec_command          },
    {   "spec_mesmerize",         spec_mesmerize        },
    {   "spec_forgetfulmind",     spec_forgetfulmind    },
    {   "spec_shadowplay",        spec_shadowplay       },
    {   "spec_nocturne",          spec_nocturne         },
    {   "spec_armsoftheabyss",    spec_armsoftheabyss   },
    {   "spec_awe",               spec_awe              },
    {   "spec_dreadgaze",         spec_dreadgaze        },
    {   "spec_eyesoftheserpent",  spec_eyesoftheserpent },
// Numina/Romani
    {   "spec_evil_eye",          spec_evil_eye         },
// Miscellaneous
    {   "spec_questmaster",       spec_questmaster      },
    {   "spec_jarjar",            spec_jarjar           },
    {   NULL,               NULL            }
};

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN *spec_lookup( const char *name )
{
   int i;

   for ( i = 0; spec_table[i].name != NULL; i++)
   {
        if (LOWER(name[0]) == LOWER(spec_table[i].name[0])
        &&  !str_prefix( name,spec_table[i].name))
            return spec_table[i].function;
   }

    return 0;
}

char *spec_name( SPEC_FUN *function)
{
    int i;

    for (i = 0; spec_table[i].function != NULL; i++)
    {
    if (function == spec_table[i].function)
        return spec_table[i].name;
    }

    return NULL;
}

/* Prototype for spec_function

bool spec_( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    return FALSE;
}
*/

bool spec_questmaster(CHAR_DATA *ch)
{
    if (IS_NPC(ch))
    {
        return TRUE;
    } else {
        return FALSE;
    }
}

bool spec_troll_member( CHAR_DATA *ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL || ch->stopped > 0)
    return FALSE;

    /* find an ogre to beat up */
    for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
    {
    if (!IS_NPC(vch) || ch == vch)
        continue;

    if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
        return FALSE;

    if (vch->pIndexData->group == GROUP_VNUM_OGRES
    &&  ch->level > vch->level - 2 && !is_safe(ch,vch))
    {
        if (number_range(0,count) == 0)
        victim = vch;

        count++;
    }
    }

    if (victim == NULL)
    return FALSE;

    /* say something, then raise hell */
    switch (number_range(0,6))
    {
    default:  message = NULL;   break;
    case 0: message = "$n yells 'I've been looking for you, punk!'";
        break;
    case 1: message = "With a scream of rage, $n attacks $N.";
        break;
    case 2: message =
        "$n says 'What's slimy Ogre trash like you doing around here?'";
        break;
    case 3: message = "$n cracks his knuckles and says 'Do ya feel lucky?'";
        break;
    case 4: message = "$n says 'There's no cops to save you this time!'";
        break;
    case 5: message = "$n says 'Time to join your brother, spud.'";
        break;
    case 6: message = "$n says 'Let's rock.'";
        break;
    }

    if (message != NULL)
        act(message,ch,NULL,victim,TO_ALL);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_ogre_member( CHAR_DATA *ch)
{
    CHAR_DATA *vch, *victim = NULL;
    int count = 0;
    char *message;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL || ch->stopped > 0)
        return FALSE;

    /* find an troll to beat up */
    for (vch = ch->in_room->people;  vch != NULL;  vch = vch->next_in_room)
    {
        if (!IS_NPC(vch) || ch == vch)
            continue;

        if (vch->pIndexData->vnum == MOB_VNUM_PATROLMAN)
            return FALSE;

        if (vch->pIndexData->group == GROUP_VNUM_TROLLS
        &&  ch->level > vch->level - 2 && !is_safe(ch,vch))
        {
            if (number_range(0,count) == 0)
                victim = vch;

            count++;
        }
    }

    if (victim == NULL)
        return FALSE;

    /* say something, then raise hell */
    switch (number_range(0,6))
    {
    default: message = NULL;    break;
        case 0: message = "$n yells 'I've been looking for you, punk!'";
                break;
        case 1: message = "With a scream of rage, $n attacks $N.'";
                break;
        case 2: message =
                "$n says 'What's Troll filth like you doing around here?'";
                break;
        case 3: message = "$n cracks his knuckles and says 'Do ya feel lucky?'";
                break;
        case 4: message = "$n says 'There's no cops to save you this time!'";
                break;
        case 5: message = "$n says 'Time to join your brother, spud.'";
                break;
        case 6: message = "$n says 'Let's rock.'";
                break;
    }

    if (message != NULL)
        act(message,ch,NULL,victim,TO_ALL);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
}

bool spec_patrolman(CHAR_DATA *ch)
{
    CHAR_DATA *vch,*victim = NULL;
    OBJ_DATA *obj;
    char *message;
    int count = 0;

    if (!IS_AWAKE(ch) || IS_AFFECTED(ch,AFF_CALM) || ch->in_room == NULL
    ||  IS_AFFECTED(ch,AFF_CHARM) || ch->fighting != NULL || ch->stopped > 0)
        return FALSE;

    /* look for a fight in the room */
    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
    if (vch == ch)
        continue;

    if (vch->fighting != NULL)  /* break it up! */
    {
        if (number_range(0,count) == 0)
            victim = (vch->level > vch->fighting->level)
            ? vch : vch->fighting;
        count++;
    }
    }

    if (victim == NULL || (IS_NPC(victim) && victim->spec_fun == ch->spec_fun))
    return FALSE;

    if (((obj = get_eq_char(ch,WEAR_NECK_1)) != NULL
    &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE)
    ||  ((obj = get_eq_char(ch,WEAR_NECK_2)) != NULL
    &&   obj->pIndexData->vnum == OBJ_VNUM_WHISTLE))
    {
    act("You blow down hard on $p.",ch,obj,NULL,TO_CHAR);
    act("$n blows on $p, ***WHEEEEEEEEEEEET***",ch,obj,NULL,TO_ROOM);

        for ( vch = char_list; vch != NULL; vch = vch->next )
        {
            if ( vch->in_room == NULL )
                continue;

            if (vch->in_room != ch->in_room
        &&  vch->in_room->area == ch->in_room->area)
                send_to_char( "You hear a shrill whistling sound.\n\r", vch );
        }
    }

    switch (number_range(0,6))
    {
    default:    message = NULL;     break;
    case 0: message = "$n yells 'All roit! All roit! break it up!'";
        break;
    case 1: message =
        "$n says 'Society's to blame, but what's a bloke to do?'";
        break;
    case 2: message =
        "$n mumbles 'bloody kids will be the death of us all.'";
        break;
    case 3: message = "$n shouts 'Stop that! Stop that!' and attacks.";
        break;
    case 4: message = "$n pulls out his billy and goes to work.";
        break;
    case 5: message =
        "$n sighs in resignation and proceeds to break up the fight.";
        break;
    case 6: message = "$n says 'Settle down, you hooligans!'";
        break;
    }

    if (message != NULL)
    act(message,ch,NULL,NULL,TO_ALL);

    multi_hit(ch,victim,TYPE_UNDEFINED);

    return TRUE;
}


bool spec_nasty( CHAR_DATA *ch )
{
    CHAR_DATA *victim, *v_next;
    long gold;

    if (!IS_AWAKE(ch) || ch->stopped > 0) {
       return FALSE;
    }

    if (ch->position != POS_FIGHTING) {
       for ( victim = ch->in_room->people; victim != NULL; victim = v_next)
       {
          v_next = victim->next_in_room;
          if (!IS_NPC(victim)
             && (victim->level > ch->level)
             && (victim->level < ch->level + 10))
          {
         do_function(ch, &do_backstab, victim->name);
             if (ch->position != POS_FIGHTING)
         {
                 do_function(ch, &do_kill, victim->name);
         }

             /* should steal some coins right away? :) */
             return TRUE;
          }
       }
       return FALSE;    /*  No one to attack */
    }

    /* okay, we must be fighting.... steal some coins and flee */
    if ( (victim = ch->fighting) == NULL)
        return FALSE;   /* let's be paranoid.... */

    switch ( number_bits(2) )
    {
        case 0:  act( "$n rips apart your coin purse, spilling your gold!",
                     ch, NULL, victim, TO_VICT);
                 act( "You slash apart $N's coin purse and gather his gold.",
                     ch, NULL, victim, TO_CHAR);
                 act( "$N's coin purse is ripped apart!",
                     ch, NULL, victim, TO_NOTVICT);
                 gold = victim->gold / 10;  /* steal 10% of his gold */
                 victim->gold -= gold;
                 ch->gold     += gold;
                 return TRUE;

        case 1:  do_function(ch, &do_flee, "");
                 return TRUE;

        default: return FALSE;
    }
}

/*
 * Core procedure for dragons.
 */
bool dragon( CHAR_DATA *ch, char *spell_name )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    int sn;

    if ( ch->position != POS_FIGHTING || ch->stopped > 0)
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 3 ) == 0 )
        break;
    }

    if ( victim == NULL )
    return FALSE;

    if ( ( sn = skill_lookup( spell_name ) ) < 0 )
    return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim, TARGET_CHAR);
    return TRUE;
}



/*
 * Special procedures for mobiles.
 */
bool spec_breath_any( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0)
    return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_bits( 3 ) )
    {
    case 0: return spec_breath_fire     ( ch );
    case 1:
    case 2: return spec_breath_lightning    ( ch );
    case 3: return spec_breath_gas      ( ch );
    case 4: return spec_breath_acid     ( ch );
    case 5:
    case 6:
    case 7: return spec_breath_frost        ( ch );
    }

    return FALSE;
}



bool spec_breath_acid( CHAR_DATA *ch )
{
    if(ch->stopped > 0) return FALSE;
    return dragon( ch, "acid breath" );
}



bool spec_breath_fire( CHAR_DATA *ch )
{
    if(ch->stopped > 0) return FALSE;
    return dragon( ch, "fire breath" );
}



bool spec_breath_frost( CHAR_DATA *ch )
{
    if(ch->stopped > 0) return FALSE;
    return dragon( ch, "frost breath" );
}



bool spec_breath_gas( CHAR_DATA *ch )
{
    int sn;

    if ( ch->position != POS_FIGHTING )
    return FALSE;

    if(ch->stopped > 0) return FALSE;

    if ( ( sn = skill_lookup( "gas breath" ) ) < 0 )
    return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, NULL,TARGET_CHAR);
    return TRUE;
}



bool spec_breath_lightning( CHAR_DATA *ch )
{
    if(ch->stopped > 0) return FALSE;
    return dragon( ch, "lightning breath" );
}



bool spec_cast_adept( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if ( !IS_AWAKE(ch) )
    return FALSE;

    if(ch->stopped > 0) return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
         && !IS_NPC(victim) && victim->level < 16)
        break;
    }

    if ( victim == NULL )
    return FALSE;

    switch ( number_bits( 4 ) )
    {
    case 0:
    act( "$n utters the word 'abrazak'.", ch, NULL, NULL, TO_ROOM );
    spell_armor( skill_lookup( "armor" ), ch->level,ch,victim,TARGET_CHAR);
    return TRUE;

    case 1:
    act( "$n utters the word 'fido'.", ch, NULL, NULL, TO_ROOM );
    spell_bless( skill_lookup( "bless" ), ch->level,ch,victim,TARGET_CHAR);
    return TRUE;

    case 2:
    act("$n utters the words 'judicandus noselacri'.",ch,NULL,NULL,TO_ROOM);
    spell_cure_blindness( skill_lookup( "cure blindness" ),
        ch->level, ch, victim,TARGET_CHAR);
    return TRUE;

    case 3:
    act("$n utters the words 'judicandus dies'.", ch,NULL, NULL, TO_ROOM );
    spell_cure_light( skill_lookup( "cure light" ),
        ch->level, ch, victim,TARGET_CHAR);
    return TRUE;

    case 4:
    act( "$n utters the words 'judicandus sausabru'.",ch,NULL,NULL,TO_ROOM);
    spell_cure_poison( skill_lookup( "cure poison" ),
        ch->level, ch, victim,TARGET_CHAR);
    return TRUE;

    case 5:
    act("$n utters the word 'candusima'.", ch, NULL, NULL, TO_ROOM );
    spell_refresh( skill_lookup("refresh"),ch->level,ch,victim,TARGET_CHAR);
    return TRUE;

/*    case 6:
    act("$n utters the words 'judicandus eugzagz'.",ch,NULL,NULL,TO_ROOM);
    spell_cure_disease(skill_lookup("cure disease"),
        ch->level,ch,victim,TARGET_CHAR);*/
    }

    return FALSE;
}



bool spec_cast_cleric( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
    return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 )
        break;
    }

    if ( victim == NULL )
    return FALSE;

    for ( ;; )
    {
    int min_level;

    switch ( number_bits( 4 ) )
    {
    case  0: min_level =  0; spell = "blindness";      break;
    case  1: min_level =  3; spell = "cause serious";  break;
    case  2: min_level =  7; spell = "earthquake";     break;
    case  3: min_level =  9; spell = "cause critical"; break;
    case  4: min_level = 10; spell = "dispel evil";    break;
    case  5: min_level = 12; spell = "curse";          break;
    case  6: min_level = 12; spell = "change sex";     break;
    case  7: min_level = 13; spell = "flamestrike";    break;
    case  8:
    case  9:
    case 10: min_level = 15; spell = "harm";           break;
    case 11: min_level = 15; spell = "change alignment";       break;
    default: min_level = 16; spell = "dispel magic";   break;
    }

    if ( ch->level >= min_level )
        break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    return FALSE;
    say_spell(ch, sn);
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_judge( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    spell = "high explosive";
    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_mage( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
    return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 )
        break;
    }

    if ( victim == NULL )
    return FALSE;

    for ( ;; )
    {
    int min_level;

    switch ( number_bits( 4 ) )
    {
    case  0: min_level =  0; spell = "blindness";      break;
    case  1: min_level =  3; spell = "chill touch";    break;
    case  2: min_level =  7; spell = "weaken";         break;
    case  3: min_level =  8; spell = "teleport";       break;
    case  4: min_level = 11; spell = "colour spray";   break;
    case  5: min_level = 12; spell = "change sex";     break;
    case  6: min_level = 13; spell = "energy drain";   break;
    case  7: min_level = 14; spell = "magic missile";   break;
    case  8: min_level = 14; spell = "slow";   break;
    case  9:
    case 10: min_level = 15; spell = "fireball";       break;
    case 11: min_level = 20; spell = "change alignment";       break;
    default: min_level = 20; spell = "acid blast";     break;
    }

    if ( ch->level >= min_level )
        break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    return FALSE;
    say_spell(ch, sn);
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_undead( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
    return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 )
        break;
    }

    if ( victim == NULL )
    return FALSE;

    for ( ;; )
    {
    int min_level;

    switch ( number_bits( 4 ) )
    {
    case  0: min_level =  0; spell = "curse";          break;
    case  1: min_level =  3; spell = "weaken";         break;
    case  2: min_level =  6; spell = "chill touch";    break;
    case  3: min_level =  9; spell = "blindness";      break;
    case  4: min_level = 12; spell = "poison";         break;
    case  5: min_level = 15; spell = "energy drain";   break;
    case  6: min_level = 18; spell = "harm";           break;
    case  7: min_level = 21; spell = "teleport";       break;
    case  8: min_level = 20; spell = "slow";       break;
    default: min_level = 18; spell = "harm";           break;
    }

    if ( ch->level >= min_level )
        break;
    }

    if ( ( sn = skill_lookup( spell ) ) < 0 )
    return FALSE;
    say_spell(ch, sn);
    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}


bool spec_executioner( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *crime;

    if(ch->stopped > 0) return FALSE;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
    return FALSE;

    for(victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;

        if (!IS_NPC(victim) && !IS_IMMORTAL(victim) && ((IS_AFFECTED(victim,AFF_FANGS) && !IS_AFFECTED(victim,AFF_SHIFT) && can_see( ch, victim )) ||
        (victim->changed == CHANGED_CRINOS && can_see( ch, victim )) || ((is_affected(victim,gsn_vicissitude_horrid) || is_affected(victim, gsn_vicissitude_chiropteran)) && can_see( ch, victim )) ||
        (is_affected(victim,gsn_vicissitude_bonecraft) && can_see( ch, victim )) || (victim->inhumanity >= 5 && can_see( ch, victim ))))
        {
            AFFECT_DATA af;
            sprintf( buf, "%s is a monster! Take this foul beast!", victim->name);
            do_function(ch, &do_say, buf );
            if(!IS_AFFECTED(victim, AFF_CURSE))
            {
                af.where     = TO_AFFECTS;
                af.type      = gsn_curse;
                af.level     = victim->level;
                af.duration  = 5;
                af.modifier  = -20;
                af.location  = APPLY_DAMROLL;
                af.bitvector = AFF_CURSE;
                affect_to_char( victim, &af );
                af.modifier  = -20;
                af.location  = APPLY_HITROLL;
                affect_to_char( victim, &af );
                af.modifier  = 1000;
                af.location  = APPLY_AC;
                affect_to_char( victim, &af );
            }
            do_function(victim, &do_emote, "runs away screaming in pain." );
            do_function(victim, &do_flee, "auto" );
        }
    }


    if ( victim == NULL )
    return FALSE;
}



bool spec_fido( CHAR_DATA *ch )
{
    OBJ_DATA *corpse;
    OBJ_DATA *c_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if(ch->stopped > 0) return FALSE;

    if ( !IS_AWAKE(ch) )
    return FALSE;

    for ( corpse = ch->in_room->contents; corpse != NULL; corpse = c_next )
    {
    c_next = corpse->next_content;
    if ( corpse->item_type != ITEM_CORPSE_NPC )
        continue;

    act( "$n savagely devours a corpse.", ch, NULL, NULL, TO_ROOM );
    for ( obj = corpse->contains; obj; obj = obj_next )
    {
        obj_next = obj->next_content;
        obj_from_obj( obj );
        obj_to_room( obj, ch->in_room );
    }
    extract_obj( corpse );
    return TRUE;
    }

    return FALSE;
}



bool spec_guard( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    CHAR_DATA *ech;
    char *crime;
    int max_evil;

    if(ch->stopped > 0) return FALSE;

    if ( !IS_AWAKE(ch) || ch->fighting != NULL )
    return FALSE;

    max_evil = 300;
    ech      = NULL;
    crime    = "";

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_KILLER)
    &&   can_see(ch,victim))
        { crime = "KILLER"; break; }

    if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_THIEF)
    &&   can_see(ch,victim))
        { crime = "THIEF"; break; }

    if ( victim->fighting != NULL
    &&   victim->fighting != ch
    &&   victim->alignment < max_evil )
    {
        max_evil = victim->alignment;
        ech      = victim;
    }
    }

    if ( victim != NULL )
    {
    sprintf( buf, "%s is a %s! I shall rid this world of you!!",
        victim->name, crime );
    REMOVE_BIT(ch->comm,COMM_NOSHOUT);
    do_function(ch, &do_yell, buf );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return TRUE;
    }

    if ( ech != NULL )
    {
    act( "$n screams 'Foul Beast! I shall rid this world of you!!'",
        ch, NULL, NULL, TO_ROOM );
    multi_hit( ch, ech, TYPE_UNDEFINED );
    return TRUE;
    }

    return FALSE;
}



bool spec_janitor( CHAR_DATA *ch )
{
    OBJ_DATA *trash;
    OBJ_DATA *trash_next;

    if(ch->stopped > 0) return FALSE;

    if ( !IS_AWAKE(ch) )
    return FALSE;

    for ( trash = ch->in_room->contents; trash != NULL; trash = trash_next )
    {
    trash_next = trash->next_content;
    if ( !IS_SET( trash->wear_flags, ITEM_TAKE ) || !can_loot(ch,trash) || count_users(trash))
        continue;
    if ( trash->item_type == ITEM_DRINK_CON
    ||   trash->item_type == ITEM_TRASH
    ||   trash->cost < 10 )
    {
        act( "$n picks up some trash.", ch, NULL, NULL, TO_ROOM );
        obj_from_room( trash );
        obj_to_char( trash, ch );
        return TRUE;
    }
    }

    return FALSE;
}



bool spec_mayor( CHAR_DATA *ch )
{

    static const char open_path[] =
    "W3a3003b33000c111d0d111Oe333333Oe22c222112212111a1S.";

    static const char close_path[] =
    "W3a3003b33000c111d0d111CE333333CE22c222112212111a1S.";

    static const char *path;
    static int pos;
    static bool move;


    if(ch->stopped > 0) return FALSE;

    if ( !move )
    {
    if ( time_info.hour ==  6 )
    {
        path = open_path;
        move = TRUE;
        pos  = 0;
    }

    if ( time_info.hour == 20 )
    {
        path = close_path;
        move = TRUE;
        pos  = 0;
    }
    }

    if ( ch->fighting != NULL )
    return spec_cast_mage( ch );
    if ( !move || ch->position < POS_SLEEPING )
    return FALSE;

    switch ( path[pos] )
    {
    case '0':
    case '1':
    case '2':
    case '3':
    move_char( ch, path[pos] - '0', FALSE );
    break;

    case 'W':
    ch->position = POS_STANDING;
    act( "$n awakens and groans loudly.", ch, NULL, NULL, TO_ROOM );
    break;

    case 'S':
    ch->position = POS_SLEEPING;
    act( "$n lies down and falls asleep.", ch, NULL, NULL, TO_ROOM );
    break;

    case 'a':
    act( "$n says 'Hello Honey!'", ch, NULL, NULL, TO_ROOM );
    break;

    case 'b':
    act( "$n says 'What a view!  I must do something about that dump!'",
        ch, NULL, NULL, TO_ROOM );
    break;

    case 'c':
    act( "$n says 'Vandals!  Youngsters have no respect for anything!'",
        ch, NULL, NULL, TO_ROOM );
    break;

    case 'd':
    act( "$n says 'Good day, citizens!'", ch, NULL, NULL, TO_ROOM );
    break;

    case 'e':
    act( "$n says 'I hereby declare the city of Midgaard open!'",
        ch, NULL, NULL, TO_ROOM );
    break;

    case 'E':
    act( "$n says 'I hereby declare the city of Midgaard closed!'",
        ch, NULL, NULL, TO_ROOM );
    break;

    case 'O':
/*  do_function(ch, &do_unlock, "gate" ); */
    do_function(ch, &do_open, "gate" );
    break;

    case 'C':
    do_function(ch, &do_close, "gate" );
/*  do_function(ch, &do_lock, "gate" ); */
    break;

    case '.' :
    move = FALSE;
    break;
    }

    pos++;
    return FALSE;
}



bool spec_poison( CHAR_DATA *ch )
{
    CHAR_DATA *victim;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING
    || ( victim = ch->fighting ) == NULL
    ||   number_percent( ) > 2 * ch->level )
    return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    act( "You bite $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n bites $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n bites you!", ch, NULL, victim, TO_VICT    );
    spell_poison( gsn_poison, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}



bool spec_thief( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    long gold,silver;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_STANDING )
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;

    if ( IS_NPC(victim)
    ||   victim->level >= LEVEL_IMMORTAL
    ||   number_bits( 5 ) != 0
    ||   !can_see(ch,victim))
        continue;

    if ( IS_AWAKE(victim) && number_range( 0, ch->level ) == 0 )
    {
        act( "You discover $n's hands in your wallet!",
        ch, NULL, victim, TO_VICT );
        act( "$N discovers $n's hands in $S wallet!",
        ch, NULL, victim, TO_NOTVICT );
        return TRUE;
    }
    else
    {
        gold = victim->gold * UMIN(number_range(1,20),ch->level / 2) / 100;
        gold = UMIN(gold, ch->level * ch->level * 10 );
        ch->gold     += gold;
        victim->gold -= gold;
        silver = victim->silver * UMIN(number_range(1,20),ch->level/2)/100;
        silver = UMIN(silver,ch->level*ch->level * 25);
        ch->silver  += silver;
        victim->silver -= silver;
        return TRUE;
    }
    }

    return FALSE;
}

bool spec_jarjar( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

    if(ch->stopped > 0) return FALSE;

    if ( !IS_AWAKE(ch) )
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
         && !IS_NPC(victim))
        break;
    }

    if ( victim == NULL )
    return FALSE;

    switch ( number_bits( 4 ) )
    {
    case 0:
    act("$n says 'How roood!'", ch, NULL, NULL, TO_ROOM );
    return TRUE;
    case 1:
    act("$n says 'Mesa Jar Jar Binks, whosa yousa?'", ch, NULL, NULL, TO_ROOM );
    return TRUE;

    case 2:
    act("$n blinks and wanders around the room poking where he shouldn't.", ch, NULL, NULL, TO_ROOM );
    return TRUE;

    case 3:
    act("$n says 'Mesa Gungan, Mesa smart!'", ch, NULL, NULL, TO_ROOM );
    return TRUE;

    case 4:
    act("$n smiles at $N and says 'Mesa lovsa yousa!'", ch, NULL, victim, TO_ROOM );
    return TRUE;

    case 5:
    act("$n starts to dance around the room but trips on his own tongue.", ch, NULL, NULL, TO_ROOM );
    return TRUE;
    }

    return FALSE;
}

bool spec_cast_lightning( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn, num;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

       spell = NULL;
       num = number_range(1, 4);

        if (num == 1)  spell = "chain lightning";
        if (num == 2)  spell = "lightning breath";
        if (num == 3)  spell = "lightning fist";
        if (num == 4)  spell = "lightning bolt";

    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;

    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_stake( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    OBJ_DATA *stake;
    char buf[MAX_STRING_LENGTH];

    return FALSE;
}
/*
    if(ch->stopped > 0) return FALSE;

    if ( ch->position == POS_FIGHTING )
        return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
         && !IS_NPC(victim))
        break;
    }

    if ( victim == NULL )
        return FALSE;
    if (victim->race != race_lookup("vampire")&& victim->race != race_lookup("methuselah"))
      return FALSE;
    if ( ( stake = get_eq_char( ch, WEAR_HOLD ) ) == NULL || !IS_OBJ_STAT(stake,ITEM_IS_STAKE))
      return FALSE;
    if (victim->position != POS_TORPOR)
      return FALSE;

    sprintf(buf, "You scream as %s drives a stake into your heart!\n\r", ch->short_descr);
    send_to_char(buf,victim);
    act( "$N screams as $n drives a stake into $S heart!", ch, NULL, victim, TO_ROOM );
        if(victim->race == race_lookup("methuselah")) victim->race = race_lookup("vampire");
        if(victim->pcdata->points < 30) victim->pcdata->points = 30;
        victim->exp   = exp_per_level(victim,victim->pcdata->points) * UMAX( 1, victim->level );
      victim->max_pblood -= 10;
        victim->pblood = victim->max_pblood;
        victim->gen += 1;
        victim->dpoints = 0;

        if(victim->auspex > 0) victim->auspex -= 1;
        if(victim->protean > 0) victim->protean -= 1;
        if(victim->celerity > 0) victim->celerity -= 1;
        if(victim->fortitude > 0) victim->fortitude -= 1;
        if(victim->potence > 0) victim->potence -= 1;
        if(victim->obfuscate > 0) victim->obfuscate -= 1;
        if(victim->obtenebration > 0) victim->obtenebration -= 1;
        if(victim->domination > 0) victim->domination -= 1;
        if(victim->presence > 0) victim->presence -= 1;
        if(victim->mortis > 0) victim->mortis -= 1;
        if(victim->quietus > 0) victim->quietus -= 1;
        if(victim->animalism > 0) victim->animalism -= 1;
        if(victim->vicissitude > 0) victim->vicissitude -= 1;

        while ( victim->affected )
            affect_remove( victim, victim->affected );
        victim->affected_by = race_table[victim->race].aff;
        kill_em(ch, victim);
        save_char_obj(victim);
        victim->max_hit -= ch->level;
        victim->pcdata->perm_hit = victim->max_hit;
    return TRUE;
}
*/

bool spec_bo_dog( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    AFFECT_DATA af;

    if(ch->stopped > 0) return FALSE;

     for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
         && !IS_NPC(victim))
        break;
    }

    if ( victim == NULL )
        return FALSE;
    if (str_prefix( victim->name, "Jasmine"))
      return FALSE;
    if (IS_AFFECTED(ch, AFF_CHARM))
      return FALSE;

    if ( ch->master )
    stop_follower( ch );
    add_follower( ch, victim );
    ch->leader = victim;
    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( ch, &af );

    act( "$n barks and jumps up and down, happy to see you!", ch, NULL, victim, TO_VICT );
    act( "$n barks and jumps up and down, happy to see $N!", ch, NULL, victim, TO_NOTVICT );
    return TRUE;
}

bool spec_lag( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;

	if (!IS_AWAKE(ch) || ch->stopped > 0 || is_affected( ch, gsn_forget ))
      return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
    v_next = victim->next_in_room;
    if ( victim != ch && can_see( ch, victim ) && number_bits( 1 ) == 0
         && !IS_NPC(victim))
        break;
    }

    if ( victim == NULL || IS_IMMORTAL(victim))
        return FALSE;
    if(number_range(1,100) <= 50)
    {
        act( "$n cackles as he reaches out to touch you... YOU ARE LAGGED!", ch, NULL, victim, TO_VICT );
        act( "$n cackles as he reaches out menacingly... $N IS LAGGED!", ch, NULL, victim, TO_NOTVICT );
        WAIT_STATE( victim, 36 );
    }
    return TRUE;
}

/* Clan Specs */
bool spec_assamite( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
      return FALSE;
/*
      switch ( number_range( 0,2 ) )
      {
      case 0: return spec_celerity (ch);
      case 1: return spec_obfuscate (ch);
      case 2: return spec_quietus (ch);
      }
*/
    return FALSE;
}

bool spec_baali( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
      return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_daimoinon (ch);
          case 1: return spec_obfuscate (ch);
          case 2: return spec_presence (ch);
          }
    */
    return FALSE;
}

bool spec_brujah( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
      return FALSE;

    switch ( number_range( 0,2 ) )
    {
      case 0: return spec_celerity (ch);
      case 1: return spec_potence (ch);
      case 2: return spec_presence (ch);
    }

    return FALSE;
}

bool spec_cappadocian( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_auspex (ch);
          case 1: return spec_fortitude (ch);
          case 2: return spec_mortis (ch);
          }
    */
    return FALSE;
}

bool spec_followersofset( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_obfuscate (ch);
          case 1: return spec_presence (ch);
          case 2: return spec_serpentis (ch);
          }
    */
    return FALSE;
}

bool spec_gangrel( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_animalism (ch);
          case 1: return spec_fortitude (ch);
          case 2: return spec_protean (ch);
          }
    */
    return FALSE;
}

bool spec_gargoyle( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_flight (ch);
          case 1: return spec_fortitude (ch);
          case 2: return spec_potence (ch);
          }
    */
    return FALSE;
}

bool spec_lasombra( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_range( 0,2 ) )
    {
    case 0: return spec_dominate (ch);
    case 1: return spec_obtenebration (ch);
    case 2: return spec_potence (ch);
    }

    return FALSE;
}

bool spec_malkavian( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_auspex (ch);
          case 1: return spec_dementation (ch);
          case 2: return spec_obfuscate (ch);
          }
    */
    return FALSE;
}

bool spec_nosferatu( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_animalism (ch);
          case 1: return spec_obfuscate (ch);
          case 2: return spec_potence (ch);
          }
    */
    return FALSE;
}

bool spec_ravnos( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_animalism (ch);
          case 1: return spec_chimerstry (ch);
          case 2: return spec_fortitude (ch);
          }
    */
    return FALSE;
}

bool spec_salubri( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,3 ) )
          {
          case 0: return spec_auspex (ch);
          case 1: return spec_fortitude (ch);
          case 2: return spec_obeah (ch);
          case 3: return spec_valeran (ch);
          }
    */
    return FALSE;
}

bool spec_toreador( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_auspex (ch);
          case 1: return spec_celerity (ch);
          case 2: return spec_presence (ch);
          }
    */
    return FALSE;
}

bool spec_tremere( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_auspex (ch);
          case 1: return spec_dominate (ch);
          case 2: return spec_thaum_regovitae (ch);
          }
    */
    return FALSE;
}

bool spec_tzimisce( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_animalism (ch);
          case 1: return spec_auspex (ch);
          case 2: return spec_vicissitude (ch);
          }
    */
    return FALSE;
}

bool spec_ventrue( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;
    /*
          switch ( number_range( 0,2 ) )
          {
          case 0: return spec_dominate (ch);
          case 1: return spec_fortitude (ch);
          case 2: return spec_presence (ch);
          }
    */
    return FALSE;
}

/* Discipline Specs */
bool spec_celerity( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
/*    char *spell;
    int sn, num;
*/
    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    act( "$n roars and suddenly attacks with inhuman speed!",ch,NULL, victim, TO_ROOM );
    multi_hit(ch,victim, TYPE_UNDEFINED);
    if(ch->level >= 25)
        multi_hit(ch,victim, TYPE_UNDEFINED);
    if(ch->level >= 50)
        multi_hit(ch,victim, TYPE_UNDEFINED);
    if(ch->level >= 75)
        multi_hit(ch,victim, TYPE_UNDEFINED);
    if(ch->level >= 100)
        multi_hit(ch,victim, TYPE_UNDEFINED);
    return TRUE;
}

bool spec_dominate( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_range( 0,2 ) )
    {
    case 0: return spec_command (ch);
    case 1: return spec_mesmerize (ch);
    case 2: return spec_forgetfulmind (ch);
    }

    return FALSE;
}

bool spec_obtenebration( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0)
    return FALSE;

    if(is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_range( 0,2 ) )
    {
    case 0: return spec_shadowplay (ch);
    case 1: return spec_nocturne (ch);
    case 2: return spec_armsoftheabyss (ch);
    }

    return FALSE;
}

bool spec_potence( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn, num;
    int potence, successes, damagesuccess, damagemod;

    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected(ch, gsn_forget))
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

    act( "$n roars and winds up for a massive roundhouse punch!", ch, NULL, victim, TO_ROOM );

    successes = godice(get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_BRAWL), 6);
    if (successes <= 0)
    {
      act("$n's swing goes wide, missing you!", ch, NULL, victim, TO_VICT);
      act("$n's swing misses $N by a wide margin!", ch, NULL, victim, TO_NOTVICT);
      return FALSE;
    }
    potence = ch->level / 12;
    damagesuccess = godice(get_attribute(ch, STRENGTH) + potence, 6);
    if (damagesuccess <= 0)
      damagesuccess = 1;
    damagemod = number_range(ch->level * 3 / 2, ch->level * 2);

    act("With a resounding crack, $n's punch collides into your chest cavity!", ch, NULL, victim, TO_VICT);
    act("With a loud crack, $n punches $N directly in the chest!", ch, NULL, victim, TO_NOTVICT);
/*  -- D10 damage calculations removed until d10 for mobs fixed and calculated correctly
    d10_damage(ch, victim, damagesuccess, damagemod*4 , gsn_hand_to_hand, DAM_BASH, DEFENSE_ARMOR, TRUE, TRUE);
*/
    damage(ch, victim, damagemod * damagesuccess, gsn_hand_to_hand, DAM_BASH, TRUE);
    DAZE_STATE(victim, 3*PULSE_VIOLENCE);
    victim->stopped += 1 + successes;

    return TRUE;
}

bool spec_presence( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_range( 0,2 ) )
    {
/*    case 0: return spec_awe (ch);
    case 1: return spec_dreadgaze (ch);
    case 2: return spec_entrancement (ch); */

    case 0:
    case 1: return spec_awe (ch);
    case 2: return spec_dreadgaze (ch);
    }

    return FALSE;
}

bool spec_serpentis( CHAR_DATA *ch )
{
    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    switch ( number_range( 0,2 ) )
    {
/*    case 0: return spec_awe (ch);
    case 1: return spec_dreadgaze (ch);
    case 2: return spec_entrancement (ch); */

    case 0:
    case 1:
    case 2: return spec_eyesoftheserpent (ch);
    }

    return FALSE;
}

/* Dominate Specs */
bool spec_command( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    AFFECT_DATA af;

    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected(ch, gsn_forget))
        return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if (( victim->fighting == ch && number_bits( 2 ) == 0 )
        && !is_affected(victim, gsn_deafened) && can_see(victim, ch) && can_see(ch, victim))
            break;
    }

    if ( victim == NULL )
        return FALSE;

    switch(number_range(1,3))
    {
        case 1:
                act( "$n chuckles and stares deep into your eyes, 'You look uncomfortable in that garb.  Strip for me, puppet.'", ch, NULL, victim, TO_VICT );
                act( "$n chuckles, looks deep into $N's eyes, saying 'You look uncomfortable in that garb.  Strip for me, puppet.'", ch, NULL, victim, TO_NOTVICT );
                do_function(victim, &do_remove, "all");
                break;
        case 2:
                act( "$n glares at you and shouts 'Run!'", ch, NULL, victim, TO_VICT );
                act( "$n glares at $N and shouts 'Run!'", ch, NULL, victim, TO_NOTVICT );
                do_function(victim, &do_flee, "auto");
                break;
        case 3:
                act( "$n peers into your eyes, then says 'Sleep my pretty...'", ch, NULL, victim, TO_VICT );
                act( "$n peers into $N's eyes, then says 'Sleep my pretty...'", ch, NULL, victim, TO_NOTVICT );
                stop_fighting(victim, ch);
                do_function(victim, &do_sleep, "");
                af.where     = TO_AFFECTS;
                af.type      = gsn_sleep;
                af.level     = ch->level/6;
                af.duration  = number_range(1,3);
                af.location  = 0;
                af.modifier  = 0;
                af.bitvector = AFF_SLEEP;
                affect_to_char( victim, &af );
                break;

        default:
                act( "$n waves $N away.", ch, NULL, victim, TO_NOTVICT );
                act( "$n waves you away.", ch, NULL, victim, TO_VICT );
                do_function(victim, &do_flee, "flee");
                break;
    }
    return TRUE;
}

bool spec_mesmerize( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  OBJ_DATA *wield;
  char buf[MSL], weapon[MSL], mob[MSL];
  int success;

    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if (( victim->fighting == ch && number_bits( 2 ) == 0 )
        && !is_affected(victim, gsn_deafened) && can_see(victim, ch) && can_see(ch, victim))
            break;
    }

    if ( victim == NULL )
        return FALSE;

    if ( ( wield = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
      return FALSE;

    success = godice(get_attribute(ch, MANIPULATION) + get_ability(ch, CSABIL_LEADERSHIP), 6);

    if (success <= 0)
      return FALSE;

    switch ( number_range( 0,6 ) )
    {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
              act( "Smiling slyly, $n looks at $N and says, '{WYou are going to hurt someone with that.  Why don't you give it to me?{x'", ch, NULL, victim, TO_NOTVICT );
              act( "Smiling slyly, $n looks at you and says, '{WYou are going to hurt someone with that.  Why don't you give it to me?{x'", ch, NULL, victim, TO_VICT );
              act( "With an intense urge, you find yourself complying without fail.", ch, NULL, victim, TO_VICT);
              sprintf(weapon, "%s", wield->name);
              do_function(victim, &do_remove, weapon);
              sprintf(mob, "%s", ch->name);
              one_argument(weapon, weapon);
              one_argument(mob, mob);
              sprintf(buf, "%s %s", weapon, mob);
              do_function(victim, &do_give, buf);
              break;
      case 6:
              act( "With a stern countenance, $n looks at $N and says, '{WYou cannot hope to win.  You need to rush on home before you get hurt!{x'", ch, NULL, victim, TO_NOTVICT );
              act( "With a stern countenance, $n looks at you and says, '{WYou cannot hope to win.  You need to rush on home before you get hurt!{x'", ch, NULL, victim, TO_VICT );
              do_function(victim, &do_flee, "auto");
              act( "With an overwhelming sense of self-preservation, you decide to recall to safety!", ch, NULL, victim, TO_VICT);
              do_function(victim, &do_recall, "");
              break;
    }

    return FALSE;
}

bool spec_forgetfulmind( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int dominate;
  AFFECT_DATA af;

    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
      v_next = victim->next_in_room;
      if ( victim->fighting == ch && number_bits( 2 ) == 0
      && can_see(victim, ch) && can_see(ch, victim))
        break;
    }

    if ( victim == NULL )
      return FALSE;

    if ( is_affected(victim, gsn_forget))
      return FALSE;

    act("You catch $n's eyes and stare deeply, manipulating memories!", ch, NULL, victim, TO_CHAR);
    act("You find yourself staring deeply into $n's eyes, as your mind gets hazy.", ch, NULL, victim, TO_VICT);
    act("$n stares deeply into $N's eyes, who seems to falter for a moment.", ch, NULL, victim, TO_NOTVICT);

    af.where     = TO_AFFECTS;
    af.type      = gsn_forget;
    af.level     = dominate;
    af.duration  = 1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    return TRUE;
}

/* Obtenebration Specs */
bool spec_shadowplay( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int shadowsuccess, obtenebration;
  AFFECT_DATA af;

  if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected(ch, gsn_forget))
      return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
  }

  if ( victim == NULL )
    return FALSE;

  if ( is_affected(victim, gsn_shadowplay))
    return FALSE;

  act( "{DDarkness{x rises from $n's {Ds{wh{Dad{wo{Dw{x and gathers around $s form.", ch, NULL, victim, TO_ROOM );
  obtenebration = ch->level / 20;
  shadowsuccess = godice(get_attribute(ch, WITS) + obtenebration, 6);

  if (shadowsuccess <= 0)
  {
    act("With but a quick flicker, the {Ddarkness{x dissipates as quickly as it appeared.", ch, NULL, victim, TO_ROOM);
    return FALSE;
  }

  act("You send shadows to blind and suffocate $N!", ch, NULL, victim, TO_CHAR);
  act("{DSh{wad{Dows{x leap at $n's command and fill your mouth and eyes!", ch, NULL, victim, TO_VICT);
  act("{DSh{wad{Dows{x leap from $n's form and attack $N!", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_shadowplay;
  af.level     = obtenebration;
  af.duration  = 1;
  af.location  = APPLY_CS_STA;
  af.modifier  = -1;
  af.bitvector = 0;
  affect_to_char( victim, &af );
  af.location  = APPLY_HITROLL;
  af.modifier  = -(shadowsuccess*ch->level);
  if (shadowsuccess > 3)
  {
    af.bitvector = AFF_BLIND;
    act("{WThe {Ds{wh{Dad{wo{Dws {Wwrithe about your face, over your eyes and obstructing your vision!{x", ch, NULL, victim, TO_VICT);
  }
  affect_to_char( victim, &af );

  return TRUE;
}

bool spec_nocturne( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  AFFECT_DATA af;
  int obtenebration, obtensuccess;

  obtenebration = (ch->level / 20) + 1;
  obtensuccess = godice(obtenebration, 6);

  if (number_range(0,2) < 2)
    return FALSE;

  if ( ch->position != POS_FIGHTING || ch->stopped > 0
    || is_affected(ch, gsn_forget) || obtensuccess <= 0)
    return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( is_affected(victim, gsn_nocturne))
      continue;
    af.where     = TO_AFFECTS;
    af.type      = gsn_nocturne;
    af.level     = obtenebration;
    af.duration  = 0;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_BLIND;
    affect_to_char( victim, &af );
    act("{DMystical Darkness descends across the area, dropping like a curtain of the empty void.{x", ch, NULL, NULL, TO_CHAR);
    act("{DMystical Darkness descends across the area, dropping like a curtain of the empty void.{x", ch, NULL, victim, TO_VICT);
    act("The darkness clings to your eyes, impairing your sight!", ch, NULL, victim, TO_VICT);
  }

  return TRUE;
}

bool spec_armsoftheabyss( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  AFFECT_DATA af;
  int obtenebration, obtensuccess;

    obtenebration = (ch->level / 20) + 1;
    obtensuccess = godice(get_attribute(ch, MANIPULATION) + get_ability(ch, CSABIL_OCCULT), 7);

    if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if (( victim->fighting == ch && number_bits( 2 ) == 0 )
        &&  can_see(ch, victim))
            break;
    }

    if ( victim == NULL )
        return FALSE;

    if (is_affected(victim, gsn_armsoftheabyss))
      return FALSE;

    act( "$n smiles as large, shadow tendrils grow and writhe from $s form.", ch, NULL, victim, TO_ROOM );
    if (obtensuccess <= 0)
      return FALSE;

    act( "The tendrils reach out and hold you firmly, preventing you from moving!", ch, NULL, victim, TO_VICT );
    act( "The tendrils strike out and hold $N tightly!", ch, NULL, victim, TO_NOTVICT );
    damage(ch, victim, ch->level / 5 * obtenebration * obtensuccess, gsn_armsoftheabyss, DAM_BASH, TRUE);

    af.where     = TO_AFFECTS;
    af.type      = gsn_armsoftheabyss;
    af.level     = obtenebration;
    af.duration  = 1;
    af.location  = APPLY_CS_DEX;
    af.modifier  = -1;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    return TRUE;
}


/* Presence Specs */
bool spec_awe( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  AFFECT_DATA af;
  int presence, awesuccess;

  if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected(ch, gsn_forget))
      return FALSE;

  presence = (ch->level / 20) + 1;
  awesuccess = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_PERFORMANCE), 7);

  if (awesuccess <= 0)
    return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( is_affected(victim, gsn_awe))
      continue;
    af.where     = TO_AFFECTS;
    af.type      = gsn_awe;
    af.level     = presence;
    af.duration  = awesuccess;
    af.location  = APPLY_HITROLL;
    af.modifier  = -3 * ch->level;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    act("You magnify your innate social magnetism.", ch, NULL, NULL, TO_CHAR);
    act("You feel your attention drawn to $n, and start to question any actions against $m.", ch, NULL, victim, TO_VICT);
  }

  return TRUE;
}

bool spec_dreadgaze( CHAR_DATA *ch)
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int dreadsuccess, dreaddiff;

  if ( ch->position != POS_FIGHTING || ch->stopped > 0 || is_affected(ch, gsn_forget))
      return FALSE;

  for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
      v_next = victim->next_in_room;
      if (( victim->fighting == ch && number_bits( 2 ) == 0 )
      &&  can_see(victim, ch) && can_see(ch, victim))
          break;
  }

  if ( victim == NULL )
      return FALSE;

  if (!IS_NPC(victim))
    dreaddiff = get_attribute(victim, WITS) + victim->pcdata->csvirtues[CSVIRT_COURAGE];
  else
    dreaddiff = get_attribute(victim, WITS) + (victim->level/25);

      act( "$n bares $s fangs and hisses at you!", ch, NULL, victim, TO_VICT );
      act( "$n bares sharp fangs and hisses at $N!", ch, NULL, victim, TO_NOTVICT );

  dreadsuccess = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_INTIMIDATION), dreaddiff);

  if (dreadsuccess <= 0)
    return FALSE;

  act("You are filled with an intense fear that rattles you to your core, and you attempt to flee immediately!", ch, NULL, victim, TO_VICT);
  do_function(victim, &do_flee, "auto");

  return TRUE;
}

/*bool spec_entrancement( CHAR_DATA *ch)
{
  return FALSE;
}
*/
/********************************/
bool spec_cast_acid( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn, num;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;
        spell = NULL;
       num = number_range(1, 2);

        if (num == 1)  spell = "acid blast";
        if (num == 2)  spell = "acid breath";

    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;

    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}


bool spec_cast_cold( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn, num;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

       spell = NULL;
       num = number_range(1, 2);

        if (num == 1)  spell = "chill touch";
        if (num == 2)  spell = "frost breath";

    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;

    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

bool spec_cast_fire( CHAR_DATA *ch )
{
    CHAR_DATA *victim;
    CHAR_DATA *v_next;
    char *spell;
    int sn, num;

    if(ch->stopped > 0) return FALSE;

    if ( ch->position != POS_FIGHTING )
        return FALSE;
    if(is_affected( ch, gsn_forget ))
    return FALSE;

    for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        if ( victim->fighting == ch && number_bits( 2 ) == 0 )
            break;
    }

    if ( victim == NULL )
        return FALSE;

       spell = NULL;
       num = number_range(1, 2);

        if (num == 1)  spell = "fireball";
        if (num == 2)  spell = "demonfire";
        if (num == 3)  spell = "fire breath";
        if (num == 4)  spell = "flamestrike";
        if (num == 5)  spell = "burning hands";

    if ( ( sn = skill_lookup( spell ) ) < 0 )
        return FALSE;

    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, victim,TARGET_CHAR);
    return TRUE;
}

// Gilean's new mob spec progs
bool spec_evil_eye( CHAR_DATA *ch )
{
  CHAR_DATA *victim;
  CHAR_DATA *v_next;
  int evileye = ch->level / 20;
  int evileyesuccess = 0;
  int evileyeaffect = 0;

  if (ch->stopped > 0 || !IS_AWAKE(ch) || ch->position != POS_FIGHTING )
    return FALSE;

  evileyesuccess = godice(get_attribute(ch, MANIPULATION) + evileye, 5);
  if (evileyesuccess <= 0)
    return FALSE;

  for(victim = ch->in_room->people; victim != NULL; victim = v_next )
  {
    v_next = victim->next_in_room;
    if ( victim->fighting == ch && number_bits( 2 ) == 0 )
      break;
  }

  if ( victim == NULL )
    return FALSE;

    AFFECT_DATA af;
    act("With a sly look, $n focuses $s gaze directly at you, seemingly piercing you to your core.", ch, NULL, victim, TO_VICT);
    act("$n shoots a quick look over at $N, who flinches momentarily.", ch, NULL, victim, TO_NOTVICT);
    if(!IS_AFFECTED(victim, AFF_CURSE))
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_curse;
      af.level     = victim->level;
      af.duration  = evileyesuccess + 5;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = AFF_CURSE;

      evileyeaffect = number_range(0,4);
      switch(evileyesuccess)
      {
        case 1:
                af.modifier = ch->level * 10;
                af.location = APPLY_AC;
                act("$n utters quite loudly, '{WI hope your gear rots in this weather!'{x", ch, NULL, NULL, TO_NOTVICT);
                break;
        case 2:
                af.modifier = -2;
                af.location = APPLY_CS_DEX;
                act("$n spits out in anger, '{WMay you be as graceful as a cow on ice!'{x", ch, NULL, NULL, TO_NOTVICT);
                break;
        case 3:
                af.modifier = -(victim->level * 10);
                af.location = APPLY_MOVE;
                act("$n intones briefly, '{WLet the weariness of your mind work through your muscles!'", ch, NULL, NULL, TO_NOTVICT);
                break;
        default:
                af.modifier  = -(ch->level) * 5;
                af.location  = APPLY_HITROLL;
                act("$n jeers, '{WYou cannot hit the broad side of your mother's rear end!{x", ch, NULL, NULL, TO_NOTVICT);
                break;
      }
      affect_to_char( victim, &af );

      send_to_char("You feel uneasy under the scrutiny of the Evil Eye.\n\r", victim);
    }

  return TRUE;
}
