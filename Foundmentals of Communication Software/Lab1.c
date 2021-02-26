#include <stdio.h>
#include<string.h>
#include<math.h>
#include<malloc.h>
#define SIZE 48

typedef struct BaseStation {
    double intensity;//基站信号强度
    int index;//基站初始序号
    int length;//基站文件数据的个数
} BASESTATION;//基站

FILE *fp;//文件指针
char root[50] = "data\\";//基站文件根目录相对路径，需要根据不同文件路径而更改
char path[100];//单个基站文件的路径

/*---------------------------------------------------------------------- 获取基站信号强度模块-----------------------------------------------------------------------------*/

/**
 *函数名：getIntensity
 *功能：根据输入的文件地址，读取、计算模值、求和后作为信号强度输出。
 *参数列表：
 *      char filePath[]：目标文件地址字符串
 *      int *length：指向记录目标文件复数个数的变量的指针，记录每个文件的复数个数，以便后边进行相关性检测
 *返回值：
 *      double：按照信号强度计算公式(所有复数模的和)得到的基站的信号强度
 */
double getIntensity(char filePath[], int *length) {
    fp = fopen(filePath, "r");//打开文件
    *length = 0;//初始化长度变量的值
    double re, im, intensity = 0.0;//单个复数的实部、虚部，整个文件内所有复数模的总和
    while(!feof(fp)) {//读取文件中的所有复数，同时取模求和
        fscanf(fp, "%lf\n%lf\n", &re, &im);//自信号文件读取复数的实部和虚部
        intensity += sqrt(re*re+im*im);
        (*length) += 2;//每次读取两个数，复数的实部和虚部
    }
    fclose(fp);//关闭文件
    return intensity;//返回基站信号强度计算结果
}

/**
 *函数名：loadFile
 *功能：分别计算所有文件的信号强度、复数个数，并存放于基站结构体数组中便于后续操作
 *参数列表：
 *      BASESTATION baseStation[]：将每个data文件的相关参数存于其中
 *返回值：无
 */
void loadFile(BASESTATION baseStation[]) {
    int i, j, k;//信号文件的通项公式：data(i*20+j) i = 0, 1, 2, 3;  j = 0, 1, 2... , 11。k为基站数组索引
    for(i = 0, k = 0; i <4; i ++)
        for(j = 0; j < 12; j++, k++) {
            sprintf(path, "%sdata%d.txt", root, i*20+j);//拼接文件路径
            baseStation[k].intensity= getIntensity(path, &baseStation[k].length);
            baseStation[k].index = i*20+j;//由于后边要排序，将打乱原始顺序，故保存文件原始序号
        }
}

/*----------------------------------------------------------------------  快速排序模块  -----------------------------------------------------------------------------------------*/

/**
 *函数名：copyBase
 *功能：将一个基站变量的值复制到另一个内
 *参数列表：
 *      BASESTATION *base1：指向副本的指针
 *      BASESTATION *base2：指向原本的指针
 *返回值：无
 */
void copyBase(BASESTATION *base1, BASESTATION *base2) {
    base1->index = base2->index;
    base1->intensity = base2->intensity;
    base1->length = base2->length;
}

/**
 *函数名：swapBase
 *功能：交换两个基站变量的值
 *参数列表：
 *      BASESTATION *base1：指向基站变量1的指针
 *      BASESTATION *base2：指向基站变量2的指针
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
 *功能：以信号强度为主关键字，将基站数组首部、尾部、中间，三个基站变量中，信号强度为次大值的基站变量作为快排枢纽，并将此次大值放在尾部
 *参数列表：
 *      BASESTATION base[]：待排序的数组
 *      BASESTATION *p：指向枢纽的指针
 *      int low：待排序序列首索引
 *      int mid：待排序序列中间索引
 *      int high：待排序序列尾索引
 *返回值：无
 */
void findPivot(BASESTATION base[], BASESTATION *p, int low, int mid, int high) {
    int i, j;
    BASESTATION temp[3] ;//借助辅助数组对其排序。
    copyBase(&temp[0], &base[low]);
    copyBase(&temp[1], &base[mid]);
    copyBase(&temp[2], &base[high]);

    //辅助数组简单降序排序
    for(i = 0; i < 3; i++)
        for(j = i+1; j < 3; j++)
            if(temp[i].intensity < temp[j].intensity)
                swapBase(&temp[i], &temp[j]);

    copyBase(&base[mid], &temp[0]);//最大值
    copyBase(&base[high], &temp[1]);//次大值
    copyBase(&base[low], &temp[2]);//最小值
    copyBase(p, &temp[1]);
}

/**
 *函数名：quickSort
 *功能：利用快速排序算法，以信号强度为主关键字为数组排序；
 *参数列表：
 *      base[]：待排序数组
 *      front：数组首索引
 *      rear：数组尾索引
 *返回值: 无
 */
void quickSort(BASESTATION base[], int front, int rear) {
    if(front >= rear) return;
    BASESTATION p;//快排枢纽
    int low = front, high = rear, mid = (rear+front)/2;
    findPivot(base, &p, low, mid, high);

    while(low < high) {//从低位开始快排
        while(low < high && base[low].intensity >=  p.intensity)
            low++;
        copyBase(&base[high], &base[low]);
        while(low < high && base[high].intensity <= p.intensity)
            high--;
        if(low >= high) break;
        copyBase(&base [low], &base[high]);
    }
    copyBase(&base[high], &p);
    quickSort(&base[0], front, low);
    quickSort(&base[0], high+1, rear);
}

/*-------------------------------------------------------------------------- 相关性检测模块 --------------------------------------------------------------------------------*/

/**
 *函数名：loadDataFile
 *功能：读入基站信号文件内所有数据，并保存在数组中
 *参数列表：
 *      int index： 基站序号
 *      double data[]：接收导入的数据的双精度浮点型数组
 *返回值: 无
 */
void loadDataFile(int index, double data[]) {
    int i;
    sprintf(path, "%sdata%d.txt",root, index);//构造文件地址字符串
    fp = fopen(path, "r");
    for(i = 0;!feof(fp); i++) //存储文件中每个数字
        fscanf(fp, "%lf\n", &data[i]);
    fclose(fp);
}

/**
 *函数名：loadPssData
 *功能：导入主同步信号文件PSSx文件中的数据
 *参数列表：
 *      char pssPath[]：文件地址
 *      int *psslen：文件中数据个数
 *      double **pssdata：指向接收数据文件数组的指针
 *返回值: 无
 */
void loadPssData(char pssPath[], int *psslen, double **pssData) {
    int i = 0;
    double temp;
    fp = fopen(pssPath, "r");
    for(*psslen = 0; !feof(fp); (*psslen )++) //获取PSS文件中数据的个数
        fscanf(fp, "%lf\n", &temp);

    *pssData = (double*)calloc((*psslen), sizeof(double));//按照数据个数申请足够的空间
    rewind(fp);//文件指针返回到文件开头
    for(i = 0; !feof(fp); i++) //存储PSS文件中每个复数的实部和虚部
        fscanf(fp, "%lf\n", *pssData + i);
    fclose(fp);
}

/**
 *函数名：test
 *功能：相关性检验
 *参数列表：
 *      double data[]： 需要检测的信号数据组
 *      double pssData[]：检测所使用的主同步信号数组
 *      double *maxResult：指向最大检测结果的指针
 *      int *maxPos：指向最大检测结果对应的信号序列起始位的指针
 *      int *pss：指向最大结果所对应pss文件的指针
 *      int datalen：信号序列的长度
 *      int psslen：pss序列的长度
 *      int prePss：本次检测所使用的的pss文件
 *返回值: 无
 */
void test(double data[], double pssData[], double *maxResult, int *maxPos, int *pss, int datalen, int psslen, int prePss) {
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
            *pss = prePss;//保存所使用的Pss文件
        }
    }
}

/**
 *函数名：correlationTest
 *功能：相关性检验
 *参数列表：
 *      BASESTATION base:需要检测的基站
 *      double *maxResult：指向最大检测结果变量的指针
 *      int *maxPos：指向最大检测结果对应的信号序列起始位的指针
 *      int *pss：指向最大检测结果所对应pss文件序号的指针
 *返回值: 无
 */
void correlationTest(BASESTATION base, double *maxResult, int *maxPos,double *pss) {
    int psslen, i;//psslen : pss文件的长度
    double *data = (double*)calloc(base.length,sizeof(double));//根据文件长度创建动态数组，以存放文件中各个复数的实部和虚部，方便后续检验
    double *pssData;
    loadDataFile(base.index, data);
    *maxResult = 0.0;
    for(i = 0; i < 3; i++) {
        pssData = NULL;
        sprintf(path, "%sPSS%d.txt",root,i); //构建PSS文件地址字符串
        loadPssData(path, &psslen, &pssData);
        test(data, pssData, maxResult, maxPos, pss, base.length, psslen, i);
        free(pssData);//释放构建动态数组时申请的内存。
    }
    free(data);
}

int main() {
    int i, pss, maxPos;
    double testMaxResult;
    BASESTATION baseStation[SIZE];

    loadFile(baseStation);
    quickSort(baseStation, 0, SIZE-1);
    //屏幕输出结果。由于在我的CodeBlocks上，无法输出汉字，因此采用英文输出
    printf("Index\t\tDistrict\tIntensity\t\tLength\n");
    for(i = 0; i < SIZE; i++)
        printf("%d\t\tdata%2d\t%20lf\t\t%d\n",i + 1 , baseStation[i].index, baseStation[i].intensity, baseStation[i].length);
    correlationTest(baseStation[0], &testMaxResult, &maxPos, &pss);
    printf("The intensity of data%d is the strongest, whose  sequence begins from %d. Its pss file is PSS%d, the result of correlation test is %10lf .\n", baseStation[0].index,maxPos, pss, testMaxResult);
    return 0;
}
