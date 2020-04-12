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
 ***************************************************************************/

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
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

extern CMD_DATA * cmd_first;
extern CMD_DATA * cmd_last;
#define CMDEDIT( fun )      bool fun (CHAR_DATA *ch, char *argument )
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2
#define LOG_FILE        3

extern const struct cmdfun_type cmdfun_table[];
extern const struct size_type cat_table[];
extern const struct size_type log_table[];
char *dofuname args ((CMD_DATA *cmd ) );
CMD_DATA *new_cmd args( (void) );
void free_cmd args( (CMD_DATA *cmd) );
DO_FUN *dofun_lookup args( (const char *name) );

DECLARE_REPEAT_FUN(do_potency, &do_bloodofpotency);
DECLARE_REPEAT_FUN(do_gogarou, &do_garouconvert);
DECLARE_REPEAT_FUN(do_wield, &do_wear);
DECLARE_REPEAT_FUN(do_pray, &do_cast);
DECLARE_REPEAT_FUN(do_spitblood, &do_acidblood);
DECLARE_REPEAT_FUN(do_coat, &do_bloodagony);
DECLARE_REPEAT_FUN(do_sheet, &do_charsheet);
DECLARE_REPEAT_FUN(do_cs, &do_charsheet);
DECLARE_REPEAT_FUN(do_tbtalk, &do_tribetalk);
DECLARE_REPEAT_FUN(do_tdtalk, &do_traditiontalk);
DECLARE_REPEAT_FUN(do_cs2, &do_charsheet2);
DECLARE_REPEAT_FUN(do_cs3, &do_charsheet3);
DECLARE_REPEAT_FUN(do_prayers, &do_spells);
DECLARE_REPEAT_FUN(do_color, &do_colour);
DECLARE_REPEAT_FUN(do_ansi, &do_colour);
DECLARE_REPEAT_FUN(do_hold, &do_wear);
DECLARE_REPEAT_FUN(do_bs, &do_backstab);
DECLARE_REPEAT_FUN(do_journal, &do_qlog);
DECLARE_REPEAT_FUN(do_poofin, &do_bamfin);
DECLARE_REPEAT_FUN(do_poofout, &do_bamfout);
DECLARE_REPEAT_FUN(do_gecho, &do_echo);
DECLARE_REPEAT_FUN(do_wizinvis, &do_invis);
DECLARE_REPEAT_FUN(do_edit, &do_olc);


bool    check_social    args( ( CHAR_DATA *ch, char *command,
                char *argument ) );



/*
 * Command logging types.
 */
#define LOG_NORMAL  0
#define LOG_ALWAYS  1
#define LOG_NEVER   2
#define LOG_FILE    3


/*
 * Log-all switch.
 */
bool                fLogAll     = FALSE;
bool                Debug       = FALSE;





/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char *cheat1;
    char cheat2[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;
    CMD_DATA *pCmd;
    CMD_DATA *fCmd;
    FILE *fp;

    /* Reset act_looking */
    ACT_LOOKING = FALSE;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
    argument++;
    if ( argument[0] == '\0' )
    return;

    if (Debug)
    {
        fclose( fpReserve );
        if ( ( fp = fopen( LAST_COMMAND_FILE, "w" ) ) == NULL )
        {
            bug( "Opening_LAST_COMMAND_FILE: fopen", 0 );
            perror( LAST_COMMAND_FILE );
        }
        else
        {
            fprintf( fp, "Name:  %s~\n", ch->name);
            fprintf( fp, "Room:  %d~\n", ch->in_room->vnum);
            fprintf( fp, "Argument:  %s~\n", argument);
        }
        fclose( fp );
        fpReserve = fopen( NULL_FILE, "r" );
    }
    /*
     * No hiding.
     */
/*    REMOVE_BIT( ch->affected_by, AFF_HIDE ); */

    /*
     * Remove veil and earthmeld.
     */
    if(is_affected(ch,gsn_earthmeld) && str_prefix( argument, "regenerate" ))
    {
        act("$n emerges from the earth in a shower of dirt.",ch,NULL,NULL,TO_NOTVICT);
        act("You emerge from from the earth in a shower of dirt.",ch,NULL,NULL,TO_CHAR);
        affect_strip(ch,gsn_earthmeld);
    }
  /*  if(ch->obfuscate < 6) REMOVE_BIT(ch->affected2_by, AFF2_VEIL);*/

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE) )
    {
    send_to_char( "You're totally frozen!\n\r", ch );
    return;
    }
    /* html cheat filtering Makes all <>s into ()s */
    if(ch->level < MAX_LEVEL-8) strcpy(argument,squash_html(argument,TRUE));


    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
    command[0] = argument[0];
    command[1] = '\0';
    argument++;
    while ( isspace(*argument) )
        argument++;
    }
    else
    {
    argument = one_argument( argument, command );
    }

    //While trapped in Gauntlet, only certain commands allowed
    if (!IS_NPC(ch) && is_affected(ch, trappedingauntlet) &&
    	str_prefix(command, "ooc") &&
    	str_prefix(command, "inventory") &&
    	str_prefix(command, "who") &&
    	str_prefix(command, "score") &&
    	str_prefix(command, "affect") &&
    	str_prefix(command, "look") &&
    	str_prefix(command, "where") &&
    	str_prefix(command, "time") &&
    	str_prefix(command, "charsheet") &&
    	str_prefix(command, "cs1") &&
    	str_prefix(command, "cs2") &&
    	str_prefix(command, "cs3") &&
    	str_prefix(command, "plead") &&
    	str_prefix(command, "freebie") &&
    	str_prefix(command, "prompt") &&
    	str_prefix(command, "group") &&
    	str_prefix(command, "traditiontalk") &&
    	str_prefix(command, "tell") &&
    	str_prefix(command, "wizlist") &&
    	str_prefix(command, "afk") &&
    	str_prefix(command, "gtell") &&
    	str_prefix(command, "reply") &&
    	str_prefix(command, "quit"))
    {
    	send_to_char("What is left of your body refuses to do anything while still entangled in the Gauntlet.\n\r", ch);
    	return;
    }


    if(IS_SET(ch->act,PLR_SPEC) && !IS_NPC(ch) &&
        str_prefix(command, "north") && str_prefix(command, "south") &&
        str_prefix(command, "east") && str_prefix(command, "west") &&
        str_prefix(command, "up") && str_prefix(command, "down") &&
        str_prefix(command, "ooc") && str_prefix(command, "recall") &&
        str_prefix(command, "inventory") && str_prefix(command, "who") &&
        str_prefix(command, "score") && str_prefix(command, "affect") &&
        str_prefix(command, "look") && str_prefix(command, "glance") &&
        str_prefix(command, "where") && str_prefix(command, "scan") &&
        str_prefix(command, "eat") && str_prefix(command, "drink") &&
        str_prefix(command, "time"))
    {
        send_to_char( "You are unable to do that while in spectate mode.\n\r", ch );
        return;
    }
/*Sengir moved losing veil here, can use certain commands while veiled*/
    if ((!IS_NPC(ch) && ch->pcdata->discipline[OBFUSCATE] < 6) &&
            str_prefix (command, "veil") &&
            str_prefix(command, "beseech blissful") &&
            str_prefix(command, "ooc") &&
            str_prefix(command, "inventory") &&
            str_prefix(command, "who") &&
            str_prefix(command, "score") &&
            str_prefix(command, "affects") &&
            str_prefix(command, "look") &&
            str_prefix(command, "glance") &&
            str_prefix(command, "where") &&
            str_prefix(command, "scan") &&
            str_prefix(command, "time") &&
            str_prefix(command, "charsheet") &&
            str_prefix(command, "cs1") &&
            str_prefix(command, "cs2") &&
            str_prefix(command, "cs3")) {
        REMOVE_BIT(ch->affected2_by, AFF2_VEIL);
        affect_strip(ch, gsn_veil);
        }
    /*
     * Look for command in command table.
     */
    found = FALSE;
    pCmd = NULL;
    trust = get_trust( ch );
    for ( fCmd = cmd_first; fCmd ; fCmd = fCmd->next )
    {
        if (found)
            break;

        if ( command[0] == fCmd->name[0]
        &&   !str_prefix( command, fCmd->name )
        &&   fCmd->level <= trust )
        {
            found = TRUE;
            pCmd = fCmd;
            break;
        }
    }


    if ( !found )
    {
    /*
     * Look for command in socials table.
     */
    if ( !check_social( ch, command, argument ) )
        send_to_char( "Huh?\n\r", ch );
    return;
    }

    /*
     * Log and snoop.
     */
    if ( !pCmd )
    {
        bug("No pCmd!", 0);
        return;
    }

    if ( pCmd->log == LOG_NEVER )
    strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   pCmd->log == LOG_ALWAYS || pCmd->log == LOG_FILE )
    {
    sprintf( log_buf, "Log %s: %s", ch->name, logline );
    if (pCmd->log == LOG_ALWAYS)
        wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
    if (pCmd->log == LOG_FILE)
        wiznet(log_buf, ch, NULL, WIZ_FILE, 0, get_trust(ch));
    log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
    write_to_buffer( ch->desc->snoop_by, "% ",    2 );
    write_to_buffer( ch->desc->snoop_by, logline, 0 );
    write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }


    /*
     * Character not in position for command?
     */
    if ( ch->position < pCmd->position )
    {
    switch( ch->position )
    {
    case POS_TORPOR:
        send_to_char( "You cannot do that. You are in Torpor.\n\r", ch );
        break;

    case POS_DEAD:
        send_to_char( "Lie still; you are DEAD.\n\r", ch );
        break;

    case POS_MORTAL:
    case POS_INCAP:
        send_to_char( "You are hurt far too bad for that.\n\r", ch );
        break;

    case POS_STUNNED:
        send_to_char( "You are too stunned to do that.\n\r", ch );
        break;

    case POS_SLEEPING:
        send_to_char( "In your dreams, or what?\n\r", ch );
        break;

    case POS_RESTING:
        send_to_char( "Nah... You feel too relaxed...\n\r", ch);
        break;

    case POS_SITTING:
        send_to_char( "Better stand up first.\n\r",ch);
        break;

    case POS_FIGHTING:
        send_to_char( "No way!  You are still fighting!\n\r", ch);
        break;

    }
    return;
    }

    /*
     * Dispatch the command.
     */
    if (!*pCmd->do_fun)
    {
        bugf("Command '%s' has no do_fun!", pCmd->name);
        cprintf(ch, "Whoops! '%s' has no do_function. Please alert an Immortal.\n\r", pCmd->name);
        tail_chain( );
        return;
    }
    (*pCmd->do_fun) ( ch, argument );

    tail_chain( );
    return;
}

/* function to keep argument safe in all commands -- no static strings */
void do_function (CHAR_DATA *ch, DO_FUN *do_fun, char *argument)
{
    char *command_string;

    /* copy the string */
    command_string = str_dup(argument);

    /* dispatch the command */
    (*do_fun) (ch, command_string);

    /* free the string */
    free_string(command_string);
}

bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
    if ( command[0] == social_table[cmd].name[0]
    &&   !str_prefix( command, social_table[cmd].name ) )
    {
        found = TRUE;
        break;
    }
    }

    if ( !found )
    return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE) )
    {
    send_to_char( "You are anti-social!\n\r", ch );
    return TRUE;
    }

    switch ( ch->position )
    {
    case POS_TORPOR:
    send_to_char( "You cannot do that. You are in Torpor.\n\r", ch );
    return TRUE;

    case POS_DEAD:
    send_to_char( "Lie still; you are DEAD.\n\r", ch );
    return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
    send_to_char( "You are hurt far too bad for that.\n\r", ch );
    return TRUE;

    case POS_STUNNED:
    send_to_char( "You are too stunned to do that.\n\r", ch );
    return TRUE;

    case POS_SLEEPING:
    /*
     * I just know this is the path to a 12" 'if' statement.  :(
     * But two players asked for it already!  -- Furey
     */
    if ( !str_cmp( social_table[cmd].name, "snore" ) )
        break;
    send_to_char( "In your dreams, or what?\n\r", ch );
    return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
    act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
    act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
    send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
    act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
    act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }
    else
    {
    act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
    act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
    act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );

    if ( !IS_NPC(ch) && IS_NPC(victim)
    &&   !IS_AFFECTED(victim, AFF_CHARM)
    &&   IS_AWAKE(victim)
    &&   victim->desc == NULL)
    {
        switch ( number_bits( 4 ) )
        {
        case 0:

        case 1: case 2: case 3: case 4:
        case 5: case 6: case 7: case 8:
        act( social_table[cmd].others_found,
            victim, NULL, ch, TO_NOTVICT );
        act( social_table[cmd].char_found,
            victim, NULL, ch, TO_CHAR    );
        act( social_table[cmd].vict_found,
            victim, NULL, ch, TO_VICT    );
        break;

        case 9: case 10: case 11: case 12:
        act( "$n slaps $N.",  victim, NULL, ch, TO_NOTVICT );
        act( "You slap $N.",  victim, NULL, ch, TO_CHAR    );
        act( "$n slaps you.", victim, NULL, ch, TO_VICT    );
        break;
        }
    }
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number ( char *arg )
{

    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit( *arg ) )
            return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
    if ( *pdot == '.' )
    {
        *pdot = '\0';
        number = atoi( argument );
        *pdot = '.';
        strcpy( arg, pdot+1 );
        return number;
    }
    }

    strcpy( arg, argument );
    return 1;
}

/*
 * Given a string like 14*foo, return 14 and 'foo'
*/
int mult_argument(char *argument, char *arg)
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '*' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '*';
            strcpy( arg, pdot+1 );
            return number;
        }
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';

    if ( *argument == '\'' || *argument == '"'
      || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *arg_first = LOWER(*argument);
    arg_first++;
    argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
    argument++;

    return argument;
}

// one_argument that retains case - Ugha
char *one_argument2( char *argument, char *arg_first )
{
    char cEnd;

    while ( isspace(*argument) )
    argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
    cEnd = *argument++;

    while ( *argument != '\0' )
    {
    if ( *argument == cEnd )
    {
        argument++;
        break;
    }
    *arg_first = *argument;
    arg_first++;
    argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
    argument++;

    return argument;
}


/*
 * Contributed by Alander.
 */
void do_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    int cmd;
    int col;
    CMD_DATA *pCmd;
    output = new_buf();


    if (IS_NULLSTR(argument))
    {
    col = 0;
    for(pCmd = cmd_first ; pCmd ; pCmd = pCmd->next )
    {
        if ( pCmd->level <  LEVEL_HERO
        &&   pCmd->level <= get_trust( ch )
    &&   pCmd->show)
    {
        sprintf( buf, "%-20s", pCmd->name );
        add_buf(output, buf);
        if ( ++col % 4 == 0 )
        add_buf(output, "\n\r");
    }
    }

    if ( col % 4 != 0 )
    add_buf(output, "\n\r");
    page_to_char(buf_string(output),ch);
    free_buf(output);
    return;
    } else {
        bool found = FALSE;
        cprintf(ch, "Searching command table for '%s'.\n\r", argument);
        col = 0;
        for (pCmd = cmd_first ; pCmd ; pCmd = pCmd->next )
        {
            if (!str_prefix(argument, pCmd->name) &&
                pCmd->level <= get_trust(ch) && pCmd->show)
                {
                    found = TRUE;
                    sprintf(buf, "%-20s", pCmd->name);
                    add_buf(output, buf);
                    if (++col % 4 == 0)
                        add_buf(output, "\n\r");
                }
        }
        if (found)
        {
            if (col % 4 != 0)
                add_buf(output, "\n\r");
            page_to_char(buf_string(output), ch);
            free_buf(output);
            return;
        } else
        {
            free_buf(output);
            sendch("Matching command not found.\n\r", ch);
            return;
        }
    }
}
/*
void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    int cmd;
    int col;

    col = 0;
    for ( cmd = 0; pCmd->name[0] != '\0'; cmd++ )
    {
        if ( pCmd->level >= LEVEL_HERO
        &&   pCmd->level <= get_trust( ch )
        &&   pCmd->show)
    {
        sprintf( buf, "%-12s", pCmd->name );
        send_to_char( buf, ch );
        if ( ++col % 6 == 0 )
        send_to_char( "\n\r", ch );
    }
    }

    if ( col % 6 != 0 )
    send_to_char( "\n\r", ch );
    return;
}
*/
//Replacement wizhelp added by Matthew. Sorts commands by level.
void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    int cmd;
    int col;
    int clevel;
    int start;
    int finish;
    CMD_DATA *pCmd;
    output = new_buf();

    if (is_number(argument) && (atoi(argument) <= get_trust(ch)))
    {
        start = atoi(argument);
        finish = start+1;
    } else
    {
        start = LEVEL_HERO + 1;
        finish = get_trust(ch) + 1;
    }


    for( clevel = start; clevel < finish; clevel++ )
    {
        sprintf(buf, "\n\r{m------------------------------ {w[Level {D%d{w] {m------------------------------{x\n\r", clevel);
        add_buf(output, buf);
        col = 0;
        for(pCmd = cmd_first ; pCmd ; pCmd = pCmd->next )
    {
            if ( pCmd->level >= LEVEL_HERO
            &&   pCmd->level <= get_trust( ch )
            &&   (pCmd->show || ch->level == MAX_LEVEL)
        &&   pCmd->level == clevel)
        {
            sprintf( buf, "{c[{x%-3d{c] %-19s{x", pCmd->level, pCmd->name );
            add_buf(output, buf);
            if ( ++col % 3 == 0 )
            add_buf(output, "\n\r");
        }
    }
    }

    if ( col % 3 != 0 )
    add_buf(output, "\n\r");
    page_to_char(buf_string(output), ch);
    free_buf(output);
    return;
}



void save_cmd_table()
{
    FILE *fp;
    CMD_DATA *cmd;

    if( (fp = fopen(COMMAND_FILE,"w+") ) != NULL )
    {
        for(cmd = cmd_first; cmd ; cmd = cmd->next )
        {

            fprintf(fp, "Name %s~\n", cmd->name );
            fprintf(fp, "Dofun %s~\n", dofuname(cmd) );
            fprintf(fp, "Position %d\n", cmd->position );
            fprintf(fp, "Level %d\n", cmd->level );
            fprintf(fp, "Log %d\n",cmd->log );
            fprintf(fp, "Show %d\n", cmd->show );
            fprintf(fp, "Cat %d\n", cmd->cat );
            fprintf(fp, "End\n\n");
        }
    }
    fprintf(fp, "$\n");
    fclose(fp);
}

void load_cmd_list()
{
    FILE *fp;
    CMD_DATA *cmd;
    char *word;
    if((fp = fopen(COMMAND_FILE, "r" )) == NULL)
    {   bug("You got no commands now! Your screwed!", 0);
        return;
    }
    for( ;; )
    {
        word = feof (fp) ? "End" : fread_word (fp);
        if(word[0] == '$' )
            return;
        switch (UPPER(word[0] ))
        {
            case 'C':
                if(!str_cmp(word, "Cat" ) )
                   cmd->cat = fread_number(fp);
                break;
            case 'D':
                if(!str_cmp(word, "Dofun" ) )
                    cmd->do_fun = dofun_lookup(fread_string(fp) );
                break;
            case 'E':
                if(!str_cmp(word, "End" ) )
                    break;
                break;
            case 'L':
                if(!str_cmp(word, "Level" ) )
                {   cmd->level = fread_number(fp);
                    break;
                }
                if(!str_cmp(word, "Log" ) )
                {   cmd->log = fread_number(fp);
                    break;
                }
                break;
            case 'N':
                if(!str_cmp(word, "Name" ) )
                {   cmd = new_cmd();
                    cmd->name = fread_string(fp);
                    break;
                }
                break;
            case 'P':
                if(!str_cmp(word, "Position" ) )
                {   cmd->position = fread_number(fp);
                    break;
                }
                break;
            case 'S':
                if(!str_cmp(word, "Show" ) )
                {   cmd->show = fread_number(fp);
                    break;
                }
                break;
        }//End of switch
    }//End of for

    file_close(fp);
}


char *dofuname (CMD_DATA *cmd)
{
    int i;
    for( i = 0; cmdfun_table[i].name != NULL ; i ++ )
    {
        if(cmd->do_fun == cmdfun_table[i].do_fun)
            return cmdfun_table[i].name;
    }
    char buf[MSL];
    sprintf(buf, "BUG: %s has no do_function", cmd->name);
    bug(buf, 0);
    return "do_huh";
}


DO_FUN *dofun_lookup(const char *name)
{
    int i;
    for (i = 0;cmdfun_table[i].name != NULL ; i ++)
    {
        if(!str_cmp(name, cmdfun_table[i].name ) )
            return cmdfun_table[i].do_fun;
    }
    return cmdfun_table[0].do_fun;
}
CMD_DATA *cmd_lookup(const char *name )
{
    CMD_DATA *cmd;

    for( cmd = cmd_first ; cmd ; cmd = cmd->next )
    {
        if(!str_cmp(name, cmd->name ) )
            return cmd;
    }
    return NULL;
}
CMD_DATA *cmd_free;

CMD_DATA *new_cmd(void)
{
    static CMD_DATA cmd_zero;
        CMD_DATA *cmd;

    if (cmd_free == NULL)
        cmd = alloc_perm(sizeof(*cmd));
        else
    {
        cmd = cmd_free;
        if (cmd->next == NULL)
            cmd_free = NULL;
    }
    *cmd = cmd_zero;

    if(!cmd_first )
        cmd_first = cmd;

    if(cmd_last)
    {   cmd_last->next = cmd;
        cmd->prev = cmd_last;
    }
    cmd_last = cmd;


     free_string(cmd->name);
     cmd->name = str_dup("NewCommand");
     cmd->do_fun = cmdfun_table[0].do_fun;
     cmd->level = 0;
     cmd->position = 0;
     cmd->show = 1;
     cmd->log = LOG_NORMAL;

    VALIDATE(cmd);
    return cmd;
}

void free_cmd(CMD_DATA *cmd)
{
    if (!IS_VALID(cmd))
                return;

    INVALIDATE(cmd);
    cmd->next = cmd_free;
    cmd->prev = cmd_free;
    cmd_free = cmd;
}


CMDEDIT(cmdedit_show )
{
    CMD_DATA *cmd;

    EDIT_COMMAND(ch, cmd );

    printf_to_char(ch, "\n\r{wCommand name        - {c%s{x\n\r", cmd->name );
          send_to_char("{w======================================{x\n\r",ch);
    printf_to_char(ch, "{wCommand Catagory    - {c%s{x\n\r", cat_table[cmd->cat].name );
    printf_to_char(ch, "{wCommand Do_function - {c%s{x\n\r", dofuname(cmd) );
    printf_to_char(ch, "{wCommand Level       - {c%d{x\n\r", cmd->level );
    printf_to_char(ch, "{wCommand Position    - {c%s{x\n\r", position_table[cmd->position].name );
    printf_to_char(ch, "{wCommand Show        - {c%s{x\n\r", cmd->show == TRUE ? "Yes" : "No" );
    printf_to_char(ch, "{wCommand Log         - {c%s{x\n\r", log_table[cmd->log].name );
    return FALSE;
}

CMDEDIT( cmdedit_name )
{
    CMD_DATA *cmd;

    EDIT_COMMAND(ch, cmd );

    if(argument[0] == '\0')
    {
        send_to_char("Syntax: name <name>\n\r",ch);
        return FALSE;
    }

    if( cmd_lookup(argument ) != NULL )
    {
        send_to_char("There is already a command by that name.\n\r",ch);
        return FALSE;
    }

    free_string(cmd->name );
    cmd->name = str_dup(argument );
    return TRUE;
}

int cat_lookup (const char *name )
{
    int i;

    for( i = 0 ; cat_table[i].name != NULL ; i++ )
    {
        if(!str_cmp(cat_table[i].name, name )  )
            return i;
    }
    return -1;
}

CMDEDIT( cmdedit_catagory )
{
    CMD_DATA *cmd;
    int cat;
    EDIT_COMMAND(ch, cmd );

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: catagory <catagory>\n\r",ch);
        return FALSE;
    }

    if( ( cat = cat_lookup (argument ) ) == -1 )
    {
        send_to_char("What type of catagory is that!?\n\r",ch);
        return FALSE;
    }

    printf_to_char(ch, "%s is now placed in the %s catagory.\n\r",cmd->name, cat_table[cat].name );
    cmd->cat = cat;
    return TRUE;
}

CMDEDIT( cmdedit_dofun )
{
    CMD_DATA *cmd;
    DO_FUN *dofun;

    EDIT_COMMAND(ch, cmd);

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: dofun <dofun>\n\r",ch);
        return FALSE;
    }

    if( ( dofun = dofun_lookup(argument ) ) == NULL )
    {
        send_to_char("No such do_function. Did you add it to the dofun_table?\n\r",ch);
        return FALSE;
    }

    cmd->do_fun = dofun;
    send_to_char("Do function set.\n\r",ch);
    return TRUE;
}

CMDEDIT (cmdedit_level )
{
    CMD_DATA *cmd;
    int level;
    char arg[MSL];

    EDIT_COMMAND(ch, cmd);

    argument = one_argument(argument,arg);

    if(arg[0] == '\0' )
    {
        send_to_char("Syntax level <#level>\n\r",ch);
        return FALSE;
    }
    if(!is_number(arg) )
    {
        send_to_char("It must be a number.\n\r",ch);
        return FALSE;
    }

    level = atoi(arg);
    if(level  > MAX_LEVEL || level < 0 )
    {
        send_to_char("It must be less than 159, and greater than 0.\n\r",ch);
        return FALSE;
    }
    send_to_char("Level set.\n\r",ch);
    cmd->level = level;
    return TRUE;
}

CMDEDIT( cmdedit_position )
{
    CMD_DATA *cmd;
    int pos;

    EDIT_COMMAND (ch, cmd );

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: position <position>\n\r",ch);
        return FALSE;
    }

    if( ( pos = position_lookup(argument ) ) == -1 )
    {
        send_to_char("What type of position is that?\n\r", ch );
        return FALSE;
    }

    send_to_char("Position set.\n\r",ch);
    cmd->position = pos;
    return TRUE;
}

CMDEDIT( cmdedit_showcmd )
{
    CMD_DATA *cmd;

    EDIT_COMMAND(ch, cmd);

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: cmdshow <true/false>\n\r",ch);
        return TRUE;
    }

    if(!str_cmp(argument, "true" ) )
    {
        send_to_char("This command will now show.\n\r",ch);
        cmd->show = TRUE;
        return TRUE;
    }

    if(!str_cmp(argument, "false" ) )
    {
        send_to_char("This command will no longer be shown.\n\r",ch);
        cmd->show = FALSE;
        return TRUE;
    }

    send_to_char("Either 'true' or 'false'.\n\r",ch);
    return FALSE;
}

CMDEDIT( cmdedit_log)
{
    CMD_DATA *cmd;

    EDIT_COMMAND(ch, cmd);

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: log <never|always|normal>\n\r",ch);
        return FALSE;
    }

    if(!str_cmp(argument, "never" ) )
    {
        send_to_char("Logging set to never.\n\r",ch);
        cmd->log = LOG_NEVER;
        return TRUE;
    }
    if(!str_cmp(argument, "always" ) )
    {
        send_to_char("Logging set to always.\n\r",ch);
        cmd->log = LOG_ALWAYS;
        return TRUE;
    }
        if(!str_cmp(argument, "normal" ) )
    {
        send_to_char("Logging set to normal.\n\r",ch);
        cmd->log = LOG_NORMAL;
        return TRUE;
    }
    if (!str_cmp(argument, "file"))
    {
        send_to_char("Loggint set to file.\n\r", ch);
        cmd->log = LOG_FILE;
        return TRUE;
    }

    send_to_char("Either never, normal, file, or always.\n\r",ch);
    return FALSE;
}

CMDEDIT( cmdedit_new )
{
    CMD_DATA *cmd;

    if(argument[0] == '\0' )
    {
        send_to_char("Syntax: new <New Name>\n\r",ch);
        return FALSE;
    }

    if( cmd_lookup(argument ) != NULL )
    {
        send_to_char("That command already exists.\n\r",ch);
        return FALSE;
    }
    cmd = new_cmd();
    free_string(cmd->name );
    cmd->name = str_dup(argument);
    ch->desc->pEdit = (void *) cmd;
    send_to_char("Command created.\n\r",ch);
    return TRUE;
}

CMDEDIT ( cmdedit_delete )
{
    CMD_DATA *cmd, *cmd_del, *cmd_next, *cmd_prev;
    char arg1[MIL], arg2[MIL];
    EDIT_COMMAND(ch, cmd);
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    if (IS_NULLSTR(arg1) || IS_NULLSTR(arg2) || str_cmp(arg2, "confirm"))
    {
        send_to_char("Syntax: delete <command> confirm\n\r", ch);
        return FALSE;
    }
    if ( (cmd_del = cmd_lookup(arg1) ) == NULL)
    {
        sendch("Delete which command?\n\r", ch);
        return FALSE;
    }
    printf_to_char(ch, "Deleting command %s\n\r", cmd_del->name);
    cmd_next = cmd_del->next;
    cmd_prev = cmd_del->prev;
    cmd_del->next = NULL;
    cmd_del->prev = NULL;

    if(cmd_last == cmd_del )
        cmd_last = cmd_prev;
    else
        cmd_next->prev = cmd_prev;

    if(cmd_first == cmd_del )
        cmd_first = cmd_next;
    cmd_prev->next = cmd_next;
    free_cmd(cmd_del);
    sendch("Command Deleted.\n\r", ch);
    return TRUE;
}


CMDEDIT( cmdedit_move )
{
    CMD_DATA *cmd, *mCmd, *tCmd, *nCmd, *pCmd;
    char arg1[MSL], arg2[MSL], arg3[MSL];


    EDIT_COMMAND(ch, cmd);

    argument = one_argument(argument, arg1);
    argument = one_argument(argument, arg2);
    argument = one_argument(argument, arg3);

    if(arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char("Syntax: move <command> <befor|after> <to-command>\n\r",ch);
        return FALSE;
    }

    if( ( tCmd = cmd_lookup(arg3 ) ) == NULL )
    {
        send_to_char("To command, not existing.\n\r",ch );
        return FALSE;
    }

    if( ( mCmd = cmd_lookup(arg1 ) ) == NULL )
    {
        send_to_char("Which command would you like to move?\n\r",ch);
        return FALSE;
    }

    if( tCmd == mCmd )
    {
        send_to_char("You can't do that!\n\r",ch);
        return FALSE;
    }



    if(!str_cmp(arg2, "befor" ) )
    {
        pCmd = mCmd->prev;
        nCmd = mCmd->next;
        mCmd->next = NULL;
        mCmd->prev = NULL;

        if(cmd_last == mCmd )
            cmd_last = pCmd;
        else
            nCmd->prev = pCmd;

        if(cmd_first == mCmd )
            cmd_first = nCmd;
        pCmd->next = nCmd;

        if(cmd_first == tCmd )
        {
            cmd_first = mCmd;
            mCmd->next = tCmd;
            tCmd->prev = mCmd;
            printf_to_char(ch, "%s moved befor %s.\n\r", mCmd->name, tCmd->name );
            return TRUE;
        }

        pCmd = tCmd->prev;
        printf_to_char(ch, "%s moved befor %s.\n\r", mCmd->name, tCmd->name );
        tCmd->prev = mCmd;
        pCmd->next = mCmd;
        mCmd->prev = pCmd;
        mCmd->next = tCmd;
        return TRUE;
    }

    if(!str_cmp(arg2, "after" ) )
    {
        pCmd = mCmd->prev;
        nCmd = mCmd->next;
        mCmd->next = NULL;
        mCmd->prev = NULL;

        if(cmd_last == mCmd )
            cmd_last = pCmd;
        else
            nCmd->prev = pCmd;

        if(cmd_first == mCmd )
            cmd_first = nCmd;
        else
            pCmd->next = nCmd;

        if(cmd_last == tCmd )
        {
            cmd_last = mCmd;
            mCmd->prev = tCmd;
            tCmd->next = mCmd;
            printf_to_char(ch, "%s moved after %s.\n\r", mCmd->name, tCmd->name );
            return TRUE;
        }
        nCmd = tCmd->next;
        printf_to_char(ch, "%s moved after %s.\n\r", mCmd->name, tCmd->name );
        nCmd->prev = mCmd;
        tCmd->next = mCmd;
        mCmd->next = nCmd;
        mCmd->prev = tCmd;
        return TRUE;
    }
    send_to_char("It must be 'befor' or 'after' the command.\n\r",ch);
    return FALSE;
}

void do_cmdlist(CHAR_DATA *ch, char *argument )
{
    int col = 0;
    CMD_DATA *cmd;

    for(cmd = cmd_first; cmd ; cmd = cmd->next )
    {
          printf_to_char( ch, " [{c%-11s{x]", cmd->name );
          if ( ++col % 5 == 0 )
                        send_to_char("\n\r",ch);
    }
    return;
}

const struct size_type cat_table[] =
{
    { "None" },
    { "Config" },
    { "Common" },
    { "Communication" },
    { "Info"   },
    { "Object Manipulation" },
    { "Movement" },
    { "Combat"   },
    { NULL }
};

const struct size_type log_table[] =
{
    { "Normal" },
    { "Always" },
    { "Never"  },
    { "File"   },
    { NULL     }
};


void do_funlist(CHAR_DATA *ch, char *argument)
{
    int i;
    int col;
    char buf[MSL];
    send_to_char("Command do_fun list:\n\r\n\r", ch);
    for (i = 0;cmdfun_table[i].name != NULL ; i ++)
    {
        sprintf( buf, "%-20s", cmdfun_table[i].name );
        send_to_char( buf, ch );
        if ( ++col % 4 == 0 )
        send_to_char( "\n\r", ch );
    }

    if ( col % 4 != 0 )
    send_to_char( "\n\r", ch );
    send_to_char( "\n\rdo_funcs cannot be set in cmdedit until added to\n\r", ch);
    send_to_char( "dofun_table at the bottom of cmdedit.c\n\r", ch);
    return;
}
