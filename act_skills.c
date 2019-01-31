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
        if ( ( victim = get_char_room( ch, arg ) ) == NULL)     /*If victim specified is not in the room*/
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


    if( ch->pcdata->csabilities[CSABIL_MEDICINE]<1)     /*First thing it checks for, if you have Medicine knowledge.*/
    {
        send_to_char("Without the proper knowledge in Medicine, you might make the wound worse.\n\r", ch);
        return;
    }

    if(ch->move <= (100-(ch->pcdata->csattributes[STAMINA]*10)))    /*Checks your movement, possible ranges are 90-50*/
    {                   /*Higher CS_Stam, lower movement cost*/
        send_to_char("You are too tired to bandage someone.\n\r", ch);
        return;
    }

    extract_obj(obj);

    act("You attempt to bandage $N...",ch,NULL,victim,TO_CHAR);     /*If all checks pass, go through the*/
    act("$n attempts to bandage your wounds...",ch,NULL,victim,TO_VICT);    /*motions of attempting to bandage*/
    act("$n attempts to bandage $N...",ch,NULL,victim,TO_NOTVICT);      /*before making subsequent checks*/

    dicepool = (ch->pcdata->csattributes[INTELLIGENCE]+ch->pcdata->csabilities[CSABIL_MEDICINE]);   /*Calculates dicepool, CS_Int +CS_Med*/
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
        duration = 2;
    }
    else if (dicesuccess < 0)               /*Checks to see if total successes are negative*/
    {                   /*meaning rolled more 1's than successes*/
        act("and only succeed in making the wound worse!",ch,NULL,victim,TO_CHAR);      /*Botching*/
        act("and only succeeds in making the wound worse!",ch,NULL,victim,TO_VICT);
        act("and only succeeds in making the wound worse!",ch,NULL,victim,TO_NOTVICT);
        duration = 1;

        if (victim->hit - (60 - (10*ch->pcdata->csabilities[CSABIL_MEDICINE]) ) <= 0)
        {               /*If botch would kill victim, instead sets to hp 1*/
            victim->hit = 1;
            return;
        }

        victim->hit -= 60-(10*ch->pcdata->csabilities[CSABIL_MEDICINE]);
                /*^^^---Damage is 60 minus10*Medicine rating*/
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

            if (victim->race != race_lookup('garou'))
                victim->hit += dicesuccess*ch->level+7;
            else
                victim->hit += (dicesuccess*ch->level/2)+10;

            if (ch->pcdata->csabilities[CSABIL_MEDICINE] >= 4)   /*If Medicine 4 or higher, heals a little agg damage*/
            {
                victim->agg_dam -= dicesuccess;
                if (victim->agg_dam < 0)
                    victim->agg_dam=0;
            }

            if (ch->pcdata->csabilities[CSABIL_MEDICINE] >= 3)   /*Bonus heal for higher medicine*/
                victim->hit += dicesuccess*ch->level/2;

            if (victim->hit > victim->max_hit)
                victim->hit = victim->max_hit; /*Max HP check */
        }
    }
    ch->move -= (90-ch->pcdata->csattributes[STAMINA]*10);  /*Movement cost = 100 minus CS_Stam*10, range*/
                                /*is from 90 - 50, higher CS_Stam, lower cost*/

    /*All this following has to do with what you see in the affects on your char*/
    af.where = TO_AFFECTS;      /*I am unsure what this does exactly...*/
    af.type = gsn_bandage;      /*Sets affect as bandage*/
    af.level = ch->pcdata->csabilities[CSABIL_MEDICINE];        /*Affect level = Medicine lvl*/
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

void do_vigor(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if(ch->mana < 50)
    {
        send_to_char("You do not have enough mana.\n\r", ch);
        return;
    }

    if(IS_AFFECTED(ch,AFF_HASTE))
    {
        send_to_char("You are already moving as quickly as possible.\n\r", ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_vigor].beats );
    if( get_skill(ch,gsn_vigor) < number_percent())
    {
        send_to_char( "You fail to enhanced your speed.\n\r", ch );
        return;
        check_improve(ch,gsn_vigor,FALSE,4);
    }

    ch->mana -= 50;
    af.where     = TO_AFFECTS;
    af.type      = gsn_vigor;
    af.level     = ch->level;
    af.duration  = ch->level/4;
    af.modifier  = ch->max_move;
    af.location  = APPLY_MOVE;
    af.bitvector = AFF_HASTE;
    affect_to_char( ch, &af );

    af.where     = TO_AFFECTS;
    af.type      = gsn_vigor;
    af.level     = ch->level;
    af.duration  = ch->level/4;
    af.modifier  = 10;
    af.location  = APPLY_HITROLL;
    affect_to_char( ch, &af );
    send_to_char( "You feel your speed and stamina increasing!\n\r", ch );
    act("$n huffs and puffs and suddenly seems to be moving much quicker.",ch,NULL,ch,TO_NOTVICT);
    check_improve(ch,gsn_vigor,TRUE,4);
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

    if ( ( victim = get_char_room( ch, arg ) ) == NULL)
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

