#include<stdio.h>

#define SIZE 50//ջ���������

int getIndex(char ch)
/*Ϊ���������������getPriority������ȡ��������ȼ��Ĵ�С��ϵ*/
{
    switch(ch)
    {
    case '-':
        return 1;
    case '*':
        return 2;
    case '/':
        return 3;
    case '(':
        return 4;
    case ')':
        return 5;
    case '#':
        return 6;
    default:
        return 0;
    }
}

char getPriority(char opter1, char opter2)
/*�Ƚ��������������ȼ�����ȷ������˳��*/
{
    char priority[7][7] =
    {
        {'>', '>', '<', '<', '<', '>', '>'},
        {'>', '>', '<', '<', '<', '>', '>'},
        {'>', '>', '>', '>', '<', '>', '>'},
        {'>', '>', '>', '>', '<', '>', '>'},
        {'<', '<', '<', '<', '<', '=', '0'},
        {'>', '>', '>', '>', '0', '=', '>'},//��������һЩ�Ķ�
        {'<', '<', '<', '<', '<', '0', '='}
    } ;
    return priority[getIndex(opter1)][getIndex(opter2)];
}

int calculate(int opend1, int opend2, char opter)
//�����ַ�����������Բ���������Ӧ����
{
    switch(opter)
    {
    case '+':
        return opend1+opend2;
    case '-':
        return opend1-opend2;
    case '*':
        return opend1*opend2;
    case '/':
        return opend1/opend2;
    }
}

void push(int k, char *opter, char c)
{
    int m = 1, n = 0, d = 0;
    //���й�������ʱ����Ҫͨ���任�����Խ�����ŵ�Ӱ��
    if(c != '(' && c != ')' && c != '#')
    {
        for(; m < k; m++)
            switch(m)
            {
            case '-':
                n++;
                break;
            case '/':
                d++;
                break;
            case '(':
                n = d = 0;
                break;
            }//�����ڵķ��Ų���Ӱ��
        if(n%2)//��������-��
        {
            if(c == '+') c = '-';
            else if(c == '-') c = '+';
        }
        if(d%2)//��������/��
        {
            if(c == '*') c = '/';
            else if(c == '/') c = '*';
        }
    }
    opter[k] = c;
}

void calculator()
/*����һλ��֮��Ļ�����㣬������+��-��*��/��������6�����������#Ϊ���������*/
{
    int i = -1, j = 0, opnd[SIZE] = {0};
    char c, opter[SIZE] = {NULL};
    //��#Ϊ�����ջ��ջ�ף�����ʽ������#��Ӧ
    opter[0] = '#';
    while(opter[0] != NULL)
        //������ֹ�����ǲ�����ջΪ��
    {
        //������������Ϊ�����˼�룬�������Ž���ʱ�������Ϊһ�������ټ�����������
        if(c != '#' && c!=')')
        {
            c = getchar();
            if(c != '#') printf("%c", c);
        }
        if(c>='0' && c <= '9')
            opnd[++i] = c - '0';
        else
            switch(getPriority(opter[j], c))
            {
            case '<':
                push(++j, opter, c);
                break;
            case '=':
                if(opter[j] == '(') c = '(';
                opter[j--] = NULL;
                break;
            case '>':
                opnd[--i] = calculate(opnd[i-1], opnd[i], opter[j]);
                push(j, opter, c);
                break;
            }
    }
    if(i)
        printf("\nERROR!");
    else
        printf("=%d", opnd[0]);
}

main()
{
    calculator();
}
