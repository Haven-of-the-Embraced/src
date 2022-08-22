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
void spell_gift_speechoftheworld( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_calmthesavagebeast( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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

        if (victim == NULL)
        {
            sendch("They aren't here.\n\r", ch);
            return;
            }

		if (type == AIR && (!is_affected(victim, gsn_fly) || !IS_AFFECTED(victim, AFF_FLYING)))
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

		success = godice(ch->pcdata->gnosis[PERM], 7);

		if (success < 1)
		{
			sendch("You fail to create the desired element.\n\r", ch);
			return;
		}

		if (type == FIRE)
		{
			int mod = ch->remorts/5 + ch->level/10 + 5;
			act("You direct a blast of flames at $N!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Fire as a blast of flame engulfs $N!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. You're engulfed in flames!", ch, NULL, victim, TO_VICT);
			d10_damage(ch, victim, UMIN(3, success), mod, gsn_gift_createelement, DAM_FIRE, DEFENSE_NONE, TRUE, TRUE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			return;
		}
		if (type == WATER)
		{
			spell_create_water(sn, level, ch, (void *) obj, target);
			return;
		}
		if (type == EARTH)
		{
			int mod = ch->remorts/5 + ch->level/10 + 5;
			act("You create a bunch of heavy rocks to drop on $N!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Earth as rocks rain down from the heavens onto $N!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. boulders are raining down on your head!", ch, NULL, victim, TO_VICT);
			d10_damage(ch, victim, UMIN(5, success), mod, gsn_gift_createelement, DAM_BASH, DEFENSE_FULL, TRUE, TRUE);
			WAIT_STATE(ch, PULSE_VIOLENCE);
			return;
		}
	    if (type == AIR)
		{
			act("You create gusts of air to knock $N from the sky!", ch, NULL, victim, TO_CHAR);
			act("$n says thanks to the Elemental Spirits of Air and gusts of air begin to buffet $N from the skies!", ch, NULL, victim, TO_NOTVICT);
			act("$n looks at you while murmering something and suddenly.. strong winds blow you out of the sky!", ch, NULL, victim, TO_VICT);
			affect_strip(victim, gsn_fly);
			REMOVE_BIT(victim->affected_by, AFF_FLYING);
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
void spell_gift_curseofhatred( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//
//“Sense Silver”
//Lunes (lunar spirits)
//Roll: Perception + Primal Urge (diff 7)
//For reasons known only to herself, Luna has been kind enough to the metis to allow them to sense a werewolf’s greatest weakness.. silver. (Allows the garou to tell when someone is carrying silver. short duration, like detect magic or evil, but if any of the objs on the char are made from silver, they’ll have an aura.)
//Cost: None.
void spell_gift_sensesilver( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 3
//
//“Eyes of the Cat”
//Any cat spirit
//Roll:None.
//The werewolf can see clearly in pitch darkness. Her eyes glow a lambent green while this power is in effect. (Pretty obvious, night vision for Garou. But since this is a free gift with no roll, the garou is blinded by sources of light.)
//Cost: None
void spell_gift_eyesofthecat( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Mental Speech”
//Bird spirits
//Roll: Charisma + Empathy (diff 8)
//This gift allows mental communication even over vast distances. (This power is made useless by tells. Will find something to replace it.)
//Cost: 1 Willpower
void spell_gift_mentalspeech( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_witherlimb( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_haresleap( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    AFFECT_DATA af;


     if (is_affected(ch, gsn_gift_haresleap))
     {
        sendch("Your leaps are already enhanced by the Hare spirits.\n\r", ch);
        return;
        }

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_haresleap;
    af.level        = ch->pcdata->rank;
    af.duration     = ch->pcdata->gnosis[PERM]*2;
    af.modifier     = 0;
    af.location     = APPLY_NONE;
    af.bitvector    = 0;
    affect_to_char(ch, &af);
    sendch("You pay homage to the Hare spirits and you feel your legs surging with strength!\n\r", ch);
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
//“Scent of Sight”
//wolf spirit
//Roll: Perception + Primal Urge (Difficulty based on sector.)
//Allows the garou to compensate for her vision completely using her sense of smell. Negates the effects of blindness, fireblindness, can see in darkness, can see invisible and hidden. Outdoors, woods, etc, low difficulty. cities, indoors, high difficulty. Very short duration, long recovery (2-3 ticks, 10-12 ticks)
//Cost: None
void spell_gift_scentofsight( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Sense the Unnatural”
//Servant of Gaia (any spirit not a bane, wyldling or weaver construct)
//Roll: Perception + Enigmas (diff 7)
//The werewolf can sense any unnatural presence and determine it’s approximate strength and type. (Allows the garou to get a sense of any supernatural creatures in the room. More successes = more information. 1 success “You sense there is certainly something amiss around you.” 4 successess “You sense that Dracula is a very ancient and powerful vampire. You feel the crackle of magical energy emanating from Merlin!”
void spell_gift_sensetheunnatural( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank 3
//“Devil’s Child”
//wolf spirit
//Roll:Manipulation + Primal Urge (diff target willpower)
//Wolves are often seen as the offspring of the devil himself, and while they don’t fully understand that implication the lupus are happy to capitalize on the fear it inspires. The garou lets loose a fierce snarl that terrifies and cows opponents. (Staredown affect. forces target to stop fighting and flee.)
//Cost None. (Optionally, can add a ‘gnosis’ option to this gift to spend a gnosis point and affect every person in the room.)
void spell_gift_devilschild( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
//“Name the Spirit”
//cat spirits
//Roll: perception + occult (diff 8)
//A werewolf with this gift becomes familiar with the denizens of the Umbra. She can sense the type and approximate trait level of spirits. (The garou can get detailed information about a spirit type mob. More successes = more information.)
//Cost: 1 Willpower
void spell_gift_namethespirit( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_scentofrunningwater( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Snow Running”
//arctic hare spirit
//Roll:None
//The deep snows of winter in much of Europe bring privation at best and starvation at worst to the unprepared and snowbound. This gift allows the garou to travel with ease across the surface of ice and snow. (Halve movement costs based on terrain. lasts a day)
//Cost: 1 gnosis
void spell_gift_snowrunning( int sn, int level, CHAR_DATA *ch, void *vo, int target){
        AFFECT_DATA af;

        if (is_affected(ch, sn))
        {
        sendch("You are already able to travel any terrain with ease.\n\r", ch);
        return;
        }
        if (ch->pcdata->gnosis[TEMP] < 1)
        {
            send_to_char("You do not possess the spiritual reserves to activate this gift.\n\r", ch);
        }

        ch->pcdata->gnosis[TEMP]--;
        af.where     = TO_AFFECTS;
        af.type      = sn;
        af.level     = ch->level;
        af.modifier  = 0;
        af.location  = APPLY_NONE;
        af.bitvector    = 0;
        af.duration  = 12 + ch->pcdata->gnosis[PERM];
        affect_to_char( ch, &af );
        send_to_char( "You feel light and sure-footed, like you could pass any terrain with ease.\n\r", ch );
        act("$n seems lighter and more sure-footed.",ch,NULL,ch,TO_NOTVICT);

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
void spell_gift_senseofprey( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_spiritspeech( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_commandspirit( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Sight from Beyond”
//crow / snake spirit
//wits + occult diff 7
//the garou becomes an oracle, she receives visions that strike without warning
void spell_gift_sightfrombeyond( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//Rank Three
//
//“Expel Spirit”
//any incarna avatar
//man + intimidation (diff spirit wp) must concentrate for 3 turns
//the garou forces a spirit to go from place to place, to get it out of a fetish diff is 9
void spell_gift_expelspirit( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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


    if ( is_affected( ch, gsn_gift_pulseoftheinvisible ) )
    {
        send_to_char("You shift your sight back to the mundane.\n\r",ch);
        affect_strip(ch,gsn_gift_pulseoftheinvisible);
    }
    else
        send_to_char("You open your mind to seeing the unseen.\n\r",ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_gift_pulseoftheinvisible;
    af.level     = level;
    af.duration  = (ch->pcdata->gnosis[PERM]*2) + 10;
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
void spell_gift_scentofthetrueform( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_strengthofpurpose( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_warriorseye( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_beastspirit( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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

    if (is_affected(ch, gsn_laryngitis))
    {
      act( "You attempt a savage howl, but your enflamed throat causes it to come out broken and disjointed.", ch, NULL, NULL, TO_CHAR );
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
            act( "You feel invigorated by $n's howl and join in the praise to Luna.", ch, NULL, gch, TO_VICT );
        }
        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_callofthewyld;
        af.level     = level;
        af.duration  = 5 + level/4;
        af.location  = APPLY_CS_STR;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_callofthewyld;
        af.level     = level;
        af.duration  = 5 + level/4;
        af.location  = APPLY_CS_STA;
        af.modifier  = 2;
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_callofthewyld;
        af.level     = level;
        af.duration  = 5 + level/4;
        af.location  = APPLY_AC;
        af.modifier  = -(level);
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_gift_callofthewyld;
        af.level     = level;
        af.duration  = 5 + level/4;
        af.location  = APPLY_SAVES;
        af.modifier  = -(level/10);
        af.bitvector = 0;
        affect_to_char( gch, &af );
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
void spell_gift_distractions( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//Rank Three
//
//“Eye of the Asp”
//venomous snake spirit
//apperance + enigmas diff victom’s wp
//3 successes needed to bring target to the garou
void spell_gift_eyeoftheasp( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}
//
//“Song of Rage”
//wolverine spirit
//manipulation + leadership diff target’s wp
//victim flies into a rage or frenzy for one turn per success
void spell_gift_songofrage( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
    OBJ_DATA *obj;

    if ( ch->pcdata->shiftform == HOMID)
    {
        send_to_char( "Turn what to silver?\n\r", ch );
        return;
    }

    if (is_affected(ch,gsn_gift_razorclaws) || is_affected(ch,gsn_gift_silverclaws))
    {
        send_to_char("You can't seem to turn your claws to silver.\n\r",ch);
        return;
    }

    af.where        = TO_OBJECT;
    af.type         = gsn_gift_silverclaws;
    af.level        = ch->level;
    af.duration     = ch->level/2;
    af.location     = APPLY_NONE;
    af.modifier     = 0;
    af.bitvector    = ITEM_SILVER;
    affect_to_obj(obj,&af);

    af.where        = TO_AFFECTS;
    af.type         = gsn_gift_silverclaws;
    af.level        = ch->level;
    af.duration     = ch->level/2;
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
void spell_gift_mansskin( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_curseoftheaeolus( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_scentofthehoneycomb( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_haltthecowardsflight( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_distractthefool( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_faerielight( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
//Rank 2
//Rank 3
//Rank 4
//Rank 5

//Shadow Lords

//Rank 1
void spell_gift_auraofconfidence( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_clapofthunder( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
//Rank 2
//Rank 3
//Rank 4
//Rank 5

//Silver Fangs
//Rank 1
void spell_gift_eyeofthefalcon( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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
void spell_gift_empathy( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Luna's Armor - duplicate gift, as ChildrenofGaia

//Rank 3
void spell_gift_exceptionalswordplay( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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

  if (ch->willpower < 1)
  {
    send_to_char("You do not have the Willpower to commune with spirits right now.\n\r", ch);
    return;
  }

  ch->willpower--;
  success = godice(get_attribute(ch,CSATTRIB_MANIPULATION) + vch->csabilities[CSABIL_CRAFTS], 7);

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

void spell_gift_giftofsalt( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Persuasion - duplicate gift, same as Homid

//Rank 2
void spell_gift_maskthepredator( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 3
//Reshape Object - duplicate gift, as HOMID

void spell_gift_tongues( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

//Rank 4
void spell_gift_attunement( int sn, int level, CHAR_DATA *ch, void *vo, int target){
    return;
}

void spell_gift_doppelganger( int sn, int level, CHAR_DATA *ch, void *vo, int target){
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


    if(IS_NPC(ch)) return;

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("You are not one of Gaia's chosen!\n\r",ch);
        return;
    }


    int i;
    int col = 1;
    BUFFER *buffer;
    buffer = new_buf();
    if (str_cmp(argument, "all"))
	{
		send_to_char("Your Gifts from Gaia:\n\r",ch);

		for (i = 0; i < MAX_GIFT; i++)
		{
			if (ch->pcdata->gift[i] == 0)
				continue;
			else {
				if (col < 2)
                    if (gift_table[ch->pcdata->gift[i]].level == 0)
                    sprintf(buf, "{r%-30s{x", capitalize(gift_table[ch->pcdata->gift[i]].name));
                    else
					sprintf(buf, "%-30s", capitalize(gift_table[ch->pcdata->gift[i]].name));
				else if (col == 2)
				{
                    if (gift_table[ch->pcdata->gift[i]].level == 0)
                    sprintf(buf, "{r%-30s{x\n\r", capitalize(gift_table[ch->pcdata->gift[i]].name));
                    else
					sprintf(buf, "%s\n\r", capitalize(gift_table[ch->pcdata->gift[i]].name));
					col = 0;
					}

				add_buf(buffer, buf);
				col++;
			}
		}
	} else {
	    send_to_char("Gifts you may learn:\n\r",ch);

		for (i = 1; i < MAX_GIFTS_CODED; i++)
		{
			if (ch->pcdata->rank < gift_table[i].rank ||
					(gift_table[i].breed != ch->pcdata->breed &&
					gift_table[i].auspice != ch->pcdata->auspice &&
					gift_table[i].tribe != ch->pcdata->tribe))
				continue;
				int gn;
				bool learned = FALSE;
				for (gn = 0; gn < MAX_GIFT; gn++)
					if (ch->pcdata->gift[gn] == i) learned = TRUE;

				if (learned)
					continue;

			else {
				if (col < 2)
                {
                    if (gift_table[i].level == 0)
                    sprintf(buf, "{r%-30s{x", capitalize(gift_table[i].name));
                    else
					sprintf(buf, "%-30s", capitalize(gift_table[i].name));
				}
                    else
				{
                    if (gift_table[i].level == 0)
                    sprintf(buf, "{r%-30s{x\n\r", capitalize(gift_table[i].name));
                    else
					sprintf(buf, "%s\n\r", capitalize(gift_table[i].name));
					col = 0;
					}

				add_buf(buffer, buf);
				col++;
			}
		}
	}

    page_to_char(buf_string(buffer), ch);
    send_to_char("\n\rNote: Gifts marked in {rred{x are not currently coded.\n\r", ch);
    return;
}
