#ifndef FILESYS_H
#define FILESYS_H

#include<ctime>

/*��������*/
#define BLOCKSIZE 256 //���С
#define FILENAMESIZE 16 //�ļ�����С
#define CLUSTERSIZE 4 //�ش�С
#define FATSIZE 512 //FAT��Ĵ�С
#define BITMAPSIZE (FATSIZE/16) //λͼ�Ĵ�С
#define NADDR 10	//
#define DIRSIZE 14 //Ŀ¼����С
/*�û�����*/
#define USERNAMESIZE 32
#define PWDSIZE 32 
#define ADMIN 0x00 //����Ա
#define USER 0x01 //��ͨ�û�
/*�ļ�����*/
#define UNREAD 0x01	//ѡ��ʱ���ɶ���Ĭ�Ͽɶ�
#define UNWIRTE 0x02 //ѡ��ʱ����д��Ĭ�Ͽ�д
#define UNEXECUTE 0x04 //ѡ��ʱ����ִ�У�Ĭ�Ͽ���ִ��
#define UNEXTEND 0x08 //ѡ��ʱ������չ��Ĭ�Ͽ�����չ
#define UNDELETE 0x10 //ѡ��ʱ����ɾ����Ĭ�Ͽ���ɾ��
#define LIST 0X11 //ѡ��ʱ�����б�
/*�ļ�����*/
#define TXT 0x01 //txt�ļ�

/**/
#define SEEK_SET 0	//�ļ���ʼ��ַ
#define DESKFULL 65535

/*���¶���һЩ���������ڿ��ٱ�ʶ*/
typedef unsigned short			uid_t;
typedef unsigned short			gid_t;
typedef unsigned short			mode_t;
//�ļ������ڵ��㣬�ܴ�СΪ32�ֽ�
typedef struct inode{
	/*���������inode�ڵ�*/
	struct inode				* i_forwd;	//ǰ��ڵ� 
	struct inode				* i_bak; //��һ���ڵ�
	unsigned short				i_ino; // inode���
	uid_t						i_uid; // �ļ������û����
	gid_t						i_gid; // �ļ��������
	unsigned short				i_addr; // FAT���еĵ�һ�������
	unsigned short				i_mode;	//�ļ���ģʽ
	unsigned short				i_acl; // �ļ�Ȩ��
	unsigned short				i_size; //�ļ���С
	unsigned short				i_blkCnt; //����
	time_t						i_atime; // �������ʱ��
	time_t						i_mtime; // ����޸�ʱ��
};
//Ŀ¼�����ڵ�
typedef struct dinode {
	unsigned short				di_dino; //dinode���
	unsigned short				di_mode; //Ŀ¼ģʽ
	unsigned short				di_uid;	//Ŀ¼�����û���id
	unsigned short				di_gid; //Ŀ¼��������id
	unsigned short				di_size; // Ŀ¼�Ĵ�С
	unsigned short				di_addr; // Ŀ¼��FAT��һ�����
};
//Ŀ¼
typedef struct direct {
	char dirname[DIRSIZE];
	unsigned short dno;
};
//�û��ṹ��
typedef struct user {
	uid_t						uid;	//�û�id
	char						u_name[USERNAMESIZE];	//�û���
	char						u_pwd[PWDSIZE];		//����
};

//FAT
unsigned short FAT1[FATSIZE]; //����
unsigned short FAT2[FATSIZE]; //���ñ�
//λͼ
unsigned short bitmap[BITMAPSIZE];
#endif
