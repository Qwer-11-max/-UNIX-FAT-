/*作者：马文轩*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk, char* wholefilename,Openqueue* queue)//将特定文件加入打开队列
{	
	int i = 0;//用于循环
	bool check = false;//用于判断是否成功存入
	unsigned short loc;//临时指针
	type_t temp_type = Split(wholefilename);//分词；
	loc=getIno( fls, disk,wholefilename, temp_type);//依据文件名等信息获取文件在fls中的位置
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_ino == NULL)//如果检测到当前队列为空
		{
			temp_ptr[i].f_ino = fls->file[loc].f_ino;//将文件名 拷入队列
			strcpy(temp_ptr[i].f_name, wholefilename);//拷入ino
			temp_ptr[i].i_type = temp_type;//拷入type
			check = true;//修改布尔值
			break;
		}
		else
			continue;
	}
	if (!check)//如果遍历队列后依旧没有成功存储，证明队列已满
	{
		cout << "you open too many files!" << endl;
		return;
	}
	return;
}
