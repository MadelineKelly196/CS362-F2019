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

int checkBaronEffect(int choice1, struct gameState *post, int p) {
    struct gameState pre;
    memcpy (&pre, post, sizeof(struct gameState));

    int r;
    //call baron function - under test
    r = baronEffect(choice1, post, p);

    pre.numBuys++;

    if(choice1 == 1)
    {
      //search hand for an estate
      if(pre.handCount[p]<=0)
      {
        choice1 = 0;
      }
      else
      {
        for(int i = 0; i<pre.handCount[p]; i++)
        {
          if(pre.hand[p][i] == estate)
          {
            //gain 4 coins
            pre.coins+=4;
            //discard estate
            pre.discard[p][pre.discardCount[p]] = pre.hand[p][i];
            pre.discardCount[p]++;
            //remove estate from hand
            pre.hand[p][i] = pre.hand[p][pre.handCount[p]-1];
            pre.hand[p][pre.handCount[p]-1] = -1;
            pre.handCount[p]--;
          }
          else if(i==(pre.handCount[p]-1))
          {
            choice1 = 0;
          }
        }
      }
    }
    //if choice is to gain Estate
    if(choice1 == 0)
    {
      //check supply count of estate and reduce by 1 if greater than 0
      if(pre.supplyCount[estate] > 0)
      {
        //reduce supply count
        pre.supplyCount[estate]--;
        //add estate to discard pile
        pre.discard[p][pre.discardCount[p]] = estate;
        pre.discardCount[p]++;
      }
    }


    myAssert (r == 0, __LINE__, __FILE__);

    int allSame = myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    if(allSame == -1 || r == -1)
    {
      printf("Expected Estate Supply Count: %d, Result: %d\n", pre.supplyCount[estate], post->supplyCount[estate]);
      printf("Expected Coins: %d, Result: %d\n", pre.coins, post->coins);
      printf("Expected Number of Buys: %d, Result: %d\n", pre.numBuys, post->numBuys);
      printf("Expected hand Count: %d, Result: %d\n", pre.handCount[p], post->handCount[p]);
      printf("Expected deck Count: %d, Result: %d\n", pre.deckCount[p], post->deckCount[p]);
      printf("Expected discard Count: %d, Result: %d\n", pre.discardCount[p], post->discardCount[p]);
    }
    return 0;
}

int main () {

    int i, n, r, p, deckCount, discardCount, handCount, choice1;

    int k[10] = {council_room, estate, feast, gardens, mine,
                 remodel, smithy, village, baron, great_hall
                };

    struct gameState G;

    printf ("Testing baronEffect.\n");

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
        G.numPlayers = rand()%(MAX_PLAYERS-1) + 2; //2-4 players
        p = rand()%G.numPlayers;
        G.coins = rand()%10;
        G.numBuys = rand()%5;
        //limit random space of each player's deck, discard, and hand size
        for(int m = 0; m<G.numPlayers; m++)
        {
          G.deckCount[m] = floor(Random() * MAX_DECK);
          G.discardCount[m] = floor(Random() * MAX_DECK);
          G.handCount[m] = rand()%6;

          //initialize each player's hand with cards from the specified game space
          for(int i=0; i< G.handCount[m]; i++)
          {
            G.hand[m][i] = k[rand()%10];
          }
        }
        //specify the size of each supply count for all card types in this game space
        for(int j=0; j<10; j++)
        {
          G.supplyCount[k[j]] = rand()%11;
        }
        //randomly choose to discard or not an estate from hand or choose an invalid number (non 0 or 1)
        choice1 = rand()%3;
        //print initial cards in hand of current player
        printf("Initial cards in hand of current player: ");
        for(int i=0; i< G.handCount[p]; i++)
        {
          G.hand[p][i] = k[rand()%10];
          printf("%d, ",G.hand[p][i]);
        }
        printf("\n");
        printf("Choice1: %d", choice1);
        //call tester function
        checkBaronEffect(choice1, &G, p);
    }

    printf ("ALL TESTS RAN!\n");

    exit(0);

}
