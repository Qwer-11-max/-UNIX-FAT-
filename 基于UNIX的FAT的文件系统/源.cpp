#include<iostream>
#include<fstream>
#include<stdio.h>
#include<stdlib.h>
#include"filesys.h"

int main() {
	//�򿪴���
	FILE* disk;	
	disk = fopen("disk", "r+");
	//����������
	superBlk* supblk = (superBlk*)malloc(sizeof(superBlk));
	if (!supblk) {
		printf("�����鴴��ʧ��");
		exit(1);
	}
	fread(supblk, sizeof(superBlk), 1, disk);
	//����ǵ�һ������ϵͳ���ʼ��ϵͳ
	if (!supblk->sys_Status) {
		InitSys(supblk);
	}
	//�رմ��� 
	fclose(disk);

	////����һ������
	//FILE* disk;
	//disk = fopen("disk","w");
	//unsigned short data[256 * 1024] = { 0 };
	//fwrite(data,sizeof(unsigned short),256*1024,disk);
	//fclose(disk);
	return 0;
}