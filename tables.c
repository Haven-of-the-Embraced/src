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
#include "merc.h"
#include "rote.h"
#include "tables.h"

const struct hometown_type hometown_table[MAX_HOMETOWN] =
{
	{"None", ROOM_VNUM_TEMPLE},
	{"Chester", 701},
	{"Geneva",  10995}
};

const struct cr_abil_type cr_abil_table[3] =
{
    /*   */

    {{ 0,0,1,4,4,0,3,1,0,0,0,2,0,2,4,0,0,0,0,1,0,0,2,2,1,0,0,0,0,0 }, { 3, 2, 5}},
    {{ 0,0,3,2,4,0,0,0,4,0,0,0,0,0,3,0,0,2,4,0,0,0,0,0,0,3,0,0,2,0 }, { 2, 5, 3}},
    {{ 0,2,1,1,1,2,0,2,0,0,0,0,1,0,1,2,0,1,0,0,1,4,0,0,0,2,2,1,0,2 }, { 4, 3, 3}}
};

const struct cr_attribute_type cr_attribute_table[3] =
{
    /*
#define CSATTRIB_STRENGTH       0
#define CSATTRIB_DEXTERITY      1
#define CSATTRIB_STAMINA        2
#define CSATTRIB_CHARISMA       3
#define CSATTRIB_MANIPULATION   4
#define CSATTRIB_APPEARANCE     5
#define CSATTRIB_PERCEPTION     6
#define CSATTRIB_INTELLIGENCE   7
#define CSATTRIB_WITS           8 */
    {{ 4,3,3,3,3,2,2,1,3 }},
    {{ 2,4,2,3,4,3,2,2,2 }},
    {{ 1,3,2,4,2,2,3,4,3 }}
};

const struct template_type template_table[] =
{
	/*
	{"Name",
	{Attributes},
	{Abilities},
	Willpower },
	*/
	{"Default",
	{2,2,3,2,2,2,3,2,2},
	{1,1,2,2,1,0,1,0,1,0,1,1,1,0,1,0,1,1,0,1,1,0,0,1,1,0,1,0,1,1},
	4},
	{"Hunter",
	{3,3,4,3,3,3,4,3,3},
	{0,2,3,3,2,0,2,1,1,2,0,2,1,0,2,1,0,0,2,1,1,2,0,1,0,0,1,2,1,1},
	6},
	{"Neonate",
	{3,3,4,3,3,3,4,3,3},
	{0,2,3,3,2,0,2,2,0,3,0,1,0,1,2,0,0,0,3,2,0,0,0,2,0,0,1,3,0,0},
	5},
	{"Ancient",
	{4,4,5,4,4,4,5,4,4},
	{0,2,4,3,2,0,3,3,0,3,0,0,2,0,3,0,0,0,3,3,3,0,0,3,2,0,2,4,0,0},
	8},
	{"Cub",
	{3,3,4,3,2,3,3,2,3},
	{1,3,2,3,1,0,3,1,0,0,3,0,0,2,3,0,2,0,3,3,1,0,0,2,0,0,1,2,0,0},
	5},
	{"Elder",
	{4,4,5,4,3,3,5,4,4},
	{2,4,3,4,0,0,4,3,0,0,4,0,0,2,4,0,2,0,4,4,1,0,0,2,0,0,1,3,0,0},
	6},
	{"Rodent",
	{1,2,2,0,0,0,2,1,2},
	{0,2,1,2,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
	2},
	{"Wolf",
	{3,3,4,0,0,0,3,2,3},
	{0,3,2,3,2,1,2,0,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0},
	4}
};

const struct cdisc_type clandisc_table[MAX_CLAN] =
{
    {0, 0, 0}, //0
    {0, 0, 0}, //1
    { DISC_AUSPEX, DISC_DOMINATE, DISC_OBFUSCATE },// Malkavian (2)
    { DISC_CELERITY, DISC_OBFUSCATE, DISC_QUIETUS},//Assamite (3)
    { DISC_MORTIS, DISC_AUSPEX, DISC_FORTITUDE  },// Cappadocian (4)
    { DISC_FORTITUDE, DISC_PROTEAN, DISC_ANIMALISM  },//Gangrel (5)
    { DISC_OBFUSCATE, DISC_POTENCE, DISC_ANIMALISM },//Nosferatu (6)
    { DISC_AUSPEX, DISC_CELERITY, DISC_PRESENCE },//Toreador (7)
    { DISC_CELERITY, DISC_POTENCE, DISC_PRESENCE },//Brujah (8)
    {DISC_DOMINATE, DISC_FORTITUDE, DISC_PRESENCE   },//Ventrue (9)
    {DISC_THAUMATURGY, DISC_DOMINATE, DISC_AUSPEX   },//Tremere (10)
    {DISC_DOMINATE, DISC_OBTENEBRATION, DISC_POTENCE    },//Lasombra (11)
     {DISC_ANIMALISM, DISC_AUSPEX, DISC_VICISSITUDE  },//Tzimisce (12)
    {0, 0, 0   }, //13
    {0, 0, 0    },//14
    {0, 0, 0   },//15
    {0, 0, 0   },//16
    {0, 0, 0   },//17
    {0, 0, 0   },//18
    {0, 0, 0   },//19
    {0, 0, 0   },//20
    {0, 0, 0   },//21
    {0, 0, 0   }, //22
    {0, 0, 0    }, //23
    {0, 0, 0   }, //24
    {0, 0, 0   },//25
    {0, 0, 0   },//26
    {DISC_CHIMERSTRY, DISC_FORTITUDE, DISC_ANIMALISM }, // Ravnos (27) (MAX)
    {0, 0, 0   },//28
    {0, 0, 0   },//29
    {0, 0, 0   }//30


    };
/* for clans */
/*
const struct clan_type clan_table[MAX_CLAN] =
{
      name,       who entry,  death-transfer room,    clan type
    {   "",     "",     ROOM_VNUM_ALTAR,    0, {0, 0, 0}},
    {   "loner",    "[ Loner ] ",   ROOM_VNUM_ALTAR,    TYPE_NONE, {0, 0, 0}    },
    {   "malkavian",    "[Malkavian] ", ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_AUSPEX, DISC_DOMINATE, DISC_OBFUSCATE} },
    {   "assamite", "[Assamite] ",  ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_CELERITY, DISC_OBFUSCATE, DISC_QUIETUS}    },
    {   "cappadocian",  "[Cappadocian] ",   ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_MORTIS, DISC_AUSPEX, DISC_FORTITUDE}   },
    {   "gangrel",  "[Gangrel] ",   ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_FORTITUDE, DISC_PROTEAN, DISC_ANIMALISM}   },
    {   "nosferatu",    "[Nosferatu] ", ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_OBFUSCATE, DISC_POTENCE, DISC_ANIMALISM}   },
    {   "toreador", "[Toreador] ",  ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_AUSPEX, DISC_CELERITY, DISC_PRESENCE}  },
    {   "brujah",   "[Brujah] ",    ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_CELERITY, DISC_POTENCE, DISC_PRESENCE} },
    {   "ventrue",  "[Ventrue] ",   ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_DOMINATE, DISC_FORTITUDE, DISC_PRESENCE}   },
    {   "Tremere",  "[Tremere] ",   ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_THAUMATURGY, DISC_DOMINATE, DISC_AUSPEX}   },
    {   "Lasombra", "[Lasombra] ",  ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_DOMINATE, DISC_OBTENEBRATION, DISC_POTENCE}    },
    {   "Tzimisce", "[Tzimisce] ",  ROOM_VNUM_ALTAR,    TYPE_CLAN, {DISC_ANIMALISM, DISC_AUSPEX, DISC_VICISSITUDE}  },
    {   "Mage", "[Mage] ",  ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "watcher",  "[Watcher] ",   ROOM_VNUM_ALTAR,    TYPE_NONE, {0, 0, 0}    },
    {   "blackfury",    "[BlackFuries] ",   ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "shadowlord",   "[ShadowLord] ",    ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "fenrir",  "[Fenrir] ",   ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "silverfang",   "[SilverFang] ",    ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "wardersofmen",    "[WardersofMen] ", ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "childrenofgaia",   "[ChildrenofGaia] ",    ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "fianna",   "[Fianna] ",    ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "bonegnawer",    "[BoneGnawer] ", ROOM_VNUM_ALTAR,    TYPE_TRIBE, {0, 0, 0}   },
    {   "akashicbrotherhood",   "[Akashic Brotherhood] ",   ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "dreamspeakers",    "[DreamSpeakers] ", ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "euthanatos",   "[Euthanatos] ",    ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "orderofhermes",    "[OrderOfHermes] ", ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "verbena",  "[Verbena] ",   ROOM_VNUM_ALTAR,    TYPE_TRADITION, {0, 0, 0}   },
    {   "rom",      "[  ROM  ] ",   ROOM_VNUM_ALTAR,    TYPE_NONE, {0, 0, 0}    }
};
*/
const struct sect_type sect_table[MAX_SECT] =
{
    /*  name,       color name,     who entry,  hall room, vamp */
    {   "",        "",          "",     ROOM_VNUM_ALTAR, FALSE},
    {   "Vampire",   "{rVampire{x", "{rVam{x",  ROOM_VNUM_ALTAR, TRUE},
    {   "Sabbat",      "{RSabbat{x",        "{RSab{x",  ROOM_VNUM_ALTAR, TRUE},
    {   "Independent", "{YIndependent{x",   "{YInd{x",  ROOM_VNUM_ALTAR, TRUE},
    {   "Immortal",    "{WImmortal{x",  "{WImm{x",  ROOM_VNUM_ALTAR, FALSE},
    {   "Mage",        "{cMage{x",      "{cMag{x",  ROOM_VNUM_ALTAR, FALSE},
    {   "Garou",       "{gGarou{x",     "{gGar{x",  ROOM_VNUM_ALTAR, FALSE}
};

/* for position */
const struct position_type position_table[] =
{
    {   "dead",         "dead"  },
    {   "in Torpor",            "torpor"    },
    {   "mortally wounded", "mort"  },
    {   "incapacitated",    "incap" },
    {   "stunned",      "stun"  },
    {   "sleeping",     "sleep" },
    {   "resting",      "rest"  },
    {   "sitting",      "sit"   },
    {   "fighting",     "fight" },
    {   "standing",     "stand" },
    {   NULL,           NULL    }
};

/* for sex */
const struct sex_type sex_table[] =
{
   {    "none"      },
   {    "male"      },
   {    "female"    },
   {    "either"    },
   {    NULL        }
};

/* for sizes */
const struct size_type size_table[] =
{
    {   "tiny"      },
    {   "small"     },
    {   "medium"    },
    {   "large"     },
    {   "huge",     },
    {   "giant"     },
    {   NULL        }
};

const struct flag_type attr_flags[] =
{
	{"strong",			A,	TRUE	},
	{"weak",			B,	TRUE	},
	{"brilliant",		C,	TRUE	},
	{"dull",			D,	TRUE	},
	{"social",			E,	TRUE	},
	{"awkward",			F,	TRUE	},
	{"allplustwo",		G,	TRUE	},
	{"allminustwo",		H,	TRUE	},
	{"allplusfour",		I,	TRUE	},
	{"allminusfour",	J,	TRUE	},
	{"crinos",			K,	TRUE	},
	{NULL,				0,	FALSE 	}
};

const struct flag_type abil_flags[] =
{
	{"warrior",			A,	TRUE	},
	{"occultist",		B,	TRUE	},
	{"mage",			C,	TRUE	},
	{"thief",			D,	TRUE	},
	{"healer",			E,	TRUE	},
	{"noble",			F,	TRUE	},
	{"clergy",			G,	TRUE	},
	{"scholar",			H,	TRUE	},
	{"merchant",	I, TRUE  },
	{"survivalist",	J, TRUE  },
	{"hermit",	K, TRUE  },
	{"entertainer",	L, TRUE  },
	{NULL,				0,	FALSE 	}
};

const struct flag_type act2_flags[] =
{
    { "influence",      A,  TRUE    },
    { "npc",            E,  FALSE   },
    { NULL,             0,  FALSE   }
};

/* various flag tables */
const struct flag_type act_flags[] =
{
    {   "npc",          A,  FALSE   },
    {   "sentinel",     B,  TRUE    },
    {   "scavenger",        C,  TRUE    },
    {   "mount",        E,  TRUE    },
    {   "aggressive",       F,  TRUE    },
    {   "stay_area",        G,  TRUE    },
    {   "wimpy",        H,  TRUE    },
    {   "pet",          I,  TRUE    },
    {   "train",        J,  TRUE    },
    {   "practice",     K,  TRUE    },
    {   "undead",       O,  TRUE    },
    {   "cleric",       Q,  TRUE    },
    {   "mage",         R,  TRUE    },
    {   "thief",        S,  TRUE    },
    {   "warrior",      T,  TRUE    },
    {   "noalign",      U,  TRUE    },
    {   "nopurge",      V,  TRUE    },
    {   "outdoors",     W,  TRUE    },
    {   "indoors",      Y,  TRUE    },
    {   "healer",       aa, TRUE    },
    {   "gain",         bb, TRUE    },
    {   "update_always",    cc, TRUE    },
    {   "changer",      dd, TRUE    },
    {   "questmob",     ee, TRUE    },
    {   NULL,           0,  FALSE   }
};

const struct flag_type plr_flags[] =
{
    {   "npc",          A,  FALSE   },
    {   "autovamp",     B,  FALSE   },
    {   "autoassist",       C,  FALSE   },
    {   "autoexit",     D,  FALSE   },
    {   "autoloot",     E,  FALSE   },
    {   "autosac",      F,  FALSE   },
    {   "autogold",     G,  FALSE   },
    {   "autosplit",        H,  FALSE   },
    {   "holylight",        N,  FALSE   },
    {   "can_loot",     P,  FALSE   },
    {   "nosummon",     Q,  FALSE   },
    {   "nofollow",     R,  FALSE   },
    {   "colour",       T,  FALSE   },
    {   "permit",       U,  TRUE    },
    {   "pueblo",       V,  FALSE   },
    {   "log",          W,  FALSE   },
    {   "deny",         X,  FALSE   },
    {   "freeze",       Y,  FALSE   },
    {   "thief",        Z,  FALSE   },
    {   "killer",       aa, FALSE   },
    {   "newbiehelper", bb, FALSE   },
    {   "arena",        ee, FALSE   },
    {   "d10combat",    ff, TRUE    },
    {   NULL,           0,  0   }
};

const struct flag_type plr2_flags[] =
{
	{   "newbie",          A,  FALSE   },
	{   "pvp",          B,  FALSE   },
	{   "questor",          C,  FALSE   },
	{   "leader",          D,  FALSE   },
	{   "is_npc",          E,  FALSE   },
	{   "autodoors",          F,  FALSE   },
	{   "wholocic",          G,  FALSE   },
	{   "wholocquiet",          H,  FALSE   },
	{   "wholocalways",          I,  FALSE   },
	{   "whoshowlevel",          J,  FALSE   },
	{   "immshowloc",          K,  FALSE   },
	{   "nocondition",          L,  FALSE   },
	{   "noshortdesc",          M,  FALSE   },
	{   "battleprompt",          N,  FALSE   },
	{   "debug",          O,  FALSE   },
	{   "debugmsg",          P,  FALSE   },
    {   NULL,           0,  0   }
};

const struct flag_type affect_flags[] =
{
    {   "blind",        A,  TRUE    },
    {   "invisible",        B,  TRUE    },
    {   "detect_evil",      C,  TRUE    },
    {   "detect_invis",     D,  TRUE    },
    {   "detect_magic",     E,  TRUE    },
    {   "detect_hidden",    F,  TRUE    },
    {   "detect_good",      G,  TRUE    },
    {   "sanctuary",        H,  TRUE    },
    {   "faerie_fire",      I,  TRUE    },
    {   "infrared",     J,  TRUE    },
    {   "curse",        K,  TRUE    },
    {   "shift",        L,  TRUE    },
    {   "poison",       M,  TRUE    },
    {   "protect_evil",     N,  TRUE    },
    {   "protect_good",     O,  TRUE    },
    {   "sneak",        P,  TRUE    },
    {   "hide",         Q,  TRUE    },
    {   "sleep",        R,  TRUE    },
    {   "charm",        S,  TRUE    },
    {   "flying",       T,  TRUE    },
    {   "pass_door",        U,  TRUE    },
    {   "haste",        V,  TRUE    },
    {   "calm",         W,  TRUE    },
/*    { "plague",       X,  TRUE    }, */
    {   "weaken",       Y,  TRUE    },
    {   "dark_vision",      Z,  TRUE    },
    {   "berserk",      aa, TRUE    },
    {   "swim",         bb, TRUE    },
    {   "regeneration",     cc, TRUE    },
    {   "slow",         dd, TRUE    },
    {   NULL,           0,  0   }
};

const struct flag_type affect2_flags[] =
{
    {   "mist",     A,  TRUE    },
    {   "veil",     B,  TRUE    },
    {   "vamp_frenzy",      C,  TRUE    },
    {   "majesty",      D,  TRUE    },
    {   "quietus_bloodcurse",       E,  TRUE    },
    {   "holyshield",   F,  TRUE    },
    {   "fireshield",   G,  TRUE    },
    {   "double_exp",   H,  TRUE    },
    {   "darkshield",   I,  TRUE    },
    {   "umbra",    J,  TRUE    },
    {   "unseen",   K, TRUE },
    {   "detect_unseen", L, TRUE},
    {   NULL,           0,  0   }
};


const struct flag_type off_flags[] =
{
    {   "area_attack",      A,  TRUE    },
    {   "backstab",     B,  TRUE    },
    {   "bash",         C,  TRUE    },
    {   "berserk",      D,  TRUE    },
    {   "disarm",       E,  TRUE    },
    {   "dodge",        F,  TRUE    },
    {   "fade",         G,  TRUE    },
    {   "fast",         H,  TRUE    },
    {   "kick",         I,  TRUE    },
    {   "dirt_kick",        J,  TRUE    },
    {   "parry",        K,  TRUE    },
    {   "rescue",       L,  TRUE    },
    {   "tail",         M,  TRUE    },
    {   "trip",         N,  TRUE    },
    {   "crush",        O,  TRUE    },
    {   "assist_all",       P,  TRUE    },
    {   "assist_align",     Q,  TRUE    },
    {   "assist_race",      R,  TRUE    },
    {   "assist_players",   S,  TRUE    },
    {   "assist_guard",     T,  TRUE    },
    {   "assist_vnum",      U,  TRUE    },
    {   "second_attack",        V,  TRUE    },
    {   "third_attack",     W,  TRUE    },
    {   "fourth_attack",        X,  TRUE    },
    {   "ultra_mob",        Y,  TRUE    },
    {   "sphere_entropy",   Z,  TRUE    },
    {   "sphere_forces",    aa, TRUE    },
    {   "sphere_life",      bb, TRUE    },
    {   "sphere_mind",      cc, TRUE    },
    {   "sphere_time",      dd, TRUE    },
    {   NULL,           0,  0   }
};

const struct flag_type imm_flags[] =
{
    {   "summon",       A,  TRUE    },
    {   "charm",        B,  TRUE    },
    {   "magic",        C,  TRUE    },
    {   "weapon",       D,  TRUE    },
    {   "bash",         E,  TRUE    },
    {   "pierce",       F,  TRUE    },
    {   "slash",        G,  TRUE    },
    {   "fire",         H,  TRUE    },
    {   "cold",         I,  TRUE    },
    {   "lightning",        J,  TRUE    },
    {   "acid",         K,  TRUE    },
    {   "poison",       L,  TRUE    },
    {   "negative",     M,  TRUE    },
    {   "holy",         N,  TRUE    },
    {   "energy",       O,  TRUE    },
    {   "mental",       P,  TRUE    },
    {   "disease",      Q,  TRUE    },
    {   "drowning",     R,  TRUE    },
    {   "light",        S,  TRUE    },
    {   "sound",        T,  TRUE    },
    {   "wood",         X,  TRUE    },
    {   "silver",       Y,  TRUE    },
    {   "iron",         Z,  TRUE    },
    {   "sunlight",         aa, TRUE    },
    {   "disarm",           bb,     TRUE},
    {   NULL,           0,  0   }
};

const struct flag_type form_flags[] =
{
    {   "edible",       FORM_EDIBLE,        TRUE    },
    {   "poison",       FORM_POISON,        TRUE    },
    {   "magical",      FORM_MAGICAL,       TRUE    },
    {   "instant_decay",    FORM_INSTANT_DECAY, TRUE    },
    {   "other",        FORM_OTHER,     TRUE    },
    {   "animal",       FORM_ANIMAL,        TRUE    },
    {   "sentient",     FORM_SENTIENT,      TRUE    },
    {   "undead",       FORM_UNDEAD,        TRUE    },
    {   "construct",        FORM_CONSTRUCT,     TRUE    },
    {   "mist",         FORM_MIST,      TRUE    },
    {   "intangible",       FORM_INTANGIBLE,    TRUE    },
    {   "biped",        FORM_BIPED,     TRUE    },
    {   "centaur",      FORM_CENTAUR,       TRUE    },
    {   "insect",       FORM_INSECT,        TRUE    },
    {   "spider",       FORM_SPIDER,        TRUE    },
    {   "crustacean",       FORM_CRUSTACEAN,    TRUE    },
    {   "worm",         FORM_WORM,      TRUE    },
    {   "blob",         FORM_BLOB,      TRUE    },
    {   "mammal",       FORM_MAMMAL,        TRUE    },
    {   "bird",         FORM_BIRD,      TRUE    },
    {   "reptile",      FORM_REPTILE,       TRUE    },
    {   "snake",        FORM_SNAKE,     TRUE    },
    {   "dragon",       FORM_DRAGON,        TRUE    },
    {   "amphibian",        FORM_AMPHIBIAN,     TRUE    },
    {   "fish",         FORM_FISH ,     TRUE    },
    {   "cold_blood",       FORM_COLD_BLOOD,    TRUE    },
    {   NULL,           0,          0   }
};

const struct flag_type part_flags[] =
{
    {   "head",         PART_HEAD,      TRUE    },
    {   "arms",         PART_ARMS,      TRUE    },
    {   "legs",         PART_LEGS,      TRUE    },
    {   "heart",        PART_HEART,     TRUE    },
    {   "brains",       PART_BRAINS,        TRUE    },
    {   "guts",         PART_GUTS,      TRUE    },
    {   "hands",        PART_HANDS,     TRUE    },
    {   "feet",         PART_FEET,      TRUE    },
    {   "fingers",      PART_FINGERS,       TRUE    },
    {   "ear",          PART_EAR,       TRUE    },
    {   "eye",          PART_EYE,       TRUE    },
    {   "long_tongue",      PART_LONG_TONGUE,   TRUE    },
    {   "eyestalks",        PART_EYESTALKS,     TRUE    },
    {   "tentacles",        PART_TENTACLES,     TRUE    },
    {   "fins",         PART_FINS,      TRUE    },
    {   "wings",        PART_WINGS,     TRUE    },
    {   "tail",         PART_TAIL,      TRUE    },
    {   "claws",        PART_CLAWS,     TRUE    },
    {   "fangs",        PART_FANGS,     TRUE    },
    {   "horns",        PART_HORNS,     TRUE    },
    {   "scales",       PART_SCALES,        TRUE    },
    {   "tusks",        PART_TUSKS,     TRUE    },
    {   NULL,           0,          0   }
};

const struct flag_type comm_flags[] =
{
    {   "quiet",        COMM_QUIET,     TRUE    },
    {   "deaf",         COMM_DEAF,      TRUE    },
    {   "nowiz",        COMM_NOWIZ,     TRUE    },
    {   "noimp",        COMM_NOIMP,     TRUE    },
    {   "noclangossip",     COMM_NOAUCTION,     TRUE    },
    {   "noic",     COMM_NOIC,      TRUE    },
    {   "noquestion",       COMM_NOQUESTION,    TRUE    },
    {   "nomusic",      COMM_NOMUSIC,       TRUE    },
    {   "noclan",       COMM_NOCLAN,        TRUE    },
    {   "noooc",        COMM_NOOOC,     TRUE    },
    {   "noannounce",       COMM_NOANNOUNCE,        TRUE    },
    {   "shoutsoff",        COMM_SHOUTSOFF,     TRUE    },
    {   "compact",      COMM_COMPACT,       TRUE    },
    {   "brief",        COMM_BRIEF,     TRUE    },
    {   "prompt",       COMM_PROMPT,        TRUE    },
    {   "combine",      COMM_COMBINE,       TRUE    },
    {   "telnet_ga",        COMM_TELNET_GA,     TRUE    },
    {   "nograts",      COMM_NOGRATS,       TRUE    },
    {   "noemote",      COMM_NOEMOTE,       FALSE   },
    {   "noshout",      COMM_NOSHOUT,       FALSE   },
    {   "notell",       COMM_NOTELL,        FALSE   },
    {   "nochannels",       COMM_NOCHANNELS,    FALSE   },
    {   "nosect",           COMM_NOSECT,    TRUE    },
    {   "snoop_proof",      COMM_SNOOP_PROOF,   FALSE   },
    {   "afk",          COMM_AFK,       TRUE    },
    {   "combat_brief", COMM_COMBAT_BRIEF,  TRUE    },
    {   "nochat",       COMM_NOCHAT, TRUE },
    {   "autoooc",      COMM_AUTOOOC, TRUE },
    {   "wiziname",     COMM_WIZINAME, FALSE },
    {   NULL,           0,          0   }
};

const struct flag_type mprog_flags[] =
{
    {   "act",          TRIG_ACT,       TRUE    },
    {   "bribe",        TRIG_BRIBE,     TRUE    },
    {   "death",        TRIG_DEATH,     TRUE    },
    {   "entry",        TRIG_ENTRY,     TRUE    },
    {   "fight",        TRIG_FIGHT,     TRUE    },
    {   "give",         TRIG_GIVE,      TRUE    },
    {   "greet",        TRIG_GREET,     TRUE    },
    {   "grall",        TRIG_GRALL,     TRUE    },
    {   "kill",         TRIG_KILL,      TRUE    },
    {   "hpcnt",        TRIG_HPCNT,     TRUE    },
    {   "random",       TRIG_RANDOM,    TRUE    },
    {   "speech",       TRIG_SPEECH,    TRUE    },
    {   "exit",         TRIG_EXIT,      TRUE    },
    {   "exall",        TRIG_EXALL,     TRUE    },
    {   "delay",        TRIG_DELAY,     TRUE    },
    {   "surr",         TRIG_SURR,      TRUE    },
	{   "systime", 		TRIG_SYSTIME, 	TRUE	},
	{   "mudtime", 		TRIG_MUDTIME, 	TRUE	},
    {   NULL,           0,          	TRUE    }
};

const struct flag_type oprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"give",			TRIG_GIVE,		TRUE	},
    {   "greet",		TRIG_GRALL,		TRUE	},
    {	"random",		TRIG_RANDOM,	TRUE	},
    {   "speech",		TRIG_SPEECH,	TRUE	},
    {	"exall",		TRIG_EXALL,		TRUE	},
    {	"delay",		TRIG_DELAY,		TRUE	},
    {	"drop",			TRIG_DROP,		TRUE	},
    {	"get",			TRIG_GET,		TRUE	},
    {	"sit",			TRIG_SIT,		TRUE	},
	{   "systime", 		TRIG_SYSTIME, 	TRUE	},
	{   "mudtime", 		TRIG_MUDTIME, 	TRUE	},
    {	NULL,			0,				TRUE	},
};

const struct flag_type rprog_flags[] =
{
    {	"act",			TRIG_ACT,		TRUE	},
    {	"fight",		TRIG_FIGHT,		TRUE	},
    {	"drop",			TRIG_DROP,		TRUE	},
    {	"greet",		TRIG_GRALL,		TRUE	},
    {	"random",		TRIG_RANDOM,	TRUE	},
    {	"speech",		TRIG_SPEECH,	TRUE	},
    {	"exall",		TRIG_EXALL,		TRUE	},
    {	"delay",		TRIG_DELAY,		TRUE	},
	{   "systime", 		TRIG_SYSTIME, 	TRUE	},
	{   "mudtime", 		TRIG_MUDTIME, 	TRUE	},
    {	NULL,			0,				TRUE	},
};

const struct flag_type area_flags[] =
{
    {   "none",         AREA_NONE,      FALSE   },
    {   "changed",      AREA_CHANGED,       TRUE    },
    {   "added",        AREA_ADDED,     TRUE    },
    {   "loading",      AREA_LOADING,       FALSE   },
    {   "pkill",        AREA_PKILL,         FALSE   },
    {   "unlinked",     AREA_UNLINKED,      TRUE    },
	{   "noteleport",   AREA_NOTELE, 		TRUE    },
    {   NULL,           0,          0   }
};



const struct flag_type sex_flags[] =
{
    {   "male",         SEX_MALE,       TRUE    },
    {   "female",       SEX_FEMALE,     TRUE    },
    {   "neutral",      SEX_NEUTRAL,        TRUE    },
    {   "random",               3,                      TRUE    },   /* ROM */
    {   "none",         SEX_NEUTRAL,        TRUE    },
    {   NULL,           0,          0   }
};



const struct flag_type exit_flags[] =
{
    {   "door",         EX_ISDOOR,      TRUE    },
    {   "closed",       EX_CLOSED,      TRUE    },
    {   "locked",       EX_LOCKED,      TRUE    },
    {   "pickproof",        EX_PICKPROOF,       TRUE    },
    {   "nopass",       EX_NOPASS,      TRUE    },
    {   "easy",         EX_EASY,        TRUE    },
    {   "hard",         EX_HARD,        TRUE    },
    {   "infuriating",      EX_INFURIATING,     TRUE    },
    {   "noclose",      EX_NOCLOSE,     TRUE    },
    {   "nolock",       EX_NOLOCK,      TRUE    },
    {   "lift",     EX_LIFT,        TRUE    },
    {   "hidden",       EX_HIDDEN,      TRUE    },
    {   NULL,           0,          0   }
};



const struct flag_type door_resets[] =
{
    {   "open and unlocked",    0,      TRUE    },
    {   "closed and unlocked",  1,      TRUE    },
    {   "closed and locked",    2,      TRUE    },
    {   NULL,           0,      0   }
};



const struct flag_type room_flags[] =
{
    {   "dark",         ROOM_DARK,      TRUE    },
    {   "no_mob",       ROOM_NO_MOB,        TRUE    },
    {   "indoors",      ROOM_INDOORS,       TRUE    },
    {   "private",      ROOM_PRIVATE,       TRUE    },
    {   "safe",         ROOM_SAFE,      TRUE    },
    {   "solitary",     ROOM_SOLITARY,      TRUE    },
    {   "pet_shop",     ROOM_PET_SHOP,      TRUE    },
    {   "mount_shop",       ROOM_MOUNT_SHOP,        TRUE    },
    {   "no_recall",        ROOM_NO_RECALL,     TRUE    },
    {   "imp_only",     ROOM_IMP_ONLY,      TRUE    },
    {   "gods_only",            ROOM_GODS_ONLY,     TRUE    },
    {   "heroes_only",      ROOM_HEROES_ONLY,   TRUE    },
    {   "newbies_only",     ROOM_NEWBIES_ONLY,  TRUE    },
    {   "law",          ROOM_LAW,       TRUE    },
    {   "nowhere",      ROOM_NOWHERE,       TRUE    },
    {   "forge",        ROOM_FORGE,         TRUE    },
    {   "no_quit",      ROOM_NO_QUIT,           TRUE    },
    {   "umbra",        ROOM_UMBRA,         TRUE    },
    {   "bank",                 ROOM_BANK,                      TRUE    },
    {   "underwater",           ROOM_UNDERWATER,                TRUE    },
    {   "human_only",           ROOM_HUMAN_ONLY,                TRUE    },
    {   "ghoul_only",           ROOM_GHOUL_ONLY,                TRUE    },
    {   "vampire_only",           ROOM_VAMPIRE_ONLY,                TRUE    },
    {   "garou_only",           ROOM_GAROU_ONLY,                TRUE    },
    {   "mage_only",           ROOM_MAGE_ONLY,                TRUE    },
	{   "no_teleport", 			ROOM_NOTELE,				  TRUE    },
    {   NULL,           0,          0   }
};



const struct flag_type sector_flags[] =
{
    {   "inside",   SECT_INSIDE,        TRUE    },
    {   "city",     SECT_CITY,      TRUE    },
    {   "field",    SECT_FIELD,     TRUE    },
    {   "forest",   SECT_FOREST,        TRUE    },
    {   "hills",    SECT_HILLS,     TRUE    },
    {   "mountain", SECT_MOUNTAIN,      TRUE    },
    {   "swim",     SECT_WATER_SWIM,    TRUE    },
    {   "noswim",   SECT_WATER_NOSWIM,  TRUE    },
    {   "unused",   SECT_UNUSED,        TRUE    },
    {   "air",      SECT_AIR,       TRUE    },
    {   "desert",   SECT_DESERT,        TRUE    },
    {   "drown",    SECT_WATER_DROWN,   TRUE    },
    {   "hot",      SECT_HOT,       TRUE    },
    {   "cold",     SECT_COLD,      TRUE    },
    {   "node",     SECT_NODE,      TRUE    },
    {   NULL,       0,          0   }
};

const struct flag_type clantype_flags[] =
{
    { "none", TYPE_NONE, TRUE},
    { "clan", TYPE_CLAN, TRUE},
    { "tribe", TYPE_TRIBE, TRUE},
    { "tradition", TYPE_TRADITION, TRUE},
    { NULL, 0, 0, }
};


const struct flag_type type_flags[] =
{
    {   "light",        ITEM_LIGHT,     TRUE    },
    {   "scroll",       ITEM_SCROLL,        TRUE    },
    {   "wand",         ITEM_WAND,      TRUE    },
    {   "staff",        ITEM_STAFF,     TRUE    },
    {   "weapon",       ITEM_WEAPON,        TRUE    },
    {   "treasure",     ITEM_TREASURE,      TRUE    },
    {   "armor",        ITEM_ARMOR,     TRUE    },
    {   "potion",       ITEM_POTION,        TRUE    },
    {   "furniture",        ITEM_FURNITURE,     TRUE    },
    {   "trash",        ITEM_TRASH,     TRUE    },
    {   "container",        ITEM_CONTAINER,     TRUE    },
    {   "drinkcontainer",   ITEM_DRINK_CON,     TRUE    },
    {   "key",          ITEM_KEY,       TRUE    },
    {   "food",         ITEM_FOOD,      TRUE    },
    {   "money",        ITEM_MONEY,     TRUE    },
    {   "boat",         ITEM_BOAT,      TRUE    },
    {   "npccorpse",        ITEM_CORPSE_NPC,    TRUE    },
    {   "pc corpse",        ITEM_CORPSE_PC,     FALSE   },
    {   "fountain",     ITEM_FOUNTAIN,      TRUE    },
    {   "pill",         ITEM_PILL,      TRUE    },
    {   "protect",      ITEM_PROTECT,       TRUE    },
    {   "map",          ITEM_MAP,       TRUE    },
    {   "portal",       ITEM_PORTAL,        TRUE    },
    {   "warpstone",        ITEM_WARP_STONE,    TRUE    },
    {   "resource",     ITEM_RESOURCE,  TRUE    },
    {   "roomkey",      ITEM_ROOM_KEY,      TRUE    },
    {   "gem",          ITEM_GEM,       TRUE    },
    {   "jewelry",      ITEM_JEWELRY,       TRUE    },
    {   "jukebox",      ITEM_JUKEBOX,       TRUE    },
    {   "trap",     ITEM_TRAP,      TRUE    },
    {   "tool",     ITEM_TOOL,      TRUE    },
    {   "crafted",      ITEM_CRAFTED,       TRUE    },
    {   NULL,           0,          0   }
};


const struct flag_type extra_flags[] =
{
    {   "glow",         ITEM_GLOW,      TRUE    },
    {   "hum",          ITEM_HUM,       TRUE    },
    {   "dark",         ITEM_DARK,      TRUE    },
    {   "hidden",       ITEM_HIDDEN,    TRUE    },
    {   "illuminate",         ITEM_ILLUMINATE,      TRUE    },
    {   "invis",        ITEM_INVIS,     TRUE    },
    {   "magic",        ITEM_MAGIC,     TRUE    },
    {   "nodrop",       ITEM_NODROP,        TRUE    },
    {   "bless",        ITEM_BLESS,     TRUE    },
//    {   "antigood",     ITEM_ANTI_GOOD,     TRUE    },
    {   "reflective",     ITEM_REFLECTIVE,     TRUE    },
//    {   "antineutral",      ITEM_ANTI_NEUTRAL,  TRUE    },
    {   "noremove",     ITEM_NOREMOVE,      TRUE    },
    {   "inventory",        ITEM_INVENTORY,     TRUE    },
    {   "nopurge",      ITEM_NOPURGE,       TRUE    },
    {   "rotdeath",     ITEM_ROT_DEATH,     TRUE    },
    {   "visdeath",     ITEM_VIS_DEATH,     TRUE    },
    {   "rare",     ITEM_RARE,      TRUE    },
    {   "nonmetal",     ITEM_NONMETAL,      TRUE    },
    {   "meltdrop",     ITEM_MELT_DROP,     TRUE    },
    {   "hadtimer",     ITEM_HAD_TIMER,     TRUE    },
    {   "sellextract",      ITEM_SELL_EXTRACT,  TRUE    },
    {   "burnproof",        ITEM_BURN_PROOF,    TRUE    },
    {   "nouncurse",        ITEM_NOUNCURSE,     TRUE    },
    {   "isstake",      ITEM_IS_STAKE,      TRUE    },
    {   "ispreserve",       ITEM_IS_PRESERVE,       TRUE    },
    {   "isenhanced",       ITEM_IS_ENHANCED,       TRUE    },
    {   "brew",         ITEM_BREW_POT,      TRUE    },
    {   "umbra",        ITEM_UMBRA,     TRUE    },
    {   NULL,           0,          0   }
};


const struct flag_type wear_flags[] =
{
    {   "take",         ITEM_TAKE,      TRUE    },
    {   "finger",       ITEM_WEAR_FINGER,   TRUE    },
    {   "neck",         ITEM_WEAR_NECK,     TRUE    },
    {   "body",         ITEM_WEAR_BODY,     TRUE    },
    {   "head",         ITEM_WEAR_HEAD,     TRUE    },
    {   "legs",         ITEM_WEAR_LEGS,     TRUE    },
    {   "feet",         ITEM_WEAR_FEET,     TRUE    },
    {   "hands",        ITEM_WEAR_HANDS,    TRUE    },
    {   "arms",         ITEM_WEAR_ARMS,     TRUE    },
    {   "shield",       ITEM_WEAR_SHIELD,   TRUE    },
    {   "about",        ITEM_WEAR_ABOUT,    TRUE    },
    {   "waist",        ITEM_WEAR_WAIST,    TRUE    },
    {   "wrist",        ITEM_WEAR_WRIST,    TRUE    },
    {   "wield",        ITEM_WIELD,     TRUE    },
    {   "hold",         ITEM_HOLD,      TRUE    },
    {   "face",         ITEM_WEAR_FACE, TRUE    },
    {   "ear",          ITEM_WEAR_EAR,  TRUE    },
    {   "nosac",        ITEM_NO_SAC,        TRUE    },
    {   "wearfloat",        ITEM_WEAR_FLOAT,    TRUE    },
/*    {   "twohands",            ITEM_TWO_HANDS,         TRUE    }, */
    {   NULL,           0,          0   }
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] =
{
    {   "none",         APPLY_NONE,     TRUE    },
    {   "strength",     APPLY_STR,      TRUE    },
    {   "dexterity",        APPLY_DEX,      TRUE    },
    {   "intelligence",     APPLY_INT,      TRUE    },
    {   "wisdom",       APPLY_WIS,      TRUE    },
    {   "constitution",     APPLY_CON,      TRUE    },
    {   "sex",          APPLY_SEX,      TRUE    },
    {   "class",        APPLY_CLASS,        TRUE    },
    {   "level",        APPLY_LEVEL,        TRUE    },
    {   "age",          APPLY_AGE,      TRUE    },
    {   "height",       APPLY_HEIGHT,       TRUE    },
    {   "weight",       APPLY_WEIGHT,       TRUE    },
    {   "mana",         APPLY_MANA,     TRUE    },
    {   "hp",           APPLY_HIT,      TRUE    },
    {   "move",         APPLY_MOVE,     TRUE    },
    {   "gold",         APPLY_GOLD,     TRUE    },
    {   "experience",       APPLY_EXP,      TRUE    },
    {   "ac",           APPLY_AC,       TRUE    },
    {   "hitroll",      APPLY_HITROLL,      TRUE    },
    {   "damroll",      APPLY_DAMROLL,      TRUE    },
    {   "saves",        APPLY_SAVES,        TRUE    },
    {   "savingpara",       APPLY_SAVING_PARA,  TRUE    },
    {   "savingrod",        APPLY_SAVING_ROD,   TRUE    },
    {   "savingpetri",      APPLY_SAVING_PETRI, TRUE    },
    {   "savingbreath",     APPLY_SAVING_BREATH,    TRUE    },
    {   "savingspell",      APPLY_SAVING_SPELL, TRUE    },
    {   "spellaffect",      APPLY_SPELL_AFFECT, FALSE   },
    {   "alignment",        APPLY_ALIGN,        TRUE    },
    {   "generation",       APPLY_GENERATION,       TRUE    },
    {   "cs_strength",      APPLY_CS_STR,   TRUE    },
    {   "cs_dexterity",     APPLY_CS_DEX,   TRUE    },
    {   "cs_stamina",       APPLY_CS_STA,   TRUE    },
    {   "cs_charisma",      APPLY_CS_CHA,   TRUE    },
    {   "cs_manipulation",  APPLY_CS_MAN,   TRUE    },
    {   "cs_appearance",    APPLY_CS_APP,   TRUE    },
    {   "cs_perception",    APPLY_CS_PER,   TRUE    },
    {   "cs_intelligence",  APPLY_CS_INT,   TRUE    },
    {   "cs_wits",          APPLY_CS_WIT,   TRUE    },
    {  "max_blood",     APPLY_MAX_BLOOD,    TRUE    },
    {   NULL,           0,          0   }
};



/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] =
{
    {   "in the inventory", WEAR_NONE,  TRUE    },
    {   "as a light",       WEAR_LIGHT, TRUE    },
    {   "on the left finger",   WEAR_FINGER_L,  TRUE    },
    {   "on the right finger",  WEAR_FINGER_R,  TRUE    },
    {   "around the neck (1)",  WEAR_NECK_1,    TRUE    },
    {   "around the neck (2)",  WEAR_NECK_2,    TRUE    },
    {   "in the left ear",  WEAR_EAR_L, TRUE    },
    {   "in the right ear", WEAR_EAR_R, TRUE    },
    {   "on the face",      WEAR_FACE,  TRUE    },
    {   "on the body",      WEAR_BODY,  TRUE    },
    {   "over the head",    WEAR_HEAD,  TRUE    },
    {   "on the legs",      WEAR_LEGS,  TRUE    },
    {   "on the feet",      WEAR_FEET,  TRUE    },
    {   "on the hands",     WEAR_HANDS, TRUE    },
    {   "on the arms",      WEAR_ARMS,  TRUE    },
    {   "as a shield",      WEAR_SHIELD,    TRUE    },
    {   "about the shoulders",  WEAR_ABOUT, TRUE    },
    {   "around the waist", WEAR_WAIST, TRUE    },
    {   "on the left wrist",    WEAR_WRIST_L,   TRUE    },
    {   "on the right wrist",   WEAR_WRIST_R,   TRUE    },
    {   "wielded",      WEAR_WIELD, TRUE    },
    {   "held in the hands",    WEAR_HOLD,  TRUE    },
    {   "floating nearby",  WEAR_FLOAT, TRUE    },
    {   NULL,           0         , 0   }
};


const struct flag_type wear_loc_flags[] =
{
    {   "none",     WEAR_NONE,  TRUE    },
    {   "light",    WEAR_LIGHT, TRUE    },
    {   "lfinger",  WEAR_FINGER_L,  TRUE    },
    {   "rfinger",  WEAR_FINGER_R,  TRUE    },
    {   "neck1",    WEAR_NECK_1,    TRUE    },
    {   "neck2",    WEAR_NECK_2,    TRUE    },
    {   "body",     WEAR_BODY,  TRUE    },
    {   "head",     WEAR_HEAD,  TRUE    },
    {   "legs",     WEAR_LEGS,  TRUE    },
    {   "feet",     WEAR_FEET,  TRUE    },
    {   "hands",    WEAR_HANDS, TRUE    },
    {   "arms",     WEAR_ARMS,  TRUE    },
    {   "shield",   WEAR_SHIELD,    TRUE    },
    {   "about",    WEAR_ABOUT, TRUE    },
    {   "waist",    WEAR_WAIST, TRUE    },
    {   "lwrist",   WEAR_WRIST_L,   TRUE    },
    {   "rwrist",   WEAR_WRIST_R,   TRUE    },
    {   "lear",     WEAR_EAR_L, TRUE    },
    {   "rear",     WEAR_EAR_R, TRUE    },
    {   "face",     WEAR_FACE,  TRUE    },
    {   "wielded",  WEAR_WIELD, TRUE    },
    {   "hold",     WEAR_HOLD,  TRUE    },
    {   "floating", WEAR_FLOAT, TRUE    },
    {   NULL,       0,      0   }
};

const struct flag_type container_flags[] =
{
    {   "closeable",        1,      TRUE    },
    {   "pickproof",        2,      TRUE    },
    {   "closed",       4,      TRUE    },
    {   "locked",       8,      TRUE    },
    {   "puton",        16,     TRUE    },
    {   NULL,           0,      0   }
};

/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/




const struct flag_type ac_type[] =
{
    {   "pierce",        AC_PIERCE,            TRUE    },
    {   "bash",          AC_BASH,              TRUE    },
    {   "slash",         AC_SLASH,             TRUE    },
    {   "exotic",        AC_EXOTIC,            TRUE    },
    {   NULL,              0,                    0       }
};


const struct flag_type size_flags[] =
{
    {   "tiny",          SIZE_TINY,            TRUE    },
    {   "small",         SIZE_SMALL,           TRUE    },
    {   "medium",        SIZE_MEDIUM,          TRUE    },
    {   "large",         SIZE_LARGE,           TRUE    },
    {   "huge",          SIZE_HUGE,            TRUE    },
    {   "giant",         SIZE_GIANT,           TRUE    },
    {   NULL,              0,                    0       },
};


const struct flag_type weapon_class[] =
{
    {   "exotic",   WEAPON_EXOTIC,      TRUE    },
    {   "sword",    WEAPON_SWORD,       TRUE    },
    {   "dagger",   WEAPON_DAGGER,      TRUE    },
    {   "spear",    WEAPON_SPEAR,       TRUE    },
    {   "mace",     WEAPON_MACE,        TRUE    },
    {   "axe",      WEAPON_AXE,     TRUE    },
    {   "flail",    WEAPON_FLAIL,       TRUE    },
    {   "whip",     WEAPON_WHIP,        TRUE    },
    {   "polearm",  WEAPON_POLEARM,     TRUE    },
    {   "lance",    WEAPON_LANCE,       TRUE    },
    {   NULL,       0,          0       }
};


const struct flag_type weapon_type2[] =
{
    {   "flaming",       WEAPON_FLAMING,       TRUE    },
    {   "frost",         WEAPON_FROST,         TRUE    },
    {   "vampiric",      WEAPON_VAMPIRIC,      TRUE    },
    {   "sharp",         WEAPON_SHARP,         TRUE    },
    {   "vorpal",        WEAPON_VORPAL,        TRUE    },
    {   "twohands",     WEAPON_TWO_HANDS,     TRUE    },
    {   "shocking",  WEAPON_SHOCKING,      TRUE    },
    {   "poison",   WEAPON_POISON,      TRUE    },
    {   "slow", WEAPON_SLOW,        TRUE    },
    {   "agg_damage",WEAPON_AGG_DAMAGE, TRUE    },
    {   NULL,              0,                    0       }
};

const struct flag_type res_flags[] =
{
    {   "summon",    RES_SUMMON,        TRUE    },
    {   "charm",         RES_CHARM,            TRUE    },
    {   "magic",         RES_MAGIC,            TRUE    },
    {   "weapon",        RES_WEAPON,           TRUE    },
    {   "bash",          RES_BASH,             TRUE    },
    {   "pierce",        RES_PIERCE,           TRUE    },
    {   "slash",         RES_SLASH,            TRUE    },
    {   "fire",          RES_FIRE,             TRUE    },
    {   "cold",          RES_COLD,             TRUE    },
    {   "lightning",     RES_LIGHTNING,        TRUE    },
    {   "acid",          RES_ACID,             TRUE    },
    {   "poison",        RES_POISON,           TRUE    },
    {   "negative",      RES_NEGATIVE,         TRUE    },
    {   "holy",          RES_HOLY,             TRUE    },
    {   "energy",        RES_ENERGY,           TRUE    },
    {   "mental",        RES_MENTAL,           TRUE    },
    {   "disease",       RES_DISEASE,          TRUE    },
    {   "drowning",      RES_DROWNING,         TRUE    },
    {   "light",         RES_LIGHT,            TRUE    },
    {   "sound",    RES_SOUND,      TRUE    },
    {   "wood",     RES_WOOD,       TRUE    },
    {   "silver",   RES_SILVER,     TRUE    },
    {   "iron",     RES_IRON,       TRUE    },
    {   "sunlight",     RES_SUNLIGHT,       TRUE    },
    {   NULL,          0,            0    }
};


const struct flag_type vuln_flags[] =
{
    {   "summon",    VULN_SUMMON,       TRUE    },
    {   "charm",    VULN_CHARM,     TRUE    },
    {   "magic",         VULN_MAGIC,           TRUE    },
    {   "weapon",        VULN_WEAPON,          TRUE    },
    {   "bash",          VULN_BASH,            TRUE    },
    {   "pierce",        VULN_PIERCE,          TRUE    },
    {   "slash",         VULN_SLASH,           TRUE    },
    {   "fire",          VULN_FIRE,            TRUE    },
    {   "cold",          VULN_COLD,            TRUE    },
    {   "lightning",     VULN_LIGHTNING,       TRUE    },
    {   "acid",          VULN_ACID,            TRUE    },
    {   "poison",        VULN_POISON,          TRUE    },
    {   "negative",      VULN_NEGATIVE,        TRUE    },
    {   "holy",          VULN_HOLY,            TRUE    },
    {   "energy",        VULN_ENERGY,          TRUE    },
    {   "mental",        VULN_MENTAL,          TRUE    },
    {   "disease",       VULN_DISEASE,         TRUE    },
    {   "drowning",      VULN_DROWNING,        TRUE    },
    {   "light",         VULN_LIGHT,           TRUE    },
    {   "sound",     VULN_SOUND,        TRUE    },
    {   "wood",          VULN_WOOD,            TRUE    },
    {   "silver",        VULN_SILVER,          TRUE    },
    {   "iron",          VULN_IRON,            TRUE    },
    {   "sunlight",          VULN_SUNLIGHT,            TRUE    },
    {   NULL,              0,                    0       }
};

const struct flag_type position_flags[] =
{
    {   "dead",           POS_DEAD,            FALSE   },
    {   "torpor",         POS_TORPOR,          FALSE   },
    {   "mortal",         POS_MORTAL,          FALSE   },
    {   "incap",          POS_INCAP,           FALSE   },
    {   "stunned",        POS_STUNNED,         FALSE   },
    {   "sleeping",       POS_SLEEPING,        TRUE    },
    {   "resting",        POS_RESTING,         TRUE    },
    {   "sitting",        POS_SITTING,         TRUE    },
    {   "fighting",       POS_FIGHTING,        FALSE   },
    {   "standing",       POS_STANDING,        TRUE    },
    {   NULL,              0,                    0       }
};

const struct flag_type portal_flags[]=
{
    {   "normal_exit",    GATE_NORMAL_EXIT, TRUE    },
    {   "no_curse",   GATE_NOCURSE,     TRUE    },
    {   "go_with",    GATE_GOWITH,      TRUE    },
    {   "buggy",      GATE_BUGGY,       TRUE    },
    {   "random",     GATE_RANDOM,      TRUE    },
	{   "umbra", 	  GATE_UMBRA, 		TRUE 	},
    {   NULL,         0,            0   }
};

const struct flag_type furniture_flags[]=
{
    {   "stand_at",   STAND_AT,     TRUE    },
    {   "stand_on",   STAND_ON,     TRUE    },
    {   "stand_in",   STAND_IN,     TRUE    },
    {   "sit_at",     SIT_AT,       TRUE    },
    {   "sit_on",     SIT_ON,       TRUE    },
    {   "sit_in",     SIT_IN,       TRUE    },
    {   "rest_at",    REST_AT,      TRUE    },
    {   "rest_on",    REST_ON,      TRUE    },
    {   "rest_in",    REST_IN,      TRUE    },
    {   "sleep_at",   SLEEP_AT,     TRUE    },
    {   "sleep_on",   SLEEP_ON,     TRUE    },
    {   "sleep_in",   SLEEP_IN,     TRUE    },
    {   "put_at",     PUT_AT,       TRUE    },
    {   "put_on",     PUT_ON,       TRUE    },
    {   "put_in",     PUT_IN,       TRUE    },
    {   "put_inside",     PUT_INSIDE,       TRUE    },
    {   NULL,         0,            0   }
};

const struct flag_type trap_flags[]=
{
    {   "bash",   TRAP_BASH,    TRUE    },
    {   "pierce",     TRAP_PIERCE,  TRUE    },
    {   "slash",      TRAP_SLASH,   TRUE    },
    {   "fire",   TRAP_FIRE,    TRUE    },
    {   "cold",   TRAP_COLD,    TRUE    },
    {   "lightning",  TRAP_LIGHTNING,   TRUE    },
    {   "acid",   TRAP_ACID,    TRUE    },
    {   "poison",     TRAP_POISON,  TRUE    },
    {   "holy",   TRAP_HOLY,    TRUE    },
    {   "energy",     TRAP_ENERGY,  TRUE    },
    {   "mental",     TRAP_MENTAL,  TRUE    },
    {   "drowning",   TRAP_DROWNING,    TRUE    },
    {   "light",      TRAP_LIGHT,   TRUE    },
    {   "harm",   TRAP_HARM,    TRUE    },
    {   "sound",      TRAP_SOUND,   TRUE    },
    {   "sunlight",   TRAP_SUNLIGHT,    TRUE    },
    {   "blind",      TRAP_BLIND,   TRUE    },
    {   "faerie_fire",TRAP_FAERIE_FIRE,TRUE },
    {   "curse",      TRAP_CURSE,   TRUE    },
    {   "sleep",      TRAP_SLEEP,   TRUE    },
    {   "calm",   TRAP_CALM,    TRUE    },
    {   "weaken",     TRAP_WEAKEN,  TRUE    },
    {   "slow",   TRAP_SLOW,    TRUE    },
    {   "death",      TRAP_DEATH,   TRUE    },
    {   NULL,         0,            0   }
};


const struct flag_type trigger_flags[]=
{
    { "north",    TRIGGER_NORTH,    TRUE    },
    {   "south",      TRIGGER_SOUTH,    TRUE    },
    {   "east",   TRIGGER_EAST, TRUE    },
    {   "west",   TRIGGER_WEST, TRUE    },
    {   "up",         TRIGGER_UP,   TRUE    },
    {   "down",   TRIGGER_DOWN, TRUE    },
    {   "all",    TRIGGER_ALL,  TRUE    },
    {   "enter",      TRIGGER_ENTER,    TRUE    },
    {   "get",    TRIGGER_GET,  TRUE    },
    {   "put",    TRIGGER_PUT,  TRUE    },
    {   "drop",   TRIGGER_DROP, TRUE    },
    {   "open",   TRIGGER_OPEN, TRUE    },
    {   NULL,         0,            0   }
};

const struct flag_type focus_flags[]=
{
    {   "single",     FOCUS_SINGLE, TRUE    },
    {   "group",      FOCUS_GROUP,  TRUE    },
    {   "room",   FOCUS_ROOM,   TRUE    },
    {   NULL,         0,            0   }
};




const   struct  flag_type   apply_types []  =
{
    {   "affects1", TO_AFFECTS, TRUE    },
    {   "affects2", TO_AFFECTS2,    TRUE    },
    {   "object",   TO_OBJECT,  TRUE    },
    {   "immune",   TO_IMMUNE,  TRUE    },
    {   "resist",   TO_RESIST,  TRUE    },
    {   "vuln",     TO_VULN,    TRUE    },
    {   "weapon",   TO_WEAPON,  TRUE    },
    {   NULL,       0,      TRUE    }
};

const   struct  bit_type    bitvector_type  []  =
{
    {   affect_flags,   "affect"    },
    {   affect2_flags,  "affect2"   },
    {   apply_flags,    "apply"     },
    {   imm_flags,  "imm"       },
    {   res_flags,  "res"       },
    {   vuln_flags, "vuln"      },
    {   weapon_type2,   "weapon"    }
};

const struct    brew_type   brew_table  [MAX_BREW]  =
{
/*  {   name,       spell number,   min level,  potion level,
        diff,       {component1,    component2, component3,
        component4, component5},    mage only,  imm only}, */

    {   "reserved",     0,  0,  0,  0,  {0, 0,
        0,      0,  0}, FALSE,  FALSE,  "none"},
    {   "cure light",   16, 1,  1,  1,  {612, 0,
        0,      0,  0}, FALSE,  FALSE,  "light blue"},
    {   "cure serious", 61, 5,  1,  1,  {5100, 612,
        0,      0,  0}, FALSE,  FALSE,  "blue"},
    {   "cure critical",    15, 10, 1,  1,  {612,   5101,
        20, 0,  0}, FALSE,  FALSE,  "dark blue"},
    {   "heal",     28, 25, 1,  1,  {13,    612,
        5005,       20, 0}, FALSE,  FALSE,  "purple"},
    {   "cure blindness",   14, 20, 3,  1,  {5100, 42,
        0,      0,  0}, FALSE,  FALSE,  "bluish-green"},
    {   "cure poison",  43, 25, 3,  1,  {613,   5102,
        0,      0,  0}, FALSE,  FALSE,  "bluish-red"},
    {   "remove curse", 35, 50, 2,  1,  {6010, 9560,
        0,      0,  0}, FALSE,  FALSE,  "bright blue"},
    {   "refresh",      81, 5,  2,  1,  {5004, 0,
        0,      0,  0}, FALSE,  FALSE,  "glowing blue"},
    {   "mana",     0,  50, 1,  1,  {5100,  5101,
        5102,        5103,   5104},   TRUE,       FALSE,  "gold"},
    {   "giant strength",   39, 20, 1,  1,  {9225,  100,
        0,      0,  0}, TRUE,       FALSE,  "light green"},
    {   "cancellation", 507,    50, 1,  1,  {9225,  5102,
        5103,       5104,   0}, TRUE,       FALSE,  "green"},
    {   "armor",        1,  25, 1,  1,  {9225,  1513,
        5105,       0,  0}, TRUE,       FALSE,  "dark green"},
    {   "fly",      56, 60, 1,  1,  {9225,  11692,
        11106,  0,  0}, TRUE,       FALSE,  "yellow"},
    {   "frenzy",       504,    60, 1,  1,  {9225,  302,
        17701,  0,  0}, TRUE,       FALSE,  "dull yellow"},
    {   "haste",        502,    60, 1,  1,  {9225,  8903,
        5106,       0,  0}, TRUE,       FALSE,  "bright yellow"},
    {   "shield",       67, 40, 1,  1,  {9225,  5107,
        0,      0,  0}, TRUE,       FALSE,  "reddish-yellow"},
    {   "detect evil",  18, 10, 2,  1,  {9225,  0,
        0,      0,  0}, TRUE,       FALSE,  "light green"},
    {   "detect good",  513,    10, 2,  1,  {9225,  0,
        0,      0,  0}, TRUE,       FALSE,  "green"},
    {   "detect magic", 20, 20, 2,  1,  {9225,  0,
        0,      0,  0}, TRUE,       FALSE,  "dull green"},
    {   "pass door",    74, 80, 1,  1,  {9225,  26073,
        10775,  5107,   5101},  TRUE,   FALSE,  "misty green"},
    {   "protection evil",34,   40, 1,  1,  {9225,  3088,
        0,      0,  0}, TRUE,       FALSE,  "shimmering yellow"},
    {   "protection good",514,  40, 1,  1,  {9225,  3088,
        10757,  0,  0}, TRUE,       FALSE,  "greenish-yellow"},
    {   "stone skin",   66, 70, 1,  1,  {9225,  9221,
        17030,  5239,   3380},  TRUE,       FALSE,  "brownish-yellow"}
};

const struct    gift_type   gift_table  [MAX_GIFTS_CODED]  =
{
/*  {   name,       min level,  lag delt,   chance to fail,
        breed required,     auspice required,   tribe required,
        renown required, rage required}, */
// Smell of Man excluded from table.
//      name                      		level  lag fail   breed   auspice   tribe    rank   rage
    {   NULL,                       	0,     0,  0,     0,      0,          0,        0,   0},
    {   "Staredown",                	1,     24, 0,     HOMID,  0,          0,        2,   0},
    {   "Gift of the Porcupine",    	1,     24, 0,     METIS,  0,          0,        4,   0},
    {   "Blissful Ignorance",       	1,     24, 0,     0,      RAGABASH,   0,        2,   0},
    {   "Pulse of the Invisible",   	1,     24, 0,     0,      THEURGE,    0,        3,   0},
    {   "Catfeet",                  	1,     24, 0,     LUPUS,  0,          0,        5,   0},
    {   "Resist Pain",              	1,     24, 0,     0,      PHILODOX,   CHILDREN_OF_GAIA, 1,   0},//Fenrir
    {   "Call of the Wyld",         	1,     24, 0,     0,      GALLIARD,   0,        1,   0},
    {   "Razor Claws",              	1,     24, 0,     0,      AHROUN,     FENRIR,        1,   0},
    {   "Silver Claws",             	1,     24, 0,     0,      AHROUN,     SILVER_FANG,   3,   0},
    {   "Sense Wyrm",         	     	1,     24, 0,     METIS,  THEURGE,    BLACK_FURY, 1,   0},//Silver Fang
    {   "Inner Strength",    	     	1,     24, 0,     0,      0,          0,        5,   0},
    {   "Bloody Feast",             	1,     24, 0,     0,      0,          0,        5,   0},
    {   "Master of Fire",           	1,     24, 0,     HOMID,  0,          0,        1,   0},
    {   "persuasion", 					1,     24, 0,     HOMID,  0,          WARDERSOFMEN, 1,   0},
    {   "speech of the world", 			0,     24, 0,     HOMID,  0,          0,        2,   0},
    {   "calm the savage beast", 		0,     24, 0,     HOMID,  0,          0,        3,   0},
    {   "reshape object", 				0,     24, 0,     HOMID,  0,          WARDERSOFMEN, 3,   0},
    {   "bury the wolf", 				0,     24, 0,     HOMID,  0,          0,        4,   0},
    {   "spirit ward", 					0,     24, 0,     HOMID,  0,          0,        4,   0},
    {   "assimilation", 				0,     24, 0,     HOMID,  0,          0,        5,   0},
    {   "false comfort", 				0,     24, 0,     HOMID,  0,          WARDERSOFMEN, 5,   0},
    {   "create element", 				1,     24, 0,     METIS,  0,          0,        1,   0},
    {   "shed", 						1,     24, 0,     METIS,  0,          0,        1,   0},
    {   "curse of hatred", 				0,     24, 0,     METIS,  0,          0,        2,   0},
    {   "sense silver", 				0,     24, 0,     METIS,  0,          0,        2,   0},
    {   "eyes of the cat", 				0,     24, 0,     METIS,  0,          0,        3,   0},
    {   "mental speech", 				0,     24, 0,     METIS,  0,          0,        3,   0},
    {   "wither limb", 					0,     24, 0,     METIS,  0,          0,        4,   0},
    {   "madness", 						0,     24, 0,     METIS,  0,          0,        5,   0},
    {   "totem gift", 					0,     24, 0,     METIS,  0,          0,        5,   0},
    {   "hares leap", 					1,     24, 0,     LUPUS,  0,          0,        1,   0},
    {   "heightened senses", 			1,     24, 0,     LUPUS,  0,          0,        1,   0},
    {   "ley lines", 					1,     24, 0,     LUPUS,  0,          0,        1,   0},
    {   "scent of sight", 				0,     24, 0,     LUPUS,  0,          0,        2,   0},
    {   "sense the unnatural", 			0,     24, 0,     LUPUS,  0,          0,        2,   0},
    {   "devils child", 				0,     24, 0,     LUPUS,  0,          0,        3,   0},
    {   "name the spirit", 				0,     24, 0,     LUPUS,  0,          0,        3,   0},
    {   "beast life", 					0,     24, 0,     LUPUS,  0,          CHILDREN_OF_GAIA, 4,   0},
    {   "gnaw", 						0,     24, 0,     LUPUS,  0,          0,        4,   0},
    {   "elemental gift", 				0,     24, 0,     LUPUS,  0,          0,        5,   0},
    {   "song of the great beast", 		0,     24, 0,     LUPUS,  0,          0,        5,   0},
    {   "blur of the milky eye", 		1,     24, 0,     0,      RAGABASH,   0,        1,   0},
    {   "scent of running water", 		0,     24, 0,     0,      RAGABASH,   0,        1,   0},
    {   "snow running", 				1,     24, 0,     0,      RAGABASH,   0,        1,   0},
    {   "sense of prey", 				0,     24, 0,     0,      RAGABASH,   BLACK_FURY, 2,   0},
    {   "open moon bridge", 			0,     24, 0,     0,      RAGABASH,   0,        3,   0},
    {   "reynards lie", 				0,     24, 0,     0,      RAGABASH,   0,        3,   0},
    {   "lepers curse", 				0,     24, 0,     0,      RAGABASH,   0,        4,   0},
    {   "lunas blessing", 				0,     24, 0,     0,      RAGABASH,   0,        4,   0},
    {   "thieving talons of the magpie",0,     24, 0,     0,      RAGABASH,   0,        5,   0},
    {   "chimerical form", 				0,     24, 0,     0,      RAGABASH,   0,        5,   0},
    {   "mothers touch", 				1,     24, 0,     0,      THEURGE,    CHILDREN_OF_GAIA, 1,   0},
    {   "spirit speech", 				0,     24, 0,     0,      THEURGE,    0,        1,   0},
    {   "command spirit", 				0,     24, 0,     0,      THEURGE,    0,        2,   0},
    {   "sight from beyond", 			0,     24, 0,     0,      THEURGE,    0,        2,   0},
    {   "expel spirit", 				0,     24, 0,     0,      THEURGE,    0,        3,   0},
    {   "grasp the beyond", 			0,     24, 0,     0,      THEURGE,    0,        4,   0},
    {   "spirit wassail", 				0,     24, 0,     0,      THEURGE,    0,        4,   0},
    {   "nightmare possession", 		0,     24, 0,     0,      THEURGE,    0,        5,   0},
    {   "malleable spirit", 			0,     24, 0,     0,      THEURGE,    0,        5,   0},
    {   "scent of the true form", 		0,     24, 0,     0,      PHILODOX,   0,        1,   0},
    {   "truth of gaia", 				0,     24, 0,     0,      PHILODOX,   0,        1,   0},
    {   "call of duty", 				0,     24, 0,     0,      PHILODOX,   0,        2,   0},
    {   "strength of purpose", 			0,     24, 0,     0,      PHILODOX,   0,        2,   0},
    {   "spirit witness", 				0,     24, 0,     0,      PHILODOX,   0,        3,   0},
    {   "warriors eye", 				0,     24, 0,     0,      PHILODOX,   0,        3,   0},
    {   "roll over", 					0,     24, 0,     0,      PHILODOX,   0,        4,   0},
    {   "scent of beyond", 				0,     24, 0,     0,      PHILODOX,   0,        4,   0},
    {   "earthfriends pact", 			0,     24, 0,     0,      PHILODOX,   0,        5,   0},
    {   "geas", 						0,     24, 0,     0,      PHILODOX,   0,        5,   0},
    {   "beast speech", 				0,     24, 0,     0,      GALLIARD,   0,        1,   0},
    {   "walking dream", 				0,     24, 0,     0,      GALLIARD,   0,        1,   0},
    {   "call of the wyrm", 			0,     24, 0,     0,      GALLIARD,   0,        2,   0},
    {   "distractions", 				0,     24, 0,     0,      GALLIARD,   0,        2,   0},
    {   "eye of the asp", 				0,     24, 0,     0,      GALLIARD,   0,        3,   0},
    {   "song of rage", 				0,     24, 0,     0,      GALLIARD,   0,        3,   0},
    {   "shadows by the firelight",		0,     24, 0,     0,      GALLIARD,   0,        4,   0},
    {   "song of mockery", 				0,     24, 0,     0,      GALLIARD,   0,        4,   0},
    {   "dream golems", 				0,     24, 0,     0,      GALLIARD,   0,        5,   0},
    {   "call for vengeance", 			0,     24, 0,     0,      GALLIARD,   0,        5,   0},
    {   "falling touch", 				1,     24, 0,     0,      AHROUN,     0,        1,   0},
    {   "inspiration", 					1,     24, 0,     0,      AHROUN,     0,        1,   0},
    {   "spirit of the fray", 			0,     24, 0,     0,      AHROUN,     0,        2,   0},
    {   "true fear", 					0,     24, 0,     0,      AHROUN,     0,        2,   0},
    {   "heart of fury", 				0,     24, 0,     0,      AHROUN,     0,        3,   0},
    {   "full moons light", 			0,     24, 0,     0,      AHROUN,     0,        4,   0},
    {   "stoking furys furnace", 		0,     24, 0,     0,      AHROUN,     0,        4,   0},
    {   "kiss of helios", 				0,     24, 0,     0,      AHROUN,     0,        5,   0},
    {   "strength of will", 			0,     24, 0,     0,      AHROUN,     0,        5,   0},
    {   "breath of the wyld", 			0,     24, 0,     0,      0,          BLACK_FURY, 1,   0},
    {   "mans skin", 					0,     24, 0,     0,      0,          BLACK_FURY, 1,   0},
    {   "curse of the aeolus", 			0,     24, 0,     0,      0,          BLACK_FURY, 2,   0},
    {   "coup de grace", 				0,     24, 0,     0,      0,          BLACK_FURY, 3,   0},
    {   "flames of hestia", 			0,     24, 0,     0,      0,          BLACK_FURY, 3,   0},
    {   "body wrack", 					0,     24, 0,     0,      0,          BLACK_FURY, 4,   0},
    {   "bow of pegasus", 				0,     24, 0,     0,      0,          BLACK_FURY, 4,   0},
    {   "gaze of the gorgon", 			0,     24, 0,     0,      0,          BLACK_FURY, 5,   0},
    {   "wyld warp", 					0,     24, 0,     0,      0,          BLACK_FURY, 5,   0},
    {   "cookery", 						0,     24, 0,     0,      0,          BONE_GNAWER, 1,   0},
    {   "resist toxin", 				0,     24, 0,     0,      0,          BONE_GNAWER, 1,   0},//Fianna
    {   "scent of the honeycomb", 		0,     24, 0,     0,      0,          BONE_GNAWER, 1,   0},
    {   "cornered rat", 				0,     24, 0,     0,      0,          BONE_GNAWER, 2,   0},
    {   "plague visage", 				0,     24, 0,     0,      0,          BONE_GNAWER, 2,   0},
    {   "call the rust", 				0,     24, 0,     0,      0,          BONE_GNAWER, 3,   0},
    {   "friend in need", 				0,     24, 0,     0,      0,          BONE_GNAWER, 3,   0},
    {   "blink", 						0,     24, 0,     0,      0,          BONE_GNAWER, 4,   0},
    {   "claws of the glass blower",	0,     24, 0,     0,      0,          BONE_GNAWER, 4,   0},
    {   "maw of the whale", 			0,     24, 0,     0,      0,          BONE_GNAWER, 5,   0},
    {   "survivor", 					0,     24, 0,     0,      0,          BONE_GNAWER, 5,   0},
    {   "eves blessing", 				0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 1,   0},
    {   "grandmothers touch", 			0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 2,   0},
    {   "lunas armor", 					0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 2,   0}, //Shadow Lord and Silver Fang
    {   "dazzle", 						0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 3,   0},
    {   "spirit friend", 				0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 3,   0},
    {   "angels semblance", 			0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 4,   0},
    {   "halo of the sun", 				0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 5,   0},
    {   "the living wood", 				0,     24, 0,     0,      0,          CHILDREN_OF_GAIA, 5,   0},
    {   "hunters harmony", 				0,     24, 0,     0,      0,          FENRIR,   1,   0},
    {   "visage of fenris", 			0,     24, 0,     0,      0,          FENRIR,   1,   0},
    {   "halt the cowards flight", 		0,     24, 0,     0,      0,          FENRIR,   2,   0},
    {   "sense guilt", 					0,     24, 0,     0,      0,          FENRIR,   2,   0},
    {   "might of thor", 				0,     24, 0,     0,      0,          FENRIR,   3,   0},
    {   "berserkers song", 				0,     24, 0,     0,      0,          FENRIR,   4,   0},
    {   "iron cannot pierce", 			0,     24, 0,     0,      0,          FENRIR,   4,   0},
    {   "blaze of glory", 				0,     24, 0,     0,      0,          FENRIR,   5,   0},
    {   "horde of valhalla", 			0,     24, 0,     0,      0,          FENRIR,   5,   0},
    {   "distract the fool", 			0,     24, 0,     0,      0,          FIANNA,   1,   0},
    {   "faerie light", 				0,     24, 0,     0,      0,          FIANNA,   1,   0},
    {   "howl of the banshee", 			0,     24, 0,     0,      0,          FIANNA,   2,   0},
    {   "warriors draught", 			0,     24, 0,     0,      0,          FIANNA,   2,   0},
    {   "faerie aid", 					0,     24, 0,     0,      0,          FIANNA,   3,   0},
    {   "woadling", 					0,     24, 0,     0,      0,          FIANNA,   3,   0},
    {   "balors gaze", 					0,     24, 0,     0,      0,          FIANNA,   4,   0},
    {   "semblance", 					0,     24, 0,     0,      0,          FIANNA,   4,   0},
    {   "fog on the moor", 				0,     24, 0,     0,      0,          FIANNA,   5,   0},
    {   "gift of the spriggan", 		0,     24, 0,     0,      0,          FIANNA,   5,   0},
    {   "aura of confidence", 			0,     24, 0,     0,      0,          SHADOW_LORD, 1,   0},
    {   "fatal flaw", 					0,     24, 0,     0,      0,          SHADOW_LORD, 1,   0},
    {   "seizing the edge", 			0,     24, 0,     0,      0,          SHADOW_LORD, 1,   0},
    {   "clap of thunder", 				0,     24, 0,     0,      0,          SHADOW_LORD, 2,   0},
    {   "ravens curse", 				0,     24, 0,     0,      0,          SHADOW_LORD, 3,   0},
    {   "shadow cutting", 				0,     24, 0,     0,      0,          SHADOW_LORD, 3,   0},
    {   "dolorous stroke", 				0,     24, 0,     0,      0,          SHADOW_LORD, 4,   0},
    {   "strength of the dominator", 	0,     24, 0,     0,      0,          SHADOW_LORD, 4,   0},
    {   "shadow pack", 					0,     24, 0,     0,      0,          SHADOW_LORD, 5,   0},
    {   "obedience", 					0,     24, 0,     0,      0,          SHADOW_LORD, 5,   0},
    {   "eye of the falcon", 			0,     24, 0,     0,      0,          SILVER_FANG, 1,   0},
    {   "lambent fire", 				1,     24, 0,     0,      0,          SILVER_FANG, 1,   0},
    {   "empathy", 						0,     24, 0,     0,      0,          SILVER_FANG, 2,   0},
    {   "exceptional wordplay", 		0,     24, 0,     0,      0,          SILVER_FANG, 3,   0},
    {   "wrath of gaia", 				0,     24, 0,     0,      0,          SILVER_FANG, 3,   0},
    {   "lordly will", 					0,     24, 0,     0,      0,          SILVER_FANG, 4,   0},
    {   "sidestep death", 				0,     24, 0,     0,      0,          SILVER_FANG, 4,   0},
    {   "lunas avenger", 				0,     24, 0,     0,      0,          SILVER_FANG, 5,   0},
    {   "paws of the newborn cub", 		0,     24, 0,     0,      0,          SILVER_FANG, 5,   0},
    {   "artisans command", 			0,     24, 0,     0,      0,          WARDERSOFMEN, 1,   0},
    {   "gift of salt", 				0,     24, 0,     0,      0,          WARDERSOFMEN, 1,   0},
    {   "mask the predator", 			0,     24, 0,     0,      0,          WARDERSOFMEN, 2,   0},
    {   "tongues", 						0,     24, 0,     0,      0,          WARDERSOFMEN, 3,   0},
    {   "attunement", 					0,     24, 0,     0,      0,          WARDERSOFMEN, 4,   0},
    {   "doppelganger", 				0,     24, 0,     0,      0,          WARDERSOFMEN, 4,   0},
    {   "command the blaze",			0,     24, 0,     0,      0,          WARDERSOFMEN, 5,   0},
    {   "redirect pain",                0,     24, 0,     0,      0,          FENRIR,   3,   0},
    {   NULL,                        	0,       0,  0,     0,      0,          0,      0,   0}
    };

const struct    cs_type ability_table[30] =
{
    {   "expression"        },
    {   "alertness"     },
    {   "athletics"     },
    {   "brawl"         },
    {   "dodge"         },
    {   "empathy"       },
    {   "intimidation"  },
    {   "leadership"    },
    {   "legerdemain"    },
    {   "subterfuge"    },
    {   "animal_ken"    },
    {   "riding"        },
    {   "etiquette"     },
    {   "archery"       },
    {   "melee"         },
    {   "performance"         },
    {   "crafts"        },
    {   "commerce"      },
    {   "stealth"       },
    {   "survival"      },
    {   "academics"     },
    {   "theology"      },
    {   "seneschal"     },
    {   "investigation" },
    {   "law"           },
    {   "linguistics"   },
    {   "medicine"      },
    {   "occult"        },
    {   "politics"      },
    {   "hearth_wisdom"       }

};

const struct    cs_type2 sec_abil_table[30] =
{
    {   "default",      0, 0, 0, 0, 0,  1},
    {   "mining",       0, 0, CSABIL_CRAFTS, 0, 0,  2},
    {   "logging",      CSABIL_CARVING, 0, 0, 0, 0, 3},
    {   "carving",      0, CSABIL_CRAFTS, 0, 0, 0,  2},
    {   "prospecting",  CSABIL_MINING, 0, 0, 0, 0,  3},
    {   "mineral",      0, CSABIL_MINING, 0, 0, 0,  3},
    {   "metal",        0, 0, CSABIL_MINING, 0, 0,  3},
    {   "precious",     0, 0, 0, CSABIL_MINING, 0,  3},
    {   "wood",         0, CSABIL_CARVING, 0, 0, 0, 3},
    {   "bone",         0, 0, CSABIL_CARVING, 0, 0, 3},
    {   "stone",        0, 0, 0, CSABIL_CARVING, 0, 3},
    {   "enigmas",      0, 0, 0, 0, 0,  1},
    {   "rituals",         0, 0, 0, 0, 0,  1},
    {   "meditation",         0, 0, 0, 0, 0,  1},
    {   "body crafts",         0, 0, 0, 0, 0,  1},
    {   "light weaponry",         CSABIL_MELEE, 0, 0, 0, 0,  1},
    {   "blunt weaponry",         CSABIL_MELEE, 0, 0, 0, 0,  1},
    {   "heavy weaponry",         CSABIL_MELEE, 0, 0, 0, 0,  1},
    {   "polearms",         CSABIL_MELEE, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1},
    {   "none",         0, 0, 0, 0, 0,  1}
};

const struct    cs_type attribute_table[9] =
{
    {   "strength"      },
    {   "dexterity"     },
    {   "stamina"       },
    {   "charisma"      },
    {   "manipulation"  },
    {   "appearance"    },
    {   "perception"    },
    {   "intelligence"  },
    {   "wits"          }
};

const struct    disc_type   sphere_table [MAX_SPHERE+1] =
{
    {  "NULL"       },
    {"correspondence"},
    {"entropy"},
    {"forces"},
    {"life"},
    {"matter"},
    {"mind"},
    {"prime"},
    {"spirit"},
    {"time"}
};


const struct    disc_type   disc_table  [MAX_DISC]  =
{
    {   "NULL"      },
    {   "Animalism" },
    {   "Auspex"    },
    {   "Protean"   },
    {   "Celerity"  },
    {   "Fortitude" },
    {   "Potence"   },
    {   "Obfuscate" },
    {   "Obtenebration" },
    {   "Dominate"  },
    {   "Presence"  },
    {   "Mortis"    },
    {   "Thaumaturgy"   },
    {   "Quietus"   },
    {   "Vicissitude"   },
    {   "Chimerstry"},
    {   "Dementation"  },
    {   "Serpentis"  },
    {   ""  },
};


const struct    csback_type csback_table    [MAX_CSBACK]    =
{
/*
#define CSBACK_NORACE           0
#define CSBACK_VAMPIRE          1
#define CSBACK_WEREWOLF         2
#define CSBACK_MAGE             3
*/
    {   "Allies", CSBACK_NORACE },
    {   "Contacts", CSBACK_NORACE },
    {   "Fame", CSBACK_NORACE   },
    {   "Generation", CSBACK_VAMPIRE    },
    {   "Herd", CSBACK_VAMPIRE  },
    {   "Influence", CSBACK_NORACE  },
    {   "Mentor", CSBACK_NORACE },
    {   "Resources", CSBACK_NORACE },
    {   "Retainers", CSBACK_NORACE },
    {   "Status", CSBACK_VAMPIRE },
    {   "Rituals", CSBACK_VAMPIRE },
    {   "Library", CSBACK_MAGE },
    {   "Fount", CSBACK_MAGE },
    {   "Familiar", CSBACK_MAGE},
    {   "Wonders", CSBACK_MAGE},
    {   "Ancestor", CSBACK_WEREWOLF},
    {   "Purebreed", CSBACK_WEREWOLF },
    {   "Fetish", CSBACK_WEREWOLF },
    {   "Rites", CSBACK_WEREWOLF }
};

const struct    tradition_type  tradition_table [MAX_TRADITION] =
{
/*  {   name,       prime sphere,   foci name}, */

    {   "reserved",                 0,  ""},
    {   "Akashic Brotherhood",      SPHERE_MIND,    "sword"},
    {   "Dreamspeakers",            SPHERE_SPIRIT,  "feather"},
    {   "Euthanatos",               SPHERE_ENTROPY, "bone"},
    {   "Order of Hermes",          SPHERE_FORCES,  "stone"},
    {   "Verbena",                  SPHERE_LIFE,    "blood"}
};

const struct    rote_type   rote_table  [MAX_ROTE]  =
{
/*  {   name,   function,   corr, entropy, forces, life, matter, mind, prime, spirit, time, target, quint, diff, vulgar, botch/fail passed through},*/

    {   "reserved",             0,                          0, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_NONE, 0, 0, FALSE, FALSE},
    {   "Perceive Entropy",     rote_perceiveentropy,       0, 1, 0, 0, 0, 0, 0, 0, 0, TARGET_CHAR, 1, 1, FALSE, FALSE},
    {   "Control Randomness",   rote_controlrandomness,     0, 2, 0, 0, 0, 0, 0, 0, 0, TARGET_NONE, 2, 2, FALSE, FALSE},
    {   "Destroy Matter",       rote_destroymatter,         0, 3, 0, 0, 0, 0, 0, 0, 0, TARGET_CHAR, 3, 3, TRUE, TRUE},
    {   "Wither Life",          rote_witherlife,            0, 4, 0, 0, 0, 0, 0, 0, 0, TARGET_CHAR, 4, 4, TRUE, FALSE},
    {   "Entropic Mind",        rote_entropicmind,          0, 5, 0, 0, 0, 0, 0, 0, 0, TARGET_CHAR, 5, 7, FALSE, FALSE},
    {   "Perceive Forces",      rote_perceiveforces,        0, 0, 1, 0, 0, 0, 0, 0, 0, TARGET_NONE, 1, 1, FALSE, FALSE},
    {   "Discharge Static",     rote_dischargestatic,       0, 0, 2, 0, 0, 0, 0, 0, 0, TARGET_CHAR, 2, 2, TRUE, FALSE},
    {   "Firestorm",            rote_firestorm,             0, 0, 3, 0, 0, 0, 2, 0, 0, TARGET_CHAR, 3, 3, TRUE, FALSE},
    {   "Kinetic Shield",       rote_kineticshield,         0, 0, 4, 0, 0, 0, 0, 0, 0, TARGET_NONE, 4, 4, TRUE, FALSE},
    {   "Crush of Mountains",   rote_crushofmountains,      0, 0, 5, 0, 0, 0, 2, 0, 0, TARGET_CHAR, 5, 5, TRUE, FALSE},
    {   "Touch of Helios",      rote_touchofhelios,         2, 0, 4, 0, 0, 0, 4, 0, 0, TARGET_CHAR, 15, 5, TRUE, FALSE},
    {   "Spatial Perceptions",  rote_spatialperceptions,    1, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_NONE, 1, 1, FALSE, FALSE},
    {   "Correspondence Perceptions",rote_correspondenceperceptions,2, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_ROOM, 2, 2, FALSE, FALSE},
    {   "Seven League Stride",rote_sevenleaguestride,3, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_ROOM, 3, 3, TRUE, FALSE},
    {   "Conjoin Localities",   rote_conjoinlocalities,     4, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_ROOM, 4, 4, TRUE, FALSE},
    {   "Merge Localities",     rote_mergelocalities,       5, 0, 0, 0, 0, 0, 0, 0, 0, TARGET_ROOM, 50,7, TRUE, FALSE},
    {   "Sense Life",           rote_senselife,             0, 0, 0, 1, 0, 0, 0, 0, 0, TARGET_CHAR, 1, 1, FALSE, FALSE},
    {   "Little Good Death",    rote_littlegooddeath,       0, 0, 0, 2, 0, 0, 0, 0, 0, TARGET_CHAR, 2, 2, TRUE, FALSE},
    {   "Heal Self",            rote_healself,              0, 0, 0, 2, 0, 0, 0, 0, 0, TARGET_NONE, 3, 3, FALSE, FALSE},
    {   "Heal Other",           rote_healother,             0, 0, 0, 3, 0, 0, 0, 0, 0, TARGET_CHAR, 4, 4, TRUE, FALSE},
    {   "Cellular Mitosis",     rote_cellularmitosis,       0, 0, 0, 5, 0, 0, 2, 0, 0, TARGET_CHAR, 25, 7, TRUE, FALSE},
    {   "Matter Perception",    rote_matterperception,      0, 0, 0, 0, 1, 0, 0, 0, 0, TARGET_NONE, 1, 1, FALSE, FALSE},
    {   "Golden Wish",          rote_goldenwish,            0, 0, 0, 0, 2, 0, 0, 0, 0, TARGET_NONE, 2, 2, TRUE, FALSE},
    {   "Open Sesame",          rote_opensesame,            0, 0, 0, 0, 3, 0, 0, 0, 0, TARGET_NONE, 5, 7, TRUE, FALSE},
    {   "Quilted Form",         rote_quiltedform,           0, 0, 0, 0, 4, 0, 0, 0, 0, TARGET_OBJ,  4, 4, TRUE, FALSE},
    {   "Refine Matter",        rote_refinematter,          0, 0, 0, 0, 5, 0, 2, 0, 0, TARGET_OBJ,  25, 7, TRUE, FALSE},
    {   "Empower Self",         rote_empowerself,           0, 0, 0, 0, 0, 1, 0, 0, 0, TARGET_NONE, 1, 1, FALSE, FALSE},
    {   "Subconscious Turmoil", rote_subconsciousturmoil,   0, 0, 0, 0, 0, 2, 0, 0, 0, TARGET_CHAR, 2, 2, FALSE, FALSE},
    {   "Mental Link",          rote_mentallink,            0, 0, 0, 0, 0, 3, 0, 0, 0, TARGET_NONE, 3, 3, FALSE, FALSE},
    {   "Control Minds",        rote_controlminds,          0, 0, 0, 0, 0, 4, 0, 0, 0, TARGET_CHAR, 25, 4, TRUE, FALSE},
    {   "Spirit Sight",         rote_spiritsight,           0, 0, 0, 0, 0, 0, 0, 1, 0, TARGET_NONE, 1, 1, FALSE, FALSE},
    {   "Call Spirit",          rote_callspirit,            0, 0, 0, 0, 0, 0, 0, 2, 0, TARGET_NONE, 25, 2, TRUE, FALSE},
    {   "Step Sideways",        rote_stepsideways,          0, 0, 0, 0, 0, 0, 0, 3, 0, TARGET_NONE, 3, 3, TRUE, TRUE},
    {   "Control Gauntlet",     rote_controlgauntlet,       0, 0, 0, 0, 0, 0, 0, 4, 0, TARGET_NONE, 25, 4, TRUE, FALSE},
    {   "Umbral Rescue",        rote_umbralrescue,          0, 0, 0, 2, 0, 0, 0, 4, 0, TARGET_ROOM, 25, 4, TRUE, FALSE},
    {   "Precognition",         rote_precognition,          0, 0, 0, 0, 0, 0, 0, 0, 2, TARGET_NONE, 2, 2, FALSE, FALSE},
    {   "Time Alteration",      rote_timealteration,        0, 0, 0, 0, 0, 0, 0, 0, 3, TARGET_NONE, 3, 3, TRUE, FALSE},
    {   "Stop the Clock",       rote_stoptheclock,          0, 0, 0, 0, 0, 0, 0, 0, 4, TARGET_CHAR, 25, 4, TRUE, FALSE},
    {   "Sidestep Time",        rote_sidesteptime,          0, 0, 0, 0, 0, 0, 0, 0, 5, TARGET_NONE, 25, 5, TRUE, FALSE},
    {   "Negation Field",       rote_negationfield,         0, 0, 3, 0, 2, 0, 5, 0, 0, TARGET_NONE, 25, 5, TRUE, FALSE}
};

/* Garou forms */
const struct garou_form_type garou_form_table[6] =
{
/*      name        long name   str     dex     sta     app     man     per     diff */
    {   "blank",    "blank",        0,      0,      0,      0,      0,      0,      0},
    {   "Homid",    "Man",      0,      0,      0,      0,      0,      0,      6},
    {   "Glabro",   "Near-Man", 2,      0,      2,      -1,     -1,     0,      7},
    {   "Crinos",   "Wolf-Man", 4,      1,      3,      -10,    -3,     0,      6},
    {   "Hispo",    "Near-Wolf",3,      2,      3,      0,      -3,     0,      7},
    {   "Lupus",    "Wolf",     1,      2,      2,      0,      -3,     2,      6}
};

const struct    tribe_type  tribe_table []  =
{
/*  {   name,       who name,   totem, */

    {   "reserved",         "",             0},
    {   "Black Furies",     "Furies",       SPIRIT_PEGASUS},
    {   "Children of Gaia", "Children",     SPIRIT_UNICORN},
    {   "Fianna",           "Fianna",       SPIRIT_STAG},
    {   "Fenrir",    "Fenrir",          SPIRIT_FENRIS_WOLF},
    {   "Shadow Lords",     "Shadowlord",   SPIRIT_GRANDFATHER_THUNDER},
    {   "Bone Gnawer",  "Gnawer",      SPIRIT_RAT},
    {   "Silver Fangs",     "Fang",         SPIRIT_FALCON},
    {   "Warders Of Men",  "Warder",    SPIRIT_COCKROACH},
};


const struct crafted_item_type crafted_item_table[] =
{
    /*  name            type        vnum    skill        skilllvl resource type     component   socket types    best resource*/
/*  {   "null",         "blank",    0,      0,              0,      0,              {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}}, */
    {   "bowl",         "none",     28870,  CSABIL_WOOD,    1,      OBJ_VNUM_BLOCK, {0,0,0,0,0},{0,0,0,0,0},{99,99,99,99,99,99,99,99,99,99}},
    {   "walkingstick", "equipment",28871,  CSABIL_WOOD,    2,      OBJ_VNUM_PLANK, {0,0,0,0,0},{1,2,3,4,5},{OAK,WALNUT,ASH,MAPLE,YEW,99,99,99,99,99}},
    {   "cross",        "relic",    28872,  CSABIL_WOOD,    3,      OBJ_VNUM_BLOCK, {0,0,0,0,0},{1,2,3,4,5},{YEW,OAK,WALNUT,MAPLE,ASH,99,99,99,99,99}},
    {   NULL,           NULL,       0,      0,              0,      0,              {0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}}
};

const struct flag_type clan_flags[] = {
   {    "Independent",          CLAN_INDEPENDENT,       TRUE    },
   {    "Delete",               CLAN_DELETED,           TRUE    },
   {    "Immortal",             CLAN_IMMORTAL,          TRUE    },
   {    "Modified",             CLAN_CHANGED,           FALSE   },
   {    NULL,                   0,                      FALSE   }
};
/*#define DIR_NORTH             0
#define DIR_EAST              1
#define DIR_SOUTH             2
#define DIR_WEST              3
#define DIR_UP                4
#define DIR_DOWN              5*/
const struct dir_type dir_table[] = {
	{"north"},
	{"east"},
	{"south"},
	{"west"},
	{"up"},
	{"down"}
};
