// matrixpooling.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void matgene(int* A, int size, int realsize)
{
    srand(time(NULL));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            A[i * realsize + j] = rand() % 1000;      //做1000以内的乘法
        }
    }
}

void matgenetest(int* A, int size, int realsize)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            A[i * realsize + j] = (i + 1) * (j + 1);      //进行测试
        }
    }
}

void matpooling(int* A, int* C, int matsize,int poolingsize,int resize)
{
    for (int i = 0; i < matsize - poolingsize+1; i++)
        for (int j = 0; j < matsize - poolingsize + 1; j++)
        {
            int maxnum = 0;
            for (int k = i; k < i + poolingsize; k++)
            {
                for (int l = j; l < j + poolingsize; l++)
                {
                    if (maxnum < A[k * matsize + l])
                        maxnum = A[k * matsize + l];
                }
            }
            C[i * resize + j] = maxnum;
        }
}

void matcov(int* A, int* B, int* C, int matsize, int poolingsize, int resize)
{
    for (int i = 0; i < matsize - poolingsize + 1; i++)
        for (int j = 0; j < matsize - poolingsize + 1; j++)
        {
            int covnum = 0;
            for (int k = i; k < i + poolingsize; k++)
            {
                for (int l = j; l < j + poolingsize; l++)
                {
                    covnum += A[k* matsize + l] * B[(k - i) * poolingsize + (l - j)];
                }
            }
            C[i * resize + j] = covnum;
        }
}

void matpartition(int* A, int matsize, int* partitionA, int part_size, int part_matsize,int adjust_size)
{
    int cnt = 0;
    for (int k = 0; k < part_size; k++)
    {
        for (int h = 0; h < part_size; h++)
        {
            for (int i = k * part_matsize; i < (k + 1) * part_matsize + adjust_size; i++)
            {
                for (int j = h * part_matsize; j < (h + 1) * part_matsize+ adjust_size; j++)
                    partitionA[cnt++] = A[i * (matsize+adjust_size) + j];
            }
        }
    }
}

void matrestore(int* partitionA, int matsize, int* A, int part_size, int part_matsize,int outmatsize)
{
    int* Atmp = (int*)calloc(matsize * matsize, sizeof(int));
    int cnt = 0;
    for (int k = 0; k < part_size; k++)
    {
        for (int h = 0; h < part_size; h++)
        {
            for (int i = k * part_matsize; i < (k + 1) * part_matsize; i++)
            {
                for (int j = h * part_matsize; j < (h + 1) * part_matsize; j++)
                    Atmp[i * matsize + j] = partitionA[cnt++];
            }
        }
    }
    //取掉最后三行
    for (int i = 0; i < outmatsize; i++)
        for (int j = 0; j < outmatsize; j++)
            A[i * outmatsize + j] = Atmp[i * matsize + j];
}

void prtmat(int* A, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%4d  ", A[i * size + j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    int rank, comm_sz;
    const int matsize = 1024;
    const int kernelsize = 4;
    const int outmatsize = matsize - kernelsize + 1;
    const int adjustsize = kernelsize - 1;
    int realsize = matsize + adjustsize;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Status status;
    int* A = NULL;
 //   int* B = NULL;
    int* C = NULL;

    double elapsed_time = 0.0;
    if (comm_sz == 1)
    {
        A = (int*)calloc((matsize) * (matsize), sizeof(int));   //做卷积或pooling，由于数据可能无法平均分配，因此填充3行0
 //       B = (int*)calloc(kernelsize * kernelsize, sizeof(int));
        C = (int*)calloc(outmatsize * outmatsize, sizeof(int));
//        matgenetest(A, matsize, matsize);
        matgene(A, matsize, matsize);
//        matgene(B, kernelsize);
        elapsed_time = -MPI_Wtime();
        matpooling(A,C, matsize,kernelsize,outmatsize);
        elapsed_time += MPI_Wtime();
        printf("elapsed time when no paeallel: %f\n", elapsed_time);
//        printf("The result of the product:\n");
//        prtmat(C, outmatsize);
    }
    else
    {
        elapsed_time = -MPI_Wtime();
        int part_size = sqrt(comm_sz);
        int part_matsize = matsize / part_size;
        int part_poolingsize = part_matsize;
        int* partA = (int*)calloc((part_matsize+adjustsize) * (part_matsize+adjustsize), sizeof(int));
//       int* partB = (int*)calloc(kernelsize * kernelsize, sizeof(int));
        int* partC = (int*)calloc(part_poolingsize * part_poolingsize, sizeof(int));
        int* partitionA = NULL;
        if (rank == 0)
        {
            A = (int*)calloc((matsize + adjustsize) * (matsize + adjustsize), sizeof(int));
 //           B = (int*)calloc(kernelsize * kernelsize, sizeof(int));
            C = (int*)calloc(outmatsize * outmatsize, sizeof(int));
//            matgenetest(A, matsize, realsize);
            matgene(A, matsize, realsize);

 //           printf("adjustsize: %d \n", adjustsize);
//            printf("A matrix:\n");
//            prtmat(A, (matsize + adjustsize));

//            matgene(B, kernelsize);
            partitionA = (int*)calloc((part_matsize + adjustsize) * (part_matsize + adjustsize)*part_size*part_size, sizeof(int));
            matpartition(A, matsize, partitionA, part_size, part_matsize, adjustsize);
        }
//        MPI_Bcast(B, kernelsize*kernelsize, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(partitionA, (part_matsize + adjustsize) * (part_matsize + adjustsize), MPI_INT, partA, (part_matsize + adjustsize) * (part_matsize + adjustsize), MPI_INT, 0, MPI_COMM_WORLD);
        free(partitionA);     
        matpooling(partA, partC, (part_matsize + adjustsize), kernelsize, part_poolingsize);
        /*
        for (int i = 0; i < comm_sz; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank == i)
            {
               
                printf("%d process:\n", rank);
                printf("partA:\n");
                prtmat(partA, (part_matsize + adjustsize));
                printf("partC:\n");
                prtmat(partC, part_poolingsize);
               
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }
        */
        int* partitionC = NULL;
        if (rank == 0)
            partitionC = (int*)calloc((outmatsize+adjustsize) * (outmatsize + adjustsize), sizeof(int));
        MPI_Gather(partC, part_poolingsize * part_poolingsize, MPI_INT, partitionC, part_poolingsize * part_poolingsize, MPI_INT, 0, MPI_COMM_WORLD);
        //还原C矩阵
        if (rank == 0)
        {
            matrestore(partitionC, (outmatsize + adjustsize), C, part_size, part_matsize, outmatsize);
        }
        free(partitionC);
        elapsed_time += MPI_Wtime();
        if (rank == 0)
        {
            printf("elapsed time when paeallel with %d processes: %f\n", comm_sz, elapsed_time);
//            printf("The result of the product:\n");
//            prtmat(C, outmatsize);
        }
        free(partA);
//        free(partB);
        free(partC);
    }
    free(A);
 //   free(B);
    free(C);
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
