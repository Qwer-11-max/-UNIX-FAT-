/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk,Openqueue* queue)//���ض��ļ�����򿪶���
{	
	char filename[FILENAMESIZE] = "\0";
	cin >> filename;
	type_t type = Split(filename);
	int i = 0;//����ѭ��
	bool check = false;//�����ж��Ƿ�ɹ�����
	unsigned short loc;//��ʱָ��
	loc=getIno( fls, disk,filename, type);//�����ļ�������Ϣ��ȡ�ļ���fls�е�λ��
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_name[0] == '\0')//�����⵽��ǰ����Ϊ��
		{
			fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + fls->file[loc].f_ino * INODESIZE,SEEK_SET);
			fread(&temp_ptr[i].f_inode, sizeof(inode), 1, disk);	//����inode��Ϣ
			strcpy(temp_ptr[i].f_name, filename);//�����ļ���
			check = true;//�޸Ĳ���ֵ
			cout << filename << "�Ѵ�\n";
			break;
		}
		else
			continue;
	}
	if (!check)//����������к�����û�гɹ��洢��֤����������
	{
		cout << "you open too many files!" << endl;
		return;
	}
	return;
}
