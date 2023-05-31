#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void CloseFile(char* filename, type_t type, Openqueue* queue)
{
	int i = 0;//用于循环
	bool check = false;//用于判断是否成功删除
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (!strcmp(temp_ptr->f_name,filename) && temp_ptr[i].i_type == type)//如果输入的名与类型皆与表中相同
		{
			temp_ptr[i].f_ino = NULL;
			temp_ptr[i].f_name[0] = '\0';
			temp_ptr[i].i_type = NULL;
			check = true;
			return;
		}
		else continue;
		i = i + 1;
	}
	if (check = true)
		cout << "successful close" << endl;
	else
		cout << "失败，文件名可能存在错误" << endl;
	return;
}