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


/*
 * Spell functions.
 * Defined in magic.c.
 */
//XP boosting stuff
DECLARE_SPELL_FUN(	spell_xp_boost          );
DECLARE_SPELL_FUN(	spell_ignis_fatuus	);
DECLARE_SPELL_FUN(	spell_null		);
DECLARE_SPELL_FUN(	spell_acid_blast	);
DECLARE_SPELL_FUN(	spell_armor		);
DECLARE_SPELL_FUN(	spell_bless		);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_call_lightning	);
DECLARE_SPELL_FUN(      spell_calm		);
DECLARE_SPELL_FUN(      spell_cancellation	);
DECLARE_SPELL_FUN(	spell_cause_critical	);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_change_sex	);
DECLARE_SPELL_FUN(	spell_change_alignment  );
DECLARE_SPELL_FUN(      spell_chain_lightning   );
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_chill_touch	);
DECLARE_SPELL_FUN(	spell_claws		);
DECLARE_SPELL_FUN(	spell_colour_spray	);
DECLARE_SPELL_FUN(	spell_continual_light	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_create_food	);
DECLARE_SPELL_FUN(	spell_create_rose	);
DECLARE_SPELL_FUN(	spell_create_spring	);
DECLARE_SPELL_FUN(	spell_create_water	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_critical	);
DECLARE_SPELL_FUN(      spell_cure_disease	);
DECLARE_SPELL_FUN(	spell_cure_light	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_cure_serious	);
DECLARE_SPELL_FUN(	spell_curse		);
DECLARE_SPELL_FUN(      spell_demonfire		);
DECLARE_SPELL_FUN(	spell_detect_evil	);
DECLARE_SPELL_FUN(	spell_detect_good	);
DECLARE_SPELL_FUN(	spell_detect_hidden	);
DECLARE_SPELL_FUN(	spell_detect_invis	);
DECLARE_SPELL_FUN(	spell_detect_magic	);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(      spell_dispel_good       );
DECLARE_SPELL_FUN(	spell_dispel_magic	);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_armor	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_farsight		);
DECLARE_SPELL_FUN(	spell_fireball		);
DECLARE_SPELL_FUN(	spell_fireproof		);
DECLARE_SPELL_FUN(	spell_flamestrike	);
DECLARE_SPELL_FUN(	spell_floating_disc	);
DECLARE_SPELL_FUN(	spell_fly		);
DECLARE_SPELL_FUN(      spell_frenzy		);
DECLARE_SPELL_FUN(	spell_gate		);
DECLARE_SPELL_FUN(	spell_giant_strength	);
DECLARE_SPELL_FUN(	spell_harm		);
DECLARE_SPELL_FUN(      spell_haste		);
DECLARE_SPELL_FUN(	spell_heal		);
DECLARE_SPELL_FUN(	spell_heat_metal	);
DECLARE_SPELL_FUN(      spell_holy_word		);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_infravision	);
DECLARE_SPELL_FUN(	spell_intervention	);
DECLARE_SPELL_FUN(  spell_invigorate        );
DECLARE_SPELL_FUN(	spell_invis		);
DECLARE_SPELL_FUN(	spell_know_alignment	);
DECLARE_SPELL_FUN(	spell_lightning_bolt	);
DECLARE_SPELL_FUN(	spell_locate_object	);
DECLARE_SPELL_FUN(	spell_magic_missile	);
DECLARE_SPELL_FUN(      spell_mass_healing	);
DECLARE_SPELL_FUN(	spell_mass_invis	);
DECLARE_SPELL_FUN(	spell_nexus		);
DECLARE_SPELL_FUN(	spell_pass_door		);
DECLARE_SPELL_FUN(      spell_plague		);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_portal		);
DECLARE_SPELL_FUN(	spell_protection_evil	);
DECLARE_SPELL_FUN(	spell_protection_good	);
DECLARE_SPELL_FUN(	spell_ray_of_truth	);
DECLARE_SPELL_FUN(	spell_recharge		);
DECLARE_SPELL_FUN(	spell_refresh		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_sanctuary		);
DECLARE_SPELL_FUN(	spell_lightning_fist	);
DECLARE_SPELL_FUN(	spell_shield		);
DECLARE_SPELL_FUN(	spell_sleep		);
DECLARE_SPELL_FUN(	spell_slow		);
DECLARE_SPELL_FUN(	spell_stone_skin	);
DECLARE_SPELL_FUN(	spell_summon		);
DECLARE_SPELL_FUN(	spell_teleport		);
DECLARE_SPELL_FUN(	vamp_frenzy		);
DECLARE_SPELL_FUN(	garou_frenzy		);
DECLARE_SPELL_FUN(	thaumaturgy_frenzy	);
DECLARE_SPELL_FUN(	spell_elemental	);
DECLARE_SPELL_FUN(	spell_fireshield	);
DECLARE_SPELL_FUN(	spell_holyshield	);
DECLARE_SPELL_FUN(	spell_darkshield	);
DECLARE_SPELL_FUN(	spell_shockshield	);
DECLARE_SPELL_FUN(	spell_mana	);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_word_of_recall	);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(	spell_caern_rage	);
DECLARE_SPELL_FUN(	spell_caern_healing	);
DECLARE_SPELL_FUN(	spell_zeal		);
DECLARE_SPELL_FUN(	spell_wrath_of_god	);
DECLARE_SPELL_FUN(	spell_agg_damage	);
DECLARE_SPELL_FUN(	spell_blood		);
DECLARE_SPELL_FUN(  spell_vampire_blood);
DECLARE_SPELL_FUN(	spell_gift_staredown);
DECLARE_SPELL_FUN(	spell_gift_porcupine);
DECLARE_SPELL_FUN(	spell_gift_blissfulignorance);
DECLARE_SPELL_FUN(	spell_gift_pulseoftheinvisible);
DECLARE_SPELL_FUN(	spell_gift_catfeet);
DECLARE_SPELL_FUN(	spell_gift_resistpain);
DECLARE_SPELL_FUN(	spell_gift_callofthewyld);
DECLARE_SPELL_FUN(	spell_gift_razorclaws);
DECLARE_SPELL_FUN(	spell_gift_silverclaws);
DECLARE_SPELL_FUN(	spell_gift_sensewyrm);
DECLARE_SPELL_FUN(	spell_gift_innerstrength);
DECLARE_SPELL_FUN(	spell_gift_bloodyfeast);
DECLARE_SPELL_FUN(  spell_gift_masteroffire);
/*Mage spell stuff*/
DECLARE_SPELL_FUN(	spell_rote_quintessence);

/*GIFTS!!*/

DECLARE_SPELL_FUN(spell_gift_persuasion);
DECLARE_SPELL_FUN(spell_gift_speechoftheworld);
DECLARE_SPELL_FUN(spell_gift_calmthesavagebeast);
DECLARE_SPELL_FUN(spell_gift_reshapeobject);
DECLARE_SPELL_FUN(spell_gift_burythewolf);
DECLARE_SPELL_FUN(spell_gift_spiritward);
DECLARE_SPELL_FUN(spell_gift_assimilation);
DECLARE_SPELL_FUN(spell_gift_falsecomfort);
DECLARE_SPELL_FUN(spell_gift_createelement);
DECLARE_SPELL_FUN(spell_gift_shed);
DECLARE_SPELL_FUN(spell_gift_curseofhatred);
DECLARE_SPELL_FUN(spell_gift_sensesilver);
DECLARE_SPELL_FUN(spell_gift_eyesofthecat);
DECLARE_SPELL_FUN(spell_gift_mentalspeech);
DECLARE_SPELL_FUN(spell_gift_witherlimb);
DECLARE_SPELL_FUN(spell_gift_madness);
DECLARE_SPELL_FUN(spell_gift_totemgift);
DECLARE_SPELL_FUN(spell_gift_haresleap);
DECLARE_SPELL_FUN(spell_gift_heightenedsenses);
DECLARE_SPELL_FUN(spell_gift_leylines);
DECLARE_SPELL_FUN(spell_gift_scentofsight);
DECLARE_SPELL_FUN(spell_gift_sensetheunnatural);
DECLARE_SPELL_FUN(spell_gift_devilschild);
DECLARE_SPELL_FUN(spell_gift_namethespirit);
DECLARE_SPELL_FUN(spell_gift_beastlife);
DECLARE_SPELL_FUN(spell_gift_gnaw);
DECLARE_SPELL_FUN(spell_gift_elementalgift);
DECLARE_SPELL_FUN(spell_gift_songofthegreatbeast);
DECLARE_SPELL_FUN(spell_gift_blurofthemilkyeye);
DECLARE_SPELL_FUN(spell_gift_scentofrunningwater);
DECLARE_SPELL_FUN(spell_gift_snowrunning);
DECLARE_SPELL_FUN(spell_gift_senseofprey);
DECLARE_SPELL_FUN(spell_gift_openmoonbridge);
DECLARE_SPELL_FUN(spell_gift_reynardslie);
DECLARE_SPELL_FUN(spell_gift_leperscurse);
DECLARE_SPELL_FUN(spell_gift_lunasblessing);
DECLARE_SPELL_FUN(spell_gift_thievingtalonsofthemagpie);
DECLARE_SPELL_FUN(spell_gift_chimericalform);
DECLARE_SPELL_FUN(spell_gift_motherstouch);
DECLARE_SPELL_FUN(spell_gift_spiritspeech);
DECLARE_SPELL_FUN(spell_gift_commandspirit);
DECLARE_SPELL_FUN(spell_gift_sightfrombeyond);
DECLARE_SPELL_FUN(spell_gift_expelspirit);
DECLARE_SPELL_FUN(spell_gift_graspthebeyond);
DECLARE_SPELL_FUN(spell_gift_spiritwassail);
DECLARE_SPELL_FUN(spell_gift_nightmarepossession);
DECLARE_SPELL_FUN(spell_gift_malleablespirit);
DECLARE_SPELL_FUN(spell_gift_scentofthetrueform);
DECLARE_SPELL_FUN(spell_gift_truthofgaia);
DECLARE_SPELL_FUN(spell_gift_callofduty);
DECLARE_SPELL_FUN(spell_gift_strengthofpurpose);
DECLARE_SPELL_FUN(spell_gift_spiritwitness);
DECLARE_SPELL_FUN(spell_gift_warriorseye);
DECLARE_SPELL_FUN(spell_gift_rollover);
DECLARE_SPELL_FUN(spell_gift_scentofbeyond);
DECLARE_SPELL_FUN(spell_gift_earthfriendspact);
DECLARE_SPELL_FUN(spell_gift_geas);
DECLARE_SPELL_FUN(spell_gift_beastspirit);
DECLARE_SPELL_FUN(spell_gift_wakingdream);
DECLARE_SPELL_FUN(spell_gift_callofthewyrm);
DECLARE_SPELL_FUN(spell_gift_distractions);
DECLARE_SPELL_FUN(spell_gift_eyeoftheasp);
DECLARE_SPELL_FUN(spell_gift_songofrage);
DECLARE_SPELL_FUN(spell_gift_shadowsbythefirelight);
DECLARE_SPELL_FUN(spell_gift_songofmockery);
DECLARE_SPELL_FUN(spell_gift_dreamgolems);
DECLARE_SPELL_FUN(spell_gift_callforvengeance);
DECLARE_SPELL_FUN(spell_gift_fallingtouch);
DECLARE_SPELL_FUN(spell_gift_inspiration);
DECLARE_SPELL_FUN(spell_gift_spiritofthefray);
DECLARE_SPELL_FUN(spell_gift_truefear);
DECLARE_SPELL_FUN(spell_gift_heartoffury);
DECLARE_SPELL_FUN(spell_gift_fullmoonslight);
DECLARE_SPELL_FUN(spell_gift_stokingfurysfurnace);
DECLARE_SPELL_FUN(spell_gift_kissofhelios);
DECLARE_SPELL_FUN(spell_gift_strengthofwill);
DECLARE_SPELL_FUN(spell_gift_breathofthewyld);
DECLARE_SPELL_FUN(spell_gift_mansskin);
DECLARE_SPELL_FUN(spell_gift_curseoftheaeolus);
DECLARE_SPELL_FUN(spell_gift_coupdegrace);
DECLARE_SPELL_FUN(spell_gift_flamesofhestia);
DECLARE_SPELL_FUN(spell_gift_bodywrack);
DECLARE_SPELL_FUN(spell_gift_bowofpegasus);
DECLARE_SPELL_FUN(spell_gift_gazeofthegorgon);
DECLARE_SPELL_FUN(spell_gift_wyldwarp);
DECLARE_SPELL_FUN(spell_gift_cookery);
DECLARE_SPELL_FUN(spell_gift_resisttoxin);
DECLARE_SPELL_FUN(spell_gift_scentofthehoneycomb);
DECLARE_SPELL_FUN(spell_gift_corneredrat);
DECLARE_SPELL_FUN(spell_gift_plaguevisage);
DECLARE_SPELL_FUN(spell_gift_calltherust);
DECLARE_SPELL_FUN(spell_gift_friendinneed);
DECLARE_SPELL_FUN(spell_gift_blink);
DECLARE_SPELL_FUN(spell_gift_clawsoftheglassblower);
DECLARE_SPELL_FUN(spell_gift_mawofthewhale);
DECLARE_SPELL_FUN(spell_gift_survivor);
DECLARE_SPELL_FUN(spell_gift_evesblessing);
DECLARE_SPELL_FUN(spell_gift_grandmotherstouch);
DECLARE_SPELL_FUN(spell_gift_lunasarmor);
DECLARE_SPELL_FUN(spell_gift_dazzle);
DECLARE_SPELL_FUN(spell_gift_spiritfriend);
DECLARE_SPELL_FUN(spell_gift_angelssemblance);
DECLARE_SPELL_FUN(spell_gift_haloofthesun);
DECLARE_SPELL_FUN(spell_gift_thelivingwood);
DECLARE_SPELL_FUN(spell_gift_huntersharmony);
DECLARE_SPELL_FUN(spell_gift_visageoffenris);
DECLARE_SPELL_FUN(spell_gift_haltthecowardsflight);
DECLARE_SPELL_FUN(spell_gift_senseguilt);
DECLARE_SPELL_FUN(spell_gift_mightofthor);
DECLARE_SPELL_FUN(spell_gift_berserkerssong);
DECLARE_SPELL_FUN(spell_gift_ironcannotpierce);
DECLARE_SPELL_FUN(spell_gift_blazeofglory);
DECLARE_SPELL_FUN(spell_gift_hordeofvalhalla);
DECLARE_SPELL_FUN(spell_gift_distractthefool);
DECLARE_SPELL_FUN(spell_gift_faerielight);
DECLARE_SPELL_FUN(spell_gift_howlofthebanshee);
DECLARE_SPELL_FUN(spell_gift_warriorsdraught);
DECLARE_SPELL_FUN(spell_gift_faerieaid);
DECLARE_SPELL_FUN(spell_gift_woadling);
DECLARE_SPELL_FUN(spell_gift_balorsgaze);
DECLARE_SPELL_FUN(spell_gift_semblance);
DECLARE_SPELL_FUN(spell_gift_fogonthemoor);
DECLARE_SPELL_FUN(spell_gift_giftofthespriggan);
DECLARE_SPELL_FUN(spell_gift_wolfatthedoor);
DECLARE_SPELL_FUN(spell_gift_auraofconfidence);
DECLARE_SPELL_FUN(spell_gift_fatalflaw);
DECLARE_SPELL_FUN(spell_gift_seizingtheedge);
DECLARE_SPELL_FUN(spell_gift_clapofthunder);
DECLARE_SPELL_FUN(spell_gift_ravenscurse);
DECLARE_SPELL_FUN(spell_gift_shadowcutting);
DECLARE_SPELL_FUN(spell_gift_dolorousstroke);
DECLARE_SPELL_FUN(spell_gift_strengthofthedominator);
DECLARE_SPELL_FUN(spell_gift_shadowpack);
DECLARE_SPELL_FUN(spell_gift_obedience);
DECLARE_SPELL_FUN(spell_gift_speedofthought);
DECLARE_SPELL_FUN(spell_gift_silence);
DECLARE_SPELL_FUN(spell_gift_eyeofthefalcon);
DECLARE_SPELL_FUN(spell_gift_lambentfire);
DECLARE_SPELL_FUN(spell_gift_empathy);
DECLARE_SPELL_FUN(spell_gift_exceptionalswordplay);
DECLARE_SPELL_FUN(spell_gift_wrathofgaia);
DECLARE_SPELL_FUN(spell_gift_lordlywill);
DECLARE_SPELL_FUN(spell_gift_sidestepdeath);
DECLARE_SPELL_FUN(spell_gift_lunasavenger);
DECLARE_SPELL_FUN(spell_gift_pawsofthenewborncub);
DECLARE_SPELL_FUN(spell_gift_artisanscommand);
DECLARE_SPELL_FUN(spell_gift_giftofsalt);
DECLARE_SPELL_FUN(spell_gift_maskthepredator);
DECLARE_SPELL_FUN(spell_gift_steelfur);
DECLARE_SPELL_FUN(spell_gift_tongues);
DECLARE_SPELL_FUN(spell_gift_attunement);
DECLARE_SPELL_FUN(spell_gift_doppelganger);
DECLARE_SPELL_FUN(spell_gift_commandtheblaze);
DECLARE_SPELL_FUN(spell_gift_redirectpain);
