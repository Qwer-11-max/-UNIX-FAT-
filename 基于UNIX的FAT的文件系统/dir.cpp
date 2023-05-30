#include "filesys.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Ŀ¼��ת����
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path) {
	//�����µ��ļ�����
	char filename[FILENAMESIZE] = "\0";
	printf("�����ļ���");
	scanf("%s", filename);
	if (strcmp(filename, "..")) {
		//�ж��ļ����Ƿ����
		unsigned short nextIno = getIno(fls, disk, filename, DIR);
		if (nextIno == 0xffff) {
			printf("��Ŀ¼������");
			return;
		}
		//��ת���µ�Ŀ¼
		setCurPath(supblk, disk, curPath, fls, nextIno);
		//����·����Ϣ
		path->file[path->size].f_ino = nextIno;
		strcpy(path->file[path->size].f_name, filename);
		path->size += 1;
	}
	else {
		setCurPath(supblk, disk, curPath, fls, path->file[path->size-1].f_ino);
		//����·����Ϣ
		path->size -= 1;
	}
	fflush(stdin);
}