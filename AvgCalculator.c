#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

// prototypes
int *createArray(int);
void printArray(int *, int);
float calculateAvg(int *, int);

main(int argc, char **argv)
{
    // tag  is arbitrary number
    int rank, size, tag = 1111, index;

    // data field for master node
    int *data = NULL;
    int data_size = 20;
    int X = 5;

    // data field for slave nodes
    int *sub_data = NULL;
    int sub_data_size;
    float avg;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("\n--------------------%d/%d--------------------", rank, size);

    // Slaves
    if (rank != 0)
    {
        // receive data
        sub_data_size = (data_size - rank * X);
        sub_data = createArray(sub_data_size);
        MPI_Recv(sub_data, sub_data_size, MPI_INT, rank - 1, tag, MPI_COMM_WORLD,
                 &status);

        printf("\nReceived %d numbers from #%d", sub_data_size, rank - 1);
        printf("\ndata local (%d): ", sub_data_size);
        printArray(sub_data, sub_data_size);

        // send average to master node
        avg = calculateAvg(sub_data, sub_data_size);
        MPI_Send(&avg, 1, MPI_FLOAT, 0, tag, MPI_COMM_WORLD);
        printf("\nSend localavg(%.1f) to #0", avg);

        // send sub array
        sub_data += X;
        sub_data_size -= X;

        if (sub_data_size > 0)
        {
            MPI_Send(sub_data, sub_data_size, MPI_INT, (rank + 1) % size, tag,
                     MPI_COMM_WORLD);
            printf("\nSend %d numbers to #%d", sub_data_size, (rank + 1) % size);
        }
    }

    // Master
    else
    {
        // create data
        printf("\nX = %d, p = %d", X, size);
        data = createArray(data_size);
        printf("\ndata whole (%d): ", data_size);
        printArray(data, data_size);

        // send sub data
        data += X;
        data_size -= X;
        MPI_Send(data, data_size, MPI_INT, 1, tag,
                 MPI_COMM_WORLD);
        printf("\nSend %d numbers to #%d", data_size, rank + 1);

        // receive avg
        for (index = 1; index < size; index++)
        {
            MPI_Recv(&avg, 1, MPI_FLOAT, index, tag, MPI_COMM_WORLD,
                     &status);
            printf("\nReceived localavg(%.1f) from #%d", avg, index);
        }
    }

    printf("\n-------------------------------------------");
    MPI_Finalize();
}

// Creates array dynamically
int *createArray(int size)
{
    int i;
    int *tmpArray = (int *)malloc(size * sizeof(int));

    if (!tmpArray)
    {
        printf("\nError : Cannot allocate memory\n");
        exit(1);
    }

    for (i = 0; i < size; i++)
    {
        tmpArray[i] = i;
    }

    return tmpArray;
}

// Prints array
void printArray(int *array, int size)
{
    int i;

    printf("[ ");
    for (i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf(" ]");
}

// Calculates average of integer array
float calculateAvg(int *array, int size)
{
    int i;
    float sum = 0.0;

    for (i = 0; i < size; i++)
    {
        sum += array[i];
    }

    return sum / size;
}