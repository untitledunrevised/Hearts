/*
	Name: Hearts
	FileName: uuhearts.cpp
	Author: Untitled_unrevised
	Description: Making a poker game "hearts"
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "uuhearts.h"

/*
	kinds:
		club: 0, diamond: 1, spade: 2, heart: 3
	numbers:
		2: 0, 3: 1, ..., 10: 8, Jack: 9, Queen: 10, King: 11, Ace: 12
	cards:
		2 of clubs: 0
		3 of clubs: 1
		4 of clubs: 2
		...
		Ace of clubs: 12
		2 of diamonds: 13
		3 of diamonds: 14
		...
		Ace of diamonds: 25
		2 of spades: 26
		...
		Queen of spades: 36
		King of spades: 37
		Ace of spades: 38
		2 of hearts: 39
		...
		Ace of hearts: 51
*/

const int UU_SOUTH = 0;
const int UU_WEST = 1;
const int UU_NORTH = 2;
const int UU_EAST = 3;

static BOOL hascard[4][52];
static int firstplayer, formalplayer_related;
static int round, circle, maxround;
static int score[4];

DLLEXPORT void uu_hearts_init()
{
	int i, j;
	for(i = 0; i < 4; ++i)
		for(j = 0; j < 13; ++j)
			hascard[i][j] = FALSE;
	firstplayer = 0;
	formalplayer_related = 0;
	round = circle = 0;
}

DLLEXPORT void uu_hearts_start()
{
	srand((unsigned int)time(NULL));
	int i, cards[52];
	
	//random shuffle
	for(i = 0; i < 52; ++i)
	{
		cards[i] = i;
		int rnd = rand() % (i + 1);
		int tmp = p[i];
		p[i] = p[rnd];
		p[rnd] = tmp;
	}
	
	for(i = 0; i < 52; ++i)
	{
		hascard[i & 3][cards[i]] = 1;
		//The player who has "2 of clubs" is the first to play
		if(cards[i] == TRUE)
			firstplayer = i & 3;
	}
}

static int exchange_card[4][3];

DLLEXPORT BOOL uu_heart_need_exchange()
{
	return (round & 0x03) ^ 0x03;
}

DLLEXPORT BOOL uu_heart_exchange_request(
	const int playerid,
	const int firstcard,
	const int secondcard,
	const int thirdcard)
{
	if(card[playerid][firstcard] == 0
	|| card[playerid][secondcard] == 0
	|| card[playerid][thirdcard] == 0)
	{
		fprintf(stderr, "Error! You dont have one of these cards!\n");
		return FALSE;
	}
	if(firstcard == secondcard
	|| secondcard == thirdcard
	|| thirdcard == firstcard)
	{
		fprintf(stderr, "Error! Request contains the same cards!\n");
		return FALSE;
	}
	exchange_card[playerid][0] = firstcard;
	exchange_card[playerid][1] = secondcard;
	exchange_card[playerid][2] = thirdcard;
	return TRUE;
}

DLLEXPORT void uu_heart_exchange()
{
	int relation = (round & 0x03) ^ ((round & 0x03) >> 1);
	int playerid = 0, cardcnt = 0;
	for(; playerid < 4; ++playerid)
	{
		for(; cardcnt < 3; ++cardcnt)
		{
			hascard[playerid][exchange_card[cardcnt]] = FALSE;
			hascard[(playerid + relation) & 0x03][exchange_card[cardcnt]] = TRUE;
			//The player who has "2 of clubs" is the first to play
			if(exchange_card[cardcnt] == 0)
				firstplayer = (playerid + relation) & 0x03;
		}
	}
}

static int formal_kind = 0;	//club
static int formal_score[4], formalscore;
static int formal_maxicard = -1;
static int formal_maxiplayer = -1;
static BOOL heartbreak = FALSE;
static BOOL roundover, gameover;

DLLEXPORT BOOL uu_heart_playcard(const int card)
{
	//test 1
	if(circle == 0 && formalplayer_related == 0 && card != 0)
	{
		fprintf(stderr, "Error! You must play \"2 of clubs\" first!\n");
		return FALSE;
	}
	
	int formalplayer = (firstplayer + formalplayer_related) & 0x03;
	
	if(hascard[formalplayer][card] == FALSE)
	{
		fprintf(stderr, "Error! You dont have this card!\n");
		return FALSE;
	}
	
	//test 2
	//This is the first player
	if(formalplayer_related == 0)
	{
		//The player is trying to play a heart card
		if(heartbreak == FALSE && (card / 13) == 3)
		{
			int cardtmp = 0;
			for(; cardtmp < 39; ++cardtmp)
			{
				//The player has other kinds of card(s)
				if(hascard[formalplayer][cardtmp] == TRUE)
				{
					fprintf(stderr, "Error! You have other kinds of card(s) to play!\n");
					return FALSE;
				}
			}
			//The player only has heart cards
			heartbreak = TRUE;
		}
		//set this circle's kind
		formal_kind = card / 13;
		formal_maxicard = card;
		formal_maxiplayer = formalplayer;
	}
	//other players
	else
	{
		//This player is trying to discard
		if(card / 13 != formal_kind)
		{
			int cardtmp = formal_kind * 13;
			for(; cardtmp < (formal_kind + 1) * 13; ++cardtmp)
			{
				//The player has the card the same kind as the first card
				if(hascard[formalplayer][cardtmp] == TRUE)
				{
					fprintf(stderr, "Error! You must play the card the same kind as the first card!\n");
					return FALSE;
				}
			}
			//discard
			//This is a heart card
			if(card / 13 == 3)
				heartbreak = TRUE;
		}
		else
		{
			if(card > formal_maxicard)
			{
				formal_maxicard = card;
				formal_maxiplayer = formalplayer;
			}
		}
	}
	
	//play
	hascard[formalplayer][card] = FALSE;
	
	//score
	//This is a heart card
	if(card / 13 == 3)
		++formalscore;
	//This is Queen of Spade
	if(card == 36)
		formalscore += 13;
	
	//next step
	++formalplayer_related;
	//this circle is over
	if(formalplayer_related >= 4)
	{
		++circle;
		formal_score[maxiplayer] += formalscore;
		formalscore = 0;
		firstplayer = maxiplayer;
		//this round is over
		if(circle >= 13)
		{
			roundover = TRUE;
			int pid = 0;
			for(; pid < 4; ++pid)
			{
				//Jackpot
				if(formal_score[pid] == 26)
				{
					int pp = 0;
					for(; pp < 4; ++pp)
						score[pp] += 26;
					score[pid] -= 52;
				}
				score[pid] += formal_score[pid];
			}
			++round;
			//game over
			if(round >= maxround)
			{
				gameover = TRUE;
			}
			circle = 0;
		}
		formalplayer_related = 0;
	}
	
	//playcard end
	return TRUE;
}

DLLEXPORT BOOL uu_heart_gameover() { return gameover; }
