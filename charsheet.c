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


void do_freebie(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int max_unlock = ch->remorts / 10;
    int unlockpoints = max_unlock - ch->unlocksspent;
    int i, count=0, cost=0, step=0, tolearn=0;
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

        if( ch->pcdata->rank > 0)
        {
            if(ch->pcdata->primal_urge == 0) 
        send_to_char("Primal-Urge              4\n\r",ch);
            else
        send_to_char("Primal-Urge:             Current Rating*2\n\r",ch);
        send_to_char("Rage:                    Current Rating*2\n\r",ch);
        send_to_char("Gnosis:                  Current Rating*2\n\r",ch);
        send_to_char("Renown:                  Current Rating*2 + 10 Temp Renown.\n\r",ch);
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
                if (ch->pcdata->csbackgrounds[CSBACK_GENERATION] == 5)
                {
                    send_to_char("Your blood is already as potent as it can be.\n\r", ch);;
                    return;
                }
                count = CSBACK_GENERATION;
                switch (ch->pcdata->csbackgrounds[CSBACK_GENERATION]) {
                case 0: minimictime = 15; minimage =  75; cost = 10; break;
                case 1: minimictime = 30; minimage = 150; cost = 20; break;
                case 2: minimictime = 60; minimage = 300; cost = 40; break;
                case 3: minimictime = 120; minimage = 400; cost = 60; break;
                case 4: minimictime = 240; minimage = 500; cost = 80; break; }

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
            if(ch->pcdata->discipline[i] == 5 && ch->gen > 7)
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
            if (ch->gen > 10)
                max = 1;
            if (ch->gen <= 10)
                max = 2;
            if (ch->gen <= 8)
                max = 3;
            if (ch->gen <= 6)
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
                ch->dpoints = 5;
                send_to_char("When you've finished setting your backgrounds, you use the freebie command to spend freebies\n\r", ch);
                send_to_char(" to further increase your traits!\n\r",ch);
                ch->pcdata->progress = 8;
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

