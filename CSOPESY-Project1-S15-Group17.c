/*************************************************************** 
Names: Tan, Ivy
       Vinco, Christianne 
Group: 17
Section: S15
***************************************************************/
#include <stdio.h>
#include <stdlib.h>

struct Process {
    int processID;
    int arrivalTime;
    int totalExeTime;
    int turnAroundTime;
    int waitingTime;
    int startTime; 
    int endTime;
    int startEndPremp[300][2];
    int countStartEnd;
    int currentExeTime;
};

int MAX_PROCESS_SIZE = 100;

void printProcesses(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, sumWait = 0;

    for (i=0; i<XYZ[1]; i++) {
        printf("P[%d]\nStart Time: %d End time: %d\n", P[i].processID, P[i].startTime, P[i].endTime);
        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n", 1.0 * sumWait / XYZ[1]);
}

// FIXME: make array time a loop
void printProcessesPreemp(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, sumWait = 0;

    for (i=0; i<XYZ[1]; i++) {
        printf("P[%d]\nStart Time: %d End time: %d\n", P[i].processID, P[i].startTime, P[i].endTime);
        printf("Waiting time: %d\n", P[i].waitingTime);
        printf("Turnaround time: %d\n", P[i].turnAroundTime);
        printf("************************************\n");

        sumWait += P[i].waitingTime;
    }

    printf("Average waiting time: %f\n", 1.0 * sumWait / XYZ[1]);
}

void arrangeProcessArrivalTimes(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j;
    struct Process key;
     
    for (i=1; i<XYZ[1]; i++) {
        key = P[i];
        j = i-1;
        while (j > -1 && P[j].arrivalTime > key.arrivalTime) {
            P[j+1] = P[j];
            j--;
        }
        P[j+1] = key;
    }
}

void firstComeFirstServe(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, time;

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    // calculate turnaround time, waiting time, start time, end time
    time = 0;
    for (i=0; i<XYZ[1]; i++) { 
        // computes start and end time
        if (time >= P[i].arrivalTime) {
            P[i].startTime = time;     
            time += (P[i].totalExeTime); 
        } else {
            P[i].startTime = P[i].arrivalTime;
            time = P[i].arrivalTime + P[i].totalExeTime; 
        }  
        P[i].endTime =  P[i].startTime + P[i].totalExeTime;
        
        // computes turnaround time
        P[i].turnAroundTime = time - P[i].arrivalTime;

        // waiting time
        if (i == 0) 
            P[i].waitingTime = 0;
        else {
            P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
        }
    }

    printProcesses(P, XYZ);    
}

void nonPreemptiveShortestJobFirst(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j, lowIndex, time, found;
    struct Process temp;

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    //sort burst time
    time = 0;
    i = 0;
    while (i < XYZ[1]) {
        found = 0;

        // find lowest index
        j=i;
        lowIndex = j;

        while (j < XYZ[1] && time >= P[j].arrivalTime) {
            found = 1;

            if (P[lowIndex].totalExeTime > P[j].totalExeTime) {
                lowIndex = j;
            }
           j++;
        }

        if (!found) {
            time++;
        } else {
            temp = P[i];
            P[i] = P[lowIndex];
            P[lowIndex] = temp;

            P[i].startTime = time;
            time += P[i].totalExeTime;
            P[i].endTime = time;

            P[i].turnAroundTime = P[i].endTime - P[i].arrivalTime;

            // waiting time
            if (i == 0) 
                P[i].waitingTime = 0;
            else {
                P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
            }

            i++;
        }
    }

    printProcesses(P, XYZ);

}

void preemptiveShortestJobFirst(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int time, i, j, lowestIndex, pastLowestIndex, countStartEnd, found; 

    // sort arrival time (using insertion sort)
    arrangeProcessArrivalTimes(P, XYZ);

    i = 0;
    time = 0;
    for (time = 0; i < XYZ[1]; time++) {
        lowestIndex = 0;
        found = 0;

        j = 0;
        //find the lowest burst time at current time
        while (j < XYZ[1] && time >= P[j].arrivalTime) {
            found = 1;

            if (P[lowestIndex].currentExeTime > P[j].currentExeTime && P[j].currentExeTime > 0) {
                lowestIndex = j;
            }
            j++;
        }
        // for (j = 0; j < XYZ[1]; j++) {
        //     printf("inside");
        //     if(P[j].arrivalTime <= time && P[j].currentExeTime < P[lowestIndex].currentExeTime && P[j].currentExeTime > 0) {
        //         found = 1;
        //         lowestIndex = j;
        //     }
        // }

        if (found) {
            //setting start time of the process (first occurence)
            countStartEnd = P[lowestIndex].countStartEnd;
            if (P[lowestIndex].currentExeTime == P[lowestIndex].totalExeTime)
                P[lowestIndex].startEndPremp[countStartEnd][0] = time;
                P[lowestIndex].startEndPremp[countStartEnd][1] = time + 1;

            // check if it is still the same process after time++
            if (lowestIndex == pastLowestIndex) {
                P[lowestIndex].startEndPremp[countStartEnd][1]++;
            } else {
                // go to the next start and end time of the array (counter for the 2D array)
                P[lowestIndex].countStartEnd++;
            }        
            
            //updating the execution time left
            P[lowestIndex].currentExeTime--;
            printf("%d\n", P[lowestIndex].currentExeTime);
            pastLowestIndex = lowestIndex;

            printf("time: %d\nlol: %d\n lowestIndex: %d\n", time, P[lowestIndex].currentExeTime, lowestIndex);

            //compute waiting time and turnaround time if no more execution time left 
            if(P[lowestIndex].currentExeTime == 0) {
                i++;
                printf("i: %d", i);
                // compute completion time (don't mind me)
                countStartEnd = P[i].countStartEnd;

                //wait_time = wait_time + end - arrival_time[smallest] - temp[smallest];
                P[i].turnAroundTime = P[i].startEndPremp[countStartEnd][1] - P[i].arrivalTime;
                P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
            }
        }
    }
}


int main () { 
    char fileName[100];
    int XYZ[3];
    int i;
    struct Process processes[MAX_PROCESS_SIZE];
    FILE *inputFile;
    
    // ask input filename from user
    printf("Enter filename: ");
    scanf("%s", fileName);

    inputFile = fopen(fileName,"rt");

    if (inputFile != NULL) {
        // get first line (X, Y, Z)
        fscanf(inputFile,"%d",&XYZ[0]);
        fscanf(inputFile,"%d",&XYZ[1]);
        fscanf(inputFile,"%d",&XYZ[2]);

        // loop all processes XYZ[1]=Y
        for (i=0; i<XYZ[1]; i++) {
            fscanf(inputFile,"%d",&processes[i].processID);
            fscanf(inputFile,"%d",&processes[i].arrivalTime);
            fscanf(inputFile,"%d",&processes[i].totalExeTime);

            processes[i].currentExeTime = processes[i].totalExeTime;
            processes[i].countStartEnd = 0;
        }

        fclose(inputFile);
	}
	else {
		printf ("%s not found.", fileName);
        exit(0);
    }

    switch (XYZ[0]) {
        // FCFS
        case 0:
            XYZ[2] = 1;
            firstComeFirstServe(processes, XYZ);
            break;
        // NSJF
        case 1:
            XYZ[2] = 1;
            nonPreemptiveShortestJobFirst(processes, XYZ);
            break;
        // PSJF
        case 2:
            XYZ[2] = 1;
            preemptiveShortestJobFirst(processes, XYZ);
            break;
        // RR 
        case 3:
            break;
    }
    
    return 0;
}