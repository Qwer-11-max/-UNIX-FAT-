#include"filesys.h"
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
			int col = i % 16;
			SetOne(supblk->bitmap[row], col);
			supblk->FAT[i] = -1;	//表示文件被占用且当前位置为文件的最后一块
			supblk->free_blk -= 1; //空闲块减一
			supblk->free_disk -= CLUSTERSIZE; //空闲空间减一个簇大小
			break;
		}
	}
	//分配空闲inode序号
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
//创建文件
void createFile(superBlk* supblk,char * filename,type_t type,uid_t uid,gid_t gid){

}
//创建目录
void mkdir(superBlk* supblk,FILE* disk,char* dirname,uid_t uid,gid_t gid,unsigned short prnt_ino) {
	//存储目录信息
	file temp;
	//文件名
	strcat(temp.f_name, dirname);
	//为新目录申请inode
	inode* dirInode = getInode(supblk, uid, gid, DIR);
	//记录目录的ino
	temp.f_ino = dirInode->i_ino;
	
	//找到父目录的inode并读取
	inode* prnt_inode = (inode*)malloc(sizeof(inode));
	if (!prnt_inode) {
		printf("索引节点分配失败");
		exit(0);
	}
	//找到父目录的inode并读取
	fseek(disk,SUPERBLKSIZE*CLUSTERSIZE+prnt_ino*INODESIZE,SEEK_SET);
	fread(prnt_inode, sizeof(inode), 1, disk);

	//找到父目录的数据块所在地并将目录信息写入
	fseek(disk, prnt_inode->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&temp, sizeof(file), 1, disk);

	//释放无用指针
	free(dirInode);
	free(prnt_inode);
}
//初始化文件系统
void InitSys(superBlk *supblk,FILE* disk) {
	//初始化用户,创建root用户
	supblk->users[0].uid = 0;
	supblk->users[0].u_mode = ADMIN;
	strcat(supblk->users[0].u_name, "root");
	strcat(supblk->users[0].u_pwd, "123456");
	//FAT表初始化, 前18个用于系统文件，其余是空闲块
	for (int i = 0; i < FATSIZE; i++) {
		if (i < SYSCLUSTERSIZE) {
			supblk->FAT[i] = -1;
		}
		else {
			supblk->FAT[i] = 0;
		}
	}
	//初始化一些基本数据
	supblk->disk_size = 524288;
	supblk->free_blk = 493;
	supblk->free_disk = 504832;
	supblk->inode_count = 1;
	supblk->inode_free = 511;
	//初始化空闲块位图，前18个块被系统占用
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap[i] = 0;
	}
	supblk->bitmap[0] = 0xffff;
	supblk->bitmap[1] = 0x03;
	//初始化空闲inode位图
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//创建根目录
	inode* root = getInode(supblk, 0, 0, DIR);
	//初始化根目录
	supblk->root_ino = root->i_ino;
	//将根目录inode数据写入磁盘
	fseek(disk, CLUSTERSIZE * 2, SEEK_SET);
	fwrite(root, sizeof(inode), 1, disk);
	//创建基础的二级目录
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
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + etc->i_ino * INODESIZE, SEEK_SET);
	fwrite(etc, sizeof(inode), 1, disk);
	free(etc);
	free(dots);
	//将二级目录写入到根目录下
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + root->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&dir_buf, sizeof(file), 3, disk);
	free(root);
	//初始化完毕
	supblk->sys_Status = true;
}