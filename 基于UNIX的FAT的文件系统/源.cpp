#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include"filesys.h"

int main() {
	//创建几个指针，用于保存系统运行时所需要的数据
	FILE* disk = NULL; //模拟磁盘的文件
	superBlk* supblk = NULL; // 超级块信息
	inode* curPath = NULL; //当前目录
	Files* files = NULL; //当前目录的子文件列表
	//模拟开机
	powerOn(&disk, &supblk, &curPath, &files); 
	setCurPath(supblk,disk,curPath,files,2);
	fclose(disk);

	////创建一个磁盘
	//FILE* disk;
	//disk = fopen("disk","w");
	//unsigned short data[256 * 1024] = { 0 };
	//fwrite(data,sizeof(unsigned short),256*1024,disk);
	//fclose(disk);
	return 0;
}