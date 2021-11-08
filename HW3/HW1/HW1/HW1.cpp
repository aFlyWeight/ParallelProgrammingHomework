// HW1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    int count;
    double elapsed_time;
    int first;
    int global_count;
    int high_value;
    int i;
    int id;
    int index;
    int low_value;
    char* marked;
    char* global_marked;
    int n;
    int p;
    int proc_size;
    int prime;
    int size;
    //char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Init(&argc, &argv);
    MPI_Barrier(MPI_COMM_WORLD);
    elapsed_time = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    if (argc != 2)
    {
        if (!id) printf("Command line: %s <m>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }

    n = atoi(argv[1]);
    size = n;
 
    marked = (char*)malloc(size * sizeof(char));
    global_marked = (char*)malloc(size * sizeof(char));
    for (i = 0;i < size;i++) { marked[i] = 0; global_marked[i] = 0; }

    index = 2;
    prime = 2;
    count = 0;
    do {
        for (i = 2*prime;i < size;i += prime)
        {
            if (i <= sqrt(n))
                marked[i] = 1;
            else
                if (count % p == id)
                    marked[i] = 1;
        }
           while (marked[++index]);
           prime = index;
           count++;
    } while (prime * prime <= n);

    printf("count %d from process %d\n", count, id);

    MPI_Reduce(marked, global_marked, n, MPI_C_BOOL, MPI_LOR, 0, MPI_COMM_WORLD);

    global_count = 0;
    if (!id)
    {
        for (i = 2;i < size;i++)
            if (!global_marked[i])
                global_count++;
    }

    if(!id)
        printf("global_count: %d", global_count);

    MPI_Finalize();
 //   std::cout << "Hello World!\n";
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
