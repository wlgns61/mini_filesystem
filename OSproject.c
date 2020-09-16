#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>

/*
��屸��: id, name, permission(�⺻�� ����Ұ�), time, ��ũ 4����(lsibling,rslbling,lchild,parent)ls


���� �⺻���� root->home->user ����, ������ġ�� user

���:
1. mkdir
 -> mkdir  ��� �� �̸� : ���丮 ����
    mkdir -p �̸�/�̸�/�̸� : �� ��ġ���� ���丮�� �����ؼ� ����

2. cd
 ->  cd .  : ������ġ�״��
     cd ..  :  ���� ���丮�� �̵�
     cd ~ : user�� �̵�
     cd /  : root�� �̵�
     cd    : user�� �̵�
     cd    :��� �� �̸�

3. pwd
->  pwd : root���� �ڽ��� ��θ� ������

4. cat
->  cat > ��� �� �̸�: �ؽ�Ʈ���� ���� (:q �Ǵ� :wq �Է½� ����)
    cat ��� �� �̸�    : �ý�Ʈ ���� ����

5. rm
->  rm ��� �� �̸� : ���� ����
    rm -rf �̸�        : ���丮 �Ǵ� ������ ���� ��� ���� ���� ����

6. ls
->  ls    : �� ���丮�� ���� ��ϵ��� ������
    ls -l or -al: �� �� �ڼ��� ������ ������(����, ��� ���� �ð�, �����, �̸�)

7.   rmdir ��� �� ����: ���丮 ����
     rmdir -p ��� : �Է��� ����� ���丮�� ������ ����

����:
save�Լ��� ���� ������ preorder������ ���̳ʸ� ���Ϸ� ����(��ɾ� ����� ��� ����)
load�Լ��� ���� ���̳ʸ� ���Ͽ� preorder������ ����� ������ ����� ��ũ�� ���� ������ �̿��� �ٽ� �ҷ���
*/

typedef struct TreeNode *treeptr;
typedef struct TreeNode *pathptr;
typedef struct TreeNode{
    char id;
    char name[10];
    char permission[12];
    int timerecord[4];
    treeptr parent, lchild, rsibling, lsibling;
}TreeNode;

struct tm *moment;

treeptr root, home, user, current_node;
int fail=0; //������ �˻��ϱ� ���� ����
int cat=0; //cat ���� �� 0�̸�
int rm=0;
sem_t semp;

void MakeDirectory(char aname[], char newid)
{//���丮 �� ���� ����
    treeptr newnode, temp;
    newnode = (treeptr)malloc(sizeof (TreeNode));
    newnode->id = newid;
    strcpy(newnode->name, aname);
    newnode->lchild = newnode->rsibling = newnode->parent=newnode->lsibling=NULL;
    time_t timer;
    timer=time(NULL);
    moment=localtime(&timer);
    int i=0;
    for(i=0;i<sizeof(aname)/sizeof(char);i++)
    {
        if(aname[i]=='/')
        {
            mkdir_path(aname,newid);
            return;
        }
    }
    if (current_node->lchild == NULL)
    {//���丮 ����
        if(newnode->id=='f'&&cat==1)
        {
            printf("cat: %s: There are no such files or directories.\n",newnode->name);
            free(newnode);
            cat=0;
            return;
        }
        newnode->timerecord[0]=moment->tm_mon+1;
        newnode->timerecord[1]=moment->tm_mday;
        newnode->timerecord[2]=moment->tm_hour;
        newnode->timerecord[3]=moment->tm_min;
        if(newnode->id=='d')
            strcpy(newnode->permission,"rwxr-xr-x");
        if(newnode->id=='f')
            strcpy(newnode->permission,"rw-r--r--");
        current_node->lchild = newnode;
        newnode->parent=current_node;
        if(newnode->id=='f'&&cat==0)
            MakeFile(newnode->name);
    }
    else
    {
        temp = current_node->lchild;
        while (temp->rsibling != NULL)
        {
            if(strcmp(aname,temp->name)==0)
                break;
            temp = temp->rsibling;
        }
        if(strcmp(aname,temp->name)==0) //�̸��� ��ġ �Ҷ�
        {
            if(temp->id=='f'&&cat==1)
            {
                read_file(temp->name);
                return;
            }
            if(temp->id=='d'||cat==2)
            {
                printf("Can't make directory: %s already exist\n",temp->name);
                fail=1;
                free(newnode);
            }
            if(temp->id=='f'&&cat==0)
            {
                MakeFile(temp->name);
                return;
            }
        }
        else
        {//���丮 ����
            if(newnode->id=='f'&&cat==1)
            {
                printf("cat: %s: There are no such files or directories.\n",newnode->name);
                free(newnode);
                return;
            }
            newnode->timerecord[0]=moment->tm_mon+1;
            newnode->timerecord[1]=moment->tm_mday;
            newnode->timerecord[2]=moment->tm_hour;
            newnode->timerecord[3]=moment->tm_min;
            strcpy(newnode->permission,"rwxr-xr-x");
            temp->rsibling = newnode;
            newnode->lsibling=temp;
            if(newnode->id=='f'&&cat==0)
                MakeFile(newnode->name);
        }
    }
}

void MakeFile(char name[])
{
    FILE *fp;
    char filename[50];
    int line=1;
    strcpy(filename,name);
    char buffer[200]="\0";
    char *buffer2="\0";
    strcat(filename,".txt");
    fp=fopen(filename,"wt");
   // printf("':q or :wq' �Է� �� ����\n");
    if (fp == NULL)
    {
        printf(" File Error \n");
        return;
    }
    while(strcmp(buffer,":wq")!=0||strcmp(buffer,":q")!=0)
    {
        gets(buffer);
        if(strcmp(buffer,":wq")==0||strcmp(buffer,":q")==0)
            break;
        fprintf(fp,"%s\n", buffer);
        line++;
    }
    fclose(fp);
}

void read_file(char name[])
{
    FILE *fp;
    char filename[50];
    int line=1;
    strcpy(filename,name);
    char buffer[200]="\0";
    strcat(filename,".txt");
    fp=fopen(filename,"r");
    if (fp == NULL)
    {
        printf(" File Error \n");
        return;
    }
    while (fgets(buffer, sizeof(buffer), fp) != NULL )
    {
        printf("%s", buffer);
    }

}

void mkdir_path(char path[],char newid)
{
    char *p_ch;
    char delimeter[2]="/";
    char dname[40]="\0";
    treeptr t,temp;
    pathptr newnode, head;
    temp=current_node;

    p_ch=strtok(path, delimeter);
    newnode = (pathptr)malloc(sizeof (TreeNode));
    strcpy(newnode->name,p_ch);
    newnode->rsibling=NULL;
    head=newnode;
    t=head;
    if(path[0]=='/')
    {
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,root->name);
        newnode->rsibling=t;
        head=newnode;
        t=head;
        t=t->rsibling;
    }
    while(p_ch)
    {
        p_ch=strtok(NULL, delimeter);
        if(p_ch==NULL)
            break;
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,p_ch);
        newnode->rsibling=NULL;
        t->rsibling=newnode;
        t=t->rsibling;
    }
    t=head;
    while(t->rsibling!=NULL)
    {
        if(fail==1)
        {
            current_node=temp;
            return;
        }
        change_directory(t->name,"");
        t=t->rsibling;
        if(t->rsibling==NULL)
            break;
    }
    MakeDirectory(t->name,newid);
    current_node=temp;
}

void mkdir_p_option(char path[])
{
    char *p_ch;
    char delimeter[2]="/";
    char dname[30]="\0";
    if(path[0]=='/')
    {
        printf("Can't make directory: Deny permission.\n");
        return;
    }
    treeptr temp=current_node;
    p_ch=strtok(path, delimeter);

    strcpy(dname,p_ch);
    MakeDirectory(dname,'d');
    change_directory(dname);
    while(p_ch)
    {
        p_ch=strtok(NULL, delimeter);
        if(p_ch==NULL||fail==1)
        {
            current_node=temp;
            break;
        }
        strcpy(dname,p_ch);
        MakeDirectory(dname,'d');
        change_directory(dname);
    }


}

void free_d(treeptr t)
{
    char name[20];
    if(t!=NULL)
    {
        strcpy(name,t->name);
        strcat(name,".txt");
        if(t->id=='f')
            remove(name);
        free_d(t->lchild);
        free_d(t->rsibling);
        free(t);
    }
}

void Remove_(char dname[],char option[])
{
    treeptr t=current_node->lchild,p=current_node;
    char name[30];
    int i=0;
    if(current_node==root&&strcmp(dname,"home")==0)
    {
        printf(" Can't remove 'home'. Deny permission.\n");
        return;
    }
    if(current_node==home&&strcmp(dname,"user")==0)
    {
        printf("Can't remove 'user'. Deny permission.\n");
        return;
    }
    for(i=0;i<sizeof(dname)/sizeof(char);i++)
    {
        if(dname[i]=='/')
        {
            Remove_path(dname,option);
            return;
        }
    }
    if(t==NULL)
    {
      printf("In directory '%s', there is no file or folder\n", current_node->name);
      return;
    }
    else //(t!=NULL)
    {
        if (strcmp(t->name,dname)==0)
        {
            if(strcmp(option,"-r")==0||strcmp(option,"-R")==0||strcmp(option,"-rf")==0||t->lchild==NULL)
            {
                if(t->id=='d'&&strcmp(option,"-rf")==0) //����
                {
                    if(t->lchild!=NULL)
                        free_d(t->lchild);
                    free(t);
                }
                else
                {
                    if(t->id=='d')
                    {
                        printf("Can't remove %s. This is directory.\n",dname);
                        return;
                    }
                    if(t->id=='f')
                    {
                        strcpy(name,t->name);
                        strcat(name,".txt");
                        remove(name);
                        free(t);
                    }
                }
                if(t->rsibling!=NULL)
                {
                    p->lchild=t->rsibling;
                    t->rsibling->parent=p;
                }
                else
                    p->lchild=NULL;
            }
            else
            {
                printf("Directory is not empty.\n");
                return;
            }
        }
        else if(strcmp(t->name,dname)!=0)
        {
            while(strcmp(t->name,dname)!=0)
            {
                p=t;
                t=t->rsibling;
                if(t==NULL)
                {
                    printf("Can't remove %s: There are no such files or directories.\n",dname);
                    return;
                }
            }
            if(strcmp(option,"-rf")==0||t->lchild==NULL)
            {
                if(t==NULL)
                {
                    printf("Can't remove directory '%s'\n",dname);
                    return;
                }
                else // t != NULL, t->name == name
                {
                    if(t->id=='d'&&strcmp(option,"-rf")==0)
                    {
                        free_d(t->lchild);
                        free(t);
                    }
                    else
                    {
                        if(t->id=='d')
                        {
                            printf("Can't remove %s.This is directory.\n",dname);
                            return;
                        }
                        if(t->id=='f')
                        {
                            strcpy(name,t->name);
                            strcat(name,".txt");
                            remove(name);
                            free(t);
                        }
                    }
                    p->rsibling=t->rsibling;
                    if(t->rsibling!=NULL)
                        t->rsibling->lsibling=p;
                }
            }
            else
            {
                printf("Directory is not empty.\n");
                return;
            }
        }
    }
}

void RemoveDirectory(char dname[],char option[])
{
    treeptr t=current_node->lchild,p=current_node;
    int i=0;
    if(current_node==root&&strcmp(dname,"home")==0)
    {
        printf("Cant't remove 'home'. Deny permission.\n");
        return;
    }
    if(current_node==home&&strcmp(dname,"user")==0)
    {
        printf("Cant't remove 'user'. Deny permission.\n");
        return;
    }
    for(i=0;i<sizeof(dname)/sizeof(char);i++)
    {
        if(dname[i]=='/')
        {
            Remove_path(dname,option);
            return;
        }
    }
    if(t==NULL)
    {
      printf("In directory '%s', there is no file or folder\n", current_node->name);
      return;
    }
    else //(t!=NULL)
    {
        if (strcmp(t->name,dname)==0)
        {
            if(t->lchild==NULL&&t->id=='d')
                free(t);
            else
            {
                if(t->id=='f')
                {
                    printf("rmdir: failed to remove '%s': This is not directory.\n",dname);
                    return;
                }
                printf("Directory is not empty.\n");
                return;
            }
            if(t->rsibling!=NULL)
            {
                p->lchild=t->rsibling;
                t->rsibling->parent=p;
            }
            else
                p->lchild=NULL;
        }
        else if(strcmp(t->name,dname)!=0)
        {
            while(strcmp(t->name,dname)!=0)
            {
                p=t;
                t=t->rsibling;
                if(t==NULL)
                {
                    printf("Cant't remove %s: There are no such files or directories.\n",dname);
                    return;
                }
            }
            if(t->lchild==NULL&&t->id=='d')
            {
                free(t);
                p->rsibling=t->rsibling;
                if(t->rsibling!=NULL)
                    t->rsibling->lsibling=p;
            }
            else
            {
                if(t->id=='f')
                {
                    printf("rmdir: failed to remove '%s': This is not directory.\n",dname);
                    return;
                }
                printf("Directory is not empty.\n");
                return;
            }
        }
    }
}

void rmdir_p_option(char path[],char option[])
{
    char *p_ch;
    char delimeter[2]="/";
    char dname[40]="\0";
    treeptr t,temp;
    pathptr newnode, head;
    temp=current_node;

    p_ch=strtok(path, delimeter);
    newnode = (pathptr)malloc(sizeof(TreeNode));
    strcpy(newnode->name,p_ch);
    newnode->rsibling=NULL;
    newnode->lsibling=NULL;
    head=newnode;
    t=head;
    if(path[0]=='/')
    {
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,root->name);
        newnode->lsibling=NULL;
        newnode->rsibling=t;
        t->lsibling=newnode;
        head=newnode;
        t=head;
        t=t->rsibling;
    }
    while(p_ch)
    {
        p_ch=strtok(NULL, delimeter);
        if(p_ch==NULL)
            break;
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,p_ch);
        newnode->rsibling=NULL;
        t->rsibling=newnode;
        newnode->lsibling=t;
        t=t->rsibling;
    }
    t=head;
    while(t!=NULL)
    {
        if(fail==1)
        {
            current_node=temp;
            return;
        }
        change_directory(t->name,"");
        t=t->rsibling;
        if(t->rsibling==NULL)
            break;
    }

    while(t!=NULL)
    {
        RemoveDirectory(t->name,"\0");
        change_directory("..","");
        t=t->lsibling;
    }
    current_node=temp;
}

void Remove_path(char path[], char option[])
{
    char *p_ch;
    char delimeter[2]="/";
    char dname[40]="\0";
    treeptr t,temp;
    pathptr newnode, head;
    temp=current_node;

    p_ch=strtok(path, delimeter);
    newnode = (pathptr)malloc(sizeof (TreeNode));
    strcpy(newnode->name,p_ch);
    newnode->rsibling=NULL;
    head=newnode;
    t=head;
    if(path[0]=='/')
    {
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,root->name);
        newnode->rsibling=t;
        head=newnode;
        t=head;
        t=t->rsibling;
    }
    while(p_ch)
    {
        p_ch=strtok(NULL, delimeter);
        if(p_ch==NULL)
            break;
        newnode = (pathptr)malloc(sizeof (TreeNode));
        strcpy(newnode->name,p_ch);
        newnode->rsibling=NULL;
        t->rsibling=newnode;
        t=t->rsibling;
    }
    t=head;
    while(t->rsibling!=NULL)
    {
        if(fail==1)
        {
            current_node=temp;
            return;
        }
        change_directory(t->name,"");
        t=t->rsibling;
        if(t->rsibling==NULL)
            break;
    }
    if(rm==0)
        Remove_(t->name,option);
    else
        RemoveDirectory(t->name,option);
    current_node=temp;
}

void change_directory_path(char path[])
{
    treeptr t;
    t=current_node; //current=user t=user
    char dname[30];
    char delimeter[2]="/";
    char *p_ch;
    if(path[0]=='/')
        current_node=root;  // 1/2/3
    p_ch=strtok(path,delimeter); //p_ch=1
    strcpy(dname,p_ch);
    change_directory(dname,""); //dname 1 ->current=1
    while(p_ch)
    {
        p_ch=strtok(NULL, delimeter);
        if(p_ch==NULL)
            break;
        strcpy(dname,p_ch);
        change_directory(dname,""); //2 current 2 current 3
        if(fail==1)
        {
            current_node=t;
            break;
        }
    }
}

void change_directory (char path[],char option[])
{
    treeptr t=current_node->lchild;
    treeptr temp=current_node;
    char dname[30];
    int i=0;
    if(strcmp(path,"/")==0)
    {
        current_node=root;
        return;
    }
    for(i=0;i<sizeof(path)/sizeof(char);i++)
    {
        if(path[i]=='/')
        {
            change_directory_path(path);
            return;
        }
    }

    if(strcmp(path,"~")==0) //user�� �̵�
        current_node=user;
    else if(strcmp(path,".")==0)//�ڱ� �ڽ�
        current_node=current_node;
    else if(strcmp(path,"..")==0) //���� ������ �̵�
    {
        if(current_node==root)
            current_node=root;
        else
        {
            t=current_node;
            while(t->lsibling!=NULL&&t->parent==NULL)
                t=t->lsibling;
            t=t->parent;
            current_node=t;
        }
    }
    else if (t != NULL)
    {
        if (t->id == 'd'||t->id == 'f')
        {
            if(strcmp(path,t->name)==0)
            {
                if(t->id=='d')
                    current_node=t;
                else
                {
                    printf("bash : cd: %s : This is not directory.\n",t->name);
                    return;
                }
            }
            else
            {
                while(strcmp(t->name,path)!=0&&t!=NULL)
                {
                    t=t->rsibling;
                    if(t==NULL)
                    {
                        fail=1;
                        break;
                    }
                }
                if(t==NULL)
                {
                    fail=1;
                    printf("There are no such files or directories.\n");
                    return;
                }
                else
                {
                    if(t->id=='d')
                        current_node=t;
                    else
                    {
                        printf("bash : cd: %s : This is not directory.\n",t->name);
                        return;
                    }
                }
            }
        }
    }
    else
    {
        fail=1;
        printf("There are no such files or directories.\n");
        return;
    }
}

void ls()
{
    treeptr t=current_node->lchild;
    int n=0;
    if(t==NULL)
        return;
    while(t!=NULL)
    {
        printf("%s  ",t->name);
        t=t->rsibling;
    }
    printf("\n");
}

void ls_long()
{
    treeptr t=current_node->lchild;
    if(t==NULL)
        return;
    while(t!=NULL)
    {
        if(t->id=='f')
            printf("-");
        else
            printf("%c",t->id);
        printf("%s ",t->permission);
        printf("user user ");
        printf("%d/%d %d:%d ",t->timerecord[0],t->timerecord[1],t->timerecord[2],t->timerecord[3]);
        printf("%s\n",t->name);
        t=t->rsibling;
    }
}

void pwd(treeptr now, treeptr endpoint)
{
    if(now == endpoint){
        return;
    }
    if(now->parent!=NULL){
        pwd(now->parent, endpoint);
        if(now->parent!=endpoint && strcmp(now->parent->name,"/")!=0)
            printf("/%s", now->parent->name);
    }
    else if(now->lsibling!=NULL){
        pwd(now->lsibling, endpoint);
    }
}

void print_working_directory(treeptr now, treeptr endpoint){

    treeptr current = now;
    treeptr end = endpoint;
    pwd(current, end);
    printf("/%s", current_node->name);
}

/*
void move(char path[])
{
    if(path[0]=='\0')
        return instruct();
    treeptr t=current_node->lchild;
    treeptr cur=current_node;
    treeptr p=t;
    char *p_ch;
    char a[10]; char b[10]; char c[50]={'\0'}; //���
    int i=0;
    char delimeter[2]=" ";
    p_ch=strtok(path,delimeter);
    if(p_ch==NULL)
        return instruct();
    strcpy(a,p_ch);
    p_ch=strtok(NULL,delimeter);
    if(p_ch==NULL)
        return instruct();
    strcpy(b,p_ch);
    p_ch=strtok(NULL,delimeter);
    if(p_ch!=NULL)
        strcpy(c,p_ch);
    while(t!=NULL)
    {
        if(strcmp(t->name,a)==0)
            break;
        t=t->rsibling;
        if(t==NULL)
        {
            printf("bash: '%s' doesn't exist\n",a);
            return;
        }
    }
    while(p!=NULL)
    {
        if(strcmp(p->name,b)==0&&strcmp(a,b)!=0)
        {
            printf("bash: '%s' already exists\n",t->name);
            return;
        }
        p=p->rsibling;
    }
    strcpy(t->name,b); //t�� source node
    if(c[0]!='\0')
    {
        change_directory(c,"");
        if(fail==1)
        {
            strcpy(t->name,a);
            return;
        }
        if(current_node->lchild!=NULL)
        {
            p=current_node->lchild;
            while(p!=NULL)
            {
                if(strcmp(p->name,t->name)==0)
                {
                    printf("bash : '%s' already exists in '%s'\n",t->name,current_node->name);
                    strcpy(t->name,a);
                    current_node=cur;
                    return;
                }
                p=p->rsibling;
            }
        }
        p=current_node;
        if(t->parent!=NULL)
        {
            if(t->rsibling!=NULL)
            {
                t->rsibling->parent=t->parent;
                t->rsibling->parent->lchild=t->rsibling;
                t->rsibling->lsibling=NULL;
                t->rsibling=NULL;
                t->lsibling=NULL;
                t->parent=NULL;
                if(p->lchild==NULL)
                {
                    p->lchild=t;
                    t->parent=p;
                }
                else
                {
                    p=p->lchild;
                    while(p->rsibling!=NULL)
                        p=p->rsibling;
                    p->rsibling=t;
                    t->lsibling=p;
                }
            }
            else //t->rsibling==NULL
            {
                t->parent->lchild=NULL;
                t->parent=NULL;
                t->rsibling=NULL;
                t->lsibling=NULL;
                if(p->lchild==NULL)
                {
                    p->lchild=t;
                    t->parent=p;
                }
                else
                {
                    p=p->lchild;
                    while(p->rsibling!=NULL)
                        p=p->rsibling;
                    p->rsibling=t;
                    t->lsibling=p;
                }
            }
        }
        else //t->parent==NULL
        {
            if(t->rsibling!=NULL)
            {
                t->rsibling->lsibling=t->lsibling;
                t->lsibling->rsibling=t->rsibling;
                t->rsibling=NULL;
                t->lsibling=NULL;
                t->parent=NULL;
                if(p->lchild==NULL)
                {
                    p->lchild=t;
                    t->parent=p;
                }
                else
                {
                    p=p->lchild;
                    while(p->rsibling!=NULL)
                        p=p->rsibling;
                    p->rsibling=t;
                    t->lsibling=p;
                }
            }
            else //t->rsibling==NULL
            {
                t->lsibling->rsibling=NULL;
                t->rsibling=NULL;
                t->lsibling=NULL;
                t->parent=NULL;
                if(p->lchild==NULL)
                {
                    p->lchild=t;
                    t->parent=p;
                }
                else
                {
                    p=p->lchild;
                    while(p->rsibling!=NULL)
                        p=p->rsibling;
                    p->rsibling=t;
                    t->lsibling=p;
                }
            }
        }
    }
    strcat(a,".txt");
    strcat(b,".txt");
    rename(a,b);
    current_node=cur;
}*/

void save_preorder(FILE *fp, treeptr t)
{
    if (fp == NULL)
    {
        printf(" save error \n");
        return;
    }
    if (t!=NULL)
    {
        fwrite(t, sizeof(TreeNode), 1, fp);
        save_preorder(fp, t->lchild);
        save_preorder(fp, t->rsibling);
    }
}

void save()
{
    treeptr t=root;
    FILE *fp;
    fp=fopen("OS_desktop2.txt","wb");
    if (fp == NULL)
    {
        printf(" save error \n");
        return;
    }
    save_preorder(fp, t);
    fclose(fp);
}

void load_preorder(treeptr t, FILE *fp)
{
    if (t != NULL)
    {
        if(t->lchild != NULL)
        {
            t->lchild = (treeptr)malloc(sizeof(TreeNode));
            fread(t->lchild, sizeof(TreeNode), 1, fp);
            t->lchild->parent = t;
            load_preorder(t->lchild,fp);
        }
        if(t->rsibling != NULL)
        {
            t->rsibling = (treeptr)malloc(sizeof(TreeNode));
            fread(t->rsibling, sizeof(TreeNode), 1, fp);
            t->rsibling->lsibling = t;
            load_preorder(t->rsibling,fp);
        }
    }
}

void load()
{
    FILE *fp;
    fp = fopen("OS_desktop2.txt", "rb");
    if (fp == NULL)
    {
        printf(" load error \n");
        return;
    }
    root = (treeptr)malloc(sizeof(TreeNode));
    fread(root, sizeof(TreeNode), 1, fp);
    load_preorder(root,fp);
}


int instruct()
{
    char fullinst[100]={'\0'};
    while(1)
    {
        int count=0; //multi�� ��츦 �˻��ϱ� ���� ����
        save();

        printf("OS Desktop:");
        if(current_node == home || current_node == root)
        {
            print_working_directory(current_node, NULL);
            printf("$ ");
        }
        else if(current_node==user)
        {
            printf("~user$ ");
        }
        else if(current_node!=home&&current_node!=root&&current_node!=user)
        {
            printf("~");
            print_working_directory(current_node, user);
            printf("$ ");
        }
        gets(fullinst); //��ɾ �ް�
        if(strcmp(fullinst,"\0")==0 || strncmp(fullinst," ",1)==0) //��ɾ ���� �ʾҰų� �������� ������ ���
            return instruct();

        char delimeter[3]=" "; //���� �ϋ�
        char *p_ch;
        char inst[10]="\0";
        char option[10]="\0";
        char path[200]="\0";
        char multi[200]="\0";
        p_ch=strtok(fullinst, delimeter);
        strcpy(inst,p_ch);
        while(p_ch) //��ɾ� �м�
        {
            p_ch=strtok(NULL,delimeter);
            if(p_ch==NULL)
                break;
            else
            {
                if(p_ch[0]=='-'||p_ch[0]=='>')
                    strcpy(option,p_ch);
                else
                {
                    strcpy(path,p_ch);
                    strcat(multi,path);
                    strcat(multi," ");
                    count++;
                }
            }
        }

        if(count>1)
            strcpy(path,multi);

        if(strcmp(inst,"cd")==0)
        {
            if(path[0]==NULL) //cd�� �Է��� ���
                strcpy(path,"~");
            change_directory(path,option);
            fail=0;
        }
        else if(strcmp(inst,"ls")==0)
        {
            if(strcmp(option,"-al")==0||strcmp(option,"-l")==0)
                ls_long();
            else
            {
                if(strcmp(option,"-al")!=0&&strcmp(option,"-l")!=0&&option[0]!='\0')
                {
                    printf("Inappropriate options.\n");
                    return instruct();
                }
                else
                    ls();
            }

        }
        else if(strcmp(inst,"mkdir")==0)
        {
            int i=0;
            cat=2;

            if(count> 1)
            {
               char *p_ch;
               char dname[30];
               p_ch=strtok(path," ");
               strcpy(dname,p_ch);
               MakeDirectory(dname,'d');
               while(p_ch)
               {
                    p_ch=strtok(NULL," ");
                    if(p_ch==NULL)
                        break;
                    strcpy(dname,p_ch);
                    MakeDirectory(dname,'d');
               }
               return instruct();
            }
            if(strcmp(option,"-p")==0)
            {
                mkdir_p_option(path);
                fail=0;
            }
            else
            {
                if(strcmp(option,"-p")!=0&&option[0]!='\0')
                {
                    printf("Inappropriate options.\n");
                    return instruct();
                }
                if(path[0]==NULL)
                {
                    printf("mkdir: Invalid operator.\n");
                    return instruct();
                }
                MakeDirectory(path,'d');
            }
        }
        else if(strcmp(inst,"cat")==0)
        {
            if(strcmp(option,">")==0)
            {
                cat=0;
                MakeDirectory(path,'f');
            }
            if(option[0]=='\0')
            {
                cat=1;
                MakeDirectory(path,'f');
                cat=0;
            }
        }
        else if(strcmp(inst,"rm")==0)
        {
            rm=0;
            if(path[0]==NULL)
                return instruct();
            if (count> 1)
            {
               char *p_ch;
               char dname[30];
               p_ch=strtok(path," ");
               strcpy(dname,p_ch);
               Remove_(dname,"");
               while(p_ch)
               {
                    p_ch=strtok(NULL," ");
                    if(p_ch==NULL)
                        return instruct();
                    strcpy(dname,p_ch);
                    Remove_(dname,"");
               }
            }
            Remove_(path,option);
        }
        else if(strcmp(inst,"rmdir")==0)
        {
            rm=1;
            if (count> 1)
            {
               char *p_ch;
               char dname[30];
               p_ch=strtok(path," ");
               strcpy(dname,p_ch);
               RemoveDirectory(dname,"");
               while(p_ch)
               {
                    p_ch=strtok(NULL," ");
                    if(p_ch==NULL)
                        return instruct();
                    strcpy(dname,p_ch);
                    RemoveDirectory(dname,"");
               }
            }
            if(path[0]==NULL)
                return instruct();
            if(strcmp(option,"-p")==0)
            {
                rmdir_p_option(path, option);
            }
            else
                RemoveDirectory(path,option);
        }
        else if(strcmp(inst,"pwd")==0)
        {
             print_working_directory(current_node, NULL);
             printf("\n");
        }
        else
            printf("Command not found.\n");
    }

}

int main()
{
    FILE *fp;
    fp = fopen("OS_desktop2.txt", "rb");
    if(fp==NULL)
    {
        time_t timer;
        timer=time(NULL);
        moment=localtime(&timer);

        root = (treeptr)malloc(sizeof(TreeNode));
        root->lchild = NULL; root->rsibling = NULL; root->parent=NULL; root->lsibling=NULL;
        root->id = 'd';
        strcpy(root->name, "root");

        home = (treeptr)malloc(sizeof(TreeNode));
        home->lchild = NULL; home->rsibling = NULL; home->parent=NULL; home->lsibling=NULL;
        home->id = 'd';
        strcpy(home->name, "home");

        user = (treeptr)malloc(sizeof(TreeNode));
        user->lchild = NULL; user->rsibling = NULL; user->parent=NULL; user->lsibling=NULL;
        user->id = 'd';
        strcpy(user->name, "user");

        user->timerecord[0]=moment->tm_mon+1;
        user->timerecord[1]=moment->tm_mday;
        user->timerecord[2]=moment->tm_hour;
        user->timerecord[3]=moment->tm_min;
        strcpy(user->permission,"rwxr-xr-x");

        root->lchild=home;
        home->parent=root;
        home->lchild=user;
        user->parent=home;
        current_node = user;
    }
    else
    {
        load();
        home=root->lchild;
        root=home->parent;
        user=home->lchild;
        home=user->parent;
        current_node=user;
    }
    instruct();
}
