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

void deal_card		args((CHAR_DATA *ch, bool face_up));
void wingame		args((CHAR_DATA *ch));
void gamebust		args((CHAR_DATA *ch));
void game_stop();

void do_game(CHAR_DATA *ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    int num1, num2;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

	if(!str_prefix(arg1, "start"))
	{
		if(bets[0] != 0 && bets[1] != 0)
		{
			send_to_char( "The game has already started!\n\r", ch );
			return;
		}
		if (arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char( "The syntax is: game start [minbet] [maxbet]\n\r", ch );
			return;
		}
		if (is_number(arg2))
			num1 = atoi(arg2);
		else
		{
			send_to_char( "The syntax is: game start [minbet] [maxbet]\n\r", ch );
			return;
		}
		if (is_number(arg3))
			num2 = atoi(arg3);
		else
		{
			send_to_char( "The syntax is: game start [minbet] [maxbet]\n\r", ch );
			return;
		}
		if (num1 == 0 || num2 == 0)
		{
			send_to_char( "The bets must be a non-zero value.\n\r", ch );
			return;
		}
		if (num1 >= num2)
		{
			send_to_char( "The minbet must be higher then maxbet.\n\r", ch );
			return;
		}
		if(ch->gold < num1)
		{
			send_to_char( "The min bet cannot be higher then your current gold.\n\r", ch );
			return;
		}

		bets[0] = num1;
		bets[1] = num2;
		send_to_char( "Game started! Waiting for player two...\n\r", ch );
		act( "$n starts a game of Blackjack. Type 'game join' to join in the fun!", ch, NULL, NULL, TO_ROOM );
		gamer1 = ch;
		return;
	}

	if(!str_prefix(arg1, "join"))
	{
		if(bets[0] == 0 && bets[1] == 0)
		{
			send_to_char( "There isn't a game currently running.\n\r", ch );
			return;
		}

		if(ch->gold < bets[0])
		{
			send_to_char( "You don't have enough gold to meet the min bet!\n\r", ch );
			return;
		}

	/* take min bet and then deal two cards */
		gamer2 = ch;
		gamer1->gold -= bets[0];
		gamer2->gold -= bets[0];
		bets[2] += bets[0];
		bets[3] += bets[0];

		deal_card(gamer1, FALSE);
		deal_card(gamer1, FALSE);
		deal_card(gamer2, FALSE);
		deal_card(gamer2, FALSE);
		return;
	}

	if(!str_prefix(arg1, "check"))
	{
		if(bets[0] == 0 && bets[1] == 0)
		{
			act( "There is no game current running.", ch, NULL, NULL, TO_CHAR );
			return;
		}
		act( "There is currently a game running.", ch, NULL, NULL, TO_CHAR );
		sprintf( buf, "The current minimum bet is %d",bets[0]);
		act( buf, ch, NULL, NULL, TO_CHAR );
		sprintf( buf, "The current maximum bet is %d",bets[1]);
		act( buf, ch, NULL, NULL, TO_CHAR );
		sprintf( buf, "The current bet is Player 1: %d Player 2: %d",bets[2],bets[3]);
		act( buf, ch, NULL, NULL, TO_CHAR );
		sprintf( buf, "Player 1 currently has %d cards.",cards[0]);
		act( buf, ch, NULL, NULL, TO_CHAR );
		sprintf( buf, "Player 2 currently has %d cards.",cards[1]);
		act( buf, ch, NULL, NULL, TO_CHAR );

		if(ch == gamer1)
		{
			sprintf( buf, "Your current score is %d.",gamescore[0]);
			act( buf, ch, NULL, NULL, TO_CHAR );
		}

		if(ch == gamer2)
		{
			sprintf( buf, "Your current score is %d.",gamescore[1]);
			act( buf, ch, NULL, NULL, TO_CHAR );
		}

		return;
	}

	if(!str_prefix(arg1, "hit"))
	{
		if(ch == gamer1)
		{
			if(stay1 == TRUE)
			{
				send_to_char( "You cannot hit after you have stayed.\n\r", ch );
				return;
			}

			if(gamer1->gold < bets[0])
			{
				send_to_char( "You do not have enough gold to hit.\n\r", ch );
				return;
			}

			gamer1->gold -= bets[0];
			bets[2] += bets[0];
			deal_card(gamer1, TRUE);
			if(gamescore[0] >= 22)
				gamebust(gamer1);
		}
		if(ch == gamer2)
		{
			if(stay2 == TRUE)
			{
				send_to_char( "You cannot hit after you have stayed.\n\r", ch );
				return;
			}

			if(gamer2->gold < bets[0])
			{
				send_to_char( "You do not have enough gold to hit.\n\r", ch );
				return;
			}

			gamer2->gold -= bets[0];
			bets[3] += bets[0];
			deal_card(gamer2, TRUE);
			if(gamescore[1] >= 22)
				gamebust(gamer2);
		}
		return;
	}

	if(!str_prefix(arg1, "stay"))
	{
		if(ch == gamer1)
		{
			act( "$n places their cards face down and stays.", ch, NULL, NULL, TO_ROOM );
			act( "You place your cards face down and stay.", ch, NULL, NULL, TO_CHAR );
			stay1 = TRUE;
			return;
		}
		if(ch == gamer2)
		{
			act( "$n places their cards face down and stays.", ch, NULL, NULL, TO_ROOM );
			act( "You place your cards face down and stay.", ch, NULL, NULL, TO_CHAR );
			stay2 = TRUE;
			return;
		}
		return;
	}
	if(!str_prefix(arg1, "raise"))
	{
		int num;

		if (ch != gamer1 && ch != gamer2)
		{
			send_to_char( "You are not playing this game!\n\r", ch );
			return;
		}

		if (is_number(arg2))
			num = atoi(arg2);
		else
		{
			send_to_char( "The syntax is: game raise [newbet]\n\r", ch );
			return;
		}
		if (num > ch->gold)
		{
			send_to_char( "You do not have that much gold.\n\r", ch );
			return;
		}
		if (num > bets[1])
		{
			send_to_char( "You cannot go above max bet.\n\r", ch );
			return;
		}

		if(show1 == TRUE || show2 == TRUE)
		{
			act( "You cannot raise the bet after cards have been shown!", ch, NULL, NULL, TO_CHAR );
			return;
		}


		if (ch == gamer1)
		{
			if(stay1 != TRUE)
			{
				act( "You must stay first!", ch, NULL, NULL, TO_CHAR );
				return;
			}
			sprintf( buf, "$n increases the bet by %d",num);
			act( buf, ch, NULL, NULL, TO_ROOM );

			gamer1->gold -= num;
			bets[2] += num;
			if(gamer2->gold < num)
			{
				act( "And $N can't pay!", ch, NULL, gamer2, TO_ROOM );
				wingame(gamer1);
			}
			else
			{
				act( "And $N pays up.", ch, NULL, gamer2, TO_ROOM );
				gamer2->gold -= num;
				bets[3] += num;
				do_function(ch, &do_game, "show" );
			}
			return;
		}
		if (ch == gamer2)
		{
			if(show2 == TRUE)
			{
				act( "You cannot raise the bet after you have shown your cards!", ch, NULL, NULL, TO_CHAR );
				return;
			}

			if(stay2 != TRUE)
			{
				act( "You must stay first!", ch, NULL, NULL, TO_CHAR );
				return;
			}

			sprintf( buf, "$n increases the bet by %d",num);
			act( buf, ch, NULL, NULL, TO_ROOM );

			gamer2->gold -= num;
			bets[3] += num;
			if(gamer1->gold < num)
			{
				act( "And $N can't pay!", ch, NULL, gamer1, TO_ROOM );
				wingame(gamer2);
				return;
			}
			else
			{
				act( "And $N pays up.", ch, NULL, gamer1, TO_ROOM );
				gamer1->gold -= num;
				bets[2] += num;
				return;
			}
			return;
		}
	return;
	}
	if(!str_prefix(arg1, "show"))
	{
		if(ch == gamer1)
		{
			if(stay1 != TRUE)
			{
				send_to_char( "You must stay first.\n\r", ch );
				return;
			}
			show1 = TRUE;
			sprintf( buf, "$n shows their cards, they have a total of %d",gamescore[0]);
			act( buf, ch, NULL, NULL, TO_ROOM );
			sprintf( buf, "You show your cards, you have a total of %d",gamescore[0]);
			act( buf, ch, NULL, NULL, TO_CHAR );

			if(show1 == TRUE && show2 == TRUE)
			{
				if(gamescore[0] > gamescore[1])
					wingame(gamer1);
				if(gamescore[1] > gamescore[0])
					wingame(gamer2);
				if(gamescore[0] == gamescore[1])
				{
					act( "The game is a tie. The dealer keeps the bets.", ch, NULL, NULL, TO_ROOM );
					game_stop();
				}
				return;
			}
		return;
		}
		if(ch == gamer2)
		{
			if(stay2 != TRUE)
			{
				send_to_char( "You must stay first.\n\r", ch );
				return;
			}
			show2 = TRUE;
			sprintf( buf, "$n shows their cards, they have a total of %d",gamescore[1]);
			act( buf, ch, NULL, NULL, TO_ROOM );
			sprintf( buf, "You show your cards, you have a total of %d",gamescore[1]);
			act( buf, ch, NULL, NULL, TO_CHAR );

			if(show1 == TRUE && show2 == TRUE)
			{
				if(gamescore[0] > gamescore[1])
					wingame(gamer1);
				if(gamescore[1] > gamescore[0])
					wingame(gamer2);
				if(gamescore[0] == gamescore[1])
				{
					act( "The game is a tie. The dealer keeps the bets.", ch, NULL, NULL, TO_ROOM );
					game_stop();
				}
				return;
			}
		return;
		}
	return;
	}
	if(!str_prefix(arg1, "stop"))
	{
		act( "$n stops the game.", ch, NULL, NULL, TO_ROOM );
		act( "You stop the game.", ch, NULL, NULL, TO_CHAR );

	return;
	}

return;
}

void deal_card(CHAR_DATA *ch, bool face_up)
{
	char card_name[MAX_STRING_LENGTH];
	char card_suit[MAX_STRING_LENGTH];
	char card_face[MAX_STRING_LENGTH];
	int card, suit, total;

	card = number_range(1, 13);
	suit = number_range(1, 4);
	total = 0;

	if (delt1[card] == 4 || delt2[card] == 4)
	{
		if(face_up == TRUE)
			deal_card(ch, TRUE);
		else
			deal_card(ch, FALSE);
		return;
	}

/* store the suit of the card */
	if( suit == 1 )
		sprintf(card_suit, "{RHearts{x");
	if( suit == 2 )
		sprintf(card_suit, "{RDiamonds{x");
	if( suit == 3 )
		sprintf(card_suit, "{BSpades{x");
	if( suit == 4 )
		sprintf(card_suit, "{BClubs{x");

/* then the name and add the value to the total */
	if( card == 1)
	{
		sprintf(card_name, "Ace");
		if((gamescore[0] + 11) >= 22)
			total += 1;
		else if((gamescore[1] + 11) >= 22)
			total += 1;

		else
			total += 11;
	}
	if( card == 2)
	{
		sprintf(card_name, "Two");
		total += 2;
	}
	if( card == 3)
	{
		sprintf(card_name, "Three");
		total += 3;
	}
	if( card == 4)
	{
		sprintf(card_name, "Four");
		total += 4;
	}
	if( card == 5)
	{
		sprintf(card_name, "Five");
		total += 5;
	}
	if( card == 6)
	{
		sprintf(card_name, "Six");
		total += 6;
	}
	if( card == 7)
	{
		sprintf(card_name, "Seven");
		total += 7;
	}
	if( card == 8)
	{
		sprintf(card_name, "Eight");
		total += 8;
	}
	if( card == 9)
	{
		sprintf(card_name, "Nine");
		total += 9;
	}
	if( card == 10)
	{
		sprintf(card_name, "Ten");
		total += 10;
	}
	if( card == 11)
	{
		sprintf(card_name, "Jack");
		total += 10;
	}
	if( card == 12)
	{
		sprintf(card_name, "Queen");
		total += 10;
	}
	if( card == 13)
	{
		sprintf(card_name, "King");
		total += 10;
	}

/* add the name and suit together then do the messaging and junk */
	sprintf(card_face, "The %s of %s", card_name, card_suit);

	if(face_up == TRUE)
		act( "The dealer deals $n $t.", ch, card_face, NULL, TO_ROOM );
	else
		act( "The dealer deals $n a card.", ch, NULL, NULL, TO_ROOM );
	act( "The dealer deals you $t.", ch, card_face, NULL, TO_CHAR );

	/* set the card as delt and the proper total */
	if (gamer1 == ch)
	{
		delt1[card] += 1;
		gamescore[0] += total;
		cards[0] ++;
	}
	if (gamer2 == ch)
	{
		delt2[card] += 1;
		gamescore[1] += total;
		cards[1] ++;
	}
	if (cards[0] >= 5 || cards[1] >= 5)
	{
		act( "$n reaches 5 cards and instantly wins!", ch, NULL, NULL, TO_ROOM );
		act( "You reache 5 cards and instantly win!", ch, NULL, NULL, TO_CHAR );
		wingame(ch);
	}
	return;
}

void wingame(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	int winnings;

	winnings = bets[2]+bets[3];

	act( "$n has won the game!", ch, NULL, NULL, TO_ROOM );
	act( "You have won the game!", ch, NULL, NULL, TO_CHAR );
	sprintf( buf, "$n earns a total of %d gold coins!",winnings);
	act( buf, ch, NULL, NULL, TO_ROOM );
	sprintf( buf, "You earn a total of %d gold coins!",winnings);
	act( buf, ch, NULL, NULL, TO_CHAR );
	ch->gold += winnings;
	game_stop();
	return;
}
void gamebust(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	int charscore;

	charscore = 0;
	if(ch == gamer1)
		charscore = gamescore[0];
	if(ch == gamer2)
		charscore = gamescore[1];

	act( "$n scores over 21 and busts!", ch, NULL, NULL, TO_ROOM );
	act( "You score over 21 and bust!", ch, NULL, NULL, TO_CHAR );


	sprintf( buf, "$n shows their cards, they have a total of %d",charscore);
	act(buf, ch, NULL, NULL, TO_ROOM );
	sprintf( buf, "You show your cards, you have a total of %d",charscore);
	act(buf, ch, NULL, NULL, TO_CHAR );

	send_to_char( "You LOOSE!\n\r", ch );
	if(ch == gamer1)
		wingame(gamer2);
	if(ch == gamer2)
		wingame(gamer1);
	return;
}

void game_stop()
{
	int count;
	count = 0;
	while(count != 13)
	{
		delt1[count] = 0;
		count ++;
	}
	count = 0;
	while(count != 13)
	{
		delt2[count] = 0;
		count ++;
	}
	count = 0;
	while(count != 4)
	{
		bets[count] = 0;
		count ++;
	}
	gamescore[0] = 0;
	gamescore[1] = 0;
	cards[0] = 0;
	cards[1] = 0;
	gamer1 = NULL;
	gamer2 = NULL;
	show1 = FALSE;
	show2 = FALSE;
	stay1 = FALSE;
	stay2 = FALSE;
	return;
}

