#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "List.h"

LIST *level_one;
LIST *level_two;
LIST *level_three;
LIST *runningQueue;
LIST *readyQueue;
LIST *busyQueue; 
LIST *semBusyQueue; 
LIST *totalJobs;
LIST *sems;
int received;



int pidValue = 0; 
//int sidValue = 0; 

//to do list:
//1(COMPLETED) add deleting from priority queue from Exit() and Kill()!!!!
//2(COMPLETED)  Find process on other priority queues and remove in SEND FUNCTION
//3(COMPLETED) deal with priority queues for all functions -- this includes deciding how one gets put on the priority queue
//4(COMPLETED) make the main for switch more dynamic so the user can input thier own values for the functions with parameters. 
//5(COMPLETED) Add a age function to decay processes to the low priority queue 
//6 (COMPLETED) Test semaphore functions
//7(COMPLETED): create read me for file to explain choices
//8: verify that the determine next is located in all appropriate places!!!
typedef struct PCB{
	int pid; 
	struct Node *next;
	char msg[41];
	int awaitingReceive; 
	int received; 
	int semSID;
	int state; //-1 = ready, 0 = blocked, 1 = running 
	int priority;
	int time; 
}PCB;

typedef struct SEM{
	int sid; //can be between 0 and 4
	int init_val; 
}SEM;


PCB *currentingRunning; 

void freeList(void *queue){
	queue = NULL;
}


//This is the process which determines how to shuffle around the priority queues and the ready queue. 
int determineNext(){
	Node * toolPID = ListFirst(totalJobs); 
	PCB *currentPCB;
	int tally = 0; 
	int currentPID;
	if(ListCount(readyQueue) == 4){
		printf("ready queue is full...");
		if(ListCount(level_one) == 0){
			if(ListCount(level_two) == 0){
				if(ListCount(level_three) == 0){
					printf("there are no current processes other the priority queues \n");
					return 0; 
				}else {
				toolPID = ListFirst(level_three);
				currentPCB = (PCB *)toolPID;
				//currentPCB->priority = 1; //set the only available process to priority 1
				currentPCB->state = -1;  
				ListAppend(level_one, currentPCB);
				}
			}
			else if(ListCount(level_two) > 0){
				toolPID = ListFirst(level_two);
				currentPCB = (PCB *)toolPID;
				//currentPCB->priority = 1; 
				currentPCB->state = -1; 
				ListRemove(level_two); //remove the next available process with the highest priority and put it on ready queue
				ListAppend(level_one, currentPCB);

				if(ListCount(level_three) >0){
					Node *toolPID2 = ListFirst(level_three);  //remove a process available from third queue
					PCB *currentPCB2 = (PCB *)toolPID2;
					//currentPCB2->priority = 2;
					ListRemove(level_three);
					ListAppend(level_two, currentPCB2);
				}
			
			}
		}	
	 return 0; 
	}
	if(ListCount(level_one) == 0){
		if(ListCount(level_two) == 0){
			if(ListCount(level_three) != 0){
				//printf("hey-o");
				toolPID = ListFirst(level_three);
				currentPCB = (PCB *)toolPID;//set the only available process to priority 1
				currentPCB->state = -1;  
				ListRemove(level_three);
				ListAppend(readyQueue, currentPCB);
			}else{
				printf("there are no current processes other than init\n");
				return 0; 
			}

		}else if(ListCount(level_two) > 0){
			toolPID = ListFirst(level_two);
			currentPCB = (PCB *)toolPID;
			currentPCB->state = -1; 
			ListRemove(level_two); //remove the next available process with the highest priority and put it on ready queue
			ListAppend(readyQueue, currentPCB);

			if(ListCount(level_three) >0){
				Node *toolPID2 = ListFirst(level_three);  //remove a process available from third queue
				PCB *currentPCB2 = (PCB *)toolPID2;
				ListRemove(level_three);
				ListAppend(level_two, currentPCB2);
			}
			
		}
	}else{
		printf("Determining next from level one\n");
		toolPID = ListFirst(level_one);
		currentPCB = (PCB *)toolPID; 
		currentPCB->state = -1; 
		ListRemove(level_one);
		ListAppend(readyQueue, currentPCB);

		if(ListCount(level_two) > 0){
			Node *toolPID2 = ListFirst(level_two);  //remove a process available from third queue
			PCB *currentPCB2 = (PCB *)toolPID2;
			ListRemove(level_two);
			ListAppend(level_one, currentPCB2);
		}

		if(ListCount(level_three) > 0){
			Node *toolPID2 = ListFirst(level_three);  //remove a process available from third queue
			PCB *currentPCB2 = (PCB *)toolPID2;
			ListRemove(level_three);
			ListAppend(level_two, currentPCB2);
		
		}
	}
	return 0; 
}


//this is initialized by the create command. 
int createProcess(int priorVal){
	if(priorVal > 3){
		printf("ERROR: INCORRECT PRIORITY VALUE\n");
		return 0; 
	}
	//allocate memory for new PCB
	PCB *newPCB = malloc(sizeof(PCB));

	newPCB->awaitingReceive = -1;
	newPCB->semSID = -1;
	newPCB->received = 0; 
	strcpy(newPCB->msg, "NULL");

	//printf("total on running queue %4d\n", ListCount(runningQueue));
	//printf("%4d", pidValue);
	//printf("%4d", priorVal);

	//check conditions to determine what goes on running queue, ready queue and priority queues. 
	if(ListCount(runningQueue) > 1){
		if(priorVal < currentingRunning->priority){
		newPCB->state = 1;
		newPCB->pid = pidValue;
		newPCB->priority = priorVal;
		//printf("new process id #1 %4d\n", newPCB->pid);

		printf("total on running queue for updating %4d\n", ListCount(runningQueue));
		if(currentingRunning->priority == 2){
			ListFirst(runningQueue);
			ListNext(runningQueue);
			ListRemove(runningQueue);
			ListAppend(level_two, currentingRunning);
			currentingRunning = newPCB;
		}else if(currentingRunning->priority == 3){
			ListFirst(runningQueue);
			ListNext(runningQueue);
			ListRemove(runningQueue);
			ListAppend(level_three, currentingRunning);
			currentingRunning = newPCB;
		}
		currentingRunning = newPCB; 
		pidValue++;
		printf("total on running queue %4d\n", ListCount(runningQueue));
		printf("Current running process ID: %4d\n", currentingRunning->pid);
		ListAppend(runningQueue, newPCB);
		printf("total on running queue after UPDATING running process %4d\n", ListCount(runningQueue));
		ListAppend(totalJobs, newPCB);
		return newPCB->pid;
	}
}else if(ListCount(runningQueue) == 1 && currentingRunning->pid ==0){

		newPCB->state = 1;
		newPCB->pid = pidValue;
		newPCB->priority = priorVal;
		//printf("new process id #1 %4d\n", newPCB->pid);
		currentingRunning = newPCB; 
		pidValue++;
		printf("total on running queue %4d\n", ListCount(runningQueue));
		printf("Current running process ID: %4d\n", currentingRunning->pid);
		ListAppend(runningQueue, newPCB);
		printf("total on running queue after adding second process %4d\n", ListCount(runningQueue));
		ListAppend(totalJobs, newPCB);
		return newPCB->pid;
		
	}
	else if(ListCount(runningQueue) == 0){
		
		newPCB->state = 1; 
		newPCB->pid = pidValue;
		//printf("new process id #0 %4d\n", newPCB->pid);
		pidValue++;
		currentingRunning = newPCB; 
		printf("total on running queue %4d\n", ListCount(runningQueue));
		printf("Current running process ID: %4d\n", currentingRunning->pid);
		ListAppend(runningQueue, newPCB);

		printf("total on running queue after adding init %4d\n", ListCount(runningQueue));
		return newPCB->pid;
	}
	
		printf("inside add other process");
		newPCB->priority = priorVal;
		newPCB->state = -1;
		
		newPCB->pid = pidValue;
		pidValue++;

		Node * toolPID = ListFirst(readyQueue);
		PCB *currentPCB;
		int tally = 0; 
		int currentPRI;
		int max; 
		PCB *maxPCB;
		printf("here");

		if(ListCount(readyQueue) == 4){
			while(toolPID!=NULL){
						
						currentPCB = (PCB *) toolPID;

						currentPRI = currentPCB->priority;

						if(currentPRI > newPCB->priority){
							if(currentPRI < max){
								break;
							}else{
								max = currentPRI;
								maxPCB = currentPCB;
							}
						}

						
						toolPID = ListNext(readyQueue);
						tally++;
					}

					if(currentPCB->priority == 2){
								ListRemove(readyQueue);
								ListAppend(level_two, currentPCB);
								ListAppend(readyQueue, newPCB);
								printf("Amount on level one: %4d\n", ListCount(level_one));
								printf("Amount on level two: %4d\n", ListCount(level_two));
								printf("Amount on level three: %4d\n", ListCount(level_three));
								printf("Amount on READY QUEUE: %4d\n", ListCount(readyQueue));
								return newPCB->pid;
							}
							if(currentPCB->priority== 3){
								ListRemove(readyQueue);
								ListAppend(level_three, currentPCB);
								ListAppend(readyQueue, newPCB);
								printf("Amount on level one: %4d\n", ListCount(level_one));
								printf("Amount on level two: %4d\n", ListCount(level_two));
								printf("Amount on level three: %4d\n", ListCount(level_three));
								printf("Amount on READY QUEUE: %4d\n", ListCount(readyQueue));
								return newPCB->pid;
							}
		}
		


		printf("Current running process ID: %4d\n", newPCB->pid);
		//printf("new process id %4d\n", newPCB->pid);

		if(priorVal == 1){
			newPCB->priority= 1;
			ListAppend(level_one, newPCB);
		}
		else if (priorVal == 2){
			newPCB->priority = 2;
			ListAppend(level_two, newPCB);
		}
		else if (priorVal ==3){
			newPCB->priority = 3;
			ListAppend(level_three, newPCB);
		}

		
		if(ListCount(readyQueue) < 4 && ListCount(runningQueue) == 2 ){
			determineNext();
			}
	
	ListAppend(totalJobs, newPCB);


	//printf("total on running queue %4d\n", ListCount(runningQueue));

	//printf("current running process %4d", currentingRunning->pid);
	printf("Amount on level one: %4d\n", ListCount(level_one));
	printf("Amount on level two: %4d\n", ListCount(level_two));
	printf("Amount on level three: %4d\n", ListCount(level_three));
	printf("Amount on READY QUEUE: %4d\n", ListCount(readyQueue));
	//printf("created:\n" );
	return newPCB->pid;

}


//copy the running process and put it on the priority queue and then determine if it should be 
int Fork (){
	PCB * runningProc;
	if(currentingRunning->pid == 0 ){
		printf("ERROR: currently running is proc_init");
		if(ListCount(runningQueue) > 1){
			ListNext(runningQueue);
		}else{
			printf("there are no process currently running");
			return 0; 
		}
	}

	runningProc = ListCurr(runningQueue);
	
	PCB *newPCB = malloc(sizeof(PCB));

	newPCB->priority = runningProc->priority;
	strcpy(newPCB->msg, runningProc->msg);
	newPCB->state = -1;
	
	//printf("%4d", pidValue);
	newPCB->pid = pidValue; 
	pidValue++;

	if(newPCB->priority == 1){
		ListAppend(level_one, newPCB);
		//ListAppend(readyQueue, newPCB);
	}
	else if (newPCB->priority== 2){
		ListAppend(level_two, newPCB);

		//ListAppend(readyQueue, newPCB);
	}
	else{
		ListAppend(level_three, newPCB);

		//ListAppend(readyQueue, newPCB);
	}

	if(ListCount(readyQueue) < 4 ){
		determineNext();
	}
	ListAppend(totalJobs, newPCB);

	printf("SUCCESS: Copy has been created :)");

	return 0; 
}


    
//loop through each queue until process id found. Then remove it from all queues that it is associated with 
int Kill(int pidNumber){
	Node * toolPID = ListFirst(totalJobs);
	PCB *currentPCB;
	int tally = 0; 
	int currentPID;
	printf("here");


	while(toolPID!=NULL){
		
		currentPCB = (PCB *) toolPID;

		currentPID = currentPCB->pid;

		if(currentPID == pidNumber){
			break;
		}

		if((currentPID!= pidNumber && tally == ListCount(totalJobs))){
			printf("there is no process with this pid");
			return 0;
		}

		toolPID = ListNext(totalJobs);
		tally++;
	}



	//printf("amount on TOTAL queue %4d\n", ListCount(totalJobs));
	ListRemove(totalJobs);

	//printf("amount on TOTAL queue AFTER removal %4d\n", ListCount(totalJobs));


	if(ListCount(level_one) > 0 && currentPCB->priority == 1){
		Node *findPID = ListFirst(level_one);
		PCB *foundPCB;
		int tally2 = 0; 
		int foundPID; 
		while(findPID != NULL){
			foundPCB = (PCB *) findPID;
			foundPID = foundPCB->pid; 

			if(foundPID == pidNumber){
				break; 
			}

			if((foundPID != pidNumber && tally2 == ListCount(level_one))){
				printf("there is not process in level one with this pid");
			}

			findPID = ListNext(level_one);
			tally2++;

		}
		ListRemove(level_one);
		determineNext();
		return 0;

	}else if(ListCount(level_two) > 0 && currentPCB->priority == 2){
		Node *findPID = ListFirst(level_two);
		PCB *foundPCB;
		int tally2 = 0; 
		int foundPID; 
		while(findPID != NULL){
			foundPCB = (PCB *) findPID;
			foundPID = foundPCB->pid; 

			if(foundPID == pidNumber){
				break; 
			}

			if((foundPID != pidNumber && tally2 == ListCount(level_two))){
				printf("there is not process in level one with this pid");
			}

			findPID = ListNext(level_two);
			tally2++;

		}
		ListRemove(level_two);
		determineNext();
		return 0;

	}else if(ListCount(level_three) > 0 && currentPCB->priority == 3){
		Node *findPID = ListFirst(level_three);
		PCB *foundPCB;
		int tally2 = 0; 
		int foundPID; 
		while(findPID != NULL){
			foundPCB = (PCB *) findPID;
			foundPID = foundPCB->pid; 

			if(foundPID == pidNumber){
				break; 
			}

			if((foundPID != pidNumber && tally2 == ListCount(level_three))){
				printf("there is not process in level one with this pid");
			}

			findPID = ListNext(level_three);
			tally2++;

		}
		ListRemove(level_three);
		return 0;
		}else if(ListCount(readyQueue) > 0 ){
			Node *findPID = ListFirst(readyQueue);
			PCB *foundPCB;
			int tally2 = 0; 
			int foundPID; 
			while(findPID != NULL){
				foundPCB = (PCB *) findPID;
				foundPID = foundPCB->pid; 

				if(foundPID == pidNumber){
					break; 
				}

				if((foundPID != pidNumber && tally2 == ListCount(readyQueue))){
					printf("there is not process in level one with this pid");
				}

				findPID = ListNext(readyQueue);
				tally2++;

			}
			ListRemove(readyQueue);
			determineNext();
			//printf("after finding new process to put on ready queue %4d", ListCount(readyQueue));
			//printf("level 1 after finding new process to put on ready queue %4d", ListCount(level_one));
			return 0;
		}else if(ListCount(runningQueue) > 0){

			Node *findPID = ListFirst(runningQueue);
			PCB *foundPCB;
			int tally2 = 0; 
			int foundPID; 
			while(findPID != NULL){
				foundPCB = (PCB *) findPID;
				foundPID = foundPCB->pid; 

				if(foundPID == pidNumber){
					break; 
				}

				if((foundPID != pidNumber && tally2 == ListCount(runningQueue))){
					printf("there is not process in level one with this pid");
				}

				findPID = ListNext(readyQueue);
				tally2++;

			}

			ListRemove(runningQueue);
			if(ListCount(readyQueue) > 0){
				toolPID = ListFirst(readyQueue);
				currentingRunning = (PCB *)toolPID;
				ListAppend(runningQueue, currentingRunning);
				ListRemove(readyQueue);
				//printf("Amount on ready queue (EXIT FUNCTION) %4d\n", ListCount(readyQueue));
				//printf("Amount on runningQueue queue (EXIT FUNCTION) %4d\n", ListCount(runningQueue));
				determineNext();
				//printf("READY queue total after UPDATING the next Process for ready %4d\n", ListCount(readyQueue));
			}else{
				currentingRunning->pid = 0; 
				//printf("you have no processes at all on ready queue 'C'");
				printf("current running process: %4d", currentingRunning->pid);
			}
			//determineNext();
			//printf("after finding new process to put on ready queue %4d", ListCount(readyQueue));
			//printf("level 1 after finding new process to put on ready queue %4d", ListCount(level_one));
			return 0;

		}
	return 0; 

}

int removeTotal(int PID){
	Node * toolPID = ListFirst(totalJobs);
	PCB * Proc; ; 
	int currentPID = 0; 
	int tally = 0; 
	while(toolPID != NULL){
		Proc = (PCB *) toolPID;

		currentPID = Proc->pid;

		if(currentPID == PID){
			break;
		}

		if((currentPID!= currentingRunning->pid && tally == ListCount(totalJobs))){
			printf("there is no RUNNING process -- ERROR?");
			return 0;
		}

		toolPID = ListNext(totalJobs);
		tally++;
	}

	ListRemove(totalJobs);
	return 0; 

}
//kill the first process on the running queue
//Then update running queue by taking first of ready queue and then shuffling around the priority queues
int Exit(){
	Node * toolPID = ListFirst(runningQueue);
	PCB * initProc = (PCB *) toolPID; 
	int removingPID;
	if(initProc->pid == 0){
		if(ListCount(runningQueue) > 2){
			printf("hey i am in exit");
			ListNext(runningQueue);
			Node *removingTotal = ListCurr(runningQueue);
			PCB *removePCB = (PCB *)removingTotal;
			removingPID = removePCB->pid;
			removeTotal(removingPID);
			ListRemove(runningQueue);
			toolPID = ListNext(runningQueue);
			currentingRunning = (PCB *)toolPID; 
			return 0;
		}else if(ListCount(runningQueue) <= 1){
			printf("nothing on running queue other than init process");
			determineNext();
			if(ListCount(readyQueue) > 0){
				toolPID = ListFirst(readyQueue);
				currentingRunning = (PCB *)toolPID;
			}else{
				printf("you have no processes at all- to create more Press 'C'");
			}

		}else if(ListCount(runningQueue) == 2){
			//printf("running queue size is 2\n");
			ListNext(runningQueue);
			Node *removingTotal = ListCurr(runningQueue);
			PCB *removePCB = (PCB *)removingTotal;
			removingPID = removePCB->pid;
			//printf("amount on TOTAL queue %4d\n", ListCount(totalJobs));
			removeTotal(removingPID);
			//printf("amount on TOTAL queue after removal %4d\n", ListCount(totalJobs));
			ListRemove(runningQueue);
			//printf("amount on level one queue %4d\n", ListCount(level_one));
			//printf("amount on ready queue %4d\n", ListCount(readyQueue));
			//determineNext();
			

			if(ListCount(readyQueue) > 0){
				toolPID = ListFirst(readyQueue);
				currentingRunning = (PCB *)toolPID;
				ListAppend(runningQueue, currentingRunning);
				ListRemove(readyQueue);
				//printf("Amount on ready queue (EXIT FUNCTION) %4d\n", ListCount(readyQueue));
				//printf("Amount on runningQueue queue (EXIT FUNCTION) %4d\n", ListCount(runningQueue));
				determineNext();//shuffle priority queues 
				//printf("READY queue total after UPDATING the next Process for ready %4d\n", ListCount(readyQueue));
			}else{
				currentingRunning->pid = 0; 
				printf("you have no processes at all on ready queue 'C'");
			}

		}

	}



	//find priorityqueue and DELETE!!!!!!!!!!!!!!!!!
	return 0; 
}


//this is for the schedulling algorithm of round robin.. Will contriol the time the quantum is at. -- this is all the process scheduling information
//will prempt currenting running process backonto ready queue if the time quantum is up. 
int Quantum(){
	Node *toolPID = ListFirst(runningQueue);
	PCB *currentPCB;
	PCB *currentPCB2;
	int currentPID;
	int tally = 0; 
	while(toolPID!= NULL){
		
		currentPCB = (PCB *) toolPID;

		currentPID = currentPCB->pid;

		if(currentPID == currentingRunning->pid){
			break;
		}

		if((currentPID!= currentingRunning->pid && tally == ListCount(runningQueue))){
			printf("there is no RUNNING process -- ERROR?");
			return 0;
		}

		toolPID = ListNext(runningQueue);
		tally++;
	}

	printf("\n");
	printf("process ID: %4d\n", currentPCB->pid);
	printf("process priority: %4d\n", currentPCB->priority);
	printf("process state: %4d\n", currentPCB->state);

	currentPCB->state = -1;

	//set a time value for PCB and then count how many times the process has gone through the ready queue. If it has gone through it more than 3 times it has to be put on the level three queue where it will move up over time. 
	
	ListRemove(runningQueue);
	Node *nextRun = ListFirst(readyQueue);
	if(ListCount(readyQueue) > 0){
		currentPCB2 = (PCB *)nextRun;
		currentPCB2->state = 1; 
		//update time here
		currentPCB2->time++;
		ListAppend(runningQueue, currentPCB2);
		ListRemove(readyQueue);
		//ADD an if statement for time to determine if it should go on ready queue or go on level_three queue. 
		ListAppend(readyQueue, currentPCB);
		determineNext();
		//printf("%4d\n", ListCount(readyQueue));
		currentingRunning = currentPCB2;
	}else{
		printf("nothing on ready queue");
		return 0;
	}

	//printf("%4d", currentingRunning->pid);
	return 0; 
}


//send a message to a process 
int send(int sendProcID, char *msgForProc){

	Node *toolPID = ListFirst(totalJobs);
	PCB* currentPCB;
	PCB* foundProcess; 
	int currentPID;
	int tally = 0; 

	//printf("%4d", sendProcID);
	while(toolPID != NULL){
		
		foundProcess = (PCB *) toolPID;

		currentPID = foundProcess->pid;

		if(currentPID == sendProcID){
			break;
		}

		if((currentPID!= sendProcID && tally == ListCount(totalJobs))){
			printf("there is no process with this pid -- SEND FAILED");
			return 0;
		}

		toolPID = ListNext(totalJobs);
		tally++;
	}

	
	printf("The following message '%s' is being sent to process:%4d \n", msgForProc, sendProcID);

	strcpy(foundProcess->msg, msgForProc);

	currentPCB = currentingRunning;

	//printf("%4d\n", currentingRunning->state);

	//take the current running process away so it can be blocked until it waits for a response from the given PID that it is sending to. 
	//
	if(currentPCB->state == 1){
		// TO DO: find found process in other queues and then conduct switching!!
		currentPCB->state = 0;
		ListRemove(runningQueue);
		//ListNext(readyQueue);
		Node *switching = ListFirst(readyQueue);

		//printf("%4d", ListCount(readyQueue));
		PCB * switchedProcess = (PCB *) switching;
		if(switchedProcess->pid == currentPID){
			//ListRemove(readyQueue);
			//printf("%4d", ListCount(readyQueue));
			switching = ListNext(readyQueue);
			switchedProcess = (PCB *) switching;

		}

		printf("Process ID for process being put on running queue: %4d\n", switchedProcess->pid);
		switchedProcess->state = 1;
		currentingRunning = switchedProcess;
		ListAppend(runningQueue, switchedProcess);
		ListRemove(readyQueue);
		
		currentPCB->awaitingReceive = foundProcess->pid; 
		printf("Process ID for process being blocked: %4d\n", currentPCB->pid);
		//PCB *blockProcess = (PCB *) blocking;
		//blockProcess->state = 0; 
		ListAppend(busyQueue, currentPCB);
		determineNext();

		//printf("TOTAL ON BQ: %4d\n", ListCount(busyQueue));
		printf("TOTAL ON RQ: %4d\n", ListCount(readyQueue));

	}else{

		currentPCB->state = 0; 
		ListAppend(busyQueue, currentPCB);
	}


	return 0; 
}

//flag that the message has been received and then exit. Find the blocked process and match it to the process on the total queue to make sure the the process that it sent the message too actually received it
//
int receive(){
	//remove a node from the busy queue and read the message and the PID. 

	Node *awaitingProc = ListFirst(busyQueue);
	Node *receivingProc = ListFirst(totalJobs);
	PCB *recPCB;
	PCB *sendPCB;
	int recPID;
	int sendPID;
	int tally = 0; 
	sendPCB = (PCB *) awaitingProc;
	sendPID = sendPCB->awaitingReceive;

	//printf("here");
	//printf("%4d\n", sendPCB->awaitingReceive);
	//printf("%4d\n", sendPID);
	while(receivingProc != NULL){
		
		recPCB = (PCB *) receivingProc;
		recPID = recPCB->pid;

		//printf("%4d\n", recPCB->pid);
		if(sendPID == recPID){
				printf("Message that was sent to process%4d: '%s'\n",sendPID, recPCB->msg);
				recPCB->received = 1; 
				return 0;
			}
		
		receivingProc = ListNext(totalJobs);
		tally++;
		if((recPID!= sendPID && tally == ListCount(totalJobs))){
			printf("there is no process with this pid -- SEND FAILED");
			return 0;
		}

	}


	return 0; 
}


//
int reply(int replyToProc, char *message){

	Node *awaitingProc = ListFirst(busyQueue);
	Node *replyingProc = ListFirst(totalJobs);
	Node *switching; 
	Node *addProcBack; 
	PCB *replyPCB;
	PCB *sendPCB;
	PCB *switchedProcess;
	int replyPID;
	int sendPID;
	int tally = 0; 
	sendPCB = (PCB *) awaitingProc;
	sendPID = sendPCB->awaitingReceive;

	//printf("%4d", ListCount(readyQueue));
	while(awaitingProc != NULL){
		sendPCB = (PCB *) awaitingProc;
		sendPID = sendPCB->awaitingReceive;

		if(replyToProc == sendPID){
				ListRemove(busyQueue);
				break;

		}
		tally++;
		if((replyPID!= sendPID && tally == ListCount(totalJobs))){
					printf("there is no process with this pid -- SEND FAILED");
					return 0;
				}
		awaitingProc = ListNext(busyQueue);
		tally++;
	}


	while(replyingProc != NULL){
		replyPCB = (PCB *) replyingProc;
		replyPID = replyPCB->pid; 

		if(replyPID == sendPID){	
				//printf("%s", message);
				if(replyPCB->received == 1){

					//printf("%s", message);
					strcpy(sendPCB->msg, message);
					sendPCB->state = -1; //unblocked
					sendPCB->awaitingReceive = -1; //no more awaiting receive
					//printf("process priority %4d\n", sendPCB->priority);
					if(sendPCB->priority == 1){
						ListAppend(readyQueue, sendPCB);
					} else if(ListCount(readyQueue) == 0){
						ListAppend(readyQueue,sendPCB);
					}
					replyPCB->received = 0; 

				}
				//strcpy(sendPCB->msg, message); 
				break;

		}
		tally++;
		if((replyPID!= sendPID && tally == ListCount(totalJobs))){
					printf("there is no process with this pid -- SEND FAILED");
					return 0;
				}
		replyingProc = ListNext(totalJobs);
		tally++;
	}
 
/*
	sendPCB->state = -1; //unblocked
	sendPCB->awaitingReceive = -1; //no more awaiting receive
				
	if(sendPCB->priority == 1){
		ListAppend(readyQueue, sendPCB);
	} //put blocked process with higher prior val back on back on ready queue
	else if(ListCount(readyQueue) == 0){
		ListAppend(readyQueue,sendPCB);
	}//else{
					//put back onto its given priority queue
				//}*/

	//printf("%4d\n", ListCount(readyQueue));
	//printf("%4d\n", ListCount(busyQueue));

	return 0;

}

//create a new semaphore given a ID between 1 and 4 make sure that the ID does not get duplicated and that the id provided falls within allowed ID's. 
int new_semaphore(int semID, int initValue){
	if(semID > 4){
		printf("ERROR: SEM ID VAL NOT IN RANGE!");
		return 0; 
	}
	//printf("here in new semaphore");

	if(semID <=4){
			Node *testSEM = ListFirst(sems);
			SEM *semPCB; 
			int semID2; 
			while(testSEM!= NULL){
				semPCB = (SEM *) testSEM;
				semID2 = semPCB -> sid; 

				if(semID2 == semID){
					semPCB->init_val = initValue;
					printf("already created: UPDATING VALUE with : %4d \n", semPCB->init_val );
					return 0; 
				}

				testSEM = ListNext(sems);
			}
		
		SEM *newSem = malloc(sizeof(sems));
		newSem->sid = semID;
		//sidValue++;
		newSem->init_val = initValue; 
		ListAppend(sems, newSem);
		return 0; 
		
	}
	return 0;
}


int semaphore_P(int semID){
	if(semID > 4){
			printf("ERROR: SEM ID VAL NOT IN RANGE!");
			return 0; 
		}
	//printf("here in semaphore p");
	Node *testSEM = ListFirst(sems);
	//Node *toolRunning = ListFirst(runningQueue);
	//toolRunning = ListNext(runningQueue);//so we dont have the init process
	PCB *runningProcessCheck;
	PCB *switchingProcess;
	SEM *semP; 
	int checkID; 
	int tally = 0; 
	while(testSEM!= NULL){
		semP = (SEM *) testSEM;
		checkID = semP->sid;

		if(checkID == semID){
			break; 
		}
		printf("%4d", checkID);
		tally++;
		if(checkID != semID && tally == ListCount(sems) ){
			printf("semaphore does not exist yet! To create please press 'N'\n");
			return 0; 
		}

		testSEM  = ListNext(sems);
	    
	}



	semP->init_val = semP->init_val--;
	if(semP->init_val <= 0){ //this follows the sem P set up in class -- only blocks when the value of the semaphore is less than or equal to zero
		if(currentingRunning->pid == 0 )//we are dealing with the initial process
		{
			printf("error: deal with initial process");
			return -1; 
		}
		else{
			//SWITCH 
			runningProcessCheck = currentingRunning;
			runningProcessCheck->state = 0; //blocked
			runningProcessCheck->semSID = semID;
			ListAppend(semBusyQueue, runningProcessCheck);
			//ListNext(runningQueue);
			ListTrim(runningQueue);
			//printf("Amount on running Queue: %4d\n", ListCount(runningQueue));
			Node *switching = ListFirst(readyQueue);
			switchingProcess = (PCB *)switching;
			switchingProcess->state = 1; 
			currentingRunning = switchingProcess;
			ListAppend(runningQueue, switchingProcess);
			//printf("Amount on ready Queue: %4d\n", ListCount(readyQueue));
			ListFirst(readyQueue);
			ListRemove(readyQueue);
			//printf("Amount on ready Queue: %4d\n", ListCount(readyQueue));
			determineNext();	
			//printf("Amount on busyQueue: %4d\n", ListCount(semBusyQueue));
			//printf("Amount on ready Queue: %4d\n", ListCount(readyQueue));
			//printf("Amount on running Queue: %4d\n", ListCount(runningQueue));

		}
	}else{
		printf("process not blocked");
		//semP->init_val = semP->init_val--;	
	}


	return 0;
}


int semaphore_V(int semID){
	if(semID > 4){
		printf("ERROR: SEM ID VAL NOT IN RANGE!");
		return 0; 
	}
	Node *testSEM = ListFirst(sems);
	Node *findBusy = ListFirst(semBusyQueue);
	PCB *findBusyPCB; 
	SEM *semP; 
	int checkID;
	int checkSID;
	int tally =0; 
	//printf("here in semaphore V"); 

	printf("amount of semaphore busy queue before unblocking %4d\n", ListCount(semBusyQueue));
	while(testSEM!= NULL){
		semP = (SEM *) testSEM;
		checkID = semP->sid;
		printf("here in while V"); 
		if(checkID == semID){
			//printf("here in while#2");
			
			break; 
		}
		//printf("here in while#2");

		tally++;
		if((checkID!= semID && tally == ListCount(sems))){
			printf("semaphore does not exist yet! To create please press 'N'");
			return 0; 
		}
		testSEM = ListNext(sems);
	}

	semP->init_val = semP->init_val++;
	if(semP->init_val <= 0){
		while(findBusy != NULL){
				findBusyPCB = (PCB *) findBusy;
				checkSID = findBusyPCB->semSID;
				if(checkSID == semID){
					//wake up P
					findBusyPCB->state = -1; //put back on ready list
					if(ListCount(readyQueue)< 4){
						ListAppend(readyQueue, findBusyPCB);
					}else if(ListCount(readyQueue) == 4){
							Node *findProcess = ListFirst(readyQueue);
							PCB *proc; 
							int prior = 0;
							int switchFlag = 0; 
							while(findProcess!= NULL){
								proc = (PCB *) findProcess;
								prior = proc->priority;

								if(prior > findBusyPCB->priority){
									//switch process off and put on priority queue
									if(prior == 2){
										ListAppend(level_two, proc);
										ListRemove(readyQueue);
										ListAppend(readyQueue, findBusyPCB);
										switchFlag = 1;
									}
									else if (prior == 3){
										ListAppend(level_three, proc);
										ListRemove(readyQueue);
										ListAppend(readyQueue, findBusyPCB);
										switchFlag = 1; 
									}
									break;
								}

								findProcess = ListNext(readyQueue);

							}
							//if the unblocked process is not allowed on ready queue because it is full then put back onto given priority queue
							if(switchFlag == 0){
								if( findBusyPCB->priority == 1){
									ListAppend(level_one, findBusyPCB);
								}else if (findBusyPCB->priority == 2){
									ListAppend(level_two, findBusyPCB);
								}else if (findBusyPCB->priority == 3){
									ListAppend(level_three, findBusyPCB);
								}
							}

					}

					ListRemove(semBusyQueue);
					//printf("%4d", ListCount(busyQueue));
					break; 
					}
				findBusy = ListNext(semBusyQueue);
			}
	}
	printf("amount of semaphore busy queue %4d\n", ListCount(semBusyQueue));
	//ListRemove(busyQueue);

	return 0;
}

//find the process of requested process
int procInfo(int PID){
	Node * toolPID = ListFirst(totalJobs);
	PCB * foundProcess = (PCB *) toolPID; 
	int currentPID; 
	int tally = 0;
	while(toolPID != NULL){
		
		foundProcess = (PCB *) toolPID;

		currentPID = foundProcess->pid;

		if(currentPID == PID){
			break;
		}

		if((currentPID!= PID && tally == ListCount(totalJobs))){
			printf("there is no process with this pid -- SEND FAILED");
			return 0;
		}

		toolPID = ListNext(totalJobs);
		tally++;
	}

	//dump info about process
	printf("Process ID: %4d\n", foundProcess->pid );
	printf("State of Process: %4d\n", foundProcess->state );
	printf("Priority of Process: %4d\n", foundProcess->priority);
	printf("What process is the process waiting to receive reply from: %4d\n", foundProcess->awaitingReceive);
	printf("Current Semaphore attached to process: %4d\n", foundProcess->semSID);
	printf("Current message for process %s\n", foundProcess->msg);
	

	return 0; 
}

//DUMP all info for all QUEUES

int totalInfo(){

	Node * toolPID1 = ListFirst(readyQueue);
	PCB * foundProcess1 = (PCB *) toolPID1; 
	int currentPID1; 
	int tally1 = 0;
	int amount = 0;
	printf("---------------------- READY QUEUE ----------------------\n");
	while(toolPID1 != NULL){
		
		foundProcess1 = (PCB *) toolPID1;

		currentPID1 = foundProcess1->pid;	
		amount++;
		printf("Process %4d from ready queue\n", amount );
		printf("Process ID: %4d\n", foundProcess1->pid );
		printf("State of Process: %4d\n", foundProcess1->state );
		printf("Priority of Process: %4d\n", foundProcess1->priority);
		printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess1->awaitingReceive);
		printf("Current Semphore attached to urrent process: %4d\n", foundProcess1->semSID);
		printf("Current message for process %s\n", foundProcess1->msg);
		printf("\n");

		/*if((currentPID!= PID && tally == ListCount(totalJobs))){
			printf("there is no process with this pid -- SEND FAILED");
			return 0;
		}*/

		toolPID1 = ListNext(readyQueue);
		tally1++;
	}
	if(ListCount(level_one) > 0){
		Node * toolPID2 = ListFirst(level_one);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Level One Queue (Highest Priority) ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;
			printf("Process %4d from Highest Priority queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(level_one);
			tally2++;
		}
	}else{
		printf("No Processes on Level one\n");
	}

	if(ListCount(level_two) > 0){
		Node * toolPID2 = ListFirst(level_two);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Level Two Queue (Medium Priority) ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;
			printf("Process %4d from Medium Priority queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(level_two);
			tally2++;
		}
	}else{
		printf("No Processes on Level Two\n");
	}
	if(ListCount(level_three) > 0){
		Node * toolPID2 = ListFirst(level_three);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Level Three Queue (Lowest Priority) ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;
			printf("Process %4d from Lowest Priority queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(level_three);
			tally2++;
		}
	}else{
		printf("No Processes on Level Three\n");
	}

	if(ListCount(runningQueue) > 0){
		Node * toolPID2 = ListFirst(runningQueue);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Running Queue  ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;

			if(foundProcess2->pid == 0){
					printf("Process %4d from running queue\n", amount2 );
					printf("INITIAL Process: %4d\n", foundProcess2->pid );
					printf("State of INITIAL Process: %4d\n", foundProcess2->state );
					printf("Priority of INITIAL Process: %4d\n", foundProcess2->priority);
					printf("What process is the INITIAL process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
					printf("Current Semphore attached to INITIAL process: %4d\n", foundProcess2->semSID);
					printf("Current message for INITIAL process %s\n", foundProcess2->msg);
					printf("\n");
			}else{
				printf("Process %4d from running queue\n", amount2 );
				printf("Process ID: %4d\n", foundProcess2->pid );
				printf("State of Process: %4d\n", foundProcess2->state );
				printf("Priority of Process: %4d\n", foundProcess2->priority);
				printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
				printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
				printf("Current message for process %s\n", foundProcess2->msg);
				printf("\n");
			}

			

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(runningQueue);
			tally2++;
		}
	}else{
		printf("ERROR: No Processes on Running Queue\n");
	}

	if(ListCount(totalJobs) > 0){
		Node * toolPID2 = ListFirst(totalJobs);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Total Jobs Queue ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;

			printf("Process %4d from total jobs queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(totalJobs);
			tally2++;
		}
	}else{
		printf("No Processes on Total Jobs\n");
	}

	if(ListCount(busyQueue) > 0){
		Node * toolPID2 = ListFirst(busyQueue);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Busy Queue ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;

			printf("Process %4d from total jobs queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(busyQueue);
			tally2++;
		}
	}else{
		printf("No Processes on Busy Queue\n");
	}
	if(ListCount(semBusyQueue) > 0){
		Node * toolPID2 = ListFirst(semBusyQueue);
		PCB * foundProcess2 = (PCB *) toolPID2; 
		int currentPID2; 
		int tally2 = 0;
		int amount2 = 0;
		printf("---------------------- Semaphore Busy Queue ----------------------\n");
		while(toolPID2 != NULL){
			
			foundProcess2 = (PCB *) toolPID2;

			currentPID2 = foundProcess2->pid;	
			amount2++;

			printf("Process %4d from total jobs queue\n", amount2 );
			printf("Process ID: %4d\n", foundProcess2->pid );
			printf("State of Process: %4d\n", foundProcess2->state );
			printf("Priority of Process: %4d\n", foundProcess2->priority);
			printf("What process is the current process waiting to receive reply from: %4d\n", foundProcess2->awaitingReceive);
			printf("Current Semphore attached to current process: %4d\n", foundProcess2->semSID);
			printf("Current message for process %s\n", foundProcess2->msg);
			printf("\n");

			/*if((currentPID!= PID && tally == ListCount(totalJobs))){
				printf("there is no process with this pid -- SEND FAILED");
				return 0;
			}*/

			toolPID2 = ListNext(semBusyQueue);
			tally2++;
		}
	}else{
		printf("No Processes on Semaphore Busy Queue\n");
	}
	return 0; 

}
//WHAT I NEED TO DO:::: FIX THE FACT THAT IT WONT LEAVE THE C CASE!!
int main(){
	int run =0; 
	level_one = ListCreate();
	level_two = ListCreate();
	level_three = ListCreate();
	runningQueue = ListCreate();
	totalJobs = ListCreate();
	readyQueue = ListCreate();
	busyQueue = ListCreate();
	semBusyQueue = ListCreate();
	sems = ListCreate();
    
	int testPid =2;
	received = 1;
	PCB *init_proc;
	
	createProcess(-2);//-1 because we are dealing with a special case!
	//printf("Enter command:");
	int PID2 = -1; 
	int PID3 = -1; 
	int sidID = -1;
	//int PID = 2;
	//int semID = 2; 

	printf("Command info:\n");
	printf("to create process press 'C'\n");
	printf("to Fork process press 'F");
	printf("to kill a specific process press 'K'\n");
	printf("to kill running process press 'E'\n");
	printf("to initiate a Quantum press 'Q'\n");
	printf("to send a message to a specific process press 'S'\n");
	printf("to receive press 'R'\n");
	printf("to reply press 'Y'\n");
	printf("to make new semaphore press 'N'\n");
	printf("to Semaphore P press 'P'\n");
	printf("to Semaphore V press 'V'\n");
	printf("To get sepecific process information press 'I'\n");
	printf("To get all info press 'T'");
	printf("To turn OS off press 'D'");


	int k = 1;
	while(run == 0){
		printf("Enter command:\n");
		char com;
		scanf(" %c", &com);
		com = toupper(com);
		switch(com){
			case 'C':
				printf("Enter Priority for Process: ");
				int prior;
				scanf("%d", &prior);
				createProcess(prior);
				break;
			case 'F':
				//printf("here");
				Fork();
				break; 
			case 'K':
				printf("Enter Process ID that you want killed: ");
				int PID;
				scanf("%d", &PID);
				Kill(PID);
				if(ListCount(level_one)!= 0){
					printf("There are things still on level_one\n");
				}
				break;
			case 'E':
				printf("killing running process....");
				Exit();
				//printf("Exiting exit with this amount on running queue:%4d\n",ListCount(runningQueue) );
				break;
			case 'Q':
				printf("Updating running process...");
				Quantum();
				break;
			case 'S':
				//char *message = getchar();
				while(PID2 == -1){
					printf("Enter Process ID:  \n");
					scanf("%d", &PID2) ;
				}

				printf("Enter message:  \n");
				char mess[41];
				scanf("%s", mess) ;
				send(PID2, mess);
				PID2 = -1;
				break;
			case 'R':
				receive();
				break;
			case 'Y':
			//char *message = getchar();
				while(PID3 == -1){
					printf("Enter Process ID that you want to reply to:  \n");
					scanf("%d", &PID3) ;
				}

				printf("Enter message for reply:  \n");
				char mess2[41];
				scanf("%s", mess2);
				reply(PID3, mess2);
				PID3 = -1;
				break;
			case 'N':
				while(sidID == -1){
					printf("Enter SEM ID that you want to create (MUST BE BETWEEN 1 AND 4): \n");
					scanf("%d", &sidID );
				}

				printf("Enter SEM VALUE that you want to attached to Semaphore: \n");
				int initialSemVal; 
				scanf("%d", &initialSemVal);
				new_semaphore(sidID , initialSemVal);
				sidID = -1;
				break;
			case 'P':
				printf("Enter semID (between 1 and 4) for semaphore P: \n");
				int semID;
				scanf("%d", &semID);
				semaphore_P(semID);
				break;
			case 'V':
				printf("Enter semID (between 1 and 4) for semaphore V: \n");
				int semID2;
				scanf("%d", &semID2);
				semaphore_V(semID2);
				break;
			case 'I':
				printf("Enter process ID that you want info about: \n");
				int PID4;
				scanf("%d", &PID4);
				procInfo(PID4);
				break;
			case 'T':
				totalInfo();
				break;
			case 'D':
				run = 1; 
				break; 

		}
	}

	ListFree(level_one, freeList);

	ListFree(level_two, freeList);

	ListFree(level_three, freeList);

	ListFree(readyQueue, freeList);

	ListFree(runningQueue, freeList);

	ListFree(busyQueue, freeList);

	ListFree(semBusyQueue, freeList);

	ListFree(totalJobs, freeList);

	return 0; 
}