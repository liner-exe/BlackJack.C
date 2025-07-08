#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 52
#define MAX_HAND_SIZE 10
#define EMPTY_STRING ""

typedef enum {
	HEARTS,
	SPADES,
	DIAMONDS,
	CLUBS
} Suit;

typedef struct {
	Suit suit;
	int value;
} Card;

int balance = 0;
int currentCardIndex = 0;
Card deck[DECK_SIZE];

Card playerHand[MAX_HAND_SIZE];
Card dealerHand[MAX_HAND_SIZE];
int playerHandCount = 0;
int dealerHandCount = 0;

void initDeck()
{
	int index = 0;

	for (Suit s = HEARTS; s <= CLUBS; s++)
	{
		for (int j = 1; j < DECK_SIZE / 4 + 1; j++)
		{
			deck[index].suit = s;
			deck[index].value = j;
			index++;
		}
	}
}

const char* getSuitCharacter(const Suit suit)
{
	switch (suit)
	{
		case HEARTS:
			return "\x03";
		case SPADES:
			return "\x04";
		case DIAMONDS:
			return "\x05";
		case CLUBS:
			return "\x06";
		default:
			return "ERR";
	}
}

const char* getCardNameByValue(const int value)
{
	switch (value)
	{
		case 1: return "A";
		case 11: return "J";
		case 12: return "Q";
		case 13: return "K";
		default: {
			static char buff[3];
			sprintf(buff, "%d", value);
			return buff;
		}
	}
}

void printCard(Card card)
{
	printf("%s%s", getSuitCharacter(card.suit),getCardNameByValue(card.value));
}

void displayDeck(const Card* deck)
{
	for (int i = 0; i < DECK_SIZE; i++)
	{
		printf("Card #%d: %s %s\n", i + 1, getSuitCharacter(deck[i].suit), getCardNameByValue(deck[i].value));
	}
}

void displayHand(const Card* hand, const int handCount, char* message)
{
	printf("%s", message);

	for (int i = 0; i < handCount; i++)
	{
		printCard(hand[i]);
		printf(" ");
	}

	printf("\n");
}

void displayDealerHandPartial(const Card* hand)
{
	printf("Dealer hand: X ");
	printCard(hand[1]);
}

void shuffleDeck(Card* deck)
{
	for (int i = DECK_SIZE - 1; i > 0; i--)
	{
		const int random = rand() % (i + 1);
		const Card temp = deck[i];
		deck[i] = deck[random];
		deck[random] = temp;
	}
}

Card drawCard()
{
	if (currentCardIndex >= 52)
	{
		shuffleDeck(deck);
		currentCardIndex = 0;
	}

	return deck[currentCardIndex++];
}

void dealCards(
	Card* playerHand, int* playerHandCount,
	Card* dealerHand, int* dealerHandCount
	)
{
	playerHand[(*playerHandCount)++] = drawCard();
	playerHand[(*playerHandCount)++] = drawCard();

	dealerHand[(*dealerHandCount)++] = drawCard();
	dealerHand[(*dealerHandCount)++] = drawCard();
}

int getCardValue(const Card card)
{
	if (card.value == 1) return 11;
	if (card.value >= 10) return 10;
	return card.value;
}

int evaluateCards(const Card* hand, const int handSize)
{
	if (hand == NULL) return -1;

	int cardsScore = 0;
	int countOfAces = 0;

	for (int i = 0; i < handSize; i++)
	{
		if (getCardValue(hand[i]) == 11)
		{
			countOfAces++;
		}
		cardsScore += getCardValue(hand[i]);
	}

	while (cardsScore > 21 && countOfAces > 0)
	{
		cardsScore -= 10;
		countOfAces--;
	}
	return cardsScore;
}

bool checkBlackJack(const Card* hand, const int handSize)
{
	return evaluateCards(hand, handSize) == 21;
}

bool checkBust(const Card* deck, const int handSize)
{
	return evaluateCards(deck, handSize) > 21;
}

bool equals(const char* originString, const char* subString)
{
	if (strlen(originString) != strlen(subString)) return false;

	for (int i = 0; i < strlen(originString); i++)
	{
		if (originString[i] != subString[i])
			return false;
	}

	return true;
}

void gameloop()
{
	char choice[2];
	int round = 1;
	int bet = 0;
	bool isPlayerStand = false;

	printf("Place your bet: ");
	scanf("%d", &bet);

	if (bet > balance || bet < 1)
	{
		printf("Invalid bet. Try again.\n");
		return;
	}

	dealCards(playerHand, &playerHandCount, dealerHand, &dealerHandCount);

	bool playerBlackjack = checkBlackJack(playerHand, playerHandCount);
	bool dealerBlackjack = checkBlackJack(dealerHand, dealerHandCount);

	if (playerBlackjack && !dealerBlackjack)
	{
		printf("You win!\n");
		balance += floor(bet * 1.5);
		return;
	}
	if (dealerBlackjack)
	{
		if (playerBlackjack) printf("Draw!\n");
		else printf("Dealer won!\n");
		return;
	}

	while (true)
	{
		printf("====== Round number %d ======\n", round);

		if (checkBlackJack(playerHand, playerHandCount))
		{
			printf("You win!\n");
			return;
		}

		displayHand(playerHand, playerHandCount, "Player hand: ");
		printf("Player score: %d\n", evaluateCards(playerHand, playerHandCount));
		displayDealerHandPartial(dealerHand);
		printf("\nDealer score: %d\n", evaluateCards(dealerHand, dealerHandCount));

		printf("(h)it or (s)tand?: ");
		scanf("%1s", &choice);

		if (choice[0] == 'h')
		{
			playerHand[playerHandCount++] = drawCard();

			if (evaluateCards(dealerHand, dealerHandCount) < 17)
			{
				dealerHand[dealerHandCount++] = drawCard();
			}
		}
		else if (choice[0] == 's')
		{
			isPlayerStand = true;

			if (evaluateCards(dealerHand, dealerHandCount) < 17)
			{
				dealerHand[dealerHandCount++] = drawCard();
				continue;
			}
		}

		// Win conditions check

		if (checkBlackJack(playerHand, playerHandCount) && !checkBlackJack(dealerHand, dealerHandCount))
		{
			printf("You win!\n");
			return;
		}

		if (checkBlackJack(dealerHand, dealerHandCount))
		{
			printf("Dealer won.\n");
			return;
		}

		if (checkBlackJack(playerHand, playerHandCount) && checkBlackJack(dealerHand, dealerHandCount))
		{
			printf("Draw!\n");
			return;
		}

		if (checkBust(playerHand, playerHandCount))
		{
			printf("Busted! You lose.\n");
			return;
		}

		if (checkBust(dealerHand, dealerHandCount))
		{
			printf("Dealer busted. You win!\n");
			return;
		}

		const int playerScore = evaluateCards(playerHand, playerHandCount);
		const int dealerScore = evaluateCards(dealerHand, dealerHandCount);

		if (playerScore > dealerScore)
		{
			printf("You win!\n");
		}
		else if (playerScore < dealerScore)
		{
			printf("You lose. Dealer won.\n");
		}
		else
		{
			printf("Draw!\n");
		}

		round++;

		return;
	}
}

int main()
{
	srand(time(NULL));

	char option[2];
	printf("Welcome to BlackJack! Developed by liner.exe\n\n");
	printf("Choose an option from below\n");
	printf("1. Play\n");
	printf("2. Options\n");
	printf("3. Exit\n");
	scanf("%1s", option);

	if (option[0] == '1')
	{
		balance = 500;
		initDeck();
		shuffleDeck(deck);
		gameloop();
	}
	else if (option[0] == '2')
	{
		printf("The selected option is not available yet!");
		return 1;
	}
	else if (option[0] == '3')
	{
		printf("Goodluck. Thank you for playing!");
		return 0;
	}

	printf("====== Results ======\n");
	printf("Balance: %d", balance);
	printf("Player | Dealer\n");
	printf("%2d     | %d ", evaluateCards(playerHand, playerHandCount), evaluateCards(dealerHand, dealerHandCount));

	return 0;
}