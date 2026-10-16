/*
 * Letter writing code
 *
 *  Written by John Patrick (j.s.patrick@ieee.org)
 *   for Ansalon (ansalon.wolfpaw.net 8679)
 *
 * Only need new item type, ITEM_NOTEPAPER.  No additional
 * values are needed, as all value[] variable are used by command.
 *
 * I'm very big in intellectual property rights.  In order to use
 * this code or any derivatives in your MUD, you must do the following:
 *
 *  - E-mail me at j.s.patrick@ieee.org
 *  - Have a helpfile for the item that includes a credit to me, with
 *    the above e-mail address in it.  (E.g. This code was written by
 *    John Patrick (j.s.patrick@ieee.org) and used with permission.)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"

void
do_write(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *letter;
  char buffer[MAX_STRING_LENGTH];

  /* NPCs can't write.  */
   if (ch->csabilities[CSABIL_ACADEMICS] < 2)
   {
       send_to_char("You're not academic enough to write letters!\n\r",ch);
       return;
   }
  if (IS_NPC(ch))
    {
      send_to_char("You can't write.\n\r",ch);
      return;
    }

  /* Look for a held item.  */
  for ( letter = ch->carrying; letter; letter = letter->next_content )
    {
      if (   (letter->item_type == ITEM_NOTEPAPER)
          && (letter->wear_loc == WEAR_HOLD))
        break;
    }


  if (letter == NULL)
    {
      send_to_char("What do you want to write on?\n\r",ch);
      return;
    }

  /* Check to see if it's blank.  */
  if (letter->value[1] != 0)
    {
      send_to_char("It's already been written on.\n\r",ch);
      return;
    }

  /* Smash the tildes! */
  smash_tilde(argument);

  /* Write the note.  */
  letter->extra_descr = new_extra_descr();
  letter->extra_descr->description = str_dup(argument);
  letter->extra_descr->keyword = str_dup("");
  letter->extra_descr->next = NULL;
     /* Note: By using an extra-description, it allows us to have a different
              long message from the contents of the message.  By using a NULLed
              keyword, it prevents a character from reading a sealed note. */

  /* Flag the letter as being written.  */
  letter->value[1] = 1;

  /* Make sure it's not flagged sealed.  */
  letter->value[2] = 0;

  /* Set the short description to say a written letter.  */
  free_string(letter->short_descr);
  letter->short_descr = str_dup("a hand-written note");

  /* Change the long description. */
  free_string(letter->description);
  sprintf(buffer,"A hand-written note is here.");
  letter->description = str_dup(buffer);

  /* Let the characters know.  */
  send_to_char("You pen your note.\n\r",ch);
  act("$n writes a note.",ch,NULL,NULL,TO_ROOM);

  return;
}

void
do_read(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *letter;

  /* Look for an item matching argument.  */
  letter = get_obj_list(ch,argument,ch->carrying);
   if (ch->csabilities[CSABIL_ACADEMICS] == 0)
   {
       send_to_char("You're not academic enough to read letters!\n\r",ch);
       return;
   }
  if (letter == NULL)
    {
      send_to_char("What do you want to read?\n\r",ch);
      return;
    }

  if (letter->item_type != ITEM_NOTEPAPER)
    {
      send_to_char("You can't read that.\n\r",ch);
      return;
    }

  /* Check to see if it's sealed.  */
  if ((letter->value[2] != 0) && !IS_IMMORTAL(ch))
    {
      send_to_char("It is sealed.\n\r",ch);
      return;
    }

  /* Check to see if it's blank.  */
  if (letter->value[1] == 0)
    {
      send_to_char("It is blank.\n\r",ch);
      return;
    }

  /* Read it.  */
  send_to_char("The note reads:\n\r",ch);
  send_to_char(letter->extra_descr->description,ch);
  send_to_char("\n\r",ch);

  /* Let the characters know.  */
  act("$n reads a note.",ch,NULL,NULL,TO_ROOM);
}

void
do_seal(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *letter;
  char buffer[MAX_STRING_LENGTH];

  /* Look for an item matching argument.  */
  letter = get_obj_list(ch,argument,ch->carrying);

  if ((ch->pcdata->csbackgrounds[CSBACK_RESOURCES] <= 1) && (ch->pcdata->csbackgrounds[CSBACK_STATUS] == 0))
  {
      send_to_char("You do not have enough resources or status for that!\n\r",ch);
      return;
  }

  if (letter == NULL)
    {
      send_to_char("What do you want to seal?\n\r",ch);
      return;
    }

  if (letter->item_type != ITEM_NOTEPAPER)
    {
      send_to_char("You can't seal that.\n\r",ch);
      return;
    }

  /* Check to see if it's sealed.  */
  if (letter->value[2] != 0)
    {
      send_to_char("It is already sealed.\n\r",ch);
      return;
    }

  /* OK, seal it, and let people know who sealed it.  */
  letter->value[2]=1;
  /* Change the long description. */
  free_string(letter->description);
  sprintf(buffer,"A note is here, sealed with the crest of %s.",ch->name);
  letter->description = str_dup(buffer);

  send_to_char("You seal the note.\n\r",ch);
  act("$n seals a note.",ch,NULL,NULL,TO_ROOM);
}

void
do_unseal(CHAR_DATA *ch, char *argument)
{
  OBJ_DATA *letter;
  char buffer[MAX_STRING_LENGTH];

  /* Look for an item matching argument.  */
  letter = get_obj_list(ch,argument,ch->carrying);

  if (letter == NULL)
    {
      send_to_char("What do you want to unseal?\n\r",ch);
      return;
    }

  if (letter->item_type != ITEM_NOTEPAPER)
    {
      send_to_char("You can't unseal that.\n\r",ch);
      return;
    }

  /* Check to see if it's sealed.  */
  if (letter->value[2] == 0)
    {
      send_to_char("It is already unsealed.\n\r",ch);
      return;
    }

  /* OK, unseal it and let people know.  */
  letter->value[2]=0;
  /* Change the long description. */
  free_string(letter->description);
  sprintf(buffer,"A hand-written note is here.");
  letter->description = str_dup(buffer);

  send_to_char("You unseal the note.\n\r",ch);
  act("$n unseals a note.",ch,NULL,NULL,TO_ROOM);
}
