My Design Decisions:

For creation, fork, quantum, exit, and kill:
	The conditions were if the running queue was empty then it needed the initial process to be added. The initital process will always be running with a special state attached to it to verify that it is the init process, along with it, the initial process has a Process ID of zero. 

	If the running queue only had one item on it then we need to add the next process to be added. I allowed the first running item to be the one who had the earliest arrival time (or the first creation by the users).

	All other processes were either added to the ready queue or their associated priority queue. I allowed only 4 items at a time to be on the ready queue (which was updated with highest priority values everytime a process was created). This was to help me attribute what I thought was equivalent to a aging attribute. So if the item was blocked and the ready queue was full it had to go back onto its current priority queue and wait until it was allowed to go back on the ready queue. If it had a priority of 1 then it was for sure to go back on the ready queue sooner than if there was a process with a priority of 3. I also determined that each process was going to keep its priority value no matter what queue it was on just so that when we have unblocking functions, and the ready queue is full, the process can start back where it is supposed to on the priority queues. 

	When a specific process was killed, then the ready queue was checked to see if it held this item. If it did then the item was removed from the ready queue and updated with the next value that could be moved. The process was added based on what priority queue the process was on. If it was one priority one (i.e., the highest priority) then it was added to the ready queue, etc. There is always going to be only 4 process on the queue no more than that. 

	I decided that the updating of priority queues involved moving processes up the priority queue everytime the determineNext() function was called. This was supposed to be equivalent to what is called age. So determineNext() was to replace time a process was on a given queue. 

	Note** I wanted to add an aging attribute but did not have the time to add it to the PCB attribute. 


For the send, receive, and reply:
	I created a separate blocked queue called the busy queue for this functionality. When the running process sent a message to another process which was found on the total job queue so it was faster than searching through all the queues individually to find. The message was attached to it the process and the running process was blocked. This invoked the determineNext() function to update the ready queue and all the priority queues. When the receive function is invoked a flag is raised to make sure that the process actually received its message for the purpose of reply. 

	When reply is invoked a message is sent back to the sending process that was blocked. The blocked process was either put back on the ready queue if the queue wasnt full or it was put back on its given priority queue. 

	The purpose of the total job queue in this implementation was not only to keep track of how many processes there were, or to make implementation faster, but to also act as a mailbox where messages can send and receive from. 

	I also provided a awaitingprocess ID to verify the the process that will receive a message is assocaited with the process that sent the message


For semaphores:
	I followed the algorithm for semaphores that was given in Lecture 9. When the semaphore was called the running process was blocked if the value of the semaphore was below or equal to 0. The running process, when blocked, was put on a separate busy queue that was dedicated just for semaphores and not for other blocking scenarios. 

	When the semaphore was unblocked, the process was removed from the semaphore block queue and placed back on either the ready queue or the priority queue given its priority. 




For QUEUES:
	I created two separate busy queues, one for semaphores and one for messages.
	I created a ready queue to manage a total of 4 processes at a time -- the content of the priority queue can be put onto the ready queue -- processes created can move ready queue values off if the priority is higher. 
	If they dont make it onto ready queue or running queue they get put onto their given priority queue where tehy will wait and move up the priority queues until they make it onto the ready queue. 













