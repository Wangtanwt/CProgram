#include <stdio.h>
#include<string.h>
#include<math.h>
#include<malloc.h>

typedef struct BaseStation {
    double intensity;
    int index;
    int length;
} BASESTATION;//�����ɸ�����ǿ�ȱ��������������ɵĽṹ�������������ΪBASESTATION(����վ��)

FILE *fp;//�����ļ�ָ���Ի�ȡ�ļ�����
char path[50];//�ļ�·��
BASESTATION temp[3],tempB, p;//Ϊ���ų������õı����Լ��ٵݹ��������Ŀռ�

/**
*���ܣ�����������ļ�����ȡ������ģֵ����ͺ���Ϊ�ź�ǿ�������
*�����б�
*    char *filePath:Ŀ���ļ�
*����ֵ��
*   �������ļ���õ����ź�ǿ��
*/
double getIntensity(char *filePath, int *length) {
    fp = fopen(filePath, "r");//���ļ�ָ��ָ��Ŀ���ļ�������Ϊֻ����
    *length = 0;
    double re, im, in = 0.0;//re:����ʵ�� im:�����鲿 in���ź�ǿ��
    while(!feof(fp)) {//�ж��ļ��Ƿ��ȡ��ĩβ�������������ȡ
        fscanf(fp, "%lf\n%lf\n", &re, &im);//�ֱ𽫶�������ݴ��벻ͬ�����������ļ���������Ϊʵ����ż����Ϊ�鲿������ÿ��һ�����ݾͻ��С�
        in = in+ sqrt(re*re+im*im);//�ۼ��ź�ǿ�ȣ����㹫ʽΪ������ģ��
        (*length)++;
    }
    fclose(fp);//�ر��ļ�
    return in;
}

/**
*���ܣ������ļ����Ա��������
*�����б�
*   BASESTATION *baseStation����ÿ��data�ļ�����ز�����������
*����ֵ����
*/
void loadFile(BASESTATION *baseStation) {
    int i, j, k;
    for(i = 0, k = 0; i < 61; i += 20)
        for(j = 0; j < 12; j++, k++) {
            sprintf(path, "data\\data%d.txt", i+j);//ƴ���ļ���·�����ļ���ź��ļ���չ�������֣���׼ȷ��ȡ
            baseStation[k].intensity= getIntensity(path, &baseStation[k].length);//ͨ���ļ���ȡ����վǿ��
            baseStation[k].index = i+j;//�����ļ�ԭʼ��ţ���Ϊ����Ҫ���򣬽�����ԭʼ˳��
        }
}

/**
*���ܣ����ÿ��������㷨�����ļ�����
*�����б�
*   baseStation������ṹ��ָ�룬ָ��Ԥ�����BASESTATION�ͽṹ������
*   n����������
*����ֵ: ��
*/
void quickSort(BASESTATION *base, int front, int rear) {
    if(front >= rear) return;//��������Ϸ��Լ���͵ݹ�������������ֹ���жϡ�
    int low = front, high = rear, mid = (rear-front)/2+front, i, j;
    //����һ�γ���ΪΪ��������Ч�ʶ����ġ�����ȡ�С���Ϊ��Ŧ��
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

    //��������ʼ
    while(low < high) {
        while(base[low].intensity >=  p.intensity && low < high)
            low++;
        if(low >= high) break;
        //��Ӧ�ǽ�������Ԫ�أ������ǵ�����һ������Ԫ��Ϊ��¼��p�е���Ŧ���ʲ���Ҫ�ٴμ�¼��
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
    //�Կ��ų���ֵõ������еݹ����
    quickSort(base, front, low-1);
    quickSort(base, high+1, rear);
}

/**
*���ܣ�����PSS0�ļ���������ļ�������Լ���
*�����б�
*   int index:�������ļ������
*   int length:�������ļ��ĳ���
*����ֵ��
*   int :ȷ���ź����е���ʼλ��
*/
int correlationTest(int index, int length) {
    double max = 0.0, temp = 0.0, re, im;
    int pss0len = 0, i, j, maxPos;
    double *data = (double*)malloc(length*sizeof(double));//�����ļ����ȶ�̬���������洢�ռ䣬�Դ���ļ��и���������ģ��������������㷨
    sprintf(path, "data\\data%d.txt",index);//�����ļ���ַ�ַ���
    fp = fopen(path, "r");//��ֻ����ʽ���ݵ�ַ���ļ�

    for(i = 0; !feof(fp); i++){//���㲢�洢�ļ���ÿ��������ģ
        fscanf(fp, "%lf\n%lf\n", &re, &im);//��fpָ����ļ��У�����"%lf\n%lf\n",�ĸ�ʽ��ȡ����˫���ȸ����������ֱ����re, im
        data[i] = sqrt(re*re+im*im);
    }
    fclose(fp);//�ر��ļ�

    sprintf(path, "data\\PSS0.txt");//����PSS0�ļ���ַ�ַ���
    fp = fopen(path, "r");
    while(!feof(fp)){//��ȡPSS0�ļ��и����ĸ�������ʼ�����������ֵ
        fscanf(fp, "%lf\n%lf\n", &re, &im);
        max += sqrt(re*re+im*im)*data[i];
        pss0len++;
    }
    double *pss0 = (double*)malloc(pss0len*sizeof(double));//����ǰ�߼���ĸ������������㹻�Ŀռ�
    rewind(fp);//���ļ���ȡָ�����õ��ļ���ͷ�������´��ļ��ж�ȡ����
    for(i = 0; !feof(fp); i++){//���㲢�洢PSS0�ļ���ÿ�����ֵ�ģ
        fscanf(fp, "%lf\n%lf\n", &re, &im);
        pss0[i] = sqrt(re*re+im*im);
    }
    for(j = 1; j <= length-pss0len; j++){//������⣬ͬʱѰ�����ֵ������¼ȡ�����ֵʱ�����ĳ���
        for(i = 0; i < pss0len; i++)
            temp += pss0[i]*data[i+j];
        if(temp > max){
            max = temp;
            maxPos = j;
        }
    }
        fclose(fp);//�ر�PSS0�ļ�
        free(pss0);//�ͷ�����Ŀռ�
        free(data);
        return maxPos;
}

int main() {
    BASESTATION baseStation[48];//48����վ
    loadFile(baseStation);
    quickSort(baseStation, 0, 47);

    printf("С����ż�ǿ�Ȱ��ɸߵ�����������\n");
    printf("С��\t\t�ź�ǿ��\n");
    for(int i = 0; i < 48; i++)
        printf("data%2d\t%20lf\n", baseStation[i].index, baseStation[i].intensity);
    printf("Ӧѡ���С����data%d\n", baseStation[0].index);
    printf("��ǿ�ź���������������ȷ���ź����е���ʼλ��(�����PSS0ȷ���ļ�)���ļ��е�%d������", correlationTest(baseStation[0].index, baseStation[0].length));
}
