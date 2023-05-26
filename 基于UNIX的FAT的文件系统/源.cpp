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
		InitSys(supblk);
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