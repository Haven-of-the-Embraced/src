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
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"
#include "recycle.h"

/*
 * Local functions.
 */
void do_chop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *log;
    OBJ_DATA *weapon;
    int success;
    extern int tree_count;

    if(IS_NPC(ch)) return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Chop down what?\n\r", ch );
    return;
    }

    if((weapon = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
        send_to_char("With what? Your teeth?\n\r",ch);
        return;
    }

    if(weapon->value[0] != WEAPON_AXE)
    {
        send_to_char("With THAT? You've got to be kidding me. Use an Axe!\n\r",ch);
        return;
    }

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "What tree?\n\r", ch );
    return;
    }

    if(obj->pIndexData->vnum != OBJ_VNUM_TREE && obj->pIndexData->vnum != OBJ_VNUM_TREE+1 &&
        obj->pIndexData->vnum != OBJ_VNUM_TREE+2 && obj->pIndexData->vnum != OBJ_VNUM_TREE+3 &&
        obj->pIndexData->vnum != OBJ_VNUM_TREE+4)
    {
        send_to_char("That is not a tree!\n\r",ch);
        return;
    }

    if(ch->pcdata->cssec_abil[CSABIL_LOGGING] == 0)
    {
        send_to_char("You might hurt yourself.\n\r",ch);
        return;
    }

    act( "You take a mighty swing at $p...", ch, obj,NULL, TO_CHAR );
    act( "$n takes a mighty swing at $p", ch, obj,NULL, TO_ROOM );

    success = godice(get_attribute(ch,STRENGTH)+ch->pcdata->cssec_abil[CSABIL_CARVING],UMAX(6, 10 - ch->pcdata->cssec_abil[CSABIL_LOGGING]));

    if(success < 1)
    {
        act( "and succeed only in chewing up the tree with your axe.", ch, NULL,NULL, TO_CHAR );
        act( "and succeeds only in chewing up the tree.", ch, NULL,NULL, TO_ROOM );
        obj->level -= number_range(1,5);
        WAIT_STATE( ch, UMAX(3, 11 - ch->pcdata->cssec_abil[CSABIL_LOGGING]) * PULSE_VIOLENCE );

        if(obj->level <= 0)
        {
            send_to_char("With a sharp crack the trunk splits, causing the tree to topple over towards you!\n\r",ch);
            act( "In its weakened state, the tree cracks and topples over and falls towards $n!", ch, NULL,NULL, TO_ROOM );

            success = godice(get_attribute(ch, DEXTERITY) + ch->csabilities[CSABIL_DODGE],7);

            if(success <= 0)
            {
                act("You stumble while trying to dive away from $p, but aren't fast enough!", ch, obj, NULL, TO_CHAR);
                act("$n stumbles and is crushed by $p!", ch, obj, NULL, TO_NOTVICT);
                damage( ch, ch, 2000, gsn_bash, DAM_BASH, FALSE);
            }

            else if (success > 0 && success < 3)
            {
                act("You dive out of the way, but a few branches still collide with you.", ch, NULL, NULL, TO_CHAR);
                act("$n dives out of the way, but is still pummelled by a few branches.", ch, NULL, NULL, TO_NOTVICT);
                damage( ch, ch, 200, gsn_bash, DAM_BASH, FALSE);
                gain_exp(ch, success * 15);
                gain_qp(ch, number_range(0, success));
            }

            act("You dive quickly out of the way, the remainder of $p splintering as it crashes to the ground.", ch, obj, NULL, TO_CHAR);
            act("Dives out of the way as $p splinters and crashes to the ground.", ch, obj, NULL, TO_NOTVICT);

            if (success > 0)
                gain_exp(ch, success * 25);
            extract_obj( obj );
            tree_count--;
        }
        return;
    }
    obj->level -= 1;

    if(obj->level <= 0)
    {
        act( "and you succeed in chopping down the tree.", ch, NULL,NULL, TO_CHAR );
        act( "and succeeds in chopping down the tree.", ch, NULL,NULL, TO_ROOM );
        tree_count--;
    }
    else
    {
        act( "and you succeed in chopping some wood from the tree.", ch, NULL,NULL, TO_CHAR );
        act( "and succeeds in chopping some wood from the tree.", ch, NULL,NULL, TO_ROOM );
    }
    log = create_object(get_obj_index(obj->pIndexData->vnum+5),0);
    obj_to_room(log,ch->in_room);
    WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_LOGGING]) * PULSE_VIOLENCE );
    
    gain_exp( ch, 15 * success );
    gain_qp(ch, number_range( 1, 2*success));

    if(obj->level <= 0)
        extract_obj( obj );
    return;
}

void do_prospect( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int success;


    if(IS_NPC(ch)) return;

    success = godice(get_attribute(ch,PERCEPTION)+ch->pcdata->cssec_abil[CSABIL_MINING],UMAX(6, 10 - ch->pcdata->cssec_abil[CSABIL_PROSPECTING]));

    act( "You carefully search the area...", ch, NULL,NULL, TO_CHAR );
    act( "$n carefully searches the area...", ch, NULL,NULL, TO_ROOM );


    if ( ( obj = get_obj_hidden( ch, "vein" ) ) == NULL)
    {
        WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
        if(success <= 0)
        {
            send_to_char("There might be something here...\n\r",ch);
            act( "and may have found something!", ch, NULL,NULL, TO_ROOM );
        }
        else
        {
            send_to_char("There is nothing here.\n\r",ch);
            act( "and finds nothing.", ch, NULL,NULL, TO_ROOM );
        }
        return;
    }


    if ( !IS_SET(obj->extra_flags, ITEM_HIDDEN) )
    {
        send_to_char("This vein has already been found.",ch);
        return;
    }

    if(success <= 0)
    {
        WAIT_STATE( ch, UMAX(3, 11 - ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
        act( "and you think you find something only to have it, and you buried in a rockslide! OUCH!.", ch, NULL,NULL, TO_CHAR );
        act( "and seems to find something briefly before being caught in a rockslide! OUCH!", ch, NULL,NULL, TO_ROOM );
        damage( ch, ch, 2000, gsn_magick, DAM_BASH, FALSE);
        extract_obj( obj );
        return;
    }

    if(success < 3)
    {
        WAIT_STATE( ch, UMAX(3, 3 - ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
        send_to_char("There might be something here...\n\r",ch);
        act( "and may have found something!", ch, NULL,NULL, TO_ROOM );
        return;
    }
    WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
    act( "EUREKA! You uncover $p!", ch, obj,NULL, TO_CHAR );
    act( "and uncovers $p!", ch, obj,NULL, TO_ROOM );
    REMOVE_BIT(obj->extra_flags,ITEM_HIDDEN);
    
    gain_exp( ch, 15 * success );
    gain_qp(ch, number_range(1, 2 * success));
    return;
}

void do_mine( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *ore;
    OBJ_DATA *tool;
    int success;
    int type = 0;

    if(IS_NPC(ch)) return;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Mine what?\n\r", ch );
    return;
    }

    if((tool = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
        send_to_char("Suuure... your going to mine through solid rock... WITH YOUR BARE HANDS?!\n\r",ch);
        return;
    }
    if (tool->item_type != ITEM_TOOL || str_cmp(tool_table[tool->value[1]].tool_name,"pickaxe"))
    {
        send_to_char("With THAT? You've got to be kidding me. Use a Pickaxe!\n\r",ch);
        return;
    }
    if (tool->value[0] < 1)
    {
        send_to_char("With that broken piece of crap? Get a decent pick, will ya?\n\r",ch);
        return;
    }

    if ( ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "What vein?\n\r", ch );
    return;
    }

    if(obj->pIndexData->vnum == OBJ_VNUM_VEIN+COAL || obj->pIndexData->vnum == OBJ_VNUM_VEIN+ZINC)
        type = CSABIL_MINERAL;
    if(obj->pIndexData->vnum == OBJ_VNUM_VEIN+IRON || obj->pIndexData->vnum == OBJ_VNUM_VEIN+COPPER
    || obj->pIndexData->vnum == OBJ_VNUM_VEIN+TIN || obj->pIndexData->vnum == OBJ_VNUM_VEIN+NICKEL
    || obj->pIndexData->vnum == OBJ_VNUM_VEIN+LEAD)
        type = CSABIL_METAL;
    if(obj->pIndexData->vnum == OBJ_VNUM_VEIN+SILVER || obj->pIndexData->vnum == OBJ_VNUM_VEIN+GOLD)
        type = CSABIL_PRECIOUS;

    if(type == 0 || ch->pcdata->cssec_abil[type] == 0)
    {
        send_to_char("That is not something you can mine!\n\r",ch);
        return;
    }

    if(ch->pcdata->cssec_abil[CSABIL_MINING] == 0)
    {
        send_to_char("You might hurt yourself.\n\r",ch);
        return;
    }

    act( "You take a mighty swing at the vein with $p...", ch, tool,NULL, TO_CHAR );
    act( "$n takes a mighty swing at the vein with $p...", ch, tool,NULL, TO_ROOM );



    success = godice(get_attribute(ch,STAMINA)+ch->pcdata->cssec_abil[CSABIL_MINING],UMAX(6, (number_range(1,3)+((100-tool->value[0])/10)) - ch->pcdata->cssec_abil[type]));

    if(success < 1)
    {
        obj->level -= number_range(1,5);
        WAIT_STATE( ch, UMAX(3, 11 - ch->pcdata->cssec_abil[CSABIL_MINING]) * PULSE_VIOLENCE );
        if(obj->level <= 0)
        {
            send_to_char("and your pick lodges in the vein...\n\rYou struggle briefly to pull it out before the head breaks off and comes flying to hit you between the eyes. OUCH!\n\r",ch);
            send_to_char("On top of an awful headache, you've also gained yourself a pile of dust.\n\rThat's about all that's left of the vein of ore.\n\r",ch);
            act( "and in a streak of bad luck, $m pickaxe breaks and the head flies up to klobber em one between the eyes!\n\rIn even more badluck, the vein is also lost.", ch, NULL,NULL, TO_ROOM );
            damage( ch, ch, 20000, gsn_magick, DAM_BASH, FALSE);
            tool->value[0] = 0;
            extract_obj( obj );
        }
        else
        {
            act( "and succeed only in pounding some of the ore to dust.", ch, NULL,NULL, TO_CHAR );
            act( "and succeeds only in pounding some of the ore to dust.", ch, NULL,NULL, TO_ROOM );
        }
        return;
    }
    obj->level -= 1;
    gain_exp( ch, 15 * success);
    gain_qp(ch, number_range(1,2 * success));

    if(obj->level <= 0)
    {
        act( "and you manage to mine out the last of the ore.", ch, NULL,NULL, TO_CHAR );
        act( "and manages to mine out the last of the ore.", ch, NULL,NULL, TO_ROOM );
    }
    else
    {
        act( "and you manage to mine out some ore.", ch, NULL,NULL, TO_CHAR );
        act( "and manages to mine out some ore.", ch, NULL,NULL, TO_ROOM );
    }
    ore = create_object(get_obj_index(obj->pIndexData->vnum+10),0);
    obj_to_room(ore,ch->in_room);
    WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_MINING]) * PULSE_VIOLENCE );
    if(obj->level <= 0)
        extract_obj( obj );
    return;
}

void do_plane( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *bench;
    OBJ_DATA *tool;
    OBJ_DATA *log;
    int success;
    int value;

    if(IS_NPC(ch)) return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Plane what?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' || (str_prefix(arg2,"block") && str_prefix(arg2,"plank")))
    {
        send_to_char( "Into what? Block or plank?\n\r", ch );
        return;
    }

    if((tool = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
        send_to_char("Yeah, keep on trying to plane a log with your tongue. Let's see how you like those splinters!\n\r",ch);
        return;
    }
    if (tool->item_type != ITEM_TOOL || str_cmp(tool_table[tool->value[1]].tool_name,"plane"))
    {
        send_to_char("Hmmm... that might work a little better.. IF YOU HAD A PLANE!\n\r",ch);
        return;
    }
    if (tool->value[0] < 1)
    {
        send_to_char("With that broken piece of crap? Get a decent plane, will ya?\n\r",ch);
        return;
    }

    if((bench = get_obj_here(ch,NULL, "bench")) == NULL || bench->item_type != ITEM_TOOL ||
        str_cmp(tool_table[bench->value[1]].tool_name,"bench"))
    {
        send_to_char("You need a bench in the room to plane wood.\n\r",ch);
        return;
    }
    if ( ( log = get_obj_carry( ch, arg, ch) ) == NULL )
    {
        send_to_char( "Plane what?\n\r", ch );
        return;
    }

    if(log->pIndexData->vnum != OBJ_VNUM_TREE+5 && log->pIndexData->vnum != OBJ_VNUM_TREE+6 &&
        log->pIndexData->vnum != OBJ_VNUM_TREE+7 && log->pIndexData->vnum != OBJ_VNUM_TREE+8 &&
        log->pIndexData->vnum != OBJ_VNUM_TREE+9)
    {
        send_to_char("That is not a log!\n\r",ch);
        return;
    }

    if(ch->pcdata->cssec_abil[CSABIL_CARVING] == 0)
    {
        send_to_char("You might hurt yourself.\n\r",ch);
        return;
    }
    act( "You run the plane along $p...", ch, log,NULL, TO_CHAR );
    act( "$n runs a plane along $p...", ch, log,NULL, TO_ROOM );



    success = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_CRAFTS],UMAX(6, (number_range(1,5)+((100-tool->value[0])/10)) - ch->pcdata->cssec_abil[CSABIL_CARVING]));

    if(success < 1)
    {
        act( "and unfortunately end up with a useless lump of scrap.", ch, tool,NULL, TO_CHAR );
        act( "and unfortunately ends up with a useless lump of scrap.", ch, tool,NULL, TO_ROOM );

        WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
            extract_obj( log );

        return;
    }

    value = log->pIndexData->vnum-(OBJ_VNUM_TREE+5);

    if(!str_prefix(arg2,"block"))
        value += OBJ_VNUM_BLOCK;
    else value += OBJ_VNUM_PLANK;

    obj = create_object(get_obj_index(value),0);

    act( "and manage to plane the log into $p!", ch, obj,NULL, TO_CHAR );
    act( "and manages to plane the log into $p!", ch, obj,NULL, TO_ROOM );

    gain_exp( ch, 10 * success );
    gain_qp(ch, number_range( 1, 2*success));
    obj_to_room(obj,ch->in_room);
    WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
    extract_obj( log );
    return;
}
void do_carve( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char desc[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *bench;
    OBJ_DATA *tool;
    OBJ_DATA *resource;
    int success;
    int craft = 0;
    int i;
    bool found = FALSE;
    bool craftable = FALSE;
    EXTRA_DESCR_DATA *ed;


    if(IS_NPC(ch)) return;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Carve what?\n\r", ch );
        return;
    }

    if ( arg2[0] == '\0' )
    {
        send_to_char( "Carve it into what?\n\r", ch );
        return;
    }

    if((tool = get_eq_char(ch,WEAR_WIELD)) == NULL)
    {
        send_to_char("You might be able to take a bite out of it but I doubt you can carve it WITH YOUR TEETH!\n\r",ch);
        return;
    }

    if (tool->item_type != ITEM_TOOL || str_cmp(tool_table[tool->value[1]].tool_name,"knife"))
    {
        send_to_char("How about trying it with... umm... I don't know... A KNIFE!\n\r",ch);
        return;
    }
    if (tool->value[0] < 1)
    {
        send_to_char("It slices, it dices, but it won't cut wood if it's broken!\n\r",ch);
        return;
    }

    if((bench = get_obj_here(ch,NULL, "bench")) == NULL || bench->item_type != ITEM_TOOL ||
        str_cmp(tool_table[bench->value[1]].tool_name,"bench"))
    {
        send_to_char("You need a bench in the room to carve wood.\n\r",ch);
        return;
    }
    if ( ( resource = get_obj_carry( ch, arg, ch) ) == NULL )
    {
        send_to_char( "Carve what?\n\r", ch );
        return;
    }

    if(ch->pcdata->cssec_abil[CSABIL_CARVING] == 0)
    {
        send_to_char("You might hurt yourself.\n\r",ch);
        return;
    }

    for ( i = 0; crafted_item_table[i].name != NULL; i++)
    {
        if(!str_prefix(arg2,crafted_item_table[i].name) &&
            ch->pcdata->cssec_abil[crafted_item_table[i].skill] >= crafted_item_table[i].lvl)
        {
            craft = i;
            craftable = TRUE;
        }
    }
    if(!craftable)
    {
        send_to_char("That is not something you can craft.\n\r",ch);
        return;
    }
    else
    {
        for(i = 0;i < 5;i++)
        {
            if(resource->pIndexData->vnum == crafted_item_table[craft].resource_type+i)
                found = TRUE;
        }
    }

    if(!found)
    {
        act( "You can't use $p to make that.",ch,resource,NULL,TO_CHAR);
        return;
    }

    if((obj = create_object(get_obj_index(crafted_item_table[craft].vnum),0)) == NULL)
    {
        send_to_char("Error. Please contact the imms.\n\r",ch);
        return;
    }

    act( "You set about carefully carving on $p...", ch, resource,NULL, TO_CHAR );
    act( "$n sets about carefully carving on $p...", ch, resource,NULL, TO_ROOM );

    success = godice(get_attribute(ch,DEXTERITY)+ch->pcdata->cssec_abil[CSABIL_CARVING],UMAX(6, ((100-tool->value[0])/10) - ch->csabilities[CSABIL_WOOD]));

    if(success < 1)
    {
        act( "and unfortunately end up with a useless lump of scrap.", ch, tool,NULL, TO_CHAR );
        act( "and unfortunately ends up with a useless lump of scrap.", ch, tool,NULL, TO_ROOM );

        WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
            extract_obj( resource );
            extract_obj( obj );

        return;
    }

    obj->material = str_dup(resource->material);

    if(obj->item_type == ITEM_CRAFTED)
    {
        obj->value[0] = (get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_CRAFTS]+ch->pcdata->cssec_abil[CSABIL_CARVING]+ch->pcdata->cssec_abil[CSABIL_WOOD]+success)*4;
        for(i = 0;i < 5;i++)
        {
            if(resource->pIndexData->vnum - crafted_item_table[craft].resource_type == crafted_item_table[craft].best[i])
            obj->value[0] -= i*5;
        }
        obj->value[0] -= number_range(1,10);
        if(obj->value[0] > 100) obj->value[0] = 100;
        if(obj->value[0] < 0) obj->value[0] = 0;
        i = 0;
        if(obj->value[0] >= 70)
        {
            obj->value[2] = 1;
            i = 1;
        }
        if(obj->value[0] >= 80)
        {
            obj->value[3] = 1;
            i = 2;
        }
        if(obj->value[0] >= 90)
        {
            obj->value[4] = 1;
            i = 3;
        }
        obj->level = number_range(50,80)-obj->value[0];
        if(obj->level < 0) obj->level = 0;
        obj->cost = obj->value[0]*(100*crafted_item_table[craft].lvl);
        sprintf(desc,"It is %s made from %s.\n\rIt seems %s made.\n\r%s\n\r",obj->short_descr, obj->material,
        obj->value[0] <= 10 ? "awfully" : obj->value[0] <= 20 ? "poorly" : obj->value[0] <= 30 ? "fairly poorly" :
        obj->value[0] <= 40 ? "fairly well" : obj->value[0] <= 50 ? "somewhat well" : obj->value[0] <= 60 ? "rather well" :
        obj->value[0] <= 70 ? "quite well" : obj->value[0] <= 80 ? "extremely well" : obj->value[0] <= 90 ? "expertly" : "masterfully",
        i == 0 ? "It cannot be further modified." : i == 1 ? "It can be modified only once." : i == 2 ? "It can be modified twice." :
        "It can be modified three times.");
    }
    else
        sprintf(desc,"It is %s made from %s.\n\r",obj->short_descr, obj->material);

    ed                  =   new_extra_descr();
    ed->keyword         =   str_dup( obj->name );
    ed->next            =   obj->extra_descr;
    obj->extra_descr    =   ed;
    ed->description     =   str_dup(desc);

    act( "and manage to create $p!", ch, obj,NULL, TO_CHAR );
    act( "and manages to create $p!", ch, obj,NULL, TO_ROOM );

    gain_exp( ch, 10 * success );
    gain_qp(ch, number_range( 2, 2*success));
    obj_to_room(obj,ch->in_room);
    WAIT_STATE( ch, UMAX(3, 6 - ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
    extract_obj( resource );
    return;
}
