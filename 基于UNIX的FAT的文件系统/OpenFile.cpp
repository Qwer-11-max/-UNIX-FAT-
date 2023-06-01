/*作者：马文轩*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void OpenFile(Files* fls, FILE* disk,Openqueue* queue)//将特定文件加入打开队列
{	
	char filename[FILENAMESIZE] = "\0";
	cin >> filename;
	type_t type = Split(filename);
	int i = 0;//用于循环
	bool check = false;//用于判断是否成功存入
	unsigned short loc;//临时指针
	loc=getIno( fls, disk,filename, type);//依据文件名等信息获取文件在fls中的位置
	Openqueue* temp_ptr = queue;
	for (i = 0;i < 10;i++)
	{
		if (temp_ptr[i].f_name[0] == '\0')//如果检测到当前队列为空
		{
			fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + fls->file[loc].f_ino * INODESIZE,SEEK_SET);
			fread(&temp_ptr[i].f_inode, sizeof(inode), 1, disk);	//拷入inode信息
			strcpy(temp_ptr[i].f_name, filename);//拷入文件名
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
