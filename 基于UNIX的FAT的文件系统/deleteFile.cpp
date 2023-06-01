/*作者：刘振铭*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
void deleteFile(superBlk* supblk,FILE* disk,Files* fls)  //删除文件
{
    char filename[FILENAMESIZE];
	unsigned short check;

	//cout << "starting delete file, please putin filename" << endl; //输入要删除的文件名
	cin >> filename;
	//cout << "please putin FlieType  (1:txt/0:dir)" << endl; //输入要删除的文件类型
	//cin >> FileType;
	type_t type = Split(filename);
	check = getIno( fls,  disk,  filename, type);	//判断文件是否存在

	if (check == 0xffff) //文件不存在
	{
		cout << "The destination file does not exist" << endl;
		return;
	}
	else //文件存在
	{
		//删除文件inode信息以及FAT表信息
		freeInode(supblk, disk, fls->file[check].f_ino);
		//清理当前目录
		fls->file[check].f_ino = 0;
		fls->file[check].f_name[0] = '\0';

		cout << "The deletion was successful" << endl;
	}
	return;

}
