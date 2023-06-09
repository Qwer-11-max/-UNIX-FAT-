/*作者：赵熙龙*/

#include "filesys.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//目录跳转功能
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path) { //赵熙龙
	//输入新的文件夹名
	char filename[FILENAMESIZE] = "\0";
	scanf("%s", filename);
	if (strcmp(filename, "..")) {
		//判断文件夹是否存在
		unsigned short loc = getIno(fls, disk, filename, DIR);
		if (loc == 0xffff) {
			printf("该目录不存在\n");
			return;
		}
		//跳转到新的目录
		unsigned short nextIno = fls->file[loc].f_ino;
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

void mkdir(superBlk* supblk, FILE* disk, Files* fls) { //赵熙龙
	//输入子目录名
	char filename[FILENAMESIZE];
	scanf("%s", filename);
	//判断目录是否存在
	unsigned short ino = getIno(fls, disk, filename, DIR);
	if (ino == 0xffff) { //不存在则创建新目录
 		inode* temp = getInode(supblk,disk, 0, 0, DIR);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		free(temp);
		printf("目录创建成功\n");
	}
	else { // 存在则打印错误信息
		printf("该目录已存在\n");
	}
}

//获取文件类型
type_t getType(FILE* disk, unsigned short ino) {
	inode temp = { NULL };
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + ino * CLUSTERSIZE, SEEK_SET);
	fread(&temp, sizeof(inode), 1, disk);
	return temp.i_type;
}
//删除一个目录，用ino来标识
void delDir(superBlk* supblk, FILE* disk, unsigned short ino) {
	inode temp = { NULL };
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + ino * INODESIZE,SEEK_SET);
	fread(&temp, sizeof(inode), 1, disk);

	Files fls = { NULL };
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + temp.i_addr * CLUSTERSIZE, SEEK_SET);
	fread(&fls, sizeof(Files), 1, disk);

	for (int i = 0; i < SUBFILENUM; i++) {
		file* t1 = &fls.file[i];
		if (t1->f_name[0] != '\0') {
			if (getType(disk, t1->f_ino) == DIR)
				delDir(supblk, disk, t1->f_ino);
			printf("%s删除成功\n",t1->f_name);
			freeInode(supblk, disk, t1->f_ino);
		}
	}
}
//删除目录
void rmdir(superBlk* supblk, FILE* disk, Files* fls) {
	//输入文件名
	char dirname[FILENAMESIZE] = "\0";
	scanf("%s", dirname);
	//判断该目录是否存在
	int loc = getIno(fls, disk, dirname, DIR);
	if (loc == 0xffff) {
		printf("不存在该目录\n");
		return;
	}
	
	delDir(supblk, disk, fls->file[loc].f_ino);
	freeInode(supblk, disk, fls->file[loc].f_ino);
	printf("%s删除成功\n", dirname);
	fls->file[loc] = { NULL };
}



