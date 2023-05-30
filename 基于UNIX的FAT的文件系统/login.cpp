#include <iostream>
#include <string.h>
#include "filesys.h"
typedef unsigned short			uid_t;
using namespace std;
User* login(superBlk *superBlk1,  char *u_name, char *u_pwd, User *act_user)//��¼����,�����û��ṹ���ָ�롣
{
    for(int i=0;i<MAXUSERS;i++)//ѭ���ҵ��Ͳ���һ���ĵ��û���
    {
        if((!strcmp(u_name,superBlk1->users[i].u_name))&&(!strcmp(u_pwd,superBlk1->users[i].u_pwd)))
        {
            act_user->uid=superBlk1->users[i].uid;//uid����
            act_user->u_mode=superBlk1->users[i].u_mode;//u_mode����
            strcpy(act_user->u_name,superBlk1->users[i].u_name);//u_name����
            strcpy(act_user->u_pwd,superBlk1->users[i].u_name);//u_pwd����
            printf("��¼�ɹ�!!\n");
            return act_user;//����һ���û�ָ��
        }
    }
    printf("��½ʧ�ܣ���˶���Ϣ���ٵ�¼\n");
}
bool create_user(superBlk *superBlk1,char *new_name,char *new_pwd)//�������û�����,����bool����
{
    int i;
    for(i=0;i<MAXUSERS;i++)
    {
        if(superBlk1->users[i].uid==0)
        {
            break;
        }
    }
    if(i==MAXUSERS)//�ж��Ƿ񳬹������
    {
        printf("�Ѵ�����û������޷��ٴ�����");
        return false;
    } else
    {
        //�洢��Ϣ
        superBlk1->users[i+1].uid=i+1;
        superBlk1->users[i+1].u_mode=USER;
        strcpy(superBlk1->users[i+1].u_name,new_name);
        strcpy(superBlk1->users[i+1].u_pwd,new_pwd);
        printf("�û������ɹ�");
        return true;
    }
}