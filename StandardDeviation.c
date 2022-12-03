#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

#define num_elements_per_proc 5
#define MASTER 0

//Prototypes
float* create_rand_nums(int);
void printFloatArray(float* , int );

main(int argc, char** argv)
{
	int	rank, size;
	int i;
	float* rand_nums = NULL;
	float local_sum = 0;
	float global_sum;
	float mean;
	float squared_local_difference = 0;
	float global_squared_local_difference;
	float standard_deviation;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("\n--------------------%d/%d--------------------\n", rank, size);
	srand(time(NULL) * rank);

	//Create random numbers in nodes
	rand_nums = create_rand_nums(num_elements_per_proc);
	printFloatArray(rand_nums, num_elements_per_proc);
	
	//Calculate local sum for all nodes
	for (i = 0; i < num_elements_per_proc; i++) {
		local_sum += rand_nums[i];
	}
	printf("\nlocal sum : %.2f", local_sum);

	//Calculate global sum for all nodes
	MPI_Allreduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM,
		MPI_COMM_WORLD);
	printf("\nglobal sum : %.2f", global_sum);

	//Calculate mean for all nodes
	mean = global_sum / (num_elements_per_proc * size);
	printf("\nglobal mean : %.2f", mean);

	//Calculate squared local difference for all nodes
	for (i = 0; i < num_elements_per_proc; i++) {
		squared_local_difference += (rand_nums[i] - mean) * (rand_nums[i] - mean);
	}
	printf("\nsquared local difference : %.2f", squared_local_difference);
	
	//Sum all squared local differences into master node
	MPI_Reduce(&squared_local_difference, &global_squared_local_difference, 1, MPI_FLOAT, MPI_SUM, MASTER,
		MPI_COMM_WORLD);

	//Calculate standard deviation with master node
	if (rank == MASTER) {
		printf("\nsquared global difference : %.2f", global_squared_local_difference);

		standard_deviation = sqrt(global_squared_local_difference /
			(num_elements_per_proc * size));

		printf("\nmean : %.2f, standard deviation : %.2f\n", mean, standard_deviation);
	}

	free(rand_nums);
	printf("\n-------------------------------------------");
	MPI_Finalize();
}

float* create_rand_nums(int num_elements) {
	float* rand_nums = (float*)malloc(sizeof(float) * num_elements);
	int i;
	for (i = 0; i < num_elements; i++) {
		rand_nums[i] = (rand() / (float)RAND_MAX);
	}
	return rand_nums;
}

void printFloatArray(float* array, int length)
{
	int i;
	printf("(%dx%d): [", 1, length);
	for (i = 0; i < length; i++)
		printf("%.2f ", array[i]);
	printf("]\n");
}