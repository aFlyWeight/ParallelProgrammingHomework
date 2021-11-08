// 8_6.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include<string.h>
#include<mpi.h>
#include <fstream>
#include <iostream>

using namespace std;

int main(void) {
	int size;
	int id;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	// 只有一个线程的时候不操作
	if (size <= 1) {
		MPI_Finalize();
		return 0;
	}
	
	int N = 0;
	int* matrix = NULL, * vec = NULL;
	vec = (int*)malloc(N * sizeof(int));
	if (id == 0) {
		ifstream file("../../data/input.txt");
		file >> N;
		matrix = (int*)malloc(N * N * sizeof(int));
		for (int i = 0; i < N; ++i)
			for (int j = 0; j < N; ++j)
				file >> matrix[j * N + i]; // transpose 
		// read ks
		for (int i = 0; i < N; ++i) file >> vec[i];
		// send data to other.
	}
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
	//		printf("%d", N);
	MPI_Bcast(vec, N, MPI_INT, 0, MPI_COMM_WORLD);
	int psize = N/size;
	int pleft = N%size;
	int * localmatrix = NULL, * result = NULL, *allresult = NULL;
	int* sendcount = (int*)malloc(sizeof(int) * (int)size);
	int* disp = (int*)malloc(sizeof(int) * (int)size);
	int* disr = (int*)malloc(sizeof(int) * (int)size);
	int* recount = (int*)malloc(sizeof(int) * (int)size);
	for (int i = 0; i < size; i++)
		sendcount[i] = psize*N;
	for (int i = 0; i < pleft; i++)
		sendcount[i]+=N;
	for (int i = 0; i < size; i++)
		recount[i] = sendcount[i]/N;
	disp[0] = 0;
	for (int i = 1; i < size; i++)
		disp[i] = disp[i - 1] + sendcount[i - 1];
	disr[0] = 0;
	for (int i = 1; i < size; i++)
		disr[i] = disr[i - 1] + recount[i - 1];
	// initialize
	
		localmatrix = (int*)malloc(sendcount[id] * sizeof(int));
		memset(localmatrix, 0, sizeof(localmatrix));
		MPI_Scatterv(matrix, sendcount, disp, MPI_INT, localmatrix, sendcount[id], MPI_INT, 0, MPI_COMM_WORLD);
		result = (int*)malloc((recount[id]) * sizeof(int));
		for (int i = 0; i < recount[id]; i++)
		{
			int sum = 0;
			for (int j = 0; j < N; j++)
				sum += vec[j] * localmatrix[i * N + j];
			result[i] = sum;
//			printf("%d", sum);
		}
		if (id == 0)
		{
			allresult = (int*)malloc(N * sizeof(int));
			MPI_Gatherv(result, sendcount[id]/N, MPI_INT, allresult, recount, disr,MPI_INT, 0, MPI_COMM_WORLD);

		}
		else
		{
			MPI_Gatherv(result, sendcount[id] / N, MPI_INT, allresult, recount, disr, MPI_INT, 0, MPI_COMM_WORLD);
		}
		if (id == 0)
		{
			printf("Final Result:[");
			for (int i = 0; i < N; i++)
				printf("%d,", allresult[i]);
			printf("]\n");
		}
		free(matrix);
		free(localmatrix);
		free(result);
		free(allresult);
		free(sendcount);
		free(disp);
	// finalize
	MPI_Finalize();

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
