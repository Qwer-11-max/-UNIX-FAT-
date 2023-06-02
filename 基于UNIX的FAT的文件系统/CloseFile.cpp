/*作者：马文轩*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void CloseFile(Openqueue* queue)
{
	char filename[FILENAMESIZE] = "\0";
	cin >> filename;

	int i = 0;//用于循环
	bool check = false;//用于判断是否成功删除
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (strcmp(temp_ptr[i].f_name, filename) == 0)//如果输入的名与类型皆与表中相同
		{
			temp_ptr[i].f_name[0] = '\0';
			temp_ptr[i].f_inode = { NULL };
			check = true;

			return;
		}
		else continue;
	}
	if (check == true)
		cout << filename <<"已关闭" << endl;
	else
		cout << "失败，文件名可能存在错误" << endl;
	return;
}