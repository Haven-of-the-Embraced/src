#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "addict.h"

PC_DATA         *pc_first;
PC_DATA         *pc_last;

void save_player_list()
{
    FILE *fp;
    PC_DATA *pcdata;
    char buf[MSL];
    int count = 0;

    if (pc_first == pc_last)
        return;

    if ((fp = fopen(PLAYERLIST_FILE, "w")) == NULL)
    {
        sprintf(buf, "Could not write to file: %s", PLAYERLIST_FILE);
        log_string( buf );
        return;
    }

    for (pcdata = pc_first; pcdata != NULL; pcdata = pcdata->pc_next)
    {
        fprintf(fp, "%d %s\n", count++, pcdata->character->name);
    }

    fprintf(fp, "-1\n");
    fclose(fp);
    return;
}

void load_player_list()
{
    FILE *fp;
    PC_DATA *pcdata;
    char *buf[MSL];
    int num;
    char name[100];
    CHAR_DATA *fch;

    if ((fp = fopen(PLAYERLIST_FILE, "r")) == NULL)
    {
        sprintf(buf, "Could not read from file: %s", PLAYERLIST_FILE);
        log_string( buf );
        return;
    }

    for (;;)
    {
        fscanf (fp, "%d %s\n", &num, name);
        if (num == -1)
            break;

        if ((fch = get_char_world(NULL, name)) == NULL)
        {
            sprintf(buf, "load_player_list: Could not find character (%s) to re-list", name);
            bug( buf, 0 );
            break;
        }

        if ((pcdata = fch->pcdata) == NULL)
        {
            sprintf(buf, "load_player_list: Got mobile (%s) to re-list, ERROR!", name);
            bug( buf, 0 );
            break;
        }

        if (!pc_first)
        {
            pc_first = pc_last = pcdata;
            sprintf(buf, "%s restored to addict spot.", name);
            log_string( buf );
            continue;
        } else {
            pcdata->pc_next = NULL;
            pcdata->pc_prev = pc_last;
            pc_last->pc_next = pcdata;
        }
        pc_last = pcdata;
        sprintf(buf, "Appended %s to end of player list.", name);
        log_string( buf );
    }

    fclose(fp);
    unlink(PLAYERLIST_FILE);

}

int get_addict_bonus(CHAR_DATA *ch, int xp)
{
    double bonus;
    double axp;

    axp = get_addict_mult(ch) * (double) xp / 100;
    axp += 0.5;
    return (int) axp;

}

double get_addict_mult(CHAR_DATA *ch)
{
    double bonus;
    bonus = (double) ADDICTED(ch);
    bonus *= 1.5;

    if (bonus > 15)
        bonus = 15;

    if (FIRST(ch))
        bonus += 1;

    return bonus;
}
