#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void CloseFile(char* filename, type_t type, Openqueue* queue)
{
	int i = 0;//����ѭ��
	bool check = false;//�����ж��Ƿ�ɹ�ɾ��
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (!strcmp(temp_ptr->f_name,filename) && temp_ptr[i].i_type == type)//���������������ͽ��������ͬ
		{
			temp_ptr[i].f_ino = NULL;
			temp_ptr[i].f_name[0] = '\0';
			temp_ptr[i].i_type = NULL;
			check = true;
			return;
		}
		else continue;
	}
	if (check == true)
		cout << "successful close" << endl;
	else
		cout << "ʧ�ܣ��ļ������ܴ��ڴ���" << endl;
	return;
}