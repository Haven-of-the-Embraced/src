/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,    *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                     *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael      *
 *  Chastain, Michael Quan, and Mitchell Tse.                  *
 *                                     *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                           *
 *                                     *
 *  Much time and thought has gone into this software and you are      *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                          *
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

/* this is a listing of all the commands and command related data */

/* wrapper function for safe command execution */
void do_function args((CHAR_DATA *ch, DO_FUN *do_fun, char *argument));

/* for command types */
#define ML  MAX_LEVEL   /* implementor */
#define L1  MAX_LEVEL - 1   /* creator */
#define L2  MAX_LEVEL - 2   /* supreme being */
#define L3  MAX_LEVEL - 3   /* deity */
#define L4  MAX_LEVEL - 4   /* god */
#define L5  MAX_LEVEL - 5   /* immortal */
#define L6  MAX_LEVEL - 6   /* demigod */
#define L7  MAX_LEVEL - 7   /* angel */
#define L8  MAX_LEVEL - 8   /* avatar */
#define IM  LEVEL_IMMORTAL  /* avatar */
#define HE  LEVEL_HERO  /* hero */

#define COM_INGORE  1


/*
 * Structure for a command in the command lookup table.
 */
struct  cmd_type
{
    char * const    name;
    DO_FUN *        do_fun;
    sh_int      position;
    sh_int      level;
    sh_int      log;
    sh_int              show;
};


/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */
DECLARE_DO_FUN( do_advance  );
DECLARE_DO_FUN( do_rat);
//DECLARE_DO_FUN( do_addwizlist);
DECLARE_DO_FUN( do_cmdlist    );
DECLARE_DO_FUN( do_admintalk );
DECLARE_DO_FUN( do_affects  );
DECLARE_DO_FUN( do_afk      );
DECLARE_DO_FUN( do_alia     );
DECLARE_DO_FUN( do_alias    );
DECLARE_DO_FUN( do_allow    );
DECLARE_DO_FUN( do_answer   );
DECLARE_DO_FUN( do_areas    );
DECLARE_DO_FUN( do_mortalterrors);
DECLARE_DO_FUN( do_at       );
DECLARE_DO_FUN( do_auction  );
DECLARE_DO_FUN( do_autoassist   );
DECLARE_DO_FUN( do_autoexit );
DECLARE_DO_FUN( do_autogold );
DECLARE_DO_FUN( do_huh      );
DECLARE_DO_FUN( do_autolist );
DECLARE_DO_FUN( do_autoloot );
DECLARE_DO_FUN( do_autosac  );
DECLARE_DO_FUN( do_autosplit    );
DECLARE_DO_FUN( do_autovamp );
DECLARE_DO_FUN( do_backstab );
DECLARE_DO_FUN( do_bamfin   );
DECLARE_DO_FUN( do_bamfout  );
DECLARE_DO_FUN( do_ban      );
DECLARE_DO_FUN( do_bash     );
DECLARE_DO_FUN( do_beep     );
DECLARE_DO_FUN( do_roll     );
DECLARE_DO_FUN( do_dice     );
DECLARE_DO_FUN(do_nocondition);
DECLARE_DO_FUN(do_noshortdesc);
DECLARE_DO_FUN( do_copyover );
DECLARE_DO_FUN( do_berserk  );
DECLARE_DO_FUN( do_brandish );
DECLARE_DO_FUN( do_brief    );
DECLARE_DO_FUN( do_combat );
DECLARE_DO_FUN( do_bug      );
DECLARE_DO_FUN( do_buy      );
DECLARE_DO_FUN( do_bestowgift  );
DECLARE_DO_FUN( do_cast     );
DECLARE_DO_FUN( do_beseech  );
DECLARE_DO_FUN( do_changes  );
DECLARE_DO_FUN( do_banker);
DECLARE_DO_FUN( do_declare);
DECLARE_DO_FUN( do_tax);
DECLARE_DO_FUN( do_dinfo);
DECLARE_DO_FUN( do_claim);
DECLARE_DO_FUN( do_channels );
DECLARE_DO_FUN( do_clone    );
DECLARE_DO_FUN( do_close    );
DECLARE_DO_FUN( do_colour       );  /* Colour Command By Lope */
DECLARE_DO_FUN( do_pueblo   );
DECLARE_DO_FUN( do_commands );
DECLARE_DO_FUN( do_combine  );
DECLARE_DO_FUN( do_compact  );
DECLARE_DO_FUN( do_compare  );
DECLARE_DO_FUN( do_consider );
DECLARE_DO_FUN( do_count    );
DECLARE_DO_FUN( do_credits  );
DECLARE_DO_FUN( do_deaf     );
DECLARE_DO_FUN( do_debug    );
DECLARE_DO_FUN( do_delet    );
DECLARE_DO_FUN( do_delete   );
DECLARE_DO_FUN( do_deny     );
DECLARE_DO_FUN( do_description  );
DECLARE_DO_FUN( do_playernotes);
DECLARE_DO_FUN( do_dirt     );
DECLARE_DO_FUN( do_disarm   );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_down     );
DECLARE_DO_FUN( do_drink    );
DECLARE_DO_FUN( do_drop     );
DECLARE_DO_FUN( do_dump     );
DECLARE_DO_FUN( do_east     );
DECLARE_DO_FUN( do_eat      );
DECLARE_DO_FUN( do_echo     );
DECLARE_DO_FUN( do_emote    );
DECLARE_DO_FUN( do_enter    );
DECLARE_DO_FUN( do_envenom  );
DECLARE_DO_FUN( do_equipment    );
DECLARE_DO_FUN( do_examine  );
DECLARE_DO_FUN( do_exits    );
DECLARE_DO_FUN( do_fill     );
DECLARE_DO_FUN( do_flag     );
DECLARE_DO_FUN( do_flee     );
DECLARE_DO_FUN( do_follow   );
DECLARE_DO_FUN( do_frpoint  );
DECLARE_DO_FUN( do_dismiss  );
DECLARE_DO_FUN( do_force    );
DECLARE_DO_FUN( do_freeze   );
DECLARE_DO_FUN( do_gain     );
DECLARE_DO_FUN( do_game     );
DECLARE_DO_FUN( do_get      );
DECLARE_DO_FUN( do_give     );
DECLARE_DO_FUN( do_ictalk   );
DECLARE_DO_FUN( do_goto     );
DECLARE_DO_FUN( do_grats    );
DECLARE_DO_FUN( do_group    );
DECLARE_DO_FUN( do_groups   );
DECLARE_DO_FUN( do_gtell    );
DECLARE_DO_FUN( do_guild        );
DECLARE_DO_FUN( do_heal     );
DECLARE_DO_FUN( do_help     );
DECLARE_DO_FUN( do_hide     );
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_idea     );
DECLARE_DO_FUN( do_immtalk  );
DECLARE_DO_FUN( do_imptalk  );
DECLARE_DO_FUN( do_incognito    );
DECLARE_DO_FUN( do_clantalk );
DECLARE_DO_FUN( do_clans );
DECLARE_DO_FUN (do_cleader);
DECLARE_DO_FUN( do_imotd    );
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis    );
DECLARE_DO_FUN( do_kick     );
DECLARE_DO_FUN( do_kill     );
DECLARE_DO_FUN( do_list     );
DECLARE_DO_FUN( do_load     );
DECLARE_DO_FUN( do_lock     );
DECLARE_DO_FUN( do_log      );
DECLARE_DO_FUN( do_look     );
DECLARE_DO_FUN( do_knock        );
DECLARE_DO_FUN( do_glance       );
DECLARE_DO_FUN( do_memory   );
DECLARE_DO_FUN( do_mfind    );
DECLARE_DO_FUN( do_mload    );
DECLARE_DO_FUN( do_mset     );
DECLARE_DO_FUN( do_mstat    );
DECLARE_DO_FUN( do_mwhere   );
DECLARE_DO_FUN( do_mob      );
DECLARE_DO_FUN( do_motd     );
DECLARE_DO_FUN( do_mpstat   );
DECLARE_DO_FUN( do_mpdump   );
DECLARE_DO_FUN( do_murde    );
DECLARE_DO_FUN( do_murder   );
DECLARE_DO_FUN( do_music    );
DECLARE_DO_FUN( do_newlock  );
DECLARE_DO_FUN( do_news     );
DECLARE_DO_FUN( do_nochannels   );
DECLARE_DO_FUN( do_noemote  );
DECLARE_DO_FUN( do_nofollow );
DECLARE_DO_FUN( do_ic       );
DECLARE_DO_FUN( do_noloot   );
DECLARE_DO_FUN( do_north    );
DECLARE_DO_FUN( do_noshout  );
DECLARE_DO_FUN( do_nosummon );
DECLARE_DO_FUN( do_note     );
DECLARE_DO_FUN( do_notell   );
DECLARE_DO_FUN( do_ofind    );
DECLARE_DO_FUN( do_oload    );
DECLARE_DO_FUN( do_open     );
DECLARE_DO_FUN( do_lift );
DECLARE_DO_FUN( do_order    );
DECLARE_DO_FUN( do_oset     );
DECLARE_DO_FUN( do_ostat    );
DECLARE_DO_FUN( do_outfit   );
DECLARE_DO_FUN( do_owhere   );
DECLARE_DO_FUN( do_pardon   );
DECLARE_DO_FUN( do_password );
DECLARE_DO_FUN( do_peace    );
DECLARE_DO_FUN( do_pecho    );
DECLARE_DO_FUN( do_sendhtml    );
DECLARE_DO_FUN( do_penalty  );
DECLARE_DO_FUN( do_rpnote  );
DECLARE_DO_FUN( do_permban  );
DECLARE_DO_FUN( do_pick );
DECLARE_DO_FUN( do_play     );
DECLARE_DO_FUN( do_pmote    );
DECLARE_DO_FUN( do_pose );
DECLARE_DO_FUN( do_pour     );
DECLARE_DO_FUN( do_practice );
DECLARE_DO_FUN( do_prefi    );
DECLARE_DO_FUN( do_prefix   );
DECLARE_DO_FUN( do_prompt   );
DECLARE_DO_FUN( do_protect  );
DECLARE_DO_FUN( do_pstat    );
DECLARE_DO_FUN( do_purge    );
DECLARE_DO_FUN( do_put  );
DECLARE_DO_FUN( do_quaff    );
DECLARE_DO_FUN( do_question );
DECLARE_DO_FUN( do_qui  );
DECLARE_DO_FUN( do_quiet    );
DECLARE_DO_FUN( do_quit );
DECLARE_DO_FUN( do_ooc      );
DECLARE_DO_FUN( do_plead    );
DECLARE_DO_FUN( do_gwho     );
DECLARE_DO_FUN( do_read     );
DECLARE_DO_FUN( do_reboo    );
DECLARE_DO_FUN( do_reboot   );
DECLARE_DO_FUN( do_recall   );
DECLARE_DO_FUN( do_recho    );
DECLARE_DO_FUN( do_recite   );
DECLARE_DO_FUN( do_remove   );
DECLARE_DO_FUN( do_rent );
DECLARE_DO_FUN( do_tsect    );
DECLARE_DO_FUN( do_replay   );
DECLARE_DO_FUN( do_reply    );
DECLARE_DO_FUN( do_report   );
DECLARE_DO_FUN( do_rescue   );
DECLARE_DO_FUN( do_rest );
DECLARE_DO_FUN( do_restore  );
DECLARE_DO_FUN( do_return   );
DECLARE_DO_FUN( do_rset );
DECLARE_DO_FUN( do_rstat    );
DECLARE_DO_FUN( do_rules    );
DECLARE_DO_FUN( do_sacrifice);
DECLARE_DO_FUN( do_save );
DECLARE_DO_FUN( do_say  );
DECLARE_DO_FUN( do_osay );
DECLARE_DO_FUN( do_score    );
DECLARE_DO_FUN( do_scan );
DECLARE_DO_FUN( do_scroll   );
DECLARE_DO_FUN( do_sell );
DECLARE_DO_FUN( do_set      );
DECLARE_DO_FUN( do_shout    );
DECLARE_DO_FUN( do_show     );
DECLARE_DO_FUN( do_smite    );
DECLARE_DO_FUN( do_shutdow  );
DECLARE_DO_FUN( do_shutdown );
DECLARE_DO_FUN( do_nuk  );
DECLARE_DO_FUN( do_nuke );
DECLARE_DO_FUN( do_clearaff );
DECLARE_DO_FUN( do_sit      );
DECLARE_DO_FUN( do_skills   );
DECLARE_DO_FUN( do_sla  );
DECLARE_DO_FUN( do_slay );
DECLARE_DO_FUN( do_sleep    );
DECLARE_DO_FUN( do_slookup  );
DECLARE_DO_FUN( do_smote    );
DECLARE_DO_FUN( do_sneak    );
DECLARE_DO_FUN( do_snoop    );
DECLARE_DO_FUN( do_socials  );
DECLARE_DO_FUN( do_south    );
DECLARE_DO_FUN( do_sockets  );
DECLARE_DO_FUN( do_spells   );
DECLARE_DO_FUN( do_split    );
DECLARE_DO_FUN( do_sset );
DECLARE_DO_FUN( do_stand    );
DECLARE_DO_FUN( do_stat     );
DECLARE_DO_FUN( do_steal    );
DECLARE_DO_FUN( do_story    );
DECLARE_DO_FUN( do_string   );
DECLARE_DO_FUN( do_surrender);
DECLARE_DO_FUN( do_switch   );
DECLARE_DO_FUN( do_tell );
DECLARE_DO_FUN( do_whisper);
DECLARE_DO_FUN( do_time );
DECLARE_DO_FUN( do_title    );
DECLARE_DO_FUN( do_train    );
DECLARE_DO_FUN( do_transfer );
DECLARE_DO_FUN( do_trip     );
DECLARE_DO_FUN( do_trust    );
DECLARE_DO_FUN( do_typo );
DECLARE_DO_FUN( do_unalias  );
DECLARE_DO_FUN( do_unlock   );
DECLARE_DO_FUN( do_unread   );
DECLARE_DO_FUN( do_up       );
DECLARE_DO_FUN( do_value    );
DECLARE_DO_FUN( do_visible  );
DECLARE_DO_FUN( do_violate  );
DECLARE_DO_FUN( do_vnum     );
DECLARE_DO_FUN( do_vlist    );
DECLARE_DO_FUN( do_flagfind );
DECLARE_DO_FUN( do_wake );
DECLARE_DO_FUN( do_wear );
DECLARE_DO_FUN( do_weather  );
DECLARE_DO_FUN( do_west );
DECLARE_DO_FUN( do_where    );
DECLARE_DO_FUN( do_who  );
DECLARE_DO_FUN( do_whois    );
DECLARE_DO_FUN( do_wimpy    );
DECLARE_DO_FUN( do_wizhelp  );
DECLARE_DO_FUN( do_wizlock  );
DECLARE_DO_FUN( do_wizlist  );
DECLARE_DO_FUN( do_wiznet   );
DECLARE_DO_FUN( do_whoconfig);
DECLARE_DO_FUN( do_pipe     );
DECLARE_DO_FUN( do_wiziname );
DECLARE_DO_FUN( do_immtitle );
DECLARE_DO_FUN( do_pretitle );
DECLARE_DO_FUN( do_worth    );
DECLARE_DO_FUN( do_yell );
DECLARE_DO_FUN( do_zap  );
DECLARE_DO_FUN( do_zecho    );
DECLARE_DO_FUN( do_olc      );
DECLARE_DO_FUN( do_asave    );
DECLARE_DO_FUN( do_replayroom);
DECLARE_DO_FUN( do_replaychannels);
DECLARE_DO_FUN( do_alist    );
DECLARE_DO_FUN( do_resets   );
DECLARE_DO_FUN( do_redit    );
DECLARE_DO_FUN( do_aedit    );
DECLARE_DO_FUN( do_medit    );
DECLARE_DO_FUN( do_oedit    );
DECLARE_DO_FUN( do_mpedit   );
DECLARE_DO_FUN( do_hedit    );
DECLARE_DO_FUN( do_embrace  );
DECLARE_DO_FUN( do_vampire  );
DECLARE_DO_FUN( do_remort   );
DECLARE_DO_FUN( do_fangs    );
DECLARE_DO_FUN( do_feed     );
DECLARE_DO_FUN( do_smell    );
DECLARE_DO_FUN( do_nourish  );
DECLARE_DO_FUN( do_claws    );
DECLARE_DO_FUN( do_gleam    );
DECLARE_DO_FUN( do_sense    );
DECLARE_DO_FUN( do_reveal   );
DECLARE_DO_FUN( do_auraperception   );
DECLARE_DO_FUN( do_taste    );
DECLARE_DO_FUN( do_bloodrage   );
DECLARE_DO_FUN( do_bloodofpotency   );
DECLARE_DO_FUN( do_theft    );
DECLARE_DO_FUN( do_cauldron );
/* DECLARE_DO_FUN( do_soulsight ); */
/* DECLARE_DO_FUN( do_mindread  ); */
DECLARE_DO_FUN( do_track    );
DECLARE_DO_FUN( do_shapeshift   );
DECLARE_DO_FUN( do_shift    );
// DECLARE_DO_FUN( do_discipline    );
DECLARE_DO_FUN( do_shroudofnight    );
DECLARE_DO_FUN( do_unseenpresence   );
DECLARE_DO_FUN( do_silenceofdeath   );
DECLARE_DO_FUN( do_armsoftheabyss   );
DECLARE_DO_FUN( do_bloodagony   );
DECLARE_DO_FUN( do_veil     );
DECLARE_DO_FUN( do_earthmeld);
DECLARE_DO_FUN( do_vanish   );
DECLARE_DO_FUN( do_mask );
DECLARE_DO_FUN( do_sweetwhispers    );
DECLARE_DO_FUN( do_beckoning    );
DECLARE_DO_FUN( do_songofserenity   );
DECLARE_DO_FUN( do_drawingoutthebeast   );
DECLARE_DO_FUN( do_forgetful    );
DECLARE_DO_FUN( do_regenerate   );
DECLARE_DO_FUN( do_command  );
DECLARE_DO_FUN( do_mesmerize    );
DECLARE_DO_FUN( do_awe      );
DECLARE_DO_FUN( do_conditioning );
DECLARE_DO_FUN( do_possession   );
DECLARE_DO_FUN( do_cloakshadow);
DECLARE_DO_FUN( do_cloakthegathering);
DECLARE_DO_FUN( do_dreadgaze);
DECLARE_DO_FUN( do_shadowwalk   );
DECLARE_DO_FUN( do_shadowform);
DECLARE_DO_FUN( do_nightshades);
DECLARE_DO_FUN( do_shadowplay );
DECLARE_DO_FUN( do_settime  );
DECLARE_DO_FUN( do_resettime    );
DECLARE_DO_FUN( do_fixme    );
DECLARE_DO_FUN( do_create   );
DECLARE_DO_FUN( do_freebie  );
DECLARE_DO_FUN( do_entrancement     );
DECLARE_DO_FUN( do_majesty  );
DECLARE_DO_FUN( do_spiritsummoning  );
DECLARE_DO_FUN( do_summon   );
DECLARE_DO_FUN( do_resurrection );
DECLARE_DO_FUN( do_animatedead  );
DECLARE_DO_FUN( do_weakness );
DECLARE_DO_FUN( do_blight   );
DECLARE_DO_FUN( do_blackdeath   );
DECLARE_DO_FUN( do_acidblood    );
DECLARE_DO_FUN( do_clotblood    );
DECLARE_DO_FUN( do_bloodcurse   );
DECLARE_DO_FUN( do_assassinate  );
DECLARE_DO_FUN( do_divine_strength  );
DECLARE_DO_FUN( do_stake    );
DECLARE_DO_FUN( do_diablerize   );
DECLARE_DO_FUN( do_lore );
DECLARE_DO_FUN( do_dip  );
DECLARE_DO_FUN( do_invoke   );
DECLARE_DO_FUN( do_activate );
DECLARE_DO_FUN( do_drain    );
DECLARE_DO_FUN( do_traditiontalk    );
DECLARE_DO_FUN( do_tribetalk      );
DECLARE_DO_FUN( do_chat );
DECLARE_DO_FUN( do_autoooc);
DECLARE_DO_FUN( do_invite   );
DECLARE_DO_FUN( do_rank     );
DECLARE_DO_FUN( do_promote  );
DECLARE_DO_FUN( do_setdhampire);
DECLARE_DO_FUN( do_dhampire );
DECLARE_DO_FUN( do_sanctus  );
DECLARE_DO_FUN( do_endow    );
DECLARE_DO_FUN( do_expel    );
DECLARE_DO_FUN( do_bandage  );
DECLARE_DO_FUN( do_dictionary   );
DECLARE_DO_FUN( do_ground   );
DECLARE_DO_FUN( do_headbutt );
DECLARE_DO_FUN( do_warcry   );
DECLARE_DO_FUN( do_blast    );
DECLARE_DO_FUN( do_slip     );
DECLARE_DO_FUN( do_test     );
DECLARE_DO_FUN( do_announce );
DECLARE_DO_FUN( do_sect     );
DECLARE_DO_FUN( do_lagem    );
DECLARE_DO_FUN( do_chant    );
DECLARE_DO_FUN( do_setarena );
DECLARE_DO_FUN( do_project  );
DECLARE_DO_FUN( do_vigor    );
DECLARE_DO_FUN( do_waylay   );
DECLARE_DO_FUN( do_fortress     );
DECLARE_DO_FUN( do_qpoint   );
DECLARE_DO_FUN( do_arename  );
DECLARE_DO_FUN( do_globalpower  );
DECLARE_DO_FUN( do_spectate     );
DECLARE_DO_FUN( do_portal   );
DECLARE_DO_FUN( do_study    );
DECLARE_DO_FUN( do_brew     );
DECLARE_DO_FUN( do_skin     );
DECLARE_DO_FUN( do_mount    );
DECLARE_DO_FUN( do_dismount );
DECLARE_DO_FUN( do_obliterate   );
DECLARE_DO_FUN( do_salvation    );
DECLARE_DO_FUN( do_changeform   );
DECLARE_DO_FUN( do_fury     );
DECLARE_DO_FUN( do_totem    );
DECLARE_DO_FUN( do_bind     );
DECLARE_DO_FUN( do_induct   );
DECLARE_DO_FUN( do_rite );
DECLARE_DO_FUN( do_touch    );
DECLARE_DO_FUN( do_telepathy    );
DECLARE_DO_FUN( do_renown   );
DECLARE_DO_FUN( do_garou    );
DECLARE_DO_FUN( do_gifts    );
DECLARE_DO_FUN( do_charsheet);
DECLARE_DO_FUN( do_charsheet2);
DECLARE_DO_FUN( do_charsheet3);
DECLARE_DO_FUN( do_horrid);
DECLARE_DO_FUN( do_fleshcraft);
DECLARE_DO_FUN( do_bonecraft);
DECLARE_DO_FUN( do_changeling);
DECLARE_DO_FUN( do_essence);
DECLARE_DO_FUN( do_teach);
DECLARE_DO_FUN( do_mage);
DECLARE_DO_FUN( do_convert);
DECLARE_DO_FUN( do_sphere);
DECLARE_DO_FUN( do_rote);
DECLARE_DO_FUN( do_flaw);
DECLARE_DO_FUN( do_dash);
DECLARE_DO_FUN( do_immmask);
DECLARE_DO_FUN( do_gomage  );
DECLARE_DO_FUN( do_magecast );
DECLARE_DO_FUN( do_learn    );
DECLARE_DO_FUN( do_wipecs   );
DECLARE_DO_FUN( do_garouconvert );
DECLARE_DO_FUN( do_rage );
DECLARE_DO_FUN( do_stepsideways );
DECLARE_DO_FUN( do_chop     );
DECLARE_DO_FUN( do_prospect );
DECLARE_DO_FUN( do_mine );
DECLARE_DO_FUN( do_plane    );
DECLARE_DO_FUN( do_carve    );
DECLARE_DO_FUN( do_register );
DECLARE_DO_FUN( do_email    );
DECLARE_DO_FUN( do_qlog);
DECLARE_DO_FUN( do_contact);
//XP Boost code / vampire regen by Zelan
DECLARE_DO_FUN( do_xp_boost);
DECLARE_DO_FUN( do_fatamorgana);
DECLARE_DO_FUN( do_vampire_regen);
DECLARE_DO_FUN( do_ignis_fatuus);
// New dice code
DECLARE_DO_FUN( do_newdice);
/*New code by Sengir*/
//DECLARE_DO_FUN( do_celerity );
DECLARE_DO_FUN( do_celerity  );
DECLARE_DO_FUN( do_incorporealpassage );
DECLARE_DO_FUN( do_bloodbuff   );
DECLARE_DO_FUN( do_behead   );
DECLARE_DO_FUN( do_arise    );
DECLARE_DO_FUN( do_showhelpindex);
//New code by Matthew
DECLARE_DO_FUN( do_backgrounds);
DECLARE_DO_FUN( do_bank);
DECLARE_DO_FUN( do_clanbank);
DECLARE_DO_FUN( do_pload );
DECLARE_DO_FUN( do_punload);
DECLARE_DO_FUN( do_forcebank);
DECLARE_DO_FUN( do_forceinfluence);
DECLARE_DO_FUN( do_makehelper);
DECLARE_DO_FUN( do_pnewpass);
DECLARE_DO_FUN( recrypt_pfiles);
DECLARE_DO_FUN( do_allies );
DECLARE_DO_FUN( do_checkhelps);
DECLARE_DO_FUN( do_checktypos);
DECLARE_DO_FUN( do_checkbugs);
DECLARE_DO_FUN( do_systemlog);
DECLARE_DO_FUN( do_funlist);
DECLARE_DO_FUN( do_cmdedit  );
DECLARE_DO_FUN(do_potency);
DECLARE_DO_FUN(do_gogarou);
DECLARE_DO_FUN(do_wield);
DECLARE_DO_FUN(do_pray);
DECLARE_DO_FUN(do_spitblood);
DECLARE_DO_FUN(do_coat);
DECLARE_DO_FUN(do_sheet);
DECLARE_DO_FUN(do_cs);
DECLARE_DO_FUN(do_tbtalk);
DECLARE_DO_FUN(do_tdtalk);
DECLARE_DO_FUN(do_cs2);
DECLARE_DO_FUN(do_cs3);
DECLARE_DO_FUN(do_prayers);
DECLARE_DO_FUN(do_combat);
DECLARE_DO_FUN(do_color);
DECLARE_DO_FUN(do_ansi);
DECLARE_DO_FUN(do_hold);
DECLARE_DO_FUN(do_bs);
DECLARE_DO_FUN(do_journal);
DECLARE_DO_FUN(do_wikihelp);
DECLARE_DO_FUN(do_poofin);
DECLARE_DO_FUN(do_poofout);
DECLARE_DO_FUN(do_gecho);
DECLARE_DO_FUN(do_echo);
DECLARE_DO_FUN(do_wizinvis);
DECLARE_DO_FUN(do_quest);
DECLARE_DO_FUN(do_edit);
DECLARE_DO_FUN(do_ticks);
DECLARE_DO_FUN(do_allowimmtalk);
DECLARE_DO_FUN(do_becomeactive);
DECLARE_DO_FUN(do_makeactive);
DECLARE_DO_FUN( do_quest        );
DECLARE_DO_FUN( do_cedit                );
DECLARE_DO_FUN( do_save_clans           ); /* clan.c */
DECLARE_DO_FUN(do_write);
DECLARE_DO_FUN(do_seal);
DECLARE_DO_FUN(do_unseal);
DECLARE_DO_FUN(do_meditate);
/* Additions by The Maniac for Languages */
DECLARE_DO_FUN( do_common       );
DECLARE_DO_FUN( do_human        );
DECLARE_DO_FUN( do_dwarvish     );
DECLARE_DO_FUN( do_elvish       );
DECLARE_DO_FUN( do_gnomish      );
DECLARE_DO_FUN( do_goblin       );
DECLARE_DO_FUN( do_orcish       );
DECLARE_DO_FUN( do_ogre         );
DECLARE_DO_FUN( do_drow         );
DECLARE_DO_FUN( do_kobold       );
DECLARE_DO_FUN( do_trollish     );
DECLARE_DO_FUN( do_hobbit       );
DECLARE_DO_FUN( do_lstat        );      /* language stat */
DECLARE_DO_FUN( do_lset         );      /* Language set */
DECLARE_DO_FUN( do_learn        );      /* learn a language */
DECLARE_DO_FUN( do_speak        );      /* Select a language to speak */
DECLARE_DO_FUN( do_donate       );

/* language.c */
void    do_language     args( ( CHAR_DATA *ch, char *argument, int language) );
int     lang_lookup     args( ( const char *name ) );