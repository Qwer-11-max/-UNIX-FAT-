/*���ߣ����*/

#include <iostream>
#include <string.h>
#include "filesys.h"
typedef unsigned short			uid_t;
using namespace std;
bool login(superBlk* superBlk1, User* act_user)//��¼����,�����û��ṹ���ָ�롣
{
    int i;
    char name[USERNAMESIZE] = "0";
    char pass[PWDSIZE] = "0";
    cout << "\n�û���:";
    cin >> name;
    cout << "\n��  ��:";
    cin >> pass;
    for (i = 0; i < 8; i++)//ѭ���ҵ��Ͳ���һ���ĵ��û���
    {
        if ((!strcmp(name, superBlk1->users[i].u_name)) && (!strcmp(pass, superBlk1->users[i].u_pwd)))
        {
            act_user->uid = superBlk1->users[i].uid;//uid����
            act_user->u_mode = superBlk1->users[i].u_mode;//u_mode����
            strcpy(act_user->u_name, superBlk1->users[i].u_name);//u_name����
            strcpy(act_user->u_pwd, superBlk1->users[i].u_name);//u_pwd����
            printf("��¼�ɹ�!!\n");
            return true;//����һ���û�ָ��
        }
    }
    printf("��½ʧ�ܣ���˶���Ϣ���ٵ�¼\n");
    return false;
}
bool create_user(superBlk* superBlk1)//�������û�����,����bool����
{
    User newUser = { 0 };
    cin >> newUser.u_name;
    cin >> newUser.u_pwd;
    int i;
    for (i = 0; i < MAXUSERS; i++)
    {
        if (superBlk1->users[i].uid == 0)
        {
            break;
        }
    }
    if (i == MAXUSERS)//�ж��Ƿ񳬹������
    {
        printf("�Ѵ�����û������޷��ٴ�����\n");
        return false;
    }
    else
    {
        //�洢��Ϣ
        superBlk1->users[i].uid = i + 1;
        superBlk1->users[i].u_mode = USER;
        strcpy(superBlk1->users[i].u_name, newUser.u_name);
        strcpy(superBlk1->users[i].u_pwd, newUser.u_pwd);
        printf("�û������ɹ�\n");
        return true;
    }
}