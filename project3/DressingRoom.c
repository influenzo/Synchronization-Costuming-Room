// Matthew Selva & Lorenzo DeSimone - Threads and Synchronization

/*
  TO DO:
	- comments
     -Create test & output text files
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

// Predefined Macros
#define EMPTY 0
#define PIRATE 1
#define NINJA 2

// General Variables
int roomOcc;
int numOcc;
int teamNum;
int totalVisits;
int numLoops;
int queueSize;
bool done;

// Pirate Variables
int pID;
int pRID;
int pRevisit[50];
int waitingPirates;
int freeC2Pirates;
int pirateAV;	
int maxPirates;	
int pirateThreshold;

// Ninja Variables
int nID;
int nRID;
int nRevisit[50];
int waitingNinjas;	
int freeC2Ninjas;	
int ninjaAV;
int maxNinjas;
int ninjaThreshold;

// Lock variables
pthread_mutex_t state;
pthread_mutex_t costLock;

// Struct to keep track of values for each customer
struct customer {
	int team;
	int numVisits;
	time_t waitTime;
	time_t visitTime;
	int owed;
};

// Struct to keep track of values for the costume team
struct team {
	bool free;
	int freeTime;
	long pTimeBusy;
	long nTimeBusy;
	long lastIn;	
	long pGoldSpent;
	long nGoldSpent;
};

struct team **teams;

// normRand: Random number generator
int normRand(int time) {
	double a = drand48();
	double b = drand48();
	int ret = abs(time * (sqrt(-2 * log(a)) * cos(2 * M_PI * b))) + 2;
	return ret;
}

// pirate: Handles pirate information
void pirate(int AV, struct customer *pirateStruct) {
	sleep(normRand(AV));
	struct timeval *tv = (struct timeval *)malloc(sizeof(struct timeval));
	gettimeofday(tv, NULL);
	pirateStruct->waitTime = tv->tv_sec;
	pthread_mutex_lock(&state);
	totalVisits++;
	waitingPirates++;
	pthread_mutex_unlock(&state);
	int pTeamNum[50];

	// checking for a room that can accept a pirate, if so, unlock and allow the pirate to enter
	while(true) {
		pthread_mutex_lock(&state);

		if((roomOcc == EMPTY || roomOcc == PIRATE) && pirateThreshold < maxPirates && numOcc < teamNum) break;

		pthread_mutex_unlock(&state);
		sleep(1);
	}

	//designating that the room is now occupied by a pirate
	roomOcc = PIRATE;
	numOcc++;
	waitingPirates--;	
	pirateThreshold++;

	//beginning to track time spent by the pirate and unlocking the room
	gettimeofday(tv, NULL);
	pirateStruct->waitTime = tv->tv_sec - pirateStruct->waitTime;
	pthread_mutex_unlock(&state);

	pthread_mutex_lock(&costLock);
	int i = 0;

	// initializing a costuming team and tracking the time spent costuming
	for(i = 0; i < teamNum; i++) {
		if(teams[i]->free) {
			teams[i]->free = false;
			gettimeofday(tv, NULL);
			teams[i]->freeTime += tv->tv_sec - teams[i]->lastIn;
			pirateStruct->team = i;
			break;
		}
	}

	//costuming a pirate through the usage of random numbers
	int serviceTime = normRand(pirateAV);
	teams[i]->pTimeBusy += serviceTime;
	pthread_mutex_unlock(&costLock);
	
	//waiting the time that was just determined to costume the pirate
	sleep(serviceTime);
	pirateStruct->visitTime = serviceTime;
	
	//once that time has been passed, relock the room and designate the costuming team as free
	pthread_mutex_lock(&costLock);
	teams[i]->free = true;
	gettimeofday(tv, NULL);
	teams[i]->lastIn = tv->tv_sec;
	
	//giving a free costume if the wait time is > 30
	if(pirateStruct->waitTime < 30){
		teams[i]->pGoldSpent += serviceTime;
		pirateStruct->owed += serviceTime;
    	} else freeC2Pirates++;

	pthread_mutex_unlock(&costLock);
	pthread_mutex_lock(&state);
	numOcc--;

	// determining when to give the room to ninjas
	if(numOcc == 0) {
		if(waitingNinjas > 0 && pirateThreshold >= maxPirates) roomOcc = NINJA;
		else roomOcc = EMPTY;

		pirateThreshold = 0;
	}

	//determining if the pirate visiting has visited before, then running the thread and printing the associated information
	pthread_mutex_unlock(&state);
	if ( (pirateStruct->visitTime == pirateStruct->owed) || ((pirateStruct->numVisits + 1) < 2) ) {
		pID++;
		printf(" -- Pirate #%d -- \n", (pID - 1));
	} else {
		printf(" -- Revisited Pirate #%d-- \n", (pRID - 1));
	}
	printf("Team: %d\n", pirateStruct->team);
	printf("Visit Number: %d\n", (pirateStruct->numVisits + 1));
	printf("Total Wait Time: %ld\n", pirateStruct->waitTime);
	printf("Total Visit Time: %ld\n", pirateStruct->visitTime);
	printf("Total Gold Owed: %d\n\n\n", pirateStruct->owed);
	fflush(stdout);

	// determining if a pirate comes back to the costume room
	if(rand() % 4 == 1 && pRevisit[pRID] != 1) {
		pRID=pID;
		pRevisit[pRID] = 1;
		pirateStruct->numVisits++;
		pirate(AV, pirateStruct);
	}
}

// ninja: Handles ninja information
void ninja(int AV, struct customer *ninjaStruct) {
	sleep(normRand(AV));
	struct timeval *tv = (struct timeval *)malloc(sizeof(struct timeval));
	gettimeofday(tv, NULL);
	ninjaStruct->waitTime = tv->tv_sec;
	pthread_mutex_lock(&state);
	totalVisits++;
	waitingNinjas++;
	pthread_mutex_unlock(&state);
	int nTeamNum[50];

	// checking for a room that can accept a ninja, if so, unlock and allow the ninja to enter
	while(true) {
		pthread_mutex_lock(&state);

		if((roomOcc == EMPTY || roomOcc == NINJA) && ninjaThreshold < maxNinjas && numOcc < teamNum) break;
		
		pthread_mutex_unlock(&state);
		sleep(1);
	}
	
	//designating that the room is now occupied by a ninja
	roomOcc = NINJA;
	numOcc++;
	waitingNinjas--;
	ninjaThreshold++;
		
	//beginning to track time spent by the ninja and unlocking the room
	gettimeofday(tv, NULL);
	ninjaStruct->waitTime = tv->tv_sec - ninjaStruct->waitTime;
	pthread_mutex_unlock(&state);

	pthread_mutex_lock(&costLock);
    	int i = 0;

	// initializing a costuming team and tracking the time spent costuming
    	for(i = 0; i < teamNum; i++) {
        	if(teams[i]->free) {
            		teams[i]->free = false;
            		gettimeofday(tv, NULL);
            		teams[i]->freeTime += tv->tv_sec - teams[i]->lastIn;
			ninjaStruct->team = i;
            		break;
        	}
    	}
    
	//costuming a ninja through the usage of random numbers
	int serviceTime = normRand(ninjaAV);
    	teams[i]->pTimeBusy += serviceTime;
    	pthread_mutex_unlock(&costLock);
    
    	
	//waiting the time that was just determined to costume the pirate
	sleep(serviceTime);
    	ninjaStruct->visitTime = serviceTime;
    
	//once that time has been passed, relock the room and designate the costuming team as free
    	pthread_mutex_lock(&costLock);
    	teams[i]->free = true;
    	gettimeofday(tv, NULL);
    	teams[i]->lastIn = tv->tv_sec;

	//giving a free costume if the wait time is > 30
    	if(ninjaStruct->waitTime < 30){
		teams[i]->nGoldSpent += serviceTime;
		ninjaStruct->owed += serviceTime;
    	} else freeC2Ninjas++;

    	pthread_mutex_unlock(&costLock);
	pthread_mutex_lock(&state);
	numOcc--;

	// determining when to give the room to pirates
	if(numOcc == 0) {
		if(waitingPirates > 0 && ninjaThreshold >= maxNinjas) roomOcc = PIRATE;
		else roomOcc = EMPTY;

		ninjaThreshold = 0;
	}

	//determining if the ninja visiting has visited before, then running the thread and printing the associated information
	pthread_mutex_unlock(&state);
	if ( (ninjaStruct->visitTime == ninjaStruct->owed) || ((ninjaStruct->numVisits + 1) < 2) ) {
		nID++;
		printf(" -- Ninja #%d -- \n", (nID - 1));
	} else {
		printf(" -- Revisited Ninja #%d-- \n", (nRID - 1));
	}
	printf("Team: %d\n", ninjaStruct->team);
	printf("Visit Number: %d\n", (ninjaStruct->numVisits + 1));
	printf("Total Wait Time: %ld\n", ninjaStruct->waitTime);
	printf("Total Visit Time: %ld\n", ninjaStruct->visitTime);
	printf("Total Gold Owed: %d\n\n\n", ninjaStruct->owed);
	fflush(stdout);

	// determining if a ninja comes back to the costume room
	if(rand() % 4 == 1 && nRevisit[nRID] != 1) {
		nRID=nID;
		nRevisit[nRID] = 1;
		ninjaStruct->numVisits++;
		ninja(AV, ninjaStruct);
	}
}

// pThread: Handles pirate threads
void pThread(int *AV) {
    	struct customer *pirateStruct = (struct customer *)calloc(1, sizeof(struct customer));
	pirateStruct->waitTime = 0;
    	pirateStruct->visitTime = 0;
    	pirateStruct->owed = 0;
    	pirate(*AV, pirateStruct);
}

// nThread: Handles ninja threads
void nThread(int *AV) {
    	struct customer *ninjaStruct = (struct customer *)calloc(1, sizeof(struct customer));
	ninjaStruct->waitTime = 0;
    	ninjaStruct->visitTime = 0;
    	ninjaStruct->owed = 0;
    	ninja(*AV, ninjaStruct);
}

// counter: Handles the lock
void counter(){
	while(!done){
		sleep(10);
		numLoops++;
		pthread_mutex_lock(&state);
		queueSize += waitingPirates + waitingNinjas;
		pthread_mutex_unlock(&state);
	}
}

int main(int argc, char *argv[]) {
	printf("\n");
	if (argc != 8) {
		printf("Incorrect number of arguments.\n");
		exit(0);
	}

	// Values for interpreting Command Line arguments.
	teamNum       = atoi(argv[1]);
	int pirateNum = atoi(argv[2]);
	int ninjaNum  = atoi(argv[3]);
	pirateAV      = atoi(argv[4]);
	ninjaAV       = atoi(argv[5]);
	int pirateArr = atoi(argv[6]);
	int ninjaArr  = atoi(argv[7]);

	int numPirates = atoi(argv[2]);
	int numNinjas  = atoi(argv[3]);

	// Checking if input is valid.
	bool validIn = true;
	
	//validating the user input
	if (teamNum < 2 || teamNum > 4) {
		validIn = false;
		printf("ERROR: %d is an invald number of costuming teams\n", teamNum);
	}

	if (pirateNum < 10 || pirateNum > 50) {
		validIn = false;
		printf("ERROR: %d is an invald number of pirates\n", pirateNum);
	}

	if (ninjaNum < 10 || ninjaNum > 50) {
		validIn = false;
		printf("ERROR: %d is an invald number of ninjas\n", ninjaNum);
	}

	if (pirateAV <= 0) {
		validIn = false;
		printf("ERROR: %d is an invald average costuming time for a pirate\n", ninjaNum);
	}

	if (ninjaAV <= 0) {
		validIn = false;
		printf("ERROR: %d is an invald average costuming time for a ninja\n", ninjaNum);
	}

	if (pirateArr <= 0) {
		validIn = false;
		printf("ERROR: %d is an invald average arrival time for a pirate\n", ninjaNum);
	}

	if (ninjaArr <= 0) {
		validIn = false;
		printf("ERROR: %d is an invald average arrival time for a ninja\n", ninjaNum);
	}

	if (!validIn) exit(0);

	// Run program
	struct timeval *tv = (struct timeval *)malloc(sizeof(struct timeval));
	int nGoldSpent = 0;
	int pGoldSpent = 0;
	srand(time(0));
	
	roomOcc = EMPTY;
	waitingPirates = 0;
	waitingNinjas = 0;
	freeC2Pirates = 0;
	freeC2Ninjas = 0;
	totalVisits = 0;
	done = false;
	numLoops = 0;
	queueSize = 0;
	pthread_mutex_init(&state, NULL);
	pthread_mutex_init(&costLock, NULL);

	maxPirates = 25 / pirateAV;
	maxNinjas = 25 / ninjaAV;
	pirateThreshold = 0;
	ninjaThreshold = 0;

	teams = (struct team **)calloc(teamNum, sizeof(struct team *));
	for(int i = 0; i < teamNum; i++) {
		gettimeofday(tv, NULL);
		teams[i] = (struct team *)malloc(sizeof(struct team));
		teams[i]->free = true;
		teams[i]->lastIn = tv->tv_sec;
		teams[i]->pGoldSpent = 0;
		teams[i]->nGoldSpent = 0;
	}

	pthread_t *pirates = (pthread_t *)calloc(pirateNum, sizeof(pthread_t));
	pthread_t *ninjas = (pthread_t *)calloc(ninjaNum, sizeof(pthread_t));
	pthread_t queueCounter;
	int i;
	int piratesMade = 0;
	int ninjasMade = 0;

	for(i = 0; i < 50; i++) {
		if(piratesMade < pirateNum) {
			pthread_create(&pirates[i], NULL, (void *)&pThread, &pirateArr);
			piratesMade++;
		}
		if(ninjasMade < ninjaNum) {
			pthread_create(&ninjas[i], NULL, (void *)&nThread, &ninjaArr);
			ninjasMade++;
		}
	}

	pthread_create(&queueCounter, NULL, (void *)&counter, NULL);

	for(i = 0; i < numPirates; i++) {
		pthread_join(pirates[i], NULL);
	}

	for(i = 0; i < numNinjas; i++) {
		pthread_join(ninjas[i], NULL);
	}

	done = true;

	// Calculating gold totals for pirates & ninjas
	for(i = 0; i < teamNum; i++) {
		nGoldSpent += teams[i]->nGoldSpent;
		pGoldSpent += teams[i]->pGoldSpent;
	}	

	// Printing final statistics
	printf("\n -- Costuming Department Bill -- \n");
	printf("Pirates owed a total of %d gold pieces.\n", pGoldSpent);
	printf("Ninjas owed a total of %d gold pieces.\n", nGoldSpent);
	printf("Expenses for employing the costume teams is %d gold pieces.\n", (5 * teamNum));
	for(i = 0; i < teamNum; i++) {
		gettimeofday(tv, NULL);
		teams[i]->freeTime += tv->tv_sec - teams[i]->lastIn;
		
		printf("Team %d was busy for %ld minutes and free for %d minutes.\n", i, (teams[i]->pTimeBusy + teams[i]->nTimeBusy), teams[i]->freeTime);
	}
	printf("Average queue length was %d customers.\n", (queueSize / numLoops));
	printf("Gross Revenue is %d gold pieces.\n", pGoldSpent + nGoldSpent);
	printf("Average gold per visit was %d gold pieces.\n", ((pGoldSpent + nGoldSpent) / totalVisits));
	printf("Total revenue was %d gold pieces.\n", pGoldSpent + nGoldSpent - (5 * teamNum));	
}
