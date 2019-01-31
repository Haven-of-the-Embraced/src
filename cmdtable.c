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
*       ROM 2.4 is copyright 1993-1998 Russ Taylor                         *
*       ROM has been brought to you by the ROM consortium                  *
*           Russ Taylor (rtaylor@hypercube.org)                            *
*           Gabrielle Taylor (gtaylor@hypercube.org)                       *
*           Brian Moore (zump@rom.org)                                     *
*       By using this code, you have agreed to follow the terms of the     *
*       ROM license, in the file Rom24/doc/rom.license                     *
***************************************************************************/
#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

const   struct  cmdfun_type cmdfun_table    [] =
{
    { "huh",        do_huh   },
    { "north",      do_north },
    { "east",       do_east },
    { "south",      do_south },
    { "west",       do_west },
    { "up",     do_up },
    { "down",       do_down },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "chat",           do_chat },
    { "checkhelps", do_checkhelps },
    { "checktypos", do_checktypos },
    { "checkbugs", do_checkbugs },
    { "claim", do_claim },
    { "systemlog", do_systemlog },
    { "at",             do_at },
    { "rat",            do_rat },
    { "pray",       do_cast }, // DOES NOT MATCH. Move to top or bottom of list for review.
    { "rote",       do_rote },
    { "banker",     do_banker},
    { "beseech",        do_beseech },
    { "buy",        do_buy },
    { "channels",       do_channels  },
    { "tax",            do_tax },
    { "dinfo",          do_dinfo },
    { "exits",      do_exits  },
    { "declare",    do_declare },
    { "get",        do_get },
    { "goto",           do_goto },
    { "group",          do_group },
    { "guild",      do_guild },
    { "inventory",  do_inventory },
    { "kill",       do_kill },
    { "look",       do_look },
    { "knock",      do_knock },
    { "glance",     do_glance },
    { "clantalk",       do_clantalk },
    { "clans",          do_clans },
    { "cleader",       do_cleader},
    { "music",          do_music  },
    { "order",      do_order  },
    { "practice",       do_practice },
    { "portal",     do_portal },
    { "rest",       do_rest },
    { "sit",        do_sit },
    { "sockets",        do_sockets },
    { "bestowgift",     do_bestowgift },
    { "stand",      do_stand },
    { "tell",       do_tell },
    { "whisper",    do_whisper},
    { "unlock",         do_unlock },
    { "wield",      do_wear }, // DOES NOT MATCH.
    { "emote",      do_emote },
    { "vampire",        do_vampire  },
    { "magecast",       do_magecast },
    { "gogarou",        do_garouconvert }, // DOES NOT MATCH.
    { "mage",       do_mage },
    { "embrace",        do_embrace },
    { "claws",      do_claws },
    { "gleam",      do_gleam },
    { "sense",      do_sense },
    { "fangs",      do_fangs },
    { "feed",       do_feed  },
    { "smell",      do_smell  },
    { "nourish", do_nourish },
    { "reveal",     do_reveal },
    { "auraperception", do_auraperception  },
    { "taste",  do_taste },
    { "potency",    do_bloodofpotency },// DNM
    { "theft",  do_theft },
    { "cauldron",   do_cauldron },
    { "bloodrage",  do_bloodrage }, // DOES NOT MATCH
    /*    { "soulsight",    do_soulsight }, */
  /*{ "mindread",       do_mindread }, */
/*    { "shapeshift",   do_shapeshift }, */
//    { "discipline",   do_discipline },
    { "sphere", do_sphere},
    { "shift",      do_shift},
    { "unseenpresence",     do_unseenpresence},
    { "silenceofdeath",     do_silenceofdeath},
    { "coat",       do_bloodagony},//DNM
    { "veil",       do_veil},
    { "vanish",     do_vanish },
    { "mask",       do_mask },
    { "feralspeech",       do_feralspeech },
    { "beckoning",      do_beckoning },
    { "songofserenity",     do_songofserenity },
    { "drawingouthebeast",      do_drawingoutthebeast },
    { "quelltheherd",           do_quelltheherd },
    { "forgetful",      do_forgetful},
    { "cloakthegathering",     do_cloakthegathering },//DNM
    { "dreadgaze",      do_dreadgaze},
    { "vampire regen", do_vampire_regen},
    { "bloodbuff",      do_bloodbuff},//dnm
    { "command",        do_command },
    { "mesmerize",      do_mesmerize},
    { "awe",        do_awe },
    { "possession",       do_possession},
    /*{ "track",        do_track},*/
    { "incorporealpassage", do_incorporealpassage},//dnm
    { "celerity",       do_celerity},//dnm
    { "shadowplay",     do_shadowplay},
    { "shadowform",     do_shadowform},
    { "nightshades",        do_nightshades},
    { "shadowwalk",     do_shadowwalk},
    { "shroudofnight",      do_shroudofnight},
    { "armsoftheabyss",     do_armsoftheabyss},
    { "entrancement",       do_entrancement },
    { "earthmeld",      do_earthmeld},
    { "majesty",        do_majesty},
    { "homunculusservant", do_homunculusservant },
    { "summon",     do_summon},
    { "resurrection",       do_resurrection},
    { "animatedead",        do_animatedead},
    { "callathanatos",      do_callathanatos},
    { "weakness",       do_weakness},
    { "blight",     do_blight},
    { "blackdeath",     do_blackdeath},
    { "spitblood",      do_acidblood },//dnm
/*    { "clotblood",        do_clotblood }, */
    { "bloodcurse",     do_bloodcurse },
    { "lore",       do_lore},
    { "dip",        do_dip },
    { "invoke",     do_invoke},
/*    { "activate",     do_activate}, */
    { "drain",      do_drain},
    { "chant",      do_chant},
    { "vigor",      do_vigor},
    { "waylay",     do_waylay},
    { "fleshcraft",     do_fleshcraft },
    { "bonecraft",      do_bonecraft},
    { "horridform",     do_horrid },
    { "essence",        do_essence},
    { "dash",       do_dash },
    { "ignisfatuus", do_ignis_fatuus},
    { "mortalterrors", do_mortalterrors},

/* for human guilds */
    { "ictalk", do_ictalk},
    { "traditiontalk",      do_traditiontalk},
    { "tdtalk",     do_traditiontalk }, // DOES NOT MATCH. create do_tdtalk(ch,argument); call do_function(ch, &do_traditiontalk, argument);
    { "tribetalk",      do_tribetalk },
    { "tbtalk",     do_tribetalk}, // DOES NOT MATCH. create do_tbtalk(ch,argument); call do_function(ch, &do_tribetalk, argument);
    { "invite",     do_invite },
    { "rank",       do_rank},
    { "promote",    do_promote},
    { "lpromote",   do_promote_leader},
    { "expel",      do_expel},
/* Garou commands */
    { "induct",     do_induct},
    { "renown",     do_renown},
    { "garou",      do_garou},
    { "gifts",      do_gifts},
    { "totem",  do_totem},

/* new specials for professions */
    { "assassinate",        do_assassinate },
    { "divine",     do_divine_strength },

/* evil evil evil */
    { "stake",      do_stake},
/*    { "diablerize",       do_diablerize}, */

    { "wizhelp",    do_wizhelp},

    /*
     * Informational commands.
     */
    { "affects",    do_affects},
    { "areas",      do_areas },
    { "dictionary",     do_dictionary },
    { "bug",        do_bug},
    //Matthew's Banking System
    { "bank",       do_bank},
    { "clanbank",   do_clanbank},
    { "changes",    do_changes },
    { "commands",   do_commands},
    { "compare",    do_compare},
    { "consider",   do_consider },
    { "count",      do_count, },
    { "credits",    do_credits},
    { "equipment",  do_equipment},
    { "examine",    do_examine},
/*  { "groups",     do_groups}, */
    { "help",       do_help },
    { "idea",       do_idea },
    { "info",           do_groups},
    { "motd",       do_motd },
    { "news",       do_news},
    { "read",       do_read},
    { "report",     do_report },
    { "rules",      do_rules},
    { "score",      do_score},
    { "charsheet",  do_charsheet },
    { "cs",         do_charsheet }, // NOT MATCH. whole block.
    { "sheet",      do_charsheet },
    { "cs2",        do_charsheet2 },
    { "cs3",        do_charsheet3 },
    { "scan",       do_scan},
    { "skills",     do_skills },
    { "socials",    do_socials },
    { "show",       do_show},
    { "prayers",        do_spells}, //DNM
    { "playernotes",    do_playernotes},
    { "story",      do_story},
    { "time",       do_time}, //WM STARTING
    { "typo",       do_typo},
    { "weather",    do_weather,},
    { "who",        do_who},
    { "whois",      do_whois},
    { "wizlist",    do_wizlist},
    { "worth",      do_worth},

    /*
     * Configuration commands.
     */
    { "alia",       do_alia},
    { "alias",      do_alias},
//    { "addwizlist", do_addwizlist},
    { "arename",        do_arename},
    { "spectate",       do_spectate},
    { "autolist",   do_autolist},
    { "autoassist", do_autoassist},
    { "autoooc",    do_autoooc},
    { "autoexit",   do_autoexit},
    { "autogold",   do_autogold},
    { "autoloot",   do_autoloot},
    { "autosac",    do_autosac},
    { "autosplit",  do_autosplit},
    { "autovamp",       do_autovamp},
    { "brief",      do_brief},
    { "combat",     do_combat}, //DNM
/*  { "channels",   do_channels}, */
    { "colour",     do_colour },
    { "pueblo",     do_pueblo},
    { "color",      do_colour}, //DNM
    { "ansi",       do_colour}, //DNM
    { "combine",    do_combine},
    { "compact",    do_compact},
    { "description",    do_description },
    { "delet",      do_delet },
    { "delete",     do_delete },
    { "nofollow",   do_nofollow },
    { "icmode",     do_ic},
    { "goic",       do_ic},
    { "noloot",     do_noloot},
    { "nosummon",   do_nosummon },
	{ "nocondition", do_nocondition },
    { "outfit",     do_outfit},
    { "password",   do_password},
    { "prompt",     do_prompt},
    { "scroll",     do_scroll},
    { "sect",       do_sect},
    { "title",      do_title},
    { "unalias",    do_unalias },
    { "wimpy",      do_wimpy},
    { "wiziname",   do_wiziname},
    { "immtitle",   do_immtitle},
    { "pretitle",   do_pretitle},

    /*
     * Communication commands.
     */
    { "afk",        do_afk},
    { "answer",     do_answer},
/*  { "auction",    do_auction}, */
    { "deaf",       do_deaf},
    /* emote moved above embrace */
    { "pmote",      do_pmote},
    { "ictalk",     do_ictalk},
/*    { "grats",        do_grats}, */
    { "gtell",      do_gtell},
/*  { "music",      do_music}, */
    { "note",       do_note},
    { "rpnote",    do_rpnote},
    { "pose",       do_pose},
    { "quest",      do_quest},
    { "question",   do_question},
    { "quest",      do_quest},
    { "ooc",        do_ooc},
    { "plead",      do_plead},
    { "gwho",       do_gwho},
    { "quiet",      do_quiet},
    { "reply",      do_reply},
    { "replay",     do_replay},
    { "replayroom", do_replayroom},
    { "replaychannels", do_replaychannels},
    { "say",        do_say},
    { "osay",       do_osay},
    { "shout",      do_shout },
    { "unread",     do_unread},
    { "yell",       do_yell},
    { "tsect",      do_tsect},

    /*
     * Object manipulation commands.
     */
    { "brandish",   do_brandish},
    { "close",      do_close},
    { "drink",      do_drink},
    { "drop",       do_drop},
    { "eat",        do_eat},
    { "envenom",    do_envenom},
    { "fill",       do_fill},
    { "give",       do_give},
    { "heal",       do_heal},
    { "hold",       do_wear}, //DNM
    { "list",       do_list},
    { "lock",       do_lock},
    { "open",       do_open},
    { "lift",       do_lift},
    { "pick",       do_pick},
    { "pour",       do_pour},
    { "put",        do_put},
    { "quaff",      do_quaff},
    { "recite",     do_recite},
    { "remove",     do_remove},
    { "sell",       do_sell},
    { "take",       do_get},
    { "sacrifice",  do_sacrifice},
    { "junk",           do_sacrifice},
/*  { "unlock",     do_unlock}, */
    { "value",      do_value},
    { "battleprompt", do_battleprompt},
    { "wear",       do_wear},
    { "zap",        do_zap},
    { "donate",     do_donate},

    /*
     * Combat commands.
     */
    { "backstab",   do_backstab},
    { "bash",       do_bash},
    { "bs",     do_backstab}, //DNM
    { "berserk",    do_berserk},
    { "dirt",       do_dirt},
    { "disarm",     do_disarm},
    { "flee",       do_flee},
    { "kick",       do_kick},
    { "murde",      do_murde},
   { "murder",      do_murder},
    { "rescue",     do_rescue},
    { "surrender",  do_surrender},
    { "trip",       do_trip},
    { "ground",     do_ground},
    { "headbutt",   do_headbutt},
    { "warcry",     do_warcry},
    { "blast",      do_blast},
    { "slip",       do_slip},
    { "project",        do_project},
    { "telepathy",      do_telepathy},
    { "touch",      do_touch},
    { "study",        do_study},
    { "skin",       do_skin},
    { "fortress",       do_fortress},
    { "bind",       do_bind},
    { "behead", do_behead},
    { "arise",  do_arise},
    { "allies", do_allies},



    /* Remort placed here so remove parses first */
    { "remort",     do_remort},
    { "register",     do_register},
    { "email",        do_email},
    { "contact",    do_contact },
    /* Condition placed here so consider parses first */

/*    { "flaw",     do_flaw}, */
    { "conditioning",      do_conditioning},
    { "convert",        do_convert},
    /* changeling here so change parses first */
    { "changeling",     do_changeling},



     /*
     * Mob command interpreter (placed here for faster scan...)
     */
    { "mob",        do_mob},

    /*
     * Miscellaneous commands.
     */
    { "enter",      do_enter},
    { "follow",     do_follow},
    { "dismiss",    do_dismiss},
    { "gain",       do_gain},
    { "go",     do_enter},
/*  { "group",      do_group}, */
    { "groups",     do_groups},
    { "hide",       do_hide},
    { "play",       do_play},
    { "games",      do_game },
/*  { "practice",   do_practice}, */
    { "qui",        do_qui},
    { "quit",       do_quit},
    { "recall",     do_recall},
    { "rent",       do_rent},
    { "save",       do_save},
    { "sleep",      do_sleep},
    { "sneak",      do_sneak},
    { "split",      do_split},
    { "steal",      do_steal},
    { "train",      do_train},
    { "visible",    do_visible},
    { "wake",       do_wake},
    { "where",      do_where},
    { "mount",      do_mount},
    { "dismount",   do_dismount},
    { "hometown",   do_hometown},
    { "rite",   do_rite},
    { "learn",  do_learn},
    { "qlog",       do_qlog},
    { "journal",        do_qlog}, //DNM

/* crafting */
    { "chop",       do_chop},
    { "prospect",   do_prospect},
    { "mine",       do_mine},
    { "plane",      do_plane},
    { "carve",      do_carve},
    { "noshortdesc", do_noshortdesc},

    /*
     * Immortal commands.
     */
//  { "wikihelp",   do_showhelpindex}, //DNM
    { "lagem",    do_lagem},
    { "advance",    do_advance},
    { "dump",       do_dump},
    { "trust",      do_trust},
    { "violate",    do_violate},
    { "teach",  do_teach},
    { "brew",   do_brew},
/*    { "settime",      do_settime}, */
    { "allow",      do_allow},
    { "allowadmintalk", do_allowadmintalk},
    { "ban",        do_ban},
    { "beep",       do_beep},
    { "roll",       do_roll},
    { "dice",       do_dice},
    { "deny",       do_deny},
    { "disconnect", do_disconnect},
    { "flag",       do_flag},
    { "freeze",     do_freeze},
    { "permban",    do_permban},
    { "protect",    do_protect},
    { "reboo",      do_reboo},
    { "reboot",     do_reboot },
    { "set",        do_set },
    { "smite",  do_smite},
    { "shutdow",    do_shutdow},
    { "shutdown",   do_shutdown},
    { "nuk",    do_nuk},
    { "nuke",   do_nuke},
    { "clearaff",   do_clearaff},
/*  { "sockets",    do_sockets}, */
    { "wizlock",    do_wizlock},
    { "qpoint", do_qpoint},
    { "gxp",       do_gxp},
    { "gqp",       do_gqp},
    { "qpmult",    do_qpmult},
    { "xpmult",    do_xpmult},
    { "cheater",   do_cheater},
    { "ipoint", do_ipoint},
    { "tag", do_tag},
    { "demeanor", do_demeanor},
    { "resetkill", do_resetkill},
    { "frpoint", do_frpoint},
    { "force",      do_force},
    { "load",       do_load},
    { "newlock",    do_newlock},
    { "nature",     do_nature},
    { "nochannels", do_nochannels },
    { "noemote",    do_noemote},
    { "noshout",    do_noshout},
    { "notell",     do_notell },
    { "pecho",      do_pecho},
    { "pardon",     do_pardon },
    { "purge",      do_purge},
    { "restore",    do_restore},
    { "sla",        do_sla},
    { "slay",       do_slay},
    { "obliterate", do_obliterate},
    { "salvation",  do_salvation},
    { "xp boost",   do_xp_boost},
    { "new dice",   do_newdice},
    { "teleport",   do_transfer},
    { "transfer",   do_transfer},
    { "write",      do_write},
    { "seal",       do_seal},
    { "unseal",     do_unseal},
    { "fatamorgana", do_fatamorgana},
    { "sendhtml",       do_sendhtml}, //DNM
    { "d10combat",      do_d10combat},

    /* Language stuff*/
        /*
     * Languages.
     *//* BROKEN!
    { "speak",          do_speak},
    { "common",         do_common},
    { "dwarvish",       do_dwarvish},
    { "elvish",         do_elvish},
    { "human",          do_human},
    { "gnomish",        do_gnomish},
    { "orcish",         do_orcish},
    { "goblin",         do_goblin},
    { "ogre",           do_ogre},
    { "trollish",       do_trollish},
    { "kobold",         do_kobold},
    { "drow",           do_drow},
    { "hobbit",         do_hobbit},
    { "learn",          do_learn},
    { "lset",           do_lset},
    { "lstat",          do_lstat},
*/
/*  { "at",     do_at}, */
    { "poofin",     do_bamfin}, //DNM
    { "poofout",    do_bamfout}, //DNM
    { "gecho",      do_echo}, //DNM
    { "announce",       do_announce},
/*  { "goto",       do_goto}, */
    { "holylight",  do_holylight},
    { "incognito",  do_incognito},
    { "invis",      do_invis},
    { "log",        do_log},
    { "debug",      do_debug},
    { "memory",     do_memory},
    { "mwhere",     do_mwhere},
    { "owhere",     do_owhere},
    { "peace",      do_peace},
    { "penalty",    do_penalty},
    { "recho",       do_recho}, //DNM
    { "return",         do_return},
    { "snoop",      do_snoop},
    { "stat",       do_stat},
    { "string",     do_string},
    { "switch",     do_switch},
    { "wizinvis",   do_invis }, //DNM
    { "vnum",       do_vnum},
    { "vlist",      do_vlist },
    { "flagfind",   do_flagfind},
    { "zecho",      do_zecho},

    { "clone",      do_clone},

    { "wiznet",     do_wiznet},
    { "whoconfig",  do_whoconfig},
    { "pipe",       do_pipe},
    { "immtalk",    do_immtalk},
    { "imptalk",    do_imptalk},
    { "immmask",    do_immmask},
    { "imotd",          do_imotd},
    { "smote",      do_smote },
    { "prefi",      do_prefi},
    { "prefix",     do_prefix},
    { "mpdump",     do_mpdump},
    { "mpstat",     do_mpstat},
    { "setarena",   do_setarena},
    { "globalpower",    do_globalpower},
    { "wipecs",     do_wipecs},
    { "pload",      do_pload},
    { "punload",    do_punload},
    { "pnewpass",   do_pnewpass},
    { "forcebank",  do_forcebank},
    { "forceinfluence", do_forceinfluence},
    { "makehelper", do_makehelper},
    { "funlist", do_funlist},
    { "allowimmtalk", do_allowimmtalk},
    { "cmdedit",    do_cmdedit      },
    { "cmdlist",    do_cmdlist      },
    { "copyover",   do_copyover   },
    { "admintalk",  do_admintalk },
    { "ticks",      do_ticks     },
    { "makeactive", do_makeactive},
    { "backgrounds", do_backgrounds },

/* placed here for parsing */
    { "bandage",        do_bandage},
    { "quest",      do_qlog}, //DNM

    /*
     * OLC
     */
    { "edit",       do_olc}, //DNM
    { "asave",          do_asave},
    { "alist",      do_alist},
    { "resets",     do_resets},
    { "redit",      do_redit},
    { "medit",      do_medit},
    { "aedit",      do_aedit},
    { "oedit",      do_oedit},
    { "mpedit",     do_mpedit},
    { "hedit",      do_hedit},
    { "cedit",      do_cedit},
    { "csave",      do_save_clans},

/* to parse first insted of resets */
    { "resettime",      do_resettime},
/*    { "fixme",        do_fixme}, */
    { "create",     do_create},
    { "freebie",    do_freebie},
    { "becomeactive", do_becomeactive},


/* new garou */
    { "shapeshift", do_shapeshift},
    { "rage",       do_rage},
    { "stepsideways",   do_stepsideways},
    { "meditate",       do_meditate},
    { NULL,         NULL            }
};
