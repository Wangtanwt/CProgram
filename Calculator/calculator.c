#include<stdio.h>

#define SIZE 50//栈的最大容量

int getIndex(char ch)
/*为运算符编序号以配合getPriority函数获取运算符优先级的大小关系*/
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
/*比较两操作符的优先级，以确定运算顺序*/
{
    char priority[7][7] =
    {
        {'>', '>', '<', '<', '<', '>', '>'},
        {'>', '>', '<', '<', '<', '>', '>'},
        {'>', '>', '>', '>', '<', '>', '>'},
        {'>', '>', '>', '>', '<', '>', '>'},
        {'<', '<', '<', '<', '<', '=', '0'},
        {'>', '>', '>', '>', '0', '=', '>'},//这里做出一些改动
        {'<', '<', '<', '<', '<', '0', '='}
    } ;
    return priority[getIndex(opter1)][getIndex(opter2)];
}

int calculate(int opend1, int opend2, char opter)
//根据字符型运算符，对操作数做相应计算
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

void pushOpter(int k, char* opter, char c)
{
    int m = 1, n = 0, d = 0;
    //当有过多括号时，需要通过变换符号以解决括号的影响
    if(c != '(' && c != ')' && c != '#')
    {
        for(; m < k; m++)
            switch(opter[m])
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
            }//括号内的符号不受影响
        if(n%2)//奇数个‘-’
        {
            if(c == '+') c = '-';
            else if(c == '-') c = '+';
        }
        if(d%2)//奇数个‘/’
        {
            if(c == '*') c = '/';
            else if(c == '/') c = '*';
        }
    }
    opter[k] = c;
}

void calculator()
/*计算一位数之间的混合运算，仅限于+、-、*、/、（，）6种运算符，以#为输入结束符*/
{
    int i = -1, j = 0, opnd[SIZE] = {0};
    char c, opter[SIZE] = {NULL};
    //以#为运算符栈的栈底，与表达式结束符#对应
    opter[0] = '#';
    while(opter[0] != NULL)
        //计算终止条件是操作符栈为空
    {
        //按照括号内视为整体的思想，在两括号结束时将其计算为一个数后，再继续后续运算
        if(c != '#' && c != ')')
        {
            c = getchar();
            if(c != '#') printf("%c", c);
        }
        if(c>='0' && c <= '9')
        {
            i++;
            opnd[i] = c - '0';
        }
        else
            switch(getPriority(opter[j], c))
            {
            case '<':
                j++;
                pushOpter(j, opter, c);
                break;
            case '=':
                if(opter[j] == '(') c = '(';
                opter[j] = NULL;
                j--;
                break;
            case '>':
                i--;
                opnd[i] = calculate(opnd[i], opnd[i+1], opter[j]);
                pushOpter(j, opter, c);
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
