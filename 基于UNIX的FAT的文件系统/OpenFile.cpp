/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk, char* wholefilename,Openqueue* queue)//���ض��ļ�����򿪶���
{	
	int i = 0;//����ѭ��
	bool check = false;//�����ж��Ƿ�ɹ�����
	unsigned short loc;//��ʱָ��
	type_t temp_type = Split(wholefilename);//�ִʣ�
	loc=getIno( fls, disk,wholefilename, temp_type);//�����ļ�������Ϣ��ȡ�ļ���fls�е�λ��
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_ino == NULL)//�����⵽��ǰ����Ϊ��
		{
			temp_ptr[i].f_ino = fls->file[loc].f_ino;//���ļ��� �������
			strcpy(temp_ptr[i].f_name, wholefilename);//����ino
			temp_ptr[i].i_type = temp_type;//����type
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
