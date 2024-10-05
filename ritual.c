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

void do_drain(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *bpotion;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *vial;
    char buf[MAX_STRING_LENGTH], arg[MIL];
    bool vfound;
    int success;
    vfound = FALSE;
    obj = vial = ch->carrying;
    argument = one_argument(argument, arg);

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if(obj->pIndexData->vnum == OBJ_VNUM_EMPTY_VIAL && !vfound)
        {
            if(!str_cmp(obj->name, "empty vial")) 
            {
                vial = obj;
                vfound = TRUE;
            }
            break;
        }
    }
    if(!vfound)
    {
        send_to_char( "You do not have an empty vial to drain your blood into.\n\r", ch );
        return;
    }
    if((bpotion = create_object(get_obj_index(OBJ_VNUM_BREW_POTION),0)) == NULL)
    {
        send_to_char( "Error! Contact the imms about this missing potion!\n\r", ch );
        return;
    }
    if (arg[0] == '\0' || !str_cmp(arg, "vial"))
    {
        if(ch->race == race_lookup("dhampire") || ch->race == race_lookup("garou"))
        {
            send_to_char( "Your blood would be useless, so there's no reason to drain it into a vial.\n\r",ch);
            return;
        }

        bpotion->level = 1;
        if (ch->race == race_lookup("human") || ch->race == race_lookup("ghoul"))
        {
            bpotion->value[0] = 15;
            sprintf(buf, "a vial of %s's blood",ch->name);
            bpotion->short_descr = str_dup(buf);
            sprintf(buf, "vial blood potion human %s", ch->name);
            bpotion->name = str_dup(buf);
            bpotion->value[1] = skill_lookup("human blood");
            if (ch->level > 50)
                bpotion->value[2] = skill_lookup("human blood");
        }
        else if (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
        {
            bpotion->value[0] = 10;
            bpotion->value[1] = skill_lookup("kindred vitae");
            if (ch->gen <= 6)
                bpotion->value[3] = skill_lookup("kindred vitae");
            sprintf(buf, "a vial of %s of %s's blood", ch->name, capitalize(clan_table[ch->clan].name));
            bpotion->short_descr = str_dup(buf);
            sprintf(buf, "vial blood potion kindred vitae %s", ch->name);
            bpotion->name = str_dup(buf);
        }
        if (ch->csmax_willpower == 0)
        {
            send_to_char("You do not have the willpower to drain your blood into a vial.\n\r", ch);
            return;
        }
        success = godice(ch->csmax_willpower, 5);

        if (success < 0)
        {
            act("$n drops a vial and it shatters into a million pieces!", ch,NULL,NULL,TO_ROOM);
            act("You drop the vial and it shatters into a million pieces!", ch,NULL,NULL,TO_CHAR);
            WAIT_STATE(ch, 12);
            extract_obj( vial );
            return;
        }
        else if (success == 0)
        {
            act("You struggle to overcome your self preserving instincts...\n\rand cannot bring yourself to drain any blood.\n\r", ch, NULL, NULL, TO_CHAR);
            WAIT_STATE(ch, 12);
            return;
        }
        else
        {
            if (IS_VAMP(ch))
            {
                if (ch->pblood < 15)
                {
                    send_to_char("You do not have enough blood!\n\r", ch);
                    return;
                }
                else
                    ch->pblood -= 10;
            }
            else
            {
                if (ch->hit <= (ch->max_hit/10)+5)
                {
                    send_to_char("You cannot afford to lose any more blood!\n\r", ch);
                    return;
                }
                else
                    ch->hit -= ch->max_hit/10;
            }

            send_to_char("You pierce your wrist and drain blood into an empty vial.\n\r",ch);
            act("$n pierces $s wrist, draining $s blood into an empty vial.", ch, NULL, ch, TO_NOTVICT );
            WAIT_STATE(ch, 12);
            extract_obj(vial);
            obj_to_char(bpotion,ch);
            return;
        }
    }
    else
    {
        CHAR_DATA *victim;
        if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }
        if (!IS_NPC(victim))
        {
            send_to_char("Not on players.\n\r", ch);
            return;
        }
        if(!IS_VAMP(victim))
        {
            send_to_char("That is not a vampire, their blood will not keep in a vial.\n\r",ch);
            return;
        }
        if (victim->position != POS_TORPOR)
        {
            send_to_char("They must be in Torpor before you can steal their vampiric vitae!\n\r", ch);
            return;
        }

        act( "You attempt to pierce one of $N's veins to steal their vitae...",  ch, NULL, victim, TO_CHAR );
        success = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_MEDICINE], 6);
        if (success < 0)
        {
            send_to_char("..and fail horribly, pushing their torpid body into Final death.\n\r", ch);
            act("$n tries to drain blood from $N... and only succeeds in killing $M.", ch, NULL, victim, TO_NOTVICT);
            kill_em(ch, victim);
            WAIT_STATE(ch, 24);
            return;
        }
        else if (success == 0)
        {
            send_to_char("... and miss the vein completely.\n\r", ch);
            act("$n tries to drain blood from $N... but misses the vein!", ch, NULL, victim, TO_NOTVICT);
            WAIT_STATE(ch, 24);
            return;
        }
        else
        {
            send_to_char("... and succeed in draining their blood into a vial.\n\r", ch);
            act("$n tries to drain blood from $N... and fills a vial with the crimson liquid.", ch, NULL, victim, TO_NOTVICT);
            bpotion->value[0] = 15 + (success * 2);
            bpotion->value[1] = skill_lookup("kindred vitae");
            sprintf(buf, "a vial of %s's blood", victim->short_descr);
            bpotion->short_descr = str_dup(buf);
            sprintf(buf, "%s vial blood potion kindred vitae", victim->name);
            bpotion->name = str_dup(buf);
            extract_obj(vial);
            obj_to_char(bpotion,ch);
            kill_em(ch, victim);
            gain_exp(ch,success*5);
            WAIT_STATE(ch, 24);
            return;
        }
    }
}

void do_invoke(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *victim;
    CHAR_DATA *vict_next;
    AFFECT_DATA af;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char *str1;
    char *str2;
    int acount;
    int bcount;
    int ccount;
    int dcount;
    int ecount;
    int fcount;
    int gcount;
    int hcount;
    int chance;

    chance = number_range(1, 100);

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    acount = 0;
    bcount = 0;
    ccount = 0;
    dcount = 0;
    ecount = 0;
    fcount = 0;
    gcount = 0;
    hcount = 0;


    if ( arg1[0] == '\0' )
    {
        send_to_char( "Invoke which Ritual?\n\r", ch );
        return;
    }


    if (!str_prefix(arg1,"wrath of fungus"))
    {

        if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
        {
            send_to_char( "At whom?\n\r", ch );
            return;
        }

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(!str_cmp(obj->short_descr, "red moss") && acount < 1)
            {
                extract_obj( obj );
                acount += 1;
            }
        }

    act("$n starts to chant and dance in a circle!", ch,NULL,NULL,TO_ROOM);
    act("You place the components in a circle and begin to chant.", ch,NULL,NULL,TO_CHAR);
    if(acount == 1)
    {
        if(!IS_IMMORTAL(ch))
            WAIT_STATE( ch, 12 );

        if( chance <= 30 - get_curr_stat(ch,STAT_INT))
        {
            act("$n curses as they mispronounce a syllable and fails the ritual!", ch,NULL,NULL,TO_ROOM);
            act("You mispronounce a syllable and fail to invoke the ritual!", ch,NULL,NULL,TO_CHAR);
            return;
        }
        act("$n casts a web of fungus over $N!", ch,NULL,victim,TO_ROOM);
        act("You throw a web of fungus over $N!", ch,NULL,victim,TO_CHAR);
        act("You struggle as $n casts a web of fungus over you!", ch,NULL,victim,TO_VICT);
        spell_slow( skill_lookup("slow"), ch->level*2, ch, victim,TAR_CHAR_OFFENSIVE);
        return;
        }
        else
        {
            send_to_char( "You fail to invoke the Ritual!\n\r", ch );
            return;
        }
    }

    if (!str_prefix(arg1,"matthews touch"))
    {
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(!str_cmp(obj->short_descr, "a chunk of platinum") && acount < 1)
            {
                extract_obj( obj );
                acount += 1;
            }
        }
    act("$n starts to chant and dance in a circle!", ch,NULL,NULL,TO_ROOM);
    act("You place the components in a circle and begin to chant.", ch,NULL,NULL,TO_CHAR);
    if(acount == 1)
    {
        if(!IS_IMMORTAL(ch))
            WAIT_STATE( ch, PULSE_TICK/3 );

        act("$n looks completely refreshed as they call down Matthew's touch.", ch,NULL,NULL,TO_ROOM);
        act("You are totally awed as Matthew restores your wounds.", ch,NULL,NULL,TO_CHAR);

        restore_one(ch, ch, FALSE);
        af.duration  = number_range(20, 25);
        af.where     = TO_AFFECTS;
        af.type      = gsn_touch;
        af.level     = ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SANCTUARY;
        affect_to_char( ch, &af );

        return;
        }
        else
        {
            send_to_char( "You do not have the required shiny sacrifice!\n\r", ch );
            return;
        }
    }

    if (!str_prefix(arg1,"ygolonacs tentacles"))
    {
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(!str_cmp(obj->short_descr, "a chunk of platinum") && acount < 1)
            {
                extract_obj( obj );
                acount += 1;
            }
        }
    act("$n starts to chant and dance in a circle!", ch,NULL,NULL,TO_ROOM);
    act("You place the components in a circle and begin to chant.", ch,NULL,NULL,TO_CHAR);
    if(acount == 1)
    {
        if(!IS_IMMORTAL(ch))
            WAIT_STATE( ch, PULSE_TICK );

        act("\n\r{D<<<<<<<<<<<<<<<<<<<<[[[[[[[[[[|||||||||||||||]]]]]]]]]]>>>>>>>>>>>>>>>>>>>>{x", ch, NULL, NULL, TO_ALL);
        act("Ygolonac manifests suddenly, and begins to dance and chant in a hypnotic", ch,NULL,NULL,TO_ALL);
        act("{xrhythm.  A mad cacophony fills your head with unintelligible, yet fully", ch,NULL,NULL,TO_ALL);
        act("{xunderstood words.  {W'In his house at R'lyeh, dead Cthulhu waits, dreaming.'{x", ch,NULL,NULL,TO_ALL);
        act("{D<<<<<<<<<<<<<<<<<<<<[[[[[[[[[[|||||||||||||||]]]]]]]]]]>>>>>>>>>>>>>>>>>>>>{x", ch, NULL, NULL, TO_ALL);

        for ( victim = char_list; victim != NULL; victim = vict_next )
        {
          vict_next = victim->next;
          if (victim->in_room == NULL || victim == ch)
            continue;

          if ( victim->in_room == ch->in_room && SAME_UMBRA(ch, victim))
          {
            if (IS_IMMORTAL(victim) || victim->level > ch->level ||
                IS_SET(victim->act2, ACT2_ULTRA_MOB) || IS_SET(victim->act, ACT_QUESTMOB) ||
               (IS_NPC(victim) && victim->pIndexData->pShop != NULL) )
            {
              act("A small rivulet of {rblood{x creeps from your nose, as the world twists and", ch, NULL, victim, TO_VICT);
              act("{xmalforms momentarily.  Everything seems {yw{xr{yo{xn{yg{x for a brief moment, but it", ch, NULL, victim, TO_VICT);
              act("{xall appears normal after a few fleeting moments of time.", ch, NULL, victim, TO_VICT);
              act("{D<<<<<<<<<<<<<<<<<<<<[[[[[[[[[[|||||||||||||||]]]]]]]]]]>>>>>>>>>>>>>>>>>>>>{x", ch, NULL, victim, TO_VICT);
              continue;
            }

            act("{D<<<<<<<<<<<<<<<<<<<<[[[[[[[[[[|||||||||||||||]]]]]]]]]]>>>>>>>>>>>>>>>>>>>>{x", ch, NULL, victim, TO_VICT);
            act("Dark tentacles manifest at the edge of your vision, waving rhythmically to", ch, NULL, victim, TO_VICT);
            act("Ygolonac's chanting.  Trying to catch a better glimpse, they disappear and", ch, NULL, victim, TO_VICT);
            act("{xthe entire world darkens; all you know ceases to exist.", ch, NULL, victim, TO_VICT);
            act("$n slumps to the ground, $s body completely devoid of signs of life.", victim, NULL, NULL, TO_ROOM);
            raw_kill( victim );
            continue;
          }
        }

        act("{D<<<<<<<<<<<<<<<<<<<<[[[[[[[[[[|||||||||||||||]]]]]]]]]]>>>>>>>>>>>>>>>>>>>>{x", ch, NULL, NULL, TO_CHAR);
        act("Dark tentacles manifest at the edge of your vision, waving rhythmically to", ch, NULL, NULL, TO_CHAR);
        act("Ygolonac's chanting.  Trying to catch a better glimpse, they disappear and", ch, NULL, NULL, TO_CHAR);
        act("{xthe entire world darkens; all you know ceases to exist.", ch, NULL, victim, TO_CHAR);
        act("$n slumps to the ground, $s body completely devoid of signs of life.", ch, NULL, NULL, TO_ROOM);
        raw_kill( ch );
        return;
    }
    else
    {
      send_to_char( "Ygolonac requires an expensive, shiny sacrifice.\n\r", ch );
      return;
    }
  }

send_to_char( "Invoke which Ritual?\n\r", ch );
return;
}

void do_activate(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];


    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Activate which stored Ritual?\n\r", ch );
        return;
    }
    if (!str_prefix(arg1,"assamite acid"))
    {
        if ( !is_affected( ch, gsn_store_assamiteacid ) )
        {
            send_to_char( "You do not have that Ritual stored!\n\r", ch );
            return;
        }
        if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
        {
            send_to_char( "Who?\n\r", ch );
            return;
        }

        act("$n activates the Ritual Assamite Acid!", ch,NULL,NULL,TO_ROOM);
        act("You activate the Ritual Assamite Acid!.", ch,NULL,NULL,TO_CHAR);
        affect_strip(ch,gsn_store_assamiteacid);
        damage( ch, victim, 200, gsn_store_assamiteacid, DAM_ACID, TRUE);
        return;
    }


    if (!str_prefix(arg1,"inferno"))
    {
        if ( !is_affected( ch, gsn_store_inferno ) )
        {
            send_to_char( "You do not have that Ritual stored!\n\r", ch );
            return;
        }

        if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
        {
            send_to_char( "Who?\n\r", ch );
            return;
        }

        act("$n activates the Ritual Inferno!", ch,NULL,NULL,TO_ROOM);
        act("You activate the Ritual Inferno!.", ch,NULL,NULL,TO_CHAR);
        affect_strip(ch,gsn_store_inferno);
        damage( ch, victim, 1000, gsn_store_inferno, DAM_FIRE, TRUE);
        return;
    }
send_to_char( "Activate which Ritual?\n\r", ch );
return;
}

void do_magecast(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Cast which spell upon whom?\n\r", ch );
        return;
    }

    if(ch->mana <= 0)
    {
        send_to_char( "You don't have enough mana!\n\r", ch );
        return;
    }

    if (ch->clan == clan_lookup("mage"))
    {
            send_to_char("You do not need this form of Magick. Use the 'rote' command.\n\r",ch);
            return;
    }
    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
        send_to_char( "Cast which spell upon whom?\n\r", ch );
        return;
    }
/* disabled due to removing clan perks - Ugha
    if (ch->thaumaturgy >= 5 && ch->clan == clan_lookup("tremere"))
    {
        if (!str_prefix(arg1,"shield of darkness"))
        {
            act("$n calls upon the powers of darkness!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of darkness!.", ch,NULL,NULL,TO_CHAR);
            spell_darkshield( skill_lookup("dark aura"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 100;
            WAIT_STATE( ch, 6 );
            return;
        }
    }
*/
    send_to_char( "Either that is not a valid spell, or you cannot cast it!\n\r", ch );
    return;
}



/* commented out for new mages
        if (!str_prefix(arg1,"Gate"))
        {
            int sn;
            if (is_affected( ch, gsn_arena ))
            {
                send_to_char("I don't THINK so!\n\r",ch);
                return;
            }

            sn = skill_lookup( "portal" );
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            ch->pcdata->learned[sn] = 100;
            sprintf(target, "portal %s", arg2);
            do_function(ch, &do_cast, target);
            ch->pcdata->learned[sn] = 0;
            return;
        }
        if (!str_prefix(arg1,"Summon"))
        {
            int sn;
            sn = skill_lookup( "summon" );
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            ch->pcdata->learned[sn] = 100;
            sprintf(target, "summon %s", arg2);
            do_function(ch, &do_cast, target);
            ch->pcdata->learned[sn] = 0;
            return;
        }

    }
*/

/*  if(ch->mana < 20 && ch->thaumaturgy >= 1)
    {
        send_to_char( "You don't have enough mana!\n\r", ch );
        return;
    }
    if(ch->mana < 10 && ch->clan == 12)
    {
        send_to_char( "You don't have enough mana!\n\r", ch );
        return;
    }

    if(ch->clan == 12)
        ch->mana += 10;
*/
/* commented out for new mages
    if (ch->clan == clan_lookup("mage"))
    {
        if (!str_prefix(arg1,"hands of flame"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_burning_hands( skill_lookup("burning hands"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 10;
            WAIT_STATE( ch, 6 );
            return;
        }

        if (!str_prefix(arg1,"strength of the bear"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_giant_strength( skill_lookup("giant strength"), ch->level, ch, victim,TAR_CHAR_DEFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"hands of ice"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_chill_touch( skill_lookup("chill touch"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 10;
            WAIT_STATE( ch, 6 );
            return;
        }
    }

    if (ch->clan == clan_lookup("mage"))
    {
        if (!str_prefix(arg1,"soaring of the eagle"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_fly( skill_lookup("fly"), ch->level, ch, victim,TAR_CHAR_DEFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }

        if (!str_prefix(arg1,"force bolt"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upon the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_magic_missile( skill_lookup("magic missile"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 10;
            WAIT_STATE( ch, 6 );
            return;
        }

        if (!str_prefix(arg1,"touch of the vampire"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_energy_drain( skill_lookup("energy drain"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 30;
            WAIT_STATE( ch, 6 );
            return;
        }
    }
    if (ch->clan == clan_lookup("mage"))
    {
        if (!str_prefix(arg1,"speed of the leopard"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_haste( skill_lookup("haste"), ch->level, ch, victim,TAR_CHAR_DEFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"fireball"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_fireball( skill_lookup("fireball"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 30;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"dispel magic"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_dispel_magic( skill_lookup("dispel magic"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 50;
            WAIT_STATE( ch, 6 );
            return;
        }
    }
*/
/* commented out for new mages
    if (ch->clan == clan_lookup("mage"))
    {
        if (!str_prefix(arg1,"protection of nature"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_shield( skill_lookup("shield"), ch->level, ch, victim,TAR_CHAR_DEFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"tower of stone"))
        {
            victim = ch;
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_stone_skin( skill_lookup("stone skin"), ch->level, ch, victim,TAR_CHAR_DEFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"venom of the viper"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_acid_blast( skill_lookup("acid blast"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);
            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"lightning fist"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_lightning_fist( skill_lookup("lightning fist"), ch->level, ch, victim,TAR_CHAR_OFFENSIVE);

            ch->mana -= 20;
            WAIT_STATE( ch, 6 );
            return;
        }

        if (!str_prefix(arg1,"Elemental Soul"))
        {
            act("$n calls upon the powers of nature!", ch,NULL,NULL,TO_ROOM);
            act("You call upons the powers of nature!.", ch,NULL,NULL,TO_CHAR);
            spell_elemental( skill_lookup("elemental"), ch->level, ch, ch,TAR_CHAR_SELF);
            ch->mana -= 30;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"Fireshield"))
        {
            act("$n summons an aura of burning flames around them!", ch,NULL,NULL,TO_ROOM);
            act("You summon an aura of burning flames around you!", ch,NULL,NULL,TO_CHAR);
            spell_fireshield( skill_lookup("fireshield"), ch->level, ch, ch,TAR_CHAR_SELF);
            ch->mana -= 50;
            WAIT_STATE( ch, 6 );
            return;
        }
        if (!str_prefix(arg1,"Static Charge"))
        {
            spell_shockshield( skill_lookup("static charge"), ch->level, ch, ch,TAR_CHAR_SELF);
            ch->mana -= 50;
            WAIT_STATE( ch, 6 );
            return;
        }


    }
*/

void do_brew(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *brew;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *vial;
    OBJ_DATA *pot;
    OBJ_INDEX_DATA *ingredient1,*ingredient2,*ingredient3,*ingredient4,*ingredient5;
    char buf[MAX_STRING_LENGTH],arg[MAX_INPUT_LENGTH];
    int count=0;
    bool allfound, vfound, pfound, boost;
    allfound = vfound = pfound = boost = FALSE;
    obj = vial = pot = ch->carrying;
    argument = one_argument(argument,arg);
    int brewsuccess = 0;
    int brewdiff = 3;

    if (ch->csabilities[CSABIL_HEARTHWISDOM] < 2)
    {
        send_to_char("You have not learned enough Hearth Wisdom to brew properly.\n\r",ch);
        return;
    }

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if(obj->pIndexData->vnum == OBJ_VNUM_EMPTY_VIAL && !vfound)
        {
            vial = obj;
            vfound = TRUE;
        }
        if(IS_OBJ_STAT(obj,ITEM_BREW_POT) && !pfound)
        {
            pot = obj;
            pfound = TRUE;
        }
    }
    if (arg[0] == '\0')
    {
        send_to_char("{-------------------------------[{YPotions Learned{x]--------------------------------}\n\r",ch);
        send_to_char("|(Lvl)  {gName           {x:     {gIngredients                                         {x|\n\r",ch);
        send_to_char("{--------------------------------------------------------------------------------}\n\r",ch);
        for ( count; count != MAX_BREW; count++ )
        {
            if (ch->pcdata->cssec_abil[CSABIL_BREWING] >= brew_table[count].level)
            {
                ingredient1 = get_obj_index( brew_table[count].component[0] );
                ingredient2 = get_obj_index( brew_table[count].component[1] );
                ingredient3 = get_obj_index( brew_table[count].component[2] );
                ingredient4 = get_obj_index( brew_table[count].component[3] );
                ingredient5 = get_obj_index( brew_table[count].component[4] );
                sprintf(buf, "  ({R%d{x) %16s : %s%s%s%s\n\r                       %s%s%s%s%s", brew_table[count].level,
                    brew_table[count].name, ingredient1 == 0 ? "" : ingredient1->short_descr, 
                    ingredient2 != 0 ? "," : "", ingredient2 == 0 ? "" : ingredient2->short_descr, 
                    ingredient3 != 0 ? "," : "", ingredient3 == 0 ? "" : ingredient3->short_descr, 
                    ingredient4 != 0 ? "," : "", ingredient4 == 0 ? "" : ingredient4->short_descr, 
                    ingredient5 != 0 ? "," : "", ingredient5 == 0 ? "" : ingredient5->short_descr);
                act( buf, ch, NULL, ch, TO_CHAR );
            }
        }
        send_to_char("{--------------------------------------------------------------------------------}\n\r",ch);
        send_to_char("|  {yWhich potion would you like to brew?{x                                          |\n\r",ch);
        send_to_char("|  Syntax:  {cbrew 'potion name'{x                                                   |\n\r", ch);
        send_to_char("{--------------------------------------------------------------------------------}\n\r",ch);
        return;
    }

    if(!vfound)
    {
        send_to_char( "You do not seem to have a proper container for this brew.\n\r", ch );
        return;
    }
    if(!pfound)
    {
        send_to_char( "You do not seem to have the proper equipment to brew.\n\r", ch );
        return;
    }
    if(skill_lookup(arg) == -1 || !brew_lookup(arg))
    {
        send_to_char( "That is not a valid spell!\n\r", ch );
        return;
    }
    if((brew = create_object(get_obj_index(OBJ_VNUM_BREW_POTION),0)) == NULL)
    {
        send_to_char( "Error! Contact the imms about this missing potion!\n\r", ch );
        return;
    }
    if(brew_table[brew_lookup(arg)].level > ch->pcdata->cssec_abil[CSABIL_BREWING])
    {
        send_to_char( "You have not learned how to brew this potion!\n\r", ch );
        return;
    }

    for ( obj = pot->contains; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        if(obj->pIndexData->vnum == brew_table[brew_lookup(arg)].component[count])
        {
            sprintf(buf, "%s is mixed in the %s as part of the brew.\n\r",obj->short_descr, pot->short_descr);
            act( buf, ch, NULL, ch, TO_ROOM );
            send_to_char(buf,ch);
            extract_obj(obj);
            count++;
            obj_next = pot->contains;
        }

        if(count >= 5 || brew_table[brew_lookup(arg)].component[count] == 0)
        {
            allfound = TRUE;
            break;
        }
    }
    for ( obj = pot->contains; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if(obj->pIndexData->vnum == OBJ_VNUM_GOLDEN_BERRY && !boost)
        {
            sprintf(buf, "%s is put in the %s. It suddenly begins to glow brightly!\n\r",obj->short_descr, pot->short_descr);
            act( buf, ch, NULL, ch, TO_ROOM );
            send_to_char(buf,ch);
            boost = TRUE;
            extract_obj(obj);
            break;
        }
    }

    if(!allfound)
    {
        send_to_char("You do not seem to have the required componets to complete this brew.\n\rAll valid components lost.",ch);
        return;
    }

    if(!IS_IMMORTAL(ch)) WAIT_STATE( ch, skill_table[gsn_brew].beats );
    brewdiff += brew_table[brew_lookup(arg)].diff;
    brewsuccess = godice(get_attribute(ch, INTELLIGENCE + ch->csabilities[CSABIL_HEARTHWISDOM]), brewdiff);

    if(brewsuccess < 0)
    {
        brew->value[0] = ch->level/5 + 1;
        switch (number_range(1, 3))
        {
            default: brew->value[1] = skill_lookup("cause light");
            case 1:
                brew->value[1] = skill_lookup("poison");
                break;
            case 2:
                brew->value[1] = skill_lookup("poison");
                brew->value[2] = skill_lookup("blind");
                break;
            case 3:
                brew->value[1] = skill_lookup("blind");
                break;
        }
        brew->level = ch->level;
        sprintf(buf, "a %s potion",brew_table[brew_lookup(arg)].color);
        brew->short_descr = str_dup(buf);
        sprintf(buf, "brew potion %s %s", brew_table[brew_lookup(arg)].name,brew_table[brew_lookup(arg)].color);
        brew->name = str_dup(buf);
        obj_to_char(brew,ch);
        sprintf(buf, "You carefully mix each component into your brew and then pour the result into an empty vial.\n\rYou now have a %s potion.\n\r",brew_table[brew_lookup(arg)].color);
        send_to_char(buf,ch);
        sprintf(buf, "%s mixes together many components and brews a %s potion.\n\r",ch->name, brew_table[brew_lookup(arg)].color);
        act( buf, ch, NULL, ch, TO_NOTVICT );
        return;
    }

    if(brewsuccess == 0)
    {
        send_to_char("You carefully mix each component into your brew, which promptly fizzes erratically!\n\r",ch);
        sprintf(buf,"A %s potion erodes its new container, wasting the concoction in the process.\n\r",brew_table[brew_lookup(arg)].color);
        act( buf, ch, NULL, ch, TO_ALL );
        extract_obj(brew);
        check_improve(ch,gsn_brew,FALSE,1);
        return;
    }

    check_improve(ch,gsn_brew,TRUE,1);
    if(brewsuccess > 4)
        brew->value[0] = (ch->level + (brewsuccess * 2));
    else
        brew->value[0] = (ch->level - (2 * brew_table[brew_lookup(arg)].plevel) + (brewsuccess));
    brew->value[1] = skill_lookup(arg);
    brew->level = ch->level;
    sprintf(buf, "a %s potion",brew_table[brew_lookup(arg)].color);
    brew->short_descr = str_dup(buf);
    sprintf(buf, "brew potion %s %s", brew_table[brew_lookup(arg)].name,brew_table[brew_lookup(arg)].color);
    brew->name = str_dup(buf);
    obj_to_char(brew,ch);
    extract_obj(vial);
    sprintf(buf, "You carefully mix each component into your brew and then pour the result into an empty vial.\n\rYou now have a %s potion.\n\r",brew_table[brew_lookup(arg)].color);
    send_to_char(buf,ch);
    sprintf(buf, "%s mixes together many components and brews a %s potion.\n\r",ch->name, brew_table[brew_lookup(arg)].color);
    act( buf, ch, NULL, ch, TO_NOTVICT );
    return;
}

void do_study( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *comp;
    char buf[MAX_STRING_LENGTH];
    int count;

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL)
    {
        send_to_char( "Study what?\n\r", ch );
        return;
    }
    if (obj->item_type != ITEM_POTION)
    {
        send_to_char("That is not a potion!\n\r",ch);
        return;
    }
    if (obj->level > ch->level)
    {
        send_to_char("This potion is too powerful for you to study.\n\r",ch);
        return;
    }
    if (ch->mana < 100)
    {
        send_to_char("You have too little energy to study right now.\n\r",ch);
        return;
    }
    if (ch->move < 100)
    {
        send_to_char("Your too tired to put the effort into studying.\n\r",ch);
        return;
    }

    if (get_skill(ch,gsn_study) <= 1)
    {
        send_to_char("You fail to learn anything useful.\n\r",ch);
        return;
    }
    if (get_skill(ch,gsn_study) < number_percent())
    {
        send_to_char("You fail to learn anything useful.\n\r",ch);
        check_improve(ch,gsn_study,FALSE,2);
        WAIT_STATE(ch, 36);
        return;
    }

    check_improve(ch,gsn_study,TRUE,2);
    if(!IS_IMMORTAL(ch)) WAIT_STATE(ch, 36);
    ch->mana -= 100;
    ch->move -= 100;

    sprintf(buf,"You study %s very carefully and learn...\n\r",obj->short_descr);
    send_to_char(buf,ch);
    sprintf(buf,"$n studies %s very carefully.",obj->short_descr);
    act( buf, ch, NULL, ch, TO_NOTVICT );
    if(obj->value[1] == -1 || skill_lookup(skill_table[obj->value[1]].name) == -1)
    {
        send_to_char( "That is not a valid spell! Contact imms at once to fix this!\n\r", ch );
        return;
    }
    if(brew_lookup(skill_table[obj->value[1]].name) == 0)
    {
        send_to_char("That it appears that this potion cannot be brewed.\n\r",ch);
        return;
    }
    sprintf(buf,"That this potion is level %d and can cast the spell %s at level %d.\n\rIt can be brewed with the following componets:\n\r",
        obj->level,skill_table[obj->value[1]].name,obj->value[0]);
    send_to_char(buf,ch);
    for(count = 0; count < 5; count++)
    {
        if(brew_table[brew_lookup(skill_table[obj->value[1]].name)].component[count] == 0)
            break;
            if (get_obj_index( brew_table[brew_lookup(skill_table[obj->value[1]].name)].component[count] ) == NULL)
            { sendch("{RNULL Obj Index for componant of brew. A bug has been filed.{x\n\r", ch);
              bugf( "do_study : Null Obj Index for Componant of potion %s.", obj->short_descr);
            } else {
        comp = create_object(get_obj_index( brew_table[brew_lookup(skill_table[obj->value[1]].name)].component[count] ),0);
        sprintf(buf,"%s\n\r", comp == NULL ? "none" : comp->short_descr);
        send_to_char(buf,ch);
        extract_obj(comp);
    }
    }
    return;
}
void do_skin(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    OBJ_DATA *skin;
    OBJ_DATA *corpse;
    char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

    if((corpse = get_obj_here( ch, NULL, arg )) == NULL)
    {
        send_to_char( "Skin what?\n\r", ch );
        return;
    }

    if(corpse->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char( "You can't skin THAT!\n\r", ch );
        return;
    }

    if(corpse->level == 0)
    {
        send_to_char("This corpse cannot be skinned.\n\r",ch);
        return;
    }

    send_to_char("You start to skin the corpse and...\n\r",ch);

    if (get_skill(ch,gsn_skin) < number_percent())
    {
        if(number_range(1,100) < 50)
        {
            send_to_char("slip and cut your finger.. OOOW IT HURTS!\n\r",ch);
            check_improve(ch,gsn_skin,FALSE,1);
            act( "$n attempts to skin $p but slips and cuts $s finger!.", ch, corpse, ch, TO_NOTVICT );
            damage( ch, ch, corpse->level*2, gsn_skin, DAM_SLASH, TRUE);
            WAIT_STATE(ch, 36);
            return;
        }
        send_to_char("fail horribly, tearing the skin into useless shreds.\n\r",ch);
        check_improve(ch,gsn_skin,FALSE,1);
        act( "$n attempts to skin $p but fails miserably, tearing the skin into useless shreds.", ch, corpse, ch, TO_NOTVICT );
        extract_obj(corpse);
        WAIT_STATE(ch, 36);
        return;
    }

    mob = create_mobile(get_mob_index(corpse->value[0]));
    char_to_room(mob, ch->in_room);

    if((skin = create_object(get_obj_index(OBJ_VNUM_SKIN),0)) == NULL)
    {
        send_to_char("Error! Contact an imm at once to fix this missing item!\n\r",ch);
        return;
    }

    act( "$n skins $p into a nice hide.", ch, corpse, ch, TO_NOTVICT );
    act("You skin $p and make a nice hide out of it.",ch, corpse, ch, TO_CHAR);
    sprintf(buf,"%s %s skin",mob->name, race_table[mob->race].name);
    skin->name = str_dup(buf);
    sprintf(buf,"the skin of %s",mob->short_descr);
    skin->short_descr = str_dup(buf);
    skin->level = corpse->level;
    skin->value[0] = corpse->value[0];
    skin->value[1] = mob->race;
    skin->cost = corpse->level*50;
    extract_obj(corpse);
    extract_char( mob, TRUE );
    obj_to_char(skin,ch);
    return;
}

void do_trophy(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *mob;
    OBJ_DATA *trophy;
    OBJ_DATA *corpse;
    char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];
    int failure;

    argument = one_argument(argument,arg);

    if((corpse = get_obj_here( ch, NULL, arg )) == NULL)
    {
        send_to_char( "Make a trophy out of what?\n\r", ch );
        return;
    }

    if(corpse->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char( "You can't make a trophy out of that!\n\r", ch );
        return;
    }

    if(corpse->level == 0)
    {
        send_to_char("This corpse has been desecrated and will not make a good trophy.\n\r",ch);
        return;
    }

    failure = number_range(1,8);
    if (IS_DEBUGGING(ch))
    {
      sprintf(buf, "{YFailure Check: {C%d{x\n\r", failure);
      send_to_char(buf, ch);
    }

    if(failure == 1)
    {
      act( "You try to claim a trophy from $p, but end up butchering it instead.", ch, corpse, NULL, TO_CHAR );
      act( "$n tries to claim a trophy from $p, but destroys the corpse instead.", ch, corpse, NULL, TO_NOTVICT );
      extract_obj(corpse);
      WAIT_STATE(ch, 36);
      return;
    }

    mob = create_mobile(get_mob_index(corpse->value[0]));
    char_to_room(mob, ch->in_room);

    if((trophy = create_object(get_obj_index(OBJ_VNUM_TROPHY),0)) == NULL)
    {
        send_to_char("Error! Contact an imm at once to fix this missing item!\n\r",ch);
        bug( "do_trophy: OBJ_VNUM_TROPHY [%d] = NULL", OBJ_VNUM_TROPHY);
        return;
    }

    act( "$n kneels down and takes a trophy from $p.", ch, corpse, ch, TO_NOTVICT );
    act("You kneel down next to $p, and take a trophy from the kill.",ch, corpse, ch, TO_CHAR);
    sprintf(buf,"%s %s trophy",mob->name, race_table[mob->race].name);
    trophy->name = str_dup(buf);
    sprintf(buf,"a trophy from %s",mob->short_descr);
    trophy->short_descr = str_dup(buf);
    sprintf(buf,"A grisly trophy has been poached from %s.",mob->short_descr);
    trophy->description = str_dup(buf);
    trophy->level = corpse->level;
    trophy->value[0] = corpse->value[0];
    trophy->value[1] = mob->race;
    trophy->cost = 0;
    extract_obj(corpse);
    extract_char( mob, TRUE );
    obj_to_char( trophy, ch);
    return;
}
/*
void do_craft(CHAR_DATA *ch, char *argument )
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    OBJ_DATA *skin;
    OBJ_DATA *corpse;
    char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH];

    argument = one_argument(argument,arg);

*/
//bind spirit obj option (off/def)
void do_bind(CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *spirit;
    OBJ_DATA *fetish;
    AFFECT_DATA af;
    char buf[MAX_STRING_LENGTH], arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    int success;

    argument = one_argument(argument,arg);
    argument = one_argument(argument,arg2);

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("Only Garou can bind spirits into items.\n\r",ch);
        return;
    }

    if ((spirit = get_char_room( ch, NULL, arg ))== NULL)
    {
        send_to_char( "Which spirit are you trying to bind?\n\r", ch );
        send_to_char("Syntax:  {cbind <spirit> <object>{x\n\r", ch);
        return;
    }

    if(spirit->race != race_lookup("spirit"))
    {
        send_to_char("Only spirits may be convinced to be bound in an object.\n\r",ch);
        return;
    }

    switch(spirit->group)
    {
        case MOB_GROUP_EARTHSPIRIT:
        case MOB_GROUP_FIRESPIRIT:
        case MOB_GROUP_WINDSPIRIT:
        case MOB_GROUP_WATERSPIRIT:
        case MOB_GROUP_ELECTRICSPIRIT:
        case MOB_GROUP_ICESPIRIT:
        case MOB_GROUP_SILVERSPIRIT:
        case MOB_GROUP_GENERALSPIRIT:
            break;

        default:
            sprintf(buf, "Spirit Mob (%d - %s) not set with a 'Spirit' Group type.", spirit->pIndexData->vnum, spirit->short_descr);
            bug(buf, 0);
            append_file( ch, BUG_FILE, buf, TRUE );
            sprintf(buf, "{R**{x Spirit Mob (%s) not set with a 'Spirit' Group type.  Inform the Admin.", spirit->short_descr);
            send_to_char(buf, ch);
            return;
    }

    if ( ( fetish = get_obj_carry( ch, arg2, ch ) ) == NULL)
    {
        send_to_char( "Which carried item are you trying to convince a spirit to be bound to?\n\r", ch );
        send_to_char("Syntax:  {cbind <spirit> <object>{x\n\r", ch);
        return;
    }

    if(IS_SET(fetish->extra_flags, ITEM_IS_ENHANCED))
    {
        send_to_char("The spirit refuses to be bound to the item.\n\r",ch);
        return;
    }

    if (!CAN_WEAR( fetish, ITEM_WEAR_TORSO) && !CAN_WEAR( fetish, ITEM_WIELD))
    {
        send_to_char( "Spirits can only be bound to weapons or body armor at this time.\n\r", ch );
        return;
    }

    if(ch->pcdata->gnosis[TEMP] < 1)
    {
        send_to_char("You do not have the spiritual energy to bind a spirit.\n\r",ch);
        return;
    }

    do_function(ch, &do_sit, "");
    act( "$n sits down and closes $s eyes, and begins to chant in a soft, soothing voice.", ch, NULL, spirit, TO_NOTVICT );
    act( "You begin the binding ritual by sitting down, and directing your chanting towards $N.", ch, NULL, spirit, TO_CHAR );
    act( "You feel a soft chanting tugging at your essence.", ch, NULL, spirit, TO_VICT );
    WAIT_STATE(ch, 72);
    success = godice(get_attribute(ch, WITS) + ch->csabilities[CSABIL_ETIQUETTE], 7);

    if (success < 0)
    {
        act( "$N attacks $n in an enraged fury!", ch, NULL, spirit, TO_NOTVICT );
        act( "$N charges you in a fury, attacking viciously!", ch, NULL, spirit, TO_CHAR );
        act( "$n's chanting offends and enrages you, causing you to attack!", ch, NULL, spirit, TO_VICT );
        multi_hit( spirit, ch, TYPE_UNDEFINED );
        return;
    }

    if (success == 0)
    {
        act( "$N flees the area quickly.", ch, NULL, spirit, TO_NOTVICT );
        act( "$N ignores your chant, and flees swiftly.", ch, NULL, spirit, TO_CHAR );
        act( "Ignoring the chant, you swiftly leave the area.", ch, NULL, spirit, TO_VICT );
        extract_char( spirit, TRUE );
        return;
    }

    ch->pcdata->gnosis[TEMP]--;
    act( "$n's chanting increases until the spirit is bound within $p!", ch, fetish, spirit, TO_NOTVICT );
    act( "You bind the spirit of $N within $p!",ch, fetish, spirit, TO_CHAR);
    act( "Giving in to the pull of the chant, you agree to empower $p.", ch, NULL, spirit, TO_VICT );
    extract_char( spirit, TRUE );

    if (CAN_WEAR(fetish, ITEM_WIELD))
    {
        af.where     = TO_OBJECT;
        af.type      = gsn_fetish;
        af.level     = success;
        af.duration  = 60 + (success*10);
        af.location  = 0;
        af.modifier  = 0;
        switch(spirit->group)
        {
            case MOB_GROUP_EARTHSPIRIT:
            case MOB_GROUP_WINDSPIRIT:
            case MOB_GROUP_WATERSPIRIT:
            case MOB_GROUP_GENERALSPIRIT:
                af.location  = APPLY_DAMROLL;
                af.modifier  = 20 * success;
                break;
            case MOB_GROUP_FIRESPIRIT:
                af.where        =   TO_WEAPON;
                af.bitvector    =   WEAPON_FLAMING;
                break;
            case MOB_GROUP_ELECTRICSPIRIT:
                af.where        =   TO_WEAPON;
                af.bitvector    =   WEAPON_SHOCKING;
                break;
            case MOB_GROUP_ICESPIRIT:
                af.where        =   TO_WEAPON;
                af.bitvector    =   WEAPON_FROST;
                break;
            case MOB_GROUP_SILVERSPIRIT:
                af.bitvector    =   ITEM_SILVER;
                break;
        }
        affect_to_obj(fetish,&af);

        af.bitvector = ITEM_IS_ENHANCED;
        af.modifier = 0;
        af.location = 0;
        affect_to_obj(fetish,&af);

        af.bitvector  = ITEM_MAGIC;
        affect_to_obj(fetish,&af);

        af.bitvector  = ITEM_NODROP;
        affect_to_obj(fetish,&af);
    }
    else
    {
      af.where     = TO_OBJECT;
      af.type      = gsn_fetish;
      af.level     = success;
      af.duration  = 60 + (success*10);
      af.location  = APPLY_AC;
      af.modifier  = -30 * success;
      af.bitvector = ITEM_IS_ENHANCED;
      affect_to_obj(fetish,&af);

      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector  = ITEM_MAGIC;
      affect_to_obj(fetish,&af);

      af.bitvector  = ITEM_NODROP;
      affect_to_obj(fetish,&af);
    }
    return;
}
