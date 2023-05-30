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
	
	check = getIno(filename);//���ò�ѯ�����������ͬ��ͬ�����ļ����򷵻ظ��ļ�ino�������ͬ���ļ��򷵻� -1
	if (check != -1)//�����־λ����-1������ͬ���ļ�
	{
		cout << "there's something wrong,FileName has been used" << endl;
		FreeInode(supblk, temp_ptr->i_ino);//�ͷ��Ѿ�����Ľڵ�
		return;
	}
	else//����
	{
		temp_ptr->i_type = FileType;//���ļ����ʹ���inode
		fls->file[10].f_ino = temp_ptr->i_ino;//���ļ�����ino��д�뵽��ǰĿ¼���ļ��б���
		strcpy(fls->file[10].f_name, filename);
		cout << "creating success" << endl;

	}
	return;

}
