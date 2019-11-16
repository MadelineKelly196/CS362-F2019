
/*
 * Include the following lines in your makefile:
 *
 * unittest1: unittest2.c dominion.c rngs.c
 *      gcc -o unittest2 -g  unittest2.c dominion.c rngs.c $(CFLAGS)
 */


#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include "myAssert.h"
#include "rngs.h"
#include <stdlib.h>

//#define TESTCARD "steward"
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

//test choice is to gain 2 coins
void posCase1(int choice1, int choice2, struct gameState *post, int currentPlayer) {
    printf("Starting posCase1: choice1 = 1 = +2 coins\n");

//set initial conditions
//------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = copper;
    post->hand[currentPlayer][1] = minion;
    post->hand[currentPlayer][2] = adventurer;
    post->hand[currentPlayer][3] = province;
    post->hand[currentPlayer][4] = silver;
    post->handCount[currentPlayer] = 5;

    post->hand[currentPlayer+1][0] = copper;
    post->hand[currentPlayer+1][1] = silver;
    post->hand[currentPlayer+1][2] = gold;
    post->hand[currentPlayer+1][3] = feast;
    post->hand[currentPlayer+1][4] = village;
    post->handCount[currentPlayer+1] = 5;

    struct gameState pre;
    memcpy(&pre, post, sizeof(struct gameState));

    int actionsBefore = post->numActions;
    int coinsBefore = post->coins;
    int r;

//run test function (minionEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run minionEffect()
    r = minionEffect(choice1, choice2, post, currentPlayer);

    //increase actions by 1
    pre.numActions++;
    //increase coins by 2
    pre.coins = pre.coins+2;

    //add minion to discard pile
    pre.discard[currentPlayer][pre.discardCount[currentPlayer]] = pre.hand[currentPlayer][1];
    pre.discardCount[currentPlayer]++;

    //remove minion from hand
    //replace minion card with card at end of hand
    pre.hand[currentPlayer][1] = pre.hand[currentPlayer][(pre.handCount[currentPlayer] - 1)];
    //set last card to -1
    pre.hand[currentPlayer][pre.handCount[currentPlayer] - 1] = -1;
    //reduce number of cards in hand
    pre.handCount[currentPlayer]--;

//check asserts
//-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of minionEffect() = 0, Result = %d\n", r);

    //checks that the number of actions was increased by 1
    myAssert ((post->numActions - actionsBefore) == (pre.numActions - actionsBefore), __LINE__, __FILE__);
    printf("\tExpected number of increased actions = %d, Result = %d\n", (pre.numActions - actionsBefore), (post->numActions - actionsBefore));

    //assert that number of cards in hand reduced by 1
    myAssert(post->handCount[currentPlayer] == pre.handCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected hand count = %d, Result = %d\n", pre.handCount[currentPlayer], post->handCount[currentPlayer]);

    //assert the minion card was removed from hands
    for (int m=0; m<post->handCount[currentPlayer]; m++)
    {
      int error = myAssert(post->hand[currentPlayer][m] != minion, __LINE__, __FILE__);
      printf("\tLooping through hand to check that there is no longer a Minion card in hand.\n");
      printf("\tWill kick out of loop if Minion card found.\n");
      if (error == -1)
      {
        printf("\tMinion card found in hand.\n");
        break;
      }
    }

    //assert that the number of cards in the current player's discard pile was increased by 1
    myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected number of cards in discard = %d, Results = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

    //assert that the discarded card is on top of discard pile
    myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)] == minion, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be a Minion, enumerated as %d, Result = %d\n", minion, post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)]);

    //assert that the top card of discard pile in post is same as pre
    myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)] == pre.discard[currentPlayer][(pre.discardCount[currentPlayer]-1)], __LINE__, __FILE__);
    printf("\tExpected card at top of discard pile to match conditions of control: control = %d, Results = %d\n", pre.discard[currentPlayer][(pre.discardCount[currentPlayer]-1)], post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)]);

    //assert that +4 coins was given for this round only
    myAssert((post->coins-coinsBefore) == (pre.coins-coinsBefore), __LINE__, __FILE__);
    printf("\tExpected number of coins added = %d, Results = %d\n", (pre.coins-coinsBefore), (post->coins-coinsBefore));

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End posCase1.\n");
}

//test choice is to discard hand, draw 4 and have next player, who has 5 cards in hand, do same
void posCase2(int choice1, int choice2, struct gameState *post, int currentPlayer) {
    printf("Starting posCase2: choice2 = 1 = discard hand, +4 cards, all other players with >=5 cards in hand do same, which in this case means all players\n");
    //set initial conditions
    //------------------------------------------------------------------------------
        post->hand[currentPlayer][0] = minion;
        post->hand[currentPlayer][1] = minion;
        post->hand[currentPlayer][2] = minion;
        post->hand[currentPlayer][3] = minion;
        post->hand[currentPlayer][4] = minion;
        post->handCount[currentPlayer] = 5;

        post->deck[currentPlayer][0] = gold;
        post->deck[currentPlayer][1] = gold;
        post->deck[currentPlayer][2] = gold;
        post->deck[currentPlayer][3] = gold;
        post->deck[currentPlayer][4] = gold;
        post->deckCount[currentPlayer] = 5;

        post->hand[currentPlayer+1][0] = copper;
        post->hand[currentPlayer+1][1] = copper;
        post->hand[currentPlayer+1][2] = copper;
        post->hand[currentPlayer+1][3] = copper;
        post->hand[currentPlayer+1][4] = copper;
        post->deckCount[currentPlayer+1] = 5;

        post->deck[currentPlayer+1][0] = gardens;
        post->deck[currentPlayer+1][1] = gardens;
        post->deck[currentPlayer+1][2] = gardens;
        post->deck[currentPlayer+1][3] = gardens;
        post->deck[currentPlayer+1][4] = gardens;
        post->deckCount[currentPlayer+1] = 5;

        struct gameState pre;
        memcpy(&pre, post, sizeof(struct gameState));

      //  int deckCountCurrentPlayerBefore = post->deckCount[currentPlayer];
      //  int deckCountNextPlayerBefore = post->deckCount[currentPlayer+1];
        //int handCountCurrentPlayerBefore = post->handCount[currentPlayer];
        //int handCountNextPlayerBefore = post->handCount[currentPlayer+1];
        int actionsBefore = post->numActions;
        int r;

    //run test function (baronEffect()) and set expected values
    //----------------------------------------------------------------------------------
        //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
        r = minionEffect(choice1, choice2, post, currentPlayer);

        //increase actions by 1
        pre.numActions++;

        //discard all cards in hand for both players, then draw 4 from deck
        for(int j = 0; j<2; j++)
        {
          for(int i = 0; i<5; i++)
          {
            pre.discard[currentPlayer+j][pre.discardCount[currentPlayer+j]] = pre.hand[currentPlayer+j][i];
            pre.discardCount[currentPlayer+j]++;

            //remove minion from hand
            //replace minion card with card at end of hand
            pre.hand[currentPlayer+j][i] = pre.hand[currentPlayer+j][(pre.handCount[currentPlayer+j] - 1)];
            //set last card to -1
            pre.hand[currentPlayer+j][pre.handCount[currentPlayer+j] - 1] = -1;
            //reduce number of cards in hand
            pre.handCount[currentPlayer+j]--;
          }
          for(int m = 0; m<2; m++)
          {
            //put card from deck into hand
            pre.hand[currentPlayer][m] = pre.deck[currentPlayer][(pre.deckCount[currentPlayer] - 1)];
            pre.handCount[currentPlayer]++;
            //remove drawn card from deck
            //set last card to -1
            pre.deck[currentPlayer][pre.deckCount[currentPlayer] - 1] = -1;
            pre.deckCount[currentPlayer]--;
          }
        }

    //check asserts
    //-----------------------------------------------------------------
        //make sure the function thinks it executed correctly
        myAssert(r==0, __LINE__, __FILE__);
        printf("\tExpected return of minionEffect() = 0, Result = %d\n", r);

        //checks that the number of actions was increased by 1
        myAssert ((post->numActions - actionsBefore) == (pre.numActions - actionsBefore), __LINE__, __FILE__);
        printf("\tExpected number of increased actions = %d, Result = %d\n", (pre.numActions - actionsBefore), (post->numActions - actionsBefore));

        //assert that number of cards in hand is 4
        myAssert(post->handCount[currentPlayer] == pre.handCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected hand count of current player = %d, Result = %d\n", pre.handCount[currentPlayer], post->handCount[currentPlayer]);

        //assert that number of cards in hand is 4
        myAssert(post->handCount[currentPlayer+1] == pre.handCount[currentPlayer+1], __LINE__, __FILE__);
        printf("\tExpected hand count of second player = %d, Result = %d\n", pre.handCount[currentPlayer+1], post->handCount[currentPlayer+1]);

        //assert that all cards in hand are gold for current player
        for (int m=0; m<post->handCount[currentPlayer]; m++)
        {
          int error = myAssert(post->hand[currentPlayer][m] == gold, __LINE__, __FILE__);
          printf("\tLooping through current player's hand to check that all cards in hand are Gold, as expected given initial conditions.\n");
          printf("\tWill kick out of loop if non-Gold card found.\n");
          if (error == -1)
          {
            printf("\tNon-Gold card found in hand.\n");
            break;
          }
        }

        //assert that all cards in hand are gardens for second player
        for (int m=0; m<post->handCount[currentPlayer+1]; m++)
        {
          int error = myAssert(post->hand[currentPlayer+1][m] == gardens, __LINE__, __FILE__);
          printf("\tLooping through second player's hand to check that all cards in hand are Garden, as expected given initial conditions.\n");
          printf("\tWill kick out of loop if non-Garden card found.\n");
          if (error == -1)
          {
            printf("\tNon-Garden card found in hand.\n");
            break;
          }
        }

        //assert that the number of cards in the current player's discard pile was increased by 5
        myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected number of cards in discard of current player = %d, Results = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

        //assert that the number of cards in the second player's discard pile was increased by 5
        myAssert (post->discardCount[currentPlayer+1] == pre.discardCount[currentPlayer+1], __LINE__, __FILE__);
        printf("\tExpected number of cards in discard of second player = %d, Results = %d\n", pre.discardCount[currentPlayer+1], post->discardCount[currentPlayer+1]);

        //assert that the discarded card is on top of discard pile of current player
        for(int i = 1; i<6; i++)
        {
          int error = myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-i)] == minion, __LINE__, __FILE__);
          printf("\tExpect that top 5 cards of discard pile of current player should be a Minion, enumerated as %d, Result = %d\n", minion, post->discard[currentPlayer][(post->discardCount[currentPlayer]-i)]);
          printf("\tWill kick out of loop if non-Minion card found.\n");
          if (error == -1)
          {
            printf("\tNon-Minion card found in hand.\n");
            break;
          }
        }

        //assert that the discarded card is on top of discard pile of second player
        for(int i = 1; i<6; i++)
        {
          int error = myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-i)] == copper, __LINE__, __FILE__);
          printf("\tExpect that top 5 cards of discard pile of second player should be a Copper, enumerated as %d, Result = %d\n", copper, post->discard[currentPlayer+1][(post->discardCount[currentPlayer]-i)]);
          printf("\tWill kick out of loop if non-Copper card found.\n");
          if (error == -1)
          {
            printf("\tNon-Copper card found in hand.\n");
            break;
          }
        }

        //assert that the number of cards in the current player's deck is 1
        myAssert (post->deckCount[currentPlayer] == pre.deckCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected number of cards in deck of current player = %d, Results = %d\n", pre.deckCount[currentPlayer], post->deckCount[currentPlayer]);

        //assert that the number of cards in the second player's deck is 1
        myAssert (post->deckCount[currentPlayer+1] == pre.deckCount[currentPlayer+1], __LINE__, __FILE__);
        printf("\tExpected number of cards in deck of second player = %d, Results = %d\n", pre.deckCount[currentPlayer+1], post->deckCount[currentPlayer+1]);


        //checks that the pre and post game states are equivalent
        //thereby verifying that nothing else changed in the gameState
        myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
        printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
        printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

        printf("End posCase2.\n");
}

//test choice is to discard hand, draw 4 and have next player, who has 4 cards in hand, do nothing
void posCase3(int choice1, int choice2, struct gameState *post, int currentPlayer) {
    printf("Starting posCase3: choice2 = 1 = discard hand, +4 cards, all other players with >=5 cards in hand do same, which in this case means only current player\n");
    //set initial conditions
    //------------------------------------------------------------------------------
        post->hand[currentPlayer][0] = minion;
        post->hand[currentPlayer][1] = minion;
        post->hand[currentPlayer][2] = minion;
        post->hand[currentPlayer][3] = minion;
        post->hand[currentPlayer][4] = minion;
        post->handCount[currentPlayer] = 5;

        post->deck[currentPlayer][0] = gold;
        post->deck[currentPlayer][1] = gold;
        post->deck[currentPlayer][2] = gold;
        post->deck[currentPlayer][3] = gold;
        post->deck[currentPlayer][4] = gold;
        post->deckCount[currentPlayer] = 5;

        post->hand[currentPlayer+1][0] = copper;
        post->hand[currentPlayer+1][1] = copper;
        post->hand[currentPlayer+1][2] = copper;
        post->hand[currentPlayer+1][3] = copper;
        post->deckCount[currentPlayer+1] = 4;

        post->deck[currentPlayer+1][0] = gardens;
        post->deck[currentPlayer+1][1] = gardens;
        post->deck[currentPlayer+1][2] = gardens;
        post->deck[currentPlayer+1][3] = gardens;
        post->deck[currentPlayer+1][4] = gardens;
        post->deckCount[currentPlayer+1] = 5;

        struct gameState pre;
        memcpy(&pre, post, sizeof(struct gameState));

      //  int deckCountCurrentPlayerBefore = post->deckCount[currentPlayer];
      //  int deckCountNextPlayerBefore = post->deckCount[currentPlayer+1];
        //int handCountCurrentPlayerBefore = post->handCount[currentPlayer];
        //int handCountNextPlayerBefore = post->handCount[currentPlayer+1];
        int actionsBefore = post->numActions;
        int r;

    //run test function (baronEffect()) and set expected values
    //----------------------------------------------------------------------------------
        //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
        r = minionEffect(choice1, choice2, post, currentPlayer);

        //increase actions by 1
        pre.numActions++;

        //discard all cards in hand for current player only, then draw 4 from deck
        for(int j = 0; j<1; j++)
        {
          for(int i = 0; i<5; i++)
          {
            pre.discard[currentPlayer+j][pre.discardCount[currentPlayer+j]] = pre.hand[currentPlayer+j][i];
            pre.discardCount[currentPlayer+j]++;

            //remove minion from hand
            //replace minion card with card at end of hand
            pre.hand[currentPlayer+j][i] = pre.hand[currentPlayer+j][(pre.handCount[currentPlayer+j] - 1)];
            //set last card to -1
            pre.hand[currentPlayer+j][pre.handCount[currentPlayer+j] - 1] = -1;
            //reduce number of cards in hand
            pre.handCount[currentPlayer+j]--;
          }
          for(int m = 0; m<4; m++)
          {
            //put card from deck into hand
            pre.hand[currentPlayer+j][m] = pre.deck[currentPlayer+j][m];
            pre.handCount[currentPlayer+j]++;
            //remove drawn card from deck
            //replace drawn card with card at end of deck
            pre.deck[currentPlayer+j][m] = pre.deck[currentPlayer+j][(pre.deckCount[currentPlayer+j] - 1)];
            //set last card to -1
            pre.deck[currentPlayer+j][pre.deckCount[currentPlayer+j] - 1] = -1;
            pre.deckCount[currentPlayer+j]--;
          }
        }

    //check asserts
    //-----------------------------------------------------------------
        //make sure the function thinks it executed correctly
        myAssert(r==0, __LINE__, __FILE__);
        printf("\tExpected return of minionEffect() = 0, Result = %d\n", r);

        //checks that the number of actions was increased by 1
        myAssert ((post->numActions - actionsBefore) == (pre.numActions - actionsBefore), __LINE__, __FILE__);
        printf("\tExpected number of increased actions = %d, Result = %d\n", (pre.numActions - actionsBefore), (post->numActions - actionsBefore));

        //assert that number of cards in hand is 4
        myAssert(post->handCount[currentPlayer] == pre.handCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected hand count of current player = %d, Result = %d\n", pre.handCount[currentPlayer], post->handCount[currentPlayer]);

        //assert that all cards in hand are gold for current player
        for (int m=0; m<post->handCount[currentPlayer]; m++)
        {
          int error = myAssert(post->hand[currentPlayer][m] == gold, __LINE__, __FILE__);
          printf("\tLooping through current player's hand to check that all cards in hand are Gold, as expected given initial conditions.\n");
          printf("\tWill kick out of loop if non-Gold card found.\n");
          if (error == -1)
          {
            printf("\tNon-Gold card found in hand.\n");
            break;
          }
        }

        //assert that the number of cards in the current player's discard pile was increased by 5
        myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected number of cards in discard of current player = %d, Results = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

        //assert that the discarded card is on top of discard pile of current player
        for(int i = 1; i<6; i++)
        {
          int error = myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-i)] == minion, __LINE__, __FILE__);
          printf("\tExpect that top 5 cards of discard pile of current player should be a Minion, enumerated as %d, Result = %d\n", minion, post->discard[currentPlayer][(post->discardCount[currentPlayer]-i)]);
          printf("\tWill kick out of loop if non-Minion card found.\n");
          if (error == -1)
          {
            printf("\tNon-Minion card found in hand.\n");
            break;
          }
        }

        //assert that the number of cards in the current player's deck is 1
        myAssert (post->deckCount[currentPlayer] == pre.deckCount[currentPlayer], __LINE__, __FILE__);
        printf("\tExpected number of cards in deck of current player = %d, Results = %d\n", pre.deckCount[currentPlayer], post->deckCount[currentPlayer]);

        //checks that the pre and post game states are equivalent
        //thereby verifying that nothing else changed in the gameState
        myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
        printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
        printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

        printf("End posCase3.\n");
}

//test choices out of bounds
void negCase(int choice1, int choice2, struct gameState *post, int currentPlayer) {
    //set initial conditions
    //------------------------------------------------------------------------------
        post->hand[currentPlayer][0] = minion;
        post->hand[currentPlayer][1] = minion;
        post->hand[currentPlayer][2] = minion;
        post->hand[currentPlayer][3] = minion;
        post->hand[currentPlayer][4] = minion;
        post->handCount[currentPlayer] = 5;

        post->deck[currentPlayer][0] = gold;
        post->deck[currentPlayer][1] = gold;
        post->deck[currentPlayer][2] = gold;
        post->deck[currentPlayer][3] = gold;
        post->deck[currentPlayer][4] = gold;
        post->deckCount[currentPlayer] = 5;

        post->hand[currentPlayer+1][0] = copper;
        post->hand[currentPlayer+1][1] = copper;
        post->hand[currentPlayer+1][2] = copper;
        post->hand[currentPlayer+1][3] = copper;
        post->deckCount[currentPlayer+1] = 4;

        post->deck[currentPlayer+1][0] = gardens;
        post->deck[currentPlayer+1][1] = gardens;
        post->deck[currentPlayer+1][2] = gardens;
        post->deck[currentPlayer+1][3] = gardens;
        post->deck[currentPlayer+1][4] = gardens;
        post->deckCount[currentPlayer+1] = 5;

        struct gameState pre;
        memcpy(&pre, post, sizeof(struct gameState));

      //  int deckCountCurrentPlayerBefore = post->deckCount[currentPlayer];
      //  int deckCountNextPlayerBefore = post->deckCount[currentPlayer+1];
        //int handCountCurrentPlayerBefore = post->handCount[currentPlayer];
        //int handCountNextPlayerBefore = post->handCount[currentPlayer+1];
        int actionsBefore = post->numActions;
        int r;

    //run test function (baronEffect()) and set expected values
    //----------------------------------------------------------------------------------
        //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
        r = minionEffect(choice1, choice2, post, currentPlayer);

        //increase actions by 1
        pre.numActions++;

    //check asserts
    //-----------------------------------------------------------------
        //make sure the function thinks it executed correctly
        myAssert(r==-1, __LINE__, __FILE__);
        printf("\tExpected return of minionEffect() = -1 or error message to pop up, Result = %d\n", r);

        //checks that the number of actions was increased by 1
        myAssert ((post->numActions - actionsBefore) == (pre.numActions - actionsBefore), __LINE__, __FILE__);
        printf("\tExpected number of increased actions = %d, Result = %d\n", (pre.numActions - actionsBefore), (post->numActions - actionsBefore));

        //checks that the pre and post game states are equivalent
        //thereby verifying that nothing else changed in the gameState
        myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
        printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
        printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

  }

int main() {
  int numPlayers = 2;
  int currentPlayer = 0;
  int seed = 1000;

  int k[10] = {minion, council_room, feast, gardens, mine,
               remodel, smithy, village, baron, great_hall
              };

  struct gameState G1, G2, G3, G4, G5, G6;

  printf ("Starting unittest2 for minionEffect().\n");

  //initialize game states
  initializeGame(numPlayers, k, seed, &G1);
  initializeGame(numPlayers, k, seed, &G2);
  initializeGame(numPlayers, k, seed, &G3);
  initializeGame(numPlayers, k, seed, &G4);
  initializeGame(numPlayers, k, seed, &G5);
  initializeGame(numPlayers, k, seed, &G6);
  posCase1(1, 0, &G1, currentPlayer);
  posCase2(0, 1, &G2, currentPlayer);
  posCase3(0, 1, &G3, currentPlayer);
  printf("Starting negCase1: choice1 = 2 = invalid entry - expect error\n");
  negCase(2, 0, &G4, currentPlayer);
  printf("End negCase1.\n");
  printf("Starting negCase2: choice2 = 2 = invalid entry - expect error\n");
  negCase(0, 2, &G5, currentPlayer);
  printf("End negCase2.\n");
  printf("Starting negCase3: choice1 = choice2 = 0 = invalid entry - expect error\n");
  negCase(0, 0, &G6, currentPlayer);
  printf("End negCase3.\n");
  printf("Ending unittest2 for minionEffect().\n");

  exit(0);
}
