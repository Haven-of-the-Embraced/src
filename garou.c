#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "interp.h"

bool    shiftform   args((CHAR_DATA *ch, int desired, bool rage));

void do_shapeshift(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int desired;
    bool rage = FALSE;

    if (IS_NPC(ch)) return;

    if(ch->pcdata->rank == 0)
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("You are not one of Gaia's chosen!\n\r",ch);
        return;
    }

    if ( arg[0] == '\0')
    {
        send_to_char("Syntax: shapeshift <form>\n\rWhere <form> is either Homid, Glabro, Crinos, Hispo or Lupus.\n\r",ch);
        return;
    }
    if(ch->pcdata->shift_count)
    {
        send_to_char("You are already shapeshifting.\n\r",ch);
        return;
    }
    if(!str_cmp(arg,"shift_count")) desired = ch->pcdata->shift_to;
    else if(!str_prefix(arg, "Homid")) desired = HOMID;
    else if(!str_prefix(arg, "Glabro")) desired = GLABRO;
    else if(!str_prefix(arg, "Crinos")) desired = CRINOS;
    else if(!str_prefix(arg, "Hispo")) desired = HISPO;
    else if(!str_prefix(arg, "Lupus")) desired = LUPUS;
    else
    {
        send_to_char("That is not a form you can shift into.\n\r",ch);
        return;
    }
    if(arg2[0] != '\0' && !str_prefix(arg2,"Rage")) rage = TRUE;

    if(desired == ch->pcdata->shiftform)
    {
        send_to_char("You are already in that form.\n\r",ch);
        return;
    }
    if(!shiftform(ch, desired, rage)) return;

    if(ch->pcdata->shiftform != desired)
    {
        ch->pcdata->shift_to = desired;
        ch->pcdata->shift_count = 1;
    }
    return;
}

bool shiftform(CHAR_DATA *ch, int desired, bool rage)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    AFFECT_DATA af;
    char buf[MAX_STRING_LENGTH];

    int tobe;

    if(rage)
    {
        if(ch->pcdata->rage[TEMP] == 0)
        {
            send_to_char("You do not have enough Rage\n\r",ch);
            return FALSE;
        }
        else ch->pcdata->rage[TEMP]--;
    }

    if(desired > ch->pcdata->shiftform) tobe = ch->pcdata->shiftform+1;
    else tobe = ch->pcdata->shiftform-1;

    if(ch->pcdata->breed != desired && godice(get_attribute(ch,STAMINA)+ch->pcdata->primal_urge,garou_form_table[tobe].diff) < 1)
    {
        send_to_char("You fail to shift forms.\n\r",ch);
        WAIT_STATE(ch,PULSE_VIOLENCE*3);
        return FALSE;
    }

    if (is_affected(ch, gsn_gift_mansskin))
    {
        affect_strip(ch, gsn_gift_mansskin);
        send_to_char("Shifting forms causes your male guise to drop.\n\r", ch);
    }

    switch(tobe)
    {
        case HOMID:
                act( "You assume the form of Man.", ch, NULL, NULL, TO_CHAR );
                act( "$n's form shrinks to that of a normal human.", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = HOMID;
            break;
        case GLABRO:
                act( "You assume the form of the Near-Man.", ch, NULL, NULL, TO_CHAR );
                act( "$n's form alters to that of a large, feral looking human.", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = GLABRO;
            break;
        case CRINOS:
                act( "You force your body into your War Form!", ch, NULL, NULL, TO_CHAR );
                act( "$n growls viciously as $s form shifts and grows into a towering half-wolf killing machine!", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = CRINOS;
            break;
        case HISPO:
                act( "You assume the form of the Near-Wolf.", ch, NULL, NULL, TO_CHAR );
                act( "$n's jaws enlongate as $e assumes the form of a huge wolf.", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = HISPO;
            break;
        case LUPUS:
                act( "You assume the form of the Wolf.", ch, NULL, NULL, TO_CHAR );
                act( "$n let's out a howl as $s form shifts into that of a wolf.", ch, NULL, NULL, TO_NOTVICT );
                ch->pcdata->shiftform = LUPUS;
            break;
    }
    if(ch->pcdata->breed != desired || !rage)   WAIT_STATE(ch,PULSE_VIOLENCE);

    affect_strip(ch,gsn_change);
    affect_strip(ch, gsn_claws);


    if(ch->pcdata->shiftform != HOMID)
    {
        af.where     = TO_AFFECTS;//Give them claws in any form but homid.
        af.type      = gsn_claws;
        af.level     = ch->level;
        af.duration  = -1;
        af.location  = APPLY_NONE;
        af.modifier  = 0;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.where     = TO_AFFECTS;
        af.type      = gsn_change;
        af.level     = 0;
        af.duration  = -1;
        af.location  = APPLY_CS_STR;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].strength;
        if(ch->pcdata->shiftform >= CRINOS)
            af.bitvector = AFF_DARK_VISION;
        else
            af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_DEX;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].dexterity;
        af.bitvector = AFF_INFRARED;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_STA;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].stamina;
        if(ch->pcdata->shiftform > CRINOS)
            af.bitvector = AFF_DETECT_HIDDEN;
        else
            af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_APP;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].appearance;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_MAN;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].manipulation;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        af.location  = APPLY_CS_PER;
        af.modifier  = garou_form_table[ch->pcdata->shiftform].perception;
        af.bitvector = 0;
        affect_to_char( ch, &af );

        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_content;
            if(obj->wear_loc != WEAR_NONE && !can_wear_in_form(ch,obj)) unequip_char( ch, obj );
        }

        switch(ch->pcdata->shiftform)
        {
            case CRINOS: sprintf(buf, "A half-wolf beast with %s fur",
                ch->pcdata->garou_fur == NULL ? "brown" : !str_cmp(ch->pcdata->garou_fur, "(null)") ? "brown" : ch->pcdata->garou_fur, ch->name); break;
            case HISPO: sprintf(buf, "A vicious looking wolf with %s fur",
                ch->pcdata->garou_fur == NULL ? "brown" : !str_cmp(ch->pcdata->garou_fur, "(null)") ? "brown" : ch->pcdata->garou_fur, ch->name); break;
            case LUPUS: sprintf(buf, "A large wolf with %s fur",
                ch->pcdata->garou_fur == NULL ? "brown" : !str_cmp(ch->pcdata->garou_fur, "(null)") ? "brown" : ch->pcdata->garou_fur, ch->name); break;
            default: sprintf(buf, "%s", ch->name); break;
        }

        ch->long_descr = str_dup(buf);
        ch->short_descr = str_dup(buf);
    }
    return TRUE;
}

void do_gogarou(CHAR_DATA *ch, char *argument)
{
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int i;



    if (IS_NPC(ch))
        return;

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("This command is only meant to convert old garou into new garou.\n\r",ch);
        return;
    }
    if(ch->pcdata->rank > 0)
    {
        send_to_char("You've already been through this process.\n\r",ch);
        return;
    }

    if(ch->pcdata->breed == 0)
    {
        if(argument[0] == '\0')
        {
            send_to_char("{g      -=[You must now pick a Breed]=-{x\n\rValid Breeds:\n\r    Homid, Metis and Lupus.\n\rSee the helpfile for each before deciding.\n\r{r    *{xType gogarou <breed> to continue.\n\r",ch);
            return;
        }
        if(!str_prefix( argument, "homid" ))
        {
            send_to_char("Alright. You are now a Homid Garou.\n\r",ch);
            ch->pcdata->breed = HOMID;
            ch->pcdata->gnosis[PERM] = 1;
            ch->pcdata->gnosis[TEMP] = 1;
        }
        else if(!str_prefix(argument, "metis"))
        {
            send_to_char("Alright. You are now a Metis Garou.\n\r",ch);
            ch->pcdata->breed = METIS;
            ch->pcdata->gnosis[PERM] = 3;
            ch->pcdata->gnosis[TEMP] = 3;
        }
        else if(!str_prefix(argument, "lupus"))
        {
            send_to_char("Alright. You are now a Lupus Garou.\n\r",ch);
            ch->pcdata->breed = LUPUS;
            ch->pcdata->gnosis[PERM] = 5;
            ch->pcdata->gnosis[TEMP] = 5;
        }
        else
        {
            send_to_char("That's not a valid breed. Please choose a Breed.\n\r",ch);
            return;
        }
        send_to_char("{g      -=[You must now pick an Auspice]=-{x\n\rValid Auspices:\n\r    Ragabash, Theurge, Philodox, Galliard, and Ahroun.\n\rSee the helpfile for each before deciding.\n\r{r    *{xType gogarou <auspice> to continue.\n\r",ch);
        return;
    }
    if(ch->pcdata->auspice == 0)
    {
        if(argument[0] == '\0')
        {
            send_to_char("You must pick an Auspice. Valid Auspices are Ragabash, Theurge, Philodox, Galliard and Ahroun.\n\rSee the helpfile for each before deciding.\n\rType gogarou <auspice> to continue.\n\r",ch);
            return;
        }
        if(!str_prefix( argument, "ragabash" ))
        {
            send_to_char("Alright. Your Auspice is Ragabash.\n\r",ch);
            ch->pcdata->rage[PERM] = 1;
            ch->pcdata->rage[TEMP] = 1;
            ch->pcdata->renown[WISDOM] = 1;
            ch->pcdata->renown[HONOR] = 1;
            ch->pcdata->renown[GLORY] = 1;
            ch->pcdata->auspice = RAGABASH;
        }
        else if(!str_prefix( argument, "theurge"))
        {
            send_to_char("Alright. Your Auspice is Theurge.\n\r",ch);
            ch->pcdata->rage[PERM] = 2;
            ch->pcdata->rage[TEMP] = 2;
            ch->pcdata->renown[WISDOM] = 3;
            ch->pcdata->auspice = THEURGE;
        }
        else if(!str_prefix( argument, "philodox"))
        {
            send_to_char("Alright. Your Auspice is Philodox.\n\r",ch);
            ch->pcdata->rage[PERM] = 3;
            ch->pcdata->rage[TEMP] = 3;
            ch->pcdata->renown[HONOR] = 3;
            ch->pcdata->auspice = PHILODOX;
        }
        else if(!str_prefix( argument, "galliard"))
        {
            send_to_char("Alright. Your Auspice is Galliard.\n\r",ch);
            ch->pcdata->rage[PERM] = 4;
            ch->pcdata->rage[TEMP] = 4;
            ch->pcdata->renown[GLORY] = 2;
            ch->pcdata->renown[WISDOM] = 1;
            ch->pcdata->auspice = GALLIARD;
        }
        else if(!str_prefix( argument, "ahroun"))
        {
            send_to_char("Alright. Your Auspice is Ahroun.\n\r",ch);
            ch->pcdata->rage[PERM] = 5;
            ch->pcdata->rage[TEMP] = 5;
            ch->pcdata->renown[GLORY] = 2;
            ch->pcdata->renown[HONOR] = 1;
            ch->pcdata->auspice = AHROUN;
        }
        else
        {
            send_to_char("That is not a valid Auspice.\n\r",ch);
            return;
        }
        send_to_char("{g      -=[You must now pick a Tribe]=-{x\n\rValid Tribes:\n\r Black Furies, Bone Gnawers, Children of Gaia, Fenrir, Fianna, Red Talons, \n\r Shadow Lords, Silent Striders, Silver Fangs and Warders of Men.\n\rSee the helpfile for each before deciding.\n\r{r    *{xType gogarou <tribe> to continue.\n\r",ch);
        return;
    }
    if(ch->pcdata->tribe == 0)
    {
        if(argument[0] == '\0')
        {
            send_to_char("You must pick a Tribe. Valid Tribes are Black Furies, Bone Gnawers, Children of Gaia, Fenrir,\n\rFianna, Red Talons, Shadow Lords, Silent Striders, Silver Fangs and Warders of Men.\n\rSee the helpfile for each before deciding.\n\rType gogarou <tribe> to continue.\n\r",ch);
            return;
        }
        if(!str_prefix( argument, "black furies") || !str_prefix( argument, "black fury"))
        {
            if (ch->pcdata->true_sex != 2)
            {
                send_to_char("You must be a true female to join the Black Furies.\n\r", ch);
                return;
            }
            send_to_char("Alright. You are now a Black Fury.\n\r",ch);
            ch->pcdata->tribe = BLACK_FURY;
            ch->clan = clan_lookup("blackfury");
        }
        else if(!str_prefix( argument, "bone gnawers") || !str_prefix( argument, "bone gnawer"))
        {
            send_to_char("Alright. You are now a Bone Gnawer.\n\r",ch);
            ch->pcdata->tribe = BONE_GNAWER;
            ch->clan = clan_lookup("bonegnawer");
        }
        else if(!str_prefix( argument, "children of gaia") || !str_prefix( argument, "child of gaia"))
        {
            send_to_char("Alright. You are now a Child of Gaia.\n\r",ch);
            ch->pcdata->tribe = CHILDREN_OF_GAIA;
            ch->clan = clan_lookup("childrenofgaia");
        }
        else if(!str_prefix( argument, "fenrir") || !str_prefix( argument, "get of fenris") || !str_prefix( argument, "getoffenris"))
        {
            send_to_char("Alright. You are now a Fenrir.\n\r",ch);
            ch->pcdata->tribe = FENRIR;
            ch->clan = clan_lookup("fenrir");
        }
        else if(!str_prefix( argument, "fianna"))
        {
            send_to_char("Alright. You are now a Fianna.\n\r",ch);
            ch->pcdata->tribe = FIANNA;
            ch->clan = clan_lookup("fianna");
        }
        else if(!str_prefix( argument, "red talons") || !str_prefix( argument, "red talon"))
        {
            send_to_char("Alright. You are now a Red Talon.\n\r",ch);
            ch->pcdata->tribe = RED_TALON;
            ch->clan = clan_lookup("redtalon");
        }
        else if(!str_prefix( argument, "shadow lords") || !str_prefix( argument, "shadow lord"))
        {
            send_to_char("Alright. You are now a Shadow Lord.\n\r",ch);
            ch->pcdata->tribe = SHADOW_LORD;
            ch->clan = clan_lookup("shadowlord");
        }
        else if(!str_prefix( argument, "silent striders") || !str_prefix( argument, "silent strider"))
        {
            send_to_char("Alright. You are now a Silent Strider.\n\r",ch);
            ch->pcdata->tribe = SILENT_STRIDER;
            ch->clan = clan_lookup("silentstrider");
        }
        else if(!str_prefix( argument, "silver fangs") || !str_prefix( argument, "silver fang"))
        {
            send_to_char("Alright. You are now a Silver Fang.\n\r",ch);
            ch->pcdata->tribe = SILVER_FANG;
            ch->clan = clan_lookup("silverfang");
        }
        else if(!str_prefix( argument, "warders of men") || !str_prefix( argument, "warder of men") || !str_prefix( argument, "wardersofmen"))
        {
            send_to_char("Alright. You are now a Warders of Men.\n\r",ch);
            ch->pcdata->tribe = WARDERSOFMEN;
            ch->clan = clan_lookup("wardersofmen");
        }
        else
        {
            send_to_char("That is not a valid Tribe. Please pick a tribe.\n\r",ch);
            return;
        }
        send_to_char("{g    -=[You must now pick your Fur Color]=-{x\n\rFur Color:  Please set your fur color to a realistic one-word color\n\r that wolves would have for fur.  This is impossible to undo. Choose wisely.\n\r{r    *{xType gogarou <fur color> to continue.\n\r",ch);
        return;
    }

    if(ch->pcdata->garou_fur == NULL || ch->pcdata->garou_fur[0] == '\0')
    {
        if(argument[0] == '\0')
        {
            send_to_char("You must pick a fur color. Type gogarou <fur color>. Remember, you cannot undo this. Choose wisely.\n\r{r    *{xType gogarou <fur color> to continue.\n\r",ch);
            return;
        }
        smash_tilde( argument );
        if ( c_str_len(argument) > 18 )
        {
            sendch("Fur color length limited to 18 visible characters. Pick something shorter!\n\r", ch);
            return;
        }
        free_string( ch->pcdata->garou_fur );
        ch->pcdata->garou_fur = str_dup( argument );
    }

    if(ch->pcdata->rank == 0)
    {
        ch->pcdata->rank = 1;

        sprintf(buf, "Your all set! You are now: %s, %s %s of the %s tribe! You have %s fur.\n\rYour Renown is %d Honor, %d Glory and %d Wisdom. You are rank 1.\n\r",
        ch->name, ch->pcdata->breed == HOMID ? "Homid" : ch->pcdata->breed == METIS ? "Metis" : "Lupus", ch->pcdata->auspice == RAGABASH ? "Ragabash" :
        ch->pcdata->auspice == THEURGE ? "Theurge" : ch->pcdata->auspice == PHILODOX ? "Philodox" :
        ch->pcdata->auspice == GALLIARD ? "Galliard" : "Ahroun", tribe_table[ch->pcdata->tribe].name,
        ch->pcdata->garou_fur, ch->pcdata->renown[HONOR], ch->pcdata->renown[GLORY], ch->pcdata->renown[WISDOM]);
        send_to_char(buf,ch);
        send_to_char("If you have not gone through the 'create' process since remorting Garou, you will need\n\r", ch);
        send_to_char("to do so again. Until you have done so, you will have no skills.\n\r", ch);
        send_to_char("Also check out the new options in 'freebie' and 'background'.\n\r",ch);
    }

    return;
}

void do_rage(CHAR_DATA *ch, char *argument)
{
    AFFECT_DATA af;

    if(IS_NPC(ch)) return;

    if(ch->pcdata->rank == 0)
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }

    if(is_affected( ch, gsn_rage))
    {
        send_to_char("You fight back your Rage and calm yourself.\n\r",ch);
        affect_strip(ch,gsn_rage);
        return;
    }

    if(ch->pcdata->rage[TEMP] == 0)
    {
        send_to_char("You have not built up enough Rage.\n\r",ch);
        return;
    }
    if(ch->pcdata->primal_urge == 0)
    {
        send_to_char("You do not have the animal nature to accomplish such a feat of outright Rage.\n\r",ch);
        return;
    }
    ch->pcdata->rage[TEMP]--;
    send_to_char("You lax your grip on the Rage within.\n\r",ch);
    af.where     = TO_AFFECTS;
    af.type      = gsn_rage;
    af.level     = ch->level;
    af.duration  = ch->pcdata->primal_urge*ch->pcdata->rage[PERM];
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = 0;
    affect_to_char( ch, &af );
    return;
}

void do_stepsideways(CHAR_DATA *ch, char *argument)
{
    ROOM_INDEX_DATA *location;
    OBJ_DATA *obj_next;
    OBJ_DATA *bag;
    OBJ_DATA *bag_next;
    OBJ_DATA *obj;
    int success, diff, i;
    OBJ_DATA *mirror;
    bool reflective = FALSE;
    if (IS_NPC(ch))
    return;

    if(ch->pcdata->rank == 0)
    {
        send_to_char("Huh?\n\r",ch);
        return;
    }

    if (ch->position == POS_FIGHTING)
    {
        send_to_char("You can't concentrate enough!\n\r", ch);
        return;
    }

    diff = get_gauntlet(ch);

    if (is_affected(ch, gsn_gift_pulseoftheinvisible))
        diff -= 2;

    if (!IS_NULLSTR(argument) && (mirror = get_obj_here(ch, NULL, argument)) != NULL &&
        IS_SET(mirror->extra_flags, ITEM_REFLECTIVE))
    {
            diff--;
            reflective = TRUE;
    }

    success = godice(ch->pcdata->gnosis[PERM], diff);

    if (IS_DEBUGGING(ch))
    {
        char buf[MSL];
        sprintf(buf, "g%dd%ds%d\n\r", get_gauntlet(ch), diff, success);
        sendch(buf, ch);
    }

    if (success == 0)
    {
        send_to_char("You fail to pierce the Gauntlet and remain where you stand.\n\r", ch);
        WAIT_STATE(ch, 24);
        return;
    } else  if (success > 0){
        if (pass_gauntlet(ch, TRUE))
        {
            do_function(ch, &do_look, "auto");
            WAIT_STATE(ch, 12);
            return;
        }
    } else {
        if (reflective) {
            extract_obj(mirror);
            sendch("The reflective surface you were using breaks, rendered completely useless!\n\r", ch);
            sendch("You remain where you stand.\n\r", ch);
            return;
        }

        AFFECT_DATA af;
        act( "You start to step into the Umbra... and suddenly feel a tugging sensation as you're surrounded by Webs!",  ch, NULL, NULL, TO_CHAR    );
        act( "A vague look of terror crosses $n's features before they suddenly cease to exist in this reality.",  ch, NULL, NULL, TO_NOTVICT );

        if (!IS_AFFECTED2(ch, AFF2_UMBRA))
                pass_gauntlet(ch, FALSE);

            af.where	=	TO_AFFECTS;
            af.type		=	gsn_trappedingauntlet;
            af.level	=	diff;
            af.duration	=	(diff) / 2 + 1;
            af.modifier	=	0;
            af.location	=	0;
            af.bitvector	=	0;
            affect_to_char( ch, &af );
            WAIT_STATE(ch, 48);
    }
}

void do_packtactics(CHAR_DATA *ch, char *argument)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *rch, *rch_next;
  bool packmates = FALSE;
  int tactics = 0, damage = 0;

  argument = one_argument( argument, arg1 );
  argument = one_argument( argument, arg2 );

  if (ch->race != race_lookup("garou"))
  {
    send_to_char("Huh?\n\r", ch);
    return;
  }

  if (ch->cswillpower < 1)
  {
    send_to_char("You do not have the willpower to lead your brethren in a tactful maneuver.\n\r", ch);
    return;
  }

  if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL)
  {
    send_to_char("The command to direct your packmates is:\n\r  packtactics <target> <tactic>\n\r  Valid tactics: fur gnarl[{runcoded{x], harrying[{runcoded{x], savage\n\r", ch);
    return;
  }

  if (
/*    str_cmp(arg2, "fur gnarl")
//  || str_cmp(arg2, "harrying")
  ||*/ str_cmp(arg2, "savage")
  )
  {
    send_to_char("That is not a valid tactic.\n\r", ch);
    send_to_char("The command to direct your packmates is:\n\r  packtactics <target> <tactic>\n\r  Valid tactics: fur gnarl[{runcoded{x], harrying[{runcoded{x], savage\n\r", ch);
    return;
  }

  for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
  {
      rch_next = rch->next_in_room;
      if ( !IS_NPC(rch) && rch->race == race_lookup("garou") && is_same_group(ch,rch) && rch != ch)
      {
        packmates = TRUE;
        break;
      }
  }

  if (packmates == FALSE)
  {
    send_to_char("You have no other warriors of Gaia nearby to aid in such a tactic.\n\r", ch);
    return;
  }

  ch->cswillpower--;

/*  if (!str_cmp(arg2, "fur gnarl"))
  {

  }


  else if (!str_cmp(arg2, "harrying"))
  {

  }


  else */
  {
    tactics = godice(get_attribute(ch, DEXTERITY)+get_ability(ch, CSABIL_BRAWL), 6);

    if (tactics < 0)
    {
      act("You attempt to knock down $N for your teammates, but stumble on your lunge!", ch, NULL, victim, TO_CHAR);
      act("$n lunges towards you, but stumbles and misses by a wide margin.", ch, NULL, victim, TO_VICT);
      act("$n tries to lunge at $N, but only manages to stumble ungracefully.", ch, NULL, victim, TO_NOTVICT);
      WAIT_STATE(ch, 9);
      return;
    }

    if (tactics == 0)
    {
      act("You collide with $N, trying to topple $M, but $E rebounds quickly.", ch, NULL, victim, TO_CHAR);
      act("$n collides directly with you, rocking your balance briefly.", ch, NULL, victim, TO_VICT);
      act("$n collides with $N, but both combatants manage to remain standing.", ch, NULL, victim, TO_NOTVICT);
      WAIT_STATE(ch, 3);
      return;
    }

    act("Moving quickly, you collide with $N and bring $M to the ground.", ch, NULL, victim, TO_CHAR);
    act("$n tackles you, bringing you to the ground.", ch, NULL, victim, TO_VICT);
    act("$n tackles $N, bringing $M down to the ground.", ch, NULL, victim, TO_NOTVICT);

    damage = godice(get_attribute(ch, STRENGTH),6);
    d10_damage(ch, victim, damage, ch->level * 5, gsn_garou_frenzy, DAM_BASH, DEFENSE_FULL, TRUE, FALSE);

    for ( rch = ch->in_room->people; rch != NULL; rch = rch_next )
    {
        rch_next = rch->next_in_room;
        if ( !IS_NPC(rch) && is_same_group(ch,rch) && rch != ch
            && rch->race == race_lookup("garou") && rch->pcdata->shiftform >= CRINOS)
        {
          act("Sensing an opening, you quickly lunge at $N and slash $M with your powerful jaws!", rch, NULL, victim, TO_CHAR);
          act("You let out howl of pain as $n bites down on your prone body!", rch, NULL, victim, TO_VICT);
          act("Darting in, $n seizes an opening and savagely bites $N's prone form!", rch, NULL, victim, TO_NOTVICT);
          damage = godice(get_attribute(rch, STRENGTH + 1),6);
          d10_damage(rch, victim, damage, ch->level * 4, gsn_garou_frenzy, DAM_PIERCE, DEFENSE_ARMOR, TRUE, FALSE);
        }
    }
  }

  return;
}

void renown_gain(CHAR_DATA *ch)
{
    int renown, chance, type;
    renown = chance = type = 0;
    renown = number_range(1, 4);

    if (ch->pcdata->renown[TEMP_GLORY] == 10 &&
        ch->pcdata->renown[TEMP_HONOR] == 10 &&
        ch->pcdata->renown[TEMP_WISDOM] == 10)
        return;

    if (ch->pcdata->breed == METIS)
        renown /= 2;

        for (chance = 0; chance < renown; chance++)
        {
            switch (ch->pcdata->auspice)
            {
                case RAGABASH:
				case GALLIARD:
                switch(number_range(1,3))
                {//33% chance of each.
                    case 1: type = TEMP_GLORY; break;
                    case 2: type = TEMP_HONOR; break;
                    case 3: type = TEMP_WISDOM; break;
                    }
                    break;
                case THEURGE:
                switch(number_range(1,4))
                {//50% chance wisdom, 25% chace other
                    case 1: type = TEMP_WISDOM; break;
                    case 2: type = TEMP_WISDOM; break;
                    case 3: type = TEMP_GLORY; break;
                    case 4: type = TEMP_HONOR; break;
                    }
                    break;
                case PHILODOX:
                switch(number_range(1,4))
                {//50% chance of honor
                    case 1: type = TEMP_HONOR; break;
                    case 2: type = TEMP_HONOR; break;
                    case 3: type = TEMP_WISDOM; break;
                    case 4: type = TEMP_GLORY; break;
                    }
                    break;
                case AHROUN:
                switch(number_range(1,4))
                {//50% chance of glory
                    case 1: type = TEMP_GLORY; break;
                    case 2: type = TEMP_HONOR; break;
                    case 3: type = TEMP_WISDOM; break;
                    case 4: type = TEMP_GLORY; break;
                    }
                    break;
            }
            if(ch->pcdata->renown[type] < 10)
            {
                ch->pcdata->renown[type]++;
            } else if ((ch->pcdata->renown[TEMP_GLORY] < 10 ||
						ch->pcdata->renown[TEMP_WISDOM] < 10 ||
						ch->pcdata->renown[TEMP_HONOR] < 10 ) &&
						number_range(0, 5) < ch->pcdata->rank )
					chance--;
        }


}


void do_renown( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MIL];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    DESCRIPTOR_DATA *d;
    int renown;
    int type;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if(IS_NPC(ch)) return;

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("You are not one of Gaia's chosen!\n\r",ch);
        return;
    }

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char( "Syntax: renown <char> <honor/glory/wisdom> <amount>\n\r",ch);
        return;
    }


    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL)
    {
        send_to_char( "They aren't here to bestow renown upon.\n\r", ch );
        return;
    }
    if (IS_NPC(victim))
    {
        send_to_char("Not on NPCs!\n\r", ch);
        return;
    }

/*
#define TEMP_GLORY              1
#define TEMP_HONOR              3
#define TEMP_WISDOM             5
*/
    if (!str_cmp(arg2, "glory"))
        type = TEMP_GLORY;
    else if (!str_cmp(arg2, "honor"))
        type = TEMP_HONOR;
    else if (!str_cmp(arg2, "wisdom"))
        type = TEMP_WISDOM;
    else
    {
        send_to_char("Unknown Renown type. (Types: Glory, Honor, Wisdom)\n\r", ch);
        return;
    }

    renown = atoi( arg3 );
    if((victim->pcdata->renown[type]+renown) > 10)
    {
        send_to_char( "You cannot grant this person so high a renown.\n\r", ch );
        return;
    }
    if (victim->pcdata->renown[type]+renown < 0)
    {
        send_to_char("They do not have that much renown to lose!\n\r", ch);
        return;
    }

    victim->pcdata->renown[type] += renown;
    sprintf(buf, "You announce to your sept that $N has earned your respect and renown.");
    act(buf,ch,NULL,victim,TO_CHAR);
    sprintf(buf, "You humbly bow your head as $n bestows great honor and renown upon you.");
    act(buf,ch,NULL,victim,TO_VICT);

    if(renown > 0)
        sprintf( buf, "$n announces to the Sept '{YHail and behold! %s has braved many adventures and has earned great honor and renown in my eyes!{x'",victim->name);
    else
        sprintf( buf, "$n announces to the Sept '{YBare witness! For I cast down %s in shame and disrespect! All should shun this pitiful wretch!{x'",victim->name);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING && d->character != ch && d->character->race == race_lookup("garou"))
        {
            act(buf,ch,NULL,d->character,TO_VICT);
        }
    }

    return;
}


void do_bestowgift (CHAR_DATA *ch, char *argument)
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int gn,i;
    bool override = FALSE;
    bool pass = FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );


        if(arg1[0] == '\0' || arg2[0] == '\0')
        {sendch("Syntax:  bestowgift (victim) '(giftname)' <override>\n\r", ch);
         sendch("Override will override T/B/A requirements (IMM only)\n\r", ch);
         sendch("Quotes are required around gift name.\n\r", ch);
        return;
        }
        if ( ( victim = get_char_room( ch, NULL, arg1 ) ) == NULL)
        {
        sendch("Bestow a gift upon whom?\n\r", ch);
        return;
        }
        if (IS_NPC(victim))
        {
        sendch("Only on players!\n\r", ch);
        return;
        }

        if((gn = gift_lookup(arg2)) < 1)
        {
        sendch("Bestow which gift?\n\r", ch);
        return;
        }
        if (!IS_NULLSTR(arg3) && !str_cmp(arg3, "override"))
            override = TRUE;

        bool learned = FALSE;
        int  li;
        for (li = 0; li < MAX_GIFT; li++)
        {
            if (victim->pcdata->gift[li] == gn)
                learned = TRUE;
                }

        if (learned)
        {
            send_to_char("They already have this gift.\n\r",ch);
            return;
        }

        if(override || victim->pcdata->breed == gift_table[gn].breed ||
            victim->pcdata->auspice == gift_table[gn].auspice ||
            victim->pcdata->tribe == gift_table[gn].tribe)
        pass=TRUE;

        if(!pass)
        {
            send_to_char("They cannot learn this Gift.\n\r",ch);
            if (IS_IMMORTAL(ch))
                sendch("Use 'override' to override this restriction.\n\r", ch);
            return;
        }


        bool full = TRUE;
        for (li = 0; li < MAX_GIFT; li++)
        {
            if (victim->pcdata->gift[li] == 0)
            {
                full = FALSE;
                break;
            }
        }

        if(full)
        {
            send_to_char("They cannot learn any more Gifts!\n\r",ch);
            return;
        } else {
        victim->pcdata->gift[li] = gn;
        sprintf(buf,"You pay homage to Gaia as the spirits teach you the gift of %s.",capitalize(gift_table[gn].name));
        act(buf,ch,NULL,victim,TO_VICT);
        cprintf(ch, "You bestow the gift of %s upon %s.\n\r", capitalize(gift_table[gn].name), victim->name);
        }
        return;

}


void do_garou( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *mob;
    OBJ_DATA *obj;
    int gn,i;
        MOB_INDEX_DATA *pMobIndex;

    bool pass=FALSE;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if(!str_cmp(arg1,"specialgarougiftteachgive123"))
    {
        if(ch->level != 110)
        {
            do_function(ch, &do_say, "The quest I assigned was not completed. I will not abide by such an offense!");
            do_function(ch, &do_emote, "disappears in a bright red flash.");
            extract_char( ch, TRUE );
            return;
        }

            int mob_vnum;
            long mcounter;
    for (mcounter = 0; mcounter < 999999; mcounter ++)
    {
    mob_vnum = number_range(50, 32600);

    if ( (pMobIndex = get_mob_index(mob_vnum) ) != NULL )
    {
            if (!IS_SET(pMobIndex->act, ACT_IS_HEALER)
        && !IS_SET(pMobIndex->act, ACT_PET)
        && !IS_SET(pMobIndex->affected_by, AFF_CHARM)
        && !IS_SET(pMobIndex->affected_by, AFF_INVISIBLE)) break;
        else pMobIndex = NULL;
    }
    }

    if ( pMobIndex == NULL || ( mob = get_char_world( ch, pMobIndex->player_name ) ) == NULL )
    {
            do_function(ch, &do_say, "I'm sorry, I have no quest for you at this time." );
            return;
        }

        if( (obj = create_object( get_obj_index( OBJ_VNUM_GIFT_TOKEN ), 1 )) == NULL)
        {
            do_function(ch, &do_say, "Error! Contact imms about a missing token!" );
            return;
        }

        obj_to_char(obj,mob);

        do_function(ch, &do_say, "Ah, a brave adventurer seeks to earn a favor from me hmm? Very well..." );
        sprintf(buf,"To seek the Gift of %s you must bring forth a magical token from %s in %s. Return, give it to me, and I will teach you.",arg2,mob->short_descr, mob->in_room->name);
        do_function(ch, &do_say, buf);
        ch->level--;
        return;
    }


    if(!str_cmp(arg1,"specialgarougiftteach123"))
    {
        if(!IS_NPC(ch) || arg2[0] == '\0' || arg3[0] == '\0')
        return;
        if ( ( victim = get_char_room( ch, NULL, arg3 ) ) == NULL)
        return;
        if((gn = gift_lookup(arg2)) == 0)
        return;
        //send_to_char(buf,victim);

        bool learned = FALSE;
        int  li;
        for (li = 0; li < MAX_GIFT; li++)
        {
            if (victim->pcdata->gift[li] == gn)
                learned = TRUE;
                }

        if (learned)
        {
            send_to_char("You already have this gift.\n\r",victim);
            return;
        }

        if(victim->pcdata->breed == gift_table[gn].breed ||
            victim->pcdata->auspice == gift_table[gn].auspice ||
            victim->pcdata->tribe == gift_table[gn].tribe)
        pass=TRUE;

        if(!pass)
        {
            send_to_char("You cannot learn this Gift.\n\r",victim);
            return;
        }


        bool full = TRUE;
        for (li = 0; li < MAX_GIFT; li++)
        {
            if (victim->pcdata->gift[li] == 0)
            {
                full = FALSE;
                break;
            }
        }

        if(full)
        {
            send_to_char("You cannot learn any more Gifts!\n\r",victim);
            return;
        } else {
        victim->pcdata->gift[li] = gn;
        do_function(ch, &do_say, "You have done well... I will teach you." );
        sprintf(buf,"You pay homage to Gaia as the spirit teaches you the gift of %s.",gift_table[gn].name);
        act(buf,ch,NULL,victim,TO_VICT);
        act("$N lets forth a savage howl in thanks to Gaia and the spirits for their divine Gift.",ch,NULL,victim,TO_NOTVICT);
        }
        return;
    }

    if(IS_NPC(ch)) return;

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("You are not one of Gaia's chosen!\n\r",ch);
        return;
    }
    if(ch->renown == 0) ch->renown = 150;

    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char( "Syntax: garou <breed> <auspice>\n\r",ch);
        return;
    }

    if(!str_prefix(arg1, "show"))
    {
        sprintf(buf, "New Garou: Your breed is %d. Your Auspice is %d. Your Tribe is %s. Your Rage is %d/%d your Gnosis is %d/%d your Rank is %d.\n\r",
            ch->pcdata->breed, ch->pcdata->auspice, tribe_table[ch->pcdata->tribe].name, ch->pcdata->rage[PERM], ch->pcdata->rage[TEMP], ch->pcdata->gnosis[PERM],
            ch->pcdata->gnosis[TEMP], ch->pcdata->rank);
        send_to_char(buf,ch);
    }

    if(ch->breed > 0)
    {
        send_to_char("You cannot use this command. Contact an admin for help.\n\r",ch);
        return;
    }

    if(!str_prefix(arg1, "lupus"))
    {
        ch->breed = BREED_LUPUS;
        send_to_char("Your breed is now Lupus.\n\r",ch);
    }
    else if(!str_prefix(arg1, "homid"))
    {
        ch->breed = BREED_HOMID;
        send_to_char("Your breed is now Homid.\n\r",ch);
    }
    else if(!str_prefix(arg1, "metis"))
    {
        ch->breed = BREED_METIS;
        send_to_char("Your breed is now Metis.\n\r",ch);
    }
    else
    {
        send_to_char("Incorrect Breed. Command aborted.\n\r",ch);
        ch->breed = 0;
        return;
    }

    if(!str_prefix(arg2, "ragabash"))
    {
        ch->auspice = AUSPICE_RAGABASH;
        send_to_char("Your Auspice is now Ragabash.\n\r",ch);
    }

    else if(!str_prefix(arg2, "theurge"))
    {
        ch->auspice = AUSPICE_THEURGE;
        send_to_char("Your Auspice is now Theurge.\n\r",ch);
    }

    else if(!str_prefix(arg2, "philodox"))
    {
        ch->auspice = AUSPICE_PHILODOX;
        send_to_char("Your Auspice is now Philodox.\n\r",ch);
    }

    else if(!str_prefix(arg2, "galliard"))
    {
        ch->auspice = AUSPICE_GALLIARD;
        send_to_char("Your Auspice is now Galliard.\n\r",ch);
    }

    else if(!str_prefix(arg2, "ahroun"))
    {
        ch->auspice = AUSPICE_AHROUN;
        send_to_char("Your Auspice is now Ahroun.\n\r",ch);
    }
    else
    {
        send_to_char("Invalid Auspice. Command aborted.\n\r",ch);
        ch->breed = 0;
        ch->auspice = 0;
        return;
    }


    send_to_char("All fixed. Your done. You now have your breed and auspice. Enjoy it... or else!\n\r",ch);
    return;
}

/*Sengir putting Totems here for now*/
void do_totem( CHAR_DATA *ch, char *argument )
{
    AFFECT_DATA af;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if (IS_NPC(ch))
        return;

    if (arg[0] == '\0')
    {
        send_to_char("The command is:   totem <totem name>\n\r", ch);
        send_to_char("Which Totem would you like to ask for a boon?\n\r", ch);
        send_to_char("Available:  {WPegasus{x, {WStag{x, {WWendigo{x\n\r", ch);
        return;
    }

    if (ch->race != race_lookup("garou"))
    {
        send_to_char("The Spirits of the mighty Totems only bestow blessings upon Gaia's chosen.\n\r", ch);
        return;
    }

    if (is_affected(ch, gsn_spirit_totem))
    {
        send_to_char("You have already asked for the assistance of a Totem Spirit recently!\n\r", ch);
        return;
    }

    if (!str_prefix( arg, "pegasus" ))
    {
        if (ch->rage < 8)
        {
            send_to_char("You do not have enough Rage to warrant the Totem's assistance.\n\r", ch);
            return;
        }

        if (ch->mana < ch->level / 2)
        {
            send_to_char("You do not have enough spiritual energy to summon the Totem.\n\r", ch);
            return;
        }

        ch->rage -= 8;
        ch->mana -= ch->level / 2;

        WAIT_STATE(ch, 16);

        send_to_char("{WPegasus{x appears with fire in her eyes, riding the air currents.\n\r", ch);
        send_to_char("She soars overhead and into the horizon, beckoning you to the skies.\n\r", ch);

        af.where    = TO_AFFECTS;
        af.type     = gsn_spirit_totem;
        af.level    = ch->level;
        af.duration = ch->level / 5 + 3;
        af.modifier = 0;
        af.location = APPLY_NONE;
        af.bitvector    = AFF_FLYING;
        affect_to_char( ch, &af );

        return;
    }

    if (!str_prefix( arg, "stag" ))
    {
        if (ch->rage < 12)
        {
            send_to_char("You do not have enough Rage to warrent the Totem's assistance.\n\r", ch);
            return;
        }

        if (ch->mana < ch->level)
        {
            send_to_char("You do not have enough spiritual energy to summon the Totem.\n\r", ch);
            return;
        }

        ch->rage -= 12;
        ch->mana -= ch->level;

        WAIT_STATE(ch, 16);

        send_to_char("{WStag{x appears, walking gracefully and delicately between Light and Dark.\n\r", ch);
        send_to_char("He bounds away, spurring you on to look to the Wild Hunt.\n\r", ch);

        af.where    = TO_AFFECTS;
        af.type     = gsn_spirit_totem;
        af.level    = ch->level;
        af.duration = ch->level / 3;
        af.modifier = ch->level * 2;
        af.location = APPLY_HIT;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        af.where    = TO_AFFECTS;
        af.type     = gsn_spirit_totem;
        af.level    = ch->level;
        af.duration = ch->level / 3;
        af.modifier = ch->level * 2;
        af.location = APPLY_MOVE;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    if (!str_prefix( arg, "wendigo" ))
    {
        if ( ch->mana < ch->level )
        {
            send_to_char("You do not have enough spiritual energy to summon this Totem.\n\r", ch);
            return;
        }

        ch->mana -= ch->level / 2 + 20;
        ch->rage += 40;

        WAIT_STATE(ch, 16);

        send_to_char("Roaring like the frozen north wind that spawned him, {WWendigo{x appears, cloaked in ice and snow.\n\r", ch);
        send_to_char("Howling with fury, he fills your heart with {RRage{x before vanishing in a storm of ice.\n\r", ch);

        af.where    = TO_AFFECTS;
        af.type     = gsn_spirit_totem;
        af.level    = ch->level;
        af.duration = 10;
        af.modifier = 0;
        af.location = APPLY_NONE;
        af.bitvector    = 0;
        affect_to_char( ch, &af );

        return;
    }

    send_to_char("That is not a valid Totem.\n\r", ch);
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
