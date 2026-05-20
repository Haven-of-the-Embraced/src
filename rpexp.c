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
#include <time.h>
#include "merc.h"

void gain_rp_activity(CHAR_DATA *ch, char *argument)
{
    if (IS_NPC(ch))
        return;

    if (!IS_SET(ch->act, PLR_IC))
        return;

    if (argument == NULL || argument[0] == '\0')
        return;

    // Increment line count
    ch->pcdata->rp_lines++;

    // Base activity points for any valid RP line
    long points = 5;

    // Bonus points based on length (1 point per 10 characters)
    int len = strlen(argument);
    points += len / 10;

    // Cap the points from a single RP action to prevent paste abuse
    if (points > 100)
        points = 100;

    ch->pcdata->rp_activity += points;

    // Cap the total activity pool
    if (ch->pcdata->rp_activity > MAX_RP_ACTIVITY)
        ch->pcdata->rp_activity = MAX_RP_ACTIVITY;
}

void gain_social_activity(CHAR_DATA *ch)
{
    if (IS_NPC(ch))
        return;

    if (!IS_SET(ch->act, PLR_IC))
        return;

    // Small fixed bonus for using a social
    long points = 5;

    ch->pcdata->rp_activity += points;

    // Cap the total activity pool
    if (ch->pcdata->rp_activity > MAX_RP_ACTIVITY)
        ch->pcdata->rp_activity = MAX_RP_ACTIVITY;
}
