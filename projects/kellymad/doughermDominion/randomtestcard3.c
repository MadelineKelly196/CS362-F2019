#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "rngs.h"
#include <stdlib.h>
#include <math.h>
#include "myAssert.h"

#define DEBUG 0
#define NOISY_TEST 1

int myAssert(int expression, int line, const char *file){
  if(expression==0){
    printf("Error found on line %d while running %s:\n", line, file);
    return -1;
  }
  else{
    printf("PASS!\n");
    return 0;
  }
}

void draw2Cards(int p, struct gameState *pre, struct gameState *post)
{
  for(int n = 0; n<2; n++)
  {
    if(pre->deckCount[p]>0)
    {
      //put card from deck into hand
      pre->hand[p][pre->handCount[p]] = pre->deck[p][(pre->deckCount[p] - 1)];
      pre->handCount[p]++;
      //remove drawn card from deck
      //set last card to -1
      pre->deck[p][pre->deckCount[p] - 1] = -1;
      pre->deckCount[p]--;
    }
    //if deck is empty, but discard is not, shuffle discard into deck
    else if(pre->discardCount[p] >0)
    {
      //since shuffle is random, we'll need to copy the results of this on post into pre
      memcpy(pre->deck[p], post->deck[p], sizeof(int) * pre->discardCount[p]);
      memcpy(pre->discard[p], post->discard[p], sizeof(int)*pre->discardCount[p]);
      pre->hand[p][pre->handCount[p]] = post->hand[p][post->handCount[p]-1];
      pre->handCount[p]++;
      pre->deckCount[p] = pre->discardCount[p]-1;
      pre->discardCount[p] = 0;
    }
  }
}

void revealCards(int p, struct gameState *pre, int tributeRevealedCards[])
{
  for(int i = 0; i<2; i++)
  {
    //if the deck is not empty
    if(pre->deckCount[p]>0)
    {
      //store card value in tributeRevealedCards Array
      tributeRevealedCards[i] = pre->deck[p][pre->deckCount[p]-1];
      //add card to discard
      pre->discard[p][pre->discardCount[p]] = pre->deck[p][pre->deckCount[p]-1];
      pre->discardCount[p]++;
      //remove drawn card from deck
      //set last card to -1
      pre->deck[p][pre->deckCount[p] - 1] = -1;
      pre->deckCount[p]--;
    }
    else if(pre->discardCount[p] >0)
    {
      //store card value in tributeRevealedCards Array
      tributeRevealedCards[i] = pre->discard[p][pre->discardCount[p]-1];
      //then leave it in discard, so nothing else needs to change
    }
  }
}

int checkTributeEffect(struct gameState *post, int p) {
    struct gameState pre;
    memcpy (&pre, post, sizeof(struct gameState));

    int r, i, np;
    //determine the index of the next player np
    if(p < pre.numPlayers-1)
    {
      np = p+1;
    }
    else if (p==pre.numPlayers-1)
    {
      np = 0;
    }

    //create arrays to store value of revealed cards
    //int tributeRevealedCardsPost[2] = {-1, -1};
    int tributeRevealedCardsPre[2] = {-1, -1};

    //call tribute function - under test
    r = tributeCard(post, np, p);

    revealCards(np, &pre, tributeRevealedCardsPre);

    //If the two revealed cards are equal, only get affect of one
    if(tributeRevealedCardsPre[0] == tributeRevealedCardsPre[1])
    {
      tributeRevealedCardsPre[1] = -1;
    }

    for(int i = 0; i<2; i++)
    {
      //for the case of a treasure card
      if(tributeRevealedCardsPre[i] == copper || tributeRevealedCardsPre[i] == silver || tributeRevealedCardsPre[i] == gold)
      {
        pre.coins +=2;
      }
      //for the case of a victory card
      else if(tributeRevealedCardsPre[i] == estate || tributeRevealedCardsPre[i] == duchy || tributeRevealedCardsPre[i] == province)
      {
        draw2Cards(p, &pre, post);
      }
      //for the case of an action card (note here I've only checked for the action cards I know to be valid cards in this game space, as set in my initial conditions)
      else if(tributeRevealedCardsPre[i] == tribute || tributeRevealedCardsPre[i] == mine || tributeRevealedCardsPre[i] == remodel || tributeRevealedCardsPre[i] == great_hall)
      {
        pre.numActions+=2;
      }
    }

    //check that function thinks it executed correctly
    myAssert (r == 0, __LINE__, __FILE__);

    //confirm that both game states are the same
    int allSame = myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    if(allSame == -1 || r == -1)
    {
      //printf("Expected tributeRevealedCard[0]: %d, Result: %d\n", tributeRevealedCardsPre[0], tributeRevealedCardsPost[0]);
      //printf("Expected tributeRevealedCard[1]: %d, Result: %d\n", tributeRevealedCardsPre[1], tributeRevealedCardsPost[1]);
      printf("Expected Number of Actions: %d, Result: %d\n", pre.numActions, post->numActions);
      printf("Expected Coins: %d, Result: %d\n", pre.coins, post->coins);
      printf("Expected hand Count: %d, Result: %d\n", pre.handCount[p], post->handCount[p]);
      printf("Expected deck Count: %d, Result: %d\n", pre.deckCount[p], post->deckCount[p]);
      printf("Expected discard Count: %d, Result: %d\n", pre.discardCount[p], post->discardCount[p]);
    }
    return 0;
}

int main () {

    int i, n, r, p, deckCount, discardCount, handCount, choice1, choice2;

    int k[10] = {province, estate, duchy, tribute, mine,
                 remodel, copper, silver, gold, great_hall
                };

    struct gameState G;

    printf ("Testing tributeEffect.\n");

    printf ("RANDOM TESTS.\n");

    srand(time(0));
    SelectStream(2);
    PutSeed(3);

    //run many scenarios to try to maximize testing space
    for (n = 0; n < 50; n++) {
      //initialize all aspects of game state with random values
        for (i = 0; i < sizeof(struct gameState); i++) {
            ((char*)&G)[i] = floor(Random() * 256);
        }
        //narrow random space for some specified parameters
        G.numPlayers = (rand()%(MAX_PLAYERS-1)) + 2; //2-4 players
        p = rand()%G.numPlayers;
        G.whoseTurn = p;
        G.numActions = rand()%10;
        G.coins = rand()%10;
        G.playedCardCount = floor(Random() * MAX_DECK);

        //limit the random space of each player's deck, discard, and hand count
        for(int m = 0; m<G.numPlayers; m++)
        {
          G.deckCount[m] = rand()%10;
          G.discardCount[m] = rand()%10;
          G.handCount[m] = (rand()%9) + 1;
          //initialize each player's hand with valid cards
          for(int i=0; i< G.handCount[m]; i++)
          {
            G.hand[m][i] = k[rand()%10];
          }
          //initialize each player's discard with valid cards
          for(int i=0; i< G.discardCount[m]; i++)
          {
            G.discard[m][i] = k[rand()%10];
          }
          //initialize each player's deck with valid cards
          for(int i=0; i< G.deckCount[m]; i++)
          {
            G.deck[m][i] = k[rand()%10];
          }
        }
        //initialize supply count of each valid card in game space
        for(int j=0; j<10; j++)
        {
          G.supplyCount[k[j]] = rand()%11;
        }
        //call check function
        checkTributeEffect(&G, p);
    }

    printf ("ALL TESTS RAN!\n");

    exit(0);

}
