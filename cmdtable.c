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
    { "freebie",    do_freebie},
    { "chat",           do_chat },
    { "checkhelps", do_checkhelps },
    { "checktypos", do_checktypos },
    { "checkbugs", do_checkbugs },
    { "checklinks", do_checklinks },
    { "checkresets", do_checkresets },
    { "claim", do_claim },
    { "sysnote", do_sysnote},
    { "systemlog", do_systemlog },
    { "at",             do_at },
    { "rat",            do_rat },
    { "pray",       do_cast }, // DOES NOT MATCH. Move to top or bottom of list for review.
    { "banker",     do_banker},
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
    { "talk",       do_talk },
    { "unlock",         do_unlock },
    { "wield",      do_wear }, // DOES NOT MATCH.
    { "emote",      do_emote },
    /*    { "soulsight",    do_soulsight },
    { "mindread",       do_mindread },
    { "shapeshift",   do_shapeshift },
    { "discipline",   do_discipline },
    { "track",        do_track},*/
    { "lore",       do_lore},
    { "classify",   do_classify},
    { "soothe",     do_soothe},
    { "cue",        do_cue},
    { "rhythm",     do_rhythm},
    { "notoriety",  do_notoriety},
    { "dip",        do_dip },
    { "invoke",     do_invoke},
/*    { "activate",     do_activate}, */
    { "drain",      do_drain},
    { "chant",      do_chant},
    { "vigor",      do_vigor},
    { "glower",     do_glower},
    { "waylay",     do_waylay},

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

/* Vampire/Ghoul commands */
    { "vampire",    do_vampire  },
    { "ghoul",     do_ghoul    },
    { "fangs",      do_fangs },
    { "feed",       do_feed  },
    { "regenerate", do_regenerate },
    { "bloodbuff",      do_bloodbuff},
    { "smell",      do_smell  },
    { "nourish", do_nourish },
    { "embrace",        do_embrace },
    { "disclist",        do_disclist },
    { "feralspeech",       do_feralspeech },
    { "beckoning",      do_beckoning },
    { "songofserenity",     do_songofserenity },
    { "drawingouthebeast",      do_drawingoutthebeast },
    { "quelltheherd",           do_quelltheherd },
    { "sense",      do_sense },
    { "reveal",     do_reveal },
    { "auraperception", do_auraperception  },
    { "touch",      do_touch},
    { "telepathy",      do_telepathy},
    { "project",        do_project},
    { "farsight",        do_farsight},
    { "celerity",       do_celerity},
    { "dash",       do_dash },
    { "ignisfatuus", do_ignis_fatuus},
    { "mortalterrors", do_mortalterrors},
    { "chimaera", do_chimaera},
    { "phantasm", do_phantasm},
    { "horridreality", do_horridreality},
    { "command",        do_command },
    { "mesmerize",      do_mesmerize},
    { "forgetful",      do_forgetful},
    { "conditioning",      do_conditioning},
    { "possession",       do_possession},
    { "incubuspassion", do_incubuspassion},
    { "hauntthesoul",   do_hauntthesoul},
    { "eyesofchaos",    do_eyesofchaos},
    { "silencethesanemind",     do_silencethesanemind},
    { "howlinglunacy",  do_howlinglunacy},
    { "blight",     do_blight},
    { "homunculusservant", do_homunculusservant },
    { "resurrection",       do_resurrection},
    { "animatedead",        do_animatedead},
    { "callathanatos",      do_callathanatos},
    { "musterthecorpsearmy", do_musterthecorpsearmy},
    { "blackdeath",     do_blackdeath},
    { "veil",       do_veil},
    { "unseenpresence",     do_unseenpresence},
    { "mask",       do_mask },
    { "vanish",     do_vanish },
    { "cloakthegathering",     do_cloakthegathering },
    { "shadowplay",     do_shadowplay},
    { "shroudofnight",      do_shroudofnight},
    { "armsoftheabyss",     do_armsoftheabyss},
    { "nightshades",        do_nightshades},
    { "shadowform",     do_shadowform},
    { "shadowwalk",     do_shadowwalk},
    { "awe",        do_awe },
    { "dreadgaze",      do_dreadgaze},
    { "entrancement",       do_entrancement },
    { "summon",     do_summon},
    { "majesty",        do_majesty},
    { "sparkofrage", do_sparkofrage },
    { "claws",      do_claws },
    { "gleam",      do_gleam },
    { "earthmeld",      do_earthmeld},
    { "shift",      do_shift},
    { "silenceofdeath",     do_silenceofdeath},
    { "weakness",       do_weakness},
    { "coat",       do_bloodagony},
    { "bloodcurse",     do_bloodcurse },
    { "spitblood",      do_acidblood },
    { "eyesoftheserpent",   do_eyesoftheserpent},
    { "tongueoftheasp",    do_tongueoftheasp},
    { "tonguelash",     do_tonguelash},
    { "skinoftheadder", do_skinoftheadder},
    { "formofthecobra", do_formofthecobra},
    { "darkheart",      do_darkheart},
    { "taste",  do_taste },
    { "bloodrage",  do_bloodrage },
    { "potency",    do_bloodofpotency },
    { "theft",  do_theft },
    { "cauldron",   do_cauldron },
    { "incorporealpassage", do_incorporealpassage},
    { "changeling",     do_changeling},
    { "fleshcraft",     do_fleshcraft },
    { "bonecraft",      do_bonecraft},
    { "horridform",     do_horrid },
    { "essence",        do_essence},
    { "chiropteranmarauder",    do_chiropteranmarauder },
    { "wingclaws", do_wingclaws },

/* Garou commands */
    { "werewolf",   do_werewolf},
    { "induct",     do_induct},
    { "renown",     do_renown},
    { "garou",      do_garou},
    { "gifts",      do_gifts},
    { "beseech",        do_beseech },
    { "totem",  do_totem},
    { "gogarou",    do_gogarou },
    { "shapeshift", do_shapeshift},
    { "rage",       do_rage},
    { "stepsideways",   do_stepsideways},
    { "meditate",       do_meditate},
    { "packtactics",    do_packtactics},

/* Mage commands */
    { "mage",       do_mage },
    { "magecast",       do_magecast },
    { "rote",       do_rote },
    { "rotelist",       do_rotelist },
    { "sphere", do_sphere},
    { "breathweapon",   do_breathweapon },

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
    { "addict",     do_addict},
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
    { "help",       do_help },
    { "helplist",   do_helplist },
    { "idea",       do_idea },
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
    {"skillmap",    do_skillmap},
    { "socials",    do_socials },
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
    { "brief",      do_brief},
    { "combat",     do_combat}, //DNM
    { "colour",     do_colour },
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
    { "rpavailable",  do_rpavailable},
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
    { "auction",    do_auction},
    { "deaf",       do_deaf},
    /* emote moved above embrace */
    { "pmote",      do_pmote},
    { "ictalk",     do_ictalk},
    { "gtell",      do_gtell},
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
    { "telnetgoahead", do_telnetgoahead},

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
    { "bite",       do_bite},
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
    { "gouge",      do_gouge},
    { "headbutt",   do_headbutt},
    { "warcry",     do_warcry},
    { "blast",      do_blast},
    { "slip",       do_slip},
    { "shred",      do_shred},
    { "study",        do_study},
    { "skin",       do_skin},
    { "fortress",       do_fortress},
    { "bind",       do_bind},
    { "behead", do_behead},
    { "arise",  do_arise},
    { "allies", do_allies},

    { "trophy", do_trophy},

    /* Remort placed here so remove parses first */
    { "remort",     do_remort},
    { "register",     do_register},
    { "email",        do_email},
    { "contact",    do_contact },


 //   { "flaw",     do_flaw},
  //   { "convert",        do_convert},
    /* changeling here so change parses first */

    /*
     * Miscellaneous commands.
     */
    { "enter",      do_enter},
    { "follow",     do_follow},
    { "dismiss",    do_dismiss},
    { "go",     do_enter},
    { "group",      do_group},
    { "hide",       do_hide},
    { "play",       do_play},
    { "games",      do_game },
    { "practice",   do_practice},
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
    { "lagem",    do_lagem},
    { "advance",    do_advance},
    { "dump",       do_dump},
    { "trust",      do_trust},
    { "violate",    do_violate},
    { "teach",  do_teach},
    { "brew",   do_brew},
    { "settime",      do_settime},
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
    { "forcetick",  do_forcetick},
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
    { "teleport",   do_transfer},
    { "transfer",   do_transfer},
    { "crossgauntlet",  do_crossgauntlet},
    { "write",      do_write},
    { "seal",       do_seal},
    { "unseal",     do_unseal},
    { "playtest",      do_playtest},
    { "announce",       do_announce},
    { "qitemlist",      do_qitemlist},
    { "placeqitems",    do_placeqitems},
    { "bslap",          do_bslap},
    { "tlookup",    do_tlookup},
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
    { "poofin",     do_bamfin}, //DNM
    { "poofout",    do_bamfout}, //DNM
    { "gecho",      do_echo}, //DNM
    { "holylight",  do_holylight},
    { "incognito",  do_incognito},
    { "invis",      do_invis},
    { "log",        do_log},
    { "debug",      do_debug},
    { "debugger",   do_debugger},
    { "debugmsg",   do_debugmsg},
    { "showdump",   do_showdump},
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
    { "areaflagfind", do_areaflagfind },
    { "mpfind",     do_mpfind },
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
    { "qiedit",     do_qiedit       },
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
    { "opedit",     do_opedit},
    { "rpedit",     do_rpedit},
    { "opdump",     do_opdump},
    { "opstat",     do_opstat},
    { "rpdump",     do_rpdump},
    { "rpstat",     do_rpstat},
    { "hedit",      do_hedit},
    { "cedit",      do_cedit},
    { "csave",      do_save_clans},

/* to parse first insted of resets */
    { "resettime",      do_resettime},
    { "fixme",        do_fixme},
    { "create",     do_create},
    { "becomeactive", do_becomeactive},

    { NULL,         NULL            }
};
