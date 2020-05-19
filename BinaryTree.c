#include <stdlib.h>
#include <stdio.h>
#define MAXSIZE 1000

typedef char DataType;

typedef struct Node
{
    DataType data;/*数据域*/
    struct Node *leftChild;/*左子树指针*/
    struct Node *rightChild;/*右子树指针*/
} BiTreeNode,*BiTree; /*结点的结构体定义*/

/*初始化创建二叉树的头结点*/
void Initiate(BiTreeNode **root)
{
    *root = (BiTreeNode *)malloc(sizeof(BiTreeNode));
    (*root)->leftChild = NULL;
    (*root)->rightChild = NULL;
}

void Destroy(BiTreeNode **root)
{
    if((*root) != NULL && (*root)->leftChild != NULL)
        Destroy(&(*root)->leftChild);

    if((*root) != NULL && (*root)->rightChild != NULL)
        Destroy(&(*root)->rightChild);

    free(*root);
}

/*若当前结点curr非空，在curr的左子树插入元素值为x的新结点*/
/*原curr所指结点的左子树成为新插入结点的左子树*/
/*若插入成功返回新插入结点的指针，否则返回空指针*/
void CreateBiTree(BiTreeNode **T)

{
    DataType ch;
    scanf("%c",&ch);
    if(ch=='#')
        T=NULL;
    else
    {
        *T=(BiTreeNode *)malloc(sizeof(BiTreeNode));
        (*T)->data=ch;
        (*T)->leftChild = (*T)->rightChild = NULL;
        CreateBiTree(&(*T)->leftChild);
        CreateBiTree(&(*T)->rightChild);
    }
}

typedef struct
{
    BiTreeNode *data[MAXSIZE];
    int top;//栈顶指针
} SqStack;

void Initstack(SqStack *S)
{
    S->top=-1;
}
int Push(SqStack *S,BiTreeNode *e)
{
    if(S->top == MAXSIZE-1) return 0;
    S->data[++S->top]=e;
    return 1;
}

int Pop(SqStack *S, BiTreeNode **p)
{
    if(S->top == -1) return 0;
    *p = S->data[S->top--];
    return 1;
}

void preorder(BiTreeNode *t)
{
    BiTreeNode *P = t;
    SqStack S;
    Initstack(&S);
    while(P||S.top!=-1)
    {
        if(P)
        {
            Push(&S,P);
            putchar(P->data);
            P = P->leftChild;
        }
        else
        {
            Pop(&S, &P);
            P=P->rightChild;
        }
    }
}

int main()

{
    BiTreeNode *T;
    Initiate(&T);
    printf("请输入二叉树:\n");
    CreateBiTree(&T);
    printf("非递归前序遍历:\n");
    preorder(T);
    Destroy(&T);
    return 0;
}
