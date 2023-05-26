#include	"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//��һ������������locλ��Ϊ1
#define SetOne(x,loc) x = ( x | 1<<loc)

//����inode�ڵ�
inode *getInode(uid_t uid, gid_t gid,type_t type) {
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
		if (FAT1[i] == 0) {
			temp->i_addr = i;
			int row = i / 16;
			int col = i & 16;
			SetOne(superblk.bitmap[row], col);
			FAT1[i] = -1;	//��ʾ�ļ���ռ���ҵ�ǰλ��Ϊ�ļ������һ��
			break;
		}
	}
	//�������inode���
	for (int i = 0; i < BITMAPSIZE; i++) {
		if (superblk.bitmap_inode[i] ^ 0b11111111) {
			unsigned short free = superblk.bitmap_inode[i] & 0b11111111;
			int loc = 0;
			while (free ^ 1) {
				free = free >> 1;
				loc++;
			}
			SetOne(superblk.bitmap_inode[i], loc);
			temp->i_ino = i * 16 + loc;
			break;
		}
	}
}
//�����ļ�
void createFile(char * filename,type_t type,uid_t uid,gid_t gid){

}
//��ʼ���ļ�ϵͳ
void InitSys() {
	//�򿪴���
	disk = fopen("disk.txt", "w+r");
	//��ʼ���û�
	
	//FAT���ʼ��, ǰʮ�Ÿ�����ϵͳ�ļ��������ǿ��п�
	for (int i = 0; i < FATSIZE; i++) {
		if (i < 19) {
			FAT1[i] = -1;
			FAT2[i] = -1;
		}
		else {
			FAT1[i] = 0;
			FAT2[i] = 0;
		}
	}
	//�������ʼ��
	superblk.disk_size = 524288;
	superblk.free_blk = 493;
	superblk.free_disk = 504832;
	superblk.inode_count = 1;
	superblk.inode_free = 511;
	//��ʼ�����п�λͼ��ǰ19���鱻ϵͳռ��
	for (int i = 0; i < BITMAPSIZE; i++) {
		superblk.bitmap[i] = 0;
	}
	superblk.bitmap[0] = 0xff;
	superblk.bitmap[1] = 0x07;
	//��ʼ������inodeλͼ
	for (int i = 0; i < BITMAPSIZE; i++) {
		superblk.bitmap_inode[i] = 0;
	}
	//��ʼ����Ŀ¼
	superblk.root = getInode(0,0,DIR);
	fseek(disk, superblk.root->i_addr * CLUSTERSIZE, SEEK_SET);
	fputs("..",disk);
	//��ʼ��inode����
	i_head.i_forwd = NULL;
	i_head.i_bak = NULL;
	i_head.inode = superblk.root;
	i_cur = i_head;
	//��ʼ�����
	superblk.sys_Status = true;
}