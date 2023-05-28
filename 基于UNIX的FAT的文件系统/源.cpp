#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include"filesys.h"

int main() {
	//打开磁盘
	FILE* disk;
	disk = fopen("disk", "r+");
	//创建超级块
	superBlk* supblk = (superBlk*)malloc(sizeof(superBlk));
	if (!supblk) {
		printf("超级块创建失败");
		exit(1);
	}
	fread(supblk, sizeof(superBlk), 1, disk);
	//如果是第一次启动系统则初始化系统
	if (!supblk->sys_Status) {
		InitSys(supblk, disk);
	}
	//获取系统当前目录
	inode* cur_path = (inode*)malloc(sizeof(inode));
	if (!cur_path) {
		printf("内存分配失败");
		exit(0);
	}
	fseek(disk, supblk->root_ino * INODESIZE + SUPERBLKSIZE * CLUSTERSIZE, SEEK_SET);
	fread(cur_path, sizeof(inode), 1, disk);
	//获取当前目录下文件信息
	files files = {NULL,0};
	int j = 0;
	for (int i = 0; i < cur_path->i_blkCnt; i++) {
		fseek(disk, SYSCLUSTERSIZE*CLUSTERSIZE + cur_path->i_addr * CLUSTERSIZE, SEEK_SET);
		fread(&files.file[j], sizeof(file), CLUSTERSIZE / sizeof(file), disk);
		j += CLUSTERSIZE / sizeof(file);
	}
	//关闭磁盘 
	fclose(disk);

	////创建一个磁盘
	//FILE* disk;
	//disk = fopen("disk","w");
	//unsigned short data[256 * 1024] = { 0 };
	//fwrite(data,sizeof(unsigned short),256*1024,disk);
	//fclose(disk);
	return 0;
}