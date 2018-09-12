#include "mpi.h"
#include "stdio.h"
#include <iostream>
#include <cmath>
using namespace std;

const unsigned int val_max = 1 << 31; // nr maxim pana la care caut
const int n_max = 1 << 10;

bool IsMersennePrime(unsigned int prime, unsigned int& number, int& powerOfTwo)
{
	while (number < prime) {
		powerOfTwo++;
		number = number * 2;
	}

	if (number - 1 != prime)
		return false;
}

// apply euclid formula
unsigned long long GetPerfectNumber(int p) 
{
	return 1LL * (1 << (p - 1)) * ((1 << p) - 1);
}

void MasterProcess(int processCount)
{
	int results[n_max];
	results[0] = 0;

	// receive results from slave processes
	for (int i = 1; i < processCount; ++i) {
		int slaveResult[n_max];
		MPI_Recv(&slaveResult, n_max, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int j = 1; j <= slaveResult[0]; j++) {
			results[++results[0]] = slaveResult[j];
		}
	}

	cout << "\n\nMASTER\n";
	for (int i = 1; i <= results[0]; i++) {
		cout << results[i] << " " << GetPerfectNumber(results[i]) << '\n';
	}
}

void SlaveProcess(int processCount, int slaveNo, unsigned int n)
{
	int patchSize = n / (processCount - 1);
	unsigned int start = patchSize * (slaveNo - 1) + 1;
	unsigned int end = start + patchSize - 1;

	if (slaveNo == processCount - 1)
		end = end + n % patchSize;

	cout << "\nSLAVE - " << slaveNo << "(" << start << ", " << end << ")";

	int* lower = new int[end - start + 1];
	int* upper = new int[end - start + 1];

	memset(lower, 0, (end - start + 1) * sizeof(int));
	memset(upper, 0, (end - start + 1) * sizeof(int));

	int sqrtN = sqrt(1.f * n);

	for (int i = 2; i <= sqrtN; i++)
	{
		if (lower[i] == 0)
		{
			for (int j = i + i; j <= sqrtN; j += i)
					lower[j] = 1;
			
			int l = start;
			while (l%i != 0 || l == i) l++;

			for (; l <= end; l += i)
					upper[l - start] = 1;
		}
	}

	int slaveResults[n_max];
	memset(slaveResults, 0, n_max * sizeof(int));

	int powerOfTwo = 0;
	unsigned int number = 1;
	for (int i = start; i <= end; i++)
	{
		if (upper[i - start] == 0 && IsMersennePrime(i, number, powerOfTwo))
			slaveResults[++slaveResults[0]] = powerOfTwo;
	}

	MPI_Send(&slaveResults, n_max, MPI_INT, 0, 1, MPI_COMM_WORLD);

	delete(lower);
	delete(upper);
}

int main(int argc, char *argv[]) 
{
	int processCount, processNo;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &processCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &processNo);

	if (processNo == 0)
		MasterProcess(processCount);
	else
		SlaveProcess(processCount, processNo, val_max);

	MPI_Finalize();

	return 0;
}