#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

void affects_update (void) {
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    AFFECT_DATA af;


    /* Examine all mobs. */
    for ( ch = char_list; ch != NULL; ch = ch_next )
    {
    ch_next = ch->next;

    //EMERGOD POISON ACTUALLY SUCKS NOW CODE
    if ( IS_AFFECTED (ch, AFF_POISON) && ch->hit > (ch->max_hit / 20) )
        damage(ch,ch,
            (ch->hit/(50*get_attribute(ch, STAMINA) > 0 ? get_attribute(ch, STAMINA) : 1 )),
            gsn_poison,DAM_POISON,FALSE);

    // Player-Only affects below this point.
    if (IS_NPC(ch))
        continue;



    } //for char_list

};
