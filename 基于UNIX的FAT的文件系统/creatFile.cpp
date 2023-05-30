#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void creatFile(superBlk* supblk,FILE* disk,Files* fls,uid_t uid,gid_t gid)//文件创建函数，需求的键盘输入,有文件名、文件类型；
{
	char filename[FILENAMESIZE];//定义临时文件名变量
	int FileType;//定义临时文件类型变量
	unsigned short check;//定义临时标志位，辅助判断文件名是否合法

	cout << "starting creatfile, please putin filename" << endl;//输出提示
	cin >> filename;//通过键盘输入文件名
	cout << "please putin FlieType  (1:txt/0:dir)" << endl;//输出提示
	cin >> FileType;//键盘输入文件类型
	
	check = getIno( fls,  disk,  filename,  FileType);//调用查询函数，如果有同名同类型文件，则返回该文件ino；如果无同名文件则返回 -1
	if (check != 0xffff)//如果标志位不是-1，即有同名文件
	{
		cout << "there's something wrong,FileName has been used" << endl;//输出错误信息
		return;
	}
	else//否则创建新文件
	{
		inode* temp = getInode(supblk, uid, gid, FileType);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		cout << "creating success" << endl;
	}
	return;

}
