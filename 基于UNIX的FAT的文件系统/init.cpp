#include	"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//将一个二进制数的loc位置为1
#define SetOne(x,loc) x = ( x | 1<<loc)

//分配inode节点
inode *getInode(superBlk *supblk,uid_t uid, gid_t gid,type_t type) {
	inode* temp = (inode*)malloc(sizeof(inode));
	if (!temp) {
		printf("创建新的inode节点失败\n");
		exit(1);
	}
	//配置初始属性
	temp->i_uid = uid;	//用户uid
	temp->i_gid = gid;	//组id
	temp->i_acl = 0;	//文件权限
	temp->i_type = type;	//文件类型
	temp->i_size = 1;	//文件大小
	temp->i_blkCnt = 1;	//文件块计数
	time(&temp->i_mtime);	//文件修改时间
	temp->i_atime = temp->i_mtime;	//文件修改时间
	//分配空闲块
	for (int i = 1; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			temp->i_addr = i;
			int row = i / 16;
			int col = i & 16;
			SetOne(supblk->bitmap[row], col);
			supblk->FAT[i] = -1;	//表示文件被占用且当前位置为文件的最后一块
			break;
		}
	}
	//分配空闲inode序号
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
//创建文件
void createFile(char * filename,type_t type,uid_t uid,gid_t gid){
}
//创建目录
void mkdir(char* dirname) {

}
//初始化文件系统
void InitSys(superBlk *supblk) {
	//初始化用户
	supblk->users[0].uid = 0;
	supblk->users[0].u_mode = ADMIN;
	strcat(supblk->users[0].u_name, "root");
	strcat(supblk->users[0].u_pwd, "123456");
	//FAT表初始化, 前十九个用于系统文件，其余是空闲块
	for (int i = 0; i < FATSIZE; i++) {
		if (i < 19) {
			supblk->FAT[i] = -1;
		}
		else {
			supblk->FAT[i] = 0;
		}
	}
	//超级块初始化
	supblk->disk_size = 524288;
	supblk->free_blk = 493;
	supblk->free_disk = 504832;
	supblk->inode_count = 1;
	supblk->inode_free = 511;
	//初始化空闲块位图，前19个块被系统占用
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap[i] = 0;
	}
	supblk->bitmap[0] = 0xff;
	supblk->bitmap[1] = 0x07;
	//初始化空闲inode位图
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//初始化根目录
	supblk->root = NULL;
	//初始化inode链表
	//初始化完毕
	supblk->sys_Status = true;
}