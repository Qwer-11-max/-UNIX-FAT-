#include "filesys.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//目录跳转功能
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path) {
	//输入新的文件夹名
	char filename[FILENAMESIZE] = "\0";
	printf("输入文件名");
	scanf("%s", filename);
	if (strcmp(filename, "..")) {
		//判断文件夹是否存在
		unsigned short nextIno = getIno(fls, disk, filename, DIR);
		if (nextIno == 0xffff) {
			printf("该目录不存在");
			return;
		}
		//跳转到新的目录
		setCurPath(supblk, disk, curPath, fls, nextIno);
		//更新路径信息
		path->file[path->size].f_ino = nextIno;
		strcpy(path->file[path->size].f_name, filename);
		path->size += 1;
	}
	else {
		setCurPath(supblk, disk, curPath, fls, path->file[path->size-1].f_ino);
		//更新路径信息
		path->size -= 1;
	}
	fflush(stdin);
}