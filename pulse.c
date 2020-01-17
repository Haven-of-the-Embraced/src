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
    if ( IS_AFFECTED (ch, AFF_POISON) && ch->hit > (ch->max_hit / 20) &&
        godice(get_attribute(ch, STAMINA), 8) < 1)
        damage(ch, ch, (ch->max_hit / (50*get_attribute(ch, STAMINA))),
            gsn_poison,DAM_POISON,FALSE);

    if (is_affected(ch, gsn_shadowplay))
    {
        int level = get_affect_level(ch, gsn_shadowplay);
        if (number_percent() < 5 * level)
        {
            act( "$n chokes and gags as shadows try to suffocate $m.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You gag and struggle to breathe through the shadows in your throat.\n\r", ch );
            if(ch->position != POS_TORPOR)
                damage(ch,ch,ch->max_hit/(10*(15-level)),gsn_shadowplay,DAM_BASH,FALSE);
        }
    }

    // Player-Only affects below this point.
    if (IS_NPC(ch))
        continue;



    } //for char_list

};
