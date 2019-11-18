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

//function to discard entire hand
void discardHand(int p, struct gameState *pre)
{
  while(pre->handCount[p] >0)
  {
    //add card to discard
    pre->discard[p][pre->discardCount[p]] = pre->hand[p][pre->handCount[p]-1];
    pre->discardCount[p]++;
    //remove card from hand
    pre->hand[p][pre->handCount[p]-1] = -1;
    pre->handCount[p]--;
  }
}

//function to draw 4 cards
void draw4Cards(int p, struct gameState *pre, struct gameState *post)
{
  for(int n = 0; n<4; n++)
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

int checkMinionEffect(int choice1, int choice2, struct gameState *post, int handPos, int p) {
    struct gameState pre;
    memcpy (&pre, post, sizeof(struct gameState));

    int r;
    //call minion function - under test
    r = minionEffect(choice1, choice2, post, handPos, p);

    //change pre to reflect expected outcomes of minion
    pre.numActions++;
    //discard minion
    pre.discard[p][pre.discardCount[p]] = pre.hand[p][handPos];
    pre.discardCount[p]++;
    //remove minion from hand
    pre.hand[p][handPos] = pre.hand[p][pre.handCount[p]-1];
    pre.hand[p][pre.handCount[p]-1] = -1;
    pre.handCount[p]--;

    if(choice1 != choice2)
    {
      if(choice1)
      {
        pre.coins+=2;
      }
      else if(choice2)
      {
        //discard all cards from hand
        discardHand(p, &pre);
        //draw 4 more cards
        draw4Cards(p, &pre, post);

        //check other player's hands for more than 5 cards and discard hands if so
        for(int j = 0; j<pre.numPlayers; j++)
        {
          //if not the current player
          if(j != p)
          {
            //hand count is 5 or more
            if(pre.handCount[j] > 4)
            {
              //discard all cards from hand
              discardHand(j, &pre);
              //draw 4 more cards
              draw4Cards(j, &pre, post);
            }
          }
        }
      }
    }


    //check that function thinks it executed correctly
    myAssert (r == 0, __LINE__, __FILE__);

    //confirm that both game states are the same
    int allSame = myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    if(allSame == -1 || r == -1)
    {
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

    int k[10] = {council_room, estate, feast, minion, mine,
                 remodel, smithy, village, baron, great_hall
                };

    struct gameState G;

    printf ("Testing minionEffect.\n");

    printf ("RANDOM TESTS.\n");

    srand(time(0));
    SelectStream(2);
    PutSeed(3);

    //run many scenarios to try to maximize testing space
    for (n = 0; n < 20; n++) {
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

        //limit random space of each player's deck, discard, and hand size
        for(int m = 0; m<G.numPlayers; m++)
        {
          G.deckCount[m] = floor(Random() * MAX_DECK);
          G.discardCount[m] = floor(Random() * MAX_DECK);
          G.handCount[m] = (rand()%9) + 1;

          //initialize all player's hands with valid cards
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
        //initialize supply count of each card type
        for(int j=0; j<10; j++)
        {
          G.supplyCount[k[j]] = rand()%11;
        }
        //choose to gain two coins or choose an invalid number (non 0 or 1)
        choice1 = rand()%3;
        //choose to discard hand and draw 4, along with other players with 5+ handCount, or choose an invalid number (non 0 or 1)
        choice2 = rand()%3;
        //ensure there is a minion in hand
        int handPos = rand()%G.handCount[p];
        G.hand[p][handPos] = minion;
        printf("handPos of minion card in current Player's hand: %d\n", handPos);
        printf("Choice1: %d", choice1);
        printf("Choice2: %d", choice2);
        //call check function
        checkMinionEffect(choice1, choice2, &G, handPos, p);
    }

    printf ("ALL TESTS RAN!\n");

    exit(0);

}
