// MY_Bcast.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "mpi.h"

void MY_Bcast(void* data_p, int count, MPI_Datatype datatype, int source_proc, MPI_Comm comm)
{
    int id;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    int p;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    if (id == source_proc)
    {
        for (int i = 0; i < p; i++)
        {
            if (i != id)
            {
                MPI_Send(data_p, count, datatype, i, 0, comm);
            }
        }
    }
    else
    {
        MPI_Recv(data_p, count, datatype, source_proc, 0, comm, MPI_STATUS_IGNORE);
    }
}

int main(int argc,char* argv[])
{
    int id;
    int p;
    double my_elapsed_time=0.0;
    double mpi_elapsed_time=0.0;
    int num_element = 1000;
    int* data = (int*)malloc(num_element * sizeof(int));
    for (int i = 0; i < num_element; i++)
        data[i] = i;
    MPI_Init(&argc, &argv);
    

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    for (int i = 0; i < 1000; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        my_elapsed_time = -MPI_Wtime();
        MY_Bcast(data, num_element, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        my_elapsed_time += MPI_Wtime();

        MPI_Barrier(MPI_COMM_WORLD);
        mpi_elapsed_time = -MPI_Wtime();
        MPI_Bcast(data, num_element, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        mpi_elapsed_time += MPI_Wtime();
    }
    if (!id)
    {
        printf("my_elapsed_time:%f\n", my_elapsed_time);
        printf("mpi_elapsed_time:%f", mpi_elapsed_time);
    }
    MPI_Finalize();
    return 0;
 //   std::cout << "Hello World!\n";
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
