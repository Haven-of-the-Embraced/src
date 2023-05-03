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
#include "tables.h"
#include "recycle.h"


/* New bandage code by Sengir */
void do_bandage(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];
    int diceroll;       /*The number rolled on each number_range() instance.*/
    int dicesuccess = 0;        /*Total number of successes on the roll.*/
    int dicepool;       /*Total dice to roll.*/
    int i;
    int duration = 12;      /*Duration of affect varies for botch/fail/success*/


    argument = one_argument( argument, arg );   /*strips line down*/

    if ( arg[0] == '\0')        /*If arg is null, it means you didnt specify a victim*/
        victim = ch;        /*therefore setting victim = the one using bandage*/
    else
        if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL)     /*If victim specified is not in the room*/
        {
            send_to_char( "You don't see that person here.\n\r", ch );      /*let the user know.*/
            return;
        }

    if(IS_NPC(victim))          /*Check to see if victim is NPC, will make it so you can bandage*/
    {               /*NPC's later, possible quest usage.*/
        if(!IS_NPC(ch))
            send_to_char("While the gesture is appreciated, it will not help at this time.\n\r", ch);
        return;
    }

    if ( ( obj = get_carry_vnum( ch, OBJ_VNUM_BANDAGE, WEAR_NONE, TRUE) ) == NULL )   /*Checks inventory for 'bandage' with vnum*/
    {
        if (!IS_NPC(ch))
            send_to_char( "You do not have any proper bandages.\n\r", ch );
        return;
    }


    /*This is a test bandage code...for allowing mobs to use the bandage command on players*/

    WAIT_STATE( ch, skill_table[gsn_bandage].beats );           /*Pauses for a moment*/


    if( is_affected(victim, gsn_bandage) )
    {
        if(IS_NPC(ch))
        {
            act("$n says, 'You have already been bandaged, $N.  I can do nothing more.'",ch,NULL,victim,TO_VICT);
            act("$n says, 'You have already been bandaged, $N.  I can do nothing more.'",ch,NULL,victim,TO_NOTVICT);
        }
        else
            send_to_char("Their wounds have already been treated with a bandage.\n\r", ch);
        return;
    }


    if(IS_NPC(ch))
    {
        /*Mob makes motions of attempting bandage...*/
        act("$n attempts to bandage your wounds...",ch,NULL,victim,TO_VICT);
        act("$n attempts to bandage $N...",ch,NULL,victim,TO_NOTVICT);

        if(number_range(1,10) >= 6)
        {       /*if successful*/
            act("...and manages to place a clean bandage on you.\n\r",ch,NULL,victim,TO_VICT);
            act("...and places a clean bandage on $N.\n\r",ch,NULL,victim,TO_NOTVICT);

            if (victim->race != race_lookup("vampire") && victim->race != race_lookup("methuselah") && victim->race != race_lookup("garou"))
            {       /*only non vamp/meth/garou get healed*/
                victim->hit += ch->level*2; /*lvl x 2 for hp*/
            if (victim->hit > victim->max_hit)
                    victim->hit = victim->max_hit;
            }

            af.where = TO_AFFECTS;      /*I am unsure what this does exactly...*/
            af.type = gsn_bandage;      /*Sets affect as bandage*/
            af.level = 1;           /*Affect lvl....2 for mobs*/
            af.duration = 24;           /*For 24 hours/ticks */
            af.location = APPLY_NONE;       /*None, as opposed to maxhp, int, CS_stat...etc*/
            af.modifier = 0;            /*Modifies none location by 0*/
            af.bitvector = 0;           /*Adds bits none, I want bit regeneration eventually for nongarou/vamp/meth*/
            affect_to_char(victim,&af);
        }

        else
        {
            act("...and fails at applying the bandage.\n\r",ch,NULL,victim,TO_VICT);
            act("...and fails at bandaging $N.\n\r",ch,NULL,victim,TO_VICT);
        }
        extract_obj(obj);
        return;
    }


    if( ch->csabilities[CSABIL_MEDICINE]<1)     /*First thing it checks for, if you have Medicine knowledge.*/
    {
        send_to_char("Without the proper knowledge in Medicine, you might make the wound worse.\n\r", ch);
        return;
    }

    if(ch->move <= (100-(ch->csattributes[STAMINA]*10)))    /*Checks your movement, possible ranges are 90-50*/
    {                   /*Higher CS_Stam, lower movement cost*/
        send_to_char("You are too tired to bandage someone.\n\r", ch);
        return;
    }

    extract_obj(obj);

    act("You attempt to bandage $N...",ch,NULL,victim,TO_CHAR);     /*If all checks pass, go through the*/
    act("$n attempts to bandage your wounds...",ch,NULL,victim,TO_VICT);    /*motions of attempting to bandage*/
    act("$n attempts to bandage $N...",ch,NULL,victim,TO_NOTVICT);      /*before making subsequent checks*/

    dicepool = (ch->csattributes[INTELLIGENCE]+ch->csabilities[CSABIL_MEDICINE]);   /*Calculates dicepool, CS_Int +CS_Med*/
    for (i = 1; i <= dicepool; i++)         /*Loops for dicepool, to make a roll for each 'die'*/
    {
        diceroll = number_range(1, 10);         /*Get random #, 1-10, and save it.*/
        if(diceroll >= 6)                       /*6 or higher, add to successes*/
            dicesuccess += 1;
        if(diceroll == 1)                       /*1 takes away a success*/
            dicesuccess -= 1;
    }

    if (dicesuccess == 0)    /*Checks to see if you gathered no successes*/
    {
        act("and tear the bandage as you wrap the wound!", ch, NULL, victim, TO_CHAR);
        act("and tears the bandage on your wound!", ch, NULL, victim, TO_VICT);
        act("and tears the bandage while wrapping $N's wound!", ch, NULL,victim,TO_NOTVICT);
        return;
    }
    else if (dicesuccess < 0)               /*Checks to see if total successes are negative*/
    {                   /*meaning rolled more 1's than successes*/
        act("and only succeed in making the wound worse!",ch,NULL,victim,TO_CHAR);      /*Botching*/
        act("and only succeeds in making the wound worse!",ch,NULL,victim,TO_VICT);
        act("and only succeeds in making the wound worse!",ch,NULL,victim,TO_NOTVICT);

        if (victim->hit - (60 - (10*ch->csabilities[CSABIL_MEDICINE]) ) <= 0)
        {               /*If botch would kill victim, instead sets to hp 1*/
            victim->hit = 1;
            return;
        }

        victim->hit -= 60-(10*ch->csabilities[CSABIL_MEDICINE]);
                /*^^^---Damage is 60 minus10*Medicine rating*/
        return;
    }
    else
    {
        if(victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
        {               /*Check for vamp/meth, success bandages but no affect to undead flesh/biological systems*/
            act("and place a clean bandage upon their flesh.",ch,NULL,victim,TO_CHAR);
            act("and succeeds in cleaning your wound, but your undead flesh doesn't heal naturally anymore.",ch,NULL,victim,TO_VICT);
            act("and patches up the wounds with a bandage.",ch,NULL,victim,TO_NOTVICT);
        }
        /*else if(victim->race == race_lookup("garou"))
        {
            act("and manage to place a clean bandage just as the wounds begin to heal.",ch,NULL,victim,TO_CHAR);
            act("and manages to place the bandage just as your accelerated healing kicks in, rendering the bandage pointless.",ch,NULL,victim,TO_VICT);
            act("and manages to place a bandage on $N.",ch,NULL,victim,TO_NOTVICT);
        }*/
        else
        {               /*Bandages victim if not vamp/meth*/
            act("and manage to staunch the bleeding from the wound.",ch,NULL,victim,TO_CHAR);
            act("and manages to stop the blood flowing from your wounds.",ch,NULL,victim,TO_VICT);
            act("and manages to stop the bleeding.",ch,NULL,victim,TO_NOTVICT);

            affect_strip(victim, gsn_bleeding);

            if (victim->race != race_lookup("garou"))
                victim->hit += dicesuccess*ch->level+7;
            else
                victim->hit += (dicesuccess*ch->level/2)+10;

            if (ch->csabilities[CSABIL_MEDICINE] >= 4)   /*If Medicine 4 or higher, heals a little agg damage*/
            {
                victim->agg_dam -= dicesuccess;
                if (victim->agg_dam < 0)
                    victim->agg_dam=0;
            }

            if (ch->csabilities[CSABIL_MEDICINE] >= 3)   /*Bonus heal for higher medicine*/
                victim->hit += dicesuccess*ch->level/2;

            if (victim->hit > victim->max_hit)
                victim->hit = victim->max_hit; /*Max HP check */
        }
    }
    ch->move -= (90-ch->csattributes[STAMINA]*10);  /*Movement cost = 100 minus CS_Stam*10, range*/
                                /*is from 90 - 50, higher CS_Stam, lower cost*/

    /*All this following has to do with what you see in the affects on your char*/
    af.where = TO_AFFECTS;      /*I am unsure what this does exactly...*/
    af.type = gsn_bandage;      /*Sets affect as bandage*/
    af.level = ch->csabilities[CSABIL_MEDICINE];        /*Affect level = Medicine lvl*/
    af.duration = duration;
    af.location = APPLY_NONE;       /*None, as opposed to maxhp, int, CS_stat...etc*/
    af.modifier = 0;            /*Modifies none location by 0*/
    af.bitvector = 0;           /*Adds bits none, I want bit regeneration eventually for nongarou/vamp/meth*/
    affect_to_char(victim,&af);

    gain_exp( ch, 6*dicesuccess );

    return;
}

void do_chant(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if (get_skill(ch,gsn_chant) == 0)
    {
        send_to_char( "You are not holy enough!\n\r", ch );
        return;
    }
    if ( is_affected( ch, gsn_chant ))
    {
        send_to_char( "You are already chanting to your God!\n\r", ch );
        return;
    }
    if(ch->position > POS_SITTING)
    {
        send_to_char( "You cannot do this standing!\n\r", ch );
        return;
    }
    WAIT_STATE( ch, skill_table[gsn_chant].beats );
    if( get_skill(ch,gsn_chant) < number_percent())
    {
        send_to_char( "You fail to get in touch with your God.\n\r", ch );
        check_improve(ch,gsn_chant,FALSE,2);
        return;
    }

    act( "$n sits down and starts to chant to $s God.",  ch, NULL, NULL, TO_ROOM );
    send_to_char( "You start to chant to your God.\n\r", ch );
    check_improve(ch,gsn_chant,TRUE,2);

    af.where    = TO_AFFECTS;
    af.type     = gsn_chant;
    af.level    = ch->level;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector    = 0;
    affect_to_char( ch, &af );
return;
}

void do_classify(CHAR_DATA *ch, char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int success;
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch)) return;

  if (get_ability(ch, CSABIL_ANIMAL_KEN) < 1)
  {
    send_to_char("You have not studied the animal kingdom enough to learn any helpful information.\n\r", ch);
    return;
  }

  if (ch->move < ch->level / 2 + 15)
  {
      send_to_char( "You are too tired to figure out a classification for your target.\n\r", ch );
      return;
  }

  if (is_affected(ch, gsn_forget))
  {
    send_to_char( "Your memory seems fuzzy and you cannot recall any information.\n\r", ch );
    return;
  }

  if ( arg1[0] == '\0')
  {
      send_to_char( "Determine classification for whom?\n\r", ch );
      return;
  }

  if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
  {
      send_to_char( "Determine classification for whom?\n\r", ch );
      return;
  }

  success = godice(get_attribute(ch, INTELLIGENCE) + ch->csabilities[CSABIL_ANIMAL_KEN], 7);
  WAIT_STATE(ch, 16);
  ch->move -= ch->level / 2 + 15;

  if (success < 0)
  {
    send_to_char("You try to focus, but cannot seem to recall any specific information.\n\r", ch);
    WAIT_STATE(ch, 16);
    return;
  }

  if (success == 0)
  {
    send_to_char("You fail to recall any pertinent information about this species.\n\r", ch);
    return;
  }

  if (!is_natural(victim))
  {
    act("$N does not seem to be a natural, non-humanoid member of the animal kingdom.", ch, NULL, victim, TO_CHAR);
    return;
  }

    send_to_char("+========================= ANIMAL CLASSIFICATION ========================+\n\r", ch);
  sprintf(buf, "| %s is a %s-sized, %s %s.\n\r",
  capitalize(victim->short_descr),
  size_table[victim->size].name,
  victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
  race_table[victim->race].name);
  send_to_char(buf,ch);
  sprintf( buf, "| %s is level %d, %d years old, and does %s-type damage.\n\r",
    victim->sex == 0 ? "It" : victim->sex == 1 ? "He" : "She",
    victim->level,   get_age(victim),
    attack_table[victim->dam_type].noun );
  send_to_char(buf,ch);
    sprintf(buf, "| Commonly considered a creature with %s Intelligence.\n\r", IS_SET(victim->form, FORM_SENTIENT) ? "a {Chigh{x" : "{clittle{x or {Dno{x");
    send_to_char(buf, ch);

  if(success > 1)
  {
    send_to_char("+============================ PHYSICAL TRAITS ===========================+\n\r", ch);
    sprintf(buf, "%s", IS_SET(victim->form, FORM_POISON) ? "|    {R*****{x Use extreme caution, known to be {gp{Do{gi{Ds{go{Dn{go{Du{gs{x! {R*****{x\n\r" : "");
    send_to_char(buf, ch);
    sprintf(buf, "| %s\n\r", IS_SET(victim->form, FORM_EDIBLE) ? "    {YSome{x of this creature's parts are considered edible." : "          {YNone{x of this creature's parts are edible.");
    send_to_char(buf, ch);
    send_to_char("+---------------------------- Creature Parts ----------------------------+\n\r", ch);
    sprintf(buf, "| %s\n\r", part_bit_name(victim->parts));
    send_to_char(buf,ch);
  }

  if (success > 2)
  {
    send_to_char("+------------------------------------------------------------------------+\n\r", ch);
    sprintf(buf, "| Immune to    : %s\n\r", imm_bit_name(victim->imm_flags));
    send_to_char(buf,ch);
    sprintf(buf, "| Resistant to : %s\n\r", imm_bit_name(victim->res_flags));
    send_to_char(buf,ch);
    sprintf(buf, "| Vulnerable to: %s\n\r", imm_bit_name(victim->vuln_flags));
    send_to_char(buf,ch);
  }

  if(success > 3)
  {
    sprintf(buf, "| Abilities    : %s\n\r",affect_bit_name(victim->affected_by));
    send_to_char(buf,ch);
    send_to_char("+========================== BEHAVIORAL TRAITS ===========================+\n\r", ch);
    sprintf(buf, "| Tactics      : %s\n\r",off_bit_name(victim->off_flags));
    send_to_char(buf,ch);
  }

  send_to_char("+========================================================================+\n\r", ch);
  gain_exp(ch, success*2);
  return;
}

void do_soothe(CHAR_DATA *ch, char *argument)
{
  char arg1 [MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  AFFECT_DATA af;
  int success;
  int difficulty = 7;
  argument = one_argument( argument, arg1 );

  if (IS_NPC(ch)) return;

  if ( arg1[0] == '\0')
  {
      send_to_char( "Soothe which animal?\n\r", ch );
      return;
  }

  if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL )
  {
      send_to_char( "Soothe which animal?\n\r", ch );
      return;
  }

  if (get_ability(ch, CSABIL_ANIMAL_KEN) < 2)
  {
    send_to_char("You have not studied the animal kingdom enough to try and soothe this beast.\n\r", ch);
    return;
  }

  if (!is_natural(victim))
  {
    act("$N does not seem to be a natural, non-humanoid member of the animal kingdom.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (!higher_beast(victim))
  {
    send_to_char("This animal isn't evolved enough to understand your attempt.\n\r", ch);
  }

  if (ch->move < ch->level / 10)
  {
      send_to_char( "You are too tired to try and soothe this animal.\n\r", ch );
      return;
  }

  if (is_affected(ch, gsn_forget))
  {
    send_to_char( "Your memory seems fuzzy and you cannot focus enough to soothe your target.\n\r", ch );
    return;
  }

  if ((victim->level > ch->level + 20) || IS_SET(victim->act2, ACT2_ULTRA_MOB))
  {
    send_to_char( "That creature is too powerful to soothe.\n\r", ch);
    return;
  }

  if (victim->level < ch->level)
    difficulty--;
  if (victim->level > ch->level + 10)
    difficulty++;

  success = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_ANIMAL_KEN], difficulty);
  WAIT_STATE(ch, 8);
  ch->move -= ch->level / 10;

  if (success < 0)
  {
    act("Your attempts to calm $N only seem to enrage $M.", ch, NULL, victim, TO_CHAR);
    act("$n tries to soothe $N, but only succeeds in enraging $M further.", ch, NULL, victim, TO_NOTVICT);
    act("$N seems like a much bigger threat, causing you to feel much more enraged.", ch, NULL, victim, TO_VICT);
    WAIT_STATE(ch, 8);

      af.where    = TO_AFFECTS;
      af.type     = gsn_berserk;
      af.level    = 1;
      af.duration = 2;
      af.modifier = 50;
      af.location = APPLY_HITROLL;
      af.bitvector    = 0;
      affect_to_char( ch, &af );

      af.location = APPLY_DAMROLL;
      affect_to_char( ch, &af );

    return;
  }

  if (success == 0)
  {
    send_to_char("Your attempt at soothing the beast seems to go unnoticed.\n\r", ch);
    check_improve(ch,gsn_soothe,FALSE,4);
    return;
  }

  act("Using slow movements and a calm voice, you soothe $N's savagery.", ch, NULL, victim, TO_CHAR);
  act("$n speaks softly to $N, and $E calms down greatly.", ch, NULL, victim, TO_NOTVICT);
  act("$N's actions and words are soothing, and $e seems to be friendly.", ch, NULL, victim, TO_VICT);

  af.where    = TO_AFFECTS;
  af.type     = gsn_soothe;
  af.level    = success;
  af.duration = 10 + success;
  af.modifier = 0;
  af.location = 0;
  af.bitvector    = AFF_CALM;
  affect_to_char( victim, &af );

  if ( victim->fighting != NULL )
      stop_fighting( victim, TRUE );
  if (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE))
      REMOVE_BIT(victim->act,ACT_AGGRESSIVE);

  gain_exp(ch, success * 4);
  check_improve(ch,gsn_soothe,TRUE,7);

  return;
}

void do_glower(CHAR_DATA *ch, char *argument)
{
  AFFECT_DATA af;
  int rbf_success = 0, rbf_diff = 7, rbf_duration = 0;

  if (IS_NPC(ch))
    return;

    if (is_affected(ch, gsn_glower))
    {
      if (get_affect_level(ch, gsn_glower) == 0)
        send_to_char("You don't have the confidence to overcome your last comical attempt yet.\n\r", ch);
      else
      {
        send_to_char("You relax your hardened facial expression.\n\r", ch);
        affect_strip(ch, gsn_glower);
      }
      return;
    }

  if (ch->cswillpower < 1)
  {
    send_to_char("You cannot muster the willpower to glower at your opponents.\n\r", ch);
    return;
  }

  ch->cswillpower--;

  if (is_affected(ch, gsn_awe) || is_affected(ch, gsn_majesty))
    rbf_diff--;
  if (is_affected(ch, gsn_berserk) || is_affected(ch, gsn_garou_frenzy)
  || is_affected(ch, gsn_thaumaturgy_frenzy) || is_affected(ch, gsn_vamp_frenzy)
  || is_affected(ch, gsn_gleam) || is_affected(ch, gsn_rage))
    rbf_diff--;
  if (is_affected(ch, gsn_botched_presence))
    rbf_diff++;
  rbf_success = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_INTIMIDATION], rbf_diff);
  rbf_success += stealth_int_shadowplay(ch, rbf_diff);
  rbf_duration = godice(get_attribute(ch, STAMINA) + ch->csabilities[CSABIL_INTIMIDATION], rbf_diff);
  if (rbf_duration < 1)
    rbf_duration = 1;

  WAIT_STATE(ch, PULSE_VIOLENCE * 2);

  if (rbf_success < 1)
  {
    act("You attempt to put on your meanest face, but end up looking comical instead.", ch, NULL, NULL, TO_CHAR);
    act("$n twists $s face up comically.", ch, NULL, NULL, TO_NOTVICT);

    af.where     = TO_AFFECTS;
    af.type      = gsn_glower;
    af.level     = 0;
    af.location  = 0;
    af.modifier  = 0;
    af.duration  = 2;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    return;
  }

  if (rbf_success == 0)
  {
    act("You attempt to glare about the room, but feel that others are unimpressed.", ch, NULL, NULL, TO_CHAR);
    act("$n tries to look tough, but you feel unimpressed.", ch, NULL, NULL, TO_NOTVICT);
    WAIT_STATE(ch, PULSE_VIOLENCE);
    check_improve(ch,gsn_glower,FALSE,5);
    return;
  }

  act("You set your facial features into the fiercest glower you can muster.", ch, NULL, NULL, TO_CHAR);
  act("$n glowers about the room, looking like $e is not to be trifled with.", ch, NULL, NULL, TO_NOTVICT);
  check_improve(ch,gsn_glower,TRUE,8);

  af.where     = TO_AFFECTS;
	af.type      = gsn_glower;
	af.level     = ch->level + rbf_success;
	af.location  = 0;
	af.modifier  = 0;
	af.duration  = rbf_duration*7;
	af.bitvector = 0;
	affect_to_char( ch, &af );

  gain_exp(ch, (rbf_success + rbf_duration) * 5);
  return;
}

void do_vigor(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int vigorsuccess = 0;
    int vigordiff = 6;

    if (get_ability(ch, CSABIL_ATHLETICS) < 2)
    {
      send_to_char("You are not athletic enough to have trained your body further.\n\r", ch);
      return;
    }

    if (is_affected(ch, gsn_vigor) && get_affect_level(ch, gsn_vigor) == 0)
    {
      send_to_char("Try as you might, you still cannot remember your training yet.\n\r", ch);
      return;
    }

    if (is_affected(ch, gsn_forget))
    {
      send_to_char("The details of your speed and stamina training seem foggy and distant.\n\r", ch);
      return;
    }

    if(ch->move < ch->level / 5)
    {
        send_to_char("You do not have enough movement.\n\r", ch);
        return;
    }

    if(ch->cswillpower < 1)
    {
      send_to_char("You do not have the willpower to focus on your speed and stamina training.\n\r", ch);
      return;
    }

    if(IS_AFFECTED(ch,AFF_HASTE))
    {
        send_to_char("You are already moving as quickly as possible.\n\r", ch);
        return;
    }

    ch->move -= ch->level / 5;
    ch->cswillpower--;

    WAIT_STATE( ch, skill_table[gsn_vigor].beats );

    if (is_affected(ch, gsn_berserk) || is_affected(ch, gsn_garou_frenzy) || is_affected(ch, gsn_vamp_frenzy)
    || is_affected(ch, gsn_thaumaturgy_frenzy) || is_affected(ch, gsn_rage))
      vigordiff++;

    if (IS_AFFECTED(ch, AFF_CALM) || is_affected(ch, gsn_gift_resistpain))
      vigordiff--;

    vigorsuccess = godice(get_attribute(ch, STAMINA) + ch->csabilities[CSABIL_ATHLETICS], vigordiff);

    if (vigorsuccess < 0)
    {
      act("You spend an extended amount of time trying to recall your speed and stamina training, but to no avail.", ch, NULL, NULL, TO_CHAR);
      af.where     = TO_AFFECTS;
      af.type      = gsn_vigor;
      af.level     = 0;
      af.duration  = 1;
      af.modifier  = 0;
      af.location  = 0;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      return;
    }

    if (vigorsuccess == 0)
    {
      act("You fail to recall the proper methods for maximizing your speed and stamina.", ch, NULL, NULL, TO_CHAR);
      check_improve(ch,gsn_vigor,FALSE,6);
      return;
    }

    act("Centering your mind, you recall your vigorous training of your body's natural abilities.",ch,NULL,ch,TO_CHAR);
    act("$n takes a few deep breaths, and seems to begin moving fluidly and methodically.",ch,NULL,ch,TO_NOTVICT);

    af.where     = TO_AFFECTS;
    af.type      = gsn_vigor;
    af.level     = vigorsuccess;
    af.duration  = 5 * vigorsuccess + 15;
    af.modifier  = (get_attribute(ch, STAMINA) * 100);
    af.location  = APPLY_MOVE;
    af.bitvector = AFF_HASTE;
    affect_to_char( ch, &af );
    check_improve(ch,gsn_vigor,TRUE,6);

    if (vigorsuccess > 3)
    {
      act("All of the knowledge flows from your memory to your movements, flawlessly mimicking your training.", ch, NULL, NULL, TO_CHAR);
      af.where     = TO_AFFECTS;
      af.type      = gsn_vigor;
      af.level     = vigorsuccess;
      af.duration  = 5 * vigorsuccess + 10;
      af.modifier  = 20 * vigorsuccess;
      af.location  = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char( ch, &af );
    }

    gain_exp(ch, vigorsuccess * 4 + 5);
    return;
}

void do_waylay(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    /* int chance=10; */

    argument = one_argument( argument, arg );

    if(get_skill(ch,gsn_waylay) < 1)
    {
        send_to_char("You aren't sneaky enough!\n\r",ch);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char("You are already fighting!\n\r",ch);
        return;
    }

    if(ch->move < ch->level)
    {
        send_to_char("You are too tired!\n\r", ch);
        return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL)
    {
        send_to_char( "Waylay whom?\n\r", ch );
        return;
    }
/*  chance += get_skill(ch,gsn_waylay)/4;

    if(!can_see(victim,ch)) chance += 50;
    if(victim->position == POS_SLEEPING) chance += 50;
    if(IS_AFFECTED(ch,AFF_SNEAK))   chance += 20;

    if(ch->level > victim->level)   chance += 10;


    if(number_range(1,100) > chance)
    {
        send_to_char("You fail to Waylay them undetected!\n\r",ch );
        if(!IS_NPC(victim)) send_to_char("Someone has tried to waylay you!\n\r",victim);
            else multi_hit( victim, ch, TYPE_UNDEFINED );
        WAIT_STATE( ch, skill_table[gsn_waylay].beats*2 );
        check_improve(ch,gsn_waylay,FALSE,1);
        return;
    }
*/
    act("$n jumps out of hiding and waylays you, catching you off guard!",ch,NULL,victim,TO_VICT);
    act("You jump out of hiding and waylay $N!",ch,NULL,victim,TO_CHAR);
    act("$n jumps out of hiding and waylays $N!",ch,NULL,victim,TO_NOTVICT);
    WAIT_STATE( victim, skill_table[gsn_waylay].beats*2);
    WAIT_STATE( ch, skill_table[gsn_waylay].beats);
    multi_hit( ch, victim, TYPE_UNDEFINED );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    ch->move -= ch->level;
    check_improve(ch,gsn_waylay,TRUE,2);
    return;
}

void do_fortress(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA *af;


    if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL )
    {
        send_to_char( "Fortify what?\n\r", ch );
        return;
    }
    if (!CAN_WEAR( obj, ITEM_WEAR_SHIELD))
    {
        send_to_char( "That is not a shield!\n\r", ch );
        return;
    }
    if(ch->mana < 100)
    {
        send_to_char( "You don't have enough mana.\n\r", ch );
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_fortress].beats );
    if( get_skill(ch,gsn_fortress) < number_percent())
    {
        send_to_char( "You fail to fortify the object.\n\r", ch );
        return;
        check_improve(ch,gsn_fortress,FALSE,4);
    }

    ch->mana -= 100;
    af = new_affect();
    af->where     = TO_OBJECT;
    af->type      = gsn_fortress;
    af->level     = ch->level;
    af->duration  = ch->level/4;
    af->location  = APPLY_AC;
    af->modifier  = -ch->level;
    af->bitvector = 0;
    obj->affected   = af;
    act("You fortify $p's protection.",ch,obj,NULL,TO_CHAR);
    act("$n fortifies $p's protection.",ch,obj,NULL,TO_NOTVICT);
    check_improve(ch,gsn_fortress,TRUE,4);
    return;
}
