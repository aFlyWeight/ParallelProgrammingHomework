// qsortwithomp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <limits.h>
#include <omp.h>

int partition(int* num, int low, int high)
{
    int pivot = num[low];
    while (low < high)
    {
        while (low < high && num[high] >= pivot)high--;
        num[low] = num[high];
        while (low < high && num[low] <= pivot)low++;
        num[high] = num[low];
    }
    return low;
}

void myqsort(int* num, int low, int high)
{
    if (low < high)
    {
        int split = partition(num, low, high);
        #pragma omp parallel sections
        {
        #pragma omp section
            myqsort(num, low, split - 1);
        #pragma omp section
            myqsort(num, split + 1, high);
        }
    }
}

void myqsort2(int* num, int low, int high)
{
    if (low < high)
    {
        int split = partition(num, low, high);
        myqsort(num, low, split - 1);
        myqsort(num, split + 1, high);
    }
}

int main()
{
//    const int len = 10;
    const int len = 1000000;
    int* num = (int*)calloc(len, sizeof(int));
    int* numtmp = (int*)calloc(len, sizeof(int));
    double elapsedtime = 0.0;
    srand(time(NULL));
    int element = 0;
    for (int i = 0; i < len; i++)
    {
        element = rand() % INT_MAX;
        num[i] = element;
        numtmp[i] = element;
    }
    /*
    printf("The array before sort:\n");
    for (int i = 0; i < len; i++)
    {
        printf("%4d ", num[i]);
    }
    printf("\n");
    */
    elapsedtime = -omp_get_wtime();
    myqsort(num, 0, len-1);
    elapsedtime += omp_get_wtime();
    printf("elapsedtime with omp: % lf\n", elapsedtime);
    //测试没有使用omp
    elapsedtime = -omp_get_wtime();
    myqsort2(numtmp, 0, len - 1);
    elapsedtime += omp_get_wtime();
    printf("elapsedtime without omp: % lf\n", elapsedtime);
    /*
    printf("The array after sort:\n");
    for (int i = 0; i < len; i++)
    {
        printf("%4d ", num[i]);
    }
    printf("\n");
    */
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
