#include"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//��һ������������locλ��Ϊ1
#define SetOne(x,loc) x = ( x | 1<<loc)

//����inode�ڵ�
inode *getInode(superBlk *supblk,uid_t uid, gid_t gid,type_t type) {
	inode* temp = (inode*)malloc(sizeof(inode));
	if (!temp) {
		printf("�����µ�inode�ڵ�ʧ��\n");
		exit(1);
	}
	//���ó�ʼ����
	temp->i_uid = uid;	//�û�uid
	temp->i_gid = gid;	//��id
	temp->i_acl = 0;	//�ļ�Ȩ��
	temp->i_type = type;	//�ļ�����
	temp->i_size = 1;	//�ļ���С
	temp->i_blkCnt = 1;	//�ļ������
	time(&temp->i_mtime);	//�ļ��޸�ʱ��
	temp->i_atime = temp->i_mtime;	//�ļ��޸�ʱ��
	//������п�
	for (int i = 1; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			temp->i_addr = i;
			supblk->FAT[i] = -1;	//��ʾ�ļ���ռ���ҵ�ǰλ��Ϊ�ļ������һ��
			supblk->free_blk -= 1; //���п��һ
			supblk->free_disk -= CLUSTERSIZE; //���пռ��һ���ش�С
			break;
		}
	}
	//�������inode���
	for (int i = 0; i < BITMAPSIZE; i++) {
		if (supblk->bitmap_inode[i] ^ 0b11111111) {
			unsigned short free = supblk->bitmap_inode[i] & 0b11111111;
			int loc = 0;
			while (free & 1) {
				free = free >> 1;
				loc++;
			}
			SetOne(supblk->bitmap_inode[i], loc);
			temp->i_ino = i * 16 + loc;
			break;
		}
	}
	supblk->inode_count += 1;
	supblk->inode_free -= 1;
	return temp;
}
//�ҵ��ļ�ռ�õĿ�����
unsigned short* getFATList(superBlk * supblk,inode* target) {
	unsigned short* FATList = (unsigned short*)malloc(sizeof(unsigned short) * target->i_blkCnt);
	if (!FATList) {
		printf("�ڴ�������");
		exit(0);
	}
	int i = 0;
	FATList[i] = target->i_addr;
	unsigned short addr = supblk->FAT[target->i_addr];
	while (addr != 0xffff) {
		i += 1;
		*(FATList + i) = addr;
		addr = supblk->FAT[addr];
	}
	return FATList;
}
// Ŀ¼��ת
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno) {
	//�����Ŀ¼��Ϣ
	int curPoint = 0;
	unsigned short data[CLUSTERSIZE / sizeof(unsigned)] = {0};
	//���µ�ǰĿ¼��inode�ڵ���Ϣ
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + curPath->i_ino * INODESIZE, SEEK_SET);
	fwrite(curPath, sizeof(inode), 1, disk);
	//�������еľ�Ŀ¼����Ĩ��
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE,SEEK_SET);
	fwrite(data, sizeof(unsigned short), CLUSTERSIZE / sizeof(unsigned), disk);
	//д��ɵ�Ŀ¼��Ϣ
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	for (int i = 0; i < SUBFILENUM; i++) {
		if (fls->file[i].f_name[0] !='\0') {
			fwrite(&fls->file, sizeof(file), 1, disk);
			fseek(disk,sizeof(file), SEEK_CUR);
		}
	}
	//��ȡ��Ŀ¼��Ϣ
	//1.��ȡinode
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + nextDirIno * INODESIZE, SEEK_SET);
	fread(curPath, sizeof(inode), 1, disk);
	//2.��ȡ���ļ�
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	fread(fls, sizeof(Files), 1, disk);
	return 0;
}
//����Ŀ¼���ļ��б��ҵ��ļ�����Ӧ��ino�����û���򷵻�-1��0xFFFF
unsigned short getIno(Files* fls,FILE* disk, char* filename,type_t type) {
	for (int i = 0; i < SUBFILENUM; i++) {
		//�ж��ļ����Ƿ�һ��
		if (!strcmp(filename,fls->file[i].f_name)) {
			inode temp = {};
			fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + fls->file[i].f_ino * INODESIZE, SEEK_SET);
			fread(&temp, sizeof(inode), 1, disk);
			//�ж��ļ������Ƿ�һ��
			if(temp.i_type == type)
				//����ino
				return fls->file[i].f_ino;
		}
	}
	//����������ļ��򷵻�-1
	return -1;
}
//
//�����ļ�
void createFile(superBlk* supblk,char * filename,type_t type,uid_t uid,gid_t gid){

}
//����Ŀ¼
void mkdir(superBlk* supblk,FILE* disk,char* dirname,uid_t uid,gid_t gid,unsigned short prnt_ino) {
	//�洢Ŀ¼��Ϣ
	file temp;
	//�ļ���
	strcat(temp.f_name, dirname);
	//Ϊ��Ŀ¼����inode
	inode* dirInode = getInode(supblk, uid, gid, DIR);
	//��¼Ŀ¼��ino
	temp.f_ino = dirInode->i_ino;
	
	//�ҵ���Ŀ¼��inode����ȡ
	inode* prnt_inode = (inode*)malloc(sizeof(inode));
	if (!prnt_inode) {
		printf("�����ڵ����ʧ��");
		exit(0);
	}
	//�ҵ���Ŀ¼��inode����ȡ
	fseek(disk,SUPERBLKSIZE*CLUSTERSIZE+prnt_ino*INODESIZE,SEEK_SET);
	fread(prnt_inode, sizeof(inode), 1, disk);

	//�ҵ���Ŀ¼�����ݿ����ڵز���Ŀ¼��Ϣд��
	fseek(disk, prnt_inode->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&temp, sizeof(file), 1, disk);

	//�ͷ�����ָ��
	free(dirInode);
	free(prnt_inode);
}

//��ʼ���ļ�ϵͳ
void InitSys(superBlk *supblk,FILE* disk) {
	//��ʼ���û�,����root�û�
	supblk->users[0].uid = 0;
	supblk->users[0].u_mode = ADMIN;
	strcat(supblk->users[0].u_name, "root");
	strcat(supblk->users[0].u_pwd, "123456");
	//FAT���ʼ��, ǰ18������ϵͳ�ļ��������ǿ��п�
	for (int i = 0; i < FATSIZE; i++) {
		if (i < SYSCLUSTERSIZE) {
			supblk->FAT[i] = -1;
		}
		else {
			supblk->FAT[i] = 0;
		}
	}
	//��ʼ��һЩ��������
	supblk->disk_size = 524288;
	supblk->free_blk = 493;
	supblk->free_disk = 504832;
	supblk->inode_count = 1;
	supblk->inode_free = 511;
	//��ʼ������inodeλͼ
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//������Ŀ¼
	inode* root = getInode(supblk, 0, 0, DIR);
	file rot;
	rot.f_ino = root->i_ino;
	strcpy(rot.f_name, "..");
	//��ʼ����Ŀ¼
	supblk->root_ino = root->i_ino;
	//����Ŀ¼inode����д�����
	fseek(disk, CLUSTERSIZE * 2, SEEK_SET);
	fwrite(root, sizeof(inode), 1, disk);
	//���������Ķ���Ŀ¼,������Ŀ¼д�뵽��Ŀ¼�ĵ�һ����Ŀ��
	file dir_buf[3] = {};
	strcpy(dir_buf[0].f_name, "..");
	dir_buf[0].f_ino = 1;
	strcpy(dir_buf[1].f_name, ".");
	dir_buf[1].f_ino = 1;
	strcpy(dir_buf[2].f_name, "etc");
	dir_buf[2].f_ino = 2;
	inode* dots = getInode(supblk, 0, 0, DIR);
	inode* etc = getInode(supblk, 0, 0, DIR);
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + dots->i_ino * INODESIZE, SEEK_SET);
	fwrite(dots, sizeof(inode), 1, disk);
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + dots->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&rot, sizeof(file), 1, disk);
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + etc->i_ino * INODESIZE, SEEK_SET);
	fwrite(etc, sizeof(inode), 1, disk);
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + etc->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&rot, sizeof(file), 1, disk);
	
	free(etc);
	free(dots);
	//������Ŀ¼д�뵽��Ŀ¼��
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + root->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&dir_buf, sizeof(file), 3, disk);
	free(root);
	//��ʼ�����
	supblk->sys_Status = true;
}
//ϵͳ����
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** fls) {
	//�򿪴���
	*disk = fopen("disk", "r+");
	if (!disk) {
		printf("���̴�ʧ��");
		exit(0);
	}
	//��ȡ������
	*supblk = (superBlk*)malloc(sizeof(superBlk));
	if (!(*supblk)) {
		printf("�����鴴��ʧ��");
		exit(1);
	}
	fread(*supblk, sizeof(superBlk), 1, *disk);
	//����ǵ�һ������ϵͳ���ʼ��ϵͳ
	if (!(*supblk)->sys_Status) {
		InitSys(*supblk, *disk);
	}
	//����ǰĿ¼��Ϊ��Ŀ¼
	*curPath = (inode*)malloc(sizeof(inode));
	if (!(*curPath)) {
		printf("�ڴ����ʧ��");
		exit(0);
	}
	fseek(*disk, (*supblk)->root_ino * INODESIZE + SUPERBLKSIZE * CLUSTERSIZE, SEEK_SET);
	fread(*curPath, sizeof(inode), 1, *disk);
	//��ȡ��Ŀ¼���ļ���Ϣ
	*fls = (Files*)malloc(sizeof(Files));
	if (!(*fls)) {
		printf("���ļ��б���ʧ��");
		exit(0);
	}
	unsigned short* FATList = getFATList(*supblk, *curPath);
	int j = 0;
	for (int i = 0; i <(*curPath)->i_blkCnt; i++) {
		fseek(*disk, SYSCLUSTERSIZE * CLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fread(&(*fls)->file[j], sizeof(file), CLUSTERSIZE / sizeof(file), *disk);
		j += CLUSTERSIZE / sizeof(file);
	}
}