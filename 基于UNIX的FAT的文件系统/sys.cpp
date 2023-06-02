/*���ߣ�������*/
#include"filesys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
//��һ������������locλ��Ϊ1
#define SetOne(x,loc) x = ( x | 1<<loc)

//Ϊ�ļ��������Ŀռ�
void allocBlk(superBlk* supblk, inode* target, unsigned short size) {
	unsigned short blkSize = ceil( float(size) / CLUSTERSIZE); //����Ҫ����Ĵ���
	if (supblk->free_blk < blkSize) {
		printf("���̿ռ䲻��");
		return;
	}
	supblk->free_blk -= blkSize;
	supblk->free_disk -= blkSize * CLUSTERSIZE;
	int lastloc = target->i_blkCnt - 1;  //��¼��ǰ�ļ��Ĵ���
	target->i_blkCnt += blkSize; // ���º�Ĵ���
	target->i_size = target->i_blkCnt * CLUSTERSIZE;//���º���ļ���С
	unsigned short* FATList = getFATList(supblk, target); //��ȡ��ǰ�ļ��Ĵ�����
	//������д�
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
			printf("�ռ�������");
			break;
		}
	}
	free(FATList);
}
//����inode�ڵ�
inode *getInode(superBlk *supblk,FILE* disk,uid_t uid, gid_t gid,type_t type) {
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
	temp->i_size = CLUSTERSIZE;	//�ļ���С
	temp->i_blkCnt = 1;	//�ļ������
	time(&temp->i_mtime);	//�ļ��޸�ʱ��
	temp->i_atime = temp->i_mtime;	//�ļ��޸�ʱ��
	//������п�
	for (int i = 1; i < FATSIZE; i++) {
		if (supblk->FAT[i] == 0) {
			temp->i_addr = i;
			supblk->FAT[i] = 0xffff;	//��ʾ�ļ���ռ���ҵ�ǰλ��Ϊ�ļ������һ��
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

	//��inode��Ϣ���浽������
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + temp->i_ino * INODESIZE, SEEK_SET);
	fwrite(temp, sizeof(inode), 1, disk);
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
		FATList[i] = addr;
		addr = supblk->FAT[addr];
	}
	return FATList;
}
// Ŀ¼��ת
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno) {
	//�����Ŀ¼��Ϣ
	//1.�����Ŀ¼inode��Ϣ
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + curPath->i_ino * INODESIZE, SEEK_SET);
	fwrite(curPath, sizeof(inode), 1, disk);
	//2.�������еľ�Ŀ¼����Ĩ��
	unsigned short data[CLUSTERSIZE / sizeof(unsigned)] = { 0 };
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE,SEEK_SET);
	fwrite(data, sizeof(unsigned short), CLUSTERSIZE / sizeof(unsigned), disk);
	//3.д�뵱ǰĿ¼��Ŀ¼��Ϣ
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	for (int i = 0; i < SUBFILENUM; i++) {
		if (fls->file[i].f_name[0] !='\0') {
			fwrite(&fls->file[i], sizeof(file), 1, disk);
		}
	}
	//��ȡ��Ŀ¼��Ϣ
	//1.��ȡinode
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + nextDirIno * INODESIZE, SEEK_SET);
	fread(curPath, sizeof(inode), 1, disk);
	//2.��ȡ���ļ��б�
	fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + curPath->i_addr * CLUSTERSIZE, SEEK_SET);
	fread(fls, sizeof(Files), 1, disk);
	return 0;
}
//����Ŀ¼���ļ��б�����fls���ļ���λ�ã����û���򷵻�-1��0xFFFF
unsigned short getIno(Files* fls,FILE* disk, char* filename,type_t type) {
	for (int i = 0; i < SUBFILENUM; i++) {
		//�ж��ļ����Ƿ�һ��
		if (!strcmp(filename,fls->file[i].f_name)) {
			return i;
		}
	}
	//����������ļ��򷵻�-1
	return 0xffff;
}
//�ͷ�inode�ڵ�
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino) {
	//��ȡinode��Ϣ
	inode target = {NULL};
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fread(&target, sizeof(inode), 1, disk);
	//�ж�inode�Ƿ����
	if (target.i_size == 0) {
		printf("Ҫ�ͷŵ�inode�ڵ㲻����");
		return 0;
	}
	//�ͷ�ռ�õ����ݿ�
	unsigned short data[CLUSTERSIZE / sizeof(unsigned short)] = {0};
	unsigned short* FATList = getFATList(supblk, &target);
	for (int i = 0; i < target.i_blkCnt; i++) {
		supblk->FAT[FATList[i]] = 0;
		fseek(disk, CLUSTERSIZE * SYSCLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fwrite(data,sizeof(unsigned short),CLUSTERSIZE / sizeof(unsigned short),disk);
	}
	free(FATList);
	//�ͷ�ռ�õ�inode��
	unsigned short data1[INODESIZE / sizeof(unsigned short)] = { 0 };
	fseek(disk, CLUSTERSIZE * SUPERBLKSIZE + ino * INODESIZE, SEEK_SET);
	fwrite(data1, sizeof(unsigned short), INODESIZE / sizeof(unsigned), disk);

	//���³�������Ϣ
	supblk->free_blk += target.i_blkCnt;
	supblk->free_disk += target.i_size;
	supblk->inode_free += 1;
	supblk->inode_count -= 1;
	
	int row = ino / 16; // ����inodeλͼ����Ϣ
	int loc = ino % 16;
	supblk->bitmap_inode[row] -= 1 << loc;
	//inode�ڵ��ͷ����
	return 0;
}
//��ʼ���ļ�ϵͳ
void InitSys(superBlk *supblk,FILE* disk) {
	//��ʼ���û�,����root�û�
	supblk->users[0].uid = 1;
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
	supblk->inode_count = 0;
	supblk->inode_free = 512;
	//��ʼ������inodeλͼ
	for (int i = 0; i < BITMAPSIZE; i++) {
		supblk->bitmap_inode[i] = 0;
	}
	//������Ŀ¼
	inode* root = getInode(supblk,disk, 0, 0, DIR);
	//��ʼ����Ŀ¼
	supblk->root_ino = root->i_ino;
	//����Ŀ¼inode����д�����
	fseek(disk, CLUSTERSIZE * 2 + root->i_ino*INODESIZE, SEEK_SET);
	fwrite(root, sizeof(inode), 1, disk);
	//���������Ķ���Ŀ¼,������Ŀ¼д�뵽��Ŀ¼�ĵ�һ����Ŀ��
	file dir_buf[3] = {};
	strcpy(dir_buf[0].f_name, "..");
	dir_buf[0].f_ino = 1;
	strcpy(dir_buf[1].f_name, ".");
	dir_buf[1].f_ino = 1;
	strcpy(dir_buf[2].f_name, "etc");
	dir_buf[2].f_ino = 2;
	inode* dots = getInode(supblk,disk, 0, 0, DIR);
	inode* etc = getInode(supblk,disk, 0, 0, DIR);
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + dots->i_ino * INODESIZE, SEEK_SET);
	fwrite(dots, sizeof(inode), 1, disk);
	fseek(disk, SUPERBLKSIZE * CLUSTERSIZE + etc->i_ino * INODESIZE, SEEK_SET);
	fwrite(etc, sizeof(inode), 1, disk);
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
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** fls,Files** path) {
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
	//����·����Ϣ
	*path = (Files*)malloc(sizeof(Files));
	if (!(*path)) {
		printf("·���ļ�����ʧ��");
		exit(0);
	}
	(*path)->file[0].f_ino = 0;
	(*path)->size = 1;
	strcpy((*path)->file[0].f_name, "..");
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
		fread(*fls, sizeof(Files),1, *disk);
	}
	free(FATList);
}
//ϵͳͣ��
void halt(superBlk* supblk,FILE* disk,inode* curPath,Files* fls) {
	fseek(disk, 0, SEEK_SET);
	//���泬������Ϣ
	fwrite(supblk,sizeof(superBlk),1,disk);
	//���浱ǰĿ¼��Ϣ
	setCurPath(supblk, disk, curPath, fls, curPath->i_ino);
	printf("ϵͳ�ػ��ɹ�\n");
}

//������
bool mainWindows(superBlk* supblk, FILE* disk, inode* curPath, User* curUser, Files* fls,Files* path,Openqueue* queue) {
	//printf("ϵͳʣ����Դ��Դһ��:\n");
	//printf("================================================================================================\n");
	//printf("| ���̴�С��%10d Byte\t| ����ʣ��ռ�: %10d Byte\t| ����ʣ��inode������%10d|\n", supblk->disk_size, supblk->free_disk, supblk->inode_free);
	//printf("| ��������inode������%10d\t| ʣ����д�: %10d\t\n", supblk->inode_count, supblk->free_blk);
	//printf("================================================================================================\n");
	//printf("�����б�:\n");
	//printf("1.�����ļ�\n2.ɾ���ļ�\n3.��תĿ¼\n4.����Ŀ¼\n5.�г���ǰĿ¼���ļ��б�\n");
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
	else if (!strcmp(instr, "write")) {
		writeFile(supblk, disk, queue);
	}
	else if (!strcmp(instr, "logout")) {
		return false;
	}
	else if (!strcmp(instr, "rmdir")) {
		rmdir(supblk, disk, fls);
	}
	else if (!strcmp(instr, "read")) {
		readFile(supblk, disk, queue);
	}
	else if (!strcmp(instr, "format")) {
		fseek(disk, 0, SEEK_SET);
		unsigned short data[256 * 1024] = { 0 };
		fwrite(data, sizeof(unsigned short), 256 * 1024, disk);
		fclose(disk);
		exit(0);
	}
	else {
		printf("�����ڸ�ָ��:%s\n", instr);
	}

	fflush(stdin);
	return true;
}