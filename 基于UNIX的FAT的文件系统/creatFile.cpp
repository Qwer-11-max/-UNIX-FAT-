#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void creatFile(superBlk* supblk,FILE* disk,Files* fls,uid_t uid,gid_t gid)//�ļ���������������ļ�������,���ļ������ļ����ͣ�
{
	char filename[FILENAMESIZE];//������ʱ�ļ�������
	int FileType;//������ʱ�ļ����ͱ���
	unsigned short check;//������ʱ��־λ�������ж��ļ����Ƿ�Ϸ�

	cout << "starting creatfile, please putin filename" << endl;//�����ʾ
	cin >> filename;//ͨ�����������ļ���
	cout << "please putin FlieType  (1:txt/0:dir)" << endl;//�����ʾ
	cin >> FileType;//���������ļ�����
	
	check = getIno( fls,  disk,  filename,  FileType);//���ò�ѯ�����������ͬ��ͬ�����ļ����򷵻ظ��ļ�ino�������ͬ���ļ��򷵻� -1
	if (check != 0xffff)//�����־λ����-1������ͬ���ļ�
	{
		cout << "there's something wrong,FileName has been used" << endl;//���������Ϣ
		return;
	}
	else//���򴴽����ļ�
	{
		inode* temp = getInode(supblk, uid, gid, FileType);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		cout << "creating success" << endl;
	}
	return;

}
