#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "tables.h"

extern void save_area(AREA_DATA *area);

int main(void)
{
    AREA_DATA *area;
    ROOM_INDEX_DATA *cliff;
    ROOM_INDEX_DATA *hall;
    ROOM_INDEX_DATA *altar;
    ROOM_INDEX_DATA *destination;
    MOB_INDEX_DATA *acolyte;
    OBJ_INDEX_DATA *cauldron;
    OBJ_INDEX_DATA *key;
    CHAR_DATA *fellwyr;
    OBJ_DATA *offering;
    RESET_DATA *reset;
    int vnum;
    int reset_count = 0;
    int high_vnum = 666666;
    char climb_up[] = "climb chain";
    char climb_down[] = "climb chains";
    SHOP_DATA shop = {0};
    QITEM_DATA quest = {0};
    PROG_CODE program = {0};
    PROG_LIST trigger = {0};
    LOOT_TABLE_DATA loot = {0};
    DOMAIN_DATA domain = {0};
    CLAN_DATA clan = {0};
    struct class_type character_class = {0};
    struct weapon_type weapon = {0};
    struct sect_type sect = {0};
    struct brew_type brew = {0};

    shop.keeper = high_vnum;
    quest.qobjvnum = quest.roomvnum = quest.mobvnum = quest.objvnum = high_vnum;
    program.vnum = trigger.vnum = loot.vnum = loot.slots[0].vnum = high_vnum;
    domain.vnum = clan.clanhall = character_class.weapon = character_class.guild[0] = weapon.vnum = high_vnum;
    sect.hall = brew.component[0] = high_vnum;
    assert(shop.keeper == high_vnum);
    assert(quest.qobjvnum == high_vnum && quest.roomvnum == high_vnum);
    assert(quest.mobvnum == high_vnum && quest.objvnum == high_vnum);
    assert(program.vnum == high_vnum && trigger.vnum == high_vnum);
    assert(loot.vnum == high_vnum && loot.slots[0].vnum == high_vnum);
    assert(domain.vnum == high_vnum && clan.clanhall == high_vnum);
    assert(character_class.weapon == high_vnum && character_class.guild[0] == high_vnum);
    assert(weapon.vnum == high_vnum);
    assert(sect.hall == high_vnum && brew.component[0] == high_vnum);
    assert(MAX_KEY_HASH == 4096);

    current_time = time(NULL);
    fpReserve = fopen(NULL_FILE, "r");
    assert(fpReserve != NULL);
    boot_db();

    area = area_first;
    assert(area != NULL && area->next == NULL);
    assert(!strcmp(area->file_name, "fane.are"));
    assert(area->min_vnum == 666666 && area->max_vnum == 666672);
    assert(top_room == 7);
    for (vnum = 666666; vnum <= 666672; vnum++)
        assert(get_room_index(vnum) != NULL);

    cliff = get_room_index(666666);
    hall = get_room_index(666667);
    altar = get_room_index(666668);
    assert(cliff->exit[DIR_SOUTH]->u1.to_room == hall);
    assert(hall->exit[DIR_NORTH]->u1.to_room == cliff);
    assert(cliff->exit[DIR_SOUTH]->key == 666667);
    assert(IS_SET(cliff->exit[DIR_SOUTH]->exit_info, EX_CLOSED));
    assert(IS_SET(hall->exit[DIR_NORTH]->exit_info, EX_CLOSED));
    assert(altar->exit[DIR_WEST]->u1.to_room->vnum == 666669);
    assert(altar->exit[DIR_EAST]->u1.to_room->vnum == 666670);
    assert(altar->exit[DIR_SOUTH]->u1.to_room->vnum == 666671);
    assert(altar->exit[DIR_UP]->u1.to_room->vnum == 666672);
    assert(IS_SET(altar->exit[DIR_UP]->exit_info, EX_CLIMB));
    assert(IS_SET(altar->exit[DIR_UP]->rs_flags, EX_CLIMB));
    assert(IS_SET(get_room_index(666672)->exit[DIR_DOWN]->exit_info, EX_CLIMB));
    assert(cmd_lookup("climb") != NULL && cmd_lookup("climb")->level == 0);
    assert(cmd_lookup("climb")->do_fun == do_climb);

    acolyte = get_mob_index(666666);
    cauldron = get_obj_index(666666);
    key = get_obj_index(666667);
    assert(acolyte != NULL && cauldron != NULL && key != NULL);
    assert(IS_SET(acolyte->act, ACT_STAY_AREA));
    assert(!IS_SET(acolyte->act, ACT_SENTINEL));
    assert(IS_SET(acolyte->act, ACT_MAGE));
    for (fellwyr = char_list; fellwyr != NULL; fellwyr = fellwyr->next)
        if (fellwyr->pIndexData == acolyte)
            break;
    assert(fellwyr != NULL && fellwyr->in_room == hall);
    for (offering = object_list; offering != NULL; offering = offering->next)
        if (offering->pIndexData == key)
            break;
    assert(offering != NULL && offering->in_obj != NULL);
    assert(offering->in_obj->pIndexData == cauldron);
    assert(offering->in_obj->in_room == altar);
    for (reset = hall->reset_first; reset != NULL; reset = reset->next)
        if (reset->command == 'M')
        {
            assert(reset->arg1 == 666666 && reset->arg3 == 666667);
            reset_count++;
        }
    assert(reset_count == 1);

    fellwyr->questobj = fellwyr->questmob = high_vnum;
    assert(fellwyr->questobj == high_vnum && fellwyr->questmob == high_vnum);
    move_char(fellwyr, DIR_NORTH, FALSE);
    assert(fellwyr->in_room == hall);
    do_open(fellwyr, "doors");
    assert(!IS_SET(cliff->exit[DIR_SOUTH]->exit_info, EX_CLOSED));
    assert(!IS_SET(hall->exit[DIR_NORTH]->exit_info, EX_CLOSED));
    move_char(fellwyr, DIR_NORTH, FALSE);
    assert(fellwyr->in_room == cliff);
    char_from_room(fellwyr);
    char_to_room(fellwyr, altar);
    move_char(fellwyr, DIR_UP, FALSE);
    assert(fellwyr->in_room == altar);
    do_climb(fellwyr, "");
    do_climb(fellwyr, "statue");
    assert(fellwyr->in_room == altar);
    interpret(fellwyr, climb_up);
    assert(fellwyr->in_room == get_room_index(666672));
    move_char(fellwyr, DIR_DOWN, FALSE);
    assert(fellwyr->in_room == get_room_index(666672));
    interpret(fellwyr, climb_down);
    assert(fellwyr->in_room == altar);
    for (vnum = 666666; vnum <= 666672; vnum++)
        SET_BIT(get_room_index(vnum)->room_flags, ROOM_SAFE);
    assert(get_random_room(fellwyr) == cliff);
    for (vnum = 666666; vnum <= 666672; vnum++)
    {
        REMOVE_BIT(get_room_index(vnum)->room_flags, ROOM_SAFE);
        destination = get_random_room(fellwyr);
        assert(destination == get_room_index(vnum));
        SET_BIT(get_room_index(vnum)->room_flags, ROOM_SAFE);
    }
    for (vnum = 666666; vnum <= 666672; vnum++)
        REMOVE_BIT(get_room_index(vnum)->room_flags, ROOM_SAFE);
    save_area(area);
    puts("PASS: high VNUMs, doors, climb-only chain route, random rooms, and area save/reload");
    return 0;
}