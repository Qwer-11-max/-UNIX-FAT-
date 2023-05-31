#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
 
void deleteFile(superBlk* supblk,FILE* disk,Files* fls)//文件删除函数，需求的键盘输入,有文件名、文件类型；
{
    char filename[FILENAMESIZE];//定义临时文件名变量
	int FileType;//定义临时文件类型变量
	unsigned short check;//定义临时标志位，判断文件名是否存在

	cout << "starting delete file, please putin filename" << endl;//输出提示
	cin >> filename;//输入文件名
	cout << "please putin FlieType  (1:txt/0:dir)" << endl;//输出提示
	cin >> FileType;//输入文件类型
	
	check = getIno( fls,  disk,  filename,  FileType);//调用查询函数，如果有同名同类型文件，则返回该文件ino；如果无同名文件则返回 -1
	if (check = 0xffff)//如果标志位是-1，即无同名文件
	{
		cout << "The destination file does not exist" << endl;//输出错误信息
		return;
	}
	else//删除文件，释放索引号和磁盘空间
	{
        freeInode(supblk, disk, check);
		cout << "The deletion was successful" << endl;
	}
	return;

}
