/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void CloseFile(Openqueue* queue)
{
	char filename[FILENAMESIZE] = "\0";
	cin >> filename;

	int i = 0;//����ѭ��
	bool check = false;//�����ж��Ƿ�ɹ�ɾ��
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (strcmp(temp_ptr[i].f_name, filename) == 0)//���������������ͽ��������ͬ
		{
			temp_ptr[i].f_name[0] = '\0';
			temp_ptr[i].f_inode = { NULL };
			check = true;

			return;
		}
		else continue;
	}
	if (check == true)
		cout << filename <<"�ѹر�" << endl;
	else
		cout << "ʧ�ܣ��ļ������ܴ��ڴ���" << endl;
	return;
}