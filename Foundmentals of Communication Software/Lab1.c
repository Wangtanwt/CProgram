#include <stdio.h>
#include<string.h>
#include<math.h>
#include<malloc.h>
#define SIZE 48
typedef struct BaseStation {
    double intensity;
    int index;
    int length;
} BASESTATION;
FILE *fp;
char root[50] = "data\\";//待处理文件根目录路径，文件夹名为data，需要根据不同文件路径而更改
char path[100];//存放单个具体文件的路径


/**
 *函数名：getIntensity
 *功能：根据输入的文件地址，读取、计算模值、求和后作为信号强度输出。
 *参数列表：
 *      char *filePath：目标文件地址字符串
 *      int *length：记录目标文件的复数个数，以便后边进行相关性检测
 *返回值：
 *      double：按照信号强度计算公式得到的信号强度
 */
double getIntensity(char *filePath, int *length) {
    fp = fopen(filePath, "r");
    *length = 0;
    double real, imaginary, intensity = 0.0;
    while(!feof(fp)) {//读取文件中的所有复数，同时取模求和
        fscanf(fp, "%lf\n%lf\n", &real, &imaginary);
        intensity += sqrt(real*real+imaginary*imaginary);
        (*length) += 2;
    }
    fclose(fp);
    return intensity;
}

/**
 *函数名：loadFile
 *功能：分别计算所有文件的信号强度、复数个数，并存放于数组中便于后续操作
 *参数列表：
 *      BASESTATION *baseStation：将每个data文件的相关参数存于其中
 *返回值：无
 */
void loadFile(BASESTATION *baseStation) {
    int i, j, k;
    for(i = 0, k = 0; i <= 60; i += 20)
        for(j = 0; j < 12; j++, k++) {
            sprintf(path, "%sdata%d.txt", root, i+j);//拼接文件路径
            baseStation[k].intensity= getIntensity(path, &baseStation[k].length);
            baseStation[k].index = i+j;//由于后边要排序，将打乱原始顺序，故保存文件原始序号       }
        }
}

/**
 *函数名：copyBase
 *功能：复制一个BASESTATION型变量所有成员的值
 *参数列表：
 *      BASESTATION *base1：接收复制的值；
 *      BASESTATION *base2：需要复制的BASESTATION型变量的地址
 *返回值：无
 */
void copyBase(BASESTATION *base1, BASESTATION *base2) {
    base1->index = base2->index;
    base1->intensity = base2->intensity;
    base1->length = base2->length;
}

/**
 *函数名：swapBase
 *功能：交换两个BASESTATION型变量的值
 *参数列表：
 *      BASESTATION *base1：待交换变量1
 *      BASESTATION *base2：待交换变量2
 *返回值：无
 */
void swapBase(BASESTATION *base1, BASESTATION *base2) {
    BASESTATION temp;
    copyBase(&temp, base1);
    copyBase(base1, base2);
    copyBase(base2, &temp);
}

/**
 *函数名：findPivot
 *功能：以信号强度为主关键字，将BASETATION数组首部、尾部、中间的信号强度次大值作为快排枢纽，并将此次大值与尾部交换
 *参数列表：
 *      BASESTATION *base：待排序的数组
 *      BASESTATION *p：枢纽元素
 *      int low：待排序序列首索引
 *      int mid：待排序序列中间索引
 *      int high：待排序序列尾索引
 *返回值：无
 */
void findPivot(BASESTATION *base, BASESTATION *p, int low, int mid, int high) {//三者取中
    int i, j;
    BASESTATION temp[3] ;//借助辅助数组对其排序。
    copyBase(temp, base+low);
    copyBase(temp+1, base+mid);
    copyBase(temp+2, base+high);

    //辅助数组简单排序
    for(i = 0; i < 3; i++)
        for(j = i+1; j < 3; j++)
            if(temp[i].intensity < temp[j].intensity)
                swapBase(temp+i, temp+j);

    copyBase(base+mid, temp);
    copyBase(base+high, temp+1);
    copyBase(base+low, temp+2);
    copyBase(p, temp+1);
}

/**
 *函数名：quickSort
 *功能：利用快速排序算法，以信号强度为主关键字为数组排序；
 *参数列表：
 *      base：待排序数组
 *      front：数组首索引
 *      rear：数组尾索引
 *返回值: 无
 */
void quickSort(BASESTATION *base, int front, int rear) {
    if(front >= rear) return;
    BASESTATION p;
    int low = front, high = rear, mid = (rear-front)/2+front;
    findPivot(base, &p, low, mid, high);

    while(low < high) {
        while(base[low].intensity >=  p.intensity && low < high)
            low++;
        if(low >= high) break;
        copyBase(base + high, base+low);
        while(base[high].intensity <= p.intensity && high > low)
            high--;
        if(low >= high) break;
        copyBase(base + low, base+high);
    }
    copyBase(base+high, &p);
    quickSort(base, front, low-1);
    quickSort(base, high+1, rear);
}

/**
 *函数名：loadDataFile
 *功能：导入信号文件
 *参数列表：
 *      int index： "基站"序号
 *      double *data：接收导入的数据的双精度浮点型数组
 *返回值: 无
 */
void loadDataFile(int index, double *data) {
    double *pd = data;
    sprintf(path, "%sdata%d.txt",root, index);//构造文件地址字符串
    fp = fopen(path, "r");
    while(!feof(fp)) //存储文件中每个数字
        fscanf(fp, "%lf\n", pd++);
    fclose(fp);
}

/**
 *函数名：loadPssFile
 *功能：导入主同步信号文件PSS
 *参数列表：
 *      char *filePath： 文件地址
 *      double *pssdata：指向接收数据文件数组的指针
 *      int *psslen:文件长度
 *返回值: 无
 */
void loadPssData(char *filePath, int *psslen, double **pssData) {
    int i = 0;
    double temp;
    fp = fopen(filePath, "r");

    for((*psslen) = 0; !feof(fp); (*psslen )++) //获取PSS文件中数据的个数
        fscanf(fp, "%lf\n", &temp);

    double *pssdata = (double*)malloc((*psslen) * sizeof(double));//按照数据个数申请足够的空间
    rewind(fp);//重置文件指针到开头
    for(i = 0; !feof(fp); i++) //存储PSS文件中每个复数的实部和虚部
        fscanf(fp, "%lf\n", pssdata + i);
    fclose(fp);
    *pssData = pssdata;//经存储好的数据文件复制给pssData所指向的数组
}

/**
 *函数名：test
 *功能：相关性检验
 *参数列表：
 *      double *data： 需要检测的信号数据组
 *      double *pssData：检测所使用的主同步信号数组
 *      double *maxResult：指向最大检测结果的指针
 *      int *maxPos：指向最大检测结果对应的信号序列起始位的指针
 *      int *pss：指向最大结果所对应pss文件的指针
 *      int datalen：信号序列的长度
 *      int psslen：pss序列的长度
 *      int presentPss：本次检测所使用的的pss文件
 *返回值: 无
 */
void test(double *data, double *pssData, double *maxResult, int *maxPos, int *pss, int datalen, int psslen, int presentPss) {
    int i, j;
    double testResult, testRe, testIm;

    for(j = 0; j <= datalen-psslen; j+=2) { //滑动检测，同时寻找最大值，并记录取得最大值时滑动的长度
        for(i = 0, testRe = testIm = 0.0; i < psslen; i+=2) {
            testRe = data[i+j]*pssData[i] - data[i+j+1]*pssData[i+1];
            testIm = data[i+j]*pssData[i+1] + data[i+j+1]*pssData[i];
        }
        testResult = sqrt(testRe*testRe + testIm * testIm);

        if(*maxResult == 0.0)//为maxResult设初值
            *maxResult = testResult;
        else if(testResult > *maxResult) {
            *maxResult = testResult;//保存最大值
            *maxPos = j+1;//相关性最好的序列的起始位置
            *pss = presentPss;//保存所使用的Pss文件
        }
    }
}

/**
 *函数名：correlationTest
 *功能：相关性检验
 *参数列表：
 *      BASESTATION base:需要检测的"基站"
 *      double *maxResult：指向最大检测结果的指针
 *      int *maxPos：指向最大检测结果对应的信号序列起始位的指针
 *      int *pss：指向最大结果所对应pss文件的指针
 *返回值: 无
 */
void correlationTest(BASESTATION base, int *maxResult, int *maxPos,double *pss) {
    int psslen, i;
    double *data = (double*)malloc(base.length*sizeof(double));//根据文件长度动态申请连续存储空间，以存放文件中各个复数的实部和虚部，方便后续检验算法

    loadDataFile(base.index, data);

    *maxResult = 0.0;
    for(i = 0; i < 3; i++) {
        double *pssData = NULL;
        sprintf(path, "%sPSS%d.txt",root,i); //构建PSS文件地址字符串
        loadPssData(path, &psslen, &pssData);
        test(data, pssData, maxResult, maxPos, pss, base.length, psslen, i);
        free(pssData);
    }
    free(data);
}

int main() {
    double testMaxResult;
    int i, pss, maxPos;
    BASESTATION baseStation[SIZE];
    loadFile(baseStation);
    quickSort(baseStation, 0, 47);
    printf("小区编号及强度按由高到低排列如下\n");
    printf("序号\t\t小区\t\t信号强度\t\t长度\n");
    for(i = 0; i < 48; i++)
        printf("%d\t\tdata%2d\t%20lf\t\t%d\n",i, baseStation[i].index, baseStation[i].intensity, baseStation[i].length);
    printf("应选择的小区是data%d\n", baseStation[0].index);
    correlationTest(baseStation[0], &testMaxResult, &maxPos, &pss);
    printf("data%d\t最强信号序列中相关性最强的信号序列起始位置是%d，对应的检测文件为PSS%2d, 检测的结果是%15lf \n", baseStation[0].index,maxPos, pss, testMaxResult);
    return 0;
}
