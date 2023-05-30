#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void createFile(superBlk* supblk, Files * fls, type_t type, uid_t uid, gid_t gid)//文件创建函数，需求的键盘输入有文件名、文件类型；
{
	char filename[FILENAMESIZE];//定义临时文件名变量
	int FileType;//定义临时文件类型变量
	unsigned short check;//定义临时标志位，辅助判断文件名是否合法

	inode *temp_ptr=getInode(supblk,uid, gid, type);//临时指针承接 函数的返回值，成为结构体指针

	cout << "starting creatfile, please putin filename" << endl;//输出提示
	cin >> filename;//通过键盘输入文件名
	cout << "please putin FlieType  (1:txt/0:dir)" << endl;//输出提示
	cin >> FileType;//键盘输入文件类型
	

	return;

}
