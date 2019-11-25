
/*
 * Include the following lines in your makefile:
 *
 * unittest1: unittest1.c dominion.c rngs.c
 *      gcc -o unittest1 -g  unittest1.c dominion.c rngs.c $(CFLAGS)
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

//function to test positive case 1 - where choice is to discarded
//Estate, Estate is found, discarded, and +4 temp coins granted
void posCase1(int choice1, struct gameState *post, int currentPlayer) {
    printf("Starting posCase1: choice1 = 1 = discard Estate and +4 coins\n");

//set initial conditions
//------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = copper;
    post->hand[currentPlayer][1] = estate;
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

    int coinsBefore = post->coins;
    int r;

//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = baronCard(choice1, post, currentPlayer);

    //increase buys by 1
    pre.numBuys++;
    //increase coins by 4
    pre.coins = pre.coins+4;

    //add Estate to discard pile
    pre.discard[currentPlayer][pre.discardCount[currentPlayer]] = pre.hand[currentPlayer][1];
    //printf("pre.hand[currentPlayer][0] = %d\n", pre.hand[currentPlayer][0]);
    //printf("pre.discard[currentPlayer][pre.discardCount[currentPlayer]] = %d\n", pre.discard[currentPlayer][pre.discardCount[currentPlayer]]);
    //printf("pre.discardCount[currentPlayer] = %d\n", pre.discardCount[currentPlayer]);
    pre.discardCount[currentPlayer]++;
    //printf("pre.discardCount[currentPlayer] = %d\n", pre.discardCount[currentPlayer]);

    //remove Estate from hand
    //replace Estate card with card at end of hand
    pre.hand[currentPlayer][1] = pre.hand[currentPlayer][(pre.handCount[currentPlayer] - 1)];
    //set last card to -1
    pre.hand[currentPlayer][pre.handCount[currentPlayer] - 1] = -1;
    //reduce number of cards in hand
    pre.handCount[currentPlayer]--;

//check asserts
//-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of baronEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert (post->numBuys == pre.numBuys, __LINE__, __FILE__);
    printf("\tExpected number of buys = %d, Result = %d\n", pre.numBuys, post->numBuys);

    //assert that number of cards in hand reduced by 1
    myAssert(post->handCount[currentPlayer] == pre.handCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected hand count = %d, Result = %d\n", pre.handCount[currentPlayer], post->handCount[currentPlayer]);

    //assert the estate card was removed from hands
    for (int m=0; m<post->handCount[currentPlayer]; m++)
    {
      int error = myAssert(post->hand[currentPlayer][m] != estate, __LINE__, __FILE__);
      printf("\tLooping through hand to check that there is no longer an Estate card in hand.\n");
      printf("\tWill kick out of loop is Estate card found.\n");
      if (error == -1)
      {
        printf("\tEstate card found in hand.\n");
        break;
      }
    }

    //assert that the number of cards in the current player's discard pile was increased by 1
    myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected number of cards in discard = %d, Results = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

    //assert that the discarded card is on top of discard pile
    myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)] == estate, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be an Estate, enumerated as %d, Result = %d\n", estate, post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)]);

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

//posCase2 is the case where the player chooses not to discard and thus gains an Estate card
void posCase2(int choice1, struct gameState *post, int currentPlayer) {
    printf("Starting posCase2: choice1 = 0 = gain Estate\n");
    int preTotalCountAfter, preTotalCountBefore, postTotalCountAfter, postTotalCountBefore, deltaPreCount, deltaPostCount;

  //set initial conditions
  //------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = copper;
    post->hand[currentPlayer][1] = mine;
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
    preTotalCountBefore = pre.handCount[currentPlayer]+pre.deckCount[currentPlayer]+pre.discardCount[currentPlayer];
    postTotalCountBefore = post->handCount[currentPlayer]+post->deckCount[currentPlayer]+post->discardCount[currentPlayer];

    //to store returned value of baronEffect()
    int r;

//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = baronCard(choice1, post, currentPlayer);

    //increase buys by 1
    pre.numBuys++;

    //gain Estate card from supply
    pre.discard[currentPlayer][ pre.discardCount[currentPlayer] ] = estate;
    pre.discardCount[currentPlayer]++;

    //decrease number in supply pile
    pre.supplyCount[estate]--;

    preTotalCountAfter = pre.handCount[currentPlayer]+pre.deckCount[currentPlayer]+pre.discardCount[currentPlayer];
    postTotalCountAfter = post->handCount[currentPlayer]+post->deckCount[currentPlayer]+post->discardCount[currentPlayer];

    deltaPreCount = preTotalCountAfter - preTotalCountBefore;
    deltaPostCount = postTotalCountAfter - postTotalCountBefore;
  //check asserts
  //-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of baronEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert (post->numBuys == pre.numBuys, __LINE__, __FILE__);
    printf("\tExpected number of buys = %d, Result = %d\n", pre.numBuys, post->numBuys);

    //checks that the discard pile count was successfully increased
    myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected number of cards in discard pile = %d, Result = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

    //assert that the discarded card is on top of discard pile
    myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)] == estate, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be an Estate, enumerated as %d, Result = %d\n", estate, post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)]);

    //checks that the estate card is at the top of the discard pile
    myAssert (post->discard[currentPlayer][post->discardCount[currentPlayer]-1] == pre.discard[currentPlayer][pre.discardCount[currentPlayer]-1], __LINE__, __FILE__);
    printf("\tExpected the top card of discard pile to be same as control: control = %d, Result = %d\n", pre.discard[currentPlayer][pre.discardCount[currentPlayer]-1], post->discard[currentPlayer][post->discardCount[currentPlayer]-1]);

    //checks that total player deck+discard+hand count increased by 1
    myAssert (deltaPostCount==deltaPreCount, __LINE__, __FILE__);
    printf("\tExpected total card count of current player to have increased by 1: control = %d, Result = %d\n",deltaPreCount, deltaPostCount);

    //check that supply count was successfully reduced
    myAssert (post->supplyCount[estate] == pre.supplyCount[estate], __LINE__, __FILE__);
    printf("\tExpected number of supplies in Estate supply pile = %d, Result = %d\n", pre.supplyCount[1], post->supplyCount[1]);

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End posCase2.\n");
}
//negCase is the case where the player chooses to discard an Estate but does not have one in hand
void negCase1(int choice1, struct gameState *post, int currentPlayer) {
    printf("Starting negCase1: choice1 = 1 but no Estate in hand.\n");
    printf("Since this has all the same initial set up, expected results, and asserts as posCase2\n");
    printf("I will simply run posCase2, with a changed input of choice = 1, instead of choice = 0.\n");
    posCase2(1, post, currentPlayer);
    printf("End negCase1.\n");
}

//where I choose to discard an Estate, don't have on in hand, but next player does
void negCase2(int choice1, struct gameState *post, int currentPlayer) {
    printf("Starting negCase2: choice1 = 1, no Estate in hand, but an estate is in hand of next player.\n");
    int preTotalCountAfter, preTotalCountBefore, postTotalCountAfter, postTotalCountBefore, deltaPreCount, deltaPostCount;

  //set initial conditions
  //------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = copper;
    post->hand[currentPlayer][1] = mine;
    post->hand[currentPlayer][2] = adventurer;
    post->hand[currentPlayer][3] = province;
    post->hand[currentPlayer][4] = silver;
    post->handCount[currentPlayer] = 5;

    post->hand[currentPlayer+1][0] = estate;
    post->hand[currentPlayer+1][1] = silver;
    post->hand[currentPlayer+1][2] = gold;
    post->hand[currentPlayer+1][3] = feast;
    post->hand[currentPlayer+1][4] = village;
    post->handCount[currentPlayer+1] = 5;

    struct gameState pre;
    memcpy(&pre, post, sizeof(struct gameState));
    preTotalCountBefore = pre.handCount[currentPlayer]+pre.deckCount[currentPlayer]+pre.discardCount[currentPlayer];
    postTotalCountBefore = post->handCount[currentPlayer]+post->deckCount[currentPlayer]+post->discardCount[currentPlayer];

    //to store returned value of baronEffect()
    int r;

//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = baronCard(choice1, post, currentPlayer);

    //increase buys by 1
    pre.numBuys++;

    //gain Estate card from supply
    pre.discard[currentPlayer][ pre.discardCount[currentPlayer] ] = 1;
    pre.discardCount[currentPlayer]++;

    //decrease number in supply pile
    pre.supplyCount[1]--;

    preTotalCountAfter = pre.handCount[currentPlayer]+pre.deckCount[currentPlayer]+pre.discardCount[currentPlayer];
    postTotalCountAfter = post->handCount[currentPlayer]+post->deckCount[currentPlayer]+post->discardCount[currentPlayer];

    deltaPreCount = preTotalCountAfter - preTotalCountBefore;
    deltaPostCount = postTotalCountAfter - postTotalCountBefore;
  //check asserts
  //-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of baronEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert (post->numBuys == pre.numBuys, __LINE__, __FILE__);
    printf("\tExpected number of buys = %d, Result = %d\n", pre.numBuys, post->numBuys);

    //checks that the discard pile count was successfully increased
    myAssert (post->discardCount[currentPlayer] == pre.discardCount[currentPlayer], __LINE__, __FILE__);
    printf("\tExpected number of cards in discard pile = %d, Result = %d\n", pre.discardCount[currentPlayer], post->discardCount[currentPlayer]);

    //assert that the discarded card is on top of discard pile
    myAssert(post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)] == estate, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be an Estate, enumerated as %d, Result = %d\n", estate, post->discard[currentPlayer][(post->discardCount[currentPlayer]-1)]);

    //checks that the estate card is at the top of the discard pile
    myAssert (post->discard[currentPlayer][post->discardCount[currentPlayer]-1] == pre.discard[currentPlayer][pre.discardCount[currentPlayer]-1], __LINE__, __FILE__);
    printf("\tExpected the top card of discard pile to be same as control: control = %d, Result = %d\n", pre.discard[currentPlayer][pre.discardCount[currentPlayer]-1], post->discard[currentPlayer][post->discardCount[currentPlayer]-1]);

    //checks that total player deck+discard+hand count increased by 1
    myAssert (deltaPostCount==deltaPreCount, __LINE__, __FILE__);
    printf("\tExpected total card count of current player to have increased by 1: control = %d, Result = %d\n",deltaPreCount, deltaPostCount);

    //check that supply count was successfully reduced
    myAssert (post->supplyCount[estate] == pre.supplyCount[estate], __LINE__, __FILE__);
    printf("\tExpected number of supplies in Estate supply pile = %d, Result = %d\n", pre.supplyCount[1], post->supplyCount[1]);

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End negCase2.\n");
}

//boundCase tests the result if choice1 is neither a 0 or 1 boolean value
void boundCase(int choice1, struct gameState *post, int currentPlayer) {
    printf("Starting boundCase: choice1 = 2 value out of bounds\n");

    //set initial conditions
    //------------------------------------------------------------------------------
      post->hand[currentPlayer][0] = copper;
      post->hand[currentPlayer][1] = mine;
      post->hand[currentPlayer][2] = adventurer;
      post->hand[currentPlayer][3] = province;
      post->hand[currentPlayer][4] = silver;
      post->handCount[currentPlayer] = 5;

      post->hand[currentPlayer+1][0] = estate;
      post->hand[currentPlayer+1][1] = silver;
      post->hand[currentPlayer+1][2] = gold;
      post->hand[currentPlayer+1][3] = feast;
      post->hand[currentPlayer+1][4] = village;
      post->handCount[currentPlayer+1] = 5;

      struct gameState pre;
      memcpy(&pre, post, sizeof(struct gameState));

      //to store returned value of baronEffect()
      int r;

  //run test function (baronEffect()) and set expected values
  //----------------------------------------------------------------------------------
      //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
      r = baronCard(choice1, post, currentPlayer);

      //increase buys by 1
      pre.numBuys++;
    //check asserts
    //-----------------------------------------------------------------
        //make sure the function thinks it executed correctly
        myAssert(r==-1, __LINE__, __FILE__);
        printf("\tExpected return of minionEffect() = -1 or error message to pop up, Result = %d\n", r);

        //checks that the number of actions was increased by 1
        myAssert (post->numBuys == pre.numBuys, __LINE__, __FILE__);
        printf("\tExpected number of increased actions = %d, Result = %d\n", pre.numBuys, post->numBuys);

        //checks that the pre and post game states are equivalent
        //thereby verifying that nothing else changed in the gameState
        myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
        printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
        printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

        printf("End boundCase.\n");
}

int main() {
  int numPlayers = 2;
  int currentPlayer = 0;
  int seed = 1000;

  int k[10] = {adventurer, council_room, feast, gardens, mine,
               remodel, smithy, village, baron, great_hall
              };

  struct gameState G1, G2, G3, G4, G5;

  printf ("Starting unittest1 for baronEffect().\n");

  //initialize game states
  initializeGame(numPlayers, k, seed, &G1);
  initializeGame(numPlayers, k, seed, &G2);
  initializeGame(numPlayers, k, seed, &G3);
  initializeGame(numPlayers, k, seed, &G4);
  initializeGame(numPlayers, k, seed, &G5);
  posCase1(1, &G1, currentPlayer);
  posCase2(0, &G2, currentPlayer);
  negCase1(1, &G3, currentPlayer);
  negCase2(1, &G4, currentPlayer);
  boundCase(2, &G5, currentPlayer);

  printf("Ending unittest1 for baronCard().\n");

  exit(0);
}
