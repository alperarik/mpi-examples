#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

#define MASTER 0
#define SIZEBUFFER 128
#define TAG 111

//prototypes
double f(double );
double Simpson(double , int , double );
char* createDoubleArray(int );

main(int argc, char** argv)
{
	int	rank, size;

	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("\n--------------------%d/%d--------------------", rank, size);

	char buffer[SIZEBUFFER];
	int position = 0;
	int to;

	double local_sum;	//total value for local intervals
	double global_sum;	//total value for global result
	double a;			//start value of integral
	double b;			//end value of integral
	int n;				//interval size
	double h;			//width value for intervals
	double local_a;		//start value of interval
	int local_b;		//end value of interval

	double startTime;	//timer variable

	if (rank == MASTER)
	{
		n = 1024;
		a = 0;
		b = 3.141592; //PI

		//pack data and send to all slave nodes
		printf("\nMASTER sent n: %d, a: %.2f, b: %.2f", n, a, b);
		MPI_Pack(&n, 1, MPI_INT, buffer, SIZEBUFFER, &position, MPI_COMM_WORLD);
		MPI_Pack(&a, 1, MPI_DOUBLE, buffer, SIZEBUFFER, &position, MPI_COMM_WORLD);
		MPI_Pack(&b, 1, MPI_DOUBLE, buffer, SIZEBUFFER, &position, MPI_COMM_WORLD);

		for (to = 1; to < size; to++)
		{
			MPI_Send(buffer, SIZEBUFFER, MPI_PACKED, to, TAG, MPI_COMM_WORLD);
		}
	}
	 
	if (rank != MASTER)
	{
		MPI_Recv(buffer, SIZEBUFFER, MPI_PACKED, MASTER, TAG, MPI_COMM_WORLD, &status);
		MPI_Unpack(buffer, SIZEBUFFER, &position, &n, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buffer, SIZEBUFFER, &position, &a, 1, MPI_DOUBLE, MPI_COMM_WORLD);
		MPI_Unpack(buffer, SIZEBUFFER, &position, &b, 1, MPI_DOUBLE, MPI_COMM_WORLD);

		printf("\nRank %d received n: %d, a: %.2f, b: %.2f", rank, n, a, b);
	}

	//calculate h
	h = (b - a) / n;

	//Calculate local start value for all nodes in master and scatter
	//(Example usage for Scatter)
	double* local_a_array = NULL;
	int i;
	if (rank == MASTER)
	{
		local_a_array = createDoubleArray(size);
		for (i = 0; i < size; i++)
		{
			local_a_array[i] = a + i * (b - a) / size;
		}
	}

	MPI_Scatter(local_a_array, 1, MPI_DOUBLE, &local_a, 1, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

	//calculate end value for process
	local_b = n / size;

	//Timer for integral calculation
	if (rank == MASTER)
	{
		startTime = MPI_Wtime();
	}

	//calculate numerical integral for process
	local_sum = Simpson(local_a, local_b, h);

	//sum all local integral values into global sum
	MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

	//print result and elapsed time
	if (rank == 0) {
		printf("\n\nNumber of processors used = %d", size);
		printf("\n\nWith n: %d intervals, estimate of the integral from %.2f to %.2f is %.2f", n, a, b, global_sum);
		printf("\n\nTime elapsed: %.2f ms", (MPI_Wtime() - startTime) * 1000);
	}

	printf("\n-------------------------------------------");
	MPI_Finalize();
}


//Simpson method (from a to n local subintervals and width of h) 
double Simpson(double a, int n, double h) {
	double result = 0;
	for (int i = 0; i <= n; i++)
	{
		// if i is 0 or n, only add f(x_i)
		if ((i == 0) || (i == n))
			result += f(a + i * h);
		// then if i is even, add 2*f(x_i)
		else if (i % 2 == 0)
			result += 2 * f(a + i * h);
		// else if i is odd, add 4*f(x_i)
		else
			result += 4 * f(a + i * h);
	}

	result *= h / 3; //3 comes from formula
	return(result);
}

double f(double x) {
	return cos(x);
}

char* createDoubleArray(int size)
{
	char* tmpArray = (double*)malloc(size * sizeof(double));

	if (!tmpArray) {
		printf("\nError : Cannot allocate memory\n");
		exit(1);
	}

	return tmpArray;
}
