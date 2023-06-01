/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void deleteFile(superBlk* supblk,FILE* disk,Files* fls)  //ɾ���ļ�
{
    char filename[FILENAMESIZE];
	unsigned short check;

	//cout << "starting delete file, please putin filename" << endl; //����Ҫɾ�����ļ���
	cin >> filename;
	//cout << "please putin FlieType  (1:txt/0:dir)" << endl; //����Ҫɾ�����ļ�����
	//cin >> FileType;
	type_t type = Split(filename);
	check = getIno( fls,  disk,  filename, type);	//�ж��ļ��Ƿ����

	if (check == 0xffff) //�ļ�������
	{
		cout << "The destination file does not exist" << endl;
		return;
	}
	else //�ļ�����
	{
		//ɾ���ļ�inode��Ϣ�Լ�FAT����Ϣ
		freeInode(supblk, disk, fls->file[check].f_ino);
		//����ǰĿ¼
		fls->file[check].f_ino = 0;
		fls->file[check].f_name[0] = '\0';

		cout << "The deletion was successful" << endl;
	}
	return;

}
