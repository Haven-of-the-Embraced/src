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


void do_setdhampire(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;


    argument = one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
        send_to_char( "Set who as a Dhampire?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "Your future childe is not here.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char( "Not on NPCs!\n\r", ch );
        return;
    }

    if (victim->race == race_lookup("dhampire"))
    {
        send_to_char( "They are already set as a Dhampire\n\r", ch );
        return;
    }
    victim->race = race_lookup("dhampire");
    victim->practice = 5;
    victim->train = 3;
    victim->level    = 1;
    victim->max_hit  = 10;
    victim->max_mana = 100;
    victim->max_move = 100;
    victim->pcdata->perm_hit = victim->max_hit;
    victim->pcdata->perm_mana = victim->max_mana;
    victim->pcdata->perm_move = victim->max_move;
    victim->hit      = victim->max_hit;
    victim->mana     = victim->max_mana;
    victim->move     = victim->max_move;
    victim->gen = 0;
    victim->dpoints = 1;
    victim->sire = str_dup("Unknown");
    victim->pblood = 100;
    victim->affected_by = race_table[victim->race].aff;
    send_to_char( "Dhampire set.\n\r", ch );
    send_to_char( "You have been reset to level 1 and turned into a Dhampire!\n\r", victim );
    victim->auspex = 2;
    victim->celerity = 3;
    victim->potence = 3;
    victim->fortitude = 3;
    victim->protean = 0;
    victim->obfuscate = 0;
    victim->obtenebration = 0;
    victim->domination = 0;
    victim->presence = 0;
    victim->mortis = 0;
    victim->quietus = 0;
    victim->thaumaturgy = 0;
    victim->animalism = 0;
    victim->exp      = exp_per_level(victim,victim->pcdata->points);
    for ( obj = victim->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if(obj->wear_loc != WEAR_NONE)
        unequip_char( victim, obj );
    }

    while ( victim->affected )
        affect_remove( victim, victim->affected );
    victim->affected_by = race_table[victim->race].aff;
    save_char_obj(victim);
    return;
}

void do_dhampire(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    if(ch->race != race_lookup("dhampire"))
    {
        send_to_char("You are not a Dhampire!\n\r" ,ch);
        return;
    }
    sprintf(buf, "You are the Childe of %s.\n\r", ch->sire);
    send_to_char(buf,ch);
    sprintf(buf, "You have {r%d{x tainted vitae in your system..\n\r", ch->pblood/10);
    send_to_char(buf,ch);
    sprintf(buf, "You have {W%d{x Discipline Points to spend.\n\r", ch->dpoints);
    send_to_char(buf,ch);
    if (IS_AFFECTED(ch,AFF_FANGS))
    {
        send_to_char("Your fangs are extended.\n\r" ,ch);
    }
    return;
}

void do_sanctus(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;
    int dicesuccess = 0;

    if (IS_NPC(ch)) return;

    if (ch->clan != clan_lookup("watcher"))
    {
        send_to_char("Huh?\n\r", ch);
        return;
    }
    if ( is_affected( ch, gsn_sanctus ) )
    {
        send_to_char("You have already asked for Divine protection from the supernatural.\n\r", ch);
        return;
    }
    if (ch->mana < ch->level)
    {
        send_to_char("You don't have enough mana.\n\r", ch);
        return;
    }

    WAIT_STATE( ch, skill_table[gsn_sanctus].beats );

    dicesuccess = godice(ch->pcdata->cshumanity, 6);

    if (dicesuccess < 0)
    {
        send_to_char("You begin praying to Heaven for protection, and feel a fiery pain in your chest!\n\r", ch);
        act("$n mutters a few words but suddenly yelps, clutching at $s chest!", ch, NULL, ch, TO_NOTVICT);
        damage(ch, ch, (ch->level * 5), gsn_sanctus, DAM_HOLY, TRUE);
        WAIT_STATE( ch, 8);
        return;
    }

    if (dicesuccess == 0)
    {
        send_to_char("You finish your prayers to Heaven, but feel strangely empty.\n\r", ch);
        act("$n finishes a quick benediction, but looks around suspiciously.", ch, NULL, ch, TO_NOTVICT);
        check_improve(ch, gsn_sanctus, FALSE, 1);
        WAIT_STATE( ch, 10);
        return;
    }


    ch->mana -= ch->level;

    af.where     = TO_AFFECTS;
    af.type      = gsn_sanctus;
    af.level     = ch->pcdata->cshumanity;
    af.duration  = dicesuccess * 5;
    af.modifier  = 0;
    af.location  = APPLY_NONE;
    af.bitvector = 0;
    affect_to_char( ch, &af );

    af.where    = TO_RESIST;
    af.type     = gsn_sanctus;
    af.level    = ch->pcdata->cshumanity;
    af.duration = dicesuccess * 5;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector    = RES_HOLY;
    affect_to_char( ch, &af );

    send_to_char( "You pray to Heaven to protect you from the darkness dwelling in the world, quietly uttering the words 'Sanctus Lucis'.\n\r", ch );
    act("$n kneels and quietly mutters 'Sanctus Lucis' to $Sself, absorbed in prayer.",ch,NULL,ch,TO_NOTVICT);
    check_improve(ch,gsn_sanctus,TRUE,1);

    gain_exp(ch, dicesuccess * 4);
    return;
}


void do_endow(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if(ch->clan != clan_lookup("watcher"))
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }
    if (IS_OBJ_STAT(obj,ITEM_BLESS))
    {
        send_to_char( "This object is already endowed with a blessing.\n\r", ch );
        return;
    }


    act("You endow $p with a holy blessing.",ch,obj,NULL,TO_CHAR);
    act("$p is endowed with a holy blessing.",ch,obj,NULL,TO_NOTVICT);
    spell_bless( skill_lookup("bless"), ch->level/2-6, ch, obj,TARGET_OBJ);

    return;
}

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
        else if(victim->race == race_lookup("garou"))
        {               /*Check for garou, bandaging doesn't really help their already accelerated healing*/
            act("and manage to place a clean bandage just as the wounds begin to heal.",ch,NULL,victim,TO_CHAR);
            act("and manages to place the bandage just as your accelerated healing kicks in, rendering the bandage pointless.",ch,NULL,victim,TO_VICT);
            act("and manages to place a bandage on $N.",ch,NULL,victim,TO_NOTVICT);
        }
        else
        {               /*Bandages victim if not garou/vamp/meth*/
            act("and manage to staunch the bleeding from the wound.",ch,NULL,victim,TO_CHAR);
            act("and manages to stop the blood flowing from your wounds.",ch,NULL,victim,TO_VICT);
            act("and manages to stop the bleeding.",ch,NULL,victim,TO_NOTVICT);

            victim->hit += dicesuccess*ch->level+7;

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
//New regen code
void do_vampire_regen(CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;

    if((ch->race != race_lookup("vampire")) & (ch->race != race_lookup("ghoul")))
    {
        send_to_char( "You are not a vampire!\n\r", ch );
        return;
    }
    if ( IS_AFFECTED2( ch, AFF2_VAMPIRE_REGEN ))
    {
        send_to_char( "You are already regenerating.\n\r", ch );
        return;
    }
    if(ch->position > POS_SITTING)
    {
        send_to_char( "You cannot do this standing!\n\r", ch );
        return;
    }

    act( "$n begins to regenerate their body.",  ch, NULL, NULL, TO_ROOM );
    send_to_char( "You begin to regenerate your body.\n\r", ch );

    af.where    = TO_AFFECTS2;
    af.type     = gsn_vampire_regen;
    af.level    = ch->level;
    af.duration = -1;
    af.modifier = 0;
    af.location = APPLY_NONE;
    af.bitvector    = AFF2_VAMPIRE_REGEN;
    affect_to_char( ch, &af );
}
// Regen code

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
        check_improve(ch,gsn_chant,FALSE,1);
        return;
    }

    act( "$n sits down and starts to chant to $s God.",  ch, NULL, NULL, TO_ROOM );
    send_to_char( "You start to chant to your God.\n\r", ch );
    check_improve(ch,gsn_chant,TRUE,1);

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
        check_improve(ch,gsn_vigor,FALSE,1);
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
    check_improve(ch,gsn_vigor,TRUE,1);
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
    check_improve(ch,gsn_waylay,TRUE,1);
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
        check_improve(ch,gsn_fortress,FALSE,1);
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
    check_improve(ch,gsn_fortress,TRUE,1);
    return;
}

/* Depulso.. Bonum Scire.. meant to push a mob outta the room */
/* void do_depulso(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    AFFECT_DATA *af;
    int direction;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );


    if(ch->race != race_lookup("human"))
    {
        send_to_char("You are not pure enough to use this Scire.\n\r" ,ch);
        return;
    }

    if(ch->alignment >= 350 || ch->alignment <= -350)
    {
        send_to_char( "You do not have the favor of Nullus.\n\r", ch );
        return;
    }
    if((victim = get_char_room( ch, arg )) == NULL)
    {
        send_to_char( "Who?\n\r", ch );
        return;
    }

    if(number_range(1,100) <= 50)
    {
        send_to_char( "You fail to shift this creature.\n\r", ch );
        return;
    }


    if ( !str_prefix( arg2, "north" ) ) direction = 0;
    if ( !str_prefix( arg2, "south" ) ) direction = 1;
    if ( !str_prefix( arg2, "east" ) ) direction = 2;
    if ( !str_prefix( arg2, "west" ) ) direction = 3;
    if ( !str_prefix( arg2, "up" ) ) direction = 4;
    if ( !str_prefix( arg2, "down" ) ) direction = 5;

    act("You focus the energies of Nullus towards $N...",ch,NULL,victim,TO_CHAR);
    act("$n focuses on $N...",ch,NULL,victim,TO_NOTVICT);

    switch(direction)
    {
        case (0): do_function(victim, &do_north, ""); break;
        case (1): do_function(victim, &do_south, ""); break;
        case (2): do_function(victim, &do_east, ""); break;
        case (3): do_function(victim, &do_west, ""); break;
        case (4): do_function(victim, &do_up, ""); break;
        case (5): do_function(victim, &do_down, ""); break;
        default: act("You fail to channel the energies of Nullus properly!",ch,NULL,victim,TO_CHAR);
                act("And fails to move $N!",ch,NULL,victim,TO_NOTVICT); break;
    }


    af.where    = TO_AFFECTS;
    af.type = gsn_scire;
    af.level    = ch->level;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);

    return;
}


void do_translatum( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    AFFECT_DATA *af;
    ROOM_INDEX_DATA *location;

    if (IS_NPC(ch)) return;

    act("You focus the energies of Nullus.",ch,NULL,victim,TO_CHAR);
    act("$n focuses inward...",ch,NULL,victim,TO_NOTVICT);

    if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
        send_to_char( "You are completely lost.\n\r", ch );
        return;
    }

    if ( ch->in_room == location )
        return;

    REMOVE_BIT(ch->act,PLR_ARENA);

    act( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    char_from_room( ch );
    char_to_room( ch, location );
    act( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_function(ch, &do_look, "auto" );

    if (ch->pet != NULL)
        do_function(ch->pet, &do_recall, "");

    af.where    = TO_AFFECTS;
    af.type = gsn_scire;
    af.level    = ch->level;
    af.duration = 24;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = 0;
    affect_to_char(ch,&af);

    return;
}

*/
/*Garou abilities-Nongift*/
void do_changeform(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA af;

    if (IS_NPC(ch)) return;

    argument = one_argument( argument, arg );

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("You are not one of Gaia's chosen!\n\r",ch);
        return;
    }

    if(ch->pcdata->rank != 0)
    {
        send_to_char("Your a new garou! Use shapeshift!\n\r",ch);
        return;
    }

    if ( arg[0] == '\0')
    {
        send_to_char("Syntax: changeform <form>\n\rWhere <form> is either Homid, Crinos or Lupus.\n\r",ch);
        return;
    }
    if ( is_affected( ch, gsn_claws ) )
    {
        do_function(ch, &do_claws, " ");
        affect_strip(ch, gsn_gift_razorclaws);
    }

    if ( !str_prefix( arg, "homid" ))
    {
        if(ch->changed == CHANGED_HOMID)
        {
            send_to_char("You are already in Homid form.\n\r",ch);
            return;
        }
        send_to_char( "You change back into Homid form.\n\r", ch );

            affect_strip(ch,gsn_change);
            affect_strip(ch,gsn_gift_porcupine);
        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
/*      ch->affected_by = race_table[ch->race].aff; */
        ch->changed = CHANGED_HOMID;
        REMOVE_BIT(ch->affected_by,AFF_SHIFT);
        act( "$n's form slowly changes into that of a human.", ch, NULL, NULL, TO_NOTVICT );
        return;
    }

    if ( !str_prefix( arg, "crinos" ) )
    {
        if(ch->changed == CHANGED_CRINOS)
        {
            send_to_char("You are already in Crinos form.\n\r",ch);
            return;
        }

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
                unequip_char( ch, obj );
        }
            affect_strip(ch,gsn_change);

        ch->changed = CHANGED_CRINOS;

        if ( !is_affected( ch, gsn_claws ) )
            do_function(ch, &do_claws, " ");


        act( "Your body slowly changes forms into that of a half wolf beast.", ch, NULL, NULL, TO_CHAR );
        act( "$n lets forth a blood curdling scream of rage as $s form slowly changes into a giant half wolf man-beast!", ch, NULL, NULL, TO_NOTVICT );
/* disabled to reduce lag... maybe - Ugha
        if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
            send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/werewolf.wav>",ch,TO_ROOM);
*/
        af.where     = TO_AFFECTS;
        af.type      = gsn_change;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = APPLY_HIT;
        af.modifier  = 10*ch->level;
        af.bitvector = 0;
        affect_to_char( ch, &af );
/*Sengir strikes again.  CS mods for garou forms */

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_STR;
        af.modifier = 4;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_DEX;
        af.modifier = 1;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_STA;
        af.modifier = 3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_APP;
        af.modifier = -(get_attribute(ch,APPEARANCE));
        af.bitvector    = 0;
        affect_to_char( ch, &af );

/*
        if (ch->position == POS_FIGHTING)
        {
            send_to_char( "The strain of changing forms during combat causes you to loose grip on your {RRage{x.\n\r", ch );
            ch->rage += 10;
            if(ch->rage > 100) ch->rage = 100;
        }
        else
            if(ch->rage < 100) ch->rage += 1;
*/
        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        sprintf(buf, "A large half-wolf beast (%s)", ch->name);
        ch->shift = str_dup(buf);
        ch->short_descr = str_dup(buf);

/*      ch->short_descr = str_dup( "A large half-wolf beast" );
            ch->shift = str_dup( "A large half-wolf beast" );
*/

        REMOVE_BIT(ch->affected_by,AFF_SHIFT);
        return;
    }

    if ( !str_prefix( arg, "lupus" ) )
    {
        if(ch->changed == CHANGED_LUPUS)
        {
            send_to_char("You are already in Lupus form.\n\r",ch);
            return;
        }

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE)
            unequip_char( ch, obj );
        }
            affect_strip(ch,gsn_change);

        ch->changed = CHANGED_LUPUS;

        if ( !is_affected( ch, gsn_claws ) )
            do_function(ch, &do_claws, " ");

        act( "Your body slowly changes forms into that of a wolf.", ch, NULL, NULL, TO_CHAR );
        act( "$n form slowly changes into that of a wolf.", ch, NULL, NULL, TO_NOTVICT );
/* disabled to reduce lag... maybe - Ugha
        if(!IS_NPC(ch) && IS_SET(ch->act,PLR_PUEBLO))
            send_html("<img xch_sound=play href=http://haven.wolfpaw.net/pueblo/sounds/werewolf2.wav>",ch,TO_ROOM);
*/
        af.where     = TO_AFFECTS;
        af.type      = gsn_change;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = APPLY_HIT;
        af.modifier  = 5*ch->level;
        af.bitvector = AFF_DARK_VISION;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_MOVE;
        af.modifier = 5*ch->level;
        af.bitvector    = 0;
        affect_to_char( ch, &af );
/* Sengir strike!  Lupus CS mods */

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_STR;
        af.modifier = 1;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_DEX;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_STA;
        af.modifier = 2;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_change;
        af.level    = ch->level;
        af.duration = -1;
        af.location = APPLY_CS_MAN;
        af.modifier = -3;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

/*
        if (ch->position == POS_FIGHTING)
        {
            send_to_char( "The strain of changing forms during combat causes you to loose grip on your {RRage{x.\n\r", ch );
            ch->rage += 10;
            if(ch->rage > 100) ch->rage = 100;
        }
        else
            if(ch->rage < 100) ch->rage += 1;
*/
        if (ch->hit > ch->max_hit) ch->hit = ch->max_hit;
        sprintf(buf, "A large timber wolf (%s)", ch->name);
        ch->shift = str_dup(buf);
        ch->short_descr = str_dup(buf);
/*
        ch->short_descr = str_dup( "A large timber wolf" );
            ch->shift = str_dup( "A large timber wolf" );
*/

        REMOVE_BIT(ch->affected_by,AFF_SHIFT);
        return;
    }
send_to_char("Syntax: changeform <form>\n\rWhere <form> is either Homid, Crinos or Lupus.\n\r",ch);
return;
}

void garou_frenzy( int sn, int level, CHAR_DATA *ch, void *vo, int target )
{
  AFFECT_DATA af;
  CHAR_DATA *tch;
  char buf[MAX_STRING_LENGTH];
  bool fdone = FALSE;

    if (IS_NPC(ch)) return;
    if ( is_affected( ch, gsn_garou_frenzy ) )
    {
        act( "$n snarls as $e enters a frenzy!",  ch, NULL, NULL, TO_NOTVICT );
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
            act( "$n throws $sself into a frenzy and attacks $N!",  ch, NULL, tch, TO_NOTVICT );
            sprintf(buf, "Frenzy overwelms you and you attack %s!\n\r", tch->name);
            send_to_char(buf,ch);
            multi_hit( ch, tch, TYPE_UNDEFINED );
        }
    }

    af.where     = TO_AFFECTS2;
    af.type      = gsn_garou_frenzy;
    af.level     = ch->level;
    af.duration  = ch->level/10;
    af.location  = APPLY_HITROLL;
    af.modifier  = -100;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
}

void do_fury( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;

    if (IS_NPC(ch))
        return;

    if(ch->pcdata->rank != 0)
    {
        send_to_char("Your a new Garou! You can't use this command!\n\r",ch);
        return;
    }

    if(ch->changed != CHANGED_CRINOS)
    {
        send_to_char("You do not have the proper form to do this.\n\r",ch);
        return;
    }

    if (ch->rage < 9)
    {
        send_to_char("You do not have enough {RRage{x to unleash!\n\r", ch);
        return;
    }

    if ( ( victim = ch->fighting ) == NULL )
    {
        send_to_char( "You aren't fighting anyone to unleash your fury upon!\n\r", ch );
        return;
    }

    WAIT_STATE( ch, 16 );
    ch->rage -= 6;

    act("You focus your Gaia driven {RRage{x onto $N, and lash out with savage fury!", ch, NULL, victim, TO_CHAR);
    act("$n unleashes a brutal series of savage attacks against you!", ch, NULL, victim, TO_VICT);
    act("$n mercilessly strikes out at $N, attacking swiftly and repeatedly.", ch, NULL, victim, TO_NOTVICT);

    multi_hit( ch, victim, TYPE_UNDEFINED );
    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


/*Mage convert from Dhamp/Mage*/
void do_gomage(CHAR_DATA *ch, char *argument)
{
    char arg [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch))
        return;

    if(ch->clan != clan_lookup("mage") || ch->avatar > 0)
    {
        send_to_char("This command is only meant to convert old mages into new mages.\n\r",ch);
        return;
    }
    if ( arg[0] == '\0')
    {
        send_to_char( "You must choose either Mage or Dhampire. You can not be both.\n\r", ch );
        return;
    }
    if (!str_prefix( arg, "mage" ))
    {
        if(arg2[0] == '\0' || (ch->tradition = tradition_lookup(arg2)) == 0)
        {
            send_to_char("You must choose a valid Tradition! Traditions are:\n\rAkashic Brotherhood, Dreamspeakers, Order of Hermes, Euthanatos and Verbena.\n\r",ch);
            return;
        }
        ch->race = race_lookup("human");
        if(ch->avatar == 0) ch->avatar = 1;
        if(ch->arete == 0) ch->arete = 1;
        ch->quintessence = ch->max_quintessence = 100;
        ch->sphere[tradition_table[ch->tradition].sphere]++;
        ch->dpoints = 5+((ch->remorts*3)/4);
        ch->auspex = 0;
        ch->celerity = 0;
        ch->potence = 0;
        ch->fortitude = 0;
        ch->protean = 0;
        ch->obfuscate = 0;
        ch->obtenebration = 0;
        ch->domination = 0;
        ch->presence = 0;
        ch->mortis = 0;
        ch->quietus = 0;
        ch->thaumaturgy = 0;
        ch->animalism = 0;
        ch->gen = 0;
        ch->size = pc_race_table[ch->race].size;
        ch->exp = exp_per_level(ch,ch->pcdata->points) * UMAX( 1, ch->level );
        while ( ch->affected )
            affect_remove( ch, ch->affected );
        ch->affected_by = race_table[ch->race].aff;
        save_char_obj(ch);

        send_to_char("You have Awakened your Avatar!\n\r",ch);
        return;
    }
    else if(!str_prefix(arg, "dhampire"))
    {
        if(ch->race != race_lookup("dhampire"))
        {
            send_to_char("This option is only available to Dhampires.\n\r",ch);
            return;
        }
        ch->clan = 0;
        ch->avatar = 0;
        ch->arete = 0;
        send_to_char("You are now a normal Dhampire.\n\r",ch);
        return;
    }
    else
    {
        send_to_char("Invalid option.\n\r",ch);
        return;
    }
    return;
}

