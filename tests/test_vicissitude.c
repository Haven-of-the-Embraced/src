#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"

struct tier_case
{
    const char *name;
    int dots;
    int blood;
    int hit_gain;
    int hit_cap;
    int hitroll;
    int damroll;
    int armor;
    bool canine;
};

static const struct tier_case tiers[] =
{
    { "hellhound", 2, 50, 1000, 2000, 100, 75, 100, TRUE },
    { "szlachta", 3, 60, 5000, 3000, 100, 100, 500, FALSE },
    { "mesnik", 4, 70, 5000, 3750, 75, 200, 650, FALSE },
    { "vozhd", 5, 80, 5000, 5000, 400, 400, 1500, FALSE }
};

extern void save_area(AREA_DATA *area);

static CHAR_DATA *new_target(const struct tier_case *tier, ROOM_INDEX_DATA *room, int health)
{
    CHAR_DATA *target = create_mobile(get_mob_index(tier->canine ? 666667 : 666668));
    int armor;

    assert(target != NULL);
    target->max_hit = target->hit = health;
    target->hitroll = 17;
    target->damroll = 23;
    for (armor = 0; armor < 4; armor++)
        target->armor[armor] = 19 + armor * 10;
    char_to_room(target, room);
    return target;
}

static void invoke(CHAR_DATA *vampire, const struct tier_case *tier)
{
    char command[MAX_INPUT_LENGTH];

    vampire->wait = 0;
    snprintf(command, sizeof(command), "fleshcraft %s %s", tier->name,
        tier->canine ? "loftcanine" : "loftvolunteer");
    interpret(vampire, command);
}

static void assert_unchanged(CHAR_DATA *vampire, CHAR_DATA *target, int blood)
{
    int armor;

    assert(vampire->pblood == blood);
    assert(vampire->pet == NULL);
    assert(vampire->wait == 0);
    assert(target->max_hit == 100 && target->hit == 100);
    assert(target->hitroll == 17 && target->damroll == 23);
    assert(target->master == NULL && target->leader == NULL);
    assert(!IS_AFFECTED(target, AFF_CHARM));
    for (armor = 0; armor < 4; armor++)
        assert(target->armor[armor] == 19 + armor * 10);
}

static void assert_created(CHAR_DATA *vampire, CHAR_DATA *target,
    const struct tier_case *tier, int dots, int health)
{
    int armor;

    assert(vampire->pblood == 10);
    assert(vampire->pet == target);
    assert(vampire->wait == 300);
    assert(target->master == vampire && target->leader == vampire);
    assert(is_name((char *)tier->name, target->name));
    assert(IS_AFFECTED(target, AFF_CHARM));
    assert(is_affected(target, gsn_charm_person));
    assert(target->max_hit == UMIN(health + tier->hit_gain, tier->hit_cap * dots));
    assert(target->hit == target->max_hit);
    assert(target->hitroll == 17 + tier->hitroll);
    assert(target->damroll == 23 + tier->damroll);
    for (armor = 0; armor < 4; armor++)
        assert(target->armor[armor] == 19 + armor * 10 - tier->armor);
    if (!strcmp(tier->name, "vozhd"))
        assert(is_affected(target, gsn_vicissitude_bonecraft));
}

int main(void)
{
    DESCRIPTOR_DATA *descriptor;
    CHAR_DATA *vampire;
    CHAR_DATA *target;
    CHAR_DATA *next;
    ROOM_INDEX_DATA *loft;
    CMD_DATA *command;
    int fixture_count = 0;
    int tier_index;
    int dots;
    int health_case;
    int cases = 0;

    current_time = time(NULL);
    fpReserve = fopen(NULL_FILE, "r");
    assert(fpReserve != NULL);
    boot_db();
    loft = get_room_index(666672);
    assert(loft != NULL);
    for (target = char_list; target != NULL; target = next)
    {
        next = target->next;
        if (target->pIndexData->vnum == 666667 || target->pIndexData->vnum == 666668)
        {
            assert(target->in_room == loft);
            fixture_count++;
            extract_char(target, TRUE);
        }
    }
    assert(fixture_count == 2);

    descriptor = new_descriptor();
    assert(load_char_obj(descriptor, "Testvamp"));
    vampire = descriptor->character;
    assert(!strcmp(vampire->name, "Testvamp"));
    assert(vampire->level == 100 && IS_VAMP(vampire));
    assert(!IS_IMMORTAL(vampire) && !IS_ADMIN(vampire));
    vampire->desc = NULL;
    descriptor->character = NULL;
    free_descriptor(descriptor);
    vampire->next = char_list;
    char_list = vampire;
    vampire->in_room = NULL;
    char_to_room(vampire, loft);
    command = cmd_lookup("fleshcraft");
    assert(command != NULL && command->do_fun == do_fleshcraft);
    assert(command->level <= vampire->level);

    for (tier_index = 0; tier_index < sizeof(tiers) / sizeof(tiers[0]); tier_index++)
    {
        const struct tier_case *tier = &tiers[tier_index];

        for (dots = 1; dots <= 5; dots++)
        {
            for (health_case = 0; health_case < 2; health_case++)
            {
                int health = health_case ? 30000 : 100;

                vampire->pcdata->discipline[VICISSITUDE] = dots;
                vampire->pblood = tier->blood + 10;
                target = new_target(tier, loft, health);
                invoke(vampire, tier);
                if (dots < tier->dots)
                {
                    assert(vampire->pblood == tier->blood + 10 && vampire->pet == NULL);
                    assert(target->max_hit == health && target->hitroll == 17 && target->damroll == 23);
                    assert(target->master == NULL && !IS_AFFECTED(target, AFF_CHARM));
                }
                else
                    assert_created(vampire, target, tier, dots, health);
                extract_char(target, TRUE);
                assert(vampire->pet == NULL);
                cases++;
            }
        }

        vampire->pcdata->discipline[VICISSITUDE] = tier->dots;
        vampire->pblood = tier->blood + 9;
        target = new_target(tier, loft, 100);
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 9);
        extract_char(target, TRUE);
        cases++;

        vampire->pblood = tier->blood + 10;
        target = new_target(tier, loft, 100);
        target->race = race_lookup(tier->canine ? "human" : "dog");
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 10);
        extract_char(target, TRUE);
        cases++;

        target = new_target(tier, loft, 100);
        target->race = race_lookup(tier->canine ? "wolf" : "ghoul");
        invoke(vampire, tier);
        assert_created(vampire, target, tier, tier->dots, 100);
        extract_char(target, TRUE);
        cases++;

        vampire->pblood = tier->blood + 10;
        target = new_target(tier, loft, 100);
        target->level = vampire->level + 26;
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 10);
        extract_char(target, TRUE);
        cases++;

        target = new_target(tier, loft, 100);
        SET_BIT(loft->room_flags, ROOM_SAFE);
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 10);
        REMOVE_BIT(loft->room_flags, ROOM_SAFE);
        extract_char(target, TRUE);
        cases++;

        target = new_target(tier, loft, 100);
        vampire->race = race_lookup("human");
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 10);
        vampire->race = race_lookup("vampire");
        extract_char(target, TRUE);
        cases++;

        target = new_target(tier, loft, 100);
        SET_BIT(vampire->affected2_by, AFF2_QUIETUS_BLOODCURSE);
        invoke(vampire, tier);
        assert_unchanged(vampire, target, tier->blood + 10);
        REMOVE_BIT(vampire->affected2_by, AFF2_QUIETUS_BLOODCURSE);
        extract_char(target, TRUE);
        cases++;

        {
            CHAR_DATA *companion;
            int blood;
            int hitroll;
            int damroll;
            char command_buffer[MAX_INPUT_LENGTH];

            vampire->pblood = 200;
            companion = new_target(tier, loft, 100);
            invoke(vampire, tier);
            assert(vampire->pet == companion);
            blood = vampire->pblood;
            target = new_target(tier, loft, 100);
            invoke(vampire, tier);
            assert(vampire->pet == companion && vampire->pblood == blood);
            assert(target->master == NULL && target->hitroll == 17 && target->damroll == 23);
            extract_char(target, TRUE);
            cases++;

            vampire->pet = NULL;
            hitroll = companion->hitroll;
            damroll = companion->damroll;
            snprintf(command_buffer, sizeof(command_buffer), "fleshcraft %s warghoul", tier->name);
            vampire->wait = 0;
            interpret(vampire, command_buffer);
            assert(vampire->pblood == blood && vampire->pet == NULL);
            assert(companion->hitroll == hitroll && companion->damroll == damroll);
            vampire->pet = companion;
            extract_char(companion, TRUE);
            assert(vampire->pet == NULL);
            cases++;
        }

        printf("PASS: %s dots, blood boundaries, HP growth/cap, stats, target rules, and restrictions\n", tier->name);
    }
    save_area(loft->area);
    printf("PASS: Testvamp completed %d Vicissitude cases; live player data unchanged\n", cases);
    return 0;
}