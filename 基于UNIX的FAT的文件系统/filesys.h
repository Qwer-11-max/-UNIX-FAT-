#ifndef FILESYS_H
#define FILESYS_H

#include<ctime>
#include<stdbool.h>
/*��������*/
#define BLOCKSIZE 256 //���С
#define FILENAMESIZE 16 //�ļ�����С
#define CLUSTERSIZE (4*BLOCKSIZE) //�ش�С
#define FATSIZE 512 //FAT��Ĵ�С
#define BITMAPSIZE (FATSIZE/16) //λͼ�Ĵ�С
#define NADDR 10	//
#define FILENAMESIZE 14 //�ļ�����С
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
#define DIR 0x00 //Ŀ¼�ļ�
/**/
#define SEEK_SET 0	//�ļ���ʼ��ַ
#define DESKFULL 65535

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
};
//����inode��Ϣ������
typedef struct inode_list {
	struct inode				*inode;	//inode��Ϣ
	struct inode				* i_forwd; //ǰ��ڵ� 
	struct inode				* i_bak;	//��̽ڵ�
};

typedef struct file {
	char						f_name[FILENAMESIZE];	//�ļ���
	unsigned short				f_ino;		//�ļ�inode���
};

//�û��ṹ��
typedef struct user {
	uid_t						uid;	//�û�id
	unsigned short				u_mode;	//�û�����������Ա����ͨ�û�
	char						u_name[USERNAMESIZE];	//�û���
	char						u_pwd[PWDSIZE];		//����
};
typedef struct superBlk {
	unsigned int				disk_size;//���̴�С
	unsigned short				free_blk;//ʣ����п�����
	unsigned int				free_disk;//ʣ����̴�С
	struct inode				* root;	//��Ŀ¼��ַ
	unsigned short				bitmap[BITMAPSIZE];//���п�λͼ��0��ʾ���У�1��ʾռ��
	unsigned short				bitmap_inode[BITMAPSIZE];//����inode�ڵ�λͼ��0��ʾ���У�1��ʾռ��
	unsigned short				inode_count;//����inode�ڵ������
	unsigned short				inode_free;//����inode�ڵ�����
	bool						sys_Status ;//ϵͳ״̬
};
//FAT
unsigned short FAT1[FATSIZE] = {0}; //����
unsigned short FAT2[FATSIZE] = {0}; //���ñ�
superBlk superblk;	//������
FILE* disk;	//����ģ���ļ�
inode_list i_head;	//���������ļ�
inode *cur_dir; //��ǰĿ¼λ��
//����
struct inode* getInode(uid_t uid,gid_t gid,type_t type);	//��ȡinode�ڵ�
void createFile(char * filelname,type_t type, uid_t uid, gid_t gid);
void InitSys();	//��ʼ��ϵͳ
#endif
