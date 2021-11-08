// Matrix_multi.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void matgene(int* A, int size)
{
    srand(time(NULL));
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            A[i*size+j] = rand() % 1000;      //做1000以内的乘法
        }
    }
}

void matgenetest(int* A, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            A[i * size + j] = (i+1)*(j+1);      //做1000以内的乘法
        }
    }
}

void matmulti(int* A, int* B, int* C, int size)
{
    for(int i=0;i<size;i++)
        for (int j = 0; j < size; j++)
        {
            C[i * size + j] = 0;
            for (int k = 0; k < size; k++)
            {
                C[i * size + j] += A[i * size + k] * B[k * size + j];
            }
        }
}

void mataccsum(int* A, int* C, int size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
        {
            C[i * size + j] += A[i * size + j];
        }
}

void matpartition(int* A,int matsize,int* partitionA,int part_size,int part_matsize)
{
    int cnt = 0;
    for (int k = 0; k < part_size; k++)
    {
        for (int h = 0; h < part_size; h++)
        {
            for (int i = k * part_matsize; i < (k + 1) * part_matsize; i++)
            {
                for (int j = h * part_matsize; j < (h + 1) * part_matsize; j++)
                    partitionA[cnt++] = A[i * matsize + j];
            }
        }
    }
}

void matrestore(int* partitionA, int matsize, int* A, int part_size, int part_matsize)
{
    int cnt = 0;
    for (int k = 0; k < part_size; k++)
    {
        for (int h = 0; h < part_size; h++)
        {
            for (int i = k * part_matsize; i < (k + 1) * part_matsize; i++)
            {
                for (int j = h * part_matsize; j < (h + 1) * part_matsize; j++)
                    A[i * matsize + j] = partitionA[cnt++];
            }
        }
    }
}

void prtmat(int* A,int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printf("%4d  ",A[i*size+j]);
        }
        printf("\n");
    }
}

int main(int argc, char* argv[])
{
    int rank, comm_sz;
    const int matsize = 1024;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Status status;
    int* A = NULL;
    int* B = NULL;
    int* C = NULL;

    double elapsed_time = 0.0;
    if (comm_sz == 1)
    {
        A = (int*)calloc(matsize * matsize, sizeof(int));
        B = (int*)calloc(matsize * matsize, sizeof(int));
        C = (int*)calloc(matsize * matsize, sizeof(int));
        matgene(A, matsize);
        matgene(B, matsize);
        elapsed_time = -MPI_Wtime();
        matmulti(A, B, C, matsize);
        elapsed_time += MPI_Wtime();
        printf("elapsed time when no paeallel: %f\n", elapsed_time);
//        printf("The result of the product:\n");
 //       prtmat(C, matsize);
    }
    else
    {
        elapsed_time = -MPI_Wtime();
        int part_size = sqrt(comm_sz);
        int part_matsize = matsize / part_size;
        int* partA = (int*)calloc(part_matsize* part_matsize, sizeof(int));
        int* partB = (int*)calloc(part_matsize* part_matsize, sizeof(int));
        int* partC = (int*)calloc(part_matsize * part_matsize, sizeof(int));
        int periods[2] = { 1,1 };
        int dims[2] = { part_size,part_size };
        MPI_Comm cart_comm;
        int coords[2];
        int right = 0, left = 0, down = 0, up = 0;
        MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);
        MPI_Cart_coords(cart_comm, rank, 2, coords);
        int* partitionA = NULL;
        int* partitionB = NULL;
        if (rank == 0)
        {
            A = (int*)calloc(matsize * matsize, sizeof(int));
            B = (int*)calloc(matsize * matsize, sizeof(int));
            C = (int*)calloc(matsize * matsize, sizeof(int));
            matgene(A, matsize);
            matgene(B, matsize);
            partitionA = (int*)calloc(matsize*matsize, sizeof(int));
            matpartition(A, matsize, partitionA, part_size, part_matsize);
            partitionB = (int*)calloc(matsize*matsize, sizeof(int));
            matpartition(B, matsize, partitionB, part_size, part_matsize);
        }
        MPI_Scatter(partitionA, part_matsize * part_matsize, MPI_INT, partA, part_matsize * part_matsize, MPI_INT, 0, cart_comm);
        MPI_Scatter(partitionB, part_matsize * part_matsize, MPI_INT, partB, part_matsize * part_matsize, MPI_INT, 0, cart_comm);
        free(partitionA);
        free(partitionB);
        //进行初始移位，A的第(i,j)个块向左循环移动i步，B的第(i,j)个块向上循环移动j步
        MPI_Cart_shift(cart_comm, 1, coords[0], &left, &right);
        MPI_Cart_shift(cart_comm, 0, coords[1], &up, &down);
        MPI_Sendrecv_replace(partA, part_matsize * part_matsize, MPI_INT, left, 11, right, 11, cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv_replace(partB, part_matsize * part_matsize, MPI_INT, up, 11, down, 11, cart_comm, MPI_STATUS_IGNORE);
        int* partCproduct = (int*)calloc(part_matsize * part_matsize, sizeof(int));
        matmulti(partA, partB, partCproduct, part_matsize);
        mataccsum(partCproduct, partC,part_matsize);

        //然后用cannon算法进行相加，每次A的第(i,j)个块向左循环移动1步，B的第(i,j)个块向上循环移动1步
        MPI_Cart_shift(cart_comm, 1, 1, &left, &right);
        MPI_Cart_shift(cart_comm, 0, 1, &up, &down);
        for (int i = 1; i < part_size; i++)
        {
            MPI_Sendrecv_replace(partA, part_matsize * part_matsize, MPI_INT, left, 11, right, 11, cart_comm, MPI_STATUS_IGNORE);
            MPI_Sendrecv_replace(partB, part_matsize * part_matsize, MPI_INT, up, 11, down, 11, cart_comm, MPI_STATUS_IGNORE);
            matmulti(partA, partB, partCproduct, part_matsize);
            mataccsum(partCproduct, partC, part_matsize);
        }
        free(partCproduct);

        int* partitionC = NULL;
        if(rank == 0)
            partitionC = (int*)calloc(matsize * matsize, sizeof(int));
        MPI_Gather(partC, part_matsize * part_matsize, MPI_INT, partitionC, part_matsize * part_matsize, MPI_INT, 0, cart_comm);
        //还原C矩阵
        if (rank == 0)
        {
            matrestore(partitionC, matsize, C, part_size, part_matsize);
        }
        free(partitionC);
        MPI_Comm_free(&cart_comm);
        elapsed_time += MPI_Wtime();
        if (rank == 0)
        {
            printf("elapsed time when paeallel with %d processes: %f\n", comm_sz, elapsed_time);
 //           printf("The result of the product:\n");
 //           prtmat(C, matsize);
        }
        free(partA);
        free(partB);
        free(partC);
    }
    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}
