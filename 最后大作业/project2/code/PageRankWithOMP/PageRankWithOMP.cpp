// PageRankWithOMP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

typedef struct node {
    int v;
    int indegree;
    int outdegree;
    int inedges[10];
}ND;

void genegraph(ND* nodes, unsigned long  nodenumber)
{
    srand(time(NULL));
    for (int i = 0; i < nodenumber; i++)
    {
        nodes[i].v = i;
        nodes[i].indegree = rand() % 10 + 1;
        int nodetmp = 0;
        for (int j = 0; j < nodes[i].indegree;)
        {
            int totalnum = 0;
            int num = 0;
            for (int k = 0; k < 6; k++)
            {
                num = rand() % 10;
                totalnum += num * pow(10, k);
            }
            int num2 = rand() % (nodenumber - 1000000);
            nodetmp = totalnum + num2;
//            nodetmp = rand() % nodenumber;
            if (nodetmp != i)
            {
                nodes[i].inedges[j] = nodetmp;
                nodes[nodetmp].outdegree++;
                j++;
            }
        }
    }
    FILE* fp = fopen("data.txt", "w");
    for (int i = 0; i < nodenumber; i++)
    {
        fprintf(fp, "%7d %7d %7d ", nodes[i].v, nodes[i].indegree, nodes[i].outdegree);
        for (int j = 0; j < nodes[i].indegree; j++)
        {
            fprintf(fp, "%7d ", nodes[i].inedges[j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void prtgraph(ND* nodes, unsigned long nodenumber)
{
    printf("The graph is:\n");
    for (int i = 0; i < nodenumber; i++)
    {
        printf("node num:%d\n", nodes[i].v);
        printf("node indegree:%d\n", nodes[i].indegree);
        printf("node outdegree:%d\n", nodes[i].outdegree);
        printf("In edges:\n");
        for (int j = 0; j < nodes[i].indegree; j++)
        {
            printf("%d ", nodes[i].inedges[j]);
        }
        printf("\n");
    }
}

int main()
{
    const unsigned long nodenumber = 1024000;
    const int maxinterations = 100;
    int thread_num;
    printf("Input thread_num:\n");
    scanf("%d", &thread_num);
    ND* nodes = (ND*)calloc(nodenumber, sizeof(ND));
    FILE* fp = fopen("data.txt", "r");
    if (fp == NULL)
    {
        genegraph(nodes, nodenumber);
    }   
    else
    {
        int i = 0;
        while (!feof(fp))
        {
            fscanf(fp, "%d %d %d ", &nodes[i].v,&nodes[i].indegree, &nodes[i].outdegree);
            for (int j = 0; j < nodes[i].indegree; j++)
            {
                fscanf(fp, "%d ", &nodes[i].inedges[j]);
            }
            i++;
        }
        fclose(fp);
    }
//    prtgraph(nodes, nodenumber);
    double* PR = (double*)calloc(nodenumber, sizeof(double));
    //将所有的网页PR值初始化为1,d = 0.85
    double d = 0.85;
    for (int i = 0; i < nodenumber; i++)
    {
        PR[i] = 1.0;
    }
    //迭代更新PR值
    double elapsedtime = 0.0;
    elapsedtime = -omp_get_wtime();
    for (int i = 0; i < maxinterations; i++)
    {
#pragma omp parallel for num_threads(thread_num) schedule(static)
        for (int u = 0; u < nodenumber; u++)
        {
            double sum = 0.0;
            for (int j = 0; j < nodes[u].indegree; j++)
            {
                int v = nodes[u].inedges[j];
                sum += (double)PR[v] / nodes[v].outdegree;   //v的出度不可能为0
            }
            PR[u] = (double)(1 - d) / nodenumber + d * sum;
        }
        //normalize
        double psum = 0.0;
#pragma omp parallel for num_threads(thread_num) reduction(+:psum) schedule(static)
        for (int u = 0; u < nodenumber; u++)
            psum += PR[u];
#pragma omp parallel for num_threads(thread_num) schedule(static)
        for (int u = 0; u < nodenumber; u++)
            PR[u] = PR[u] / psum;

    }
    elapsedtime += omp_get_wtime();
    printf("elapsedtime: % lf\n", elapsedtime);
    FILE* fp1 = fopen("result.txt", "w");
    printf("The result PR is in flie result.txt!");
    for (int i = 0; i < nodenumber; i++)
    {
        fprintf(fp1,"%.8f\n", PR[i]);
    }
    fclose(fp1);
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
