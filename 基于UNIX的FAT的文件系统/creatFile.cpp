#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void createFile(superBlk* supblk, Files * fls, type_t type, uid_t uid, gid_t gid)//�ļ���������������ļ����������ļ������ļ����ͣ�
{
	char filename[FILENAMESIZE];//������ʱ�ļ�������
	int FileType;//������ʱ�ļ����ͱ���
	unsigned short check;//������ʱ��־λ�������ж��ļ����Ƿ�Ϸ�

	inode *temp_ptr=getInode(supblk,uid, gid, type);//��ʱָ��н� �����ķ���ֵ����Ϊ�ṹ��ָ��

	cout << "starting creatfile, please putin filename" << endl;//�����ʾ
	cin >> filename;//ͨ�����������ļ���
	cout << "please putin FlieType  (1:txt/0:dir)" << endl;//�����ʾ
	cin >> FileType;//���������ļ�����
	

	return;

}
