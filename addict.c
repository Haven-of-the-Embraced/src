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

    return bonus;
}
