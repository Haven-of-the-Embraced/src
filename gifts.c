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
/*  Adding a Gift:
 * In order to add a gift the following must be done.
 * Write the gift spell_fun and put it in this file. // DONE
 * Declare the gift GSN under this notice. // DONE
 * Declare_spell_fun(spell_giftname) in magic.h // DONE
 * declare extern sh_int gsn_gift_giftname in garou.h  // DONE
 * Add the gift to the skill table. / DONE
 * Add the gift to the gift table. / DONE
 *
 *
 * Go to the gift spell_fun below and write the function.
 *
 * Change the level of the gift from level0 to level1
 */
 char *gift_target_name;
char *trueargs;


/*======Garou Gifts======*/
/*Here I will list garou gifts, organized by Breed, Auspice, and Tribe, and then by Rank.
I will include: Any point costs, Any rolls, The spirit that teaches it, Ideas for quest to learn it,
and ideas for affects.
*/
//
//Breeds
//Homid
//Rank 1
//“Master of Fire”
//Ancestor or Fire Spirit
//Roll: None.
//The Garou communes with fire spirits and they agree not to consume his flesh for the duration of the scene. (Resistance to fire, 5-10 ticks? Maybe use Gnosis as the default duration for gifts.)
//Cost: 1 gnosis
//
void spell_gift_masteroffire( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if (ch->pcdata->gnosis[TEMP] < 1)
    {
        sendch("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
        }

     if (is_affected(ch, gsn_gift_masteroffire))
     {
        sendch("The Elemental spirits of Fire have already agreed not to consume your flesh.\n\r", ch);
        return;
        }

	ch->pcdata->gnosis[TEMP]--;

    af.where        = TO_IMMUNE;
    af.type         = gsn_gift_masteroffire;
    af.level        = ch->pcdata->rank;
    af.duration     = ch->pcdata->gnosis[PERM]*2;
    af.modifier     = 0;
    af.location     = 0;
    af.bitvector    = IMM_FIRE;
    affect_to_char(ch, &af);
    sendch("You pay homage to the Elemental spirits of Fire for protection from their flames.\n\r", ch);
    return;

    }
//
//“Persuasion”
//Ancestor spirit
//Roll: Charisma + Subterfuge (diff 7)
//Allows the user to be more persuasive when dealing with others. (+1 charisma, +1 manipulation if successful. for 5-10 ticks?)
//Cost: None.
void spell_gift_persuasion( int sn, int level, CHAR_DATA *ch, void *vo, int target){

	AFFECT_DATA af;
	int success;
	if (is_affected(ch, gsn_gift_persuasion))
	{
		sendch("You have already mystically enhanced your credibility.\n\r", ch);
		return;
	}
	success = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_SUBTERFUGE), 7);

	if (success < 1)
	{
		sendch("You fail to enhance your persuasive prowess.\n\r", ch);
		return;
	}

	af.where = TO_AFFECTS;
	af.type  = gsn_gift_persuasion;
	af.level = ch->pcdata->rank;
	af.duration = ch->pcdata->gnosis[PERM];
	af.bitvector = 0;
	af.modifier = 1;

	af.location = APPLY_CS_CHA;
	affect_to_char(ch, &af);

	af.location = APPLY_CS_MAN;
	affect_to_char(ch, &af);
	return;

}


//“Smell of Man”
//Ancestor Spirit
//Roll: None.
//Nature knows that where humans walk, death follows. This gift allows a garou to enhance the human scent around him, causing the lesser animals around her to grow uneasy. (Majesty-like affect that only works on animal mobs.  They don’t aggro on the garou, and will flee if attacked. Drops appearance and charisma by 1 due to bad BO.)
//Cost: None.
//
//
//
//
//
//Rank 2
//“Speech of The World”
//Owl spirit
//Roll: Intelligence + Linguinstics (diff 7)
//Those who wander can find a new dialect just over the next ridge, and it seems every hamlet has it’s own variations of the local tongue. This gift allows the garou to speak any human language that she encounters. (Would involve a language code. May need to find another gift for this one,)
//Cost: None.
void spell_gift_speechoftheworld( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_speechoftheworld))
  {
    sendch("You are already mimicking dialects.\n\r", ch);
    return;
  }

  if (ch->move <= ch->level)
  {
    sendch("You are too tired to call the spirits.\n\r", ch);
    return;
  }

  ch->move -= ch->level;
  success = godice(get_attribute(ch, INTELLIGENCE) + get_ability(ch, CSABIL_LINGUISTICS), 7);

  if (success < 0)
  {
    sendch("The spirits of dialects and speech shun your request.\n\r", ch);
    WAIT_STATE(ch, 9);
    return;
  }

  if (success == 0)
  {
    sendch("The spirits of dialects and speech fail to respond.\n\r", ch);
    return;
  }

sendch("The spirits of dialects and speech lend you extra comprehension of spoken languages.\n\r", ch);

  af.where = TO_AFFECTS;
  af.type  = gsn_gift_speechoftheworld;
  af.level = success;
  af.duration = success * 2 + 5;
  af.bitvector = 0;
  af.modifier = 0;
  af.location = 0;
  affect_to_char(ch, &af);
  return;
}

//
//“Staredown”
//Ram or Snake spirit
//Roll: Charisma + Intimidation (Diff 5+ targets [rank] or [courage/2 + 1])
//A werewolf’s stare can strike fear into the hearts of mortals and animals, causing them to flee for their lives. (If garou succeeds on the roll against another garou, garou will be frozen with terror and cannot attack, anyone else will flee the room)
//Cost: None.
void spell_gift_staredown( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
    int staresuccess = 0;
    int starediff = 5;
    char buf[MSL];

    if (victim == NULL)
    {
        send_to_char( "Terrorize whom?\n\r", ch );
        return;
    }

    if (victim == ch)
    {
      send_to_char("You cannot stare yourself into submission, choose someone else.\n\r", ch);
      return;
    }

    if (IS_NPC(victim))
    {
      starediff += victim->level / 30;
      if (IS_DEBUGGING(ch))
      {
        sprintf(buf, "{MTarget: Mob {R(Difficulty: %d)\n\r{x", starediff);
        send_to_char(buf, ch);
      }
    }
    else
    {
      if(victim->race == race_lookup("garou"))
      {
        starediff += victim->pcdata->rank;
        if (IS_DEBUGGING(ch))
        {
          sprintf(buf, "{MTarget: PC Garou {R(Difficulty: %d)\n\r{x", starediff);
          send_to_char(buf, ch);
        }
      }
      else
      {
        starediff += 1 + (victim->pcdata->csvirtues[CSVIRT_COURAGE] / 2);
        if (IS_DEBUGGING(ch))
        {
          sprintf(buf, "{MTarget: PC Non-Garou {R(Difficulty: %d)\n\r{x", starediff);
          send_to_char(buf, ch);
        }
      }
    }

    staresuccess = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_INTIMIDATION], starediff );
    if (IS_SET(victim->res_flags, RES_CHARM))
    {
      staresuccess--;
      if (IS_DEBUGGING(ch))
        send_to_char("{GTarget: Resist charm!  -1 success{x\n\r", ch);
    }

    if(IS_DEBUGGING(ch))
    {
      sprintf(buf, "{YSuccesses Rolled: %d\n\r{x", staresuccess);
      send_to_char(buf, ch);
    }

    WAIT_STATE(ch,8);

    act( "$n stares fiercefully into the eyes of $N.",  ch, NULL, victim, TO_NOTVICT );
    act( "You stand your ground, and stare fiercely into the eyes of $N.",  ch, NULL, victim, TO_CHAR );
    act( "$n locks eyes with you and begins to stare you down.",  ch, NULL, victim, TO_VICT );

    if (staresuccess < 0)
    {
      act( "With a quick movement, $n turns and flees the scene.",  ch, NULL, victim, TO_NOTVICT );
      act( "$N returns your gaze with a fierceness you were not prepared for, and turn and flee!",  ch, NULL, victim, TO_CHAR );
      act( "You return $n's gaze with even more ferocity, watching $e flee in terror.",  ch, NULL, victim, TO_VICT );
      WAIT_STATE(ch,4);
      do_function(ch, &do_flee, "auto" );
      return;
    }

    if (staresuccess == 0)
    {
      act( "$N doesn't seem to be fazed by the staring contest.",  ch, NULL, victim, TO_NOTVICT );
      act( "You and $N continue to lock gazes, neither backing down.",  ch, NULL, victim, TO_CHAR );
      act( "You and $n continue to have your gazes locked, neither backing down.",  ch, NULL, victim, TO_VICT );
      WAIT_STATE(ch, 6);
      return;
    }

    if (IS_SET(victim->imm_flags, IMM_CHARM) || (ch->level + 10 < victim->level))
    {
      act( "$N doesn't seem to back down, and in fact stares back with even more resolve.",  ch, NULL, victim, TO_NOTVICT );
      act( "Staring back at you, $N's eyes show a force of resolve seldom seen.",  ch, NULL, victim, TO_CHAR );
      act( "With ease, you stare directly back into $n's eyes with fierce resolve.",  ch, NULL, victim, TO_VICT );
      WAIT_STATE(ch,4);
      if (IS_DEBUGGING(ch))
        send_to_char("{GTarget Immune to Charm or Over 10 levels higher.{x\n\r", ch);
      return;
    }

    gain_exp(ch, staresuccess);

    if(victim->race == race_lookup("garou") || IS_SET(victim->act2, ACT2_ULTRA_MOB))
    {
      act( "$N pauses for a moment, frozen in fear.",  ch, NULL, victim, TO_NOTVICT );
      act( "$N freezes with fear under your fierce gaze.",  ch, NULL, victim, TO_CHAR );
      act( "$n's gaze fills you with terror as you freeze momentarily with fear.",  ch, NULL, victim, TO_VICT );
      victim->stopped += staresuccess * 2;
      return;
    }

    act( "$N buckles under the gaze, and flees in terror.",  ch, NULL, victim, TO_NOTVICT );
    act( "$N buckles under your fierce gaze, fleeing the scene in terror.",  ch, NULL, victim, TO_CHAR );
    act( "$n's gaze is too much to handle, and you flee in terror!",  ch, NULL, victim, TO_VICT );
    do_function(victim, &do_flee, "auto" );
    if (ch->in_room == victim->in_room)
    {
      act( "In such a terrorized state, $N was unable to escape and instead attacks $n in a blind panic!", ch, NULL, victim, TO_NOTVICT);
      act( "$N seems to panic, unable to find an escape route, then turns and attacks you in a blind panic!", ch, NULL, victim, TO_CHAR);
      act( "Failing to find a suitable retreat, you attack $n in panic!", ch, NULL, victim, TO_VICT);
    }
    return;
}
//
//Rank 3
//“Calm the Savage Beast”
//unicorn spirit
//roll: Manipulation + Primal Urge (diff 8)
//This gift allows the werewolf to ‘lend’ a frenzying garou the will to loose the hold that her Rage has on her. (If successful, stop combat and remove aggressive)
//Cost: 1 Willpower
void spell_gift_calmthesavagebeast( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int success;

  if (IS_NPC(ch)) return;

  if ( victim == NULL )
  {
    send_to_char("Whom do you want to lend your strength of will to?\n\r", ch );
    return;
  }

  if (victim == ch)
  {
    send_to_char("You cannot calm your own beastial fury.\n\r", ch);
    return;
  }

  if(!IS_NPC(victim))
  {
    send_to_char("The spirits of Unicorn frown upon using this against other players.\n\r",ch);
    return;
  }

  if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
  {
    send_to_char("Unicorn wishes you to continue to be able to purchase supplies.\n\r",ch);
    return;
  }

  if ( IS_AFFECTED(victim, AFF_CALM)) 
  {
    send_to_char("They are already pretty calm!\n\r", ch);
    return;
  }

  if ( IS_IMMORTAL(victim))
  {
    send_to_char( "Yeah, right.\n\r", ch );
    return;
  }

  if ( ch->cswillpower < 1 )
  {
    send_to_char( "You don't the strength of will to activate this Gift.\n\r", ch );
    return;
  }

  act("You petition Unicorn to help lend control to $N.", ch, NULL, victim, TO_CHAR);
  if ( IS_AFFECTED(victim, AFF_CHARM) || IS_SET(victim->act2, ACT2_ULTRA_MOB) ||
    ch->level+25 < victim->level)
  {
    act( "$N appears to be unable to calm down.",  ch, NULL, victim, TO_CHAR );
    return;
  }

  if (victim->race != race_lookup("vampire") && victim->race != race_lookup("methuselah") &&
    victim->race != race_lookup("garou") && victim->race != race_lookup("fera") &&
    victim->race != race_lookup("abomination"))
  {
    send_to_char("Your victim isn't harboring an inherent supernatural fury.\n\r", ch);
    return;
  }

  WAIT_STATE( ch, 24 );
  success = godice(get_attribute(ch,MANIPULATION)+ch->pcdata->primal_urge, 8);
  if(success < 0)
  {
    act( "$N seems unmoved by the power of the spirits, and instead attacks!",  ch, NULL, victim, TO_CHAR );
    send_to_char("The toll of summoning Unicorn leaves you feeling less determined.\n\r", ch);
    multi_hit( victim, ch, TYPE_UNDEFINED );
    ch->cswillpower--;
    return;
  }

  if (success == 0) 
  {
    act( "$N seems unaffected by the spirit's presence.", ch, NULL, victim, TO_CHAR);
    if (godice(ch->csmax_willpower, 6) <= 0)
    {
      send_to_char("The toll of summoning Unicorn leaves you feeling less determined.\n\r", ch);
      ch->cswillpower--;
    }
    return;
  }

  act("Spirits of Unicorn grant your request, calming the fury dwelling in $N.",ch,NULL,victim,TO_CHAR);

  REMOVE_BIT(victim->act, ACT_AGGRESSIVE);
  affect_strip(victim, gsn_berserk);
  affect_strip(victim, gsn_vamp_frenzy);
  affect_strip(victim, gsn_garou_frenzy);
  affect_strip(victim, gsn_thaumaturgy_frenzy);
  affect_strip(victim, gsn_vigor);
  affect_strip(victim, gsn_zeal);

  af.where = TO_AFFECTS;
  af.type = skill_lookup("calm");
  af.level = success*20;
  af.duration = success*4;
  af.location = APPLY_HITROLL;
  if (!IS_NPC(victim))
    af.modifier = -success * 10;
  else
    af.modifier = (-success * 25) - ch->level / 2;
  af.bitvector = AFF_CALM;
  affect_to_char(victim,&af);

  af.location = APPLY_DAMROLL;
  af.modifier = -success * 10;
  af.bitvector = 0;
  affect_to_char(victim,&af);

  af.location = APPLY_AC;
  af.modifier = success * 50;
  affect_to_char(victim, &af);
  return;
}
//
//“Reshape Object”
//beaver spirit
//Roll: Manipulation + Crafts (Variable difficulty)
//The garou can shape natural materials into something else instantly. Stones become spearheads, animal hides become armor, a flower becomes perfume. (Different affects for different types of materials. “reshape wood weapon” to make a weak wooden weapon, “reshape ore armor” to make a strong piece of armor. Number of successes determine how long the object lasts. (WOOO OBJECT TIMERS) )
//Cost: 1 Gnosis
void spell_gift_reshapeobject( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank 4
//“Bury the Wolf”
//wolf spirit
//Roll: Gnosis (difficulty of their own willpower)
//From time to time it may be necessary for a werewolf to hide her true nature from those around and put a halt on the uneasy feeling all humans get around a Garou in any form. (If successful, the garou appears to all senses (disciplines, magic, gifts, etc) to be a normal human. Their rage is reduced by the number of successes on the roll. Cannot use Rage while active, cannot shapeshift while active. Can use gifts that do not use Rage. Successes also affect duration. successess * gnosis?)
//Cost: 1 Gnosis to activate. 1 gnosis to deactivate before timer runs out.
void spell_gift_burythewolf( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Spirit Ward”
//Ancestor spirit
//Roll: Manipulation + Rituals (Diff 7)
//This gift allows the garou to protect herself from spirits by warding the area around her. (Forces any spirit-type mobs in the room into the Umbra. Spirit type mobs cannot enter the same room the Garou is in.)
//cost: 1 Gnosis
void spell_gift_spiritward( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank 5
//“Assimilation”
//Chameleon spirit
//Roll: Manipulation + Empathy (diff 7)
//A werewolf with this gift blends smoothly into any culture, no matter how strange or unfamiliar he might normally find it. (Allow the werewolf to assume the shape of someone else. Also let them speak that person’s language for the duration Lasts 24 hours.)
//Cost: 1 Willpower
//
void spell_gift_assimilation( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“False Comfort”
//Ancestor spirit
//Roll: Gnosis (diff 7)
//Humanity has done much to surround itself with comfort and safety. Armor to protect themselves from their own weapons, locks to keep out their own brothers, but the ancestor spirits remember when these comforts were merely dreams and can teach the Garou how to bypass them (Allows the garou to ignore human technology. Give immunity to weapon, completely ignore opponents armor class. 1 tick per success)
//Cost: 1 Gnosis
//
void spell_gift_falsecomfort( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Metis
//Rank 1
//“Create Element”
//Any elemental spirit
//Roll: Gnosis (diff 7)
//The Metis has the power to create a small amount of one of the four basic elements, fire, water, air, or earth. (create water -> fill a waterskin. Create Earth -> dumps rocks on target, stunning, create fire -> directs a small blast of flame at the target. Create Air -> Not sure. Knock flying mobs out of the air?)
//Cost:1 Gnosis
#define FIRE 1
#define WATER 2
#define EARTH 3
#define AIR 4
void spell_gift_createelement( int sn, int level, CHAR_DATA *ch, void *vo, int target){

		char arg1[MIL];
		char arg2[MIL];
		CHAR_DATA *victim;
		OBJ_DATA *obj;
		gift_target_name = one_argument(gift_target_name, arg1);
		gift_target_name = one_argument(gift_target_name, arg2);
		int success;
    int mod = ch->level;
		int type = 0;

		if (IS_NULLSTR(arg1) && IS_NULLSTR(arg2))
		{
			sendch("Syntax:\n\r", ch);
			sendch("beseech 'create element' water <container>			-Fill Waterskin\n\r", ch);
			sendch("beseech 'create element' earth <target> 			-Dump rocks on target\n\r", ch);
			sendch("beseech 'create element' fire <target>				-Send a blast of flame at target\n\r", ch);
			sendch("beseech 'create element' air <target>				-Knock target out of air.\n\r", ch);
			return;
		}

		if (!str_cmp(arg1, "fire"))
			type = FIRE;
	    else if (!str_cmp(arg1, "water"))
			type = WATER;
		else if (!str_cmp(arg1, "earth"))
			type = EARTH;
		else if (!str_cmp(arg1, "air"))
			type = AIR;

		if (!type)
		{
			do_function(ch, &do_beseech, "'create element'");
			return;
		}

		if (type == FIRE || type == EARTH || type == AIR)
		{
			if (arg2[0] == '\0')
			{
				if ( (victim = ch->fighting) == NULL )
				{
					sendch("Direct an elemental attack at whom?\n\r", ch);
					return;
					}
			} else if ( (victim = get_char_room(ch, NULL, arg2)) == NULL )
				{
					sendch("They aren't here.\n\r", ch);
					return;
				}
		}

		if (type == AIR && !IS_AFFECTED(victim, AFF_FLYING))
		{
			sendch("They are not flying!\n\r", ch);
			return;
			}

		if (type == WATER && (obj =  get_obj_carry(ch, arg2, ch)) == NULL)
		{
			sendch("You don't seem to have that container...", ch);
			return;
		}

		if (ch->pcdata->gnosis[TEMP] < 1)
		{
        sendch("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
		}

		ch->pcdata->gnosis[TEMP]--;
		success = godice(ch->pcdata->gnosis[PERM], 6);

    if (success < 0)
    {
      send_to_char("The elements you try to command turn against you!\n\r", ch);
      d10_damage(ch, ch, -success, mod, gsn_gift_createelement, DAM_AGGREVATED, DEFENSE_NONE, TRUE, TRUE);
      return;
    }

		if (success == 0)
		{
			sendch("Your attempt to call forth the desired element.\n\r", ch);
			return;
		}

		if (type == FIRE)
		{
			mod = ch->level * 3;
			act("You direct a blast of flames at $N!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Fire as a blast of flame engulfs $N!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. You're engulfed in flames!", ch, NULL, victim, TO_VICT);
			d10_damage(ch, victim, success, mod, gsn_gift_createelement, DAM_FIRE, DEFENSE_NONE, TRUE, TRUE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
      if (success > 4)
      {
        act("The flames leap into $N's eyes, blinding $M!", ch, NULL, victim, TO_CHAR);
        act("The flames leap into your eyes, burning your sockets!", ch, NULL, victim, TO_VICT);
        act("$N howls in pain as the flames lick across $S face.", ch, NULL, victim, TO_NOTVICT);
        fire_effect(victim, success, ch->level, TARGET_CHAR);
      }
			return;
		}
		if (type == WATER)
		{
      act("Focusing on $p, you summon the element of water to bubble forth into existance.", ch, obj, victim, TO_CHAR);
			spell_create_water(sn, level, ch, (void *) obj, target);
      if (success > 4)
      {
        act("The spirits of water cause $p to overflow, leaving a large puddle on the ground!", ch, obj, victim, TO_CHAR);
        act("Water overflows from $p, leaving a large puddle on the ground.", ch, obj, victim, TO_NOTVICT);
        OBJ_DATA *spring;
        spring = create_object( get_obj_index( OBJ_VNUM_SPRING ), 0 );
        spring->timer = success * 2;
        spring->name = str_dup("puddle water");
        spring->short_descr = str_dup("a large puddle of water");
        spring->description = str_dup("A large puddle of clear water invites all to drink.");
        obj_to_room( spring, ch->in_room );
      }
			return;
		}
		if (type == EARTH)
		{
			mod = ch->level * 3;
			act("You create a bunch of heavy rocks to drop on $N!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Earth as rocks rain down from the heavens onto $N!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. boulders are raining down on your head!", ch, NULL, victim, TO_VICT);
			d10_damage(ch, victim, success, mod, gsn_gift_createelement, DAM_BASH, DEFENSE_FULL, TRUE, TRUE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
      if (success > 4)
      {
        act("The boulders collide with $N's head, jarring $M briefly!", ch, NULL, victim, TO_CHAR);
        act("The boulders collide with $N's head, jarring $M briefly!", ch, NULL, victim, TO_ROOM);
        DAZE_STATE(victim, success * PULSE_VIOLENCE);
      }
			return;
		}
	    if (type == AIR)
		{
      if (success > 4)
			   mod = ch->level * 4;
      else
          mod = ch->level;
			act("You create gusts of air to knock $N from the sky!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Air and gusts of air begin to buffet $N from the skies!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. strong winds blow you out of the sky!", ch, NULL, victim, TO_VICT);
			affect_strip(victim, gsn_fly);
			REMOVE_BIT(victim->affected_by, AFF_FLYING);
      d10_damage(ch, victim, success, mod, gsn_gift_createelement, DAM_BASH, DEFENSE_FULL, TRUE, TRUE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			return;
		}


    return;
}
//
//“Sense Wyrm”
//Any spirit
//Roll: Perception + Occult (Difficulty variable)
//This allows the garou to sense whether a target is wyrm tainted. It is not a mystical sense but many garou will say something ‘stinks of the wyrm’. (Lets the garou sense whether the target is wyrmtainted in any way. The less obvious the taint, the more difficult the roll. Sensing a vampire with low humanity = diff 4, sensing a human with ‘evil’ alignment diff 8)
//Cost: None.
//
void spell_gift_sensewyrm( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    int success;

    if(is_affected(ch, gsn_gift_sensewyrm))
    {
        send_to_char("You renew your vision of the Wyrm's foul taint.\n\r",ch);
        affect_strip(ch,gsn_gift_sensewyrm);
    }

    success = godice(get_attribute(ch, PERCEPTION) + get_ability(ch, CSABIL_OCCULT), 6);

    if ( success < 1)
    {
    sendch("You fail to attune your senses to sniffing out the Wyrm's foul influence.\n\r", ch);
    return;
    }

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_sensewyrm;
    af.level        = level;
    af.duration     = ch->pcdata->gnosis[PERM] * success;
    af.modifier     = 0;
    af.location     = APPLY_NONE;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    act("You become watchful of the world around you and sense the taint of wyrm near.",ch,NULL,NULL,TO_CHAR);
    act("$n seems somehow more watchful.",ch,NULL,NULL,TO_NOTVICT);
    return;
}

//“Shed”
//lizard or snake spirit
//Roll: Dexterity + Primal Urge (diff 6)
//The metis can shed a layer of fur and skin, slipping from an opponents grasp or escaping damage from a weapon as it merely slides off her body. (Add to armor. Make resistant to bashing damage. short duration, long recovery. 2-3 ticks active, 10-12 ticks unusuable.)
//Cost: 1 willpower
void spell_gift_shed( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    AFFECT_DATA af;
    int shed_success = 0;

    if (ch->cswillpower < 1)
    {
        sendch("You do not possess the strength of Will to activate this gift.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_gift_shed))
    {
        sendch("You are already shedding fur and flesh!\n\r", ch);
        return;
    }

    ch->cswillpower--;

    shed_success = godice(get_attribute(ch, DEXTERITY)+ch->pcdata->primal_urge, 7);

    if (shed_success < 0)
    {
      act("You reach out to the spirits of the serpents, and feel a sharp bite as your skin shreds and rips, losing most of your fur.", ch, NULL, NULL, TO_CHAR);

      af.where        = TO_VULN;
      af.type         = gsn_gift_shed;
      af.level        = -1;
      af.duration     = -(shed_success);
      af.modifier     = 0;
      af.location     = APPLY_NONE;
      af.bitvector    = VULN_BASH;
      affect_to_char(ch, &af);
      return;
    }

    if (shed_success == 0)
    {
      act("Your plea to the spirits seems to go unanswered.", ch, NULL, NULL, TO_CHAR);
      return;
    }

    act("Your fur and skin begin to loosen, ready to fall out at any moment.", ch, NULL, NULL, TO_CHAR);

    af.where        = TO_RESIST;
    af.type         = gsn_gift_shed;
    af.level        = shed_success;
    af.duration     = (ch->pcdata->gnosis[PERM]) + (shed_success*3);
    af.modifier     = 0;
    af.location     = 0;
    af.bitvector    = RES_BASH;
    affect_to_char(ch, &af);

    gain_exp(ch, shed_success);
    return;
}
//
//Rank 2
//“Curse of Hatred”
//spirit of hate
//Roll: Manipulation + Expression (diff: target’s willpower)
//Metis suffer constant abuse throughout their lives, yet they know they are the fortunate ones. Most Metis aren’t even allowed to live. The Garou can verbalize the hatred in her heart, disheartening opponents. (“-2 rage, -2 willpower” is what the book says. If mobs won’t have rage and willpower, lower strength, hitroll and damroll. Another short duration, long recovery. 2-3 ticks active, 10-12 recovery.)
//Cost: 1 gnosis
void spell_gift_curseofhatred( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int cursesuccess = 0;
  int diff = 6;

  if (is_affected(victim, gsn_gift_curseofhatred) || IS_AFFECTED(victim, AFF_CURSE))
  {
      sendch("Your target has already been afflicted with a curse.\n\r", ch);
      return;
  }

  if (ch->move <= ch->level * 2)
  {
      sendch("You are too tired to call to the spirits\n\r", ch);
      return;
  }

  if (IS_AFFECTED(ch,AFF_CURSE))
    diff++;

  ch->move -= ch->level * 2;
  cursesuccess = godice(get_attribute(ch, MANIPULATION)+get_ability(ch, CSABIL_EXPRESSION), diff);

  if (cursesuccess < 0)
  {
    send_to_char("The spirits of hate are offended, turning their ire upon you!\n\r", ch);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_curseofhatred;
    af.level        = cursesuccess;
    af.duration     = 3;
    af.modifier     = 0;
    af.location     = 0;
    af.bitvector    = AFF_CURSE;
    affect_to_char(ch, &af);
    return;
  }

  if (cursesuccess == 0)
  {
    send_to_char("Your call of hatred carries no weight, seeming to go unanswered.\n\r", ch);
    return;
  }

    send_to_char("With spirits of hatred lending strength, you focus a curse at you opponent.\n\r", ch);
    act("$N shirks back, as $n glares menacingly.", ch, NULL, victim, TO_NOTVICT);
    act("You shirk momentarily, as $n glares at you.", ch, NULL, victim, TO_VICT);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_curseofhatred;
    af.level        = cursesuccess;
    af.duration     = 1 + (cursesuccess*3);
    af.modifier     = -(cursesuccess * 50);
    af.location     = APPLY_HITROLL;
    af.bitvector    = AFF_CURSE;
    affect_to_char(victim, &af);

    af.modifier     = -(cursesuccess * 25);
    af.location     = APPLY_DAMROLL;
    af.bitvector    = 0;
    affect_to_char(victim, &af);
    gain_exp(ch, cursesuccess);
    return;
}

//
//“Sense Silver”
//Lunes (lunar spirits)
//Roll: Perception + Primal Urge (diff 7)
//For reasons known only to herself, Luna has been kind enough to the metis to allow them to sense a werewolf’s greatest weakness.. silver. (Allows the garou to tell when someone is carrying silver. short duration, like detect magic or evil, but if any of the objs on the char are made from silver, they’ll have an aura.)
//Cost: None.
void spell_gift_sensesilver( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int silversuccess = 0;

  if (is_affected(ch, gsn_gift_sensesilver))
  {
      sendch("You have already asked the spirits for a boon regarding silver.\n\r", ch);
      return;
  }

  silversuccess = godice(get_attribute(ch, PERCEPTION)+ch->pcdata->primal_urge, 7);

  if (silversuccess < 0)
  {
    act("Your request seems to have offended the Lunar spirits.", ch, NULL, NULL, TO_CHAR);

    af.where        = TO_VULN;
    af.type         = gsn_gift_sensesilver;
    af.level        = -1;
    af.duration     = 2;
    af.modifier     = 0;
    af.location     = APPLY_NONE;
    af.bitvector    = VULN_SILVER;
    affect_to_char(ch, &af);
    return;
  }

  if (silversuccess == 0)
  {
    act("Your plea to the lunar spirits seems to be unanswered.", ch, NULL, NULL, TO_CHAR);
    return;
  }

  act("The Lunes bless you with the ability to sense the presence of silver.", ch, NULL, NULL, TO_CHAR);

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_sensesilver;
  af.level        = silversuccess;
  af.duration     = ( silversuccess * 10 ) + 10;
  af.modifier     = 0;
  af.location     = 0;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  gain_exp(ch, silversuccess);
  return;
}

//Rank 3
//
//“Eyes of the Cat”
//Any cat spirit
//Roll:None.
//The werewolf can see clearly in pitch darkness. Her eyes glow a lambent green while this power is in effect. (Pretty obvious, night vision for Garou. But since this is a free gift with no roll, the garou is blinded by sources of light.)
//Cost: None
void spell_gift_eyesofthecat( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int dicesuccess;

  if (is_affected(ch, gsn_gift_eyesofthecat))
  {
    sendch("Your feline sight fades, along with the green glow of your eyes.\n\r", ch);
    affect_strip(ch, gsn_gift_eyesofthecat);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  if (ch->move <= ch->level / 3)
  {
    send_to_char("You are too tired to ask for your feline blessing.\n\r", ch);
    return;
  }

  ch->move -= ch->move / 3;

  sendch("Your eyes take on a green glow, as you can now see clearly in the darkness.\n\r",ch);
  act("$n's eyes begin to softly glow green.", ch, NULL, NULL, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 8;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = AFF_DARK_VISION;
  affect_to_char( ch, &af );

  af.where      = TO_VULN;
  af.bitvector  = VULN_LIGHT;
  affect_to_char(ch, &af);

  return;
}
//
//“Mental Speech”
//Bird spirits
//Roll: Charisma + Empathy (diff 8)
//This gift allows mental communication even over vast distances. (This power is made useless by tells. Will find something to replace it.)
//Cost: 1 Willpower
void spell_gift_mentalspeech( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *groupmate, *group_next;
  int success;

  if (is_affected(ch, gsn_gift_mentalspeech))
  {
    if (get_affect_level(ch, gsn_gift_mentalspeech) == -1)
    {
      send_to_char("The constant chirping in the background makes it hard to focus.\n\r", ch);
      return;
    }
    send_to_char("You drop the mental coordination with your current group.\n\r", ch);
    for ( groupmate = char_list; groupmate != NULL; groupmate = group_next )
    {
      group_next = groupmate->next;
      if(!IS_NPC(groupmate) || groupmate->in_room == NULL )
        continue;

      if ( SAME_UMBRA(ch, groupmate) && is_same_group(ch, groupmate))
        affect_strip(groupmate, gsn_gift_mentalspeech);

      continue;
    }
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have enough Willpower to enact a group coordination.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  success = godice(get_attribute(ch, CHARISMA)+get_ability(ch, CSABIL_EMPATHY), 8);

  if (success < 0)
  {
    send_to_char("Bird spirits assault you with a cacaphony of sounds in the background.\n\r", ch);
    if (!is_affected(ch, gsn_deafened))
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_gift_mentalspeech;
      af.level     = -1;
      af.duration  = 1;
      af.modifier  = -ch->level;
      af.location  = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      af.bitvector = APPLY_DAMROLL;
      af.modifier  = -ch->level / 2;
      affect_to_char( ch, &af );
    }
    WAIT_STATE(ch, 3);
    return;
  }

  if (success == 0)
  {
    send_to_char("Bird spirits are refusing to carry your mental communications.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  for ( groupmate = char_list; groupmate != NULL; groupmate = group_next )
  {
    group_next = groupmate->next;
    if(!IS_NPC(groupmate) || groupmate->in_room == NULL )
      continue;

    if ( SAME_UMBRA(ch, groupmate) && is_same_group(ch, groupmate))
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_gift_mentalspeech;
      af.level     = success;
      af.duration  = success * 10 + 20;
      af.modifier  = ch->level * 2 + 20;
      af.location  = APPLY_HITROLL;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      af.bitvector = APPLY_DAMROLL;
      af.modifier  = ch->level / 2;
      affect_to_char( ch, &af );
    }
    continue;
  }

  return;
}
//
//Rank 4
//“Gift of the Porcupine”
//Porcupine spirit
//Roll: None.
//Using this gift the werewolf undergoes a startling transformation. Her fur becomes elongated, bristly, and sharp like porcupine quills. Must be Crinos/Hispo/Lupus. (Gift of the Porcupine gift thats already coded. Quills do aggrevated damage. adds extra damage to bash, ground, etc.)
//Cost: 1 gnosis

void spell_gift_porcupine( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( is_affected( ch, gsn_gift_porcupine ) )
    {
        send_to_char("You have already called upon this Gift.\n\r",ch);
        return;
    }
    if(ch->pcdata->shiftform < CRINOS)
    {
        send_to_char("The Goddess Luna reaches out and whaps you on the head and says '{WWhat fur?{x'\n\r",ch);
        return;
    }
    if (ch->pcdata->gnosis[TEMP] < 1)
    {
        sendch("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
        }
	ch->pcdata->gnosis[TEMP]--;

    af.where     = TO_RESIST;
    af.type      = gsn_gift_porcupine;
    af.level     = level;
    af.duration  = 5;
    af.modifier  = -(level/4);
    af.location  = APPLY_HITROLL;
    af.bitvector = RES_PIERCE;
    affect_to_char( ch, &af );

    act("$n lets forth a savage roar as $s fur grows long and sharp like spikes.",ch,NULL,NULL,TO_ROOM);
    send_to_char( "You roar savagely as your fur lengthens and grows sharp.\n\r", ch );
    return;
}

//
//“Wither Limb”
//Venomous spider or snake, disease spirits
//Roll: (To touch) Dex + Brawl (diff 7 for leg, diff 8 for arm) vs wits + dodge (diff 6)
//Roll: (To wither) Willpower (diff target stamina + 4)
//The garou can cause their victims limbs to twist and wither with a touch. (“wither (leg/arm)” Roll dex+bra to grapple, if successful roll for gift activation. Withering leg makes them easier to hit (Harder to move around, harder to defend) Withering arm makes them do less damage (with weapons. What about kick/bite/etc?)
//Cost: 1 Gnosis
void spell_gift_witherlimb( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int grab, wither;

  if (IS_NPC(ch)) return;

  if (ch->move <= ch->level)
  {
    send_to_char("You are too tired to grab your opponent./n/r", ch);
    return;
  }

  ch->move -= ch->level;

  grab = godice(get_attribute(ch, DEXTERITY) + get_ability(ch, CSABIL_BRAWL), 6);
  wither = godice(ch->csmax_willpower, 6);

  WAIT_STATE(ch, 3);

  if (grab < 0)
  {
    act("You stumble while trying to grab $N's limb.", ch, NULL, victim, TO_CHAR);
    act("$n stumbles briefly.", ch, NULL, victim, TO_NOTVICT);
    act("$n stumbles briefly.", ch, NULL, victim, TO_VICT);
    ch->stopped++;
    return;
  }

  if (grab == 0)
  {
    act("You can't seem to get a good hold on one of $N's limbs.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  if (is_affected(victim, gsn_gift_witherlimb))
  {
    sendch("You sense the touch of disease already upon your target.\n\r", ch);
    return;
  }

  ch->pcdata->gnosis[TEMP] -= 1;

  if (wither < 0)
  {
    act("The disease spirits are offended at your request.", ch, NULL, victim, TO_CHAR);

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_witherlimb;
    af.level     = -1;
    af.duration  = -wither;
    af.location  = APPLY_CS_STR;
    af.modifier  = -1;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  if (wither == 0)
  {
    act("Grabbing $N, you ask the spirit's for aid, but receive nothing.", ch, NULL, victim, TO_CHAR);
    act("$n grabs your limb, but lets go quickly.", ch, NULL, victim, TO_VICT);
    act("$n grabs $N's limb, but lets go quickly.", ch, NULL, victim, TO_NOTVICT);
    return;
  }

  act( "Grabbing $N, $E screams in anguish and agony as spirits ravage $S body!", ch, NULL, victim, TO_CHAR );
  act( "$n reaches out and grabs $N, who suddenly screams in agony!", ch, NULL, victim, TO_NOTVICT );
  act("Your body flares in excrutiating pain as you feel crippled by $n's grab!", ch, NULL, victim, TO_VICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_witherlimb;
  af.level     = wither;
  af.duration  = (wither * 2) + 1;
  af.location  = APPLY_CS_STR;
  if (wither >= 4)
  {
    d10_damage( ch, victim, wither, ch->level * 3, gsn_magick, DAM_DISEASE, DEFENSE_NONE, TRUE, TRUE);
    af.modifier = -3;
  }
  else
    af.modifier  = -2;
  af.bitvector = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_DEX;
  affect_to_char( victim, &af );
  return;
}
//
//Rank 5
//“Madness”
//Lunes, spirits of trickery, madness
//Roll:Manipulation + Intimidation (Difficulty target willpower)
//This gift allows the metis to force her inner demons and memories of horror and abuse on others, causing madness. (Book says “Gives the target a derangement.”. I’m thinking, forgetful mind type ability that decimates hitroll. (much harder to hit something when you’re dodging invisible unicorns launching watermelons from Photon cannons.)
//Cost: 1 Gnosis
void spell_gift_madness( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Totem Gift”
//Pack Totem
//Roll: Charisma + Rituals (diff 7)
//The torment and abuse that a metis endure growing up gives it a very strong bond with it’s tribal totem and she may plead directly to the mighty spirit, gaining some of its power. (Pretty much exactly the ‘totem’ ability newrou have now.)
//Cost: 1 Gnosis
void spell_gift_totemgift( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//
//Lupus
//Rank 1
//“Hare’s Leap”
//Hare spirits, occasionally cats, frogs
//Roll: Strength + Athletics (diff 7)
//Allows the garou to leap incredible distances. (Not sure what we would do with this one.)
// Michial suggested an evade-like skill. short-duration. garou is able to 'jump' out of range
// of attacks and back in for quick spurts of damage.
void spell_gift_haresleap( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_haresleap))
  {
    sendch("Your leaps are already enhanced by the Hare spirits.\n\r", ch);
    return;
  }

  if (ch->move < (ch->level * 5) / 4)
  {
    send_to_char("You are too exhausted to activate this Gift.\n\r", ch);
    return;
  }

  if (success < 0)
  {
    send_to_char("A herd of hare spirits bound past your legs, slamming into you.\n\r", ch);
    d10_damage( ch, ch, 5, ch->level, gsn_bash, DAM_BASH, DEFENSE_NONE, TRUE, TRUE);
    WAIT_STATE(ch, 6);
    return;
  }

  if (success == 0)
  {
    send_to_char("Hare spirits bound away, ignoring your request.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  ch->move -= (ch->level * 5) / 4;
  success = godice(get_attribute(ch, STRENGTH) + get_ability(ch, CSABIL_ATHLETICS), 7);

  sendch("You pay homage to the Hare spirits and you feel your legs surging with strength!\n\r", ch);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_haresleap;
  af.level        = ch->pcdata->rank;
  af.duration     = success * 5 + 20;
  af.modifier     = 0;
  af.location     = APPLY_NONE;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  return;
}
//
//“Heightened Senses”
//wolf spirit
//Roll: None
//The wolf with this gift tunes in to the world around her, increasing her senses vastly. (Auspex like affect. Can see heightened senses descriptions and hidden mobs by sniffing them out. Gives +2 perception, )
//Cost: 1 Gnosis
void spell_gift_heightenedsenses( int sn, int level, CHAR_DATA *ch, void *vo, int target){

    AFFECT_DATA af;

    if (is_affected(ch, gsn_gift_heightenedsenses))
    {
    sendch("You dull your senses back to normal.\n\r", ch);
    affect_strip(ch, gsn_gift_heightenedsenses);
    return;
    }

    if (ch->pcdata->gnosis[TEMP] < 1)
    {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
    }
    ch->pcdata->gnosis[TEMP]--;
    sendch("You senses increase tenfold and you take notice of things you missed before...\n\r",ch);

    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = ch->pcdata->gnosis[PERM] * 4;
    af.modifier  = 1;
    af.location  = APPLY_CS_PER;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( ch, &af );

    af.where     = TO_AFFECTS2;
    af.level     = ch->rank;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = AFF2_DETECT_UNSEEN;
    affect_to_char(ch, &af );
    return;
}
//
//“Ley Lines”
//owl spirit
//Roll: Perception + Enigmas (diff 7)
//This gift allows the garou to instinctively sense where the nearest caern is. (If successful, tells the garou whether or not there is a Caern/node in the area. If 3 or more successes, allows them to see the room itself (like telepathy)
//Cost: 1 Gnosis
void spell_gift_leylines( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    ROOM_INDEX_DATA *pRoomIndex;
    AREA_DATA       *pArea;
    int             vnum;
    BUFFER          *output;
    char            buf  [ MAX_STRING_LENGTH   ];
    int             dicesuccess;
    int             found;

    if (ch->pcdata->gnosis[TEMP] < 1)
    {
        sendch("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
        }

    ch->pcdata->gnosis[TEMP]--;
    dicesuccess = godice(get_attribute(ch, PERCEPTION) + get_ability2(ch, CSABIL_ENIGMAS), 7);

    if (dicesuccess < 1)
    {
        send_to_char("You are unable to sense the lies of energy passing through here.\n\r", ch);
        return;
    }

    send_to_char("You open your eyes to seeing the lines of energy in the area..\n\r", ch);
    pArea = ch->in_room->area;
    output = new_buf();
    found = 0;

    for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
        {
            if ( ( pRoomIndex = get_room_index( vnum ) ) && pRoomIndex->sector_type == SECT_NODE )
            {
                sprintf( buf, "%s\n\r", pRoomIndex->name );
                add_buf(output, buf);
                found++;

            }
        }
    if (found)
    {
        sprintf(buf, "..and you detect %d %s in this area.\n\r", found, found == 1 ? "Caern" : "Caerns");
        send_to_char(buf,ch);
        page_to_char(buf_string(output), ch);
    } else
        send_to_char("..but there are no Caerns to be found.\n\r", ch);

    return;
}
//
//Rank 2
//
//“Name the Spirit”
//cat spirits
//Roll: perception + occult (diff 8)
//A werewolf with this gift becomes familiar with the denizens of the Umbra. She can sense the type and approximate trait level of spirits. (The garou can get detailed information about a spirit type mob. More successes = more information.)
//Cost: 1 Willpower
void spell_gift_namethespirit( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int dicesuccess;

  if (is_affected(ch, gsn_gift_namethespirit))
  {
    send_to_char("You already feel attuned to beings of the Umbra.\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    sendch("You do not have the strength of will to activate this gift.\n\r", ch);
    return;
  }

  dicesuccess = godice(get_attribute(ch, PERCEPTION) + get_ability(ch, CSABIL_OCCULT), 8);

  ch->cswillpower--;

    if (dicesuccess <= 0)
  {
    send_to_char("You try to attune yourself to the spirits in the Umbra, but you cannot seem to grasp it.\n\r", ch);
    WAIT_STATE(ch, 8);
    return;
  }

   sendch("You reach out with your senses and become attuned to the denizens of the Umbra.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = (dicesuccess * 10) + 50;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  gain_exp(ch, dicesuccess * 5);

  return;
}
//“Scent of Sight”
//wolf spirit
//Roll: Perception + Primal Urge (Difficulty based on sector.)
//Allows the garou to compensate for her vision completely using her sense of smell. Negates the effects of blindness, fireblindness, can see in darkness, can see invisible and hidden. Outdoors, woods, etc, low difficulty. cities, indoors, high difficulty. Very short duration, long recovery (2-3 ticks, 10-12 ticks)
//Cost: None
void spell_gift_scentofsight( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int dicesuccess;

  if (is_affected(ch, gsn_gift_scentofsight))
  {
    sendch("You tone down your senses of smell and hearing back to normal.\n\r", ch);
    affect_strip(ch, gsn_gift_scentofsight);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  dicesuccess = godice(get_attribute(ch, PERCEPTION) + get_ability2(ch, ch->pcdata->primal_urge), 7);

  ch->pcdata->gnosis[TEMP]--;

  if (dicesuccess < 0)
  {
    send_to_char("Your supplication to Gaea for extra senses seems to be in vain.\n\r", ch);
    WAIT_STATE(ch, 8);
    return;
  }

  if (dicesuccess == 0)
  {
    send_to_char("Your senses do not seem to have increased as you expected.\n\r", ch);
    WAIT_STATE(ch, 2);
    return;
  }

  sendch("Your senses of smell and hearing sharpen drastically, allowing you to compensate for sight.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = (dicesuccess * 10) + 20;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = AFF_DARK_VISION;
  affect_to_char( ch, &af );

  af.where      = TO_VULN;
  af.bitvector  = VULN_SOUND;
  affect_to_char(ch, &af);

  gain_exp(ch, dicesuccess * 3);

  return;
}
//
//Rank 3
//“Devil’s Child”
//wolf spirit
//Roll:Manipulation + Primal Urge (diff target willpower)
//Wolves are often seen as the offspring of the devil himself, and while they don’t fully understand that implication the lupus are happy to capitalize on the fear it inspires. The garou lets loose a fierce snarl that terrifies and cows opponents. (Staredown affect. forces target to stop fighting and flee.)
//Cost None. (Optionally, can add a ‘gnosis’ option to this gift to spend a gnosis point and affect every person in the room.)
void spell_gift_devilschild( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *victim;
  CHAR_DATA *vict_next;
  int success;

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  if (ch->pcdata->shiftform != LUPUS)
  {
    send_to_char("You must be in Lupus form to properly channel wolf-spirits for this Gift.\n\r", ch);
    return;
  }

  ch->pcdata->gnosis[TEMP]--;
  success = godice(get_attribute(ch, CSATTRIB_MANIPULATION) + ch->pcdata->primal_urge, 6);

  if (success < 0)
  {
    act("You bare teeth, snarling for a long time posturing, but the wolf-spirits refuse to lend a hand.", ch, NULL, NULL, TO_CHAR);
    act("$n attempts to snarl, but it comes off quite comical.", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE(ch, 9);
    return;
  }

  if (success == 0)
  {
    act("You bare your teeth, snarling, but your wolf-spirit kin are nowhere to be found.", ch, NULL, NULL, TO_CHAR);
    act("$n bares $s teeth, snarling.", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE(ch, 3);
    return;
  }

  act("Snarling viciously, you bare your teeth and channel wolf-spirits for assistance!",ch,NULL,victim,TO_CHAR);
  act("$n bares $s teeth, and snarls ferociously, inciting you to flee!",ch,NULL,NULL,TO_ROOM);
  for ( victim = char_list; victim != NULL; victim = vict_next )
  {
    vict_next = victim->next;
    if(!IS_NPC(victim) || victim->in_room == NULL )
      continue;

    if ( victim->in_room == ch->in_room && SAME_UMBRA(ch, victim) && !IS_AFFECTED(victim, AFF_INVISIBLE) &&
        victim != ch && !IS_SET(victim->imm_flags,IMM_CHARM) && !IS_SET(victim->act2, ACT2_ULTRA_MOB ) &&
        !is_same_group(ch, victim))
      {
        act("The urge is overwhelming, and you leave as fast as possible!",ch,NULL,victim,TO_VICT);
        do_function(victim, &do_flee, "auto" );
      }
      continue;
    }

  return;
}

//"Catfeet"
//cat spirits
//Roll:  None
//
void spell_gift_catfeet( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if(is_affected(ch, gsn_gift_catfeet))
    {
        send_to_char("You thank the feline spirits as your grace and balance slowly fade.\n\r",ch);
        affect_strip(ch,gsn_gift_catfeet);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_catfeet;
    af.level     = level;
    af.duration  = level;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char("Calling upon the grace of the feline spirits, your body acquires extraordinary balance.\n\r",ch);
    act("$n howls a soft dirge to the skies, and begins to move with a feline grace.",ch,NULL,NULL,TO_ROOM);
    return;
}
//
//“Sense the Unnatural”
//Servant of Gaia (any spirit not a bane, wyldling or weaver construct)
//Roll: Perception + Enigmas (diff 7)
//The werewolf can sense any unnatural presence and determine it’s approximate strength and type. (Allows the garou to get a sense of any supernatural creatures in the room. More successes = more information. 1 success “You sense there is certainly something amiss around you.” 4 successess “You sense that Dracula is a very ancient and powerful vampire. You feel the crackle of magical energy emanating from Merlin!”
void spell_gift_sensetheunnatural( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int dicesuccess;

  if (ch->move < ch->level / 5)
  {
    sendch("You are too tired to try and determine supernatural presence nearby.\n\r", ch);
    return;
  }

  dicesuccess = godice(get_attribute(ch, PERCEPTION) + get_ability2(ch, CSABIL_ENIGMAS), 6);

  ch->move -= ch->level / 5;

  if (dicesuccess < 0)
  {
    send_to_char("Your senses are overloaded by the presence of supernatural beings, they appear to be everywhere!\n\r", ch);
    WAIT_STATE(ch, 14);
    return;
  }

  if (dicesuccess == 0)
  {
    send_to_char("Your request for assistance seems to go unanswered.\n\r", ch);
    WAIT_STATE(ch, 2);
    return;
  }

  sendch("Beseeching the spirit servants of Gaia, you take notice of supernatural beings nearby.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = dicesuccess;
  af.duration  = (dicesuccess * 10) + 20;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  gain_exp(ch, dicesuccess * 3);

  return;
}
//
//Rank 4
//“Beast Life”
//Lion or wolf spirit
//Roll: Charisma + Animal Ken (diff 7)
//The werewolf can communicate with and even command wild animals. (Gives the garou an affect that lets them ‘command’ any animal type mob (one line command. last 6-10 ticks.)
//Cost: 1 gnosos
void spell_gift_beastlife( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//
//
//
//“Gnaw”
//wolf spirit
//Roll: Stamina + 4 (Variable difficulty)
//The garou’s jaw grows massively powerful and it’s teeth razored sharp allowing it to do large amounts of damage in combat and gnaw through almost any material. (Multiple affects. “gnaw” by itself Affects melee damage done by bite, adding two extra dice, difficulty 7 “Gnaw (direction)” Will allow the garou to gnaw through the lock on a door. Difficulty based on pick-ability of the lock. A pickable lock would be difficulty 5, an infuriating lock would be difficulty 8)
//Cost: 1 willpower
void spell_gift_gnaw( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank 5
//“Elemental Gift”
//elemental spirit
//Roll:Gnosis (diff 8)
//Gaia herself steps in to lend a hand, offering part of herself to the character. Allows the garou to summon forth a powerful elemental. (A powerful area attack that can either summon a massive inferno, rain down showers of sedan sized boulders, throw a barrage of  icicle spears, or a whirlwind of some sort that picks up the target and deposits them half a world away)
//(OR Summon a powerful elemental charmed mob that comes in four flavors. does different kinds of magic damage.)
//cost: 1 gnosis
void spell_gift_elementalgift( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Song of the Great Beast”
//Rare quest to hidden umbral realm
//Roll: Charisma + Primal Urge (Diff 8)
//The garou travels out into the furthest wildernesses of the Umbra and lets out a mournful howl.. and summons forth a creature that existed long before the time of Man. (Summon a mastadon. Or sabre tooth tiger. Or a tyrannasaurus rex. Unleash it upon your enemies.)
//Cost: A lot.
void spell_gift_songofthegreatbeast( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//
//Auspices
//Ragabash
//Rank 1
//“Blur of the Milky Eye”
//ermine spirit
//Roll:Manipulation + Stealth (diff 7)
//The garou’s form becomes blurred and indistinct, as if viewed through the tired vision of a crone, allowing him to pass unnoticed among others. (Hide skill.)
//Cost:none.
void spell_gift_blurofthemilkyeye( int sn, int level, CHAR_DATA *ch, void *vo, int target){

    AFFECT_DATA af;
    int success;
    if (is_affected(ch, gsn_gift_blurofthemilkyeye))
    {
    sendch("You cannot blur your form again so soon.\n\r", ch);
    return;
    }
    success = godice(get_attribute(ch, MANIPULATION)+get_ability(ch, CSABIL_STEALTH), 7);

    if (success < 1)
    {
    sendch("You fail to blur your form and remain in plain sight.\n\r", ch);
    return;
    }
    sendch("Your form becomes blurry and indistinct.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = sn;
    af.level     = level;
    af.duration  = ch->pcdata->gnosis[PERM] * success;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = AFF_HIDE;
    affect_to_char( ch, &af );

	return;
}
//
//“Scent of Running Water”
//fox spirit
//Roll:None.
//The garou can mask her scent completely, making her difficult to track. Once learned, gift is innate and always active unless the garou wishes to leave a scent. (We don’t have any sort of tracking system.. Find another gift.)
void spell_gift_scentofrunningwater( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_scentofrunningwater))
  {
    sendch("The fox spirit has already blessed you by masking your scent.\n\r", ch);
    return;
  }

  if (ch->move < ch->level / 10)
  {
    send_to_char("You are too tired to mask your scent.\n\r", ch);
    return;
  }

  ch->move -= ch->level / 10;

  success = godice(ch->pcdata->gnosis[PERM], 5);

  sendch("The spirit of the fox grants your request, masking your scent from those tracking you.\n\r", ch);
  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 25 + (success * 5);
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  gain_exp(ch, success);

  return;
}
//
//“Snow Running”
//arctic hare spirit
//Roll:None
//The deep snows of winter in much of Europe bring privation at best and starvation at worst to the unprepared and snowbound. This gift allows the garou to travel with ease across the surface of ice and snow. (Halve movement costs based on terrain. lasts a day)
//Cost: 1 gnosis
void spell_gift_snowrunning( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, sn))
  {
    sendch("You are already able to travel any terrain with ease.\n\r", ch);
    return;
  }
  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  success = godice(ch->pcdata->gnosis[PERM], 6);
  ch->pcdata->gnosis[TEMP]--;

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = ch->level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  af.duration  = 50 + (success * 5);
  affect_to_char( ch, &af );
  send_to_char( "You feel light and sure-footed, as if you could pass any terrain with ease.\n\r", ch );
  act("$n seems lighter and more sure-footed.",ch,NULL,ch,TO_NOTVICT);

  gain_exp(ch, success * 2);
  return;
}
//
//Rank 2
//“Blissful Ignorance”
//moth spirit
//dexterity + stealth (7)
//The garou can become invisible by remaining perfectly still. (Veil skill. Current gift recoded?)
//

void spell_gift_blissfulignorance( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    if ( IS_AFFECTED2(ch, AFF2_VEIL) )
    {
        REMOVE_BIT(ch->affected2_by, AFF2_VEIL);
        act( "$n suddenly appears!", ch, 0, 0, TO_NOTVICT );
        send_to_char( "You step from your place of hiding.\n\r", ch );
        return;
    }

    send_to_char( "Using your Gaia bestowed gift, you veil yourself from prying eyes..\n\r", ch );
    SET_BIT(ch->affected2_by, AFF2_VEIL);
    return;
}
//“Sense of the Prey”
//wolf
//perception + enigmas (wits+stealth / gnosis / 6)
//If the garou knows something about her intended prey, even part of a name or a description, , she can find it anywhere in the world. (scry/telepathy affect.)
void spell_gift_senseofprey( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *buffer;
    CHAR_DATA *victim;
    CHAR_DATA *prey = (CHAR_DATA *) vo;
    bool found;
    int count = 0;
    char arg1[MIL];
    gift_target_name = one_argument(gift_target_name, arg1);

    if (ch->move <= ch->level)
    {
      send_to_char("You are too tired to track prey at this time.\n\r", ch);
      return;
    }

    ch->move -= ch->level;

    found = FALSE;
    buffer = new_buf();
    for ( victim = char_list; victim != NULL; victim = victim->next )
    {
      if (victim != NULL && victim->in_room != NULL
      &&  is_name( arg1, victim->name ) && can_see(ch,victim)
      &&  can_see_room(ch,victim->in_room) && (!IS_SET(victim->act, ACT_QUESTMOB)))
      {
        found = TRUE;
        count++;
        sprintf( buf, "%3d) %-28s %s\n\r", count,
        IS_NPC(victim) ? victim->short_descr : victim->name,
        victim->in_room->name );
        add_buf(buffer,buf);
      }
    }

    if ( !found )
    {
      sprintf(buf, "You cannot seem to locate %s.\n\r", arg1);
      send_to_char(buf, ch);
    }
    else
        page_to_char(buf_string(buffer),ch);

    free_buf(buffer);
    return;
}
//
//Rank 3
//“Open Moon Bridge”
//lune
//cost: 1 gnosis
//The garou has the ability to open a moon bridge with or without the permission of the totem of the Caern. (Allows the garou to create a permenant bridge between Caerns. Will have to re-code caerns.)
void spell_gift_openmoonbridge( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Reynards Lie”
//fox spirit
//wits + subterfuge (wits+subterfuge)
//The garou can pass off a lie, no matter how ridiculous, as credible fact and utter truth. For a short while. (No idea. Find something else.)
void spell_gift_reynardslie( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank 4
//“Leper’s Curse”
//pain or disease
//cost: 1 gnosis
//roll: gnosis (willpower) vs Gnosis/stamina (garou’s gnosis)
//The garou can invoke a dabilitating curse on her enemy, afflicting her foe with bodily weakness and diseased, insensate patches of skin and pelt. (Causes negative physical attributes as well as aggravated damage every tick like poison. )
void spell_gift_leperscurse( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//“Luna’s Blessing”
//lune spirit
//cost: 1 gnosis
//This gift is a mighty token of Luna’s favor. The light of the moon upon the garou protects her from the damage of silver. The protection is so complete that the attacker’s blows might even be turned back against him. (Immunity to silver, “radiant” type aura that only works at night, when the moon is out (Not new moon))
void spell_gift_lunasblessing( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//Rank 5
//“Thieving Talons of the Magpie”
//magpie spirit
//wits + stealth (willpower)
//recurring gnosis cost to hold affect
//The garou who learns this gift becomes as gleeful a thief as the magpie, capable of stealing the magical abilities of others. (Allows the garou to steal disciplines/rotes/etc that the target has. May be able to code that. Short duration.)
void spell_gift_thievingtalonsofthemagpie( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Chimerical Form”
//chimerling spirit
//1 gnosis
//intelligence + animal ken (6)
//The garou with this gift can change his shape at will to the form of any animal he wishes to be.
//(Can take the form of an animal mob. Maybe add a few forms like protean)
void spell_gift_chimericalform( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Theurge
//Rank 1
//“Mother’s Touch”
//unicorn
//1 gnosis
//intelligence + empathy (difficulty 5)
//The garou can channel healing energy through her hands, easing the wounds of any living creature, even aggrevated. Does not work on the undead or spirits. (powerful healing affect)
void spell_gift_motherstouch( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int heal;
    int aggheal;
    int dicesuccess;

    if (ch->pcdata->gnosis[TEMP] < 1) {
        send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
    }

    if( victim->race == race_lookup("MIB") ||
        victim->race == race_lookup("spirit") ||
        victim->race == race_lookup("wraith") ||
        victim->race == race_lookup("bane") ||
        victim->race == race_lookup("vampire") ||
        victim->race == race_lookup("methuselah")) {

        send_to_char("The touch of Gaia will not heal this creature.\n\r", ch);
        return;
    }

    dicesuccess = godice(get_attribute(ch, INTELLIGENCE) + get_ability(ch, CSABIL_EMPATHY), 5);
    ch->pcdata->gnosis[TEMP]--;
    if (dicesuccess > 1)
    {
        if ( ch != victim ) {
            act("You lay hands upon $N and channel Gaia's love into healing their body.",ch,NULL,victim,TO_CHAR);
            act("$n lays their hands upon $N and $N's wounds begin to close before your eyes!",ch,NULL,victim,TO_NOTVICT);
            act("$n lays their hands upon you and your wounds begin to close before your eyes!", ch, NULL, victim, TO_VICT);
            heal = dicesuccess * 1000;
        } else
        {
            act("You feel Gaia's embrace and your wounds heal by the power of Mother's Touch.", ch, NULL, NULL, TO_CHAR);
            act("$n closes their eyes for a moment and $s wounds begin to close before your eyes!", ch, NULL, NULL, TO_ROOM);
            heal = dicesuccess * 500;
        }
        aggheal = heal/100;
        victim->hit = UMIN( victim->hit + heal, victim->max_hit );
        victim->agg_dam = UMAX(victim->agg - aggheal, 0);
        update_pos( victim );

    } else {
        send_to_char("You fail to channel Gaia's love through yourself.\n\r", ch);
        return;
    }

    return;
}
//
//“Sense Wyrm”
//See Metis Gift

//
//“Spirit Speech”
//any
//int + enigmas roll diff by storyteller to puzzle out riddles from spirits
//the garou gains an intuitive understanding of the language of spirits
void spell_gift_spiritspeech( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, sn))
  {
    sendch("You already understand the language of spirits.\n\r", ch);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  if (ch->move <= ch->level + 10)
  {
    send_to_char("You are too tired to try and understand spirits' languages.\n\r", ch);
    return;
  }

  success = godice(get_attribute(ch, CSATTRIB_INTELLIGENCE) + get_ability(ch, CSABIL_ENIGMAS), 6);
  ch->move -= ch->level + 10;

  if (success < 0)
  {
    send_to_char("Trying to understand the nuances of spirits is mentally and spiritually taxxing.\n\r", ch);
    ch->pcdata->gnosis[TEMP]--;
    return;
  }

  if (success == 0)
  {
    send_to_char("Spirits you encounter lately seem to be beyond your language abilities.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = ch->level;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  af.duration  = 50 + (success * 5);
  affect_to_char( ch, &af );
  send_to_char( "The spirits lend you aid in understanding their speech and mannerisms.\n\r", ch );
  return;
}
//
//Rank Two
//
//“Command Spirit”
//any incarna avatar
//1 wp
//char + leadership (diff spirit gnosis)
//gives commands to spirits, botch makes the spirit attack the garou
void spell_gift_commandspirit( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *gch;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int group_members = 0;

  if (victim->race != race_lookup("spirit") && victim->race != race_lookup("elemental"))
  {
    send_to_char("This Gift only allows you to speak with spirits or elementals.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_laryngitis))
  {
    send_to_char("Your throat is still too swollen to request assistance.\n\r", ch);
    return;
  }

  if(ch->pet != NULL)
  {
    send_to_char( "You cannot command that many creatures at once!\n\r",ch );
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have the force of Willpower to command such a Gift.\n\r", ch);
    return;
  }

  if (room_is_silent( ch, ch->in_room ))
  {
    send_to_char("Try as you might, no sound emerges from your lips.\n\r",ch);
    return;
  }

  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (is_same_group(gch, ch) && gch != ch)
      group_members++;
  }

  if (group_members + 1 > (ch->csabilities[CSABIL_LEADERSHIP]*2)+1)
  {
    send_to_char("You are already leading around as much of an entourage as you can manage.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  sh_int diff, success;
  success = 0;
  diff = 6;

  if (victim->level > ch->level)
    diff++;
  if (victim->level > ch->level + 10)
    diff++;
  if (IS_SET(victim->res_flags, RES_CHARM) || IS_SET(victim->res_flags, RES_MENTAL))
    diff++;
  if (IS_SET(victim->vuln_flags, VULN_CHARM) || IS_SET(victim->vuln_flags, VULN_MENTAL))
    diff-= 2;
  if (is_affected(ch, gsn_gift_spiritspeech))
    diff--;
  success = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_LEADERSHIP], diff);

  if (success < 0)
  {
    act( "$n speaks gibberish, offending you as $e does so!", ch, NULL, victim, TO_VICT );
    act( "$n speaks gibberish to $N, who attacks at the insult!", ch, NULL, victim, TO_NOTVICT );
    act( "You speak plainly to $N, but $E seems offended and attacks!", ch, NULL, victim, TO_CHAR );

    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
  }

  if (success == 0)
  {
    act( "$n speaks slowly, but $e is quite boring.", ch, NULL, victim, TO_VICT );
    act( "$n speaks to $N, who seems unimpressed.", ch, NULL, victim, TO_NOTVICT );
    act( "Your words seem to bore $N.", ch, NULL, victim, TO_CHAR );
    WAIT_STATE(ch, 6);
    return;
  }

  if (is_affected(victim, gsn_deafened))
  {
    act( "$n tries to speak to you, but you cannot hear a single word.", ch, NULL, victim, TO_VICT );
    act( "$n begins speaking to $N, but seems perplexed at the lack of response.", ch, NULL, victim, TO_NOTVICT );
    act( "You try to speak to $N, but $E doesn't seem to hear you.", ch, NULL, victim, TO_CHAR );
    return;
  }

  if ( IS_AFFECTED(victim, AFF_CHARM)
  ||   IS_AFFECTED(ch, AFF_CHARM)
  ||   IS_SET(victim->imm_flags,IMM_CHARM)
  ||   saves_spell( ch->level, victim,DAM_CHARM)
  ||   IS_SET(victim->act2, ACT2_ULTRA_MOB))
  {
    act( "$n speaks, but you ignore the words completely.", ch, NULL, victim, TO_VICT );
    act( "$n speaks to $N, who promptly ignores $m.", ch, NULL, victim, TO_NOTVICT );
    act( "You feel slighted, $N is ignoring you completely.", ch, NULL, victim, TO_CHAR );
    return;
  }

  act( "With a strange feeling of complacency, you agree to $n's request for aid.", ch, NULL, victim, TO_VICT );
  if ( ch != victim )
    act("After coaxing, $N offers little resistance as $E follows your request for aid.",ch,NULL,victim,TO_CHAR);
  act( "With a bit of coaxing, $N begins to follow $n's commands.",  ch, NULL, victim, TO_NOTVICT );
  if (success > 4)
    act("Realizing what you say is for the best, $N pledges service to you.", ch, NULL, victim, TO_CHAR);

  if ( victim->master )
  stop_follower( victim );
  add_follower( victim, ch );
  victim->leader = ch;
  ch->pet = victim;
  af.where     = TO_AFFECTS;
  af.type      = gsn_charm_person;
  af.level     = ch->pcdata->rank;
  if (success > 4)
    af.duration = -1;
  else
    af.duration  = success * 10 + 50;
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = AFF_CHARM;
  affect_to_char( victim, &af );
  gain_exp(ch, success * 10);
  return;
}
//
//“Sight from Beyond”
//crow / snake spirit
//wits + occult diff 7
//the garou becomes an oracle, she receives visions that strike without warning
void spell_gift_sightfrombeyond( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_sightfrombeyond))
  {
    sendch("Visions already periodically assail your mind.\n\r", ch);
    return;
  }

  if (ch->move < (ch->level * 5) / 4)
  {
    send_to_char("You are too exhausted to activate this Gift.\n\r", ch);
    return;
  }

  ch->move -= (ch->level * 5) / 4;
  success = godice(get_attribute(ch, WITS) + get_ability(ch, CSABIL_OCCULT), 7);

  if (success < 0)
  {
    send_to_char("An explosion of visions assaults your mind, wreaking havoc.\n\r", ch);
    d10_damage( ch, ch, 5, ch->level, gsn_telepathy, DAM_MENTAL, DEFENSE_NONE, TRUE, TRUE);
    WAIT_STATE(ch, 6);
    return;
  }

  if (success == 0)
  {
    send_to_char("Neither crow- nor snake-spirits answer your request for precognizance.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  sendch("A large group of spirits, a murder of crows and a den of snakes, imparts knowledge upon you.\n\r", ch);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_sightfrombeyond;
  af.level        = success;
  af.duration     = success * 10 + 20;
  af.modifier     = 0;
  af.location     = APPLY_NONE;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  return;
}
//
//Rank Three
//
//“Expel Spirit”
//any incarna avatar
//man + intimidation (diff spirit wp) must concentrate for 3 turns
//the garou forces a spirit to go from place to place, to get it out of a fetish diff is 9
void spell_gift_expelspirit( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  char arg1[MIL];
  OBJ_DATA *obj;
  gift_target_name = one_argument(gift_target_name, arg1);
  AFFECT_DATA *paf;
  char buf[MAX_STRING_LENGTH];
  int success;

  if ((obj =  get_obj_carry(ch, arg1, ch)) == NULL)
  {
    sendch("You aren't carrying that item.\n\r", ch);
    return;
  }

  if (paf = affect_find(obj->affected,skill_lookup("fetish")) == NULL)
  {
    sprintf(buf, "There are no {MSpirits{x inhabiting %s.", obj->short_descr);
    send_to_char(buf, ch);
    return;
  }

  if (obj->level > ch->level)
  {
    send_to_char("This object is too powerful to use this Gift on.\n\r", ch);
    return;
  }

  if (ch->move < (ch->level * 5) / 4)
  {
    send_to_char("You are too exhausted to activate this Gift.\n\r", ch);
    return;
  }

  if (ch->cswillpower <= 1)
  {
    send_to_char("You do not have the strength of Willpower to expel the Spirit.\n\r", ch);
    return;
  }

  ch->move -= (ch->level * 5) / 4;
  success = godice(get_attribute(ch, WITS) + get_ability(ch, CSBACK_RITUALS), 7);

  if (success < 0)
  {
    send_to_char("You focus your intent to expel the Spirit, but it stubbornly clings on.\n\r", ch);
    ch->cswillpower--;
    send_to_char("The struggle taxes your Willpower.\n\r", ch);
    WAIT_STATE(ch, 9);
    return;
  }

  if (success == 0)
  {
    send_to_char("Try as you might, the Spirit is refusing to budge.\n\r", ch);
    WAIT_STATE(ch, 3);
    return;
  }

  send_to_char("With help from an Incarna, you force the Spirit out forcefully!\n\r", ch);
  affect_strip_obj(obj, gsn_fetish);

  return;
}
//
//“Pulse of the Invisible”
//any spirit
//if perm gnosis => gauntlet rating then can see into umbra without problems, if not must roll gnosis diff gauntlet rating
//
void spell_gift_pulseoftheinvisible( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
  int success;

  if (ch->move <= (ch->level * 2) + 10)
  {
    send_to_char("You are too tired to view into the Umbra.\n\r", ch);
  }

  if ( is_affected( ch, gsn_gift_pulseoftheinvisible ) )
  {
    send_to_char("You shift your sight back to the mundane.\n\r",ch);
    affect_strip(ch,gsn_gift_pulseoftheinvisible);
    return;
  }
  else
    send_to_char("You open your mind to seeing the unseen.\n\r",ch);

  success = godice(ch->pcdata->gnosis[PERM], get_gauntlet(ch));
  if (success < 1)
    success = 1;

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_pulseoftheinvisible;
  af.level     = success;
  af.duration  = (success * 5) + 10;
  af.modifier  = 0;
  af.location  = APPLY_NONE;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  act("$n's eyes turn a pale, unearthly white for a moment then fade back to normal.",ch,NULL,NULL,TO_ROOM);
  return;
}
//Rank Four
//
//“Grasp the Beyond”
//wp burning depending on size of item (1 small ‘knife’, 2 medium ‘rucksack’, 3 large ‘a person, nothing larger than can be held in two hands’)
//gnosis diff gauntlet rating to step sideways, if successful the garou bring that item/person with them, if person wants to resist then is wp diff 7 (each success from victim takes away one success from gauntlet roll, must have 3 successes minimum to ‘force’ someone through)
void spell_gift_graspthebeyond( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Spirit Wassail”
//rat spirit
//resisted gnosis roll
//drains power from a spirit, if successful the spirit loses one essense point per success, for every 2 points drained the char gains a temp wp pt, extra wp is lost at end of scene
void spell_gift_spiritwassail( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Five
//
//“Nightmare Possession”
//2 gnosis = one pt intellegence destroyed
//Wyldling spirit
//wits + empathy diff target’ wp + 3 max 10
//the garou called up a wyld spirit and binds it to its target which causes intelligence to be lost, no memories, no intellect
void spell_gift_nightmarepossession( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Malleable Spirit”
//Chimerling
//resisted gnosis roll against spirit diff (6 for wp/rage/gnosis - 1 pt gnosis changed per success of spirit) (8 for disposition [friendly, neutral, hostile] or element type [water, fire, earth, etc]) (10 for type, [Naturae, Elemental, Bane, etc]
//changes spirit’s innate nature
void spell_gift_malleablespirit( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Philodox
//
//Rank One
//
//“Resist Pain”
//1 wp
//badger
//Ignore health level penalties until incapacitated
//

void spell_gift_resistpain( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if(is_affected(ch, gsn_gift_resistpain))
    {
        send_to_char("You don't have the strength of will to resist your wounds again so soon.\n\r",ch);
        return;
    }
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_resistpain;
    af.level        = level;
    af.duration     = 24;
    af.modifier     = 0;
    af.location     = APPLY_NONE;
    af.bitvector    = 0;
    affect_to_char(ch, &af);


    ch->hit += level*5;
    if(ch->hit > ch->max_hit) ch-> hit = ch->max_hit;

    send_to_char("You snarl and growl to throw yourself into such a fury that you no longer feel the pain of your wounds.\n\r",ch);
    act("$n snarls and growls a moment, then seems to calm as their wounds no longer trouble them.",ch,NULL,NULL,TO_ROOM);
    return;
}

//“Scent of the True Form”
//vulture
//can smell kinfolk or fellow werewolf immediatly, others roll per + primal-urge diff 6
//1 success = humans, 2 successes = vampire, faerie, other shapeshifter, 4 = mage / fomor
void spell_gift_scentofthetrueform( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int successes;

  if(is_affected(ch, gsn_gift_scentofthetrueform))
  {
    send_to_char("Your olfactary senses already sniff out beings' true forms.\n\r",ch);
    return;
  }

  if(ch->move <= ch->level * 2)
  {
    send_to_char("You are too tired to sniff out beings' true forms.\n\r",ch);
    return;
  }

  ch->move -= ch->level * 2;
  successes = godice(get_attribute(ch, PERCEPTION) + ch->pcdata->primal_urge, 6);

  if(successes < 0)
  {
    send_to_char("Your nose becomes overloaded with scents, briefly disorienting you.\n\r",ch);
    WAIT_STATE(ch, 9);
    return;
  }

  if(successes == 0)
  {
    send_to_char("The spirits do not seem to have answered your request.\n\r",ch);
    return;
  }

  send_to_char("The spirits allow you to sniff out enemies' true selves.\n\r",ch);

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_scentofthetrueform;
  af.level        = successes;
  af.duration     = successes * 3 + 5;
  af.modifier     = 0;
  af.location     = APPLY_NONE;
  af.bitvector    = 0;
  affect_to_char(ch, &af);
  return;
}
//
//“Truth of Gaia”
//gaffling of falcon
//int + empathy diff subjects man + subterfuge
//reveals which words are true and which are false
void spell_gift_truthofgaia( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//Rank Two
//“Call of Duty”
//incarna avatar
//2 gnosis pts spent
//char + leadership diff spirit’s wp
//summon any spirit to you and command it to do one thing
//
void spell_gift_callofduty( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Strength of Purpose”
//wolf spirit
//stamina + rituals diff 7 every 2 successes the char recovers 1 wp pt up to his max, useable once per scene
//
void spell_gift_strengthofpurpose( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int successes;

  if(is_affected(ch, gsn_gift_strengthofpurpose))
  {
    send_to_char("You have already requested aid from the wolf spirits recently.\n\r",ch);
    return;
  }

  if (ch->cswillpower == ch->csmax_willpower)
  {
    send_to_char("Your Willpower is already at maximum, even wolf-spirits cannot give you more.\n\r", ch);
    return;
  }

  if (ch->move <= ch->level * 3)
  {
    send_to_char("You are much too tired to call upon the strength of wolf-spirits.\n\r", ch);
    return;
  }

  ch->move -= ch->level * 3;
  successes = godice(get_attribute(ch, CSATTRIB_STAMINA) + get_ability(ch, CSABIL_RITUALS), 7);
  send_to_char("Calling upon aid from wolf-spirits, you howl loudly.\n\r", ch);
  act("$n lets out a loud howl.", ch, NULL, NULL, TO_ROOM);

  if (successes < 0)
    successes = 0;

  ch->cswillpower += successes / 2;
  if (successes > ch->csmax_willpower)
    ch->cswillpower = ch->csmax_willpower;

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_strengthofpurpose;
  af.level        = successes;
  af.duration     = 40;
  af.modifier     = 0;
  af.location     = APPLY_NONE;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  return;
}
//Rank Three
//
//“Spirit Witness”
//tortoise spirit
//find spirit first then char + investigation diff 7 (tree/rock spirit unmmobile) diff 8 (roaming freely) diff 9 (past year) diff 10 (past century or more)   -1 to diff if spirit is brought to site, non-gaia spirits can resist wp diff garou’s wp .. spirit’s successes take away garou’s succeses
//a way to study/view something in the past
//
void spell_gift_spiritwitness( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Warrior’s Eye”
//wind-spirits
//full turn - per + brawl or melee diff 8
//each success gives the player one bonus die to add to attack or damage rolls against their opponent, can split the dice ie 4 successes gives you 3 to attack and 1 to damage, cannot change where these dice are set and only useable once per scene
//ability to exploit weakness of opponent
//
void spell_gift_warriorseye( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  OBJ_DATA *wield;
  int successes, weapondice;

  if(is_affected(ch, gsn_gift_warriorseye))
  {
    send_to_char("Wind-spirits already help your attacks strike true.\n\r",ch);
    return;
  }

  if (ch->move <= ch->level * 2)
  {
    send_to_char("You are too tired to call out to wind-spirits.\n\r", ch);
    return;
  }

  if (wield = get_eq_char( ch, WEAR_WIELD ) == NULL)
    weapondice = get_ability(ch, CSABIL_BRAWL);
  else
    weapondice = get_ability(ch, CSABIL_MELEE);

  ch->move -= ch->level * 2;
  successes = godice(get_attribute(ch, CSATTRIB_PERCEPTION) + weapondice, 7);

  if(successes < 0)
  {
    send_to_char("Wind-spirits are offended at your request, buffeting you incessantly.\n\r",ch);
    WAIT_STATE(ch, 9);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_warriorseye;
    af.level        = -1;
    af.duration     = -successes;
    af.modifier     = -ch->level * 2;
    af.location     = APPLY_HITROLL;
    af.bitvector    = 0;
    affect_to_char(ch, &af);

    af.location     = APPLY_DAMROLL;
    affect_to_char(ch, &af);
    return;
  }

  if(successes == 0)
  {
    send_to_char("The spirits of the wind whisper past, and then are gone.\n\r",ch);
    return;
  }

  send_to_char("Wind-spirits guide your attacks to strike true.\n\r",ch);

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_warriorseye;
  af.level        = successes;
  af.duration     = successes * 2 + 5;
  af.modifier     = ch->level * 2;
  af.location     = APPLY_HITROLL;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  af.location     = APPLY_DAMROLL;
  af.modifier     = ch->level;
  affect_to_char(ch, &af);
  return;
}
//Rank Four
//
//“Roll Over”
//wolf spirit
//extended wp contest, when a player has scored three or more successes than an opponent that opponent loses, losers will not take ANY action without approval of the victor
//
void spell_gift_rollover( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Scent of Beyond”
//bird spirit
//per + enigmas diff 8
//the wolf can send her senses to any place with which she is familiar, umbra diff is 8 or gauntlet rating whichever is HIGHER, body is left senseless
//
void spell_gift_scentofbeyond( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank Five
//
//“Earthfriend’s Pact”
//1 pt gnosis
//earth elemental
//char + expression diff 8
//earthquake (must make dex + athletics diff 7 roll to or fall to the ground and suffer five levels of bashing damage) THEN those who are left standing make a dex + dodge diff 6 roll fo avoid falling into a hole in the earth, if they fail they get 10 levels of bashing damage as the crack in the earth closes on them, survivors must claw their way out or die
//if 4 or more successes are given on initial roll then extra above 4 can be used to extend the tremors or increase the violence one per round
//
void spell_gift_earthfriendspact( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Geas”
//1 gnosis pt
//incarna avatar
//man + leadership diff opponent’s wp (if a group use highest wp)
//compulsion on a person or group to pledge an oath, cannot act against the task given, will not kill themselves with it (like you can’t dominate someone to stab themselves) lasts until the task is finished or target is incapacitated too much to be able to continue on the quest
void spell_gift_geas( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Galliard
//
//Rank One
//
//“Beast Speech”
//any animal spirit
//charisma + animal ken
//allows you to speak with animals, not influence them
void spell_gift_beastspeech( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if(is_affected(ch, gsn_gift_beastspeech))
  {
    send_to_char("Animal-spirits already help you converse with beasts.\n\r",ch);
    return;
  }

  if (ch->move <= ch->level * 2)
  {
    send_to_char("You are too tired to ask animal-spirits for aid.\n\r", ch);
    return;
  }

  ch->move -= ch->level * 2;
  success = godice(get_attribute(ch, CSATTRIB_CHARISMA) + get_ability(ch, CSABIL_ANIMAL_KEN), 6);

  if (success < 0)
  {
    act("You spend an extended amount of time waiting for animal-spirits to aid, but to no avail.", ch, NULL, NULL, TO_CHAR);
    WAIT_STATE(ch, 12);
    return;
  }

  if (success == 0)
  {
    act("The animal-spirits seem to ignore your request.", ch, NULL, NULL, TO_CHAR);
    return;
  }

  act("Adhering to your call, animal-spirits help you communicate with natural creatures.", ch, NULL, NULL, TO_CHAR);
  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_beastspeech;
  af.level     = success;
  af.duration  = 25 + (success * 10);
  af.location  = 0;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  return;
}
//
//“Call of the Wyld”
//wolf spirit
//stam + empathy (successes determines how far away the call is heard)
//various results - bonus die to pool per 2 successes for the howl/pt of rage or wp given to those who have zero/etc
//

void spell_gift_callofthewyld( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
  CHAR_DATA *gch;
  int success;

  if (is_affected(ch, gsn_laryngitis))
  {
    act( "You attempt a savage howl, but your enflamed throat causes it to come out broken and disjointed.", ch, NULL, NULL, TO_CHAR );
    return;
  }

  if (ch->move <= (ch->level * 2) + 50)
  {
    send_to_char("You are too tired to call spirits of aid with your howl.\n\r", ch);
    return;
  }

  ch->move -= (ch->level * 2) + 50;
  success = godice(get_attribute(ch, CSATTRIB_STAMINA) + get_ability(ch, CSABIL_EMPATHY), 6);

  if (success < 0)
  {
    act( "You attempt a savage howl of rage and praise, but stop as your voice strains.", ch, NULL, NULL, TO_CHAR );
    act( "$n begins to howl, but $s voice cracks.", ch, NULL, NULL, TO_ROOM );
    af.where     = TO_AFFECTS;
    af.type      = gsn_laryngitis;
    af.level     = level;
    af.duration  = 3;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( gch, &af );
    return;
  }

  if (success == 0)
  {
    act( "You let out a loud howl, but no spirits come to aid.", ch, NULL, NULL, TO_CHAR );
    act( "$n begins a loud howl, lasting for a good minute.", ch, NULL, NULL, TO_ROOM );
    WAIT_STATE(ch, 9);
    return;
  }

  act( "You let forth a savage howl of rage and praise to Luna.", ch, NULL, NULL, TO_CHAR );
  act( "$n lets forth a savage howl.", ch, NULL, NULL, TO_ROOM );

  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
  {
    if ( !is_same_group( gch, ch ) || is_affected(gch, gsn_gift_callofthewyld) || gch->race != race_lookup("garou") )
      continue;
    if(gch != ch)
    {
      act( "$N raises $S head and joins $n in a savage howl!", ch, NULL, gch, TO_NOTVICT );
      act( "$N raises $S head and joins you in your savage howl!", ch, NULL, gch, TO_CHAR );
      act( "You feel invigorated by $n's howl and join in the praise to Luna.", ch, NULL, gch, TO_VICT );
    }
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_callofthewyld;
    af.level     = success;
    af.duration  = 10 + (success * 5);
    af.location  = APPLY_CS_STR;
    af.modifier  = 1;
    af.bitvector = 0;
    affect_to_char( gch, &af );

    if (success > 3)
    {
      af.location  = APPLY_CS_STA;
      affect_to_char( gch, &af );
    }
  }

    return;
}
//“Waking Dream”
//1 wp
//Chimerling
//manipulation + empathy diff target’s wp (roll only required if target is unwilling)
//can enter the dream and interact/battle with no harm done; if want to interact with their immobile body then +2 penalty to dice pools
void spell_gift_wakingdream( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Two
//
//“Call of the Wyrm”
//spirit servant of gaia spirit
//resisted Manipulation + performance diff 7 against wyrm creature’s wp roll diff 7
//if wyrm loses it must come to the person who calls it
void spell_gift_callofthewyrm( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Distractions”
//wolf spirit
//wits + performance diff target wp
//each success takes one from target’s dice pool on next turn
//
void spell_gift_distractions( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int success, diff = 6;

  if (ch-> move < ch->level * 2)
  {
    send_to_char("You are too tired to call on wolf-spirits for assistance.\n\r", ch);
    return;
  }

  if (is_affected(victim, gsn_gift_distractions))
  {
    send_to_char("Your target is already bombarded by howls and yips.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_laryngitis))
  {
    send_to_char("Your throat is too sore to properly yip and howl.\n\r", ch);
    return;
  }

  if (victim->level < ch->level - 10)
    diff--;
  if (victim->level > ch->level + 10)
    diff++;
  if (IS_SET(victim->act2, ACT2_ULTRA_MOB))
    diff++;

  ch->move -= ch->level * 2;
  success = godice(get_attribute(ch, CSATTRIB_WITS) + get_ability(ch, CSABIL_PERFORMANCE), diff);

  if (success < 0)
  {
    send_to_char("Wolf-spirits answer, but are angered at your request.  They constantly harrass you.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_distractions;
    af.level     = -1;
    af.duration  = 2;
    af.location  = APPLY_HITROLL;
    af.modifier  = -success * 200;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  if (success == 0)
  {
    send_to_char("The wolf-spirits do not recognize you as alpha, and refuse to aid.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  act("You request a pack of wolf-spirits to harrass $N!", ch, NULL, victim, TO_CHAR);
  act("Wolf howls and yips assault your eardrums!", ch, NULL, victim, TO_VICT);
  act("$n tries to reach out and touch $N, but misses.", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_distractions;
  af.level     = success;
  af.duration  = 2;
  af.location  = APPLY_HITROLL;
  af.modifier  = -success * ch->level - 25;
  af.bitvector = 0;
  affect_to_char( victim, &af );
  return;
}
//Rank Three
//
//“Eye of the Asp”
//venomous snake spirit
//apperance + enigmas diff victom’s wp
//3 successes needed to bring target to the garou
void spell_gift_eyeoftheasp( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int success, diff = 6, appearance;

  if (ch-> move < ch->level * 2)
  {
    send_to_char("You are too tired to beseech venomous snake spirits.\n\r", ch);
    return;
  }

  if (is_affected(victim, gsn_gift_eyeoftheasp))
  {
    send_to_char("Your target is as close to you as possible already.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_gift_eyeoftheasp) && get_affect_level(ch, gsn_gift_eyeoftheasp) == -1)
  {
    send_to_char("Venous snake spirits are entangling your legs, ignoring your further reqests.\n\r", ch);
    return;
  }

  if (victim->level < ch->level - 10)
    diff--;
  if (victim->level > ch->level + 10)
    diff++;
  if (IS_SET(victim->act2, ACT2_ULTRA_MOB))
    diff++;
  if (is_affected(ch, gsn_change))
    appearance = 2;
  else
    appearance = get_attribute(ch, CSATTRIB_APPEARANCE);


  ch->move -= ch->level * 2;
  success = godice(appearance + get_ability(ch, CSABIL_ENIGMAS), diff);

  if (success < 0)
  {
    send_to_char("Angered at your request, the snake spirits constantly entwine your legs.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_eyeoftheasp;
    af.level     = -1;
    af.duration  = 2;
    af.location  = APPLY_CS_DEX;
    af.modifier  = -1;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  if (success == 0)
  {
    send_to_char("The venomous snake spirits slither away, ignoring your call.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  act("Snake spirits help allure $N, bringing $M in closer.", ch, NULL, victim, TO_CHAR);
  act("$n seems hypnotic and alluring, you move closer to $m.", ch, NULL, victim, TO_VICT);
  act("$N moves towards $n, as if in a trance.", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_eyeoftheasp;
  af.level     = success;
  af.duration  = 2;
  af.location  = APPLY_AC;
  af.modifier  = success * ch->level + 100;
  af.bitvector = 0;
  affect_to_char( victim, &af );
  return;
}
//
//“Song of Rage”
//wolverine spirit
//manipulation + leadership diff target’s wp
//victim flies into a rage or frenzy for one turn per success
void spell_gift_songofrage( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int success, diff = 6;

  if (is_affected(ch, gsn_laryngitis))
  {
    send_to_char("Your throat is too enflamed to produce a proper howl.\n\r", ch);
    return;
  }

  if (ch-> move < ch->level * 2)
  {
    send_to_char("You are too tired to invoke rage in another.\n\r", ch);
    return;
  }

  if (is_affected(victim, gsn_gift_songofrage) || is_affected(victim, gsn_berserk)
    || is_affected(victim, gsn_garou_frenzy) || is_affected(victim, gsn_vamp_frenzy)
    || is_affected(victim, gsn_thaumaturgy_frenzy) || IS_AFFECTED(ch, AFF_BERSERK))
  {
    send_to_char("Your target is already in a frenzy.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_gift_songofrage))
  {
    send_to_char("You are too enraged to concentrate on calling wolverine-spirits.\n\r", ch);
    return;
  }

  if (victim->level < ch->level - 10)
    diff--;
  if (victim->level > ch->level + 10)
    diff++;
  if (IS_SET(victim->act2, ACT2_ULTRA_MOB))
    diff++;

  ch->move -= ch->level * 2;
  success = godice(get_attribute(ch, CSATTRIB_MANIPULATION) + get_ability(ch, CSABIL_LEADERSHIP), diff);

  if (success < 0)
  {
    send_to_char("Wolverine-spirits don't take kindly to having their time wasted.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_songofrage;
    af.level     = -1;
    af.duration  = 2;
    af.location  = APPLY_HITROLL;
    af.modifier  = -100 - (ch->level * 2);
    af.bitvector = AFF_BERSERK;
    affect_to_char( ch, &af );
    return;
  }

  if (success == 0)
  {
    send_to_char("Your howl doesn't seem to reach the spirits as intended.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

  act("You howl a note of pure fury, stirring $N into a rage.", ch, NULL, victim, TO_CHAR);
  act("$n howls furiously, assaulting your ears.", ch, NULL, victim, TO_VICT);
  act("$N enrages at hearing $n's howl.", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_songofrage;
  af.level     = success;
  af.duration  = success + 2;
  af.location  = APPLY_HITROLL;
  af.modifier  = -success * 100 - ch->level;
  af.bitvector = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_DAMROLL;
  af.modifier  = -success * 25;
  return;
}
//
//Rank Four
//
//“Shadows by the Firelight”
//three gnosis (if unwilling victim)
//ancestor spirit
//manipulation  + performance (diff victims’ wp) only if unwilling
//puppets the target through a scene or until the actor isa ttacked
void spell_gift_shadowsbythefirelight( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Song of Mockery”
//mockingbird spirit
//wits + investigation diff 8
//helps learn their rivals secrets, must be sung before a crowd
void spell_gift_songofmockery( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Five
//
//“Dream Golem”
//chimerlings
//1 gnosis per turn to keep the false image up
//extended inte + performance diff 8 roll
//like chimerstry, you create a creature if roll is botched creature gets to eat you
void spell_gift_dreamgolems( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//“Call for Vengeance”
//ancestor spirit
//stamin + performance diff 7
//20 mile radius call, garou who hear it do nto have to join the hunt but their wp increases by 2 up to 10 for the duration, if a criminal can hear the howl her wp is reduced by 2 pts for next week… cant use this gift again until that criminal is found and brought to justice (ie target mob maybe?)
void spell_gift_callforvengeance( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Ahroun
//
//Rank One
//
//“Falling Touch”
//any flying spirit
//dex + medicine diff victim’s stamin + athletics
//sends their foe sprawling with a touch
void spell_gift_fallingtouch( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    CHAR_DATA *victim = (CHAR_DATA *) vo;
    int dicesuccess = 0;
    int diff = 0;

        if (IS_NPC(ch))
                return;

        if (ch == victim)
        {
                send_to_char("You wisely reconsider that.\n\r", ch );
                return;
        }

    if(IS_AFFECTED2(ch,AFF2_MIST) || IS_AFFECTED2(victim,AFF2_MIST))
    {
        send_to_char("Your hand passes right through!\n\r",ch);
        return;
    }

    if(victim->position == POS_SLEEPING || victim->position == POS_TORPOR)
    {
            send_to_char("They are already incapacitated!\n\r", ch );
            return;
    }

    if (ch->move < ch->level / 20)
    {
      send_to_char("You are too tired to perform this ability.\n\r", ch);
      return;
    }

    ch->move -= ch->level / 20;
    diff = get_attribute(victim, STAMINA) + get_ability(victim, CSABIL_ATHLETICS);
    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_MEDICINE], diff);

    WAIT_STATE(ch, 3);

    if(dicesuccess <= 0)
    {
        act("You miss $N!", ch, NULL, victim, TO_CHAR);
        act("$n tries to reach out and touch you and misses!", ch, NULL, victim, TO_VICT);
        act("$n tries to reach out and touch $N, but misses.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, 6);
        return;
    }

    act( "You reach out and touch $N, sending $M sprawling!", ch, NULL, victim, TO_CHAR );
    act( "$n reaches out and touches you, and you are sent sprawling!", ch, NULL, victim, TO_VICT );
    act( "$n reaches out and touches $N, sending $M sprawling!", ch, NULL, victim, TO_ROOM );

    STOPPED(victim, UMAX(2, dicesuccess) * PULSE_VIOLENCE);
    damage(ch, victim, dicesuccess*UMAX(5, ch->level * 3 / 2), gsn_gift_fallingtouch, DAM_BASH, TRUE);
    victim->position = POS_RESTING;
    gain_exp(ch, dicesuccess*2);

    return;
}

//
//“Inspiration”
//1 gnosis
//lion or wolf spirit
//no roll
//any allies who can see the ahroun receive one auto success on all wp rolls made during the scene (not the ahroun who did the gift gets it)
//
void spell_gift_inspiration( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    AFFECT_DATA af;
    CHAR_DATA *gch;
    int glory;

        if (ch->pcdata->gnosis[TEMP] < 1) {
        send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        return;
    }

    ch->pcdata->gnosis[TEMP]--;
    act( "Your prowess as a True Warrior shines through and inspires your pack to greatness!", ch, NULL, NULL, TO_CHAR );

    glory = ch->pcdata->renown[GLORY];
    if (glory < 2)
        glory = 2;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( gch == ch || is_affected(gch, gsn_gift_inspiration) || gch->race != race_lookup("garou") )
            continue;
            act( "$N catches sight of you and seems revitalized and inspired!", ch, NULL, gch, TO_CHAR );
            act( "$N catches sight of $n and seems revitalized and inspired!", ch, NULL, gch, TO_NOTVICT );
            act( "The sight of $n strengthens your resolve and fills you with righteous fury!", ch, NULL, gch, TO_VICT );

            if (gch->pcdata->rage[TEMP] < gch->pcdata->rage[PERM])
                gch->pcdata->rage[TEMP] = gch->pcdata->rage[PERM];

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_inspiration;
        af.level     = glory;
        af.duration  = 5 + glory/2;
        af.location  = APPLY_CS_STR;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_inspiration;
        af.level     = glory;
        af.duration  = 5 + glory/2;
        af.location  = APPLY_CS_STA;
        af.modifier  = 2;
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_inspiration;
        af.level     = glory;
        af.duration  = 5 + glory/2;
        af.location  = APPLY_AC;
        af.modifier  = -(glory*20);
        af.bitvector = 0;
        affect_to_char( gch, &af );
    }

    return;
}

//“Razor Claws”
//one rage pt
//cat spirit
//no roll
//claws inflict one more die of damage
//

void spell_gift_razorclaws( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *environment;
    environment = ch->in_room;
    int rocks = environment->sector_type;
    int diff = 7;
    int successes = 0;

    if ( ch->pcdata->shiftform < GLABRO)
    {
        send_to_char( "You need to have claws in order to sharpen them.\n\r", ch );
        return;
    }

    if (is_affected(ch, gsn_gift_razorclaws) || is_affected(ch,gsn_gift_silverclaws))
    {
        send_to_char("You can't seem to sharpen your claws further.\n\r",ch);
        return;
    }

    if (ch->pcdata->rage[TEMP] < 1)
    {
      send_to_char("You do not have enough Rage to activate this Gift.\n\r",ch);
      return;
    }

    if (rocks == SECT_INSIDE || rocks == SECT_WATER_SWIM || rocks == SECT_WATER_NOSWIM ||
    rocks == SECT_UNUSED || rocks == SECT_AIR || rocks == SECT_WATER_DROWN ||
    rocks == SECT_HOT || rocks == SECT_COLD || rocks == SECT_NODE)
    {
      send_to_char("There are no rocks nearby to sharpen your claws.", ch);
      return;
    }

    if (rocks == SECT_CITY || rocks == SECT_DESERT)
      diff++;

    if (rocks == SECT_MOUNTAIN)
      diff -= 2;

    ch->pcdata->rage[TEMP]--;

    successes = godice(get_attribute(ch, PERCEPTION) + get_ability(ch, CSABIL_SURVIVAL), diff);
    if (successes < 0)
      successes = 0;

    af.where      = TO_AFFECTS;
    af.type       = gsn_gift_razorclaws;
    af.level      = successes;
    af.duration  = (successes * 3) + 15;
    af.location  = APPLY_DAMROLL;
    af.modifier  = (successes * 10) + 50;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    act("You scrape your claws across nearby stones and sharpen them to a razor edge.",ch,NULL,NULL,TO_CHAR);
    act("$n scrapes $s claws across nearby stones to sharpen them.",ch,NULL,NULL,TO_NOTVICT);
    return;
}
//Rank Two
//
//“Spirit of the Fray”
//1 gnosis pt (optional if want to add another 10 to roll)
//cat spirit
//no roll
//player adds 10 to all initiative rolls, cannot spend rage for extra actions when this is activated
//
void spell_gift_spiritofthefray( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//“True Fear”
//spirit of fear
//str + intimidation diff target wp
//each success cows the target for one rturn, they cannot attack but can defend
void spell_gift_truefear( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}//
//Rank Three
//
//“Heart of Fury”
//boar spirit
//wp diff victim’s perm rage rating
//every 2 successes adds one to frenzy diff, at the end of the scene the victim must spend a wp or immedialyt check for frenzy
//
void spell_gift_heartoffury( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Silver Claws”
//luna spirits
//gnosis diff 7
//turns claws to aggy SILVER damage that is unsoakable by most garou; this hurts the wielder and each turn they gain a rage pt, all non-combat diffs increased by 2 in the haze of pain, of the rage pts exceed his wp they do a check for frenzy, lasts one scene or sooner
//

void spell_gift_silverclaws( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if ( ch->pcdata->shiftform == HOMID)
    {
        send_to_char( "You do not have claws to turn to silver in your Homid form.\n\r", ch );
        return;
    }

    if (is_affected(ch,gsn_gift_razorclaws) || is_affected(ch,gsn_gift_silverclaws))
    {
        send_to_char("You cannot turn your claws to silver when they're aleady enhanced.\n\r",ch);
        return;
    }

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_silverclaws;
    af.level        = ch->level;
    af.duration     = 24;
    af.location     = APPLY_NONE;
    af.modifier     = level;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    damage( ch, ch, 10, gsn_gift_silverclaws,DAM_NEGATIVE,FALSE);
    act("You scream in agony as your claws slowly turn to silver.",ch,NULL,NULL,TO_CHAR);
    act("$n screams in agony as $s claws slowly turn to silver.",ch,NULL,NULL,TO_NOTVICT);
    return;
}
//Rank Four
//
//“Full Moon’s Light”
//1 gnosis pt
//lunes
//no roll
//anyone who is against the pack glows, can go against those who are obfuscated or have magic that hides them but only if the target is trying to harm the pack
void spell_gift_fullmoonslight( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Stoking Fury’s Furnace”
//1 rage pt
//wolvering spirit
//no roll
//rage pt spent when doing damage, this rage does not cause a frenzy check, can spend one rage pt per turn without losing temp rage, if more than one is spent per turn then all are taken off the temp rage
//
void spell_gift_stokingfurysfurnace( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank Five
//
//“Kiss of Helios”
//1 gnosis
//servant of helios spirit
//rest of the scene char is unharmed by any natural source of fire (even lava), supernatural fire inflicts 1/4th damage and is treacted as bashing; the char inflicts 2 additional dice of aggy damage if she attacks with flaming fists/claws/fangs
//
void spell_gift_kissofhelios( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Strength of Will”
//1 wp
//wolf spirit or incarna avatar
//char + leadership diff 8
//each success grants all garou allies within 100 ft (and packmates within 100 miles) an extra pt of wp, lasts the scene and can go above 10 wp, only useable once per scene
void spell_gift_strengthofwill( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Tribes
//Black Furies
//Rank One
//
//“Breath of the Wyld”
//pegasus spirit
//gnosis diff 5 against garou, 6 against anything else
//must touch the person
//grants one extra die on all mental rolls for the next scene, also adds one to the diff of rage rolls (no work on dead creatures)
//
void spell_gift_breathofthewyld( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int success, difficulty = 6;

    if (is_affected(ch, gsn_forget))
    {
      send_to_char("The details of how to properly enact this Gift seem a bit fuzzy at the moment.\n\r", ch);
      return;
    }

    if( ch->in_room->sector_type == SECT_INSIDE || ch->in_room->sector_type == SECT_CITY
    || ch->in_room->sector_type == SECT_UNUSED )
    {
      send_to_char("Gaia's influence is not strong enough here to impart her vitality.\n\r", ch);
      return;
    }

    if (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah")
    || victim->race == race_lookup("spirit") || victim->race == race_lookup("bane")
    || victim->race == race_lookup("construct"))
    {
      send_to_char("That target is not a naturally living being, and Gaia will not lend her aid.\n\r", ch);
      return;
    }

    if (is_affected(victim, gsn_gift_breathofthewyld) || is_affected(victim, gsn_empower))
    {
      if (victim == ch)
        send_to_char("Your mind has already been heightened through mystical means.\n\r", ch);
      else
        send_to_char("Your target's mind is already heightened beyond normal capacity.\n\r", ch);
      return;
    }

    if (victim->race == race_lookup("garou") || victim->race == race_lookup("fera") )
      difficulty--;

    WAIT_STATE(ch, PULSE_VIOLENCE);
    success = godice(ch->pcdata->gnosis[PERM], difficulty);

    if (success < 0)
    {
      act("As you try to enact the actions necessary for channeling Gaia's essence, you feel a sudden backlash of spiritual energy, leaving your mind in a cloudy state.", ch, NULL, victim, TO_CHAR);
      act("$n's face slacks for a minute, staring off into space and ignoring the world around $m.", ch, NULL, victim, TO_NOTVICT);

      af.where     = TO_AFFECTS;
      af.type      = gsn_forget;
      af.level     = ch->pcdata->rank;
      af.duration  = -(success);
      af.modifier  = -1;
      af.location  = APPLY_CS_WIT;
      af.bitvector = 0;
      affect_to_char( victim, &af );
      return;
    }

    if (success == 0)
    {
      act("You try to impart some of Gaia's energy, but cannot seem to get in touch with your spiritual side.", ch, NULL, victim, TO_CHAR);
      WAIT_STATE(ch, PULSE_VIOLENCE * 6);
      return;
    }

    if (ch == victim)
      act("You feel a rush of energy and clarity of thought from Gaia's blessing.", ch, NULL, victim, TO_CHAR);
    else
      act("You touch $N, imparting a rush of energy from Gaia.", ch, NULL, victim, TO_CHAR);
    act("$n places a hand upon your shoulder, and you suddenly feel a clarity of thought.", ch, NULL, victim, TO_VICT);
    act("$n places a hand upon $N's shoulder.", ch, NULL, victim, TO_NOTVICT);

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_breathofthewyld;
    af.level     = ch->pcdata->rank;
    af.duration  = (success * 5) + (ch->pcdata->rank * 2);
    af.modifier  = 1;
    af.location  = APPLY_CS_PER;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    af.location  = APPLY_CS_INT;
    affect_to_char( victim, &af );

    af.location  = APPLY_CS_WIT;
    affect_to_char( victim, &af );

    return;
}
//“Man’s Skin”
//ancestor spirit
//char + subterfuge diff 7
//illusion to make your female self look like a man for a scene or until you shift forms
//
void spell_gift_mansskin( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (ch->sex == 1 || ch->sex == 0)
  {
    send_to_char("You are not a female, and cannot use this Gift.\n\r", ch);
    return;
  }

  if ( is_affected(ch, gsn_change_sex))
  {
    send_to_char("Your sex has already been changed by another ability.\n\r", ch);
    return;
  }

  if ( is_affected(ch, gsn_gift_mansskin))
  {
    send_to_char("You shift your visage back to your feminine form.\n\r",ch);
    act("$N's visage flows back to that of a woman.",ch,NULL,NULL,TO_ROOM);
    affect_strip(ch, gsn_gift_mansskin);
    return;
  }

  success = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_SUBTERFUGE), 7);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_mansskin;
  af.level     = level;
  af.duration  = 20 + (2 * success);
  af.location  = APPLY_SEX;
  af.modifier  = -1;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  send_to_char( "You mask your feminine form with a masculine illusion.\n\r", ch);
  act("$n's form flows into a very masculine visage.",ch,NULL,NULL,TO_ROOM);
  return;
}

//“Sense Wyrm” duplicate gift
//same as Metis Gift
//
//Rank Two
//
//“Curse of Aeolus”
//fog totem
//gnosis roll diff (4 water, 6 temperate climate, 9 in desert)
//covers you and pack with fog (exactly like obfusc 5)
//
void spell_gift_curseoftheaeolus( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *gch;
  int success, diff;
  ROOM_INDEX_DATA *room;
  room = ch->in_room;

  switch (room->sector_type)
  {
    case SECT_INSIDE:
     send_to_char("You need to be outside to activate this Gift.\n\r", ch);
     return;
    case SECT_WATER_DROWN:
      send_to_char("You cannot use this Gift underwater!\n\r", ch);
      return;
    case SECT_UNUSED:
    case SECT_HOT:
    case SECT_COLD:
      send_to_char("There isn't enough moisture in the air.\n\r", ch);
      return;
    case SECT_WATER_SWIM:
    case SECT_WATER_NOSWIM:
      diff = 4;
      break;
    case SECT_DESERT:
      diff = 9;
      break;
    default:
      diff = 6;
      break;
  }

  if (ch->move < ch->level * 2)
  {
    send_to_char("You are too tired to ask for spiritual aid.\n\r", ch);
    return;
  }

  ch->move -= ch->level * 2;
  success = godice(ch->pcdata->gnosis[PERM], diff);

  if (success < 0)
  {
    act("You silently call out to the spirits of fog and mist, and feel nothing in response.", ch, NULL, NULL, TO_CHAR);
    WAIT_STATE(ch, 10);
    return;
  }

  if (success == 0)
  {
    act("Your silent request to the spirits goes unanswered.", ch, NULL, NULL, TO_CHAR);
    return;
  }

  act("You silently call out to the spirits for aid in masking your group's presence.", ch, NULL, NULL, TO_CHAR);
  act("A thick fog rolls in, seemingly out of nowhere.", ch, NULL, NULL, TO_CHAR);
  act("A thick fog rolls in, seemingly out of nowhere.", ch, NULL, NULL, TO_ROOM);

  for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
  {
    if ( is_same_group( gch, ch ) && !is_affected(gch, spell_gift_curseoftheaeolus))
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_gift_curseoftheaeolus;
      af.level     = ch->pcdata->rank;
      af.duration  = (success * 2) + 10;
      af.modifier  = 0;
      af.location  = APPLY_NONE;
      af.bitvector = AFF_HIDE;
      affect_to_char( gch, &af );
    }
    else if (!is_same_group( gch, ch ) && !is_affected(gch, spell_gift_curseoftheaeolus))
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_gift_curseoftheaeolus;
      af.level     = -1;
      af.duration  = (success * 2) + 10;
      af.modifier  = -2;
      af.location  = APPLY_CS_PER;
      af.bitvector = 0;
      affect_to_char( gch, &af );
    }
  }

  return;
}
//“Sense of the Prey” duplicate gift
//same as ragabash gift
//
//Rank Three
//
//“Coup de Grace”
//1 wp
//owl
//Perception + brawl diff 6 / target rolls per + subterfuge diff 6
//if player rolls more successes than target the player doubled the damage dice rolled on next successful attack on target
//
void spell_gift_coupdegrace( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Flames of Hestia”
//1 gnosis
//hestia (purity/healing) spirit
//roll gnosis dif 7
//each success removes taint from a barrel’s worth of water or enough food to fill a trencher; each success heals one health level of damage inflicted by disease or poison; if touching a bane with this then flames cause one health level of aggy damage per success
//
void spell_gift_flamesofhestia( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank Four
//
//“Body Wrack”
//1 gnosis
//pain spirit
//rolls gnosis diff target’s stam + 3
//target is left stunned and convulsing for one round per success, also subtracts one die per success from all attribute rolls for the rest of the scene
//
void spell_gift_bodywrack( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Bow of Pegasus”
//1 gnosis
//pegasus
//roll gnosis diff 6
//success allows the character to shoot arrows that travel instantly to any target in the range of visibility, target cannot dodg attack and diff to hit the target drops by 4 while gift activated, lasts one turn per success
void spell_gift_bowofpegasus( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Five
//
//“Gaze of the Gorgon”
//gorgon spirit
//per + occult diff target’s wp
//successes = how many rounds the target is turned to stone
//
void spell_gift_gazeofthegorgon( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//“Wyld Warp”
//1 gnosis 1 rage
//wyldling
//wits + enigmas diff local gauntlet
//summons wyldlings that helps the player, examples - fly into a frenzy to harm enemies; increase all rage in garou around them, etc is unpredictable
void spell_gift_wyldwarp( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Bone Gnawers
//Rank One
//
//“Cookery”
//raccoon
//wits + survival diff 6 for inedible not poisonous ingrediants to 10 for hot cinders or rusted iron
//must have pot and ladle or spoon and water
//takes inedible stuff and makes it into an edible stew
void spell_gift_cookery( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Resist Toxin”
//rat
//stamin + survival diff 6
//successes add 3 dice to chars stamina for the purpose of resisting poisons that are supernatural in nature, lasts one scene
void spell_gift_resisttoxin( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Scent of the Honeycomb”
//1 gnosis
//air elemental
//wits + subterfuge diff 6
//choose a target and a swarm of biting insects will cover them, bite them, annoy them
void spell_gift_scentofthehoneycomb( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int success;

  if ( victim == NULL )
  {
      send_to_char( "Whom are you trying to assault?\n\r", ch );
      return;
  }

  if(victim == ch)
  {
      send_to_char( "You shouldn't summon the insects to yourself.\n\r", ch );
      return;
  }

  if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
  {
      send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
      return;
  }

  if(is_affected(victim,gsn_gift_scentofthehoneycomb))
  {
      send_to_char("Your target has already been assaulted by insects.\n\r",ch);
      return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You do not have enough Gnosis to activate this Gift.\n\r",ch);
    return;
  }

  ch->pcdata->gnosis[TEMP]--;

  success = godice(get_attribute(ch, WITS) + get_ability(ch, CSABIL_SUBTERFUGE), 6);

  if (success < 0)
  {
    act("You begin to give off a sickly sweet odor, and are suddenly plagued by vermin!", ch, NULL, victim, TO_CHAR);
    act("$n is suddenly beseiged by a horde of insects, rodents, and other vermin.", ch, NULL, NULL, TO_NOTVICT);

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_scentofthehoneycomb;
    af.level     = -success;
    af.duration  = 2;
    af.modifier  = -1;
    af.location  = APPLY_CS_DEX;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_CHA;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_MAN;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_INT;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_PER;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_WIT;
    affect_to_char( ch, &af );
    return;
  }

  if (success == 0)
  {
    act("Your attempt to attract vermin towards $N appears to have failed.", ch, NULL, victim, TO_CHAR);
    return;
  }

  act("You direct your attention towards $N, who is suddenly plagued by vermin!", ch, NULL, victim, TO_CHAR);
  act("You begin to give off a sickly sweet odor, and are suddenly plagued by vermin!", ch, NULL, victim, TO_VICT);
  act("$n is suddenly beseiged by a horde of insects, rodents, and other vermin.", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_scentofthehoneycomb;
  af.level     = success;
  af.duration  = (success * 5) + 5;
  af.modifier  = -1;
  af.location  = APPLY_CS_DEX;
  af.bitvector = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_CHA;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_MAN;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_INT;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_PER;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_WIT;
  affect_to_char( victim, &af );
    return;
}
//
//Rank Two
//
//“Cornered Rat”
//1 rage pt
//rat or badger
//rage diff 8
//forces their rage to go into frenzy
void spell_gift_corneredrat( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Plague Visage”
//rat
//manipul + medicine diff 6
//illusion lasts one scene, makes them look like  a leper
//humans have to roll humanity diff 8 or risk running way from them, supernaturals diff 6
void spell_gift_plaguevisage( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Three
//
//“Call the Rust”
//1 gnosis
//water elemental
//wits + crafts diff dependant ont eh amoutn of metal being affected
//each success allows the werewolf to coorode one ferrous metal object into uselessness, detailed work will count as 2 successes, does not have to touch it, only see it
void spell_gift_calltherust( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Friend in Need”
//1 wp
//dog
//wp diff 7
//lends a packmate rage, wp, health levels or any gift the char knows for one scene (cannot lend gifts that are of a higher rank than the recipiant, cannot lend abilities or attributes either)
void spell_gift_friendinneed( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Four
//
//“Blink”
//1 gnosis
//rat
//man + steatlh diff 6
//can duck into shadow and pop out somewhere else up to 20 yards away per success, cannot pop out of thin air, must be able to pop out of a ‘logical’ hiding place
void spell_gift_blink( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Claws of the Glassblower”
//1 gnosis
//sand or lightening spirit
//no roll
//if hit target with claws, claws break off with shards of glass in teh wound; glass causes an additional aggy health level of damage as long as it remains within teh flesh, removal takes a whole turn and has to focus on only taking it out
void spell_gift_clawsoftheglassblower( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Five
//
//“Maw of the Whale”
//catfish
//stamina + primal-urge diff 8
//can resist being swalloed by dex + dodge roll
//the werewolf can open their jaws wider to be able to swallow objects or beings whole, it ‘stores’ them in the umbra, can store them for up to hours equal to his stamina before it automatically vomits them back up, UNHARMED, nto the physical world...if object is large like a horse or person, the successes needed is one extra per every health level (so 8 for humans)
void spell_gift_mawofthewhale( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Survivor”
//1 gnosis
//tortoise
//stamin + survival diff 7
//does not have to eat/drink/sleep for the duration of teh gifts affects; gains 3 pts to stamina (can go over 10), natural disease and poisons ahve no affect and those of supernatural variety half damage
void spell_gift_survivor( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Children of Gaia
//Rank One
//
//“Eve’s Blessing”
//1 gnosis
//cow
//putting their hands on a pregnant mother’s belly gives tit and child are given an extra dog in stamina each for the purposes of surviving childbirth/carrying to term/resisting illness and infection, lasts for a week, the garou auto learns how many children are going to be born and fi used on a wolf it affects all cubs
void spell_gift_evesblessing( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Mother’s Touch” duplicate gift of theurge
//
//“Resist Pain” duplicate gift of philodox
//
//Rank Two
//
//“Grandmother’s Touch”
//same as Mother’s touch but can heal yourself as well
void spell_gift_grandmotherstouch( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Luna’s Armor”
//1 gnosis
//lune
//stamin + survival diff 6
//each success allows the garou to add one die of stamina for purposes of soaking damage including silver but only lets you soak with teh successes gotten by the roll, does NOT include yoru current stam to soak silver
void spell_gift_lunasarmor( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//Rank Three
//
//“Dazzle”
//unicorn
//charisma + empathy diff target’s wp
//is like awe, can do nothing but go ooo shiny for the remainder of the scene
void spell_gift_dazzle( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Spirit Friend”
//unicorn
//charisma + expression diff 7
//each success adds one dice to ALL garou’s dice pools for interacting with spirits (not banes) lasts one scene
void spell_gift_spiritfriend( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Four
//
//“Angel’s Semblance”
//1 gnosis
//hope spirit
//no roll
//like passion mixed with dreadgaze but in reverse, instead of fear and panic it is awe and bliss and yeah rawr we can do this!!! of delierium responses on the target
void spell_gift_angelssemblance( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Beast Life” duplicate of the Lupus gift
//
//Rank Five
//
//“Halo of the Sun”
//1 gnosis
//sunbeam spirit
//no roll
//receive 2 extra dice to damage of any hand to hand attacks and causes aggy damage when in homid or glabro forms, anyone facing garou directly adds three to attack diffs because of glare of the halo and vampires within 20 yards suffer damage as if exposed to true sunlight (3 health levels per turn)
void spell_gift_haloofthesun( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“The Living Wood”
//1 gnosis
//glade child spirit
//charisma + survival duff 8
//the character animates one tree for each success rolled, trees will block/restrain/even fight for teh garou
void spell_gift_thelivingwood( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//

// Fenrir
//Rank 1

// Hunters Harmony
// charisma + primal-urge diff 6
// Allows the garou to convey complex battle concepts to pack members.
// buff that boosts pack bonus.
void spell_gift_huntersharmony( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Resist Pain - Duplicate gift, as the Philodox gift

// Visage of Fenris
// charisma + intimidation diff 6
// Foes flinch and lose a point of initiative.
// Majesty-like effect that causes mobs to attack someone else first.
void spell_gift_visageoffenris( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *rch;
  AFFECT_DATA af;
  int successes;

  if (is_affected(ch, gsn_gift_visageoffenris))
  {
    if (get_affect_level(ch, gsn_gift_visageoffenris) == 0)
    {
      act("Fenris has not yet forgiven your last pathetic attempt to cow others in his name.", ch, NULL, NULL, TO_CHAR);
      return;
    }

    affect_strip(ch, gsn_gift_visageoffenris);
    act("You release Fenris's gift, gaining your regular countenance.", ch, NULL, NULL, TO_CHAR);
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("Your do not have enough Willpower to be worthy of this mighty gift from Fenris.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  successes = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_INTIMIDATION), 6);

  if (successes < 0)
  {
    act("You attempt to command respect and fear from those around you, but everyone only seems to laugh.", ch, NULL, rch, TO_CHAR);
    act("$n tries to appear more intimidating, but the affect only appears comical.", ch, NULL, rch, TO_NOTVICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_visageoffenris;
    af.level     = 0;
    af.duration  = 2;
    af.modifier  = -1;
    af.location  = APPLY_CS_CHA;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    af.location  = APPLY_CS_MAN;
    affect_to_char( ch, &af );
    return;
  }

  if (successes == 0)
  {
    act("You attempt to command respect and fear from those around you, but nobody seems impressed.", ch, NULL, rch, TO_CHAR);
    act("$n tries to appear more intimidating, but you aren't impressed in the slightest.", ch, NULL, rch, TO_NOTVICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_visageoffenris;
    af.level     = 0;
    af.duration  = 1;
    af.modifier  = 0;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
  {
    if ( rch->fighting != NULL && (rch->level <= ch->level + 10 + successes)
      && (IS_NPC(rch) && !IS_SET(rch->act2, ACT2_ULTRA_MOB)) && !IS_SET(rch->imm_flags,IMM_CHARM))
      stop_fighting( rch, TRUE );
    if (rch == ch)
      act("Using your Gaia bestowed gift, you command respect and fear from those around you in the name of Fenris.", ch, NULL, rch, TO_CHAR);
    else
      act("With a stern glare, $n appears much more fearsome, cowing you into brief submission and respect.", ch, NULL, rch, TO_VICT);
  }

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_visageoffenris;
  af.level     = ch->level + successes;
  af.duration  = successes + 3;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  gain_exp(ch, successes * 5);

  return;
}
// Rank 2

//Halt the Cowards Flight
// charisma + intimidation diff target wp
// Stop mobs from fleeing, possibly slow them down in combat
void spell_gift_haltthecowardsflight( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
   CHAR_DATA *victim = (CHAR_DATA *) vo;
   AFFECT_DATA af;
   int successes;
   int diff = 6;

    if ( victim == NULL )
    {
        send_to_char( "Whom are you trying to halt?\n\r", ch );
        return;
    }

    if(victim == ch)
    {
        send_to_char( "You cannot halt yourself!\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(is_affected(victim,gsn_gift_haltthecowardsflight))
    {
        send_to_char("Your target has already been halted.\n\r",ch);
        return;
    }

    if(ch->move < ch->level)
    {
      send_to_char("You are too tired to focus on your target.\n\r", ch);
      return;
    }

    ch->move -= ch->level;

    if(is_affected(victim, gsn_vamp_frenzy) || is_affected(victim, gsn_rage) || is_affected(victim, gsn_berserk)
      || is_affected(victim, gsn_garou_frenzy) || is_affected(victim, gsn_thaumaturgy_frenzy))
      diff++;

    successes = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_INTIMIDATION), diff);

    if(successes < 0)
    {
      act( "You feel a mental backlash, and your body becomes sluggish!",  ch, NULL, victim, TO_CHAR );
      af.where     = TO_AFFECTS;
      af.type      = gsn_gift_haltthecowardsflight;
      af.level     = ch->level;
      af.duration  = -successes;
      af.location  = 0;
      af.modifier  = 0;
      af.bitvector = AFF_SLOW;
      affect_to_char( ch, &af );
      return;
    }

    if(successes == 0)
    {
      act( "You concentrate for a moment, but $N doesn't seem to be affected.",  ch, NULL, victim, TO_CHAR );
      return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_haltthecowardsflight;
    af.level     = ch->level;
    af.duration  = successes * 2;
    af.location  = APPLY_CS_DEX;
    af.modifier  = -1;
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );

    act( "You suddenly feel terrified by $n, and your body refuses to react as swiftly as normal!",  ch, NULL, victim, TO_VICT );
    act( "$N appears to be terrified by $n!",  ch, NULL, victim, TO_NOTVICT );
    act( "You concentrate for a moment, and $N becomes terrified of you!",  ch, NULL, victim, TO_CHAR );
    return;
}

// Sense Guilt
// perception + empathy diff 8
// Reveals guilt of the target. No idea for codewise.
void spell_gift_senseguilt( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 3

//Might of Thor
// 1 gnosis, 1 rage
// Doubles strength for one turn, wearoff causes -1 to physical attributes
void spell_gift_mightofthor( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
// Redirect Pain
// 1 rage
// aura, reduces damage by 1/2, 1/2 damage gets sent back.
void spell_gift_redirectpain( int sh, int level, CHAR_DATA *ch, void *vo, int target) {
	return;
}

//Rank 4
void spell_gift_berserkerssong( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_ironcannotpierce( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 5
void spell_gift_blazeofglory( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_hordeofvalhalla( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Fianna
//Rank 1
void spell_gift_distractthefool( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *rch;
  int success;

  if (ch->move < ch->level *3)
  {
    send_to_char("You are too tired to activate this Gift.\n\r", ch);
    return;
  }

  ch->move -= ch->level *3;
  act("With a remarkable claim, you point excitedly behind your enemy.", ch, NULL, NULL, TO_CHAR);

  success = godice(get_attribute(ch, MANIPULATION) + ch->pcdata->cssec_abil[CSABIL_EXPRESSION], 7);

  if (success < 0)
  {
    act("Your bold claim seems to be ignored.", ch, NULL, NULL, TO_CHAR);
    act("$N makes an unbelievable claim, which you recognize as only a ruse.", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE(ch, 6);
    return;
  }

  if (success == 0)
  {
    act("Nobody takes any special notice of your bold statement.", ch, NULL, NULL, TO_CHAR);
    act("You ignore the ridiculous claim that $n is making.", ch, NULL, NULL, TO_ROOM);
    return;
  }

  act("Many in the room turn to gawk where you are pointing.", ch, NULL, NULL, TO_CHAR);
  act("Many in the room turn to gawk where $n is pointing.", ch, NULL, NULL, TO_ROOM);

  for ( rch = ch->in_room->people; rch != NULL; rch = rch->next_in_room )
  {
    if (!IS_IMMORTAL(rch) && !IS_SET(rch->act2, ACT2_ULTRA_MOB) && !IS_SET(rch->imm_flags,IMM_CHARM) &&
      can_see(rch, ch) && ch->level < rch->level - 10)
    {
      if ( rch->fighting != NULL )
        stop_fighting( rch, TRUE );
      if (IS_NPC(rch) && IS_SET(rch->act,ACT_AGGRESSIVE))
        REMOVE_BIT(rch->act,ACT_AGGRESSIVE);
      act("$n's wild claim distracts you!", ch, NULL, rch, TO_VICT);
    }
  }

  return;
}

void spell_gift_faerielight( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  OBJ_DATA *light;
  int success;

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }

  ch->pcdata->gnosis[TEMP]--;
  success = godice(get_attribute(ch, WITS) + ch->pcdata->cssec_abil[CSABIL_ENIGMAS], 6);

  if (success < 0)
  {
    act("You twiddle your thumbs for an excessive amount of time, but nothing happens.", ch, NULL, NULL, TO_CHAR);
    act("$n twiddles $s thumbs for a while, looking foolish.", ch, NULL, NULL, TO_ROOM);
    WAIT_STATE(ch, 6);
    return;
  }

  if (success == 0)
  {
    act("You twiddle your thumbs, but nothing seems to happen.", ch, NULL, NULL, TO_CHAR);
    act("$n looks silly, twiddling $s thumbs.", ch, NULL, NULL, TO_ROOM);
    return;
  }

  light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ), 1 );
  light->value[2] += (success * 10) + (ch->level / 2);
  light->value[3] += (success * 10) + (ch->level / 2);
  light->timer += (success * 10) + (ch->level / 2);
  obj_to_room( light, ch->in_room );
  act( "$n twiddles $s thumbs and $p appears.",   ch, light, NULL, TO_ROOM );
  act( "You twiddle your thumbs and $p appears.", ch, light, NULL, TO_CHAR );
  return;
}

//Resist Toxin - Duplicate gift, as Bone Gnawer gift

//Rank 2
void spell_gift_howlofthebanshee( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_warriorsdraught( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 3
void spell_gift_faerieaid( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_woadling( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 4
void spell_gift_balorsgaze( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_semblance( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 5
void spell_gift_fogonthemoor( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_giftofthespriggan( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Red Talons
//Rank 1
void spell_gift_wolfatthedoor( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int success;

  if (IS_NPC(ch))
    return;

  if (!IS_NPC(victim))
  {
    send_to_char("{R*You cannot use this Gift on players.{x\n\r", ch);
    return;
  }

  if (is_affected(victim, gsn_gift_wolfatthedoor))
  {
    send_to_char("Your target is already terrified of the wilderness.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_gift_wolfatthedoor))
  {
    send_to_char("You are too terrified to instill fear in another.\n\r", ch);
    return;
  }

  if (!human_variant(victim))
  {
    send_to_char("Your target does not seem to be human.\n\r", ch);
    return;
  }

  if ( ch->position == POS_FIGHTING || victim->position == POS_FIGHTING)
  {
    send_to_char( "Neither you, nor your target, can be fighting to do this.\n\r", ch );
    return;
  }

  if (!can_see(ch, victim) || !can_see(victim, ch))
  {
    send_to_char("You and your target must be able to see each other's eyes to do this.\n\r", ch);
    return;
  }

  if (!IS_SET(victim->parts, PART_EYE))
  {
    send_to_char("Your target does not have any eyes to lock with!\n\r", ch);
    return;
  }

  if (ch->move < ch->level)
  {
    send_to_char("You are too tired to instill fear into your target.\n\r", ch);
    return;
  }

  ch->move -= ch->level;

  success = godice(get_attribute(ch, CHARISMA) + ch->pcdata->primal_urge, 6);

  if (success < 0)
  {
    act("With a moment of terror, the thought of the open wilderness fills you with dread.", ch, NULL, victim, TO_CHAR);
    act("$n gets a terrified look on $s face.", ch, NULL, victim, TO_ROOM);

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_wolfatthedoor;
    af.level     = ch->level;
    af.duration  = -(2 * success);
    af.modifier  = -1;
    af.location  = APPLY_CS_CHA;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_MAN;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_PER;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_INT;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_WIT;
    affect_to_char( ch, &af );

    return;
  }

  if (success == 0)
  {
    act("You shoot a look at $N, but $E seems unfazed.", ch, NULL, victim, TO_CHAR);
    return;
  }

  act("With a look, you send the fear of the wilderness coursing through $N!", ch, NULL, victim, TO_CHAR);
  act("With a moment of terror, the thought of the open wilderness fills you with dread.", ch, NULL, victim, TO_VICT);
  act("$N gets a terrified look on $S face.", ch, NULL, victim, TO_NOTVICT);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_wolfatthedoor;
  af.level     = ch->level;
  af.duration  = success * 5;
  af.modifier  = -2;
  af.location  = APPLY_CS_CHA;
  af.bitvector = 0;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_MAN;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_PER;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_INT;
  affect_to_char( victim, &af );

  af.location  = APPLY_CS_WIT;
  affect_to_char( victim, &af );

  return;
}
//Rank 2
//Rank 3
//Rank 4
//Rank 5

//Shadow Lords

//Rank 1
void spell_gift_auraofconfidence( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (IS_NPC(ch))
    return;

  if (is_affected(ch, gsn_gift_auraofconfidence))
  {
    if (get_affect_level(ch, gsn_gift_auraofconfidence) == -1)
      send_to_char("Dishonor on you.  Dishonor on your family.  Dishonor on your cow.\n\r", ch);
    else
      send_to_char("You already exude a remarkable amount of confidence.\n\r", ch);
    return;
  }

  if (ch->move < ch->level + 5)
  {
    send_to_char("You are too tired to request a bolster to your confidence.\n\r", ch);
    return;
  }

  ch->move -= (ch->level + 5);
  success = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_SUBTERFUGE), 7);

  if (success < 0)
  {
    act("The ancestor-spirits decree that you have brought shame and dishonor to the Garou!", ch, NULL, NULL, TO_CHAR);
    act("$N hangs $S head low for a moment.", ch, NULL, NULL, TO_NOTVICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_auraofconfidence;
    af.level     = -1;
    af.duration  = -success;
    af.modifier  = -1;
    af.location  = APPLY_CS_CHA;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    WAIT_STATE(ch, 6);
    return;
  }

  if (success == 0)
  {
    send_to_char("The ancestor-spirits seem to have shunned you, and refuse to answer.\n\r", ch);
    WAIT_STATE(ch, 6);
    return;
  }

    act("The ancestor-spirits decree you are worthy, bringing honor to the Garou.  Your rightful place is to rule.", ch, NULL, NULL, TO_CHAR);
    act("$N raises $S head high, holding it regally.", ch, NULL, NULL, TO_NOTVICT);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_auraofconfidence;
    af.level     = success;
    af.duration  = (success * 5) + ch->level;
    af.modifier  = 2;
    af.location  = APPLY_CS_CHA;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    WAIT_STATE(ch, 6);

  return;
}

void spell_gift_fatalflaw( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  MOB_INDEX_DATA *vMob;
  AFFECT_DATA af;
  char buf[MSL];
  int successes = 0;
  int difficulty = 6;
  int quarry = 0;

  if (IS_NPC(ch))
    return;

  if (is_affected(ch, gsn_gift_fatalflaw))
  {
    quarry = get_affect_modifier(ch, gsn_gift_fatalflaw);
    vMob = get_mob_index(quarry);
    if (get_affect_level(ch, gsn_gift_fatalflaw) == 0)
    {
      sprintf(buf, "You are still in awe over %s's perfection, and have trouble getting over this fact.\n\r", vMob->short_descr);
      send_to_char(buf, ch);
      return;
    }
    sprintf(buf, "You decide that %s is no longer worth your time, and prepare for studying your next opponent.\n\r", capitalize(vMob->short_descr));
    send_to_char(buf, ch);
    affect_strip(ch, gsn_gift_fatalflaw);
    return;
  }

  if (!IS_NPC(victim))
  {
    send_to_char("{R*You cannot use this Gift on players.{x\n\r", ch);
    return;
  }

  if (ch == victim)
  {
          send_to_char("You already know yourself well enough.\n\r", ch );
          return;
  }

  if (ch->move < ch->level)
  {
    send_to_char("You are too tired to pinpoint your target's weaknesses.\n\r", ch);
    return;
  }

  if (ch->position == POS_FIGHTING)
  {
    send_to_char("You cannot concentrate in battle long enough to try and gauge your opponent.\n\r", ch);
    return;
  }

  difficulty = get_attribute(victim, WITS) + get_ability(victim, CSABIL_SUBTERFUGE);
  successes = godice(get_attribute(ch, PERCEPTION) + get_ability(ch, CSABIL_EMPATHY), difficulty);
  ch->move -= ch->level;
  WAIT_STATE(ch, 12);

  if (successes < 0)
  {
    act("After careful consideration and observing your target for some time,\n\r you determine that $N is perfect, and has no flaws whatsoever.", ch, NULL, victim, TO_CHAR);
    WAIT_STATE(ch, 12);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_fatalflaw;
    af.level     = 0;
    af.duration  = -(successes) + 1;
  //af.modifier holds mob vnum for comparison in d10_damage to get +1 die if against same mob
    af.modifier  = victim->pIndexData->vnum;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    return;
  }

  if (successes == 0)
  {
    act("Even though you are watching closely, $N betrays no obvious weaknesses.", ch, NULL, victim, TO_CHAR);
    WAIT_STATE(ch, 9);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_fatalflaw;
    af.level     = 0;
    af.duration  = successes;
  //af.modifier holds mob vnum for comparison in d10_damage to get +1 die if against same mob
    af.modifier  = victim->pIndexData->vnum;
    af.location  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  act("Focusing your attention solely on $N, you manage to size $M up \n\rand pinpoint $S most prominent flaw to exploit.", ch, NULL, victim, TO_CHAR);

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_fatalflaw;
  af.level     = successes;
  af.duration  = (2 * successes) + 10;
//af.modifier holds mob vnum for comparison in d10_damage to get +1 die if against same mob
  af.modifier  = victim->pIndexData->vnum;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  if (successes > 3)
  {
    sprintf(buf, "With a sudden flash of insight, you realise that %s is weak to:\n\r{r[{Y%s{r]{x\n\r", victim->short_descr, imm_bit_name(victim->vuln_flags));
    send_to_char(buf,ch);
  }

  gain_exp(ch, successes*2);

  return;
}

void spell_gift_seizingtheedge( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 2
void spell_gift_clapofthunder( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  int wpsuccess = 0, wpdice = 0;
  int clapdamage = ch->level;

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You do not possess the spiritual reserves to activate this Gift.\n\r", ch);
    return;
  }

  ch->pcdata->gnosis[TEMP] -= 1;
  act("You slam your hands together, creating a mighty {Ythunderclap{x.", ch, NULL, NULL, TO_CHAR);
  act("$n slam $s hands together, creating a mighty {Ythunderclap{x.", ch, NULL, NULL, TO_NOTVICT);
  WAIT_STATE(ch, 6);

  for ( vch = char_list; vch != NULL; vch = vch_next )
  {
    vch_next    = vch->next;
    if ( vch->in_room == NULL )
      continue;
    if ( vch->in_room == ch->in_room && SAME_UMBRA(ch, vch))
    {
      if ( vch != ch && !is_same_group(vch, ch) && IS_NPC(vch)
        && !is_affected(vch, gsn_gift_clapofthunder))
      {
        wpdice = vch->level / 30;
        if (vch->level > ch->level + 15)
          wpdice++;
        if (vch->level < ch->level)
          wpdice--;
        wpsuccess = godice(wpdice, 8);
        if (wpsuccess < 0)
          clapdamage = clapdamage * 2;
        if (wpsuccess <= 0)
        {
          d10_damage( ch, vch, 1, clapdamage, gsn_gift_clapofthunder, DAM_SOUND, DEFENSE_NONE, TRUE, TRUE);
          vch->stopped += 5;
          if (wpsuccess < 0)
            vch->stopped += 3;
          send_to_char("The thunderclap rings in your ears, disorienting you momentarily!\n\r", vch);
          act("$N appears disoriented by the thunderclap.", ch, NULL, vch, TO_CHAR);
          act("$N appears disoriented by the thunderclap.", ch, NULL, vch, TO_NOTVICT);
          af.where     = TO_AFFECTS;
          af.type      = gsn_gift_clapofthunder;
          af.level     = 1;
          af.duration  = 1;
          if (wpsuccess < 0)
            af.modifier = -2;
          else
            af.modifier  = -1;
          af.location  = APPLY_CS_PER;
          af.bitvector = 0;
          affect_to_char( vch, &af );
        }
      }
    }
  }

  return;
}
//Luna's Armor - Duplicate gift, as ChildrenofGaia

//Rank 3
void spell_gift_ravenscurse( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_shadowcutting( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 4
void spell_gift_dolorousstroke( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_strengthofthedominator( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 5
void spell_gift_shadowpack( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_obedience( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Silent Striders
//Rank 1
void spell_gift_speedofthought( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;

  if (is_affected(ch, gsn_gift_speedofthought))
  {
    sendch("You slow your enhanced swiftness back to normal speed.\n\r", ch);
    affect_strip(ch, gsn_gift_speedofthought);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }
  ch->pcdata->gnosis[TEMP]--;
  sendch("You call upon spirits of speed, and begin to move swiftly.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 50;
  af.modifier  = (ch->level * 3) + 50;
  af.location  = APPLY_MOVE;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  return;
}

void spell_gift_silence( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;

  if (is_affected(ch, gsn_gift_silence))
  {
    sendch("You drop the ability to muffle sounds.\n\r", ch);
    affect_strip(ch, gsn_gift_silence);
    return;
  }

  if (ch->move < 2 * ch->level)
  {
    sendch("You are too tired to activate this Gift.\n\r", ch);
    return;
  }
  ch->move -= 2 * ch->level;
  sendch("You call upon the owl spirits, and muffle any sound you make.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 25;
  af.modifier  = 0;
  af.location  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  return;
}
//Rank 2
//Rank 3
//Rank 4
//Rank 5

//Silver Fangs
//Rank 1
void spell_gift_eyeofthefalcon( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;

  if (is_affected(ch, gsn_gift_eyeofthefalcon))
  {
    sendch("You dull your vision to normal sensing, no longer viewing with bird of prey acuity.\n\r", ch);
    affect_strip(ch, gsn_gift_eyeofthefalcon);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }
  ch->pcdata->gnosis[TEMP]--;
  sendch("Your eyesight sharpens to predatory avian clarity.\n\r",ch);

  af.where     = TO_AFFECTS;
  af.type      = sn;
  af.level     = level;
  af.duration  = 50;
  af.modifier  = 1;
  af.location  = APPLY_CS_PER;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  return;
}

void spell_gift_lambentfire( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  int blindcheck = 0;

  if (is_affected(ch, gsn_gift_lambentfire))
  {
    act("You will the silvery brilliance to fade from your body.", ch, NULL, NULL, TO_CHAR);
    act("The silvery light shining from $n fades away.", ch, NULL, NULL, TO_NOTVICT);
    affect_strip(ch, gsn_gift_lambentfire);
    ch->in_room->light -= 2;
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have enough willpower to enact this Gift.\n\r", ch);
    return;
  }

  ch->cswillpower--;

  act("Focusing your energies, you ask for Luna's blessing, turning yourself into a shining beacon of silver light.", ch, NULL, NULL, TO_CHAR);
  act("A brilliant silver light erupts from $n, and continues to glow brightly.", ch, NULL, NULL, TO_NOTVICT);

  ch->in_room->light += 2;

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_lambentfire;
  af.level        = level;
  af.duration     = 10 * ((ch->rank / 2) + 1);
  af.modifier     = 0;
  af.location     = 0;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  for ( vch = char_list; vch != NULL; vch = vch_next )
  {
    vch_next    = vch->next;
    if ( vch->in_room == NULL )
      continue;
    if ( vch->in_room == ch->in_room && SAME_UMBRA(ch, vch))
    {
      if ( vch != ch  && !is_safe_spell(ch,vch,TRUE))
      {
        blindcheck = godice(get_attribute(vch,CSATTRIB_WITS) + vch->csabilities[CSABIL_ALERTNESS], 8);
        if (is_same_group(vch,ch))
        {
          if (blindcheck > 0)
            act("You manage to turn and shield your eyes in time from $N's sudden luminescence.", vch, NULL, ch, TO_CHAR);
          else
          {
            act("Caught off guard, you are dazzled momentarily by the intensity of the silver light.", vch, NULL, NULL, TO_CHAR);
            vch->stopped += ch->rank;
          }
        }
        else
        {
          if (blindcheck > 0)
            act("You manage to turn and shield your eyes in time from $N's sudden luminescence.", vch, NULL, ch, TO_CHAR);
          else
          {
            act("Caught off guard by $N's dazzling brilliance, you are briefly blinded!", vch, NULL, ch, TO_CHAR);
            af.where        = TO_AFFECTS;
            af.type         = gsn_blindness;
            af.level        = level;
            af.duration     = 1;
            af.modifier     = 0;
            af.location     = 0;
            af.bitvector    = 0;
            affect_to_char(vch, &af);
          }
        }
      }
    }
    continue;
  }

  return;
}

//Sense Wyrm - Duplicate gift, as METIS

//Rank 2
void spell_gift_empathy( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  int success, followers = 0;

  if (is_affected(ch, gsn_gift_empathy))
  {
    if (get_affect_level(ch, gsn_gift_empathy) == -1)
      send_to_char("You don't need additional information, you've led this group many times.\n\r", ch);
    else
    {  
      act("You've decided that you no longer need to understand your group's expectations.", ch, NULL, NULL, TO_CHAR);
      affect_strip(ch, gsn_gift_empathy);
    }
    return;
  }

  for ( vch = char_list; vch != NULL; vch = vch->next )
  {
    if ( is_same_group( vch, ch ) && vch != ch )
    followers++;
  }

  if (followers == 0)
  {
    send_to_char("You do not currently have a group in need of a leader.\n\r", ch);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    sendch("You do not have the spiritual reserves to activate this gift.\n\r", ch);
    return;
  }
  ch->pcdata->gnosis[TEMP]--;

  success = godice(get_attribute(ch,CSATTRIB_INTELLIGENCE) + ch->csabilities[CSABIL_EMPATHY], 7);

  if (success < 0)
  {
    sendch("You misinterpret the information given, haughtily believing that you already know better.\n\r",ch);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_empathy;
    af.level        = -1;
    af.duration     = 2;
    af.modifier     = -100;
    af.location     = APPLY_HITROLL;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    af.modifier     = -25;
    af.location     = APPLY_DAMROLL;
    affect_to_char(ch, &af);
    return;
  }

  if (success == 0)
  {
    sendch("The Falcon-spirits do not approve of your request, soaring away swiftly.\n\r",ch);
    WAIT_STATE(ch, 9);
    return;
  }

  sendch("Falcon-spirits relay the feelings and expectations of your group, for you to act upon.\n\r",ch);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_empathy;
  af.level        = level;
  af.duration     = (success * 5) + 50;
  af.modifier     = (followers * 75) + (success * 10);
  af.location     = APPLY_HITROLL;
  af.bitvector    = 0;
  affect_to_char(ch, &af);
  af.modifier     = (followers * 20) + (success * 3);
  af.location     = APPLY_DAMROLL;
  affect_to_char(ch, &af);
  return;
}

//Luna's Armor - duplicate gift, as ChildrenofGaia

//Rank 3
void spell_gift_exceptionalswordplay( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_exceptionalswordplay))
  {
    send_to_char("You've already asked the spirits for aid in your melee prowess.\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    sendch("You do not have the Willpower to commune with spirits right now.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  success = godice(get_attribute(ch,CSATTRIB_INTELLIGENCE) + ch->csabilities[CSABIL_MELEE], 8);

  if (success < 0)
  {
    sendch("The spirits of war buffet about you, distracting instead of helping.\n\r",ch);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_exceptionalswordplay;
    af.level        = -1;
    af.duration     = 2;
    af.modifier     = -2;
    af.location     = APPLY_CS_DEX;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    return;
  }

  if (success == 0)
  {
    sendch("The spirits of war seem to be off in battle somewhere.\n\r",ch);
    WAIT_STATE(ch, 9);
    return;
  }

  sendch("Your call to rouse the spirits of war is successful, and they aid you with gusto.\n\r",ch);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_exceptionalswordplay;
  af.level        = ch->level;
  af.duration     = (success * 5) + 40;
  af.modifier     = success * 3 + 1;
  af.location     = 0;
  af.bitvector    = 0;
  affect_to_char(ch, &af);
  return;
}

void spell_gift_wrathofgaia( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 4
void spell_gift_lordlywill( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_sidestepdeath( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank 5
void spell_gift_lunasavenger( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_pawsofthenewborncub( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Warders of Men
//Rank 1
void spell_gift_artisanscommand( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success = 0;

  if (is_affected(ch, gsn_gift_artisanscommand))
  {
    if (get_affect_level(ch, gsn_gift_artisanscommand) < 0)
      send_to_char("The spirits of technology have not forgiven you yet.\n\r", ch);
    else
      send_to_char("You are already speaking to the spirits of rudimentary technology.\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have the Willpower to commune with spirits right now.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  success = godice(get_attribute(ch,CSATTRIB_MANIPULATION) + ch->csabilities[CSABIL_CRAFTS], 7);

  if (success < 0)
  {
    act("You try to commune with the spirits of technology, but they seem to shun you.",ch,NULL,NULL,TO_CHAR);
    act("$n concentrates for a moment, but seems a little disturbed.",ch,NULL,NULL,TO_ROOM);

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_artisanscommand;
    af.level        = -1;
    af.duration     = -(success * 2);
    af.modifier     = 0;
    af.location     = 0;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    return;
  }

  if (success == 0)
  {
    act("You concentrate and try to commune with the spirits of technology, but they seem to be absent.",ch,NULL,NULL,TO_CHAR);
    return;
  }

  act("You calm your mind and reach out to the spirits of technology, who respond to your commands.",ch,NULL,NULL,TO_CHAR);

  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_artisanscommand;
  af.level        = success;
  af.duration     = (success * 2) + 20;
  af.modifier     = 0;
  af.location     = 0;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  return;
}

void spell_gift_giftofsalt( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  int gnosisroll;

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You don't have the spiritual energy to attempt to activate this gift.\n\r", ch);
    return;
  }

  gnosisroll = godice(ch->pcdata->gnosis[PERM], 6);
  if (gnosisroll < 1)
  {
    ch->pcdata->gnosis[TEMP]--;
    send_to_char("The strain of preserving the food of the room has taxed your gnosis supply.\n\r", ch);
  }

  send_to_char("You shunt spiritual energy into the room, intending to preserve food.\n\r", ch);

  for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
  {
    obj_next = obj->next_content;
    if (obj->item_type == ITEM_FOOD)
        if (obj->timer > 0)
        {
          act("The freshness of $p has been preserved.", ch, obj, NULL, TO_CHAR);
          obj->timer = -1;
        }
  }
  return;
}

//Persuasion - duplicate gift, same as Homid

//Rank 2
void spell_gift_maskthepredator( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success = 0;

  success = godice(get_attribute(ch,CSATTRIB_CHARISMA) + ch->csabilities[CSABIL_ANIMAL_KEN], 6);

  send_to_char("You call upon aid from the spirits of domesticated animals to help mask your inner Rage.\n\r", ch);

  if (success < 0)
  {
    act("Unexpectadly, you feel your Rage begin to bristle slightly.", ch, NULL, NULL, TO_CHAR);
    act("With a fierce look, $n seems to be just a little more primal than $e was before.", ch, NULL, NULL, TO_ROOM);
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_maskthepredator;
    af.level        = 0;
    af.duration     = 2;
    af.modifier     = 0;
    af.location     = 0;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    return;
  }

  if (success == 0)
  {
    send_to_char("You fear that the spirits refused to answer your request.\n\r", ch);
    return;
  }

  act("You feel a slight soothing sensation as the spirits grant your request.", ch, NULL, NULL, TO_CHAR);
  act("$n flashes a quick smile, and seems to be much more approachable.", ch, NULL, NULL, TO_ROOM);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_maskthepredator;
  af.level        = 0;
  af.duration     = (success * 5) + 20;
  af.modifier     = 0;
  af.location     = 0;
  af.bitvector    = 0;
  affect_to_char(ch, &af);
  return;
}

void spell_gift_steelfur( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success = 0;

  if (is_affected(ch, gsn_gift_steelfur))
  {
    sendch("Your fur is already turned metallic.\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have the Willpower to commune with spirits right now.\n\r", ch);
    return;
  }

  ch->cswillpower--;
  success = godice(get_attribute(ch,CSATTRIB_STAMINA) + ch->csabilities[CSABIL_ACADEMICS], 7);

  if (success < 0)
  {
    send_to_char("The metal spirits are offended, turning your fur metallic, but soft and malleable.\n\r", ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_steelfur;
    af.level     = -1;
    af.duration  = 2;
    af.modifier  = 100;
    af.location  = APPLY_AC;
    af.bitvector = 0;
    affect_to_char(ch, &af);
    return;
  }

  if (success == 0)
  {
    send_to_char("Metal elemental spirits ignore your plea for assistance.\n\r", ch);
    return;
  }

  act("Your plea is granted, as your fur hardens and takes on a metallic sheen.", ch, NULL, NULL, TO_CHAR);
  act("$n's fur becomes hard and metallic.", ch, NULL, NULL, TO_ROOM);
  af.where        = TO_AFFECTS;
  af.type         = gsn_gift_steelfur;
  af.level        = success;
  af.duration     = (success * 10) + 50;
  af.modifier     = -(success * 50);
  af.location     = APPLY_AC;
  af.bitvector    = 0;
  affect_to_char(ch, &af);

  return;
}

//Rank 3
//Reshape Object - duplicate gift, as HOMID

void spell_gift_tongues( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  AFFECT_DATA af;
  int success;

  if (is_affected(ch, gsn_gift_tongues))
  {
    sendch("Raven-spirits have already granted you perfect understanding of any language.\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    sendch("You do not have the strength of will to call the spirits.\n\r", ch);
    return;
  }

  ch->cswillpower -= 1;
  success = godice(get_attribute(ch, INTELLIGENCE) + get_ability(ch, CSABIL_LINGUISTICS), 6);

  if (success < 0)
  {
    sendch("The raven-spirits mock your paltry attempt to garner a favor.\n\r", ch);
    WAIT_STATE(ch, 12);
    return;
  }

  if (success == 0)
  {
    sendch("The spirits of ravens watch from afar, but do not answer.\n\r", ch);
    return;
  }

  sendch("An unkindness of raven-spirits opens your mind to perfect understanding of all languages.\n\r", ch);

  af.where = TO_AFFECTS;
  af.type  = gsn_gift_tongues;
  af.level = success;
  af.duration = success * 5 + 75;
  af.bitvector = 0;
  af.modifier = success;
  af.location = APPLY_CS_CHA;
  affect_to_char(ch, &af);
  return;
}

//Rank 4
void spell_gift_attunement( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  char buf[MAX_STRING_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  AREA_DATA *pArea;
  AFFECT_DATA af;
  int vnum;
  int i, success = 0;
  bool found = FALSE, lvl = TRUE, pop = FALSE, race = FALSE, shop = FALSE, heal = FALSE;

  pArea = ch->in_room->area;
  success = godice(get_attribute(ch, PERCEPTION) + get_ability(ch, CSABIL_ENIGMAS), 6);

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You don't have the spiritual energy to attempt to activate this gift.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_gift_attunement))
  {
    send_to_char("It is too soon to request information from cockroach-spirits again.\n\r", ch);
    return;
  }

  if (success < 0)
  {
    sendch("The cockroach-spirits converse at length, finally revealing that they know nothing.\n\r", ch);
    sendch("Adding insult to injury, you feel your spiritual reserves partially drained.\n\r", ch);
    ch->pcdata->gnosis[TEMP]--;
    af.where = TO_AFFECTS;
    af.type  = gsn_gift_attunement;
    af.level = -1;
    af.duration = -success + 1;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = 0;
    affect_to_char(ch, &af);
    WAIT_STATE(ch, 12);
    return;
  }

  if (success == 0)
  {
    sendch("The spirits of the area are scattered, refusing your call for information.\n\r", ch);
    af.where = TO_AFFECTS;
    af.type  = gsn_gift_attunement;
    af.level = 0;
    af.duration = 1;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = 0;
    affect_to_char(ch, &af);
    return;
  }

  if (success > 1)
    pop = TRUE;
  if (success > 2)
  {
    shop = TRUE;
    heal = TRUE;
  }
  if (success > 3)
    race = TRUE;

  sprintf(buf, "The spirits relay information about the inhabitants of {Y%s{x:\n\r", pArea->name);
  send_to_char(buf, ch);
  sprintf(buf, "{R     %s  {G%s {C%s {M%s{W  Name{x\n\r", race ? "      Race" : "??????????",
    shop || heal ? "  Special  " : "  ???????  ", pop ? "x #" : "???", lvl ? "Level" : "?????");
  send_to_char(buf, ch);
  for ( vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++ )
  {
    if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
    {
      if(pMobIndex->count > 0)
      {
        found = TRUE;
        sprintf( buf, "({R%14s{W) [{G%s %s{W] {Cx%2d{W <{M%3d{W>  %s\n\r", 
          race ? race_table[pMobIndex->race].name : "       unknown", 
          shop && pMobIndex->pShop ? "Shop" : "    ", 
          heal && IS_SET(pMobIndex->act, ACT_IS_HEALER) ? "Heal" : "    ",
          pop ? pMobIndex->count : 00, lvl ? pMobIndex->level : 00, pMobIndex->short_descr );
        send_to_char( buf, ch );
      }
    }
  }
  if(!found)
    send_to_char( "{YNot even the cockroach-spirits could find any signs of other inhabitants.\n\r", ch );

    af.where = TO_AFFECTS;
    af.type  = gsn_gift_attunement;
    af.level = success;
    af.duration = 3;
    af.bitvector = 0;
    af.modifier = 0;
    af.location = 0;
    affect_to_char(ch, &af);

  return;
}

void spell_gift_doppelganger( int sn, int level, CHAR_DATA *ch, void *vo, int target)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  int success;

  if (IS_NPC(ch)) return;

  if (victim == NULL)
  {
    send_to_char( "Whom are you trying to copy?\n\r", ch );
    return;
  }

  if (is_affected( ch, gsn_gift_doppelganger ))
  {
    if (get_affect_level(ch, gsn_gift_doppelganger) == -1)
    {
      send_to_char("You can't seem to undo the spirit's guise cast upon you.\n\r", ch);
      return;
    }
    send_to_char( "You drop your Doppelganger guise.\n\r", ch );
    affect_strip(ch,gsn_gift_doppelganger);
    return;
  }

  if (victim == ch)
  {
    if (is_affected(ch, gsn_gift_doppelganger))
    {
      if (get_affect_level(ch, gsn_gift_doppelganger) == -1)
      {
        send_to_char("You can't seem to undo the spirit's guise cast upon you.\n\r", ch);
        return;
      }
      send_to_char( "You drop your Doppelganger guise.\n\r", ch );
      affect_strip(ch,gsn_gift_doppelganger);
      return;
    }
    else
    {
      sendch("What would be the point of becoming a Doppelganger of yourself?\n\r", ch);
      return;
    }
  }

  if (!doppelganger_copy(victim))
  {
    send_to_char("This Gift only allows you to mimic a humanoid, wolf, or another Garou.\n\r", ch);
    return;
  }

  if (ch->pcdata->gnosis[TEMP] < 1)
  {
    send_to_char("You don't have the spiritual energy to attempt to activate this gift.\n\r", ch);
    return;
  }

  ch->pcdata->gnosis[TEMP]--;
  success = godice(get_attribute(ch, CHARISMA) + get_ability(ch, CSABIL_PERFORMANCE), 8);

  if (success < 0)
  {
    sendch("The illusion spirit is offended at your request; the guise you assume is less than pleasing.\n\r", ch);
    sprintf(buf, "'Ugly' %s",ch->name,ch->name);
    ch->short_descr = str_dup( buf );
    sprintf(buf, "'Ugly' %s",ch->name);
    ch->shift = str_dup( buf );
    af.where = TO_AFFECTS;
    af.type  = gsn_gift_doppelganger;
    af.level = -1;
    af.duration = 2;
    af.bitvector = 0;
    af.modifier = -1;
    af.location = APPLY_CS_APP;
    affect_to_char(ch, &af);
    WAIT_STATE(ch, 12);
    return;
  }

  if (success == 0)
  {
    sendch("Spirits of illusion seem to be in short supply at the moment.\n\r", ch);
    WAIT_STATE(ch, 12);
    return;
  }

  if(!IS_NPC(victim))
  {
    act( "Your features alter and shift, turning yourself into a copy of $N.", ch, NULL, victim, TO_CHAR );
    act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
    sprintf(buf, "%s",victim->name,ch->name);
    ch->short_descr = str_dup( buf );
    sprintf(buf, "%s",victim->name);
    ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_doppelganger;
    af.level     = ch->level;
    af.duration  = (success * 4) + 48;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  act( "Your features alter and shift, turning yourself into a copy of $N.", ch, NULL, victim, TO_CHAR );
  act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
  sprintf(buf, "%s",capitalize(victim->short_descr),victim->long_descr);
  ch->short_descr = str_dup( buf );
  sprintf(buf, "%s",capitalize(victim->short_descr));
  ch->shift = str_dup( buf );

  af.where     = TO_AFFECTS;
  af.type      = gsn_gift_doppelganger;
  af.level     = ch->level;
  af.duration  = (success * 4) + 48;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );
  return;
}

//Rank 5
void spell_gift_commandtheblaze( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//False Comfort, duplicate gift, as HOMID

/* * * * * Uncoded/Invalid Tribes * * * * */

//Stargazers
//Rank 1

//Rank 2

//Rank 3
//"Inner Strength"
void spell_gift_innerstrength( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    AFFECT_DATA af;

    if(is_affected(ch, gsn_gift_innerstrength))
    {
        send_to_char("You cannot find the calm of mind needed to release your Rage right now.\n\r",ch);
        return;
    }
    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_innerstrength;
    af.level        = level;
    af.duration     = 12;
    af.modifier     = -(ch->rage);
    af.location     = APPLY_AC;
    af.bitvector    = 0;
    affect_to_char(ch, &af);

    ch->rage = 0;
    ch->position = POS_SITTING;

    send_to_char("You sit and calm your mind, transforming your Rage into strength of will.\n\r",ch);
    act("$n sits and meditates a moment then seems much calmer.",ch,NULL,NULL,TO_ROOM);
    return;
}

//Rank 4
//Rank 5

//Wendigo Gift
//Rank 1
//Rank 2
//Rank 3
//"Bloody Feast"
void spell_gift_bloodyfeast( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
    OBJ_DATA *obj;
    AFFECT_DATA af;

    if ((obj = get_obj_here( ch, NULL, "corpse" )) == NULL)
    {
        send_to_char("Feast upon what?\n\r",ch);
        return;
    }
    if(obj->item_type != ITEM_CORPSE_NPC)
    {
        send_to_char("You cannot feast upon that.\n\r",ch);
        return;
    }
    if(obj->level == 0 || obj->timer < 2)
    {
        send_to_char("You cannot find any fresh meat left on it.\n\r",ch);
        return;
    }

    if (ch->pcdata->condition[COND_FULL] > 40 )
    {
        send_to_char( "You are too full to feast.\n\r", ch );
        return;
    }

    act( "$n squats over $p and devours its flesh in a fury, splashing blood and entrails everywhere.",  ch, obj, NULL, TO_ROOM );
    act( "You squat over $p and tear it to shreds, devowering the fresh,\n\rwarm, bloody meat with great enjoyment.", ch, obj, NULL, TO_CHAR );

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_bloodyfeast;
    af.level        = level;
    af.duration     = level/5;
    af.modifier     = 10;
    af.location     = APPLY_DAMROLL;
    af.bitvector    = 0;
    affect_to_char(ch, &af);

    gain_condition( ch, COND_FULL, obj->level/10);
    gain_condition( ch, COND_HUNGER, obj->level/10);
    ch->rage += 5;

    extract_obj( obj );
    return;
}
//Rank 4
//Rank 5

/*======End Gifts======*/

void do_beseech(CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    void *vo;
    int sn;
    int gn;
    int target;
    int i;
    bool learned=FALSE;
    bool pass=FALSE;


    gift_target_name = one_argument( argument, arg1 );
    one_argument( gift_target_name, arg2 );

    if (IS_NPC(ch))
      return;

    if ( arg1[0] == '\0' )
    {
    send_to_char( "Beseech Gaia to bestow you what gift?\n\r", ch );
    return;
    }

    if ((sn = skill_lookup(arg1)) < 1 || (gn = gift_lookup(arg1)) < 0)
    {
    send_to_char( "There are no Gifts by that name..\n\r", ch );
    return;
    }

    if(str_cmp(skill_table[sn].name,gift_table[gn].name))
    {
    send_to_char( "There are no Gifts by that name..\n\r", ch );
    return;
    }

    if ( ch->position < skill_table[sn].minimum_position )
    {
    send_to_char( "You can't concentrate enough.\n\r", ch );
    return;
    }

    for (i = 0; i < MAX_GIFT; i++)
    {
        if (ch->pcdata->gift[i] == gn)
            learned = TRUE;
    }
    if (!learned)
    {
        send_to_char("The spirits have not yet taught you this Gift of Gaia.\n\r",ch);
        return;
    }

/*
    if(ch->level < gift_table[gn].level)
    {
        send_to_char("You are not yet worthy to use that Gift.\n\r",ch);
        return;
    }
*/
    if(ch->pcdata->rank < gift_table[gn].rank)
    {
        send_to_char("You are not known enough to Gaia and your brothers and sisters to use this Gift.\n\r",ch);
        return;
    }


    if(ch->pcdata->breed == gift_table[gn].breed) pass=TRUE;
    if(ch->pcdata->auspice == gift_table[gn].auspice) pass=TRUE;
    if(ch->pcdata->tribe == gift_table[gn].tribe) pass=TRUE;

    if(!pass)
    {
        send_to_char("You were not born into this gift.\n\r",ch);
        return;
    }

    /*
     * Locate targets.
     */
    victim  = NULL;
    obj     = NULL;
    vo      = NULL;
    target  = TARGET_NONE;

    switch ( skill_table[sn].target )
    {
    default:
    bug( "Do_cast: bad target for sn %d.", sn );
    return;

    case TAR_IGNORE:
    break;

    case TAR_CHAR_OFFENSIVE:
    if ( arg2[0] == '\0' )
    {
        if ( ( victim = ch->fighting ) == NULL )
        {
        send_to_char( "Beseech this upon whom?\n\r", ch );
        return;
        }
    }
    else
    {
        if ( ( victim = get_char_room( ch, NULL, gift_target_name ) ) == NULL )
        {
        send_to_char( "They aren't here.\n\r", ch );
        return;
        }
    }

    if ( IS_AFFECTED2(ch, AFF2_MIST) || is_affected(ch,gsn_shadowform))
    {
        send_to_char("Not in this form you don't!\n\r",ch);
        return;
    }
    if (IS_SET(victim->act,PLR_SPEC) && !IS_NPC(victim))
    {
        send_to_char("I don't think so!\n\r",ch);
        return;
    }

    if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
    {
        send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
        return;
    }

    if ( !IS_NPC(ch) )
    {

            if (is_safe(ch,victim) && victim != ch)
        {
        send_to_char("Not on that target.\n\r",ch);
        return;
        }

        if ( !IS_NPC(victim) && !IS_NPC(ch) && !IN_LEVEL(ch, victim))
        {
            send_to_char( "You cannot harm them, they are not in level to you!\n\r", ch );
            return;
        }

    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
    {
        send_to_char( "You can't do that on your own follower.\n\r",
        ch );
        return;
    }

    vo = (void *) victim;
    target = TARGET_CHAR;
    break;

    case TAR_CHAR_DEFENSIVE:
    if ( arg2[0] == '\0' )
    {
        victim = ch;
    }
    else
    {
        if ( ( victim = get_char_room( ch, NULL, gift_target_name ) ) == NULL )
        {
        send_to_char( "They aren't here.\n\r", ch );
        return;
        }
    }

    vo = (void *) victim;
    target = TARGET_CHAR;
    break;

    case TAR_CHAR_SELF:
    if ( arg2[0] != '\0' && !is_name( gift_target_name, ch->name ) )
    {
        send_to_char( "You cannot grant this to another.\n\r", ch );
        return;
    }

    vo = (void *) ch;
    target = TARGET_CHAR;
    break;

    case TAR_OBJ_INV:
    if ( arg2[0] == '\0' )
    {
        send_to_char( "What shall this be used upon?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, gift_target_name, ch ) ) == NULL )
    {
        send_to_char( "You are not carrying that.\n\r", ch );
        return;
    }

    vo = (void *) obj;
    target = TARGET_OBJ;
    break;

    case TAR_OBJ_CHAR_OFF:
    if (arg2[0] == '\0')
    {
        if ((victim = ch->fighting) == NULL)
        {
        send_to_char("Do this to whom or what?\n\r",ch);
        return;
        }

        target = TARGET_CHAR;
    }
    else if ((victim = get_char_room(ch,NULL, gift_target_name)) != NULL)
    {
        target = TARGET_CHAR;
    }

    if (target == TARGET_CHAR) /* check the sanity of the attack */
    {
        if(is_safe_spell(ch,victim,FALSE) && victim != ch)
        {
        send_to_char("Not on that target.\n\r",ch);
        return;
        }

            if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim )
            {
                send_to_char( "You can't do that on your own follower.\n\r",
                    ch );
                return;
            }

        vo = (void *) victim;
    }
    else if ((obj = get_obj_here(ch,NULL, gift_target_name)) != NULL)
    {
        vo = (void *) obj;
        target = TARGET_OBJ;
    }
    else
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    break;

    case TAR_OBJ_CHAR_DEF:
        if (arg2[0] == '\0')
        {
            vo = (void *) ch;
            target = TARGET_CHAR;
        }
        else if ((victim = get_char_room(ch,NULL, gift_target_name)) != NULL)
        {
            vo = (void *) victim;
            target = TARGET_CHAR;
    }
    else if ((obj = get_obj_carry(ch,gift_target_name,ch)) != NULL)
    {
        vo = (void *) obj;
        target = TARGET_OBJ;
    }
    else
    {
        send_to_char("You don't see that here.\n\r",ch);
        return;
    }
    break;
    }

    WAIT_STATE( ch, gift_table[gn].lag );

    (*skill_table[sn].spell_fun) ( sn, ch->level, ch, vo,target);


    if ((skill_table[sn].target == TAR_CHAR_OFFENSIVE
    ||   (skill_table[sn].target == TAR_OBJ_CHAR_OFF && target == TARGET_CHAR))
    &&   victim != ch
    &&   victim->master != ch)
    {
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next = vch->next_in_room;
        if ( victim == vch && victim->fighting == NULL )
        {
        multi_hit( victim, ch, TYPE_UNDEFINED );
        break;
        }
    }
    }
    return;
}

void do_gifts(CHAR_DATA *ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MSL];
  int i, gbreed, gauspice, gtribe, uncoded = 0, coded = 0, learned = 0;
  int col = 1;
  BUFFER *buffer;
  buffer = new_buf();
  bool list = FALSE, learn = FALSE;

  if(IS_NPC(ch)) return;

  if (!str_cmp(argument, "\0"))
  {
    send_to_char("          {GHaven Garou Gifts from Gaia{x\n\r", ch);
    send_to_char(" {WSyntax:  {cgifts (all/breed/auspice/tribe/list/learn){x\n\r", ch);
    send_to_char("    all:  Display all Gifts in Haven.\n\r", ch);
    send_to_char("  breed:  Display all Breed Gifts.\n\r", ch);
    send_to_char("auspice:  Display all Auspice Gifts.\n\r", ch);
    send_to_char("  tribe:  Display all Tribe Gifts.\n\r", ch);
    send_to_char("  {G*{xlist:  Display Gifts you currently know.\n\r", ch);
    send_to_char(" {G*{xlearn:  Display Gifts you can learn based on your Breed/Auspice/Tribe.\n\r\n\r", ch);
    send_to_char("{G*{xThese options available to {GGarou{x only.\n\r", ch);
    return;
  }

  if (!str_cmp(argument, "all"))
  {
    send_to_char("All of Haven's Garou Gifts from Gaia:\n\r\n\r",ch);
    send_to_char("{R*{xGift is currently Uncoded.\n\r", ch);
    cprintf(ch, "%s{x\n\r", " [{BRank{x] ({RBreed{x) {{{MAuspice{x } <     {GTribe{x      > {D:: {CGift Name");
    send_to_char("/===============================================================================\\\n\r", ch);
    for (i = 1; i < MAX_GIFTS_CODED; i++)
    {
      gbreed = gift_table[i].breed;
      gauspice = gift_table[i].auspice;
      gtribe = gift_table[i].tribe;
      if (gift_table[i].level == 0)
        uncoded++;
      else
        coded++;
      sprintf(buf, "{R%s{x[  {B%d{x ] %s %s %s {D:: {C%-30s{x\n\r",
        gift_table[i].level == 0 ? "*" : " ",
        gift_table[i].rank,
        gbreed == HOMID ? "({RHomid{x)" : gbreed == LUPUS ? "({RLupus{x)" : gbreed == METIS ? "({RMetis{x)" : "       ",
        gauspice == RAGABASH ? "{{{MRagabash{x}" : gauspice == THEURGE ? "{{{MTheurge{x }" : gauspice == PHILODOX ? "{{{MPhilodox{x}" : 
        gauspice == GALLIARD ? "{{{MGalliard{x}" : gauspice == AHROUN ? "{ {MAhroun{x }" : "          ",
        gtribe == BLACK_FURY ? "<   {GBlack Fury{x   >" : gtribe == CHILDREN_OF_GAIA ? "<{GChildren of Gaia{x>" : 
        gtribe == FIANNA ? "<     {GFianna{x     >" : gtribe == FENRIR ? "< {GGet of Fenris{x  >" : 
        gtribe == SHADOW_LORD ? "<  {GShadow Lord{x   >" : gtribe == BONE_GNAWER ? "<  {GBone Gnawer{x   >" : 
        gtribe == SILVER_FANG ? "<  {GSilver Fang{x   >" : gtribe == WARDERSOFMEN ? "< {GWarders of Men{x >" : 
        gtribe == RED_TALON ? "<   {GRed Talon{x    >" : gtribe == SILENT_STRIDER ? "< {GSilent Strider{x >" : 
        "                  ",
        capitalize(gift_table[i].name));
      send_to_char(buf, ch);
    }
    send_to_char("\\===============================================================================/\n\r", ch);
    sprintf(buf, " \\  {R*{x Total Uncoded Gifts:  %3d                                                /\n\r", uncoded);
    send_to_char(buf, ch);
    sprintf(buf, "  \\    Total Coded Gifts:  %3d                                                /\n\r", coded);
    send_to_char(buf, ch);
    send_to_char("   \\=========================================================================/\n\r", ch);

    return;
  }

  if (!str_cmp(argument, "breed") || !str_cmp(argument, "tribe") || !str_cmp(argument, "auspice"))
  {
    sprintf(buf, "Haven's Garou %s%s{x Gifts from Gaia:\n\r\n\r", 
      !str_cmp(argument, "breed") ? "{R" : !str_cmp(argument, "tribe") ? "{G" :
      !str_cmp(argument, "auspice") ? "{M" : "", capitalize(argument));
    send_to_char(buf, ch);
    send_to_char("{R*{xGift is currently Uncoded.\n\r", ch);
    sprintf(buf, " [Rank] %s(Breed){x %s{Auspice }{x %s<     Tribe      >{x :: Gift\n\r",
      !str_cmp(argument, "breed") ? "{R" : "", !str_cmp(argument, "auspice") ? "{M" : "",
      !str_cmp(argument, "tribe") ? "{G" : "");
    send_to_char(buf, ch);
    send_to_char("/===============================================================================\\\n\r", ch);
    for (i = 1; i < MAX_GIFTS_CODED; i++)
    {
      gbreed = gift_table[i].breed;
      gauspice = gift_table[i].auspice;
      gtribe = gift_table[i].tribe;
      sprintf(buf, "{R%s{x[  %d ] %s%s{x %s%s{x %s%s{x :: %-30s{x\n\r",
        gift_table[i].level == 0 ? "*" : " ",
        gift_table[i].rank,
        !str_cmp(argument, "breed") ? "{R" : "",
        gbreed == HOMID ? "(Homid)" : gbreed == LUPUS ? "(Lupus)" : gbreed == METIS ? "(Metis)" : "       ",
        !str_cmp(argument, "auspice") ? "{M" : "",
        gauspice == RAGABASH ? "{{Ragabash}" : gauspice == THEURGE ? "{Theurge }" : gauspice == PHILODOX ? "{Philodox}" : 
        gauspice == GALLIARD ? "{{Galliard}" : gauspice == AHROUN ? "{ Ahroun }" : "          ",
        !str_cmp(argument, "tribe") ? "{G" : "",
        gtribe == BLACK_FURY ? "<   Black Fury   >" : gtribe == CHILDREN_OF_GAIA ? "<Children of Gaia>" : 
        gtribe == FIANNA ? "<     Fianna     >" : gtribe == FENRIR ? "< Get of Fenris  >" : 
        gtribe == SHADOW_LORD ? "<  Shadow Lord   >" : gtribe == BONE_GNAWER ? "<  Bone Gnawer   >" : 
        gtribe == SILVER_FANG ? "<  Silver Fang   >" : gtribe == WARDERSOFMEN ? "< Warders of Men >" : 
        gtribe == RED_TALON ? "<   Red Talon    >" : gtribe == SILENT_STRIDER ? "< Silent Strider >" : 
        "                  ",
        capitalize(gift_table[i].name));
      if ((!str_cmp(argument, "breed") && gbreed != 0))
      {
        if (gift_table[i].level == 0)
          uncoded++;
        else
          coded++;
        send_to_char(buf, ch);
      }
      if ((!str_cmp(argument, "auspice") && gauspice != 0))
      {
        if (gift_table[i].level == 0)
          uncoded++;
        else
          coded++;
        send_to_char(buf, ch);
      }
      if ((!str_cmp(argument, "tribe") && gtribe != 0))
      {
        if (gift_table[i].level == 0)
          uncoded++;
        else
          coded++;
        send_to_char(buf, ch);
      }
    }
    send_to_char("\\===============================================================================/\n\r", ch);
    sprintf(buf, " \\  {R*{x Total Uncoded %7s Gifts:  %3d                                        /\n\r", capitalize(argument), uncoded);
    send_to_char(buf, ch);
    sprintf(buf, "  \\    Total Coded %7s Gifts:  %3d                                        /\n\r", capitalize(argument), coded);
    send_to_char(buf, ch);
    send_to_char("   \\=========================================================================/\n\r", ch);

    return;
  }

  if(ch->race != race_lookup("garou"))
  {
    send_to_char("You are not one of Gaia's chosen!\n\r",ch);
    return;
  }

  if(ch->pcdata->rank == 0)
  {
    send_to_char("You must finish setting your Garou stats with {Ggogarou{x before using gifts.\n\r", ch);
    return;
  }

  if (!str_cmp(argument, "list"))
	{
    list = TRUE;
		send_to_char("Your Gifts from Gaia:\n\r\n\r",ch);
    send_to_char(" [{BRank{x]  {RBre{x|{MAus{x|{GTri{x  - {CGift Name{x\n\r", ch);
    send_to_char("----------------------------------------------------------{x\n\r", ch);
		for (i = 0; i < MAX_GIFT; i++)
		{
      gbreed = gift_table[ch->pcdata->gift[i]].breed;
      gauspice = gift_table[ch->pcdata->gift[i]].auspice;
      gtribe = gift_table[ch->pcdata->gift[i]].tribe;
			if (ch->pcdata->gift[i] == 0)
				continue;
			else 
      {
        sprintf(buf, "%s  {B%d{x     %s %s %s  - {C%-30s{x\n\r", 
          gift_table[ch->pcdata->gift[i]].level == 0 ? "{r*{x" : " ",
          gift_table[ch->pcdata->gift[i]].rank, 
          gbreed == HOMID ? "{RHom{x" : gbreed == LUPUS ? "{RLup{x" : gbreed == METIS ? "{RMet{x" : "   ",
          gauspice == RAGABASH ? "{MRag{x" : gauspice == THEURGE ? "{MThe{x" : gauspice == PHILODOX ? "{MPhi{x" : 
          gauspice == GALLIARD ? "{MGal{x" : gauspice == AHROUN ? "{MAhr{x" : "   ",
          gtribe == BLACK_FURY ? "{GBla{x" : gtribe == CHILDREN_OF_GAIA ? "{GChi{x" : 
          gtribe == FIANNA ? "{GFia{x" : gtribe == FENRIR ? "{GGet{x" : 
          gtribe == SHADOW_LORD ? "{GSha{x" : gtribe == BONE_GNAWER ? "{GBon{x" : 
          gtribe == SILVER_FANG ? "{GSil{x" : gtribe == WARDERSOFMEN ? "{GWar{x" : 
          gtribe == RED_TALON ? "{GRed{x" : gtribe == SILENT_STRIDER ? "{GStr{x" : "   ",
          capitalize(gift_table[ch->pcdata->gift[i]].name));
				add_buf(buffer, buf);
        learned++;
			}
		}
	}
  else
  if (!str_cmp(argument, "learn"))
  {
    learn = TRUE;
	  send_to_char("Gifts you may learn:\n\r\n\r",ch);
    send_to_char(" [{BRank{x]  {RBre{x|{MAus{x|{GTri{x  - {CGift Name{x\n\r", ch);
    send_to_char("----------------------------------------------------------{x\n\r", ch);

    for (i = 1; i < MAX_GIFTS_CODED; i++)
		{
			int gn;
      bool islearned = FALSE;
      gbreed = gift_table[i].breed;
      gauspice = gift_table[i].auspice;
      gtribe = gift_table[i].tribe;

      if (ch->pcdata->rank < gift_table[i].rank || (gift_table[i].breed != ch->pcdata->breed &&
					gift_table[i].auspice != ch->pcdata->auspice && gift_table[i].tribe != ch->pcdata->tribe))
				continue;

			for (gn = 0; gn < MAX_GIFT; gn++)
				if (ch->pcdata->gift[gn] == i) islearned = TRUE;

			if (islearned)
				continue;

      sprintf(buf, "%s  {B%d{x     %s %s %s  - {C%-30s{x\n\r", gift_table[i].level == 0 ? "{r*{x" : " ", 
        gift_table[i].rank, 
        gbreed == HOMID ? "{RHom{x" : gbreed == LUPUS ? "{RLup{x" : gbreed == METIS ? "{RMet{x" : "   ",
        gauspice == RAGABASH ? "{MRag{x" : gauspice == THEURGE ? "{MThe{x" : gauspice == PHILODOX ? "{MPhi{x" : 
        gauspice == GALLIARD ? "{MGal{x" : gauspice == AHROUN ? "{MAhr{x" : "   ",
        gtribe == BLACK_FURY ? "{GBla{x" : gtribe == CHILDREN_OF_GAIA ? "{GChi{x" : 
        gtribe == FIANNA ? "{GFia{x" : gtribe == FENRIR ? "{GGet{x" : 
        gtribe == SHADOW_LORD ? "{GSha{x" : gtribe == BONE_GNAWER ? "{GBon{x" : 
        gtribe == SILVER_FANG ? "{GSil{x" : gtribe == WARDERSOFMEN ? "{GWar{x" : 
        gtribe == RED_TALON ? "{GRed{x" : gtribe == SILENT_STRIDER ? "{GStr{x" : "   ",
        capitalize(gift_table[i].name));
			add_buf(buffer, buf);
		}
  }
  else
  {
    send_to_char("Valid options are: all/breed/auspice/tribe/list/learn\n\r", ch);
    return;
  }

    page_to_char(buf_string(buffer), ch);
    send_to_char("----------------------------------------------------------{x\n\r", ch);
    if (list)
    {
      sprintf(buf, "   You have learned [{Y%2d{x/{y%2d{x] Gifts.\n\r", learned, MAX_GIFT);
      send_to_char(buf, ch);
    }
    send_to_char("  {r*{xGift is not currently coded.\n\r", ch);
    if (learn)
    {
      switch(ch->pcdata->rank)
      {
        case 1: sprintf(buf2,"Cliath"); break;
        case 2: sprintf(buf2,"Fostern"); break;
        case 3: sprintf(buf2,"Adren"); break;
        case 4: sprintf(buf2,"Athro"); break;
        case 5: sprintf(buf2,"Elder"); break;
        default: sprintf(buf2,"Pup"); break;
      }
      sprintf(buf, "   Gifts above your current Rank [ {B%s{x ({B%d{x) ] are not shown.\n\r", buf2, ch->pcdata->rank);
      send_to_char(buf, ch);
    }

    return;
}
