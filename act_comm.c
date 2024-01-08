/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 **************************************************************************/

/***************************************************************************
*   ROM 2.4 is copyright 1993-1998 Russ Taylor             *
*   ROM has been brought to you by the ROM consortium          *
*       Russ Taylor (rtaylor@hypercube.org)                *
*       Gabrielle Taylor (gtaylor@hypercube.org)               *
*       Brian Moore (zump@rom.org)                     *
*   By using this code, you have agreed to follow the terms of the     *
*   ROM license, in the file Rom24/doc/rom.license             *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "recycle.h"
#include "tables.h"

bool RECORD_TO_REPLAYROOM;

void check_unseen args ( (CHAR_DATA *ch, int type) );
#define UNSEEN_TALK 0
#define UNSEEN_MOVE 1

void check_unseen(CHAR_DATA *ch, int type)
{

    if (IS_NPC(ch))
        return;
    if (!IS_VAMP(ch))
        return;
    if (!is_affected(ch, gsn_unseen))
        return;

    sh_int success, diff;
    success = diff = 0;

    diff = 4;
    switch (type)
    {
        case UNSEEN_TALK: diff = 6;break;
        case UNSEEN_MOVE: diff = 9;break;
    }
    success = godice(get_attribute(ch, WITS) + ch->csabilities[CSABIL_STEALTH], diff);
    success += stealth_int_shadowplay(ch, diff);
    if (success < 1)
    {
        affect_strip(ch, gsn_unseen);
        act("You reveal yourself.", ch, NULL, NULL, TO_CHAR);
        act("$n reveals $mself.", ch, NULL, NULL, TO_NOTVICT);
    }
    return;
}

/* RT code to delete yourself */

void do_delet( CHAR_DATA *ch, char *argument)
{
    send_to_char("You must type the full command to delete yourself.\n\r",ch);
}

void do_delete( CHAR_DATA *ch, char *argument)
{
   char strsave[MAX_INPUT_LENGTH];

   if (IS_NPC(ch))
    return;

   if (ch->pcdata->confirm_delete)
   {
    if (argument[0] != '\0')
    {
        send_to_char("{GDelete{x status removed.\n\r",ch);
        ch->pcdata->confirm_delete = FALSE;
        return;
    }
    else
    {
            sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
        wiznet("$N turns $Mself into line noise.",ch,NULL,0,0,0);
        stop_fighting(ch,TRUE);
        do_function(ch, &do_quit, "");
        unlink(strsave);
        sprintf( strsave, "%s%s", BK_PLAYER_DIR, capitalize( ch->name ) );
        unlink(strsave);
        return;
    }
    }

    if (argument[0] != '\0')
    {
    send_to_char("Just type {rdelete{x. No argument.\n\r",ch);
    return;
    }

    send_to_char("Type {rdelete{x again to confirm this command.\n\r",ch);
    send_to_char("{RWARNING!{x: this command is {Birreversible{x.\n\r",ch);
    send_to_char("Typing delete with an argument will undo delete status.\n\r",ch);
    ch->pcdata->confirm_delete = TRUE;
    wiznet("$N is contemplating deletion.",ch,NULL,0,0,get_trust(ch));
}


/* RT code to display channel status */

void do_channels(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];

    /* lists all channels and their status */
    printf_to_char(ch, "%-18s  %-4s\n\r", "{CChannel{x", "{CStatus{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{w-------{x", "{w------{x");

    if (IS_IMMORTAL(ch))
        printf_to_char(ch, "%-18s  %-4s\n\r", "{wImm Channel{x", (!IS_SET(ch->comm, COMM_NOWIZ)) ? "{GON{x" : "{ROFF{x");

    if (ch->level >= MAX_LEVEL)
        printf_to_char(ch, "%-18s  %-4s\n\r", "{wImp Channel{x", (!IS_SET(ch->comm, COMM_NOIMP)) ? "{GON{x" : "{ROFF{x");

    if (IS_ADMIN(ch))
        printf_to_char(ch, "%-18s  %-4s\n\r", "{wAdmin Channel{x", (!IS_SET(ch->comm, COMM_NOADMIN)) ? "{GON{x" : "{ROFF{x");

    printf_to_char(ch, "%-18s  %-4s\n\r", "{wAnnounce{x",   (!IS_SET(ch->comm, COMM_NOANNOUNCE)) ?  "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wOOC{x",        (!IS_SET(ch->comm, COMM_NOOOC)) ?       "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wChat{x",       (!IS_SET(ch->comm, COMM_NOCHAT)) ?      "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wQ/A{x",        (!IS_SET(ch->comm, COMM_NOQUESTION)) ?  "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wMusic{x",      (!IS_SET(ch->comm, COMM_NOMUSIC)) ?     "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wGrats{x",      (!IS_SET(ch->comm, COMM_NOGRATS)) ?     "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wAuction{x",    (!IS_SET(ch->comm, COMM_NOAUCTION)) ?   "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wIC{x",         (!IS_SET(ch->comm, COMM_NOIC)) ?        "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wShouts{x",     (!IS_SET(ch->comm, COMM_SHOUTSOFF)) ?   "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wClan{x",       (!IS_SET(ch->comm, COMM_NOCLAN)) ?      "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wTells{x",      (!IS_SET(ch->comm, COMM_DEAF)) ?        "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wQuiet Mode{x", (IS_SET(ch->comm, COMM_QUIET)) ?        "{GON{x" : "{ROFF{x");
    printf_to_char(ch, "%-18s  %-4s\n\r", "{wTelnet GA{x",  (!IS_SET(ch->comm, COMM_TELNET_GA)) ?   "{GON{x" : "{ROFF{x");



    if (IS_SET(ch->comm, COMM_AFK))
    {
        send_to_char("{wYou are {RAFK{x.\n\r", ch);
    }

    if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
    {
        send_to_char("{GYou are immune to snooping.{x\n\r", ch);
    }
    if (IS_SET(ch->comm,COMM_AUTOOOC))
    send_to_char("{wYou automatically turn OOC on at login.{x\n\r", ch);

    if (ch->lines != PAGELEN)
    {
    if (ch->lines)
    {
        sprintf(buf,"{wYou display {c%d{w lines of scroll.{x\n\r",ch->lines+2);
        send_to_char(buf,ch);
    }
    else
        send_to_char("{wScroll buffering is {roff{x.\n\r",ch);
    }

    if (ch->prompt != NULL)
    {
    sprintf(buf,"{wYour current prompt is: %s{x\n\r",ch->prompt);
    send_to_char(buf,ch);
    }

    if (IS_SET(ch->comm,COMM_NOSHOUT))
      send_to_char("{rYou cannot shout.{x\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOTELL))
      send_to_char("{rYou cannot use tell.{x\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
     send_to_char("{rYou cannot use channels.{x\n\r",ch);

    if (IS_SET(ch->comm,COMM_NOEMOTE))
      send_to_char("{rYou cannot show emotions.{x\n\r",ch);

}

// Turn Telnet_Ga On/off
void do_telnetgoahead( CHAR_DATA *ch, char *argument)
{

   if (IS_SET(ch->comm,COMM_TELNET_GA))
   {
     send_to_char("The MUD will no longer send Go Ahead with output.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_TELNET_GA);
   }
   else
   {
     send_to_char("The MUD will now send a Telnet Go Ahead with output.\n\r",ch);
     SET_BIT(ch->comm,COMM_TELNET_GA);
   }
}

//Matthew - Autoooc turns on ooc when you login.
void do_nocondition( CHAR_DATA *ch, char *argument)
{

   if (!IS_SET(ch->act2,PLR2_NOCONDITION))
   {
     send_to_char("You will no longer see condition messages.\n\r",ch);
     SET_BIT(ch->act2,PLR2_NOCONDITION);
   }
   else
   {
     send_to_char("From now on, you will see condition messages on ticks.\n\r",ch);
     REMOVE_BIT(ch->act2,PLR2_NOCONDITION);
   }
}

//Matthew - Autoooc turns on ooc when you login.
void do_autoooc( CHAR_DATA *ch, char *argument)
{

   if (IS_SET(ch->comm,COMM_AUTOOOC))
   {
     send_to_char("You will no longer automatically turn OOC on.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_AUTOOOC);
   }
   else
   {
     send_to_char("From now on, you will automatically turn OOC on at login.\n\r",ch);
     SET_BIT(ch->comm,COMM_AUTOOOC);
   }
}

/* RT deaf blocks out all shouts */

void do_deaf( CHAR_DATA *ch, char *argument)
{

   if (IS_SET(ch->comm,COMM_DEAF))
   {
     send_to_char("You can now hear tells again.\n\r",ch);
     REMOVE_BIT(ch->comm,COMM_DEAF);
   }
   else
   {
     send_to_char("From now on, you won't hear tells.\n\r",ch);
     SET_BIT(ch->comm,COMM_DEAF);
   }
}

/* RT quiet blocks out all communication */

void do_quiet ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_QUIET))
    {
      send_to_char("Quiet mode removed.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_QUIET);
    }
   else
   {
     send_to_char("From now on, you will only hear says and emotes.\n\r",ch);
     SET_BIT(ch->comm,COMM_QUIET);
   }
}

/* afk command */

void do_afk ( CHAR_DATA *ch, char * argument)
{
    if (IS_SET(ch->comm,COMM_AFK))
    {
      send_to_char("AFK mode removed. Type 'replay' to see tells.\n\r",ch);
      REMOVE_BIT(ch->comm,COMM_AFK);
    }
   else
   {
     send_to_char("You are now in AFK mode.\n\r",ch);
     SET_BIT(ch->comm,COMM_AFK);
   }
}

/* RT auction rewritten in ROM style */
void do_auction( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOAUCTION))
      {
    send_to_char("Auction channel is now ON.\n\r",ch);
    REMOVE_BIT(ch->comm,COMM_NOAUCTION);
      }
      else
      {
    send_to_char("Auction channel is now OFF.\n\r",ch);
    SET_BIT(ch->comm,COMM_NOAUCTION);
      }
    }
    else  /* auction message sent, turn auction on if it is off */
    {
    if (IS_SET(ch->comm,COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r",ch);
      return;
    }

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOAUCTION);
    }

    sprintf( buf, "You auction '%s'\n\r", argument );
    channel_to_char( buf, ch );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    CHAR_DATA *victim;

    victim = d->original ? d->original : d->character;

    if ( d->connected == CON_PLAYING &&
         d->character != ch &&
         !IS_SET(victim->comm,COMM_NOAUCTION) &&
         !IS_SET(victim->comm,COMM_QUIET) )
    {
        act_new("{y$n auctions '$t'{x",
            ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
    }
    }
}

/* RT chat replaced with ROM gossip */
void do_ictalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOIC))
      {
        send_to_char("IC channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOIC);
      }
      else
      {
        send_to_char("IC channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOIC);
      }
    }
    else  /* gossip message sent, turn gossip on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;

        }

      REMOVE_BIT(ch->comm,COMM_NOIC);

      sprintf( buf, "{xYou IC '{C%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "{x$n ICs '{C$t{x'",
           ch,argument, d->character, TO_VICT,POS_RESTING, TRUE );
        }
      }
    }
}

void do_grats( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOGRATS))
      {
        send_to_char("Grats channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOGRATS);
      }
      else
      {
        send_to_char("Grats channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOGRATS);
      }
    }
    else  /* grats message sent, turn grats on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;

        }

      REMOVE_BIT(ch->comm,COMM_NOGRATS);

      sprintf( buf, "You grats '%s'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOGRATS) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "$n grats '$t'",
                   ch,argument, d->character, TO_VICT,POS_DEAD, TRUE );
        }
      }
    }
}

void do_ooc( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOOOC))
      {
        send_to_char("OOC channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOOOC);
      }
      else
      {
        send_to_char("OOC channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOOOC);
      }
    }
   else  /* OOC message sent, turn OOC on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }
/*
    if(ch->move <= 0)
    {
        send_to_char("You are too tired to use the OOC channel.\n\r",ch);
        return;
    }
*/
        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;

        }

      REMOVE_BIT(ch->comm,COMM_NOOOC);
    if (IS_AFFECTED2(ch, AFF2_UMBRA))
        sprintf( buf, "{xYou OOC ({mUmbra{x) '{Y%s{x'\n\r", argument );
    else
      sprintf( buf, "{xYou OOC '{Y%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOOOC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
          act_new( "{x$n OOCs '{Y$t{x'",
                   ch,argument, d->character, TO_VICT,POS_DEAD, TRUE );
        }
      }
    }
}
//3/28/12 Matthew's 'chat' channel.

void do_chat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCHAT))
      {
        send_to_char("{cChat{x channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCHAT);
      }
      else
      {
        send_to_char("{cChat{x channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCHAT);
      }
    }
   else  /* chat message sent, turn chat on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;

        }

      REMOVE_BIT(ch->comm,COMM_NOCHAT);

      sprintf( buf, "{xYou Chat '{c%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOCHAT) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
        act_new( "{x$n Chats '{c$t{x'", ch, argument, d->character, TO_VICT, POS_RESTING, TRUE );

        }
      }
    }
}

/* RT question channel */
void do_question( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* question sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
    }

        REMOVE_BIT(ch->comm,COMM_NOQUESTION);

      sprintf( buf, "{xYou question '{G%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
      act_new("{x$n questions '{G$t{x'",
          ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
      }
    }
}

/* RT answer channel - uses same line as questions */
void do_answer( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOQUESTION))
      {
        send_to_char("Q/A channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOQUESTION);
      }
      else
      {
        send_to_char("Q/A channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOQUESTION);
      }
    }
    else  /* answer sent, turn Q/A on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
    }

        REMOVE_BIT(ch->comm,COMM_NOQUESTION);

      sprintf( buf, "{xYou answer '{G%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOQUESTION) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
      act_new("$n answers '{G$t{x'",
          ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
      }
    }
}

/* RT music channel */
void do_music( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOMUSIC))
      {
        send_to_char("Music channel is now ON.\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOMUSIC);
      }
      else
      {
        send_to_char("Music channel is now OFF.\n\r",ch);
        SET_BIT(ch->comm,COMM_NOMUSIC);
      }
    }
    else  /* music sent, turn music on if it isn't already */
    {
        if (IS_SET(ch->comm,COMM_QUIET))
        {
          send_to_char("You must turn off quiet mode first.\n\r",ch);
          return;
        }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
          send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
    }

        REMOVE_BIT(ch->comm,COMM_NOMUSIC);

      sprintf( buf, "{xYou music: '{M%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      sprintf( buf, "{x$n musics: '{M%s{x'", argument );
      for ( d = descriptor_list; d != NULL; d = d->next )
      {
        CHAR_DATA *victim;

        victim = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
             !IS_SET(victim->comm,COMM_NOMUSIC) &&
             !IS_SET(victim->comm,COMM_QUIET) )
        {
        act_new("{x$n musics: '{M$t{x'",
            ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
      }
    }
}

/* clan channels */
void do_clantalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if(IS_NPC(ch)) return;
    if (!is_clan(ch))
    {
    send_to_char("You aren't in a clan.\n\r",ch);
    return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Clan channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Clan channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "You tell your clan '{Y%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      sprintf( buf, "$n tells the clan '{Y%s{x'", argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
         is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
         !IS_SET(d->character->comm,COMM_QUIET) )
        {
            act_new("$n tells the clan '{Y$t{x'",ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
    }

    return;
}

void do_traditiontalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if(IS_NPC(ch)) return;
    if (!is_clan(ch))
    {
    send_to_char("You aren't in a Tradition.\n\r",ch);
    return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Tradition channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Tradition channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "You tell your Tradition '{Y%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      sprintf( buf, "%s tells the Tradition '{Y%s{x'", ch->name, argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
         is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
         !IS_SET(d->character->comm,COMM_QUIET) )
        {
            sprintf(buf,"%s tells the Tradition '{Y$t{x'",ch->name);
            act_new(buf,ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
    }

    return;
}
void do_tribetalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if(IS_NPC(ch)) return;
    if (!is_clan(ch))
    {
    send_to_char("You aren't in a Tribe.\n\r",ch);
    return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOCLAN))
      {
        send_to_char("Tribe channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOCLAN);
      }
      else
      {
        send_to_char("Tribe channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOCLAN);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOCLAN);

      sprintf( buf, "You tell your Tribe '{Y%s{x'\n\r", argument );
      channel_to_char( buf, ch );
      sprintf( buf, "%s tells the Tribe '{Y%s{x'", ch->name, argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
         is_same_clan(ch,d->character) &&
             !IS_SET(d->character->comm,COMM_NOCLAN) &&
         !IS_SET(d->character->comm,COMM_QUIET) )
        {
            sprintf(buf,"%s tells the Tribe '{Y$t{x'",ch->name);
            act_new(buf,ch,argument,d->character,TO_VICT,POS_RESTING, TRUE);
        }
    }

    return;
}


void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if (!IS_IMMORTAL(ch) && !IS_SET(ch->act, PLR_IMMTALK))
    {
        send_to_char("Huh?", ch);
        return;
    }

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOWIZ))
      {
    send_to_char("Immortal channel is now ON\n\r",ch);
    REMOVE_BIT(ch->comm,COMM_NOWIZ);
      }
      else
      {
    send_to_char("Immortal channel is now OFF\n\r",ch);
    SET_BIT(ch->comm,COMM_NOWIZ);
      }
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOWIZ);

    sprintf( buf, "{D[{w$n{D]: {G%s{x", argument );
    act_new("{D[{w$n{D]: {G$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD, TRUE);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d->connected == CON_PLAYING &&
         (IS_IMMORTAL(d->character) || IS_SET(d->character->act, PLR_IMMTALK)) &&
             !IS_SET(d->character->comm,COMM_NOWIZ) )
    {
        act_new("{D[{w$n{D]: {G$t{x",ch,argument,d->character,TO_VICT,POS_DEAD, TRUE);
    }
    }

    return;
}

void do_admintalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    d = ch->desc;

    if (!IS_ADMIN(ch))
             {
                 send_to_char("Huh?\n\r", ch);
                 return;
             }

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOADMIN))
      {
    send_to_char("Admin channel is now ON\n\r",ch);
    REMOVE_BIT(ch->comm,COMM_NOADMIN);
      }
      else
      {
    send_to_char("Admin channel is now OFF\n\r",ch);
    SET_BIT(ch->comm,COMM_NOADMIN);
      }
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOADMIN);

    act_new("{g<{w$n{g>: {c$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD, TRUE);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d->connected == CON_PLAYING && IS_ADMIN(d->character) )
    {
        act_new("{g<{w$n{g>: {c$t{x",ch,argument,d->character,TO_VICT,POS_DEAD, TRUE);
    }
    }

    return;
}



void do_imptalk( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOIMP))
      {
    send_to_char("Imp channel is now ON\n\r",ch);
    REMOVE_BIT(ch->comm,COMM_NOIMP);
      }
      else
      {
    send_to_char("Imp channel is now OFF\n\r",ch);
    SET_BIT(ch->comm,COMM_NOIMP);
      }
      return;
    }

    REMOVE_BIT(ch->comm,COMM_NOIMP);
    act_new("{D*{m$n{D*: {D$t{x",ch,argument,NULL,TO_CHAR,POS_DEAD, TRUE);
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d->connected == CON_PLAYING &&
         (get_trust(d->character) >= MAX_LEVEL) &&
             !IS_SET(d->character->comm,COMM_NOIMP) )
    {
        act_new("{D*{m$n{D*: {D$t{x",ch,argument,d->character,TO_VICT,POS_DEAD, TRUE);
    }
    }

    return;
}


void do_say( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
    send_to_char( "Say what?\n\r", ch );
    return;
    }
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );

    check_unseen(ch, UNSEEN_TALK );
    RECORD_TO_REPLAYROOM=TRUE;
    act( "{x$n says '{w$T{x'", ch, NULL, argument, TO_ROOM );
    act( "{xYou say '{w$T{x'", ch, NULL, argument, TO_CHAR );
    RECORD_TO_REPLAYROOM=FALSE;
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;
    if ( !IS_NPC(ch) )
    {
        CHAR_DATA *mob, *mob_next;
        OBJ_DATA *obj, *obj_next;
        for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
        {
            mob_next = mob->next_in_room;
            if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC))
            mob->pcdata->room_last_pose = 0;

            if ( IS_NPC(mob) && SAME_UMBRA(ch,mob)  && HAS_TRIGGER_MOB( mob, TRIG_SPEECH )
            &&   mob->position == mob->pIndexData->default_pos )
            p_act_trigger( argument, mob, NULL, NULL, ch, NULL, NULL, TRIG_SPEECH );

            for ( obj = mob->carrying; obj; obj = obj_next )
            {
                obj_next = obj->next_content;
                if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) && SAME_UMBRA_OBJ(ch, obj))
                    p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
            }
        }
        for ( obj = ch->in_room->contents; obj; obj = obj_next )
        {
            obj_next = obj->next_content;
            if ( HAS_TRIGGER_OBJ( obj, TRIG_SPEECH ) && SAME_UMBRA_OBJ(ch, obj))
            p_act_trigger( argument, NULL, obj, NULL, ch, NULL, NULL, TRIG_SPEECH );
        }

        if ( HAS_TRIGGER_ROOM( ch->in_room, TRIG_SPEECH ) )
            p_act_trigger( argument, NULL, NULL, ch->in_room, ch, NULL, NULL, TRIG_SPEECH );
    }
    return;
}

void do_osay( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
    send_to_char( "Say what OOCly?\n\r", ch );
    return;
    }

    check_unseen(ch, UNSEEN_TALK );
    RECORD_TO_REPLAYROOM = TRUE;
    act_new( "{x$n says OOCly '{C$T{x'", ch, NULL, argument, TO_ROOM, POS_DEAD, FALSE );
    act_new( "{xYou say OOCly '{C$T{x'", ch, NULL, argument, TO_CHAR, POS_DEAD, FALSE );
    RECORD_TO_REPLAYROOM = FALSE;
    return;
}



void do_shout( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }

    if (argument[0] == '\0' )
    {
        if (IS_SET(ch->comm,COMM_SHOUTSOFF))
        {
            send_to_char("You can hear shouts again.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);
        }
        else
        {
            send_to_char("You will no longer hear shouts.\n\r",ch);
            SET_BIT(ch->comm,COMM_SHOUTSOFF);
        }
        return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't shout.\n\r", ch );
        return;
    }

    REMOVE_BIT(ch->comm,COMM_SHOUTSOFF);

    WAIT_STATE( ch, 12 );

    act_new( "{xYou shout '{R$T{x'", ch, NULL, argument, TO_CHAR, POS_RESTING, TRUE );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    CHAR_DATA *victim;

    victim = d->original ? d->original : d->character;

    if ( d->connected == CON_PLAYING &&
         d->character != ch &&
         !IS_SET(victim->comm, COMM_SHOUTSOFF) &&
         !IS_SET(victim->comm, COMM_QUIET) )
    {
        act_new ("{x$n shouts '{R$t{x'",ch,argument,d->character,TO_VICT, POS_RESTING, TRUE);
    }
    }

    return;
}

void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH], buf[MSL];
    CHAR_DATA *victim, *dch;
    DESCRIPTOR_DATA *d;

    argument = one_argument(argument, arg);

    if (arg[0] == '\0' || argument[0] == '\0')
    {
        sendch("Whisper whom what?\n\r", ch);
        return;
    }

    if ((victim = get_char_room(ch, NULL, arg)) == NULL) //Get character from ch->in_room.
    {
        sendch("How do you expect to whisper to someone who isn't here?!\n\r", ch);
        return;
    }

    sprintf(buf,"{xYou whisper to %s '{w%s{x'\n\r", PERS2(victim,ch),argument);
    sendch(buf, ch);
    record_replay_event(ch, buf);//Record to replay buffer.

    sprintf(buf, "{x%s whispers to you '{w%s{x'\n\r", PERS2(ch, victim), argument);
    sendch(buf, victim);
    record_replay_event(ch, buf);

    for (d = descriptor_list; d != NULL; d = d->next) // Loop through descriptor list.
    {
        dch = d->character; // shorthand pointer so we don't have to type d->character over and over.

        if (d->connected != CON_PLAYING || dch == ch || dch == victim
            || dch->in_room != ch->in_room)
            continue;
            // If d isn't fully online, or dch is ch or victim, or dch isn't in the same room,
            // go on to next char in descriptor_list.
            int resistance = (godice(get_attribute(ch, WITS) + ch->csabilities[CSABIL_SUBTERFUGE], 7));
            int challenge = (godice(get_attribute(dch, WITS) + dch->csabilities[CSABIL_ALERTNESS], 7));
            int listsuccess = challenge - resistance;
        if  (listsuccess > 3)// 1 in 10 chance of overhearing.
        // To add a diceroll you could do if(godice(ch's perception + ch's alertness, diff 9))
        // That would do a perc+alert roll diff 9 and allow the listener to overhear on a success.
        {
            sprintf(buf, "{xYou overhear %s whisper to %s '{w%s{x'", PERS2(ch, dch), PERS2(victim, dch), argument);
            sendch(buf, dch);
            record_replay_event(dch, buf);
            return;
        } //if
            if ((listsuccess == 1) || (listsuccess ==2))
            {
                sprintf(buf, "You notice %s whispering to %s, but can't make out what was said.", PERS2(ch, dch), PERS2(victim, dch));
                sendch(buf,dch);
            }
    }//for

    if (!IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER_MOB(victim,TRIG_SPEECH) &&
        (victim->position > POS_SLEEPING) && (victim->position != POS_FIGHTING))
        p_act_trigger( argument, victim,NULL, NULL, ch, NULL, NULL, TRIG_SPEECH );

}// do_whisper



void do_tell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH],buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm,COMM_DEAF))
    {
    send_to_char( "Your message didn't get through.\n\r", ch );
    return;
    }

    if ( IS_SET(ch->comm, COMM_QUIET) )
    {
    send_to_char( "You must turn off quiet mode first.\n\r", ch);
    return;
    }

    if (IS_SET(ch->comm,COMM_DEAF))
    {
    send_to_char("You must turn off deaf mode first.\n\r",ch);
    return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
    send_to_char( "Tell whom what?\n\r", ch );
    return;
    }

    /*
     * Can tell to PC's anywhere, but NPC's only in same room.
     * -- Furey
     */
    if ( ( victim = get_char_world( ch, arg ) ) == NULL
    || ( IS_NPC(victim) && victim->in_room != ch->in_room ) )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
    act("$N seems to have misplaced $S link...try again later.",
        ch,NULL,victim,TO_CHAR);
        sprintf(buf,"{x%s tells you '{Y%s{x'\n\r",PERS(ch,victim, TRUE),argument);
        buf[0] = UPPER(buf[0]);
        record_replay_event(victim, buf);
    return;
    }

    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    && !IS_IMMORTAL(ch))
    {
    act( "$E is not receiving tells.", ch, 0, victim, TO_CHAR );
    return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
    if (IS_NPC(victim))
    {
        act("$E is AFK, and not receiving tells.",ch,NULL,victim,TO_CHAR);
        return;
    }

    act("$E is AFK, but your tell will go through when $E returns.",
        ch,NULL,victim,TO_CHAR);
    sprintf(buf,"{x%s tells you '{Y%s{x'\n\r",PERS(ch,victim, TRUE),argument);
    buf[0] = UPPER(buf[0]);
    record_replay_event(victim, buf);
    sendch(buf, victim);
    return;
    }

    sprintf(buf2,"{xYou tell %s '{Y%s{x'\n\r", PERS(victim,ch, TRUE),argument);
    send_to_char(buf2,ch);
    record_replay_event(ch, buf2);
    sprintf(buf2,"{x%s tells you '{Y%s{x'\n\r", PERS(ch,victim, TRUE),argument);
    send_to_char(buf2,victim);
    record_replay_event(victim, buf2);
    victim->reply   = ch;

    if ( !IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER_MOB(victim,TRIG_SPEECH) )
        p_act_trigger( argument, victim,NULL, NULL, ch, NULL, NULL, TRIG_SPEECH );


    return;
}



void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];

    if ( IS_SET(ch->comm, COMM_NOTELL) )
    {
    send_to_char( "Your message didn't get through.\n\r", ch );
    return;
    }

    if ( ( victim = ch->reply ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( victim->desc == NULL && !IS_NPC(victim))
    {
        act("$N seems to have misplaced $S link...try again later.",
            ch,NULL,victim,TO_CHAR);
        sprintf(buf,"%s tells you '{Y%s{x'\n\r",PERS(ch,victim, TRUE),argument);
        buf[0] = UPPER(buf[0]);
        record_replay_event(victim, buf);
        return;
    }



    if ((IS_SET(victim->comm,COMM_QUIET) || IS_SET(victim->comm,COMM_DEAF))
    &&  !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    {
        act_new( "$E is not receiving tells.", ch, 0, victim, TO_CHAR,POS_DEAD, TRUE);
        return;
    }

    if (IS_SET(victim->comm,COMM_AFK))
    {
        if (IS_NPC(victim))
        {
            act_new("$E is AFK, and not receiving tells.",
        ch,NULL,victim,TO_CHAR,POS_DEAD, TRUE);
            return;
        }

        act_new("$E is AFK, but your tell will go through when $E returns.",
            ch,NULL,victim,TO_CHAR,POS_DEAD, TRUE);
        sprintf(buf,"%s tells you  '{Y%s{x'\n\r",PERS(ch,victim, TRUE),argument);
    buf[0] = UPPER(buf[0]);
        record_replay_event(victim, buf);
        sendch(buf, victim);
        return;
    }

    sprintf(buf2,"{xYou tell %s '{Y%s{x'\n\r", PERS(victim,ch, TRUE),argument);
    send_to_char(buf2,ch);
    record_replay_event(ch, buf2);
    sprintf(buf2,"{x%s tells you '{Y%s{x'\n\r", PERS(ch,victim, TRUE),argument);
    send_to_char(buf2,victim);
    record_replay_event(victim, buf2);

    victim->reply   = ch;

    return;
}



void do_yell( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if (IS_SET(ch->comm,COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
      return;
    }

    if ( IS_SET(ch->comm, COMM_NOSHOUT) )
    {
        send_to_char( "You can't yell.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
    send_to_char( "Yell what?\n\r", ch );
    return;
    }
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );

    check_unseen(ch, UNSEEN_TALK );
    RECORD_TO_REPLAYROOM = TRUE;
    act("{xYou yell '{r$t{x'",ch,argument,NULL,TO_CHAR);
    RECORD_TO_REPLAYROOM = FALSE;
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
    if ( d->connected == CON_PLAYING
    &&   d->character != ch
    &&   d->character->in_room != NULL
    &&   d->character->in_room->area == ch->in_room->area
        &&   !IS_SET(d->character->comm,COMM_QUIET) )
    {
        RECORD_TO_REPLAYROOM = TRUE;
        act("{x$n yells '{r$t{x'",ch,argument,d->character,TO_VICT);
        RECORD_TO_REPLAYROOM = FALSE;
    }
    }

    return;
}


void do_emote( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );

    check_unseen(ch, UNSEEN_MOVE );
    MOBtrigger = FALSE;
    RECORD_TO_REPLAYROOM=TRUE;
    act( "$n $T", ch, NULL, argument, TO_ROOM );
    act( "$n $T", ch, NULL, argument, TO_CHAR );
    RECORD_TO_REPLAYROOM=FALSE;
    MOBtrigger = TRUE;
    CHAR_DATA *mob, *mob_next;
    for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC) && mob != ch)
        mob->pcdata->room_last_pose = 0;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;

    return;
}


void do_pmote( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *vch;
    char *letter,*name;
    char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
    int matches = 0;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }
    if(is_affected(ch, gsn_unseen) && ch->pcdata->discipline[OBFUSCATE] < 2)
    affect_strip( ch, gsn_unseen );    act( "$n $t", ch, argument, NULL, TO_CHAR );

    for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
    {
    if (vch->desc == NULL || vch == ch)
        continue;

    if ((letter = strstr(argument,vch->name)) == NULL)
    {
        MOBtrigger = FALSE;
        RECORD_TO_REPLAYROOM=TRUE;
        act("$N $t",vch,argument,ch,TO_CHAR);
        RECORD_TO_REPLAYROOM=FALSE;
        MOBtrigger = TRUE;
        continue;
    }

    strcpy(temp,argument);
    temp[strlen(argument) - strlen(letter)] = '\0';
    last[0] = '\0';
    name = vch->name;

    for (; *letter != '\0'; letter++)
    {
        if (*letter == '\'' && matches == strlen(vch->name))
        {
        strcat(temp,"r");
        continue;
        }

        if (*letter == 's' && matches == strlen(vch->name))
        {
        matches = 0;
        continue;
        }

        if (matches == strlen(vch->name))
        {
        matches = 0;
        }

        if (*letter == *name)
        {
        matches++;
        name++;
        if (matches == strlen(vch->name))
        {
            strcat(temp,"you");
            last[0] = '\0';
            name = vch->name;
            continue;
        }
        strncat(last,letter,1);
        continue;
        }

        matches = 0;
        strcat(temp,last);
        strncat(temp,letter,1);
        last[0] = '\0';
        name = vch->name;
    }
    MOBtrigger = FALSE;
    RECORD_TO_REPLAYROOM=TRUE;
    act("$N $t",vch,temp,ch,TO_CHAR);
    RECORD_TO_REPLAYROOM=FALSE;
    MOBtrigger = TRUE;
    CHAR_DATA *mob, *mob_next;
    for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC) && mob != ch)
        mob->pcdata->room_last_pose = 0;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;

    }

    return;
}



void do_pose( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Pose what?\n\r", ch );

    return;
    }

    for ( d = descriptor_list; d; d = d->next )
    {
    if ( d->connected == CON_PLAYING
    &&   d->character->in_room == ch->in_room )
    {
            cprintf(d->character, "{g[{DPose - %s{g] :: {x%s\n\r", ch->name, argument);

    }
    }
        CHAR_DATA *mob, *mob_next;
    for ( mob = ch->in_room->people; mob != NULL; mob = mob_next )
    {
        mob_next = mob->next_in_room;
        if (!IS_NPC(mob) && IS_SET(mob->act, PLR_IC) && mob != ch)
        mob->pcdata->room_last_pose = 0;
    }
    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_IC))
        ch->pcdata->last_pose = 0;

    return;
}



void do_dictionary( CHAR_DATA *ch, char *argument )
{
    char word [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf [MAX_INPUT_LENGTH];
    char buf2 [MAX_INPUT_LENGTH];

    argument = one_argument( argument, word );
    argument = one_argument( argument, arg2 );

    if(word[0] == '\0')
    {
        if(IS_IMMORTAL(ch))
        {
            send_to_char( "Syntax is: dictionary <word>\n\rTo add a word to the dictionary: dictionary <word> add\n\r", ch );
            return;
        }
        else
        {
            send_to_char( "Search for which word?\n\r", ch );
            return;
        }
    }

    send_to_char( "Searching...\n\r", ch );

    sprintf(buf,"grep -i %s %s",word, DICT_FILE);
    if(0==system(buf))
    {
        sprintf( buf2, "The word '%s' is in the Dictionary.\n\r", word );
        send_to_char(buf2,ch);
        return;
    }
    else
    {
        if(!str_cmp(arg2, "add") && IS_IMMORTAL(ch))
        {
            append_file( ch, DICT_FILE, word, FALSE );
            sprintf( buf2, "The word '%s' has been added to the Dictionary.\n\r", word );
            send_to_char(buf2,ch);
            return;
        }
        send_to_char( "Word is not in the Dictionary.\n\r", ch );
        return;
    }
}

void do_bug( CHAR_DATA *ch, char *argument )
{
    append_file( ch, BUG_FILE, argument, TRUE );
    send_to_char( "Bug logged.\n\r", ch );
    return;
}

void do_typo( CHAR_DATA *ch, char *argument )
{
    append_file( ch, TYPO_FILE, argument, TRUE );
    send_to_char( "Typo logged.\n\r", ch );
    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}


void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}



void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d,*d_next;
    ROOM_INDEX_DATA *location;
    int id;
    int quote;

    if ( IS_NPC(ch) )
    return;

	if (is_affected(ch, gsn_astralprojection))
	{
        send_to_char("You need to return to the material plane first, or you will be lost forever.\n\r", ch);
		return;
	}

    if ( ch->position == POS_FIGHTING )
    {
    send_to_char( "No way! You are fighting.\n\r", ch );
    return;
    }

    if ( ch->position  < POS_TORPOR  )
    {
    send_to_char( "You're not DEAD yet.\n\r", ch );
    return;
    }
    if(IS_SET(ch->in_room->room_flags, ROOM_NO_QUIT))
    {
        if(ch->desc != NULL)
        {
            send_to_char("A mystical force prevents you from exiting the realm.\n\r",ch);
            return;
        }
        if ( ( location = get_room_index( ROOM_VNUM_TEMPLE ) ) == NULL )
            return;
        stop_fighting( ch, TRUE );
        REMOVE_BIT(ch->act,PLR_ARENA);
        REMOVE_BIT(ch->act,PLR_SPEC);
        char_from_room( ch );
        char_to_room( ch, location );
    }

   REMOVE_BIT(ch->act,PLR_ARENA);
       affect_strip(ch,gsn_meditation);
    if(is_affected( ch, gsn_chant ))
    {
        send_to_char( "You stop your chanting.\n\r", ch );
        act( "$n stops chanting to $s God.",  ch, NULL, NULL, TO_ROOM );
        affect_strip(ch,gsn_chant);
    }
    if (is_affected( ch, gsn_vampire_regen))
    {
    send_to_char( "You stop regenerating.\n\r", ch);
    act( "$n stops regenerating.", ch, NULL, NULL, TO_ROOM);
    affect_strip(ch, gsn_vampire_regen);
    }
    REMOVE_BIT(ch->comm, COMM_DEAF);
    REMOVE_BIT(ch->comm, COMM_QUIET);
    REMOVE_BIT(ch->act, PLR_IC);
    quote = number_range(1,26);
    if(quote == 1)
        send_to_char("Sex is a good cure for headaches, repeat as needed\n\r - Hatchet, Creator of MERC\n\r",ch);
    if(quote == 2)
        send_to_char("Think for yourself, or you're better off dead. Either way, I'm satisfied\n\r - Unknown Brujah\n\r",ch);
    if(quote == 3)
        send_to_char("Shadows? Hah! I wield Darkness itself, not mere shadows! Tell me - could a shadow do this?\n\r - Lasombra Prince\n\r",ch);
    if(quote == 4)
        send_to_char("Caine, son of Adam, you are cursed to ever walk the night. You will ever repeat your crimes throughout time.\n\r - God speaking to Caine, the first Vampire.\n\r",ch);
    if(quote == 5)
        send_to_char("Help me Child, help me wash away my sorrows with the sweetness of your blood.\n\r - Unknown Kindred\n\r",ch);
    if(quote == 6)
        send_to_char("I have walked the paths; the shadowed roads that led to terror's breast.\n\r I have plumbed the depths of Hatred's womb and scaled Destruction's crest.\n\r\n\rFor every secret left unveiled, for every power learned,\n\r I'd sell the remnants of my soul, regardless how it burned.\n\rAnd still I sought a higher wisdom few could have attained.\n\rThough I found it, it would leave me - broken, damned and drained.\n\r\n\rFor now I find this power gained is more unto a curse.\n\rMy spirit burns with every spell and each irreverent verse.\n\rDespite this strength and knowledge earned, I have paid a heavy toll,\n\rNever should've traded power for my own immortal soul.\n\r - Unknown Kindred\n\r",ch);
    if(quote == 7)
        send_to_char("You've impressed me.  And I'm not often impressed.  Oh, hey look!  A rabbit!\n\r - Anonymous Malkavian\n\r",ch);
    if(quote == 8)
        send_to_char("My childe, you dreamt of immortality?  Come, let me show you what nightmares forever holds.\n\r - Lasombra Elder\n\r",ch);
    if(quote == 9)
        send_to_char("Soaring through the skies on tattered wings I look down upon\n\ryou and wonder. Wonder if you would call me a monster, or\n\rwelcome me into your heart. Wonder if my face would bring\n\rforth fear or sorrow. I wonder if my deeds would sicken you,\n\ror cause pity.\n\r\n\rI look down and wonder if I could be near you without becoming\n\rthat monster you fear. Becoming that face of horror, commiting\n\rthose sickening deeds..\n\r\n\rI wonder if.. No. I suppose not.\n\r",ch);
	if(quote == 10)
        send_to_char("There is something at work in my soul, which I do not understand.\n\r - Mary Shelley, Frankenstein\n\r",ch);
	if(quote == 11)
        send_to_char("Some ghosts are so quiet you would hardly know they were there.\n\r - Bernie Mcgill, The Butterfly Cabinet\n\r",ch);
	if(quote == 12)
        send_to_char("I have lost the faculty of enjoying their destruction, and I am too idle to destroy for nothing.\n\r - Emily Bronte, Wuthering Heights\n\r",ch);
	if(quote == 13)
        send_to_char("I run blindly through the madhouse ... And I cannot even pray ... For I have no God.\n\r - Batman: Arkham Asylum - A Serious House on Serious Earth\n\r",ch);
	if(quote == 14)
        send_to_char("Our very existence refuses your laws and your science, your religions and your philosophies.\n\r - B.E. Scully\n\r",ch);
	if(quote == 15)
        send_to_char("From even the greatest of horrors irony is seldom absent.\n\r - H.P. Lovecraft, Tales of H.P. Lovecraft\n\r",ch);
	if(quote == 16)
        send_to_char("That is not dead which can eternal lie, And with strange aeons even death may die.\n\r - Howard Phillips Lovecraft, The Nameless City\n\r",ch);
	if(quote == 17)
        send_to_char("People should be smart with their consumptions no matter what it is.\n\r - Riley Reid\n\r",ch);
	if(quote == 18)
        send_to_char("A starving child is a frightful sight. A starving vampire, even worse.\n\r - Anne Rice\n\r",ch);
	if(quote == 19)
        send_to_char("None of us really changes over time; we only become more fully what we are.\n\r - Anne Rice, Queen of the Damned\n\r",ch);
	if(quote == 20)
        send_to_char("How do we seem to you? Do you find us beautiful and magical? Our white skin, our fierce eyes? ‘Drink,’ you ask me. Do you have any idea of the things you will become.\n\r - Anne Rice, Interview with The Vampire\n\r",ch);
	if(quote == 21)
        send_to_char("The blood is the life!\n\r - Bram Stoker, Dracula\n\r",ch);
	if(quote == 22)
        send_to_char("I am the monster that breathing men would kill. I am Dracula.\n\r - Bram Stoker, Dracula\n\r",ch);
	if(quote == 23)
        send_to_char("Listen to them, the children of the night. What music they make!\n\r - Bram Stoker, Dracula\n\r",ch);
	if(quote == 24)
        send_to_char("I have never met a vampire personally, but I don’t know what might happen tomorrow.\n\r - Bela Lugosi\n\r",ch);
	if(quote == 25)
        send_to_char("The oldest and strongest emotion of mankind is fear, and the oldest and strongest kind of fear is fear of the unknown.\n\r - H. P. Lovecraft\n\r",ch);
	if(quote == 26)
        send_to_char("Monsters are real, and ghosts are real too. They live inside us, and sometimes, they win.\n\r - Stephen King\n\r",ch);

    act( "$n has left the game.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has quit.", ch->name );
    log_string( log_buf );
     wiznet("$N rejoins the real world.",ch,NULL,WIZ_LOGINS,0,get_trust(ch));
    announce(ch, NULL, WIZ_SITES);
    /*
     * After extract_char the ch is no longer valid!
     */
    save_char_obj( ch );
    id = ch->id;
    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
    close_socket( d );

    /* toast evil cheating bastards */
    for (d = descriptor_list; d != NULL; d = d_next)
    {
    CHAR_DATA *tch;

    d_next = d->next;
    tch = d->original ? d->original : d->character;
    if (tch && tch->id == id)
    {
        extract_char(tch,TRUE);
        close_socket(d);
    }
    }

    return;
}



void do_save( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
    return;
    save_char_obj( ch );
/*    restore_affects( ch ); */
    send_to_char("Saving. Remember that Haven automatically saves.\n\r", ch);
    return;
}



void do_follow( CHAR_DATA *ch, char *argument )
{
/* RT changed to allow unlimited following and follow the NOFOLLOW rules */
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
    send_to_char( "Follow whom?\n\r", ch );
    return;
    }
    if(!IS_NPC(ch) && IS_SET(ch->act,PLR_ARENA))
    {
    send_to_char("No following in the Arena!\n\r",ch);
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL )
    {
    act( "But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR );
    return;
    }

    if ( victim == ch )
    {
    if ( ch->master == NULL )
    {
        send_to_char( "You already follow yourself.\n\r", ch );
        return;
    }
    stop_follower(ch);
    return;
    }

    if (!IS_NPC(victim) && IS_SET(victim->act,PLR_NOFOLLOW) && !IS_IMMORTAL(ch))
    {
    act("$N doesn't seem to want any followers.\n\r",
             ch,NULL,victim, TO_CHAR);
        return;
    }
   if ( victim->master != NULL )
    {
        send_to_char( "But they are already following someone else!\n\r", ch );
        return;
    }


    REMOVE_BIT(ch->act,PLR_NOFOLLOW);

    if ( ch->master != NULL )
    stop_follower( ch );

    add_follower( ch, victim );
    return;
}


void do_dismiss( CHAR_DATA *ch, char *argument )
{
    nuke_pets(ch);
    return;
}


void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
    if ( ch->master != NULL )
    {
    bug( "Add_follower: non-null master.", 0 );
    return;
    }

    ch->master        = master;
    ch->leader        = NULL;

    if ( can_see( master, ch ) )
    act( "$n now follows you.", ch, NULL, master, TO_VICT );

    act( "You now follow $N.",  ch, NULL, master, TO_CHAR );

    return;
}



void stop_follower( CHAR_DATA *ch )
{
    if ( ch->master == NULL )
    {
    bug( "Stop_follower: null master.", 0 );
    return;
    }

    if ( IS_AFFECTED(ch, AFF_CHARM) )
    {
    REMOVE_BIT( ch->affected_by, AFF_CHARM );
    affect_strip( ch, gsn_charm_person );
    }

    if ( can_see( ch->master, ch ) && ch->in_room != NULL)
    {
    act( "$n stops following you.",     ch, NULL, ch->master, TO_VICT    );
        act( "You stop following $N.",      ch, NULL, ch->master, TO_CHAR    );
    }
    if (ch->master->pet == ch)
    ch->master->pet = NULL;

    ch->master = NULL;
    ch->leader = NULL;
    return;
}

/* nukes charmed monsters and pets */
void nuke_pets( CHAR_DATA *ch )
{
    CHAR_DATA *pet;

/*    if(!IS_NPC(ch)) return; */

    if ((pet = ch->pet) != NULL)
    {
        stop_follower(pet);
        if (pet->in_room != NULL)
            act("$N slowly fades away.",ch,NULL,pet,TO_NOTVICT);
        extract_char(pet,TRUE);
    }
    ch->pet = NULL;

    if ( ch->mount && (ch->mount->in_room == ch->in_room || ch->mount->in_room==NULL) )
    {
        pet = ch->mount;
        do_dismount(ch, "");
        if (pet->in_room != NULL)
            act("$N slowly fades away.",ch,NULL,pet,TO_NOTVICT);
        else
            log_string("void nuke_pets: Extracting null pet");
        ch->mount = NULL;
        ch->riding = FALSE;
        extract_char(pet, TRUE);
    }
    else if (ch->mount)
    {
        ch->mount->mount = NULL;
        ch->mount->riding = FALSE;
    }
    ch->mount = NULL;

    return;
}



void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->master != NULL )
    {
        if (ch->master->pet == ch)
            ch->master->pet = NULL;
    stop_follower( ch );
    }

    ch->leader = NULL;

    for ( fch = char_list; fch != NULL; fch = fch->next )
    {
    if ( fch->master == ch )
        stop_follower( fch );
    if ( fch->leader == ch )
        fch->leader = fch;
    }

    return;
}



void do_order( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *och;
    CHAR_DATA *och_next;
    bool found;
    bool fAll;

    argument = one_argument( argument, arg );
    one_argument(argument,arg2);

    if (!str_cmp(arg2,"delete") || !str_prefix(arg2,"mob") || !str_prefix(arg2,"quaff"))
    {
        send_to_char("That will NOT be done.\n\r",ch);
        return;
    }

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
    send_to_char( "Order whom to do what?\n\r", ch );
    return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) )
    {
    send_to_char( "You feel like taking, not giving, orders.\n\r", ch );
    return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
    fAll   = TRUE;
    victim = NULL;
    }
    else
    {
    fAll   = FALSE;

    if (!strcmp(arg, "mount"))
    {
        if (!ch->mount)
        {
            send_to_char("Your don't have a mount.\n\r", ch);
            return;
        }

        if (ch->mount->in_room != ch->in_room)
        {
            send_to_char("Your mount isn't here!\n\r", ch);
            return;
        }
        else
        {
            victim = ch->mount;
        }
    }

    else if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Aye aye, right away!\n\r", ch );
        return;
    }

    if ( victim->mount == ch )
    {
        if ( !mount_success(ch, victim, FALSE) )
        {
            act("$N ignores your orders.", ch, NULL, victim, TO_CHAR);
            return;
        }
        else
        {
        sprintf(buf,"%s orders you to \'%s\'.", ch->name, argument);
        send_to_char(buf, victim);
        interpret( victim, argument );
            return;
        }
    }
    else if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch
    ||  (IS_IMMORTAL(victim) && victim->trust >= ch->trust))
    {
        send_to_char( "Do it yourself!\n\r", ch );
        return;
    }
    }

    found = FALSE;
    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
    och_next = och->next_in_room;

    if ( IS_AFFECTED(och, AFF_CHARM)
    &&   och->master == ch
    && ( fAll || och == victim ) )
    {
        found = TRUE;
        sprintf( buf, "$n orders you to '%s'.", argument );
        act( buf, ch, NULL, och, TO_VICT );
        interpret( och, argument );
    }
    }

    if ( found )
    {
    WAIT_STATE(ch,PULSE_VIOLENCE);
    send_to_char( "Ok.\n\r", ch );
    }
    else
    send_to_char( "You have no followers here.\n\r", ch );
    return;
}



void do_group( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MSL];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *groupie;

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2);


    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_ARENA))
    {
    send_to_char( "No grouping in the arena!\n\r", ch );
    return;
    }

    if ( arg[0] == '\0' )
    {
    CHAR_DATA *gch;
    CHAR_DATA *leader;
    int maxfollower = get_ability(ch, CSABIL_LEADERSHIP) * 2 + 1;
    int followers = 0;

    leader = (ch->leader != NULL) ? ch->leader : ch;
    for ( gch = char_list; gch != NULL; gch = gch->next )
        if ( is_same_group( gch, ch ) && gch != ch )
          followers++;

    send_to_char("  {D _________________________________________________________________________{x \n\r", ch);
    sprintf( buf2, "%s's group:", PERS(leader, ch,TRUE));
    if (ch == leader)
      sprintf(buf, "{D  / {xYou may have %2d more grouped follower%s out of a Maximum of %2d grouped. %s{D/{x \n\r",
        maxfollower-followers, maxfollower-followers == 1 ? "" : "s",
        maxfollower, maxfollower-followers == 1 ? " " : "");
    else
      sprintf(buf, "{D  / {xWhen leading, you may have up to %2d grouped follower%s.%s{D                 /{x \n\r",
        maxfollower, maxfollower == 1 ? "" : "s", maxfollower == 1 ? " " : "");
      send_to_char(buf, ch);
      sprintf( buf, "{D /  {y%-30s{x   ({GGood{x  {YFair  {RCritical{x)               {D/{x \n\r", buf2 );
      send_to_char( buf, ch );
    send_to_char("{D/________________________________________________________________________/{x \n\r", ch);

    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
        if ( is_same_group( gch, ch ) )
        {
        sprintf( buf,
        "[%3d] %-26s HP: %s%3d%{x MN: %s%3d%{x MV: %s%3d%{x  {x%5d xp tnl\n\r",
            gch->level,
            capitalize( PERS(gch, ch, TRUE) ),
            100 * gch->hit / gch->max_hit < 15 ? "{R" : 100 * gch->hit / gch->max_hit < 50 ? "{Y" : "{G",
            100 * gch->hit / gch->max_hit,
            IS_NPC(gch) ? "{G" : 100 * gch->mana / gch->max_mana < 15 ? "{R" : 100 * gch->mana / gch->max_mana < 50 ? "{Y" : "{G",
            IS_NPC(gch) ? 100 : 100 * gch->mana / gch->max_mana,
            IS_NPC(gch) ? "{G" : 100 * gch->move / gch->max_move < 15 ? "{R" : 100 * gch->move / gch->max_move < 50 ? "{Y" : "{G",
            IS_NPC(gch) ? 100 : 100 * gch->move / gch->max_move,
            IS_NPC(gch) ? 0 : ((gch->level + 1) * exp_per_level(gch,gch->pcdata->points) - gch->exp) );
        send_to_char( buf, ch );
        }
    }
    send_to_char("{D+------------------------------------------------------------------------+{x \n\r", ch);
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    return;
    }

    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
    send_to_char( "But you are following someone else!\n\r", ch );
    return;
    }

    if ( victim->master != ch && ch != victim )
    {
    act_new("$N isn't following you.",ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    return;
    }

    if (IS_AFFECTED(victim,AFF_CHARM))
    {
        send_to_char("You can't remove charmed mobs from your group.\n\r",ch);
        return;
    }

    if (IS_AFFECTED(ch,AFF_CHARM))
    {
        act_new("You like your master too much to leave $m!",
        ch,NULL,victim,TO_VICT,POS_DEAD, FALSE);
        return;
    }

    if ( is_same_group( victim, ch ) && ch != victim )
    {
    victim->leader = NULL;
    act_new("$n removes $N from $s group.",
        ch,NULL,victim,TO_NOTVICT,POS_DEAD, FALSE);
    act_new("$n removes you from $s group.",
        ch,NULL,victim,TO_VICT,POS_DEAD, FALSE);
    act_new("You remove $N from your group.",
        ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    if (!str_cmp(arg2, "abandon"))
    {
        act_new("You abandon $N entirely.", ch, NULL, victim, TO_CHAR, POS_DEAD, FALSE);
        victim->master = NULL;
    }
    return;
    }

    for ( groupie = char_list; groupie != NULL; groupie = groupie->next )
    {
        if ( is_same_group( groupie, ch ) && !IN_LEVEL(victim,groupie))
        {
            send_to_char("That person is unable to join this group.\n\r",ch);
            return;
        }
    }

    if (!IN_LEVEL(ch,victim))
    {
      act_new("$n tries to group $N but $n fails.",
        ch,NULL,victim,TO_NOTVICT,POS_DEAD, FALSE);
      act_new("$n is unable to add you to $s group.",
        ch,NULL,victim,TO_VICT,POS_DEAD, FALSE);
      act_new("You are unable to add $N to your group.",
        ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
      return;
    }

    if(!IS_NPC(ch))
    {
        CHAR_DATA *gch;
        int members;

        members = 0;
        for ( gch = char_list; gch != NULL; gch = gch->next )
        {
            if ( is_same_group( gch, ch) && gch != ch && !IS_NPC(gch))
            members++;
        }

        if(members > (ch->csabilities[CSABIL_LEADERSHIP]*2)+1)
        {
            send_to_char("You do not have the skills needed to lead so many people.\n\r",ch);
            return;
        }
    }

    victim->leader = ch;
    act_new("$N joins $n's group.",ch,NULL,victim,TO_NOTVICT,POS_DEAD, FALSE);
    act_new("You join $n's group.",ch,NULL,victim,TO_VICT,POS_DEAD, FALSE);
    act_new("$N joins your group.",ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    return;

}



/*
 * 'Split' originally by Gnort, God of Chaos.
 */
void do_split( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH],arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    int members;
    int amount_gold = 0, amount_silver = 0;
    int share_gold, share_silver;
    int extra_gold, extra_silver;

    argument = one_argument( argument, arg1 );
           one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
    send_to_char( "Split how much?\n\r", ch );
    return;
    }

    amount_silver = atoi( arg1 );

    if (arg2[0] != '\0')
    amount_gold = atoi(arg2);

    if ( amount_gold < 0 || amount_silver < 0)
    {
    send_to_char( "Your group wouldn't like that.\n\r", ch );
    return;
    }

    if ( amount_gold == 0 && amount_silver == 0 )
    {
    send_to_char( "You hand out zero coins, but no one notices.\n\r", ch );
    return;
    }

    if ( ch->gold <  amount_gold || ch->silver < amount_silver)
    {
    send_to_char( "You don't have that much to split.\n\r", ch );
    return;
    }

    members = 0;
    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
    if ( is_same_group( gch, ch ) && !IS_AFFECTED(gch,AFF_CHARM))
        members++;
    }

    if ( members < 2 )
    {
    send_to_char( "Just keep it all.\n\r", ch );
    return;
    }

    share_silver = amount_silver / members;
    extra_silver = amount_silver % members;

    share_gold   = amount_gold / members;
    extra_gold   = amount_gold % members;

    if ( share_gold == 0 && share_silver == 0 )
    {
    send_to_char( "Don't even bother, cheapskate.\n\r", ch );
    return;
    }

    ch->silver  -= amount_silver;
    ch->silver  += share_silver + extra_silver;
    ch->gold    -= amount_gold;
    ch->gold    += share_gold + extra_gold;

    if (share_silver > 0)
    {
    sprintf(buf,
        "You split %d silver coins. Your share is %d silver.\n\r",
        amount_silver,share_silver + extra_silver);
    send_to_char(buf,ch);
    }

    if (share_gold > 0)
    {
    sprintf(buf,
        "You split %d gold coins. Your share is %d gold.\n\r",
         amount_gold,share_gold + extra_gold);
    send_to_char(buf,ch);
    }

    if (share_gold == 0)
    {
    sprintf(buf,"$n splits %d silver coins. Your share is %d silver.",
        amount_silver,share_silver);
    }
    else if (share_silver == 0)
    {
    sprintf(buf,"$n splits %d gold coins. Your share is %d gold.",
        amount_gold,share_gold);
    }
    else
    {
    sprintf(buf,
"$n splits %d silver and %d gold coins, giving you %d silver and %d gold.\n\r",
     amount_silver,amount_gold,share_silver,share_gold);
    }

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
    if ( gch != ch && is_same_group(gch,ch) && !IS_AFFECTED(gch,AFF_CHARM))
    {
        act( buf, ch, NULL, gch, TO_VICT );
        gch->gold += share_gold;
        gch->silver += share_silver;
    }
    }

    return;
}


void do_gtell( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *gch;

    if ( argument[0] == '\0' )
    {
    send_to_char( "Tell your group what?\n\r", ch );
    return;
    }

    if ( IS_SET( ch->comm, COMM_NOTELL ) )
    {
    send_to_char( "Your message didn't get through!\n\r", ch );
    return;
    }

    act_new("$n tells the group '{Y$t{x'",
    ch,argument,NULL,TO_CHAR,POS_DEAD, TRUE);

    for ( gch = char_list; gch != NULL; gch = gch->next )
    {
    if ( is_same_group( gch, ch ) )
        act_new("$n tells the group '{Y$t{x'",
        ch,argument,gch,TO_VICT,POS_DEAD, TRUE);
    }

    return;
}



/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
    if ( ach == NULL || bch == NULL)
    return FALSE;

    if ( ach->leader != NULL ) ach = ach->leader;
    if ( bch->leader != NULL ) bch = bch->leader;
    return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
void do_colour( CHAR_DATA *ch, char *argument )
{
    char    arg[ MAX_STRING_LENGTH ];

    argument = one_argument( argument, arg );
    if(IS_NPC(ch)) return;
    if( !*arg )
    {

    if( !IS_SET( ch->act, PLR_COLOUR ) )
    {
        SET_BIT( ch->act, PLR_COLOUR );
        send_to_char( "{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r", ch );
    }
    else
    {
        send_to_char_bw( "Colour is now OFF, <sigh>\n\r", ch );
        REMOVE_BIT( ch->act, PLR_COLOUR );
    }
    return;
    }
    else
    {
    send_to_char_bw( "Colour Configuration is unavailable in this\n\r", ch );
    send_to_char_bw( "version of colour, sorry\n\r", ch );
    }

    return;
}

void do_announce(CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   DESCRIPTOR_DATA *d;

    if(get_trust(ch) >= (MAX_LEVEL - 4) && argument[0] != '\0' )
    {
          sprintf( buf, "{R*{YAnnouncement{R*{w %s{x\n\r", argument);
          send_to_char( buf, ch );

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch)
            {
                sprintf( buf, "{R*{YAnnouncement{R*{w %s{x\n\r", argument);
                send_to_char( buf, d->character );
            }
        }

    }
    else
    {
        if (IS_SET(ch->comm,COMM_NOANNOUNCE))
        {
            send_to_char("Announcement channel is now ON.\n\r",ch);
            REMOVE_BIT(ch->comm,COMM_NOANNOUNCE);
        }
        else
        {
            send_to_char("Announcement channel is now OFF.\n\r",ch);
            SET_BIT(ch->comm,COMM_NOANNOUNCE);
        }
    }
return;
}

void announce( CHAR_DATA *ch, CHAR_DATA *victim, long flag )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *target;
    DESCRIPTOR_DATA *d;

    if(flag == WIZ_NEWBIE)
        sprintf( buf, "{R*{YAnnouncement{R*{w Haven of the Embraced welcomes it's newest player, %s!{x\n\r", ch->name);
    if(flag == WIZ_LOGINS)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s leaves the real world behind to seek sanctuary in the Haven.{x\n\r", ch->name);
    if(flag == WIZ_SITES)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s bravely leaves the Haven in search of adventure in the real world.{x\n\r", ch->name);
    if(flag == WIZ_DEATHS)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s has fallen to the wrath of %s!{x\n\r", ch->name, IS_NPC(victim) ? victim->short_descr : victim->name);
    if(flag == WIZ_MOBDEATHS)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s has been killed by %s in {RArena{w!{x\n\r", ch->name, IS_NPC(victim) ? victim->short_descr : victim->name);
    if(flag == WIZ_LOAD)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s has entered the {RArena{w!{x\n\r", ch->name);
    if(flag == WIZ_LEVELS)
        sprintf( buf, "{R*{YAnnouncement{R*{w %s has reached level %d!{x\n\r", ch->name, ch->level);

    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        target = d->original ? d->original : d->character;

        if ( d->connected == CON_PLAYING &&
/*        d->character != ch && */
          !IS_SET(target->comm,COMM_NOANNOUNCE) &&
          (can_see(target,ch) || flag == WIZ_DEATHS) &&
          !IS_IMMORTAL(ch))
            send_to_char( buf, d->character );
    }
return;
}

void do_tsect( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if(IS_NPC(ch))
        return;

    if (ch->pcdata->sect == 0)
    {
    send_to_char("You aren't in a sect.\n\r",ch);
    return;
    }
    if ( argument[0] == '\0' )
    {
      if (IS_SET(ch->comm,COMM_NOSECT))
      {
        send_to_char("Sect channel is now ON\n\r",ch);
        REMOVE_BIT(ch->comm,COMM_NOSECT);
      }
      else
      {
        send_to_char("Sect channel is now OFF\n\r",ch);
        SET_BIT(ch->comm,COMM_NOSECT);
      }
      return;
    }

        if (IS_SET(ch->comm,COMM_NOCHANNELS))
        {
         send_to_char("The gods have revoked your channel priviliges.\n\r",ch);
          return;
        }

        REMOVE_BIT(ch->comm,COMM_NOSECT);

      sprintf( buf, "You tell the %s '{G%s{x'\n\r", sect_table[ch->pcdata->sect].cname, argument );
      send_to_char( buf, ch );
      sprintf( buf, "%s tells the %s '{G%s{x'", ch->name, sect_table[ch->pcdata->sect].cname, argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        if ( d->connected == CON_PLAYING &&
             d->character != ch &&
         ch->pcdata->sect == d->character->pcdata->sect &&
             !IS_SET(d->character->comm,COMM_NOSECT) &&
             !IS_NPC(d->character) &&
         !IS_SET(d->character->comm,COMM_QUIET) )
        {
/*      send_to_char(buf,d->character); */
            act_new(buf,ch,NULL,d->character,TO_VICT,POS_DEAD, TRUE);
        }
    }

    return;
}

void do_huh(CHAR_DATA *ch, char *argument)
{
    send_to_char( "Huh?\n\r", ch );
    return;
}

/**************************************************************************/
// Kal - April 03
void do_replayroom(CHAR_DATA *ch, char *argument)
{
    if (!ch && IS_NPC(ch))
            return;

    if(!ch->pcdata){
        println( "Uncontrolled mobs can't replay room events.", ch);
        return;
    }

    bool found=FALSE;
    int i;
    BUFFER *buffer;
    buffer = new_buf();
    int end=(ch->pcdata->next_replayroom-1+MAX_REPLAYROOM)%MAX_REPLAYROOM;
    for(i=ch->pcdata->next_replayroom; i!=end; i= (i+1)%MAX_REPLAYROOM){
        if(!IS_NULLSTR(ch->pcdata->replayroom_text[i])){
            if(!found){
                titlebar(ch);
            }
            add_buf(buffer, ch->pcdata->replayroom_text[i]);
            found=TRUE;
        }
    }
    if(!IS_NULLSTR(ch->pcdata->replayroom_text[end])){
        if(!found){
                add_buf(buffer, "{D-===========================================================================-{x\n\r");
        }
        add_buf(buffer, ch->pcdata->replayroom_text[end]);
        found=TRUE;
    }
    if(!found){
        println("You haven't seen/heard any room events since you logged on.", ch);
        free_buf(buffer);
        return;
    }

    if(IS_NEWBIE(ch)){
        add_buf(buffer, "note: you can also replay your tells by typing 'replay'\n\r");
        add_buf(buffer, "      and replay channel events by typing 'replaychannels'\n\r");
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}
/**************************************************************************/
// Adapted from code by Kal - Sept 20
void do_replaychannels(CHAR_DATA *ch, char *argument)
{
    if (!ch && IS_NPC(ch))
            return;

    if(!ch->pcdata){
        println( "Uncontrolled mobs can't replay room events.", ch);
        return;
    }
    BUFFER *buffer;
    buffer = new_buf();
    bool found=FALSE;
    int i;
    int end=(ch->pcdata->next_replaychannels-1+MAX_REPLAYCHANNELS)%MAX_REPLAYCHANNELS;
    for(i=ch->pcdata->next_replaychannels; i!=end; i= (i+1)%MAX_REPLAYCHANNELS){
        if(!IS_NULLSTR(ch->pcdata->replaychannels_text[i])){
            if(!found){
                add_buf(buffer, "{D-===========================================================================-{x\n\r");

            }
            add_buf(buffer, ch->pcdata->replaychannels_text[i]);
            found=TRUE;
        }
    }
    if(!IS_NULLSTR(ch->pcdata->replaychannels_text[end])){
        if(!found){
                add_buf(buffer, "{D-===========================================================================-{x\n\r");
        }
        add_buf(buffer, ch->pcdata->replaychannels_text[end]);
        found=TRUE;
    }
    if(!found){
        println("You haven't seen/heard any channels since you logged on.", ch);
        return;
    }

    if(IS_NEWBIE(ch)){
        add_buf(buffer, "note: you can also replay your tells by typing 'replay'\n\r");
        add_buf(buffer, "      and replay room events by typing 'replayroom'\n\r");
    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
    return;
}
/**************************************************************************/
// Adapted from code by Kal - Sept 20
void do_replay(CHAR_DATA *ch, char *argument)
{
    if (!ch && IS_NPC(ch))
            return;

    if(!ch->pcdata){
        println( "Uncontrolled mobs can't replay room events.", ch);
        return;
    }

    bool found=FALSE;
    int i;
    BUFFER *buffer;
    buffer = new_buf();
    int end=(ch->pcdata->next_replaytell-1+MAX_REPLAYTELL)%MAX_REPLAYTELL;
    for(i=ch->pcdata->next_replaytell; i!=end; i= (i+1)%MAX_REPLAYTELL){
        if(!IS_NULLSTR(ch->pcdata->replaytell_text[i])){
            if(!found){
                add_buf(buffer, "\n\r");
                add_buf(buffer, "{D-===========================================================================-{x\n\r");
            }
            add_buf(buffer, ch->pcdata->replaytell_text[i]);
            found=TRUE;
        }
    }
    if(!IS_NULLSTR(ch->pcdata->replaytell_text[end])){
        if(!found){
        add_buf(buffer, ch->pcdata->replaytell_text[end]);
        found=TRUE;
    }
    if(!found){
        println("You haven't received any tells since you logged on.", ch);
        free_buf(buffer);
        return;
    }

    if(IS_NEWBIE(ch)){
        add_buf(buffer, "note: you can also replay your tells by typing 'replay'\n\r");
        add_buf(buffer, "      and replay room events by typing 'replayroom'\n\r");
    }

    }
    page_to_char(buf_string(buffer),ch);
    free_buf(buffer);
}
/**************************************************************************/

void do_promote_leader( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *groupie;

    one_argument( argument, arg );

    if (!IS_NPC(ch) && IS_SET(ch->act,PLR_ARENA))
    {
    send_to_char( "No grouping in the arena!\n\r", ch );
    return;
    }


    if ( ch->master != NULL || ( ch->leader != NULL && ch->leader != ch ) )
    {
    send_to_char( "But you are following someone else!\n\r", ch );
    return;
    }

    if ( ( victim = get_char_room( ch, NULL, arg ) ) == NULL )
    {
    send_to_char( "Promote whom to lead your group?\n\r", ch );
    return;
    }



    if (IS_NPC(victim))
    {
        sendch("You cannot promote an NPC to group leader!\n\r", ch);
        return;
    }

        CHAR_DATA *gch;
        int members;

        members = 0;
        for ( gch = char_list; gch != NULL; gch = gch->next )
        {
            if ( is_same_group( gch, ch) && gch != ch && gch != victim && !IS_NPC(gch))
            members++;
        }

        if(members > (victim->csabilities[CSABIL_LEADERSHIP]*2)+1)
        {
            send_to_char("They do not have the skills needed to lead so many people.\n\r",ch);
            return;
        }

        for ( gch = char_list; gch != NULL; gch = gch->next )
        {
            if ( gch->leader == ch && gch != victim)
            {
                gch->leader = victim;
                gch->master = victim;
            }
        }

        ch->leader = victim;
        ch->master = victim;
        victim->leader = NULL;
        victim->master = NULL;
    act_new("$n transfers leadership of their group to $N.",ch,NULL,victim,TO_NOTVICT,POS_DEAD, FALSE);
    act_new("You are now the leader of $n's group.",ch,NULL,victim,TO_VICT,POS_DEAD, FALSE);
    act_new("$N now leads your group.",ch,NULL,victim,TO_CHAR,POS_DEAD, FALSE);
    return;

}
