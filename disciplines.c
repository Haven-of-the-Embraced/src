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

/* Local prototype for can_use_disc(). If we need it in other files, move it to merc.h */
bool can_use_disc(CHAR_DATA *ch, int disc, int dot, int blood, bool message);

void do_feralspeech(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if (!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r", ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char("To what creature do you wish to speak to?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "No creature here by that name.\n\r", ch );
        return;
    }
        if (!IS_NPC(victim))
        {
                send_to_char("Disabled on players due to the potential for abuse.\n\r", ch);
                return;
        }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (ch->pcdata->discipline[ANIMALISM] < 1)
    {
        send_to_char( "You are not skilled enough in Animalism!.\n\r", ch );
        return;
    }
    if (!str_prefix(argument,"delete"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"ic"))
        {
                send_to_char("Nope!\n\r",ch);
                return;
    }
    if (!str_prefix(argument, "quit"))
        {
                send_to_char("I don't think so!.\n\r", ch);
                return;
        }
    if (!str_prefix(argument, "follow"))
    {
        send_to_char("You cannot do that.\n\r", ch);
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "You get a headache trying to control yourself.\n\r", ch );
        return;
    }

    if ( IS_IMMORTAL(victim))
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }
    if ( ch->pblood < 20 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 10;
    if(victim->race == race_lookup("vampire") && ch->gen > victim->gen)
    {
        send_to_char( "You cannot command your betters!\n\r", ch );
        sprintf( buf, "%s foolishly attempts to dominate your mind!\n\r", ch->name );
        send_to_char(buf,victim);
        return;
    }
    if(IS_SET(victim->form,FORM_SENTIENT))
    {
        send_to_char("This creature is too intelligent to control.\n\r",ch);
        return;
    }
    if(victim->race != race_lookup("bat") && victim->race != race_lookup("bear") &&
        victim->race != race_lookup("cat") && victim->race != race_lookup("centipede") &&
        victim->race != race_lookup("dog") && victim->race != race_lookup("fido") &&
        victim->race != race_lookup("fox") && victim->race != race_lookup("lizard") &&
        victim->race != race_lookup("pig") && victim->race != race_lookup("rabbit") &&
        victim->race != race_lookup("snake") && victim->race != race_lookup("song bird") &&
        victim->race != race_lookup("water fowl") && victim->race != race_lookup("wolf") &&
        victim->race != race_lookup("horse"))
    {
        send_to_char("You can only use this ability on animals.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   IS_SET(victim->imm_flags,IMM_CHARM))
    {
        act( "$N refuses to heed to your will.",  ch, NULL, victim, TO_CHAR );
        act( "$n stares into the eyes of $N... then frowns suddenly as $N snubs them.",  ch, NULL, victim, TO_NOTVICT );
        return;
    }
    int success;
    if ((success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_ANIMAL_KEN], 6)) < 0)
    {
        act( "$N ignores you a moment before it suddenly takes offense and attacks!",  ch, NULL, victim, TO_CHAR );
        act( "$n stares into the eyes of $N a moment... $N suddenly takes offense and attacks!",  ch, NULL, victim, TO_NOTVICT );
        multi_hit( victim, ch, TYPE_UNDEFINED );
        return;
    }else if (success == 0) {
        act( "$N refuses to heed to your will.",  ch, NULL, victim, TO_CHAR );
        act( "$n stares into the eyes of $N... then frowns suddenly as $N snubs them.",  ch, NULL, victim, TO_NOTVICT );
        return;
    }

    sprintf( buf, "$n speaks to your mind, asking you to '%s'.\n\r", argument );
    act(buf,ch,NULL,victim,TO_VICT);
    sprintf( buf, "You look into the eyes of $N and ask them to '%s'.\n\r", argument );
    act(buf,ch,NULL,victim,TO_CHAR);
    act( "$n stares into $N's eyes a moment then whispers a command.",  ch, NULL, victim, TO_NOTVICT );
    interpret( victim, argument );
    return;
}


void do_beckoning(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    AFFECT_DATA af;
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[ANIMALISM] < 2)
    {
        send_to_char( "You have not yet studied enough in the ways of Animalism.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if ( ch->pblood < 25)
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if(ch->pet != NULL)
    {
        send_to_char( "You cannot summon another pet!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_HORDE_RATS)) == NULL )
    {
        send_to_char( "Error: please inform the Coders!\n\r", ch );
        return;
    }

    dicesuccess = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_SURVIVAL], 6);

    WAIT_STATE(ch, 10);

    if (dicesuccess < 0)
    {
        ch->pblood -= 15;
        mob = create_mobile( pMobIndex );
        char_to_room( mob, ch->in_room );
        mob->level = ch->level / 4;
        mob->max_hit = ch->max_hit / 5;
        mob->hitroll = ch->hitroll / 2;
        mob->damroll = ch->damroll / 3;
        mob->hit = mob->max_hit;
        mob->short_descr = str_dup("a small group of rats");
        mob->long_descr = str_dup("A small group of rats runs amok, sniffing the area.\n\r");
        mob->name = str_dup("rats");

        SET_BIT(mob->off_flags, OFF_SECOND_ATTACK);
        SET_BIT(mob->off_flags, OFF_THIRD_ATTACK);

        act("Loud chittering can be heard as $N charges forth and attacks you!", ch, NULL, mob, TO_CHAR);
        act("Loud chittering can be heard as $N charges forth and attacks $n!", ch, NULL, mob, TO_ROOM);
        multi_hit( mob, ch, TYPE_UNDEFINED );
        return;
    }

    if (dicesuccess == 0)
    {
        act("You chitter and squeak, but your call goes unanswered.", ch, NULL, NULL, TO_CHAR);
        act("$n squeaks and chitters, calling out in vain.", ch, NULL, NULL, TO_NOTVICT);
        ch->pblood -= 15;
        return;
    }


    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );

    ch->pblood -= 15;
    mob->max_hit = ch->max_hit/5 + ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 50);
    mob->hitroll = ch->hitroll/5 + ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 5);
    mob->damroll = ch->damroll/10 + ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 2);
    mob->damage[DICE_NUMBER] = 3*ch->level/5;
    mob->damage[DICE_TYPE] = 3;
    mob->level  = ch->level;
    mob->armor[AC_PIERCE] = -(ch->level/2) * dicesuccess;
    mob->armor[AC_BASH] = -(ch->level/10) * dicesuccess;
    mob->armor[AC_SLASH] = -(ch->level/5) * dicesuccess;
    mob->armor[AC_EXOTIC] = -(ch->level/20) * dicesuccess;

    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->pcdata->discipline[ANIMALISM];
    af.duration  = (dicesuccess * 10) + 30;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if (dicesuccess > 1)
    {
        SET_BIT(mob->off_flags, OFF_SECOND_ATTACK);
        if (mob->level > 10) {
            mob->max_hit += ((ch->pcdata->discipline[ANIMALISM]) * 50);
            mob->hitroll += ((ch->pcdata->discipline[ANIMALISM]) * 10);
            mob->damroll += ((ch->pcdata->discipline[ANIMALISM]) * 2);
        }
        mob->short_descr = str_dup("a pair of rats");
        mob->long_descr = str_dup("A pair of rats scurry about, looking for food.\n\r");
        mob->name = str_dup("pair rats animalism");
    }

    if (dicesuccess > 2)
    {
        if (mob->level > 49) SET_BIT(mob->off_flags, OFF_THIRD_ATTACK);
        SET_BIT(mob->off_flags, OFF_AREA_ATTACK);
        if (mob->level > 20)
        {
            mob->max_hit += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 50);
            mob->hitroll += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 5);
            mob->damroll += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 2);
        }
        mob->short_descr = str_dup("a small group of rats");
        mob->long_descr = str_dup("A small group of rats runs about, sniffing the area.\n\r");
        mob->name = str_dup("rats animalism");
    }

    if (dicesuccess > 3)
    {
        if (mob->level > 50) SET_BIT(mob->off_flags, OFF_FOURTH_ATTACK);
        if (mob->level > 30)
        {
            mob->max_hit += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 50);
            mob->hitroll += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 10);
            mob->damroll += ((ch->pcdata->discipline[ANIMALISM]) * 2);
        }

    }

    if (dicesuccess > 4)
    {
        mob->short_descr = str_dup("a horde of rats");
        if (mob->level > 49) SET_BIT(mob->off_flags, OFF_FAST);
        af.bitvector = AFF_HASTE;
        affect_to_char( mob, &af );
        if (mob->level > 40)
        {
            mob->max_hit += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 50);
            mob->hitroll += ((ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 10);
            mob->damroll += ((ch->pcdata->discipline[ANIMALISM]) * 2);
        }
        mob->long_descr = str_dup("A horde of rats swarms the area, nipping at anything in sight.\n\r");
        mob->name = str_dup("horde rats animalism");
    }

    add_follower( mob, ch );
    mob->leader = ch;
    mob->hit = mob->max_hit;
    ch->pet = mob;
    act("Chittering and squeaking, $n coaxes $N out of hiding as they await commands from $m.", ch, NULL, mob, TO_NOTVICT);
    act("Chittering and squeaking, you coax and call out to $N as you prepare to issue your next commands.", ch, NULL, mob, TO_CHAR);

    gain_exp(ch, (ch->pcdata->discipline[ANIMALISM] + dicesuccess) * 2);
    return;
}

void do_songofserenity(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   AFFECT_DATA af;
   int success;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char("To whom do you wish to calm?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "No one here by that name.\n\r", ch );
        return;
    }
    if(!IS_NPC(victim))
    {
        send_to_char("That would be useless.\n\r",ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED(victim, AFF_CALM)) {
        send_to_char("They are already pretty calm!\n\r", ch);
        return;
    }
    if (ch->pcdata->discipline[ANIMALISM] < 3)
    {
        send_to_char( "You are not skilled enough in Animalism!.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "Your already pretty calm!\n\r", ch );
        return;
    }


    if ( IS_IMMORTAL(victim))
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }
    if ( ch->pblood < 20 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 10;
    send_to_char("You lift your voice in a sweet song of peace and serenity.\n\r",ch);
    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   ch->level+25 < victim->level)
    {
        act( "$N ignores your sweet song.",  ch, NULL, victim, TO_CHAR );
        act( "$n sings a sweet song to $N, which seems to completely ignore it..",  ch, NULL, victim, TO_NOTVICT );
        return;
    }

    WAIT_STATE( ch, 24 );
    success = godice(get_attribute(ch,MANIPULATION)+ch->csabilities[CSABIL_EMPATHY],7);
    if(success < 0)
    {
        act( "$N ignores you a moment before it suddenly takes offense and attacks!",  ch, NULL, victim, TO_CHAR );
        act( "$n sings softly to $N a moment... $N suddenly takes offense and attacks!",  ch, NULL, victim, TO_NOTVICT );
        multi_hit( victim, ch, TYPE_UNDEFINED );
        return;
    }
    if (success == 0) {
        act( "$N seems utterly unaffected by your song.", ch, NULL, victim, TO_CHAR);
        act( "$n sings a sweet song to $N, which seems to completely ignore it..",  ch, NULL, victim, TO_NOTVICT );
        return;
    }

    act("Your song seems to sway the heart of $N",ch,NULL,victim,TO_CHAR);
    act( "$n sings a soft and sweet song to $N who seems calmed by it.",  ch, NULL, victim, TO_NOTVICT );
    REMOVE_BIT(victim->act, ACT_AGGRESSIVE);

    if (success > 2) {
    affect_strip(victim, gsn_berserk);
    affect_strip(victim, gsn_vamp_frenzy);
    affect_strip(victim, gsn_garou_frenzy);
    affect_strip(victim, gsn_thaumaturgy_frenzy);
    affect_strip(victim, gsn_vigor);
    affect_strip(victim, gsn_zeal);
    }

    af.where = TO_AFFECTS;
    af.type = skill_lookup("calm");
    af.level = success*20;
    af.duration = success*4;
    af.location = APPLY_HITROLL;
    if (!IS_NPC(victim))
      af.modifier = -success * 10;
    else
      af.modifier = -success * 25;
    af.bitvector = AFF_CALM;
    affect_to_char(victim,&af);

    af.location = APPLY_DAMROLL;
    affect_to_char(victim,&af);

    af.location = APPLY_AC;
    af.modifier = success * 50;
    affect_to_char(victim, &af);
    return;
}
void do_drawingoutthebeast( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int success;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[ANIMALISM] < 5)
    {
    send_to_char( "You are not skilled enough with the powers of Animalism.\n\r", ch );
    return;
    }

    if (ch->pblood < 30)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Draw the beast to the surface of whom?\n\r", ch );
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }
    success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_ANIMAL_KEN], 8);

    if (success < 2)
    {
        send_to_char("You are unable to draw their beastial nature to the surface.\n\r", ch);
        return;
    }

    ch->pblood -= 20;
    act( "$n stares at $N. $N suddenly starts to twitch then seems to enter a Frenzy!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n stares at you... you are suddenly overcome by Frenzy!",  ch, NULL, victim, TO_VICT );
    act( "You draw forth $N's beast to the surface!",  ch, NULL, victim, TO_CHAR );
    thaumaturgy_frenzy( gsn_thaumaturgy_frenzy, success, victim, victim, TARGET_CHAR);
    multi_hit( victim, ch, TYPE_UNDEFINED );
    WAIT_STATE(ch, PULSE_VIOLENCE*2);
    return;
}

void do_quelltheherd(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   CHAR_DATA *vict_next;
   AFFECT_DATA af;
   int success;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[ANIMALISM] < 6)
    {
        send_to_char( "You are not skilled enough in Animalism!.\n\r", ch );
        return;
    }



    if ( ch->pblood < 40 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 30;
    send_to_char("You lift your voice in a sweet song of peace and serenity.\n\r",ch);

    WAIT_STATE( ch, 24 );
    success = godice(get_attribute(ch,STRENGTH)+ch->csabilities[CSABIL_INTIMIDATION],7);

    if (success == 0) {
        act( "And it seems to have no effect..", ch, NULL, victim, TO_CHAR);
        act( "$n sings a sweet song of peace and serenity, which seems to go completely ignored..",  ch, NULL, victim, TO_NOTVICT );
        return;
    }

    if(success < 0)
        {
        for ( victim = char_list; victim != NULL; victim = vict_next )
        {
            vict_next       = victim->next;
            if(!IS_NPC(victim) || victim->in_room == NULL || victim->pIndexData->pShop != NULL || victim == ch)
            continue;

            if ( victim->in_room == ch->in_room )
                {
                    act( "Instead of quelling the herd your song enrages $N!",  ch, NULL, victim, TO_CHAR );
                    act( "$n sings a soft, calming song... which drives $N to attack!",  ch, NULL, victim, TO_NOTVICT );
                    multi_hit( victim, ch, TYPE_UNDEFINED);
                }
                continue;
            }
            return;
        }


    act( "$n sings a soft and sweet song, a sense of calm descending over the room.",  ch, NULL, victim, TO_NOTVICT );

    for ( victim = char_list; victim != NULL; victim = vict_next )
    {
        vict_next       = victim->next;
        if(!IS_NPC(victim))
            continue;
        if ( victim->in_room == NULL || victim == ch)
            continue;
        if ( victim->in_room == ch->in_room && victim->level < ch->level+25 )
        {

            REMOVE_BIT(victim->act, ACT_AGGRESSIVE);
            act("Your song seems to sway the heart of $N.",ch,NULL,victim,TO_CHAR);
            if (success > 2) {
                affect_strip(victim, gsn_berserk);
                affect_strip(victim, gsn_vamp_frenzy);
                affect_strip(victim, gsn_garou_frenzy);
                affect_strip(victim, gsn_thaumaturgy_frenzy);
                affect_strip(victim, gsn_vigor);
                affect_strip(victim, gsn_zeal);
            }

            af.where = TO_AFFECTS;
            af.type = skill_lookup("calm");
            af.level = success*20;
            af.duration = success*4;
            af.bitvector = AFF_CALM;

            af.location = APPLY_HITROLL;
            af.modifier = -success * 25;
            affect_to_char(victim,&af);

            af.location = APPLY_DAMROLL;
            affect_to_char(victim,&af);

            af.location = APPLY_AC;
            af.modifier = success * 50;
            affect_to_char(victim, &af);


            continue;
        }
    }
    return;
}

void do_reveal(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if (is_affected(ch, gsn_reveal))
    {
        send_to_char("You force your senses to dull, leaving the world of the extraordinary and returning to the realm of the normal.\n\r", ch);
        affect_strip(ch, gsn_reveal);
        return;
    }

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(!can_use_disc(ch,AUSPEX,1,20,TRUE))
        return;
/*  if(ch->pcdata->discipline[AUSPEX] < 1)
    {
        send_to_char( "You are not trained in Auspex!\n\r", ch );
        return;
    }
*/
    if (ch->pblood < 20)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    ch->pblood -= 10;

    send_to_char( "You expand your five senses to inhuman levels, indulging in sensations no mere mortal could even begin to comprehend.\n\r", ch);

        af.where     = TO_AFFECTS;
        af.type      = gsn_reveal;
        af.level     = ch->pcdata->discipline[AUSPEX];
        af.duration  = ch->level/2 + ch->pcdata->discipline[AUSPEX]*10;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );

    if (ch->pcdata->discipline[AUSPEX] >= 2 && !IS_AFFECTED(ch,AFF_DETECT_HIDDEN))
    {
        af.where     = TO_AFFECTS;
        af.type      = gsn_reveal;
        af.level     = ch->pcdata->discipline[AUSPEX];
        af.duration  = ch->level/2 + ch->pcdata->discipline[AUSPEX]*10;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_HIDDEN;
        affect_to_char( ch, &af );
    }

    if (ch->pcdata->discipline[AUSPEX] >= 3 && !IS_AFFECTED(ch,AFF_DETECT_INVIS))
    {
        af.where     = TO_AFFECTS;
        af.type      = gsn_reveal;
        af.level     = ch->pcdata->discipline[AUSPEX];
        af.duration  = ch->level/2 + ch->pcdata->discipline[AUSPEX]*10;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_INVIS;
        affect_to_char( ch, &af );
    }
    return;
}


void do_sense( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *was_in;
    EXIT_DATA *pexit;
    int door;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[AUSPEX] < 1)
    {
    send_to_char( "You are not trained enough in Auspex!\n\r", ch );
    return;
    }

    if (ch->pblood < 11)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
    send_to_char("Sense what?\n\r", ch );
    return;
    }

         if ( !str_prefix( arg1, "n" ) || !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "e" ) || !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "s" ) || !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "w" ) || !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "u" ) || !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "d" ) || !str_prefix( arg1, "down"  ) ) door = 5;
    else
    {
    send_to_char( "You can only sense directions.\n\r", ch );
    return;
    }

    if ( (pexit = ch->in_room->exit[door] ) == NULL )
    {
    send_to_char( "You sense nothing in that direction.\n\r", ch );
    return;
    }

    to_room = ch->in_room->exit[door]->u1.to_room;
    was_in = ch->in_room;

    char_from_room( ch );
    char_to_room( ch, to_room );

    ch->pblood -= 1;
    do_look( ch, "" );

    char_from_room( ch );
    char_to_room( ch, was_in );
}


void do_auraperception( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int success;
    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[AUSPEX] < 2)
    {
    send_to_char( "You are not trained enough in Auspex!\n\r", ch );
    return;
    }

    if (ch->pblood < 20)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0')
    {
        send_to_char( "View whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "View whom?\n\r", ch );
        return;
    }

    success = godice(get_attribute(ch, PERCEPTION) + ch->csabilities[CSABIL_EMPATHY], 8);

    ch->pblood -= 10;

    if (success < 0)
    { send_to_char("You are unable to read their aura.\n\r", ch);
    return;
    }

    sprintf( buf, "%s is %d years old.\n\r", victim->name, get_age(victim));
    send_to_char( buf, ch );

    sprintf(buf, "%s is a %s %s.\n\r",
    victim->name,
    victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
    race_table[victim->race].name);
    send_to_char(buf,ch);

    if(success > 1)
    {
        sprintf( buf,
        "They have %d/%d hit, %d/%d mana and %d/%d movement.\n\r",
        victim->hit,  victim->max_hit,
        victim->mana, victim->max_mana,
        victim->move, victim->max_move);
        send_to_char( buf, ch );

    }
    if(success > 2)
    {
        switch ( victim->position )
        {
        case POS_TORPOR:
        send_to_char( "They are in Torpor.\n\r",        ch );
        break;
        case POS_DEAD:
        send_to_char( "They are DEAD!!\n\r",        ch );
        break;
        case POS_MORTAL:
        send_to_char( "They are mortally wounded.\n\r", ch );
        break;
        case POS_INCAP:
        send_to_char( "They are incapacitated.\n\r",    ch );
        break;
        case POS_STUNNED:
        send_to_char( "They are stunned.\n\r",      ch );
        break;
        case POS_SLEEPING:
        send_to_char( "They are sleeping.\n\r",     ch );
        break;
        case POS_RESTING:
        send_to_char( "They are resting.\n\r",      ch );
        break;
        case POS_SITTING:
        send_to_char( "They are sitting.\n\r",      ch );
        break;
        case POS_STANDING:
        send_to_char( "They are standing.\n\r",     ch );
        break;
        case POS_FIGHTING:
        send_to_char( "They are fighting.\n\r",     ch );
        break;
        }

        sprintf( buf,"Armor: pierce: %d  bash: %d  slash: %d  magic: %d\n\r",
            GET_AC(victim,AC_PIERCE),
            GET_AC(victim,AC_BASH),
            GET_AC(victim,AC_SLASH),
            GET_AC(victim,AC_EXOTIC));
            send_to_char(buf,ch);

        sprintf( buf, "They have: Hitroll: %d  Damroll: %d.\n\r",
            GET_HITROLL(victim), GET_DAMROLL(victim) );
        send_to_char( buf, ch );

        sprintf( buf, "Their alignment is %d.\n\r", victim->alignment );
        send_to_char( buf, ch );
    }
    if(success > 3)
    {
        if(IS_VAMP(victim))
        {
        sprintf(buf, "Their Generation is %d.\n\r", victim->gen);
        send_to_char(buf,ch);
        sprintf(buf, "Their Sire is %s.\n\r", victim->sire);
        send_to_char(buf,ch);
        sprintf(buf, "They have %d Blood Points.\n\r", victim->pblood);
        send_to_char(buf,ch);
        }
    }
    return;
}

// New Auspex 5 by Gilean
void do_project(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
    OBJ_DATA *vamp_corpse;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *bag;
    OBJ_DATA *bag_next;
    AFFECT_DATA af;
    char name[MSL];
    int success;

	if(!can_use_disc(ch,AUSPEX,5,0,TRUE))
	{
		send_to_char("Your mastery of Auspex is not high enough yet.\n\r", ch);
		return;
	}

	sprintf(name, "%sastralcorpse", ch->name);
	vamp_corpse = get_obj_world( ch, name, TRUE);

	if (is_affected(ch, gsn_astralprojection) && vamp_corpse == NULL ) // You already got the corpse. Use it.
	{
		send_to_char("Your anchored body is missing, contact an immortal immediately!\n\r", ch);
		return;
	}


	if (is_affected(ch, gsn_astrallylost))
	{
		send_to_char("You are still hopelessly lost and cannot find your ephemeral cord to link back to your body.\n\r", ch);
		return;
	}

	if (IS_AFFECTED2(ch, AFF2_UMBRA))
	{
		if (!is_affected(ch, gsn_astralprojection))
		{
			send_to_char("You did not enter the Umbra through Psychic Projection, and have no body anchoring for your return.  You must find another way back.\n\r", ch);
			return;
		}

		act("$N grabs a shining silver cord attached to $s body, and zips away following it into the distance.", ch, NULL, NULL, TO_NOTVICT);
        location = vamp_corpse->in_room;
        char_from_room( ch );
		char_to_room( ch, location );
		extract_obj(vamp_corpse);
        REMOVE_BIT(ch->affected2_by, AFF2_UMBRA);
        affect_strip(ch, gsn_astralprojection);

		act( "You follow your ephemeral cord back to the physical realm and reenter your torpid shell of a body.",  ch, NULL, NULL, TO_CHAR);
		act( "$n stirs slowly and opens $s eyes.",  ch, NULL, NULL, TO_NOTVICT );

		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			REMOVE_BIT(obj->extra_flags,ITEM_UMBRA);
			if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC)
			{
				for ( bag = obj->contains; bag != NULL; bag = bag_next )
				{
					bag_next = bag->next_content;
					REMOVE_BIT(bag->extra_flags, ITEM_UMBRA);
				}
			}
		}

		do_function(ch, &do_look, "auto" );
		return;
    }

    if (ch->position > POS_SITTING)
	{
		send_to_char("You can't concentrate enough. You must be relaxed to do this! \n\r", ch);
		return;
	}

	if (ch->cswillpower < 1)
	{
		send_to_char("You do not have the strength of will it requires to force your spirit into the astral plane.\n\r", ch);
		return;
	}

	ch->cswillpower--;

	location = ch->in_room;
	vamp_corpse  = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
	vamp_corpse->timer   = 0;
	REMOVE_BIT(vamp_corpse->wear_flags, ITEM_TAKE);
	vamp_corpse->cost = 0;
	vamp_corpse->level = ch->level;

	sprintf( buf, vamp_corpse->short_descr, ch->name );
	free_string( vamp_corpse->short_descr );
	vamp_corpse->short_descr = str_dup( buf );

	sprintf( buf, vamp_corpse->description, ch->name );
	free_string( vamp_corpse->description );
	vamp_corpse->description = str_dup( buf );

	free_string( vamp_corpse->name );
	vamp_corpse->name = str_dup( name );

	obj_to_room(vamp_corpse, location);

	success = godice(get_attribute(ch, PERCEPTION) + ch->csabilities[CSABIL_OCCULT], get_gauntlet(ch));

	if (success < 0)
	{
		act("You force your way into the Astral Realm, but in the process snap your psychic cord link to your body and are flung through the astral plane!", ch, NULL, NULL, TO_CHAR);
		act("$n jerks violently backward, and drops to the ground in a torpid state, unmoving and unaware of the world around $s.", ch, NULL, NULL, TO_NOTVICT);
		location = get_random_room(ch);
		char_from_room( ch );
		char_to_room( ch, location );
		SET_BIT(ch->affected2_by, AFF2_UMBRA);
		for ( obj = ch->carrying; obj != NULL; obj = obj_next )
		{
			obj_next = obj->next_content;
			SET_BIT(obj->extra_flags,ITEM_UMBRA);
			if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC)
			{
				for ( bag = obj->contains; bag != NULL; bag = bag_next )
				{
					bag_next = bag->next_content;
					SET_BIT(bag->extra_flags, ITEM_UMBRA);
				}
			}
		}

		af.where	= TO_AFFECTS;
		af.type		= gsn_astrallylost;
		af.level	= ch->level;
		af.duration	= 20 - ch->cswillpower;
		af.modifier	= 0;
		af.location	= 0;
		af.bitvector	= 0;
		affect_to_char( ch, &af );

		af.where	= TO_AFFECTS;
		af.type		= gsn_astralprojection;
		af.level	= ch->level;
		af.duration	= -1;
		af.modifier	= 0;
		af.location	= 0;
		af.bitvector	= 0;
		affect_to_char( ch, &af );

		do_function(ch, &do_look, "auto" );
		return;
	}

	if (success == 0)
	{
		act("You struggle for a moment, but cannot muster enough force to eject your psyche from your body.  After a brief moment, you realize the effort is wasted.", ch, NULL, NULL, TO_CHAR);
		extract_obj(vamp_corpse);
		WAIT_STATE(ch, 24);
		return;
	}

	act("Your astral form slides easily out of your body, leaving only an empty vessel on the ground as you cross the Gauntlet into the Astral Plane.", ch, NULL, NULL, TO_CHAR);
	act("$n's body slowly slumps to the ground, eyes closed and lying in a torpid, yet peaceful state.", ch, NULL, NULL, TO_NOTVICT);
	SET_BIT(ch->affected2_by, AFF2_UMBRA);
	for ( obj = ch->carrying; obj != NULL; obj = obj_next )
	{
		obj_next = obj->next_content;
		SET_BIT(obj->extra_flags,ITEM_UMBRA);
		if (obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_CORPSE_PC)
		{
			for ( bag = obj->contains; bag != NULL; bag = bag_next )
			{
				bag_next = bag->next_content;
				SET_BIT(bag->extra_flags, ITEM_UMBRA);
			}
		}
	}

	af.where	= TO_AFFECTS;
	af.type		= gsn_astralprojection;
	af.level	= ch->level;
	af.duration	= -1;
	af.modifier	= 0;
	af.location	= 0;
	af.bitvector	= 0;
	affect_to_char( ch, &af );

	gain_exp(ch, success * 10);
	do_function(ch, &do_look, "auto" );
	return;
}

void do_touch( CHAR_DATA *ch, char *argument )
{

    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    int success;

    if (IS_NPC(ch)) return;

    one_argument( argument, arg );


    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if(!can_use_disc(ch,AUSPEX,3,15,TRUE))
                return;
/*  if (ch->pcdata->discipline[AUSPEX] < 3)
    {
        send_to_char( "You are not skilled enough in your powers of Auspex!.\n\r", ch );
        return;
    }
*/
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if ( ch->pblood < 15)
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }


    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL)
    {
        send_to_char( "Touch what?.\n\r", ch );
        return;
    }

    ch->pblood -= 5;

    WAIT_STATE(ch, 36);
    act( "$n gently touches $p.",  ch, obj, NULL, TO_ROOM );
    act( "You gently touch $p and gain insight on the object.",  ch, obj, NULL, TO_CHAR );

    success = godice(get_attribute(ch,PERCEPTION)+ch->csabilities[CSABIL_EMPATHY],7);

    if (success < 1)
    {
        send_to_char("You fail to discern any impressions left on this item.\n\r", ch);
        return;
    }

    sprintf( buf,
        "Object '%s' is type %s, extra flags %s.\n\rWeight is %d, value is %d, level is %d.\n\r",
        obj->name,
        item_name(obj->item_type),
        extra_bit_name( obj->extra_flags ),
        obj->weight / 10,
        obj->cost,
        obj->level
        );
    send_to_char( buf, ch );
    if (success > 1)
    {
        switch ( obj->item_type )
        {
            case ITEM_SCROLL:
            case ITEM_POTION:
            case ITEM_PILL:
                sprintf( buf, "Level %d spells of:", obj->value[0] );
                send_to_char( buf, ch );
                if ( obj->value[1] >= 0 && obj->value[1] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[obj->value[1]].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[2] >= 0 && obj->value[2] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[obj->value[2]].name, ch );
                    send_to_char( "'", ch );
                }

                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[obj->value[3]].name, ch );
                    send_to_char( "'", ch );
                }

                if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
                {
                    send_to_char(" '",ch);
                    send_to_char(skill_table[obj->value[4]].name,ch);
                    send_to_char("'",ch);
                }

                send_to_char( ".\n\r", ch );
            break;

            case ITEM_WAND:
            case ITEM_STAFF:
                sprintf( buf, "Has %d charges of level %d",
                obj->value[2], obj->value[0] );
                send_to_char( buf, ch );
                if ( obj->value[3] >= 0 && obj->value[3] < MAX_SKILL )
                {
                    send_to_char( " '", ch );
                    send_to_char( skill_table[obj->value[3]].name, ch );
                    send_to_char( "'", ch );
                }
                send_to_char( ".\n\r", ch );
            break;
            case ITEM_DRINK_CON:
                sprintf(buf,"It holds %s-colored %s.\n\r",
                liq_table[obj->value[2]].liq_color,
                liq_table[obj->value[2]].liq_name);
                send_to_char(buf,ch);
            break;

            case ITEM_CONTAINER:
                sprintf(buf,"Capacity: %d#  Maximum weight: %d#  flags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
                send_to_char(buf,ch);
                if (obj->value[4] != 100)
                {
                    sprintf(buf,"Weight multiplier: %d%%\n\r",
                    obj->value[4]);
                    send_to_char(buf,ch);
                }
            break;

            case ITEM_WEAPON:
                send_to_char("Weapon type is ",ch);
                switch (obj->value[0])
                {
                    case(WEAPON_EXOTIC) : send_to_char("exotic.\n\r",ch);   break;
                    case(WEAPON_SWORD)  : send_to_char("sword.\n\r",ch);    break;
                    case(WEAPON_DAGGER) : send_to_char("dagger.\n\r",ch);   break;
                    case(WEAPON_SPEAR)  : send_to_char("spear/staff.\n\r",ch);  break;
                    case(WEAPON_MACE)   : send_to_char("mace/club.\n\r",ch);    break;
                    case(WEAPON_AXE)    : send_to_char("axe.\n\r",ch);      break;
                    case(WEAPON_FLAIL)  : send_to_char("flail.\n\r",ch);    break;
                    case(WEAPON_WHIP)   : send_to_char("whip.\n\r",ch);     break;
                    case(WEAPON_POLEARM): send_to_char("polearm.\n\r",ch);  break;
                    case(WEAPON_LANCE): send_to_char("lance.\n\r",ch);  break;
                default     : send_to_char("unknown.\n\r",ch);  break;
                }
                if (obj->pIndexData->new_format)
                    sprintf(buf,"Damage is %dd%d (average %d).\n\r",
                    obj->value[1],obj->value[2],
                    (1 + obj->value[2]) * obj->value[1] / 2);
                else
                    sprintf( buf, "Damage is %d to %d (average %d).\n\r",
                    obj->value[1], obj->value[2],
                    ( obj->value[1] + obj->value[2] ) / 2 );
                send_to_char( buf, ch );
                if (obj->value[4])  /* weapon flags */
                {
                    sprintf(buf,"Weapons flags: %s\n\r",weapon_bit_name(obj->value[4]));
                    send_to_char(buf,ch);
                }
            break;
            case ITEM_ARMOR:
                sprintf( buf,
                "Armor class is %d pierce, %d bash, %d slash, and %d vs. magic.\n\r",
                obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
                send_to_char( buf, ch );
            break;
        }
    }
    if (success > 2)
    {
        if (!obj->enchanted)
            for ( paf = obj->pIndexData->affected; paf != NULL; paf = paf->next )
            {
                if ( paf->location != APPLY_NONE && paf->modifier != 0 )
                {
                    sprintf( buf, "Affects %s by %d.\n\r",
                    affect_loc_name( paf->location ), paf->modifier );
                    send_to_char(buf,ch);
                    if (paf->bitvector)
                    {
                        switch(paf->where)
                        {
                            case TO_AFFECTS:
                                sprintf(buf,"Adds %s affect.\n",
                                affect_bit_name(paf->bitvector));
                            break;
                            case TO_OBJECT:
                                sprintf(buf,"Adds %s object flag.\n",
                                extra_bit_name(paf->bitvector));
                            break;
                            case TO_IMMUNE:
                                sprintf(buf,"Adds immunity to %s.\n",
                                imm_bit_name(paf->bitvector));
                            break;
                            case TO_RESIST:
                                sprintf(buf,"Adds resistance to %s.\n\r",
                                imm_bit_name(paf->bitvector));
                            break;
                            case TO_VULN:
                                sprintf(buf,"Adds vulnerability to %s.\n\r",
                                imm_bit_name(paf->bitvector));
                            break;
                            default:
                                sprintf(buf,"Unknown bit %d: %d\n\r",
                                paf->where,paf->bitvector);
                            break;
                        }
                        send_to_char( buf, ch );
                    }
                }
            }

            for ( paf = obj->affected; paf != NULL; paf = paf->next )
            {
                if ( paf->location != APPLY_NONE && paf->modifier != 0 )
                {
                    sprintf( buf, "Affects %s by %d",
                    affect_loc_name( paf->location ), paf->modifier );
                    send_to_char( buf, ch );
                    if ( paf->duration > -1)
                        sprintf(buf,", %d hours.\n\r",paf->duration);
                    else
                        sprintf(buf,".\n\r");
                        send_to_char(buf,ch);
                    if (paf->bitvector)
                    {
                        switch(paf->where)
                        {
                            case TO_AFFECTS:
                                sprintf(buf,"Adds %s affect.\n",
                                affect_bit_name(paf->bitvector));
                            break;
                            case TO_OBJECT:
                                sprintf(buf,"Adds %s object flag.\n",
                                extra_bit_name(paf->bitvector));
                            break;
                            case TO_WEAPON:
                                sprintf(buf,"Adds %s weapon flags.\n",
                                weapon_bit_name(paf->bitvector));
                            break;
                            case TO_IMMUNE:
                                sprintf(buf,"Adds immunity to %s.\n",
                                imm_bit_name(paf->bitvector));
                            break;
                            case TO_RESIST:
                                sprintf(buf,"Adds resistance to %s.\n\r",
                                imm_bit_name(paf->bitvector));
                            break;
                            case TO_VULN:
                                sprintf(buf,"Adds vulnerability to %s.\n\r",
                                imm_bit_name(paf->bitvector));
                            break;
                            default:
                                sprintf(buf,"Unknown bit %d: %d\n\r",
                                paf->where,paf->bitvector);
                            break;
                        }
                    send_to_char(buf,ch);
                    }
                }
            }
    }
return;
}


void do_telepathy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int success, diff;
    success = diff = 0;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[AUSPEX] < 4)
    {
        send_to_char( "You are not trained enough in Auspex!\n\r", ch );
        return;
    }

    if (ch->pblood < 20)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0')
    {
        send_to_char( "View whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world( ch, argument ) ) == NULL)
    {
        send_to_char( "View whom?\n\r", ch );
        return;
    }

    if (!can_see_room(ch, victim->in_room))
    {
        send_to_char("Their presence seems to be clouded by something...\n\r", ch);
        return;
    }

    if (IS_NPC(victim))
        diff = 6;
    else
        diff = victim->csmax_willpower;

    if (ch->level*2 < victim->level)
        diff += 2;
    if (!can_see(ch, victim))
        diff += 1;

    success = godice(get_attribute(ch, INTELLIGENCE) + ch->csabilities[CSABIL_SUBTERFUGE], diff);
    ch->pblood -= 10;
    WAIT_STATE(ch, 5);
    if (success < 1)
    {
        send_to_char("You fail to locate their presence in the realm.\n\r", ch);
        WAIT_STATE(ch, 15);
        return;
    }
    if (success > 0)
    {
    sprintf( buf, "You sense the presence of %s in %s.\n\r", IS_NPC(victim) ? victim->short_descr : victim->name, victim->in_room->name);
    send_to_char( buf, ch );
    switch ( victim->position )
        {
        case POS_TORPOR:send_to_char( "They are in Torpor.\n\r",ch );break;
        case POS_DEAD:send_to_char( "They are DEAD!!\n\r",ch ); break;
        case POS_MORTAL:send_to_char( "They are mortally wounded.\n\r", ch );break;
        case POS_INCAP: send_to_char( "They are incapacitated.\n\r",ch );break;
        case POS_STUNNED:send_to_char( "They are stunned.\n\r", ch );break;
        case POS_SLEEPING:send_to_char( "They are sleeping.\n\r",ch );break;
        case POS_RESTING:send_to_char( "They are resting.\n\r",ch );break;
        case POS_SITTING:send_to_char( "They are sitting.\n\r", ch );break;
        case POS_STANDING:send_to_char( "They are standing.\n\r",ch );break;
        case POS_FIGHTING:
        sprintf(buf, "They are engaged in battle with %s.\n\r", IS_NPC(victim->fighting) ? victim->fighting->short_descr : victim->fighting->name);
        send_to_char(buf,ch );break;
        }
    }
    if (success > 2) {
        WAIT_STATE(ch, 2);
    sprintf( buf,"They have %d/%d hit, %d/%d mana and %d/%d movement.\n\r",
        victim->hit,  victim->max_hit, victim->mana, victim->max_mana,
        victim->move, victim->max_move);
    send_to_char( buf, ch );
    sprintf( buf, "%s is %d years old.\n\r", IS_NPC(victim) ? victim->short_descr : victim->name, get_age(victim));
    send_to_char( buf, ch );
    sprintf(buf, "They are a %s %s.\n\r", victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
        race_table[victim->race].name);
    send_to_char(buf,ch);

    }

}


void do_dash(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[CELERITY] < 6)
    {
        send_to_char( "You are not skilled enough in your powers of Celerity!.\n\r", ch );
        return;
    }
    if(IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char("You must leave the Arena first!\n\r",ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if ( ch->pblood < 80 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Dash to whom?\n\r", ch );
        return;
    }

    if ( (victim = get_char_world(ch, argument)) == NULL)
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_QUIT))
    {
        send_to_char( "You are not powerful enough.\n\r", ch );
        return;
    }

    if (room_is_private(victim->in_room))
    {
        send_to_char( "That room is private right now.\n\r", ch );
        return;
    }

    ch->pblood -= 70;
    act("$n's body suddenly blurs as if moving at great speed, then disappears!.",ch,NULL,victim,TO_ROOM);
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    WAIT_STATE( ch, 24 );
    act("A spot in space in front of you blurs as $n appears.",ch,NULL,victim,TO_ROOM);
    do_function(ch, &do_look, "auto" );
    return;
}


void do_celerity(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


    if (IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (is_affected(ch, gsn_celbuff))
    {
        sendch("You cannot move any faster!\n\r", ch);
        return;
        }

    if (ch->pcdata->discipline[CELERITY] == 0)
    {
        send_to_char("You have not begun studying the metahuman speed of Celerity.\n\r", ch);
        return;
    }

    if (ch->pblood < ch->gen*ch->pcdata->discipline[CELERITY]+5)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= ch->gen*ch->pcdata->discipline[CELERITY];

    af.where     = TO_AFFECTS;
    af.type      = gsn_celbuff;
    af.level     = ch->level;
    af.duration  = ch->pcdata->discipline[CELERITY]+(20 - ch->gen);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char( "You draw upon the power of your vampiric nature and enhance your speed to superhuman levels.\n\r", ch );
    act("$n suddenly starts moving so fast that $E seems a blur!",ch,NULL,ch,TO_NOTVICT);
    return;
}

// Ravnos discs
void do_ignis_fatuus( CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
    char arg [MAX_INPUT_LENGTH];
    AFFECT_DATA af;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;

    if(!can_use_disc(ch,CHIMERSTRY,1,0,TRUE))
            return;

    if ( arg[0] == '\0' )
    {
        send_to_char("Confound the senses of which poor soul?\n\r",ch);
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char("You can't confound the senses of someone you cannot see.\n\r",ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("Clearly you don't need help with that!\n\r",ch);
        return;
    }

    if (IS_AFFECTED2(victim, AFF2_IGNIS_FATUUS))
    {
        send_to_char("They're already trapped in illusion!\n\r",ch);
        return;
    }

    if (ch->cswillpower < 1)
    {
        sendch("You do not possess the strength of will to affect reality in such a way.\n\r", ch);
        return;
    }

    ch->cswillpower--;

    af.where     = TO_AFFECTS2;
    af.type      = gsn_ignis_fatuus;
    af.level     = ch->pcdata->discipline[CHIMERSTRY];
    if (ch->pcdata->discipline[CHIMERSTRY] < 4)
        af.duration  = 2 * ch->pcdata->discipline[CHIMERSTRY];
    else
        af.duration = -1;
    af.modifier  =  -10 * ch->pcdata->discipline[CHIMERSTRY];
    af.location  = APPLY_HITROLL;
    af.bitvector = AFF2_IGNIS_FATUUS;
    affect_to_char( victim, &af );
    af.location = APPLY_CS_PER;
    af.modifier = -1;
    affect_to_char(victim, &af );


    act( "You twist reality around $N and confound their senses!", ch, NULL, victim, TO_CHAR);
    act( "You suddenly begin hearing and seeing things that aren't there!", ch, NULL, victim, TO_VICT    );
    if (victim->position == POS_FIGHTING)
        STOPPED(victim, 2* PULSE_VIOLENCE);
    else {
        int success;
        success = godice(get_attribute(victim, PERCEPTION) + get_ability(victim, CSABIL_ALERTNESS), 6);
        if (success)
        {
            act("You notice $n staring at you, they must be causing your hallucinations!", ch, NULL, victim, TO_VICT);
            act("$N surmises you're the cause of their hallucinations and attacks!", ch, NULL, victim, TO_CHAR);
            act("$N seems confused for a moment, then attacks $n!", ch, NULL, victim, TO_ROOM);
            multi_hit(victim, ch, TYPE_UNDEFINED);
        }
    }
    return;
}
void do_chimaera(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    AFFECT_DATA af;
    char buf[MSL];
    int level;
    bool perm;

    if (IS_NPC(ch)) return;

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot summon another pet!\n\r",ch );
        return;
    }

    if(!can_use_disc(ch,CHIMERSTRY,2,0,TRUE))
            return;

    if (is_affected(ch, gsn_chimerstry) && get_affect_level(ch, gsn_chimerstry) == 2)
    {
        sendch("You do not have the force of will to summon such an illusion so soon.\n\r", ch);
        return;
    }

    if (ch->cswillpower < 1)
    {
        send_to_char("You do not have enough will to bring forth this creation.\n\r",ch);
        return;
    }

    if ( (pMobIndex = get_mob_index(MOB_VNUM_DOPPLEGANGER)) == NULL )
    {
        send_to_char( "{RError: {Wplease inform the Coders!{x\n\r", ch );
        return;
    }

    ch->cswillpower -= 1;

    level = ch->pcdata->discipline[CHIMERSTRY];

    if (!str_prefix(argument, "permanent") && level > 3)
    {
        if (ch->pblood < 15)
        {
            sendch("You do not have enough blood to make this illusion permanent!\n\r", ch);
        } else {
            perm = TRUE;
            ch->pblood -= 10;
        }
    }

    WAIT_STATE(ch, 3 * PULSE_VIOLENCE);

    mob = create_mobile( pMobIndex );

    mob->leader = ch;
    mob->level  = UMAX(1, (ch->level)/2);
    mob->max_hit = URANGE(500, ch->max_hit * 3, 5000);
    mob->hit = mob->max_hit;

    af.where     = TO_AFFECTS;
    af.type      = gsn_chimaera;
    af.level     = level;
    if (!perm)
        af.duration  = level * 2;
    else
        af.duration = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    if (!perm)
    {
    af.type      = gsn_chimerstry;
    af.duration  = level * 3;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    }

    sprintf( buf, mob->short_descr, ch->name );
    free_string( mob->short_descr );
    mob->short_descr = str_dup( buf );

    sprintf( buf, mob->long_descr, ch->name );
    free_string( mob->long_descr );
    mob->long_descr = str_dup( buf );

    char_to_room( mob, ch->in_room );
    act("$n appears to split into two, creating $N out of thin air!", ch, NULL, mob, TO_NOTVICT );
    send_to_char("You summon forth a doppleganger to assist you.\n\r",ch);
    add_follower( mob, ch );
    mob->leader = ch;
    ch->pet = mob;

    return;
}

void do_phantasm (CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int level;
    bool perm = FALSE;

    if (!can_use_disc(ch, CHIMERSTRY, 3, 0, TRUE))
        return;

    if (is_affected(ch, gsn_phantasm))
    {
        sendch("You release the illusion masking your true position.\n\r", ch);
        affect_strip(ch, gsn_phantasm);
        return;
    }

    if (is_affected(ch, gsn_chimerstry) && get_affect_level(ch, gsn_chimerstry) == 3)
    {
        sendch("You do not have the force of will to summon such an illusion so soon.\n\r", ch);
        return;
    }

    if (ch->cswillpower < 1)
    {
        send_to_char("You do not have enough will to bring forth this creation.\n\r",ch);
        return;
    }
    level = ch->pcdata->discipline[CHIMERSTRY];

    if (!str_prefix(argument, "permanent") && level > 3)
    {
        perm = TRUE;
        if (ch->pblood > 25)
            ch->pblood -= 20;
        else {
            sendch("You do not have enough blood to make this effect permanent!\n\r", ch);
            perm = FALSE;
        }
    }

    ch->cswillpower -= 1;

    af.where     = TO_AFFECTS;
    af.type      = gsn_phantasm;
    af.level     = level;
    if (!perm)
        af.duration  = level * 2;
    else
        af.duration = -1;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    if (!perm)
    {
        af.type     = gsn_chimerstry;
        af.duration = level * 6;
        affect_to_char( ch, &af);
    }

    act("$n concentrates for a moment and $s form suddenly shifts six inches to the left.", ch, NULL, NULL, TO_ROOM);
    sendch("You construct an illusion around yourself, masking your true location.\n\r", ch);
    return;
}

#define FLAMES 0
#define BLADES 1

void do_horridreality(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int dice, diff, success;
    char arg1[MIL];
    char arg2[MIL];
    int  type;

    if (IS_NPC(ch))
        return;

    if (!can_use_disc(ch, CHIMERSTRY, 5, 0, TRUE))
        return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if ( !str_prefix(arg1, "flames") )
        type = FLAMES;
    else if (!str_prefix(arg1, "blades"))
        type = BLADES;
    else {
        sendch("Inflict illusions of FLAMES or BLADES?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_room( ch, arg2 ) ) == NULL &&
        (victim = ch->fighting) == NULL )
    {
        send_to_char("Who do you wish to inflict this horrid reality upon?\n\r",ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("You are well aware of the Horrors reality has to offer...\n\r",ch);
        return;
    }

    if (ch->cswillpower < 2)
    {
        send_to_char("You do not have enough will to bring forth this creation.\n\r",ch);
        return;
    }
    ch->cswillpower -= 2;
    dice = get_attribute(ch, MANIPULATION) + get_ability(ch, CSABIL_SUBTERFUGE);
    diff = get_attribute(victim, PERCEPTION) + IS_NPC(victim) ?
            victim->cswillpower / 2 : victim->pcdata->csvirtues[SELF_CONTROL];
    if (diff < 5)
        diff = 5;
    if (diff > 9)
        diff = 9;
    success = godice(dice, diff);

    if (success < 1)
    {
        act("$N is not fooled by your illusions!", ch, NULL, victim, TO_CHAR);
        multi_hit(victim, ch, TYPE_UNDEFINED);
        return;
    }

    if (type == FLAMES)
    {
        act("You engulf $N in illusory flames, leaving them with horrible burns!", ch, NULL, victim, TO_CHAR);
        act("$n concentrates for a moment, and $N begins shrieking as horrible burns begin appearing all over $S body!", ch, NULL, victim, TO_ROOM);
        act("You are suddenly engulfed in flames!", ch, NULL, victim, TO_VICT);
        d10_damage(ch, victim, success, victim->max_hit/7, gsn_horridreality, DAM_FIRE, DEFENSE_NONE, TRUE, FALSE);
    } else {
        act("You shower $N in a hail of blades, cutting them to shreds!", ch, NULL, victim, TO_CHAR);
        act("$n concentrates for a moment, and $N begins shrieking as large gashes begin appearing all over $S body!", ch, NULL, victim, TO_ROOM);
        act("You are suddenly assaulted by a hail of flying blades!", ch, NULL, victim, TO_VICT);
        d10_damage(ch, victim, success, victim->max_hit/7, gsn_horridreality, DAM_SLASH, DEFENSE_NONE, TRUE, FALSE);

    }
}

void do_command(CHAR_DATA *ch, char *argument)
{
   char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int success, diff;
     AFFECT_DATA af;
   success = diff = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

        if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pcdata->discipline[DOMINATE] < 1)
    {
        send_to_char( "You are not trained in Domination!.\n\r", ch );
        return;
    }
        if (ch->pblood < 20)
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Command whom to do what?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Nobody here by that name.\n\r", ch );
        return;
    }
    if (!IS_NPC(victim))
    {
        send_to_char("Disabled on players due to the potential for abuse.\n\r", ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (!str_prefix(arg2,"delete"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if (!str_prefix(arg2,"ic"))
        {
                send_to_char("Nope!\n\r",ch);
                return;
        }
    if (!str_prefix(arg2, "quit"))
        {
                send_to_char("I don't think so!.\n\r", ch);
                return;
        }

    if ( victim == ch )
    {
        send_to_char( "You get a headache trying to command yourself.\n\r", ch );
        return;
    }

    if ( IS_IMMORTAL(victim))
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }

    if (victim->race != race_lookup("human")
     && victim->race != race_lookup("vampire")
     && victim->race != race_lookup("ghoul")
     && victim->race != race_lookup("garou")
     && victim->race != race_lookup("methuselah")
     && victim->race != race_lookup("dhampire")
     || !IS_SET(victim->form,FORM_SENTIENT)) {
         send_to_char("Your powers of Domminate only work on sentient, humanoid beings.\n\r", ch);
         return;
     }
    ch->pblood -= 10;
    diff = 5;
    if (victim->race != race_lookup("human"))
        diff += 2;
    if (victim->level > ch->level)
        diff++;
    if (victim->level > ch->level*2)
        diff++;
    success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_INTIMIDATION], diff);

    if (success < 1)
    {
        sprintf( buf, "%s resists your power of Domination!\n\r", victim->name );
        send_to_char(buf,ch);
        sprintf( buf, "%s attempts to dominate your mind!\n\r", ch->name );
        send_to_char(buf,victim);
        return;
    } else {
    sprintf( buf, "%s commands you to '%s'.\n\r", ch->name, arg2 );
    send_to_char(buf,victim);
    sprintf( buf, "You command %s to '%s'.\n\r", victim->name, arg2 );
    send_to_char(buf,ch);
    act( "$n stares into $N's eyes a moment then whispers a command.",  ch, NULL, victim, TO_NOTVICT );
    interpret( victim, arg2 );
    }

    return;
}


void do_forgetful (CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[DOMINATE] < 3)
    {
        send_to_char( "You are not skilled enough in your powers of Dominate!.\n\r", ch );
        return;
    }
       if ( (victim = get_char_room(ch, argument)) == NULL)
    {
        send_to_char( "Dominate whom?\n\r", ch );
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

        if ( ch->pblood < 30 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 20;

    act( "$n stares a moment into $N's eyes. $N suddenly seems dazed.",  ch, NULL, victim, TO_NOTVICT );
    act( "You stare into $N's eyes and rob them of their memories.",  ch, NULL, victim, TO_CHAR );

    af.where     = TO_AFFECTS;
    af.type      = gsn_forget;
    af.level     = ch->level + ch->pcdata->discipline[DOMINATE];
    af.duration  = ch->level/2 + ch->pcdata->discipline[DOMINATE];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}


void do_mesmerize(CHAR_DATA *ch, char *argument)
{
   char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;
   int success, diff;
     AFFECT_DATA af;
   success = diff = 0;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pcdata->discipline[DOMINATE] < 2)
    {
        send_to_char( "You are not trained in Domination!.\n\r", ch );
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char("Mesmerize whom to do what?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Nobody here by that name.\n\r", ch );
        return;
    }
    if (!IS_NPC(victim))
    {
        send_to_char("Disabled on players due to the potential for abuse.\n\r", ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (!str_prefix(arg2,"delete"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if (!str_prefix(arg2,"ic"))
        {
                send_to_char("Nope!\n\r",ch);
                return;
        }
    if (!str_prefix(arg2, "quit"))
        {
                send_to_char("I don't think so!.\n\r", ch);
                return;
        }

    if ( victim == ch )
    {
        send_to_char( "You get a headache trying to command yourself.\n\r", ch );
        return;
    }
    if ( IS_IMMORTAL(victim))
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }
    if ( ch->pblood < 30 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if (victim->race != race_lookup("human")
     && victim->race != race_lookup("vampire")
     && victim->race != race_lookup("ghoul")
     && victim->race != race_lookup("garou")
     && victim->race != race_lookup("methuselah")
     && victim->race != race_lookup("dhampire")
     || !IS_SET(victim->form,FORM_SENTIENT)) {
         send_to_char("Your powers of Domminate only work on sentient, humanoid beings.\n\r", ch);
         return;
     }
    ch->pblood -= 20;
    diff = 5;
    if (victim->race != race_lookup("human"))
        diff += 2;
    if (victim->level > ch->level)
        diff++;
    if (victim->level > ch->level*2)
        diff++;
    success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_LEADERSHIP], diff);

    if (success < 1)
    {
        sprintf( buf, "%s resists your power of Domination!\n\r", victim->name );
        send_to_char(buf,ch);
        sprintf( buf, "%s attempts to dominate your mind!\n\r", ch->name );
        send_to_char(buf,victim);
        return;
    } else {
    sprintf( buf, "%s manipulates your mind, forcing you to '%s'.\n\r", ch->name, arg2 );
    send_to_char(buf,victim);
    sprintf( buf, "You force %s to '%s'.\n\r", victim->name, arg2 );
    send_to_char(buf,ch);
    act( "$n stares into $N's eyes a moment then whispers a command.",  ch, NULL, victim, TO_NOTVICT );
    interpret( victim, arg2 );
    }

    return;
}

void do_conditioning(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim, *gch;
    int group_members = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if (ch->pcdata->discipline[DOMINATE] < 4)
    {
        send_to_char( "You are not skilled enough in Dominate!.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nobody here by that name.\n\r", ch );
        return;
    }

        if (!IS_NPC(victim))
        {
                send_to_char("Disabled on players due to the potential for abuse.\n\r", ch);
                return;
        }
    if (is_safe(ch,victim)) return;

    if ( victim == ch )
    {
    send_to_char( "You cannot Condition yourself.\n\r", ch );
    return;
    }
    if(ch->pet != NULL)
    {
        send_to_char( "You cannot command that many creatures at once!\n\r",ch );
        return;
    }
    if ( ch->pblood < 50 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }
    if (victim->race != race_lookup("human")
     && victim->race != race_lookup("vampire")
     && victim->race != race_lookup("ghoul")
     && victim->race != race_lookup("garou")
     && victim->race != race_lookup("methuselah")
     && victim->race != race_lookup("dhampire")
     || !IS_SET(victim->form,FORM_SENTIENT)) {
         send_to_char("Your powers of Domminate only work on sentient, humanoid beings.\n\r", ch);
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

    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   IS_SET(victim->imm_flags,IMM_CHARM)
    ||   saves_spell( ch->level, victim,DAM_CHARM))
    {
        act( "$n stares into your eyes... and fails to Condition you!", ch, NULL, victim, TO_VICT );
        act( "$n stares into $N's eyes... and fails to Condition them!", ch, NULL, victim, TO_NOTVICT );
        act( "You fail to Condition $N!", ch, NULL, victim, TO_CHAR );
        return;
    }
    ch->pblood -= 40;
    sh_int diff, success;
    diff = success = 0;
        diff = 7;
    if (victim->race != race_lookup("human"))
        diff += 2;
    if (victim->level > ch->level)
        diff++;
    if (victim->level > ch->level*2)
        diff++;
    success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_LEADERSHIP], diff);

    if (success > 0)
    {
    if ( victim->master )
    stop_follower( victim );
    add_follower( victim, ch );
    victim->leader = ch;
    ch->pet = victim;
    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = ch->pcdata->discipline[DOMINATE]*5;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( "$n Conditions your mind!", ch, NULL, victim, TO_VICT );
    if ( ch != victim )
    act("$N submits to your will.",ch,NULL,victim,TO_CHAR);
    act( "$n conditions $N's mind with a penetrating gaze.",  ch, NULL, victim, TO_NOTVICT );
    } else {
                act( "$n stares into your eyes... and fails to Condition you!", ch, NULL, victim, TO_VICT );
        act( "$n stares into $N's eyes... and fails to Condition them!", ch, NULL, victim, TO_NOTVICT );
        act( "You fail to Condition $N!", ch, NULL, victim, TO_CHAR );
        return;
    }

    return;
}

void do_possession( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

        if ( IS_NPC(ch) || ch->desc == NULL )
        return;

        if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[DOMINATE] < 5)
    {
        send_to_char( "You are not skilled enough in your powers of Dominate!.\n\r", ch );
        return;
    }
        if ( ch->desc->original != NULL )
    {
    send_to_char( "You are already possessing a victim.\n\r", ch );
    return;
    }
    if ( arg[0] == '\0' )
    {
    send_to_char( "Possess whom?\n\r", ch );
    return;
    }


    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here to possess.\n\r", ch );
    return;
    }

    if ( victim == ch )
    {
    send_to_char( "You already possess your own mind.\n\r", ch );
    return;
    }

    if (!IS_NPC(victim) || victim->level > (10-ch->gen)*10+ch->level/2  /* ch->level/2*/ || victim->race != race_lookup("human"))
    {
    send_to_char("That person's mind is too powerful to possess.\n\r",ch);
    return;
    }
    if(ch->pblood < 110)
    {
        send_to_char("You don't have enough blood.\n\r",ch);
        return;
    }
    if ( victim->desc != NULL )
    {
    send_to_char( "For some reason you cannot possess them.\n\r", ch );
    return;
    }
    sprintf(buf,"$N switches into %s using Possession.",victim->short_descr);
    wiznet(buf,ch,NULL,WIZ_SWITCHES,WIZ_SECURE,get_trust(ch));

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    /* change communications to match */
    if (ch->prompt != NULL)
        victim->prompt = str_dup(ch->prompt);
    victim->comm = ch->comm;
    victim->lines = ch->lines;
    act("You possess the mind of $N!",ch,NULL,victim,TO_VICT);
    return;
}


void do_blight(CHAR_DATA *ch, char *argument)
{
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   AFFECT_DATA af;
  int dicesuccess = 0;
   int chance;

    if (IS_NPC(ch)) return;

    if(!can_use_disc(ch,MORTIS,1,15,TRUE))
            return;

    argument = one_argument( argument, arg );
    chance = number_range(1, 100);

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Who?\n\r", ch );
        return;
    }

    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (ch->cswillpower < 1)
    {
        send_to_char("You do not have any Willpower left to spend for this.\n\r", ch);
        return;
    }

   dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 6);

    if (dicesuccess < 0)
    {
        multi_hit( victim, ch, TYPE_UNDEFINED );
        act("You fail to grab $N and they attack!", ch, NULL, victim, TO_CHAR);
        act("$n attempts to grab you!", ch, NULL, victim, TO_VICT);
        act("$n fails to grab on to $N and $E attacks $m!", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    if (dicesuccess == 0)
    {
        act("You miss grabbing $N.", ch, NULL, victim, TO_CHAR);
        act("$n tries to grab you and misses by inches.", ch, NULL, victim, TO_VICT);
        act("$n tries to grab $N and misses $M by inches.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    act("You grab on to $N and focus your vitae into your curse...", ch, NULL, victim, TO_CHAR);
    act("$n grabs on to you and seems to focus for a moment...", ch, NULL, victim, TO_VICT);
    act("$n grabs on to $N and seems to focus for a moment...", ch, NULL, victim, TO_NOTVICT);

    dicesuccess = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_MEDICINE], 6);
    ch->pblood -= 10;

    if (dicesuccess < 0)
    {
        act("... and Blight yourself instead!", ch, NULL, victim, TO_CHAR);
        act("... and $e recoils in pain!", ch, NULL, victim, TO_VICT);
        act("... and $n recoils in pain!", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch,48);
        af.where     = TO_AFFECTS;
        af.type      = gsn_quietus_weakness;
        af.level     = ch->level;
        af.duration  = 6 - ch->pcdata->discipline[MORTIS];
        af.location  = APPLY_CS_STR;
        af.modifier  = -ch->pcdata->discipline[MORTIS];
        af.bitvector = AFF_SLOW;
        affect_to_char( ch, &af );
        return;
    }

    if (dicesuccess == 0)
    {
        act("... and nothing happens.", ch, NULL, victim, TO_CHAR);
        act("... and nothing happens.", ch, NULL, victim, TO_VICT);
        act("... and nothing happens.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, 24);
        return;
    }

    act("... and you curse them with the weakness of death!", ch, NULL, victim, TO_CHAR);
    act("... and you suddenly feel weak and sluggish!", ch, NULL, victim, TO_VICT);
    act("... and $N suddenly seems weak and sluggish.", ch, NULL, victim, TO_NOTVICT);

    WAIT_STATE(ch,24);
    af.where     = TO_AFFECTS;
    af.type      = gsn_quietus_weakness;
    af.level     = ch->level;
    af.duration  = (5+(ch->pcdata->discipline[MORTIS]*10))-ch->gen;
    af.location  = APPLY_CS_STR;
    af.modifier  = -ch->pcdata->discipline[MORTIS];
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );
    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}
void do_blackdeath(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   DESCRIPTOR_DATA *d;

    if (IS_NPC(ch)) return;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Who?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[MORTIS] < 5)
    {
        send_to_char( "You are not skilled enough in Mortis!.\n\r", ch );
        return;
    }
    if (ch->pblood <= 110)
    {
        send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }
    if(!IS_NPC(victim) && victim->race == race_lookup("vampire") && victim->gen < ch->gen)
    {
        send_to_char("That person is too close to Caine to kill outright.\n\r",ch);
        return;
    }
    if(victim->race == race_lookup("methuselah") && ch->race != race_lookup("methuselah"))
    {
        send_to_char("That kindred is too ancient and powerful to fall victim to you.\n\r",ch);
        return;
    }
    if(ch->clan == clan_lookup("cappadocian") && victim->level > (ch->level+10))
    {
        send_to_char("Even your powers over death cannot compete with the might of this soul.\n\r",ch);
        return;
    }
    if(ch->clan != clan_lookup("cappadocian") && victim->level > ch->level)
    {
        send_to_char("That person is too powerful to cause death within.\n\r",ch);
        return;
    }
    if(IS_IMMORTAL(victim) && !IS_IMMORTAL(ch))
    {
        send_to_char("You GOT to be kidding!\n\r",ch);
        return;
    }
    if(is_affected(victim,gsn_shadowform) || IS_AFFECTED2(victim, AFF2_MIST) || is_affected(victim,gsn_earthmeld))
    {
        send_to_char("You cannot do this to someone in that form.\n\r",ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }
    WAIT_STATE( ch, 60 );
    ch->pblood -= 60;

    act("$n comfortingly lays a hand upon $N who suddenly screams in pain and falls over dead.",ch,NULL,victim,TO_NOTVICT);
    act("$n comfortingly lays a hand upon you... OH THE AGONY!",ch,NULL,victim,TO_VICT);
    act("You lay a hand upon $N to ease them into the next world.",ch,NULL,victim,TO_CHAR);
    if(!IS_NPC(victim) && !IS_SET(victim->act,PLR_ARENA))
    {
        if(victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah"))
        {
            act("$N enters Torpor.",ch,NULL,victim,TO_NOTVICT);
            act("$N enters Torpor.",ch,NULL,victim,TO_CHAR);
            act("You enter Torpor.",ch,NULL,victim,TO_VICT);
            stop_fighting( victim, TRUE );
            victim->position = POS_TORPOR;
            victim->hit = -20;
            for ( d = descriptor_list; d != NULL; d = d->next )
            {
                if ( d->connected == CON_PLAYING &&
                d->character != victim &&
                is_same_clan(victim,d->character) &&
                !IS_SET(d->character->comm,COMM_NOCLAN))
                act_new("You sense that $n has entered Torpor!",victim,NULL,d->character,TO_VICT,POS_DEAD, FALSE);
                return;
            }
        }
        else
        kill_em(ch,victim);

    }
    else
    kill_em(ch,victim);
    return;
}

void do_resurrection(CHAR_DATA *ch, char *argument)
{
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if(ch->position != POS_TORPOR)
    {
        send_to_char( "You are not in Torpor!\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[MORTIS] < 3)
    {
        send_to_char( "You are not skilled enough in Mortis!\n\r", ch );
        return;
    }

    if (ch->cswillpower < 2)
    {
        send_to_char("You do not have enough Willpower to force yourself out of {RT{ro{Drp{ro{Rr{x.\n\r", ch);
        return;
    }

    dicesuccess = godice(ch->csmax_willpower, 13 - ch->pcdata->cshumanity);

    if (dicesuccess < 0)
    {
        act_new("You try to force yourself out of Torpor early, but fail horribly.", ch, NULL, NULL, TO_CHAR, POS_TORPOR, FALSE);
        return;
    }

    if (dicesuccess == 0)
    {
        act_new("You fail in your attempt to Resurrect yourself.", ch, NULL, NULL, TO_CHAR, POS_TORPOR, FALSE);
        return;
    }

    ch->position=POS_RESTING;
    ch->hit = ch->max_hit / 12;
    ch->cswillpower -= 2;
    affect_strip(ch,gsn_vamp_frenzy);
    affect_strip(ch, gsn_torpor);
    if (ch->pblood == 0)
        ch->pblood = 10;

    gain_exp( ch, dicesuccess * 5);

    act("Using the power of your {rvitae{x and the determination of your own {ywill{x, you force yourself from {RT{ro{Drp{ro{Rr{x!", ch, NULL, NULL, TO_CHAR);
    act( "$n forces $mself out of {RT{ro{Drp{ro{Rr{x!",  ch, NULL, NULL, TO_NOTVICT );
    return;
}

void do_homunculusservant(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int dicesuccess;

    if (IS_NPC(ch)) return;


    if(!can_use_disc(ch,MORTIS,2,20,TRUE))
            return;

    if((obj = get_obj_here( ch, argument )) == NULL)
    {
        send_to_char( "You require a fresh corpse to craft a Homunculus.\n\r", ch );
        return;
    }

    if( obj->item_type != ITEM_CORPSE_NPC )
    {
        send_to_char( "That is not an acceptable corpse.\n\r", ch );
        return;
    }
    if ( obj->timer < 2)
    {
        send_to_char( "This corpse is too far gone to animate.\n\r", ch );
        return;
    }
    if(ch->pet != NULL)
    {
        send_to_char( "You can control only one servant at a time.\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_HOMUNCULUS)) == NULL )
    {
        send_to_char( "{RError: {Wplease inform the Coders!{x\n\r", ch );
        return;
    }

    ch->pblood -= 20;
    dicesuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_OCCULT], 7);

    if (dicesuccess < 0)
    {
        act("You dribble vitae on $p and it smokes and sizzles, destroying the remnants of life left within the corpse!", ch, obj, NULL, TO_CHAR);
        act("$N dribbles vitae on $p and it smokes and sizzles!", ch, obj, NULL, TO_NOTVICT);
        obj->timer = 0;
        WAIT_STATE( ch, 5*PULSE_VIOLENCE );
        return;
    }
    if (dicesuccess == 0)
    {
        act("You dribble vitae upon $p... and nothing happens.", ch, obj, NULL, TO_CHAR);
        act("$N dribbles vitae upon $p... and nothing happens.", ch, obj, NULL, TO_NOTVICT);
        return;
        WAIT_STATE( ch, 5*PULSE_VIOLENCE );
    }

    mob = create_mobile( pMobIndex );
    act( "$n dribbles some vitae upon $p and $N sloughs off and starts moving around!", ch, obj, mob, TO_NOTVICT );
    act( "You dribble some vitae upon $p and $N sloughs off and awaits your bidding.", ch, obj, mob, TO_CHAR);

    WAIT_STATE( ch, 2*PULSE_VIOLENCE );
    obj->timer = 1;

    char_to_room( mob, ch->in_room );
    mob->level  = ch->level;
    mob->max_hit = ch->max_hit/10;
    mob->hit = mob->max_hit;
    mob->hitroll = mob->level/2 + (ch->pcdata->discipline[MORTIS] * dicesuccess) * 4;
    mob->damroll = mob->level/4 + (ch->pcdata->discipline[MORTIS] * dicesuccess) * 2;
    // Dice curve so mobs don't suck at level 1.
    int dicenum;
    if (mob->level < 10)
        dicenum = 10;
    if (mob->level >= 10 && mob->level < 15)
        dicenum = 9;
    if (mob->level >= 15 && mob->level < 20)
        dicenum = 8;
    if (mob->level >= 20 && mob->level < 25)
        dicenum = 7;
    if (mob->level >= 25)
        dicenum = mob->level/4;
    mob->damage[DICE_NUMBER] = dicenum;
    mob->damage[DICE_TYPE] = 3;
    mob->damage[DICE_BONUS] = mob->level;
    add_follower( mob, ch );
    ch->pet = mob;
    mob->leader = ch;

    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->pcdata->discipline[MORTIS];
    af.duration  = ch->pcdata->discipline[MORTIS]+ch->csabilities[CSABIL_OCCULT]+20;  /*tmp hack while I fix time code */
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );

    return;
}

void do_animatedead(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    char buf[MAX_STRING_LENGTH];
    char *str1;
    char *str2;
    int dicesuccess;

    if (IS_NPC(ch)) return;

    if(!can_use_disc(ch,MORTIS,3,30,TRUE))
            return;

    if((obj = get_obj_here( ch, argument )) == NULL)
    {
        send_to_char( "Animate what?.\n\r", ch );
        return;
    }

    if( obj->item_type != ITEM_CORPSE_NPC )
    {
        send_to_char( "This corpse cannot be animated.\n\r", ch );
        return;
    }
    if(obj->level > (ch->level+30))
    {
        send_to_char("This creature's spirit is too powerful to control.\n\r",ch);
        return;
    }

    if(obj->level < 5)
    {
        send_to_char( "The spirit of this corpse is not strong enough to reanimate.\n\r", ch );
        return;
    }
    if ( obj->timer < 2)
    {
        send_to_char( "This corpse is too far gone to animate.\n\r", ch );
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two creatures at once!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_MORTIS_ZOMBIE)) == NULL )
    {
        send_to_char( "{RError: {Wplease inform the Coders!{x\n\r", ch );
        return;
    }
    WAIT_STATE(ch, 48);
    act( "$n dribbles some vitae into the mouth of $p and begins chanting...", ch, obj, NULL, TO_NOTVICT );
    act( "You dribble some vitae into the mouth of $p and begin chanting...", ch, obj, NULL, TO_CHAR);
    ch->pblood -= 30;
    dicesuccess = godice(get_attribute(ch, STAMINA) + ch->csabilities[CSABIL_OCCULT], 6);

    if (dicesuccess < 0)
    {
        act( "... and $p is devoured in unholy black flames!", ch, obj, mob, TO_NOTVICT );
        act( "... and $p is devoured in unholy black flames!", ch, obj, mob, TO_CHAR);
        extract_obj(obj);
        return;
    }
    if (dicesuccess == 0)
    {
        act( "... and nothing happens.", ch, NULL, NULL, TO_NOTVICT );
        act( "... and nothing happens.", ch, NULL, NULL, TO_CHAR);
        return;
    }


    act( "... and $p begins moving, standing up to do $n's bidding!", ch, obj, mob, TO_NOTVICT );
    act( "... and $p is animated into your undead servant!", ch, obj, mob, TO_CHAR);

    str1 = str_dup(obj->short_descr);
    str2 = str_dup("none");
    str1 = one_argument( str1, str2 );
    str1 = one_argument( str1, str2 );
    str1 = one_argument( str1, str2 );
    str1 = one_argument( str1, str2 );

    obj->level = obj->level/2;
    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );
    mob->level  = obj->level;
    mob->max_hit = (ch->level + obj->level) * 100;
    mob->hit = mob->max_hit;
    mob->hitroll = obj->level/4;
    mob->damroll = obj->level/8;
    mob->damage[DICE_NUMBER] = obj->level/2;
    mob->damage[DICE_TYPE] = 3;
    mob->armor[0] -= ch->level*10;
    mob->armor[1] -= ch->level*10;
    mob->armor[2] -= ch->level*10;
    mob->armor[3] -= ch->level*10;

    sprintf(buf, "The walking corpse of %s", str1);
    mob->short_descr = str_dup(buf);
    sprintf(buf, "corpse zombie %s", str1);
    mob->name = str_dup(buf);

    add_follower( mob, ch );
    mob->leader = ch;
    ch->pet = mob;
    extract_obj(obj);

    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = (15*ch->pcdata->discipline[MORTIS])-ch->gen;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );
    return;
}


void do_callathanatos(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    char buf[MAX_STRING_LENGTH];
    char *str1;
    char *str2;
    int dicesuccess;

    if (IS_NPC(ch)) return;

    if(!can_use_disc(ch,MORTIS,4,40,TRUE))
            return;

    if((obj = get_obj_here( ch, argument )) == NULL)
    {
        send_to_char( "Animate what?.\n\r", ch );
        return;
    }

    if( obj->item_type != ITEM_CORPSE_NPC )
    {
        send_to_char( "This corpse cannot be animated.\n\r", ch );
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two creatures at once!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_ATHANATOS)) == NULL )
    {
        send_to_char( "{RError: {Wplease inform the Coders!{x\n\r", ch );
        return;
    }
    WAIT_STATE(ch, 48);
    act( "$n dribbles some vitae onto $p and begins chanting...", ch, obj, NULL, TO_NOTVICT );
    act( "You dribble some vitae onto $p and begin chanting...", ch, obj, NULL, TO_CHAR);

    dicesuccess = godice(get_attribute(ch, STAMINA) + ch->csabilities[CSABIL_OCCULT], 6);

    if (dicesuccess == 0)
    {
        act( "... and nothing happens.", ch, NULL, NULL, TO_NOTVICT );
        act( "... and nothing happens.", ch, NULL, NULL, TO_CHAR);
        return;
    }

    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );
    mob->level  = ch->level;
    mob->max_hit = UMAX( 500, (mob->level) * 80);
    mob->hit = mob->max_hit;
    mob->hitroll = mob->level * 5;
    mob->damage[DICE_NUMBER] = UMAX(25, mob->level);
    mob->damage[DICE_TYPE] = 3;
    mob->armor[0] -= ch->level*10;
    mob->armor[1] -= ch->level*10;
    mob->armor[2] -= ch->level*10;
    mob->armor[3] -= ch->level*10;

    if (dicesuccess < 0)
    {
        act( "... and $p begins transforming into a nightmarish monster that turns and attacks $m!", ch, obj, mob, TO_NOTVICT );
        act( "... and you summon forth a corpse knight, that immediately turns on you!", ch, obj, mob, TO_CHAR);
        multi_hit(mob, ch, TYPE_UNDEFINED);
    } else {
        act( "... and $p begins transforming into a nightmarish monster!", ch, obj, mob, TO_NOTVICT );
        act( "... and you summon forth a corpse knight to slay your foes.", ch, obj, mob, TO_CHAR);
        add_follower( mob, ch );
        mob->leader = ch;
        ch->pet = mob;

        af.where     = TO_AFFECTS;
        af.type      = gsn_charm_person;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( mob, &af );
    }

    extract_obj(obj);
    return;
}

void do_musterthecorpsearmy(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    AFFECT_DATA af;
    char buf[MAX_STRING_LENGTH];
    char *str1;
    char *str2;
    int dicesuccess;

    if (IS_NPC(ch)) return;

    if(!can_use_disc(ch,MORTIS,5,50,TRUE))
            return;

    if((obj = get_obj_here( ch, argument )) == NULL)
    {
        send_to_char( "Animate what?.\n\r", ch );
        return;
    }

    if( obj->item_type != ITEM_CORPSE_NPC )
    {
        send_to_char( "This corpse cannot be animated.\n\r", ch );
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two creatures at once!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_ATHANATOS)) == NULL )
    {
        send_to_char( "{RError: {Wplease inform the Coders!{x\n\r", ch );
        return;
    }
    WAIT_STATE(ch, 48);
    act( "$n dribbles some vitae onto $p and begins chanting...", ch, obj, NULL, TO_NOTVICT );
    act( "You dribble some vitae onto $p and begin chanting...", ch, obj, NULL, TO_CHAR);

    dicesuccess = godice(get_attribute(ch, STAMINA) + ch->csabilities[CSABIL_OCCULT], 6);

    if (dicesuccess == 0)
    {
        act( "... and nothing happens.", ch, NULL, NULL, TO_NOTVICT );
        act( "... and nothing happens.", ch, NULL, NULL, TO_CHAR);
        return;
    }

    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );
    mob->level  = ch->level;
    mob->max_hit = UMAX( 500, (mob->level) * 100);
    mob->hit = mob->max_hit;
    mob->hitroll = mob->level * 30;
    mob->damage[DICE_NUMBER] = UMAX(25, 3*mob->level/2);
    mob->damage[DICE_TYPE] = 3;
    mob->armor[0] -= ch->level*30;
    mob->armor[1] -= ch->level*30;
    mob->armor[2] -= ch->level*30;
    mob->armor[3] -= ch->level*30;
    SET_BIT(mob->off_flags, OFF_THIRD_ATTACK);
    SET_BIT(mob->off_flags, OFF_FOURTH_ATTACK);
    SET_BIT(mob->affected_by, AFF_HASTE);
    SET_BIT(mob->affected_by, AFF_BERSERK);
    mob->short_descr = str_dup("An Army of Death Knights");
    mob->long_descr = str_dup("An army of perfectly silent Knights of Death stand here, awaiting their Master's orders.\n\r");
    mob->name = str_dup("death knight athanatos army");

    if (dicesuccess < 0)
    {
        act( "... and $p begins transforming into a nightmarish monster that turns and attacks $m!", ch, obj, mob, TO_NOTVICT );
        act( "... and you summon forth an army of corpse knights, that immediately turn on you!", ch, obj, mob, TO_CHAR);
        multi_hit(mob, ch, TYPE_UNDEFINED);
    } else {
        act( "... and $p begins multiplying and transforming into an army of nightmarish horrors!", ch, obj, mob, TO_NOTVICT );
        act( "... and you summon forth an army of Athanatos to slay your foes.", ch, obj, mob, TO_CHAR);
        add_follower( mob, ch );
        mob->leader = ch;
        ch->pet = mob;

        af.where     = TO_AFFECTS;
        af.type      = gsn_charm_person;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char( mob, &af );
    }

    extract_obj(obj);
    return;
}

void do_mask(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[OBFUSCATE] < 3)
    {
        send_to_char( "You are not skilled enough in Obfuscate to assume a mask.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0')
    {
        if (is_affected( ch, gsn_mask ) || !str_cmp(argument, "self"))
        {
            send_to_char( "You drop your Mask of a Thousand Faces.\n\r", ch );
            affect_strip(ch,gsn_mask);
        }
        else
            send_to_char("Assume a mask of whom?\n\r",ch);
        return;
    }
    if ((victim = get_char_room( ch, argument ))== NULL)
    {
        send_to_char( "Assume a mask of whom?\n\r", ch );
        return;
    }

    if (victim == ch)
    {
        if (is_affected(ch, gsn_mask))
        {
            send_to_char( "You drop your Mask of a Thousand Faces.\n\r", ch );
            affect_strip(ch,gsn_mask);
            return;
        } else {
            sendch("What would be the point of masking yourself?\n\r", ch);
            return;
        }
    }

    if(!IS_NPC(victim))
    {
    ch->pblood -= 20;
    act( "Your features alter and shift until you assume the mask of $N", ch, NULL, victim, TO_CHAR );
    act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
    sprintf(buf, "%s",victim->name,ch->name);
        ch->short_descr = str_dup( buf );
    sprintf(buf, "%s",victim->name);
            ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_mask;
    af.level     = ch->level + ch->pcdata->discipline[OBFUSCATE];
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
return;
    }

    if (is_affected( ch, gsn_vicissitude_horrid ) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("Your form is too inhuman to alter in this way.\n\r",ch);
        return;
    }

    if(ch->pblood < 30)
    {
        send_to_char("You do not have enough blood to assume this guise.\n\r",ch);
        return;
    }
    ch->pblood -= 20;
    act( "Your features alter and shift until you assume the mask of $N", ch, NULL, victim, TO_CHAR );
    act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
    sprintf(buf, "%s",victim->short_descr,victim->long_descr);
        ch->short_descr = str_dup( buf );
    sprintf(buf, "%s",victim->short_descr);
            ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_mask;
    af.level     = ch->level + ch->pcdata->discipline[OBFUSCATE];
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
return;
}


void do_cloakthegathering(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[OBFUSCATE] < 5)
    {
        send_to_char( "You are not skilled enough in Obfuscate.\n\r", ch );
        return;
    }

    if(ch->pblood < 50)
    {
        send_to_char("You do not have enough blood.\n\r",ch);
        return;
    }
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
    if ( !is_same_group( gch, ch ))
        continue;
    act( "$n slowly fades from sight.", gch, NULL, NULL, TO_ROOM );
    send_to_char( "You slowly fades from sight.\n\r", gch );

    if (IS_NPC(gch))
    {
    if(is_affected( gch, gsn_cloak_the_gathering ))
        affect_strip( ch, gsn_cloak_the_gathering );

    af.where     = TO_AFFECTS;
    af.type      = gsn_cloak_the_gathering;
    af.level     = ch->level/2;
    af.duration  = ch->level*ch->pcdata->discipline[OBFUSCATE];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( gch, &af );
    af.bitvector = AFF_SNEAK;
    affect_to_char( gch, &af );
    af.bitvector = AFF_HIDE;
    affect_to_char( gch, &af );
    } else {
    if(is_affected( gch, gsn_unseen ))
        affect_strip( ch, gsn_unseen );

    af.where     = TO_AFFECTS;
    af.type      = gsn_unseen;
    af.level     = ch->level/2;
    af.duration  = ch->level*ch->pcdata->discipline[OBFUSCATE];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( gch, &af );
    af.bitvector = AFF_SNEAK;
    affect_to_char( gch, &af );
    af.bitvector = AFF_HIDE;
    affect_to_char( gch, &af );
    }


    }
    send_to_char( "You surround your group in a veil of shadows.\n\r", ch );
    ch->pblood -= 40;
    return;
}

void do_unseenpresence(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int dice, success, level;
    if (IS_NPC(ch)) return;

    if (!can_use_disc(ch, DISC_OBFUSCATE, 2, 25, TRUE))
        return;

    if ( is_affected(ch, gsn_unseen) )
    {
        affect_strip( ch, gsn_sneak );
        affect_strip( ch, gsn_unseen);
        act( "$n steps out of nothingness.", ch, 0, 0, TO_NOTVICT );
        send_to_char( "You reveal yourself.\n\r", ch );
        return;
    }
    level = ch->pcdata->discipline[DISC_OBFUSCATE];
    ch->pblood -= 15;
    dice = get_attribute(ch, WITS) + get_ability(ch, CSABIL_STEALTH);
    success = godice(dice, UMAX(5, 9 - level));

    if (success < 1)
    {
        sendch("You fail to mask your presence to those around you.\n\r", ch);
        return;
    }

    act( "$n fades into the surroundings!", ch, 0, 0, TO_NOTVICT );
    send_to_char( "You fade into nothingness.\n\r", ch );

    af.where     = TO_AFFECTS;
    af.type      = gsn_sneak;
    af.level     = level;
    af.duration  = UMAX(6, level * 5);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );

    af.type      = gsn_unseen;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    return;
}

void do_veil(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if((ch->pcdata->discipline[OBFUSCATE] == 6 && ch->pblood < 60) || (ch->pcdata->discipline[OBFUSCATE] < 6 && ch->pblood < 40))
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[OBFUSCATE] < 1)
    {
        send_to_char( "You are not skilled enough in Obfuscate.\n\r", ch );
        return;
    }

    if ( is_affected(ch, gsn_unseen) || IS_AFFECTED2(ch, AFF2_VEIL) )
    {
        if (is_affected(ch, gsn_unseen))
        affect_strip(ch, gsn_unseen);
        else
        {
        REMOVE_BIT(ch->affected2_by, AFF2_VEIL);
        affect_strip(ch, gsn_veil);
        }
        act( "$n suddenly appears!", ch, 0, 0, TO_NOTVICT );
        send_to_char( "You drop your veil of shadows.\n\r", ch );
        return;
    }
        if(ch->pcdata->discipline[OBFUSCATE] == 6)
            ch->pblood -= 50;
        else ch->pblood -= 30;

        af.where     = TO_AFFECTS;
        if(ch->pcdata->discipline[OBFUSCATE] == 6) af.duration  = UMAX(24, ch->level*2);
        else af.duration = -1;
        af.level     = ch->level;
        af.modifier  = 0;
        af.location  = APPLY_NONE;

        if (ch->pcdata->discipline[OBFUSCATE] == 6)
        {
            af.type      = gsn_veil;
            af.bitvector = AFF_SNEAK;
            affect_to_char( ch, &af );
        }

        af.where    = TO_AFFECTS2;
        af.bitvector = AFF2_VEIL;
        af.type = gsn_veil;
        affect_to_char(ch, &af );

        send_to_char( "You veil yourself in darkness.\n\r", ch );
    return;
}

void do_vanish( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int door;
    AFFECT_DATA af;

    argument = one_argument( argument, arg1 );
    if (IS_NPC(ch)) return;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[OBFUSCATE] < 4)
    {
    send_to_char( "You are not trained enough in Obfuscate!\n\r", ch );
    return;
    }

    if (ch->pblood < 30)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
    send_to_char("Vanish where?\n\r", ch );
    return;
    }

         if ( !str_prefix( arg1, "n" ) || !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "e" ) || !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "s" ) || !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "w" ) || !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "u" ) || !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "d" ) || !str_prefix( arg1, "down"  ) ) door = 5;
    else
    {
    send_to_char( "You can only vanish in directions.\n\r", ch );
    return;
    }

    if ( (pexit = ch->in_room->exit[door] ) == NULL || IS_SET(pexit->exit_info, EX_CLOSED))
    {
    send_to_char( "You cannot vanish in that direction.\n\r", ch );
    return;
    }
    if ( ch->position != POS_FIGHTING )
    {
    send_to_char( "You are not fighting.\n\r", ch );
    return;
    }

    stop_fighting( ch, TRUE );
    to_room = ch->in_room->exit[door]->u1.to_room;

    char_from_room( ch );
    char_to_room( ch, to_room );
    affect_strip( ch, gsn_sneak );

    if (!IS_AFFECTED(ch,AFF_SNEAK))
        {
    af.where     = TO_AFFECTS;
    af.type      = gsn_sneak;
    af.level     = ch->level;
    af.duration  = ch->level*ch->pcdata->discipline[OBFUSCATE];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );
}
    af.where     = TO_AFFECTS2;
    af.type      = gsn_unseen;
    af.level     = ch->level;
    af.duration  = ch->level*ch->pcdata->discipline[OBFUSCATE];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    send_to_char("Using a bit of subtle misdirection, you fade from combat.\n\r", ch);
    ch->pblood -= 20;
    do_look( ch, "" );
}

void do_armsoftheabyss(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
   AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Who?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself! (Or at least you shouldn't pubicly)\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[OBTENEBRATION] < 3)
    {
        send_to_char( "You are not skilled enough in the arts of Obtenebration.\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(is_affected(victim,gsn_armsoftheabyss))
    {
        send_to_char("They are already entangled.\n\r",ch);
        return;
    }
    if ( ch->pblood < 25 && !is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if(!is_affected(ch,gsn_shadowform)) ch->pblood -= 15;
    if(number_range(1,100) < 30)
    {
        send_to_char("You fail to summon forth tendrils of darkness!\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_armsoftheabyss;
    af.level     = ch->level;
    af.duration  = ch->pcdata->discipline[OBTENEBRATION];
    af.location  = APPLY_STR;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    multi_hit( victim, ch, TYPE_UNDEFINED );

    act( "$n entangles you in tendrils of darkness!",  ch, NULL, victim, TO_VICT );
    act( "$n entangles $N in tendrils of shadow!",  ch, NULL, victim, TO_NOTVICT );
    act( "You entangle $N in tendrils of darkness!",  ch, NULL, victim, TO_CHAR );
    return;
}

void do_shadowform(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[OBTENEBRATION] < 5)
    {
        send_to_char( "You are not skilled enough in Obtenebration to assume the Shadow Form.\n\r", ch );
        return;
    }
    if ( is_affected( ch, gsn_claws ) )

        do_function(ch, &do_claws, " ");


    if ( is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You shift back into your humanoid form.\n\r", ch );

        affect_strip(ch,gsn_shadowform);
        ch->affected_by = race_table[ch->race].aff;
        act( "$n's form slowly shifts into a more humaniod form.", ch, NULL, NULL, TO_NOTVICT );

        if ( !IS_AFFECTED(ch, AFF_FANGS)) do_function(ch, &do_fangs, "" );
        return;
    }
    if (ch->pblood < 60)
    {
        send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }


    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if(obj->wear_loc != WEAR_NONE)
        unequip_char( ch, obj );
    }


    act( "Drawing upon the darkness lurking in your heart and soul, your body becomes a mass of shadows.", ch, NULL, NULL, TO_CHAR );
    act( "$n's form fades into a shadowy mass, writhing and undulating in beat to an unseen rythm.", ch, NULL, NULL, TO_NOTVICT );
    ch->pblood -= 50;

    af.where     = TO_AFFECTS;
    af.type      = gsn_shadowform;
    af.level     = ch->level;
    af.duration  = (ch->level*ch->pcdata->discipline[OBTENEBRATION]/2)-ch->gen;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( ch, &af );

    af.where     = TO_AFFECTS;
    af.type      = gsn_shadowform;
    af.level     = ch->level;
    af.duration  = (ch->level*ch->pcdata->discipline[OBTENEBRATION]/2)-ch->gen;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );

    af.where     = TO_AFFECTS;
    af.type      = gsn_shadowform;
    af.level     = ch->level;
    af.duration  = (ch->level*ch->pcdata->discipline[OBTENEBRATION]/2)-ch->gen;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_INVIS;
    affect_to_char( ch, &af );

    af.where     = TO_AFFECTS;
    af.type      = gsn_shadowform;
    af.level     = ch->level;
    af.duration  = (ch->level*ch->pcdata->discipline[OBTENEBRATION]/2)-ch->gen;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_PASS_DOOR;
    affect_to_char( ch, &af );
    ch->short_descr = str_dup( "A dark shadow" );
    sprintf(buf, "A dark shadow that cloaks the area in darkness");
        ch->shift = str_dup( buf );
return;
}

void do_shadowplay (CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   char arg[MIL];
   CHAR_DATA *victim;
     one_argument(argument, arg);


    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

        if (ch->pcdata->discipline[OBTENEBRATION] < 1)
    {
        send_to_char( "You are not skilled enough in your powers of Obtenebration!.\n\r", ch );
        return;
    }


    if (arg[0] == '\0')
    {
        if ( ch->pblood < 20 && !is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if(!is_affected(ch,gsn_shadowform)) ch->pblood -= 10;

    act( "$n vanishes into the shadows.",  ch, NULL, NULL, TO_NOTVICT );
    send_to_char("You veil yourself in darkness from those around you.\n\r" ,ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_invis;
    af.level     = ch->pcdata->discipline[OBTENEBRATION];
    af.duration  = (5+(ch->pcdata->discipline[OBTENEBRATION]*10))-ch->gen;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INVISIBLE;
    affect_to_char( ch, &af );
    return;
} else {
    WAIT_STATE( ch, 6 );
            if ( ch->pblood < 20 && !is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if(!is_affected(ch,gsn_shadowform)) ch->pblood -= 10;

        if ((victim = get_char_room(ch, arg)) != NULL)
        {
        sh_int success;
        success = godice(get_attribute(ch, WITS) + ch->pcdata->discipline[OBTENEBRATION], 7);
        if ( success > 0)
        {
            AFFECT_DATA af;
            act("You send shadows to blind and suffocate $N!", ch, NULL, victim, TO_CHAR);
            act("Shadows leap from the corners at $n's command and fill your mouth and eyes!", ch, NULL, victim, TO_VICT);
            act("$n commands shadows to leap into $N's mouth and eyes!", ch, NULL, victim, TO_ROOM);

            if (!is_affected(victim, gsn_shadowplay))
            {
            af.where     = TO_AFFECTS;
            af.type      = gsn_shadowplay;
            af.level     = ch->pcdata->discipline[OBTENEBRATION];
            af.duration  = 1 + ch->pcdata->discipline[OBTENEBRATION];
            af.location  = APPLY_CS_STA;
            af.modifier  = -(ch->pcdata->discipline[OBTENEBRATION]/2);
            af.bitvector = AFF_BLIND;
            affect_to_char( victim, &af );
            af.location  = APPLY_HITROLL;
            af.modifier  = -20*ch->pcdata->discipline[OBTENEBRATION];
            if (ch->pcdata->discipline[OBTENEBRATION] > 2)
                af.bitvector = AFF_SLOW;
            else
                af.bitvector = 0;
            affect_to_char( victim, &af );
            }

            if (success > 1)
                damage(ch, victim, success*(ch->level/2), gsn_shadowplay, DAM_BASH, TRUE);
        } else {
            send_to_char("You fail to summon shadows to suffocate your target.\n\r", ch);
            return;
        }
        } else {
            send_to_char("Attack whom with shadows?\n\r", ch);
            return;
        }
    }
}

void do_nightshades(CHAR_DATA *ch, char *argument)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[OBTENEBRATION] < 4)
    {
        send_to_char( "You are not skilled enough in your powers of Obtenebration!.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }


    if ( ch->pblood < 35 && !is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if (time_info.hour < 19 && time_info.hour > 5)
    {
        send_to_char( "You cannot control the shadows well enough during the day.\n\r",ch);
        return;
    }
    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two shades at once!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_OBTEN_SERVANT)) == NULL )
    {
        send_to_char( "Error: please infrorm the Coders!\n\r", ch );
        return;
    }
    if(!is_affected(ch,gsn_shadowform)) ch->pblood -= 15;
    mob = create_mobile( pMobIndex );
    char_to_room( mob, ch->in_room );
    mob->level  = ch->level;
    mob->max_hit = ch->max_hit;
    mob->hitroll = ch->hitroll;
    mob->damroll = ch->damroll;
    mob->hit = mob->max_hit;
    ch->pet = mob;
    add_follower( mob, ch );
    mob->leader = ch;
    act( "$n weaves and tugs at the darkness of the Abyss, summonning forth $N!", ch, NULL, mob, TO_NOTVICT );
    send_to_char("You extend your will, pulling tangible darkness from the Abyss itself, and form it into a willing doppleganger.\n\r", ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->pcdata->discipline[OBTENEBRATION];
    af.duration  = ch->level*ch->pcdata->discipline[OBTENEBRATION]+10;  /*tmp hack while I fix time code */
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );
    return;
}

void do_shadowwalk(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[OBTENEBRATION] < 6)
    {
        send_to_char( "You are not skilled enough in your powers of Obtenebration!.\n\r", ch );
        return;
    }
    if(ch->clan != clan_lookup("lasombra"))
    {
        send_to_char("Only members of the Lasombra clan have this degree of mastery over the shadows.\n\r",ch);
        return;
    }
    if(IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char("You must leave the Arena first!\n\r",ch);
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }


    if ( ch->pblood < 30+ch->gen )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Shadow walk to whom?\n\r", ch );
        return;
    }
    SET_BIT(ch->act, PLR_HOLYLIGHT);
    if ( (victim = get_char_world(ch, argument)) == NULL)
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    REMOVE_BIT(ch->act, PLR_HOLYLIGHT);

    if ( !room_is_dark( ch->in_room ) && !IS_SET(ch->imm_flags,IMM_SUNLIGHT))
    {
        send_to_char( "You need total darkness to do this.\n\r",ch);
        return;
    }
    if ( !room_is_dark( victim->in_room ) && (time_info.hour < 19 && time_info.hour > 5))
    {
        send_to_char( "You sense no shadows dark enough around your victim to walk through.\n\r",ch);
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(IS_SET(ch->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_QUIT)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL))
    {
        send_to_char( "You are not powerful enough.\n\r", ch );
        return;
    }

    if (room_is_private(victim->in_room))
    {
        send_to_char( "That room is private right now.\n\r", ch );
        return;
    }


    if(!IS_NPC(victim))
    {
        send_to_char( "That being is too powerful to shadowwalk to.\n\r", ch );
        return;
    }
    ch->pblood -= 20-ch->gen;
    act("The shadows seem to reach out and envelop $n... as the darkness vanishes, so does $e.",ch,NULL,victim,TO_ROOM);
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    WAIT_STATE( ch, 24 );
    act("The shadows writhe and crawl along the walls and floor as $n steps from within the darkness.",ch,NULL,victim,TO_ROOM);
    do_function(ch, &do_look, "auto" );
    return;
}

void do_shroudofnight(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[OBTENEBRATION] < 2)
    {
        send_to_char( "You are not skilled enough in your powers of Obtenebration!.\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }


    if ( ch->pblood < 40 && !is_affected(ch,gsn_shadowform))
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if ( is_affected( ch, gsn_cloakshadow ) )
    {
        send_to_char( "You release the shadow cloaking you.\n\r", ch );
        affect_strip(ch,gsn_cloakshadow);
        return;
    }

    if(!is_affected(ch,gsn_shadowform)) ch->pblood -= 30;
    af.where     = TO_IMMUNE;
    af.type      = gsn_cloakshadow;
    af.level     = ch->level;
    af.duration  = 20-ch->gen;
    if (ch->clan == clan_lookup("Lasombra"))
        af.duration += 12;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = IMM_SUNLIGHT;
    affect_to_char( ch, &af );
    send_to_char( "You call darkness to protect you against the sun.\n\r", ch );
    WAIT_STATE(ch, 8);
    return;
}

/* New Awe code by Sengir*/

void do_awe(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int dicesuccess = 0;

    if (IS_NPC(ch))
        return;
    if (!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!", ch);
        return;
    }
    if (is_affected(ch, gsn_awe))
    {
        send_to_char("You have already affected your social magnetism.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_botched_presence))
    {
        send_to_char("You have not quite gotten over your last social disgrace.\n\r", ch);
        return;
    }

    if (IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood has been cursed, and you are unable to bolster your natural charm!\n\r", ch);
        return;
    }

    if (!IS_VAMP(ch))
    {
        send_to_char("You do not possess supernatural magnetism to augment!\n\r", ch);
        return;
    }

    if (ch->pcdata->discipline[PRESENCE] < 1)
    {
        send_to_char("You cannot expect to have others bend to your social graces without learning the basics of Presence first!\n\r", ch);
        return;
    }

    if (!IS_AFFECTED(ch, AFF_FANGS) && ch->race != race_lookup("ghoul"))
    {
        send_to_char("Your fangs aren't extended!\n\r", ch);
        return;
    }

    if (ch->pblood < 15)
    {
        send_to_char("You don't have enough blood to Awe those around you.\n\r", ch);
        return;
    }

    ch->pblood -= 10;

    send_to_char("Flashing your most charming smile, you draw into your personal reserves and\n\rattempt to augment your supernatural attraction...\n\r", ch);

    dicesuccess = godice(get_attribute(ch, CHARISMA) + ch->csabilities[CSABIL_EXPRESSION], 7);

    if (dicesuccess < 0)
    {
        act("...but only seem to make yourself look like a fool!", ch, NULL, NULL, TO_CHAR);
        act("$n flashes a toothy grin, only making themselves look much more comical and less dignified.", ch, NULL, NULL, TO_NOTVICT);

        af.where    = TO_AFFECTS;
        af.type     = gsn_botched_presence;
        af.level    = ch->pcdata->discipline[PRESENCE];
        af.duration = 2 + ch->pcdata->discipline[PRESENCE];
        af.location = APPLY_CS_CHA;
        af.modifier = dicesuccess;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_botched_presence;
        af.level    = ch->pcdata->discipline[PRESENCE];
        af.duration = 2 + ch->pcdata->discipline[PRESENCE];
        af.location = APPLY_CS_MAN;
        af.modifier = dicesuccess;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    else if (dicesuccess == 0)
    {
        act("...but nobody seems to take any extra notice of you.", ch, NULL, NULL, TO_CHAR);

        af.where    = TO_AFFECTS;
        af.type     = gsn_botched_presence;
        af.level    = ch->pcdata->discipline[PRESENCE];
        af.duration = 1;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = 0;
        affect_to_char( ch, &af);

        return;
    }

    else if (dicesuccess > 0)
    {
        if (dicesuccess >= 5)
            send_to_char("...and gain the nearly undivided attention of everyone nearby.\n\r", ch);
        if (dicesuccess == 4 || dicesuccess == 3)
            send_to_char("...and feel many eyes in the room turn to regard you with awe.\n\r", ch);
        if (dicesuccess == 2 || dicesuccess == 1)
            send_to_char("...and notice a few people regarding you with approval.\n\r", ch);
        act("You turn and notice $n, whose very presence seems to fill you with awe.", ch, NULL, NULL, TO_NOTVICT);


        af.where    = TO_AFFECTS;
        af.type     = gsn_awe;
        af.level    = ch->pcdata->discipline[PRESENCE];
        af.duration = dicesuccess * 9;
        af.location = APPLY_HITROLL;
        af.modifier = ((dicesuccess/2)+1)*ch->level/3 + 10;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        gain_exp(ch, dicesuccess * 6);
    }
    return;
}


void do_majesty(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;

    if (IS_NPC(ch)) return;

   if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( is_affected( ch, gsn_majesty ) )
    {
        send_to_char( "You release the aura of superiority and power surrounding you.\n\r", ch );
        affect_strip(ch, gsn_majesty);
        return;
    }



    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (ch->pcdata->discipline[PRESENCE] < 5)
    {
        send_to_char( "You are not skilled enough in Presence!\n\r", ch );
        return;
    }



    if ( ch->cswillpower < 1 )
    {
        send_to_char( "You do not have the strength of will to bolster your social magnificence.\n\r", ch);
        return;
    }

    if ( ch->pblood < 15 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }

    ch->pblood -= 10;
    ch->cswillpower--;

    af.where     = TO_AFFECTS2;
    af.type      = gsn_majesty;
    af.level     = ch->level;
    af.location  = 0;
    af.modifier  = 0;
    af.duration  = 24;
    af.bitvector = AFF2_MAJESTY;
    affect_to_char( ch, &af );

    send_to_char("You radiate an almost palpable aura of mystique, power, intrigue, and greatness; very few dare to oppose you.\n\r" ,ch);
    act( "You take special notice of $n, realizing with astonishing clarity how truly magnificent $e is.",  ch, NULL, NULL, TO_NOTVICT );
    return;
}


void do_entrancement(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *victim;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;
        if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char("Entrance whom to do what?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Nobody here by that name.\n\r", ch );
        return;
    }
        if (!IS_NPC(victim))
        {
                send_to_char("Disabled on players due to the potential for abuse.\n\r", ch);
                return;
        }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (ch->pcdata->discipline[PRESENCE] < 3)
    {
        send_to_char( "You are not skilled enough in Presence!.\n\r", ch );
        return;
    }
    if (!str_prefix(argument,"delete"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"kill"))
    {
        send_to_char("They refused to because it maybe dangerous.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"cast"))
    {
        send_to_char("They refused to because it maybe dangerous.\n\r",ch);
        return;
    }

    if (!str_prefix(argument,"zap"))
    {
        send_to_char("They refused to because it maybe dangerous.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"brandish"))
    {
        send_to_char("They refused to because it maybe dangerous.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"recite"))
    {
        send_to_char("They refused to because it maybe dangerous.\n\r",ch);
        return;
    }
    if (!str_prefix(argument,"ic"))
    {
        send_to_char("Nope!\n\r",ch);
        return;
    }
    if (!str_prefix(argument, "quit"))
    {
        send_to_char("I don't think so!\n\r", ch);
        return;
    }
    if (!str_prefix(argument, "give"))
    {
        send_to_char("They shake their head and refuse your power!\n\r", ch);
        return;
    }
    if (!str_prefix(argument, "drop"))
    {
        send_to_char("They shake their head and refuse your power!\n\r", ch);
        return;
    }
    if (!str_prefix(argument, "remove"))
    {
        send_to_char("They shake their head and refuse your power!\n\r", ch);
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "You get a headache trying to entrance yourself.\n\r", ch );
        return;
    }

    if ( IS_IMMORTAL(victim))
    {
        send_to_char( "Yeah, right.\n\r", ch );
        return;
    }
    if ( ch->pblood < 40 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 30;
    if(victim->race == race_lookup("vampire") && ch->gen > victim->gen)
    {
        send_to_char( "You cannot command your betters!\n\r", ch );
        sprintf( buf, "%s foolishly attempts to dominate your mind!\n\r", ch->name );
        send_to_char(buf,victim);
        return;
    }
    if ( IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   ch->level < victim->level
    ||   IS_SET(victim->imm_flags,IMM_CHARM)
    ||   saves_spell( ch->level, victim,DAM_CHARM))
    {
        sprintf( buf, "%s resists your power of Presence!\n\r", victim->name );
        send_to_char(buf,ch);
        sprintf( buf, "%s attempts to entrance you into doing a favor but fails!\n\r", ch->name );
        send_to_char(buf,victim);
        return;
    }

    sprintf( buf, "%s entrances you into doing a favor, forcing you to '%s'.\n\r", ch->name, argument );
    send_to_char(buf,victim);
    sprintf( buf, "You entrance %s to '%s'.\n\r", victim->name, argument );
    send_to_char(buf,ch);
    act( "$n entrances $N into doing a favor.",  ch, NULL, victim, TO_NOTVICT );
    interpret( victim, argument );
    return;
}

void do_summon(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if (ch->pcdata->discipline[PRESENCE] < 4)
    {
        send_to_char( "You are not skilled enough in Presence!\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }


    if ( ch->pblood < 70 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL)
    {
        send_to_char("Send a mental summons to whom?\n\r",ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(victim == ch
    ||   victim->in_room == NULL
    ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
	||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE))
    ||   victim->fighting != NULL
    ||   (IS_NPC(victim) && IS_SET(victim->imm_flags,IMM_SUMMON))
    ||   (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    ||   (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOSUMMON))
    ||   (IS_NPC(victim) && saves_spell( ch->level, victim,DAM_OTHER)) )
    {
    send_to_char( "You send a mental summons, but seem to fail.\n\r", ch );
    ch->pblood -= 30;
    return;
    }


    ch->pblood -= 60;
    act( "You send a mental nudge out to $N, beckoning $M to come swiftly.", ch, NULL, victim, TO_CHAR );
    act( "$n summons forth $N!", ch, NULL, victim, TO_NOTVICT );
    act( "$n disappears suddenly.", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    act( "You feel a mental tug from $n, and immediately rush to $s side.", ch, NULL, victim,   TO_VICT );
    do_function(victim, &do_look, "auto" );
    return;
}

void do_dreadgaze(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

   if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if (ch->pcdata->discipline[PRESENCE] < 2)
    {
        send_to_char( "You are not skilled enough in Presence!\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }


    if ( ch->pblood < 25 )
    {
        send_to_char( "You don't have enough blood.\n\r", ch );
        return;
    }
    if ((victim = get_char_room( ch, argument ))== NULL)
    {
        send_to_char( "Frighten whom?\n\r", ch );
        return;
    }
    if(victim->level > 3*ch->level/2)
    {
        send_to_char("You are not powerful enough to force them to flee!\n\r",ch);
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(!IS_NPC(victim) && victim->race == race_lookup("vampire") && ch->gen > victim->gen)
    {
        send_to_char("They are too close to Caine to affect in this way.\n\r",ch);
        return;
    }
    if(victim->race == race_lookup("methuselah") && ch->race != race_lookup("methuselah"))
    {
        send_to_char("They are too ancient and wise to be controlled in this manner.\n\r",ch);
        return;
    }
    ch->pblood -= 15;

    act( "$n flashes a dreadful gaze at $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "You flash a dreadful gaze at $N!",  ch, NULL, victim, TO_CHAR );
    act( "$n frightens you to death with a Dreadful Gaze!",  ch, NULL, victim, TO_VICT );
    do_function(victim, &do_flee, "auto" );
    return;
}

void do_gleam(CHAR_DATA *ch, char *argument)
{

    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if(ch->pcdata->discipline[PROTEAN] == 0)
    {
        send_to_char( "Are not trained in Protean.\n\r", ch );
        return;
    }

    if (ch->pblood < 10)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if (is_affected(ch, gsn_gleam)) {
    affect_strip(ch, gsn_gleam);
    act( "$n's eyes cease to glow bright red.", ch, 0, 0, TO_NOTVICT );
    send_to_char( "Your eyes cease to glow as your vitae enhanced vision returns to normal.\n\r", ch );
    return;
    }

    send_to_char("You cull forth the Beast and your eyes begin to glow red.\n\r",ch);
    act( "$n lets forth a savage growl and their eyes begin glowing red.", ch, 0, 0, TO_NOTVICT );
    ch->pblood -= 5;
    af.where     = TO_AFFECTS;
    af.type      = gsn_gleam;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = APPLY_CS_CHA;
    af.modifier  = -1;
    af.bitvector = AFF_INFRARED;
    affect_to_char( ch, &af );
    af.where     = TO_AFFECTS;
    af.type      = gsn_gleam;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = APPLY_CS_MAN;
    af.modifier  = -1;
    af.bitvector = AFF_DARK_VISION;
    affect_to_char( ch, &af );
    return;
}


void do_claws(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    OBJ_DATA *claws;
    OBJ_DATA *weapon;

    if (IS_NPC(ch)) return;

    if (ch->race == race_lookup("garou"))
    {
        if (is_affected(ch, gsn_claws))
        {
            if (ch->pcdata->shiftform == HISPO || ch->pcdata->shiftform == LUPUS)
            {
                sendch("You cannot fight with fists.. You're a wolf.\n\r", ch);
                return;
            }
            affect_strip(ch, gsn_claws);
            sendch("You will no longer use your claws in combat.\n\r", ch);
            return;
        }

        if (ch->pcdata->shiftform == HOMID)
        {
            sendch("You cannot use claws in Homid form!\n\r", ch);
            return;
        }

        sendch("You will now use your claws for unarmed combat.\n\r", ch);

        af.where     = TO_AFFECTS;
        af.type      = gsn_claws;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );
        return;
    }//if garou
    if((!IS_VAMP(ch)))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }



    weapon = get_eq_char(ch,WEAR_WIELD);
    if (weapon != NULL)
    {
        if ( is_affected( ch, gsn_claws ) || weapon->pIndexData->vnum == OBJ_VNUM_CLAWS)
        {
            affect_strip(ch,gsn_claws);
            if(weapon == NULL) return;
            extract_obj(weapon);
            send_to_char("Your claws slide back under your nails.\n\r", ch);
            act("$n's claws slide back under their fingernails.",ch,NULL,NULL,TO_NOTVICT);
            return;
        }
    }
    if( weapon != NULL)
    {
        send_to_char( "You cannot have claws and wield a weapon as well!\n\r", ch );
        return;
    }

    if (IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[PROTEAN] < 2)
    {
        send_to_char("You do not have enough skill in the arts of Protean.\n\r",ch);
        return;
    }

    if (ch->pblood < 30)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    ch->pblood -= 20;
    claws = create_object(get_obj_index(OBJ_VNUM_CLAWS), 0);
    claws->level = ch->level;
    claws->value[1] = 3*ch->level/5 + (ch->gen <= 8 ? 30:20);
    claws->value[2] = 1 + ch->pcdata->discipline[PROTEAN];
    obj_to_char(claws,ch);
    equip_char( ch, claws, WEAR_WIELD );

    af.where     = TO_AFFECTS;
    af.type      = gsn_claws;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    send_to_char( "Your claws extend from your fingernails.\n\r", ch );
    act("Claws suddenly slide out from under $n's fingernails!",ch,NULL,ch,TO_NOTVICT);
    return;
}

void do_earthmeld(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pblood < 30)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[PROTEAN] < 3)
    {
        send_to_char( "You are not skilled enough in Protean.\n\r", ch );
        return;
    }

    if ((ch->in_room->sector_type == SECT_INSIDE) || (ch->in_room->sector_type == SECT_UNUSED) || (ch->in_room->sector_type == SECT_HOT) || (ch->in_room->sector_type == SECT_COLD) || (ch->in_room->sector_type == SECT_NODE))
    {
        send_to_char( "You cannot meld through the ground here!\n\r", ch);
        return;
    }

    if (ch->in_room->sector_type == SECT_AIR)
    {
        send_to_char( "There is no earth to meld with up in the sky!\n\r", ch);
        return;
    }

    if ((ch->in_room->sector_type == SECT_WATER_SWIM) || (ch->in_room->sector_type == SECT_WATER_NOSWIM) || (ch->in_room->sector_type == SECT_WATER_DROWN))
    {
        send_to_char( "You cannot meld with water, only earth.\n\r", ch);
        return;
    }

    act("$n sinks into the ground, becoming one with the very earth and disappearing from view.", ch, NULL, NULL, TO_ROOM);

    af.where     = TO_AFFECTS2;
    af.type      = gsn_earthmeld;
    af.level     = ch->level;
    af.duration  = ch->level + 5 + (2 * ch->pcdata->discipline[PROTEAN]);
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    ch->pblood -= 20;

    act( "Willing your body into the ground, you assimilate yourself into the very earth, protecting yourself from the elements.",  ch, NULL, NULL, TO_CHAR );

    return;
}

/*Sengir got a hold of shifting...changed duration, and added CS mods*/
void do_shift(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


        if (is_affected(ch, gsn_vicissitude_horrid)) {
        sendch("You have already altered your form!\n\r", ch);
        return;
        }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[PROTEAN] < 4)
    {
        send_to_char( "You are not skilled enough in Protean to shift forms.\n\r", ch );
        return;
    }
    if ( is_affected( ch, gsn_claws ) )
        do_function(ch, &do_claws, " ");

    if ( arg[0] == '\0')
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You shift back into your humanoid form.\n\r", ch );

            affect_strip(ch,gsn_reveal);
            affect_strip(ch,gsn_shift);
            if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
            ch->affected_by = race_table[ch->race].aff;
            act( "$n's form slowly shifts into a more humanoid form.", ch, NULL, NULL, TO_NOTVICT );
            ch->dam_type = 17;
            if ( !IS_AFFECTED(ch, AFF_FANGS)) do_function(ch, &do_fangs, "" );
            return;
        }

        send_to_char( "You may shift into bat, wolf, rat, tiger, bear, and raven forms.\n\r", ch );
        return;
    }

    WAIT_STATE(ch, 80);

    if ( !str_prefix( arg, "bat" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!\n\r", ch );
            return;
        }
        if (ch->pblood < 20)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a bat.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a bat.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 10;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = -(ch->level*2);
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_MOVE;
        af.modifier = ch->level*4;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_FLYING;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_INVIS;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_HIDDEN;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STR;
        af.modifier = -(get_attribute(ch,STRENGTH)-1);
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A strange bat" );
        sprintf(buf, "A strange bat flittering about the room");
            ch->shift = str_dup( buf );
        return;
    }

    if ( !str_prefix( arg, "rat" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!\n\r", ch );
            return;
        }
        if (ch->pblood < 20)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a rat.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a rat.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 10;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = -(ch->level*2);
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SNEAK;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_HIDE;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STR;
        af.modifier = -(get_attribute(ch,STRENGTH)-1);
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "An ugly rat" );
        sprintf(buf, "An ugly rat searching for food");
            ch->shift = str_dup( buf );
        return;
    }

    if ( !str_prefix( arg, "wolf" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 50)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }
        act( "Your body slowly shifts forms into a wolf.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a wolf.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 40;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_MOVE;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_CS_STR;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_DEX;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STA;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A large mountain wolf" );
        sprintf(buf, "A large mountain wolf");
            ch->shift = str_dup( buf );
        return;
    }

//* Zelans extra form code below

    if ( !str_prefix( arg, "tiger" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 50)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a tiger.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a tiger.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 40;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_MOVE;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_CS_STR;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_DEX;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STA;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A powerful looking tiger" );
        sprintf(buf, "A powerful looking tiger");
            ch->shift = str_dup( buf );
        return;
    }
//Raven form

 if ( !str_prefix( arg, "raven" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 30)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a raven.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a raven.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 20;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_MOVE;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_CS_STR;
        af.modifier  = 1;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_DEX;
        af.modifier = 3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STA;
        af.modifier = 1;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_MAN;
        af.modifier = -1;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A black raven" );
        sprintf(buf, "A black raven");
            ch->shift = str_dup( buf );
        return;
    }
// Bear form
 if ( !str_prefix( arg, "bear" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 90)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a bear.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a bear.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 80;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = 5*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_MOVE;
        af.modifier  = 3*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_CS_STR;
        af.modifier  = 4;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_DEX;
        af.modifier = -1;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STA;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A strong looking bear" );
        sprintf(buf, "A strong looking bear");
            ch->shift = str_dup( buf );
        return;
    }
// Shadowolf form
 if ( !str_prefix( arg, "shadowwolf" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 50)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }
if(ch->pcdata->discipline[OBTENEBRATION] < 1)
{
        send_to_char( "You are not skilled enough in obtenebration!.\n\r", ch );
        return;
}
        act( "Your body slowly shifts forms into a shadow wolf.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts their form into that of a shadow wolf.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 45;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_HIT;
        af.modifier  = 10*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_MOVE;
        af.modifier  = 10*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_CS_STR;
        af.modifier  = 3;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_DEX;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_STA;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_shift;
        af.level    = ch->pcdata->discipline[PROTEAN];
        af.duration = 24;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A shadowy wolf" );
        sprintf(buf, "A shadowy wolf");
            ch->shift = str_dup( buf );
        return;
    }
// Easter egg (Remove it at some point, syrup form)
if ( !str_prefix( arg, "syrup" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first!.\n\r", ch );
            return;
        }
        if (ch->pblood < 50)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into good canadian maple syrup.", ch, NULL, NULL, TO_CHAR );

        act( "$n shifts their form into that of good canadian maple syrup.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 1;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;

        ch->short_descr = str_dup( "A pile of gooey yummy canadian syrup" );
        sprintf(buf, "A pile of gooey yummy canadian syrup");
            ch->shift = str_dup( buf );
        return;
    }

// Zelans extra form code above

    if ( !str_prefix( arg, "mist" ) )
    {
        if ( IS_AFFECTED(ch, AFF_SHIFT))
        {
            send_to_char( "You must return to vampire form first.\n\r", ch );
            return;
        }
        if (ch->pblood < 20)
        {
            send_to_char( "You don't have enough blood!\n\r", ch );
            return;
        }

        if(ch->pcdata->discipline[PROTEAN] < 5)
        {
            send_to_char( "You are not yet skilled enough to gain this complex form.\n\r", ch );
            return;
        }

        act( "Your body slowly shifts forms into a watery mist.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of a watery mist.", ch, NULL, NULL, TO_NOTVICT );
        ch->pblood -= 15;
        affect_strip(ch,gsn_reveal);

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_HIT;
        af.modifier  = -(3*ch->level);
        af.bitvector = AFF_PASS_DOOR;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_FANGS;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_FLYING;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_INVISIBLE;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_DETECT_INVIS;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS2;
        af.type      = gsn_shift;
        af.level     = ch->pcdata->discipline[PROTEAN];
        af.duration  = 24+ch->level;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF2_MIST;
        affect_to_char( ch, &af );

        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        ch->short_descr = str_dup( "A swirling mist" );
        sprintf(buf, "A watery mist swirling around");
            ch->shift = str_dup( buf );
        return;
    }

send_to_char( "You may shift into bat, wolf, tiger, bear, rat, and raven forms.\n\r", ch );
return;
}


/*Sengir made weakness more TT-ish */
void do_weakness(CHAR_DATA *ch, char *argument)
{
   char arg[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   AFFECT_DATA af;
    int dicesuccess = 0;
    int wpdiff;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if(!IS_VAMP(ch))
    {
        send_to_char("You do not have any vampiric blood to change.\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Whom do you wish to affect with the toxin?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("The curse laid upon your blood prevents you from creating the toxin.\n\r" ,ch);
        return;
    }
    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("If you weaken the shopkeeper, you won't be served in the future.\n\r",ch);
        return;
    }

    if (ch->pcdata->discipline[QUIETUS] < 2)
    {
        send_to_char( "You must progress beyond the rudimentaries in Quietus to learn the art of transmuting your blood into toxin.\n\r", ch );
        return;
    }

    if ( ch->pblood < 20 )
    {
        send_to_char( "Transmuting blood now will leave you dangerously low.\n\r", ch );
        return;
    }

    if (is_affected(victim, gsn_quietus_weakness))
    {
        send_to_char("That victim is already suffering from the affects of toxic blood.\n\r", ch);
        return;
    }

    ch->pblood -= 20;

    send_to_char("You channel your vitae to seep onto the palms of your hands, transmuting the blood into a mystical toxin.\n\r", ch);

    dicesuccess = godice(get_attribute(ch, DEXTERITY)+ch->csabilities[CSABIL_BRAWL], 6);

    if (dicesuccess < 0)
    {
        act("You reach forward and try to grab $N, but instead trip and fall to the ground; the toxin smears onto the ground, wasted.", ch, NULL, victim, TO_CHAR);
        act("$n takes a step forward, before falling gracelessly to the ground.", ch, NULL, victim, TO_VICT);
        act("$n trips and falls to the ground while approaching $N.", ch, NULL, victim, TO_NOTVICT);
        ch->position = POS_RESTING;
        WAIT_STATE(ch, 18);
        return;
    }

    if (dicesuccess == 0)
    {
        act("You reach out and grasp towards $N, but clumsily miss your grab; the toxin loses its potency quickly.", ch, NULL, victim, TO_CHAR);
        act("$n reaches out and clumsily grasps at you, missing you by a foot.", ch, NULL, victim, TO_VICT);
        act("$n reaches out clumsily and tries to grab $N, missing $s target by quite a bit.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, 9);
        return;
    }

    act("You reach out and grab $N firmly, allowing the toxin to seep onto $S body.", ch, NULL, victim, TO_CHAR);
    act("$n reaches out and clutches you firmly.", ch, NULL, victim, TO_VICT);
    act("$n reaches over and grabs $N forcefully.", ch, NULL, victim, TO_NOTVICT);

    gain_exp(ch, dicesuccess);

    WAIT_STATE(ch, 12);

    if(IS_NPC(victim))
    {
        wpdiff = 4;

    }

    else
        wpdiff = get_attribute(ch, STAMINA) + ch->pcdata->discipline[FORTITUDE];

    dicesuccess = godice(ch->cswillpower, wpdiff);

    if (dicesuccess < 0)
    {
        act("Startled by the toxin's apparent refusal to infect $N, you do not resist when $E breaks from your grasp and attacks!", ch, NULL, victim, TO_CHAR);
        act("You break from from $n's hold, and attack $m in anger!", ch, NULL, victim, TO_VICT);
        act("$N breaks free from $n and proceeds to attack $m.", ch, NULL, victim, TO_NOTVICT);
        multi_hit( victim, ch, TYPE_UNDEFINED );
        return;
    }

    if (dicesuccess == 0)
    {
        act("You let go of $N, disgusted as the toxin seems to have failed.", ch, NULL, victim, TO_CHAR);
        act("$n loosens $S grip, letting you go suddenly.", ch, NULL, victim, TO_VICT);
        act("$n roughly lets go of $S grip on $N.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

        act("Letting go of $N, you smile as the toxin seeps into $s body, slowly stripping away $S resilience.", ch, NULL, victim, TO_CHAR);
        act("$n lets go of you, but you feel a little worn down and ill.", ch, NULL, victim, TO_VICT);
        act("$n lets go of $N, but $E looks a little under the weather.", ch, NULL, victim, TO_NOTVICT);

        gain_exp(ch, dicesuccess);

        multi_hit( victim, ch, TYPE_UNDEFINED);

    if (IS_NPC(victim))
    {
        af.where     = TO_AFFECTS;
        af.type      = gsn_quietus_weakness;
        af.level     = ch->pcdata->discipline[QUIETUS];
        af.duration  = dicesuccess  * (15 - ch->gen);
        af.location  = APPLY_CON;
        af.modifier  = -5;
        af.bitvector = 0;
        affect_to_char( victim, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_quietus_weakness;
        af.level    = ch->pcdata->discipline[QUIETUS];
        af.duration = dicesuccess  * (15 - ch->gen);
        af.location = APPLY_HIT;
        af.modifier = -victim->level * 5;
        af.bitvector    = 0;
        affect_to_char( victim, &af );

        return;
    }

    af.where    = TO_AFFECTS;
    af.type     = gsn_quietus_weakness;
    af.level    = ch->pcdata->discipline[QUIETUS];
    af.duration = dicesuccess * (15 - ch->gen);
    af.location = APPLY_CS_STA;
    af.modifier = -ch->pcdata->discipline[QUIETUS];
    af.bitvector    = 0;
    if (get_attribute(victim, STAMINA) < af.modifier)
        af.modifier = get_attribute(victim, STAMINA);
    affect_to_char( victim, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_quietus_weakness;
    af.level    = ch->pcdata->discipline[QUIETUS];
    af.duration = dicesuccess * (15 - ch->gen);
    af.location = APPLY_HIT;
    af.modifier = -(ch->level * ch->pcdata->discipline[QUIETUS]) * 5;
    af.bitvector    = 0;
    if (victim->max_hit < af.modifier)
        af.modifier = victim->max_hit - 10;
    affect_to_char( victim, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_quietus_weakness;
    af.level    = ch->pcdata->discipline[QUIETUS];
    af.duration = dicesuccess * (15 - ch->gen);
    af.location = APPLY_HITROLL;
    af.modifier = -ch->level * ch->pcdata->discipline[QUIETUS];
    af.bitvector    = 0;
    affect_to_char( victim, &af );


    return;
}

void do_acidblood(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    AFFECT_DATA af;
//  char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
//  int dam;            /*To be passed into the damage code*/
    int dicesuccess = 0;        /*For determining if hit succeeds*/
    int damagesuccess = 0;      /*For determining damage success*/
    int transmutedblood;        /*For determining damage dice*/

    if(ch->gen >= 15) transmutedblood = 1;
    else transmutedblood = UMIN(5,(14 - ch->gen));



    if (IS_NPC(ch))
        return;

    argument = one_argument(argument, arg);

    if (!IS_VAMP(ch))
    {
        send_to_char("You don't have any vampiric vitae to transmute!\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, arg)) == NULL)
    {
        send_to_char("Spit caustic blood at whom?\n\r", ch);
        return;
    }

    if (victim == ch)
    {
        send_to_char("You are immune to your own blood acid.\n\r", ch);
        return;
    }

    if (ch->stopped > 0)
    {
/*      if IS_AFFECTED(ch, gsn_stoptheclock)
            send_to_char("Your body is frozen in time!\n\r", ch);
        else
            send_to_char("Your throat still hasn't completely cleared of acid.\n\r", ch);
*/
        send_to_char("You are unable to spit blood right now.\n\r",ch);
        return;
    }

    if (IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("The curse upon your blood prevents you from transmuting your own vitae into acid.\n\r", ch);
        return;
    }

    if (ch->pcdata->discipline[QUIETUS] < 5)
    {
        send_to_char("Haqim has not deemed you worthy enough for this potent gift.\n\r", ch);
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that corroding the shopkeeper might prevent you from purchasing goods in the future.\n\r", ch);
        return;
    }

    if (ch->pblood <= (3 * transmutedblood) + 10)
    {
        send_to_char("Transmuting blood now will leave you dangerously low on precious vitae.\n\r", ch);
        return;
    }

    if (!IS_AFFECTED(ch, AFF_FANGS) && ch->race != race_lookup("ghoul"))
    {
        send_to_char("You must extend your fangs first before spitting at your enemies.\n\r", ch);
        return;
    }


    WAIT_STATE(ch, 20);

    ch->pblood -= (3 * transmutedblood);

    send_to_char("Focusing upon the birthright of Haqim, you transmute precious vitae into a virulent acid and project it towards your enemy!\n\r", ch);

    dicesuccess = godice(get_attribute(ch,STAMINA)+ch->csabilities[CSABIL_ATHLETICS], 6);

        damagesuccess = godice(2 * transmutedblood, 6);

    if (damagesuccess <0)    /*can't -botch- a damage roll*/
        damagesuccess = 0;


    if (dicesuccess < 0)
    {
        act("Choking violently, you begin coughing up globules of acid that have begun welling up in your throat.", ch, NULL, victim, TO_CHAR);
        act("$n bends over and coughs violently, expelling globs of acid onto the ground.", ch, NULL, victim, TO_NOTVICT);
        ch->stopped =  2;
        WAIT_STATE(ch, 4);
        return;
    }

    else if (dicesuccess == 0)
    {
        act("Misjudging the trajectory of your projectile, the acid harmlessly misses $N.", ch, NULL, victim, TO_CHAR);
        if(!IS_NPC(victim))
            act("You watch in horror and fascination as a stream of corrosive acid barely misses striking you.", ch, NULL, victim, TO_VICT);
        act("$n spits a stream of acid at $N, but narrowly misses.", ch, NULL, victim, TO_NOTVICT);
        return;
    }

    else if (dicesuccess > 0)
    {
        act("Haqim smiles upon thee, as your acidic blood strikes true upon $N; the acrid smell of corroding flesh wafts into your nostrils.", ch, NULL, victim, TO_CHAR);
        if(!IS_NPC(victim))
            act("Intense pain burns its way through your body as $n's acidic blood collides with your body, washing over your skin.", ch, NULL, victim, TO_VICT);
        act("$n spits a stream of acid and hits $N with full force.", ch, NULL, victim, TO_NOTVICT);
        gain_exp(ch, dicesuccess);
    }

    damage(ch, victim, (damagesuccess*get_modifier(ch, MODTYPE_GODLY)), gsn_acidblood, DAM_ACID, TRUE);

    if (dicesuccess > 4)        /*Many successes, great aim.  Blind baby!*/
    {
        af.where    = TO_AFFECTS;
        af.type     = gsn_acidblood;
        af.level    = ch->pcdata->discipline[QUIETUS];
        af.location = APPLY_HITROLL;
        af.modifier = -(dicesuccess*3);
        af.duration = 1;
        af.bitvector    = AFF_BLIND;
        affect_to_char(victim, &af);

        act("With precision aiming, your projectile has splashed into $N's face, momentarily blinding $S.  You are truly the favored of Haqim.", ch, NULL, victim, TO_CHAR);
        if(!IS_NPC(victim))
            act("Your eyes burn as the acid splashes across your face.  Flailing wildly, you begin to hurriedly wipe it off.", ch, NULL, victim, TO_VICT);
        act("$N flails wildly and begins clawing at $S face.", ch, NULL, victim, TO_NOTVICT);
        gain_exp(ch, 2);
    }
    return;
}

void do_bloodcurse(CHAR_DATA *ch, char *argument)
{
   AFFECT_DATA af;
   CHAR_DATA *victim;
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_INPUT_LENGTH];
   int chance;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );
    chance = number_range(1, 100);
    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Who?\n\r", ch );
        return;
    }
    if(!IS_VAMP(victim))
    {
        send_to_char("They are not a vampire!\n\r" ,ch);
        return;
    }
    if(IS_NPC(victim)) return;
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[QUIETUS] < 3)
    {
        send_to_char( "You are not skilled enough in Quietus!.\n\r", ch );
        return;
    }

    if(ch->pblood < 30)
    {
            send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }
    ch->pblood -= 20;
    if(ch->gen <= victim->gen) chance += 10;
    else chance -= 10;
    if(get_curr_stat(ch,STAT_DEX) >= get_curr_stat(victim,STAT_DEX)) chance += 10;
    else chance -= 10;

    if(chance < 50)
    {
        sprintf(buf, "%s attempts to grasp you and fails!\n\r", ch->name);
        send_to_char(buf,victim);
        sprintf(buf, "You fail to grasp %s!\n\r", victim->name);
        send_to_char(buf,ch);
        multi_hit( victim, ch, TYPE_UNDEFINED );
        return;
    }

    WAIT_STATE( ch, 12 );

    af.where     = TO_AFFECTS2;
    af.type      = gsn_quietus_bloodcurse;
    af.level     = ch->level;
    af.duration  = 4;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF2_QUIETUS_BLOODCURSE;
    affect_to_char( victim, &af );

    sprintf(buf, "%s curses your blood!\n\r", ch->name);
    send_to_char(buf,victim);
    sprintf(buf, "You lay a curse on %s's blood!\n\r", victim->name);
    send_to_char(buf,ch);
    act("$N gasps as $n curses their blood!", ch,NULL,victim,TO_NOTVICT);
    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}


void do_silenceofdeath(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pblood < 15)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[QUIETUS] < 1)
    {
        send_to_char( "You are not skilled in the arts of Quietus.\n\r", ch );
        return;
    }

    if ( IS_AFFECTED(ch, AFF_SNEAK) )
    {
        affect_strip( ch, gsn_sneak );

/*      act( "$n starts emitting sounds again.", ch, 0, 0, TO_NOTVICT ); */
        send_to_char( "You drop your aura of silence.\n\r", ch );
        return;
    }
    ch->pblood -= 15;
/*  act( "$n suddenly stops emitting sounds.", ch, 0, 0, TO_NOTVICT ); */
    send_to_char( "You create a field of Silence around yourself.\n\r", ch );


    affect_strip( ch, gsn_sneak );



    if (IS_AFFECTED(ch,AFF_SNEAK))
        return;

    af.where     = TO_AFFECTS;
    af.type      = gsn_sneak;
    af.level     = ch->level;
    af.duration  = ch->level*ch->pcdata->discipline[QUIETUS];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SNEAK;
    affect_to_char( ch, &af );
    return;
}
void do_bloodagony(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA *af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pblood < 40)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[QUIETUS] < 4)
    {
        send_to_char( "You are not skilled in the arts of Quietus.\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, argument, ch ) ) == NULL )
    {
        send_to_char( "Coat what?\n\r", ch );
        return;
    }
    if (!CAN_WEAR( obj, ITEM_WIELD))
    {
        send_to_char( "That is not a weapon!\n\r", ch );
        return;
    }
    ch->pblood -= 30;

    af = new_affect();
    af->where     = TO_WEAPON;
    af->type      = gsn_agg_damage;
    af->level     = ch->level;
    af->duration  = ch->level*ch->pcdata->discipline[QUIETUS];
    af->location  = 0;
    af->modifier  = 0;
    af->bitvector = WEAPON_AGG_DAMAGE;
    obj->affected   = af;
    act("You coat $p in your acidic blood.",ch,obj,NULL,TO_CHAR);
    act("$n coats $p in acidic blood.",ch,obj,NULL,TO_NOTVICT);
    return;
}

void do_taste( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[THAUMATURGY] < 1)
    {
    send_to_char( "You are not trained in Thaumaturgy!\n\r", ch );
    return;
    }

    if (ch->pblood < 20)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Gain knowledge from whom?\n\r", ch );
        return;
    }

    ch->pblood -= 10;
    act( "You taste $N's blood and gain insight on them.", ch, argument, victim, TO_CHAR );
    act( "$n tastes $N's blood.", ch, argument, victim, TO_NOTVICT );
    act( "$n tastes your blood and gains information on you!", ch, argument, victim, TO_VICT );

    sprintf( buf, "%s is %d years old.\n\r", IS_NPC(victim) ? victim->short_descr : victim->name, get_age(victim));
    send_to_char( buf, ch );

    sprintf(buf, "%s is a %s %s.\n\r",
    IS_NPC(victim) ? victim->short_descr : victim->name,
    victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
    race_table[victim->race].name);
    send_to_char(buf,ch);

    sprintf( buf, "Their alignment is %d.\n\r", victim->alignment );
        send_to_char( buf, ch );
    if(!IS_NPC(victim) && IS_VAMP(victim))
    {
        sprintf(buf, "Their Generation is %d.\n\r", victim->gen);
        send_to_char(buf,ch);
        sprintf(buf, "Their Sire is %s.\n\r", victim->sire);
        send_to_char(buf,ch);
        sprintf(buf, "They have %d Blood Points.\n\r", victim->pblood);
        send_to_char(buf,ch);
    }
    return;
}
void do_bloodrage( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    int touchsuccess = 0;
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[THAUMATURGY] < 2)
    {
    send_to_char( "You are not trained enough in Thaumaturgy!\n\r", ch );
    return;
    }

    if (ch->pblood < 15)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Cause whom to rage?\n\r", ch );
        return;
    }
    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (!IS_VAMP(victim))
    {
        send_to_char("You can only force targets who have vampiric blood into a vitae induced Frenzy!\n\r", ch);
        return;
    }

    if(is_affected( victim, gsn_thaumaturgy_frenzy ))
    {
        send_to_char("They are already in Frenzy.\n\r",ch);
        return;
    }

    touchsuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 4);

    if (touchsuccess < 0)
    {
        act("You trip and fall as soon as you move towards $N.", ch, NULL, victim, TO_CHAR);
        act("$n takes a step and falls on $s face.", ch, NULL, victim, TO_VICT);
        act("$n falls flat on $s face.", ch, NULL, victim, TO_NOTVICT);
        damage(ch, ch, ch->level, gsn_trip, DAM_BASH, TRUE);
        ch->position = POS_RESTING;
        WAIT_STATE(ch, 14);
        return;
    }

    if (touchsuccess == 0)
    {
        act("You reach for $N, but your fingers brush against nothing but air.", ch, NULL, victim, TO_CHAR);
        act("$n reaches out towards you, misjudging the distance.", ch, NULL, victim, TO_VICT);
        act("$n reaches out for $N, but is too far away.", ch, NULL, victim, TO_NOTVICT);
        WAIT_STATE(ch, 12);
        return;
    }

    ch->pblood -= 10;
    WAIT_STATE(ch, 12);
    gain_exp(ch, touchsuccess);

    act( "$n reaches over and brushes $s hand across $N's chest.",  ch, NULL, victim, TO_NOTVICT );
    act( "$n reaches over, dragging $s hand lightly across your chest.",  ch, NULL, victim, TO_VICT );
    act( "You reach over to $N and lightly drag your hand across $S chest.",  ch, NULL, victim, TO_CHAR );

    dicesuccess = godice(ch->csmax_willpower, 5);

    if (dicesuccess < 0)
    {
        act("You recoil in horror as your own body flushes with the use of blood, inciting you into a Frenzy!", ch, NULL, victim, TO_CHAR);
        thaumaturgy_frenzy( gsn_thaumaturgy_frenzy, 8, ch, ch, TARGET_CHAR);
        return;
    }

    if (dicesuccess == 0)
    {
        act("Perplexed, you take a step back as nothing seems to happen.", ch, NULL, victim, TO_CHAR);
        WAIT_STATE(ch, 6);
        return;
    }

    thaumaturgy_frenzy( gsn_thaumaturgy_frenzy, dicesuccess, victim, victim, TARGET_CHAR);

    act("You smile with delight as $N spends precious vitae and enters a Frenzy.", ch, NULL, victim, TO_CHAR);
    if (!IS_NPC(victim))
        victim->pblood -= 10;
    if (victim->pblood < 0)
        victim->pblood = 0;
    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}
void thaumaturgy_frenzy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
  CHAR_DATA *tch;
  char buf[MAX_STRING_LENGTH];
  bool fdone = FALSE;


    if ( !is_affected( ch, gsn_thaumaturgy_frenzy ) )
    {
        act( "$n snarls as $e enter a frenzy!",  ch, NULL, NULL, TO_ROOM );
        send_to_char( "Your bloodlust overcomes you and you enter a Frenzy!\n\r", ch );
    }

    for (tch = ch->in_room->people; tch != NULL; tch = tch->next_in_room)
    {
        if (fdone) /* fdone = TRUE, break out of the for, else keep going */
            break;

        if (tch == ch || !IS_NPC(tch)) /* stop icky victim == ch in dmg code */
            continue;

        if (number_range(1, 100) <= 10 && tch != NULL)
        {
            fdone = TRUE;
            act( "$n throws $mself into a frenzy and attacks $N!",  ch, NULL, tch, TO_NOTVICT );
            sprintf(buf, "Frenzy overwelms you and you attack %s!\n\r", tch->name);
            send_to_char(buf,ch);
            multi_hit( ch, tch, TYPE_UNDEFINED );
        }
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_thaumaturgy_frenzy;
    af.level     = ch->level;
    af.duration  = level;
    af.location  = APPLY_HITROLL;
    af.modifier  = -level * 25;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location  = APPLY_DAMROLL;
    af.modifier  = -(level * 2) * 25;
    affect_to_char( ch, &af );

    af.location = APPLY_AC;
    af.modifier = (level * 3) * 25;
    affect_to_char( ch, &af );

    return;
}
void do_bloodofpotency(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

//  send_to_char("Disabled due to massive bugginess.\n\r", ch);
//  return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (ch->pblood < 10)
    {
        send_to_char( "You do not have enough blood.\n\r", ch );
        return;
    }
    if (ch->pcdata->discipline[THAUMATURGY] < 3)
    {
        send_to_char( "You are not skilled enough in the arts of Thaumaturgy.\n\r", ch );
        return;
    }

    if(ch->race == race_lookup("methuselah"))
    {
        send_to_char("You fear that if you step any closer to Caine you may lose the ancient battle for sanity.\n\r",ch);
        return;
    }
    if(ch->race == race_lookup("dhampire"))
    {
        send_to_char("You are not kindred, so you cannot reduce your generation. Your more powerful then these petty undead anyways.\n\r",ch);
        return;
    }
    if(is_affected(ch,gsn_bloodofpotency))
    {
        send_to_char("You have already tried condensing the power of you blood as much as you are able to right now.\n\r",ch);
        return;
    }

    if (ch->position > POS_SITTING)
    {
        send_to_char("You cannot focus enough while standing up.\n\r", ch);
        return;
    }

    dicesuccess = godice(ch->csmax_willpower, 6);
    ch->pblood -= 10;

    WAIT_STATE(ch, 16);

    if (dicesuccess < 0)
    {
        act("Your blood begins pounding in anticipation, but your heart suddenly stops as your entire body feels weak and helpless.", ch, NULL, NULL, TO_CHAR);
        act("$n looks a bit confused, and even quite fatigued and weak.", ch, NULL, NULL, TO_ROOM);

        af.where    = TO_AFFECTS;
        af.type     = gsn_bloodofpotency;
        af.level    = -1;
        af.duration = -(dicesuccess * 5) + 10;
        af.location = APPLY_GENERATION;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_bloodofpotency;
        af.level    = -1;
        af.duration = -(dicesuccess * 5) + 10;
        af.location = APPLY_MAX_BLOOD;
        af.modifier = -20;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        WAIT_STATE(ch, 6);

        return;
    }

    if(dicesuccess == 0)
    {
        act("You expend precious blood in hopes to concentrate your vitae, but you end up feeling exactly the same.", ch, NULL, NULL, TO_CHAR);

        af.where    = TO_AFFECTS;
        af.type     = gsn_bloodofpotency;
        af.level    = 0;
        af.duration = 5;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    act("Your entire body exults in the newfound potency of your blood, filling you with a sense of superiority.", ch, NULL, NULL, TO_CHAR);

    if (dicesuccess == 1)
        dicesuccess++;

    int genmod;
    genmod = -(dicesuccess -1);
    if ((ch->gen + genmod) < 4)
        genmod = -(ch->gen - 4);

    af.modifier = genmod;
    af.duration = (10*dicesuccess);
    af.where     = TO_AFFECTS;
    af.type      = gsn_bloodofpotency;
    af.level     = ch->pcdata->discipline[THAUMATURGY];
    af.location  = APPLY_GENERATION;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_bloodofpotency;
    af.level    = ch->pcdata->discipline[THAUMATURGY];
    af.duration = (10*dicesuccess);
    af.location = APPLY_MAX_BLOOD;
    af.modifier = 10*(-genmod);
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    gain_exp(ch, dicesuccess * 5);
    return;
}

void do_theft(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
/*   int chance=0;*/
    int dicesuccess;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Steal the lifeblood from whom?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }

    if(ch->race == race_lookup("dhampire"))
    {
        if (ch->cswillpower == 0)
        {
            send_to_char("You lack the {WWillpower{x to overcome your base human nature and feed on blood.\n\r",ch);
            return;
        }

        ch->cswillpower--;
    }

    if(ch->race == race_lookup("ghoul"))
    {
        send_to_char("You must only feed from your master!\n\r",ch);
        return;
    }

    if (ch->pcdata->discipline[THAUMATURGY] < 4)
    {
        send_to_char( "You are not skilled enough in the arts of Thaumaturgy!\n\r", ch );
        return;
    }

    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (ch->pblood <= 20)
    {
        send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }



    if (victim->position == POS_TORPOR)
    {
        send_to_char("Your target is already in {RT{ro{Drp{ro{Rr{x, finish the job!\n\r", ch);
        return;
    }

    if (victim->race == race_lookup("centipede") || victim->race == race_lookup("construct") || victim->race == race_lookup("wraith") || victim->race == race_lookup("undead") || victim->race == race_lookup("spirit") || victim->race == race_lookup("bane") || victim->race == race_lookup("spider") )
    {
        send_to_char("Your target has no blood to drain!\n\r", ch);
        return;
    }

    WAIT_STATE( ch, 18 );

    dicesuccess = godice(ch->csmax_willpower, 7);

    ch->pblood -= 10;

    if (dicesuccess < 0)
    {
        act("Your body suffers the backlash of blood magic gone awry.", ch, NULL, victim, TO_CHAR);
        act("$n jerks $s head to the side as $e begins bleeding profusely from $s nose.", ch, NULL, victim, TO_ROOM);
        ch->pblood -= 10;
        WAIT_STATE( ch, 4 );
        return;
    }

    if (dicesuccess == 0)
    {
        act("You attempt to steal blood from $N, but nothing happens.", ch, NULL, victim, TO_CHAR);
        WAIT_STATE( ch, 6 );
        return;
    }

    ch->pblood += dicesuccess * 15;
    if (ch->pblood > ch->max_pblood)
        ch->pblood = ch->max_pblood;
    if (!IS_NPC(victim) && IS_VAMP(victim))
        victim->pblood -= dicesuccess * 10;

    act("You smile wickedly as the blood you steal from $N mystically flies from $M,", ch, NULL, victim, TO_CHAR);
    act("and absorbs upon contact with your skin.", ch, NULL, victim, TO_CHAR);
    act("A stream of blood arcs through the air, completing a circuit between $N to $n.", ch, NULL, victim, TO_NOTVICT);
    act("You feel weak as blood mystically leaves your body, streaming towards $n.", ch, NULL, victim, TO_VICT);

    damage(ch, victim, (get_modifier(ch, MODTYPE_WEAKMELEE) * dicesuccess), gsn_theft, DAM_NONE, TRUE);
    gain_exp(ch, dicesuccess);

    return;
/*
    if(victim->level > ch->level)
    chance += 10;
    if(!IS_NPC(victim) && victim->gen < ch->gen)
    chance += 10;

    if((number_range(1,100)+chance) < 75)
    {
        act( "You steal some of $N's sweet vitae!",  ch, NULL, victim, TO_CHAR );
        act( "A stream of blood flies from $N to $n!",  ch, NULL, victim, TO_NOTVICT );
        act( "You feel weakened as $n steals some of your blood!",  ch, NULL, victim, TO_VICT );
        if(!IS_NPC(victim) && IS_VAMP(victim))
        {
            multi_hit( victim, ch, TYPE_UNDEFINED );
            victim->pblood -= victim->level/4;
        }
        else
            damage(ch,victim, (ch->level*ch->pcdata->discipline[THAUMATURGY]), gsn_theft, DAM_FIRE, TRUE);
        ch->pblood += victim->level/5;
    }
    else
    {
        act( "You try to steal some of $N's vitae but fail loosing your own in the process!",  ch, NULL, victim, TO_CHAR );
        act( "Blood spurts from $n as they fail to steal vitae from $N!",  ch, NULL, victim, TO_NOTVICT );
        act( "Blood spurts from $n as they fail to steal your vitae!",  ch, NULL, victim, TO_VICT );
        ch->pblood -= 20;
    }
    if(ch->pblood > ch->max_pblood) ch->pblood = ch->max_pblood;
    return;

*/
}
void do_cauldron(CHAR_DATA *ch, char *argument)
{
   CHAR_DATA *victim;
    int dicesuccess = 0;
    int touchsuccess = 0;
    int dam = 0;
    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a vampire!\n\r" ,ch);
        return;
    }
    if ( ( victim = get_char_room( ch, argument ) ) == NULL )
    {
        send_to_char( "Boil whom?\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if(victim == ch)
    {
        send_to_char( "You cannot do this to yourself!\n\r", ch );
        return;
    }

    if (ch->pcdata->discipline[THAUMATURGY] < 5)
    {
        send_to_char( "You are not skilled enough in the arts of Thaumaturgy!.\n\r", ch );
        return;
    }

    if (ch->pblood <= 25)
    {
        send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }



    if (victim->race == race_lookup("spirit") || victim->race == race_lookup("bane") || victim->race == race_lookup("wraith") || victim->race == race_lookup("construct"))
    {
        send_to_char("Your opponent does not have blood to boil!\n\r", ch);
        return;
    }

    WAIT_STATE(ch, 12);
    touchsuccess = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_BRAWL], 4);

    if (touchsuccess < 0)
    {
        act("You reach towards $N, but manage to stumble and fall.", ch, NULL, victim, TO_CHAR);
        act("$n reaches out to you, but stumbles and falls.", ch, NULL, victim, TO_VICT);
        act("$n takes a step towards $N, reaching out with $s hand, but stumbles and falls.", ch, NULL, victim, TO_NOTVICT);
        ch->position = POS_RESTING;
        return;
    }

    if (touchsuccess == 0)
    {
        act("You reach out towards $N, but miss making contact.", ch, NULL, victim, TO_CHAR);
        act("$n reaches out towards $N, who simply moves out of the way.", ch, NULL, victim, TO_NOTVICT);
        act("$n takes a step towards you, as you simply move to the side.", ch, NULL, victim, TO_VICT);
        return;
    }

    act("You reach forward and touch $N, ", ch, NULL, victim, TO_CHAR);
    act("$n reaches out and brushes $s hand against you, ", ch, NULL, victim, TO_VICT);
    act("$n reaches out and touches $N.  ", ch, NULL, victim, TO_NOTVICT);

    gain_exp(ch, touchsuccess);

    ch->pblood -= 20;
    dicesuccess = godice(ch->csmax_willpower, 7);

    if (dicesuccess < 0)
    {
        act("but you back away immediately as your blood begins boiling!", ch, NULL, victim, TO_CHAR);
        act("but backs away with a shove and a yelp.", ch, NULL, victim, TO_VICT);
        act("$e backs away quickly with a pained yelp.", ch, NULL, victim, TO_NOTVICT);
        ch->pblood -= 10 * dicesuccess;
        if (ch->pblood < 0)
            ch->pblood = 0;
        damage(ch, ch, ch->level*2, gsn_cauldron, DAM_FIRE, TRUE);
        return;
    }

    if (dicesuccess == 0)
    {
        act("$S blood refusing to boil.", ch, NULL, victim, TO_CHAR);
        act("but you shrug it off.", ch, NULL, victim, TO_VICT);
        return;
    }

    dam = number_range(525, 575);
    dam += ((2*dicesuccess) * ch->level * (15-ch->gen));

    act("willing the blood in $S body to react to your whim.\n\r$E screams in agony as $S blood boils.", ch, NULL, victim, TO_CHAR);
    act("and you scream in agony as your body feels as if it is burning from the inside out!", ch, NULL, victim, TO_VICT);
    act("$E lets out a blood curdling scream of anguish.", ch, NULL, victim, TO_NOTVICT);
    damage(ch, victim,dam, gsn_cauldron, DAM_FIRE, TRUE);

    if (!IS_NPC(victim))
    {
        victim->pblood -= dicesuccess * 5;
        if (victim->pblood < 0)

            victim->pblood = 0;
    }

    gain_exp(ch, dicesuccess * 2);

    if (dicesuccess > 3 && (victim->level < ch->level + 10) && IS_NPC(victim) && IS_VALID(victim))
    {
        if (!IS_VAMP(victim) && victim->race != race_lookup("garou") && victim->race != race_lookup("fera") && victim->race != race_lookup("undead"))
        {
            act("$N lets out a final shriek, and then slumps to the ground in a lifeless heap.", ch, NULL, victim, TO_NOTVICT);
            act("$N lets out a final shriek, and then slumps to the ground in a lifeless heap.", ch, NULL, victim, TO_CHAR);
            kill_em(ch, victim);
        }
    }

/*  if((number_range(1,100)) > 25)
    {
        act( "You touch $N and grin as they scream in agony!",  ch, NULL, victim, TO_CHAR );
        act( "$n touches $N and $N screams in agony as their blood boils!",  ch, NULL, victim, TO_NOTVICT );
        act( "You scream in agony as $n boils your blood!",  ch, NULL, victim, TO_VICT );
        if(!IS_NPC(victim) && IS_VAMP(victim))
        {
            victim->pblood -= 50;
            if(victim->pblood < 0) victim->pblood = 0;
        }

        damage(ch,victim, 8500, gsn_cauldron, DAM_FIRE, TRUE);
        ch->pblood -= 60;
    }
    else
    {
        act( "You are unable to touch $N!",  ch, NULL, victim, TO_CHAR );
        act( "$n leaps out to touch $N but misses!",  ch, NULL, victim, TO_NOTVICT );
        act( "$n leaps out to touch you but misses!",  ch, NULL, victim, TO_VICT );
        damage(ch,victim, 1, gsn_cauldron, DAM_NEGATIVE, FALSE);
        ch->pblood -= 20;
    }
*/  return;
}


void do_changeling(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(ch->pcdata->discipline[VICISSITUDE] < 1)
    {
        send_to_char( "You are not skilled enough in Vicissitude to alter your form.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0')
    {
        if (is_affected( ch, gsn_mask ))
        {
            send_to_char( "You return your features to your original form.\n\r", ch );
            affect_strip(ch,gsn_mask);
        }
        else
            send_to_char("Assume the form of whom?\n\r",ch);
        return;
    }
    if ((victim = get_char_room( ch, argument ))== NULL)
    {
        send_to_char( "Assume the form of whom?\n\r", ch );
        return;
    }

    if(!IS_NPC(victim))
    {
    ch->pblood -= 25;
    act( "Your features alter and shift until you assume the form of $N", ch, NULL, victim, TO_CHAR );
    act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
    sprintf(buf, "%s",victim->name,ch->name);
        ch->short_descr = str_dup( buf );
    sprintf(buf, "%s",victim->name,ch->pcdata->title);
            ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_mask;
    af.level     = ch->level + ch->pcdata->discipline[VICISSITUDE];
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
return;
    }
    if(ch->pblood-25 < 10)
    {
        send_to_char("You do not have enough Vitae to inact this change.\n\r",ch);
        return;
    }

    if (is_affected( ch, gsn_vicissitude_horrid ) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("Your form is too inhuman to alter in this way.\n\r",ch);
        return;
    }
    ch->pblood -= 25;
    act( "Your features alter and shift until you assume the form of $N", ch, NULL, victim, TO_CHAR );
    act( "$n's features alter and shift until they assume the form of $N.", ch, NULL, victim, TO_ROOM );
    sprintf(buf, "%s",victim->short_descr,ch->name);
        ch->short_descr = str_dup( buf );
    sprintf(buf, "%s",victim->short_descr,ch->pcdata->title);
            ch->shift = str_dup( buf );

    af.where     = TO_AFFECTS;
    af.type      = gsn_mask;
    af.level     = ch->level + ch->pcdata->discipline[VICISSITUDE];
    af.duration  = -1;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
return;
}

void do_fleshcraft(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *mob;
    AFFECT_DATA af;


    argument = one_argument( argument, arg );

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if(ch->pcdata->discipline[VICISSITUDE] < 2)
    {
        send_to_char( "You are not skilled enough in Vicissitude to perform fleshcraft.\n\r", ch );
        return;
    }



    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if(!str_prefix(arg,"sex"))
    {
        if(ch->pcdata->discipline[VICISSITUDE] < 5)
        {
            send_to_char("You are not skilled enough in fleshcrafting to alter yourself to that extent.\n\r",ch);
            return;
        }
        if(ch->pblood-50 < 10)
        {
            send_to_char("You do not have enough blood to alter your sex.\n\r",ch);
            return;
        }
        WAIT_STATE( ch, 100 );
        if(!str_prefix(argument,"male"))
        {
            send_to_char("You alter your sex to be male.\n\r",ch);
            act( "$n slowly alters its sex to male.", ch, NULL, NULL, TO_NOTVICT );
            ch->sex = 1;
            ch->pcdata->true_sex = 1;
            ch->pblood -= 50;
            return;
        }
        if(!str_prefix(argument,"female"))
        {
            send_to_char("You alter your sex to be female.\n\r",ch);
            act( "$n slowly alters its sex to female.", ch, NULL, NULL, TO_NOTVICT );
            ch->sex = 2;
            ch->pcdata->true_sex = 2;
            ch->pblood -= 50;
            return;
        }
        if(!str_prefix(argument,"it"))
        {
            send_to_char("You alter your sex to be an it.\n\r",ch);
            act( "$n slowly alters its sex to it.", ch, NULL, NULL, TO_NOTVICT );
            ch->sex = 0;
            ch->pcdata->true_sex = 0;
            ch->pblood -= 50;
            return;
        }
        send_to_char("That is not a proper sex! Choose male, female or it.\n\r",ch);
        return;
    }
    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two creatures at once!\n\r",ch );
        return;
    }
    if((mob = get_char_room( ch, argument )) == NULL)
    {
        send_to_char( "Fleshcraft whom?.\n\r", ch );
        return;
    }
    if(!IS_NPC(mob))
    {
        send_to_char("You can't alter that!\n\r",ch);
        return;
    }

    if (is_safe (ch, mob))
        return; //is_safe has its own mesasges. easiest that way!

    if (mob->level > 25+ch->level)
    {
        send_to_char( "That creature is too strong for your fleshcraft.\n\r", ch );
        return;
    }
    if ( !str_prefix(arg,"hellhound"))
    {
        if(mob->race != race_lookup("wolf") &&
                mob->race != race_lookup("fido") &&
                mob->race != race_lookup("dog"))
        {
            send_to_char("This must be preformed upon a Canine.\n\r",ch);
            return;
        }
        if(ch->pblood-50 < 10)
        {
            send_to_char("You do not have enough blood to alter this person's form.\n\r",ch);
            return;
        }
        ch->pblood -= 50;

        act( "$n reaches out and gently massages $N's form into a twisted, disgusting shadow of it's former self.", ch, NULL, mob, TO_NOTVICT );

        mob->name = str_dup("hellhound warghoul ghoul");
        sprintf(buf,"A poor and twisted creature that slighty resembles %s cringes before its master.\n\r",mob->short_descr);
        mob->long_descr = str_dup(buf);
        mob->short_descr = str_dup("A Hellhound");
        mob->level = ch->level;
        mob->max_hit = UMIN(mob->max_hit+1000, 2000 * ch->pcdata->discipline[VICISSITUDE]);
        mob->hit = mob->max_hit;
        mob->hitroll += 100;
        mob->damroll += 25;
        mob->armor[0] -= 100;
        mob->armor[1] -= 100;
        mob->armor[2] -= 100;
        mob->armor[3] -= 100;

        add_follower( mob, ch );
        mob->leader = ch;
        ch->pet = mob;

        af.where     = TO_AFFECTS;
        af.type      = gsn_charm_person;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char(mob,&af);
        WAIT_STATE( ch, 300 );
        return;
    }

    if ( !str_prefix( arg, "szlachta" ) )
    {
        if(ch->pcdata->discipline[VICISSITUDE] < 3)
        {
            send_to_char( "You are not skilled enough in Vicissitude to perform this fleshcrafting.\n\r", ch );
            return;
        }
        if (mob->race != race_lookup("human") && mob->race != race_lookup("ghoul"))
        {
            send_to_char( "You can only fleshcraft a human into a warghoul!\n\r", ch );
            return;
        }

        if(ch->pblood-60 < 10)
        {
            send_to_char("You do not have enough blood to alter this person's form.\n\r",ch);
            return;
        }
        ch->pblood -= 60;

        act( "$n lulls $N into a trance and slowly starts to reshape the poor fellow into a vicious and horrid version of itself.", ch, NULL, mob, TO_NOTVICT );

        mob->name = str_dup("szlachta warghoul ghoul");
        sprintf(buf,"A vicious creature that slighty resembles %s snarls in rage at its current form.\n\r",mob->short_descr);
        mob->long_descr = str_dup(buf);
        mob->short_descr = str_dup("A Szlachta Warghoul");
        mob->max_hit = UMIN(mob->max_hit+5000, 3000 * ch->pcdata->discipline[VICISSITUDE]);
        mob->hit = mob->max_hit;
        mob->hitroll += 100;
        mob->damroll += 100;
        mob->armor[0] -= 500;
        mob->armor[1] -= 500;
        mob->armor[2] -= 500;
        mob->armor[3] -= 500;

        add_follower( mob, ch );
        mob->leader = ch;
        ch->pet = mob;

        af.where     = TO_AFFECTS;
        af.type      = gsn_charm_person;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char(mob,&af);

        WAIT_STATE( ch, 300 );
        return;
    }

    if ( !str_prefix( arg, "vozhd" ) )
    {
        if(ch->pcdata->discipline[VICISSITUDE] < 4)
        {
            send_to_char( "You are not skilled enough in Vicissitude to perform this fleshcrafting.\n\r", ch );
            return;
        }

        if (is_affected(mob, gsn_vicissitude_bonecraft))
        {
            sendch("This beast cannot be altered again in such a way!\n\r", ch);
            return;
            }

        if(ch->pblood-80 < 10)
        {
            send_to_char("You do not have enough blood to alter this person's form.\n\r",ch);
            return;
        }
        ch->pblood -= 80;

        act( "$n grasps $N and wrenches their limbs and body, calling forth bone armor from within and twists the poor creature into a warped monster of destruction.", ch, NULL, mob, TO_NOTVICT );

        mob->name = str_dup("vozhd warghoul ghoul");
        sprintf(buf,"A vicious bone-armored and spiked version of %s snarls and barely holds back it's urge to kill.\n\r",mob->short_descr);
        mob->long_descr = str_dup(buf);
        mob->short_descr = str_dup("A Vozhd Warghoul");
        mob->max_hit = UMIN(mob->max_hit+5000, 20000);
        mob->hit = mob->max_hit;
        mob->hitroll += 400;
        mob->damroll += 400;
        mob->armor[0] -= 1000;
        mob->armor[1] -= 1000;
        mob->armor[2] -= 1000;
        mob->armor[3] -= 1000;

        add_follower( mob, ch );
        mob->leader = ch;
        ch->pet = mob;

        af.where     = TO_AFFECTS;
        af.type      = gsn_charm_person;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = AFF_CHARM;
        affect_to_char(mob,&af);

        af.where     = TO_AFFECTS;
        af.type      = gsn_vicissitude_bonecraft;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = 0;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char(mob,&af);
        WAIT_STATE( ch, 300 );
        return;
    }

    send_to_char( "You may only fleshcraft someone into a ghoul.\n\r", ch );
    return;
}

void do_bonecraft(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }


    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }
    if (is_affected( ch, gsn_vicissitude_bonecraft ))
    {
        affect_strip(ch,gsn_vicissitude_bonecraft);
        act( "You let your skeletal structure return to its 'normal' state.", ch, NULL, NULL, TO_CHAR );
        act( "$n's spikes and plates slowly sink back into their body.", ch, NULL, NULL,TO_NOTVICT );
        return;
    }
    if(ch->pcdata->discipline[VICISSITUDE] < 3)
    {
        send_to_char( "You are not skilled enough in Vicissitude to alter your bones in such a way.\n\r", ch );
        return;
    }
    if(ch->pblood-30 < 10)
    {
        send_to_char("You don't have enough blood to force this alteration.\n\r",ch);
        return;
    }
    ch->pblood -= 30;

    damage( ch, ch, ch->level*3, gsn_vicissitude_bonecraft, DAM_PIERCE, TRUE);

        af.where     = TO_AFFECTS;
        af.type      = gsn_vicissitude_bonecraft;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = APPLY_AC;
        af.modifier  = -3*ch->level/2;
        af.bitvector = 0;
        affect_to_char(ch,&af);
        WAIT_STATE( ch, 60 );
    act( "You let forth an inhuman roar as you pull and push your bone structure, puncturing through your flesh and skin, providing harsh spikes jutting out from your body.", ch, NULL, NULL, TO_CHAR );
    act( "$n lets forth an inhuman roar as $e pulls bony spikes through $s body.", ch, NULL, NULL,TO_NOTVICT );
    return;
}

void do_horrid(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA af;

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch, AFF_SHIFT)) {
        sendch("You have already altered your form!\n\r", ch);
        return;
        }


    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if(ch->pcdata->discipline[VICISSITUDE] < 4)
    {
        send_to_char( "You are not skilled enough in Vicissitude to assume the Horrid Form.\n\r", ch );
        return;
    }

    if (is_affected( ch, gsn_vicissitude_horrid ))
    {
        send_to_char( "Your form shrinks and you slowly return to your 'normal' form.\n\r", ch );
        affect_strip(ch,gsn_vicissitude_horrid);
        if (ch->hit > ch->max_hit)
            ch->hit = ch->max_hit;
        act( "$n's body slowly shrinks and alters back into their 'normal' form.", ch, NULL, NULL, TO_NOTVICT );
        ch->dam_type = 17;
        if ( !IS_AFFECTED(ch, AFF_FANGS))
            do_function(ch, &do_fangs, "" );
        return;
    }
    if (is_affected( ch, gsn_mask ))
    {
        if ( !IS_AFFECTED(ch, AFF_FANGS))
        SET_BIT(ch->affected_by, AFF_FANGS);
            do_function(ch, &do_mask, "" );
    }
    if (ch->pblood-70 < 10)
    {
        send_to_char( "You don't have enough blood!\n\r", ch );
        return;
    }
    ch->pblood -= 70;

    act( "Your body slowly shifts into that of a horrible Zulo.", ch, NULL, NULL, TO_CHAR );
    act( "$n's size doubles as their body is altered into that of a horrible Zulo.", ch, NULL, NULL,TO_NOTVICT );


    af.where     = TO_AFFECTS;
    af.type      = gsn_vicissitude_horrid;
    af.level     = ch->level;
    af.duration  = -1;
    af.location  = APPLY_HIT;
    af.modifier  = 8 * UMAX(20, ch->level);
    af.bitvector = 0;
    affect_to_char( ch, &af );


    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_STR;
    af.modifier = 4;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_DEX;
    af.modifier = 4;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_STA;
    af.modifier = 3;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_CHA;
    af.modifier = -(get_attribute(ch, CHARISMA));
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_MAN;
    af.modifier = -(get_attribute(ch, MANIPULATION));
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.where    = TO_AFFECTS;
    af.type     = gsn_vicissitude_horrid;
    af.level    = ch->level;
    af.duration = -1;
    af.location = APPLY_CS_APP;
    af.modifier = -(get_attribute(ch, APPEARANCE));
    af.bitvector    = 0;
    affect_to_char( ch, &af );


    if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
    sprintf(buf, "A grotesque monstrosity",ch->name);
    ch->short_descr = str_dup( buf );
    sprintf(buf, "A huge, grotesque, horned monstrosity towers over you with an evil sneer on it's... face?");
    ch->shift = str_dup( buf );
    WAIT_STATE( ch, 80 );
    return;
}

void do_essence(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if(IS_NPC(ch)) return;

    if(!IS_VAMP(ch))
    {
        send_to_char("You are not a Vampire!\n\r",ch);
        return;
    }



    if ( IS_AFFECTED2(ch, AFF2_QUIETUS_BLOODCURSE))
    {
        send_to_char("Your blood curse prevents it!\n\r" ,ch);
        return;
    }

    if(ch->pcdata->discipline[VICISSITUDE] < 5)
    {
        send_to_char( "You are not skilled enough in Vicissitude to perform Inner Essence.\n\r", ch );
        return;
    }

    if(ch->hit < ch->max_hit/4)
    {
        send_to_char("Your body is too weak to perform Inner Essence!\n\r",ch);
        return;
    }
    if (number_range(0,100) < 10)
    {
        send_to_char("You try to invoke your Inner Essence, but you fail and scream in agony as your body is disrupted.\n\r", ch);
        ch->hit -= ch->max_hit/4;
        if(ch->pblood-10 < 0) ch->pblood = 1; else ch->pblood -= 10;
    }
    else
    {
        send_to_char("You invoke your Inner essence and transform parts of your body into sweet vitae!\n\r.", ch);
        ch->hit -= ch->max_hit/4;
        ch->pblood += ch->max_pblood/6;
    }
    WAIT_STATE( ch, 60 );
    return;
}



/*******************************************************************************
 *                                                                             *
 *                        Beginning of Combo Disciplines.                      *
 *                                                                             *
 *******************************************************************************/



void do_mortalterrors( CHAR_DATA *ch, char *argument)
{

    int door;
    EXIT_DATA       *pexit;
    ROOM_INDEX_DATA *original_room;
    ROOM_INDEX_DATA *pRoom;
    CHAR_DATA *victim;
    CHAR_DATA *victim_next;
    char arg1[MIL];
    bool hit = FALSE;
    int dicepool, success, diff;

    argument = one_argument( argument, arg1 );

    if (IS_NPC(ch))
        return;

    if (!IS_VAMP(ch))
    {
        sendch("You are not a Vampire!\n\r", ch);
        return;
    }
    if (ch->pcdata->discipline[PRESENCE] < 2 ||
        ch->pcdata->discipline[OBTENEBRATION] < 3)
    {
        sendch("You do not know that ability!\n\r", ch);
        return;
    }
    if (ch->gen > 9)
    {
        sendch("Your blood is not potent enough to perform that ability.\n\r", ch);
        return;
    }
    if (ch->pblood < 30)
    {
        sendch("You do not have enough blood to perform that!", ch);
        return;
    }

    if (IS_NULLSTR(arg1))
    {
        sendch("Send shades to terrorize which direction?\n\r", ch);
        return;
    }

       if ( !str_prefix( arg1, "n" ) || !str_prefix( arg1, "north" ) ) door = 0;
    else if ( !str_prefix( arg1, "e" ) || !str_prefix( arg1, "east"  ) ) door = 1;
    else if ( !str_prefix( arg1, "s" ) || !str_prefix( arg1, "south" ) ) door = 2;
    else if ( !str_prefix( arg1, "w" ) || !str_prefix( arg1, "west"  ) ) door = 3;
    else if ( !str_prefix( arg1, "u" ) || !str_prefix( arg1, "up"    ) ) door = 4;
    else if ( !str_prefix( arg1, "d" ) || !str_prefix( arg1, "down"  ) ) door = 5;
    else
    {
    send_to_char( "In which direction do you wish to send your shades?\n\r", ch );
    return;
    }
    // Is it a legal exit and the door open?
    if ( (pexit = ch->in_room->exit[door] ) == NULL || IS_SET(pexit->exit_info, EX_CLOSED))
    {
    send_to_char( "You cannot send shades in that direction.\n\r", ch );
    return;
    }
    original_room = ch->in_room;
    pRoom = ch->in_room->exit[door]->u1.to_room;
    //Does the exit lead anywhere?
    if (!pRoom)
    {
        sendch("Something went wrong... Contact an Immortal.\n\r", ch);
        return;
    }
    if (pRoom->people == NULL)
    {
        sendch("There's nobody there!\n\r", ch);
        return;
    }
    ch->pblood -= 20;
    act("$n summons forth terrible nightshades and sends them off to strike fear in the hearts of mortals.", ch, NULL, NULL, TO_NOTVICT);

    // We have a legal room, with mobs in it. Now check for terrors.
              for (victim = pRoom->people; victim != NULL; victim = victim_next)
              {
                  victim_next = victim->next_in_room;

                   if ( // Some limiters.. May change later.
                           !can_see(ch, victim)
                           || !IS_NPC(victim)
                           ||   victim->in_room == NULL || victim == ch
                           ||   (IS_NPC(victim) && IS_SET(victim->act,ACT_AGGRESSIVE))
                           ||   victim->fighting != NULL
                           ||   (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
                           ||   victim->level > ch->level+20
                           ||   is_safe_spell(ch, victim, TRUE)
                     )
                      continue;

                  //Have a mob we can influence. Roll for terror!
                  dicepool = get_attribute(ch, MANIPULATION) + get_ability(ch, CSABIL_INTIMIDATION);
                  diff = victim->level / 12;
                  if (diff > 10) diff = 10;
                  if (diff < 4)  diff = 4;
                  success = godice(dicepool, diff);
                  if (success < 0)
                  {
                      sendch("Your shades recoil and disappear back to the Abyss.\n\r", ch);
                      break;
                  } else if (success == 0)
                      continue;

                  char_from_room( victim );
                  char_to_room( victim, ch->in_room );
                  hit = TRUE;
                  act( "$n runs in from nearby, screaming in terror!", victim, NULL, NULL, TO_NOTVICT );
                  }
    if (!hit)
    {
        sendch("You fail to terrorize anyone.\n\r", ch);
        return;
    }
    return;
    };
