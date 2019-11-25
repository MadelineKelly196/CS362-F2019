
/*
 * Include the following lines in your makefile:
 *
 * unittest4: unittest4.c dominion.c rngs.c
 *      gcc -o unittest4 -g  unittest4.c dominion.c rngs.c $(CFLAGS)
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

//tests effect of next player revealing an action and a victory card
void posCase1(struct gameState *post, int currentPlayer) {
    printf("Starting posCase1: village and estate revealed from next player's deck = +2 Actions and +2 cards\n");

//set initial conditions
//------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = gold;
    post->hand[currentPlayer][1] = gold;
    post->hand[currentPlayer][2] = gold;
    post->hand[currentPlayer][3] = gold;
    post->hand[currentPlayer][4] = tribute;
    post->handCount[currentPlayer] = 5;

    post->deck[currentPlayer][0] = silver;
    post->deck[currentPlayer][1] = silver;
    post->deck[currentPlayer][2] = silver;
    post->deck[currentPlayer][3] = silver;
    post->deck[currentPlayer][4] = silver;
    post->deckCount[currentPlayer] = 5;

    post->deck[currentPlayer+1][0] = copper;
    post->deck[currentPlayer+1][1] = copper;
    post->deck[currentPlayer+1][2] = copper;
    post->deck[currentPlayer+1][3] = estate;
    post->deck[currentPlayer+1][4] = village;
    post->deckCount[currentPlayer+1] = 5;

    post->discard[currentPlayer+1][0] = province;
    post->discard[currentPlayer+1][1] = adventurer;
    post->discard[currentPlayer+1][2] = province;
    post->discard[currentPlayer+1][3] = adventurer;
    post->discard[currentPlayer+1][4] = province;
    post->discard[currentPlayer+1][5] = adventurer;
    post->discardCount[currentPlayer+1] = 6;

    struct gameState pre;
    memcpy(&pre, post, sizeof(struct gameState));

    int handCountBefore = post->handCount[currentPlayer];
    int r;
    //int tributeRevealedCardsPost[2] = {-1, -1};
    int tributeRevealedCardsPre[2] = {-1, -1};
    int nextPlayer = currentPlayer+1;
    int i = 0;
//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = tributeCard(post, nextPlayer, currentPlayer);

    //increase buys by 1
    pre.numActions +=2;
    //draw two cards from deck
    for(int m = 0; m<2; m++)
    {
      //put card from deck into hand
      pre.hand[currentPlayer][pre.handCount[currentPlayer]-1] = pre.deck[currentPlayer][(pre.deckCount[currentPlayer] - 1)];
      pre.handCount[currentPlayer]++;
      //remove drawn card from deck
      //set last card to -1
      pre.deck[currentPlayer][pre.deckCount[currentPlayer] - 1] = -1;
      pre.deckCount[currentPlayer]--;
    }

    //add two cards to tribute revealed array and then discard from next player's deck
    for(int j = 0; j<2; j++)
    {
      //add to tributeRevealedCards
      tributeRevealedCardsPre[j] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      //add revealed card to discard
      pre.discard[nextPlayer][pre.discardCount[nextPlayer]] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      pre.discardCount[nextPlayer]++;
      //remove from deck
      pre.deck[nextPlayer][pre.deckCount[nextPlayer] - 1] = -1;
      pre.deckCount[nextPlayer]--;
    }

//check asserts
//-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of tributeEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert (post->numActions == pre.numActions, __LINE__, __FILE__);
    printf("\tExpected number of actions = %d, Result = %d\n", pre.numActions, post->numActions);

    //assert that number of cards in hand increased by 2
    myAssert((post->handCount[currentPlayer]-handCountBefore) == (pre.handCount[currentPlayer]-handCountBefore), __LINE__, __FILE__);
    printf("\tExpected hand count increase = %d, Result = %d\n", (pre.handCount[currentPlayer]-handCountBefore), (post->handCount[currentPlayer]-handCountBefore));

    //assert that the cards in tribute array match
    //myAssert(tributeRevealedCardsPost[0] == tributeRevealedCardsPre[0], __LINE__, __FILE__);
    //printf("\tExpected first card in tribute revealed card to be a village, enumerated as %d, Control: %d, Result: %d\n", village, tributeRevealedCardsPre[0], tributeRevealedCardsPost[0]);
    //myAssert(tributeRevealedCardsPost[1] == tributeRevealedCardsPre[1], __LINE__, __FILE__);
    //printf("\tExpected second card in tribute revealed card to be a estate, enumerated as %d, Control: %d, Result: %d\n", estate, tributeRevealedCardsPre[1], tributeRevealedCardsPost[1]);

    //assert the top two cards were removed from next player's deck
    for (int m=0; m<post->deckCount[currentPlayer+1]; m++)
    {
      int error = myAssert(post->deck[currentPlayer+1][m] == copper, __LINE__, __FILE__);
      printf("\tLooping through deck to check that the top two cards of next player's deck were removed (should only be coppers left).\n");
      printf("\tWill kick out of loop if non-Copper card found.\n");
      if (error == -1)
      {
        printf("\tNon-Copper card found in hand.\n");
        break;
      }
    }

    //assert that the number of cards in the next player's deck pile was increased by 2
    myAssert (post->deckCount[currentPlayer+1] == pre.deckCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's deck = %d, Results = %d\n", pre.deckCount[currentPlayer+1], post->deckCount[currentPlayer+1]);

    //assert that the number of cards in the next player's discard pile was increased by 2
    myAssert (post->discardCount[currentPlayer+1] == pre.discardCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's discard = %d, Results = %d\n", pre.discardCount[currentPlayer+1], post->discardCount[currentPlayer+1]);

    //assert that top two discarded cards match the tribute cards
    myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)] == village, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be a Village, matching the card in the tribute array, enumerated as %d, Result = %d\n", village, post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)]);

    //assert that top two discarded cards match the tribute cards
    myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-2)] == estate, __LINE__, __FILE__);
    printf("\tExpect that second from the top card of discard pile should be a Estate, matching the card in the tribute array, enumerated as %d, Result = %d\n", estate, post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-2)]);

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End posCase1.\n");
}

//tests case of next player revealing two of the same treasure card
void posCase2(struct gameState *post, int currentPlayer) {
    printf("Starting posCase2: revealed cards = two coppers = +2 coins\n");

//set initial conditions
//------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = gold;
    post->hand[currentPlayer][1] = gold;
    post->hand[currentPlayer][2] = gold;
    post->hand[currentPlayer][3] = gold;
    post->hand[currentPlayer][4] = tribute;
    post->handCount[currentPlayer] = 5;

    post->deck[currentPlayer][0] = silver;
    post->deck[currentPlayer][1] = silver;
    post->deck[currentPlayer][2] = silver;
    post->deck[currentPlayer][3] = silver;
    post->deck[currentPlayer][4] = silver;
    post->deckCount[currentPlayer] = 5;

    post->deck[currentPlayer+1][0] = estate;
    post->deck[currentPlayer+1][1] = estate;
    post->deck[currentPlayer+1][2] = estate;
    post->deck[currentPlayer+1][3] = copper;
    post->deck[currentPlayer+1][4] = copper;
    post->deckCount[currentPlayer+1] = 5;

    post->discard[currentPlayer+1][0] = province;
    post->discard[currentPlayer+1][1] = adventurer;
    post->discardCount[currentPlayer+1] = 2;

    struct gameState pre;
    memcpy(&pre, post, sizeof(struct gameState));

    int coinsBefore = post->coins;
    int r;
    //int tributeRevealedCardsPost[2] = {-1, -1};
    int tributeRevealedCardsPre[2] = {-1, -1};
    int nextPlayer = currentPlayer+1;
    int i = 0;
//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = tributeCard(post, nextPlayer, currentPlayer);

    //increase buys by 1
    pre.coins +=2;

    //add two cards to tribute revealed array and then discard from next player's deck
    for(int j = 0; j<2; j++)
    {
      //add to tributeRevealedCards
      tributeRevealedCardsPre[j] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      //add revealed card to discard
      pre.discard[nextPlayer][pre.discardCount[nextPlayer]] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      pre.discardCount[nextPlayer]++;
      //remove from deck
      pre.deck[nextPlayer][pre.deckCount[nextPlayer] - 1] = -1;
      pre.deckCount[nextPlayer]--;
    }

    //because duplicate, code makes this -1
    tributeRevealedCardsPre[1] = -1;

//check asserts
//-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of tributeEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert ((post->coins-coinsBefore) == (pre.coins-coinsBefore), __LINE__, __FILE__);
    printf("\tExpected increase in number of coins = %d, Result = %d\n", (pre.coins-coinsBefore), (post->coins-coinsBefore));

    //assert that the cards in tribute array match
    //myAssert(tributeRevealedCardsPost[0] == tributeRevealedCardsPre[0], __LINE__, __FILE__);
    //printf("\tExpected first card in tribute revealed card to be a copper, enumerated as %d, Control: %d, Result: %d\n", copper, tributeRevealedCardsPre[0], tributeRevealedCardsPost[0]);
    //myAssert(tributeRevealedCardsPost[1] == tributeRevealedCardsPre[1], __LINE__, __FILE__);
    //printf("\tExpected second card in tribute revealed card to be -1, since both were copper, Control: %d, Result: %d\n", tributeRevealedCardsPre[1], tributeRevealedCardsPost[1]);

    //assert the top two cards were removed from next player's deck
    for (int m=0; m<post->deckCount[currentPlayer+1]; m++)
    {
      int error = myAssert(post->deck[currentPlayer+1][m] == estate, __LINE__, __FILE__);
      printf("\tLooping through deck to check that the top two cards of next player's deck were removed (should only be estates left).\n");
      printf("\tWill kick out of loop if non-estate card found.\n");
      if (error == -1)
      {
        printf("\tNon-estate card found in hand.\n");
        break;
      }
    }


    //assert that the number of cards in the next player's deck pile was increased by 2
    myAssert (post->deckCount[currentPlayer+1] == pre.deckCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's deck = %d, Results = %d\n", pre.deckCount[currentPlayer+1], post->deckCount[currentPlayer+1]);

    //assert that the number of cards in the next player's discard pile was increased by 2
    myAssert (post->discardCount[currentPlayer+1] == pre.discardCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's discard = %d, Results = %d\n", pre.discardCount[currentPlayer+1], post->discardCount[currentPlayer+1]);

    //assert that top two discarded cards match the tribute cards
    myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)] == copper, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be a copper, matching the card in the tribute array, enumerated as %d, Result = %d\n", copper, post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)]);

    //assert that top two discarded cards match the tribute cards
    myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-2)] == copper, __LINE__, __FILE__);
    printf("\tExpect that second from the top card of discard pile should be a copper, matching the card in the tribute array, enumerated as %d, Result = %d\n", copper, post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-2)]);

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End posCase2.\n");
}

//tests case of next player reveal one copper and only having one card in deck/discard combined
void posCase3(struct gameState *post, int currentPlayer) {
    printf("Starting posCase3: only one revealed card = one copper = +2 coins\n");

//set initial conditions
//------------------------------------------------------------------------------
    post->hand[currentPlayer][0] = gold;
    post->hand[currentPlayer][1] = gold;
    post->hand[currentPlayer][2] = gold;
    post->hand[currentPlayer][3] = gold;
    post->hand[currentPlayer][4] = tribute;
    post->handCount[currentPlayer] = 5;

    post->deck[currentPlayer][0] = silver;
    post->deck[currentPlayer][1] = silver;
    post->deck[currentPlayer][2] = silver;
    post->deck[currentPlayer][3] = silver;
    post->deck[currentPlayer][4] = silver;
    post->deckCount[currentPlayer] = 5;

    post->deck[currentPlayer+1][0] = copper;
    post->deckCount[currentPlayer+1] = 1;


    struct gameState pre;
    memcpy(&pre, post, sizeof(struct gameState));

    int coinsBefore = post->coins;
    int r;
    //int tributeRevealedCardsPost[2] = {-1, -1};
    int tributeRevealedCardsPre[2] = {-1, -1};
    int nextPlayer = currentPlayer+1;
    int i = 0;
//run test function (baronEffect()) and set expected values
//----------------------------------------------------------------------------------
    //run baronEffect() with choice1 = 1, gameState post (equivalent to pre), and currentPlayer = 0;
    r = tributeCard(post, nextPlayer, currentPlayer);

    //increase buys by 1
    pre.coins +=2;

    //add card to tribute revealed array and then discard from next player's deck
      //add to tributeRevealedCards
      tributeRevealedCardsPre[0] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      //add revealed card to discard
      pre.discard[nextPlayer][pre.discardCount[nextPlayer]] = pre.deck[nextPlayer][pre.deckCount[nextPlayer]-1];
      pre.discardCount[nextPlayer]++;
      //remove from deck
      pre.deck[nextPlayer][pre.deckCount[nextPlayer] - 1] = -1;
      pre.deckCount[nextPlayer]--;


//check asserts
//-----------------------------------------------------------------
    //make sure the function thinks it executed correctly
    myAssert(r==0, __LINE__, __FILE__);
    printf("\tExpected return of tributeEffect() = 0, Result = %d\n", r);

    //checks that the number of buys was increased by 1
    myAssert ((post->coins-coinsBefore) == (pre.coins-coinsBefore), __LINE__, __FILE__);
    printf("\tExpected increase in number of coins = %d, Result = %d\n", (pre.coins-coinsBefore), (post->coins-coinsBefore));

    //assert that the cards in tribute array match
    //myAssert(tributeRevealedCardsPost[0] == tributeRevealedCardsPre[0], __LINE__, __FILE__);
    //printf("\tExpected first card in tribute revealed card to be a copper, enumerated as %d, Control: %d, Result: %d\n", copper, tributeRevealedCardsPre[0], tributeRevealedCardsPost[0]);
    //myAssert(tributeRevealedCardsPost[1] == tributeRevealedCardsPre[1], __LINE__, __FILE__);
    //printf("\tExpected second card in tribute revealed card to be -1, since the next player could only reveal one card. Control: %d, Result: %d\n", tributeRevealedCardsPre[1], tributeRevealedCardsPost[1]);

    //assert the top two cards were removed from next player's deck
    for (int m=0; m<post->deckCount[currentPlayer+1]; m++)
    {
      int error = myAssert(post->deck[currentPlayer+1][m] == estate, __LINE__, __FILE__);
      printf("\tLooping through deck to check that the top two cards of next player's deck were removed (should only be estates left).\n");
      printf("\tWill kick out of loop if non-estate card found.\n");
      if (error == -1)
      {
        printf("\tNon-estate card found in hand.\n");
        break;
      }
    }


    //assert that the number of cards in the next player's deck pile was increased by 2
    myAssert (post->deckCount[currentPlayer+1] == pre.deckCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's deck = %d, Results = %d\n", pre.deckCount[currentPlayer+1], post->deckCount[currentPlayer+1]);

    //assert that the number of cards in the next player's discard pile was increased by 2
    myAssert (post->discardCount[currentPlayer+1] == pre.discardCount[currentPlayer+1], __LINE__, __FILE__);
    printf("\tExpected number of cards in next player's discard = %d, Results = %d\n", pre.discardCount[currentPlayer+1], post->discardCount[currentPlayer+1]);

    //assert that top two discarded cards match the tribute cards
    myAssert(post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)] == copper, __LINE__, __FILE__);
    printf("\tExpect that top card of discard pile should be a copper, matching the card in the tribute array, enumerated as %d, Result = %d\n", copper, post->discard[currentPlayer+1][(post->discardCount[currentPlayer+1]-1)]);

    //checks that the pre and post game states are equivalent
    //thereby verifying that nothing else changed in the gameState
    myAssert(memcmp(&pre, post, sizeof(struct gameState)) == 0, __LINE__, __FILE__);
    printf("\tAssert that the control and test game states are the same and nothing else has changed.\n");
    printf("\tNote: this will only indicate other changes with certainty if everything above passes.\n");

    printf("End posCase3.\n");
}

int main() {
  int numPlayers = 2;
  int currentPlayer = 0;
  int seed = 1000;

  int k[10] = {adventurer, council_room, feast, gardens, mine,
               remodel, smithy, village, baron, tribute
              };

  struct gameState G1, G2;
  struct gameState G3;

  printf ("Starting unittest4 for tributeEffect().\n");

  //initialize game states
  initializeGame(numPlayers, k, seed, &G1);
  initializeGame(numPlayers, k, seed, &G2);
  initializeGame(numPlayers, k, seed, &G3);
  posCase1(&G1, currentPlayer);
  posCase2(&G2, currentPlayer);
  posCase3(&G3, currentPlayer);


  printf("Ending unittest4 for tributeCard().\n");

  exit(0);
}
