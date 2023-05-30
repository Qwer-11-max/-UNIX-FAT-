#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk, char* filename, type_t type,Openqueue* queue)//将特定文件加入打开队列
{	
	int i = 0;//用于循环
	bool check = false;//用于判断是否成功存入
	unsigned short temp_ino;//临时指针
	temp_ino=getIno( fls, disk,filename, type);//依据文件名等信息获取 ino
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_ino == NULL)//如果检测到当前队列为空
		{
			temp_ptr[i].f_ino = temp_ino;//将文件名和其 ino 拷入队列
			strcpy(temp_ptr[i].f_name, filename);
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
