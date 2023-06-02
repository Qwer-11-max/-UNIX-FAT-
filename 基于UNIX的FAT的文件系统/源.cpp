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
	Openqueue Openqueue[10] = { NULL };//队列打开文件列表
	WholeName* wholename = NULL;//文件完整名结构体
	Files* path = NULL;		//文件路径
	User curUser = { NULL }; //存储的当前用户信息
	//模拟开机
	powerOn(&disk, &supblk, &curPath, &files,&path); 
	//登录
	while (1) {
		while (!login(supblk, &curUser));
		while (mainWindows(supblk, disk, curPath, &curUser, files, path, Openqueue));
	}
	fclose(disk);
	return 0;
}