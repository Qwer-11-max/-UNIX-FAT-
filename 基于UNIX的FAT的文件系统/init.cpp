#include	"filesys.h"
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
			int row = i / 16;
			int col = i & 16;
			SetOne(supblk->bitmap[row], col);
			supblk->FAT[i] = -1;	//��ʾ�ļ���ռ���ҵ�ǰλ��Ϊ�ļ������һ��
			break;
		}
	}
	//�������inode���
	for (int i = 0; i < BITMAPSIZE; i++) {
		if (supblk->bitmap_inode[i] ^ 0b11111111) {
			unsigned short free = supblk->bitmap_inode[i] & 0b11111111;
			int loc = 0;
			while (free ^ 1) {
				free = free >> 1;
				loc++;
			}
			SetOne(supblk->bitmap_inode[i], loc);
			temp->i_ino = i * 16 + loc;
			break;
		}
	}
	return temp;
}
//�����ļ�
void createFile(char * filename,type_t type,uid_t uid,gid_t gid){
}
//����Ŀ¼
void mkdir(char* dirname) {

}
//��ʼ���ļ�ϵͳ
void InitSys(superBlk *supblk) {
	//��ʼ���û�
	supblk->users[0].uid = 0;
	supblk->users[0].u_mode = ADMIN;
	strcat(supblk->users[0].u_name, "root");
	strcat(supblk->users[0].u_pwd, "123456");
	//FAT���ʼ��, ǰʮ�Ÿ�����ϵͳ�ļ��������ǿ��п�
	for (int i = 0; i < FATSIZE; i++) {
		if (i < 19) {
			supblk->FAT[i] = -1;
		}
		else {
			supblk->FAT[i] = 0;
		}
	}
	//�������ʼ��
	supblk->disk_size = 524288;
	supblk->free_blk = 493;
	supblk->free_disk = 504832;
	supblk->inode_count = 1;
	supblk->inode_free = 511;
	//��ʼ�����п�λͼ��ǰ19���鱻ϵͳռ��
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap[i] = 0;
	}
	supblk->bitmap[0] = 0xff;
	supblk->bitmap[1] = 0x07;
	//��ʼ������inodeλͼ
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//��ʼ����Ŀ¼
	supblk->root = NULL;
	//��ʼ��inode����
	//��ʼ�����
	supblk->sys_Status = true;
}