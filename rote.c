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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

bool paradox_check(CHAR_DATA *ch, bool vulgar)
{
    AFFECT_DATA af;
    int dox = 1;
    if(vulgar) dox += 4;
    int ward = get_affect_modifier(ch, gsn_paradoxward);

    if (is_affected(ch, gsn_paradoxward))
    {

      af.where     = TO_AFFECTS;
      af.type      = gsn_paradoxward;
      af.level     = 0;
      af.duration  = 0;
      af.location  = APPLY_NONE;
      af.modifier  = -dox;
      af.bitvector = 0;
      affect_join(ch, &af);

      send_to_char("Your masterful ward nullifies some of the Paradox that was targeting your Pattern.\n\r", ch);

      if (( get_affect_modifier(ch, gsn_paradoxward)) <= 0)
      {
        affect_strip(ch, gsn_paradoxward);
        send_to_char("Your ward is depleted, Paradox can accumulate again.\n\r", ch);
      }
    }

      dox -= ward;

    if (dox > 0)
      ch->paradox += dox;
    else
      return;

    if(number_range(1,(50 - ch->paradox)) <= 1)
    {
            damage( ch, ch, vulgar ? ch->paradox*100 : ch->paradox*50, gsn_brew, DAM_NEGATIVE, TRUE);
            ch->paradox = 0;
            return TRUE;
    }
    return FALSE;
}

void do_rotelist(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH], color[3];
    int rote, sph = 0, multi = 0;
    sprintf(color,"{x");

    if(IS_NPC(ch)) return;

    if(ch->avatar == 0)
    {
        send_to_char("You are not one of the Enlightened!\n\r",ch);
        return;
    }

    send_to_char("=============================================================\n\r",ch);
    send_to_char("|       {BCorrespondence  {DEntropy  {RForces  {GLife  {yMatter{x       |\n\r",ch);
    send_to_char("|          {CMind  {YPrime  {MSpirit  {cTime  {WCombination{x           |\n\r",ch);
    send_to_char("=============================================================\n\r",ch);
    send_to_char(" Sphere Level       Rote\n\r",ch);

    for(rote = 1; rote < MAX_ROTE; rote++)
    {
        if(
            rote_table[rote].correspondence <= ch->sphere[SPHERE_CORRESPONDENCE] &&
            rote_table[rote].entropy <= ch->sphere[SPHERE_ENTROPY] &&
            rote_table[rote].forces <= ch->sphere[SPHERE_FORCES] &&
            rote_table[rote].life <= ch->sphere[SPHERE_LIFE] &&
            rote_table[rote].matter <= ch->sphere[SPHERE_MATTER] &&
            rote_table[rote].mind <= ch->sphere[SPHERE_MIND] &&
            rote_table[rote].prime <= ch->sphere[SPHERE_PRIME] &&
            rote_table[rote].spirit <= ch->sphere[SPHERE_SPIRIT] &&
            rote_table[rote].time <= ch->sphere[SPHERE_TIME]
        )
        {
            if (rote_table[rote].correspondence > 0)
                multi++;
            if (rote_table[rote].correspondence > sph)
            {
                sph = rote_table[rote].correspondence;
                sprintf(color,"{B");
            }
            if (rote_table[rote].entropy > 0)
                multi++;
            if (rote_table[rote].entropy > sph)
            {
                sph = rote_table[rote].entropy;
                sprintf(color,"{D");
            }
            if (rote_table[rote].forces > 0)
                multi++;
            if (rote_table[rote].forces > sph)
            {
                sph = rote_table[rote].forces;
                sprintf(color,"{R");
            }
            if (rote_table[rote].life > 0)
                multi++;
            if (rote_table[rote].life > sph)
            {
                sph = rote_table[rote].life;
                sprintf(color,"{G");
            }
            if (rote_table[rote].matter > 0)
                multi++;
            if (rote_table[rote].matter > sph)
            {
                sph = rote_table[rote].matter;
                sprintf(color,"{y");
            }
            if (rote_table[rote].mind > 0)
                multi++;
            if (rote_table[rote].mind > sph)
            {
                sph = rote_table[rote].mind;
                sprintf(color,"{C");
            }
            if (rote_table[rote].prime > 0 && rote_table[rote].prime != 2)
                multi++;
            if (rote_table[rote].prime > sph)
            {
                sph = rote_table[rote].prime;
                sprintf(color,"{Y");
            }
            if (rote_table[rote].spirit > 0)
                multi++;
            if (rote_table[rote].spirit > sph)
            {
                sph = rote_table[rote].spirit;
                sprintf(color,"{M");
            }
            if (rote_table[rote].time > 0)
                multi++;
            if (rote_table[rote].time > sph)
            {
                sph = rote_table[rote].time;
                sprintf(color,"{c");
            }
            if (multi > 1)
                sprintf(color,"{W");
            sprintf(buf, "{x    <{g%d{x>         %s%s{x\n\r", sph, color, rote_table[rote].name);
            send_to_char(buf,ch);
            sph = 0;
            multi = 0;
        }
    }

    return;
}

void do_rote(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim = ch;
    OBJ_DATA *obj = NULL;
    /* char buf[MAX_STRING_LENGTH]; */
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    int rote, count, roll, success = 0;
    bool pass=TRUE;
    bool extra=FALSE;
    bool countered=FALSE;
    argument = one_argument( argument, arg1 );
    argument = first_arg( argument, arg2, FALSE );
    argument = first_arg( argument, arg3, FALSE );
    //argument = one_argument( argument, arg2 );
    //argument = one_argument( argument, arg3 );

    if(ch->avatar == 0)
    {
        send_to_char("Your Avatar is not Awakened!\n\r",ch);
        return;
    }
    if(arg1[0] == '\0')
    {
        send_to_char( "Create what Magickal effect?\n\r", ch );
        return;
    }
    if((rote = rote_lookup(arg1)) == 0)
    {
        send_to_char("While the metaphysical concepts of magickal effects conceptually permits the\n\rinvocation of virtually any phenomenon, that is an effect you have no knowledge of.\n\r",ch);
        return;
    }
    if(ch->sphere[SPHERE_CORRESPONDENCE] < rote_table[rote].correspondence)
    pass = FALSE;
    if(ch->sphere[SPHERE_ENTROPY] < rote_table[rote].entropy)
    pass = FALSE;
    if(ch->sphere[SPHERE_FORCES] < rote_table[rote].forces)
    pass = FALSE;
    if(ch->sphere[SPHERE_LIFE] < rote_table[rote].life)
    pass = FALSE;
    if(ch->sphere[SPHERE_MATTER] < rote_table[rote].matter)
    pass = FALSE;
    if(ch->sphere[SPHERE_MIND] < rote_table[rote].mind)
    pass = FALSE;
    if(ch->sphere[SPHERE_PRIME] < rote_table[rote].prime)
    pass = FALSE;
    if(ch->sphere[SPHERE_SPIRIT] < rote_table[rote].spirit)
    pass = FALSE;
    if(ch->sphere[SPHERE_TIME] < rote_table[rote].time)
    pass = FALSE;
    if(!pass)
    {
        send_to_char("You lack skill in the required Spheres of Knowledge to invoke this effect.\n\r",ch);
        return;
    }
/*
    if(ch->arete < 5)
    {
        obj = get_eq_char(ch,WEAR_WIELD);
        if(obj == NULL || !IS_OBJ_STAT(obj,ITEM_FOCI) || obj->pIndexData->vnum != ch->foci) pass = FALSE;

        obj = get_eq_char(ch,WEAR_HOLD);
        if(obj == NULL || !IS_OBJ_STAT(obj,ITEM_FOCI) || obj->pIndexData->vnum != ch->foci) pass = FALSE;

        if(!pass)
        {
            send_to_char("You are not enlightened enough yet to preform Magick without a Foci.\n\r",ch);
            return;
        }
    }
*/

    if(arg2[0] != '\0' && !str_prefix(arg2,"extra")) extra = TRUE;

    switch(rote_table[rote].target)
    {
        case TARGET_NONE:   victim = ch; break;
        case TARGET_CHAR:   if(arg2[0] == '\0'|| extra)
                            {
                                if(ch->fighting != NULL) victim = ch->fighting;
                                else
                                {
                                    send_to_char("Upon whom do you wish to invoke this affect?\n\r",ch);
                                    return;
                                }
                            }
                            else if((victim = get_char_room(ch,NULL, arg2)) == NULL && !extra)
                            {
                                send_to_char("There is no one here by that name.\n\r",ch);
                                return;
                            }
                            break;
        case TARGET_ROOM:   if(arg2[0] == '\0' || (victim = get_char_world(ch,arg2)) == NULL)
                            {
                                send_to_char("You sense no one by that name.\n\r",ch);
                                return;
                            }
                            break;
        case TARGET_OBJ:    if(arg2[0] == '\0'|| (obj = get_obj_carry(ch,arg2,ch)) == NULL)
                            {
                                send_to_char("You do not have that object.\n\r",ch);
                                return;
                            }
                            break;
    }

/*

    if(arg2[0] == '\0')
    {
        if ( ch->fighting != NULL ) victim = ch->fighting;
        else victim = ch;
    }
    if(!str_prefix(arg2,"extra"))
    {
        if(ch->quintessence < rote_table[rote].quintessence*2)
        {
            send_to_char("You do not possess enough Quintessential energy to inact that powerful of an effect.\n\r",ch);
            return;
        }
        extra = TRUE;
        if(rote_table[rote].target != TARGET_OBJ)
        {
            if ( ch->fighting != NULL ) victim = ch->fighting;
            else victim = ch;
        }
    }
    if((obj = get_obj_carry(ch,arg2,ch)) != NULL && rote_table[rote].target == TARGET_OBJ)
        victim = ch;
    else if(rote_table[rote].target == TARGET_OBJ)
    {
        send_to_char("You do not have that item.\n\r",ch);
        return;
    }
    else if((victim = get_char_world(ch,arg2)) == NULL)
    {
            send_to_char("You sense no one by that name.\n\r",ch);
            return;
    }
    else if(rote_table[rote].target == TARGET_CHAR && victim->in_room != ch->in_room)
    {
        send_to_char("There is no one here by that name.\n\r",ch);
        return;
    }
    if(arg3[0] != '\0' && !str_prefix(arg3,"extra") && !extra)
    {
        if(ch->quintessence < rote_table[rote].quintessence*2)
        {
            send_to_char("You do not possess enough Quintessential energy to inact that powerful of an effect.\n\r",ch);
            return;
        }
        extra = TRUE;
    }
*/
    if(arg3[0] != '\0' && !str_prefix(arg3,"extra")) extra = TRUE;
    if(extra)
    {
        if(ch->quintessence < rote_table[rote].quintessence + 1)
        {
            send_to_char("You do not possess enough Quintessential energy to enact that powerful of an effect.\n\r",ch);
            return;
        }
        else
        {
            ch->quintessence -= rote_table[rote].quintessence + 1;
            extra = TRUE;
        }
    }
    else if(ch->quintessence < rote_table[rote].quintessence)
    {
            send_to_char("Your corporal form has not yet accumulated the quintessential energies needed to invoke this effect.\n\r",ch);
            return;
    }
    else ch->quintessence -= rote_table[rote].quintessence;
/*
    if(!extra)
    {
        if(ch->quintessence < rote_table[rote].quintessence)
        {
            send_to_char("Your corporal form has not yet accumulated the quintessential energies needed to invoke this effect.\n\r",ch);
            return;
        }
        ch->quintessence -= rote_table[rote].quintessence;
    }
    else ch->quintessence -= rote_table[rote].quintessence*2;
*/
    for(count = ch->arete+(ch->cswillpower/2); count > 0; count--)
    {
        roll = number_range(1,10);
        if(roll == 1) success--;
        if(extra && roll >= (rote_table[rote].diff+2)) success++;
        else if(roll >= rote_table[rote].diff+3) success++;
    }

    if(success == 0)
    {
      if (rote_table[rote].passthrough)
        (*rote_table[rote].rote_fun) (ch,success,victim,obj);
      else if (rote_table[rote].vulgar && number_percent() < ch->paradox)
        {
          paradox_check(ch,rote_table[rote].vulgar);
          send_to_char("A cold chill enters your body as you feel the eye of Paradox swing towards you.\n\r",ch);
        }
      else
          send_to_char("The forces of Paradox smooth reality out before the effect can take place.\n\r",ch);
      return;
      }

    if(success < 0)
    {
      if (rote_table[rote].passthrough)
        (*rote_table[rote].rote_fun) (ch,success,victim,obj);
      else
        send_to_char("A cold chill enters your body as you feel the eye of Paradox swing towards you.\n\r",ch);
      paradox_check(ch,rote_table[rote].vulgar);
      return;
    }
    if(!IS_AFFECTED2(ch,AFF2_UMBRA) && rote_table[rote].vulgar) ch->paradox++;
    while(ch->quintessence+ch->paradox > ch->max_quintessence)
        ch->quintessence--;
    WAIT_STATE( ch, 12 );
    if((victim->race == race_lookup("MIB") ||
    victim->race == race_lookup("HIT Mark") ||
    victim->race == race_lookup("Marauder") ||
    victim->race == race_lookup("Nephandi")) &&
    number_range(1,victim->level/10) > success)
    countered = TRUE;

    if(countered)
    {
        send_to_char("Your opponent imposes his Will upon your Magicks and smooths out the effect before it can enter this reality.\n\r",ch);
        return;
    }

    if(rote == rote_lookup("mutate form"))
      rote_mutateform(ch,success,arg2);
    else
      (*rote_table[rote].rote_fun) (ch,success,victim,obj);
    if (number_range(1, 3) == 2)
        gain_exp(ch, success*2);
    return;
}

void do_mage(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    int quint, natta, para;

    if(ch->avatar == 0)
    {
        send_to_char("Your Avatar is not Awakened.\n\r" ,ch);
        return;
    }
    sprintf(buf,"You are a mage of the {W%s{x Tradition.\n\r",capitalize(tradition_table[ch->tradition].name));
    send_to_char(buf,ch);
    sprintf(buf,"You hold within you {W%d{x Quintessence and {R%d{x Paradox out of {W%d{x.\n\r",ch->quintessence,ch->paradox, ch->max_quintessence);
    send_to_char(buf,ch);
    sprintf(buf,"Your Avatar has a strength of {Y%d{x.\n\r", ch->avatar);
    send_to_char(buf,ch);
    sprintf(buf,"Your level of Enlightenment (Arete) is {Y%d{x.\n\r",ch->arete);
    send_to_char(buf,ch);
    sprintf(buf,"Your strength in the Spheres are as follows:\n\r");
    send_to_char(buf,ch);
    sprintf(buf,"Prime:            %s\n\r",dots(ch->sphere[SPHERE_PRIME], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Spirit:           %s\n\r",dots(ch->sphere[SPHERE_SPIRIT], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Time:             %s\n\r",dots(ch->sphere[SPHERE_TIME], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Mind:             %s\n\r",dots(ch->sphere[SPHERE_MIND], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Matter:           %s\n\r",dots(ch->sphere[SPHERE_MATTER], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Life:             %s\n\r",dots(ch->sphere[SPHERE_LIFE], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Forces:           %s\n\r",dots(ch->sphere[SPHERE_FORCES], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Correspondence:   %s\n\r",dots(ch->sphere[SPHERE_CORRESPONDENCE], FALSE));
    send_to_char(buf,ch);
    sprintf(buf,"Entropy:          %s\n\r",dots(ch->sphere[SPHERE_ENTROPY], FALSE));
    send_to_char(buf,ch);

    return;
}

void do_sphere(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;


    if (IS_NPC(ch)) return;


    if ( IS_IMMORTAL(ch) && (victim = get_char_world(ch, argument)) != NULL &&
    !IS_NPC(victim))
    {//print to IMMs.
        cprintf(ch,"%s's Arete knowledge is %d and your Avatar strength is %d.\n\r",victim->name, victim->arete, victim->avatar);
        cprintf(ch,"%s's strength in the Spheres are as follows:\n\r", victim->name);
        cprintf(ch,"Prime:           %s\n\r",dots(victim->sphere[SPHERE_PRIME], FALSE));
        cprintf(ch,"Spirit:          %s\n\r",dots(victim->sphere[SPHERE_SPIRIT], FALSE));
        cprintf(ch,"Time:            %s\n\r",dots(victim->sphere[SPHERE_TIME], FALSE));
        cprintf(ch,"Mind:            %s\n\r",dots(victim->sphere[SPHERE_MIND], FALSE));
        cprintf(ch,"Matter:          %s\n\r",dots(victim->sphere[SPHERE_MATTER], FALSE));
        cprintf(ch,"Life:            %s\n\r",dots(victim->sphere[SPHERE_LIFE], FALSE));
        cprintf(ch,"Forces:          %s\n\r",dots(victim->sphere[SPHERE_FORCES], FALSE));
        cprintf(ch,"Correspondence:  %s\n\r",dots(victim->sphere[SPHERE_CORRESPONDENCE], FALSE));
        cprintf(ch,"Entropy:         %s\n\r",dots(victim->sphere[SPHERE_ENTROPY], FALSE));
        return;
    }
    if(ch->avatar == 0)
    {
        send_to_char("You are not one of the Enlightened.\n\r",ch);
        return;
    }
        //sprintf(buf,"You have %d Sphere Points unused.\n\r", ch->dpoints);
        //send_to_char(buf,ch);
        sprintf(buf,"Your Arete knowledge is %d and your Avatar strength is %d.\n\r",ch->arete, ch->avatar);
        send_to_char(buf,ch);
        sprintf(buf,"Your strength in the Spheres are as follows:\n\r");
        send_to_char(buf,ch);
        sprintf(buf,"Prime:           %s\n\r",dots(ch->sphere[SPHERE_PRIME], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Spirit:          %s\n\r",dots(ch->sphere[SPHERE_SPIRIT], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Time:            %s\n\r",dots(ch->sphere[SPHERE_TIME], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Mind:            %s\n\r",dots(ch->sphere[SPHERE_MIND], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Matter:          %s\n\r",dots(ch->sphere[SPHERE_MATTER], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Life:            %s\n\r",dots(ch->sphere[SPHERE_LIFE], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Forces:          %s\n\r",dots(ch->sphere[SPHERE_FORCES], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Correspondence:  %s\n\r",dots(ch->sphere[SPHERE_CORRESPONDENCE], FALSE));
        send_to_char(buf,ch);
        sprintf(buf,"Entropy:         %s\n\r",dots(ch->sphere[SPHERE_ENTROPY], FALSE));
        send_to_char(buf,ch);
        return;

}

// ACTUAL ROTES CODE FOLLOW HERE.
// bm_rote -- bookmark for rotes.
void rote_perceiveentropy(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;
    send_to_char("+======================== SENSE FATE AND FORTUNE  =======================+\n\r", ch);
    send_to_char("|   You tap into the realm of Entropic states surrounding your target.   |\n\r", ch);
    if (IS_NPC(victim))
      sprintf(buf, "+====================[ %s ]====================+\n\r", center(victim->short_descr, 28, " "));
    else
      sprintf(buf, "+====================[ %s ]====================+\n\r", center(victim->name, 28, " "));
    send_to_char(buf, ch);
    sprintf(buf, "|                    [       Age: %3d year%s old     ]                    |\n\r",
      get_age(victim), get_age(victim) == 1 ? " " : "s");
    send_to_char(buf,ch);
    if (is_affected(victim, gsn_curse) || IS_AFFECTED(victim, AFF_CURSE))
      send_to_char("|        {DA dark fate seems to loom over this individual's future.{x        |\n\r", ch);
    if (is_affected(victim, gsn_touch))
      send_to_char("|   {YThe highest of the pantheons seem to favor your target with grace.{x   |\n\r", ch);
    if (is_affected(victim, skill_lookup("bless")))
      send_to_char("|              {WThe gods smile upon this favored individual.{x              |\n\r", ch);
    if (is_affected(victim, gsn_forget))
      send_to_char("|             {mEntropic energies blanket your target's mind.{x              |\n\r", ch);
    if (IS_SET(victim->act2, ACT2_INFLUENCE))
      send_to_char("|       {BGreat, influential things await this target in the future.{x       |\n\r", ch);

    if(success > 1 )
    {
      send_to_char("+------------------------------------------------------------------------+\n\r", ch);
      sprintf(buf, "| Immune to    : %s\n\r", imm_bit_name(victim->imm_flags));
      send_to_char(buf,ch);
      sprintf(buf, "| Resistant to : %s\n\r", imm_bit_name(victim->res_flags));
      send_to_char(buf,ch);
      sprintf(buf, "| Vulnerable to: %s\n\r", imm_bit_name(victim->vuln_flags));
      send_to_char(buf,ch);
    }

    if(success >= 3)
    {
      send_to_char("+------------------------------------------------------------------------+\n\r", ch);
      sprintf(buf, "| %6d/%6d Health     %6d/%6d Mana     %6d/%6d Movement |\n\r",
        victim->hit,  victim->max_hit,
        victim->mana, victim->max_mana,
        victim->move, victim->max_move);
      send_to_char(buf,ch);
    }

    send_to_char("+------------------------------------------------------------------------+\n\r", ch);
    return;
}

void rote_controlrandomness(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_controlrandomness))
    {
        send_to_char("You are already altering the laws of chance.\n\r",ch);
        return;
    }

    send_to_char("You bend the laws of chance in your favor.\n\r" ,ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_controlrandomness;
    af.level     = success;
    af.duration  = 15 + 5*success;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
}

void rote_destroymatter( CHAR_DATA *ch, int success, CHAR_DATA *victim )
{
    OBJ_DATA *obj;
    char buf[MSL];

    if (IS_NPC(ch)) return;

    if(victim == NULL || victim == ch)
    {
        send_to_char("This must be used on another.\n\r",ch);
        return;
    }
    if(!IS_NPC(victim))
    {
        send_to_char("I don't THINK so!\n\r",ch);
        return;
    }

    if (victim->pIndexData->pShop != NULL)
    {
        send_to_char("If you destroy the shopkeeper's wares, there will be nothing left to purchase.\n\r",ch);
        return;
    }

    if ( ( obj = get_eq_char( victim, WEAR_WIELD ) ) == NULL )
      if ( ( obj = get_eq_char( victim, WEAR_SHIELD ) ) == NULL )
        if ( ( obj = get_eq_char( victim, WEAR_HOLD ) ) == NULL )
        {
          send_to_char("Your opponent has nothing readily available to destroy!\n\r",ch);
          return;
        }

    if (success < 0)
    {
      act("The feedback from trying to alter reality causes your equipment to heat and burn!", ch, obj, victim, TO_CHAR);
      act("$n glances in your direction and yelps in pain.", ch, obj, victim, TO_VICT);
      act("$n yelps in pain and surprise!", ch, obj, victim, TO_NOTVICT);
      spell_heat_metal( gsn_heat_metal, ch->level / 2 + 50, ch, ch, 0 );
      return;
    }

    if (success == 0)
    {
      act("You point at $p, but the power flows from your body and into the universe.", ch, obj, victim, TO_CHAR);
      act("$n points at you and for a brief moment, $p vibrates slightly.", ch, obj, victim, TO_VICT);
      act("$n gestures for a moment, then points at $p being used by $N.", ch, obj, victim, TO_NOTVICT);
      return;
    }

    act( "You point at $N, snapping your fingers, and $p turns into dust!",  ch, obj, victim, TO_CHAR    );
    act( "$n points at you and snaps $s fingers, turning $p into dust before your eyes!", ch, obj, victim, TO_VICT);
    act( "$n points at $N, snaps $s fingers, and $p crumbles into dust!",  ch, obj, victim, TO_NOTVICT );

    extract_obj( obj );
    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}

void rote_witherlife(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if (success < 0)
    {
      act("Entropic energies rush back into you as you feel weaker.", ch, NULL, victim, TO_CHAR);

      af.where     = TO_AFFECTS;
      af.type      = gsn_quietus_weakness;
      af.level     = 1;
      af.duration  = -success;
      af.location  = APPLY_CS_STR;
      af.modifier  = -1;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      return;
    }

    if (success == 0)
    {
      act("You focus your entropic energies at $N, but they dissipate before reaching the target.", ch, NULL, victim, TO_CHAR);
      return;
    }

    act( "You reach out and touch $N who screams in anguish and agony!",  ch, NULL, victim, TO_CHAR    );
    act( "$n reaches out and touches $N who suddenly screams in agony!",  ch, NULL, victim, TO_NOTVICT );
//    damage( ch, victim, success*ch->level*8, gsn_magick, DAM_DISEASE, TRUE);
    d10_damage( ch, victim, success, ch->level * 3, gsn_magick, DAM_DISEASE, DEFENSE_NONE, TRUE, TRUE);

    if (success >= 4)
    {
      act("You watch closely as the entropic energies wrack $N's body, weakening $M.", ch, NULL, victim, TO_CHAR);
      act("Your body flares in excrutiating pain as entropic energies ravage you!", ch, NULL, victim, TO_VICT);
      act("$N writhes in agony as $S body is weakened!", ch, NULL, victim, TO_NOTVICT);

      af.where     = TO_AFFECTS;
      af.type      = gsn_quietus_weakness;
      af.level     = success;
      af.duration  = success*5;
      af.location  = APPLY_CS_STR;
      af.modifier  = -1;
      af.bitvector = 0;
      affect_to_char( victim, &af );

      af.location  = APPLY_CS_STA;
      affect_to_char( victim, &af );
    }

    return;
}

void rote_entropicmind(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if (!IS_NPC(victim)) return;

    if (success < 0)
    {
      act("Entropic energies rush through your mind, and you can't seem to remember why.", ch, NULL, victim, TO_CHAR);

      af.where     = TO_AFFECTS;
      af.type      = gsn_forget;
      af.level     = 1;
      af.duration  = -success;
      af.location  = APPLY_CS_INT;
      af.modifier  = -1;
      af.bitvector = 0;
      affect_to_char( ch, &af );
      return;
    }

    if (success == 0)
    {
      act("You focus your entropic energies at $N's mind, but nothing seems to happen.", ch, NULL, victim, TO_CHAR);
      return;
    }

    act( "You advance the entropic chaos in $N's mind to an advanced state.",  ch, NULL, victim, TO_CHAR );

    af.where     = TO_AFFECTS;
    af.type      = gsn_forget;
    af.level     = success;
    af.duration  = success*3 + 5;
    af.location  = APPLY_CS_INT;
    af.modifier  = -2;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    if (success > 4)
    {
      act( "The entropic energies take hold of $N and enfeeble $S mind even more!",  ch, NULL, victim, TO_CHAR );

      af.location = APPLY_CS_PER;
      affect_to_char(victim, &af);
      af.location = APPLY_CS_WIT;
      affect_to_char(victim, &af);
    }

    multi_hit( victim, ch, TYPE_UNDEFINED );
    return;
}

void rote_perceiveforces(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(IS_AFFECTED(ch, AFF_INFRARED))
    {
        send_to_char("You already perceive the forces around you.\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_magick;
    af.level     = ch->level;
    af.duration  = (success * 5) + 25;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_INFRARED;
    affect_to_char( victim, &af );

    send_to_char("You heighten your senses to perceive the Forces around you.\n\r",ch);
    return;
}

void rote_dischargestatic(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (IS_NPC(ch)) return;

    act( "You focus the static energy in your body and direct it towards $N, who suddenly has a very shocking experience!",  ch, NULL, victim, TO_CHAR    );
    act( "$n's hair stands on end a moment before pointing at $N who gets a real charge out of it!",  ch, NULL, victim, TO_NOTVICT );
    act( "$n points at you and.. zzzzzZAP! You have an electrifying experience!",  ch, NULL, victim, TO_VICT    );

//    damage( ch, victim, success*ch->level*3, gsn_magick, DAM_LIGHTNING, TRUE);
    d10_damage( ch, victim, success, ch->level, gsn_magick, DAM_LIGHTNING, DEFENSE_NONE, TRUE, TRUE);
    if (success > 4 )
    {
        STOPPED(victim, 2*PULSE_VIOLENCE);
        act("The static shock overloads the impulses in your brain!",ch, NULL, victim, TO_VICT);
        act("The {Ystatic jolt{x surges through $N's body!", ch, NULL, victim, TO_NOTVICT);
    }
    return;
}

void rote_firestorm(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (IS_NPC(ch)) return;

    act( "You alter the patterns of air around $N and feed Quintessence into your creation, instantly engulfing $M in an inferno of flame.",  ch, NULL, victim, TO_CHAR    );
    act( "$n peers intently at $N who suddenly bursts into flame!",  ch, NULL, victim, TO_NOTVICT );
    act( "A wall of flame consumes you as a flash of heat engulfs your body and burns away at your skin!",  ch, NULL, victim, TO_VICT );

    if (success >= 4)
    {
        act( "Your magickal flames ignite and burn brightly, fueled by your whims, and blind $N with smoke and fire!", ch, NULL, victim, TO_CHAR);
        act( "The flames leap into $N's face, blinding $M with smoke and flame!", ch, NULL, victim, TO_NOTVICT);
        act( "The flames leap into your face, blinding you with searing pain!", ch, NULL, victim, TO_VICT);
        fire_effect(victim, success, ch->level, TARGET_CHAR);
    }

//    damage( ch, victim, success*ch->level*6, gsn_magick, DAM_FIRE, TRUE);
    d10_damage( ch, victim, success, ch->level * 2, gsn_magick, DAM_FIRE, DEFENSE_NONE, TRUE, TRUE);

    return;
}

void rote_kineticshield(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_kineticshield))
    {
        send_to_char("You already reflect the kinetic energy of weapons directed towards you.\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_kineticshield;
    af.level     = ch->level;
    af.duration  = (ch->avatar+success)*10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    act( "You bend the forces of kinetic energy to surround your body in a defensive field.",  ch, NULL, victim, TO_CHAR    );
    act( "$n focuses as a shimmering bubble of energy surrounds $m.",  ch, NULL, victim, TO_NOTVICT );

    return;
}

void rote_crushofmountains(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (IS_NPC(ch)) return;

    act( "You focus your will intently, altering the gravitational patterns around $N and feeding a flood of quintessence into it... causing $N to instantly implode!",  ch, NULL, victim, TO_CHAR    );
    act( "The room seems to shift and bend, suddenly everyone is pulled towards $N as they nearly implode!",  ch, NULL, victim, TO_NOTVICT    );
    act( "The air thickens around you, grating your bones, forcing the air from your lungs in an endless, silent scream as the last of your life is crushed from your body!",  ch, NULL, victim, TO_VICT );

//    damage( ch, victim, success*ch->level*12, gsn_magick, DAM_NEGATIVE, TRUE);
    d10_damage( ch, victim, success, ch->level * 4, gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);

    return;
}

void rote_touchofhelios(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    act( "You bend reality and bring into existance a single ray of sunlight directed at $N!",  ch, NULL, victim, TO_CHAR    );
    act( "You are blinded briefly as $n directs a bright beam of sunlight towards $N!",  ch, NULL, victim, TO_NOTVICT    );
    act( "All sight is stolen from you briefly, as $n shines a brilliant beam of sunlight directly at you!",  ch, NULL, victim, TO_VICT );

    if(IS_SET(victim->vuln_flags,VULN_SUNLIGHT))
//      damage( ch, victim, success*ch->level*16, gsn_magick, DAM_SUNLIGHT, TRUE);
      d10_damage( ch, victim, success, ch->level * 5, gsn_magick, DAM_SUNLIGHT, DEFENSE_NONE, TRUE, TRUE);

    else
//      damage( ch, victim, success*ch->level*8, gsn_magick, DAM_SUNLIGHT, TRUE);
      d10_damage( ch, victim, success, ch->level * 2, gsn_magick, DAM_SUNLIGHT, DEFENSE_NONE, TRUE, TRUE);


    if (success > 4)
    {
      af.where     = TO_AFFECTS;
      af.type      = gsn_touchofhelios;
      af.level     = success;
      af.location  = 0;
      af.modifier  = 0;
      af.duration  = 1;
      af.bitvector = AFF_BLIND;
      affect_to_char( victim, &af );
      act("$n is blinded momentarily by the sunlight beam.",victim,NULL,NULL,TO_ROOM);
      act("A pinpointed sunlight beam blinds you momentarily!",victim,NULL,NULL,TO_CHAR);
      return;
    }
    return;
}

void rote_spatialperceptions(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(IS_AFFECTED(ch, AFF_DARK_VISION))
    {
        send_to_char("You are already aware of everything around you.\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_magick;
    af.level     = ch->level;
    af.duration  = (success * 5) + 25;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DARK_VISION;
    affect_to_char( victim, &af );
    send_to_char("You open your mind to everything around you.\n\r",ch);
    return;
}

void rote_correspondenceperceptions(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *was_room;
    char buf[MSL];

    if(room_is_private(victim->in_room) || victim->obfuscate >= 4 || !can_see(ch, victim))
    {
        send_to_char( "You do not have the strength to sense that person.\n\r", ch );
        return;
    }

    if (IS_SET(victim->act, ACT_QUESTMOB))
    {
      send_to_char("Quit trying to cheat the system!  You should be ashamed of yourself.\n\r", ch);
      sprintf(buf,"FLAG:: %s thought they were sneaky and tried to scry on %s with Correspondence Perceptions.\n\r    Don't worry, I stopped them.",ch->name, victim->name);
      wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
      return;
    }

    was_room = ch->in_room;
    char_from_room( ch );
    if (victim == ch)
        char_to_room(ch, was_room);
    else
        char_to_room( ch, victim->in_room );
    do_function(ch, &do_look, "auto" );
    char_from_room( ch );
    char_to_room( ch, was_room );
    return;
}

void rote_sevenleaguestride(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    char buf[MSL];
    ROOM_INDEX_DATA *pRoomIndex;

    if(victim == ch)
    {
        send_to_char("You are already at yourself.\n\r",ch);
        return;
    }

    if (IS_SET(victim->act, ACT_QUESTMOB))
    {
      send_to_char("Quit trying to cheat the system!  You should be ashamed of yourself.\n\r", ch);
      sprintf(buf,"FLAG:: %s thought they were sneaky and tried to Seven League Stride to %s.\n\r    Don't worry, I stopped them.",ch->name, victim->name);
      wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
      return;
    }

    if(IS_SET(victim->in_room->room_flags, ROOM_SAFE)
	|| IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
		||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
	|| IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
	|| IS_SET(ch->act,PLR_ARENA)
	|| IS_SET(victim->act,PLR_ARENA))
    {
        send_to_char( "You fear that you will not be able to complete the transition.\n\r", ch );
        return;
    }

    if (room_is_private(victim->in_room))
    {
        send_to_char( "You may not enter that room.\n\r", ch );
        return;
    }

    if (success < 0)
    {
      pRoomIndex = get_random_room(victim);
      act("Focusing upon a distant location, you fold the physical {Ds{wp{Wa{wc{De{x in front of you and step through to your destination.",ch,NULL,victim,TO_CHAR);
      act("The {Ds{wp{Wa{wc{De{x immediately in front of $n seems to warp for just a moment, before $e takes a step forward and vanishes altogether!",ch,NULL,victim,TO_ROOM);
      char_from_room( ch );
      char_to_room( ch, pRoomIndex );
      act("A strong presence can be felt before the physical {Ds{wp{Wa{wc{De{x nearby suddenly folds in on itself; $n steps out of the distortion.",ch,NULL,victim,TO_ROOM);
      do_function(ch, &do_look, "auto" );
    	return;
    }

    if (success == 0)
    {
    	act("Focusing upon a distant place, reality bends briefly in front of you before snapping back in place.", ch, NULL, victim, TO_CHAR);
    	act("The space in front of $n seems to bend for a split second, before snapping back into place.", ch, NULL, victim, TO_NOTVICT);
    	WAIT_STATE(ch, 9);
    	return;
    }

    act("Focusing upon a distant location, you fold the physical {Ds{wp{Wa{wc{De{x in front of you and step through to your destination.",ch,NULL,victim,TO_CHAR);
    act("The {Ds{wp{Wa{wc{De{x immediately in front of $n seems to warp for just a moment, before $e takes a step forward and vanishes altogether!",ch,NULL,victim,TO_ROOM);
    char_from_room( ch );
    char_to_room( ch, victim->in_room );
    act("A strong presence can be felt before the physical {Ds{wp{Wa{wc{De{x nearby suddenly folds in on itself; $n steps out of the distortion.",ch,NULL,victim,TO_ROOM);
    do_function(ch, &do_look, "auto" );
    return;
}
void rote_conjoinlocalities(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    OBJ_DATA *portal;
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MSL];

    if(victim == ch)
    {
        send_to_char("You are already at yourself.\n\r",ch);
        return;
    }

    if (IS_SET(victim->act, ACT_QUESTMOB))
    {
      send_to_char("Quit trying to cheat the system!  You should be ashamed of yourself.\n\r", ch);
      sprintf(buf,"FLAG:: %s thought they were sneaky and tried to Conjoin Localities to %s.\n\r    Don't worry, I stopped them.",ch->name, victim->name);
      wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
      return;
    }

    if(victim->in_room == NULL || IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
	||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->imm_flags,IMM_SUMMON)
    ||   IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char( "You understanding of reality does not extend to defying the base laws of the universal structure.\n\r", ch );
        return;
    }

    if (success < 0)
    	{
        portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    	  SET_BIT(portal->value[2], GATE_RANDOM);
        portal->timer = success;
        portal->value[3] = victim->in_room->vnum;

        obj_to_room(portal,ch->in_room);

        act("$n opens a hole in the fabric of space and time, thus creating $p.",ch,portal,NULL,TO_ROOM);
        act("You create a hole in space and time then form it into $p.",ch,portal,NULL,TO_CHAR);
    		return;
    	}

      if (success == 0)
      {
      	act("Focusing upon a distant place, reality bends briefly in front of you before snapping back in place.", ch, NULL, victim, TO_CHAR);
      	act("The space in front of $n seems to bend for a split second, before snapping back into place.", ch, NULL, victim, TO_NOTVICT);
      	WAIT_STATE(ch, 9);
      	return;
      }

    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = success;
    portal->value[3] = victim->in_room->vnum;

    obj_to_room(portal,ch->in_room);

    act("$n opens a hole in the fabric of space and time, thus creating $p.",ch,portal,NULL,TO_ROOM);
    act("You create a hole in space and time then form it into $p.",ch,portal,NULL,TO_CHAR);
    return;
}

void rote_mergelocalities(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    OBJ_DATA *portal;
    OBJ_DATA *portal2;
    char buf[MSL];

    if(victim == ch)
    {
        send_to_char("You are already at yourself.\n\r",ch);
        return;
    }

    if (IS_SET(victim->act, ACT_QUESTMOB))
    {
      send_to_char("Quit trying to cheat the system!  You should be ashamed of yourself.\n\r", ch);
      sprintf(buf,"FLAG:: %s thought they were sneaky and tried to Merge Localities to %s.\n\r    Don't worry, I stopped them.",ch->name, victim->name);
      wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
      return;
    }

    if(victim->in_room == NULL
    ||   IS_SET(victim->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SAFE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_PRIVATE)
    ||   IS_SET(ch->in_room->room_flags, ROOM_PRIVATE)
	  ||   IS_SET(victim->in_room->room_flags, ROOM_NOTELE)
	  ||   IS_SET(ch->in_room->room_flags, ROOM_NOTELE)
	  ||   IS_SET(victim->in_room->area->area_flags, AREA_NOTELE)
	  ||   IS_SET(ch->in_room->area->area_flags, AREA_NOTELE)
    ||   IS_SET(victim->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(ch->in_room->room_flags, ROOM_SOLITARY)
    ||   IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
    ||   IS_SET(victim->imm_flags,IMM_SUMMON)
    ||   IS_SET(victim->act,PLR_ARENA)
    ||   IS_SET(ch->act,PLR_ARENA))
    {
        send_to_char( "Your understanding of reality does not extend to defying the base laws of the universal structure.\n\r", ch );
        return;
    }

    if (success < 0)
    {
      return;
    }

    if (success == 0)
    {
      return;
    }

    portal = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal->timer = success * 3;
    portal->value[3] = victim->in_room->vnum;
    obj_to_room(portal, ch->in_room);

    portal2 = create_object(get_obj_index(OBJ_VNUM_PORTAL),0);
    portal2->timer = success * 3;
    portal2->value[3] = ch->in_room->vnum;
    obj_to_room(portal2,victim->in_room);

    act("$n opens a hole in reality, creating $p to span two locations.",ch,portal,NULL,TO_ROOM);
    act("You rip a hole directly through the Tellurian, widening $p to merge two locations.",ch,portal,NULL,TO_CHAR);
    act("A gaping hole rips in reality, and $p now spans between two locations.",victim,portal2,NULL,TO_ROOM);
    act("A tear in reality appears in front of you, leaving $p to merge two locations.",victim,portal2,NULL,TO_CHAR);
    return;
}

void rote_senselife(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA *paf;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    if (!living_being(victim))
    {
        send_to_char("[ You detect no trace of any {gLife{x Pattern. ]\n\r", ch);
        return;
    }

    {
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;
    send_to_char("{g+========================   SENSE LIFE PATTERN    =======================+{x\n\r", ch);
    send_to_char("{g|           {xYou scrutinize the Pattern of Life of your target.           {g|{x\n\r", ch);
    if (IS_NPC(victim))
      sprintf(buf, "{g+====================[{x %s {g]====================+{x\n\r", center(victim->short_descr, 28, " "));
    else
      sprintf(buf, "{g+====================[{x %s {g]====================+{x\n\r", center(victim->name, 28, " "));
    send_to_char(buf, ch);
    sprintf(buf, "{g|                   {x Race: %-16s Sex: %s{g               |{x\n\r",
        race_table[victim->race].name,
        victim->sex == 0 ? "{Wsexless  " : victim->sex == 1 ? "{Bmale     " : "{Mfemale   ");
    send_to_char(buf, ch);
    sprintf(buf, "{g|{x                    Blood: {r%3d{x   Age: %5d year%s old                   {g|{x\n\r",
      victim->pblood, get_age(victim), get_age(victim) == 1 ? " " : "s");
    send_to_char(buf,ch);
    sprintf(buf, "{g|{x %6d/%6d Health     %6d/%6d Mana     %6d/%6d Movement {g|{x\n\r",
        victim->hit,  victim->max_hit, victim->mana, victim->max_mana, victim->move, victim->max_move);
    send_to_char(buf,ch);

    if (success > 1)
    {
        send_to_char("{g+----------------------------      Body      ----------------------------+{x\n\r", ch);
        sprintf(buf, "{g|{x Names: %s\n\r", victim->name);
        send_to_char(buf,ch);
        sprintf(buf, "{g|{x Forms: %s\n\r", form_bit_name(victim->form));
        send_to_char(buf,ch);
        sprintf(buf, "{g|{x Parts: %s\n\r", part_bit_name(victim->parts));
        send_to_char(buf,ch);
    if ( !IS_NPC(victim) )
    {
        sprintf( buf,
        "{g|{x   Thirst: %2d    Hunger: %2d    Full: %3d    Drunk: %2d                   {g|{x\n\r",
        victim->pcdata->condition[COND_THIRST], victim->pcdata->condition[COND_HUNGER],
        victim->pcdata->condition[COND_FULL], victim->pcdata->condition[COND_DRUNK]);
    send_to_char( buf, ch );
    }
    }

    if (success > 2)
    {
        send_to_char("{g+----------------------------  Alterations   ----------------------------+{x\n\r", ch);
        if (IS_AFFECTED(victim, AFF_SHIFT))
            send_to_char("{g|  {RYour target's base Life Pattern has been changed from original form.{g  |{x\n\r", ch);
        if (is_affected(victim, gsn_poison))
            send_to_char("{g|               {GPoison courses through your target's body.{g               |{x\n\r", ch);
        if (is_affected(victim, gsn_change_sex))
            send_to_char("{g|{W                  Your target's sex has been altered.                   {g|{x\n\r", ch);
        if (is_affected(victim, gsn_berserk))
            send_to_char("{g|{W             Pulse races spectacularly through your target.             {g|{x\n\r", ch);
        if (is_affected(victim, gsn_bleeding))
            send_to_char("{g|r                 Blood flows profusely from many wounds.                 {g|{x\n\r", ch);
        if (is_affected(victim, gsn_deafened))
            send_to_char("{g|{W                        Your target cannot hear.                        {g|{x\n\r", ch);
        if (is_affected(victim, gsn_laryngitis))
            send_to_char("{g|{W                        Your target cannot speak.                       {g|{x\n\r", ch);
        if (is_affected(victim, gsn_blight))
            send_to_char("{g|{W         The strains of advanced age ravage your target's body.         {g|{x\n\r", ch);
        if (is_affected(victim, gsn_betterbody))
            send_to_char("{g|{W            Your target's body surges with increased muscle.            {g|{x\n\r", ch);
    }

    send_to_char("{g+------------------------------------------------------------------------+{x\n\r", ch);
    return;
}
}

void rote_littlegooddeath(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if(ch == victim)
    {
        send_to_char("You cannot do this to yourself!\n\r",ch);
        return;
    }
    if(!IS_NPC(victim))
    {
        send_to_char("You cannot use this on players.\n\r",ch);
        return;
    }
    if(is_affected(victim,gsn_shadowform) || IS_AFFECTED2(victim, AFF2_MIST) || is_affected(victim,gsn_earthmeld))
    {
        send_to_char("You cannot do this to someone in that form.\n\r",ch);
        return;
    }
    if (victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if (success < 0)
    {
        act("Your heart skips a few beats as it spasms wildly!", ch, NULL, victim, TO_CHAR);
        act("$n looks stricken and holds a hand to $s heart, swooning.", ch, NULL, victim, TO_ROOM);
//        damage(ch, ch, ch->level * 3, gsn_magick, DAM_HARM, TRUE);
        d10_damage( ch, ch, -success, ch->level / 3, gsn_magick, DAM_HARM, DEFENSE_NONE, TRUE, TRUE);

        return;
    }

    if (success == 0)
    {
        act("Your efforts to disrupt the flow of Life through $N fail miserably.", ch, NULL, victim, TO_CHAR);
        return;
    }

    if (success >= 5 && victim->level <= ch->level + 10)
    {
        act("$n darts out a hand and touches $N who slumps over dead.",ch,NULL,victim,TO_NOTVICT);
        act("You reach out and touch $N, disrupting the vital flows of Quintessence through $S Life pattern.",ch,NULL,victim,TO_CHAR);
        kill_em(ch,victim);
        return;
    }

//    damage(ch, victim, success * ch->level * 2, gsn_magick, DAM_HARM, TRUE);
    d10_damage( ch, victim, success, ch->level, gsn_magick, DAM_HARM, DEFENSE_NONE, TRUE, TRUE);

    return;
}

void rote_healself(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if(victim != ch)
    {
        send_to_char("You require more advanced knowledge to alter the Life Pattern of another.\n\r",ch);
        return;
    }
    int gain;
    if (ch->level < 20)
    gain = success * ch->level * (10*ch->sphere[SPHERE_LIFE]);
    else
    gain = success * ch->level * (5*ch->sphere[SPHERE_LIFE]);
    if (gain > 200)
    gain = (gain-200)/2 + 200;
    if (gain > 400)
    gain = (gain-400)/2 + 400;
    if (gain > 600)
    gain = (gain-600)/2 + 600;
    if (gain > 800)
    gain = (gain-800)/2 + 800;
    if (gain > 1000)
    gain = (gain-1000)/2 + 1000;

    victim->hit = UMIN( victim->hit + gain, victim->max_hit );
    victim->move = UMIN(victim->move + (gain/5), victim->max_move);
    victim->agg_dam -= success * ch->sphere[SPHERE_LIFE];
    if(victim->hit > victim->max_hit - victim->agg_dam) victim->hit = (victim->max_hit - victim->agg_dam);
    if(victim->agg_dam < 0) victim->agg_dam = 0;
    update_pos( victim );
    send_to_char( "You feel your wounds mend as you restore your Life pattern..\n\r", victim );
    return;
}

void rote_healother(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    int gain;
    if (ch->level < 20)
    gain = success * ch->level * (10*ch->sphere[SPHERE_LIFE]);
    else
    gain = success * ch->level * (5*ch->sphere[SPHERE_LIFE]);
    if (gain > 200)
    gain = (gain-200)/2 + 200;
    if (gain > 400)
    gain = (gain-400)/2 + 400;
    if (gain > 600)
    gain = (gain-600)/2 + 600;
    if (gain > 800)
    gain = (gain-800)/2 + 800;
    if (gain > 1000)
    gain = (gain-1000)/2 + 1000;

    victim->hit = UMIN( victim->hit + gain, victim->max_hit );
    victim->move = UMIN(victim->move + (gain/5), victim->max_move);
    victim->agg_dam -= success * ch->sphere[SPHERE_LIFE];
    if(victim->hit > victim->max_hit - victim->agg_dam) victim->hit = (victim->max_hit - victim->agg_dam);
    if(victim->agg_dam < 0) victim->agg_dam = 0;
    update_pos( victim );
    act("$n reaches out and lightly touches $N who seems to be feeling better.",ch,NULL,victim,TO_NOTVICT);
    act("You reach out and touch $N regulating their Life Patterns.",ch,NULL,victim,TO_CHAR);
    act("$n reaches out and lightly touches you. You feel healing energies enter your body and feel much better.",ch,NULL,victim,TO_VICT);
    return;
}

void rote_betterbody(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;
    int botch;

    if (is_affected(ch, gsn_betterbody))
    {
      if (get_affect_level(ch, gsn_betterbody) == -1)
        send_to_char("Your Pattern is too strained to be altered further.\n\r", ch);
      else
        send_to_char("You have already boosted your body's physical prowess!\n\r", ch);
      return;
    }

    if (success < 0)
    {
      act("You gently tug at your own Pattern, and accidentally rip part of it!",ch,NULL,NULL,TO_CHAR);
      act("$n flashes a look of pain, as $e doubles over.",ch,NULL,NULL,TO_NOTVICT);

      botch = number_range(1,3);

      switch (botch)
      {
        default:
          break;
        case 1:   af.location = APPLY_CS_STR;
          break;
        case 2:   af.location = APPLY_CS_DEX;
          break;
        case 3:   af.location = APPLY_CS_STA;
          break;
      }

      af.where    = TO_AFFECTS;
      af.type     = gsn_betterbody;
      af.level    = -1;
      af.duration = 1;
      af.modifier = -1;
      af.bitvector    = 0;
      affect_to_char( ch, &af );
      return;
    }

    if (success == 0)
    {
      send_to_char("You gently nudge your Pattern into a strenghthened form, but it promptly revers back to normal.\n\r", ch);
      return;
    }

    send_to_char("You tap into your own Pattern, altering your physical form to its peak.\n\r", ch);

    af.where    = TO_AFFECTS;
    af.type     = gsn_betterbody;
    af.level    = 1;
    af.duration = 15 + (success * 2);
    af.location = APPLY_CS_STR;
    af.modifier = 1;
    af.bitvector    = 0;
    affect_to_char( ch, &af );

    af.location = APPLY_CS_DEX;
    affect_to_char( ch, &af );

    af.location = APPLY_CS_STA;
    affect_to_char( ch, &af );

    return;
}

void rote_mutateform(CHAR_DATA *ch, int success, char *arg)
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA af;

    if (arg[0] == '\0')
    {
      send_to_char( "You may shift into: owl, panther, bear, squirrel, sloth\n\r", ch );
      send_to_char( "You must shift back to human form before changing to another.\n\r", ch );
      ch->paradox--;
      ch->quintessence += 5;
      return;
    }

    if (!str_prefix(arg, "human"))
    {
      ch->quintessence += 4;
      if (!is_affected(ch, gsn_mutateform))
      {
        send_to_char("You have not altered your body into another form.\n\r", ch);
        ch->paradox--;
        return;
      }
      if (is_affected(ch, gsn_mutateform) && get_affect_level(ch, gsn_mutateform) == -1)
      {
        send_to_char("Your body is moving too slowly to effectively change back from sloth form.\n\r", ch);
        return;
      }

      send_to_char( "You reset your Pattern back into humanoid form.\n\r", ch );
      affect_strip(ch,gsn_mutateform);
      affect_strip(ch,gsn_claws);
      ch->affected_by = race_table[ch->race].aff;
      act( "$n's form slowly shifts back into a humanoid form.", ch, NULL, NULL, TO_NOTVICT );
      ch->dam_type = 17;
      return;
    }

    if ( IS_AFFECTED(ch, AFF_SHIFT))
    {
        send_to_char( "You must reform your Pattern back to normal first.\n\r{WSyntax: {crote 'mutate form' human{x\n\r", ch );
        ch->quintessence += 4;
        return;
    }

    if (success < 0)
    {
        act( "Your body shifts forms, and you feel very lethargic.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of a sloth.", ch, NULL, NULL, TO_NOTVICT );
        ch->short_descr = str_dup( "A slow-moving sloth" );
        sprintf(buf, "A slow-moving sloth");
        ch->shift = str_dup( buf );

        af.where     = TO_AFFECTS;
        af.type      = gsn_mutateform;
        af.level     = -1;
        af.duration  = 2;
        af.location  = APPLY_CS_STR;
        af.modifier  = -2;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_DEX;
        af.modifier  = -4;
        af.bitvector = AFF_SLOW;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_MAN;
        af.modifier  = -3;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_MOVE;
        af.modifier  = -ch->level;
        affect_to_char( ch, &af );

        return;
    }

    if (success == 0)
    {
      send_to_char("You begin to tug and weave at your body to transmute into an animal, but your Pattern refuses to budge.\n\r", ch );
      return;
    }

    WAIT_STATE(ch, 60);

    if ( !str_prefix( arg, "owl" ) )
    {
        act( "You feel your body grow lighter as you shift into a night bird.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of an owl.", ch, NULL, NULL, TO_NOTVICT );
        ch->short_descr = str_dup( "A brown and white owl" );
        sprintf(buf, "A brown and white owl");
        ch->shift = str_dup( buf );

        af.where     = TO_AFFECTS;
        af.type      = gsn_mutateform;
        af.level     = MUTATE_OWL;
        af.duration  = success * 15;
        af.location  = APPLY_CS_STR;
        af.modifier  = -1;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_DEX;
        af.modifier  = 1;
        af.bitvector = AFF_FLYING;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_MAN;
        af.modifier  = -3;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_PER;
        af.modifier  = 3;
        af.bitvector = AFF_DARK_VISION;
        affect_to_char( ch, &af );

        af.location  = APPLY_MOVE;
        af.modifier  = ch->level*7;
        affect_to_char( ch, &af );

        return;
    }

    if ( !str_prefix( arg, "squirrel" ))
    {
        act( "You shift forms, and suddenly gain a hankering for nuts.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of a squirrel.", ch, NULL, NULL, TO_NOTVICT );
        ch->short_descr = str_dup( "An auburn colored squirrel" );
        sprintf(buf, "An auburn colored squirrel");
        ch->shift = str_dup( buf );

        af.where     = TO_AFFECTS;
        af.type      = gsn_mutateform;
        af.level     = MUTATE_SQUIRREL;
        af.duration  = success * 15;
        af.location  = APPLY_CS_STR;
        af.modifier  = -1;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_DEX;
        af.modifier  = 2;
        af.bitvector = AFF_SNEAK;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_STA;
        af.modifier  = 2;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_PER;
        af.modifier  = 3;
        affect_to_char( ch, &af );

        return;
    }

    if ( !str_prefix( arg, "panther" ) )
    {
        act( "Your body grows sleeker, as you become a master hunter.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of a panther.", ch, NULL, NULL, TO_NOTVICT );
        ch->short_descr = str_dup( "A sleek gray panther" );
        sprintf(buf, "A sleek gray panther");
        ch->shift = str_dup( buf );

        af.where     = TO_AFFECTS;
        af.type      = gsn_mutateform;
        af.level     = MUTATE_PANTHER;
        af.duration  = success * 15;
        af.location  = APPLY_CS_STR;
        af.modifier  = 2;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_DEX;
        af.modifier  = 3;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_STA;
        af.modifier  = 2;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_MAN;
        af.modifier  = -3;
        affect_to_char( ch, &af );

        af.location  = APPLY_MOVE;
        af.modifier  = 5*ch->level;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_claws;
        af.level     = ch->level;
        af.duration  = success * 15;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        return;
    }

    if ( !str_prefix( arg, "bear" ) )
    {
        act( "With a mighty stretch, your body grows massive.", ch, NULL, NULL, TO_CHAR );
        act( "$n shifts $s form into that of a bear.", ch, NULL, NULL, TO_NOTVICT );
        ch->short_descr = str_dup( "A strong looking bear" );
        sprintf(buf, "A strong looking bear");
        ch->shift = str_dup( buf );

        af.where     = TO_AFFECTS;
        af.type      = gsn_mutateform;
        af.level     = MUTATE_BEAR;
        af.duration  = success * 15;
        af.location  = APPLY_CS_STR;
        af.modifier  = 3;
        af.bitvector = AFF_SHIFT;
        affect_to_char( ch, &af );

        af.location = APPLY_CS_STA;
        af.modifier = 3;
        affect_to_char( ch, &af );

        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_claws;
        af.level     = ch->level;
        af.duration  = success * 15;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        return;
    }

    if ( !str_prefix( arg, "sloth") )
    {
      act( "Your body shifts forms, and you feel very lethargic.", ch, NULL, NULL, TO_CHAR );
      act( "$n shifts their form into that of a sloth.", ch, NULL, NULL, TO_NOTVICT );
      ch->short_descr = str_dup( "A slow-moving sloth" );
      sprintf(buf, "A slow-moving sloth");
      ch->shift = str_dup( buf );

      af.where     = TO_AFFECTS;
      af.type      = gsn_mutateform;
      af.level     = MUTATE_SLOTH;
      af.duration  = success * 15;
      af.location  = APPLY_CS_STR;
      af.modifier  = -2;
      af.bitvector = AFF_SHIFT;
      affect_to_char( ch, &af );

      af.location  = APPLY_CS_DEX;
      af.modifier  = -4;
      af.bitvector = AFF_SLOW;
      affect_to_char( ch, &af );

      af.location  = APPLY_CS_MAN;
      af.modifier  = -3;
      af.bitvector = 0;
      affect_to_char( ch, &af );

      af.location  = APPLY_MOVE;
      af.modifier  = -ch->level;
      affect_to_char( ch, &af );

      return;
    }

    return;
}

void rote_cellularmitosis(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    CHAR_DATA *clone;
    AFFECT_DATA af;

    if(victim == ch)
    {
        send_to_char("You believe it would just be too... odd to have two of you around.\n\r",ch);
        return;
    }

    if(!IS_NPC(victim) || victim->level > ch->level || IS_SET(victim->act2, ACT2_ULTRA_MOB))
    {
        send_to_char("That being is too powerful to divide.\n\r",ch);
        return;
    }

    if (!living_being(victim))
    {
        send_to_char("[ After trying diligently, you find no {gLife{x Pattern to divide and replicate. ]\n\r", ch);
        return;
    }

    if(IS_SET(victim->in_room->room_flags,ROOM_LAW) || IS_SET(victim->in_room->room_flags, ROOM_SAFE))
    {
        send_to_char("You cannot do this here.\n\r",ch);
        return;
    }

    if (victim->pIndexData->pShop != NULL)
    {
        send_to_char("You fear that it may hinder your future purchases.\n\r",ch);
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char("You already have one loyal servant.\n\r",ch);
        return;
    }

    clone = create_mobile(victim->pIndexData);
    clone_mobile(victim,clone);
    char_to_room(clone,ch->in_room);

    if ( clone->master )
    stop_follower( clone );
    add_follower( clone, ch );
    clone->leader = ch;
    ch->pet = clone;
    SET_BIT(clone->act, ACT_PET);
    SET_BIT(clone->imm_flags,IMM_MENTAL);
    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = (success * 20) + 50;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( clone, &af );

    act("$n stares intently at $N, who shivers and shakes as if ill; a split appears as $e divides into two!",ch,NULL,clone,TO_NOTVICT);
    act("You examine $N's {gLife{x pattern; Forming one of your own, you overlay it on top of $m to ease the clone into this Reality.",ch,NULL,clone,TO_CHAR);

    if (success > 3)
    {
        act("Your Mastery of {gLife{x allows you to subtly alter $N's body with physical enhancements.",ch,NULL,clone,TO_CHAR);
    af.where     = TO_AFFECTS;
    af.type      = gsn_betterbody;
    af.level     = ch->level;
    af.duration  = (success * 10) + 50;
    af.location  = APPLY_CS_STR;
    af.modifier  = 1;
    af.bitvector = 0;
    affect_to_char( clone, &af );
    af.location  = APPLY_CS_DEX;
    affect_to_char( clone, &af );
    af.location  = APPLY_CS_STA;
    affect_to_char( clone, &af );
    }
    return;
}

void rote_matterperception(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA *paf;
    char buf[MAX_STRING_LENGTH];

    if (IS_NPC(ch)) return;

    WAIT_STATE(ch, 36);
    act( "You open your sight to try and decipher $p's Pattern.\n\r",  ch, obj, NULL, TO_CHAR );

    if (success < 1)
    {
        send_to_char("While staring into the Pattern, you lose track of time and focus.\n\r", ch);
        WAIT_STATE(ch, 36);
        return;
    }

    if (success == 0)
    {
        send_to_char("You try to study the Pattern, but your concentration wanes.\n\r", ch);
        return;
    }

    sprintf( buf, "{y[#-#-#-#-#-#      Matter Pattern: {W%s      {y#-#-#-#-#-#]\n\r{x", obj->short_descr);
    send_to_char( buf, ch );
    send_to_char( "{y#{x\n\r", ch );

    sprintf( buf,
        "{y#{x  Level:     %3d                Type:       %s\n\r{y#{x  Weight: %4d.%d lbs            Durability: %3d%%{x\n\r",
        obj->level, item_name(obj->item_type),
        obj->weight / 10, obj->weight % 10,  obj->condition
        );
    send_to_char( buf, ch );
    if (success > 1)
    {
        switch ( obj->item_type )
        {
            case ITEM_DRINK_CON:
                sprintf(buf,"{y#  {xIt holds %s-colored %s.\n\r",
                liq_table[obj->value[2]].liq_color,
                liq_table[obj->value[2]].liq_name);
                send_to_char(buf,ch);
            break;

            case ITEM_CONTAINER:
                sprintf(buf,"{y#  {xCapacity: %d#  Maximum weight: %d#  \n\r{y#  {xFlags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
                send_to_char(buf,ch);
                if (obj->value[4] != 100)
                {
                    sprintf(buf,"{y#  {xWeight multiplier: %d%%\n\r",
                    obj->value[4]);
                    send_to_char(buf,ch);
                }
            break;

            case ITEM_WEAPON:
                send_to_char("{y#  {xWeapon Type:  ",ch);
                switch (obj->value[0])
                {
                    case(WEAPON_EXOTIC) : send_to_char("Exotic\n\r",ch);   break;
                    case(WEAPON_SWORD)  : send_to_char("Sword\n\r",ch);    break;
                    case(WEAPON_DAGGER) : send_to_char("Dagger\n\r",ch);   break;
                    case(WEAPON_SPEAR)  : send_to_char("Spear\n\r",ch);  break;
                    case(WEAPON_MACE)   : send_to_char("Mace\n\r",ch);    break;
                    case(WEAPON_AXE)    : send_to_char("Axe\n\r",ch);      break;
                    case(WEAPON_FLAIL)  : send_to_char("Flail\n\r",ch);    break;
                    case(WEAPON_WHIP)   : send_to_char("Whip\n\r",ch);     break;
                    case(WEAPON_POLEARM): send_to_char("Polearm\n\r",ch);  break;
                    case(WEAPON_LANCE): send_to_char("Lance\n\r",ch);  break;
                default     : send_to_char("Unknown\n\r",ch);  break;
                }
                sprintf(buf, "{y#  {xD10 Damage Dice Bonus: {D({r+%d{D){x\n\r", obj->value[1] / 20);
                send_to_char(buf, ch);
            break;
            case ITEM_ARMOR:
                sprintf( buf,
                "{y#  {xArmor Class: %4d Pierce       %4d Bash\n\r{y#              {x %4d Slash        %4d Magic\n\r",
                obj->value[0], obj->value[1], obj->value[2], obj->value[3] );
                send_to_char( buf, ch );
            break;
            case ITEM_FOOD:
                sprintf( buf,
                "{y#  {xNourishment:  %3d             Poisoned: %s",
                obj->value[0], obj->value[3] != 0 ? "{RYes{x" : "{GNo{x\n\r");
                send_to_char( buf, ch );
            break;
            case ITEM_TOOL:
                sprintf( buf,
                "{y#  {xQuality:   %3d                Tool Type:  %s\n\r",
                obj->value[0], tool_table[obj->value[1]].tool_name);
                send_to_char( buf, ch );
            default:
            break;
        }
    }

    if (success > 2)
    {
      if ( IS_SET(obj->extra_flags, ITEM_NONMETAL) )
        send_to_char("{y#  {xThis item does not have any metallic properties.\n\r", ch);
      if ( IS_SET(obj->extra_flags, ITEM_BURN_PROOF) )
        send_to_char("{y#  {xThis item's Pattern is protected from burning.\n\r", ch);
      if ( IS_SET(obj->extra_flags, ITEM_IS_STAKE) )
        send_to_char("{y#  {xThis can be used as a stake.\n\r", ch);
      if ( IS_SET(obj->extra_flags, ITEM_IS_PRESERVE) )
        send_to_char("{y#  {xThis item has preservative properties.\n\r", ch);
    }

    send_to_char( "{y#{x\n\r", ch );
    sprintf( buf, "{y[#-#-#-#-#-#      Matter Pattern: {W%s      {y#-#-#-#-#-#]\n\r{x", obj->short_descr);
    send_to_char( buf, ch );
    return;
}

void rote_goldenwish(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (success <= 0)
    {
        send_to_char("You attempt to transform raw energy into solid matter, but the energy runs wild.\n\r", ch);
        d10_damage( ch, ch, -success, ch->level, gsn_magick, DAM_ENERGY, DEFENSE_NONE, TRUE, TRUE);
        return;
    }

    if (success == 0)
    {
        send_to_char("The Quintessence slips from your grasp, and dissipates back to the Tellurian.\n\r", ch);
        return;
    }

    ch->gold += 5 + success*10;

    act("$n holds out an empty hand, closes it, and then opens it to reveal golden coins..",ch,NULL,NULL,TO_NOTVICT);
    act("You form a Matter pattern, feed Quintessence into it and create golden coins.",ch,NULL,NULL,TO_CHAR);
    return;
}

void rote_watertowine(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if ( obj->item_type != ITEM_DRINK_CON )
    {
      act("It appears that $p is unable to hold liquids.", ch, obj, NULL, TO_CHAR);
      return;
    }

    if ( obj->value[1] == 0 )
    {
      act("The drink container is empty.  You need water to transmute.", ch, obj, NULL, TO_CHAR);
      return;
    }

    if ( obj->value[2] != LIQ_WATER )
    {
      act("The liquid in $p is not water.", ch, obj, NULL, TO_CHAR);
      return;
    }

    if (success < 0)
    {
      act("The energies you try to command seem to twist and rot.", ch, obj, NULL, TO_CHAR);
      obj->value[2] = LIQ_MUSCUS;
      return;
    }

    if (success == 0)
    {
      act("You feel the Quintessential energies rush through $p, and out into the Tellurian.", ch, obj, NULL, TO_CHAR);
      return;
    }

    act("You focus on $p, and smile as the water inside changes to sweet wine.", ch, obj, NULL, TO_CHAR);
    obj->value[2] = LIQ_BENEDICTINE;

    return;
}

void rote_opensesame(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    EXIT_DATA *pexit_rev;
    bool found = FALSE;
    int door;

    for ( door = 0; door <= 5; door++ )
    {
        found = FALSE;
        if ( ( pexit = ch->in_room->exit[door] ) != NULL
        &&   IS_SET(pexit->exit_info, EX_ISDOOR)
        &&   !IS_SET(pexit->exit_info, EX_PICKPROOF)
        )
        {
            REMOVE_BIT(pexit->exit_info, EX_HIDDEN );
            REMOVE_BIT(pexit->exit_info, EX_LOCKED);
            REMOVE_BIT(pexit->exit_info, EX_CLOSED);
            found = TRUE;
        }
        if ( found && ( to_room   = pexit->u1.to_room            ) != NULL
        &&   ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL
        &&   pexit_rev->u1.to_room == ch->in_room )
        {
            REMOVE_BIT( pexit_rev->exit_info, EX_HIDDEN );
            REMOVE_BIT( pexit_rev->exit_info, EX_LOCKED );
            REMOVE_BIT( pexit_rev->exit_info, EX_CLOSED );
        }
    }
    act("$n snaps their fingers and suddenly every door in the room pops open.",ch,NULL,NULL,TO_NOTVICT);
    act("You disassociate the patterns of the doors surrounding you from their locks which causes them to pop open.",ch,NULL,NULL,TO_CHAR);
    return;
}

void rote_quiltedform(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    OBJ_DATA  *clone;

    if(IS_SET(obj->extra_flags,ITEM_ROT_DEATH) || obj->cost > 999999
    || IS_SET(obj->extra_flags,ITEM_BURN_PROOF) || IS_SET(obj->extra_flags,ITEM_RARE)
    || IS_SET(obj->extra_flags,ITEM_SELL_EXTRACT)
    || obj->pIndexData->vnum == OBJ_VNUM_TOME)
    {
        send_to_char("That item is too powerful for you to duplicate.\n\r",ch);
        return;
    }

    clone = create_object(obj->pIndexData,0);
    clone_object(obj,clone);
    if (obj->carried_by != NULL)
        obj_to_char(clone,ch);
    else
        obj_to_room(clone,ch->in_room);
    SET_BIT(clone->extra_flags,ITEM_ROT_DEATH);
    SET_BIT(obj->extra_flags,ITEM_ROT_DEATH);
    act("$p slowly divides and replicates in the hands of $n",ch,obj,NULL,TO_NOTVICT);
    act("You examine the patterns of $p and replicate them, then feed Quintessence into it to bring it into this reality.",ch,obj,NULL,TO_CHAR);
    return;
}

void rote_refinematter(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if(IS_SET(obj->extra_flags, ITEM_IS_ENHANCED))
    {
        send_to_char("This item is already holding as much energy as it can. Perhaps the Oracles can do more, but you cannot.\n\r",ch);
        return;
    }

    if (!CAN_WEAR( obj, ITEM_WEAR_HEAD) && !CAN_WEAR( obj, ITEM_WEAR_TORSO)
      && !CAN_WEAR( obj, ITEM_WEAR_LEGS) && !CAN_WEAR( obj, ITEM_WIELD))
    {
        send_to_char( "You can only enhance weapons, headpieces, body armor, and leggings.\n\r", ch );
        return;
    }

    if (success < 0)
    {
      act("As you funnel Quintessence, your will falters and $p wavers slightly.",ch,obj,NULL,TO_CHAR);
      act("$n stares intently at $p which seems to shift and warp slightly.",ch,obj,NULL,TO_NOTVICT);

      af.where     = TO_OBJECT;
      af.type      = gsn_refinematter;
      af.level     = 0;
      af.duration  = 1;
      if (CAN_WEAR( obj, ITEM_WIELD))
      {
        af.location  = APPLY_DAMROLL;
        af.modifier = -50;
      }
      else
      {  
        af.location  = APPLY_AC;
        af.modifier  = 150;
      }  
      af.bitvector = ITEM_IS_ENHANCED;
      affect_to_obj(obj,&af);

      return;
    }

    if (success == 0)
    {
      act("The Quintessence you channel for $p's enhancement grounds itself, and dissipates into the world.",ch,obj,NULL,TO_CHAR);
      return;
    }

    if (CAN_WEAR(obj, ITEM_WIELD))
    {
      af.where     = TO_OBJECT;
      af.type      = gsn_refinematter;
      af.level     = success;
      af.duration  = 50 + (success*10);
      af.location  = APPLY_DAMROLL;
      af.modifier  = 20 * success;
      af.bitvector = ITEM_IS_ENHANCED;
      affect_to_obj(obj,&af);

      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector  = ITEM_MAGIC;
      affect_to_obj(obj,&af);
    }
    else
    {
      af.where     = TO_OBJECT;
      af.type      = gsn_refinematter;
      af.level     = success;
      af.duration  = 50 + (success*10);
      af.location  = APPLY_AC;
      af.modifier  = -30 * success;
      af.bitvector = ITEM_IS_ENHANCED;
      affect_to_obj(obj,&af);

      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector  = ITEM_MAGIC;
      affect_to_obj(obj,&af);
    }
    act("You pour raw Quintessence into $p's pattern, refining it into a stronger form.",ch,obj,NULL,TO_CHAR);
    act("$n stares intently at $p which seems to shift and warp slightly.",ch,obj,NULL,TO_NOTVICT);
    return;
}

void rote_empowerself(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_empower))
    {
        send_to_char("You are already empowering your mind to supernatural levels.\n\r",ch);
        return;
    }

    send_to_char("You enhance and open your mind to new possibilities.\n\r" ,ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_empower;
    af.level     = success;
    af.duration  = ch->avatar*success*5;
    af.location  = APPLY_CS_INT;
    af.modifier  = ch->sphere[SPHERE_MIND];
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.location  = APPLY_CS_WIT;
    affect_to_char( ch, &af );

    af.type      = gsn_mental_resilience;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    if (success > 4)
    {
        af.where     = TO_IMMUNE;
        af.bitvector = IMM_MENTAL;
    }
    else
    {
        af.where     = TO_RESIST;
        af.bitvector = RES_MENTAL;
    }
    affect_to_char( ch, &af );
    return;
}

void rote_subconsciousturmoil(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (IS_NPC(ch)) return;

    act( "You link with the mind of $N, dredging up memories of the most traumatic event in $S life.",  ch, NULL, victim, TO_CHAR    );
    act( "You suddenly have a flashback of the most traumatic event of your life.. the strain is agony.",  ch, NULL, victim, TO_VICT );
    act("$N flinches momentarily as a painful look crosses $s features.", ch, NULL, victim, TO_NOTVICT);

//    damage( ch, victim, success * ch->level * 2, gsn_magick, DAM_MENTAL, TRUE);
    d10_damage( ch, victim, success, ch->level * 2 / 3, gsn_magick, DAM_MENTAL, DEFENSE_NONE, TRUE, TRUE);

    return;
}

void rote_mentallink(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    if (IS_NPC(ch)) return;

    if(victim != ch)
    {
        send_to_char("You enact this effect by will, not by choice. Use it without a target.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_empower))
    {
        send_to_char("You are already empowering your mind to supernatural levels.\n\r",ch);
        return;
    }
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( !is_same_group( gch, ch ) || is_affected(gch,gsn_empower) || IS_NPC(gch) )
            continue;
        send_to_char( "Your mind reaches a clarity it has never before achieved.\n\r", gch );

        af.where     = TO_AFFECTS;
        af.type      = gsn_empower;
        af.level     = success;
        af.duration  = ch->avatar*success*5;
        af.location  = APPLY_CS_INT;
        af.modifier  = ch->sphere[SPHERE_MIND];
        af.bitvector = 0;
        affect_to_char( gch, &af );

        af.location  = APPLY_CS_WIT;
        affect_to_char( gch, &af );

    af.type      = gsn_mental_resilience;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    if (success > 4)
    {
        af.where     = TO_IMMUNE;
        af.bitvector = IMM_MENTAL;
    }
    else
    {
        af.where     = TO_RESIST;
        af.bitvector = RES_MENTAL;
    }
        affect_to_char( ch, &af );
    }
    send_to_char("Reaching out with your mind, you bestow the gifts of clarity and thought amongst your allies.\n\r" ,ch);
    return;
}

void rote_controlminds(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;
    CHAR_DATA *gch;
    int group_members = 0;

    if (IS_NPC(ch)) return;

    if(victim == NULL) return;

        if(ch->pet != NULL)
    {
        send_to_char( "You cannot command that many creatures at once!\n\r",ch );
        return;
    }
    for (gch = char_list; gch != NULL; gch = gch->next)
    {
        if (is_same_group(gch, ch) && gch != ch)
        group_members++;
    }

    if (group_members + 1 > ch->csabilities[CSABIL_LEADERSHIP] + 1)
    {
        send_to_char("Even if you subverted your target's mind, you cannot lead that many people.\n\r", ch);
        return;
    }

    if (IS_NPC(victim) && (victim->pIndexData->pShop != NULL))  /*Sengir - NO CHARMING SHOPKEEPERS! */
    {
        send_to_char("You decide against overriding the shopkeeper's mind.  Too many variables involved.\n\r", ch);
        return;
    }

    if (!IS_NPC(victim))
    {
        send_to_char("Charming players has been disabled due to possibility of abuse.\n\r", ch);
        return;
    }

    if (IS_AFFECTED(victim, AFF_CHARM)
    ||   IS_AFFECTED(ch, AFF_CHARM)
    ||   (ch->level+ch->avatar) < victim->level
    ||   IS_SET(victim->imm_flags,IMM_CHARM)
    ||   saves_spell( ch->level, victim,DAM_CHARM))
    {
        act( "You link with the mind of $N and attempt to compel them to your will but find their mind is too strong.",  ch, NULL, victim, TO_CHAR    );
        do_function(victim, &do_kill, ch->name );
        return;
    }

    if ( victim->master )
    stop_follower( victim );
    add_follower( victim, ch );
    victim->leader = ch;
    ch->pet = victim;
    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = ch->level;
    af.duration  = ch->level*ch->avatar;
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( victim, &af );
    act( "You suddenly feel an overwelming desire to follow the wishes of $n!", ch, NULL, victim, TO_VICT );
    if ( ch != victim )
    act("You link with the mind of $N and compel them to submit to your will.",ch,NULL,victim,TO_CHAR);
    act( "$n focuses on the mind of $N and $S eyes seem to glaze over.",  ch, NULL, victim, TO_NOTVICT );
    return;
}

void rote_primesense(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  AFFECT_DATA af;

  if(victim != ch)
  {
   send_to_char("You can only use this effect on yourself.\n\r",ch);
   return;
  }

  if(is_affected(ch,gsn_primesense))
  {
   send_to_char("The flow of Quintessential energy already permeates your vision.\n\r",ch);
   return;
  }

  af.where     = TO_AFFECTS;
  af.type      = gsn_primesense;
  af.level     = ch->sphere[SPHERE_PRIME];
  af.duration  = (success * 5) + 25;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = AFF_DETECT_MAGIC;
  affect_to_char( victim, &af );

  send_to_char("You open yourself up to the energy of the world, seeing Quintessence in all its glory.\n\r",ch);
  return;
}

void rote_quintessencebolt(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    if (IS_NPC(ch)) return;

    if (success < 0)
    {
      act("The {Yraw energy{x of Quintessence spins wildly out of control as you attempt to harness it!", ch, NULL, victim, TO_CHAR);
      act("A brief flash of energy erupts from $n, as $e shrieks in agony!", ch, NULL, victim, TO_NOTVICT);
//      damage( ch, ch, ch->level*2, gsn_magick, DAM_ENERGY, TRUE);
      d10_damage( ch, ch, -success, ch->level, gsn_magick, DAM_ENERGY, DEFENSE_NONE, TRUE, TRUE);
      ch->agg_dam += ch->level;
      paradox_check(ch,rote_table[rote_lookup("quintessence bolt")].vulgar);
      return;
    }

    if (success == 0)
    {
      act("You feel a brief burning sensation as the raw Quintessence slips out of your grasp, and back into the world.", ch, NULL, victim, TO_CHAR);
      paradox_check(ch,rote_table[rote_lookup("quintessence bolt")].vulgar);
      return;
    }

    act( "Pulling raw Quintessence from the surroundings, you hurl a pulsating bolt of {Ysupercharged energy{x at $N!",  ch, NULL, victim, TO_CHAR    );
    act( "$n hurls a blinding bolt of pure energy directly at $N!",  ch, NULL, victim, TO_NOTVICT );
    act( "A pulsing bolt of energy arcs from $n, ravaging your body to your very core!",  ch, NULL, victim, TO_VICT );

    if (success >= 4)
    {
        act( "You watch in satisfaction as the energy rips through $N's pattern!", ch, NULL, victim, TO_CHAR);
        act( "The energy rips its way through your body, causing unimaginable pain!", ch, NULL, victim, TO_VICT);
        victim->agg_dam += success * ch->level;
    }

//    damage( ch, victim, success*ch->level*3, gsn_magick, DAM_ENERGY, TRUE);
    d10_damage( ch, victim, success, ch->level, gsn_magick, DAM_ENERGY, DEFENSE_NONE, TRUE, TRUE);


    return;
}

void rote_imbueobject(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  AFFECT_DATA af;

  if (success < 0)
  {
    act("An excess amount of Quintessence rips through $p's Pattern, as it explodes!", ch, obj, NULL, TO_CHAR);
    act("With a ripple of {Yenergy{x, $p explodes suddenly!", ch, obj, NULL, TO_ROOM);
    extract_obj(obj);
    return;
  }

  if (success == 0)
  {
    act("The Quintessence you try to imbue into $p goes wild, and dissipates into the world.", ch, obj, NULL, TO_CHAR);
    return;
  }

  if (IS_OBJ_STAT(obj, ITEM_MAGIC))
  {
    act("You funnel more Quintessence into $p than it can hold, causing it to explode!", ch, obj, NULL, TO_CHAR);
    act("With a blast of energy, $p explodes violently!", ch, obj, NULL, TO_ROOM);
    extract_obj(obj);
    return;
  }

  act("You funnel Quintessence into $p, charging it with Primal {Yenergy{x.", ch, obj, NULL, TO_CHAR);

  af.where     = TO_OBJECT;
  af.type      = gsn_imbueobject;
  af.level     = success;
  af.duration  = 50 + (success * 15);
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = ITEM_MAGIC;
  affect_to_obj(obj,&af);

  return;
}

void rote_wellspring(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  AFFECT_DATA af;

  if (success < 0)
  {
    send_to_char("Depleting your own reserves, the Quintessence from your body wildly streams into the universe!\n\r", ch);
    ch->quintessence = 0;

    af.where     = TO_AFFECTS;
    af.type      = gsn_wellspring;
    af.level     = -success;
    af.duration  = 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
  }

  if (success == 0)
  {
    send_to_char("You cannot seem to still your mind long enough to try and absorb any ambient Quintessence.\n\r", ch);
    return;
  }

  if (ch->in_room->sector_type != SECT_NODE)
  {
    send_to_char("You must be in a place of Quintessential power to draw in ambient energy!\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_wellspring))
  {
    send_to_char("You cannot shunt such a large amount of Quintessence back into your body so soon.\n\r", ch);
    return;
  }

  act("Closing your eyes, you command the energies of the area to flow into yourself, replenishing your Quintessence.", ch, NULL, NULL, TO_CHAR);
  act("$n closes $s eyes, seemingly at peace for a moment.", ch, NULL, NULL, TO_NOTVICT);

  ch->quintessence = ch->max_quintessence - ch->paradox;

  af.where     = TO_AFFECTS;
  af.type      = gsn_wellspring;
  af.level     = success;
  af.duration  = 100 - (success * 5);
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  return;
}

void rote_paradoxward(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  AFFECT_DATA af;

  if (success < 0)
  {
    send_to_char("Reality lashes back upon you, as a wave of Paradox fills your Pattern!\n\r", ch);
    ch->paradox += 5;
    if (ch->quintessence + ch->paradox > ch->max_quintessence)
      ch->quintessence = ch->max_quintessence - ch->paradox;
    paradox_check(ch, TRUE);
    return;
  }

  if (success == 0)
  {
    send_to_char("You strive to resist the effects of Paradox, but it is not so easily staved off.\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_paradoxward))
  {
    send_to_char("You have already bulwarked yourself against Paradox.  Don't push your luck.\n\r", ch);
    return;
  }

  send_to_char("You feel a layer of Primal energy covering your Pattern, resisting the inevitability of Paradox.\n\r", ch);

  af.where     = TO_AFFECTS;
  af.type      = gsn_paradoxward;
  af.level     = success;
  af.duration  = 15 + (success *3);
  af.location  = APPLY_NONE;
  af.modifier  = success * 2;
  af.bitvector = 0;
  affect_to_char( ch, &af );

  return;
}

void rote_channelquintessence(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  if (success < 0)
  {
    act("A surge of {Ywild Quintessence{x rips through $p's Pattern, destroying it.", ch, obj, NULL, TO_CHAR);
    act("With a bright flash, $p explodes in a ball of {Yenergy{x.", ch, obj, NULL, TO_NOTVICT);
    if (obj->contains != NULL)
    {
      OBJ_DATA *in, *in_next;

      act("The contents that were contained within $p spill to the ground.",ch,obj,NULL,TO_ROOM);
      act("You watch as the contents formerly contained within $p spill to the ground.",ch,obj,NULL,TO_CHAR);
      for (in = obj->contains; in != NULL; in = in_next)
      {
        in_next = in->next_content;
        obj_from_obj(in);
        obj_to_room(in,victim->in_room);
      }
    }

    extract_obj(obj);
    return;
  }

  if (success == 0)
  {
    act("You try to draw raw Quintessence from $p, but the energy remains in place.", ch, obj, NULL, TO_CHAR);
    return;
  }

  if(!IS_OBJ_STAT(obj,ITEM_MAGIC))
  {
    act("There doesn't seem to be any mystical energy to pull from $p.", ch, obj, NULL, TO_CHAR);
    return;
  }

  act("Weaving and tugging at the raw energy stored in $p, you absorb it into yourself.", ch, obj, NULL, TO_CHAR);
  act("In a flash of color, $p dissipates into {Yenergy{x and leaves nothing but motes of light behind.", ch, obj, NULL, TO_ROOM);

  ch->quintessence += UMIN(success * 5, obj->level / 2);
  if (ch->quintessence > ch->max_quintessence)
    ch->quintessence = ch->max_quintessence;

  if (obj->contains != NULL)
  {
    OBJ_DATA *in, *in_next;

    act("The contents that were contained within $p spill to the ground.",ch,obj,NULL,TO_ROOM);
    act("You watch as the contents formerly contained within $p spill to the ground.",ch,obj,NULL,TO_CHAR);
    for (in = obj->contains; in != NULL; in = in_next)
    {
      in_next = in->next_content;
      obj_from_obj(in);
      obj_to_room(in,victim->in_room);
    }
  }

  extract_obj(obj);

  return;
}

void rote_spiritsight(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch, gsn_spiritsight) || IS_AFFECTED2(ch, AFF2_UMBRA))
    {
        send_to_char("You are already viewing the etheral plane of existance.\n\r",ch);
        return;
    }

    send_to_char("You expand your vision to see the etheral plane of existance.\n\r" ,ch);

    af.where     = TO_AFFECTS;
    af.type      = gsn_spiritsight;
    af.level     = ch->level;
    af.duration  = (success * 5) + 25;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
}

void rote_callspirit(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA *mob;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(ch->pet != NULL)
    {
        send_to_char( "You cannot control two creatures at once!\n\r",ch );
        return;
    }
    if ( (pMobIndex = get_mob_index(MOB_VNUM_MORTIS_SPIRIT)) == NULL )
    {
        send_to_char( "Error: please infrorm the Coders!\n\r", ch );
        return;
    }

    act( "Concentrating, you call out across the Gauntlet and deep into the Umbra.", ch, NULL, mob, TO_CHAR );
    mob = create_mobile( pMobIndex );

    if (success < 0)
    {
      char_to_room( mob, ch->in_room );
      mob->level  = ch->level;
      mob->max_hit = ch->max_hit / 4;
      mob->hitroll = ch->hitroll;
      mob->damroll = ch->damroll;
      mob->hit = mob->max_hit;
      mob->short_descr = str_dup("a ravenous spectre");
      mob->long_descr = str_dup("Lured from the depths of the Tempest, a shadowy spectre lunges for its prey.\n\r");
      mob->name = str_dup("ravenous spectre");

      act("A shadowy form rips through the Gauntlet and into existence, and it turns immediately upon you!", ch, NULL, NULL, TO_CHAR);
      act("Seemingly out of nowhere, a shadowy figure emerges and immediately attacks $n!", ch, NULL, NULL, TO_NOTVICT);
      act("Shrieking, $N says, '{DYour soul will be a feast for Oblivion!{x'", ch, NULL, mob, TO_ROOM);
      multi_hit( mob, ch, TYPE_UNDEFINED );
      return;
    }

    if (success == 0)
    {
      act("Your call seems to have gone unanswered, as nothing responds from across the Gauntlet.", ch, NULL, NULL, TO_CHAR);
      WAIT_STATE(ch, 9);
      return;
    }

    char_to_room( mob, ch->in_room );

    act( "A disruption in the Gauntlet appears before you, and $N tears through into this reality.", ch, NULL, mob, TO_CHAR );
    act( "With eyes seemingly boring into your soul, $E rasps, '{WI have answered, what is thy bidding?{x'", ch, NULL, mob, TO_CHAR);
    act( "With a look of anguish and pain, $N materializes before your very eyes!", ch, NULL, mob, TO_NOTVICT );
    act( "In a raspy voice, $E looks at $n and says, '{WI have answered, what is they bidding?{x'", ch, NULL, mob, TO_NOTVICT);

    add_follower( mob, ch );
    mob->leader = ch;
    mob->level  = ch->level;
    ch->pet = mob;
    mob->max_hit = ch->max_hit / 2;
    mob->hitroll = ch->hitroll;
    mob->damroll = ch->damroll * 2 / 3;
    mob->hit = mob->max_hit;

    af.where     = TO_AFFECTS;
    af.type      = gsn_charm_person;
    af.level     = (ch->level + ch->sphere[SPHERE_SPIRIT]);
    af.duration  = 15 + (8 * success);
    af.location  = 0;
    af.modifier  = 0;
    af.bitvector = AFF_CHARM;
    affect_to_char( mob, &af );
    return;
}

void rote_stepsideways(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj_next;
    OBJ_DATA *bag;
    OBJ_DATA *bag_next;
    AFFECT_DATA af;
    int gauntlet_diff = get_gauntlet(ch);
    int success_needed;
    int avatar_storm = godice(ch->paradox + ch->arete, 6);

    if ( ( location = get_room_index( ROOM_VNUM_PARADOX ) ) == NULL )
    {
        send_to_char( "Error! Contact the imms at once!\n\r", ch );
        return;
    }

    if (ch->position == POS_FIGHTING)
  	{
  		send_to_char("Your concentration was broken because you are fighting!\n\r", ch);
  		return;
  	}

    switch(gauntlet_diff)
  	{
  		default:
  			success_needed = 6;
  			break;
  		case 0:
  		case 1:
  		case 2:
  		case 3:
  			success_needed = 1;
  			break;
  		case 4:
  		case 5:
  			success_needed = 2;
  			break;
  		case 6:
  			success_needed = 3;
  			break;
  		case 7:
  			success_needed = 4;
  			break;
  		case 8:
  		case 9:
  		case 10:
  			success_needed = 5;
  			break;
  	}

    if (avatar_storm < 0)
    		avatar_storm = 0;

    if(success < 0)
    {
        act( "You start to step into the Umbra... and suddenly feel a tugging sensation as you're caught within the Gauntlet!",  ch, NULL, NULL, TO_CHAR    );
        act( "A vague look of terror crosses $n's features before they suddenly cease to exist in this reality.",  ch, NULL, NULL, TO_NOTVICT );

        if (!IS_AFFECTED2(ch, AFF2_UMBRA))
    			pass_gauntlet(ch, FALSE);

    		af.where	=	TO_AFFECTS;
    		af.type		=	gsn_trappedingauntlet;
    		af.level	=	gauntlet_diff;
    		af.duration	=	(success_needed) / 2 + 1;
    		af.modifier	=	0;
    		af.location	=	0;
    		af.bitvector	=	0;
    		affect_to_char( ch, &af );

    		d10_damage(ch, ch, avatar_storm, ch->level + (ch->avatar * 5), gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);
    		return;
    	}

      if (success < success_needed)
    	{
    		act( "You calm your mind and focus on the spirit realm, but meet resistance in the Gauntlet.",  ch, NULL, NULL, TO_CHAR    );
    		act( "$n wavers slightly before your eyes, before becoming whole once again.",  ch, NULL, NULL, TO_NOTVICT );
    		WAIT_STATE(ch, (success_needed - success)*3);
    		d10_damage(ch, ch, avatar_storm, ch->level + (ch->avatar * 5), gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);
    		return;
    	}

      if (pass_gauntlet(ch, FALSE))
    	{
    		act( "Concentrating intently, you feel a tingling sensation as your physical body navigates the Gauntlet to the spirit realm on the other side.",  ch, NULL, NULL, TO_CHAR    );
    		act( "$n's body becomes slowly translucent, before $s melts into thin air.",  ch, NULL, NULL, TO_NOTVICT );
    		WAIT_STATE(ch, (success_needed - success)*3);
        send_to_char("{y***************{Y**************************{y***************{x\n\r", ch);
        send_to_char("The energies of the {yGauntlet{x flay apart your Avatar!\n\r", ch);
    		d10_damage(ch, ch, avatar_storm, (ch->level / 5) + ch->avatar * 5, gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);
        send_to_char("{y***************{Y**************************{y***************{x\n\r", ch);
        send_to_char("\n\r", ch);
    		do_function(ch, &do_look, "auto");
    		return;
    	}
}

void rote_controlgauntlet(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *location;
    OBJ_DATA *rift, *rift2;
    AFFECT_DATA af;
    int gauntlet_diff = get_gauntlet(ch);
    int success_needed;
    int avatar_storm = godice(ch->paradox + ch->arete, 6);

    if (avatar_storm < 0)
  		avatar_storm = 0;

    if ( ( location = get_room_index( ROOM_VNUM_PARADOX ) ) == NULL )
    {
        send_to_char( "Error! Contact the imms at once!\n\r", ch );
        return;
    }

    if (ch->position == POS_FIGHTING)
  	{
  		send_to_char("Your concentration was broken because you are fighting!\n\r", ch);
  		return;
  	}

    switch(gauntlet_diff)
  	{
  		default:
  			success_needed = 6;
  			break;
  		case 0:
  		case 1:
  		case 2:
  		case 3:
  			success_needed = 1;
  			break;
  		case 4:
  		case 5:
  			success_needed = 2;
  			break;
  		case 6:
  			success_needed = 3;
  			break;
  		case 7:
  			success_needed = 4;
  			break;
  		case 8:
  		case 9:
  		case 10:
  			success_needed = 5;
  			break;
  	}

    if (success < 0)
    {
      act( "You begin to weave and pull to pierce the Gauntlet, but get trapped between realms instead!",  ch, NULL, NULL, TO_CHAR    );
      act( "A vague look of terror crosses $n's features before they suddenly cease to exist in this reality.",  ch, NULL, NULL, TO_NOTVICT );

      if (!IS_AFFECTED2(ch, AFF2_UMBRA))
    		pass_gauntlet(ch, FALSE);

    	af.where	=	TO_AFFECTS;
    	af.type		=	gsn_trappedingauntlet;
    	af.level	=	gauntlet_diff;
    	af.duration	=	(success_needed) / 2 + 1;
    	af.modifier	=	0;
    	af.location	=	0;
    	af.bitvector	=	0;
    	affect_to_char( ch, &af );

    	d10_damage(ch, ch, avatar_storm, ch->level + (ch->avatar * 5), gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);
    	return;
    }

    if (success < success_needed)
    {
      act( "Tugging at the Gauntlet, you meet with resistance and energy feedback.",  ch, NULL, NULL, TO_CHAR    );
      act( "$n weaves and waves $s hands, staring into the distance with a blank look on $s face.",  ch, NULL, NULL, TO_NOTVICT );
      WAIT_STATE(ch, (success_needed - success)*3);
      d10_damage(ch, ch, avatar_storm, ch->level + (ch->avatar * 5), gsn_magick, DAM_NEGATIVE, DEFENSE_NONE, TRUE, TRUE);
      return;
    }

    rift = create_object(get_obj_index(OBJ_VNUM_RIFT),0);
    rift->timer = success;
    rift->value[3] = victim->in_room->vnum;
    obj_to_room(rift,ch->in_room);
    rift2 = create_object(get_obj_index(OBJ_VNUM_RIFT),0);
    SET_BIT(rift2->extra_flags, ITEM_UMBRA);
    rift2->timer = success;
    rift2->value[3] = victim->in_room->vnum;
    obj_to_room(rift2, ch->in_room);

    if (IS_AFFECTED2(ch, AFF2_UMBRA))
    {
      act("$n begins peeling away reality, revealing $p across the realms.",ch,rift2,NULL,TO_ROOM);
      act("You peel away layers from reality, piercing the Gauntlet via $p.",ch,rift2,NULL,TO_CHAR);
    }
    else
    {
      act("$n begins peeling away reality, revealing $p across the realms.",ch,rift,NULL,TO_ROOM);
      act("You peel away layers from reality, piercing the Gauntlet via $p.",ch,rift,NULL,TO_CHAR);
    }
    return;
}

void rote_umbralrescue(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    ROOM_INDEX_DATA *location;

    if ( ( location = get_room_index( ROOM_VNUM_PARADOX ) ) == NULL )
    {
        send_to_char( "Error! Contact the imms at once!\n\r", ch );
        return;
    }
    if ( ch->in_room == location )
    {
        send_to_char("You cannot rescue yourself from the Umbra!\n\r",ch);
        return;
    }
    if ( victim->in_room != location )
    {
        send_to_char("You cannot rescue someone who is not trapped!\n\r",ch);
        return;
    }

    if(success < get_gauntlet(ch))
    {
        send_to_char("You've failed to extract them from the Umbra.\n\r",ch);
        return;
    }
    char_from_room( victim );
    char_to_room( victim, ch->in_room );
    do_function(victim, &do_look, "auto" );
    act( "You open a hole in reality and $N steps forth!",  ch, NULL, victim, TO_CHAR    );
    act( "A hole in reality opens before you and you are sucked in!",  ch, NULL, victim, TO_VICT    );
    act( "$N is drawn into this reality by $n!",  ch, NULL, victim, TO_NOTVICT );
    return;
}

void rote_timesense(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
  AFFECT_DATA af;

  if(victim != ch)
  {
   send_to_char("You can only use this effect on yourself.\n\r",ch);
   return;
  }

  if(is_affected(ch,gsn_timesense))
  {
   send_to_char("You are already observing the ebb and flow of time.\n\r",ch);
   return;
  }

  af.where     = TO_AFFECTS;
  af.type      = gsn_timesense;
  af.level     = ch->sphere[SPHERE_TIME];
  af.duration  = (success * 5) + 25;
  af.location  = APPLY_NONE;
  af.modifier  = 0;
  af.bitvector = 0;
  affect_to_char( victim, &af );

  send_to_char("You attune yourself to the flow of time, taking note of its alterations.\n\r",ch);
  return;
}

void rote_precognition(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_precognition))
    {
        send_to_char("You are already more aware of the flow of Time.\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_precognition;
    af.level     = success;
    af.duration  = (5 * success) + 15;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char("You open your eyes to all the possibilities in the stream of time.\n\r",ch);
    return;
}

void rote_timealteration(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_timealteration))
    {
        send_to_char("You are already altering the flow of time around your body, to do much more is to risk the Quintessence leaking from your Pattern.\n\r",ch);
        return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_timealteration;
    af.level     = ch->sphere[SPHERE_TIME];
    af.duration  = (success*5) + 10;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    send_to_char("You dilate the fabric of time around your body allowing you to appear to move more swiftly.\n\r",ch);
    return;
}

void rote_stoptheclock(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if (success < 0)
    {
      act( "With a flash of realization, your body freezes in place as the temporal energies surround you.",  ch, NULL, victim, TO_CHAR );
      act( "$n begins to gesture, but suddenly freezes in place!",  ch, NULL, victim, TO_NOTVICT );

      if (!is_affected(ch, gsn_stoptheclock))
      {
        af.where     = TO_AFFECTS;
        af.type      = gsn_stoptheclock;
        af.level     = -1;
        af.duration  = 1;
        af.location  = APPLY_NONE;
        af.modifier  = 2;
        af.bitvector = 0;
        affect_to_char( ch, &af );
      }
      return;
    }

    if (success < 0)
    {
      send_to_char("The temporal energies you try to nudge refuse to stop at your command.\n\r", ch);
      return;
    }

    if(is_affected(victim, gsn_stoptheclock))
    {
      act( "You direct temporal energies towards $N, but $E seems to have already been frozen in the time stream.",  ch, NULL, victim, TO_CHAR );
      return;
    }

    act( "$n waves a hand at $N who suddenly seems frozen!",  ch, NULL, victim, TO_NOTVICT    );
    act( "You suddenly find time rushing by too fast to perceive.",  ch, NULL, victim, TO_VICT );
    act( "You feel the strain of the world bending to your whim as you stop the flow of time itself around $N.", ch, NULL, victim, TO_CHAR );

    af.where     = TO_AFFECTS;
    af.type      = gsn_stoptheclock;
    af.level     = success;
    af.duration  = 1;
    af.location  = APPLY_NONE;
    af.modifier  = success + 2;
    af.bitvector = 0;
    affect_to_char( victim, &af );
    return;
}

void rote_sidesteptime(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_sidesteptime))
    {
        send_to_char("You carefully reinsert your Pattern back into the flow of Time.\n\r",ch);
        act("$n suddenly appears, as if from nowhere.", ch, NULL, NULL, TO_ROOM);
        affect_strip(ch, gsn_sidesteptime);
        return;
    }

    if (success < 0)
    {
      send_to_char("You feel an intense backlash as Time moves forward, leaving you frozen in place.\n\r", ch);
      ch->paradox += 10;
      WAIT_STATE(ch, 20);
      return;
    }

    if (success == 0)
    {
      send_to_char("The Pattern of Time refuses to budge, leaving you in the current Time Stream.\n\r", ch);
      WAIT_STATE(ch, 5);
      return;
    }

    if (ch->position == POS_FIGHTING)
    {
      stop_fighting( ch, TRUE );
      act("With a tug on the Pattern of Time, you remove yourself from its flow, and combat as well.", ch, NULL, NULL, TO_CHAR);
    }
    else
      send_to_char("You impose your Will onto the timeline, and step completely out of its natural flow.\n\r",ch);

    act("$n suddenly disappears from existence.", ch, NULL, NULL, TO_ROOM);

    af.where     = TO_AFFECTS;
    af.type      = gsn_sidesteptime;
    af.level     = success;
    af.duration  = success * 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( victim, &af );

    return;
}

//Combination Rotes

void rote_camouflagediscernment(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
    {
        send_to_char("You are already aware of everything around you.\n\r",ch);
        return;
    }

    if (success < 0)
    {
      af.where     = TO_VULN;
      af.type      = gsn_camouflagediscernment;
      af.level     = -1;
      af.duration  = 2;
      af.location  = APPLY_NONE;
      af.modifier  = 0;
      af.bitvector = VULN_MENTAL;
      affect_to_char( victim, &af );
      send_to_char("The Quintessence reverberates through you, jarring your brain.\n\r",ch);

      return;
    }

    if (success == 0)
    {
      send_to_char("The flow of Quintessence leaves your body, but your senses seem to be unaffected.\n\r", ch);
      return;
    }

    af.where     = TO_AFFECTS;
    af.type      = gsn_camouflagediscernment;
    af.level     = success;
    af.duration  = (success * 5) + 25;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_DETECT_HIDDEN;
    affect_to_char( victim, &af );
    send_to_char("Focusing on thoughts nearby, you pinpoint life forms hiding in the room.\n\r",ch);
    return;
}

void rote_negationfield(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
   AFFECT_DATA af;

    if(victim != ch)
    {
        send_to_char("You can only use this effect on yourself.\n\r",ch);
        return;
    }

    if(is_affected(ch,gsn_negationfield))
    {
        send_to_char("You are already protecting yourself from the energies of others.\n\r",ch);
        return;
    }

    stop_fighting( ch, TRUE );

    af.where     = TO_RESIST;
    af.type      = gsn_negationfield;
    af.level     = success;
    af.duration  = 15 + success;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = RES_MAGIC;

    if (success > 4)
    {
      af.where    = TO_IMMUNE;
      af.duration = 7 + success;
      af.bitvector = IMM_MAGIC;
    }

    affect_to_char( victim, &af );

    send_to_char("You summon about yourself a field of negative energy to protect yourself from the magicks of others.\n\r",ch);
    return;
}

void rote_sluggishspeed(CHAR_DATA *ch, int success, CHAR_DATA *victim, OBJ_DATA *obj)
{
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    if (success < 0)
    {
      act( "The temporal energies go awry, instead causing your own body to slow horribly!",  ch, NULL, victim, TO_CHAR );
      act( "$n's movements seem to slow down greatly.",  ch, NULL, victim, TO_NOTVICT );

      if (!is_affected(ch, gsn_sluggishspeed))
      {
        af.where     = TO_AFFECTS;
        af.type      = gsn_sluggishspeed;
        af.level     = -1;
        af.duration  = 1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = AFF_SLOW;
        affect_to_char( ch, &af );
      }
      return;
    }

    if (success < 0)
    {
      send_to_char("You begin to nudge the Patterns of Time, but they refuse to slow.\n\r", ch);
      return;
    }

    if(is_affected(victim, gsn_sluggishspeed) || IS_AFFECTED(victim, AFF_SLOW))
    {
      act( "You direct temporal energies towards $N, but $E is already moving slowly.",  ch, NULL, victim, TO_CHAR );
      return;
    }

    act( "$n waves a hand at $N, who seems to slow down drastically.",  ch, NULL, victim, TO_NOTVICT    );
    act( "Your body slows down, refusing to move as you normally expect.",  ch, NULL, victim, TO_VICT );
    act( "Gesturing to $N, you command the flow of Time to slow down around $m.", ch, NULL, victim, TO_CHAR );

    af.where     = TO_AFFECTS;
    af.type      = gsn_sluggishspeed;
    af.level     = success;
    af.duration  = success + 2;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_SLOW;
    affect_to_char( victim, &af );

    if (success > 4)
    {
      af.location  = APPLY_CS_DEX;
      af.modifier  = -1;
      af.bitvector = 0;
      affect_to_char( victim, &af );
    }
    return;
}