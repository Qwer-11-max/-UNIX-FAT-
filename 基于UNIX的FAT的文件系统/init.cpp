#include	"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//将一个二进制数的loc位置为1
#define SetOne(x,loc) x = ( x | 1<<loc)

//分配inode节点
inode *getInode(uid_t uid, gid_t gid,type_t type) {
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
		if (FAT1[i] == 0) {
			temp->i_addr = i;
			int row = i / 16;
			int col = i & 16;
			SetOne(superblk.bitmap[row], col);
			FAT1[i] = -1;	//表示文件被占用且当前位置为文件的最后一块
			break;
		}
	}
	//分配空闲inode序号
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
//创建文件
void createFile(char * filename,type_t type,uid_t uid,gid_t gid){
	fseek(disk, cur_dir->i_addr * CLUSTERSIZE, SEEK_SET);
}
//创建目录
void mkdir(char* dirname) {

}
//初始化文件系统
void InitSys() {
	//初始化用户
	
	//FAT表初始化, 前十九个用于系统文件，其余是空闲块
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
	//超级块初始化
	superblk.disk_size = 524288;
	superblk.free_blk = 493;
	superblk.free_disk = 504832;
	superblk.inode_count = 1;
	superblk.inode_free = 511;
	//初始化空闲块位图，前19个块被系统占用
	for (int i = 0; i < BITMAPSIZE; i++) {
		superblk.bitmap[i] = 0;
	}
	superblk.bitmap[0] = 0xff;
	superblk.bitmap[1] = 0x07;
	//初始化空闲inode位图
	for (int i = 0; i < BITMAPSIZE; i++) {
		superblk.bitmap_inode[i] = 0;
	}
	//初始化根目录
	superblk.root = getInode(0,0,DIR);
	fseek(disk, superblk.root->i_addr * CLUSTERSIZE, SEEK_SET);
	fputs("..",disk);
	//初始化inode链表
	i_head.i_forwd = NULL;
	i_head.i_bak = NULL;
	i_head.inode = superblk.root;
	cur_dir = superblk.root;
	//初始化完毕
	superblk.sys_Status = true;
}