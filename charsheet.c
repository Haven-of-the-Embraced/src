#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "interp.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

void do_demeanor(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg);

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0')
    {
       if (ch->cslock == 1)
       {
       sprintf(buf,"Your demeanor is set to %s.\n\r",ch->pcdata->csdemeanor);
       send_to_char(buf,ch);
       send_to_char("If you wish to see a list, see HELP DEMEANOR.\n\rTo change it, contact an immortal\n\r",ch);
       return;
       }

       if (ch->cslock == 0)
       {
       send_to_char("You have yet to set your demeanor. See HELP DEMEANOR for a list of possible choices.\n\r",ch);
       send_to_char("Type demeanor # to set your demeanor. Once set, you will need an immortal to change it.\n\r",ch);
       return;
       }

    }
    if (!str_prefix(arg,"Architect"))
    {
    ch->pcdata->csdemeanor = "Architect";
    send_to_char("You have selected Architect as your demeanor.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Autocrat"))
    {
    ch->pcdata->csdemeanor = "Autocrat";
    send_to_char("You have selected Autocrat as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Autist"))
    {
    ch->pcdata->csdemeanor = "Autist";
    send_to_char("You have selected Autist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Avant-Garde"))
    {
    ch->pcdata->csdemeanor = "Avant-Garde";
    send_to_char("You have selected Avant-Garde as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Barbarian"))
    {
    ch->pcdata->csdemeanor = "Barbarian";
    send_to_char("You have selected Barbarian as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Bon Vivant"))
    {
    ch->pcdata->csdemeanor = "Bon Vivant";
    send_to_char("You have selected Bon Vivant as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Bravo"))
    {
    ch->pcdata->csdemeanor = "Bravo";
    send_to_char("You have selected Bravo as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Caregiver"))
    {
    ch->pcdata->csdemeanor = "Caregiver";
    send_to_char("You have selected Caregiver as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Cavalier"))
    {
    ch->pcdata->csdemeanor = "Cavalier";
    send_to_char("You have selected Cavalier as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Child"))
    {
    ch->pcdata->csdemeanor = "Child";
    send_to_char("You have selected Child as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Celebrant"))
    {
    ch->pcdata->csdemeanor = "Celebrant";
    send_to_char("You have selected Celebrant as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Competitor"))
    {
    ch->pcdata->csdemeanor = "Competitor";
    send_to_char("You have selected Competitor as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Confidant"))
    {
    ch->pcdata->csdemeanor = "Confidant";
    send_to_char("You have selected Confidant as your demeanor.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Conformist"))
    {
    ch->pcdata->csdemeanor = "Conformist";
    send_to_char("You have selected Conformist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Conniver"))
    {
    ch->pcdata->csdemeanor = "Conniver";
    send_to_char("You have selected Conniver as your demeanor.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Critic"))
    {
    ch->pcdata->csdemeanor = "Critic";
    send_to_char("You have selected Critic as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Curmudgeon"))
    {
    ch->pcdata->csdemeanor = "Curmudgeon";
    send_to_char("You have selected Curmudgeon as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Defender"))
    {
    ch->pcdata->csdemeanor = "Defender";
    send_to_char("You have selected Defender as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Deviant"))
    {
    ch->pcdata->csdemeanor = "Deviant";
    send_to_char("You have selected Deviant as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Director"))
    {
    ch->pcdata->csdemeanor = "Director";
    send_to_char("You have selected Director as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Fanatic"))
    {
    ch->pcdata->csdemeanor = "Fanatic";
    send_to_char("You have selected Fanatic as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Gallant"))
    {
    ch->pcdata->csdemeanor = "Gallant";
    send_to_char("You have selected Gallant as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Honest-Abe"))
    {
    ch->pcdata->csdemeanor = "Honest-Abe";
    send_to_char("You have selected Honest-Abe as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Jester"))
    {
    ch->pcdata->csdemeanor = "Jester";
    send_to_char("You have selected Jester as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Jobsworth"))
    {
    ch->pcdata->csdemeanor = "Jobsworth";
    send_to_char("You have selected Jobsworth as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Judge"))
    {
    ch->pcdata->csdemeanor = "Judge";
    send_to_char("You have selected Judge as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Loner"))
    {
    ch->pcdata->csdemeanor = "Loner";
    send_to_char("You have selected Loner as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Manipulator"))
    {
    ch->pcdata->csdemeanor = "Manipulator";
    send_to_char("You have selected Manipulator as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Martyr"))
    {
    ch->pcdata->csdemeanor = "Martyr";
    send_to_char("You have selected Martyr as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Masochist"))
    {
    ch->pcdata->csdemeanor = "Masochist";
    send_to_char("You have selected Masochist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Mediator"))
    {
    ch->pcdata->csdemeanor = "Mediator";
    send_to_char("You have selected Mediator as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Monsters"))
    {
    ch->pcdata->csdemeanor = "Monsters";
    send_to_char("You have selected Monsters as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Optimist"))
    {
    ch->pcdata->csdemeanor = "Optimist";
    send_to_char("You have selected Optimist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Pedagogue"))
    {
    ch->pcdata->csdemeanor = "Pedagogue";
    send_to_char("You have selected Pedagogue as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Penitent"))
    {
    ch->pcdata->csdemeanor = "Penitent";
    send_to_char("You have selected Penitent as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Perfectionist"))
    {
    ch->pcdata->csdemeanor = "Perfectionist";
    send_to_char("You have selected Perfectionist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Plotter"))
    {
    ch->pcdata->csdemeanor = "Plotter";
    send_to_char("You have selected Plotter as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Poltroon"))
    {
    ch->pcdata->csdemeanor = "Poltroon";
    send_to_char("You have selected Poltroon as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Praise-Seeker"))
    {
    ch->pcdata->csdemeanor = "Praise-Seeker";
    send_to_char("You have selected Praise-Seeker as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Rebel"))
    {
    ch->pcdata->csdemeanor = "Rebel";
    send_to_char("You have selected Rebel as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Rogue"))
    {
    ch->pcdata->csdemeanor = "Rogue";
    send_to_char("You have selected Rogue as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Survivor"))
    {
    ch->pcdata->csdemeanor = "Survivor";
    send_to_char("You have selected Survivor as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Sycophant"))
    {
    ch->pcdata->csdemeanor = "Sycophatn";
    send_to_char("You have selected Sycophant as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Traditionalist"))
    {
    ch->pcdata->csdemeanor = "Traditionalist";
    send_to_char("You have selected Traditionalist as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Thrill-Seeker"))
    {
    ch->pcdata->csdemeanor = "Thrill-Seeker";
    send_to_char("You have selected Thrill-Seeker as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Trickster"))
    {
    ch->pcdata->csdemeanor = "Trickster";
    send_to_char("You have selected Trickster as your demeanor.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Visionary"))
    {
    ch->pcdata->csdemeanor = "Visionary";
    send_to_char("You have selected Visonary as your demeanor.\n\r",ch);
    return;
    }

    send_to_char("What? Are you trying to break Zelan's code?\n\r",ch);

    return;
}

void do_nature(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg);

    if (IS_NPC(ch)) return;

    if ( arg[0] == '\0')
    {
       if (ch->cslock == 1)
       {
       sprintf(buf,"Your nature is set to %s.\n\r",ch->pcdata->csnature);
       send_to_char(buf,ch);
       send_to_char("If you wish to see a list, see HELP NATURE.\n\rTo change it, contact an immortal\n\r",ch);
       return;
       }

       if (ch->cslock == 0)
       {
       send_to_char("You have yet to set your nature. See HELP nature for a list of possible choices.\n\r",ch);
       send_to_char("Type nature <name> to set your nature. Once set, you will need an immortal to change it.\n\r",ch);
       return;
       }

    }
    if (!str_prefix(arg,"Architect"))
    {
    ch->pcdata->csnature = "Architect";
    send_to_char("You have selected Architect as your nature.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Autocrat"))
    {
    ch->pcdata->csnature = "Autocrat";
    send_to_char("You have selected Autocrat as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Autist"))
    {
    ch->pcdata->csnature = "Autist";
    send_to_char("You have selected Autist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Avant-Garde"))
    {
    ch->pcdata->csnature = "Avant-Garde";
    send_to_char("You have selected Avant-Garde as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Barbarian"))
    {
    ch->pcdata->csnature = "Barbarian";
    send_to_char("You have selected Barbarian as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Bon Vivant"))
    {
    ch->pcdata->csnature = "Bon Vivant";
    send_to_char("You have selected Bon Vivant as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Bravo"))
    {
    ch->pcdata->csnature = "Bravo";
    send_to_char("You have selected Bravo as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Caregiver"))
    {
    ch->pcdata->csnature = "Caregiver";
    send_to_char("You have selected Caregiver as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Cavalier"))
    {
    ch->pcdata->csnature = "Cavalier";
    send_to_char("You have selected Cavalier as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Child"))
    {
    ch->pcdata->csnature = "Child";
    send_to_char("You have selected Child as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Celebrant"))
    {
    ch->pcdata->csnature = "Celebrant";
    send_to_char("You have selected Celebrant as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Competitor"))
    {
    ch->pcdata->csnature = "Competitor";
    send_to_char("You have selected Competitor as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Confidant"))
    {
    ch->pcdata->csnature = "Confidant";
    send_to_char("You have selected Confidant as your nature.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Conformist"))
    {
    ch->pcdata->csnature = "Conformist";
    send_to_char("You have selected Conformist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Conniver"))
    {
    ch->pcdata->csnature = "Conniver";
    send_to_char("You have selected Conniver as your nature.\n\r",ch);
    return;
    }
    if (!str_prefix(arg,"Critic"))
    {
    ch->pcdata->csnature = "Critic";
    send_to_char("You have selected Critic as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Curmudgeon"))
    {
    ch->pcdata->csnature = "Curmudgeon";
    send_to_char("You have selected Curmudgeon as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Defender"))
    {
    ch->pcdata->csnature = "Defender";
    send_to_char("You have selected Defender as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Deviant"))
    {
    ch->pcdata->csnature = "Deviant";
    send_to_char("You have selected Deviant as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Director"))
    {
    ch->pcdata->csnature = "Director";
    send_to_char("You have selected Director as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Fanatic"))
    {
    ch->pcdata->csnature = "Fanatic";
    send_to_char("You have selected Fanatic as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Gallant"))
    {
    ch->pcdata->csnature = "Gallant";
    send_to_char("You have selected Gallant as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Honest-Abe"))
    {
    ch->pcdata->csnature = "Honest-Abe";
    send_to_char("You have selected Honest-Abe as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Jester"))
    {
    ch->pcdata->csnature = "Jester";
    send_to_char("You have selected Jester as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Jobsworth"))
    {
    ch->pcdata->csnature = "Jobsworth";
    send_to_char("You have selected Jobsworth as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Judge"))
    {
    ch->pcdata->csnature = "Judge";
    send_to_char("You have selected Judge as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Loner"))
    {
    ch->pcdata->csnature = "Loner";
    send_to_char("You have selected Loner as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Manipulator"))
    {
    ch->pcdata->csnature = "Manipulator";
    send_to_char("You have selected Manipulator as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Martyr"))
    {
    ch->pcdata->csnature = "Martyr";
    send_to_char("You have selected Martyr as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Masochist"))
    {
    ch->pcdata->csnature = "Masochist";
    send_to_char("You have selected Masochist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Mediator"))
    {
    ch->pcdata->csnature = "Mediator";
    send_to_char("You have selected Mediator as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Monsters"))
    {
    ch->pcdata->csnature = "Monsters";
    send_to_char("You have selected Monsters as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Optimist"))
    {
    ch->pcdata->csnature = "Optimist";
    send_to_char("You have selected Optimist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Pedagogue"))
    {
    ch->pcdata->csnature = "Pedagogue";
    send_to_char("You have selected Pedagogue as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Penitent"))
    {
    ch->pcdata->csnature = "Penitent";
    send_to_char("You have selected Penitent as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Perfectionist"))
    {
    ch->pcdata->csnature = "Perfectionist";
    send_to_char("You have selected Perfectionist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Plotter"))
    {
    ch->pcdata->csnature = "Plotter";
    send_to_char("You have selected Plotter as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Poltroon"))
    {
    ch->pcdata->csnature = "Poltroon";
    send_to_char("You have selected Poltroon as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Praise-Seeker"))
    {
    ch->pcdata->csnature = "Praise-Seeker";
    send_to_char("You have selected Praise-Seeker as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Rebel"))
    {
    ch->pcdata->csnature = "Rebel";
    send_to_char("You have selected Rebel as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Rogue"))
    {
    ch->pcdata->csnature = "Rogue";
    send_to_char("You have selected Rogue as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Survivor"))
    {
    ch->pcdata->csnature = "Survivor";
    send_to_char("You have selected Survivor as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Sycophant"))
    {
    ch->pcdata->csnature = "Sycophant";
    send_to_char("You have selected Sycophant as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Traditionalist"))
    {
    ch->pcdata->csnature = "Traditionalist";
    send_to_char("You have selected Traditionalist as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Thrill-Seeker"))
    {
    ch->pcdata->csnature = "Thrill-Seeker";
    send_to_char("You have selected Thrill-Seeker as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Trickster"))
    {
    ch->pcdata->csnature = "Trickster";
    send_to_char("You have selected Trickster as your nature.\n\r",ch);
    return;
    }

    if (!str_prefix(arg,"Visionary"))
    {
    ch->pcdata->csnature = "Visionary";
    send_to_char("You have selected Visonary as your nature.\n\r",ch);
    return;
    }

    send_to_char("What? Are you trying to break Zelan's code?\n\r",ch);

    return;
}


void do_freebie(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int max_unlock = ch->remorts / 10;
    int unlockpoints = max_unlock - ch->unlocksspent;
    int i, count=0, cost=0, step=0, tolearn=0;
    int costcounter;
    costcounter = (ch->pcdata->csbackgrounds[CSBACK_GENERATION])*30;
    bool mentor = FALSE;
	int li = 0;
	int gn = 0;

    argument = one_argument( argument, arg );

    if (IS_NPC(ch)) return;


    if (ch->pcdata->progress < 8)
    {
        sendch("\n\rYou cannot use freebies until you complete the creation process using the 'create'\n\r", ch);
        sendch("command. Please do this now as many of your character's stats affect gameplay and\n\r", ch);
        sendch("leaving them at their default state will negatively impact your playing experience.\n\r", ch);
        return;
    }
    
    if ( arg[0] == '\0')
    {
        send_to_char("The freebie costs are as follows:\n\r",ch);
        send_to_char("New Abilities:           3\n\r",ch);
        send_to_char("New Secondary Abilities: 1\n\r",ch);
//        send_to_char("New Background:          7\n\r",ch);
        send_to_char("Secondary Abilities:     Current Rating*2\n\r",ch);
        send_to_char("Attributes:              Current Rating*4\n\r",ch);
        send_to_char("Abilities:               Current Rating*2\n\r",ch);
        send_to_char("Virtues:                 Current Rating*2\n\r",ch);
//        send_to_char("Backgrounds:             Current Rating*4\n\r",ch);
        send_to_char("Humanity:                Current Rating*2\n\r",ch);
        send_to_char("Willpower:               Current Rating\n\r",ch);
        if(ch->avatar > 0)
        {
        send_to_char("Arete:                   Current Rating*6\n\r",ch);
        send_to_char("Avatar:                  Current Rating*7\n\r",ch);
        send_to_char("Tradition Sphere:        Current Rating*5\n\r",ch);
        send_to_char("Other Sphere:            Current Rating*6\n\r",ch);
        send_to_char("New Sphere:              10\n\r", ch);
		}

        if(ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah")) {
        send_to_char("New Discipline:          10\n\r",ch);
        send_to_char("Clan Discipline:         Current Rating*5\n\r",ch);
        send_to_char("Out Of Clan Discipline:  Current Rating*6\n\r",ch);
        if (ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 0)
        send_to_char("{YGeneration{x:              10\n\r",ch);
        if ((ch->pcdata->csbackgrounds[CSBACK_GENERATION] > 0) && (ch->pcdata->csbackgrounds[CSBACK_GENERATION] < 5))
        {
        sprintf(buf,"Generation:              {Y%d{x\n\r",costcounter);
        send_to_char(buf,ch);
        }    
        send_to_char("Unlock costs:\n\r",ch);
        send_to_char("Combat Discipline:       1 Unlock point\n\r",ch);
        send_to_char("Common Disciplines:      2 Unlock points\n\r",ch);
        send_to_char("Clan Disciplines:        3 Unlock points\n\r",ch);
    }
        if (ch->race == race_lookup("ghoul"))
        {
        send_to_char("New Combat Discipline:   8\n\r",ch);
        send_to_char("New Clan Disc:           10\n\r",ch);
        send_to_char("New Common Disc:         15\n\r", ch);
        send_to_char("Clan/Combat Discipline:  Current Rating*7\n\r",ch);
        send_to_char("Other Discipline:        Current Rating*10\n\r",ch);
    }

        if(( ch->pcdata->rank > 0) && (ch->race == race_lookup("garou")))
        {
            if(ch->pcdata->primal_urge == 0) 
        send_to_char("Primal-Urge              4\n\r",ch);
            else
        send_to_char("Primal-Urge:             Current Rating*2\n\r",ch);
        send_to_char("Rage:                    Current Rating*2\n\r",ch);
        send_to_char("Gnosis:                  Current Rating*2\n\r",ch);
        send_to_char("Renown:                  Current Rating*2 + 10 Temp Renown.\n\r",ch);
        send_to_char("Gifts:                   Rank of Gift * 3\n\r",ch);
        send_to_char("\n\r{WNote:{x Gift name must be enclosed in '' quotes.\n\r", ch);
        send_to_char("      ex: freebie 'pulse of the invisible'\n\r", ch);
        }
        send_to_char("\n\rTo increase a trait type 'freebie <trait>'\n\rExample: freebie animal_ken\n\r",ch);
        sprintf(buf,"You have %d freebies remaining.\n\r",ch->freebie);
        send_to_char(buf,ch);
	if (IS_VAMP(ch)) {
        sprintf(buf,"You have %d unlock points remaining.\n\r",unlockpoints);
        send_to_char(buf,ch);}
        return;
    }
    for(i = 0; i < 30; i++)
    {
        if(!str_prefix(arg, ability_table[i].name))
        {
            if(ch->pcdata->csabilities[i] == 5)
            {
                sprintf(buf,"Your %s is already at max.\n\r",ability_table[i].name);
                send_to_char(buf,ch);
                return;
            }
            if(ch->pcdata->csabilities[i] == 0)
                cost = 3;
            else
                cost = ch->pcdata->csabilities[i]*2;

            if (ch->pcdata->csbackgrounds[CSBACK_MENTOR] > 0) {
            int chance = number_range(1, 100);
            if ((chance <= ch->pcdata->csbackgrounds[CSBACK_MENTOR]*2) && cost != 2)
                {
                    mentor = TRUE;
                    cost--;
                }
            }
            count = i;
            step = 1;
            break;
        }
    }
    for(i = 0; i < 9; i++)
    {
        if(!str_prefix(arg, attribute_table[i].name))
        {
            if(ch->pcdata->csattributes[i] == 5)
            {
                sprintf(buf,"Your %s is already at max.\n\r",capitalize(attribute_table[i].name));
                send_to_char(buf,ch);
                return;
            }
            if(i == APPEARANCE)
            {
                send_to_char("You are unable to raise your appearance.\n\r",ch);
                return;
            }
            cost = ch->pcdata->csattributes[i]*4;
            count = i;
            step = 2;
            break;
        }
    }
	
	
/*    for(i = 0; i < MAX_CSBACK; i++)
    {
        if(!str_prefix(arg, csback_table[i].name))
        {
            if(ch->pcdata->csbackgrounds[i] == 5)
            {
                sprintf(buf,"Your %s is already at max.\n\r",capitalize(csback_table[i].name));
                send_to_char(buf,ch);
                return;
            }
        switch (csback_table[i].race)
        {
            case CSBACK_VAMPIRE:
                if (!IS_VAMP(ch))
                {
                    send_to_char("That is a Kindred background and you are not a vampire!\n\r", ch);
                    return;
                }
                break;
            case CSBACK_WEREWOLF:
                if (ch->race != race_lookup("garou")) {
                    send_to_char("That is a Garou background and you are not a werewolf!\n\r", ch);
                    return;
                }
                break;
            case CSBACK_MAGE:
                if (ch->arete < 1 && ch->avatar < 1) {
                    send_to_char("That is a Mage background and you are not a Mage!\n\r", ch);
                    return;
                }
                break;
            }
            if (!str_prefix(arg, "generation"))
            {
                if (!IS_VAMP(ch)) {
                    send_to_char("You cannot purchase this background.\n\r", ch);
                    return;
                }
                step = 19;
                sh_int minimictime;
                sh_int minimage;
                if (ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 5)
                {
                    send_to_char("Your blood is already as potent as it can be.\n\r", ch);;
                    return;
                }
                count = i;
                switch (ch->pcdata->csbackgrounds[CSBACK_GENERATION]) {
                case 0: minimictime = 30; minimage = 100; cost = 15; break;
                case 1: minimictime = 60; minimage = 150; cost = 30; break;
                case 2: minimictime = 90; minimage = 200; cost = 45; break;
                case 3: minimictime = 120; minimage = 250; cost = 60; break;
                case 4: minimictime = 150; minimage = 300; cost = 75; break; }

                if (get_age(ch) < minimage) {
                    send_to_char("You have not spent enough time on this character to purchase that level of Generation.\n\r", ch);
                    return;
                }
                if ((ch->pcdata->IC_total/60) < minimictime) {
                    send_to_char("You have not spent enough time in the IC affairs on Haven to purchase that level of Generation.\n\r", ch);
            }
            } else {

            if (ch->pcdata->csbackgrounds[i] == 0)
            {
            cost = 7;
            count = i;
            step = 19;
            break;
        } else {
            cost = ch->pcdata->csbackgrounds[i]*4;
            count = i;
            step = 19;
            break;
        }
    }
    }
}*/
    if (!str_prefix(arg, "generation"))
            {
                if (!IS_VAMP(ch)) {
                    send_to_char("You cannot purchase this background.\n\r", ch);
                    return;
                }
                step = 19;
                sh_int minimictime;
                sh_int minimage;
                if ((ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 1) && (ch->remorts < 100))
                {
                   send_to_char("You require at least {Y100{x remorts to lower your generation further.\n\r",ch);
                   return;
                }
                if ((ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 2) && (ch->remorts < 200))
                {
                   send_to_char("You require at least {Y200{x remorts to lower your generation further.\n\r",ch);
                   return;
                }
                if ((ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 3) && (ch->remorts < 300))
                {
                   send_to_char("You require at least {Y300{x remorts to lower your generation further.\n\r",ch);
                   return;
                }
                if ((ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 4) && (ch->remorts < 400))
                {
                   send_to_char("You require at least {Y400{x remorts to lower your generation further.\n\r",ch);
                   return;
                }
                if (ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 5)
                {
                    send_to_char("Your blood is already as potent as it can be.\n\r", ch);;
                    return;
                }
                count = CSBACK_GENERATION;
                switch (ch->pcdata->csbackgrounds[CSBACK_GENERATION]) {
                case 0: minimictime = 0; minimage = 0 ; cost = 10; break;
                case 1: minimictime = 0; minimage = 0; cost = 30; break;
                case 2: minimictime = 0; minimage = 0; cost = 60; break;
                case 3: minimictime = 0; minimage = 0; cost = 90; break;
                case 4: minimictime = 0; minimage = 0; cost = 120; break; }

                if (get_age(ch) < minimage) {
                    send_to_char("You have not spent enough time on this character to purchase that level of Generation.\n\r", ch);
                    return;
                }
                if ((ch->pcdata->IC_total/60) < minimictime) {
                    send_to_char("You have not spent enough time in the IC affairs on Haven to purchase that level of Generation.\n\r", ch);
			return;
            }
    }
                
    if(!str_prefix( arg, "conscience"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] == 5)
        {
            send_to_char("Your Conscience is already at max.\n\r",ch);
            return;
        }
        cost = ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]*2;
        step = 3;
    }
    if(!str_prefix( arg, "self-control") || !str_prefix( arg, "self_control"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] == 5)
        {
            send_to_char("Your Self Control is already at max.\n\r",ch);
            return;
        }
        cost = ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]*2;
        step = 4;
    }
    if(!str_prefix( arg, "courage"))
    {
        if(ch->pcdata->csvirtues[CSVIRT_COURAGE] == 5)
        {
            send_to_char("Your Courage is already at max.\n\r",ch);
            return;
        }
        cost = ch->pcdata->csvirtues[CSVIRT_COURAGE]*2;
        step = 5;
    }
    if(!str_prefix( arg, "willpower"))
    {
        if(ch->pcdata->csmax_willpower == 10)
        {
            send_to_char("Your Willpower is already at max.\n\r",ch);
            return;
        }
        cost = ch->pcdata->csmax_willpower;
        step = 6;
    }
    if(!str_prefix( arg, "humanity"))
    {
        if(ch->pcdata->cshumanity == 10)
        {
            send_to_char("Your Humanity is already at max.\n\r",ch);
            return;
        }
        cost = ch->pcdata->cshumanity*2;
        step = 7;
    }
    if(ch->pcdata->rank > 0)
    {
        if(!str_prefix( arg, "primal-urge"))
        {
            if(ch->pcdata->primal_urge == 5)
            {
                send_to_char("Your Primal Urge is already at max.\n\r",ch);
                return;
            }
            if(ch->pcdata->primal_urge == 0)
                cost = 4;
            else
                cost = ch->pcdata->primal_urge*2;
            step = 8;
        }
        if(!str_prefix( arg, "gnosis"))
        {
            if(ch->pcdata->gnosis[PERM] == 10)
            {
                send_to_char("Your Gnosis is already at max.\n\r",ch);
                return;
            }
            cost = ch->pcdata->gnosis[PERM]*2;
            step = 9;
        }
        if(!str_prefix( arg, "rage"))
        {
            if(ch->pcdata->rage[PERM] == 10)
            {
                send_to_char("Your Rage is already at max.\n\r",ch);
                return;
            }
            cost = ch->pcdata->rage[PERM]*2;
            step = 10;
        }
        if(!str_prefix( arg, "glory"))
        {
            if(ch->pcdata->renown[TEMP_GLORY] < 10)
            {
                send_to_char("Your Temporary Glory is not high enough.\n\r",ch);
                return;
            }
            cost = ch->pcdata->renown[GLORY]*2;
            step = 16;
        }
        if(!str_prefix( arg, "honor"))
        {
            if(ch->pcdata->renown[TEMP_HONOR] < 10)
            {
                send_to_char("Your Temporary Honor is not high enough.\n\r",ch);
                return;
            }
            cost = ch->pcdata->renown[HONOR]*2;
            step = 17;
        }
        if(!str_prefix( arg, "wisdom"))
        {
            if(ch->pcdata->renown[TEMP_WISDOM] < 10)
            {
                send_to_char("Your Temporary Wisdom is not high enough.\n\r",ch);
                return;
            }
            cost = ch->pcdata->renown[WISDOM]*2;
            step = 18;
        }
		    
		for ( gn = 1; gn < MAX_GIFTS_CODED; gn++ )

		{
			if ( !str_cmp( arg, gift_table[gn].name ) )
			{
				for (i = 0; i < MAX_GIFT; i++)
				{
					if (ch->pcdata->gift[i] == gn)
					{
						sendch("You already possess that blessing of Gaia.\n\r", ch);
						return;
					}
				}
					
					
					if(ch->pcdata->rank < gift_table[gn].rank)
					{
						send_to_char("You are not known enough to Gaia and your brothers and sisters to learn this Gift.\n\r",ch);
						return;
					}
					bool pass;
					if(ch->pcdata->breed == gift_table[gn].breed) pass=TRUE;
					if(ch->pcdata->auspice == gift_table[gn].auspice) pass=TRUE;
					if(ch->pcdata->tribe == gift_table[gn].tribe) pass=TRUE;

					if(!pass)
					{
						send_to_char("You were not born into this gift.\n\r",ch);
						return;
					}
				
					int gcount = 0;;
					for (i = 0; i < MAX_GIFT; i++)
					{
						if (ch->pcdata->gift[i] != 0)
						gcount++;
					}
					if (gcount > 15)
					{
						sendch("You can only purchase up to a max of fifteen gifts with freebies.\n\r", ch);
						return;
					}
				
				    bool full = TRUE;
					for (li = 0; li < MAX_GIFT; li++)
					{
						if (ch->pcdata->gift[li] == 0)
						{
							full = FALSE;
							break;
						}
					}
					if (full)
					{
						sendch("You cannot learn any more gifts!\n\r", ch);
						return;
					}
				cost = gift_table[gn].rank * 3;
				step = 20;
			
			break;
			}
		}
		

    }

    if(ch->avatar > 0)
    {
        if(!str_prefix( arg, "arete"))
        {
            if (ch->pcdata->csmax_willpower <= ch->arete)
            {
                sendch("Your magickal potential cannot rise above your willpower.\n\r", ch);
                return;
            }
            
            if(ch->arete == 10)
            {
                send_to_char("Your Arete is already at max.\n\r",ch);
                return;
            }
            cost = ch->arete*6;
            step = 13;
        }


        if ( !str_prefix( arg, "Prime" ) ) tolearn = SPHERE_PRIME;
        else if ( !str_prefix( arg, "Spirit" ) ) tolearn = SPHERE_SPIRIT;
        else if ( !str_prefix( arg, "Time" ) ) tolearn = SPHERE_TIME;
        else if ( !str_prefix( arg, "Mind" ) ) tolearn = SPHERE_MIND;
        else if ( !str_prefix( arg, "Matter" ) ) tolearn = SPHERE_MATTER;
        else if ( !str_prefix( arg, "Life" ) ) tolearn = SPHERE_LIFE;
        else if ( !str_prefix( arg, "Forces" ) ) tolearn = SPHERE_FORCES;
        else if ( !str_prefix( arg, "Correspondence" ) ) tolearn = SPHERE_CORRESPONDENCE;
        else if ( !str_prefix( arg, "Entropy" ) ) tolearn = SPHERE_ENTROPY;

        if(tolearn != 0)
        {
            if(ch->sphere[tolearn] == 5)
            {
                send_to_char("Only the Oracles may progress beyond the 5th rank.\n\r",ch);
                return;
            }

            if(ch->arete < ch->sphere[tolearn]+1)
            {
                send_to_char("You are not yet Enlightened enough to progress further into that Sphere.\n\r",ch);
                return;
            }

            if(tradition_table[ch->tradition].sphere == tolearn) cost = ch->sphere[tolearn]*5;
                else cost = ch->sphere[tolearn]*6;
            if(ch->sphere[tolearn] == 0)
                cost = 10;

            if (ch->pcdata->csbackgrounds[CSBACK_LIBRARY] > 0) {
            int chance = number_range(1, 100);
            if ((chance <= ch->pcdata->csbackgrounds[CSBACK_LIBRARY]*4))
                {
                    mentor = TRUE;
                    cost -= 2;
                }
            }

            step = 14;
        }
        if(!str_prefix( arg, "avatar"))
        {
            if(ch->arete == 5)
            {
                send_to_char("Your Avatar is already as strong as it can be. Perhaps it will gain strength in it's next Incarnation.\n\r",ch);
                return;
            }
            cost = ch->avatar*7;
            step = 15;
        }

    }

    for(i = 0; i < 30; i++)
    {
        if(!str_prefix(arg, sec_abil_table[i].name))
        {
            if(ch->pcdata->cssec_abil[i] == 5)
            {
                sprintf(buf,"Your %s is already at max.\n\r",sec_abil_table[i].name);
                send_to_char(buf,ch);
                return;
            }
            if(sec_abil_table[i].depth == 2)
            {
                if (sec_abil_table[i].req1 != 0)
                {
                    if(ch->pcdata->csabilities[sec_abil_table[i].req1] < 1)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req2 != 0)
                {
                    if(ch->pcdata->csabilities[sec_abil_table[i].req2] < 2)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req3 != 0)
                {
                    if(ch->pcdata->csabilities[sec_abil_table[i].req3] < 3)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req4 != 0)
                {
                    if(ch->pcdata->csabilities[sec_abil_table[i].req4] < 4)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req5 != 0)
                {
                    if(ch->pcdata->csabilities[sec_abil_table[i].req5] < 5)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
            }
            if(sec_abil_table[i].depth == 3)
            {
                if (sec_abil_table[i].req1 != 0)
                {
                    if(ch->pcdata->cssec_abil[sec_abil_table[i].req1] < 1)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req2 != 0)
                {
                    if(ch->pcdata->cssec_abil[sec_abil_table[i].req2] < 2)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req3 != 0)
                {
                    if(ch->pcdata->cssec_abil[sec_abil_table[i].req3] < 3)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req4 != 0)
                {
                    if(ch->pcdata->cssec_abil[sec_abil_table[i].req4] < 4)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
                if (sec_abil_table[i].req5 != 0)
                {
                    if(ch->pcdata->cssec_abil[sec_abil_table[i].req5] < 5)
                    {
                        send_to_char("You don't have the requirements to gain that skill.\n\r",ch);
                        return;
                    }
                }
            }

            if(ch->pcdata->cssec_abil[i] == 0)
                cost = 1;
            else
                cost = ch->pcdata->cssec_abil[i]*2;
            count = i;
            step = 11;
            break;
        }
    }

    int disc, num, unlockneeded, freebiecost;

    for(i = 0; i < MAX_DISC; i++)
    {
        if(!str_prefix(arg, disc_table[i].name))
        {
            if(ch->pcdata->discipline[i] == 0)

            {
                disc = i;

                if (disc == SERPENTIS || disc == PROTEAN || disc == OBTENEBRATION || 
                disc == MORTIS || disc == QUIETUS || disc == THAUMATURGY || 
                disc == VICISSITUDE || disc == CHIMERSTRY || disc == DEMENTATION)
                 {
                     unlockneeded = 3;
                     if (disc == VICISSITUDE)
                     {
                     send_to_char("One does not simply 'learn' Vicissitude.\n\r",ch);
                     return;
                     }

                }
                if (disc == ANIMALISM || disc == AUSPEX || disc == DOMINATE ||
                    disc == OBFUSCATE || disc == PRESENCE)
                {
                    unlockneeded = 2;
                }
                if (disc == CELERITY || disc == POTENCE || disc == FORTITUDE)
                {
                    unlockneeded = 1;
                }


            if (ch->race != race_lookup("ghoul"))
            freebiecost = 10;
            else
            {
                freebiecost = 15;

                for (num = 0; num < 3; num ++)
                {
                    if (i == clandisc_table[ch->clan].clan_disc[num])
                    {
                        freebiecost = 10;
                        break;
                    }
                }
                if (disc == CELERITY || disc == FORTITUDE || disc == POTENCE)
                    freebiecost = 8;
            } // if race == ghoul




            if (ch->freebie < freebiecost)
                {
                    cprintf(ch, "You must have at least %d freebies for this discpline.\n\r", freebiecost );
                            return;
                }
                if (unlockpoints < unlockneeded)
                {
                    cprintf(ch, "You must have at least %d unlock points for this discpline.\n\r", unlockneeded );
                            return;
                }
                ch->pcdata->discipline[i]++;
                ch->unlocksspent += unlockneeded;
                ch->freebie -= freebiecost;
                unlockpoints = max_unlock - ch->unlocksspent;
                cprintf(ch,"You spend %d freebies and use %d unlock points to learn the Discipline of %s.\n\r",freebiecost, unlockneeded, capitalize(disc_table[i].name));
                cprintf(ch, "You have %d unlock points and %d freebies remaining.\n\r",unlockpoints, ch->freebie);
                return;
            }


        if (ch->race == race_lookup("vampire") || ch->race == race_lookup("methuselah"))
        {
            if(ch->pcdata->discipline[i] == 5 && ch->pcdata->csgeneration > 7)
            {
                send_to_char("You must be of 7th generation or lower to achieve this. Please seek a way to better your generation first.\n\r",ch);
                return;
            }
            if(ch->pcdata->discipline[i] == 6)
            {
                send_to_char("Your already at the maximum level available of that Discipline.\n\r",ch);
                return;
            }
            cost = ch->pcdata->discipline[i]*6;
            int num;
            for (num = 0; num < 3; num ++)
            {
                if (i == clandisc_table[ch->clan].clan_disc[num])
                {
                    cost = ch->pcdata->discipline[i]*5;
                    break;
                }
            }

        } else if (ch->race == race_lookup("ghoul"))
        {
            int max;
            int num;
            if (ch->pcdata->csgeneration > 10)
                max = 1;
            if (ch->pcdata->csgeneration <= 10)
                max = 2;
            if (ch->pcdata->csgeneration <= 8)
                max = 3;
            if (ch->pcdata->csgeneration <= 6)
                max = 4;

            if (ch->pcdata->discipline[i] >= max) {
                send_to_char("The vampiric vitae within your veins is not strong enough to learn that discipline.\n\r", ch);
                return;
            }
            if (ch->pblood < 75)
            {
                send_to_char("You do not have enough vampiric vitae within you to learn that discipline.\n\r", ch);
                return;
            }

            if (i == CELERITY || i == FORTITUDE || i == POTENCE)
                cost = ch->pcdata->discipline[i]*7;
            else
                cost = ch->pcdata->discipline[i]*10;

            for (num = 0; num < 3; num ++)
            {
                if (i == clandisc_table[ch->clan].clan_disc[num])
                {
                    cost = ch->pcdata->discipline[i]*7;
                    break;
                }
            }
        }
        count = i;
        step = 12;
        break;
        }
    }

    if(step)
    {
        if(ch->freebie < cost)
        {
            send_to_char("You don't have enough freebies.\n\r",ch);
            return;
        }
        ch->freebie -= cost;
        switch(step)
        {
            case 1:
                    ch->pcdata->csabilities[count]++;
                    if (mentor)
                        send_to_char("You seek out your mentor's aid and wisdom... and it pays off.\n\r", ch);
                    sprintf(buf,"You increase your %s.\n\r",capitalize(ability_table[count].name));
                    send_to_char(buf,ch);
                    break;
            case 2:
                    ch->pcdata->csattributes[count]++;
                    sprintf(buf,"You increase your %s.\n\r",capitalize(attribute_table[count].name));
                    send_to_char(buf,ch);
                    break;
            case 3:
                    ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]++;
                    send_to_char("You increase your Conscience.\n\r",ch);
                    break;
            case 4:
                    ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]++;
                    send_to_char("You increase your Self-Control.\n\r",ch);
                    break;
            case 5:
                    ch->pcdata->csvirtues[CSVIRT_COURAGE]++;
                    send_to_char("You increase your Courage.\n\r",ch);
                    break;
            case 6:
                    ch->pcdata->csmax_willpower++;
                    ch->pcdata->cswillpower++;
                    send_to_char("You increase your Willpower.\n\r",ch);
                    break;
            case 7:
                    ch->pcdata->cshumanity++;
                    send_to_char("You increase your humanity.\n\r",ch);
                    break;
            case 8:
                    ch->pcdata->primal_urge++;
                    send_to_char("You increase your Primal-Urge.\n\r",ch);
                    break;
            case 9:
                    ch->pcdata->gnosis[PERM]++;
                    ch->pcdata->gnosis[TEMP]++;
                    send_to_char("You increase your Gnosis.\n\r",ch);
                    break;
            case 10:
                    ch->pcdata->rage[PERM]++;
                    ch->pcdata->rage[TEMP]++;
                    send_to_char("You increase your Rage.\n\r",ch);
                    break;
            case 11:
                    ch->pcdata->cssec_abil[count]++;
                    sprintf(buf,"You increase your %s.\n\r",capitalize(sec_abil_table[count].name));
                    send_to_char(buf,ch);
                    break;
            case 12:
                    ch->pcdata->discipline[count]++;
                    sprintf(buf,"You increase your %s Discipline.\n\r",capitalize(disc_table[count].name));
                    send_to_char(buf,ch);
                    break;
            case 13:
                    ch->arete++;
                    sprintf(buf, "You sense an ocean of possibilities open up as reality unfolds before your eyes.\n\r");
                    break;
            case 14:
                    ch->sphere[tolearn]++;
                    if (mentor)
                        send_to_char("You search your library for some relevant tomes... And find something promising!\n\r", ch);
                    sprintf(buf,"Your hard work and research pays off... you finally grasp the knowledge of the Sphere.\n\r");
                    break;
            case 15:
                    ch->avatar++;
                    sprintf(buf,"You feel Quintessence flood your body as your Avatar Awakens further.\n\r");
                    break;
            case 16:
                    ch->pcdata->renown[GLORY]++;
                    ch->pcdata->renown[TEMP_GLORY] = 0;
                    send_to_char("Your Glory has increased.\n\r",ch);
                    break;
            case 17:
                    ch->pcdata->renown[HONOR]++;
                    ch->pcdata->renown[TEMP_HONOR] = 0;
                    send_to_char("Your Honor has increased.\n\r",ch);
                    break;
            case 18:
                    ch->pcdata->renown[WISDOM]++;
                    ch->pcdata->renown[TEMP_WISDOM] = 0;
                    send_to_char("Your Wisdom has increased.\n\r",ch);
                    break;
            case 19:
                    ch->pcdata->csbackgrounds[count]++;
                    if (count == CSBACK_GENERATION)
                    {
                        ch->gen--;
                        ch->pcdata->csgeneration = ch->gen;
                        if (ch->max_pblood+10 <= 200)
                          ch->max_pblood += 10;
                    }
                    if (count == CSBACK_FOUNT)
                        ch->max_quintessence += 10;
                    sprintf(buf,"You increase your %s.\n\r",capitalize(csback_table[count].name));
                    send_to_char(buf,ch);
                    break;
			case 20:
				ch->pcdata->gift[li] = gn;
				cprintf(ch, "The spirits bless you with the gift of '%s'.\n\r", capitalize(gift_table[gn].name));
				break;
				
            default:
                    send_to_char("Error! Contact an immortal!\n\r",ch);
                    break;
        }
    }
    else
    {
        send_to_char("That is not a valid choice.\n\r",ch);
        return;
    }
    sprintf(buf,"You have %d freebies remaining.\n\r",ch->freebie);
    cskill_update(ch);
    send_to_char(buf,ch);
    return;
}


void do_create( CHAR_DATA *ch, char *argument )
{
    char buf [MAX_STRING_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    int section = 0;
    int stat = 0;
    bool backgrounds;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if (IS_NPC(ch)) return;


    if(arg1[0] == '\0')
    {
        if(ch->pcdata->progress <= 1)
        {
            send_to_char("Because of the fact that Haven is created using a ROM codebase it uses standard\n\r",ch);
            send_to_char("ROM stats for such things as combat and skills. This is fine for the basics.. but\n\r",ch);
            send_to_char("it discourages roleplay and prevents us from adding some World of Darkness abilities.\n\r",ch);
            send_to_char("The solution to this is to have another set of stats or attributes based on the\n\r",ch);
            send_to_char("WoD stat system. That is the purpose of this command.\n\r\n\r",ch);

            send_to_char("The first field of stats to be created will be your attributes.\n\r",ch);
            send_to_char("The attributes are divided into three sections, physical, social and mental. You choose\n\r",ch);
            send_to_char("which of these sections is more important during the following steps, and are given\n\r",ch);
            send_to_char("points to distribute among these stats.\n\r\n\r",ch);

            send_to_char("Please choose which is most important to your character, which is your primary stat\n\r",ch);
            send_to_char("section. Do this by typing 'create primary <section>' to choose your primary then do \n\r",ch);
            send_to_char("same for secondary and tertiary.\n\r",ch);
            send_to_char("Type one of the following:\n\r create primary physical\n\r create primary social\n\r create primary mental\n\r",ch);
            if(ch->pcdata->progress == 0) ch->pcdata->progress++;
        }
        else if(ch->pcdata->progress == 2 || ch->pcdata->progress == 3)
        {
            send_to_char("The next step is setting up your abilities. Abilities are numeric values for what your\n\r",ch);
            send_to_char("character knows and can learn.\n\r\n\r",ch);

            send_to_char("Abilities are devided into three sections:\n\r",ch);
            send_to_char("Talents, which are what you intuitively know naturally without instruction,\n\r",ch);
            send_to_char("Skills which are abilities learned through training, apprenticeship or other instruction.\n\r",ch);
            send_to_char("Knowledges are the traits that you gained through schooling or book-learning.\n\r\n\r",ch);
            send_to_char("You must choose your Primary, Secondary and Tertiary abilities to continue.\n\r",ch);
            send_to_char("Please assign your Primary by typing 'create primary <section>' then do the same for\n\r",ch);
            send_to_char("Secondary and Tertiary.\n\r",ch);
            send_to_char("Type one of the following:\n\r create primary talents\n\r create primary skills\n\r create primary knowledges\n\r",ch);
            if(ch->pcdata->progress == 2) ch->pcdata->progress++;
        }
        else if(ch->pcdata->progress == 4 || ch->pcdata->progress == 5 || ch->pcdata->progress == 6)
        {
            send_to_char("Your almost finished with this process. Next is setting your Virtues.\n\r\n\r",ch);
            send_to_char("Virtues are the moral guidelines for your character. What your character feels about\n\r",ch);
            send_to_char("him/herself and the world around them.\n\r\n\r",ch);
            send_to_char("There are three Virtues:\n\r",ch);
            send_to_char("Conscience is your ability to tell what's 'right' from what's 'wrong'.\n\r",ch);
            send_to_char("Self_Control defines your character's discipline over their inner Beast and urges.\n\r",ch);
            send_to_char("Courage allows your character to stand before a threat without cowering.\n\r\n\r",ch);
            send_to_char("Like Attributes, you receive one free point in each Virtue. In addition you may spend\n\r",ch);
            send_to_char("seven points on them in any way you choose. Note that once you finish spending your\n\r",ch);
            send_to_char("points you must type 'create done' to finalize them.\n\r",ch);
            send_to_char("You may distribute your points now by typing 'create <virtue> +/-. Example: create self_control +\n\r",ch);
            if(ch->pcdata->progress == 5)
            {
                ch->pcdata->progress++;
                ch->pcdata->primary = 7;
            }
        }
/*      else if(ch->pcdata->progress == 7) do_function(ch, &do_create, "done" ); */
        else
            send_to_char("Nothing new to set on your stats. Sorry.\n\r",ch);

        return;
    }
    if(arg1[0] == '\0' || ch->pcdata->progress >= 8)
    {
        send_to_char("Nothing new to set on your stats. Sorry.\n\r",ch);
        return;
    }
    if(strlen(arg1) < 3)
    {
        send_to_char("You must type out at least three letters of each option.\n\r",ch);
        return;
    }

    if(!str_cmp(arg1, "primary") || !str_cmp(arg1, "secondary") || !str_cmp(arg1, "tertiary"))
    {
        if(arg2[0] == '\0' && ch->pcdata->progress == 1)
        {
            send_to_char("The sentax is create <primary/secondary/tertiary> <physical/social/mental>\n\r",ch);
            return;
        }
        if(arg2[0] == '\0' && ch->pcdata->progress == 3)
        {
            send_to_char("The sentax is create <primary/secondary/tertiary> <skills/talents/knowledges>\n\r",ch);
            return;
        }
        if(ch->pcdata->progress == 1 && ch->pcdata->stat[PHYSICAL] > 0 && ch->pcdata->stat[SOCIAL] > 0 && ch->pcdata->stat[MENTAL] > 0)
        {
            send_to_char("You have already assigned priorities for your attributes. Please continue by distributing\n\r",ch);
            send_to_char("your attribute points.\n\r",ch);
            return;
        }
        if(ch->pcdata->progress == 3 && ch->pcdata->stat[SKILLS] > 0 && ch->pcdata->stat[TALENTS] > 0 && ch->pcdata->stat[KNOWLEDGES] > 0)
        {
            send_to_char("You have already assigned priorities for your abilities. Please continue by distributing\n\r",ch);
            send_to_char("your ability points.\n\r",ch);
            return;
        }

        if(!str_cmp(arg1,"primary"))
            section = PRIMARY;
        if(!str_cmp(arg1,"secondary"))
            section = SECONDARY;
        if(!str_cmp(arg1,"tertiary"))
            section = TERTIARY;
        if(ch->pcdata->progress == 1)
        {
            if(!str_cmp(arg2, "physical"))
                stat = PHYSICAL;
            else if(!str_cmp(arg2, "social"))
                stat = SOCIAL;
            else if(!str_cmp(arg2, "mental"))
                stat = MENTAL;
            else
            {
                send_to_char("Please choose Physical, Social or Mental.\n\r",ch);
                return;
            }
        }
        else if(ch->pcdata->progress == 3)
        {
            if(!str_cmp(arg2, "skills"))
                stat = SKILLS;
            else if(!str_cmp(arg2, "talents"))
                stat = TALENTS;
            else if(!str_cmp(arg2, "knowledges"))
                stat = KNOWLEDGES;
            else
            {
                send_to_char("Please choose Skills, Talents or Knowledges.\n\r",ch);
                return;
            }
        }
        else
        {
            send_to_char("Invalid choice.\n\r",ch);
            return;
        }

        if(ch->pcdata->stat[stat] == PRIMARY || ch->pcdata->stat[stat] == SECONDARY || ch->pcdata->stat[stat] == TERTIARY ||
        (ch->pcdata->progress == 1 && (ch->pcdata->stat[PHYSICAL] == section || ch->pcdata->stat[SOCIAL] == section || ch->pcdata->stat[MENTAL] == section)) ||
        (ch->pcdata->progress == 3 && (ch->pcdata->stat[SKILLS] == section || ch->pcdata->stat[TALENTS] == section || ch->pcdata->stat[KNOWLEDGES] == section)))
        {
            send_to_char("You have already assigned a priority for that trait. I'll now reset your priorities so\n\r",ch);
            send_to_char("you may start over.",ch);
            if(ch->pcdata->progress == 1)
            {
                send_to_char("Please type create <primary/secondary/tertiary> <physical/social/mental>\n\r",ch);
                ch->pcdata->stat[PHYSICAL] = 0;
                ch->pcdata->stat[SOCIAL] = 0;
                ch->pcdata->stat[MENTAL] = 0;
            }
            else if(ch->pcdata->progress == 3)
            {
                send_to_char("Please type create <primary/secondary/tertiary> <skills/talents/knowledges>\n\r",ch);
                ch->pcdata->stat[SKILLS] = 0;
                ch->pcdata->stat[TALENTS] = 0;
                ch->pcdata->stat[KNOWLEDGES] = 0;
            }
            return;
        }
        if(ch->pcdata->progress == 1 && ch->clan == clan_lookup("nosferatu") && stat == SOCIAL && section == PRIMARY)
        {
            send_to_char("Do me a favor will ya? Take a look in the mirror. Do you REALLY think Social is your primary\n\r",ch);
            send_to_char("attribute set? Please pick either Mental or Physical as your primary.\n\r",ch);
            return;
        }

        ch->pcdata->stat[stat] = section;

        sprintf(buf,"You have chosen %s as your %s.\n\r", stat == PHYSICAL ? "Physical" : stat == SOCIAL ? "Social" : "Mental",
                section == PRIMARY ? "Primary" : section == SECONDARY ? "Secondary" : "Tertiary");

        if(ch->pcdata->progress == 3)
            sprintf(buf, "You have chosen %s as your %s.\n\r", stat == SKILLS ? "Skills" : stat == TALENTS ? "Talents" : "Knowledges",
                section == PRIMARY ? "Primary" : section == SECONDARY ? "Secondary" : "Tertiary");
        send_to_char(buf,ch);

        if(ch->pcdata->progress == 1 && ch->pcdata->stat[PHYSICAL] > 0 && ch->pcdata->stat[SOCIAL] > 0 && ch->pcdata->stat[MENTAL] > 0)
        {
            send_to_char("You have finished assigning priorities. Your next step is to assign points to these attributes.\n\r\n\r",ch);
            send_to_char("You may do this by typing create <attribute> +/-. Example: create strength +\n\r",ch);
            send_to_char("For more information on each of the attributes please type help <attribute>. Example: help strength\n\r\n\r",ch);
            send_to_char("You receive one 'free' point in each attribute. You can then use points to increase these attributes\n\r",ch);
            send_to_char("up to the 4th 'dot'. Note that once you finish this process you must type: create done.\n\r",ch);
            send_to_char("You have 7 Primary points, 5 Secondary points and 3 Tertiary points. You must use them all before\n\r",ch);
            send_to_char("finishing this process.\n\r",ch);
            send_to_char("You have the following remaining points: Primary: 7, Secondary: 5, Tertiary: 3\n\r\n\r",ch);
            ch->pcdata->primary = 7;
            ch->pcdata->secondary = 5;
            ch->pcdata->tertiary = 3;
            do_function(ch, &do_create, "showattr" );
        }
        if(ch->pcdata->progress == 3 && ch->pcdata->stat[SKILLS] > 0 && ch->pcdata->stat[TALENTS] > 0 && ch->pcdata->stat[KNOWLEDGES] > 0)
        {
            send_to_char("You have finished assigning priorities. Your next step is to assign points to these abilities.\n\r\n\r",ch);
            send_to_char("You may do this by typing create <ability> +/-. For example: create brawl +\n\r",ch);
            send_to_char("For more information on each of these abilities please type help <ability>. Example: help brawl\n\r\n\r",ch);
            send_to_char("Please note that all abilities 'cap' at 4th dot. To finish this process you must type: create done.\n\r",ch);
            send_to_char("You have the following remaining points: Primary: 13, Secondary: 9, Tertiary: 5\n\r\n\r",ch);
            ch->pcdata->primary = 13;
            ch->pcdata->secondary = 9;
            ch->pcdata->tertiary = 5;
            ch->pcdata->progress++;
            do_function(ch, &do_create, "showabil");
        }
        return;
    }

    if(ch->pcdata->progress == 1 && (ch->pcdata->stat[PHYSICAL] == 0 || ch->pcdata->stat[SOCIAL] == 0 || ch->pcdata->stat[MENTAL] == 0))
    {
        send_to_char("You must assign your attribute priorities before continuing.\n\r",ch);
        return;
    }

    if(!str_cmp(arg1,"showabil"))
    {
        send_to_char(" |<=--------------------------=Abilities=---------------------------=>|\n\r",ch);
        send_to_char(" |                                                                    |\n\r",ch);
        send_to_char(" |",ch);
        sprintf(buf,"TALENTS(%d)",
            ch->pcdata->stat[TALENTS] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[TALENTS] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary);
        sprintf(buf2,"%s",center(buf,22," "));
        send_to_char(buf2,ch);
        sprintf(buf,"SKILLS(%d)",
            ch->pcdata->stat[SKILLS] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[SKILLS] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary);
        sprintf(buf2,"%s",center(buf,23," "));
        send_to_char(buf2,ch);
        sprintf(buf,"KNOWLEDGES(%d)",
            ch->pcdata->stat[KNOWLEDGES] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[KNOWLEDGES] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary);
        sprintf(buf2,"%s",center(buf,23," "));
        send_to_char(buf2,ch);
        send_to_char("|\n\r",ch);
        sprintf(buf, " | Expression_____%s   Animal Ken_____%s   Academics______%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_EXPRESSION], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_ANIMAL_KEN], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_ACADEMICS], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Alertness______%s   Archery________%s   Seneschal______%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_ALERTNESS], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_ARCHERY], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_SENESCHAL], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Athletics______%s   Crafts_________%s   Investigation__%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_ATHLETICS], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_CRAFTS], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_INVESTIGATION], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Brawl__________%s   Etiquette______%s   Law____________%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_BRAWL], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_ETIQUETTE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_LAW], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Dodge__________%s   Melee__________%s   Linguistics____%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_DODGE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_MELEE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_LINGUISTICS], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Empathy________%s   Performance____%s   Medicine_______%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_EMPATHY], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_PERFORMANCE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_MEDICINE], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Intimidation___%s   Riding_________%s   Occult_________%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_INTIMIDATION], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_RIDING], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_OCCULT], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Leadership_____%s   Commerce_______%s   Politics_______%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_LEADERSHIP], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_COMMERCE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_POLITICS], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Legerdemain____%s   Stealth________%s   Hearth Wisdom__%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_LEGERDEMAIN], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_STEALTH], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_HEARTHWISDOM], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Subterfuge_____%s   Survival_______%s   Theology_______%s |\n\r",
        dots(ch->pcdata->csabilities[CSABIL_SUBTERFUGE], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_SURVIVAL], FALSE),
        dots(ch->pcdata->csabilities[CSABIL_THEOLOGY], FALSE));
        send_to_char(buf,ch);
        send_to_char(" |<=----------------------------------------------------------------=>|\n\r",ch);
        return;
    }

    if(!str_cmp(arg1,"showattr"))
    {
        send_to_char(" |<=-------------------------=Attributes=---------------------------=>|\n\r",ch);
        send_to_char(" |                                                                    |\n\r",ch);
        sprintf(buf, " |      PHYSICAL(%d)             SOCIAL(%d)               MENTAL(%d)     |\n\r",
            ch->pcdata->stat[PHYSICAL] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[PHYSICAL] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary,
            ch->pcdata->stat[SOCIAL] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[SOCIAL] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary,
            ch->pcdata->stat[MENTAL] == PRIMARY ? ch->pcdata->primary : ch->pcdata->stat[MENTAL] == SECONDARY ? ch->pcdata->secondary : ch->pcdata->tertiary);
        send_to_char(buf,ch);
        sprintf(buf, " | Strength_______%s   Charisma_______%s   Perception_____%s |\n\r",
            dots(ch->pcdata->csattributes[CSATTRIB_STRENGTH], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_CHARISMA], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_PERCEPTION], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Dexterity______%s   Manipulation___%s   Intelligence___%s |\n\r",
            dots(ch->pcdata->csattributes[CSATTRIB_DEXTERITY], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_MANIPULATION], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_INTELLIGENCE], FALSE));
        send_to_char(buf,ch);
        sprintf(buf, " | Stamina________%s   Appearance_____%s   Wits___________%s |\n\r",
            dots(ch->pcdata->csattributes[CSATTRIB_STAMINA], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_APPEARANCE], FALSE),
            dots(ch->pcdata->csattributes[CSATTRIB_WITS], FALSE));
        send_to_char(buf,ch);
        send_to_char(" |<=----------------------------------------------------------------=>|\n\r",ch);
        return;
    }

    if(!str_cmp(arg1,"showvirtue"))
    {
        send_to_char(" |<=-------------------------=Advantages=---------------------------=>|\n\r",ch);
        send_to_char(" |                                                                    |\n\r",ch);
        sprintf(buf, " |                              VIRTUES(%d)                            |\n\r",ch->pcdata->primary);
        send_to_char(buf,ch);
        sprintf(buf, " | Conscience_____%s   Self_Control___%s   Courage________%s |\n\r",
            dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE),
            dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE),
            dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
        send_to_char(buf,ch);
        send_to_char(" |                                                                    |\n\r",ch);
        send_to_char(" |<=----------------------------------------------------------------=>|\n\r",ch);
        return;
    }


    if(!str_prefix(arg1, "expression") || !str_prefix(arg1, "alertness") || !str_prefix(arg1, "athletics") ||
        !str_prefix(arg1, "brawl") || !str_prefix(arg1, "dodge") || !str_prefix(arg1, "empathy") ||
        !str_prefix(arg1, "intimidation") || !str_prefix(arg1, "leadership") || !str_prefix(arg1, "legerdemain") ||
        !str_prefix(arg1, "subterfuge"))
        stat = TALENTS;
    else if(!str_prefix(arg1, "animal_ken") || !str_prefix(arg1, "riding") || !str_prefix(arg1, "etiquette") ||
        !str_prefix(arg1, "archery") || !str_prefix(arg1, "melee") || !str_prefix(arg1, "performance") ||
        !str_prefix(arg1, "crafts") || !str_prefix(arg1, "commerce") || !str_prefix(arg1, "stealth") ||
        !str_prefix(arg1, "survival"))
        stat = SKILLS;
    else if(!str_prefix(arg1, "academics") || !str_prefix(arg1, "theology") || !str_prefix(arg1, "seneschal") ||
        !str_prefix(arg1, "investigation") || !str_prefix(arg1, "law") || !str_prefix(arg1, "linguistics") ||
        !str_prefix(arg1, "medicine") || !str_prefix(arg1, "occult") || !str_prefix(arg1, "politics") ||
        !str_prefix(arg1, "hearth_wisdom"))
        stat = KNOWLEDGES;
    if(!str_prefix(arg1, "strength") || !str_prefix(arg1, "dexterity") || !str_prefix(arg1, "stamina"))
        stat = PHYSICAL;
    else if(!str_prefix(arg1, "charisma") || !str_prefix(arg1, "manipulation") || !str_prefix(arg1, "appearance"))
        stat = SOCIAL;
    else if(!str_prefix(arg1, "perception") || !str_prefix(arg1, "intelligence") || !str_prefix(arg1, "wits"))
        stat = MENTAL;

    if(arg2[0] != '\0' && !str_prefix(arg2,"+"))
    {

        if((ch->pcdata->stat[stat] == PRIMARY && ch->pcdata->primary == 0) ||
            (ch->pcdata->stat[stat] == SECONDARY && ch->pcdata->secondary == 0) ||
            (ch->pcdata->stat[stat] == TERTIARY && ch->pcdata->tertiary == 0))
        {
            send_to_char("You don't have enough points.\n\r",ch);
            return;
        }
        if(ch->pcdata->progress == 6 && ch->pcdata->primary <= 0)
        {
            send_to_char("You don't have enough points.\n\r",ch);
            return;
        }
        if(ch->pcdata->progress == 1)
        {
            if(!str_prefix(arg1,"strength"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_STRENGTH] >= 4)
                {
                    send_to_char("Your Strength is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_STRENGTH]++;
            }
            else if(!str_prefix(arg1,"dexterity"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_DEXTERITY] >= 4)
                {
                    send_to_char("Your Dexterity is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_DEXTERITY]++;
            }
            else if(!str_prefix(arg1,"stamina"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_STAMINA] >= 4)
                {
                    send_to_char("Your Stamina is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_STAMINA]++;
            }
            else if(!str_prefix(arg1,"charisma"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_CHARISMA] >= 4)
                {
                    send_to_char("Your Charisma is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_CHARISMA]++;
            }
            else if(!str_prefix(arg1,"manipulation"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_MANIPULATION] >= 4)
                {
                    send_to_char("Your Manipulation is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_MANIPULATION]++;
            }
            else if(!str_prefix(arg1,"appearance"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_APPEARANCE] >= 4)
                {
                    send_to_char("Your Appearance is already at maximum.\n\r",ch);
                    return;
                }
                if(ch->clan == clan_lookup("nosferatu"))
                {
                    send_to_char("You can't increase your Appearance. Your just too darned ugly.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_APPEARANCE]++;
            }
            else if(!str_prefix(arg1,"perception"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_PERCEPTION] >= 4)
                {
                    send_to_char("Your Perception is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_PERCEPTION]++;
            }
            else if(!str_prefix(arg1,"intelligence"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_INTELLIGENCE] >= 4)
                {
                    send_to_char("Your Intelligence is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_INTELLIGENCE]++;
            }
            else if(!str_prefix(arg1,"wits"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_WITS] >= 4)
                {
                    send_to_char("Your Wits is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_WITS]++;
            }
        }
        else if (ch->pcdata->progress == 4)
        {
            if(!str_prefix(arg1,"expression"))
            {
                if(ch->pcdata->csabilities[CSABIL_EXPRESSION] >= 4)
                {
                    send_to_char("Your Expression is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_EXPRESSION]++;
            }
            else if(!str_prefix(arg1,"alertness"))
            {
                if(ch->pcdata->csabilities[CSABIL_ALERTNESS] >= 4)
                {
                    send_to_char("Your Alertness is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ALERTNESS]++;
            }
            else if(!str_prefix(arg1,"athletics"))
            {
                if(ch->pcdata->csabilities[CSABIL_ATHLETICS] >= 4)
                {
                    send_to_char("Your Athletics is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ATHLETICS]++;
            }
            else if(!str_prefix(arg1,"brawl"))
            {
                if(ch->pcdata->csabilities[CSABIL_BRAWL] >= 4)
                {
                    send_to_char("Your Brawl is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_BRAWL]++;
            }
            else if(!str_prefix(arg1,"dodge"))
            {
                if(ch->pcdata->csabilities[CSABIL_DODGE] >= 4)
                {
                    send_to_char("Your Dodge is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_DODGE]++;
            }
            else if(!str_prefix(arg1,"empathy"))
            {
                if(ch->pcdata->csabilities[CSABIL_EMPATHY] >= 4)
                {
                    send_to_char("Your Empathy is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_EMPATHY]++;
            }
            else if(!str_prefix(arg1,"intimidation"))
            {
                if(ch->pcdata->csabilities[CSABIL_INTIMIDATION] >= 4)
                {
                    send_to_char("Your Intimidation is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_INTIMIDATION]++;
            }
            else if(!str_prefix(arg1,"leadership"))
            {
                if(ch->pcdata->csabilities[CSABIL_LEADERSHIP] >= 4)
                {
                    send_to_char("Your Leadership is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LEADERSHIP]++;
            }
            else if(!str_prefix(arg1,"legerdemain"))
            {
                if(ch->pcdata->csabilities[CSABIL_LEGERDEMAIN] >= 4)
                {
                    send_to_char("Your Legerdemain is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LEGERDEMAIN]++;
            }
            else if(!str_prefix(arg1,"subterfuge"))
            {
                if(ch->pcdata->csabilities[CSABIL_SUBTERFUGE] >= 4)
                {
                    send_to_char("Your Subterfuge is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SUBTERFUGE]++;
            }
            else if(!str_prefix(arg1,"animal_ken"))
            {
                if(ch->pcdata->csabilities[CSABIL_ANIMAL_KEN] >= 4)
                {
                    send_to_char("Your Animal_Ken is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ANIMAL_KEN]++;
            }
            else if(!str_prefix(arg1,"riding"))
            {
                if(ch->pcdata->csabilities[CSABIL_RIDING] >= 4)
                {
                    send_to_char("Your Riding is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_RIDING]++;
            }
            else if(!str_prefix(arg1,"etiquette"))
            {
                if(ch->pcdata->csabilities[CSABIL_ETIQUETTE] >= 4)
                {
                    send_to_char("Your Etiquette is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ETIQUETTE]++;
            }
            else if(!str_prefix(arg1,"archery"))
            {
                if(ch->pcdata->csabilities[CSABIL_ARCHERY] >= 4)
                {
                    send_to_char("Your Archery is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ARCHERY]++;
            }
            else if(!str_prefix(arg1,"melee"))
            {
                if(ch->pcdata->csabilities[CSABIL_MELEE] >= 4)
                {
                    send_to_char("Your Melee is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_MELEE]++;
            }
            else if(!str_prefix(arg1,"performance"))
            {
                if(ch->pcdata->csabilities[CSABIL_PERFORMANCE] >= 4)
                {
                    send_to_char("Your Performance is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_PERFORMANCE]++;
            }
            else if(!str_prefix(arg1,"crafts"))
            {
                if(ch->pcdata->csabilities[CSABIL_CRAFTS] >= 4)
                {
                    send_to_char("Your Crafts is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_CRAFTS]++;
            }
            else if(!str_prefix(arg1,"commerce"))
            {
                if(ch->pcdata->csabilities[CSABIL_COMMERCE] >= 4)
                {
                    send_to_char("Your Commerce is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_COMMERCE]++;
            }
            else if(!str_prefix(arg1,"stealth"))
            {
                if(ch->pcdata->csabilities[CSABIL_STEALTH] >= 4)
                {
                    send_to_char("Your Stealth is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_STEALTH]++;
            }
            else if(!str_prefix(arg1,"survival"))
            {
                if(ch->pcdata->csabilities[CSABIL_SURVIVAL] >= 4)
                {
                    send_to_char("Your Survival is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SURVIVAL]++;
            }
            else if(!str_prefix(arg1,"academics"))
            {
                if(ch->pcdata->csabilities[CSABIL_ACADEMICS] >= 4)
                {
                    send_to_char("Your Academics is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ACADEMICS]++;
            }
            else if(!str_prefix(arg1,"theology"))
            {
                if(ch->pcdata->csabilities[CSABIL_THEOLOGY] >= 4)
                {
                    send_to_char("Your Theology is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_THEOLOGY]++;
            }
            else if(!str_prefix(arg1,"seneschal"))
            {
                if(ch->pcdata->csabilities[CSABIL_SENESCHAL] >= 4)
                {
                    send_to_char("Your Seneschal is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SENESCHAL]++;
            }
            else if(!str_prefix(arg1,"investigation"))
            {
                if(ch->pcdata->csabilities[CSABIL_INVESTIGATION] >= 4)
                {
                    send_to_char("Your Investigation is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_INVESTIGATION]++;
            }
            else if(!str_prefix(arg1,"law"))
            {
                if(ch->pcdata->csabilities[CSABIL_LAW] >= 4)
                {
                    send_to_char("Your Law is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LAW]++;
            }
            else if(!str_prefix(arg1,"linguistics"))
            {
                if(ch->pcdata->csabilities[CSABIL_LINGUISTICS] >= 4)
                {
                    send_to_char("Your Linguistics is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LINGUISTICS]++;
            }
            else if(!str_prefix(arg1,"medicine"))
            {
                if(ch->pcdata->csabilities[CSABIL_MEDICINE] >= 4)
                {
                    send_to_char("Your Medicine is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_MEDICINE]++;
            }
            else if(!str_prefix(arg1,"occult"))
            {
                if(ch->pcdata->csabilities[CSABIL_OCCULT] >= 4)
                {
                    send_to_char("Your Occult is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_OCCULT]++;
            }
            else if(!str_prefix(arg1,"politics"))
            {
                if(ch->pcdata->csabilities[CSABIL_POLITICS] >= 4)
                {
                    send_to_char("Your Politics is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_POLITICS]++;
            }
            else if(!str_prefix(arg1,"hearth_wisdom"))
            {
                if(ch->pcdata->csabilities[CSABIL_HEARTHWISDOM] >= 4)
                {
                    send_to_char("Your Hearth_Wisdom is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_HEARTHWISDOM]++;
            }
        }
        else if (ch->pcdata->progress == 6)
        {
            if(!str_prefix(arg1,"conscience"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] == 5)
                {
                    send_to_char("Your Conscience is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]++;
                ch->pcdata->primary--;
            }
            else if(!str_prefix(arg1,"self_control") || !str_prefix(arg1,"self-control"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] == 5)
                {
                    send_to_char("Your Self-Control is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]++;
                ch->pcdata->primary--;
            }
            else if(!str_prefix(arg1,"courage"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_COURAGE] == 5)
                {
                    send_to_char("Your Courage is already at maximum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_COURAGE]++;
                ch->pcdata->primary--;
            }
            else
            {
                send_to_char("Please choose Conscience, Self-Control or Courage.\n\r",ch);
                return;
            }

        } else if (ch->pcdata->progress == 8) {
        return;
        }
        else
        {
            send_to_char("That is not a valid choice.\n\r",ch);
            return;
        }
        if(ch->pcdata->progress != 5)
        {
            if(ch->pcdata->stat[stat] == PRIMARY) ch->pcdata->primary--;
            if(ch->pcdata->stat[stat] == SECONDARY) ch->pcdata->secondary--;
            if(ch->pcdata->stat[stat] == TERTIARY) ch->pcdata->tertiary--;
        }
        if(ch->pcdata->progress == 1) do_function(ch, &do_create, "showattr" );
        if(ch->pcdata->progress == 4) do_function(ch, &do_create, "showabil" );
        if(ch->pcdata->progress == 6) do_function(ch, &do_create, "showvirtue");
        if(ch->pcdata->primary == 0 && ch->pcdata->secondary == 0 && ch->pcdata->tertiary == 0)
            send_to_char("If you've finished setting your traits, please type 'create done' to finalize your choices.\n\r",ch);
        return;
    }

    if(arg2[0] != '\0' && !str_prefix(arg2,"-"))
    {
        if(ch->pcdata->progress == 1)
        {
            if(!str_prefix(arg1,"strength"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_STRENGTH] == 1)
                {
                    send_to_char("Your Strength is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_STRENGTH]--;
            }
            else if(!str_prefix(arg1,"dexterity"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_DEXTERITY] == 1)
                {
                    send_to_char("Your Dexterity is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_DEXTERITY]--;
            }
            else if(!str_prefix(arg1,"stamina"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_STAMINA] == 1)
                {
                    send_to_char("Your Stamina is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_STAMINA]--;
            }
            else if(!str_prefix(arg1,"charisma"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_CHARISMA] == 1)
                {
                    send_to_char("Your Charisma is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_CHARISMA]--;
            }
            else if(!str_prefix(arg1,"manipulation"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_MANIPULATION] == 1)
                {
                    send_to_char("Your Manipulation is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_MANIPULATION]--;
            }
            else if(!str_prefix(arg1,"appearance"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_APPEARANCE] <= 1)
                {
                    send_to_char("Your Appearance is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_APPEARANCE]--;
            }
            else if(!str_prefix(arg1,"perception"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_PERCEPTION] == 1)
                {
                    send_to_char("Your Perception is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_PERCEPTION]--;
            }
            else if(!str_prefix(arg1,"intelligence"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_INTELLIGENCE] == 1)
                {
                    send_to_char("Your Intelligence is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_INTELLIGENCE]--;
            }
            else if(!str_prefix(arg1,"wits"))
            {
                if(ch->pcdata->csattributes[CSATTRIB_WITS] == 1)
                {
                    send_to_char("Your Wits is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csattributes[CSATTRIB_WITS]--;
            }
        }
        else if(ch->pcdata->progress == 4)
        {
            if(!str_prefix(arg1,"expression"))
            {
                if(ch->pcdata->csabilities[CSABIL_EXPRESSION] == 0)
                {
                    send_to_char("Your Expression is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_EXPRESSION]--;
            }
            else if(!str_prefix(arg1,"alertness"))
            {
                if(ch->pcdata->csabilities[CSABIL_ALERTNESS] == 0)
                {
                    send_to_char("Your Alertness is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ALERTNESS]--;
            }
            else if(!str_prefix(arg1,"athletics"))
            {
                if(ch->pcdata->csabilities[CSABIL_ATHLETICS] == 0)
                {
                    send_to_char("Your Athletics is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ATHLETICS]--;
            }
            else if(!str_prefix(arg1,"brawl"))
            {
                if(ch->pcdata->csabilities[CSABIL_BRAWL] == 0)
                {
                    send_to_char("Your Brawl is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_BRAWL]--;
            }
            else if(!str_prefix(arg1,"dodge"))
            {
                if(ch->pcdata->csabilities[CSABIL_DODGE] == 0)
                {
                    send_to_char("Your Dodge is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_DODGE]--;
            }
            else if(!str_prefix(arg1,"empathy"))
            {
                if(ch->pcdata->csabilities[CSABIL_EMPATHY] == 0)
                {
                    send_to_char("Your Empathy is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_EMPATHY]--;
            }
            else if(!str_prefix(arg1,"intimidation"))
            {
                if(ch->pcdata->csabilities[CSABIL_INTIMIDATION] == 0)
                {
                    send_to_char("Your Intimidation is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_INTIMIDATION]--;
            }
            else if(!str_prefix(arg1,"leadership"))
            {
                if(ch->pcdata->csabilities[CSABIL_LEADERSHIP] == 0)
                {
                    send_to_char("Your Leadership is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LEADERSHIP]--;
            }
            else if(!str_prefix(arg1,"legerdemain"))
            {
                if(ch->pcdata->csabilities[CSABIL_LEGERDEMAIN] == 0)
                {
                    send_to_char("Your Legerdemain is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LEGERDEMAIN]--;
            }
            else if(!str_prefix(arg1,"subterfuge"))
            {
                if(ch->pcdata->csabilities[CSABIL_SUBTERFUGE] == 0)
                {
                    send_to_char("Your Subterfuge is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SUBTERFUGE]--;
            }
            else if(!str_prefix(arg1,"animal_ken"))
            {
                if(ch->pcdata->csabilities[CSABIL_ANIMAL_KEN] == 0)
                {
                    send_to_char("Your Animal_Ken is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ANIMAL_KEN]--;
            }
            else if(!str_prefix(arg1,"riding"))
            {
                if(ch->pcdata->csabilities[CSABIL_RIDING] == 0)
                {
                    send_to_char("Your Riding is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_RIDING]--;
            }
            else if(!str_prefix(arg1,"etiquette"))
            {
                if(ch->pcdata->csabilities[CSABIL_ETIQUETTE] == 0)
                {
                    send_to_char("Your Etiquette is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ETIQUETTE]--;
            }
            else if(!str_prefix(arg1,"archery"))
            {
                if(ch->pcdata->csabilities[CSABIL_ARCHERY] == 0)
                {
                    send_to_char("Your Archery is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ARCHERY]--;
            }
            else if(!str_prefix(arg1,"melee"))
            {
                if(ch->pcdata->csabilities[CSABIL_MELEE] == 0)
                {
                    send_to_char("Your Melee is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_MELEE]--;
            }
            else if(!str_prefix(arg1,"performance"))
            {
                if(ch->pcdata->csabilities[CSABIL_PERFORMANCE] == 0)
                {
                    send_to_char("Your Performance is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_PERFORMANCE]--;
            }
            else if(!str_prefix(arg1,"crafts"))
            {
                if(ch->pcdata->csabilities[CSABIL_CRAFTS] == 0)
                {
                    send_to_char("Your Crafts is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_CRAFTS]--;
            }
            else if(!str_prefix(arg1,"commerce"))
            {
                if(ch->pcdata->csabilities[CSABIL_COMMERCE] == 0)
                {
                    send_to_char("Your Commerce is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_COMMERCE]--;
            }
            else if(!str_prefix(arg1,"stealth"))
            {
                if(ch->pcdata->csabilities[CSABIL_STEALTH] == 0)
                {
                    send_to_char("Your Stealth is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_STEALTH]--;
            }
            else if(!str_prefix(arg1,"survival"))
            {
                if(ch->pcdata->csabilities[CSABIL_SURVIVAL] == 0)
                {
                    send_to_char("Your Survival is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SURVIVAL]--;
            }
            else if(!str_prefix(arg1,"academics"))
            {
                if(ch->pcdata->csabilities[CSABIL_ACADEMICS] == 0)
                {
                    send_to_char("Your Academics is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_ACADEMICS]--;
            }
            else if(!str_prefix(arg1,"theology"))
            {
                if(ch->pcdata->csabilities[CSABIL_THEOLOGY] == 0)
                {
                    send_to_char("Your Theology is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_THEOLOGY]--;
            }
            else if(!str_prefix(arg1,"seneschal"))
            {
                if(ch->pcdata->csabilities[CSABIL_SENESCHAL] == 0)
                {
                    send_to_char("Your Seneschal is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_SENESCHAL]--;
            }
            else if(!str_prefix(arg1,"investigation"))
            {
                if(ch->pcdata->csabilities[CSABIL_INVESTIGATION] == 0)
                {
                    send_to_char("Your Investigation is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_INVESTIGATION]--;
            }
            else if(!str_prefix(arg1,"law"))
            {
                if(ch->pcdata->csabilities[CSABIL_LAW] == 0)
                {
                    send_to_char("Your Law is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LAW]--;
            }
            else if(!str_prefix(arg1,"linguistics"))
            {
                if(ch->pcdata->csabilities[CSABIL_LINGUISTICS] == 0)
                {
                    send_to_char("Your Linguistics is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_LINGUISTICS]--;
            }
            else if(!str_prefix(arg1,"medicine"))
            {
                if(ch->pcdata->csabilities[CSABIL_MEDICINE] == 0)
                {
                    send_to_char("Your Medicine is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_MEDICINE]--;
            }
            else if(!str_prefix(arg1,"occult"))
            {
                if(ch->pcdata->csabilities[CSABIL_OCCULT] == 0)
                {
                    send_to_char("Your Occult is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_OCCULT]--;
            }
            else if(!str_prefix(arg1,"politics"))
            {
                if(ch->pcdata->csabilities[CSABIL_POLITICS] == 0)
                {
                    send_to_char("Your Politics is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_POLITICS]--;
            }
            else if(!str_prefix(arg1,"hearth_wisdom"))
            {
                if(ch->pcdata->csabilities[CSABIL_HEARTHWISDOM] == 0)
                {
                    send_to_char("Your Hearth_wisdom is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csabilities[CSABIL_HEARTHWISDOM]--;
            }
        }
        else if (ch->pcdata->progress == 6)
        {
            if(!str_prefix(arg1,"conscience"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE] == 1)
                {
                    send_to_char("Your Conscience is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]--;
                ch->pcdata->primary++;
            }
            else if(!str_prefix(arg1,"self_control") || !str_prefix(arg1,"self-control"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL] == 1)
                {
                    send_to_char("Your Self-Control is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL]--;
                ch->pcdata->primary++;
            }
            else if(!str_prefix(arg1,"courage"))
            {
                if(ch->pcdata->csvirtues[CSVIRT_COURAGE] == 1)
                {
                    send_to_char("Your Courage is already at minimum.\n\r",ch);
                    return;
                }
                ch->pcdata->csvirtues[CSVIRT_COURAGE]--;
                ch->pcdata->primary++;
            }
            else
            {
                send_to_char("Please choose Conscience, Self-Control or Courage.\n\r",ch);
                return;
            }
        }
        else
        {
            send_to_char("That is not a valid choice.\n\r",ch);
            return;
        }
        if(ch->pcdata->progress < 5)
        {
            if(ch->pcdata->stat[stat] == PRIMARY) ch->pcdata->primary++;
            if(ch->pcdata->stat[stat] == SECONDARY) ch->pcdata->secondary++;
            if(ch->pcdata->stat[stat] == TERTIARY) ch->pcdata->tertiary++;
        }
        if(ch->pcdata->progress == 1) do_function(ch, &do_create, "showattr" );
        if(ch->pcdata->progress == 4) do_function(ch, &do_create, "showabil" );
        if(ch->pcdata->progress == 6) do_function(ch, &do_create, "showvirtue" );
        return;
    }

    if(!str_prefix(arg1,"done"))
    {
        if((ch->pcdata->progress == 1 || ch->pcdata->progress == 4 || ch->pcdata->progress == 6) && ch->pcdata->primary == 0 && ch->pcdata->secondary == 0 && ch->pcdata->tertiary == 0)
        {
            send_to_char("You've finished and finalized your traits!\n\r",ch);
            ch->pcdata->progress++;
/*          if(ch->pcdata->progress == 7)
            {
                if(ch->freebie < (ch->remorts*3)/4) ch->freebie = (ch->remorts*3)/4;

                ch->freebie += 5;
                ch->pcdata->progress++;
                send_to_char("You can now use the freebie command to spend freebies to further increase your traits!\n\r",ch);
            }
*/
            if(ch->pcdata->progress == 7)
            {
                ch->pcdata->cshumanity = ch->pcdata->csvirtues[CSVIRT_CONSCIENCE]+ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL];
                ch->pcdata->cswillpower = ch->pcdata->csvirtues[CSVIRT_COURAGE];
                ch->pcdata->csmax_willpower = ch->pcdata->cswillpower;
                sprintf(buf,"Calculating your Humanity (Conscience + Self-Control)...%d\n\r",ch->pcdata->cshumanity);
                send_to_char(buf,ch);
                sprintf(buf,"Calculating your Willpower (Courage)... %d\n\r\n\r",ch->pcdata->cswillpower);
                send_to_char(buf,ch);
                send_to_char("Advantage traits are all set.\n\r",ch);
                send_to_char("You can now use the 'backgrounds' command to set your background traits. You have been\n\r",ch);
                send_to_char("given 5 'background freebie' points to be spent on backgrounds using that command.\n\r", ch);
                /*if(ch->freebie < (ch->remorts*3)/4) ch->freebie = (ch->remorts*3)/4;
                ch->freebie += 5;*/
                int i;
              for (i = 0;i < MAX_CSBACK;i++)
    {
                    if (ch->pcdata->csbackgrounds[i] > 0)
                    {
                        backgrounds = FALSE;
                    }
                }
                if (backgrounds) ch->dpoints = 5;
                
                send_to_char("When you've finished setting your backgrounds, you use the freebie command to spend freebies\n\r", ch);
                send_to_char(" to further increase your traits!\n\r",ch);
                ch->pcdata->progress = 8;
                cskill_update(ch);
            }
            else do_function(ch, &do_create, "" );
        }
        else
            send_to_char("Only use Done when you have completed a section.\n\r",ch);

        return;
    }
    send_to_char("That is not a valid option. Please try again.\n\r",ch);
    return;
}

void do_charsheet (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MSL];
    CHAR_DATA *sch;
    CHAR_DATA *xch;

    one_argument(argument, arg);
    if (arg[0] != '\0')
    {
        if (IS_IMMORTAL(ch) && IS_SUPR(ch) && (xch = get_char_world(ch, arg)) != NULL)
        {
            if (IS_NPC(xch))
            {
                send_to_char("Not on NPCs.\n\r", ch);
                return;
            }
            sch = ch;
            ch = xch;
        } else
        {
            sch = ch;
        }
    } else
        sch = ch;

    if(IS_NPC(sch)) return;

send_to_char("\n\r              |     |                            |      |\n\r",sch);
send_to_char("              |\\   /|                            |\\    /|\n\r",sch);
send_to_char("              | | | |  ___   |           |  ___  | |  | |\n\r",sch);
send_to_char("              | |_| | / _  \\ |\\         /| / __\\ |  \\ \\ |\n\r",sch);
send_to_char("              |  _  || /_|  || |       | | | __> | | \\  |\n\r",sch);
send_to_char("             / /  |_| \\___/\\| \\ \\     / /  \\___/ |_|  \\ \\\n\r",sch);
send_to_char("            /_/                \\ \\   / /               \\_\\\n\r",sch);
send_to_char("<==========/====================\\ \\ / /===================\\============>\n\r",sch);
send_to_char(" |                               \\ v /                                |\n\r",sch);
/*send_to_char(" |  Name:                         \\ /                          :Sire  |\n\r",ch); */
 sprintf(buf," |  Name: %s  \\",center(ch->name,22," "));
    send_to_char(buf,sch);
 sprintf(buf," / %s :Nature |\n\r",center("None",23," "));
send_to_char(buf,sch);
sprintf(buf," |  Race: %s    v",center(capitalize(race_table[ch->race].name),21," "));
send_to_char(buf,sch);
sprintf(buf,"  %s :Demeanor |\n\r",center("None",21," "));
send_to_char(buf,sch);
if(ch->race == race_lookup("vampire"))
{
    sprintf(buf," |  Sire:   %s  ",center(ch->sire,21," "));
    send_to_char(buf,sch);
    sprintf(buf,"  %s :Generation |\n\r",center(ch->pcdata->csgeneration == 1 ? "First" : ch->pcdata->csgeneration == 2 ? "Second" : ch->pcdata->csgeneration == 3 ? "Third" :
        ch->pcdata->csgeneration == 4 ? "Fourth" : ch->pcdata->csgeneration == 5 ? "Fifth" : ch->pcdata->csgeneration == 6 ? "Sixth" : ch->pcdata->csgeneration == 7 ? "Seventh" :
        ch->pcdata->csgeneration == 8 ? "Eighth" : ch->pcdata->csgeneration == 9 ? "Ninth" : ch->pcdata->csgeneration == 10 ? "Tenth" : ch->pcdata->csgeneration == 11 ? "Eleventh" :
        ch->pcdata->csgeneration == 12 ? "Twelfth" : ch->pcdata->csgeneration == 13 ? "Thirteenth" : "Unknown",20," "));
    send_to_char(buf,sch);
    sprintf(buf," |  Clan:   %s   ",center(capitalize(clan_table[ch->clan].name),19," "));
    send_to_char(buf,sch);
    sprintf(buf,"  %s       :Haven |\n\r",center("Unset",20," "));
    send_to_char(buf,sch);

}
if(ch->race == race_lookup("ghoul"))
{
    sprintf(buf," |  Domitor:  %s  ",center(ch->vamp_master,19," "));
    send_to_char(buf,sch);
    sprintf(buf,"  %s :Domitor Gen |\n\r",center(ch->pcdata->csgeneration == 4 ? "Methuselah" : ch->pcdata->csgeneration == 5 ? "Fifth" : ch->pcdata->csgeneration == 6 ? "Sixth" : ch->pcdata->csgeneration == 7 ? "Seventh" : ch->pcdata->csgeneration == 8 ? "Eighth" : ch->pcdata->csgeneration == 9 ? "Ninth" : ch->pcdata->csgeneration == 10 ? "Tenth" : ch->pcdata->csgeneration == 11 ? "Eleventh" : ch->pcdata->csgeneration == 12 ? "Twelvth" : ch->pcdata->csgeneration == 13 ? "Thirteenth" : "Negligent",19," "));
    send_to_char(buf,sch);
    sprintf(buf," |  Domitor Clan:   %s  ",center(capitalize(clan_table[ch->clan].name),12," "));
    send_to_char(buf,sch);
    sprintf(buf,"  %s    :Bond |\n\r",center(ch->bonded == 0 ? "None" : ch->bonded == 1 ? "Lv 1" : ch->bonded == 2 ? "Lv 2" : ch->bonded == 3 ? "Lv 3" : "Lv3",19," "));
    send_to_char(buf,sch);

}
if(ch->race == race_lookup("garou"))
{
    sprintf(buf," |  Breed:  %s   ",center(ch->pcdata->breed == METIS ? "Metis" : ch->pcdata->breed == LUPUS ? "Lupus" : ch->pcdata->breed == HOMID ? "Homid" : "Error",21," "));
    send_to_char(buf,sch);
    sprintf(buf," %s  :Tribe |\n\r",center(capitalize(clan_table[ch->clan].name),24," "));
    send_to_char(buf,sch);
    sprintf(buf," |  Auspice:  %s   ",center(ch->pcdata->auspice == RAGABASH ? "Ragabash" : ch->pcdata->auspice ==THEURGE ? "Theurge" :
        ch->pcdata->auspice == PHILODOX ? "Philodox" : ch->pcdata->auspice == GALLIARD ? "Galliard" : ch->pcdata->auspice == AHROUN ?
        "Ahroun" : "Error",19," "));
    send_to_char(buf,sch);
    sprintf(buf,"  %s       :Home |\n\r",center("Unset",19," "));
    send_to_char(buf,sch);
}
if(ch->race == race_lookup("human") && ch->clan == clan_lookup("mage"))
{
    sprintf(buf," |  Tradition: %s ",center(capitalize(tradition_table[ch->tradition].name),19," "));
    send_to_char(buf,sch);
    sprintf(buf," %s :Essence |\n\r",center("None",24," "));
    send_to_char(buf,sch);
}
send_to_char(" |                                                                    |\n\r",sch);
send_to_char(" |<=-------------------------=Attributes=---------------------------=>|\n\r",sch);
send_to_char(" |                                                                    |\n\r",sch);
send_to_char(" |      PHYSICAL                SOCIAL                  MENTAL        |\n\r",sch);
sprintf(buf, " | Strength_______%s Charisma_______%s Perception____%s|\n\r",dots(get_attribute(ch,STRENGTH),TRUE),dots(get_attribute(ch,CHARISMA),TRUE),dots(get_attribute(ch,PERCEPTION),TRUE));
send_to_char(buf,sch);
sprintf(buf, " | Dexterity______%s Manipulation___%s Intelligence__%s|\n\r",dots(get_attribute(ch,DEXTERITY),TRUE),dots(get_attribute(ch,MANIPULATION),TRUE),dots(get_attribute(ch,INTELLIGENCE),TRUE));
send_to_char(buf,sch);
sprintf(buf, " | Stamina________%s Appearance_____%s Wits__________%s|\n\r",dots(get_attribute(ch,STAMINA),TRUE),dots(get_attribute(ch,APPEARANCE),TRUE),dots(get_attribute(ch,WITS),TRUE));
send_to_char(buf,sch);
send_to_char(" |                                                                    |\n\r",sch);
send_to_char(" |<=--------------------------=Abilities=---------------------------=>|\n\r",sch);
send_to_char(" |                                                                    |\n\r",sch);
send_to_char(" |       TALENTS               SKILLS                KNOWLEDGE        |\n\r",sch);
sprintf(buf, " | Expression_____%s   Animal Ken_____%s   Academics______%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_EXPRESSION], FALSE),dots(ch->pcdata->csabilities[CSABIL_ANIMAL_KEN], FALSE),dots(ch->pcdata->csabilities[CSABIL_ACADEMICS], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Alertness______%s   Archery________%s   Seneschal______%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_ALERTNESS], FALSE),dots(ch->pcdata->csabilities[CSABIL_ARCHERY], FALSE),dots(ch->pcdata->csabilities[CSABIL_SENESCHAL], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Athletics______%s   Crafts_________%s   Investigation__%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_ATHLETICS], FALSE),dots(ch->pcdata->csabilities[CSABIL_CRAFTS], FALSE),dots(ch->pcdata->csabilities[CSABIL_INVESTIGATION], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Brawl__________%s   Etiquette______%s   Law____________%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_BRAWL], FALSE),dots(ch->pcdata->csabilities[CSABIL_ETIQUETTE], FALSE),dots(ch->pcdata->csabilities[CSABIL_LAW], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Dodge__________%s   Melee__________%s   Linguistics____%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_DODGE], FALSE),dots(ch->pcdata->csabilities[CSABIL_MELEE], FALSE),dots(ch->pcdata->csabilities[CSABIL_LINGUISTICS], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Empathy________%s   Performance____%s   Medicine_______%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_EMPATHY], FALSE),dots(ch->pcdata->csabilities[CSABIL_PERFORMANCE], FALSE),dots(ch->pcdata->csabilities[CSABIL_MEDICINE], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Intimidation___%s   Riding_________%s   Occult_________%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_INTIMIDATION], FALSE),dots(ch->pcdata->csabilities[CSABIL_RIDING], FALSE),dots(ch->pcdata->csabilities[CSABIL_OCCULT], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Leadership_____%s   Commerce_______%s   Politics_______%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_LEADERSHIP], FALSE),dots(ch->pcdata->csabilities[CSABIL_COMMERCE], FALSE),dots(ch->pcdata->csabilities[CSABIL_POLITICS], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Legerdemain____%s   Stealth________%s   Hearth Wisdom__%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_LEGERDEMAIN], FALSE),dots(ch->pcdata->csabilities[CSABIL_STEALTH], FALSE),dots(ch->pcdata->csabilities[CSABIL_HEARTHWISDOM], FALSE));
send_to_char(buf,sch);
sprintf(buf, " | Subterfuge_____%s   Survival_______%s   Theology_______%s |\n\r",dots(ch->pcdata->csabilities[CSABIL_SUBTERFUGE], FALSE),dots(ch->pcdata->csabilities[CSABIL_SURVIVAL], FALSE),dots(ch->pcdata->csabilities[CSABIL_THEOLOGY], FALSE));
send_to_char(buf,sch);
send_to_char(" |                                                                    |\n\r",sch);
send_to_char("<======================================================================>\n\r",sch);
send_to_char("You can see additional information by typing CS2.\n\r",sch);
    return;
}

void do_charsheet2 (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char arg[MSL];
    CHAR_DATA *sch;
    CHAR_DATA *xch;

    one_argument(argument, arg);
    if (arg[0] != '\0')
    {
        if (IS_IMMORTAL(ch) && IS_SUPR(ch) && (xch = get_char_world(ch, arg)) != NULL)
        {
            if (IS_NPC(xch))
            {
                send_to_char("Not on NPCs.\n\r", ch);
                return;
            }
            sch = ch;
            ch = xch;
        } else
        {
            sch = ch;
        }
    } else
        sch = ch;

    if(IS_NPC(sch)) return;


    if(IS_VAMP(ch))
    {
        send_to_char("\n\r |<=-------------------------=Advantages=---------------------------=>|\n\r",sch);
        send_to_char(" |                                                                    |\n\r",sch);
        send_to_char(" |      DISCIPLINES           BACKGROUNDS               VIRTUES       |\n\r",sch);
        sprintf(buf, " | Animalism______%s Allies_________%s   Conscience____%s  |\n\r",dots(ch->pcdata->discipline[ANIMALISM], TRUE),dots(ch->pcdata->csbackgrounds[CSBACK_ALLIES], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE));
        send_to_char(buf,sch);
        sprintf(buf, " | Auspex_________%s Contacts_______%s                        |\n\r",dots(ch->pcdata->discipline[AUSPEX], TRUE), dots(ch->pcdata->csbackgrounds[CSBACK_CONTACTS], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Celerity_______%s Influence______%s   Self-Control__%s  |\n\r",dots(ch->pcdata->discipline[CELERITY], TRUE),dots(ch->pcdata->csbackgrounds[CSBACK_INFLUENCE], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Chimerstry_____%s Resources______%s                        |\n\r",dots(ch->pcdata->discipline[CHIMERSTRY], TRUE), dots(ch->pcdata->csbackgrounds[CSBACK_RESOURCES], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Dominate_______%s Status_________%s   Courage_______%s  |\n\r",dots(ch->pcdata->discipline[DOMINATE], TRUE),dots(ch->pcdata->csbackgrounds[CSBACK_STATUS], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Dementation____%s                                             |\n\r",dots(ch->pcdata->discipline[DEMENTATION], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Fortitude______%s                                             |\n\r",dots(ch->pcdata->discipline[FORTITUDE], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Mortis_________%s                                             |\n\r",dots(ch->pcdata->discipline[MORTIS], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Obfuscate______%s<------Humanity----->   <------Health------> |\n\r",dots(ch->pcdata->discipline[OBFUSCATE], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Obtenebration__%s %s        Bruised        _ |\n\r",dots(ch->pcdata->discipline[OBTENEBRATION], TRUE),dots2(ch->pcdata->cshumanity, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Potence________%s<------Willpower---->       Hurt        -1 _ |\n\r",dots(ch->pcdata->discipline[POTENCE], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Presence_______%s %s        Injured     -1 _ |\n\r",dots(ch->pcdata->discipline[PRESENCE], TRUE),dots2(ch->pcdata->csmax_willpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Protean________%s %s        Wounded     -2 _ |\n\r",dots(ch->pcdata->discipline[PROTEAN], TRUE),dots3(ch->pcdata->cswillpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Quietus________%s                            Mauled      -2 _ |\n\r",dots(ch->pcdata->discipline[QUIETUS], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Serpentis______%s                            Crippled    -5 _ |\n\r",dots(ch->pcdata->discipline[SERPENTIS], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Thaumaturgy____%s                            Incapacitated  _ |\n\r",dots(ch->pcdata->discipline[THAUMATURGY], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Vicissitude____%s                                             |\n\r",dots(ch->pcdata->discipline[VICISSITUDE], TRUE));
        send_to_char(buf, sch);
        send_to_char("<======================================================================>\n\r",sch);
    }
    if(ch->race == race_lookup("garou") && ch->pcdata->rank == 0)
    {
        send_to_char("\n\r |<=-------------------------=Advantages=---------------------------=>|\n\r",sch);
        send_to_char(" |                                                                    |\n\r",sch);
        send_to_char(" |         GIFTS              BACKGROUNDS               VIRTUES       |\n\r",sch);
        sprintf(buf, " | %s  Allies_________%s    Conscience____%s |\n\r",center(!str_cmp(ch->gift1,"(null)") ? " " : ch->gift1,21," "),dots(ch->pcdata->csbackgrounds[CSBACK_ALLIES], FALSE),dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | %s  Contacts_______%s                        |\n\r",center(!str_cmp(ch->gift2,"(null)") ? " " : ch->gift2,21," "), dots(ch->pcdata->csbackgrounds[CSBACK_CONTACTS], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | %s  Influence______%s    Self-Control__%s |\n\r",center(!str_cmp(ch->gift3,"(null)") ? " " : ch->gift3,21," "),dots(ch->pcdata->csbackgrounds[CSBACK_INFLUENCE], FALSE),dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | %s  Resources______%s                        |\n\r",center(!str_cmp(ch->gift4,"(null)") ? " " : ch->gift4,21," "), dots(ch->pcdata->csbackgrounds[CSBACK_RESOURCES], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | %s  Status_________%s    Courage_______%s |\n\r",center(!str_cmp(ch->gift5,"(null)") ? " " : ch->gift5,21," "),dots(ch->pcdata->csbackgrounds[CSBACK_STATUS], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |<=----------------------------------------------------------------=>|\n\r");
        send_to_char(buf, sch);
        sprintf(buf2,"%d",ch->renown);
        sprintf(buf, " | Renown:  %s     <------Humanity----->   <------Health------> |\n\r",center(buf2,8," "));
        send_to_char(buf, sch);
        sprintf(buf2,"%d",ch->rage);
        sprintf(buf, " | Rage:    %s      %s        Bruised        _ |\n\r",center(buf2,8," "),dots2(ch->pcdata->cshumanity, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Form:   %s    <------Willpower---->       Hurt        -1 _ |\n\r",center(ch->changed == CHANGED_HOMID ? "Homid" : ch->changed == CHANGED_LUPUS ? "Lupus" : ch->changed == CHANGED_CRINOS ? "Crinos" : "Error", 10, " "));
        send_to_char(buf, sch);
        sprintf(buf, " |                        %s        Injured     -1 _ |\n\r",dots2(ch->pcdata->csmax_willpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |                        %s        Wounded     -2 _ |\n\r",dots3(ch->pcdata->cswillpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |                                                   Mauled      -2 _ |\n\r");
        send_to_char(buf, sch);
        sprintf(buf, " |                                                   Crippled    -5 _ |\n\r");
        send_to_char(buf, sch);
        sprintf(buf, " |                                                   Incapacitated  _ |\n\r");
        send_to_char(buf, sch);
        send_to_char("<======================================================================>\n\r",sch);
    }
    if(ch->race == race_lookup("garou") && ch->pcdata->rank > 0)
    {
        send_to_char("\n\r |<=-------------------------=Advantages=---------------------------=>|\n\r",sch);
        send_to_char(" |                                                                    |\n\r",sch);
        send_to_char(" | <------Renown------->     -BACKGROUNDS-             -VIRTUES-      |\n\r",sch);
        sprintf(buf, " |      --Glory--         Allies_________%s    Conscience____%s |\n\r",dots(ch->pcdata->csbackgrounds[CSBACK_ALLIES], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s   Contacts_______%s                        |\n\r",dots2(ch->pcdata->renown[GLORY],TRUE), dots(ch->pcdata->csbackgrounds[CSBACK_CONTACTS], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s   Influence______%s    Self-Control__%s |\n\r",dots3(ch->pcdata->renown[TEMP_GLORY], TRUE),dots(ch->pcdata->csbackgrounds[CSBACK_INFLUENCE], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |      --Honor--         Resources______%s                        |\n\r", dots(ch->pcdata->csbackgrounds[CSBACK_RESOURCES], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s   Status_________%s    Courage_______%s |\n\r",dots2(ch->pcdata->renown[HONOR],TRUE),dots(ch->pcdata->csbackgrounds[CSBACK_STATUS], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s  <=-----------------------------------------=>|\n\r", dots3(ch->pcdata->renown[TEMP_HONOR],TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |      --Wisdom--       <-------Rage-------->   <------Health------> |\n\r");
        send_to_char(buf, sch);
        sprintf(buf, " |  %s   %s        Bruised        _ |\n\r",dots2(ch->pcdata->renown[WISDOM],TRUE),dots2(ch->pcdata->rage[PERM],TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s   %s        Hurt        -1 _ |\n\r",dots3(ch->pcdata->renown[TEMP_WISDOM],TRUE), dots3(ch->pcdata->rage[TEMP],TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |                       <------Gnosis------->       Injured     -1 _ |\n\r");
        send_to_char(buf, sch);
        switch(ch->pcdata->rank)
        {
            case 1: sprintf(buf2,"Cliath"); break;
            case 2: sprintf(buf2,"Fostern"); break;
            case 3: sprintf(buf2,"Adren"); break;
            case 4: sprintf(buf2,"Athro"); break;
            case 5: sprintf(buf2,"Elder"); break;
            default: sprintf(buf2,"Error"); break;
        }
        sprintf(buf, " |     Rank %s       %s        Wounded     -2 _ |\n\r",center(buf2, 7, " "), dots2(ch->pcdata->gnosis[PERM], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |                        %s        Mauled      -2 _ |\n\r",dots3(ch->pcdata->gnosis[TEMP], TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |   <-Primal Urge->     <-----Willpower----->       Crippled    -5 _ |\n\r");
        send_to_char(buf, sch);
        sprintf(buf, " |        %s           %s        Incapacitated  _ |\n\r", dots(ch->pcdata->primal_urge, FALSE), dots2(ch->pcdata->csmax_willpower,TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |                        %s                         |\n\r",dots3(ch->pcdata->cswillpower,TRUE));
        send_to_char(buf, sch);
        send_to_char("<======================================================================>\n\r",sch);
    }
    if(ch->race == race_lookup("human") && ch->clan == clan_lookup("mage"))
    {
        send_to_char("\n\r |<=-------------------------=Advantages=---------------------------=>|\n\r",sch);
        send_to_char(" |                                                                    |\n\r",sch);
        send_to_char(" |         SPHERES            BACKGROUNDS               VIRTUES       |\n\r",sch);
        sprintf(buf, " | Prime:          %s  Allies_________%s    Conscience____%s |\n\r",dots(ch->sphere[SPHERE_PRIME], FALSE),dots(ch->pcdata->csbackgrounds[CSBACK_ALLIES], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Spirit:         %s  Contacts_______%s                        |\n\r",dots(ch->sphere[SPHERE_SPIRIT], FALSE), dots(ch->pcdata->csbackgrounds[CSBACK_CONTACTS], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Time:           %s  Influence______%s    Self-Control__%s |\n\r",dots(ch->sphere[SPHERE_TIME], FALSE),dots(ch->pcdata->csbackgrounds[CSBACK_INFLUENCE], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Mind:           %s  Resources______%s                        |\n\r",dots(ch->sphere[SPHERE_MIND], FALSE), dots(ch->pcdata->csbackgrounds[CSBACK_RESOURCES], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Matter:         %s  Status_________%s    Courage_______%s |\n\r",dots(ch->sphere[SPHERE_MATTER], FALSE),dots(ch->pcdata->csbackgrounds[CSBACK_STATUS], FALSE),  dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Life:           %s                                              |\n\r",dots(ch->sphere[SPHERE_LIFE], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Forces:         %s  <------Humanity----->  <------Health------> |\n\r",dots(ch->sphere[SPHERE_FORCES], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " | Correspondence: %s   %s       Bruised        _ |\n\r",dots(ch->sphere[SPHERE_CORRESPONDENCE], FALSE),dots2(ch->pcdata->cshumanity, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Entropy:        %s  <------Willpower---->      Hurt        -1 _ |\n\r",dots(ch->sphere[SPHERE_ENTROPY], FALSE));
        send_to_char(buf, sch);
        sprintf(buf, " |  <------Arete------>    %s       Injured     -1 _ |\n\r",dots2(ch->pcdata->csmax_willpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " |  %s    %s       Wounded     -2 _ |\n\r",dots2(ch->arete, TRUE),dots3(ch->pcdata->cswillpower, TRUE));
        send_to_char(buf, sch);
        sprintf(buf, " | Avatar:       %s                               Mauled      -2 _ |\n\r", dots(ch->avatar, FALSE));
        send_to_char(buf, sch);
        sprintf(buf2,"%d",ch->quintessence);
        sprintf(buf, " | Quintessence: %s                                 Crippled    -5 _ |\n\r", center(buf2,3," "));
        send_to_char(buf, sch);
        sprintf(buf2,"%d",ch->paradox);
        sprintf(buf, " | Paradox:      %s                                 Incapacitated  _ |\n\r", center(buf2,3," "));
        send_to_char(buf, sch);
        send_to_char("<======================================================================>\n\r",sch);
    }
    else if(ch->race == race_lookup("human"))
        {
            send_to_char("\n\r |<=-------------------------=Advantages=---------------------------=>|\n\r",sch);
            send_to_char(" |                                                                    |\n\r",sch);
            send_to_char(" |       OTHER TRAITS         BACKGROUNDS               VIRTUES       |\n\r",sch);
            sprintf(buf, " | ____________________   Allies_________%s   Conscience____%s  |\n\r",dots(ch->pcdata->csbackgrounds[CSBACK_ALLIES], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_CONSCIENCE], FALSE));
            send_to_char(buf, sch);
            sprintf(buf, " | ____________________   Contacts_______%s                        |\n\r", dots(ch->pcdata->csbackgrounds[CSBACK_CONTACTS], FALSE));
            send_to_char(buf, sch);
            sprintf(buf, " | ____________________   Influence______%s   Self-Control__%s  |\n\r",dots(ch->pcdata->csbackgrounds[CSBACK_INFLUENCE], FALSE),dots(ch->pcdata->csvirtues[CSVIRT_SELF_CONTROL], FALSE));
            send_to_char(buf, sch);
            sprintf(buf, " | ____________________   Resources______%s                        |\n\r", dots(ch->pcdata->csbackgrounds[CSBACK_RESOURCES], FALSE));
            send_to_char(buf, sch);
            sprintf(buf, " | ____________________   Status_________%s   Courage_______%s  |\n\r",dots(ch->pcdata->csbackgrounds[CSBACK_STATUS], FALSE), dots(ch->pcdata->csvirtues[CSVIRT_COURAGE], FALSE));
            send_to_char(buf, sch);
            sprintf(buf, " | <=--------------------------------------------------------------=> |\n\r");
            send_to_char(buf, sch);
            sprintf(buf, " |                        <------Humanity----->  <------Health------> |\n\r");
            send_to_char(buf, sch);
            sprintf(buf, " |                         %s       Bruised        _ |\n\r", dots2(ch->pcdata->cshumanity, TRUE));
            send_to_char(buf, sch);
            sprintf(buf, " |                                                   Hurt        -1 _ |\n\r");
            send_to_char(buf, sch);
            sprintf(buf, " |                        <------Willpower---->      Injured     -1 _ |\n\r");
            send_to_char(buf, sch);
            sprintf(buf, " |                         %s       Wounded     -2 _ |\n\r",dots2(ch->pcdata->csmax_willpower, TRUE));
            send_to_char(buf, sch);
            sprintf(buf, " |                         %s       Mauled      -2 _ |\n\r", dots3(ch->pcdata->cswillpower, TRUE));
            send_to_char(buf, sch);
            sprintf(buf, " |                                                   Crippled    -5 _ |\n\r");
            send_to_char(buf, sch);
            sprintf(buf, " |                                                   Incapacitated  _ |\n\r");
            send_to_char(buf, sch);
            send_to_char("<======================================================================>\n\r",sch);
    }
    return;
}

void do_charsheet3 (CHAR_DATA * ch, char *argument)
{
    int row = 1;
    char buf[MAX_STRING_LENGTH];
    char name[9][MAX_STRING_LENGTH];
    char sec1[9][MAX_STRING_LENGTH];
    char sec2[9][MAX_STRING_LENGTH];
    char sec3[9][MAX_STRING_LENGTH];
    char sec4[9][MAX_STRING_LENGTH];
    char sec5[9][MAX_STRING_LENGTH];
    int i;
    char arg[MSL];
    CHAR_DATA *sch;
    CHAR_DATA *xch;

    one_argument(argument, arg);
    if (arg[0] != '\0')
    {
        if (IS_IMMORTAL(ch) && IS_SUPR(ch) && (xch = get_char_world(ch, arg)) != NULL)
        {
            if (IS_NPC(xch))
            {
                send_to_char("Not on NPCs.\n\r", ch);
                return;
            }
            sch = ch;
            ch = xch;
        } else
        {
            sch = ch;
        }
    } else
        sch = ch;


    if(IS_NPC(sch)) return;


    send_to_char("\n\r<=========================Secondary Abilities==========================>\n\r",sch);

    for(i = 0; i < 9;i++)
    {
        name[i][0] = '\0';
        sec1[i][0] = '\0';
        sec2[i][0] = '\0';
        sec3[i][0] = '\0';
        sec4[i][0] = '\0';
        sec5[i][0] = '\0';
    }
    
    if(ch->race == race_lookup("garou"))
    {
        sprintf(name[row], "      Skills      ");
        sprintf(sec1[row], "                  ");
        sprintf(sec2[row], "Enigmas      %s", dots(ch->pcdata->cssec_abil[CSABIL_ENIGMAS], FALSE));
        sprintf(sec3[row], "Rituals      %s", dots(ch->pcdata->cssec_abil[CSABIL_RITUALS], FALSE));
        sprintf(sec4[row], "Meditation   %s", dots(ch->pcdata->cssec_abil[CSABIL_MEDITATION], FALSE));
        sprintf(sec5[row], "                  ");
        row++;
} else if(ch->race == race_lookup("vampire"))
    {
        sprintf(name[row], "      Skills      ");
        sprintf(sec1[row], "                  ");
        sprintf(sec2[row], "Body Crafts  %s", dots(ch->pcdata->cssec_abil[CSABIL_BODYCRAFTS], FALSE));
        sprintf(sec3[row], "Rituals      %s", dots(ch->pcdata->cssec_abil[CSABIL_RITUALS], FALSE));
        sprintf(sec4[row], "                  ");
        sprintf(sec5[row], "                  ");
        row++;
    } else if(ch->avatar > 0)
    {
        sprintf(name[row], "      Skills      ");
        sprintf(sec1[row], "                  ");
        sprintf(sec2[row], "Enigmas      %s", dots(ch->pcdata->cssec_abil[CSABIL_ENIGMAS], FALSE));
        sprintf(sec3[row], "Meditation   %s", dots(ch->pcdata->cssec_abil[CSABIL_MEDITATION], FALSE));
        sprintf(sec4[row], "                  ");
        sprintf(sec5[row], "                  ");
        row++;
}                        
    if (ch->pcdata->csabilities[CSABIL_MELEE] > 0)
    {
        sprintf(name[row], "  Weapon Skills   ");
        sprintf(sec1[row], "Light        %s", dots(ch->pcdata->cssec_abil[CSABIL_LIGHT], FALSE));
        sprintf(sec2[row], "Heavy        %s", dots(ch->pcdata->cssec_abil[CSABIL_HEAVY], FALSE));
        sprintf(sec3[row], "Blunt        %s", dots(ch->pcdata->cssec_abil[CSABIL_BLUNT], FALSE));
        sprintf(sec4[row], "Polearms     %s", dots(ch->pcdata->cssec_abil[CSABIL_POLEARM], FALSE));
        sprintf(sec5[row], "                  ");
        row++;
    }
    if(ch->pcdata->csabilities[CSABIL_CRAFTS] > 0)
    {
        sprintf(name[row], "      Crafts      ");
        sprintf(sec1[row], "                  ");
        sprintf(sec2[row], "Carving      %s", dots(ch->pcdata->cssec_abil[CSABIL_CARVING], FALSE));
        sprintf(sec3[row], "Mining       %s", dots(ch->pcdata->cssec_abil[CSABIL_MINING], FALSE));
        sprintf(sec4[row], "                  ");
        sprintf(sec5[row], "                  ");
        row++;
    }

    if(ch->pcdata->cssec_abil[CSABIL_MINING] > 0)
    {
        sprintf(name[row], "      Mining      ");
        sprintf(sec1[row], "Prospecting  %s", dots(ch->pcdata->cssec_abil[CSABIL_PROSPECTING], FALSE));
        sprintf(sec2[row], "Mineral      %s", dots(ch->pcdata->cssec_abil[CSABIL_MINERAL], FALSE));
        sprintf(sec3[row], "Metal        %s", dots(ch->pcdata->cssec_abil[CSABIL_METAL], FALSE));
        sprintf(sec4[row], "Precious     %s", dots(ch->pcdata->cssec_abil[CSABIL_PRECIOUS], FALSE));
        sprintf(sec5[row], "                  ");
        row++;
    }

    if(ch->pcdata->cssec_abil[CSABIL_CARVING] > 0)
    {
        sprintf(name[row], "      Carving     ");
        sprintf(sec1[row], "Logging      %s", dots(ch->pcdata->cssec_abil[CSABIL_LOGGING], FALSE));
        sprintf(sec2[row], "Wood         %s", dots(ch->pcdata->cssec_abil[CSABIL_WOOD], FALSE));
        sprintf(sec3[row], "Bone         %s", dots(ch->pcdata->cssec_abil[CSABIL_BONE], FALSE));
        sprintf(sec4[row], "Stone        %s", dots(ch->pcdata->cssec_abil[CSABIL_STONE], FALSE));
        sprintf(sec5[row], "                  ");
        row++;
    }

    if(row-1 == 0)
        send_to_char(" |                  You have no secondary abilities.                  |\n\r",sch);

    if(row-1 > 0)
    {
        send_to_char(" |                                                                    |\n\r",sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",name[1][0] != '\0' ? name[1] : "                  ",
                    name[2][0] != '\0' ? name[2] : "                  ",
                    name[3][0] != '\0' ? name[3] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec1[1][0] != '\0' ? sec1[1] : "                  ",
                    sec1[2][0] != '\0' ? sec1[2] : "                  ",
                    sec1[3][0] != '\0' ? sec1[3] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec2[1][0] != '\0' ? sec2[1] : "                  ",
                    sec2[2][0] != '\0' ? sec2[2] : "                  ",
                    sec2[3][0] != '\0' ? sec2[3] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec3[1][0] != '\0' ? sec3[1] : "                  ",
                    sec3[2][0] != '\0' ? sec3[2] : "                  ",
                    sec3[3][0] != '\0' ? sec3[3] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec4[1][0] != '\0' ? sec4[1] : "                  ",
                    sec4[2][0] != '\0' ? sec4[2] : "                  ",
                    sec4[3][0] != '\0' ? sec4[3] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec5[1][0] != '\0' ? sec5[1] : "                  ",
                    sec5[2][0] != '\0' ? sec5[2] : "                  ",
                    sec5[3][0] != '\0' ? sec5[3] : "                  ");
        send_to_char(buf, sch);
    }
    if(row-1 > 3)
    {
        sprintf(buf, " |   %s    %s    %s   |\n\r",name[4][0] != '\0' ? name[4] : "                  ",
                    name[5][0] != '\0' ? name[5] : "                  ",
                    name[6][0] != '\0' ? name[6] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec1[4][0] != '\0' ? sec1[4] : "                  ",
                    sec1[5][0] != '\0' ? sec1[5] : "                  ",
                    sec1[6][0] != '\0' ? sec1[6] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec2[4][0] != '\0' ? sec2[4] : "                  ",
                    sec2[5][0] != '\0' ? sec2[5] : "                  ",
                    sec2[6][0] != '\0' ? sec2[6] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec3[4][0] != '\0' ? sec3[4] : "                  ",
                    sec3[5][0] != '\0' ? sec3[5] : "                  ",
                    sec3[6][0] != '\0' ? sec3[6] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec4[4][0] != '\0' ? sec4[4] : "                  ",
                    sec4[5][0] != '\0' ? sec4[5] : "                  ",
                    sec4[6][0] != '\0' ? sec4[6] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec5[4][0] != '\0' ? sec5[4] : "                  ",
                    sec5[5][0] != '\0' ? sec5[5] : "                  ",
                    sec5[6][0] != '\0' ? sec5[6] : "                  ");
        send_to_char(buf, sch);
    }
    if(row-1 > 6)
    {
        sprintf(buf, " |   %s    %s    %s   |\n\r",name[7][0] != '\0' ? name[7] : "                  ",
                    name[8][0] != '\0' ? name[8] : "                  ",
                    name[9][0] != '\0' ? name[9] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec1[7][0] != '\0' ? sec1[7] : "                  ",
                    sec1[8][0] != '\0' ? sec1[8] : "                  ",
                    sec1[9][0] != '\0' ? sec1[9] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec2[7][0] != '\0' ? sec2[7] : "                  ",
                    sec2[8][0] != '\0' ? sec2[8] : "                  ",
                    sec2[9][0] != '\0' ? sec2[9] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec3[7][0] != '\0' ? sec3[7] : "                  ",
                    sec3[8][0] != '\0' ? sec3[8] : "                  ",
                    sec3[9][0] != '\0' ? sec3[9] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec4[7][0] != '\0' ? sec4[7] : "                  ",
                    sec4[8][0] != '\0' ? sec4[8] : "                  ",
                    sec4[9][0] != '\0' ? sec4[9] : "                  ");
        send_to_char(buf, sch);
        sprintf(buf, " |   %s    %s    %s   |\n\r",sec5[7][0] != '\0' ? sec5[7] : "                  ",
                    sec5[8][0] != '\0' ? sec5[8] : "                  ",
                    sec5[9][0] != '\0' ? sec5[9] : "                  ");
        send_to_char(buf, sch);
    }
    send_to_char("<======================================================================>\n\r",sch);
    return;
}

void do_backgrounds (CHAR_DATA *ch, char *argument)
{
//Show background stats to a char.
//also, set 5 free bg points at creation.
    char    buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char    arg1[MIL];
    sh_int  trait;
    sh_int  max;

    argument = one_argument(argument, arg1);

    if ( IS_IMMORTAL(ch) && (victim = get_char_world(ch, arg1)) != NULL &&
    !IS_NPC(victim))
    {//print to IMMs.
            int i;
    int col = 0;
    send_to_char("\n\r <=-------------------------=Backgrounds=--------------------------=>\n\r",ch);
    send_to_char("                                                                     \n\r",ch);
    for (i = 0;i < MAX_CSBACK;i++)
    {
        if (victim->pcdata->csbackgrounds[i] > 0)
        {
            sprintf(buf, " %-15s %s ", csback_table[i].name, dots(victim->pcdata->csbackgrounds[i], FALSE));
            col++;
            if (col > 2) {
                strcat( buf, "\n\r" );
                col = 0;
            }
            send_to_char(buf, ch);
        }
    }
    sprintf(buf, "\n\r <=----------------------------------------------------------------=> \n\r");
    send_to_char(buf,ch);
    return;
    }

    if (!str_cmp(arg1, "show"))
    {
    int i;
    int col = 0;
    send_to_char("\n\r <=-------------------------=Backgrounds=--------------------------=>\n\r",ch);
    send_to_char("                                                                     \n\r",ch);
    for (i = 0;i < MAX_CSBACK;i++)
    {
            if ((csback_table[i].race == CSBACK_VAMPIRE && !IS_VAMP(ch)) ||
                (csback_table[i].race == CSBACK_WEREWOLF && ch->race != race_lookup("garou")) ||
                (csback_table[i].race == CSBACK_MAGE && (ch->arete < 1 && ch->avatar < 1)))
                    continue;

            sprintf(buf, " %-15s ", csback_table[i].name);
            col++;
            if (col > 2) {
                strcat( buf, "\n\r" );
                col = 0;
            }
            send_to_char(buf, ch);
        }
    sprintf(buf, "\n\r <=----------------------------------------------------------------=> \n\r");
    send_to_char(buf,ch);
    if (ch->dpoints > 0)
    {
        sprintf(buf, "\n\rYou have %d points to be spent on backgrounds.\n\rType 'background <trait>' to set these stats.\n\r", ch->dpoints);
        send_to_char(buf, ch);
        sendch("{RPlease be aware that points set on Backgrounds are permenant so be sure of your selection.{x\n\r", ch);
    }
    return;
    }

    trait = -1;
    max = 4;
    trait = background_lookup(arg1);
    if (trait == background_lookup("generation"))
        max = 1;

    if (trait != -1)
    {
        switch (csback_table[trait].race)
        {
            case CSBACK_VAMPIRE:
                if (!IS_VAMP(ch))
                {
                    send_to_char("That is a Kindred background and you are not a vampire!\n\r", ch);
                    return;
                }
                break;
            case CSBACK_WEREWOLF:
                if (ch->race != race_lookup("garou")) {
                    send_to_char("That is a Garou background and you are not a werewolf!\n\r", ch);
                    return;
                }
                break;
            case CSBACK_MAGE:
                if (ch->arete < 1 && ch->avatar < 1) {
                    send_to_char("That is a Mage background and you are not a Mage!\n\r", ch);
                    return;
                }
                break;
            }
        if(ch->pcdata->csbackgrounds[trait] >= max)
        {
            send_to_char("You cannot set that trait any higher at creation.\n\r",ch);
            return;
        }
        if ((trait == CSBACK_GENERATION) && (ch->pcdata->csgeneration <= 9))
        {
            send_to_char("You cannot purchase any further points in Generation without seeking Admin approval.\n\r",ch);
            return;
        }
        if (ch->dpoints < 1)
        {
            send_to_char("You have no more points to spend!\n\r", ch);
            return;
        }
        if (trait == CSBACK_GENERATION)
        {
            ch->gen--;
            ch->pcdata->csgeneration = ch->gen;
            ch->max_pblood += 10;
            if (ch->max_pblood > 200)
                ch->max_pblood = 200;
        }
        if (trait == CSBACK_FOUNT)
            ch->max_quintessence += 10;
        ch->pcdata->csbackgrounds[trait]++;
        ch->dpoints--;
    }
    int i;
    int col = 0;
    send_to_char("\n\r <=-------------------------=Backgrounds=--------------------------=>\n\r",ch);
    send_to_char("                                                                     \n\r",ch);
    for (i = 0;i < MAX_CSBACK;i++)
    {
        if (ch->pcdata->csbackgrounds[i] > 0)
        {
            sprintf(buf, " %-15s %s ", csback_table[i].name, dots(ch->pcdata->csbackgrounds[i], FALSE));
            col++;
            if (col > 2) {
                strcat( buf, "\n\r" );
                col = 0;
            }
            send_to_char(buf, ch);
        }
    }
    sprintf(buf, "\n\r <=----------------------------------------------------------------=> \n\r");
    send_to_char(buf,ch);
    if (ch->dpoints > 0) {
        sprintf(buf, "\n\rYou have %d points to be spent on backgrounds.\n\rPlease type 'background <trait>' to set these stats.\n\r", ch->dpoints);
        send_to_char(buf, ch);
        send_to_char("Type 'background show' to show the list of backgrounds available to you.\n\r", ch);
        sendch("{RPlease be aware that points set on Backgrounds are permenant so be sure of your selection.{x\n\r", ch);
        }
}
