// 9_7.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include<string.h>
#include<mpi.h>
#include <fstream>
#include <iostream>

using namespace std;

void MatricToLinear(int **C, int* line, int n, int startrow, int endrow)
{
    for (int i = startrow; i < endrow; i++)
        for (int j = 0; j < n; j++)
            *line++ = C[i][j];
}

void LinearToMatric(int **arry, int* line, int n, int len)
{
    for (int i = 0; i < len; i++)
        for (int j = 0; j < n; j++)
            arry[i][j] = *line++;
}

int main()
{
    int id;
    int size;
    MPI_Status status;
    MPI_Comm worker_comm;
    int N = 0;
    int** matrix = NULL, * vec = NULL;
    vec = (int*)malloc(N * sizeof(int));
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    // 只有一个线程的时候不操作
    if (size <= 1) {
        MPI_Finalize();
        return 0;
    }
    if (id == 0) {
        ifstream file("../../data/input.txt");
        file >> N;
        matrix = (int**)malloc(N * sizeof(int*));
        for(int i=0;i<N;i++)
            matrix[i] = (int*)malloc(N * sizeof(int));
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                file >> matrix[j][i]; // transpose 
        // read ks
        for (int i = 0; i < N; ++i) file >> vec[i];
        // send data to other.
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //		printf("%d", N);
    MPI_Bcast(vec, N, MPI_INT, 0, MPI_COMM_WORLD);
    int psize = N / (size-1);
    int pleft = N % (size-1);
    int* disr = (int*)malloc(sizeof(int) * (int)size);
    int* count = (int*)malloc(sizeof(int) * (int)size);
    for (int i = 1; i < size; i++)
        count[i] = psize;
    for (int i = 1; i <= pleft; i++)
        count[i] ++ ;
    disr[1] = 0;
    for (int i = 2; i < size; i++)
        disr[i] = disr[i - 1] + count[i - 1];

   int* line = (int*)malloc(N * (psize+1) * sizeof(int));
   int* result = NULL;
   int* allresult = NULL;

    if (!id)
    {
        result = (int*)malloc((psize + 1) * sizeof(int));
        allresult = (int*)malloc(N * sizeof(int));
        MPI_Barrier(MPI_COMM_WORLD);
        for (int i = 1; i < size; i++)
        {
            MatricToLinear(matrix, line, N, disr[i], disr[i] + count[i]);
            MPI_Send(line, count[i] * N, MPI_INT,
                i, 0, MPI_COMM_WORLD);
        }
        int k = 0;
        for (int i = 1; i < size; i++)
        {
            MPI_Recv(result, count[i], MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            for (int j = 0; j < count[i]; j++)
            {
                allresult[k++] = result[j];
//                printf("%d   ",result[j]);
            }
        }
        printf("Final Result:[ ");
        for (int i = 0; i < N; i++)
            printf(" %d", allresult[i]);
        printf("]\n");
    }
    else
    {
        result = (int*)malloc(count[id] * sizeof(int));
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Recv(line, count[id]*N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        for (int i = 0; i < count[id]; i++)
        {
            int t = 0;
            for (int j = 0; j < N; j++)
            {
                 t += line[i * N + j] * vec[j];
            }
            result[i] = t;
        }
        MPI_Send(result, count[id], MPI_INT,
            0, 0, MPI_COMM_WORLD);
    }
    free(line);
    free(allresult);
    free(result);
    free(count);
    free(disr);
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
