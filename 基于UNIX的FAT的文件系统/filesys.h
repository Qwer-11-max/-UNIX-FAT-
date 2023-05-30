#ifndef FILESYS_H
#define FILESYS_H
#include<stdio.h>
#include<ctime>
#include<stdbool.h>
/*��������*/
#define BLOCKSIZE			256		//���С,��λ��BYTE
#define INODESIZE			32		//inode��С
#define FILENAMESIZE		14	//�ļ�����С
#define CLUSTERSIZE			(4*BLOCKSIZE)	//�ش�С
#define CLUSTERNUM			512		//�ص�����
#define FATSIZE				512		//FAT��Ĵ�С
#define BITMAPSIZE			(FATSIZE/16)	//λͼ�Ĵ�С
#define MAXUSERS			8		//�û������޶�
#define SUBFILENUM			(CLUSTERSIZE/(FILENAMESIZE+2))		//һ��Ŀ¼���ļ���������
/*���̻���*/
#define SUPERBLKSIZE		2	//������ռ��������
#define INODEBLKSIZE		(INODESIZE*CLUSTERNUM/CLUSTERSIZE) //inode��ռ�õĴش�С
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

//����
inode* getInode(superBlk* supblk, uid_t uid, gid_t gid, type_t type);	//��ȡinode�ڵ�
unsigned short* getFATList(superBlk* supblk, inode* target); //�ҵ��ļ�ռ�õĿ�����
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* files, unsigned short nextDirIno); // Ŀ¼��ת
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** files); //ϵͳ����
void creatFile(superBlk* supblk, Files* fls, type_t type, uid_t uid, gid_t gid,FILE* disk);//�����ļ�
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino); //�ͷ�ռ�õ�inode
unsigned short getIno(Files* fls, FILE* disk, char* filename, type_t type);//����Ŀ¼���ļ��б��ҵ��ļ�����Ӧ��ino�����û���򷵻�-1��0xFFFF
void InitSys(superBlk* supblk, FILE* disk);	//��ʼ��ϵͳ
void mkdir(superBlk* supblk,FILE* disk, char* dirname, uid_t uid, gid_t gid, unsigned short prnt_ino);

#endif
