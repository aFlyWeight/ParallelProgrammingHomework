// MonteCarloAlgorithmForPI.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#define _USE_MATH_DEFINES
#include <math.h>


int main(int argc, char** argv)
{
    unsigned long point_num_incyc = 0;
    unsigned long point_num;
    long long int i;
    int thread_num;
    printf("Input thread_num:\n");
    scanf("%d",&thread_num);
    printf("Input point_num:\n");
    scanf("%lu", &point_num);
    srand(time(NULL));
    double x, y;
    double elapsedtime = 0.0;
    elapsedtime = -omp_get_wtime();
#pragma omp parallel for num_threads(thread_num) default(none) reduction(+:point_num_incyc) shared(point_num) private(i,x,y)
    for (i = 0; i < point_num; i++)
    {

        x = (double)rand() / (double)RAND_MAX;
        y = (double)rand() / (double)RAND_MAX;
        if (x * x + y * y <= 1)
        {
            point_num_incyc++;
        }
    }
    double est_pi = 4 * (double)point_num_incyc / point_num;
    elapsedtime += omp_get_wtime();
    printf("the estimate value of pi is % lf\n", est_pi);
    printf("the absolute error is % lf\n", fabs(est_pi- M_PI)/M_PI);
    printf("elapsedtime: % lf\n", elapsedtime);
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
