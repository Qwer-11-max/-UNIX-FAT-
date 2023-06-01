/*作者：赵熙龙*/
#include"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
//将一个二进制数的loc位置为1
#define SetOne(x,loc) x = ( x | 1<<loc)

//为文件分配额外的空间
void allocBlk(superBlk* supblk, inode* target, unsigned short size) {
	unsigned short blkSize = ceil( float(size) / CLUSTERSIZE); //计算要分配的簇数
	if (supblk->free_blk < blkSize) {
		printf("磁盘空间不足");
		return;
	}
	supblk->free_blk -= blkSize;
	supblk->free_disk -= blkSize * CLUSTERSIZE;
	int lastloc = target->i_blkCnt - 1;  //记录当前文件的簇数
	target->i_blkCnt += blkSize; // 更新后的簇数
	target->i_size = target->i_blkCnt * CLUSTERSIZE;//更新后的文件大小
	unsigned short* FATList = getFATList(supblk, target); //获取当前文件的簇序列
	//分配空闲簇
	int last = 0;
	for (int i = 0; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			if (supblk->FAT[FATList[lastloc]] == 0xffff) {
				supblk->FAT[FATList[lastloc]] = i;
				last = i;
			}
			else {
				supblk->FAT[last] = i;
				last = i;
			}
			blkSize -= 1;
		}
		if (blkSize == 0) {
			supblk->FAT[last] = 0xffff;
			printf("空间分配完毕");
			break;
		}
	}
	free(FATList);
}
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
	temp->i_size = CLUSTERSIZE;	//文件大小
	temp->i_blkCnt = 1;	//文件块计数
	time(&temp->i_mtime);	//文件修改时间
	temp->i_atime = temp->i_mtime;	//文件修改时间
	//分配空闲块
	for (int i = 1; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			temp->i_addr = i;
			supblk->FAT[i] = 0xffff;	//表示文件被占用且当前位置为文件的最后一块
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
//找到文件占用的块序列
unsigned short* getFATList(superBlk * supblk,inode* target) {
	unsigned short* FATList = (unsigned short*)malloc(sizeof(unsigned short) * target->i_blkCnt);
	if (!FATList) {
		printf("内存分配错误");
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
// 目录跳转
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno) {
	//保存旧目录信息
	int curPoint = 0;
	unsigned short data[CLUSTERSIZE / sizeof(unsigned)] = {0};
	//更新当前目录的inode节点信息
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + curPath->i_ino * INODESIZE, SEEK_SET);
	fwrite(curPath, sizeof(inode), 1, disk);
	//将磁盘中的旧目录数据抹除
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE,SEEK_SET);
	fwrite(data, sizeof(unsigned short), CLUSTERSIZE / sizeof(unsigned), disk);
	//写入旧的目录信息
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	for (int i = 0; i < SUBFILENUM; i++) {
		if (fls->file[i].f_name[0] !='\0') {
			fwrite(&fls->file[i], sizeof(file), 1, disk);
			fseek(disk,sizeof(file), SEEK_CUR);
		}
	}
	//读取新目录信息
	//1.读取inode
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + nextDirIno * INODESIZE, SEEK_SET);
	fread(curPath, sizeof(inode), 1, disk);
	//2.读取子文件
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	fread(fls, sizeof(Files), 1, disk);
	return 0;
}
//遍历目录子文件列表，返回fls中文件的位置，如果没有则返回-1即0xFFFF
unsigned short getIno(Files* fls,FILE* disk, char* filename,type_t type) {
	for (int i = 0; i < SUBFILENUM; i++) {
		//判断文件名是否一致
		if (!strcmp(filename,fls->file[i].f_name)) {
			return i;
		}
	}
	//不存在这个文件则返回-1
	return 0xffff;
}
//释放inode节点
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino) {
	//读取inode信息
	inode target = {NULL};
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fread(&target, sizeof(inode), 1, disk);
	//判断inode是否存在
	if (target.i_size == 0) {
		printf("要释放的inode节点不存在");
		return 0;
	}
	//释放占用的数据块
	unsigned short data[CLUSTERSIZE / sizeof(unsigned short)] = {0};
	unsigned short* FATList = getFATList(supblk, &target);
	for (int i = 0; i < target.i_blkCnt; i++) {
		supblk->FAT[FATList[i]] = 0;
		fseek(disk, CLUSTERSIZE * SYSCLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fwrite(data,sizeof(unsigned short),CLUSTERSIZE / sizeof(unsigned short),disk);
	}
	free(FATList);
	//释放占用的inode块
	unsigned short data1[INODESIZE / sizeof(unsigned short)] = { 0 };
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fwrite(data1, sizeof(unsigned short), INODESIZE / sizeof(unsigned), disk);

	//更新超级块信息
	supblk->free_blk += target.i_blkCnt;
	supblk->free_disk += target.i_size;
	supblk->inode_free += 1;
	supblk->inode_count -= 1;
	
	int row = ino / 16; // 重置inode位图的信息
	int loc = ino % 16;
	supblk->bitmap_inode[row] -= 1 << loc;
	//inode节点释放完毕
	return 0;
}
//初始化文件系统
void InitSys(superBlk *supblk,FILE* disk) {
	//初始化用户,创建root用户
	supblk->users[0].uid = 1;
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
	supblk->inode_count = 0;
	supblk->inode_free = 512;
	//初始化空闲inode位图
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//创建根目录
	inode* root = getInode(supblk, 0, 0, DIR);
	//初始化根目录
	supblk->root_ino = root->i_ino;
	//将根目录inode数据写入磁盘
	fseek(disk, CLUSTERSIZE * 2 + root->i_ino*INODESIZE, SEEK_SET);
	fwrite(root, sizeof(inode), 1, disk);
	//创建基础的二级目录,并将父目录写入到子目录的第一个条目中
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
//系统开机
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** fls,Files** path) {
	//打开磁盘
	*disk = fopen("disk", "r+");
	if (!disk) {
		printf("磁盘打开失败");
		exit(0);
	}
	//读取超级块
	*supblk = (superBlk*)malloc(sizeof(superBlk));
	if (!(*supblk)) {
		printf("超级块创建失败");
		exit(1);
	}
	fread(*supblk, sizeof(superBlk), 1, *disk);
	//如果是第一次启动系统则初始化系统
	if (!(*supblk)->sys_Status) {
		InitSys(*supblk, *disk);
	}
	//将当前目录置为根目录
	*curPath = (inode*)malloc(sizeof(inode));
	if (!(*curPath)) {
		printf("内存分配失败");
		exit(0);
	}
	fseek(*disk, (*supblk)->root_ino * INODESIZE + SUPERBLKSIZE * CLUSTERSIZE, SEEK_SET);
	fread(*curPath, sizeof(inode), 1, *disk);
	//设置路径信息
	*path = (Files*)malloc(sizeof(Files));
	if (!(*path)) {
		printf("路径文件创建失败");
		exit(0);
	}
	(*path)->file[0].f_ino = 0;
	(*path)->size = 1;
	strcpy((*path)->file[0].f_name, "..");
	//读取根目录子文件信息
	*fls = (Files*)malloc(sizeof(Files));
	if (!(*fls)) {
		printf("子文件列表创建失败");
		exit(0);
	}
	unsigned short* FATList = getFATList(*supblk, *curPath);
	int j = 0;
	for (int i = 0; i <(*curPath)->i_blkCnt; i++) {
		fseek(*disk, SYSCLUSTERSIZE * CLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fread(*fls, sizeof(Files),1, *disk);
	}
	free(FATList);
}
//系统停机
void halt(superBlk* supblk,FILE* disk,inode* curPath,Files* fls) {
	fseek(disk, 0, SEEK_SET);
	//保存超级块信息
	fwrite(supblk,sizeof(superBlk),1,disk);
	//保存当前目录信息
	//1.保存inode
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + curPath->i_ino * INODESIZE, SEEK_SET);
	fwrite(curPath, sizeof(inode), 1, disk);
	//2.保存子目录
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(fls, sizeof(Files), 1, disk);
	printf("系统关机成功\n");
}

//主界面
void mainWindows(superBlk* supblk, FILE* disk, inode* curPath, User* curUser, Files* fls,Files* path,Openqueue* queue) {
	//printf("系统剩余资源资源一览:\n");
	//printf("================================================================================================\n");
	//printf("| 磁盘大小：%10d Byte\t| 磁盘剩余空间: %10d Byte\t| 磁盘剩余inode数量：%10d|\n", supblk->disk_size, supblk->free_disk, supblk->inode_free);
	//printf("| 磁盘已用inode数量：%10d\t| 剩余空闲簇: %10d\t\n", supblk->inode_count, supblk->free_blk);
	//printf("================================================================================================\n");
	//printf("功能列表:\n");
	//printf("1.创建文件\n2.删除文件\n3.跳转目录\n4.创建目录\n5.列出当前目录子文件列表\n");
	printf("<%s@%06d>  C:",curUser->u_name,curUser->uid);
	for (int i = 0; i < path->size; i++) {
		printf("\\%s", path->file[i].f_name);
	}
	printf(">");
	char instr[20];
	scanf("%s",instr);

	if(!strcmp(instr,"cd")){
		chdir(supblk, disk, curPath, fls, path);
	}
	else if (!strcmp(instr, "mkdir")) {
		mkdir(supblk, disk, fls);
	}
	else if (!strcmp(instr, "touch")) {
		creatFile(supblk, disk, fls, curUser->uid, 0);
	}
	else if (!strcmp(instr, "ls")) {
		Ls(fls);
	}
	else if (!strcmp(instr, "cls")) {
		system("cls");
	}
	else if (!(strcmp(instr, "exit"))) {
		halt(supblk,disk,curPath,fls);
		exit(0);
	}
	else if (!strcmp(instr, "del")) {
		deleteFile(supblk, disk, fls);
	}
	else if (!strcmp(instr, "open")) {
		OpenFile(fls, disk, queue);
	}
	else if (!strcmp(instr, "close")) {
		CloseFile(queue);
	}
	else if (!strcmp(instr, "createUser")) {
		create_user(supblk);
	}
	else {
		printf("不存在该指令:%s\n", instr);
	}

	fflush(stdin);
}