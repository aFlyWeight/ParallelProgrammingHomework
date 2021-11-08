// 生命游戏.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define N 52
#define M 52
char cells[N][M];

void init(char cells[N][M])
{
    srand(time(0));

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            if (i == 0 || j == 0 || i == N - 1 || j == M - 1)
                cells[i][j] = '*';
            else
                cells[i][j] = rand() % 2+'0';
}

void show(char cells[N][M])
{
    for (int i = 0; i < N; i++)
    {
        printf("%2d ", i);
        for (int j = 0; j < M; j++)
            printf("%c ",cells[i][j]);
        printf("\n");
    }
}

void MatricToLinear(char C[N][M], char* line)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            *line++ = C[i][j];
}

void LinearToMatric(char* line, char arry[][M], int len)
{
    for (int i = 0; i < len; i++)
        for (int j = 0; j < M; j++)
            arry[i][j] = *line++;
}
int around(char cells[N][M], int x, int y)
{
    int cnt = 0;
    for (int i = x - 1; i < x + 2; i++)
        for (int j = y - 1; j < y + 2; j++)
            if (cells[i][j] == '1')
                cnt++;
    cnt--;
    return cnt;
 }

void evolve(char cells[N][M], int srow, int erow)
{
    int lives;
    for (int i = srow; i < erow; i++)
        for (int j = 1; j < M -1; j++)
        {
            lives = around(cells, i, j);
            if (cells[i][j] == '1')
            {
                if (lives < 2|| lives > 3)
                    cells[i][j] = '0';
            }
            else if (cells[i][j] == '0')
            {
                if (lives == 3)
                    cells[i][j] = '1';
            }
        }
        
}

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Status status;
    int id;
    int size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int prows = (N - 2) / (size - 1);
    int startrow = 0;
    int endrow = 0;
    int outputnum = atoi(argv[1]);
    int cyclenum = atoi(argv[2]);
    if (!id)
    {
        init(cells);
    }
    for (int i = 0; i < cyclenum; i++)
    {
        if (i % outputnum == 0)
        {
            if (!id)
            {
                show(cells);
                printf("\n");
            }
        }
        if (!id)
        {
            evolve(cells, 1, prows+1);
            MPI_Send(cells[prows - 1], M, MPI_CHAR, id + 1, 0, MPI_COMM_WORLD);
        }
        else if (id == size - 1)
        {
            char* cellarr = (char*)malloc(M * sizeof(char));
            MPI_Recv(cellarr, M, MPI_CHAR, id - 1, 0, MPI_COMM_WORLD, &status);
            for (int i = 0; i < M; i++)
                cells[id * prows][i] = cellarr[i];
            evolve(cells, id * prows + 1, N);
        }
        else
        {
            char* cellarr = (char*)malloc(M * sizeof(char));
            MPI_Recv(cellarr, M, MPI_CHAR, id - 1, 0, MPI_COMM_WORLD,&status);
            for (int i = 0; i < M; i++)
                cells[id * prows][i] = cellarr[i];
            evolve(cells, id * prows + 1, (id + 1) * prows + 1);
        }  
    }
    MPI_Finalize();
    return 0;
//    std::cout << "Hello World!\n";
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
