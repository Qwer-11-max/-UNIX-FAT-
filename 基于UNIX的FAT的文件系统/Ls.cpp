/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>
using namespace std;
void Ls(Files* fls)//չʾ��ǰĿ¼�������ļ���
{	
	int i = 0;
	Files* temp_ptr = fls;//��ʱָ����ղ���
	for (i = 0;i < SUBFILENUM;i++){//ѭ��
		if (temp_ptr->file[i].f_name[0] != '\0')
			cout << temp_ptr->file[i].f_name << "  ";//�ļ������������ӡ֮
	}
	cout << endl;
}
