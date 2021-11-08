#include "mpi.h"
#include <stdio.h>
#include<stdlib.h>
#include<string.h>

int mpi_my_allgather(const void* sendbuf, int sendcount, MPI_Datatype sendtype, void* recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
    int rank, size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < size; i++)
    {
        if (rank != i)
            MPI_Send(sendbuf, sendcount, sendtype, i, i, MPI_COMM_WORLD);
        else
            memcpy(recvbuf + i * recvcount * sizeof(recvtype), sendbuf, sendcount * sizeof(sendtype));
    }
    for (int i = 0; i < size; i++)
    {
        if (rank != i)
        {
            void *tmp = (void*)malloc(recvcount * sizeof(recvtype));
            MPI_Status status;
            MPI_Recv(tmp, recvcount, recvtype, i, rank, comm, &status);
            memcpy(recvbuf + i * recvcount * sizeof(recvtype), tmp, sendcount * sizeof(sendtype));
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{
    int rank, size;
    double my_elapsed_time = 0.0;
    double mpi_elapsed_time = 0.0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int sdata = rank + 1;
    int* myrdata = (int*)malloc((size + 1) * sizeof(int));
    int* mpirdata = (int*)malloc((size + 1) * sizeof(int));

    MPI_Barrier(MPI_COMM_WORLD);
    my_elapsed_time = -MPI_Wtime();
    mpi_my_allgather(&sdata, 1, MPI_INT, myrdata, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    my_elapsed_time += MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    mpi_elapsed_time = -MPI_Wtime();
    MPI_Allgather(&sdata, 1, MPI_INT, mpirdata, 1, MPI_INT, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    mpi_elapsed_time += MPI_Wtime();

    for (int i = 0; i < size; i++)
    {
        if (i == rank)
        {
            printf("**********%d**************", rank);
            printf("\n");
            for (int i = 0; i < size; i++)
            {
                printf("My implement: %d\n", myrdata[i]);
                printf("MPI: %d\n", mpirdata[i]);
            }
            printf("\n");
            printf("**********%d**************", rank);
            printf("\n");
        }
    }
    if (!rank)
    {
        printf("my_elapsed_time:%f\n", my_elapsed_time);
        printf("mpi_elapsed_time:%f\n", mpi_elapsed_time);
    }
    MPI_Finalize();
    return 0;
}