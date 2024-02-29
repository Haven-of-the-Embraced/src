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
#define CD CHAR_DATA
#define OD OBJ_DATA
bool    remove_obj  args( (CHAR_DATA *ch, int iWear, bool fReplace ) );
void    wear_obj    args( (CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
CD *    find_keeper args( (CHAR_DATA *ch ) );
int get_cost    args( (CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void    obj_to_keeper   args( (OBJ_DATA *obj, CHAR_DATA *ch ) );
OD *    get_obj_keeper  args( (CHAR_DATA *ch,CHAR_DATA *keeper,char *argument));

#undef OD
#undef  CD

/* RT part of the corpse looting code */

bool can_loot(CHAR_DATA *ch, OBJ_DATA *obj)
{
    CHAR_DATA *owner, *wch;

    if (IS_IMMORTAL(ch))
    return TRUE;

    if (!obj->owner || obj->owner == NULL)
    return TRUE;

    owner = NULL;
    for ( wch = char_list; wch != NULL ; wch = wch->next )
        if (!str_cmp(wch->name,obj->owner))
            owner = wch;

    if (owner == NULL)
    return TRUE;

    if (!str_cmp(ch->name,owner->name))
    return TRUE;

    if (!IS_NPC(owner) && IS_SET(owner->act,PLR_CANLOOT))
    return TRUE;

    if(IS_NPC(ch) && !IS_SET(owner->act,PLR_CANLOOT))
    return FALSE;

    if (is_same_group(ch,owner))
    return TRUE;

    return FALSE;
}

bool can_wear_in_form(CHAR_DATA *ch, OBJ_DATA *obj)
{
    /*if(ch->pcdata->shiftform == CRINOS)
    {
        if(CAN_WEAR(obj,ITEM_HOLD)      ||
        CAN_WEAR(obj,ITEM_WIELD)        ||
        CAN_WEAR(obj,ITEM_WEAR_NECK)    ||
        CAN_WEAR(obj,ITEM_WEAR_FLOAT)   ||
        CAN_WEAR(obj,ITEM_WEAR_WRIST)   ||
        CAN_WEAR(obj,ITEM_WEAR_SHIELD)  ||
        CAN_WEAR(obj,ITEM_WEAR_FACE)    ||
        CAN_WEAR(obj,ITEM_WEAR_EAR))
            return TRUE;
        else
            return FALSE;
    }
    if(ch->pcdata->shiftform == LUPUS  || ch->pcdata->shiftform == HISPO)
    {
        if(CAN_WEAR(obj,ITEM_WEAR_NECK) ||
        CAN_WEAR(obj,ITEM_WEAR_FLOAT)   ||
        CAN_WEAR(obj,ITEM_WEAR_EAR))
        return TRUE;
        else return FALSE;
    }
*/
/*  for future expanded use change... less messy repeated crap code.

    if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
    {
        if(IS_OBJ_STAT(obj,ITEM_BLESS)) return FALSE;
    }
    if(is_affected(ch,gsn_vicissitude_horrid))
    {
        if(CAN_WEAR(obj,ITEM_HOLD)      ||
        CAN_WEAR(obj,ITEM_WIELD)        ||
        CAN_WEAR(obj,ITEM_WEAR_FLOAT)   ||
        CAN_WEAR(obj,ITEM_WEAR_SHIELD)  ||
        CAN_WEAR(obj,ITEM_WEAR_FACE))
        return TRUE;
        else return FALSE;
    }
*/
    return TRUE;
}

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{
    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];

    if ( !CAN_WEAR(obj, ITEM_TAKE) )
    {
    send_to_char( "You can't take that.\n\r", ch );
    return;
    }

    if (IS_OBJ_STAT(obj,ITEM_BLESS) && (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")))
    {
    send_to_char( "You are repeled by this object!\n\r", ch );
    return;
    }


    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
    act( "$d: you can't carry that many items.",
        ch, NULL, obj->name, TO_CHAR );
    return;
    }

    if ((!obj->in_obj || obj->in_obj->carried_by != ch)
    &&  (get_carry_weight(ch) + get_obj_weight(obj) > can_carry_w(ch)))
    {
    act( "$d: you can't carry that much weight.",
        ch, NULL, obj->name, TO_CHAR );
    return;
    }

    if (!can_loot(ch,obj))
    {
    act("Corpse looting is not permitted.",ch,NULL,NULL,TO_CHAR );
    return;
    }

    if (obj->in_room != NULL)
    {
    for (gch = obj->in_room->people; gch != NULL; gch = gch->next_in_room)
        if (gch->on == obj)
        {
        act("$N appears to be using $p.",
            ch,obj,gch,TO_CHAR);
        return;
        }
    }


    if ( container != NULL )
    {
        if (container->pIndexData->vnum == OBJ_VNUM_PIT
    &&  get_trust(ch) < obj->level)
    {
        send_to_char("You are not powerful enough to use it.\n\r",ch);
        return;
    }

        if (container->pIndexData->vnum == OBJ_VNUM_PIT
    &&  !CAN_WEAR(container, ITEM_TAKE)
    &&  !IS_OBJ_STAT(obj,ITEM_HAD_TIMER))
        obj->timer = 0;
    act( "You get $p from $P.", ch, obj, container, TO_CHAR );
    act( "$n gets $p from $P.", ch, obj, container, TO_ROOM );
    REMOVE_BIT(obj->extra_flags,ITEM_HAD_TIMER);
    obj_from_obj( obj );
    if (!IS_NPC(ch))
        qitem_check(ch, obj);
    }
    else
    {
    act( "You get $p.", ch, obj, container, TO_CHAR );
    act( "$n gets $p.", ch, obj, container, TO_ROOM );
    obj_from_room( obj );
    if (!IS_NPC(ch))
        qitem_check(ch, obj);
    }

    if ( obj->item_type == ITEM_MONEY)
    {
    ch->silver += obj->value[0];
    ch->gold += obj->value[1];
        if (IS_SET(ch->act,PLR_AUTOSPLIT))
        { /* AUTOSPLIT code */
          members = 0;
          for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
          {
            if (!IS_AFFECTED(gch,AFF_CHARM) && is_same_group( gch, ch ) )
              members++;
          }

      if ( members > 1 && (obj->value[0] > 1 || obj->value[1]))
      {
        sprintf(buffer,"%d %d",obj->value[0],obj->value[1]);
        do_function(ch, &do_split, buffer);
      }
        }

    extract_obj( obj );
    }
    else
    {
    obj_to_char( obj, ch );
    if ( HAS_TRIGGER_OBJ( obj, TRIG_GET ) )
        p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_GET );
    if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GET ) )
        p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GET );
    }

    return;
}



void do_get( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *container;
    OBJ_INDEX_DATA *pObjIndex;  // for get homekey - Ugha
    bool found;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (!str_cmp(arg2,"from"))
    argument = one_argument(argument,arg2);

    /* Get type. */
    if ( arg1[0] == '\0' )
    {
    send_to_char( "Get what?\n\r", ch );
    return;
    }

    if(!str_cmp(arg1, "homekey") && !IS_NPC(ch))
    {
        if(ch->pcdata->home == 0)
        {
            send_to_char("You have no home!\n\r",ch);
            return;
        }

        if ( ( pObjIndex = get_obj_index( ch->pcdata->home ) ) == NULL )
        {
            send_to_char( "Can't find your key! Contact an Immortal.\n\r", ch );
            return;
        }

        obj = create_object( pObjIndex, ch->level );
        obj->timer = 1;
        if ( CAN_WEAR(obj, ITEM_TAKE) )
            obj_to_char( obj, ch );
        else
            obj_to_room( obj, ch->in_room );
        send_to_char("You pull out your house key.\n\r",ch);
        act( "$n pulls a key from somewhere.", ch, NULL, NULL, TO_ROOM );
        return;
    }

if ( (IS_AFFECTED(ch, AFF_SHIFT) && (!is_affected(ch, gsn_vicissitude_horrid) && !is_affected(ch, gsn_vicissitude_chiropteran)) )
     || is_affected(ch,gsn_shadowform) || (!IS_NPC(ch) && (ch->pcdata->shiftform == HISPO || ch->pcdata->shiftform == LUPUS)))
    {
    send_to_char( "You do not have the proper form to pickup things!\n\r", ch );
    return;
    }

    if ( arg2[0] == '\0' )
    {
    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
        /* 'get obj' */
        obj = get_obj_list( ch, arg1, ch->in_room->contents );
        if ( obj == NULL )
        {
        act( "I see no $T here.", ch, NULL, arg1, TO_CHAR );
        return;
        }

        get_obj( ch, obj, NULL );
    }
    else
    {
        /* 'get all' or 'get all.obj' */
        found = FALSE;
        for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
        {
        obj_next = obj->next_content;
        if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
        &&   can_see_obj( ch, obj ) )
        {
            found = TRUE;
            get_obj( ch, obj, NULL );
        }
        }

        if ( !found )
        {
        if ( arg1[3] == '\0' )
            send_to_char( "I see nothing here.\n\r", ch );
        else
            act( "I see no $T here.", ch, NULL, &arg1[4], TO_CHAR );
        }
    }
    }
    else
    {
    /* 'get ... container' */
    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
        send_to_char( "You can't do that.\n\r", ch );
        return;
    }

    if ( ( container = get_obj_here( ch, NULL, arg2 ) ) == NULL )
    {
        act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
        return;
    }

    switch ( container->item_type )
    {
    default:
        send_to_char( "That's not a container.\n\r", ch );
        return;

    case ITEM_CONTAINER:
    case ITEM_CORPSE_NPC:
        break;

    case ITEM_CORPSE_PC:
        {

        if (!can_loot(ch,container))
        {
            send_to_char( "You can't do that.\n\r", ch );
            return;
        }
        }
    }

    if ( IS_SET(container->value[1], CONT_CLOSED) )
    {
        act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
        return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
        /* 'get obj container' */
        obj = get_obj_list( ch, arg1, container->contains );
        if ( obj == NULL )
        {
        act( "I see nothing like that in the $T.",
            ch, NULL, arg2, TO_CHAR );
        return;
        }
        get_obj( ch, obj, container );
    }
    else
    {
        /* 'get all container' or 'get all.obj container' */
        found = FALSE;
        for ( obj = container->contains; obj != NULL; obj = obj_next )
        {
        obj_next = obj->next_content;
        if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
        &&   can_see_obj( ch, obj ) )
        {
            found = TRUE;
            if (container->pIndexData->vnum == OBJ_VNUM_PIT
            &&  !IS_IMMORTAL(ch))
            {
            send_to_char("Don't be so greedy!\n\r",ch);
            return;
            }
            get_obj( ch, obj, container );
        }
        }

        if ( !found )
        {
        if ( arg1[3] == '\0' )
            act( "I see nothing in the $T.",
            ch, NULL, arg2, TO_CHAR );
        else
            act( "I see nothing like that in the $T.",
            ch, NULL, arg2, TO_CHAR );
        }
    }
    }

    return;
}



void do_put( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_DATA *container;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (!str_cmp(arg2,"in") || !str_cmp(arg2,"on"))
    argument = one_argument(argument,arg2);

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Put what in what?\n\r", ch );
    return;
    }

    if ( !str_cmp( arg2, "all" ) || !str_prefix( "all.", arg2 ) )
    {
    send_to_char( "You can't do that.\n\r", ch );
    return;
    }

    if ( ( container = get_obj_here( ch, NULL, arg2 ) ) == NULL )
    {
    act( "I see no $T here.", ch, NULL, arg2, TO_CHAR );
    return;
    }

    if ( container->item_type != ITEM_CONTAINER )
    {
    send_to_char( "That's not a container.\n\r", ch );
    return;
    }

    if ( IS_SET(container->value[1], CONT_CLOSED) )
    {
    act( "The $d is closed.", ch, NULL, container->name, TO_CHAR );
    return;
    }

    if ( str_cmp( arg1, "all" ) && str_prefix( "all.", arg1 ) )
    {
    /* 'put obj container' */
    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( obj == container )
    {
        send_to_char( "You can't fold it into itself.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
        return;
    }

        if (WEIGHT_MULT(obj) != 100)
        {
           send_to_char("You have a feeling that would be a bad idea.\n\r",ch);
            return;
        }

    if (get_obj_weight( obj ) + get_true_weight( container ) > (container->value[0] ))
    {
      send_to_char("That addition would put the container over weight maximum.\n\r", ch);
      return;
    }

    if (get_obj_number(obj) + get_obj_number(container) > (container->value[3] ))
    {
        send_to_char( "The container is already full.\n\r", ch );
        return;
    }

    if (container->pIndexData->vnum == OBJ_VNUM_PIT
    &&  !CAN_WEAR(container,ITEM_TAKE))
        {
            if (obj->timer)
        SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
        else
            obj->timer = number_range(100,200);
        }

    obj_from_char( obj );
    obj_to_obj( obj, container );

    if (IS_SET(container->value[1],CONT_PUT_ON))
    {
        act("$n puts $p on $P.",ch,obj,container, TO_ROOM);
        act("You put $p on $P.",ch,obj,container, TO_CHAR);
    }
    else
    {
        act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
        act( "You put $p in $P.", ch, obj, container, TO_CHAR );
    }
    }
    else
    {
    /* 'put all container' or 'put all.obj container' */
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        if ( ( arg1[3] == '\0' || is_name( &arg1[4], obj->name ) )
        &&   can_see_obj( ch, obj )
        &&   WEIGHT_MULT(obj) == 100
        &&   obj->wear_loc == WEAR_NONE
        &&   obj != container
        &&   can_drop_obj( ch, obj )
        &&   get_obj_weight( obj ) + get_true_weight( container )
         <= (container->value[0] * 10)
        &&   get_obj_weight(obj) < (container->value[3] * 10))
        {
            if (container->pIndexData->vnum == OBJ_VNUM_PIT
            &&  !CAN_WEAR(obj, ITEM_TAKE) )
                {
                    if (obj->timer)
            SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
                else
                        obj->timer = number_range(100,200);
                }
        obj_from_char( obj );
        obj_to_obj( obj, container );

            if (IS_SET(container->value[1],CONT_PUT_ON))
            {
                    act("$n puts $p on $P.",ch,obj,container, TO_ROOM);
                    act("You put $p on $P.",ch,obj,container, TO_CHAR);
            }
        else
        {
            act( "$n puts $p in $P.", ch, obj, container, TO_ROOM );
            act( "You put $p in $P.", ch, obj, container, TO_CHAR );
        }
        }
    }
    }

    return;
}



void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Drop what?\n\r", ch );
    return;
    }

    if ( is_number( arg ) )
    {
    /* 'drop NNNN coins' */
    int amount, gold = 0, silver = 0;

    amount   = atoi(arg);
    argument = one_argument( argument, arg );
    if ( amount <= 0
    || ( str_cmp( arg, "coins" ) && str_cmp( arg, "coin" ) &&
         str_cmp( arg, "gold"  ) && str_cmp( arg, "silver") ) )
    {
        send_to_char( "Sorry, you can't do that.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "coins") || !str_cmp(arg,"coin")
    ||   !str_cmp( arg, "silver"))
    {
        if (ch->silver < amount)
        {
        send_to_char("You don't have that much silver.\n\r",ch);
        return;
        }

        ch->silver -= amount;
        silver = amount;
    }

    else
    {
        if (ch->gold < amount)
        {
        send_to_char("You don't have that much gold.\n\r",ch);
        return;
        }

        ch->gold -= amount;
        gold = amount;
    }

    for ( obj = ch->in_room->contents; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        switch ( obj->pIndexData->vnum )
        {
        case OBJ_VNUM_SILVER_ONE:
        silver += 1;
        extract_obj(obj);
        break;

        case OBJ_VNUM_GOLD_ONE:
        gold += 1;
        extract_obj( obj );
        break;

        case OBJ_VNUM_SILVER_SOME:
        silver += obj->value[0];
        extract_obj(obj);
        break;

        case OBJ_VNUM_GOLD_SOME:
        gold += obj->value[1];
        extract_obj( obj );
        break;

        case OBJ_VNUM_COINS:
        silver += obj->value[0];
        gold += obj->value[1];
        extract_obj(obj);
        break;
        }
    }

    obj_to_room( create_money( gold, silver ), ch->in_room );
    act( "$n drops some coins.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "OK.\n\r", ch );
    return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
    /* 'drop obj' */
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
        return;
    }

    obj_from_char( obj );
    obj_to_room( obj, ch->in_room );
    act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
    act( "You drop $p.", ch, obj, NULL, TO_CHAR );
    if(IS_AFFECTED2(ch, AFF2_UMBRA))
        SET_BIT(obj->extra_flags,ITEM_UMBRA);

        if ( HAS_TRIGGER_OBJ( obj, TRIG_DROP ) )
    	    p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_DROP );
    	if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_DROP ) )
    	    p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_DROP );

    if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
    {
        act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
        act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
        extract_obj(obj);
    }
    }
    else
    {
    /* 'drop all' or 'drop all.obj' */
    found = FALSE;
    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;

        if ( ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
        &&   can_see_obj( ch, obj )
        &&   obj->wear_loc == WEAR_NONE
        &&   can_drop_obj( ch, obj ) )
        {
        found = TRUE;
        obj_from_char( obj );
        obj_to_room( obj, ch->in_room );
        if(IS_AFFECTED2(ch, AFF2_UMBRA))
            SET_BIT(obj->extra_flags,ITEM_UMBRA);
        act( "$n drops $p.", ch, obj, NULL, TO_ROOM );
        act( "You drop $p.", ch, obj, NULL, TO_CHAR );
        if ( HAS_TRIGGER_OBJ( obj, TRIG_DROP ) )
            p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_DROP );
        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_DROP ) )
            p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_DROP );

            if (IS_OBJ_STAT(obj,ITEM_MELT_DROP))
            {
                    act("$p dissolves into smoke.",ch,obj,NULL,TO_ROOM);
                    act("$p dissolves into smoke.",ch,obj,NULL,TO_CHAR);
                    extract_obj(obj);
            }
        }
    }

    if ( !found )
    {
        if ( arg[3] == '\0' )
        act( "You are not carrying anything.",
            ch, NULL, arg, TO_CHAR );
        else
        act( "You are not carrying any $T.",
            ch, NULL, &arg[4], TO_CHAR );
    }
    }

    return;
}



void do_give( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Give what to whom?\n\r", ch );
    return;
    }

    if ( is_number( arg1 ) )
    {
    /* 'give NNNN coins victim' */
    int amount;
    bool silver;

    amount   = atoi(arg1);
    if ( amount <= 0
    || ( str_cmp( arg2, "coins" ) && str_cmp( arg2, "coin" ) &&
         str_cmp( arg2, "gold"  ) && str_cmp( arg2, "silver")) )
    {
        send_to_char( "Sorry, you can't do that.\n\r", ch );
        return;
    }

    silver = str_cmp(arg2,"gold");

    argument = one_argument( argument, arg2 );
    if ( arg2[0] == '\0' )
    {
        send_to_char( "Give what to whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( (!silver && ch->gold < amount) || (silver && ch->silver < amount) )
    {
        send_to_char( "You haven't got that much.\n\r", ch );
        return;
    }

    if (silver)
    {
        ch->silver      -= amount;
        victim->silver  += amount;
    }
    else
    {
        ch->gold        -= amount;
        victim->gold    += amount;
    }

    sprintf(buf,"$n gives you %d %s.",amount, silver ? "silver" : "gold");
    act( buf, ch, NULL, victim, TO_VICT    );
    act( "$n gives $N some coins.",  ch, NULL, victim, TO_NOTVICT );
    sprintf(buf,"You give $N %d %s.",amount, silver ? "silver" : "gold");
    act( buf, ch, NULL, victim, TO_CHAR    );
    /*
     * Bribe trigger
     */
    if ( IS_NPC(victim) && HAS_TRIGGER_MOB( victim, TRIG_BRIBE ) )
        p_bribe_trigger( victim, ch, silver ? amount : amount * 100 );


    if (IS_NPC(victim) && IS_SET(victim->act,ACT_IS_CHANGER))
    {
        int change;

        change = (silver ? 95 * amount / 100 / 100
                 : 95 * amount);


        if (!silver && change > victim->silver)
            victim->silver += change;

        if (silver && change > victim->gold)
        victim->gold += change;

        if (change < 1 && can_see(victim,ch))
        {
        act(
    "$n tells you 'I'm sorry, you did not give me enough to change.'"
            ,victim,NULL,ch,TO_VICT);
        ch->reply = victim;
        sprintf(buf,"%d %s %s",
            amount, silver ? "silver" : "gold",ch->name);
        do_function(victim, &do_give, buf);
        }
        else if (can_see(victim,ch))
        {
        sprintf(buf,"%d %s %s",
            change, silver ? "gold" : "silver",ch->name);
        do_function(victim, &do_give, buf);
        if (silver)
        {
            sprintf(buf,"%d silver %s",
            (95 * amount / 100 - change * 100),ch->name);
            do_function(victim, &do_give, buf);
        }
        act("$n tells you 'Thank you, come again.'",
            victim,NULL,ch,TO_VICT);
        ch->reply = victim;
        }
    }
    return;
    }

    if ( ( obj = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    if ( obj->wear_loc != WEAR_NONE )
    {
    send_to_char( "You must remove it first.\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if (IS_OBJ_STAT(obj,ITEM_BLESS) && (victim->race == race_lookup("vampire") || victim->race == race_lookup("methuselah")))
    {
    send_to_char( "They are repeled by that object!\n\r", ch );
    send_to_char( "You are repeled by that object!\n\r", victim );
    return;
    }


    if (IS_NPC(victim) && victim->pIndexData->pShop != NULL)
    {
    act("$N tells you 'Sorry, you'll have to sell that.'",
        ch,NULL,victim,TO_CHAR);
    ch->reply = victim;
    return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
    send_to_char( "You can't let go of it.\n\r", ch );
    return;
    }

    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
    act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if (get_carry_weight(victim) + get_obj_weight(obj) > can_carry_w( victim ) )
    {
    act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
    return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
    act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
    return;
    }

    obj_from_char( obj );
    obj_to_char( obj, victim );
    MOBtrigger = FALSE;
    act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT );
    act( "$n gives you $p.",   ch, obj, victim, TO_VICT    );
    act( "You give $p to $N.", ch, obj, victim, TO_CHAR    );
    MOBtrigger = TRUE;
    /*
     * Give trigger
     */
    if ( IS_NPC(victim) && HAS_TRIGGER_MOB( victim, TRIG_GIVE ) )
        p_give_trigger( victim, NULL, NULL, ch, obj, TRIG_GIVE );
    if ( HAS_TRIGGER_OBJ( obj, TRIG_GIVE ) )
        p_give_trigger( NULL, obj, NULL, ch, obj, TRIG_GIVE );
    if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_GIVE ) )
        p_give_trigger( NULL, NULL, ch->in_room, ch, obj, TRIG_GIVE );
    return;
}


/* for poisoning weapons and food/drink */
void do_envenom(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    AFFECT_DATA af;
    int skill;
    int dicesuccess = 0;

    /* find out what */
    if (argument[0] == '\0')
    {
    send_to_char("Envenom what item?\n\r",ch);
    return;
    }

    obj =  get_obj_list(ch,argument,ch->carrying);

    if (obj== NULL)
    {
    send_to_char("You don't have that item.\n\r",ch);
    return;
    }

    if ((skill = get_skill(ch,gsn_envenom)) < 1)
    {
    send_to_char("Are you crazy? You'd poison yourself!\n\r",ch);
    return;
    }


    if (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON)
    {
/*Removed fail for bless/burnproof  ---  why couldnt you poison those????
    if (IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF))
    {
        act("You fail to poison $p.",ch,obj,NULL,TO_CHAR);
        return;
    }
*/
    dicesuccess = godice(get_attribute(ch, INTELLIGENCE) + ch->csabilities[CSABIL_MEDICINE], 4);
    WAIT_STATE(ch,skill_table[gsn_envenom].beats);

    if (dicesuccess < 0)
    {
        act("You failed to poison $p, smashing it in your impatience.", ch, obj, NULL, TO_CHAR);
        act("$n smashes $p in $s impatience.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        return;
    }

    else if (dicesuccess == 0)
    {
        act("You fail to properly administer the poison.", ch, NULL, NULL, TO_CHAR);
        return;
    }

    act("$n treats $p with deadly poison.",ch,obj,NULL,TO_ROOM);
    act("You treat $p with deadly poison.",ch,obj,NULL,TO_CHAR);
    if (!obj->value[3])
    {
        obj->value[3] = 1;
        check_improve(ch,gsn_envenom,TRUE,2);
    }
    gain_exp(ch, dicesuccess-1);
    return;
     }

    if (obj->item_type == ITEM_WEAPON)
    {
        if (IS_WEAPON_STAT(obj,WEAPON_FLAMING)
        ||  IS_WEAPON_STAT(obj,WEAPON_FROST)
        ||  IS_WEAPON_STAT(obj,WEAPON_VAMPIRIC)
        ||  IS_WEAPON_STAT(obj,WEAPON_VORPAL)
        ||  IS_WEAPON_STAT(obj,WEAPON_SHOCKING)
/*        ||  IS_OBJ_STAT(obj,ITEM_BLESS) || IS_OBJ_STAT(obj,ITEM_BURN_PROOF) */   )
        {
            act("You can't seem to envenom $p.",ch,obj,NULL,TO_CHAR);
            return;
        }

    if (obj->value[3] < 0
    ||  attack_table[obj->value[3]].damage == DAM_BASH)
    {
        send_to_char("You can only envenom edged weapons.\n\r",ch);
        return;
    }

        if (IS_WEAPON_STAT(obj,WEAPON_POISON))
        {
            act("$p is already envenomed.",ch,obj,NULL,TO_CHAR);
            return;
        }

    WAIT_STATE(ch, skill_table[gsn_envenom].beats);
    dicesuccess = godice(get_attribute(ch, INTELLIGENCE) + ch->csabilities[CSABIL_MEDICINE], 6);

    if (dicesuccess < 0)
    {
        act("In the course of trying to envenom $p, you poison yourself!", ch, obj, NULL, TO_CHAR);
        act("$n mutters a few curses as $e accidently poisons $mself.", ch, obj, NULL, TO_ROOM);
        poison_effect(ch, ch->csabilities[CSABIL_MEDICINE], ch->level, TARGET_CHAR);
        return;
    }

    else if (dicesuccess == 0)
    {
        act("You fail to envenom $p.", ch, obj, NULL, TO_CHAR);
        act("$n fails to envenom $p.", ch, obj, NULL, TO_ROOM);
        return;
    }


            af.where     = TO_WEAPON;
            af.type      = gsn_poison;
            af.level     = ch->csabilities[CSABIL_MEDICINE];
            af.duration  = dicesuccess * 25;
            af.location  = 0;
            af.modifier  = 0;
            af.bitvector = WEAPON_POISON;
            affect_to_obj(obj,&af);

            act("$n coats $p with deadly venom.",ch,obj,NULL,TO_ROOM);
        act("You coat $p with venom.",ch,obj,NULL,TO_CHAR);
        check_improve(ch,gsn_envenom,TRUE,2);
        gain_exp(ch, dicesuccess + 1);
            return;
    }

    act("You can't poison $p.",ch,obj,NULL,TO_CHAR);
    return;
}

void do_fill( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    OBJ_DATA *fountain;
    bool found;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Fill what?\n\r", ch );
    return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    found = FALSE;
    for ( fountain = ch->in_room->contents; fountain != NULL;
    fountain = fountain->next_content )
    {
    if ( (fountain->item_type == ITEM_FOUNTAIN) && SAME_UMBRA_OBJ(ch, fountain) )
    {
        found = TRUE;
        break;
    }
    }

    if ( !found )
    {
    send_to_char( "There is no fountain here!\n\r", ch );
    return;
    }

    if ( obj->item_type != ITEM_DRINK_CON )
    {
    send_to_char( "You can't fill that.\n\r", ch );
    return;
    }

    if ( obj->value[1] != 0 && obj->value[2] != fountain->value[2] )
    {
    send_to_char( "There is already another liquid in it.\n\r", ch );
    return;
    }

    if ( obj->value[1] >= obj->value[0] )
    {
    send_to_char( "Your container is full.\n\r", ch );
    return;
    }

    sprintf(buf,"You fill $p with %s from $P.",
    liq_table[fountain->value[2]].liq_name);
    act( buf, ch, obj,fountain, TO_CHAR );
    sprintf(buf,"$n fills $p with %s from $P.",
    liq_table[fountain->value[2]].liq_name);
    act(buf,ch,obj,fountain,TO_ROOM);
    obj->value[2] = fountain->value[2];
    obj->value[1] = obj->value[0];
    return;
}

void do_pour (CHAR_DATA *ch, char *argument)
{
    char arg[MAX_STRING_LENGTH],buf[MAX_STRING_LENGTH];
    OBJ_DATA *out, *in;
    CHAR_DATA *vch = NULL;
    int amount;

    argument = one_argument(argument,arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
    send_to_char("Pour what into what?\n\r",ch);
    return;
    }


    if ((out = get_obj_carry(ch,arg, ch)) == NULL)
    {
    send_to_char("You don't have that item.\n\r",ch);
    return;
    }

    if (out->item_type != ITEM_DRINK_CON)
    {
    send_to_char("That's not a drink container.\n\r",ch);
    return;
    }

    if (!str_cmp(argument,"out"))
    {
    if (out->value[1] == 0)
    {
        send_to_char("It's already empty.\n\r",ch);
        return;
    }

    out->value[1] = 0;
    out->value[3] = 0;
    sprintf(buf,"You invert $p, spilling %s all over the ground.",
        liq_table[out->value[2]].liq_name);
    act(buf,ch,out,NULL,TO_CHAR);

    sprintf(buf,"$n inverts $p, spilling %s all over the ground.",
        liq_table[out->value[2]].liq_name);
    act(buf,ch,out,NULL,TO_ROOM);
    return;
    }

    if ((in = get_obj_here(ch,NULL, argument)) == NULL)
    {
    vch = get_char_room(ch,NULL,argument);

    if (vch == NULL)
    {
        send_to_char("Pour into what?\n\r",ch);
        return;
    }

    in = get_eq_char(vch,WEAR_HOLD);

    if (in == NULL)
    {
        send_to_char("They aren't holding anything.",ch);
        return;
    }
    }

    if (in->item_type != ITEM_DRINK_CON)
    {
    send_to_char("You can only pour into other drink containers.\n\r",ch);
    return;
    }

    if (in == out)
    {
    send_to_char("You cannot change the laws of physics!\n\r",ch);
    return;
    }

    if (in->value[1] != 0 && in->value[2] != out->value[2])
    {
    send_to_char("They don't hold the same liquid.\n\r",ch);
    return;
    }

    if (out->value[1] == 0)
    {
    act("There's nothing in $p to pour.",ch,out,NULL,TO_CHAR);
    return;
    }

    if (in->value[1] >= in->value[0])
    {
    act("$p is already filled to the top.",ch,in,NULL,TO_CHAR);
    return;
    }

    amount = UMIN(out->value[1],in->value[0] - in->value[1]);

    in->value[1] += amount;
    out->value[1] -= amount;
    in->value[2] = out->value[2];

    if (vch == NULL)
    {
        sprintf(buf,"You pour %s from $p into $P.",
        liq_table[out->value[2]].liq_name);
        act(buf,ch,out,in,TO_CHAR);
        sprintf(buf,"$n pours %s from $p into $P.",
        liq_table[out->value[2]].liq_name);
        act(buf,ch,out,in,TO_ROOM);
    }
    else
    {
        sprintf(buf,"You pour some %s for $N.",
            liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_CHAR);
    sprintf(buf,"$n pours you some %s.",
        liq_table[out->value[2]].liq_name);
    act(buf,ch,NULL,vch,TO_VICT);
        sprintf(buf,"$n pours some %s for $N.",
            liq_table[out->value[2]].liq_name);
        act(buf,ch,NULL,vch,TO_NOTVICT);

    }
}

void do_drink( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int amount;
    int liquid;

    one_argument( argument, arg );

    if((ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")) && !IS_IMMORTAL(ch))
    {
        send_to_char("You cannot drink anything but blood!\n\r" ,ch);
        return;
    }

    if ( arg[0] == '\0' )
    {
    for ( obj = ch->in_room->contents; obj; obj = obj->next_content )
    {
        if ( (obj->item_type == ITEM_FOUNTAIN) && (SAME_UMBRA_OBJ(ch, obj)) )
        break;
    }

    if (( obj == NULL ) || (!SAME_UMBRA_OBJ(ch, obj)))
    {
        send_to_char( "Drink what?\n\r", ch );
        return;
    }
    }
    else
    {
    if ( ( obj = get_obj_here( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    }

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10 )
    {
    send_to_char( "You fail to reach your mouth.  *Hic*\n\r", ch );
    return;
    }

    switch ( obj->item_type )
    {
    default:
    send_to_char( "You can't drink from that.\n\r", ch );
    return;

    case ITEM_FOUNTAIN:
        if ( ( liquid = obj->value[2] )  < 0 )
        {
            bug( "Do_drink: bad liquid number %d.", liquid );
            liquid = obj->value[2] = 0;
        }
    amount = liq_table[liquid].liq_affect[4] * 3;
    break;

    case ITEM_DRINK_CON:
    if ( obj->value[1] <= 0 )
    {
        send_to_char( "It is already empty.\n\r", ch );
        return;
    }

    if ( ( liquid = obj->value[2] )  < 0 )
    {
        bug( "Do_drink: bad liquid number %d.", liquid );
        liquid = obj->value[2] = 0;
    }

        amount = liq_table[liquid].liq_affect[4];
        amount = UMIN(amount, obj->value[1]);
    break;
     }
    if (!IS_NPC(ch) && !IS_IMMORTAL(ch)
    &&  ch->pcdata->condition[COND_FULL] > 45)
    {
    send_to_char("You're too full to drink more.\n\r",ch);
    return;
    }

    act( "$n drinks $T from $p.",
    ch, obj, liq_table[liquid].liq_name, TO_ROOM );
    act( "You drink $T from $p.",
    ch, obj, liq_table[liquid].liq_name, TO_CHAR );

    gain_condition( ch, COND_DRUNK,
    amount * liq_table[liquid].liq_affect[COND_DRUNK] / 36 );
    gain_condition( ch, COND_FULL,
    amount * liq_table[liquid].liq_affect[COND_FULL] / 4 );
    gain_condition( ch, COND_THIRST,
    amount * liq_table[liquid].liq_affect[COND_THIRST] / 10 );
    gain_condition(ch, COND_HUNGER,
    amount * liq_table[liquid].liq_affect[COND_HUNGER] / 2 );

    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK]  > 10 )
    send_to_char( "You feel drunk.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL]   > 40 )
    send_to_char( "You are full.\n\r", ch );
    if ( !IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] > 40 )
    send_to_char( "Your thirst is quenched.\n\r", ch );

    if ( obj->value[3] != 0 )
    {
    /* The drink was poisoned ! */
    AFFECT_DATA af;

    act( "$n chokes and gags.", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You choke and gag.\n\r", ch );
    af.where     = TO_AFFECTS;
    af.type      = gsn_poison;
    af.level     = number_fuzzy(amount);
    af.duration  = 3 * amount;
    af.location  = APPLY_NONE;
    af.modifier  = 0;
    af.bitvector = AFF_POISON;
    affect_join( ch, &af );
    }

    if (obj->value[0] > 0)
        obj->value[1] -= amount;

    return;
}



void do_eat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
    send_to_char( "Eat what?\n\r", ch );
    return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    if ( !IS_IMMORTAL(ch) )
    {
      if ( obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL )
      {
        send_to_char( "That's not edible.\n\r", ch );
        return;
      }

      if ( !IS_NPC(ch) && ch->pcdata->condition[COND_FULL] > 40 )
      {
        send_to_char( "You are too full to eat more.\n\r", ch );
        return;
      }
    }

    if((ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")) && !IS_IMMORTAL(ch) && obj->item_type != ITEM_PILL)
    {
        send_to_char("You cannot eat things!\n\r" ,ch);
        return;
    }

    if (obj->item_type == ITEM_PILL && obj->level > ch->level)
    {
      send_to_char("That item is too powerful to eat.\n\r", ch);
      return;
    }

    act( "$n eats $p.",  ch, obj, NULL, TO_ROOM );
    act( "You eat $p.", ch, obj, NULL, TO_CHAR );

    switch ( obj->item_type )
    {
      case ITEM_FOOD:
        if ( !IS_NPC(ch) )
        {
          int condition;

          condition = ch->pcdata->condition[COND_HUNGER];
          gain_condition( ch, COND_FULL, obj->value[0] );
          gain_condition( ch, COND_HUNGER, obj->value[1]);
          if ( condition == 0 && ch->pcdata->condition[COND_HUNGER] > 0 )
            send_to_char( "You are no longer hungry.\n\r", ch );
          else if ( ch->pcdata->condition[COND_FULL] > 40 )
          send_to_char( "You are full.\n\r", ch );
        }

        if ( obj->value[3] != 0 )
        {
        /* The food was poisoned! */
          AFFECT_DATA af;

          act( "$n chokes and gags.", ch, 0, 0, TO_ROOM );
          send_to_char( "You choke and gag.\n\r", ch );

          af.where     = TO_AFFECTS;
          af.type      = gsn_poison;
          af.level     = number_fuzzy(obj->value[0]);
          af.duration  = 2 * obj->value[0];
          af.location  = APPLY_NONE;
          af.modifier  = 0;
          af.bitvector = AFF_POISON;
          affect_join( ch, &af );
        }
        break;

      case ITEM_PILL:
        obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
        obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
        obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );
        break;
    }

    extract_obj( obj );
    return;
}



/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
    return TRUE;

    if ( !fReplace )
    return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
    act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
    return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}



/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    char buf[MAX_STRING_LENGTH];

    if(!IS_NPC(ch) && !can_wear_in_form(ch,obj))
    {
        send_to_char("You can't wear that in this form.\n\r",ch);
        return;
    }
    if ( ch->level < obj->level )
    {
    sprintf( buf, "You must be level %d to use this object.\n\r",
        obj->level );
    send_to_char( buf, ch );
    act( "$n tries to use $p, but is too inexperienced.",
        ch, obj, NULL, TO_ROOM );
    return;
    }

    if (!canwear_objaffect(ch, obj))
    {
        sendch("You are not strong enough to wear this equipment!\n\r", ch);
        act( "$n tries to use $p, but is not strong enough!", ch, obj, NULL, TO_ROOM);
        return;
    }

    if ( obj->item_type == ITEM_LIGHT )
    {
    if ( !remove_obj( ch, WEAR_LIGHT, fReplace ) )
        return;
    act( "$n lights $p and holds it.", ch, obj, NULL, TO_ROOM );
    act( "You light $p and hold it.",  ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_LIGHT );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
    {

    if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
    &&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
    &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
    &&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
        return;

    if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
    {
        act( "$n wears $p on $s left finger.",    ch, obj, NULL, TO_ROOM );
        act( "You wear $p on your left finger.",  ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_FINGER_L );
        return;
    }

    if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
    {
        act( "$n wears $p on $s right finger.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p on your right finger.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_FINGER_R );
        return;
    }

    bug( "Wear_obj: no free finger.", 0 );
    send_to_char( "You already wear two rings.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
    {

    if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
    &&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
    &&   !remove_obj( ch, WEAR_NECK_1, fReplace )
    &&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
        return;

    if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
    {
        act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_NECK_1 );
        return;
    }

    if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
    {
        act( "$n wears $p around $s neck.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p around your neck.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_NECK_2 );
        return;
    }

    bug( "Wear_obj: no free neck.", 0 );
    send_to_char( "You already wear two neck items.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_TORSO ) )
    {

    if ( !remove_obj( ch, WEAR_TORSO, fReplace ) )
        return;
    act( "$n wears $p on $s torso.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your torso.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_TORSO );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
    {

    if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
        return;
    act( "$n wears $p on $s head.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your head.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_HEAD );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
    {

    if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
        return;
    act( "$n wears $p on $s legs.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your legs.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_LEGS );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
    {

    if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
        return;
    act( "$n wears $p on $s feet.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your feet.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_FEET );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
    {

    if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
        return;
    act( "$n wears $p on $s hands.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your hands.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_HANDS );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
    {

    if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
        return;
    act( "$n wears $p on $s arms.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your arms.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_ARMS );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
    {

    if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
        return;
    act( "$n wears $p about $s torso.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p about your torso.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_BODY );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
    {
    if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
        return;
    act( "$n wears $p about $s waist.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p about your waist.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_WAIST );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
    {

    if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
    &&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
    &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
    &&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
        return;

    if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
    {
        act( "$n wears $p around $s left wrist.",
        ch, obj, NULL, TO_ROOM );
        act( "You wear $p around your left wrist.",
        ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_WRIST_L );
        return;
    }

    if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
    {
        act( "$n wears $p around $s right wrist.",
        ch, obj, NULL, TO_ROOM );
        act( "You wear $p around your right wrist.",
        ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_WRIST_R );
        return;
    }

    bug( "Wear_obj: no free wrist.", 0 );
    send_to_char( "You already wear two wrist items.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
    OBJ_DATA *weapon;

    if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
        return;

    weapon = get_eq_char(ch,WEAR_WIELD);
    if (weapon != NULL && ch->size < SIZE_LARGE
    &&  IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS))
    {
        if(get_eq_char(ch,WEAR_HOLD) != NULL)
        {
            send_to_char("Your hands are tied up with your weapon and with what you're holding!\n\r",ch);
            return;
        }
    }

    act( "$n wears $p as a shield.", ch, obj, NULL, TO_ROOM );
    act( "You wear $p as a shield.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_SHIELD );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
    int sn,skill;

    if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
        return;

    if (!IS_NPC(ch) && ch->size < SIZE_LARGE
    &&  IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)
    &&  get_eq_char(ch,WEAR_SHIELD) != NULL
    && get_eq_char(ch,WEAR_HOLD) != NULL)
    {
        send_to_char("You need two hands free for that weapon.\n\r",ch);
        return;
    }
    act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
    act( "You wield $p.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_WIELD );
        sn = get_weapon_sn(ch);

    if (sn == gsn_hand_to_hand)
       return;

        skill = get_weapon_skill(ch,sn);

        if (skill >= 100)
            act("$p feels like a part of you!",ch,obj,NULL,TO_CHAR);
        else if (skill > 85)
            act("You feel quite confident with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 70)
            act("You are skilled with $p.",ch,obj,NULL,TO_CHAR);
        else if (skill > 50)
            act("Your skill with $p is adequate.",ch,obj,NULL,TO_CHAR);
        else if (skill > 25)
            act("$p feels a little clumsy in your hands.",ch,obj,NULL,TO_CHAR);
        else if (skill > 1)
            act("You fumble and almost drop $p.",ch,obj,NULL,TO_CHAR);
        else
            act("You don't even know which end is up on $p.",
                ch,obj,NULL,TO_CHAR);

    return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
    OBJ_DATA *weapon;
    if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
        return;
    weapon = get_eq_char(ch,WEAR_WIELD);

    if (weapon != NULL && ch->size < SIZE_LARGE
    &&  IS_WEAPON_STAT(weapon,WEAPON_TWO_HANDS)
    && get_eq_char(ch,WEAR_SHIELD) != NULL)
    {
        send_to_char("Your hands are tied up with your weapon!\n\r",ch);
        return;
    }

    act( "$n holds $p in $s hand.",   ch, obj, NULL, TO_ROOM );
    act( "You hold $p in your hand.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_HOLD );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_EAR ) )
    {
    /*if(is_affected(ch,gsn_vicissitude_horrid))
    {
        send_to_char("You cannot wear that in this form.\n\r",ch);
        return;
    }*/

    if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
    &&   get_eq_char( ch, WEAR_EAR_R ) != NULL
    &&   !remove_obj( ch, WEAR_EAR_L, fReplace )
    &&   !remove_obj( ch, WEAR_EAR_R, fReplace ) )
        return;

    if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
    {
        act( "$n wears $p in $s ear.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p in your ear.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_EAR_L );
        return;
    }

    if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
    {
        act( "$n wears $p in $s ear.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p in your ear.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_EAR_R );
        return;
    }

    bug( "Wear_obj: no free ear.", 0 );
    send_to_char( "You already wear two ear items.\n\r", ch );
    return;
    }

    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
    {
    /*if(is_affected(ch,gsn_vicissitude_horrid))
    {
        send_to_char("You cannot wear that in this form.\n\r",ch);
        return;
    }*/

    if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
        return;
    act( "$n wears $p on $s face.",   ch, obj, NULL, TO_ROOM );
    act( "You wear $p on your face.", ch, obj, NULL, TO_CHAR );
    equip_char( ch, obj, WEAR_FACE );
    return;
    }

    if ( CAN_WEAR(obj,ITEM_WEAR_FLOAT) )
    {
    if (!remove_obj(ch,WEAR_FLOAT, fReplace) )
        return;
    act("$n releases $p to float next to $m.",ch,obj,NULL,TO_ROOM);
    act("You release $p and it floats next to you.",ch,obj,NULL,TO_CHAR);
    equip_char(ch,obj,WEAR_FLOAT);
    return;
    }

    if ( CAN_WEAR(obj,ITEM_WEAR_BACK) )
    {
    if (!remove_obj(ch,WEAR_BACK, fReplace) )
        return;
    act("$n wears $p on $s back.",ch,obj,NULL,TO_ROOM);
    act("You wear $p on your back.",ch,obj,NULL,TO_CHAR);
    equip_char(ch,obj,WEAR_BACK);
    return;
    }

    if ( fReplace )
    send_to_char( "You can't wear, wield, or hold that.\n\r", ch );

    return;
}

void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Wear, wield, or hold what?\n\r", ch );
    return;
    }
/*
if ( IS_AFFECTED(ch, AFF_SHIFT) || ch->changed == CHANGED_LUPUS || is_affected(ch,gsn_shadowform))
    {
    send_to_char( "You cannot wear things in this form!\n\r", ch );
    return;
    }*/
    if ( !str_cmp( arg, "all" ) )
    {
    OBJ_DATA *obj_next;

    for ( obj = ch->carrying; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
        {
/*          if (ch->changed == CHANGED_CRINOS)
                {
                    if(IS_SET(obj->wear_flags,  ITEM_WEAR_TORSO) ||
                        IS_SET(obj->wear_flags,  ITEM_HOLD) ||
                        IS_SET(obj->wear_flags,  ITEM_WIELD) ||
                        IS_SET(obj->wear_flags,  ITEM_LIGHT) ||
                        IS_SET(obj->wear_flags,  ITEM_WEAR_NECK) ||
                        IS_SET(obj->wear_flags,  ITEM_WEAR_FLOAT) ||
                        IS_SET(obj->wear_flags,  ITEM_WEAR_WRIST))
                                wear_obj( ch, obj, FALSE );
                }
            else
            */if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
                {
                    if(!IS_OBJ_STAT(obj,ITEM_BLESS))
                        wear_obj( ch, obj, FALSE );
                }
            /*else if(is_affected(ch,gsn_vicissitude_horrid))
                {
                    if(IS_SET(obj->wear_flags, ITEM_LIGHT) || IS_SET(obj->wear_flags, ITEM_HOLD) || IS_SET(obj->wear_flags,  ITEM_WIELD) || IS_SET(obj->wear_flags, ITEM_WEAR_FLOAT))
                    wear_obj(ch,obj,FALSE);
                } */
            else
                wear_obj( ch, obj, FALSE );
        }
    }
    return;
    }
    else
    {
    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

if (IS_OBJ_STAT(obj,ITEM_BLESS) && (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")))
    {
    send_to_char( "You are repeled by that object!\n\r", ch );
    return;
    }

    wear_obj( ch, obj, TRUE );
    }

    return;
}



void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
        one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Remove what?\n\r", ch );
        return;
    }
        if ( !str_cmp( arg, "all" ) )
        {
        OBJ_DATA *obj_next;
        for ( obj = ch->carrying; obj != NULL; obj = obj_next )
        {
                obj_next = obj->next_content;
                if(obj->wear_loc != WEAR_NONE) remove_obj(ch,obj->wear_loc, TRUE);
        }
        return;
        }
        else
    {
        if ( ( obj = get_obj_wear( ch, arg, TRUE ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }
    remove_obj( ch, obj->wear_loc, TRUE );
    return;
        }
}



/* void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Remove what?\n\r", ch );
    return;
    }

    if ( ( obj = get_obj_wear( ch, arg , TRUE) ) == NULL )
    {
    send_to_char( "You do not have that item.\n\r", ch );
    return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
    return;
} */

bool acceptable_sacrifice(CHAR_DATA *ch, OBJ_DATA *obj)
{
  CHAR_DATA *fch;

  if ( obj->item_type == ITEM_CORPSE_PC && obj->contains)
     return FALSE;

  if ( !CAN_WEAR(obj, ITEM_TAKE) || CAN_WEAR(obj, ITEM_NO_SAC))
  return FALSE;

  if (obj->in_room != NULL)
  {
  for (fch = obj->in_room->people; fch != NULL; fch = fch->next_in_room)
      if (fch->on == obj)
        return FALSE;
  }

  return TRUE;

}

void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char* name;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj, *obj_next;
    int silver, total, count;

    /* variables for AUTOSPLIT */
    CHAR_DATA *gch;
    int members;
    char buffer[100];


    one_argument( argument, arg );

    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
    {
    act( "$n offers $mself to the Immortals, who graciously decline.",
        ch, NULL, NULL, TO_ROOM );
    send_to_char(
        "The Immortals appreciate your offer and may accept it later.\n\r", ch );
    return;
    }

    if (!str_cmp(arg, "all"))
    {
      silver = total = count = 0;

      for (obj = ch->in_room->contents; obj; obj = obj_next)
      {
        if (!obj)
          break;

        obj_next = obj->next_content;

        if (!acceptable_sacrifice(ch, obj))
          continue;

        silver = UMAX(1,obj->level * 3);
        count++;
        if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
            silver = UMIN(silver,obj->cost);

        total += silver;
        extract_obj(obj);
      }

      if (IS_SET(ch->act,PLR_AUTOSPLIT) )
      { /* AUTOSPLIT code */
          members = 0;
          for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
              {
                  if ( is_same_group( gch, ch ) )
                  members++;
              }

          ch->silver += total;
          sprintf(buffer,"You sacrifice %d items to the gods! You receive %d silver!\n\r", count, total);
          sendch(buffer, ch);

          if ( members > 1 && total > 1)
          {
              sprintf(buffer,"%d",total);
              do_function(ch, &do_split, buffer);
          }
      }

      act( "$n sacrifices a number of items to the Immortals.", ch, NULL, NULL, TO_ROOM );
      wiznet("$N sacrificed the whole room!",
         ch,NULL,WIZ_SACCING,0,0);
      return;
    }

    obj = get_obj_list( ch, arg, ch->in_room->contents );
    if ( obj == NULL )
    {
    send_to_char( "You can't find it.\n\r", ch );
    return;
    }

    if (!acceptable_sacrifice(ch, obj))
    {
      send_to_char("That item cannot be sacrificed to the Gods.\n\r", ch);
      return;
    }

    silver = UMAX(1,obj->level * 3);

    if (obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
        silver = UMIN(silver,obj->cost);

    if (silver == 1)
        send_to_char(
        "The Immortals give you one silver coin for your sacrifice.\n\r", ch );
    else
    {
    sprintf(buf,"The Immortals give you %d silver coins for your sacrifice.\n\r",
        silver);
    send_to_char(buf,ch);
    }

    ch->silver += silver;

    if (IS_SET(ch->act,PLR_AUTOSPLIT) )
    { /* AUTOSPLIT code */
        members = 0;
    for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
        {
            if ( is_same_group( gch, ch ) )
            members++;
        }

    if ( members > 1 && silver > 1)
    {
        sprintf(buffer,"%d",silver);
        do_function(ch, &do_split, buffer);
    }
    }

    act( "$n sacrifices $p to the Immortals.", ch, obj, NULL, TO_ROOM );
    wiznet("$N sends up $p as a burnt offering.",
       ch,obj,WIZ_SACCING,0,0);
    extract_obj( obj );
    return;
}



void do_quaff( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;

    one_argument( argument, arg );

    if (IS_NPC(ch))
        return;

    if ( arg[0] == '\0' )
    {
    send_to_char( "Quaff what?\n\r", ch );
    return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
    send_to_char( "You do not have that potion.\n\r", ch );
    return;
    }

    if ( obj->item_type != ITEM_POTION )
    {
    send_to_char( "You can quaff only potions.\n\r", ch );
    return;
    }

    if (ch->level < obj->level)
    {
    send_to_char("This liquid is too powerful for you to drink.\n\r",ch);
    return;
    }

    if (!IS_IMMORTAL(ch) && ch->pcdata->condition[COND_FULL] > 45)
    {
    send_to_char("You are too full to quaff this.\n\r",ch);
    return;
    }

    act( "$n quaffs $p.", ch, obj, NULL, TO_ROOM );
    act( "You quaff $p.", ch, obj, NULL ,TO_CHAR );
    ch->pcdata->condition[COND_FULL] += 5;

    if( ch->pcdata->condition[COND_FULL] > 100)
    ch->pcdata->condition[COND_FULL] = 100;

    obj_cast_spell( obj->value[1], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[2], obj->value[0], ch, ch, NULL );
    obj_cast_spell( obj->value[3], obj->value[0], ch, ch, NULL );

    extract_obj( obj );
    return;
}



void do_recite( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *scroll;
    OBJ_DATA *obj;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( ( scroll = get_obj_carry( ch, arg1, ch ) ) == NULL )
    {
    send_to_char( "You do not have that scroll.\n\r", ch );
    return;
    }

    if ( scroll->item_type != ITEM_SCROLL )
    {
    send_to_char( "You can recite only scrolls.\n\r", ch );
    return;
    }

    if ( ch->level < scroll->level)
    {
    send_to_char(
        "This scroll is too complex for you to comprehend.\n\r",ch);
    return;
    }

    obj = NULL;
    if ( arg2[0] == '\0' )
    {
    victim = ch;
    }
    else
    {
    if ( ( victim = get_char_room ( ch, NULL, arg2 ) ) == NULL
    &&   ( obj    = get_obj_here  ( ch, NULL, arg2 ) ) == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    }

    act( "$n recites $p.", ch, scroll, NULL, TO_ROOM );
    act( "You recite $p.", ch, scroll, NULL, TO_CHAR );

    /* Lag on scrolls added to prevent abuse - Ugha */
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if (number_percent() >= 20 + get_skill(ch,gsn_scrolls) * 4/5)
    {
    send_to_char("You mispronounce a syllable.\n\r",ch);
    check_improve(ch,gsn_scrolls,FALSE,2);
    }

    else
    {
        obj_cast_spell( scroll->value[1], scroll->value[0], ch, victim, obj );
        obj_cast_spell( scroll->value[2], scroll->value[0], ch, victim, obj );
        obj_cast_spell( scroll->value[3], scroll->value[0], ch, victim, obj );
    check_improve(ch,gsn_scrolls,TRUE,2);
    }

    extract_obj( scroll );
    return;
}



void do_brandish( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    OBJ_DATA *staff;
    int sn;

    if ( ( staff = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
    send_to_char( "You hold nothing in your hand.\n\r", ch );
    return;
    }

    if ( staff->item_type != ITEM_STAFF )
    {
    send_to_char( "You can brandish only with a staff.\n\r", ch );
    return;
    }

    if ( ( sn = staff->value[3] ) < 0
    ||   sn >= MAX_SKILL
    ||   skill_table[sn].spell_fun == 0 )
    {
    bug( "Do_brandish: bad sn %d.", sn );
    return;
    }

    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( staff->value[2] > 0 )
    {
    act( "$n brandishes $p.", ch, staff, NULL, TO_ROOM );
    act( "You brandish $p.",  ch, staff, NULL, TO_CHAR );
    if ( ch->level < staff->level
    ||   number_percent() >= 20 + get_skill(ch,gsn_staves) * 4/5)
    {
        act ("You fail to invoke $p.",ch,staff,NULL,TO_CHAR);
        act ("...and nothing happens.",ch,NULL,NULL,TO_ROOM);
        check_improve(ch,gsn_staves,FALSE,2);
    }

    else for ( vch = ch->in_room->people; vch; vch = vch_next )
    {
        vch_next    = vch->next_in_room;

        switch ( skill_table[sn].target )
        {
        default:
        bug( "Do_brandish: bad target for sn %d.", sn );
        return;

        case TAR_IGNORE:
        if ( vch != ch )
            continue;
        break;

        case TAR_CHAR_OFFENSIVE:
        if ( IS_NPC(ch) ? IS_NPC(vch) : !IS_NPC(vch) )
            continue;
        break;

        case TAR_CHAR_DEFENSIVE:
        if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch) )
            continue;
        break;

        case TAR_CHAR_SELF:
        if ( vch != ch )
            continue;
        break;
        }

        obj_cast_spell( staff->value[3], staff->value[0], ch, vch, NULL );
        check_improve(ch,gsn_staves,TRUE,2);
    }
    }

    if ( --staff->value[2] <= 0 )
    {
    act( "$n's $p blazes bright and is gone.", ch, staff, NULL, TO_ROOM );
    act( "Your $p blazes bright and is gone.", ch, staff, NULL, TO_CHAR );
    extract_obj( staff );
    }

    return;
}



void do_zap( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *wand;
    OBJ_DATA *obj;

    one_argument( argument, arg );
    if ( arg[0] == '\0' && ch->fighting == NULL )
    {
    send_to_char( "Zap whom or what?\n\r", ch );
    return;
    }

    if ( ( wand = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
    send_to_char( "You hold nothing in your hand.\n\r", ch );
    return;
    }

    if ( wand->item_type != ITEM_WAND )
    {
    send_to_char( "You can zap only with a wand.\n\r", ch );
    return;
    }

    obj = NULL;
    if ( arg[0] == '\0' )
    {
    if ( ch->fighting != NULL )
    {
        victim = ch->fighting;
    }
    else
    {
        send_to_char( "Zap whom or what?\n\r", ch );
        return;
    }
    }
    else
    {
    if ( ( victim = get_char_room ( ch, NULL, arg ) ) == NULL
    &&   ( obj    = get_obj_here  ( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "You can't find it.\n\r", ch );
        return;
    }
    }
    if(wand->value[2] <= 0)
    {
        send_to_char("This object has no magical energy remaining.\n\r",ch);
        return;
    }
    WAIT_STATE( ch, 2 * PULSE_VIOLENCE );

    if ( wand->value[2] > 0 )
    {
    if ( victim != NULL )
    {
        act( "$n zaps $N with $p.", ch, wand, victim, TO_NOTVICT );
        act( "You zap $N with $p.", ch, wand, victim, TO_CHAR );
        act( "$n zaps you with $p.",ch, wand, victim, TO_VICT );
    }
    else
    {
        act( "$n zaps $P with $p.", ch, wand, obj, TO_ROOM );
        act( "You zap $P with $p.", ch, wand, obj, TO_CHAR );
    }

    if (ch->level < wand->level
    ||  number_percent() >= 20 + get_skill(ch,gsn_wands) * 4/5)
    {
        act( "Your efforts with $p produce only smoke and sparks.",
         ch,wand,NULL,TO_CHAR);
        act( "$n's efforts with $p produce only smoke and sparks.",
         ch,wand,NULL,TO_ROOM);
        check_improve(ch,gsn_wands,FALSE,2);
    }
    else
    {
        obj_cast_spell( wand->value[3], wand->value[0], ch, victim, obj );
        check_improve(ch,gsn_wands,TRUE,2);
    }
    }

    if ( --wand->value[2] <= 0 )
    {
    act( "$n's $p explodes into fragments.", ch, wand, NULL, TO_ROOM );
    act( "Your $p explodes into fragments.", ch, wand, NULL, TO_CHAR );
    extract_obj( wand );
    }

    return;
}



void do_steal( CHAR_DATA *ch, char *argument )
{
    char buf  [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    int percent;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
    send_to_char( "Steal what from whom?\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg2 ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }
    if(!IS_NPC(ch) && !IS_NPC(victim))
    {
    send_to_char( "Due to abuse, player stealing is removed. Do it and die pig!\n\r", ch );
    return;
    }

      if ( IS_AFFECTED(victim, AFF2_MIST) || IS_AFFECTED(ch, AFF2_MIST) || is_affected(ch,gsn_shadowform) || is_affected(victim,gsn_shadowform))
    {
        send_to_char( "Your hand passes right through them!\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
    send_to_char( "That's pointless.\n\r", ch );
    return;
    }

    if (is_safe(ch,victim))
    return;

    if ( IS_NPC(victim)
      && victim->position == POS_FIGHTING)
    {
    send_to_char(  "Kill stealing is not permitted.\n\r"
               "You'd better not -- you might get hit.\n\r",ch);
    return;
    }

    WAIT_STATE( ch, skill_table[gsn_steal].beats );
    percent  = number_percent();

    if (!IS_AWAKE(victim))
        percent -= 10;
    else if (!can_see(victim,ch))
        percent += 25;
    else
    percent += 50;

    if ( ((ch->level + 7 < victim->level || ch->level -7 > victim->level)
    && !IS_NPC(victim) && !IS_NPC(ch) )
    || ( !IS_NPC(ch) && percent > get_skill(ch,gsn_steal))
    || ( !IS_NPC(ch) && !is_clan(ch)) )
    {
    /*
     * Failure.
     */
    send_to_char( "Oops.\n\r", ch );
    affect_strip(ch,gsn_sneak);
    REMOVE_BIT(ch->affected_by,AFF_SNEAK);

    act( "$n tried to steal from you.\n\r", ch, NULL, victim, TO_VICT    );
    act( "$n tried to steal from $N.\n\r",  ch, NULL, victim, TO_NOTVICT );
    switch(number_range(0,3))
    {
    case 0 :
       sprintf( buf, "%s is a lousy thief!", ch->name );
       break;
        case 1 :
       sprintf( buf, "%s couldn't rob %s way out of a paper bag!",
            ch->name,(ch->sex == 2) ? "her" : "his");
       break;
    case 2 :
        sprintf( buf,"%s tried to rob me!",ch->name );
        break;
    case 3 :
        sprintf(buf,"Keep your hands out of there, %s!",ch->name);
        break;
        }
        if (!IS_AWAKE(victim))
            do_function(victim, &do_wake, "");
    if (IS_AWAKE(victim))
        do_function(victim, &do_yell, buf );
    if ( !IS_NPC(ch) )
    {
        if ( IS_NPC(victim) )
        {
            check_improve(ch,gsn_steal,FALSE,2);
        multi_hit( victim, ch, TYPE_UNDEFINED );
        }
        else
        {
/*      sprintf(buf,"$N tried to steal from %s.",victim->name);
        wiznet(buf,ch,NULL,WIZ_FLAGS,0,0);
        if ( !IS_SET(ch->act, PLR_THIEF) )
        {
            SET_BIT(ch->act, PLR_THIEF);
            send_to_char( "*** You are now a THIEF!! ***\n\r", ch );
            save_char_obj( ch );
        }
*/
        }
    }

    return;
    }

    if ( !str_cmp( arg1, "coin"  )
    ||   !str_cmp( arg1, "coins" )
    ||   !str_cmp( arg1, "gold"  )
    ||   !str_cmp( arg1, "silver"))
    {
    int gold, silver;

    gold = victim->gold * number_range(1, ch->level) / MAX_LEVEL;
    silver = victim->silver * number_range(1,ch->level) / MAX_LEVEL;
    if ( gold <= 0 && silver <= 0 )
    {
        send_to_char( "You couldn't get any coins.\n\r", ch );
        return;
    }

    ch->gold        += gold;
    ch->silver      += silver;
    victim->silver  -= silver;
    victim->gold    -= gold;
    if (silver <= 0)
        sprintf( buf, "Bingo!  You got %d gold coins.\n\r", gold );
    else if (gold <= 0)
        sprintf( buf, "Bingo!  You got %d silver coins.\n\r",silver);
    else
        sprintf(buf, "Bingo!  You got %d silver and %d gold coins.\n\r",
            silver,gold);

    send_to_char( buf, ch );
    check_improve(ch,gsn_steal,TRUE,2);
    return;
    }

    if ( ( obj = get_obj_carry( victim, arg1, ch ) ) == NULL )
    {
    send_to_char( "You can't find it.\n\r", ch );
    return;
    }

    if ( !can_drop_obj( ch, obj )
    ||   IS_SET(obj->extra_flags, ITEM_INVENTORY)
    ||   obj->level > ch->level )
    {
    send_to_char( "You can't pry it away.\n\r", ch );
    return;
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
    send_to_char( "You have your hands full.\n\r", ch );
    return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
    send_to_char( "You can't carry that much weight.\n\r", ch );
    return;
    }

    obj_from_char( obj );
    obj_to_char( obj, ch );
    act("You pocket $p.",ch,obj,NULL,TO_CHAR);
    check_improve(ch,gsn_steal,TRUE,2);
    send_to_char( "Got it!\n\r", ch );
    if (!IS_NPC(ch))
        qitem_check(ch, obj);
    return;
}



/*
 * Shopping commands.
 */
CHAR_DATA *find_keeper( CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *keeper;
    SHOP_DATA *pShop;
    char *open;
    char *close;

    pShop = NULL;
    for ( keeper = ch->in_room->people; keeper; keeper = keeper->next_in_room )
    {
    if ( IS_NPC(keeper) && (pShop = keeper->pIndexData->pShop) != NULL )
        break;
    }

    if ( pShop == NULL )
    {
    send_to_char( "You can't do that here.\n\r", ch );
    return NULL;
    }

    /*
     * Invisible or hidden people.
     */
    if ( !can_see( keeper, ch ) )
    {
    do_function(keeper, &do_say, "I don't trade with folks I can't see.");
    return NULL;
    }

    /*
     * Undesirables.
     *
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_KILLER) )
    {
    do_function(keeper, &do_say, "Killers are not welcome!");
    sprintf(buf, "%s the KILLER is over here!\n\r", ch->name);
    do_function(keeper, &do_yell, buf );
    return NULL;
    }

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_THIEF) )
    {
    do_function(keeper, &do_say, "Thieves are not welcome!");
    sprintf(buf, "%s the THIEF is over here!\n\r", ch->name);
    do_function(keeper, &do_yell, buf );
    return NULL;
    }
    */
    /*
     * Shop hours.
     */
    open = str_dup(get_time_string(pShop->open_hour));
    close = str_dup(get_time_string(pShop->close_hour));

    if ( pShop->open_hour < pShop->close_hour )
    {
      if ( time_info.hour < pShop->open_hour )
      {
        do_function(keeper, &do_say, "Sorry, I am closed. Come back later.");
        sprintf(buf, "My hours of operation are from %s to %s.", open, close);
        do_function(keeper, &do_say, buf);
        return NULL;
      }

      if ( time_info.hour > pShop->close_hour )
      {
        do_function(keeper, &do_say, "Sorry, I am closed. Come back tomorrow.");
        sprintf(buf, "My hours of operation are from %s to %s.", open, close);
        do_function(keeper, &do_say, buf);
        return NULL;
      }
    }

    if ( pShop->open_hour > pShop->close_hour )
    {
      if (( time_info.hour < pShop->open_hour ) && (time_info.hour > pShop->close_hour ))
      {
        do_function(keeper, &do_say, "Sorry, I am closed. Come back later.");
        sprintf(buf, "My hours of operation are from %s to %s.", open, close);
        do_function(keeper, &do_say, buf);
        return NULL;
      }
    }

    return keeper;
}

/* insert an object at the right spot for the keeper */
void obj_to_keeper( OBJ_DATA *obj, CHAR_DATA *ch )
{
    OBJ_DATA *t_obj, *t_obj_next;

    /* see if any duplicates are found */
    for (t_obj = ch->carrying; t_obj != NULL; t_obj = t_obj_next)
    {
    t_obj_next = t_obj->next_content;

    if (obj->pIndexData == t_obj->pIndexData
    &&  !str_cmp(obj->short_descr,t_obj->short_descr))
    {
        /* if this is an unlimited item, destroy the new one */
        if (IS_OBJ_STAT(t_obj,ITEM_INVENTORY))
        {
        extract_obj(obj);
        return;
        }
        obj->cost = t_obj->cost; /* keep it standard */
        break;
    }
    }

    if (t_obj == NULL)
    {
    obj->next_content = ch->carrying;
    ch->carrying = obj;
    }
    else
    {
    obj->next_content = t_obj->next_content;
    t_obj->next_content = obj;
    }

    obj->carried_by      = ch;
    obj->in_room         = NULL;
    obj->in_obj          = NULL;
    ch->carry_number    += get_obj_number( obj );
    ch->carry_weight    += get_obj_weight( obj );
}

/* get an object from a shopkeeper's list */
OBJ_DATA *get_obj_keeper( CHAR_DATA *ch, CHAR_DATA *keeper, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = keeper->carrying; obj != NULL; obj = obj->next_content )
    {
        if (obj->wear_loc == WEAR_NONE
        &&  can_see_obj( keeper, obj )
    &&  can_see_obj(ch,obj)
        &&  is_name( arg, obj->name ) )
        {
            if ( ++count == number )
                return obj;

        /* skip other objects of the same name */
        while (obj->next_content != NULL
        && obj->pIndexData == obj->next_content->pIndexData
        && !str_cmp(obj->short_descr,obj->next_content->short_descr))
        obj = obj->next_content;
        }
    }

    return NULL;
}

int get_cost( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy )
{
    SHOP_DATA *pShop;
    int cost;

    if ( obj == NULL || ( pShop = keeper->pIndexData->pShop ) == NULL )
    return 0;

    if ( fBuy )
    {
    cost = obj->cost * pShop->profit_buy  / 100;
    }
    else
    {
    OBJ_DATA *obj2;
    int itype;

    cost = 0;
    for ( itype = 0; itype < MAX_TRADE; itype++ )
    {
        if ( obj->item_type == pShop->buy_type[itype] )
        {
        cost = obj->cost * pShop->profit_sell / 100;
        break;
        }
    }

    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
        for ( obj2 = keeper->carrying; obj2; obj2 = obj2->next_content )
        {
            if ( obj->pIndexData == obj2->pIndexData
        &&   !str_cmp(obj->short_descr,obj2->short_descr) )
                {
            if (IS_OBJ_STAT(obj2,ITEM_INVENTORY))
            cost /= 2;
            else
                        cost = cost * 3 / 4;
                }
        }
    }

    if ( obj->item_type == ITEM_STAFF || obj->item_type == ITEM_WAND )
    {
    if (obj->value[1] == 0)
        cost /= 4;
    else
        cost = cost * obj->value[2] / obj->value[1];
    }

    return cost;
}



void do_buy( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cost,roll;
    int success = 0;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Buy what?\n\r", ch );
    return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_MOUNT_SHOP) )
    {
    do_function(ch, &do_buy_mount, argument);
    return;
    }

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
    {
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *pet;
    ROOM_INDEX_DATA *pRoomIndexNext;
    ROOM_INDEX_DATA *in_room;

    smash_tilde(argument);

    if ( IS_NPC(ch) )
        return;

    argument = one_argument(argument,arg);

    /* hack to make new thalos pets work */
    if (ch->in_room->vnum == 9621)
        pRoomIndexNext = get_room_index(9706);
    else
        pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );
    if ( pRoomIndexNext == NULL )
    {
        bug( "Do_buy: bad pet shop at vnum %d.", ch->in_room->vnum );
        send_to_char( "Sorry, you can't buy that here.\n\r", ch );
        return;
    }

    in_room     = ch->in_room;
    ch->in_room = pRoomIndexNext;
    pet         = get_char_room( ch, NULL, arg );
    ch->in_room = in_room;

    if ( pet == NULL || !IS_SET(pet->act, ACT_PET) )
    {
        send_to_char( "Sorry, you can't buy that here.\n\r", ch );
        return;
    }

    if ( ch->pet != NULL )
    {
        send_to_char("You already own a pet.\n\r",ch);
        return;
    }

    cost = pet->pIndexData->wealth;


    if ( ch->level < pet->level )
    {
        send_to_char(
        "You're not powerful enough to master this pet.\n\r", ch );
        return;
    }

    if ( (ch->silver + 100 * ch->gold) < cost )
    {
        send_to_char( "You can't afford it.\n\r", ch );
        return;
    }

    /* haggle */
        success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_COMMERCE], 6);
    if (success < 0)
    {
        send_to_char("The Shopkeeper seems insulted by your attempt to haggle down the price and ups the price!\n\r", ch);
        cost += cost/10;
    } else if (success > 0)
    {
        if (success > 10)
            success = 10;
        send_to_char("You haggle down the price.\n\r", ch);
        //Haggle math! Gives rougghly maximum 20% discount for 10 successes.
        cost = (100-(success*2))*cost/100;
        check_improve(ch,gsn_haggle,TRUE,4);
}

    deduct_cost(ch,cost);
    pet         = create_mobile( pet->pIndexData );
    SET_BIT(pet->act, ACT_PET);
    SET_BIT(pet->affected_by, AFF_CHARM);
    pet->gold = 0;
    pet->silver = 0;
    pet->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;

    argument = one_argument( argument, arg );
    if ( arg[0] != '\0' )
    {
        sprintf( buf, "%s %s", pet->name, arg );
        free_string( pet->name );
        pet->name = str_dup( buf );
    }

    sprintf( buf, "%sA neck tag says 'I belong to %s'.\n\r",
        pet->description, ch->name );
    free_string( pet->description );
    pet->description = str_dup( buf );

    char_to_room( pet, ch->in_room );
    add_follower( pet, ch );
    pet->leader = ch;
    ch->pet = pet;
    send_to_char( "Enjoy your pet.\n\r", ch );
    act( "$n bought $N as a pet.", ch, NULL, pet, TO_ROOM );
    return;
    }
    else
    {
    CHAR_DATA *keeper;
    OBJ_DATA *obj,*t_obj;
    char arg[MAX_INPUT_LENGTH];
    int number, count = 1;
    int tax;
    int hagglediff = 7;

    if ( ( keeper = find_keeper( ch ) ) == NULL )
        return;

    if (is_affected(ch, gsn_sidesteptime))
    {
      send_to_char("You must return to the normal flow of Time to purchase items.\n\r", ch);
      return;
    }

    if(is_affected(ch, gsn_change) || is_affected(ch, gsn_shift) || is_affected(ch, gsn_shadowform) || is_affected(ch, gsn_vicissitude_horrid) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("The shopkeeper is too terrified of you to sell anything.\n\r", ch);
        return;
    }

    number = mult_argument(argument,arg);
    obj  = get_obj_keeper( ch,keeper, arg );
    cost = get_cost( keeper, obj, TRUE );

    if (number < 1 || number > 99)
    {
        act("$n tells you 'Get real!",keeper,NULL,ch,TO_VICT);
        return;
    }

    if ( cost <= 0 || !can_see_obj( ch, obj ) )
    {
        act( "$n tells you 'I don't sell that -- try 'list''.",
        keeper, NULL, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if (!IS_OBJ_STAT(obj,ITEM_INVENTORY))
    {
        for (t_obj = obj->next_content;
             count < number && t_obj != NULL;
             t_obj = t_obj->next_content)
        {
            if (t_obj->pIndexData == obj->pIndexData
            &&  !str_cmp(t_obj->short_descr,obj->short_descr))
            count++;
            else
            break;
        }

        if (count < number)
        {
            act("$n tells you 'I don't have that many in stock.",
            keeper,NULL,ch,TO_VICT);
            ch->reply = keeper;
            return;
        }
    }


    if ( obj->level > ch->level )
    {
        act( "$n tells you 'You can't use $p yet'.",
        keeper, obj, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if (ch->carry_number +  number * get_obj_number(obj) > can_carry_n(ch))
    {
        send_to_char( "You can't carry that many items.\n\r", ch );
        return;
    }

    if ( ch->carry_weight + number * get_obj_weight(obj) > can_carry_w(ch))
    {
        send_to_char( "You can't carry that much weight.\n\r", ch );
        return;
    }

    if (IS_OBJ_STAT(obj,ITEM_BLESS) && (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")))
    {
        send_to_char( "You are repelled by that object!\n\r", ch );
        return;
    }

 /*Influence, another 1% off per dot of influence.*/
 if (!IS_NPC(ch) && !IS_OBJ_STAT(obj, ITEM_SELL_EXTRACT) && ch->pcdata->csbackgrounds[CSBACK_INFLUENCE] > 0)
        cost = (100-ch->pcdata->csbackgrounds[CSBACK_INFLUENCE])*cost/100;


     if (keeper->pIndexData->area->domain != NULL) {
         tax = cost * keeper->pIndexData->area->domain->tax /100;
         cost += tax;
     }

    if ( (ch->silver + ch->gold * 100) < cost * number )
    {
        if (number > 1)
        act("$n tells you 'You can't afford to buy that many.",
            keeper,obj,ch,TO_VICT);
        else
            act( "$n tells you 'You can't afford to buy $p'.",
            keeper, obj, ch, TO_VICT );
        ch->reply = keeper;
        return;
    }

    if (has_affect_modifier(ch, gsn_awe, keeper->pIndexData->vnum) && !IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
    {
      success = godice(get_attribute(ch, APPEARANCE) + ch->csabilities[CSABIL_ETIQUETTE], 6);
      if (success > 0)
      {
        act("$N seems to have taken a liking to you, and lowers $S price.", ch, NULL, keeper, TO_CHAR);
        cost -= cost / (100 - (success * 2));
      }
    }

    /* haggle */
    if (!IS_NPC(ch) && !IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT) && ch->csabilities[CSABIL_COMMERCE] > 0 )
    {
        if is_affected(ch, gsn_gift_speechoftheworld)
        {
            send_to_char("You attempt to haggle in the shopkeeper's native language.\n\r", ch);
            hagglediff--;
            if (get_affect_level(ch, gsn_gift_speechoftheworld) > 4)
                hagglediff--;
        }
        success = godice(get_attribute(ch, MANIPULATION) + ch->csabilities[CSABIL_COMMERCE], hagglediff);
        if (success < 0)
        {
            act("$N seems insulted by your attempt to haggle down the price and ups the price!", ch, NULL, keeper, TO_CHAR);
            cost += cost/10;
        }
        else if (success > 0)
        {
            if (success > 10)
                success = 10;
            act("You haggle with $N.", ch, NULL, keeper, TO_CHAR);
            act("$n haggles with $N.", ch, NULL, keeper, TO_ROOM);
            //Haggle math! Gives rougghly maximum 20% discount for 10 successes.
            cost = (100-(success*2))*cost/100;
            check_improve(ch,gsn_haggle,TRUE,4);
        }
    }

    if (number > 1)
    {
        sprintf(buf,"$n buys $p[%d].",number);
        act(buf,ch,obj,NULL,TO_ROOM);
        sprintf(buf,"You buy $p[%d] for %d silver.",number,cost * number);
        act(buf,ch,obj,NULL,TO_CHAR);
    }
    else
    {
        act( "$n buys $p.", ch, obj, NULL, TO_ROOM );
        sprintf(buf,"You buy $p for %d silver.",cost);
        act( buf, ch, obj, NULL, TO_CHAR );
    }
    deduct_cost(ch,cost * number);
    keeper->gold += cost * number/100;
    keeper->silver += cost * number - (cost * number/100) * 100;
    if (keeper->pIndexData->area->domain != NULL && keeper->pIndexData->area->domain->clan != 0)
    {
        clan_table[keeper->pIndexData->area->domain->clan].bank += tax * number/100;
        save_clans(ch, "");
    }

    for (count = 0; count < number; count++)
    {
        if ( IS_SET( obj->extra_flags, ITEM_INVENTORY ) )
            t_obj = create_object( obj->pIndexData, obj->level );
        else
        {
        t_obj = obj;
        obj = obj->next_content;
            obj_from_char( t_obj );
        }

        if (t_obj->timer > 0 && !IS_OBJ_STAT(t_obj,ITEM_HAD_TIMER))
            t_obj->timer = 0;
        REMOVE_BIT(t_obj->extra_flags,ITEM_HAD_TIMER);
        obj_to_char( t_obj, ch );
        if (cost < t_obj->cost)
            t_obj->cost = cost;
    }
    }
}



void do_list( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    long cost;

    if ( IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) || IS_SET(ch->in_room->room_flags, ROOM_MOUNT_SHOP) )
    {
    ROOM_INDEX_DATA *pRoomIndexNext;
    CHAR_DATA *pet;
    bool found;

        /* hack to make new thalos pets work */
        if (ch->in_room->vnum == 9621)
            pRoomIndexNext = get_room_index(9706);
        else
            pRoomIndexNext = get_room_index( ch->in_room->vnum + 1 );

    if ( pRoomIndexNext == NULL )
    {
        bug( "Do_list: bad pet shop at vnum %d.", ch->in_room->vnum );
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }

    found = FALSE;
    for ( pet = pRoomIndexNext->people; pet; pet = pet->next_in_room )
    {
        if ( IS_SET(pet->act, ACT_PET) || IS_SET(pet->act, ACT_MOUNT) )
        {
        if ( !found )
        {
            found = TRUE;
            if (IS_SET(pet->act, ACT_PET))
                send_to_char( "Pets for sale:\n\r", ch );
        }
        cost = pet->pIndexData->wealth;
        if(IS_SET(pet->act, ACT_PET))
        {
            sprintf( buf, "[%2d] %8d - %s\n\r",
                pet->level,
                (int) cost,
                pet->short_descr );
            send_to_char( buf, ch );
        }
        else if(IS_SET(pet->act, ACT_MOUNT))
        {
            sprintf( buf, "[%2d] %8d - %s\n\r",
                pet->level,
                (int) cost,
                pet->short_descr );
            send_to_char( buf, ch );
        }
        }
    }
    if ( !found )
    {
            if (IS_SET(ch->in_room->room_flags, ROOM_PET_SHOP) )
                send_to_char( "Sorry, we're out of pets right now.\n\r", ch );
            else
                send_to_char( "Sorry, we're out of mounts right now.\n\r", ch );        return;
    }
    return;
    }
    else
    {
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost,count;
    bool found;
    char arg[MAX_INPUT_LENGTH];

    if ( ( keeper = find_keeper( ch ) ) == NULL )
        return;
        one_argument(argument,arg);

    found = FALSE;

    if (is_affected(ch, gsn_sidesteptime))
    {
      send_to_char("You are not existing in the normal stream of time at this moment.\n\r", ch);
      return;
    }

    if (is_affected(ch, gsn_vicissitude_horrid) || is_affected(ch, gsn_change) || is_affected(ch, gsn_shift) || is_affected(ch, gsn_shadowform) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("The shopkeeper is terrified of you in this form!\n\r", ch);
        return;
    }

    for ( obj = keeper->carrying; obj; obj = obj->next_content )
    {
        if ( obj->wear_loc == WEAR_NONE
        &&   can_see_obj( ch, obj )
        &&   ( cost = get_cost( keeper, obj, TRUE ) ) > 0
        &&   ( arg[0] == '\0'
           ||  is_name(arg,obj->name) ))
        {
        if ( !found )
        {
            found = TRUE;
            send_to_char( "[Lv Price Qty] Item\n\r", ch );
        }
        if (keeper->pIndexData->area->domain != NULL && keeper->pIndexData->area->domain->clan != 0)
        {
            int tax;
            tax = cost * keeper->pIndexData->area->domain->tax / 100;
            cost += tax;
        }

        if (IS_OBJ_STAT(obj,ITEM_INVENTORY))
            sprintf(buf,"[%2d %5d -- ] %s\n\r",
            obj->level,cost,obj->short_descr);
        else
        {
            count = 1;

            while (obj->next_content != NULL
            && obj->pIndexData == obj->next_content->pIndexData
            && !str_cmp(obj->short_descr,
                    obj->next_content->short_descr))
            {
            obj = obj->next_content;
            count++;
            }
            sprintf(buf,"[%2d %5d %2d ] %s\n\r",
            obj->level,cost,count,obj->short_descr);
        }
        send_to_char( buf, ch );
        }
    }

    if ( !found )
        send_to_char( "You can't buy anything here.\n\r", ch );
    return;
    }
}

void do_sell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper = NULL;
    OBJ_DATA *obj = NULL;
    int cost,roll, success, number, total_cost = 0, sold = 0 ;
    bool haggle = FALSE;

    number = mult_argument(argument,arg);

    if ( arg[0] == '\0' )
    {
     send_to_char( "Sell what?\n\r", ch );
     return;
    }
    if ( ( keeper = find_keeper( ch ) ) == NULL )
     return;

     if (is_affected(ch, gsn_sidesteptime))
     {
       send_to_char("You must return to the normal flow of Time to sell your items.\n\r", ch);
       return;
     }

    if (is_affected(ch, gsn_change) || is_affected(ch, gsn_shift) || is_affected(ch, gsn_shadowform) || is_affected(ch, gsn_vicissitude_horrid) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("The shopkeeper is too terrified of you to purchase anything you have.\n\r", ch);
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
     {
              act( "$n tells you 'You don't have that item'.",
                   keeper, NULL, ch, TO_VICT );
              return;
     }

    if (number < 1 || number > 99)
    {
       act("$n tells you 'Get real!",keeper,NULL,ch,TO_VICT);
         return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
         act( "$n tells you 'You don't have that item'.",
              keeper, NULL, ch, TO_VICT );
         ch->reply = keeper;
         return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
         send_to_char( "You can't let go of it.\n\r", ch );
         return;
    }

    if (!can_see_obj(keeper,obj))
    {
       act("$n doesn't see what you are offering.",keeper,NULL,ch,TO_VICT);
       return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
       act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
       return;
    }
    if ( cost*number > (keeper->silver + 100 * keeper->gold) )
    {
      act("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
         keeper,obj,ch,TO_VICT);
      return;
    }
    cost = 0;
    roll = number_percent();
    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT) && roll < get_skill(ch,gsn_haggle))
    {
        haggle = TRUE;
        send_to_char("You haggle with the shopkeeper.\n\r",ch);
    }
    while (sold < number)
    {
        if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
        {
            send_to_char("You don't have that many.\n\r",ch);
            break;
        }
        cost = get_cost(keeper,obj,FALSE);

        if (has_affect_modifier(ch, gsn_awe, keeper->pIndexData->vnum) && !IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
        {
          success = godice(get_attribute(ch, APPEARANCE) + ch->csabilities[CSABIL_ETIQUETTE], 6);
          if (success > 0)
          {
            act("$N seems to have taken a liking to you, and raises $S offer.", ch, NULL, keeper, TO_CHAR);
            cost += cost / (100 - (success * 2));
          }
        }

     /* haggle */
        if(haggle)
        {
            success = godice(get_attribute(ch,MANIPULATION)+ch->csabilities[CSABIL_COMMERCE],4);
            if(success <= 0)
                cost /= 2;
            else
                cost += cost/5 * success;
        }
/*
     roll = number_percent();
     if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT) && roll < get_skill(ch,gsn_haggle))
     {
          send_to_char("You haggle with the shopkeeper.\n\r",ch);
          cost += obj->cost / 2 * roll / 100;
          cost = UMIN(cost,95 * get_cost(keeper,obj,TRUE) / 100);
          cost = UMIN(cost,(keeper->silver + 100 * keeper->gold));
          check_improve(ch,gsn_haggle,TRUE,4);
     }
*/
        total_cost += cost;

        deduct_cost(keeper,cost);
        if ( keeper->gold < 0 )
            keeper->gold = 0;
        if ( keeper->silver< 0)
            keeper->silver = 0;
        sprintf(name,"%s",obj->short_descr);
         if ( obj->item_type == ITEM_TRASH ||IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
         {
              extract_obj( obj );
         }
         else
         {
              obj_from_char( obj );
              if (obj->timer)
              SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
              else
              obj->timer = number_range(50,100);
              obj_to_keeper( obj, keeper );
         }
         sold++;
    }
     if (number > 1)
     {
       sprintf( buf, "You sell %s[%d] for %d silver and %d gold piece%s.",name,
        sold, total_cost - (total_cost/100) * 100, total_cost/100, total_cost == 1 ? "" : "s" );
     }
     else
     {
        sprintf( buf, "You sell %s for %d silver and %d gold piece%s.",name,
           total_cost - (total_cost/100) * 100, total_cost/100, total_cost == 1 ? "" : "s" );
     }

    act( buf, ch, obj, NULL, TO_CHAR );
     ch->gold     += total_cost/100;
     ch->silver      += total_cost - (total_cost/100) * 100;

    return;
}

/*
void do_sell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost,roll;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Sell what?\n\r", ch );
    return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
    return;

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
    act( "$n tells you 'You don't have that item'.",
        keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
    send_to_char( "You can't let go of it.\n\r", ch );
    return;
    }

    if (!can_see_obj(keeper,obj))
    {
    act("$n doesn't see what you are offering.",keeper,NULL,ch,TO_VICT);
    return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
    act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
    return;
    }
    if ( cost > (keeper-> silver + 100 * keeper->gold) )
    {
    act("$n tells you 'I'm afraid I don't have enough wealth to buy $p.",
        keeper,obj,ch,TO_VICT);
    return;
    }

    act( "$n sells $p.", ch, obj, NULL, TO_ROOM );
    / haggle /
    roll = number_percent();
    if (!IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT) && roll < get_skill(ch,gsn_haggle))
    {
        send_to_char("You haggle with the shopkeeper.\n\r",ch);
        cost += obj->cost / 2 * roll / 100;
        cost = UMIN(cost,95 * get_cost(keeper,obj,TRUE) / 100);
    cost = UMIN(cost,(keeper->silver + 100 * keeper->gold));
        check_improve(ch,gsn_haggle,TRUE,4);
    }
    sprintf( buf, "You sell $p for %d silver and %d gold piece%s.",
    cost - (cost/100) * 100, cost/100, cost == 1 ? "" : "s" );
    act( buf, ch, obj, NULL, TO_CHAR );
    ch->gold     += cost/100;
    ch->silver   += cost - (cost/100) * 100;
    deduct_cost(keeper,cost);
    if ( keeper->gold < 0 )
    keeper->gold = 0;
    if ( keeper->silver< 0)
    keeper->silver = 0;

    if ( obj->item_type == ITEM_TRASH || IS_OBJ_STAT(obj,ITEM_SELL_EXTRACT))
    {
    extract_obj( obj );
    }
    else
    {
    obj_from_char( obj );
    if (obj->timer)
        SET_BIT(obj->extra_flags,ITEM_HAD_TIMER);
    else
        obj->timer = number_range(50,100);
    obj_to_keeper( obj, keeper );
    }

    return;
}
*/


void do_value( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *keeper;
    OBJ_DATA *obj;
    int cost;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Value what?\n\r", ch );
    return;
    }

    if ( ( keeper = find_keeper( ch ) ) == NULL )
    return;

    if(is_affected(ch, gsn_change) || is_affected(ch, gsn_shift) || is_affected(ch, gsn_shadowform) || is_affected(ch, gsn_vicissitude_horrid) || is_affected(ch, gsn_vicissitude_bonecraft))
    {
        send_to_char("The shopkeeper is too terrified of you to appraise your items.\n\r", ch);
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
    act( "$n tells you 'You don't have that item'.",
        keeper, NULL, ch, TO_VICT );
    ch->reply = keeper;
    return;
    }

    if (!can_see_obj(keeper,obj))
    {
        act("$n doesn't see what you are offering.",keeper,NULL,ch,TO_VICT);
        return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
    send_to_char( "You can't let go of it.\n\r", ch );
    return;
    }

    if ( ( cost = get_cost( keeper, obj, FALSE ) ) <= 0 )
    {
    act( "$n looks uninterested in $p.", keeper, obj, ch, TO_VICT );
    return;
    }

    sprintf( buf,
    "$n tells you 'I'll give you %d silver and %d gold coins for $p'.",
    cost - (cost/100) * 100, cost/100 );
    act( buf, keeper, obj, ch, TO_VICT );
    ch->reply = keeper;

    return;
}

void do_lore( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AFFECT_DATA *paf;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL && get_char_room( ch, NULL, arg ) == NULL)
    {
        send_to_char( "Lore what?.\n\r", ch );
        return;
    }
    if (ch->mana < 50)
    {
        send_to_char("You have too little magical energy.\n\r",ch);
        return;
    }

    if (ch->move < 100)
    {
        send_to_char("You are too tired to put the effort into loring.\n\r",ch);
        return;
    }

    if (get_skill(ch,gsn_lore) <= 1)
    {
        send_to_char("You fail to gain any useful information.\n\r",ch);
        return;
    }
    if (number_percent() >= 20 + get_skill(ch,gsn_lore) * 4/5)
    {
        send_to_char("You fail to gain any useful information.\n\r",ch);
        check_improve(ch,gsn_lore,FALSE,2);
        WAIT_STATE(ch, 36);
        return;
    }

    check_improve(ch,gsn_lore,TRUE,2);
    WAIT_STATE(ch, 36);
    ch->mana -= 50;
    ch->move -= 100;
    if((victim = get_char_room( ch, NULL, arg )) != NULL)
    {
        if(!IS_NPC(victim))
        {
            send_to_char("Not on players!", ch);
            return;
        }
        sprintf(buf, "%s is a %s %s.\n\r",
            victim->short_descr,
            victim->sex == 0 ? "sexless" : victim->sex == 1 ? "male" : "female",
            race_table[victim->race].name);
        send_to_char(buf,ch);
        sprintf( buf,
            "They have %d around hit points.\n\r",
            UMAX( 1, number_range(victim->hit-victim->level, victim->hit+victim->level)));
        send_to_char( buf, ch );
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
        if (victim->affected_by)
        {
            sprintf(buf, "%s is affected by %s\n\r",
            victim->short_descr,
            affect_bit_name(victim->affected_by));
            send_to_char(buf,ch);
        }

        if (victim->affected2_by)
        {
            sprintf(buf, "%s is also affected by %s\n\r",
            victim->short_descr,
            affect2_bit_name(victim->affected2_by) );
            send_to_char(buf,ch);
        }

    }
    else
    {
        sprintf( buf,
            "Object '%s' is type %s, extra flags %s.\n\rWeight is %d.%d, value is %d, level is %d.\n\r",
            obj->name,
            item_name(obj->item_type),
            extra_bit_name( obj->extra_flags ),
            obj->weight / 10, obj->weight % 10,
            obj->cost,
            obj->level
            );
        send_to_char( buf, ch );

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
                sprintf(buf,"Capacity: %d#  Maximum weight: %d.%d#  flags: %s\n\r",
                obj->value[3], obj->value[0] / 10, obj->value[0] % 10, cont_bit_name(obj->value[1]));
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
                sprintf(buf, "D10 Damage Dice Bonus: {D({r+%d{D){x\n\r", obj->value[1] / 20);
                send_to_char(buf, ch);
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

void do_dip(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MAX_STRING_LENGTH];
           int chance;


    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if((obj = get_obj_here( ch, NULL, arg1 )) == NULL)
    {
        send_to_char("You do not have that item!\n\r" ,ch);
        return;
    }

    if((obj2 = get_obj_here( ch, NULL, arg2 )) == NULL)
    {
        send_to_char("Dip it into what?\n\r" ,ch);
        return;
    }

    if (!IS_OBJ_STAT(obj2,ITEM_IS_PRESERVE))
    {
        send_to_char( "That isn't a preservative!\n\r", ch );
        return;
    }

    if (obj2->condition <= 0)
    {
      send_to_char("That preservative has been used up.  You toss it away.\n\r", ch);
      extract_obj(obj2);
      return;
    }

    if ( obj->item_type != ITEM_FOOD )
    {
        send_to_char( "That item cannot be preserved!\n\r", ch );
        return;
    }

    if ( obj->timer == -1)
    {
      send_to_char("That item is already preserved to stand the test of time.\n\r", ch);
      return;
    }

    chance = godice(get_attribute(ch, INTELLIGENCE) + get_ability(ch, CSABIL_SURVIVAL), 6);

    act( "You dip $p into $P.", ch, obj, obj2, TO_CHAR );
    act( "$n dips $p into $P.", ch, obj, obj2, TO_ROOM );

    obj2->condition -= 3;

    if(chance < 0)
    {
        act( "You curse as you ruin $p while trying to preserve it, and throw it away.", ch, obj, obj2, TO_CHAR );
        act( "$n curses and throws away $p.", ch, obj, obj2, TO_ROOM );
        extract_obj( obj );
        obj2->condition -= 10;
    }

    if (chance == 0)
    {
      act( "You mutter as you had to attempt multiple times to properly preserve $p fully.", ch, obj, obj2, TO_CHAR );
      act( "$n mutters in disappointment while looking over $p.", ch, obj, obj2, TO_ROOM );
      obj->condition -= 6;
    }

    if (chance > 0)
    {
      act( "You feel satisfied that you were able to preserve $p fully.", ch, obj, obj2, TO_CHAR );
    }

    if (chance >= 0)
      obj->timer = -1;

    if (obj2->condition <= 0)
    {
      act("You throw away $P, as its preservative properties have run out.", ch, obj, obj2, TO_CHAR);
      act("$n throws away $P.", ch, obj, obj2, TO_ROOM);
      extract_obj(obj2);
    }

    gain_exp(ch, chance);
    return;
}

void do_rite( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *garou;
    CHAR_DATA *garou_next;
    CHAR_DATA *spirit;
    OBJ_DATA *caern;
    OBJ_DATA *obj;
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *old_room;
    MOB_INDEX_DATA *pMobIndex;
    int count=0;
    int rites_roll = 0;
    bool ritemaster = FALSE;

    one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
    send_to_char( "Preform which Rite?\n\r", ch );
    return;
    }

    if(ch->race != race_lookup("garou"))
    {
        send_to_char("This is a Garou ability!\n\r",ch);
        return;
    }

    if ( ( caern = get_obj_here( ch, NULL,  "caern" ) ) == NULL )
    {
    send_to_char( "This Rite must be performed in the presence of a Caern.\n\r", ch );
    return;
    }
/*No longer have to be IC
    if (!IS_SET(ch->act,PLR_IC))
    {
        send_to_char("You must be IC to perform this rite.\n\r",ch);
        return;
    }
*/
    if (!str_cmp(arg,"of Moot"))
    {
        if (ch->pcdata->csbackgrounds[CSBACK_RITES] < 1)
        {
          ritemaster = TRUE;
          sendch("You do not have the mystical knowledge to perform this rite yourself,\n\r", ch);
          if ( ( obj = get_obj_here( ch, NULL, "platinum" ) ) == NULL )
          {
            send_to_char("and the Ritesmaster summoned to assist requires proper payment.\n\r",ch);
            return;
          }
          send_to_char("but the Ritesmaster summoned to assist will gladly accept your tribute.\n\r", ch);
        }

        if (ch->pcdata->gnosis[TEMP] < 4)
        {
          send_to_char("You do not have enough spiritual energy to recharge this Caern.\n\r", ch);
          return;
        }

        ch->pcdata->gnosis[TEMP] -= 3;
       if (ritemaster)
        {
          extract_obj(obj);
          rites_roll = godice(4, 7);
          act("The Ritemaster accepts the tribute and leads the garou present in a resonating howl.", ch, NULL, NULL, TO_ROOM);
        }
        else
        {
          rites_roll = godice(ch->pcdata->gnosis[PERM], 7);
          act("You begin a resonating howl, pledging your energies to the Caern.", ch, NULL, NULL, TO_CHAR);
          act("$n lifts $s head in a resonating howl, echoing throughout the area.", ch, NULL, NULL, TO_NOTVICT);
        }

        if (rites_roll < 0)
        {
          act("Midway through the Rite, you notice that the howling isn't as fluid as it should be.\n\rWith a pulse of energy, $p's spiritual essence dims, and finally winks out.", ch, caern, NULL, TO_CHAR);
          act("Amidst the howling, $p pulses briefly and then subsides.", ch, caern, NULL, TO_NOTVICT);
          caern->value[2] = 0;
          return;
        }

        if (rites_roll == 0)
        {
          act("The howling subsides, but $p's spiritual essence seems unchanged.", ch, caern, NULL, TO_CHAR);
          act("The howling reaches a final point, and then there is silence.", ch, caern, NULL, TO_NOTVICT);
          return;
        }

        caern->value[2] = caern->value[1];
        send_to_char("As you complete the Rite of Moot for this month, you feel the essence of the Caern revitalize.\n\r",ch);
        act( "The Rite of Moot concludes, and there is an almost palpable charge in the air.", ch, NULL, NULL, TO_ROOM );
    }
    else if(!str_cmp(arg,"of the Opened Caern"))
    {

                if (ch->pcdata->csbackgrounds[CSBACK_RITES] < 1)
        {
        sendch("You do not have the mystical knowledge to perform this rite.\n\r", ch);
        return;
        }
        if ( ( obj = get_obj_here( ch, NULL, "heart" ) ) == NULL )
        {
            send_to_char( "This Rite requires the heart of brave creature or person to be placed in the center of the Caern.\n\r", ch );
            return;
        }
        if(caern->value[2] <= 0)
        {
            send_to_char("This Caern has been drained of the power of Gaia.\n\r",ch);
            return;
        }

        extract_obj(obj);

        for(garou = ch; garou != NULL; garou = garou->next_in_room)
        {
            if(garou->race == race_lookup("garou"))
            {
                act( "You feel the power of Gaia enter you from $p.", garou, caern, NULL, TO_CHAR );
                obj_cast_spell( caern->value[3], caern->value[0], ch, garou, caern );
            }
        }
        caern->value[2]--;
        send_to_char("You complete the Rite of the Opened Caern.\n\r",ch);
        act( "$n completes the Rite of the Opened Caern over $p.", ch, caern, NULL, TO_ROOM );
    }
    else if(!str_cmp(arg,"of Summoning"))
    {
        if (ch->pcdata->csbackgrounds[CSBACK_RITES] < 2)
        {
        sendch("You do not have the mystical knowledge to perform this rite.\n\r", ch);
        return;
        }

        if(ch->pcdata->auspice != THEURGE)
        {
            send_to_char("Only a true Mystic can summon forth a spirit.\n\r",ch);
            return;
        }
        if ( ( obj = get_obj_here( ch, NULL, "platinum" ) ) == NULL )
        {
            send_to_char("This sacrifice is not great enough to please Gaia.\n\r",ch);
            return;
        }
        extract_obj(obj);

/*      if(number_range(1,100) > 90)
        {
            send_to_char("You failed to pierce the Gauntlet. The spirit is not summoned.\n\r",ch);
            return;
        }
*/
        #define MOB_VNUM_SPIRITQUEST number_range(30720,30731)
/*
        if ( (pMobIndex = get_mob_index(number_range(30720,30731)) == NULL )
*/
        if ( (pMobIndex = get_mob_index(MOB_VNUM_SPIRITQUEST)) == NULL )
        {
            send_to_char( "Error: please infrorm the Coders!\n\r", ch );
            return;
        }

        spirit = create_mobile( pMobIndex );
        char_to_room( spirit, ch->in_room );
        WAIT_STATE(ch,50);
        send_to_char("You complete the Rite of summon.\n\r",ch);
        act( "You complete the Rite of Summoning and summon forth $N!", ch, caern, spirit, TO_CHAR );
        act( "$n completes the Rite of Summoning over $p and summons forth $N!", ch, caern, spirit, TO_ROOM );
    }
    else if(!str_cmp(arg,"of the Opened Bridge"))
    {
        for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
        {
            if (obj->item_type == ITEM_WARP_STONE)
            {
                obj_from_char(obj);
                extract_obj(obj);
                count++;
                break;
            }
        }
        if(count == 0)
        {
            send_to_char("You do not have the proper Pathstone.\n\r",ch);
            return;
        }
        act( "You feel the power of Gaia enter you from $p.", ch, caern, NULL, TO_CHAR );
        act( "A Moon Gate opens before you and you step within.", ch, NULL, NULL, TO_CHAR );
        act( "$n steps within a shimmering moongate.", ch, NULL, NULL, TO_ROOM );
        old_room = ch->in_room;
        location = get_room_index(caern->pIndexData->vnum);
        char_from_room(ch);
        char_to_room(ch, location);
        do_function(ch, &do_look, "auto");

        for ( garou = old_room->people; garou != NULL; garou = garou_next )
        {
            garou_next = garou->next_in_room;

            if(garou->race == race_lookup("garou"))
            {
                act( "You feel the power of Gaia enter you from $p.", garou, caern, NULL, TO_CHAR );
                act( "A Moon Gate opens before you and you step within.", garou, NULL, NULL, TO_CHAR );
                act( "$n steps within a shimmering moongate.", garou, NULL, NULL, TO_ROOM );
                location = get_room_index(caern->pIndexData->vnum);
                char_from_room(garou);
                char_to_room(garou, location);
                do_function(garou, &do_look, "auto");
            }
        }
    }
    else
    {
            send_to_char("There is no such Rite!\n\r",ch);
            return;
    }
    if(number_range(1,100) < 5)
    {
        if(number_range(1,100) > 25)
        {
            for(garou = ch; garou != NULL; garou = garou->next_in_room)
            {
                if(garou->race == race_lookup("garou"))
                {

                    act( "The spirit of Gaia smiles upon you.", garou, NULL, NULL, TO_CHAR );
                    act( "$n howls in honor of the spirit of Gaia.", garou, NULL, NULL, TO_ROOM );
                    garou->hit = garou->max_hit;
                    garou->mana = garou->max_mana;
                    garou->move = garou->max_move;
                    garou->rage = 0;
                }
            }
        }
        else
        {
            for(garou = ch; garou != NULL; garou = garou->next_in_room)
            {
                if(garou->race == race_lookup("garou"))
                {

                    act( "The spirit of Gaia sheds a single tear as the Wyrm grips the world.", garou, NULL, NULL, TO_CHAR );
                    act( "$n howls in pain as the forces of the Wyrm gain power in the world.", garou, NULL, NULL, TO_ROOM );
                    garou->hit = 1;
                    garou->mana = 1;
                    garou->move = 1;
                    garou->rage = 100;
                }
            }
        }
    }
    return;
}



void do_learn(CHAR_DATA *ch, char *argument)
{
    char arg[MAX_INPUT_LENGTH];
    char *rarity;
    char *stat;
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int tlev, i;

    argument = one_argument( argument, arg );


    if(IS_NPC(ch)) return;

    if ( ( obj = get_obj_carry( ch, arg, ch ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
        return;
    }

    if(obj->pIndexData->vnum != OBJ_VNUM_TOME )
    {
        send_to_char( "That is not a mystical tome!\n\r", ch );
        return;
    }
    rarity = str_dup(obj->name);
    rarity = one_argument(rarity,arg);
    stat = one_argument(rarity, rarity);

    if(!str_cmp(rarity, "ancient")) tlev = 5;
    else if(!str_cmp(rarity, "arcane")) tlev = 4;
    else if(!str_cmp(rarity, "rare")) tlev = 3;
    else if(!str_cmp(rarity, "lesser")) tlev = 2;
    else tlev = 1;

    if(ch->csabilities[CSABIL_LINGUISTICS] < tlev)
    {
        send_to_char("You cannot decypher the language of this tome.\n\r",ch);
        return;
    }
    if(get_attribute(ch,INTELLIGENCE) < 3)
    {
        send_to_char("You are not intelligent enough to understand this tome.\n\r",ch);
        return;
    }

    for(i = 0; i < 30; i++)
    {
        if(!str_cmp(stat, ability_table[i].name))
        {
            if(ch->csabilities[i] != tlev-1)
            {
                sprintf(buf,"Your %s is too %s.\n\r",ability_table[i].name, ch->csabilities[i] > tlev-1 ? "high" : "low");
                send_to_char(buf,ch);
                return;
            }

            ch->csabilities[i]++;
            extract_obj( obj );
            send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
            cskill_update(ch, TRUE);
            return;
        }
    }

    for(i = 0; i < 9; i++)
    {
        if(!str_cmp(stat, attribute_table[i].name))
        {
            if(ch->csattributes[i] != tlev-1)
            {
                sprintf(buf,"Your %s is too %s.\n\r",capitalize(attribute_table[i].name), ch->csattributes[i] > tlev-1 ? "high" : "low");
                send_to_char(buf,ch);
                return;
            }
            if(i == APPEARANCE && ch->clan == clan_lookup("nosferatu"))
            {
                send_to_char("You are unable to raise your appearance.\n\r",ch);
                return;
            }
            ch->csattributes[i]++;
            extract_obj( obj );
            send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
            cskill_update(ch, TRUE);
            return;
        }
    }


    if(!str_cmp( stat, "conscience"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] != tlev-1)
        {
            sprintf(buf,"Your Conscience is too %s.\n\r",ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] > tlev-1 ? "high" : "low");
            send_to_char(buf,ch);
            return;
        }
        ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]++;
        extract_obj( obj );
        send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
        return;
    }
    if(!str_cmp( stat, "self-control") || !str_prefix( arg, "self_control"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] != tlev-1)
        {
            sprintf(buf,"Your Self-Control is too %s.\n\r",ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] > tlev-1 ? "high" : "low");
            send_to_char(buf,ch);
            return;
        }
        ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]++;
        extract_obj( obj );
        send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
        return;
    }
    if(!str_cmp( stat, "courage"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_COURAGE] != tlev-1)
        {
            sprintf(buf,"Your Courage is too %s.\n\r",ch->pcdata->csvirtues[CSVIRT_COURAGE] > tlev-1 ? "high" : "low");
            send_to_char(buf,ch);
            return;
        }
        ch->pcdata->csvirtues[CSVIRT_COURAGE]++;
        extract_obj( obj );
        send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
        return;
    }
    if(!str_cmp( stat, "willpower"))
    {
        if(ch->csmax_willpower != tlev-1+5)
        {
            sprintf(buf,"Your Willpower is too %s.\n\r",ch->csmax_willpower > tlev-1+5 ? "high" : "low");
            send_to_char(buf,ch);
            return;
        }
        ch->csmax_willpower++;
        ch->cswillpower++;
        extract_obj( obj );
        send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
        return;
    }
    if(!str_cmp( stat, "humanity"))
    {
        if(ch->pcdata->cshumanity != tlev-1+5)
        {
            sprintf(buf,"Your Humanity is too %s.\n\r",ch->pcdata->cshumanity > tlev-1+5 ? "high" : "low");
            send_to_char(buf,ch);
            return;
        }
/*      sprintf(buf,"Success! the player goes from %d to %d in his/her %s!",tlev-1+5,tlev+5,stat);
        send_to_char(buf,ch);
*/
        ch->pcdata->cshumanity++;
        extract_obj( obj );
        send_to_char("You concentrate intently on the mystical tome and feel you have a grasp on it's concepts.\n\rThe tome fades from existence.\n\r",ch);
        return;
    }


    send_to_char("There is something wrong with this Tome. Please contact an immortal to get it fixed.\n\r",ch);
    return;
}
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

    success = godice(get_attribute(ch,STRENGTH)+ch->pcdata->cssec_abil[CSABIL_CARVING],10-ch->pcdata->cssec_abil[CSABIL_LOGGING]);

    if(success < 1)
    {
        act( "and succeed only in chewing up the tree with your axe.", ch, NULL,NULL, TO_CHAR );
        act( "and succeeds only in chewing up the tree.", ch, NULL,NULL, TO_ROOM );
        obj->level -= number_range(1,5);
        WAIT_STATE( ch, (11- ch->pcdata->cssec_abil[CSABIL_LOGGING]) * PULSE_VIOLENCE );

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
    WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_LOGGING]) * PULSE_VIOLENCE );
    
    gain_exp( ch, 60*success );
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

    success = godice(get_attribute(ch,PERCEPTION)+ch->pcdata->cssec_abil[CSABIL_MINING],10-ch->pcdata->cssec_abil[CSABIL_PROSPECTING]);

    act( "You carefully search the area...", ch, NULL,NULL, TO_CHAR );
    act( "$n carefully searches the area...", ch, NULL,NULL, TO_ROOM );


    if ( ( obj = get_obj_hidden( ch, "vein" ) ) == NULL)
    {
        WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
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
        WAIT_STATE( ch, (11- ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
        act( "and you think you find something only to have it, and you buried in a rockslide! OUCH!.", ch, NULL,NULL, TO_CHAR );
        act( "and seems to find something briefly before being caught in a rockslide! OUCH!", ch, NULL,NULL, TO_ROOM );
        damage( ch, ch, 2000, gsn_magick, DAM_BASH, FALSE);
        extract_obj( obj );
        return;
    }

    if(success < 3)
    {
        WAIT_STATE( ch, (3- ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
        send_to_char("There might be something here...\n\r",ch);
        act( "and may have found something!", ch, NULL,NULL, TO_ROOM );
        return;
    }
    WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_PROSPECTING]) * PULSE_VIOLENCE );
    act( "EUREKA! You uncover $p!", ch, obj,NULL, TO_CHAR );
    act( "and uncovers $p!", ch, obj,NULL, TO_ROOM );
    REMOVE_BIT(obj->extra_flags,ITEM_HIDDEN);
    
    gain_exp( ch, 60 * success );
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



    success = godice(get_attribute(ch,STAMINA)+ch->pcdata->cssec_abil[CSABIL_MINING],(number_range(1,3)+((100-tool->value[0])/10))-ch->pcdata->cssec_abil[type]);

    if(success < 1)
    {
        obj->level -= number_range(1,5);
        WAIT_STATE( ch, (11- ch->pcdata->cssec_abil[CSABIL_MINING]) * PULSE_VIOLENCE );
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
    gain_exp( ch, 60 * success);
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
    WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_MINING]) * PULSE_VIOLENCE );
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



    success = godice(get_attribute(ch,DEXTERITY)+ch->csabilities[CSABIL_CRAFTS],(number_range(1,5)+((100-tool->value[0])/10))-ch->pcdata->cssec_abil[CSABIL_CARVING]);

    if(success < 1)
    {
        act( "and unfortunately end up with a useless lump of scrap.", ch, tool,NULL, TO_CHAR );
        act( "and unfortunately ends up with a useless lump of scrap.", ch, tool,NULL, TO_ROOM );

        WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
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

    gain_exp( ch, 35*success );
    gain_qp(ch, number_range( 1, 2*success));
    obj_to_room(obj,ch->in_room);
    WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
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

    success = godice(get_attribute(ch,DEXTERITY)+ch->pcdata->cssec_abil[CSABIL_CARVING],((100-tool->value[0])/10)-ch->csabilities[CSABIL_WOOD]);

    if(success < 1)
    {
        act( "and unfortunately end up with a useless lump of scrap.", ch, tool,NULL, TO_CHAR );
        act( "and unfortunately ends up with a useless lump of scrap.", ch, tool,NULL, TO_ROOM );

        WAIT_STATE( ch, (6 - ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
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

    gain_exp( ch, 40*success );
    gain_qp(ch, number_range( 2, 2*success));
    obj_to_room(obj,ch->in_room);
    WAIT_STATE( ch, (6- ch->pcdata->cssec_abil[CSABIL_CARVING]) * PULSE_VIOLENCE );
    extract_obj( resource );
    return;
}


void do_donate( CHAR_DATA *ch, char *argument)
{
   OBJ_DATA *pit;
   OBJ_DATA *obj;
   ROOM_INDEX_DATA *original;
   char arg[MAX_INPUT_LENGTH];
   char buf[MAX_STRING_LENGTH];
   int amount;

   argument = one_argument(argument, arg);


   if (arg[0] == '\0' )
   {
      send_to_char("Donate what?\n\r",ch);
      return;
   }

   if (IS_AFFECTED2(ch, AFF2_UMBRA))
   {
       sendch("You cannot donate things from the Umbra.\n\r", ch);
       return;
   }

   original = ch->in_room;
   if (ch->position == POS_FIGHTING)
   {
      send_to_char(" You're {Yfighting!{x\n\r",ch);
      return;
   }

   if ( (obj = get_obj_carry (ch, arg, ch)) == NULL)
   {
      send_to_char("You do not have that!\n\r",ch);
      return;
   }
   else
   {
      if (!can_drop_obj(ch, obj) && ch->level < 101)
      {
         send_to_char("It seems to be stuck to you.\n\r",ch);
         return;
      }
      if ((obj->item_type == ITEM_CORPSE_NPC) ||
         (obj->item_type == ITEM_CORPSE_PC))
      {
         send_to_char("You cannot donate that!\n\r",ch);
         return;
      }
      if (obj->timer > 0)
      {
         send_to_char("You cannot donate that.\n\r",ch);
         return;
      }
      act("$n donates $p.",ch,obj,NULL,TO_ROOM);
      act("You donate $p.",ch,obj,NULL,TO_CHAR);

      char_from_room(ch);
      char_to_room(ch,get_room_index(ROOM_VNUM_DONATE));
      pit = get_obj_list(ch, "donation", ch->in_room->contents);
      if (!pit)
      {
          sendch("Something went wrong... Bug reported.\n\r", ch);
          bug("do_donate: NULL donation chest", 0);
          return;
      }
      obj_from_char(obj);
      obj_to_obj(obj, pit);
      //Return to original room.
      char_from_room(ch);
      char_to_room(ch,original);
      return;
     }
}
