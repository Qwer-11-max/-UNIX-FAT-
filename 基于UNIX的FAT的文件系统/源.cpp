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
	Openqueue Openqueue[10] = { NULL };//���д��ļ��б�
	WholeName* wholename = NULL;//�ļ��������ṹ��
	Files* path = NULL;		//�ļ�·��
	User curUser = { NULL }; //�洢�ĵ�ǰ�û���Ϣ
	//ģ�⿪��
	powerOn(&disk, &supblk, &curPath, &files,&path); 
	//��¼
	while (1) {
		//while (!login(supblk, &curUser));
		while (mainWindows(supblk, disk, curPath, &curUser, files, path, Openqueue));
	}
	fclose(disk);

	/*����һ������*/
	//FILE* disk;
	//disk = fopen("disk","w");
	//unsigned short data[256 * 1024] = { 0 };
	//fwrite(data,sizeof(unsigned short),256*1024,disk);
	//fclose(disk);
	return 0;
}