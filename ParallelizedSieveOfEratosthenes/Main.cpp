#include "mpi.h"
#include "stdio.h"
#include <iostream>
#include <cmath>
#include <queue>
using namespace std;

const unsigned int val_max = 1 << 31; // nr maxim pana la care caut
const unsigned int max_array_size = 1 << 26;

bool IsMersennePrime(unsigned int prime, unsigned int& number, int& powerOfTwo)
{
	while (number < prime) 
	{
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

// compute mersenne prime numbers for one interval
int* ComputeMersennePrimeNumbers(unsigned int patchSize, unsigned int start, unsigned int end, unsigned int n)
{
	int* lower = new int[patchSize + 10];
	int* upper = new int[patchSize + 10];

	memset(lower, 0, (patchSize + 10) * sizeof(int));
	memset(upper, 0, (patchSize + 10) * sizeof(int));

	int sqrtN = sqrt(1.f * n);

	for (int i = 2; i <= sqrtN; i++)
	{
		if (lower[i] == 0)
		{
			for (int j = i + i; j <= sqrtN; j += i)
				lower[j] = 1;

			unsigned int l = start;
			while (l%i != 0 || l == i) l++;

			for (; l <= end; l += i)
				upper[l - start] = 1;
		}
	}

	int* result = new int[patchSize];
	memset(result, 0, patchSize * sizeof(int));

	int powerOfTwo = 0;
	unsigned int number = 1;
	for (unsigned int i = start; i <= end; i++)
	{
		if (upper[i - start] == 0 && IsMersennePrime(i, number, powerOfTwo))
			result[++result[0]] = powerOfTwo;
	}

	delete(lower);
	delete(upper);

	return result;
}

// compute mersenne prime numbers for one process
int* ComputeMersennePrimeNumbers(int processCount, int processNo, unsigned int n)
{
	unsigned int patchSize = n / processCount;
	unsigned int start = processNo * patchSize + 1;
	unsigned int end = start + patchSize - 1;
	if (processNo == processCount - 1)
		end = end + n % patchSize;

	cout << "\nProcess - " << processNo << " (" << start << ", " << end << ")";

	unsigned int partialPatchSize = min(patchSize, max_array_size);
	unsigned int partialStart = start;
	unsigned int partialEnd = partialStart + partialPatchSize - 1;
	std::queue<int> mersennePrimeNumbersQueue;

	while (partialStart <= end)
	{
		int* mersennePrimeNumbers = ComputeMersennePrimeNumbers(partialPatchSize, partialStart, partialEnd, n);
		for (int j = 1; j <= mersennePrimeNumbers[0]; j++)
			mersennePrimeNumbersQueue.push(mersennePrimeNumbers[j]);
		delete(mersennePrimeNumbers);

		partialStart = partialEnd + 1;
		partialEnd = min(partialStart + partialPatchSize - 1, end);
	}

	int* result = new int[mersennePrimeNumbersQueue.size() + 1];
	memset(result, 0, (mersennePrimeNumbersQueue.size() + 1) * sizeof(int));

	while (!mersennePrimeNumbersQueue.empty())
	{
		int mersennePimeNumber = mersennePrimeNumbersQueue.front();
		mersennePrimeNumbersQueue.pop();
		result[++result[0]] = mersennePimeNumber;
	}

	return result;
}

void MasterProcess(int processCount, unsigned int n)
{
	std::queue<int> mersennePrimeNumbersQueue;

	// compute first batch of prime numbers
	int* masterPrimeNumbers = ComputeMersennePrimeNumbers(processCount, 0, n);
	for (int j = 1; j <= masterPrimeNumbers[0]; j++) 
		mersennePrimeNumbersQueue.push(masterPrimeNumbers[j]);
	delete(masterPrimeNumbers);

	// receive results from slave processes
	for (int i = 1; i < processCount; ++i) 
	{
		int* slaveResult = new int[];
		MPI_Recv(slaveResult, 1 << 20, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		for (int j = 1; j <= slaveResult[0]; j++)
			mersennePrimeNumbersQueue.push(slaveResult[j]);
	}

	// compute perfect numbers
	cout << "\n\nResult:\n";	
	while (!mersennePrimeNumbersQueue.empty()) 
	{
		int mersennePimeNumber = mersennePrimeNumbersQueue.front();
		mersennePrimeNumbersQueue.pop();
		cout << "p = " << mersennePimeNumber << " => " << GetPerfectNumber(mersennePimeNumber) << '\n';
	}
}

void SlaveProcess(int processCount, int slaveNo, unsigned int n)
{
	int* slaveResult = ComputeMersennePrimeNumbers(processCount, slaveNo, n);
	int count = slaveResult[0] == 0 ? 0 : slaveResult[0] + 1;
	MPI_Send(slaveResult, count, MPI_INT, 0, 1, MPI_COMM_WORLD);
	delete(slaveResult);
}

int main(int argc, char *argv[])
{
	int processCount = 10, processNo = 9;

	MPI_Init(NULL, NULL);
	double startTime = MPI_Wtime();

	MPI_Comm_size(MPI_COMM_WORLD, &processCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &processNo);

	if (processNo == 0)
		MasterProcess(processCount, val_max);
	else
		SlaveProcess(processCount, processNo, val_max);

	MPI_Barrier(MPI_COMM_WORLD);

	double endTime = MPI_Wtime();
	double elapsedTime = (endTime - startTime);
	
	MPI_Finalize();

	if (processNo == 0)
		cout << "\nTotal time: " << elapsedTime << " (seconds)\n";

	return 0;
}