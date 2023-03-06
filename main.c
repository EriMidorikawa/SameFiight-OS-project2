#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

//grobal variables
#define NUM_DECK 52
#define MAX_P_CARDS 2
#define NUM_SUITS 4
#define NUM_RANKS 13
#define MAX_ROUND 2

char* MY_DECK[NUM_DECK];        //the deck of cards
char* DISCARDED_DECK[NUM_DECK]; //the deck of discareded cards
char* CARDS_P1[MAX_P_CARDS];    //cards for each player
char* CARDS_P2[MAX_P_CARDS];
char* CARDS_P3[MAX_P_CARDS];
char* CARDS_P4[MAX_P_CARDS];
int CARDS_INDEX_P1 = 0;         //the current index of the cards for each player
int CARDS_INDEX_P2 = 0;
int CARDS_INDEX_P3 = 0;
int CARDS_INDEX_P4 = 0;
bool p1_status = false;         //true if you win
bool p2_status = false;
bool p3_status = false;
bool p4_status = false;
int INDEX = 0;                  //index of the deck of cards
int INDEX2 = 0;                 // index for the deck of deicarded cards
int FINISH = 0;                 //FINISH = 1 when a team wins
int ROUND_NUM = 0;              //count the number of round
FILE *fp;                       //log file


pthread_mutex_t dealer_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player1_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player4_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dealer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t player1_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t player2_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t player3_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t player4_cond = PTHREAD_COND_INITIALIZER;
int player1_initialized = 0;    //mutex for each player
int player2_initialized = 0;
int player3_initialized = 0;
int player4_initialized = 0;
int round_initialized = 0;      //increment when players exit the round

//function prototypes
void initialDeck();
void shuffleDeck();
void resetValues();
void displayOutput();
void *dealer(void*);
void *player1(void* );
void *player2(void* );
void *player3(void* );
void *player4(void*);

int main(){
    pthread_t thread_dealer, thread_p1, thread_p2, thread_p3, thread_p4;
    int ret_d, ret_p1, ret_p2, ret_p3, ret_p4;
    time_t seed;
    seed = time(NULL);
    //create a deck of cards
    initialDeck(MY_DECK);
    initialDeck(DISCARDED_DECK);

    //create the log file
    fp = fopen("data.txt","w");

    //create threads
    ret_d = pthread_create(&thread_dealer, NULL, &dealer, &seed);
    ret_p1 = pthread_create(&thread_p1, NULL, &player1, &seed);
    ret_p2 = pthread_create(&thread_p2, NULL, &player2, &seed);
    ret_p3 = pthread_create(&thread_p3, NULL, &player3, &seed);
    ret_p4 = pthread_create(&thread_p4, NULL, &player4, &seed);
    
    //join threads
    pthread_join(thread_dealer, NULL);
    pthread_join(thread_p1, NULL);
    pthread_join(thread_p2, NULL);
    pthread_join(thread_p3, NULL);
    pthread_join(thread_p4, NULL);

    //close the log file
    fclose(fp);

    return 0;
}

//create the initial deck of cards
void initialDeck(){
    INDEX = 0;
    for(int i = 0; i < NUM_SUITS; i++){
        for(int j = 0; j < NUM_RANKS; j++){
             switch(j){
                case 0: 
                    MY_DECK[INDEX] = "1";
                    break;
                case 1: 
                    MY_DECK[INDEX] = "2";
                    break;
                case 2: 
                    MY_DECK[INDEX] = "3";
                    break;
                case 3: 
                    MY_DECK[INDEX] = "4";
                    break;
                case 4: 
                    MY_DECK[INDEX] = "5";
                    break;
                case 5: 
                    MY_DECK[INDEX] = "6";
                    break;
                case 6: 
                    MY_DECK[INDEX] = "7";
                    break;
                case 7: 
                    MY_DECK[INDEX] = "8";
                    break;
                case 8: 
                    MY_DECK[INDEX] = "9";
                    break;
                case 9: 
                    MY_DECK[INDEX] = "10";
                    break;
                case 10: 
                    MY_DECK[INDEX] = "J";
                    break;
                case 11: 
                    MY_DECK[INDEX] = "Q";
                    break;
                case 12: 
                    MY_DECK[INDEX] = "K";
                    break;
                default: 
                    break;
             }
            INDEX++;
        }
    }
    INDEX = 0;
}

//shuffle the deck of cards
void shuffleDeck(){
    //switch value of each index with a ramdom index
    for(int i = 0; i < NUM_DECK; i++){
        int swap_index = rand() % NUM_DECK;
        char *temp = MY_DECK[i];
        MY_DECK[i] = MY_DECK[swap_index];
        MY_DECK[swap_index] = temp;
    }
}

//reset variables
void resetValues(){
    INDEX = 0;
    INDEX2 = 0;
    FINISH = 0;
    round_initialized = 0;
    player1_initialized = 0;
    player2_initialized = 0;
    player3_initialized = 0;
    player4_initialized = 0;
    CARDS_P1[0] = NULL;
    CARDS_P1[1] = NULL;
    CARDS_P2[0] = NULL;
    CARDS_P2[1] = NULL;
    CARDS_P3[0] = NULL;
    CARDS_P3[1] = NULL;
    CARDS_P4[0] = NULL;
    CARDS_P4[1] = NULL;
    CARDS_INDEX_P1 = 0; 
    CARDS_INDEX_P2 = 0;
    CARDS_INDEX_P3 = 0;
    CARDS_INDEX_P4 = 0;
    p1_status = false; 
    p2_status = false;
    p3_status = false;
    p4_status = false;
}

//display the status of the game
void displayOutput(){
    int i;

    //show cards of each player
    printf("PLAYER 1: hand %s", CARDS_P1[CARDS_INDEX_P1]);
    if(CARDS_INDEX_P1 == 0)
        i = 1;
    else
        i = 0;
    if(CARDS_P1[i])
        printf(", %s\n", CARDS_P1[i]);
    else
        printf("\n");

    printf("PLAYER 2: hand %s", CARDS_P2[CARDS_INDEX_P2]);
    if(CARDS_INDEX_P2 == 0)
        i = 1;
    else
        i = 0;
    if(CARDS_P2[i])
        printf(", %s\n", CARDS_P2[i]);
    else
        printf("\n");

    printf("PLAYER 3: hand %s", CARDS_P3[CARDS_INDEX_P3]);
    if(CARDS_INDEX_P3 == 0)
        i = 1;
    else
        i = 0;
    if(CARDS_P3[i])
        printf(", %s\n", CARDS_P3[i]);
    else
        printf("\n");

    printf("PLAYER 4: hand %s", CARDS_P4[CARDS_INDEX_P4]); 
    if(CARDS_INDEX_P4 == 0)
        i = 1;
    else
        i = 0;
    if(CARDS_P4[i])
        printf(", %s\n", CARDS_P4[i]);
    else
        printf("\n");

    //show the remaining cards on the deck
    printf("DECK: ");
    for(int i = INDEX; i < NUM_DECK; i++){
        printf("%s, ", MY_DECK[i]);
    }
    if(INDEX > 4){
        for(int i = 0; i < INDEX2; i++)
            printf("%s, ", MY_DECK[i]);
    }
    printf("\n\n");

    //show result of the round
    if(FINISH == 1){
        printf("**********ROUND %d RESULT*************\n", (ROUND_NUM + 1));
        if(p1_status == true)
            printf("PLAYER 1: WIN\n");
        else
            printf("PLAYER 1: LOST\n");

        if(p2_status == true)
            printf("PLAYER 2: WIN\n");
        else
            printf("PLAYER 2: LOST\n");

        if(p3_status == true)
            printf("PLAYER 3: WIN\n");
        else
            printf("PLAYER 3: LOST\n");

        if(p4_status == true)
            printf("PLAYER : WIN\n");
        else
            printf("PLAYER 4: LOST\n");
        printf("\n");
    }
}


void *dealer(void* ptr){
    //set the seed of random numbers
    unsigned int* seed;
    seed = (unsigned int*) ptr;
    srand(*seed);

    do{
        resetValues();
        fprintf(fp, "DEALER: shuffle\n");
        shuffleDeck();

        //hand first 4 cards to each player
        CARDS_P1[0] = MY_DECK[INDEX++];
        CARDS_P2[0] = MY_DECK[INDEX++];
        CARDS_P3[0] = MY_DECK[INDEX++];
        CARDS_P4[0] = MY_DECK[INDEX++];

        //start the round
        if(ROUND_NUM == 0){
            fprintf(fp, "DEALER: starting from player 1 for round 1\n");
            printf("DEALER: starting from player 1 for round 1\n\n");
            player1_initialized = 1;
            player2_initialized = 0;
            player3_initialized = 0;
            player4_initialized = 0;
            pthread_cond_signal(&player1_cond);
        }
        else{
            fprintf(fp, "DEALER: starting from player 2 for round 2\n");
            printf("DEALER: starting from player 2 for round 2\n\n");
            player1_initialized = 0;
            player2_initialized = 1;
            player3_initialized = 0;
            player4_initialized = 0;
            pthread_cond_signal(&player2_cond);
        }

        //wait for finishing the round
        pthread_mutex_lock(&dealer_lock);
        while(round_initialized < 4){
            pthread_cond_wait(&dealer_cond, &dealer_lock);
        }
        ROUND_NUM++;
        pthread_mutex_unlock(&dealer_lock);
        if(ROUND_NUM  == 2)
            break;
    }while(ROUND_NUM < MAX_ROUND);
    pthread_exit(NULL);
}

void *player1(void* ptr){
    //set the seed of random numbers
    unsigned int* seed;
    seed = (unsigned int*) ptr;
    srand(*seed);
    char* temp;

    do{ 
        do{
            //wait for his turn
            pthread_mutex_lock(&player1_lock);
            while(player1_initialized == 0)
                pthread_cond_wait(&player1_cond, &player1_lock);
            pthread_mutex_unlock(&player1_lock);

            //exit the loop when the round finished
            if(FINISH == 1)
                break;
            
            printf("--TURN PLAYER 1--\n");

            //show the contents of the deck(log)
            fprintf(fp, "DECK: ");
            for(int i = INDEX; i < NUM_DECK; i++){
                fprintf(fp, "%s, ", MY_DECK[i]);
            }
            if(INDEX > 4){
                for(int i = 0; i < INDEX2 ; i++)
                    fprintf(fp, "%s, ", DISCARDED_DECK[i]);
            }
            fprintf(fp, "\n");

            //show the card(log)
            fprintf(fp, "PLAYER 1: hand %s\n", CARDS_P1[CARDS_INDEX_P1]);

            //fix the card index to draw a new card
            if(CARDS_INDEX_P1 == 0)
                CARDS_INDEX_P1 = 1;
            else
                CARDS_INDEX_P1 = 0;

            //draw a card from the deck(log)
            CARDS_P1[CARDS_INDEX_P1] = MY_DECK[INDEX];
            fprintf(fp, "PLAYER 1: draw %s\n", CARDS_P1[CARDS_INDEX_P1]);
            fprintf(fp, "PLAYER 1: hand (%s,%s)\n", CARDS_P1[0], CARDS_P1[1]);

            //fix the INDEX of MY_DECK
            if(INDEX == (NUM_DECK - 1)){
                INDEX = 0;
                INDEX2 = 0;
                for(int i = 0; i < NUM_DECK; i++){
                    MY_DECK[i] = DISCARDED_DECK[i];
                }
            }
            else
                INDEX++;

            //compare cards with teammate, exit game if the team wins
            if(CARDS_P1[0] == CARDS_P3[CARDS_INDEX_P3]||CARDS_P1[1] == CARDS_P3[CARDS_INDEX_P3]){
                //log
                fprintf(fp, "PLAYER 1: hand (%s,%s) <> PLAYER 3 has %s\n", CARDS_P1[0], CARDS_P1[1], CARDS_P3[CARDS_INDEX_P3]);
                fprintf(fp, "PLAYER 1: TEAM 1 wins\n");

                FINISH = 1;

                p1_status = true;
                p3_status = true;
                displayOutput();
                
                //tell everyone to exit the round
                player2_initialized = 1;
                player3_initialized = 1;
                player4_initialized = 1;
                pthread_cond_signal(&player2_cond);
                pthread_cond_signal(&player3_cond);
                pthread_cond_signal(&player4_cond);
                
                break;
            }

            //discard a random card(log)
            CARDS_INDEX_P1 = (rand() % 2);
            fprintf(fp, "PLAYER 1: discard %s at random\n", CARDS_P1[CARDS_INDEX_P1]);
            temp = CARDS_P1[CARDS_INDEX_P1];
            DISCARDED_DECK[INDEX2++] = temp;
            CARDS_P1[CARDS_INDEX_P1] = NULL;

            //fix the card index
            if(CARDS_INDEX_P1 == 0)
                 CARDS_INDEX_P1 = 1;
            else
                CARDS_INDEX_P1 = 0;
            
            displayOutput();

            //to next player
            player2_initialized = 1;
            player1_initialized = 0;
            pthread_cond_signal(&player2_cond);
            
        }while(FINISH == 0);

        //tell dealer to exit the round(log)
        fprintf(fp, "PLAYER 1: exits round\n"); 
        pthread_mutex_lock(&dealer_lock);
        round_initialized++;
        pthread_cond_signal(&dealer_cond);
        pthread_mutex_unlock(&dealer_lock);

        if(ROUND_NUM == 1)
            break;

        //wait until dealer reset values
        pthread_mutex_lock(&player1_lock);
        while(round_initialized != 0)
            pthread_cond_wait(&player1_cond, &player1_lock);
        pthread_mutex_unlock(&player1_lock);
    }while(ROUND_NUM < MAX_ROUND);
    pthread_exit(NULL);
}

void *player2(void* ptr){
    //set the seed of random numbers
    unsigned int* seed;
    seed = (unsigned int*) ptr;
    srand(*seed);
    char* temp;
    
    do{

        do{
            //wait for his turn
            pthread_mutex_lock(&player2_lock);
            while(player2_initialized == 0)
                pthread_cond_wait(&player2_cond, &player2_lock);
            pthread_mutex_unlock(&player2_lock);

            //exit loop when the round finished
            if(FINISH == 1)
                break;

            printf("--TURN PLAYER 2--\n");
            //show the contents of the deck(log)
            fprintf(fp, "DECK: ");
            for(int i = INDEX; i < NUM_DECK; i++){
                fprintf(fp, "%s, ", MY_DECK[i]);
            }
            if(INDEX > 4){
                for(int i = 0; i < INDEX2; i++)
                    fprintf(fp, "%s, ", DISCARDED_DECK[i]);
            }
            fprintf(fp, "\n");

            //show the hand(log)
            fprintf(fp, "PLAYER 2: hand %s\n", CARDS_P2[CARDS_INDEX_P2]);

            //fix the card index to draw a new card
            if(CARDS_INDEX_P2 == 0)
                CARDS_INDEX_P2 = 1;
            else
                CARDS_INDEX_P2 = 0;

            //draw a card from the deck(log)
            CARDS_P2[CARDS_INDEX_P2] = MY_DECK[INDEX];
            fprintf(fp, "PLAYER 2: draw %s\n", CARDS_P2[CARDS_INDEX_P2]);
            fprintf(fp, "PLAYER 2: hand (%s,%s)\n", CARDS_P2[0], CARDS_P2[1]);
        
            //fix the INDEX of MY_DECK to the next content
            if(INDEX == NUM_DECK){
                INDEX = 0;
                INDEX2 = 0;
                for(int i = 0; i < NUM_DECK; i++){
                    MY_DECK[i] = DISCARDED_DECK[i];
                }
            }
            else
                INDEX++;

            //compare cards with teammate
            if(CARDS_P2[0] == CARDS_P4[CARDS_INDEX_P4]||CARDS_P2[1] == CARDS_P4[CARDS_INDEX_P4]){
                fprintf(fp, "PLAYER 2: hand (%s,%s) <> PLAYER 4 has %s\n", CARDS_P2[0], CARDS_P2[1], CARDS_P4[CARDS_INDEX_P4]);
                fprintf(fp, "PLAYER 2: TEAM 2 wins\n");
                
                FINISH = 1;
                
                p2_status = true;
                p4_status = true;
                displayOutput();
                
                //tell everyone to exit the round
                player1_initialized = 1;
                player3_initialized = 1;
                player4_initialized = 1;
                pthread_cond_signal(&player1_cond);
                pthread_cond_signal(&player3_cond);
                pthread_cond_signal(&player4_cond);
                
                break;
            }

            //discard a random card(log)
            CARDS_INDEX_P2 = (rand() % 2);
            fprintf(fp, "PLAYER 2: discard %s at random\n", CARDS_P2[CARDS_INDEX_P2]);
            temp = CARDS_P2[CARDS_INDEX_P2];
            DISCARDED_DECK[INDEX2++] = temp;
            CARDS_P2[CARDS_INDEX_P2] = NULL;

            //fix the card index
            if(CARDS_INDEX_P2 == 0)
                CARDS_INDEX_P2 = 1;
            else
                CARDS_INDEX_P2 = 0;

            displayOutput();
            
            //next turn
            player3_initialized = 1;
            player2_initialized = 0;
            pthread_cond_signal(&player3_cond);
            
        }while(FINISH == 0);
        
        //tell dealer to exit the round(log)
        fprintf(fp, "PLAYER 2: exits round\n");
        pthread_mutex_lock(&dealer_lock);
        round_initialized++;
        pthread_cond_signal(&dealer_cond);
        pthread_mutex_unlock(&dealer_lock);

        if(ROUND_NUM == 1)
            break;

        //wait until dealer reset values
        pthread_mutex_lock(&player2_lock);
        while(round_initialized != 0)
            pthread_cond_wait(&player2_cond, &player2_lock);
        pthread_mutex_unlock(&player2_lock);
    }while(ROUND_NUM < MAX_ROUND);
    pthread_exit(NULL);
}

void *player3(void* ptr){
    //set the seed of random numbers
    unsigned int* seed;
    seed = (unsigned int*) ptr;
    srand(*seed);
    char* temp;

    do{
        do{
            //wait for his turn
            pthread_mutex_lock(&player3_lock);
            while(player3_initialized == 0)
                pthread_cond_wait(&player3_cond, &player3_lock);
            pthread_mutex_unlock(&player3_lock);

            //exit loop when the round finish
            if(FINISH == 1)
                break;

            printf("--TURN PLAYER 3--\n");
            //show the contents of the deck(log)
            fprintf(fp, "DECK: ");
            for(int i = INDEX; i < NUM_DECK; i++){
                fprintf(fp, "%s, ", MY_DECK[i]);
            }
            if(INDEX > 4){
                for(int i = 0; i < INDEX2; i++)
                    fprintf(fp, "%s, ", MY_DECK[i]);
            }
            fprintf(fp, "\n");

            //show the hand(log)
            fprintf(fp, "PLAYER 3: hand %s\n", CARDS_P3[CARDS_INDEX_P3]);


            //fix the card index to draw a new card
            if(CARDS_INDEX_P3 == 0)
                CARDS_INDEX_P3 = 1;
            else
                CARDS_INDEX_P3 = 0;

            //draw a card from the deck(log)
            CARDS_P3[CARDS_INDEX_P3] = MY_DECK[INDEX];
            fprintf(fp, "PLAYER 3: draw %s\n", CARDS_P3[CARDS_INDEX_P3]);
            fprintf(fp, "PLAYER 3: hand (%s,%s)\n", CARDS_P3[0], CARDS_P3[1]);

            //fix the INDEX of MY_DECK to the next content
            if(INDEX == NUM_DECK){
                INDEX = 0;
                INDEX2 = 0;
                for(int i = 0; i < NUM_DECK; i++){
                    MY_DECK[i] = DISCARDED_DECK[i];
                }
            }
            else
                INDEX++;

            //compare cards with teammate(log)
            if(CARDS_P3[0] == CARDS_P1[CARDS_INDEX_P3]||CARDS_P3[1] == CARDS_P1[CARDS_INDEX_P1]){
                fprintf(fp, "PLAYER 3: hand (%s,%s) <> PLAYER 1 has %s\n", CARDS_P3[0], CARDS_P3[1], CARDS_P1[CARDS_INDEX_P1]);
                fprintf(fp, "PLAYER 3: TEAM 1 wins\n");
                
                FINISH = 1;
                
                p1_status = true;
                p3_status = true;
                displayOutput();
                
                //tell everyone to exit the round
                player1_initialized = 1;
                player2_initialized = 1;
                player4_initialized = 1;
                pthread_cond_signal(&player1_cond);
                pthread_cond_signal(&player2_cond);
                pthread_cond_signal(&player4_cond);
                
                break;
            }

            //discard a random card(log)
            CARDS_INDEX_P3 = (rand() % 2);
            fprintf(fp, "PLAYER 3: discard %s at random\n", CARDS_P3[CARDS_INDEX_P3]);
            temp = CARDS_P3[CARDS_INDEX_P3];
            DISCARDED_DECK[INDEX2++] = temp;
            CARDS_P3[CARDS_INDEX_P3] = NULL;

            //fix the card index to the next content
            if(CARDS_INDEX_P3 == 0)
                CARDS_INDEX_P3 = 1;
            else
                CARDS_INDEX_P3 = 0;

            displayOutput();
            
            //next turn
            player4_initialized = 1;
            player3_initialized = 0;
            pthread_cond_signal(&player4_cond);
            

        }while(FINISH == 0);
        
        //tell dealer to exit the round(log)
        fprintf(fp, "PLAYER 3: exits round\n");
        pthread_mutex_lock(&dealer_lock);
        round_initialized++;
        pthread_cond_signal(&dealer_cond);
        pthread_mutex_unlock(&dealer_lock);

        if(ROUND_NUM == 1)
            break;
    
        //wait until dealer reset values
        pthread_mutex_lock(&player3_lock);
        while(round_initialized != 0)
            pthread_cond_wait(&player3_cond, &player3_lock);
        pthread_mutex_unlock(&player3_lock);
    }while(ROUND_NUM < MAX_ROUND);
    pthread_exit(NULL);
}

void *player4(void* ptr){
    //set the seed of random numbers
    unsigned int* seed;
    seed = (unsigned int*) ptr;
    srand(*seed);
    char* temp;

    do{
        do{
            //wait for his turn
            pthread_mutex_lock(&player4_lock);
            while(player4_initialized == 0)
                pthread_cond_wait(&player4_cond, &player4_lock);
            pthread_mutex_unlock(&player4_lock);

            //exit loop when the round finished
            if(FINISH == 1)
                break;
            
            printf("--TURN PLAYER 4--\n");
            //show the contents of the deck
            fprintf(fp, "DECK: ");
            for(int i = INDEX; i < NUM_DECK; i++){
                fprintf(fp, "%s, ", MY_DECK[i]);
            }
            if(INDEX > 4){
                for(int i = 0; i < INDEX2; i++)
                    fprintf(fp, "%s, ", MY_DECK[i]);
            }
            fprintf(fp, "\n");
            
            //show the hand(log)
            fprintf(fp, "PLAYER 4: hand %s\n", CARDS_P4[CARDS_INDEX_P4]);

            //fix the card index to draw a new card
            if(CARDS_INDEX_P4 == 0)
                CARDS_INDEX_P4 = 1;
            else
                CARDS_INDEX_P4 = 0;

            //draw a card from the deck
            CARDS_P4[CARDS_INDEX_P4] = MY_DECK[INDEX];
            fprintf(fp, "PLAYER 4: draw %s\n", CARDS_P4[CARDS_INDEX_P4]);
            fprintf(fp, "PLAYER 4: hand (%s,%s)\n", CARDS_P4[0], CARDS_P4[1]);

            //fix the INDEX of MY_DECK to the next content
            if(INDEX == NUM_DECK){
                INDEX = 0;
                INDEX2 = 0;
                for(int i = 0; i < NUM_DECK; i++){
                    MY_DECK[i] = DISCARDED_DECK[i];
                }
            }
            else
                INDEX++;

            //compare cards with teammate
            if(CARDS_P4[0] == CARDS_P2[CARDS_INDEX_P2]||CARDS_P4[1] == CARDS_P2[CARDS_INDEX_P2]){
                fprintf(fp, "PLAYER 4: hand (%s,%s) <> PLAYER 2 has %s\n", CARDS_P4[0], CARDS_P4[1], CARDS_P2[CARDS_INDEX_P2]);
                
                FINISH = 1;
                
                p1_status = true;
                p3_status = true;
                displayOutput();
                
                //tell everyone to exit the round
                player1_initialized = 1;
                player2_initialized = 1;
                player3_initialized = 1;
                pthread_cond_signal(&player1_cond);
                pthread_cond_signal(&player2_cond);
                pthread_cond_signal(&player3_cond);
                
                break;
            }
            
            //discard a random card
            CARDS_INDEX_P4 = (rand() % 2);
            fprintf(fp, "PLAYER 4: discard %s at random\n", CARDS_P4[CARDS_INDEX_P4]);
            temp = CARDS_P1[CARDS_INDEX_P1];
            DISCARDED_DECK[INDEX2++] = temp;
            CARDS_P4[CARDS_INDEX_P4] = NULL;

            //fix the card index
            if(CARDS_INDEX_P4 == 0)
                CARDS_INDEX_P4 = 1;
            else
                CARDS_INDEX_P4 = 0;

            displayOutput();
            
            //next turn
            player1_initialized = 1;
            player4_initialized = 0;
            pthread_cond_signal(&player1_cond);
            
        }while(FINISH == 0);
        
        //tell dealer to exit the round(log)
        fprintf(fp, "PLAYER 4: exits round\n");
        pthread_mutex_lock(&dealer_lock);
        round_initialized++;
        pthread_cond_signal(&dealer_cond);
        pthread_mutex_unlock(&dealer_lock);

        if(ROUND_NUM == 1)
            break;

        //wait until dealer reset values
        pthread_mutex_lock(&player4_lock);
        while(round_initialized != 0)
            pthread_cond_wait(&player4_cond, &player4_lock);
        pthread_mutex_unlock(&player4_lock);
    }while(ROUND_NUM < MAX_ROUND);
    pthread_exit(NULL);
}
