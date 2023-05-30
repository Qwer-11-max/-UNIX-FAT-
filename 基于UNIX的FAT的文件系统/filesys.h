#ifndef FILESYS_H
#define FILESYS_H
#include<stdio.h>
#include<ctime>
#include<stdbool.h>
/*��������*/
#define BLOCKSIZE			256		//���С,��λ��BYTE
#define INODESIZE			32		//inode��С
#define FILENAMESIZE		14	//�ļ�����С
#define CLUSTERSIZE			(4*BLOCKSIZE)	//�ش�С   1024BYTE
#define CLUSTERNUM			512		//�ص�����
#define FATSIZE				512		//FAT��Ĵ�С
#define BITMAPSIZE			(FATSIZE/16)	//λͼ�Ĵ�С   32
#define MAXUSERS			8		//�û������޶�
#define SUBFILENUM			(CLUSTERSIZE/(FILENAMESIZE+2))		//һ��Ŀ¼���ļ���������  1024/16            64
/*���̻���*/
#define SUPERBLKSIZE		2	//������ռ��������
#define INODEBLKSIZE		(INODESIZE*CLUSTERNUM/CLUSTERSIZE) //inode��ռ�õĴش�С        32*512/1024=16
#define SYSCLUSTERSIZE		(SUPERBLKSIZE+INODEBLKSIZE) //ϵͳ��������ռ�õĴ��̴ش�С
/*�û�����*/
#define USERNAMESIZE		38 
#define PWDSIZE				20 
#define ADMIN				0x00	//����Ա
#define USER				0x01	//��ͨ�û�
/*�ļ�����*/
#define UNREAD				0x01	//ѡ��ʱ���ɶ���Ĭ�Ͽɶ�
#define UNWIRTE				0x02 //ѡ��ʱ����д��Ĭ�Ͽ�д
#define UNEXECUTE			0x04 //ѡ��ʱ����ִ�У�Ĭ�Ͽ���ִ��
#define UNEXTEND			0x08 //ѡ��ʱ������չ��Ĭ�Ͽ�����չ
#define UNDELETE			0x10 //ѡ��ʱ����ɾ����Ĭ�Ͽ���ɾ��
#define LIST				0X11 //ѡ��ʱ�����б�
/*�ļ�����*/
#define TXT					0x01 //txt�ļ�
#define DIR					0x00 //Ŀ¼�ļ�

/*���¶���һЩ���������ڿ��ٱ�ʶ*/
typedef unsigned short			uid_t;
typedef unsigned short			gid_t;
typedef unsigned short			type_t;
//�ļ������ڵ��㣬�ܴ�СΪ32�ֽ�
typedef struct inode{
	unsigned short				i_ino; // inode���
	uid_t						i_uid; // �ļ������û����
	gid_t						i_gid; // �ļ��������
	unsigned short				i_addr; // FAT���еĵ�һ�������
	type_t						i_type;	//�ļ�����
	unsigned short				i_acl; // �ļ�Ȩ��
	unsigned short				i_size; //�ļ���С
	unsigned short				i_blkCnt; //����
	time_t						i_atime; // �������ʱ��
	time_t						i_mtime; // ����޸�ʱ��
}inode;

typedef struct file {
	char						f_name[FILENAMESIZE];	//�ļ���
	unsigned short				f_ino;		//�ļ�inode���
}file;

typedef struct Files {
	file						file[SUBFILENUM]; //���ļ���Ϣ
	unsigned					size; //���ļ�����
}Files;


typedef struct Openqueue {
	char						f_name[FILENAMESIZE];	//�ļ���
	unsigned short				f_ino;		//�ļ�inode���
	type_t						i_type;   //�ļ�����
}Openqueue;

//�û��ṹ��
typedef struct User {
	uid_t						uid;	//�û�id
	unsigned short				u_mode;	//�û�����������Ա����ͨ�û�
	char						u_name[USERNAMESIZE];	//�û���
	char						u_pwd[PWDSIZE];		//����
}User;

//�����飬������֯�ļ�
//һ��ռ��������
typedef struct superBlk {
	unsigned int				disk_size;//���̴�С
	unsigned short				free_blk;//ʣ����д�����
	unsigned int				free_disk;//ʣ����̴�С
	unsigned short				root_ino;	//��Ŀ¼ino���
	unsigned short				bitmap_inode[BITMAPSIZE];//����inode�ڵ�λͼ��0��ʾ���У�1��ʾռ��
	unsigned short				FAT[FATSIZE];//FAT
	unsigned short				inode_count;//����inode�ڵ������
	unsigned short				inode_free;//����inode�ڵ�����
	bool						sys_Status ;//ϵͳ״̬
	User						users[MAXUSERS];//�洢���û�����
}superBlk;

/*=================ͨ��ģ��=======================*/
inode* getInode(superBlk* supblk, uid_t uid, gid_t gid, type_t type);	//��ȡinode�ڵ�
unsigned short* getFATList(superBlk* supblk, inode* target); //�ҵ��ļ�ռ�õĿ�����
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno); // Ŀ¼��ת
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino); //�ͷ�ռ�õ�inode
unsigned short getIno(Files* fls, FILE* disk, char* filename, type_t type);//����Ŀ¼���ļ��б��ҵ��ļ�����Ӧ��ino�����û���򷵻�-1��0xFFFF
void allocBlk(superBlk* supblk, inode* target, unsigned short size); //Ϊ�ļ��������Ŀռ�


/*=================����ģ��=======================*/
/*ϵͳ��*/
void mainWindows(superBlk* supblk, FILE* disk, inode* curPath, User* curUser, Files* fls, Files* path); //ϵͳ�ӽ���
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** files, Files** path); //ϵͳ����
void InitSys(superBlk* supblk, FILE* disk);	//��ʼ��ϵͳ

/*Ŀ¼��*/
void mkdir(superBlk* supblk, FILE* disk, Files* fls);//����Ŀ¼
void Ls(Files* fls);//չʾ��ǰĿ¼�������ļ���
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path); //Ŀ¼��ת

/*�ļ���*/
void creatFile(superBlk* supblk, FILE* disk, Files* fls, uid_t uid, gid_t gid);//�����ļ�
void OpenFile(Files* fls, FILE* disk, char* filename, type_t type, Openqueue* queue);//���ض��ļ�����򿪶���
void CloseFile(char* filename, type_t type, Openqueue* queue);//���ض��ļ��Ӵ򿪶���ɾ��

/*�û���*/
User* login(superBlk* superBlk1, char* u_name, char* u_pwd, User* act_user);//��¼����,�����û��ṹ���ָ�롣
bool create_user(superBlk* superBlk1, char* new_name, char* new_pwd);//�������û�����,����bool����

#endif
