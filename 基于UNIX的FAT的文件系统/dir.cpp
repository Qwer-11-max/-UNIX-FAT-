/*���ߣ�������*/

#include "filesys.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Ŀ¼��ת����
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path) { //������
	//�����µ��ļ�����
	char filename[FILENAMESIZE] = "\0";
	scanf("%s", filename);
	if (strcmp(filename, "..")) {
		//�ж��ļ����Ƿ����
		unsigned short loc = getIno(fls, disk, filename, DIR);
		if (loc == 0xffff) {
			printf("��Ŀ¼������\n");
			return;
		}
		//��ת���µ�Ŀ¼
		unsigned short nextIno = fls->file[loc].f_ino;
		setCurPath(supblk, disk, curPath, fls, nextIno);
		//����·����Ϣ
		path->file[path->size].f_ino = nextIno;
		strcpy(path->file[path->size].f_name, filename);
		path->size += 1;
	}
	else {
		setCurPath(supblk, disk, curPath, fls, path->file[path->size-2].f_ino);
		//����·����Ϣ
		path->size -= 1;
	}
	fflush(stdin);
}

void mkdir(superBlk* supblk, FILE* disk, Files* fls) { //������
	//������Ŀ¼��
	char filename[FILENAMESIZE];
	scanf("%s", filename);
	//�ж�Ŀ¼�Ƿ����
	unsigned short ino = getIno(fls, disk, filename, DIR);
	if (ino == 0xffff) { //�������򴴽���Ŀ¼
 		inode* temp = getInode(supblk, 0, 0, DIR);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		printf("Ŀ¼�����ɹ�\n");
	}
	else { // �������ӡ������Ϣ
		printf("��Ŀ¼�Ѵ���\n");
	}
}