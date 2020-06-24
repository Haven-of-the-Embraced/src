#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"
#include "interp.h"
#include "recycle.h"

QITEM_DATA      *qitem_list;
QITEM_DATA      *qitem_free;

#define QIEDIT( fun )      bool fun (CHAR_DATA *ch, char *argument )

void do_qiedit( CHAR_DATA *ch, char *argument )
{
    QITEM_DATA *item;
    char arg1[MAX_STRING_LENGTH];
    char arg2[MSL];

    arg1[0] = '\0';
    arg2[0] = '\0';

    if ( IS_NPC(ch) )
        return;

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);

    if(arg1[0]  == '\0')
    {
        send_to_char("Syntax: Qiedit <name>\n\r",ch);
        send_to_char("Or    : Qiedit new <name>\n\r", ch);
        send_to_char("Or:     Qiedit list\n\r", ch);
        return;
    }

    if (!str_cmp(arg1, "list"))
    {
        do_function(ch, &do_qitemlist, "");
        return;
    }

    if (!str_prefix(arg1, "new" ))
    {
        if( qitem_lookup(arg2 ) != NULL )
            {
                send_to_char("That quest item already exists.\n\r",ch);
                return FALSE;
            }
        if (!str_cmp(arg2, "new") || !str_cmp(arg2, "list"))
        {
            send_to_char("Unacceptable keyword! 'new' and 'list' are reserved.\n\r", ch);
            return;
        }
        if (arg2[0] == '\0')
            {
                send_to_char("Syntax: Qiedit <name>\n\r",ch);
                send_to_char("Or    : Qiedit new <name>\n\r", ch);
                send_to_char("Or:     Qiedit list\n\r", ch);
                return;
            }

            item = new_qitem();
            free_string(item->name );
            item->name = str_dup(arg2);
            ch->desc->pEdit = (void *) item;
            send_to_char("Qitem created.\n\r",ch);
            ch->desc->pEdit = (void *)item;
            ch->desc->editor = ED_QITEM;
            printf_to_char(ch, "Entering Quest Item Editor for %s.\n\r", item->name );
            return;
    }

    if ( (item = qitem_lookup(arg1) ) == NULL )
    {
        send_to_char("What quest item is this?\n\r",ch);
        return;
    }

    ch->desc->pEdit = (void *)item;
    ch->desc->editor = ED_QITEM;
    printf_to_char(ch, "Entering Quest Item Editor for %s.\n\r", item->name );


    return;
}

void qiedit( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int  cmd;
    QITEM_DATA *pQitem;

    arg[0] = '\0';

    smash_tilde( argument );
    strcpy( arg, argument );
    argument = one_argument( argument, command );

    EDIT_QITEM(ch, pQitem);

    if ( !str_cmp(command, "done") )
    {
        edit_done( ch );
        return;
    }

    if ( command[0] == '\0' )
    {
    qiedit_show(ch, argument);
        return;
    }

    /* Search Table and Dispatch Command. */
    for ( cmd = 0; qiedit_table[cmd].name != NULL; cmd++ )
    {
        if ( !str_prefix( command, qiedit_table[cmd].name ) )
        {
            qiedit_table[cmd].olc_fun( ch, argument );
            return;
        }
    }

    /* Default to Standard Interpreter. */
    interpret( ch, arg );
    return;
}

QIEDIT(qiedit_show )
{
    QITEM_DATA *item;
    OBJ_INDEX_DATA *pItem;
    ROOM_INDEX_DATA *pRoom;
    MOB_INDEX_DATA *pMob;
    OBJ_INDEX_DATA *pObj;

    EDIT_QITEM(ch, item );

    pItem = get_obj_index(item->qobjvnum);
    pRoom = get_room_index(item->roomvnum);
    pMob  = get_mob_index(item->mobvnum);
    pObj  = get_obj_index(item->objvnum);

    printf_to_char(ch, "\n\r{wQuest                         - {c%s{x\n\r", item->name );
          send_to_char("{w====================================================={x\n\r",ch);
    printf_to_char(ch, "{wItem Vnum                     - {c%d (%s){x\n\r", item->qobjvnum, pItem != NULL ? pItem->short_descr : "Null" );
    printf_to_char(ch, "{wPlaced in/on Room/Mob/Obj     - {c%s{x\n\r", item->place == PLACE_ROOM ? "Room" : item->place == PLACE_MOB ? "Mobile" : "Obj" );
    printf_to_char(ch, "{wPlaced Room                   - {c%d (%s){x\n\r", item->roomvnum, pRoom != NULL ? pRoom->name : "Null" );
    if (item->place == PLACE_MOB)
        printf_to_char(ch, "{wPlaced Mob                    - {c%d (%s){x\n\r", item->mobvnum, pMob != NULL ? pMob->short_descr : "Null");
    if (item->place ==  PLACE_OBJ)
        printf_to_char(ch, "{wPlaced In Obj                 - {c%d (%s){x\n\r", item->objvnum, pObj != NULL ? pObj->short_descr : "Null");
    printf_to_char(ch, "{wNotify Someone?               - {c%s{x\n\r", item->notify ? "True" : "False" );
    if (item->notify)
        printf_to_char(ch, "{wNotify Whom?                  - {c%s{x\n\r", item->notified );
    printf_to_char(ch, "{wLoaded?                       - {c%s{x\n\r", item->loaded ? "True" : "False");
    printf_to_char(ch, "{wFound?                        - {c%s{x\n\r", item->found ? "True" : "False");
    if (item->found)
        printf_to_char(ch, "{wFound by:                     - {c%s{x\n\r", item->foundby);
    return FALSE;
}
QIEDIT( qiedit_vnum )
{
    QITEM_DATA *pItem;
    OBJ_INDEX_DATA *pObj;
    int         vnum;
    EDIT_QITEM(ch, pItem);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  vnum [number]\n\r", ch );
    return FALSE;
    }
    vnum = atoi( argument );

    if ( ( pObj = get_obj_index( vnum ) ) == NULL )
    {
        send_to_char("That Object does not exist!\n\r", ch);
        return FALSE;
    }

    pItem->qobjvnum = vnum;
    printf_to_char(ch, "Obj set to vnum %d, '%s'\n\r", pItem->qobjvnum, pObj->short_descr);
    return TRUE;
}

QIEDIT( qiedit_place )
{
    QITEM_DATA *pItem;

    EDIT_QITEM(ch, pItem);

    if ( argument[0] == '\0' )
    {
    send_to_char( "Syntax:  place <room/mobile/object>\n\r", ch );
    return FALSE;
    }
    if (!str_prefix(argument, "room"))
    {
        pItem->place = PLACE_ROOM;
        send_to_char("Object will be placed in the room.\n\r", ch);
        return TRUE;
    }
    if (!str_prefix(argument, "mobile"))
    {
        pItem->place = PLACE_MOB;
        send_to_char("Object will be placed on a mobile.\n\r", ch);
        return TRUE;
    }
    if (!str_prefix(argument, "object"))
    {
        pItem->place = PLACE_OBJ;
        send_to_char("Object will be placed in another object.\n\r", ch);
        return TRUE;
    }

    send_to_char("That is not a valid placement. Options are Room, Mobile or Object.\n\r", ch);
    return FALSE;


}

QIEDIT( qiedit_room )
{
    QITEM_DATA *pItem;
    ROOM_INDEX_DATA *pRoom;
    int         vnum;
    EDIT_QITEM(ch, pItem);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  room [number]\n\r", ch );
    return FALSE;
    }
    vnum = atoi( argument );



    if ( ( pRoom = get_room_index( vnum ) )  == NULL )
    {
        send_to_char("That Room does not exist!\n\r", ch);
        return FALSE;
    }

    pItem->roomvnum = vnum;
    printf_to_char(ch, "Room set to vnum %d, '%s'\n\r", pItem->roomvnum, pRoom->name);
    return TRUE;
}
QIEDIT( qiedit_mobile )
{
    QITEM_DATA *pItem;
    MOB_INDEX_DATA *pMob;
    int         vnum;
    EDIT_QITEM(ch, pItem);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  mob [number]\n\r", ch );
    return FALSE;
    }
    vnum = atoi( argument );



    if ( ( pMob = get_mob_index( vnum ) )  == NULL )
    {
        send_to_char("That mobile does not exist!\n\r", ch);
        return FALSE;
    }

    pItem->mobvnum = vnum;
    printf_to_char(ch, "Mobile set to vnum %d, '%s'\n\r", pItem->mobvnum, pMob->short_descr);
    return TRUE;
}
QIEDIT( qiedit_object )
{
    QITEM_DATA *pItem;
    OBJ_INDEX_DATA *pObj;
    int         vnum;
    EDIT_QITEM(ch, pItem);

    if ( argument[0] == '\0' || !is_number( argument ) )
    {
    send_to_char( "Syntax:  room [number]\n\r", ch );
    return FALSE;
    }
    vnum = atoi( argument );



    if ( ( pObj = get_obj_index( vnum ) )  == NULL )
    {
        send_to_char("That object does not exist!\n\r", ch);
        return FALSE;
    }

    if (pObj->item_type != ITEM_CONTAINER)
    {
        send_to_char("That item is not a container!\n\r", ch);
        return FALSE;
    }

    pItem->objvnum = vnum;
    printf_to_char(ch, "Object set to vnum %d, '%s'\n\r", pItem->objvnum, pObj->short_descr);
    return TRUE;
}
QIEDIT( qiedit_reset )
{
    QITEM_DATA *pItem;
    EDIT_QITEM(ch, pItem);

    if (str_cmp(argument, "confirm"))
    {
        send_to_char("Syntax:  reset confirm\n\r", ch);
        send_to_char("This will re-set the quest, clearing loaded, and found states.\n\r",ch);
        return FALSE;
    }
    pItem->loaded = FALSE;
    pItem->found  = FALSE;
    pItem->foundby = str_dup("");
    send_to_char("Quest reset. Run placeqitems to re-place item in the game.\n\r", ch);
    return TRUE;
}
QIEDIT (qiedit_name )
{
    QITEM_DATA *pItem;

    EDIT_QITEM(ch, pItem);

    if (argument[0] == '\0')
    {
        send_to_char("Syntax:  name [name]\n\r", ch);
        send_to_char("Re-names the quest.\n\r", ch);
        return FALSE;
    }
    pItem->name = str_dup(argument);
    return TRUE;
}
QIEDIT( qiedit_delete )
{
    QITEM_DATA *pItem;
    EDIT_QITEM(ch, pItem);

    if (str_cmp(argument, "confirm"))
    {
        send_to_char("Syntax:  delete confirm\n\r", ch);
        return FALSE;
    }
    pItem->name = str_dup("");
    pItem->found = FALSE;
    pItem->foundby = str_dup("None");
    pItem->loaded = FALSE;
    pItem->mobvnum = 0;
    pItem->notified = str_dup("None");
    pItem->notify = FALSE;
    pItem->objvnum = 0;
    pItem->roomvnum = 0;
    pItem->qobjvnum = 0;
    free_qitem(pItem);
    edit_done( ch );
    return TRUE;
}

QIEDIT( qiedit_notify)
{
    QITEM_DATA *pItem;
    char        name[MIL];
    EDIT_QITEM(ch, pItem);

    argument = one_argument(argument, name);

    if (name[0] == '\0')
    {
        send_to_char("Syntax:  notify [name]\n\r", ch);
        send_to_char("         notify none\n\r", ch);
        send_to_char("Name can be any note-acceptable name. eg: Matthew or Immortal\n\r", ch);
        return FALSE;
    }

    if (!str_cmp(name, "none"))
    {
        pItem->notify = FALSE;
        pItem->notified = str_dup("None");
        send_to_char("Notification disabled.\n\r", ch);
        return TRUE;
    }

    pItem->notify = TRUE;
    pItem->notified = str_dup(name);
    return TRUE;

}
QITEM_DATA *new_qitem (void)
{

    extern QITEM_DATA *qitem_list;
    QITEM_DATA *qitem;

//    if (qitem_free == NULL)
    qitem = alloc_perm(sizeof(*qitem));
//    else
//    {
//    qitem = qitem_free;
//    qitem_free = qitem_free->next;
//    }

    qitem->next = qitem_list;
    qitem_list = qitem;

    qitem->name = str_dup("");
    qitem->notified = str_dup("None");
    qitem->foundby = str_dup("None");

    VALIDATE(qitem);
    return qitem;
}

void free_qitem (QITEM_DATA *qitem)
{
    QITEM_DATA *prev;

    if (!IS_VALID(qitem))
    return;

    if (qitem == qitem_list)
        qitem_list = qitem->next;
    if (qitem->next != NULL)
        for (prev = qitem_list ; prev ; prev = prev->next)
        {
            if (prev->next == qitem)
            {
                prev->next = qitem->next;
                break;
            }
        }

    qitem->next = qitem_free;
    qitem_free  = qitem;
    qitem->delete = TRUE;

    INVALIDATE(qitem);
    return;
}

QITEM_DATA *qitem_lookup(const char *name )
{
    QITEM_DATA *qitem;

    for( qitem = qitem_list ; qitem ; qitem = qitem->next )
    {
        if (qitem->delete)
            continue;

        if(!str_cmp(name, qitem->name ) )
            return qitem;
    }
    return NULL;
}

QITEM_DATA *qitem_obj_lookup( OBJ_INDEX_DATA *pObj )
{
    QITEM_DATA *qitem;

    for (qitem = qitem_list ; qitem ; qitem = qitem->next)
    {
        if (qitem->delete)
            continue;
        if (pObj->vnum == qitem->qobjvnum)
            return qitem;
    }
    return NULL;
}

void qitem_check ( CHAR_DATA *ch, OBJ_DATA *obj)
{
    QITEM_DATA *qitem;


    if ((qitem = obj->qitem) != NULL && qitem->found == FALSE && !qitem->delete )
    {
        qitem->found = TRUE;
        qitem->foundby = str_dup(ch->name);

        send_to_char("You found a quest item!\n\r", ch);

        if (qitem->notify)
        {
            char buf[MSL];
            char subject[MSL];
            char *strtime;

            sprintf(subject, "Quest '%s' completed by %s", qitem->name, ch->name);
            strtime             = (char *) ctime( &current_time );
            strtime[strlen(strtime)-1]  = '\0';
            sprintf(buf, "%s found item %s, finishing quest named '%s'.\n\rOn: %s\n\rRoom: %s\n\r", ch->name, obj->short_descr, qitem->name, strtime, ch->in_room->name);
            auto_note(qitem->notified, subject, buf);
        }
    }
}

void do_qitemlist (CHAR_DATA *ch, char *argument)
{
    QITEM_DATA *item;
    OBJ_INDEX_DATA *pItem;
    ROOM_INDEX_DATA *pRoom;
    MOB_INDEX_DATA *pMob;
    OBJ_INDEX_DATA *pObj;

    for (item = qitem_list ; item ; item = item->next)
    {
        if (item->delete)
            continue;

    pItem = get_obj_index(item->qobjvnum);
    pRoom = get_room_index(item->roomvnum);
    pMob  = get_mob_index(item->mobvnum);
    pObj  = get_obj_index(item->objvnum);

    printf_to_char(ch, "\n\r{wQuest                         - {c%s{x\n\r", item->name );
          send_to_char("{w====================================================={x\n\r",ch);
    printf_to_char(ch, "{wItem Vnum                     - {c%d (%s){x\n\r", item->qobjvnum, pItem != NULL ? pItem->short_descr : "Null" );
    printf_to_char(ch, "{wPlaced in/on Room/Mob/Obj     - {c%s{x\n\r", item->place == PLACE_ROOM ? "Room" : item->place == PLACE_MOB ? "Mobile" : "Obj" );
    printf_to_char(ch, "{wPlaced Room                   - {c%d (%s){x\n\r", item->roomvnum, pRoom != NULL ? pRoom->name : "Null" );
    if (item->place == PLACE_MOB)
        printf_to_char(ch, "{wPlaced Mob                    - {c%d (%s){x\n\r", item->mobvnum, pMob != NULL ? pMob->short_descr : "Null");
    if (item->place ==  PLACE_OBJ)
        printf_to_char(ch, "{wPlaced In Obj                 - {c%d (%s){x\n\r", item->objvnum, pObj != NULL ? pObj->short_descr : "Null");
    printf_to_char(ch, "{wNotify Someone?               - {c%s{x\n\r", item->notify ? "True" : "False" );
    printf_to_char(ch, "{wNotify Whom?                  - {c%s{x\n\r", item->notified );
    printf_to_char(ch, "{wLoaded?                       - {c%s{x\n\r", item->loaded ? "True" : "False");
    printf_to_char(ch, "{wFound?                        - {c%s{x\n\r", item->found ? "True" : "False");
    if (item->found)
        printf_to_char(ch, "{wFound by:                     - {c%s{x\n\r", item->foundby);
    send_to_char("{w====================================================={x\n\r",ch);

    }
}

void do_placeqitems (CHAR_DATA *ch, char *argument)
{
    place_qitems();
    send_to_char("Qitems Loaded.\n\r", ch);
    return;
}

void place_qitems( void )
{
    QITEM_DATA     *qitem;
    ROOM_INDEX_DATA *pRoom;
    OBJ_INDEX_DATA *pObj;
    MOB_INDEX_DATA *pMob;
    CHAR_DATA      *mob;
    OBJ_INDEX_DATA *pItem;
    OBJ_DATA       *obj;
    OBJ_DATA       *container;
    char            buf[MSL];

        for( qitem = qitem_list ; qitem ; qitem = qitem->next )
    {
            if (qitem->delete)
                continue;

        pItem = get_obj_index(qitem->qobjvnum);
        pRoom = get_room_index(qitem->roomvnum);
        pMob  = get_mob_index(qitem->mobvnum);
        pObj  = get_obj_index(qitem->objvnum);

        if (qitem->loaded || qitem->found || pItem == NULL || pRoom == NULL ||
                (qitem->place == PLACE_MOB && pMob == NULL) ||
                (qitem->place == PLACE_OBJ && pObj == NULL))
            continue;

        if ((obj = create_object(pItem, 0)) == NULL)
                continue;

        //reference qitem on Obj, for qitem_check
        obj->qitem = qitem;

        switch (qitem->place) {
            case PLACE_ROOM:
                obj_to_room(obj, pRoom);
                qitem->loaded = TRUE;
                break;
            case PLACE_MOB:
                    for ( mob = pRoom->people; mob; mob = mob->next_in_room )
                    if ( IS_NPC( mob ) && mob->pIndexData->vnum == qitem->mobvnum )
                    {
                        obj_to_char(obj, mob);
                        qitem->loaded = TRUE;
                        break;
                    }
                    if (qitem->loaded == FALSE)
                    {
                        sprintf(buf, "place_qitems: Mob %s not in room %d (Quest %s)", pMob->short_descr, pRoom->vnum, qitem->name);
                        log_string( buf);
                        break;
                    }
            case PLACE_OBJ:
                    for ( container = pRoom->contents; container; container = container->next_content )
                    if ( container->pIndexData->vnum == qitem->objvnum )
                    {
                        obj_to_obj(obj,container);
                        qitem->loaded = TRUE;
                        break;
                    }
                    if (qitem->loaded == FALSE)
                    {
                        sprintf(buf, "place_qitems: Obj %s not in room %d (Quest %s)", pObj->short_descr, pRoom->vnum, qitem->name);
                        log_string( buf);
                        break;
                    }

        }
    }
}
DECLARE_DO_FUN( do_say );

ROOM_INDEX_DATA *find_location args( ( CHAR_DATA *ch, char * arg ) );

struct reward_type
{
  char *name;
  char *keyword;
  int cost;
  bool object;
  int value;
  void *      where;
};

struct quest_desc_type {
  char *name;
  char *long_descr;
  char *short_descr;
};

/* Descriptions of quest items go here:
Format is: "keywords", "Short description", "Long description" */
const struct quest_desc_type quest_desc[] =
{
{"quest sceptre",   "the Sceptre of Courage",
"The Sceptre of Courage is lieing here, waiting to be returned to its owner."},

{"quest crown",     "the Crown of Wisdom",
"The Crown of Wisdom is lieing here, waiting to be returned to its owner."},

{"quest gauntlet",  "the Gauntlets of Strength",
"The Gauntlets of Strength are lieing here, waiting to be returned to its owner."},

{NULL, NULL, NULL}
};

/* Local functions */
void generate_quest args(( CHAR_DATA *ch, CHAR_DATA *questman ));
void quest_update   args(( void ));
bool quest_level_diff   args(( int clevel, int mlevel));

/* The main quest function */
void do_quest(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *questman;
    CHAR_DATA *victim;
    OBJ_DATA *obj=NULL, *obj_next;
    OBJ_INDEX_DATA *questinfoobj;
    MOB_INDEX_DATA *questinfo;
    char buf [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MIL];
    int i;

/* Add your rewards here.  Works as follows:
"Obj name shown when quest list is typed", "keywords for buying",
"Amount of quest points",  Does it load an object?,  IF it loads an
object, then the vnum, otherwise a value to set in the next thing,  This
is last field is a part of the char_data to be modified */

// Name, keywords, questpoints, Load obj?, vnum/value, char_data pointer
const struct reward_type reward_table[]=
{

  { "Mystical Orb","mystical orb",   2000, TRUE,  6617,    0},
  { "Emblem of Power",        "emblem power", 2500, TRUE,  6618,    0},
  { "Tabard of Ancient Mysteries", "tabard ancient mysteries", 3000, TRUE,  6601,  0},
  { "a Spirit Guide",     "spirit guide",     3500, TRUE,  6602,  0},
  { "an Umbral Pouch","umbral pouch",  4000, TRUE,  6603,    0},
  { "Arm Bands of Purity",        "arm band purity", 4000, TRUE,  6604,    0},
  { "a Status Ring", "status ring", 4500, TRUE,  6605,  0},
  { "Earrings of Darkness",     "earrings darkness",     5000, TRUE,  6615,  0},
  { "Shield of the Avenging Angel","shield avenging angel",   5000, TRUE,  6606,    0},
  { "Bracer of Willpower",        "bracer willpower", 5500, TRUE,  6607,    0},
  { "Rerebrace of Frenzy", "rerebrace frenzy", 5500, TRUE,  6608,  0},
  { "Gauntlets of Antiquity",     "gauntlets antiquity",    6000, TRUE,  6614,  0},
  { "Face of the Beast","face beast",   6500, TRUE,  6609,    0},
  { "Sandals of the Prophet",        "sandals prophet", 7000, TRUE,  6610,    0},
  { "Greaves of Stamina", "greaves stamina",7500, TRUE, 6611,  0},
  { "Cloak of Subterfuge",     "cloak subterfuge",    8600, TRUE,  6612,  0},
  { "Mask of the Damned","mask damned",  9000, TRUE,  6616,    0},
  { "Archonian Plate",        "archonian plate", 9500, TRUE,  6613,    0},
  { "a potion of Escape to a True Friend", "escape true friend", 250, TRUE, 30432, 0},
  { "an XP Boost Potion", "potion experience boost xp", 500, TRUE, 1103, 0},
  { "the Godly Potion of Mana", "mana", 250, TRUE, 3099, 0},
  { "One Freebie",      "freebie",      3500, FALSE, 1, &ch->freebie},


//  { "350,000 Gold Pieces",     "350000 gold",   500, FALSE, 350000,&ch->gold},
//  { "4 Practices",             "practices",     500, FALSE, 4,     &ch->practice},
  { NULL, NULL, 0, FALSE, 0, 0  } /* Never remove this!!! */
};


    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if(IS_NPC(ch))
    { send_to_char("NPC's can't quest.\n\r",ch); return; }

    if (arg1[0] == '\0')
    {
        send_to_char("Quest commands: Level, List, and Buy.\n\r",ch);
        send_to_char("For more information, type 'Help Quest'.\n\r",ch);
    return;
    }
/*
    if (!strcmp(arg1, "info"))
    {
    if (IS_SET(ch->act2,PLR2_QUESTOR))
    {
        if (ch->questmob == -1 && ch->questgiver->short_descr != NULL) {
            sprintf(buf,"Your quest is ALMOST complete!\n\rGet back to %s before your time runs out!\n\r",ch->questgiver->short_descr);
        send_to_char(buf, ch);
        }
        else if (ch->questobj > 0)
        {
                questinfoobj = get_obj_index(ch->questobj);
        if (questinfoobj != NULL) {
            sprintf(buf,"You are on a quest to recover the fabled %s!\n\r",questinfoobj->name);
            send_to_char(buf, ch);
        }
        else send_to_char("You aren't currently on a quest.\n\r",ch);
        return;
        }
        else if (ch->questmob > 0)
        {
                questinfo = get_mob_index(ch->questmob);
        if (questinfo != NULL) {
            sprintf(buf,"You are on a quest to slay the dreaded %s!\n\r",questinfo->short_descr);
            send_to_char(buf, ch);
        }
        else send_to_char("You aren't currently on a quest.\n\r",ch);
        return;
        }
    }
    else
        send_to_char("You aren't currently on a quest.\n\r",ch);
    return;
    }
    else if (!strcmp(arg1, "time"))
    {
    if (!IS_SET(ch->act2,PLR2_QUESTOR))
    {
        send_to_char("You aren't currently on a quest.\n\r",ch);
        if (ch->nextquest > 1) {
            sprintf(buf,"There are %d minutes remaining until you can go on another quest.\n\r",ch->nextquest);
        send_to_char(buf, ch);
        }
        else if (ch->nextquest == 1) {
        sprintf(buf, "There is less than a minute remaining until you can go on another quest.\n\r");
        send_to_char(buf, ch);
        }
    }
        else if (ch->countdown > 0)
        {
        sprintf(buf, "Time left for current quest: %d\n\r",ch->countdown);
        send_to_char(buf, ch);
    }
    return;
    }
    */
/*
    for ( questman = ch->in_room->people; questman != NULL; questman = questman->next_in_room )
    {
    if (!IS_NPC(questman)) continue;
        if (questman->spec_fun == spec_lookup( "spec_questmaster" )) break;
    }

    if (questman == NULL || questman->spec_fun != spec_lookup("spec_questmaster" ))
    {
        send_to_char("You can't do that here.\n\r",ch);
        return;
    }
    if ( questman->fighting != NULL)
    {
    send_to_char("Wait until the fighting stops.\n\r",ch);
        return;
    }

    ch->questgiver = questman;
*/

    if (!strcmp(arg1, "level"))
    {
        int level;
        int iLevel;
        if (!is_number(arg2))
        {
            send_to_char("You can only buy levels in numerical quantities.\n\r", ch);
            return;
        }
        level = ch->level +  atoi( arg2 );
    if ( level < 1 || atoi (arg2) < 1)
    {
    sprintf(buf,"Level must be 1 to %d.\n\r", 100-ch->level);
    send_to_char(buf, ch);
    return;
    }
    if (atoi(arg2) * 100 > ch->qpoints)
    {
        send_to_char("You do not have enough quest points to buy that many levels.\n\r", ch);
        return;
    }
    if (level > 100)
    {
    send_to_char("You cannot purchase yourself that many levels!", ch);
    return;
    }
     for ( iLevel = ch->level ; iLevel < level; iLevel++ )
    {
    ch->level += 1;
    advance_level( ch,TRUE);
    ch->qpoints -= 100;
    }
    sprintf(buf, "You spend %d quest points and purchase yourself to level %d!\n\r", atoi(arg2)*100, ch->level);
    send_to_char(buf, ch);
    sprintf(buf, "You have %d quest points remaining.\n\r", ch->qpoints);
    send_to_char(buf, ch);
    ch->exp   = exp_per_level(ch,ch->pcdata->points)
          * UMAX( 1, ch->level );
    save_char_obj(ch);
    return;
}
    if (!strcmp(arg1, "gift"))
    {
        int level;
        int iLevel;
        if ( (victim = get_char_world(ch, arg2)) == NULL )
        {
            send_to_char("That person is not currently online or you cannot see them!\n\r", ch);
            return;
        }
        if (!is_number(arg3))
        {
            send_to_char("You can only gift levels in numerical quantities.\n\r", ch);
            return;
        }
        level = victim->level +  atoi( arg3 );
    if ( level < 1 || atoi (arg3) < 1)
    {
    sprintf(buf,"Level must be 1 to %d.\n\r", 100-victim->level);
    send_to_char(buf, ch);
    return;
    }
    if (atoi(arg3) * 100 > ch->qpoints)
    {
        send_to_char("You do not have enough quest points to buy that many levels.\n\r", ch);
        return;
    }
    if (level > 100)
    {
    sprintf(buf, "You cannot purchase %s that many levels!\n\r", victim->name);
    send_to_char(buf, ch);
    return;
    }
     for ( iLevel = victim->level ; iLevel < level; iLevel++ )
    {
    victim->level += 1;
    advance_level( victim,TRUE);
    ch->qpoints -= 100;
    }
    sprintf(buf, "You spend %d quest points and purchase %s to level %d!\n\r", atoi(arg3)*100, victim->name, victim->level);
    send_to_char(buf, ch);
    sprintf(buf, "%s has purchased %d levels for you! Remember to say thank you.\n\r", ch->name, atoi(arg3));
    send_to_char(buf, victim);
    sprintf(buf, "You have %d quest points remaining.\n\r", ch->qpoints);
    send_to_char(buf, ch);
    victim->exp   = exp_per_level(victim,victim->pcdata->points)
          * UMAX( 1, victim->level );
    save_char_obj(ch);
    save_char_obj(victim);
    return;
}
    if (!strcmp(arg1, "list"))
    {
    send_to_char("You ask the Immortals for a list of quest items.\n\r", ch);
    send_to_char("Current Quest Items available for Purchase:\n\r", ch);
    if(reward_table[0].name == NULL)
      send_to_char("  Nothing.\n\r",ch);
    else {
    send_to_char("  {w[{WCost{w]     [{CName{w]\n\r",ch);
    for(i=0;reward_table[i].name != NULL;i++) {
          sprintf(buf,"   {W%-4d{w       {c%s{w\n\r"
            ,reward_table[i].cost,reward_table[i].name);
          send_to_char(buf, ch);
    }
         }
    send_to_char("\n\rTo buy an item, type 'Quest buy <item>'.\n\r",ch);
    return;
    }

    else if (!strcmp(arg1, "buy"))
    {
    bool found=FALSE;
    if (arg2[0] == '\0')
    {
        send_to_char("To buy an item, type 'Quest buy <item>'.\n\rTo see the list type 'Quest list'.\n\r",ch);
        return;
    }
    /* Use keywords rather than the name listed in qwest list */
    /* Do this to avoid problems with something like 'qwest buy the' */
    /* And people getting things they don't want... */
      for(i=0;reward_table[i].name != NULL;i++)
    if (is_name(arg2, reward_table[i].keyword))
    { found = TRUE;
        if (ch->qpoints >= reward_table[i].cost)
        {
        ch->qpoints -= reward_table[i].cost;
        if(reward_table[i].object)
                obj = create_object(get_obj_index(reward_table[i].value),ch->level);
        else
           {
           sprintf(buf,"In exchange for %d quest points, the Immortals give you %s.\n\r",
            reward_table[i].cost, reward_table[i].name );
           send_to_char(buf, ch);
           sprintf(buf, "You have %d quest points remaining.\n\r", ch->qpoints);
           send_to_char(buf, ch);
           *(int *)reward_table[i].where += reward_table[i].value;
           }
        break;
        }
        else
        {
        send_to_char( "Sorry, but you don't have enough quest points for that.",ch);
        return;
        }
    }
    if(!found)
    {
        send_to_char("We don't have that item.",ch);
    }
    if (obj != NULL)
    {
        sprintf(buf,"In exchange for %d quest points, The Immortals give you %s.\n\r",
            reward_table[i].cost, obj->short_descr);
        send_to_char(buf, ch);
           sprintf(buf, "You have %d quest points remaining.\n\r", ch->qpoints);
           send_to_char(buf, ch);
        obj_to_char(obj, ch);
    }
    return;
    }/*
    else if (!strcmp(arg1, "request"))
    {
        act("$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
    act("You ask $N for a quest.",ch, NULL, questman, TO_CHAR);
    if (IS_SET(ch->act2,PLR2_QUESTOR))
    {
        sprintf(buf, "But you're already on a quest!");
        do_say(questman, buf);
        return;
    }
    if (ch->nextquest > 0)
    {
        sprintf(buf, "You're very brave, %s, but let someone else have a chance.",ch->name);
        do_say(questman, buf);
        sprintf(buf, "Come back later.");
        do_say(questman, buf);
        return;
    }

    sprintf(buf, "Thank you, brave %s!",ch->name);
    if (!IS_SET(ch->act2,PLR2_QUESTOR))
    do_say(questman, buf);
        ch->questmob = 0;
    ch->questobj = 0;
    generate_quest(ch, questman);

        if (ch->questmob > 0 || ch->questobj > 0)
    {
            ch->countdown = number_range(10,30);
        SET_BIT(ch->act2,PLR2_QUESTOR);
        sprintf(buf, "You have %d minutes to complete this quest.",ch->countdown);
        do_say(questman, buf);
        sprintf(buf, "May the gods go with you!");
        do_say(questman, buf);
    }
    return;
    }

    else if (!strcmp(arg1, "complete"))
    {
        act("$n informs $N $e has completed $s quest.", ch, NULL, questman, TO_ROOM);
    act("You inform $N you have completed $s quest.",ch, NULL, questman, TO_CHAR);
    if (ch->questgiver != questman)
    {
        sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
        do_say(questman,buf);
        return;
    }

    if (IS_SET(ch->act2,PLR2_QUESTOR))
    {
    bool obj_found = FALSE;
        if (ch->questobj > 0 && ch->countdown > 0)
        {
            for (obj = ch->carrying; obj != NULL; obj= obj_next)
            {
                obj_next = obj->next_content;

            if (obj != NULL && obj->pIndexData->vnum == ch->questobj)
            {
            obj_found = TRUE;
                        break;
            }
            }
        }

     if ((ch->questmob == -1 || (ch->questobj && obj_found))
    && ch->countdown > 0)
    {
    int reward, pointreward;
            reward = number_range(2500,25000);
            pointreward = number_range(20,50);

        sprintf(buf, "Congratulations on completing your quest!");
        do_say(questman,buf);
        sprintf(buf,"As a reward, I am giving you %d glory points, and %d gold.",pointreward,reward);
        do_say(questman,buf);

            REMOVE_BIT(ch->act2,PLR2_QUESTOR);
            ch->questgiver = NULL;
            ch->countdown = 0;
            ch->questmob = 0;
        ch->questobj = 0;
            ch->nextquest = 10;
        ch->gold += reward;
        ch->qpoints += pointreward;
        ch->quest_accum += pointreward;
        if(obj_found) extract_obj(obj);
     return;
    }
     else if((ch->questmob > 0 || ch->questobj > 0)
          && ch->countdown > 0)
        {
        sprintf(buf, "You haven't completed the quest yet, but there is still time!");
        do_say(questman, buf);
        return;
        }
     }
    if (ch->nextquest > 0)
    {
        sprintf(buf,"But you didn't complete your quest in time!");
    }
    else sprintf(buf, "You have to request a quest first, %s.",ch->name);
    do_say(questman, buf);
    return;
    }

    else if (!strcmp(arg1, "quit"))
    {
        act("$n informs $N $e wishes to quit $s quest.", ch, NULL,questman, TO_ROOM);
    act ("You inform $N you wish to quit $s quest.",ch, NULL, questman, TO_CHAR);
    if (ch->questgiver != questman)
    {
        sprintf(buf, "I never sent you on a quest! Perhaps you're thinking of someone else.");
        do_say(questman,buf);
        return;
    }

    if (IS_SET(ch->act2,PLR2_QUESTOR))
    {
            REMOVE_BIT(ch->act2,PLR2_QUESTOR);
            ch->questgiver = NULL;
            ch->countdown = 0;
            ch->questmob = 0;
        ch->questobj = 0;
            ch->nextquest = 30;

        sprintf(buf, "Your quest is over, but for your cowardly behavior, you may not quest again for 30 minutes.");
        do_say(questman,buf);
            return;
        }
    else
    {
    send_to_char("You aren't on a quest!",ch);
    return;
    }
    }
    */


    send_to_char("Quest commands: Level, List, and Buy.\n\r",ch);
//    send_to_char("For more information, type 'Help Quest'.\n\r",ch);
    return;
}

void generate_quest(CHAR_DATA *ch, CHAR_DATA *questman)
{
    CHAR_DATA *victim;
    MOB_INDEX_DATA *vsearch;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *questitem;
    char buf [MAX_STRING_LENGTH];
    long mcounter;
    int mob_vnum;

    for (mcounter = 0; mcounter < 99999; mcounter ++)
    {
    mob_vnum = number_range(50, 32600);

    if ( (vsearch = get_mob_index(mob_vnum) ) != NULL )
    {
            if (quest_level_diff(ch->level, vsearch->level) == TRUE
                && vsearch->pShop == NULL
            && !IS_SET(vsearch->act, ACT_TRAIN)
            && !IS_SET(vsearch->act, ACT_PRACTICE)
            && !IS_SET(vsearch->act, ACT_IS_HEALER)
        && !IS_SET(vsearch->act, ACT_PET)
        && !IS_SET(vsearch->affected_by, AFF_CHARM)
        && !IS_SET(vsearch->affected_by, AFF_INVISIBLE)
        && number_percent() < 40) break;
        else vsearch = NULL;
    }
    }

    if ( vsearch == NULL || ( victim = get_char_world( ch, vsearch->player_name ) ) == NULL )
    {
    sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
    do_say(questman, buf);
    sprintf(buf, "Try again later.");
    do_say(questman, buf);
    ch->nextquest = 2;
        return;
    }

    if ( ( room = find_location( ch, victim->name ) ) == NULL )
    {
    sprintf(buf, "I'm sorry, but I don't have any quests for you at this time.");
    do_say(questman, buf);
    sprintf(buf, "Try again later.");
    do_say(questman, buf);
    ch->nextquest = 2;
        return;
    }

    /*  40% chance it will send the player on a 'recover item' quest. */

    if (number_percent() < 40)
    {
    int numobjs=0;
    int descnum = 0;

    for(numobjs=0;quest_desc[numobjs].name != NULL;numobjs++)
        ;
    numobjs--;
    descnum = number_range(0,numobjs);
        questitem = create_object( get_obj_index(QUEST_OBJ), ch->level );
if(descnum > -1)
{
    if(questitem->short_descr)
       free_string(questitem->short_descr);
    if(questitem->description)
       free_string(questitem->description);
    if(questitem->name)
       free_string(questitem->name);

    questitem->name        = str_dup(quest_desc[descnum].name);
    questitem->description = str_dup(quest_desc[descnum].long_descr);
    questitem->short_descr = str_dup(quest_desc[descnum].short_descr);
}
    obj_to_room(questitem, room);
    ch->questobj = questitem->pIndexData->vnum;

    sprintf(buf, "Vile pilferers have stolen %s from the royal treasury!",questitem->short_descr);
    do_say(questman, buf);
    do_say(questman, "My court wizardess, with her magic mirror, has pinpointed its location.");

    sprintf(buf, "Look in the general area of %s for %s!",room->area->name, room->name);
    do_say(questman, buf);

    return;
    }

    /* Quest to kill a mob */

    else
    {
    switch(number_range(0,1))
    {
    case 0:
        sprintf(buf, "An enemy of mine, %s, is making vile threats against the crown.",victim->short_descr);
        do_say(questman, buf);
        sprintf(buf, "This threat must be eliminated!");
        do_say(questman, buf);
    break;

    case 1:
    sprintf(buf, "Rune's most heinous criminal, %s, has escaped from the dungeon!",victim->short_descr);
    do_say(questman, buf);
    sprintf(buf, "Since the escape, %s has murdered %d civillians!",victim->short_descr, number_range(2,20));
    do_say(questman, buf);
    do_say(questman,"The penalty for this crime is death, and you are to deliver the sentence!");

    break;
    }

    if (room->name != NULL)
    {
        sprintf(buf, "Seek %s out somewhere in the vicinity of %s!",victim->short_descr,room->name);
        do_say(questman, buf);

    sprintf(buf, "That location is in the general area of %s.",room->area->name);
    do_say(questman, buf);
    }
    ch->questmob = victim->pIndexData->vnum;
    }

    return;
}

bool quest_level_diff(int clevel, int mlevel)
{
    if (clevel < 9 && mlevel < clevel + 2) return TRUE;
    else if (clevel <= 9 && mlevel < clevel + 3
      && mlevel > clevel - 5) return TRUE;
    else if (clevel <= 14 && mlevel < clevel + 4
      && mlevel > clevel - 5) return TRUE;
    else if (clevel <= 21 && mlevel < clevel + 5
      && mlevel > clevel - 4) return TRUE;
    else if (clevel <= 29 && mlevel < clevel + 6
      && mlevel > clevel - 3) return TRUE;
    else if (clevel <= 37 && mlevel < clevel + 7
      && mlevel > clevel - 2) return TRUE;
    else if (clevel <= 55 && mlevel < clevel + 8
      && mlevel > clevel - 1) return TRUE;
    else if(clevel > 55) return TRUE; /* Imms can get anything :) */
    else return FALSE;
}

/* Called from update_handler() by pulse_area */

void quest_update(void)
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if (d->character != NULL && d->connected == CON_PLAYING)
        {
        ch = d->character;
    if(IS_NPC(ch))
    continue;
        if (ch->nextquest > 0)
        {
            ch->nextquest--;
            if (ch->nextquest == 0)
            {
                send_to_char("You may now quest again.\n\r",ch);
                return;
            }
        }
        else if (IS_SET(ch->act2,PLR2_QUESTOR))
        {
            if (--ch->countdown <= 0)
            {
                ch->nextquest = 10;
                sprintf(buf,"You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",ch->nextquest);
        send_to_char(buf, ch);
                REMOVE_BIT(ch->act2,PLR2_QUESTOR);
                ch->questgiver = NULL;
                ch->countdown = 0;
                ch->questmob = 0;
                ch->questobj = 0;
            }
            if (ch->countdown > 0 && ch->countdown < 6)
            {
                send_to_char("Better hurry, you're almost out of time for your quest!\n\r",ch);
                return;
            }
        }
        }
    }
    return;
}
