#include <iostream>
#include <string.h>
#include "filesys.h"
typedef unsigned short			uid_t;
using namespace std;
User* login(superBlk *superBlk1,  char *u_name, char *u_pwd, User *act_user)//登录函数,返回用户结构体的指针。
{
    for(int i=0;i<MAXUSERS;i++)//循环找到和参数一样的的用户。
    {
        if((!strcmp(u_name,superBlk1->users[i].u_name))&&(!strcmp(u_pwd,superBlk1->users[i].u_pwd)))
        {
            act_user->uid=superBlk1->users[i].uid;//uid复制
            act_user->u_mode=superBlk1->users[i].u_mode;//u_mode复制
            strcpy(act_user->u_name,superBlk1->users[i].u_name);//u_name复制
            strcpy(act_user->u_pwd,superBlk1->users[i].u_name);//u_pwd复制
            printf("登录成功!!\n");
            return act_user;//返回一个用户指针
        }
    }
    printf("登陆失败，请核对信息后再登录\n");
}
bool create_user(superBlk *superBlk1,char *new_name,char *new_pwd)//创建新用户函数,返回bool类型
{
    int i;
    for(i=0;i<MAXUSERS;i++)
    {
        if(superBlk1->users[i].uid==0)
        {
            break;
        }
    }
    if(i==MAXUSERS)//判断是否超过最大数
    {
        printf("已达最大用户数，无法再创建。");
        return false;
    } else
    {
        //存储信息
        superBlk1->users[i+1].uid=i+1;
        superBlk1->users[i+1].u_mode=USER;
        strcpy(superBlk1->users[i+1].u_name,new_name);
        strcpy(superBlk1->users[i+1].u_pwd,new_pwd);
        printf("用户创建成功");
        return true;
    }
}