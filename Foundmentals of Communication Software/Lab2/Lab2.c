#include <stdio.h>
#define INF 10000//相对无穷大
#define VERTEXSIZE 6//顶点数

typedef struct ShortestPathOfVertex {
    int size;//路径实际长度-2，因为起点已由beginning变量存储，终点为当前结点
    int power;//路径权值
    int beginning;//路径起点
    int path[VERTEXSIZE-2];//路径
} PATH;//最短路径

typedef struct VertexOfGraph {
    char id[50];//节点名
    PATH shortestPath;//此顶点的最短路径
} VERTEX;//节点

typedef struct {
    VERTEX vertex[VERTEXSIZE];//图的节点
    int adjMatrix[(VERTEXSIZE+1)*VERTEXSIZE/2];//压缩存储的邻接矩阵
} GRAPH;//图

/*------------------------------------------------------路由拓补图的导入和存取模块--------------------------------------------*/
/**
 *函数名 setTime
 * 功能:按照对称矩阵压缩存储的规则放置时间值
 *参数列表:
 *      int matrix[]：存储图的矩阵
 *      int row：行序号
 *      int col：列序号
 *      int time：时间值
 *返回值:无
 */
void setTime(int matrix[], int row, int col, int time) {
    if(row >= col)
        matrix[col + row*(row+1)/2] = time;
    else
        matrix[row + col*(col+1)/2] = time;
}

/**
 *函数名：getTime
 * 功能：按照对称矩阵压缩存储的规则获取时间值
 *参数列表:
 *      int matrix[]：存储图的矩阵
 *      int row：行序号
 *      int col：列序号
 *返回值:
 *      int：时间
 */
int getTime(int *matrix, int row, int col) {
    if(row >= col)
        return matrix[col + row*(row+1)/2];
    else
        return matrix[row + col*(col+1)/2];
}

/**
 *函数名：initGraph
 * 功能：将路由表文件中的数据导入程序
 *参数列表:
 *      GRAPH graph：待操作的图
 */
void initGraph(GRAPH *graph) {
    FILE *fp;
    char path[50];
    char temp[50];
    int i,index, time;
    //初始化邻接矩阵
    for(i = 0; i < (VERTEXSIZE+1)*VERTEXSIZE/2; i++)
        graph->adjMatrix[i] = INF;
    for(i = 0; i < VERTEXSIZE; i++) {
        //初始化每个节点最短路径的参数
        graph->vertex[i].shortestPath.size = 0;
        graph->vertex[i].shortestPath.power = INF;
        graph->vertex[i].shortestPath.beginning = 0;

        sprintf(graph->vertex[i].id, "%d", i+1);//为顶点命名
        sprintf(path, "router\\router%d.txt", i);
        fp = fopen(path, "r");
        fgets(temp, sizeof(temp), fp);//去除文件第一行的说明
        while(!feof(fp)) {//读入数据
            fscanf(fp, "%d\t%d\n", &index, &time);
            setTime(graph->adjMatrix, i, index-1, time);
        }
        fclose(fp);
    }
}

/**
 *函数名 display
 * 功能:将图对应的邻接矩阵屏幕输出
 *参数列表:
 *      GRAPH graph：需屏幕输出的图
 */
void display(GRAPH graph) {
    int i, j, temp;
    printf("The adjacent matrix of Graph is listed as follows:\n");
    for(i = 0; i < VERTEXSIZE; i++)
        printf("\t%s", graph.vertex[i].id);//横向输出节点名
    for(i = 0; i <VERTEXSIZE ; i++) {
        printf("\n\n%s", graph.vertex[i].id);//纵向输出节点名
        for(j = 0; j < VERTEXSIZE; j++) {
            temp = getTime(graph.adjMatrix, i, j);//输出矩阵的值
            if(temp == INF)
                printf(" \tINF");
            else
                printf("\t%d", temp);
        }
    }
    printf("\n");
}

/*----------------------------------------------------------------获取最短路径模块---------------------------------------------------------*/
/**
 *函数名：getShortestPath
 * 功能：利用迪杰斯特拉算法获取图各个结点的最短路径
 *参数列表:
 *      GRAPH graph：待操作的图
 *返回值：无
 */
void getShortestPath(GRAPH *graph) {
    int i, j, from, to, rear = 1, front = 0;
    int queue[VERTEXSIZE-1];//辅助顺序队列
    int tempPower[VERTEXSIZE-1];//辅助顺序栈

    for(i = 1; i < VERTEXSIZE; i++)//将与起始结点相连的所有节点入队
        if(getTime(graph->adjMatrix, 0, i) < INF) {
            queue[rear++] = i;
            tempPower[i-1] = getTime(graph->adjMatrix, 0, i);
        } else
            tempPower[i-1] = INF;

    while(++front != rear)//队列是否为空
        //从每个刚入队的新节点(上次遍历，最短路径被刷新的结点)，依次与出此节点和源结点外其他结点做寻找最短路所需要的权值计算
        for(i = 1, from = queue[front%(VERTEXSIZE-1)]; i < VERTEXSIZE; i++) {
            to = (from+i)%VERTEXSIZE;//路径结束结点
            if(tempPower[from-1] + getTime(graph->adjMatrix, from, to) < tempPower[to-1]) {//判断新路径的权值是否比原路径小
                tempPower[to-1] = tempPower[from-1] + getTime(graph->adjMatrix, from, to);//记录这个更小的权值
                queue[(rear++)%(VERTEXSIZE-1)] = to;//由于此节点的最短路被刷新，所以进入队列，以便依次检验通过此节点到其他路径是否最短

                for(j = 0; j < graph->vertex[from].shortestPath.size; j++)//复制当前最短路的路径
                    graph->vertex[to].shortestPath.path[j] = graph->vertex[from].shortestPath.path[j];
                graph->vertex[to].shortestPath.path[j] = from;//将更短路尾，节点加入路径，因为在记录最短路时，没有记录为节点
                graph->vertex[to].shortestPath.size = graph->vertex[from].shortestPath.size + 1;//(路径节点数-2)增加。
            }
        }
    for(i = 0; i < VERTEXSIZE-1; i++)
        graph->vertex[i+1].shortestPath.power = tempPower[i];//记录路径权值
}

/**
 *函数名：displayShortestPath
 * 功能：屏幕输出图各个结点自起始点1的最短路径
 *参数列表:
 *      GRAPH graph：待操作的图
 *      int beginning：起始点
 *返回值:无
 */
void displayShortestPath(GRAPH graph, int beginning) {
    int i, j;
    PATH tempPath;//为操作方便，使用临时变量记录某个结点的最短路径
    printf("The shortest path starting at %s of each graph's vertexes is listed as follows:\n", graph.vertex[beginning].id);
    printf("Id\tShortest Path\tPower\n");
    for(i = 1; i < VERTEXSIZE; i++) {
        tempPath = graph.vertex[i].shortestPath;
        if(tempPath.power != INF) {//若最短路径不为相对无穷大，就屏幕输出所得出的最短路径
            printf("%s\t%s", graph.vertex[i].id, graph.vertex[tempPath.beginning].id);
            for(j = 0; j < tempPath.size; j++)
                printf("->%s", graph.vertex[tempPath.path[j]].id);
            printf("->%s\t\t%d\n", graph.vertex[i].id, tempPath.power);
        } else//否则直接显示起点指向终点，权值为无穷大
            printf("%s:%s->%s\tINF", graph.vertex[i].id, graph.vertex[tempPath.beginning].id, graph.vertex[i].id);
    }
}

int main() {
    GRAPH graph;
    initGraph(&graph);
    display(graph);
    getShortestPath(&graph);
    displayShortestPath(graph, 0);
    return 0;
}
