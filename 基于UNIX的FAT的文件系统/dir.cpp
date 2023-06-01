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
 		inode* temp = getInode(supblk,disk, 0, 0, DIR);
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		free(temp);
		printf("Ŀ¼�����ɹ�\n");
	}
	else { // �������ӡ������Ϣ
		printf("��Ŀ¼�Ѵ���\n");
	}
}

//��ȡ�ļ�����
type_t getType(FILE* disk, unsigned short ino) {
	inode temp = { NULL };
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + ino * CLUSTERSIZE, SEEK_SET);
	fread(&temp, sizeof(inode), 1, disk);
	return temp.i_type;
}
//ɾ��һ��Ŀ¼����ino����ʶ
void delDir(superBlk* supblk, FILE* disk, unsigned short ino) {
	inode temp = { NULL };
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + ino * INODESIZE,SEEK_SET);
	fread(&temp, sizeof(inode), 1, disk);

	Files fls = { NULL };
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + temp.i_addr * CLUSTERSIZE, SEEK_SET);
	fread(&fls, sizeof(Files), 1, disk);

	for (int i = 0; i < SUBFILENUM; i++) {
		file* t1 = &fls.file[i];
		if (t1->f_name[0] != '\0') {
			if (getType(disk, t1->f_ino) == DIR)
				delDir(supblk, disk, t1->f_ino);
			printf("%sɾ���ɹ�\n",t1->f_name);
			freeInode(supblk, disk, t1->f_ino);
		}
	}
}
//ɾ��Ŀ¼
void rmdir(superBlk* supblk, FILE* disk, Files* fls) {
	//�����ļ���
	char dirname[FILENAMESIZE] = "\0";
	scanf("%s", dirname);
	//�жϸ�Ŀ¼�Ƿ����
	int loc = getIno(fls, disk, dirname, DIR);
	if (loc == 0xffff) {
		printf("�����ڸ�Ŀ¼\n");
		return;
	}
	
	delDir(supblk, disk, fls->file[loc].f_ino);
	freeInode(supblk, disk, fls->file[loc].f_ino);
	printf("%sɾ���ɹ�\n", dirname);
	fls->file[loc] = { NULL };
}



