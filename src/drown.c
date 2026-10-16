#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "music.h"
#include "recycle.h"
#include "tables.h"






void underwater_update( void )
{
   CHAR_DATA *ch;
   CHAR_DATA *ch_next;

   for ( ch = char_list; ch != NULL; ch = ch_next)
   {
      ch_next = ch->next;
      if (ch->in_room != NULL && !IS_SET (ch->in_room->room_flags, ROOM_UNDERWATER) && (ch->airtick < 5))
      {// Changed || or to && and so ch only gets air if they're not underwater
      // AND their air is less than 5.
      ch->airtick += 1;
      }
      if (!IS_NPC(ch)
      && !IS_IMMORTAL(ch)
      && ch->race != race_lookup("vampire") // moved ! from !ch->race to ch->race !=
      // !ch->race is silly It translates in code to "The opposite of ch's race", not
      // what you wanted it to mean.
      && IS_SET(ch->in_room->room_flags, ROOM_UNDERWATER) )
     {
          if (ch->airtick > 0)
          {
              ch->airtick -= 1;
              send_to_char("You're under water and running out of air!\n\r",ch);
          } else {
              send_to_char("You're drowning!!!\n\r", ch);
              damage(ch,ch,ch->max_hit/4,0,DAM_DROWNING,FALSE);
                    /* Using damage instead of manually changing HP makes the
                    the rest of this function unnecessary. Damage automatically
                    Kills the char if their HP drops below 0. Using FALSE as the
                    last argument makes it so the MUD doesn't show the damage, it
                    does it silently and just takes away hp.*/


            }
        }
    }
}
