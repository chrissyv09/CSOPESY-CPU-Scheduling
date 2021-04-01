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
    // FIXME: not sure if efficient
    int startTime; 
    int endTime;
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

void firstComeFirstServe(struct Process P[MAX_PROCESS_SIZE], int XYZ[3]) {
    int i, j, sum;
    struct Process key;

    // sort arrival time (using insertion sort)
    for (i=1; i<XYZ[1]; i++) {
        key = P[i];
        j = i-1;
        while (j > -1 && P[j].arrivalTime > key.arrivalTime) {
            P[j+1] = P[j];
            j--;
        }
        P[j+1] = key;
    }

    // calculate turnaround time, waiting time, start time, end time
    sum = 0;
    for (i=0; i<XYZ[1]; i++) { 
        // computes start and end time
        if (sum >= P[i].arrivalTime) {
            P[i].startTime = sum;     
            sum += (P[i].totalExeTime); 
        } else {
            P[i].startTime = P[i].arrivalTime;
            sum = P[i].arrivalTime + P[i].totalExeTime; 
        }  
        P[i].endTime =  P[i].startTime + P[i].totalExeTime;
        
        // computes turnaround time
        P[i].turnAroundTime = sum - P[i].arrivalTime;

        // waiting time
        if (i == 0) 
            P[i].waitingTime = 0;
        else {
            P[i].waitingTime = P[i].turnAroundTime - P[i].totalExeTime;
        }
    }

    printProcesses(P, XYZ);    
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
        }
        
        fclose(inputFile);
	}
	else {
		printf ("%s not found.", fileName);
        exit(0);
    }

    // printf("X: %d\n", XYZ[0]);
    // printf("Y: %d\n", XYZ[1]);
    // printf("Z: %d\n\n processes:\n", XYZ[2]);

    switch (XYZ[0]) {
        // FCFS
        case 0:
            XYZ[2] = 1;
            printf("I entered! \n");
            firstComeFirstServe(processes, XYZ);
            break;
        // NSJF
        case 1:
            XYZ[2] = 1;
            break;
        // PSJF
        case 2:
            XYZ[2] = 1;
            break;
        // RR 
        case 3:
            break;
    }
    
    return 0;
}