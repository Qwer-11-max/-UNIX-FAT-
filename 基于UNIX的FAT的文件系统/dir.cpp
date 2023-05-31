#include "filesys.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//目录跳转功能
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path) {
	//输入新的文件夹名
	char filename[FILENAMESIZE] = "\0";
	scanf("%s", filename);
	if (strcmp(filename, "..")) {
		//判断文件夹是否存在
		unsigned short nextIno = getIno(fls, disk, filename, DIR);
		if (nextIno == 0xffff) {
			printf("该目录不存在\n");
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
		setCurPath(supblk, disk, curPath, fls, path->file[path->size-2].f_ino);
		//更新路径信息
		path->size -= 1;
	}
	fflush(stdin);
}

void mkdir(superBlk* supblk, FILE* disk, Files* fls) {
	//输入子目录名
	char filename[FILENAMESIZE];
	scanf("%s", filename);
	//判断目录是否存在
	unsigned short ino = getIno(fls, disk, filename, DIR);
	if (ino == 0xffff) { //不存在则创建新目录
 		inode* temp = getInode(supblk, 0, 0, DIR);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		printf("目录创建成功\n");
	}
	else { // 存在则打印错误信息
		printf("该目录已存在\n");
	}
}