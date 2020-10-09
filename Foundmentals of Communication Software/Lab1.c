#include <stdio.h>
#include<string.h>
#include<math.h>
#include<malloc.h>
#define SIZE 48//共有48个基站

typedef struct BaseStation {
    double intensity;
    int index;
    int length;
} BASESTATION;

FILE *fp;
char root[50] = "data\\";//待处理文件根目录路径(相对路径或绝对路径都可，这里是相对路径，即此C文件所在文件夹中的名为data的文件夹内)
char path[50];//存放单个具体文件的路径

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
    fp = fopen(filePath, "r");//以只读格式打开目标文件
    *length = 0;
    double real, imaginary, intensity = 0.0;//real：复数实部   imaginary：复数虚部   intensity：信号强度
    while(!feof(fp)) {//判断文件是否读取到末尾，若无则继续读取
        fscanf(fp, "%lf\n%lf\n", &real, &imaginary);//分别将读入的数据存入不同变量，由于文件中奇数行为实部、偶数行为虚部，所以每次读两行。
        intensity += sqrt(real*real+imaginary*imaginary);//累计信号强度，计算公式为复数模的计算公式。
        (*length)++;
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
    for(i = 0, k = 0; i < 61; i += 20)
        for(j = 0; j < 12; j++, k++) {
            sprintf(path, "%sdata%d.txt", root, i+j);//拼接文件根路径、文件序号和文件扩展名三部分，以准确获取
            baseStation[k].intensity= getIntensity(path, &baseStation[k].length);//通过文件获取，基站强度
            baseStation[k].index = i+j;//由于后边要排序，将打乱原始顺序，故保存文件原始序号
        }
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
void findPivot(BASESTATION *base, BASESTATION *p, int low, int mid, int high) {
    int i, j;
    BASESTATION temp[3] = {{base[mid].intensity, base[mid].index, 0}, {base[low].intensity, base[low].index, 0}, {base[high].intensity, base[high].index, 0}};//借助辅助数组对其排序。
    BASESTATION tempB;

    for(i = 0; i < 3; i++)
        for(j = i+1; j < 3; j++)
            if(temp[i].intensity < temp[j].intensity) {
                tempB.intensity = temp[i].intensity;
                temp[i].intensity = temp[j].intensity;
                temp[j].intensity = tempB.intensity;

                tempB.index = temp[i].index;
                temp[i].index = temp[j].index;
                temp[j].index = tempB.index;
            }

    base[mid].intensity = temp[0].intensity;
    p->intensity = base[high].intensity = temp[1].intensity;
    base[low].intensity = temp[2].intensity;

    base[mid].index = temp[0].index;
    p->index = base[high].index = temp[1].index;
    base[low].index = temp[2].index;
}

/**
 *函数名：quickSort
 *功能：利用快速排序算法，以信号强度为主关键字为数组排序；
 *参数列表：
 *  base：待排序数组
 *  front：数组首索引
 *  rear：数组尾索引
 *返回值: 无
 */
void quickSort(BASESTATION *base, int front, int rear) {
    if(front >= rear) return;//排序终止判断。
    BASESTATION p;//快排枢纽
    int low = front, high = rear, mid = (rear-front)/2+front, i, j;
    findPivot(base, &p, low, mid, high);

    while(low < high) {
        while(base[low].intensity >=  p.intensity && low < high)
            low++;
        if(low >= high) break;
        //交换数组元素，考虑到其中一个交换元素为记录在p中的枢纽，故不需要每次都赋值。
        base[high].intensity = base[low].intensity;
        base[high--].index = base[low].index;

        while(base[high].intensity <= p.intensity && high > low)
            high--;
        if(low >= high) break;
        base[low].intensity = base[high].intensity;
        base[low++].index = base[high].index;
    }
    //因排序过程中未为枢纽元素赋值，故枢纽在数组中被覆盖，但它存在另一变量p中，所以通过赋值还原
    base[high].index = p.index;
    base[high].intensity = p.intensity;
    //对快排程序分得的子序列递归快排
    quickSort(base, front, low-1);
    quickSort(base, high+1, rear);
}

/**
 *函数名：correlationTest
 *功能：对指定的文件做相关性检验
 *参数列表：
 *   int index:待处理文件的序号
 *   int length:待处理文件中复数的个数
 *返回值：
 *   int :确定信号序列的起始位置
 */
int correlationTest(int index, int length) {
    int psslen, maxPos, i, j, k;
    double max = 0.0, temp, real, imaginary;

    double *data = (double*)malloc(length*sizeof(double));//根据文件长度动态申请连续存储空间，以存放文件中各个复数的模，方便后续检验算法
    sprintf(path, "%sdata%d.txt",root, index);//构造文件地址字符串
    fp = fopen(path, "r");//以只读方式根据地址打开文件
    for(i = 0; !feof(fp); i++) { //计算并存储文件中每个复数的模
        fscanf(fp, "%lf\n%lf\n", &real, &imaginary);//从fp指向的文件中，按照"%lf\n%lf\n",的格式读取两个双精度浮点数，并分别存于re, im
        data[i] = sqrt(real*real+imaginary*imaginary);
    }
    fclose(fp);//关闭文件

    for(k = 0; k < 3; k++, psslen = 0) {
        sprintf(path, "data\\PSS%d.txt", k);//构建PSS文件地址字符串
        fp = fopen(path, "r");
        for(i = 0, temp = 0.0; !feof(fp); i++, psslen++) { //获取PSS文件中复数的个数
            fscanf(fp, "%lf\n%lf\n", &real, &imaginary);
            temp += sqrt(real*real+imaginary*imaginary)*data[i];
        }
        rewind(fp);//将文件读取指针重置到文件开头，以便后续重新从文件中读取数据

        if(max == 0.0) //初始化检验结果最大值
            max = temp;

        double *pss = (double*)malloc(psslen*sizeof(double));//按照复数个数申请足够的空间
        for(i = 0; !feof(fp); i++) { //计算并存储PSS文件中每个复数的模
            fscanf(fp, "%lf\n%lf\n", &real, &imaginary);
            pss[i] = sqrt(real*real+imaginary*imaginary);
        }

        for(j = 1; j <= length-psslen; j++) { //滑动检测，同时寻找最大值，并记录取得最大值时滑动的长度
            for(i = 0, temp = 0.0; i < psslen; i++)
                temp += pss[i]*data[i+j];
            if(temp > max) {
                max = temp;
                maxPos = j+1;
            }
        }

        fclose(fp);
        free(pss);//释放申请的空间
    }

    free(data);
    return maxPos;
}

int main() {
    int i;
    BASESTATION baseStation[SIZE];
    loadFile(baseStation);
    quickSort(baseStation, 0, SIZE -1);
    
    printf("小区编号及强度按由高到低排列如下\n");
    printf("小区\t\t信号强度\n");
    for(i = 0; i < 48; i++)
        printf("data%2d\t%20lf\n", baseStation[i].index, baseStation[i].intensity);
    printf("应选择的小区是data%d\n", baseStation[0].index);
    printf("最强信号序列中所包含的确定信号序列的起始位置是文件中第%d个复数", correlationTest(baseStation[0].index, baseStation[0].length));
    return 0;
}
