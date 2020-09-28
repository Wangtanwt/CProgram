#include <stdio.h>
#include<string.h>
#include<math.h>
#include<malloc.h>

typedef struct BaseStation {
    double intensity;
    int index;
    int length;
} BASESTATION;//定义由浮点型强度变量和整形序号组成的结构体变量，并命名为BASESTATION(“基站”)

FILE *fp;//定义文件指针以获取文件数据
char path[50];//文件路径
BASESTATION temp[3],tempB, p;//为快排程序设置的变量以减少递归过程申请的空间

/**
*功能：根据输入的文件，读取、计算模值、求和后作为信号强度输出。
*参数列表：
*    char *filePath:目标文件
*返回值：
*   经处理文件后得到的信号强度
*/
double getIntensity(char *filePath, int *length) {
    fp = fopen(filePath, "r");//将文件指针指向目标文件，并设为只读型
    *length = 0;
    double re, im, in = 0.0;//re:复数实部 im:复数虚部 in：信号强度
    while(!feof(fp)) {//判断文件是否读取到末尾，若无则继续读取
        fscanf(fp, "%lf\n%lf\n", &re, &im);//分别将读入的数据存入不同变量，由于文件中奇数行为实部、偶数行为虚部，所以每读一个数据就换行。
        in = in+ sqrt(re*re+im*im);//累计信号强度，计算公式为复数的模。
        (*length)++;
    }
    fclose(fp);//关闭文件
    return in;
}

/**
*功能：导入文件，以便后续操作
*参数列表：
*   BASESTATION *baseStation：将每个data文件的相关参数存于其中
*返回值：无
*/
void loadFile(BASESTATION *baseStation) {
    int i, j, k;
    for(i = 0, k = 0; i < 61; i += 20)
        for(j = 0; j < 12; j++, k++) {
            sprintf(path, "data\\data%d.txt", i+j);//拼接文件根路径、文件序号和文件扩展名三部分，以准确获取
            baseStation[k].intensity= getIntensity(path, &baseStation[k].length);//通过文件获取，基站强度
            baseStation[k].index = i+j;//保存文件原始序号，因为后期要排序，将打乱原始顺序
        }
}

/**
*功能：利用快速排序算法，对文件排序；
*参数列表：
*   baseStation：传入结构体指针，指向预处理的BASESTATION型结构体数组
*   n：数组容量
*返回值: 无
*/
void quickSort(BASESTATION *base, int front, int rear) {
    if(front >= rear) return;//输入参数合法性检验和递归排序子排序终止的判断。
    int low = front, high = rear, mid = (rear-front)/2+front, i, j;
    //以下一段程序为为提升程序效率而做的”三者取中“作为枢纽；
    temp[0].index = base[mid].index;
    temp[1].index = base[low].index;
    temp[2].index = base[high].index;
    temp[0].intensity = base[mid].intensity;
    temp[1].intensity = base[low].intensity;
    temp[2].intensity = base[high].intensity;
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
    p.intensity = base[high].intensity = temp[1].intensity;
    base[low].intensity = temp[2].intensity;
    base[mid].index = temp[0].index;
    p.index = base[high].index = temp[1].index;
    base[low].index = temp[2].index;

    //快速排序开始
    while(low < high) {
        while(base[low].intensity >=  p.intensity && low < high)
            low++;
        if(low >= high) break;
        //本应是交换数组元素，但考虑到其中一个交换元素为记录在p中的枢纽，故不需要再次记录。
        base[high].intensity = base[low].intensity;
        base[high--].index = base[low].index;

        while(base[high].intensity <= p.intensity && high > low)
            high--;
        if(low >= high) break;
        base[low].intensity = base[high].intensity;
        base[low++].index = base[high].index;
    }
    base[high].index = p.index;
    base[high].intensity = p.intensity;
    //对快排程序分得的子序列递归快排
    quickSort(base, front, low-1);
    quickSort(base, high+1, rear);
}

/**
*功能：根据PSS0文件对输入的文件做相关性检验
*参数列表：
*   int index:待处理文件的序号
*   int length:待处理文件的长度
*返回值：
*   int :确定信号序列的起始位置
*/
int correlationTest(int index, int length) {
    double max = 0.0, temp = 0.0, re, im;
    int pss0len = 0, i, j, maxPos;
    double *data = (double*)malloc(length*sizeof(double));//根据文件长度动态申请连续存储空间，以存放文件中各个复数的模，方便后续检验算法
    sprintf(path, "data\\data%d.txt",index);//构造文件地址字符串
    fp = fopen(path, "r");//以只读方式根据地址打开文件

    for(i = 0; !feof(fp); i++){//计算并存储文件中每个复数的模
        fscanf(fp, "%lf\n%lf\n", &re, &im);//从fp指向的文件中，按照"%lf\n%lf\n",的格式读取两个双精度浮点数，并分别存于re, im
        data[i] = sqrt(re*re+im*im);
    }
    fclose(fp);//关闭文件

    sprintf(path, "data\\PSS0.txt");//构建PSS0文件地址字符串
    fp = fopen(path, "r");
    while(!feof(fp)){//获取PSS0文件中复数的个数，初始化检验结果最大值
        fscanf(fp, "%lf\n%lf\n", &re, &im);
        max += sqrt(re*re+im*im)*data[i];
        pss0len++;
    }
    double *pss0 = (double*)malloc(pss0len*sizeof(double));//按照前边计算的复数个数申请足够的空间
    rewind(fp);//将文件读取指针重置到文件开头，以重新从文件中读取数据
    for(i = 0; !feof(fp); i++){//计算并存储PSS0文件中每个数字的模
        fscanf(fp, "%lf\n%lf\n", &re, &im);
        pss0[i] = sqrt(re*re+im*im);
    }
    for(j = 1; j <= length-pss0len; j++){//滑动检测，同时寻找最大值，并记录取得最大值时滑动的长度
        for(i = 0; i < pss0len; i++)
            temp += pss0[i]*data[i+j];
        if(temp > max){
            max = temp;
            maxPos = j;
        }
    }
        fclose(fp);//关闭PSS0文件
        free(pss0);//释放申请的空间
        free(data);
        return maxPos;
}

int main() {
    BASESTATION baseStation[48];//48个基站
    loadFile(baseStation);
    quickSort(baseStation, 0, 47);

    printf("小区编号及强度按由高到低排列如下\n");
    printf("小区\t\t信号强度\n");
    for(int i = 0; i < 48; i++)
        printf("data%2d\t%20lf\n", baseStation[i].index, baseStation[i].intensity);
    printf("应选择的小区是data%d\n", baseStation[0].index);
    printf("最强信号序列中所包含的确定信号序列的起始位置(相对于PSS0确定文件)是文件中第%d个复数", correlationTest(baseStation[0].index, baseStation[0].length));
}
