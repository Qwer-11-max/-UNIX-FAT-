/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk, char* filename, type_t type,Openqueue* queue)//���ض��ļ�����򿪶���
{	
	int i = 0;//����ѭ��
	bool check = false;//�����ж��Ƿ�ɹ�����
	unsigned short loc;//��ʱָ��
	loc=getIno( fls, disk,filename, type);//�����ļ�������Ϣ��ȡ�ļ���fls�е�λ��
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_ino == NULL)//�����⵽��ǰ����Ϊ��
		{
			temp_ptr[i].f_ino = fls->file[loc].f_ino;//���ļ��� �������
			strcpy(temp_ptr[i].f_name, filename);//����ino
			temp_ptr[i].i_type = type;//����type
			check = true;//�޸Ĳ���ֵ
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
