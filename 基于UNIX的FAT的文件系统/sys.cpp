#include"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

//½«Ò»¸ö¶ş½øÖÆÊıµÄlocÎ»ÖÃÎª1
#define SetOne(x,loc) x = ( x | 1<<loc)

//·ÖÅäinode½Úµã
inode *getInode(superBlk *supblk,uid_t uid, gid_t gid,type_t type) {
	inode* temp = (inode*)malloc(sizeof(inode));
	if (!temp) {
		printf("´´½¨ĞÂµÄinode½ÚµãÊ§°Ü\n");
		exit(1);
	}
	//ÅäÖÃ³õÊ¼ÊôĞÔ
	temp->i_uid = uid;	//ÓÃ»§uid
	temp->i_gid = gid;	//×éid
	temp->i_acl = 0;	//ÎÄ¼şÈ¨ÏŞ
	temp->i_type = type;	//ÎÄ¼şÀàĞÍ
	temp->i_size = 1;	//ÎÄ¼ş´óĞ¡
	temp->i_blkCnt = 1;	//ÎÄ¼ş¿é¼ÆÊı
	time(&temp->i_mtime);	//ÎÄ¼şĞŞ¸ÄÊ±¼ä
	temp->i_atime = temp->i_mtime;	//ÎÄ¼şĞŞ¸ÄÊ±¼ä
	//·ÖÅä¿ÕÏĞ¿é
	for (int i = 1; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			temp->i_addr = i;
			supblk->FAT[i] = -1;	//±íÊ¾ÎÄ¼ş±»Õ¼ÓÃÇÒµ±Ç°Î»ÖÃÎªÎÄ¼şµÄ×îºóÒ»¿é
			supblk->free_blk -= 1; //¿ÕÏĞ¿é¼õÒ»
			supblk->free_disk -= CLUSTERSIZE; //¿ÕÏĞ¿Õ¼ä¼õÒ»¸ö´Ø´óĞ¡
			break;
		}
	}
	//·ÖÅä¿ÕÏĞinodeĞòºÅ
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
//ÕÒµ½ÎÄ¼şÕ¼ÓÃµÄ¿éĞòÁĞ
unsigned short* getFATList(superBlk * supblk,inode* target) {
	unsigned short* FATList = (unsigned short*)malloc(sizeof(unsigned short) * target->i_blkCnt);
	if (!FATList) {
		printf("ÄÚ´æ·ÖÅä´íÎó");
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
// Ä¿Â¼Ìø×ª
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno) {
	//±£´æ¾ÉÄ¿Â¼ĞÅÏ¢
	int curPoint = 0;
	unsigned short data[CLUSTERSIZE / sizeof(unsigned)] = {0};
	//¸üĞÂµ±Ç°Ä¿Â¼µÄinode½ÚµãĞÅÏ¢
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + curPath->i_ino * INODESIZE, SEEK_SET);
	fwrite(curPath, sizeof(inode), 1, disk);
	//½«´ÅÅÌÖĞµÄ¾ÉÄ¿Â¼Êı¾İÄ¨³ı
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE,SEEK_SET);
	fwrite(data, sizeof(unsigned short), CLUSTERSIZE / sizeof(unsigned), disk);
	//Ğ´Èë¾ÉµÄÄ¿Â¼ĞÅÏ¢
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	for (int i = 0; i < SUBFILENUM; i++) {
		if (fls->file[i].f_name[0] !='\0') {
			fwrite(&fls->file, sizeof(file), 1, disk);
			fseek(disk,sizeof(file), SEEK_CUR);
		}
	}
	//¶ÁÈ¡ĞÂÄ¿Â¼ĞÅÏ¢
	//1.¶ÁÈ¡inode
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + nextDirIno * INODESIZE, SEEK_SET);
	fread(curPath, sizeof(inode), 1, disk);
	//2.¶ÁÈ¡×ÓÎÄ¼ş
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	fread(fls, sizeof(Files), 1, disk);
	return 0;
}
//±éÀúÄ¿Â¼×ÓÎÄ¼şÁĞ±í£¬ÕÒµ½ÎÄ¼şÃû¶ÔÓ¦µÄino£¬Èç¹ûÃ»ÓĞÔò·µ»Ø-1¼´0xFFFF
unsigned short getIno(Files* fls,FILE* disk, char* filename,type_t type) {
	for (int i = 0; i < SUBFILENUM; i++) {
		//ÅĞ¶ÏÎÄ¼şÃûÊÇ·ñÒ»ÖÂ
		if (!strcmp(filename,fls->file[i].f_name)) {
			inode temp = {};
			fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + fls->file[i].f_ino * INODESIZE, SEEK_SET);
			fread(&temp, sizeof(inode), 1, disk);
			//ÅĞ¶ÏÎÄ¼şÀàĞÍÊÇ·ñÒ»ÖÂ
			if(temp.i_type == type)
				//·µ»Øino
				return fls->file[i].f_ino;
		}
	}
	//²»´æÔÚÕâ¸öÎÄ¼şÔò·µ»Ø-1
	return -1;
}
//ÊÍ·Åinode½Úµã
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino) {
	//¶ÁÈ¡inodeĞÅÏ¢
	inode target = {NULL};
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fread(&target, sizeof(inode), 1, disk);
	//ÅĞ¶ÏinodeÊÇ·ñ´æÔÚ
	if (target.i_size == 0) {
		printf("ÒªÊÍ·ÅµÄinode½Úµã²»´æÔÚ");
		return 0;
	}
	//ÊÍ·ÅÕ¼ÓÃµÄÊı¾İ¿é
	unsigned short data[CLUSTERSIZE / sizeof(unsigned short)] = {0};
	unsigned short* FATList = getFATList(supblk, &target);
	for (int i = 0; i < target.i_blkCnt; i++) {
		supblk->FAT[FATList[i]] = 0;
		fseek(disk, CLUSTERSIZE * SYSCLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fwrite(data,sizeof(unsigned short),CLUSTERSIZE / sizeof(unsigned short),disk);
	}
	//ÊÍ·ÅÕ¼ÓÃµÄinode¿é
	unsigned short data1[INODESIZE / sizeof(unsigned short)] = { 0 };
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fwrite(data1, sizeof(unsigned short), INODESIZE / sizeof(unsigned), disk);

	//¸üĞÂ³¬¼¶¿éĞÅÏ¢
	supblk->free_blk += target.i_blkCnt;
	supblk->free_disk += target.i_size;
	supblk->inode_free += 1;
	
	int row = ino / 16; // ÖØÖÃinodeÎ»Í¼µÄĞÅÏ¢
	int loc = ino % 16;
	SetOne(supblk->bitmap_inode[row], loc);
	//inode½ÚµãÊÍ·ÅÍê±Ï
	return 0;
}
//³õÊ¼»¯ÎÄ¼şÏµÍ³
void InitSys(superBlk *supblk,FILE* disk) {
	//³õÊ¼»¯ÓÃ»§,´´½¨rootÓÃ»§
	supblk->users[0].uid = 0;
	supblk->users[0].u_mode = ADMIN;
	strcat(supblk->users[0].u_name, "root");
	strcat(supblk->users[0].u_pwd, "123456");
	//FAT±í³õÊ¼»¯, Ç°18¸öÓÃÓÚÏµÍ³ÎÄ¼ş£¬ÆäÓàÊÇ¿ÕÏĞ¿é
	for (int i = 0; i < FATSIZE; i++) {
		if (i < SYSCLUSTERSIZE) {
			supblk->FAT[i] = -1;
		}
		else {
			supblk->FAT[i] = 0;
		}
	}
	//³õÊ¼»¯Ò»Ğ©»ù±¾Êı¾İ
	supblk->disk_size = 524288;
	supblk->free_blk = 493;
	supblk->free_disk = 504832;
	supblk->inode_count = 1;
	supblk->inode_free = 511;
	//³õÊ¼»¯¿ÕÏĞinodeÎ»Í¼
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//´´½¨¸ùÄ¿Â¼
	inode* root = getInode(supblk, 0, 0, DIR);
	file rot;
	rot.f_ino = root->i_ino;
	strcpy(rot.f_name, "..");
	//³õÊ¼»¯¸ùÄ¿Â¼
	supblk->root_ino = root->i_ino;
	//½«¸ùÄ¿Â¼inodeÊı¾İĞ´Èë´ÅÅÌ
	fseek(disk, CLUSTERSIZE * 2, SEEK_SET);
	fwrite(root, sizeof(inode), 1, disk);
	//´´½¨»ù´¡µÄ¶ş¼¶Ä¿Â¼,²¢½«¸¸Ä¿Â¼Ğ´Èëµ½×ÓÄ¿Â¼µÄµÚÒ»¸öÌõÄ¿ÖĞ
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
	//½«¶ş¼¶Ä¿Â¼Ğ´Èëµ½¸ùÄ¿Â¼ÏÂ
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + root->i_addr * CLUSTERSIZE, SEEK_SET);
	fwrite(&dir_buf, sizeof(file), 3, disk);
	free(root);
	//³õÊ¼»¯Íê±Ï
	supblk->sys_Status = true;
}
//ÏµÍ³¿ª»ú
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** fls) {
	//´ò¿ª´ÅÅÌ
	*disk = fopen("disk", "r+");
	if (!disk) {
		printf("´ÅÅÌ´ò¿ªÊ§°Ü");
		exit(0);
	}
	//¶ÁÈ¡³¬¼¶¿é
	*supblk = (superBlk*)malloc(sizeof(superBlk));
	if (!(*supblk)) {
		printf("³¬¼¶¿é´´½¨Ê§°Ü");
		exit(1);
	}
	fread(*supblk, sizeof(superBlk), 1, *disk);
	//Èç¹ûÊÇµÚÒ»´ÎÆô¶¯ÏµÍ³Ôò³õÊ¼»¯ÏµÍ³
	if (!(*supblk)->sys_Status) {
		InitSys(*supblk, *disk);
	}
	//½«µ±Ç°Ä¿Â¼ÖÃÎª¸ùÄ¿Â¼
	*curPath = (inode*)malloc(sizeof(inode));
	if (!(*curPath)) {
		printf("ÄÚ´æ·ÖÅäÊ§°Ü");
		exit(0);
	}
	fseek(*disk, (*supblk)->root_ino * INODESIZE + SUPERBLKSIZE * CLUSTERSIZE, SEEK_SET);
	fread(*curPath, sizeof(inode), 1, *disk);
	//¶ÁÈ¡¸ùÄ¿Â¼×ÓÎÄ¼şĞÅÏ¢
	*fls = (Files*)malloc(sizeof(Files));
	if (!(*fls)) {
		printf("×ÓÎÄ¼şÁĞ±í´´½¨Ê§°Ü");
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
	printf("ç³»ç»Ÿå‰©ä½™èµ„æºèµ„æºä¸€è§ˆ:\n");