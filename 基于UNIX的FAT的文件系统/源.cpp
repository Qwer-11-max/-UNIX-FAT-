#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include"filesys.h"

int main() {
	//��������ָ�룬���ڱ���ϵͳ����ʱ����Ҫ������
	FILE* disk = NULL; //ģ����̵��ļ�
	superBlk* supblk = NULL; // ��������Ϣ
	inode* curPath = NULL; //��ǰĿ¼
	Files* files = NULL; //��ǰĿ¼�����ļ��б�
	Openqueue* Openqueue[10] = { NULL };//�����ļ��б�
	Files* path = NULL;
	//ģ�⿪��
	powerOn(&disk, &supblk, &curPath, &files,&path); 
	//��¼
	while (1) {
		mainWindows(supblk, disk, curPath, NULL, files, path);
		//system("cls");
	}
	fclose(disk);

	////����һ������
	//FILE* disk;
	//disk = fopen("disk","w");
	//unsigned short data[256 * 1024] = { 0 };
	//fwrite(data,sizeof(unsigned short),256*1024,disk);
	//fclose(disk);
	return 0;
}