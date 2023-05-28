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
		InitSys(supblk, disk);
	}
	//��ȡϵͳ��ǰĿ¼
	inode* cur_path = (inode*)malloc(sizeof(inode));
	if (!cur_path) {
		printf("�ڴ����ʧ��");
		exit(0);
	}
	fseek(disk, supblk->root_ino * INODESIZE + SUPERBLKSIZE * CLUSTERSIZE, SEEK_SET);
	fread(cur_path, sizeof(inode), 1, disk);
	//��ȡ��ǰĿ¼���ļ���Ϣ
	files files = {NULL,0};
	int j = 0;
	for (int i = 0; i < cur_path->i_blkCnt; i++) {
		fseek(disk, SYSCLUSTERSIZE*CLUSTERSIZE + cur_path->i_addr * CLUSTERSIZE, SEEK_SET);
		fread(&files.file[j], sizeof(file), CLUSTERSIZE / sizeof(file), disk);
		j += CLUSTERSIZE / sizeof(file);
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