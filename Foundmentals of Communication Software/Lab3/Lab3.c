#include <stdio.h>
#include <math.h>
#define SIZE 15//国家数量

typedef struct{
    float first;//2006
    float second;//2010
    float third;//2007
}GRADE;//三次战绩

typedef struct {
    char country[30];//国家名
    GRADE grade;//三次战绩
    int category;//类别
}NATION;//国家

typedef struct {
    int rank;//聚类等级
    GRADE center;//质心
    int members[SIZE];//本类成员的索引
    int num;//成员数量
} CATEGORY;

/*--------------------------------------------------------------数据导入和处理模块-----------------------------------------------------------------------*/
/**
 *函数名：copyGrade
 * 功能：复制GRADE型变量
 * 参数列表：
 *       GRADE *grade1：指向副本的指针
 *       GRADE *grade2：指向原本的指针
 *返回值：无
 */
void copyGrade(GRADE *grade1, GRADE *grade2) {
    grade1->first = grade2->first;
    grade1->second = grade2->second;
    grade1->third = grade2->third;
}

/**
 *函数名：findMaxAndMin
 * 功能：寻找最大值和最小值的子过程。当当前成绩中，某次成绩大于当前成绩最大值或小于当前成绩最小值，则更新最大值和最小值
 *参数列表：
 *      GRADE grade：当前国家的三次战绩
 *      GRADE *max：指向存放15个国家三次战绩最大值变量的指针
 *      GRADE *min：指向存放15个国家三次战绩最小值变量的指针
 *返回值：无
 */
void findMaxAndMin(GRADE grade, GRADE *max, GRADE *min){
    if(grade.first > max -> first)    max->first = grade.first;
    if(grade.first < min -> first)    min->first = grade.first;

    if(grade.second > max -> second)  max->second = grade.second;
    if(grade.second < min -> second)  min->second = grade.second;

    if(grade.third > max -> third)    max->third = grade.third;
    if(grade.third < min -> third)    min->third = grade.third;
}

/**
 *函数名:transformGrade
 *功能：根据最大值和最小值，将三次成绩分别映射到[0, 1]上，便于后续比较
 *参数列表:
 *        GRADE *grade：某个国家的三次战绩
 *        GRADE max：当前三次战绩最大值
 *        GRADE min：当前三次战绩最小值
 *返回值:无
 */
void transformGrade(GRADE *grade, GRADE max, GRADE min){
    grade -> first = (grade -> first - min.first)/(max .first - min.first);
    grade -> second = (grade -> second - min.second)/(max.second - min.second);
    grade -> third = (grade -> third - min.third)/(max.third - min.third);
}

/**
 *函数名:display
 *功能:屏幕输出15个国家的三次战绩
 *参数列表:
 *        NATION nations：储存各个国家数据的数组
 *返回值：无
 */
void display(NATION nations[]){
    int i;
    printf("Country\t\t\t2006\t\t2010\t\t2007\n");
    printf("------------------------------------------------------------\n");
    for(i = 0; i < SIZE; i++)
        printf("%-10s\t\t%.2f\t\t%.2f\t\t%.2f\n", nations[i].country, nations[i].grade.first, nations[i].grade.second, nations[i].grade.third);
    printf("\n");
}

/**
 *函数名：loadData
 *功能：将文件中储存的15个国家3次战绩导入程序，并用结构体数组
 *参数列表：
 *        NATION nations：存储战绩数据的结构体数组
 *返回值:无
 */
void loadData(NATION nations[]) {
    int i;
    GRADE max, min;
    FILE *fp=fopen("worldCup.txt","r");
    fscanf(fp,"%s%f%f%f",nations[0].country, &nations[0].grade.first, &nations[0].grade.second, &nations[0].grade.third);

    //初始化最值
    copyGrade(&max, &nations[0].grade);
    copyGrade(&min, &nations[0].grade);
    for(i = 1; !feof(fp) && i < SIZE; i++) {
        fscanf(fp,"%s%f%f%f",nations[i].country, &nations[i].grade.first, &nations[i].grade.second, &nations[i].grade.third);
        findMaxAndMin(nations[i].grade, &max, &min);
    }

    //检查是否成功输入数据
    display(nations);
    for(i = 0; i < SIZE; i++)
        transformGrade(&nations[i].grade, max, min);
    //检查映射结果是否正确
    display(nations);
}

/*------------------------------------------------------------------聚类模块 -----------------------------------------------------------------------------------------*/
/**
 *函数名：getDistance
 * 功能：计算两个质心之间的欧式距离
 *参数列表：
 *        GRADE grade1：操作数1
 *        GRADE grade2：操作数2
 *返回值:
 *        float：两个“战绩”的欧式距离
 */
float getDistance(GRADE grade1, GRADE grade2){
    return sqrt(pow(grade1.first - grade2.first, 2) + pow(grade1.second - grade2.second, 2) + pow(grade1.third - grade2.third, 2));
}

/**
 *函数名：cluster
 *功能：为每个nation分类
 *参数列表
 *        NATION *nation：待分类国家
 *        CATEGORY cate[]:n个聚类
 *        int n：目标聚类个数
 *返回值:无
 */
void cluster(NATION *nation, CATEGORY cate[], int n){
    int i;
    float distance, temp;
    distance = getDistance(nation->grade, cate[0].center);
    nation->category = 0;
    for(i = 1; i < n; i++) {
            temp = getDistance(nation->grade, cate[i].center);
            if(distance > temp) {
                distance = temp;
                nation->category = i;
            }
        }
}

/**
 *函数名:updateCenter
 *功能:聚类各成员每次战绩的平均值作为新质心，
 *参数列表：
 *        NATION nations[]：参与聚类的所有国家
 *        CATEGORY cate[]：n个聚类
 *        int n：目标聚类个数
 *返回值:
 *        int：质心更新前后是否一致，若是则为1，否则为0
 */
int updateCenter(NATION nations[], CATEGORY cate[], int n){
    int i, j, changed = 0;//i,j：循环辅助变量，changed：质心是否改变标志
    GRADE sum, mean;
    for(i = 0; i < n; i++) {
            sum.first = sum.second = sum.third = 0;
            for(j = 0; j < cate[i].num; j++) {
                sum.first += nations[cate[i].members[j]].grade.first;
                sum.second += nations[cate[i].members[j]].grade.second;
                sum.third += nations[cate[i].members[j]].grade.third;
            }

            mean.first = sum.first/cate[i].num;
            mean.second = sum.second/cate[i].num;
            mean.third = sum.third/cate[i].num;

            if(cate[i].center.first != mean.first) {
                cate[i].center.first = mean.first;
                changed = 1;
            }
            if(cate[i].center.second != mean.second) {
                cate[i].center.second = mean.second;
                changed = 1;
            }
            if(cate[i].center.third != mean.third) {
                cate[i].center.third = mean.third;
                changed = 1;
            }
        }
    return changed;
}

/**
 *函数名:nameCategory
 *功能：为KMeans算法所得每个聚类命名
 *参数列表:
 *        CATEGORY cate[]：各个聚类
 *返回值:无
 */
void nameCategory(CATEGORY cate[]){
    int i, j, temp;
    float score[3];
    for(i < 0; i < 3; i++)
        score[i] = sqrt(pow(cate[i].center.first, 2) + pow(cate[i].center.second, 2) + pow(cate[i].center.third, 2));
    for(i = 0; i < 3; i++){
            cate[i].rank = 1;
        for(j = 1; j < 3; j++){
                if(score[i] > score[i+j])
                    cate[i].rank++;
        }
    }
}

/**
 *函数名:kMeans
 * 功能：使用K-Means算法为以每个国家三次战绩为主关键字，为国家分类
 * 参数列表
 *        NATION nations[]：待分类国家
 *        CATEGORY cate[]：聚类结果
 *返回值：无
 */
void kMeans(NATION nations[], CATEGORY cate[]) {
    int changed, i;
    //质心初始化,其中0/8/12可任取，不同初始化方式可能导致不同结果
    copyGrade(&cate[0].center, &nations[0].grade);
    copyGrade(&cate[1].center, &nations[8].grade);
    copyGrade(&cate[2].center, &nations[12].grade);

    do {
        cate[0].num = cate[1].num = cate[2].num = 0;
        for(i=0; i<SIZE; i++){
                cluster(nations+i, cate, 3);
                cate[nations[i].category].members[cate[nations[i].category].num++] = i;
        }
        changed = updateCenter(nations, cate, 3);
    } while(changed);
    nameCategory(cate);
}

int main() {
    NATION nations[SIZE];
    CATEGORY category[3];

    loadData(nations);
    kMeans(nations, category);

    printf("China is belong to rank.%d\n", category[nations[0].category].rank);
    return 0;
}
