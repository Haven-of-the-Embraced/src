/***************************************************************************
 *  Original Diku Mud copyright (C) 1-10, 1-11 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1-12, 1-13 by Michael          *
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
*   ROM 2.4 is copyright 1-13-1-18 Russ Taylor             *
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
#include "merc.h"
#include "magic.h"
#include "interp.h"


/* item type list */
const struct item_type      item_table  []  =
{
    {   ITEM_LIGHT, "light"     },
    {   ITEM_SCROLL,    "scroll"    },
    {   ITEM_WAND,  "wand"      },
    {   ITEM_STAFF, "staff"     },
    {   ITEM_WEAPON,    "weapon"    },
    {   ITEM_TREASURE,  "treasure"  },
    {   ITEM_ARMOR, "armor"     },
    {   ITEM_POTION,    "potion"    },
    {   ITEM_CLOTHING,  "clothing"  },
    {   ITEM_FURNITURE, "furniture" },
    {   ITEM_TRASH, "trash"     },
    {   ITEM_CONTAINER, "container" },
    {   ITEM_DRINK_CON, "drink"     },
    {   ITEM_KEY,   "key"       },
    {   ITEM_FOOD,  "food"      },
    {   ITEM_MONEY, "money"     },
    {   ITEM_BOAT,  "boat"      },
    {   ITEM_CORPSE_NPC,"npc_corpse"    },
    {   ITEM_CORPSE_PC, "pc_corpse" },
    {   ITEM_FOUNTAIN,  "fountain"  },
    {   ITEM_TOOL,  "tool"  },
    {   ITEM_CRAFTED,   "crafted"   },
    {   ITEM_PILL,  "pill"      },
    {   ITEM_PROTECT,   "protect"   },
    {   ITEM_MAP,   "map"       },
    {   ITEM_PORTAL,    "portal"    },
    {   ITEM_WARP_STONE,"warp_stone"    },
    {   ITEM_RESOURCE,"resource"    },
    {   ITEM_ROOM_KEY,  "room_key"  },
    {   ITEM_GEM,   "gem"       },
    {   ITEM_JEWELRY,   "jewelry"   },
    {   ITEM_JUKEBOX,   "jukebox"   },
    {   ITEM_TRAP,  "trap"  },
    {   ITEM_NOTEPAPER, "paper" },
    {   0,      NULL        }
};


/* weapon selection table */
const   struct  weapon_type weapon_table    []  =
{
   { "sword",   OBJ_VNUM_SCHOOL_SWORD,  WEAPON_SWORD,   &gsn_sword  },
   { "mace",    OBJ_VNUM_SCHOOL_MACE,   WEAPON_MACE,    &gsn_mace   },
   { "dagger",  OBJ_VNUM_SCHOOL_DAGGER, WEAPON_DAGGER,  &gsn_dagger },
   { "axe", OBJ_VNUM_SCHOOL_AXE,    WEAPON_AXE, &gsn_axe    },
   { "staff",   OBJ_VNUM_SCHOOL_STAFF,  WEAPON_SPEAR,   &gsn_spear  },
   { "flail",   OBJ_VNUM_SCHOOL_FLAIL,  WEAPON_FLAIL,   &gsn_flail  },
   { "whip",    OBJ_VNUM_SCHOOL_WHIP,   WEAPON_WHIP,    &gsn_whip   },
   { "lance",   OBJ_VNUM_SCHOOL_LANCE,  WEAPON_LANCE,   &gsn_lance    },
   { "polearm", OBJ_VNUM_SCHOOL_POLEARM,WEAPON_POLEARM, &gsn_polearm    },
   { NULL,  0,              0,  NULL        }
};



/* wiznet table and prototype for future flag setting */
const   struct wiznet_type      wiznet_table    []              =
{
   {    "on",           WIZ_ON,         IM },
   {    "prefix",   WIZ_PREFIX, IM },
   {    "ticks",        WIZ_TICKS,      IM },
   {    "logins",       WIZ_LOGINS,     IM },
   {    "sites",        WIZ_SITES,      L4 },
   {    "links",        WIZ_LINKS,      L7 },
   {    "newbies",  WIZ_NEWBIE, IM },
   {    "spam",     WIZ_SPAM,   L5 },
   {    "deaths",       WIZ_DEATHS,     IM },
   {    "resets",       WIZ_RESETS,     L4 },
   {    "mobdeaths",    WIZ_MOBDEATHS,  L4 },
   {    "flags",    WIZ_FLAGS,  L5 },
   {    "penalties",    WIZ_PENALTIES,  L5 },
   {    "pkill",   WIZ_PKILL, L7},
   {    "saccing",  WIZ_SACCING,    L5 },
   {    "levels",   WIZ_LEVELS, IM },
   {    "load",     WIZ_LOAD,   L3 },
   {    "restore",  WIZ_RESTORE,    L3 },
   {    "snoops",   WIZ_SNOOPS, L2 },
   {    "switches", WIZ_SWITCHES,   L3 },
   {    "secure",   WIZ_SECURE, L2 },
   {    "file",     WIZ_FILE, L2    },
   {    "bugs",     WIZ_BUGS, L2    },
   {    NULL,       0,      0  }
};

/* attack table  -- not very organized :( */
const   struct attack_type  attack_table    [MAX_DAMAGE_MESSAGE]    =
{
    {   "none",     "hit",      -1      },  /*  0 */
    {   "slice",    "slice",    DAM_SLASH   },
    {   "stab",     "stab",     DAM_PIERCE  },
    {   "slash",    "slash",    DAM_SLASH   },
    {   "whip",     "whip",     DAM_SLASH   },
    {   "claw",     "claw",     DAM_SLASH   },  /*  5 */
    {   "blast",    "blast",    DAM_BASH    },
    {   "pound",    "pound",    DAM_BASH    },
    {   "crush",    "crush",    DAM_BASH    },
    {   "grep",     "grep",     DAM_SLASH   },
    {   "bite",     "bite",     DAM_PIERCE  },  /* 10 */
    {   "pierce",   "pierce",   DAM_PIERCE  },
    {   "suction",  "suction",  DAM_BASH    },
    {   "beating",  "beating",  DAM_BASH    },
    {   "digestion",    "digestion",    DAM_ACID    },
    {   "charge",   "charge",   DAM_BASH    },  /* 15 */
    {   "slap",     "slap",     DAM_BASH    },
    {   "punch",    "punch",    DAM_BASH    },
    {   "wrath",    "wrath",    DAM_ENERGY  },
    {   "magic",    "magic",    DAM_ENERGY  },
    {   "divine",   "divine power", DAM_HOLY    },  /* 20 */
    {   "cleave",   "cleave",   DAM_SLASH   },
    {   "scratch",  "scratch",  DAM_PIERCE  },
    {   "peck",     "peck",     DAM_PIERCE  },
    {   "peckb",    "peck",     DAM_BASH    },
    {   "chop",     "chop",     DAM_SLASH   },  /* 25 */
    {   "sting",    "sting",    DAM_PIERCE  },
    {   "smash",     "smash",   DAM_BASH    },
    {   "shbite",   "shocking bite",DAM_LIGHTNING   },
    {   "flbite",   "flaming bite", DAM_FIRE    },
    {   "frbite",   "freezing bite", DAM_COLD   },  /* 30 */
    {   "acbite",   "acidic bite",  DAM_ACID    },
    {   "chomp",    "chomp",    DAM_PIERCE  },
    {   "drain",    "life drain",   DAM_NEGATIVE    },
    {   "thrust",   "thrust",   DAM_PIERCE  },
    {   "slime",    "slime",    DAM_ACID    }, /* 35 */
    {   "shock",    "shock",    DAM_LIGHTNING   },
    {   "thwack",   "thwack",   DAM_BASH    },
    {   "flame",    "flame",    DAM_FIRE    },
    {   "chill",    "chill",    DAM_COLD    },
    {   "claws",    "claws",    DAM_AGGREVATED }, /*40 */
    {   NULL,       NULL,       0       } /* 41 */
};
/* Language Table*/ /* BROKEN!
const   struct  lang_type       lang_table      [ MAX_LANGUAGE ]  =
{
        { "common"      },
        { "human"       },
        { "dwarvish"    },
        { "elvish"      },
        { "gnomish"     },
        { "goblin"      },
        { "orcish"      },
        { "ogre"        },
        { "drow"        },
        { "kobold"      },
        { "trollish"    },
        { "hobbit"      }
};
                     * /oo
/* race table */
const   struct  race_type   race_table  []      =
{
/*
    {
    name,       pc_race?,
    act bits,   aff_by bits,    off bits,
    imm,        res,        vuln,
    form,       parts
    },
*/
    { "unique",     FALSE, 0, 0, 0, 0, 0, 0, 0, 0 },

    {
      "dhampire",         TRUE,
      0,      0,      0,
      IMM_SUNLIGHT,       RES_POISON|RES_BASH,    VULN_FIRE,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "garou",            TRUE,
      0,      AFF_INFRARED,   0,
      IMM_SUNLIGHT|IMM_POISON,        0,      VULN_NEGATIVE,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "ghoul",        TRUE,
      0,      0,      0,
      IMM_SUNLIGHT,       RES_POISON,     VULN_FIRE|VULN_CHARM|VULN_MENTAL,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "human",        TRUE,
      0,      0,      0,
      IMM_SUNLIGHT,       0,      VULN_MENTAL,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "methuselah",           TRUE,
      0,      0,  0,
      IMM_DROWNING,       RES_CHARM|RES_POISON|RES_BASH,  VULN_FIRE|VULN_SUNLIGHT|VULN_HOLY,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "vampire",          TRUE,
      0,      0,  0,
      IMM_DROWNING,       RES_CHARM|RES_POISON|RES_BASH,  VULN_FIRE|VULN_SUNLIGHT|VULN_HOLY,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

//Mob-only Races Starting Here
    {
      "abomination",        FALSE,
      0,      AFF_BERSERK,      0,
      IMM_MENTAL,      RES_WEAPON|RES_DROWNING,      VULN_MAGIC,
      0,      0
    },

    {
      "amphibian",        FALSE,
      0,      AFF_SWIM,      0,
      0,      RES_POISON|RES_DROWNING,      VULN_COLD|VULN_LIGHTNING|VULN_ACID,
      FORM_EDIBLE|FORM_AMPHIBIAN|FORM_COLD_BLOOD,      PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EYE|PART_LONG_TONGUE
    },

    {
      "ape",          FALSE,
      0,      0,      0,
      0,      0,      0,
      FORM_EDIBLE|FORM_ANIMAL|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,   PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE
    },

    {
      "bane",         FALSE,
      0,      0,  OFF_DODGE,
      IMM_POISON|IMM_DISEASE|IMM_DROWNING|IMM_SUNLIGHT,   RES_WEAPON|RES_COLD|RES_ACID|RES_FIRE,  VULN_NEGATIVE|VULN_HOLY|VULN_ENERGY,
      FORM_SENTIENT,    0
    },

    {
      "bat",          FALSE,
      0,      AFF_FLYING|AFF_DARK_VISION, OFF_DODGE|OFF_FAST,
      0,      0,      VULN_LIGHT,
      A|G|V,      A|C|D|E|F|H|J|K|P
    },

    {
      "bear",         FALSE,
      0,      0,      OFF_CRUSH|OFF_DISARM|OFF_BERSERK,
      0,      RES_BASH|RES_COLD,  0,
      A|G|V,      A|B|C|D|E|F|H|J|K|U|V
    },

    {
      "bygone",        FALSE,
      0,      AFF_DETECT_HIDDEN|AFF_DARK_VISION|AFF_REGENERATION,      0,
      IMM_DISEASE|IMM_SUNLIGHT,      RES_MENTAL|RES_CHARM|RES_SUMMON|RES_MAGIC|RES_WEAPON|RES_MAGIC|RES_POISON,      VULN_FIRE|VULN_COLD|VULN_LIGHTNING|VULN_ACID|VULN_ENERGY,
      FORM_MAGICAL|FORM_ANIMAL|FORM_SENTIENT,      PART_HEAD|PART_HEART|PART_BRAINS|PART_GUTS|PART_EYE|PART_TAIL|PART_LEGS
    },

    {
      "cat",          FALSE,
      0,      AFF_DARK_VISION,    OFF_FAST|OFF_DODGE,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K|Q|U|V
    },

    {
      "centipede",        FALSE,
      0,      AFF_DARK_VISION,    0,
      0,      RES_PIERCE|RES_COLD,    VULN_BASH,
      A|B|G|O,        A|C|K
    },

    {
      "construct",        FALSE,
      0,      0,      OFF_PARRY|OFF_PARRY,
      IMM_COLD|IMM_POISON|IMM_NEGATIVE|IMM_MENTAL|IMM_DISEASE|IMM_SUNLIGHT
      |IMM_DROWNING|IMM_CHARM,    RES_LIGHT|RES_SLASH|RES_PIERCE,
      VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
      C|J,    A|B|C|G|H
    },

    {
      "demon",        FALSE,
      0,      AFF_DARK_VISION,      0,
      IMM_POISON|IMM_DISEASE|IMM_NEGATIVE,      RES_CHARM,      VULN_SUMMON|VULN_HOLY|VULN_SUNLIGHT,
      FORM_SENTIENT|FORM_BIPED,      PART_HEAD|PART_ARMS|PART_HEART| PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE|PART_CLAWS|PART_TAIL|PART_FANGS|PART_HORNS
    },

    {
      "dog",          FALSE,
      0,      0,      OFF_FAST,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K|U|V
    },

    {
      "dragon",       FALSE,
      0,          AFF_INFRARED|AFF_FLYING,    0,
      0,          RES_FIRE|RES_BASH|RES_CHARM,  VULN_PIERCE|VULN_COLD,
      A|H|Z,      A|C|D|E|F|G|H|I|J|K|P|Q|U|V|X
    },

    {
      "elemental",        FALSE,
      0,      0,      0,
      0,      0,      0,
      FORM_INSTANT_DECAY|FORM_SENTIENT,      0
    },

    {
      "faerie",        FALSE,
      0,      0,      0,
      IMM_SUNLIGHT,      RES_CHARM|RES_MAGIC,      VULN_IRON,
      FORM_EDIBLE|FORM_MAGICAL|FORM_SENTIENT|FORM_BIPED,      PART_HEAD|PART_ARMS|PART_HEART| PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE
    },

    {
      "fera",         FALSE,
      0,      AFF_BERSERK|AFF_REGENERATION,   OFF_DODGE|OFF_FAST,
      IMM_SUNLIGHT,   RES_POISON|RES_DISEASE,     VULN_SILVER,
      A|C|H|M,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "fish",     FALSE,
      0,      0,      0,
      IMM_DROWNING,   RES_MENTAL, VULN_FIRE|VULN_LIGHTNING,
      A|G|X|cc,   A|C|D|E|F|H|K|Q|V
    },

    {
      "fomor",      FALSE,
      0,      0,   0,
      0,  RES_CHARM|RES_POISON|RES_NEGATIVE|RES_MENTAL|RES_DISEASE,  VULN_HOLY,
      FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,      PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE
    },

    {
      "fox",          FALSE,
      0,      AFF_DARK_VISION,    OFF_FAST|OFF_DODGE,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K|Q|V
    },

    {
       "horse",                FALSE,
        0,              0,   0,
        0,              RES_POISON,     VULN_CHARM,
        A|M|V,        A|C|D|E|F|J|K|Q
    },

    {
      "insect",        FALSE,
      0,      0,      0,
      IMM_MENTAL,      RES_POISON|RES_SUNLIGHT|RES_DROWNING,      VULN_FIRE|VULN_COLD|VULN_ACID,
      FORM_EDIBLE|FORM_INSECT,      PART_HEAD|PART_LEGS|PART_GUTS|PART_EYE
    },

    {
    "kinfolk",        FALSE,
      0,      0,      0,
      0,      RES_CHARM|RES_MENTAL,   0,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "kuei-jin",     FALSE,
      0,              0,   0,
      0,              RES_POISON,     VULN_FIRE|VULN_SUNLIGHT,
      FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL|FORM_UNDEAD,    PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE
    },

    {
      "lizard",       FALSE,
      0,      0,      0,
      0,      RES_POISON, VULN_COLD,
      A|G|X|cc,   A|C|D|E|F|H|K|Q|V
    },

    {
      "mage",        FALSE,
      0,      0,      0,
      IMM_SUNLIGHT,       RES_POISON|RES_NEGATIVE,      VULN_MENTAL,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "marine mammal",         FALSE,
      0,      AFF_SWIM,      0,
      0,      RES_COLD|RES_DROWNING,          VULN_LIGHTNING,
      FORM_EDIBLE|FORM_ANIMAL|FORM_SENTIENT|FORM_MAMMAL,    PART_HEAD|PART_HEART|PART_BRAINS|PART_GUTS|PART_EYE
    },

    {
      "monkey",        FALSE,
      0,      0,      0,
      0,      0,      0,
      FORM_EDIBLE|FORM_ANIMAL|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,   PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE|PART_TAIL
    },

    {
      "ooze",        FALSE,
      0,      AFF_BLIND|AFF_SLOW,      0,
      IMM_LIGHT|IMM_MENTAL|IMM_DROWNING,      RES_WEAPON,      VULN_MAGIC|VULN_FIRE,
      FORM_BLOB,      0
    },

    {
      "pig",          FALSE,
      0,      0,      0,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K
    },

    {
      "rabbit",       FALSE,
      0,      0,      OFF_DODGE|OFF_FAST,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K
    },

    {
      "rodent",       FALSE,
      0,      0,      OFF_DODGE|OFF_FAST,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|H|J|K
    },

    {
      "romani",       FALSE,
      0,      0,      OFF_KICK_DIRT|ASSIST_RACE,
      0,              RES_MAGIC,      VULN_DROWNING,
      FORM_EDIBLE|FORM_ANIMAL|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "shadow",        FALSE,
      0,      AFF_HIDE|AFF_SNEAK|AFF_PASS_DOOR|AFF_DARK_VISION,      0,
      IMM_POISON|IMM_DISEASE|IMM_NEGATIVE|IMM_DROWNING,      0,      VULN_SUNLIGHT|VULN_LIGHT,
      FORM_INSTANT_DECAY|FORM_INTANGIBLE,      0
    },

    {
      "snake",        FALSE,
      0,      0,      0,
      0,      RES_POISON, VULN_COLD,
      A|G|X|Y|cc, A|D|E|F|K|L|Q|V|X
    },

    {
      "song bird",        FALSE,
      0,      AFF_FLYING,     OFF_FAST|OFF_DODGE,
      0,      0,      0,
      A|G|W,      A|C|D|E|F|H|K|P
    },

    {
      "spider",       FALSE,
      0,      0,      0,
      0,      RES_POISON, VULN_COLD,
      A|G|X|cc,   A|C|D|E|F|H|K|Q|V
    },

    {
      "spirit",           FALSE,
      0,      0,  OFF_DODGE,
      IMM_POISON|IMM_DISEASE|IMM_DROWNING|IMM_SUNLIGHT,   RES_WEAPON|RES_COLD|RES_ACID|RES_FIRE,  VULN_NEGATIVE|VULN_HOLY|VULN_ENERGY,
      FORM_SENTIENT,    0
    },

    {
      "undead",           FALSE,
      0,      0,  OFF_DODGE,
      0,      RES_COLD,       VULN_HOLY,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "water fowl",       FALSE,
      0,      AFF_SWIM|AFF_FLYING,    0,
      0,      RES_DROWNING,       0,
      A|G|W,      A|C|D|E|F|H|K|P
    },

    {
      "wolf",         FALSE,
      0,      AFF_DARK_VISION,    OFF_FAST|OFF_DODGE,
      0,      0,      0,
      A|G|V,      A|C|D|E|F|J|K|Q|V
    },

    {
      "vegetation",        FALSE,
      0,      0,      0,
      IMM_SUNLIGHT|IMM_DROWNING|IMM_POISON,      RES_BASH|RES_LIGHT|RES_WOOD|RES_MENTAL,      VULN_COLD|VULN_SLASH|VULN_FIRE,
      FORM_EDIBLE,      0
    },

    {
      "wraith",       FALSE,
      0,      AFF_SNEAK|AFF_PASS_DOOR|AFF_DARK_VISION, OFF_FADE,
      IMM_POISON|IMM_DISEASE|IMM_DROWNING|IMM_SUNLIGHT,   RES_WEAPON|RES_COLD|RES_ACID|RES_FIRE,
      VULN_NEGATIVE|VULN_HOLY|VULN_ENERGY,
      C|D|H|I,    0
    },

//Unused/Replaceable Mob Races Here
    {
      "placeholder",          FALSE,
      0,      0,      0,
      0,      0,      0,
      0,      0
    },

    {
      "goblin",       FALSE,
      0,      AFF_INFRARED,   0,
      0,      RES_DISEASE,    VULN_MAGIC,
      A|H|M|V,    A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "hobgoblin",        FALSE,
      0,      AFF_INFRARED,   0,
      0,      RES_DISEASE|RES_POISON, 0,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K|Y
    },

    {
      "modron",       FALSE,
      0,      AFF_INFRARED,       ASSIST_RACE|ASSIST_ALIGN,
      IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE,
        RES_FIRE|RES_COLD|RES_ACID, 0,
        H,      A|B|C|G|H|J|K
    },

    {
      "svirfnebli",           FALSE,
      0,              AFF_INFRARED,   0,
      0,              RES_MAGIC|RES_POISON|RES_DISEASE, VULN_BASH,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "HIT Mark",             FALSE,
      0,              AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_DARK_VISION, OFF_FAST,
      IMM_MENTAL|IMM_CHARM,       RES_PIERCE|RES_BASH|RES_SLASH|RES_WEAPON, VULN_LIGHTNING,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "MIB",              FALSE,
      0,              AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_DARK_VISION|AFF_SNEAK,   OFF_FAST,
      IMM_CHARM|IMM_NEGATIVE,     0, VULN_HOLY,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "Marauder",             FALSE,
      ACT_AGGRESSIVE,             AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_DARK_VISION,
      IMM_CHARM,      RES_MAGIC, VULN_MENTAL,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "Nephandi",             FALSE,
      0,              AFF_DETECT_INVIS|AFF_DETECT_HIDDEN|AFF_DARK_VISION,
      IMM_MENTAL|IMM_NEGATIVE,        RES_WEAPON|RES_MAGIC, VULN_HOLY,
      A|H|M|V,        A|B|C|D|E|F|G|H|I|J|K
    },

    {
      "troll",        FALSE,
      0,      AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN,      OFF_BERSERK,
      0,  RES_CHARM|RES_BASH, VULN_FIRE|VULN_ACID,
      A|B|H|M|V,      A|B|C|D|E|F|G|H|I|J|K|U|V
    },


/*    {
    "unique",       FALSE,
    0,      0,      0,
    0,      0,      0,
    0,      0
    },
*/

    {
    NULL, 0, 0, 0, 0, 0, 0
    }
};

/*
 * Class table.
 */
const   struct  class_type  class_table [MAX_CLASS] =
{             //Name     whoname    primeattr       begin weapon         guild rooms  max prac tha0 tha0 hpm hpx  mana     skills          groups
 /*0*/    {    "none", " ",      STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*1*/    {    "cleric", " ",    STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*2*/    {    "thief", " ",     STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*3*/    {    "warrior", " ",   STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*4*/    {    "bandit", "",     STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*5*/    {    "assassin", "",   STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*6*/    {    "trickster", "",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*7*/    {    "mercenary", "",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*8*/    {    "guardsman", "",  STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*9*/    {    "knight", "",     STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*10*/   {    "priest", "",     STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*11*/   {    "monk", "",       STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
 /*12*/   {    "crusader", "",   STAT_INT,  OBJ_VNUM_SCHOOL_DAGGER,   { 3003, 9619 },  20,  20,  -15,  20,  25,  TRUE,   "rom basic", "rom basic"},
};


/*
 * Attribute bonus tables.
 */
const   struct  str_app_type    str_app     [26]        =
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  7 },
    {  0,  0, 100,  8 },
    {  0,  0, 100,  9 },
    {  0,  0, 115, 10 }, /* 10  */
    {  0,  0, 115, 11 },
    {  0,  0, 130, 12 },
    {  0,  0, 130, 13 }, /* 13  */
    {  0,  1, 140, 14 },
    {  1,  1, 150, 15 }, /* 15  */
    {  1,  2, 165, 16 },
    {  2,  3, 180, 22 },
    {  2,  3, 200, 25 }, /* 18  */
    {  3,  4, 225, 30 },
    {  3,  5, 250, 35 }, /* 20  */
    {  4,  6, 300, 40 },
    {  4,  6, 350, 45 },
    {  5,  7, 400, 50 },
    {  5,  8, 450, 55 },
    {  6,  9, 500, 60 }  /* 25   */
};



const   struct  int_app_type    int_app     [26]        =
{
    {  3 }, /*  0 */
    {  5 }, /*  1 */
    {  7 },
    {  8 }, /*  3 */
    {  9 },
    { 10 }, /*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 }, /* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 }, /* 15 */
    { 34 },
    { 37 },
    { 40 }, /* 18 */
    { 44 },
    { 49 }, /* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 80 },
    { 85 }  /* 25 */
};



const   struct  wis_app_type    wis_app     [26]        =
{
    { 0 },  /*  0 */
    { 0 },  /*  1 */
    { 0 },
    { 0 },  /*  3 */
    { 0 },
    { 1 },  /*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 1 },  /* 10 */
    { 1 },
    { 1 },
    { 1 },
    { 1 },
    { 2 },  /* 15 */
    { 2 },
    { 2 },
    { 3 },  /* 18 */
    { 3 },
    { 3 },  /* 20 */
    { 3 },
    { 4 },
    { 4 },
    { 4 },
    { 5 }   /* 25 */
};



const   struct  dex_app_type    dex_app     [26]        =
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};


const   struct  con_app_type    con_app     [26]        =
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },   /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, -1 },   /* 18 */
    {  3, -1 },
    {  4, -1 },   /* 20 */
    {  4, -1 },
    {  5, -1 },
    {  6, -1 },
    {  7, -1 },
    {  8, -1 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in world.obj.
 */
const   struct  liq_type    liq_table   []  =
{
/*    name          color   proof, full, thirst, food, ssize */
    { "water",          "clear",    {   0, 1, 10, 0, 16 }   },
    { "beer",           "amber",    {  12, 1,  8, 1, 12 }   },
    { "red wine",       "burgundy", {  30, 1,  8, 1,  5 }   },
    { "ale",            "brown",    {  15, 1,  8, 1, 12 }   },
    { "dark ale",       "dark",     {  16, 1,  8, 1, 12 }   },

    { "whisky",         "golden",   { 120, 1,  5, 0,  2 }   },
    { "lemonade",       "pink",     {   0, 1,  9, 2, 12 }   },
    { "firebreather",       "boiling",  { 190, 0,  4, 0,  2 }   },
    { "local specialty",    "clear",    { 151, 1,  3, 0,  2 }   },
    { "slime mold juice",   "green",    {   0, 2, -8, 1,  2 }   },

    { "milk",           "white",    {   0, 2,  9, 3, 12 }   },
    { "tea",            "tan",      {   0, 1,  8, 0,  6 }   },
    { "coffee",         "black",    {   0, 1,  8, 0,  6 }   },
    { "blood",          "red",      {   0, 2, -1, 2,  6 }   },
    { "salt water",     "clear",    {   0, 1, -2, 0,  1 }   },

    { "coke",           "brown",    {   0, 2,  9, 2, 12 }   },
    { "root beer",      "brown",    {   0, 2,  9, 2, 12 }   },
    { "elvish wine",        "green",    {  35, 2,  8, 1,  5 }   },
    { "white wine",     "golden",   {  28, 1,  8, 1,  5 }   },
    { "champagne",      "golden",   {  32, 1,  8, 1,  5 }   },

    { "mead",           "honey-colored",{  34, 2,  8, 2, 12 }   },
    { "rose wine",      "pink",     {  26, 1,  8, 1,  5 }   },
    { "benedictine wine",   "burgundy", {  40, 1,  8, 1,  5 }   },
    { "vodka",          "clear",    { 130, 1,  5, 0,  2 }   },
    { "cranberry juice",    "red",      {   0, 1,  9, 2, 12 }   },

    { "orange juice",       "orange",   {   0, 2,  9, 3, 12 }   },
    { "absinthe",       "green",    { 200, 1,  4, 0,  2 }   },
    { "brandy",         "golden",   {  80, 1,  5, 0,  4 }   },
    { "aquavit",        "clear",    { 140, 1,  5, 0,  2 }   },
    { "schnapps",       "clear",    {  90, 1,  5, 0,  2 }   },

    { "icewine",        "purple",   {  50, 2,  6, 1,  5 }   },
    { "amontillado",        "burgundy", {  35, 2,  8, 1,  5 }   },
    { "sherry",         "red",      {  38, 2,  7, 1,  5 }   },
    { "framboise",      "red",      {  50, 1,  7, 1,  5 }   },
    { "rum",            "amber",    { 151, 1,  4, 0,  2 }   },

    { "cordial",        "clear",    { 100, 1,  5, 0,  2 }   },
    { "muscus-like slush",  "greenish-white",   { 0, 1,  10, 1,  16 }   },
    { NULL,         NULL,       {   0, 0,  0, 0,  0 }   }
};


const   struct  tool_type   tool_table  []  =
{
    { "hammer"  },
    { "knife"  },
    { "chisel"  },
    { "plane"  },
    { "hoe"  },
    { "pitchfork"  },
    { "saw"  },
    { "rake"  },
    { "net"  },
    { "pestle and mortar" },
    { "grindstone" },
    { "loom" },
    { "furnace" },
    { "pickaxe" },
    { "anvil" },
    { "bench" },
    { NULL  }
};

const   struct  crafted_type    crafted_table   []  =
{
    { "decorative"  },
    { "equipment"   },
    { "relic"   },
    { NULL  }
};

#define ATTR 0
#define ABIL 1
#define SECA 2

const struct cs_skill_type csskill_table [] =
{
//Weapon Skills and Hand to Hand
    {"axe", &gsn_axe,  1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"dagger", &gsn_dagger, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"flail", &gsn_flail, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"lance", &gsn_lance, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"mace", &gsn_mace, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"polearm", &gsn_polearm, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"spear", &gsn_spear, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"sword", &gsn_sword, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"whip", &gsn_whip, 1,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"hand to hand", &gsn_hand_to_hand,  1,
    {CSABIL_BRAWL, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

//Skills
    //    {"alertness", &gsn_alertness, 0,
    //    {CSABIL_ALERTNESS, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"assassinate", &gsn_assassinate, 50,
    {CSABIL_MELEE, 5, ABIL}, {CSABIL_MEDICINE, 3, ABIL} },

    {"backstab", &gsn_backstab, 10,
    {CSABIL_MELEE, 3, ABIL}, {CSABIL_STEALTH, 1, ABIL} },

    {"bandage", &gsn_bandage, 1,
    {CSABIL_MEDICINE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"bash", &gsn_bash, 10,
    {CSABIL_MELEE, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"berserk", &gsn_berserk, 40,
    {CSABIL_INTIMIDATION, 3, ABIL}, {CSABIL_HEARTHWISDOM, 1, ABIL} },

    {"blast", &gsn_blast, 40,
    {CSABIL_LEGERDEMAIN, 4, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"block", &gsn_block, 5,
    {CSABIL_BRAWL, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"brew", &gsn_brew, 35,
    {CSABIL_HEARTHWISDOM, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"chant", &gsn_chant, 15,
    {CSABIL_NONE, 0, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"classify", &gsn_classify, 15,
    {CSABIL_ANIMAL_KEN, 1, ABIL}, {CSABIL_NONE, 0, ABIL}},

    {"critical strike", &gsn_critical_strike, 70,
    {CSABIL_MEDICINE, 3, ABIL}, {CSABIL_MELEE, 2, ABIL} },

    {"dirt kick", &gsn_dirt, 5,
    {CSABIL_BRAWL, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"disarm", &gsn_disarm,30,
    {CSABIL_MELEE, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"divine strength", &gsn_divine_strength, 55,
    {CSABIL_THEOLOGY, 4, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"dodge", &gsn_dodge, 1,
    {CSABIL_DODGE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"enhanced damage", &gsn_enhanced_damage, 55,
    {STRENGTH, 5, ATTR}, {CSABIL_NONE, 0, ABIL} },

    {"enhanced speed", &gsn_enhanced_speed, 55,
    {DEXTERITY, 5, ATTR}, {CSABIL_NONE, 0, ABIL} },

    {"envenom", &gsn_envenom, 45,
    {CSABIL_HEARTHWISDOM, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"fortress", &gsn_fortress, 60,
    {CSABIL_CRAFTS, 3, ABIL}, {CSABIL_SURVIVAL, 1, ABIL} },

    {"glower", &gsn_glower, 50,
    {CSABIL_INTIMIDATION, 3, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"gouge", &gsn_gouge, 25,
    {CSABIL_BRAWL, 5, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"ground", &gsn_ground, 30,
    {CSABIL_BRAWL, 3, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"haggle", &gsn_haggle, 20,
    {CSABIL_COMMERCE, 2, ABIL}, {CSABIL_SUBTERFUGE, 1, ABIL} },

    {"headbutt", &gsn_headbutt, 50,
    {CSABIL_BRAWL, 4, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"hide", &gsn_hide, 20,
    {CSABIL_STEALTH, 3, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"kick", &gsn_kick, 1,
    {CSABIL_BRAWL, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"knights training", &gsn_knight_training, 65,
    {CSABIL_MELEE, 5, ABIL}, {CSABIL_ETIQUETTE, 1, ABIL} },

    {"lore", &gsn_lore, 15,
    {CSABIL_HEARTHWISDOM, 2, ABIL}, {CSABIL_ACADEMICS, 1, ABIL} },

    {"meditation", &gsn_meditation, 5,
    {CSABIL_MEDITATION, 1, SECA}, {CSABIL_NONE, 0, ABIL} },

    {"parry", &gsn_parry, 15,
    {CSABIL_MELEE, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"peek", &gsn_peek, 15,
    {CSABIL_STEALTH, 2, ABIL}, {CSABIL_LEGERDEMAIN, 1, ABIL} },

    {"pick lock", &gsn_pick_lock, 20,
    {CSABIL_LEGERDEMAIN, 3, ABIL}, {CSABIL_ACADEMICS, 1, ABIL} },

    {"renown", &gsn_renown, 75,
    {CSABIL_SURVIVAL, 3, ABIL}, {CSABIL_ATHLETICS, 1, ABIL} },

    {"rescue", &gsn_rescue, 25,
    {CSABIL_BRAWL, 2, ABIL}, {CSABIL_LEADERSHIP, 1, ABIL} },

    {"riding", &gsn_riding, 30,
    {CSABIL_RIDING, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"scrolls", &gsn_scrolls, 5,
    {CSABIL_OCCULT, 2, ABIL}, {CSABIL_ACADEMICS, 1, ABIL} },

    {"second attack", &gsn_second_attack, 30,
    {DEXTERITY, 2, ATTR}, {CSABIL_NONE, 0, ABIL} },

    {"shield block", &gsn_shield_block, 25,
    {CSABIL_MELEE, 2, ABIL}, {CSABIL_ATHLETICS, 1, ABIL} },

    {"skin", &gsn_skin, 20,
    {CSABIL_SURVIVAL, 2, ABIL}, {CSABIL_CRAFTS, 1, ABIL} },

    {"slip", &gsn_slip,70,
    {CSABIL_STEALTH, 4, ABIL}, {CSABIL_LEGERDEMAIN, 2, ABIL} },

    {"sneak", &gsn_sneak,10,
    {CSABIL_STEALTH, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"staves", &gsn_staves, 5,
    {CSABIL_THEOLOGY, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"steal", &gsn_steal, 45,
    {CSABIL_LEGERDEMAIN, 4, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"study", &gsn_study, 25,
    {CSABIL_HEARTHWISDOM, 2, ABIL}, {CSABIL_OCCULT, 1, ABIL} },

    {"third attack", &gsn_third_attack, 60,
    {DEXTERITY, 4, ATTR}, {CSABIL_NONE, 0, ABIL} },

    {"trip", &gsn_trip, 30,
    {CSABIL_BRAWL, 2, ABIL}, {CSABIL_ATHLETICS, 1, ABIL} },

    {"vigor", &gsn_vigor,20,
    {CSABIL_ATHLETICS, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"wands", &gsn_wands, 5,
    {CSABIL_HEARTHWISDOM, 2, ABIL}, {CSABIL_OCCULT, 1, ABIL} },

    {"warcry", &gsn_warcry, 65,
    {CSABIL_INTIMIDATION, 4, ABIL}, {CSABIL_LEADERSHIP, 2, ABIL} },

    {"waylay", &gsn_waylay, 55,
    {CSABIL_STEALTH, 3, ABIL}, {CSABIL_MELEE, 1, ABIL} },

//Prayers
    {"blindness", &gsn_blindness, 45,
    {CSABIL_OCCULT, 3, ABIL}, {CSABIL_HEARTHWISDOM, 1, ABIL} },

    {"call lightning", &gsn_call_lightning, 40,
    {CSABIL_THEOLOGY, 3, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"cause light", &gsn_cause_light, 10,
    {CSABIL_OCCULT, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"create food", &gsn_create_food, 5,
    {CSABIL_HEARTHWISDOM, 3, ABIL}, {CSABIL_SURVIVAL, 1, ABIL} },

    {"create spring", &gsn_create_spring, 30,
    {CSABIL_HEARTHWISDOM, 4, ABIL}, {CSABIL_SURVIVAL, 2, ABIL} },

    {"create water", &gsn_create_water, 20,
    {CSABIL_THEOLOGY, 1, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"cure blindness", &gsn_cure_blindness, 25,
    {CSABIL_THEOLOGY, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"cure poison", &gsn_cure_poison, 15,
    {CSABIL_HEARTHWISDOM, 1, ABIL}, {CSABIL_OCCULT, 1, ABIL} },

    {"curse", &gsn_curse, 60,
    {CSABIL_OCCULT, 4, ABIL}, {CSABIL_HEARTHWISDOM, 2, ABIL} },

    {"dispel", &gsn_dispel, 75,
    {CSABIL_OCCULT, 5, ABIL}, {CSABIL_ACADEMICS, 2, ABIL} },

    {"earthquake", &gsn_earthquake, 45,
    {CSABIL_HEARTHWISDOM, 5, ABIL}, {CSABIL_OCCULT, 2, ABIL} },

    {"lightning bolt", &gsn_lightning_bolt, 35,
    {CSABIL_OCCULT, 2, ABIL}, {CSABIL_NONE, 0, ABIL} },

    {"ray of truth", &gsn_ray_of_truth, 60,
    {CSABIL_THEOLOGY, 5, ABIL}, {CSABIL_LINGUISTICS, 2, ABIL} },

    {"remove curse", &gsn_remove_curse, 40,
    {CSABIL_THEOLOGY, 4, ABIL}, {CSABIL_HEARTHWISDOM, 1, ABIL} },

    {"zeal", &gsn_zeal, 15,
    {CSABIL_THEOLOGY, 3, ABIL}, {CSABIL_EXPRESSION, 3, ABIL} },

    { NULL, 0, 0, {-1, 0, ABIL}, {-1, 0, ABIL}}


};
int cskill_lookup (int gsn)
{
    int i;
    for (i = 0; csskill_table[i].name != NULL; i++)
    {
    if (*csskill_table[i].gsn == gsn)
    return i;
    }
    return -1;
}
#define STAT 0
#define DOT 1
#define TYPE 2
void cskill_update(CHAR_DATA *ch, bool show)
{

    int i, c;
    bool primary, secondary;
    primary = FALSE;
    secondary = FALSE;

    for (i = 0; csskill_table[i].name != NULL; i++)
    {
        primary = FALSE;
        secondary = FALSE;
        switch (csskill_table[i].primary[TYPE])
        {
            case ABIL:
                if (ch->csabilities[csskill_table[i].primary[STAT]] >= csskill_table[i].primary[DOT])
                    primary = TRUE;
                break;
            case ATTR:
                if (ch->csattributes[csskill_table[i].primary[STAT]] >= csskill_table[i].primary[DOT])
                    primary = TRUE;
                break;
            case SECA:
                if (ch->pcdata->cssec_abil[csskill_table[i].primary[STAT]] >= csskill_table[i].primary[DOT])
                    primary = TRUE;
                break;
            default:
                bug("csskill_update: Invalid primary cs type.", 0);
                return;
        }

        switch (csskill_table[i].secondary[TYPE])
        {
            case ABIL:
                if (csskill_table[i].secondary[STAT] == CSABIL_NONE ||
                (ch->csabilities[csskill_table[i].secondary[STAT]] >= 1 &&
                ch->csabilities[csskill_table[i].secondary[STAT]] >= csskill_table[i].secondary[DOT]))
                    secondary = TRUE;
                break;
            case ATTR:
                if (csskill_table[i].secondary[STAT] == CSABIL_NONE ||
                (ch->csattributes[csskill_table[i].secondary[STAT]] >= 1 &&
                ch->csattributes[csskill_table[i].secondary[STAT]] >= csskill_table[i].secondary[DOT]))                    secondary = TRUE;
                break;
            case SECA:
                if (csskill_table[i].secondary[STAT] == CSABIL_NONE ||
                (ch->pcdata->cssec_abil[csskill_table[i].secondary[STAT]] >= 1 &&
                ch->pcdata->cssec_abil[csskill_table[i].secondary[STAT]] >= csskill_table[i].secondary[DOT]))                    secondary = TRUE;
                break;
            default:
                bug("csskill_update: Invalid secondary cs type.", 0);
                return;
        }



        if (primary == FALSE || secondary == FALSE)
        {
            if (ch->pcdata->learned[*csskill_table[i].gsn] > 0)
            {
                ch->pcdata->learned[*csskill_table[i].gsn] = 0;
                if (show)
                    cprintf(ch, "Losing the %s skill.\n\r", csskill_table[i].name);
            }
        }
            else
            {
            if (ch->pcdata->learned[*csskill_table[i].gsn] < 10)
            {
                ch->pcdata->learned[*csskill_table[i].gsn] = 50;
                if (show)
                    cprintf(ch, "Learning the %s skill!\n\r", csskill_table[i].name);
                }
            }


        for (c = 0; c < MAX_SKILL; c++)
        {
            if (cskill_lookup(c) == -1)
                ch->pcdata->learned[c] == 0;
        }
    }

}

/*
 * The skill and spell table.
 * Slot numbers must never be changed as they appear in #OBJECTS sections.
 */
#define SLOT(n) n

const   struct  skill_type  skill_table [MAX_SKILL] =
{
// TOTAL SKILLS AS OF 2/17/21 =  428
// MAX_SKILL INCREASED TO 435
/*
 * Magic spells.
 */
//Total 108
    { "reserved", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, 0, TAR_IGNORE, POS_STANDING,
    NULL, SLOT( 0),    0,  0,    "",         "",     ""    },

    { "acid blast", { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_acid_blast, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL, SLOT(70),   20, 12, "acid blast",       "!Acid Blast!"    },

    { "armor", {  7,  2, 10, 5, 102, 102, 102, 102, 102, 102, 11, 20, 10},
    { 1,  1,  2,  2 , 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_armor, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT( 1),    5, 12,    "",         "You feel less armored.",   ""    },

    { "bless", { 102,  7, 102, 8, 102, 102, 102, 102, 102, 102, 68, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_bless, TAR_OBJ_CHAR_DEF,   POS_STANDING,
    NULL,           SLOT( 3),    5, 12,    "",   "You feel less righteous.",    "$p's holy aura fades."    },

    { "blindness",        {  12,  8, 17, 15, 102, 102, 102, 102, 102, 102, 31, 35, 102},
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_blindness, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_blindness,     SLOT( 4),    5, 12,   "",         "You can see again.",   ""    },

    { "burning hands", {  7, 102, 10, 9, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_burning_hands, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT( 5),   15, 12, "burning hands",    "!Burning Hands!",  "" },

    { "call lightning",   { 26, 18, 31, 22, 102, 102, 102, 102, 102, 102, 39, 102, 30 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_call_lightning,   TAR_IGNORE,     POS_FIGHTING,
    &gsn_call_lightning,           SLOT( 6),   15, 12, "lightning bolt",   "!Call Lightning!", "" },

    { "calm",         { 48, 16, 50, 20 , 102, 102, 102, 102, 102, 102, 21, 25, 16},
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_calm,     TAR_IGNORE,     POS_FIGHTING,
    NULL,           SLOT(509),  30, 12, "",         "You have lost your peace of mind.",    "" },

    { "cancellation",     { 18, 26, 34, 34, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cancellation, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(507),  20, 12, ""          "!cancellation!",   "" },

    { "cause critical",   { 102,  13, 102, 19, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cause_critical,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(63),   20, 12, "spell",        "!Cause Critical!", "" },

    { "cause light",      { 102,  1, 102, 3, 102, 102, 102, 102, 102, 102, 1, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cause_light,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_cause_light,           SLOT(62),   15, 12, "spell",        "!Cause Light!",    "" },

    { "cause serious",    { 102,  7, 102, 10, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cause_serious,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(64),   17, 12, "spell",        "!Cause Serious!",  "" },

    { "chain lightning",  { 33, 102, 39, 36, 102, 102, 102, 102, 102, 102, 37, 102, 30 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_chain_lightning,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(500),  25, 12, "lightning",        "!Chain Lightning!",    "" },

    { "change sex",       { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_change_sex,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(82),   15, 12, "",         "Your body feels familiar again.",  "" },

    { "change alignment",     { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_change_alignment, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(45),   15, 12, "",         "Your aura returns to normal.", "" },

    { "charm person",     { 20, 102, 25, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_charm_person, TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_charm_person,  SLOT( 7),    5, 12, "",         "You feel more self-confident.",    "" },

    { "chill touch",      {  4, 102, 6, 6, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_chill_touch,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT( 8),   15, 12, "chilling touch",   "You feel less cold.",  "" },

    { "colour spray",     { 16, 102, 22, 20, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_colour_spray, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(10),   15, 12, "colour spray",     "!Colour Spray!",   "" },

    { "continual light",  {  6,  4, 6, 9, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_continual_light,  TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(57),    7, 12, "",         "!Continual Light!",    "" },

    { "control weather",  { 15, 19, 28, 22, 102, 102, 102, 102, 102, 102, 15, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_control_weather,  TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(11),   25, 12, "",         "!Control Weather!",    "" },

    { "create food",      { 10, 5, 11, 12, 102, 102, 102, 102, 102, 102, 1, 1, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_create_food,  TAR_IGNORE,     POS_STANDING,
    &gsn_create_food,           SLOT(12),    5, 12, "",         "!Create Food!",    "" },

    { "create rose",      { 16, 11, 10, 24, 102, 102, 102, 102, 102, 102, 29, 102, 102 },
    { 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 2, 2, 2 }, spell_create_rose,  TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(511),  30,     12, "",         "!Create Rose!",    "" },

    { "create spring",    { 14, 17, 23, 20, 102, 102, 102, 102, 102, 102, 27, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_create_spring,    TAR_IGNORE,     POS_STANDING,
    &gsn_create_spring,           SLOT(80),   20, 12, "",         "!Create Spring!",  "" },

    { "create water",     { 8,  3, 12, 11, 102, 102, 102, 102, 102, 102, 3, 5, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_create_water, TAR_OBJ_INV,        POS_STANDING,
    &gsn_create_water,           SLOT(13),    5, 12, "",         "!Create Water!",   "" },

    { "cure blindness",   { 102,  6, 102, 8 , 102, 102, 102, 102, 102, 102, 7, 102, 102},
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 2, 2}, spell_cure_blindness,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    &gsn_cure_blindness,           SLOT(14),    5, 12, "",         "!Cure Blindness!", "" },

    { "cure critical",    { 102,  13, 102, 19, 102, 102, 102, 102, 102, 102, 41, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cure_critical,    TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(15),   20, 12, "",         "!Cure Critical!",  "" },

    { "zeal",     {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 5},
    { 1,  1,  2,  2 , 1, 1, 1, 1, 1, 1, 1, 1, 3}, spell_zeal,     TAR_CHAR_SELF,  POS_STANDING,
    &gsn_zeal,           SLOT( 49),   40,    12, "",         "The glory of God slowly fades from your body.",    "" },

    { "cure light",       { 102,  1, 102, 3, 102, 102, 102, 102, 102, 102, 5, 30, 1 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 2, 2}, spell_cure_light,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(16),   10, 12, "",         "!Cure Light!",     "" },

    { "cure poison",      { 102,  14, 102, 16, 102, 102, 102, 102, 102, 102, 13, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_cure_poison,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_cure_poison,           SLOT(43),    5, 12, "",         "!Cure Poison!",    "" },

    { "cure serious",     { 102,  7, 102, 10, 102, 102, 102, 102, 102, 102, 25, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_cure_serious, TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(61),   15, 12, "",         "!Cure Serious!",   "" },

    { "curse",        { 18, 18, 26, 22, 102, 102, 102, 102, 102, 102, 43, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_curse,        TAR_OBJ_CHAR_OFF,   POS_FIGHTING,
    &gsn_curse,     SLOT(17),   20, 12, "curse",        "The curse wears off.", "$p is no longer impure." },

    { "demonfire",        { 102, 34, 102, 45, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_demonfire,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(505),  20, 12, "torments",     "!Demonfire!",      "" },

    { "detect evil",      { 11,  4, 12, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_evil,  TAR_CHAR_SELF,      POS_STANDING,
    NULL,           SLOT(18),    5, 12, "",         "The red in your vision disappears.",   "" },

    { "detect good",          { 11,  4, 12, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_good,      TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   SLOT(513),        5,     12, "",                     "The gold in your vision disappears.",  "" },

    { "detect hidden",    { 15, 11, 12, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_hidden,    TAR_CHAR_SELF,      POS_STANDING,
    &gsn_detect_hidden,         SLOT(44),    5, 12, "",         "You feel less aware of your surroundings.", "" },

    { "detect invis",     {  3,  8, 6, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_invis, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_detect_invis,          SLOT(19),    5, 12, "",         "You no longer see invisible objects.", "" },

    { "detect magic",     {  2,  6, 5, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_magic, TAR_CHAR_SELF,      POS_STANDING,
    NULL,           SLOT(20),    5, 12, "",         "The detect magic wears off.",  "" },

    { "detect poison",    { 15,  7, 9, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_detect_poison,    TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(21),    5, 12, "",         "!Detect Poison!",  "" },

    { "dispel evil",      { 102, 15, 102, 21, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_dispel_evil,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(22),   15, 12, "dispel evil",      "!Dispel Evil!",    "" },

    { "dispel good",          { 102, 15, 102, 21, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_dispel_good,      TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                   SLOT(512),      15,     12, "dispel good",          "!Dispel Good!",    "" },

    { "dispel magic",     { 16, 24, 30, 30, 102, 102, 102, 102, 102, 102, 70, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 1, 1}, spell_dispel_magic, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_dispel,           SLOT(59),   15, 12, "",         "!Dispel Magic!",   "" },

    { "earthquake",       { 102,  10, 102, 14, 102, 102, 102, 102, 102, 102, 45, 102, 45 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_earthquake,   TAR_IGNORE,     POS_FIGHTING,
    &gsn_earthquake,           SLOT(23),   15, 12, "earthquake",       "!Earthquake!",     "" },

    { "enchant armor",    { 16, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 2,  2,  4,  4, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_enchant_armor,    TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(510),  100,    24, "",         "!Enchant Armor!",  "" },

    { "enchant weapon",   { 17, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 2,  2, 4,  4, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_enchant_weapon,   TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(24),   100,    24, "",         "!Enchant Weapon!", "" },

    { "energy drain",     { 19, 22, 26, 23, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_energy_drain, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(25),   35, 12, "energy drain",     "!Energy Drain!",   "" },

    { "faerie fire",      {  6,  3, 5, 8, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_faerie_fire,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(72),    5, 12, "faerie fire",      "The pink aura around you fades away.", "" },

    { "faerie fog",       { 14, 21, 16, 24, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_faerie_fog,   TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(73),   12, 12, "faerie fog",       "!Faerie Fog!",     "" },

    { "farsight",     { 14, 16, 16, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_farsight,     TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(521),  36, 20, "farsight",     "!Farsight!",       "" },

    { "fireball",     { 22, 102, 30, 26, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_fireball,     TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(26),   15, 12, "fireball",     "!Fireball!",       "" },

    { "fireproof",        { 13, 12, 19, 18, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_fireproof,    TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(523),  10, 12, "",         "", "$p's protective aura fades." },

    { "flamestrike",      { 102, 20, 102, 27, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_flamestrike,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(65),   20, 12, "flamestrike",      "!Flamestrike!",        "" },

    { "fly",          { 10, 18, 20, 22, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_fly,      TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(56),   10, 18, "",         "You slowly float to the ground.",  "" },

    { "floating disc",    {  4, 10, 7, 16, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_floating_disc,    TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(522),  40, 24, "",         "!Floating disc!",  "" },

    { "frenzy",               { 102, 24, 102, 26, 102, 102, 102, 102, 102, 102, 75, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_frenzy,           TAR_CHAR_DEFENSIVE,     POS_STANDING,
    NULL,                   SLOT(504),      30,     24, "",                     "Your rage ebbs.",  "" },

    { "gate",         { 27, 17, 32, 28, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_gate,     TAR_IGNORE,     POS_FIGHTING,
    NULL,           SLOT(83),   80, 12, "",         "!Gate!",       "" },

    { "giant strength",   {  11, 102, 22, 20, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_giant_strength,   TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(39),   20, 12, "",         "You begin to feel weaker as the mystical energy leaves your muscles.", "" },

    { "harm",         { 102, 23, 102, 28, 102, 102, 102, 102, 102, 102, 33, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 5, 5, 1}, spell_harm,     TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(27),   35, 12, "harm spell",       "!Harm!",       "" },

    { "haste",        { 21, 102, 26, 29, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_haste,        TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(502),  30, 12, "",         "You feel yourself slow down.", "" },

    { "XP Boost",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_xp_boost,    TAR_CHAR_DEFENSIVE,      POS_RESTING,
    &gsn_xp_boost,           SLOT(0),     0, 12, "",         "",  "" },

    { "heal",         { 102, 21, 33, 30, 102, 102, 102, 102, 102, 102, 50, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_heal,     TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(28),   100,    12, "",         "!Heal!",       "" },

    { "heat metal",       { 102, 16, 102, 23, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_heat_metal,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_heat_metal,           SLOT(516),  25, 18, "searing equipment",        "!Heat Metal!",     ""},

    { "holy word",        { 102, 36, 102, 42, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 2,  2,  4,  4, 1, 1, 1, 1, 1, 1, 0, 0, 0}, spell_holy_word,    TAR_IGNORE, POS_FIGHTING,
    NULL,           SLOT(506),  200,    24, "divine wrath",     "!Holy Word!",      "" },

    { "identify",     { 15, 16, 18, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_identify,     TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(53),   12, 24, "",         "!Identify!",       "" },

    { "infravision",      {  9,  13, 10, 16, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_infravision,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(77),    5, 18, "",         "You no longer see in the dark.",   "" },

    { "intervention",     {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_intervention, TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(78),    5, 18, "",         "The awe of divine intervention slowly fades.", "" },

    { "invigorate",      {  8,  5, 12, 9, 102, 102, 102, 102, 102, 102, 9, 15, 1 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_invigorate,      TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(81),   20, 25, "invigorate",      "!Invigorate!",        "" },

    { "invisibility",     {  5, 102, 9, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_invis,        TAR_OBJ_CHAR_DEF,   POS_STANDING,
    &gsn_invis,     SLOT(29),    5, 12, "",         "You are no longer invisible.", "$p fades into view." },

    { "know alignment",   {  12,  9, 20, 102, 102, 102, 102, 102, 102, 102, 20, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_know_alignment,   TAR_CHAR_DEFENSIVE, POS_FIGHTING,
    NULL,           SLOT(58),    9, 12, "",         "!Know Alignment!", "" },

    { "lightning bolt",   {  13, 23, 18, 16, 102, 102, 102, 102, 102, 102, 37, 102, 25 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_lightning_bolt,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_lightning_bolt,           SLOT(30),   15, 12, "lightning bolt",   "!Lightning Bolt!", "" },

    { "locate object",    {  9, 15, 11, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_locate_object,    TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(31),   20, 18, "",         "!Locate Object!",  "" },

    { "magic missile",    {  1, 102, 2, 2, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_magic_missile,    TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(32),   15, 12, "magic missile",    "!Magic Missile!",  "" },

    { "mass healing",     { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 75, 102, 102 },
    { 2,  2,  4,  4, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_mass_healing, TAR_IGNORE,     POS_STANDING,
    &gsn_mass_healing,          SLOT(508),  250,    12, "",         "!Mass Healing!",   "" },

    { "mass invis",       { 22, 25, 31, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_mass_invis,   TAR_IGNORE,     POS_STANDING,
    &gsn_mass_invis,    SLOT(69),   20, 24, "",         "You are no longer invisible.",     "" },

    { "nexus",                { 40, 35, 50, 45, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 2,  2,  4,  4, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_nexus,            TAR_IGNORE,             POS_STANDING,
    NULL,                   SLOT(520),       150,   36, "",                     "!Nexus!",      "" },

    { "pass door",        { 24, 32, 25, 37, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_pass_door,    TAR_CHAR_SELF,      POS_STANDING,
    NULL,           SLOT(74),   20, 12, "",         "You feel solid again.",    "" },

    { "poison",       { 17,  12, 15, 21, 102, 102, 102, 102, 102, 102, 35, 83, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 3, 3, 3}, spell_poison,       TAR_OBJ_CHAR_OFF,   POS_FIGHTING,
    &gsn_poison,        SLOT(33),   10, 12, "poison",       "You feel less sick.", "The poison on $p dries up." },

    { "portal",               { 35, 30, 45, 40, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 2,  2,  4,  4, 0, 0, 0, 0, 0, 0, 0, 0, 0}, spell_portal,           TAR_IGNORE,             POS_STANDING,
    NULL,                   SLOT(519),       100,     24, "",                     "!Portal!",     "" },

    { "protection evil",  { 12,  9, 17, 11, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_protection_evil,  TAR_CHAR_SELF,      POS_STANDING,
    NULL,           SLOT(34),   5,  12, "",         "You feel less protected.", "" },

    { "protection good",      { 12,  9, 17, 11, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_protection_good,  TAR_CHAR_SELF,          POS_STANDING,
    NULL,                   SLOT(514),       5,     12, "",                     "You feel less protected.", "" },

    { "ray of truth",         { 102, 35, 102, 47, 102, 102, 102, 102, 102, 102, 47, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 5, 5, 5}, spell_ray_of_truth,     TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_ray_of_truth,                   SLOT(518),      20,     12, "ray of truth",         "!Ray of Truth!",   "" },

    { "recharge",     { 9, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_recharge,     TAR_OBJ_INV,        POS_STANDING,
    NULL,           SLOT(517),  60, 24, "",         "!Recharge!",       "" },

    { "refresh",      {  8,  5, 12, 9, 102, 102, 102, 102, 102, 102, 9, 15, 1 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_refresh,      TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(81),   12, 18, "refresh",      "!Refresh!",        "" },

    { "remove curse",     { 102, 18, 102, 22, 102, 102, 102, 102, 102, 102, 17, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 2, 2, 2}, spell_remove_curse, TAR_OBJ_CHAR_DEF,   POS_STANDING,
    &gsn_remove_curse,           SLOT(35),    5, 12, "",         "!Remove Curse!",   "" },

    { "sanctuary",        { 36, 20, 42, 30, 102, 102, 102, 102, 102, 102, 50, 50, 50 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 5, 5, 5}, spell_sanctuary,    TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_sanctuary,     SLOT(36),   125,    12, "",         "The white aura around your body fades.", "" },

    { "shield",       { 20, 35, 35, 40, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_shield,       TAR_CHAR_DEFENSIVE, POS_STANDING,
    NULL,           SLOT(67),   12, 18, "",         "Your force shield shimmers then fades away.", "" },

    { "lightning fist",   {  10, 102, 14, 13, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_lightning_fist,   TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(53),   15, 12, "lightning fist",   "!Lightning Fist!", "" },

    { "sleep",        { 10, 102, 11, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_sleep,        TAR_CHAR_OFFENSIVE, POS_STANDING,
    &gsn_sleep,     SLOT(38),   15, 12, "",         "You feel less tired.", "" },

    { "slow",                 { 23, 30, 29, 32, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_slow,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_slow,                   SLOT(515),      30,     12, "",                     "You feel yourself speed up.",  "" },

    { "stone skin",       { 25, 40, 40, 45, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_stone_skin,   TAR_CHAR_SELF,      POS_STANDING,
    NULL,           SLOT(66),   12, 18, "",         "Your skin feels soft again.",  "" },

    { "summon",       { 24, 12, 29, 22, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1,  1,  2,  2, 0, 0, 0, 0, 0, 0, 0, 0, 0}, spell_summon,       TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(40),   50, 12, "",         "!Summon!",     "" },

    { "teleport",     {  13, 22, 25, 36, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_teleport,     TAR_CHAR_SELF,      POS_FIGHTING,
    &gsn_teleport,          SLOT( 2),   35, 12, "",         "!Teleport!",       "" },

    { "ventriloquate",    {  1, 102, 2, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_ventriloquate,    TAR_IGNORE,     POS_STANDING,
    NULL,           SLOT(41),    5, 12, "",         "!Ventriloquate!",  "" },

    { "weaken",       {  11, 14, 16, 17, 102, 102, 102, 102, 102, 102, 23, 30, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 4, 4, 4}, spell_weaken,       TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_weaken,            SLOT(68),   20, 12, "spell",        "You feel stronger.",   "" },

    { "word of recall",   { 32, 28, 40, 30, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_word_of_recall,   TAR_CHAR_SELF,      POS_RESTING,
    NULL,           SLOT(42),    5, 12, "",         "!Word of Recall!", "" },

    { "acid breath",      { 31, 32, 33, 34, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_acid_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(200),  100,    24, "blast of acid",    "!Acid Breath!",    "" },

    { "fire breath",      { 40, 45, 50, 51, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_fire_breath,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(201),  200,    24, "blast of flame",   "The smoke leaves your eyes.",  "" },

    { "frost breath",     { 34, 36, 38, 40, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_frost_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(202),  125,    24, "blast of frost",   "!Frost Breath!",   "" },

    { "gas breath",       { 39, 43, 47, 50, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_gas_breath,   TAR_IGNORE,     POS_FIGHTING,
    NULL,           SLOT(203),  175,    24, "blast of gas",     "!Gas Breath!",     "" },

    { "lightning breath", { 37, 40, 43, 46, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_lightning_breath, TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    NULL,           SLOT(204),  150,    24, "blast of lightning",   "!Lightning Breath!",   "" },

    { "wrath of god", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_wrath_of_god, TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    NULL,                     SLOT(666),     0, 12, "wrath of god",      "!wrath of god!",  "" },

    { "radiant aura",     {  102, 102,102,102, 102, 102, 102, 102, 102, 102, 40, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_holyshield,   TAR_CHAR_SELF,      POS_STANDING,
    &gsn_holyshield,            SLOT(0),     0, 12, "radiant aura",         "Your Radiant blessing slowly slips away...", "" },

    { "dark aura",        {  102, 102,102,102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_darkshield,   TAR_CHAR_SELF,      POS_STANDING,
    &gsn_darkshield,            SLOT(0),     0, 12, "dark aura",            "Your evil aura of darkness slowly slips away...", "" },

    { "static charge",        {  102, 102,102,102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_shockshield,  TAR_CHAR_SELF,      POS_STANDING,
    &gsn_shockshield,           SLOT(0),     0, 12, "lightning touch",          "The static field around your body fades away...", "" },

    { "mana", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_mana, TAR_CHAR_SELF,      POS_RESTING,
    NULL,           SLOT(0),     0, 12, "",         "!Mana!",   "" },

    { "blood potion", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_blood,    TAR_CHAR_SELF,      POS_RESTING,
    NULL,           SLOT(0),     0, 12, "",         "!Blood!",  "" },

    { "human blood",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_blood,    TAR_CHAR_SELF,      POS_RESTING,
    NULL,           SLOT(0),     0, 12, "",         "!Blood!",  "" },

    { "kindred vitae",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_vampire_blood,    TAR_CHAR_SELF,      POS_RESTING,
    NULL,           SLOT(0),     0, 12, "",         "!Blood!",  "" },

/* combat and weapons skills
Total (31) */
    { "axe",{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_axe,               SLOT( 0),       0,      0, "",                     "!Axe!",        "" },

    { "dagger",{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_dagger,            SLOT( 0),       0,      0, "",                     "!Dagger!",     "" },

    { "flail",{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_flail,             SLOT( 0),       0,      0, "",                     "!Flail!",      "" },

    { "mace",         { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_mace,              SLOT( 0),       0,      0, "",                     "!Mace!",       "" },

    { "polearm",      { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_polearm,           SLOT( 0),       0,      0, "",                     "!Polearm!",        "" },

    { "lance expertise",      { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_lance,           SLOT( 0),       0,      0, "",                     "!Lance Expertise!",        "" },

    { "shield block",     { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_FIGHTING,
    &gsn_shield_block,  SLOT(0),    0,  0, "",         "!Shield!",     "" },

    { "spear",        { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_spear,             SLOT( 0),       0,      0, "",                     "!Spear!",      "" },

    { "sword",        { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_sword,             SLOT( 0),       0,      0, "",                     "!sword!",      "" },

    { "whip",         { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_whip,              SLOT( 0),       0,      0, "",                     "!Whip!",   "" },

    { "backstab",             { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_backstab,          SLOT( 0),        0,     24, "backstab",             "!Backstab!",       "" },

    { "bash",        { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_bash,              SLOT( 0),       0,      24, "shield bash",                 "!Shield Bash!",     "" },

    { "berserk",      { 40, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_berserk,           SLOT( 0),       0,      24, "",                     "You feel your pulse slow down.",   "" },

    { "dirt kicking",      { 5, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_FIGHTING,
    &gsn_dirt,      SLOT( 0),   0,  24, "kicked dirt",      "You rub the dirt out of your eyes.",   "" },

    { "disarm",              { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_disarm,            SLOT( 0),        0,     24, "",                     "!Disarm!",     "" },

    { "dodge",                { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_dodge,             SLOT( 0),        0,     0, "",                     "!Dodge!",      "" },

    { "block",                { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_block,             SLOT( 0),        0,     0, "",                     "!Block!",      "" },

    { "enhanced damage",      { 125, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_enhanced_damage,   SLOT( 0),        0,     0, "",                     "!Enhanced Damage!",    "" },

    { "retreat",      { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_retreat,   SLOT( 0),        0,     0, "",                     "!Retreat!",    "" },

    { "critical strike",     { 60, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_critical_strike,   SLOT( 0),        0,     0, "",                     "!Critical Strike!",    "" },

    { "knight's training",     { 50, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_knight_training,   SLOT( 0),        0,     0, "",                     "!Knight's Training!",  "" },

    { "envenom",      { 145, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },spell_null,     TAR_IGNORE,     POS_RESTING,
    &gsn_envenom,       SLOT(0),    0,  36, "",         "!Envenom!",        "" },

    { "hand to hand",     { 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_FIGHTING,
    &gsn_hand_to_hand,  SLOT( 0),   0,  0, "punch",         "!Hand to Hand!",   "" },

    { "kick",                 { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_kick,              SLOT( 0),        0,     12, "kick",                 "!Kick!",       "" },

    { "parry",                { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_parry,             SLOT( 0),        0,     0, "",                     "!Parry!",      "" },

    { "rescue",               { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_rescue,            SLOT( 0),        0,     12, "",                     "!Rescue!",     "" },

    { "trip",         { 30, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_FIGHTING,
    &gsn_trip,      SLOT( 0),   0,  24, "trip",         "!Trip!",       "" },

    { "second attack",        { 20, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_second_attack,     SLOT( 0),        0,     0, "",                     "!Second Attack!",  "" },

    { "third attack",         { 25, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_third_attack,      SLOT( 0),        0,     0, "",                     "!Third Attack!",   "" },

    { "fourth attack",        { 35, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_fourth_attack,     SLOT( 0),        0,     0, "",                     "!Fourth Attack!",  "" },


/* non-combat skills
Total (48) */
    { "classify", { 15, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_IGNORE, POS_STANDING,
    &gsn_classify, SLOT( 0),    0,  0,    "",         "",     ""    },

    { "fast healing",     { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_SLEEPING,
    &gsn_fast_healing,  SLOT( 0),   0,  0, "",         "!Fast Healing!",   "" },

    { "glower", { 50, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_IGNORE, POS_STANDING,
    &gsn_glower, SLOT( 0),    0,  0,    "",         "You relax your facial expressions to your normal countenance.",     ""    },

    { "haggle",       { 5, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_RESTING,
    &gsn_haggle,        SLOT( 0),   0,  0, "",         "!Haggle!",     "" },

    { "hide",         { 30, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_RESTING,
    &gsn_hide,      SLOT( 0),    0, 12, "",         "You feel exposed.",        "" },

    { "lore",         { 15, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_RESTING,
    &gsn_lore,      SLOT( 0),   0,  36, "",         "!Lore!",       "" },

    { "study",            { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_RESTING,
    &gsn_study,     SLOT( 0),   0,  36, "",         "!Study!",      "" },

    { "meditation",       { 30, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_SLEEPING,
    &gsn_meditation,    SLOT( 0),   0,  0, "",         "Meditation",       "" },

    { "peek",         { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_peek,      SLOT( 0),    0,  0, "",         "!Peek!",       "" },

    { "pick lock",        { 5, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_pick_lock,     SLOT( 0),    0, 12, "",         "!Pick!",       "" },

    { "sneak",       { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_sneak,     SLOT( 0),    0, 12, "",         "You no longer feel stealthy.", "" },

    { "steal",{ 40, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_steal,     SLOT( 0),    0, 24, "",         "!Steal!",      "" },

    { "scrolls",  { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_scrolls,       SLOT( 0),   0,  24, "",         "!Scrolls!",        "" },

    { "staves",       {  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_staves,        SLOT( 0),   0,  12, "",         "!Staves!",     "" },

    { "wands",        {  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_wands,     SLOT( 0),   0,  12, "",         "!Wands!",      "" },

    { "recall",       {  1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_recall,        SLOT( 0),   0,  12, "",         "!Recall!",     "" },

    { "behead",   { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_IGNORE,     POS_STANDING,
    &gsn_behead,    SLOT( 0),   0,  12, "attempted beheading",      "!Behead!",     "" },

    { "riding",       { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_riding,        SLOT( 0),   0,  12, "",         "!Riding!",     "" },

    { "track",    { 102, 102, 102, 102, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
	{ 0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_track,         SLOT(0),     0, 12, "",         "!Track!",  "" },

    { "regenerate",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_regen,         SLOT(0),     0, 12, "",         "Your body has rested enough to heal aggravated damage again.", "" },

    { "elemental soul",       {  102, 102,102,102, 50, 50, 50, 50, 50, 50, 50, 50, 50 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_elemental,         SLOT(0),     0, 12, "",         "Your link with the powers of the Elements fades away.", "" },

    { "fireshield", {  102, 102,102,102, 50, 50, 50, 50, 50, 50, 50, 50, 50 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_fireshield,            SLOT(0),     0, 12, "fireshield",           "Your fireshield fizzles and disappears...", "" },

    { "chant",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 10, 60, 70 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_chant,         SLOT(0),     0, 12, "",         "!chant!",  "" },

    { "botched allies",       {  102, 102,102,102, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_botched_allies,            SLOT(0),     0, 12, "boched allies",            "Your allies are once again receptive to your requests for aid.", "" },

    { "summoned illusion",       {  102, 102,102,102, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_summoned_illusion,            SLOT(0),     0, 12, "summoned illusion",            "Your illusion loses coherence and dissipates", "" },

    { "allies",       {  102, 102,102,102, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_allies,            SLOT(0),     0, 12, "allies",           "Your allies are once again receptive to your requests for aid.", "" },

    { "sanctus lucis",        {  102, 102,102,102, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_sanctus,           SLOT(0),     0, 12, "botched prayer",           "The darkness of the world once again creeps into your soul as you feel Heaven's protection lifting from your body.", "" },

    { "scry",     {  102, 102,102,102, 50, 50, 50, 50, 50, 50, 50, 50, 50 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_scry,          SLOT(0),     0, 12, "",         "!Scry!", "" },

    { "brew",     {  10, 10,10,10, 10, 10, 10, 10, 10, 10, 10, 10, 10 },
	{ 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,5 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_brew,          SLOT(0),     0, 36, "explosion",            "!Brew!", "" },

    { "skin",     { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_skin,          SLOT(0),     0, 240, "slip",         "!SKIN!", "" },

    { "bandage",             { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_bandage,          SLOT( 0),        0,     24, "failure",             "You remove your bloody bandages to make room for clean ones.",      "" },

    { "bleeding",       { 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1,  1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_null,       TAR_OBJ_CHAR_OFF,   POS_FIGHTING,
    &gsn_bleeding,        SLOT(33),   10, 12, "bleeding",       "Your bleeding trickles and stops, your wounds finally scab over.", "" },

    { "divine strength",             { 150, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 50, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_divine_strength,          SLOT( 0),        0,     24, "",             "Your divine blessing slowly fades away.",      "" },

    { "assassinate",             { 50, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_assassinate,          SLOT( 0),        0,     24, "assassination",             "!Assassinate!",       "" },

    { "ground",                 { 30, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_ground,              SLOT( 0),        0,     12, "ground",                 "!Ground!",       "" },

    { "warcry",                 { 55, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_warcry,              SLOT( 0),        0,     12, "warcry",                 "!Warcry!",       "" },

    { "headbutt",                { 50, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_headbutt,              SLOT( 0),        0,     24, "headbutt",                 "The soreness in your head fades away...",      "" },

    { "gouge",                { 25, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_gouge,              SLOT( 0),        0,     24, "eye gouge",                 "The wounds afflicting your eyes heal..",      "" },

    { "blast",           { 50, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_blast,              SLOT( 0),        0,     12, "blast",                 "!Blast!",     "" },

    { "slip",           { 60, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_FIGHTING,
    &gsn_slip,              SLOT( 0),        0,     24, "",                 "!Slip!",       "" },

    { "enhanced speed",            { 75, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_enhanced_speed,              SLOT( 0),        0,     0, "",                 "!Enhanced Speed!",     "" },

    { "renown",            { 70, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_renown,              SLOT( 0),        0,     0, "",                 "!Renown!",     "" },

    { "second kick",              { 25, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_OFFENSIVE,     POS_STANDING,
    &gsn_second_kick,              SLOT( 0),        0,     0, "",                 "!Second Kick!",        "" },

    { "vigor",       { 10, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_CHAR_SELF,      POS_STANDING,
    &gsn_vigor,         SLOT(0),     0, 12, "",         "You feel yourself slowing down.", "" },

    { "waylay",     { 55, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_CHAR_SELF,      POS_STANDING,
    &gsn_waylay,        SLOT(0),     0, 12, "",         "!Waylay!", "" },

    { "fortress",     { 55, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,     POS_STANDING,
    &gsn_fortress,          SLOT(0),     0, 12, "",         "The enhancement of your shield fades away.", "" },\

    { "arena",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_arena,          SLOT( 0),        0,     24, "",             "You may now recall from the {RArena{x.",       "" },

  { "hometown change", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
  { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_hometown_change,   SLOT(0),    0,  12, "hometown changed", " {R __________________________________________{x\n\r{R [{xYou may now set a new {Chometown{x once again.{R]{x\n\r  {RTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT", "" },

/* Skill Related Debuffs
Total (3) */
{ "deafened", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
&gsn_deafened,      SLOT(0),    0,  12, "deafened",         "Noises assault your eardrums as your hearing returns.", "" },

{ "laryngitis", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
&gsn_laryngitis,      SLOT(0),    0,  12, "laryngitis",         "The swelling in your throat heals and you are able to talk again.", "" },

{ "socially disgraced",       {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
&gsn_socialdisgrace,            SLOT(0),     0, 12, "",         "You finally feel comfortable showing your face in social circumstances again.", "" },


/* Generic Old Ritual Stuff
Total (3) */
    { "Stored: Inferno",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_store_inferno,          SLOT( 0),        0,     24, "Inferno",             "You loose control on your stored Ritual: Inferno",      "" },

    { "Stored: Assamite Acid",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_store_assamiteacid,          SLOT( 0),        0,     24, "Acidic Blast",             "You loose control on your stored Ritual: Assamite Acid",       "" },

    { "blood magic",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_blood_magic,          SLOT( 0),        0,     24, "blood magic",             "You feel your blood magic fading away.",        "" },

/*
  ------Vampire Affects-----
  Total (5)
*/
    { "torpor",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_CHAR_SELF,      POS_TORPOR,
    &gsn_torpor,        SLOT(0),    0,  12, "",     "You feel strong enough to try to rise from Torpor.",   "" },

    { "arise",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_CHAR_SELF,      POS_TORPOR,
    &gsn_arise,     SLOT(0),    0,  12, "",     "!arise!",  "" },

    { "blood buff",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_CHAR_SELF,      POS_FIGHTING,
    &gsn_blood_buff,        SLOT(0),    0,  8, "buffed blood",     "The enhanced power coursing through your veins slowly subsides.",  "" },

    { "vampire regen",        { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1,  1,  2,  2, 1, 1, 1, 1, 1, 1, 1, 1, 1}, spell_null,        TAR_CHAR_SELF, POS_STANDING,
    &gsn_vampire_regen,           SLOT(502),  30, 12, "",         "You stop regenerating.", "" },

    { "vampire frenzy",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, vamp_frenzy,    TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_vamp_frenzy,           SLOT(403),   0, 12, "frenzy",           "Your frenzy slowly fades away.", "" },

//  ***Disciplines***
//    --Animalism--
// Total (0)

//    --Auspex--
// Total (3)
    { "reveal",       {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_reveal,            SLOT(0),     0, 12, "",         "Your heightened senses fade away, leaving you aware of only the mundane world once again.",    "" },

    { "astrally lost",   { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_IGNORE,         POS_STANDING,
    &gsn_astrallylost,        SLOT( 0),     0,    24, "",      "Out of the corner of your eye, you spot your silver astral cord that leads back to your physical body!",  "" },

    { "astral projection",   { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,  TAR_IGNORE,        POS_STANDING,
    &gsn_astralprojection,     SLOT( 0),    0,    24, "",     "!!Wearoffmsg!!",     ""},

//    --Celerity--
// Total (1)
    { "celerity", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_celbuff,           SLOT(0),     0, 12, "",         "The world suddenly seems to speed up around you, as your vitae enhanced speed fades away.", "" },

//    --Chimerstry--
// Total (5)
    { "ignis fatuus",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_ignis_fatuus,            SLOT(405),   0, 12, "ignis",           "Your confusion fades away.", "" },

    { "chimaera",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_chimaera,            SLOT(0),   0, 12, "chimaera",           "!chimaera!", "" },

    { "phantasm",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_phantasm,            SLOT(0),   0, 12, "phantasm",           "The illusion masking your true position fades away.", "" },

    { "horrid reality",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_horridreality,            SLOT(0),   0, 12, "horrid reality",           "!Horrid Reality!", "" },

    { "chimerstry",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_chimerstry,            SLOT(0),   0, 12, "chimerstry",           "You once again feel strong enough to twist reality to your whim.", "" },

//    --Dementation--
// Total (0)

//    --Dominate--
// Total (2)
    { "forgetful mind",       {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_forget,            SLOT(0),     0, 12, "",         "Your memory slowly returns...", "" },

    { "mental resilience",       {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_mental_resilience,            SLOT(0),     0, 12, "",         "Your increased mental resistance has faded.", "" },

//    --Fortitude--
// Total (0)

//    --Mortis--
// Total (0)

//    --Obfuscate--
// Total (4)
    { "unseen",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_CHAR_SELF,      POS_TORPOR,
    &gsn_unseen,        SLOT(0),    0,  12, "",     "Your power to vanish from the minds of other fades and you become visible.",   "" },

    { "veil",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_CHAR_SELF,      POS_TORPOR,
    &gsn_veil,        SLOT(0),    0,  12, "",     "Your power to vanish from the minds of other fades and you become visible.",   "" },

    { "cloak the gathering",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_cloak_the_gathering,           SLOT(0),     0, 12, "",         "Your cloak of shadows fades away...",  "" },

    { "mask",     {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_mask,          SLOT(0),     0, 12, "",         "Your Mask of a Thousand Faces fades away revealing your true guise.", "" },

//    --Obtenebration--
// Total (5)
    { "arms of the abyss",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_armsoftheabyss,            SLOT(405),   0, 12, "shadow tendrils",         "The shadowy tendrils release you, and fade away.", "" },

    { "shadowplay",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_shadowplay,            SLOT(403),   0, 12, "shadowplay",         "The shadows surrounding you fade away.", "" },

    { "shroud of night",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_cloakshadow,           SLOT(0),     0, 12, "",         "Your cloak of shadows fades away...",  "" },

    { "nocturne",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_nocturne,           SLOT(0),     0, 12, "",         "The darkness lifts from around you.",  "" },

    { "shadowform",       {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_shadowform,            SLOT(0),     0, 12, "",         "Your shadowy form fades away.", "" },

//    --Potence--
// Total (0)

//    --Presence--
// Total (4)
    { "awe",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_awe,           SLOT(0),     0, 12, "",         "The impressive social mystique you bolstered slowly fades away.",  "" },

    { "botched presence", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_botched_presence,      SLOT(0),    0,  12, "botched presence",         "You feel prepared to show your face in public again.", "" },

    { "majesty",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_majesty,           SLOT(0),     0, 12, "",         "Your aura of greatness fades away.",   "" },

    { "spark of rage",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
  { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_sparkofrage,           SLOT(0),     0, 12, "",         "The aura of hatred following you begins to fade.",   "" },

//    --Protean--
// Total (4)
    { "claws",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_claws,         SLOT(0),     0, 12, "",         "Your claws slide back under your nails.",  "" },

    { "gleam",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_gleam,         SLOT(0),     0, 12, "",         "Your eyes cease to glow as your vitae enhanced vision returns to normal.", "" },

    { "shift",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_shift,                     SLOT(0),     0, 12, "",      "You loose control over your shifted form.",   "" },

    { "earthmeld",        {  102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_earthmeld,         SLOT(0),     0, 12, "",         "You emerge from the earth in a shower of dirt.", "" },

//    --Quietus--
// Total (3)
    { "weakness", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_quietus_weakness,          SLOT(0),     0, 12, "",         "Your strength returns as the weakness fades.", "" },

    { "blood curse",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_quietus_bloodcurse,            SLOT(0),     0, 12, "",         "You sigh in relief as your blood purifys the curse away.", "" },

    { "acidblood",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_acidblood,         SLOT(0),     0, 12, "acidic blood",         "!acid blood!", "" },

//    --Serpentis--
// Total (2)
    { "tongue of the asp",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_tongueoftheasp,            SLOT(0),   0, 12, "forked tongue",  "", "" },

    { "skin of the adder",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_skinoftheadder,            SLOT(0),   0, 12, "serpentine skin",  "", "" },

//    --Thaumaturgy: Path of Blood--
// Total (4)
    { "blood rage",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, thaumaturgy_frenzy, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_thaumaturgy_frenzy,            SLOT(405),   0, 12, "frenzy",           "Your frenzy slowly fades away.", "" },

    { "blood of potency", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	  { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_bloodofpotency,            SLOT(405),   0, 12, "",         "You feel suddenly weaker as the potency of your blood returns to normal.", "" },

    { "theft of vitae",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_theft,         SLOT(406),   0, 12, "theft",            "", "" },

    { "cauldron of blood",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_cauldron,          SLOT(406),   0, 12, "boiling blood",            "", "" },

//    --Vicissitude--
// Total (4)
    { "bonecrafting",     { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_vicissitude_bonecraft,          SLOT( 0),        0,     48, "bone spikes",             "Your body slowly reforms into its normal skeletal structure.",      "" },

    { "horrid form",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_vicissitude_horrid,                     SLOT(0),    0, 12, "",      "Your form shrinks and you slowly return to your 'normal' form.",  "" },

    { "chiropteran marauder",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
  	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
  	&gsn_vicissitude_chiropteran,	SLOT(0), 0, 12, "", "Your leathery wings merge with your body as you return to your normal form.",  "" },

    { "wing claws",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
  	{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
  	&gsn_wingclaws,	SLOT(0), 0, 12, "", "Your bony claws retract from the ends of your winged hands.",  "" },

//  ***Rituals***
//    --Thaumaturgy--
// Total (1)
    { "incorporeal passage",      { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null,     TAR_CHAR_SELF,      POS_STANDING,
    &gsn_incorporeal_passage,   SLOT( 0),   0,  12, "smashed mirror",   "Your body slowly solidifies as the ritual wears off.",     "" },

/*
  -----Werewolf Affects-----
  Total (5)
*/
    { "garou frenzy", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, garou_frenzy,   TAR_CHAR_SELF,      POS_SLEEPING,
    &gsn_garou_frenzy,          SLOT(403),   0, 12, "frenzy",           "Your frenzy slowly fades away.", "" },

    { "fetish",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
	  { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_fetish,          SLOT( 0),        0,     24, "",             "!Fetish!",     "" },

    { "spirit totem",     { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_IGNORE,     POS_RESTING,
    &gsn_spirit_totem,  SLOT(0),    0,  16, "Spirit Totem", "As the blessing of the Totem leaves your body, you give praise to the Spirit.",    "" },

    { "change form",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_change,                     SLOT(0),    0, 12, "",      "You loose control over your current form.",   "" },

    { "rage", { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_rage,                     SLOT(0),  0, 12, "",      "Your rage slowly fades.", "" },

//  ***Gifts***
//    --Homid--
// Total (10)
    { "master of fire",  { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_masteroffire,             TAR_IGNORE,             POS_STANDING,
    &gsn_gift_masteroffire,          SLOT( 0),        0,     12, "",  "The blessing of the Elemental spirits of Fire leaves you.",        "" },

    { "persuasion", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_persuasion,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_persuasion,          SLOT( 0),        0,     12, "",   "Your enhanced pursuasive abilities wear off.",        "" },

    { "speech of the world", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_speechoftheworld,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_speechoftheworld,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "staredown",        { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_staredown,             TAR_CHAR_OFFENSIVE,             POS_FIGHTING,
    NULL,          SLOT( 0),        0,     12, "",             "!STAREDOWN!",      "" },

    { "calm the savage beast", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_calmthesavagebeast,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_calmthesavagebeast,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "reshape object", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_reshapeobject,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_reshapeobject,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "bury the wolf", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_burythewolf,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_burythewolf,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "spirit ward", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritward,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_spiritward,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "assimilation", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_assimilation,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_assimilation,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "false comfort", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_falsecomfort,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_falsecomfort,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Metis--
// Total (11)
    { "create element", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_createelement,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_createelement,          SLOT( 0),        0,     24, "elemental blast",             "!!Wearoffmsg!!",        "" },

    { "sense wyrm",               { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_sensewyrm,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_sensewyrm,          SLOT( 0),        0,     12, "",             "Your perception of the Taint around you ebbs away.",       "" },

    { "shed", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_shed,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_shed,          SLOT( 0),        0,     12, "",             "Your fur will no longer slough off in combat.",        "" },

    { "curse of hatred", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_curseofhatred,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_curseofhatred,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "sense silver", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_sensesilver,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_sensesilver,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "eyes of the cat", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_eyesofthecat,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_eyesofthecat,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "mental speech", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_mentalspeech,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_mentalspeech,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "gift of the porcupine",        { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_porcupine,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_porcupine,          SLOT( 0),        0,     24, "quills",             "Your skin prickles as your fur returns to normal.",      "" },

    { "wither limb", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_witherlimb,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_witherlimb,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "madness", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_madness,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_madness,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "totem gift", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_totemgift,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_totemgift,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Lupus--
// Total (12)
    { "hares leap", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_haresleap,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_haresleap,          SLOT( 0),        0,     12, "",             "The blessing of the Hare spirits wears off.",        "" },

    { "heightened senses", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_heightenedsenses,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_heightenedsenses,          SLOT( 0),        0,     12, "",             "Your senses return to normal.",        "" },

    { "ley lines", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_leylines,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_leylines,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "scent of sight", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_scentofsight,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_scentofsight,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "sense the unnatural", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_sensetheunnatural,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_sensetheunnatural,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "devils child", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_devilschild,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_devilschild,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "catfeet",              { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_catfeet,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_catfeet,          SLOT( 0),        0,     12, "",             "You feel your attunement to the feline spirits fade away.",         "" },

    { "name the spirit", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_namethespirit,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_namethespirit,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "beast life", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_beastlife,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_beastlife,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "gnaw", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_gnaw,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_gnaw,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "elemental gift", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_elementalgift,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_elementalgift,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "song of the great beast", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_songofthegreatbeast,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_songofthegreatbeast,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Ragabash--
// Total (11)
    { "blur of the milky eye", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_blurofthemilkyeye,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_blurofthemilkyeye,          SLOT( 0),        0,     12, "",             "Your form becomes solid once again.",        "" },

    { "scent of running water", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_scentofrunningwater,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_scentofrunningwater,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "snow running", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_snowrunning,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_snowrunning,          SLOT( 0),        0,     12, "",             "Your spirit enhanced lightness and dexterity fades.",        "" },

    { "blissful ignorance",   { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_blissfulignorance,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_blissfulignorance,          SLOT( 0),        0,     12, "",             "You cease to hide yourself from others.",      "" },

    { "sense of prey", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_senseofprey,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_senseofprey,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "open moon bridge", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_openmoonbridge,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_openmoonbridge,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "reynards lie", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_reynardslie,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_reynardslie,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "lepers curse", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_leperscurse,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_leperscurse,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "lunas blessing", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_lunasblessing,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_lunasblessing,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "thieving talons of the magpie", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_thievingtalonsofthemagpie,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_thievingtalonsofthemagpie,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "chimerical form", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_chimericalform,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_chimericalform,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Theurge--
// Total (10)
    { "mothers touch", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_motherstouch,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_gift_motherstouch,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "spirit speech", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritspeech,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_spiritspeech,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "command spirit", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_commandspirit,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_commandspirit,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "sight from beyond", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_sightfrombeyond,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_sightfrombeyond,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "expel spirit", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_expelspirit,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_expelspirit,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "pulse of the invisible",               { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_pulseoftheinvisible,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_pulseoftheinvisible,          SLOT( 0),        0,     12, "",             "Your perception of the unseen blurs until it finally fades away.",     "" },

    { "grasp the beyond", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_graspthebeyond,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_graspthebeyond,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "spirit wassail", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritwassail,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_spiritwassail,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "nightmare possession", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_nightmarepossession,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_nightmarepossession,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "malleable spirit", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_malleablespirit,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_malleablespirit,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Philodox--
// Total (11)
    { "resist pain",              { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_resistpain,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_resistpain,          SLOT( 0),        0,     12, "",             "You now have the courage to resist your wounds.",      "" },

    { "scent of the true form", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_scentofthetrueform,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_scentofthetrueform,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "truth of gaia", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_truthofgaia,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_truthofgaia,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "call of duty", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_callofduty,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_callofduty,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "strength of purpose", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_strengthofpurpose,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_strengthofpurpose,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "spirit witness", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritwitness,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_spiritwitness,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "warriors eye", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_warriorseye,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_warriorseye,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "roll over", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_rollover,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_rollover,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "scent of beyond", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_scentofbeyond,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_scentofbeyond,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "earthfriends pact", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_earthfriendspact,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_earthfriendspact,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "geas", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_geas,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_geas,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Galliard--
// Total (11)
    { "beast spirit", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_beastspirit,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_beastspirit,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "call of the wyld",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_callofthewyld,             TAR_IGNORE,             POS_FIGHTING,
    &gsn_gift_callofthewyld,          SLOT( 0),        0,     12, "",             "You feel your bloodlust subsiding.",       "" },

    { "waking dream", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_wakingdream,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_wakingdream,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "call of the wyrm", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_callofthewyrm,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_callofthewyrm,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "distractions", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_distractions,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_distractions,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "eye of the asp", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_eyeoftheasp,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_eyeoftheasp,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "song of rage", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_songofrage,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_songofrage,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "shadows by the firelight", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_shadowsbythefirelight,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_shadowsbythefirelight,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "song of mockery", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_songofmockery,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_songofmockery,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "dream golems", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_dreamgolems,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_dreamgolems,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "call for vengeance", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_callforvengeance,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_callforvengeance,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Ahroun--
// total (11)
    { "falling touch", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_fallingtouch,  TAR_CHAR_OFFENSIVE, POS_FIGHTING,
    &gsn_gift_fallingtouch,          SLOT( 0),        0,     12, "touch",             "!!Wearoffmsg!!",        "" },

    { "inspiration", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_inspiration,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_inspiration,          SLOT( 0),        0,     12, "",             "Your determination and resolve fade.",        "" },

    { "razor claws",              { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_razorclaws,             TAR_CHAR_SELF,             POS_FIGHTING,
    &gsn_gift_razorclaws,          SLOT( 0),        0,     24, "",             "Your claws dull from battle back to their original sharpness.",        "" },

    { "spirit of the fray", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritofthefray,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_spiritofthefray,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "true fear", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_truefear,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_truefear,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "heart of fury", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_heartoffury,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_heartoffury,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "silver claws",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_silverclaws,             TAR_CHAR_SELF,             POS_FIGHTING,
    &gsn_gift_silverclaws,          SLOT( 0),        0,     24, "",             "You sigh in relief as the silver fades from your claws.",      "" },

    { "full moons light", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_fullmoonslight,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_fullmoonslight,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "stoking furys furnace", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_stokingfurysfurnace,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_stokingfurysfurnace,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "kiss of helios", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_kissofhelios,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_kissofhelios,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "strength of will", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_strengthofwill,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_strengthofwill,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Black Furies--
// Total (9)
    { "breath of the wyld", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_breathofthewyld,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_gift_breathofthewyld,          SLOT( 0),        0,     12, "",             "Gaia's rush of energy and clarity of thought fades from your mind.",        "" },

    { "mans skin", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_mansskin,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_mansskin,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "curse of the aeolus", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_curseoftheaeolus,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_curseoftheaeolus,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "coup de grace", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_coupdegrace,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_coupdegrace,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "flames of hestia", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_flamesofhestia,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_flamesofhestia,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "body wrack", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_bodywrack,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_bodywrack,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "bow of pegasus", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_bowofpegasus,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_bowofpegasus,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "gaze of the gorgon", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_gazeofthegorgon,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_gazeofthegorgon,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "wyld warp", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_wyldwarp,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_wyldwarp,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Bone Gnawers--
// Total (11)
    { "cookery", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_cookery,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_cookery,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "resist toxin", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_resisttoxin,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_resisttoxin,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "scent of the honeycomb", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_scentofthehoneycomb,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_scentofthehoneycomb,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "cornered rat", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_corneredrat,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_corneredrat,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "plague visage", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_plaguevisage,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_plaguevisage,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "call the rust", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_calltherust,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_calltherust,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "friend in need", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_friendinneed,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_friendinneed,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "blink", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_blink,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_blink,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "claws of the glass blower", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_clawsoftheglassblower,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_clawsoftheglassblower,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "maw of the whale", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_mawofthewhale,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_mawofthewhale,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    { "survivor", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_survivor,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_survivor,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Children of Gaia--
// Total (8)
{   "eves blessing", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_evesblessing,  TAR_IGNORE, POS_STANDING,
&gsn_gift_evesblessing,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "grandmothers touch", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_grandmotherstouch,  TAR_IGNORE, POS_STANDING,
&gsn_gift_grandmotherstouch,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "lunas armor", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_lunasarmor,  TAR_IGNORE, POS_STANDING,
&gsn_gift_lunasarmor,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "dazzle", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_dazzle,  TAR_IGNORE, POS_STANDING,
&gsn_gift_dazzle,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "spirit friend", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_spiritfriend,  TAR_IGNORE, POS_STANDING,
&gsn_gift_spiritfriend,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "angels semblance", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_angelssemblance,  TAR_IGNORE, POS_STANDING,
&gsn_gift_angelssemblance,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "halo of the sun", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_haloofthesun,  TAR_IGNORE, POS_STANDING,
&gsn_gift_haloofthesun,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

{   "the living wood", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
{ 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_thelivingwood,  TAR_IGNORE, POS_STANDING,
&gsn_gift_thelivingwood,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//    --Get of Fenris--

//    --Fianna--

//    --Red Talons--

//    --Shadow Lords--

//    --Silent Striders--

//    --Silver Fangs--

//    --Stargazers--

//    --Warders of Men--

// Total (49)
    { "inner strength",               { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_innerstrength,             TAR_CHAR_SELF,             POS_STANDING,
    &gsn_gift_innerstrength,          SLOT( 0),        0,     48, "",             "Your resolve has strengthened to the point that you can summon your Inner Strength again.",        "" },

    { "bloody feast",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_bloodyfeast,             TAR_IGNORE,             POS_STANDING,
    &gsn_gift_bloodyfeast,          SLOT( 0),        0,     12, "",             "The flesh you consumed loses it's power over you.",        "" },

    {   "hunters harmony", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_huntersharmony,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_huntersharmony,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "visage of fenris", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_visageoffenris,  TAR_IGNORE, POS_FIGHTING,
    &gsn_gift_visageoffenris,          SLOT( 0),        0,     12, "",             "The power of Fenris leaves your body, leaving you less intimidating.",        "" },

    {   "halt the cowards flight", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_haltthecowardsflight,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_haltthecowardsflight,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "sense guilt", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_senseguilt,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_senseguilt,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "might of thor", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_mightofthor,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_mightofthor,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "berserkers song", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_berserkerssong,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_berserkerssong,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "iron cannot pierce", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_ironcannotpierce,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_ironcannotpierce,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "blaze of glory", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_blazeofglory,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_blazeofglory,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "horde of valhalla", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_hordeofvalhalla,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_hordeofvalhalla,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "distract the fool", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_distractthefool,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_distractthefool,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "faerie light", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_faerielight,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_faerielight,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "howl of the banshee", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_howlofthebanshee,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_howlofthebanshee,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "warriors draught", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_warriorsdraught,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_warriorsdraught,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "faerie aid", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_faerieaid,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_faerieaid,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "woadling", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_woadling,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_woadling,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "balors gaze", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_balorsgaze,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_balorsgaze,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "semblance", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_semblance,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_semblance,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "fog on the moor", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_fogonthemoor,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_fogonthemoor,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "gift of the spriggan", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_giftofthespriggan,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_giftofthespriggan,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "aura of confidence", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_auraofconfidence,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_auraofconfidence,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "fatal flaw", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_fatalflaw,  TAR_CHAR_DEFENSIVE, POS_STANDING,
    &gsn_gift_fatalflaw,          SLOT( 0),        0,     12, "",             "You decide that your Fatal Flaw opponent is no longer worth your time.",        "" },

    {   "seizing the edge", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_seizingtheedge,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_seizingtheedge,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "clap of thunder", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_clapofthunder,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_clapofthunder,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "ravens curse", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_ravenscurse,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_ravenscurse,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "shadow cutting", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_shadowcutting,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_shadowcutting,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "dolorous stroke", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_dolorousstroke,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_dolorousstroke,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "strength of the dominator", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_strengthofthedominator,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_strengthofthedominator,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "shadow pack", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_shadowpack,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_shadowpack,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "obedience", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_obedience,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_obedience,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "eye of the falcon", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_eyeofthefalcon,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_eyeofthefalcon,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "lambent fire", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_lambentfire,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_lambentfire,          SLOT( 0),        0,     12, "",             "The brilliant silver light emanating from your body slowly dims and winks out.",        "" },

    {   "empathy", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_empathy,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_empathy,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "exceptional wordplay", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_exceptionalswordplay,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_exceptionalswordplay,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "wrath of gaia", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_wrathofgaia,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_wrathofgaia,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "lordly will", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_lordlywill,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_lordlywill,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "sidestep death", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_sidestepdeath,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_sidestepdeath,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "lunas avenger", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_lunasavenger,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_lunasavenger,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "paws of the newborn cub", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_pawsofthenewborncub,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_pawsofthenewborncub,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "artisans command", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_artisanscommand,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_artisanscommand,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "gift of salt", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_giftofsalt,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_giftofsalt,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "mask the predator", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_maskthepredator,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_maskthepredator,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "tongues", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_tongues,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_tongues,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "attunement", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_attunement,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_attunement,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "doppelganger", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_doppelganger,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_doppelganger,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "command the blaze", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_commandtheblaze,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_commandtheblaze,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

    {   "redirect pain", { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_gift_redirectpain,  TAR_IGNORE, POS_STANDING,
    &gsn_gift_redirectpain,          SLOT( 0),        0,     12, "",             "!!Wearoffmsg!!",        "" },

//  ***Caern Affects***
// Total (2)
    { "caern: rage",      {120, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_caern_rage,  TAR_IGNORE,     POS_FIGHTING,
    NULL,                   SLOT(401),      0,      12, "rage", "!rage!",   "" },

    { "caern: healing",       { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 }, spell_caern_healing,   TAR_IGNORE,     POS_FIGHTING,
    NULL,                   SLOT(402),      0,      12, "healing",  "!healing!",    "" },

/*
  -------Mage Affects-------
  Total (2)
*/
    { "tass: quintessence",   { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_rote_quintessence,    TAR_IGNORE, POS_RESTING,
    NULL,       SLOT(402),  0,  12, "rush of quintessence", "!quintessencerush!",   "" },

/*  { "quintessential bond",  { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, spell_null, TAR_IGNORE, POS_RESTING,
    &gsn_quintessentialbond,    SLOT(0),    0,  12, "", "!quintessentialbond!", "" }, */

    { "magick",               { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_magick,          SLOT( 0),        0,     24, "magick",             "The magick infusing you slowly fades...",        "" },


//  ***Rotes***
//    --Correspondence--
// Total (0)

//    --Entropy--
// Total (1)
    { "control randomness",    { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,    TAR_IGNORE,      POS_STANDING,
    &gsn_controlrandomness,     SLOT( 0),  0,  24, "",  "You feel your power to control random events slowly fade.",   "" },

//    --Forces--
// Total (1)
    { "kinetic shield",           { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },     \
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_kineticshield,          SLOT( 0),        0,     24, "",             "Your ability to deflect physical blows fades away.",       "" },

//    --Life--
// Total (0)

//    --Matter--
// Total (0)

//    --Mind--
// Total (1)
    { "empower",              { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,     TAR_IGNORE,      POS_STANDING,
    &gsn_empower,          SLOT( 0),        0,     24, "",  "Your mind seems to dim as the forces of reality wash away your clarity of thought.",   "" },

//    --Prime--
// Total (1)
    { "prime sense",		{ 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,	TAR_IGNORE,	POS_STANDING,
    &gsn_primesense,	SLOT( 0),	0,	24, "",	"The flow of Quintessence through reality fades from your vision.", ""},


//    --Spirit--
// Total (2)
    { "spirit sight",		{ 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,	TAR_IGNORE,	POS_STANDING,
    &gsn_spiritsight,	SLOT( 0),	0,	24, "",	"Your ethereal senses shift back to this reality.", ""},

    { "trapped in gauntlet",   { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null, TAR_IGNORE,         POS_STANDING,
    &gsn_trappedingauntlet,        SLOT( 0),     0,    24, "",  "Your very Essence finally untangles itself from the Gauntlet!",  "" },

//    --Time--
// Total (4)
    { "time sense",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_timesense,          SLOT( 0),        0,     24, "",             "Your sensing of the flow of time ebbs away.",     "" },

    { "precognition",             { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_precognition,          SLOT( 0),        0,     24, "",             "Your foresight of time fades away as you lose focus.",     "" },

    { "time alteration",          { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_timealteration,          SLOT( 0),        0,     24, "",             "Your pattern can no longer take the stress of such time dilation.",        "" },

    { "sidestep time",            { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_sidesteptime,          SLOT( 0),        0,     24, "",             "You step back into the normal stream of time.",        "" },

/*    --Extra Combination Sphere Rotes--
Total (1)
*/
    { "negation field",           { 102, 102,  102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, spell_null,             TAR_IGNORE,             POS_STANDING,
    &gsn_negationfield,          SLOT( 0),        0,     24, "",             "You sense your ability to negate the magicks of others fading.",       "" },

/*
  ----Immortal Functions----
Total (1)
*/
    { "touch",    { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, spell_null, TAR_CHAR_SELF,      POS_STANDING,
    &gsn_touch,                     SLOT(0),     0, 12, "",      "The touch of the Gods fades from your body...",    "" },

/*
  ----Unused Skills / Spells / GSNS----
*/

// END OF TABLE.
};


const   struct  group_type      group_table     [MAX_GROUP]     =
{

    {
    "rom basics",       { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { "recall" }
    },

    {
    "mage basics",      { 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    { "dagger" }
    },

    {
    "cleric basics",    { -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "mace" }
    },

    {
    "thief basics",     { -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    { "dagger", "steal" }
    },

    {
    "warrior basics",   { -1, -1, -1, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "sword", "second attack" }
    },

    {
    "rogue basics", { -1, -1, -1, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1 },
    { "dagger" }
    },

    {
    "swordsman basics", { -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, -1, -1, -1 },
    { "sword" }
    },

    {
    "clergy basics",    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0 },
    { "mace" }
    },

    {
    "mage default",     { 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "lore", "beguiling", "combat", "detection", "enhancement", "illusion",
      "maladictions", "protective", "transportation", "weather" }
    },

    {
    "cleric default",   { -1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "flail", "attack", "creation", "curative",  "benedictions",
      "detection", "healing", "maladictions", "protective", "shield block",
      "transportation", "weather" }
    },

    {
    "thief default",    { -1, -1, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "mace", "sword", "backstab", "disarm", "dodge", "second attack",
      "trip", "hide", "peek", "pick lock", "sneak" }
    },

    {
    "warrior default",  { -1, -1, -1, 30, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "weaponsmaster", "shield block", "bash", "disarm", "enhanced damage",
      "parry", "rescue", "third attack" }
    },

    {
    "bandit basic", { -1, -1, -1, -1, 30, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "dagger", "parry",  "backstab", "dodge", "second attack", "dodge",
      "third attack", "pick lock", "steal", "dirt kicking", "sneak", "berserk",
      "enhanced damage", "kick", "headbutt"}
    },

    {
    "assassin basic",   { -1, -1, -1, -1, -1, 30, -1, -1, -1, -1, -1, -1, -1 },
    { "dagger", "backstab", "parry", "dodge", "second attack",
      "sneak", "enhanced damage", "hide", "dirt kicking", "third attack",
      "envenom", "assassinate"}
    },

    {
    "trickster basic",  { -1, -1, -1, -1, -1, -1, 30, -1, -1, -1, -1, -1, -1 },
    { "dagger", "haggle" "backstab", "sneak", "parry", "disarm", "pick lock",
      "dodge", "second attack", "trip", "peek", "steal", "kick", "hide",
      "blast", "slip" }
    },

    {
    "mercenary basic",  { -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, -1, -1, -1 },
    { "sword", "shield block", "enhanced damage",  "parry", "vigor", "fourth attack",
      "second attack", "kick", "third attack", "dodge", "fifth attack", "waylay",
      "enhanced speed", "berserk" }
    },

    {
    "guardsman basic",  { -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, -1, -1 },
    { "sword", "shield block", "parry", "dodge", "rescue", "bash",
      "fast healing", "disarm", "second attack", "third attack", "kick",
      "enhanced damage", "fourth attack", "block", "fortress" }
    },

    {
    "knight basic",     { -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1, -1 },
    { "sword", "shield block", "rescue", "parry", "dodge", "kick", "fast healing",
      "second attack", "enhanced damage", "third attack", "fourth attack",
      "knight's training", "warcry", "renown" }
    },

    {
    "priest basic",     { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1, -1 },
    { "mace","dodge", "parry" "chant", "lore", "meditation",
      "priest prayers" }
    },

    {
    "monk basic",       { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30, -1 },
    { "mace", "hand to hand", "kick", "dodge",  "parry", "disarm", "meditation",
      "second attack", "third attack", "lore", "divine strength", "chant"
      "enhanced damage", "monk prayers" }
    },

    {
    "crusader basic",   { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 30 },
    { "mace", "shield block", "second attack",  "parry", "rescue", "lore",
      "enhanced damage", "dodge", "third attack", "meditation",
      "fourth attack", "crusader prayers" }
    },

    {
    "priest prayers",   { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0,-1, -1 },
    { "create food", "create water", "cure poison", "cure blindness",
      "refresh", "armor", "control weather", "remove curse", "frenzy",
      "weaken", "create spring", "harm", "dispel magic",
      "blindness", "poison", "lightning bolt", "call lightning", "heal", "mass healing",
      "curse", "earthquake", "ray of truth", "sanctuary", "cure light",
      "radiant aura", "cure serious", "cure critical", "cause light", "bless"}
    },

    {
    "monk prayers", { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1 },
    { "create food", "create water", "refresh",  "armor",
      "weaken", "blindness", "poison", "sanctuary", "cure light" }
    },

    {
    "crusader prayers", { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0 },
    { "refresh", "armor", "lightning bolt", "call lightning",
      "curse", "earthquake", "sanctuary", "cure light", "zeal" }
    },

    {
    "weaponsmaster",    { 40, 40, 40, 20, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    { "axe", "dagger", "flail", "mace", "polearm", "spear", "sword","whip" }
    },

    {
    "attack",       { -1, 5, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "demonfire", "dispel evil", "dispel good", "earthquake",
      "flamestrike", "heat metal", "ray of truth" }
    },

    {
    "beguiling",        { 4, -1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "calm", "charm person", "sleep" }
    },

    {
    "benedictions",     { -1, 4, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "bless", "calm", "frenzy", "holy word", "remove curse" }
    },

    {
    "combat",       { 6, -1, 10, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "acid blast", "burning hands", "chain lightning", "chill touch",
      "colour spray", "fireball", "lightning bolt", "magic missile",
      "lightning fist" }
    },

    {
    "creation",     { 4, 4, 8, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "continual light", "create food", "create spring", "create water",
      "create rose", "floating disc" }
    },

    {
    "curative",     { -1, 4, -1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "cure blindness", "cure disease", "cure poison" }
    },

    {
    "detection",        { 4, 3, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "detect evil", "detect good", "detect hidden", "detect invis",
      "detect magic", "detect poison", "farsight", "identify",
      "locate object" }
    },

    {
    "draconian",        { 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "acid breath", "fire breath", "frost breath", "gas breath",
      "lightning breath"  }
    },

    {
    "enchantment",      { 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "enchant armor", "enchant weapon", "fireproof", "recharge" }
    },

    {
    "enhancement",      { 5, -1, 9, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "giant strength", "haste", "infravision", "refresh" }
    },

    {
    "harmful",      { -1, 3, -1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "cause critical", "cause light", "cause serious", "harm" }
    },

    {
    "healing",      { -1, 3, -1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "cure critical", "cure light", "cure serious", "heal",
      "mass healing", "refresh" }
    },

    {
    "illusion",     { 4, -1, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "invis", "mass invis", "ventriloquate" }
    },

    {
    "maladictions",     { 5, 5, 9, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "blindness", "change sex", "curse", "energy drain", /*"plague", */
      "poison", "slow", "weaken" }
    },

    {
    "protective",       { 4, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "armor", "cancellation", "dispel magic", "fireproof",
      "sanctuary", "shield", "stone skin" }
    },

    {
    "transportation",   { 4, 4, 8, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "fly", "gate", "nexus", "pass door", "portal", "summon", "teleport",
      "word of recall" }
    },

    {
    "weather",      { 4, 4, 8, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    { "call lightning", "control weather", "faerie fire", "faerie fog",
      "lightning bolt" }
    }



};
