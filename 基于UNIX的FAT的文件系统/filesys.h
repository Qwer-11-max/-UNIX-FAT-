#ifndef FILESYS_H
#define FILESYS_H

#include<ctime>
#include<stdbool.h>
/*基础属性*/
#define BLOCKSIZE 256 //块大小
#define FILENAMESIZE 16 //文件名大小
#define CLUSTERSIZE (4*BLOCKSIZE) //簇大小
#define FATSIZE 512 //FAT表的大小
#define BITMAPSIZE (FATSIZE/16) //位图的大小
#define NADDR 10	//
#define FILENAMESIZE 14 //文件名大小
/*用户属性*/
#define USERNAMESIZE 32
#define PWDSIZE 32 
#define ADMIN 0x00 //管理员
#define USER 0x01 //普通用户
/*文件操作*/
#define UNREAD 0x01	//选择时不可读，默认可读
#define UNWIRTE 0x02 //选择时不可写，默认可写
#define UNEXECUTE 0x04 //选择时不可执行，默认可以执行
#define UNEXTEND 0x08 //选择时不可拓展，默认可以拓展
#define UNDELETE 0x10 //选择时不可删除，默认可以删除
#define LIST 0X11 //选择时可以列表
/*文件类型*/
#define TXT 0x01 //txt文件
#define DIR 0x00 //目录文件
/**/
#define SEEK_SET 0	//文件起始地址
#define DESKFULL 65535

/*重新定义一些变量，用于快速标识*/
typedef unsigned short			uid_t;
typedef unsigned short			gid_t;
typedef unsigned short			type_t;
//文件索引节点结点，总大小为32字节
typedef struct inode{
	unsigned short				i_ino; // inode序号
	uid_t						i_uid; // 文件所属用户序号
	gid_t						i_gid; // 文件所在组号
	unsigned short				i_addr; // FAT表中的第一个块序号
	type_t						i_type;	//文件类型
	unsigned short				i_acl; // 文件权限
	unsigned short				i_size; //文件大小
	unsigned short				i_blkCnt; //块数
	time_t						i_atime; // 最近访问时间
	time_t						i_mtime; // 最近修改时间
};
//管理inode信息的链表
typedef struct inode_list {
	struct inode				*inode;	//inode信息
	struct inode				* i_forwd; //前向节点 
	struct inode				* i_bak;	//后继节点
};

typedef struct file {
	char						f_name[FILENAMESIZE];	//文件名
	unsigned short				f_ino;		//文件inode序号
};

//用户结构体
typedef struct user {
	uid_t						uid;	//用户id
	unsigned short				u_mode;	//用户的类别，如管理员与普通用户
	char						u_name[USERNAMESIZE];	//用户名
	char						u_pwd[PWDSIZE];		//密码
};
typedef struct superBlk {
	unsigned int				disk_size;//磁盘大小
	unsigned short				free_blk;//剩余空闲块数量
	unsigned int				free_disk;//剩余磁盘大小
	struct inode				* root;	//根目录地址
	unsigned short				bitmap[BITMAPSIZE];//空闲块位图，0表示空闲，1表示占用
	unsigned short				bitmap_inode[BITMAPSIZE];//空闲inode节点位图，0表示空闲，1表示占用
	unsigned short				inode_count;//已用inode节点的数量
	unsigned short				inode_free;//空闲inode节点数量
	bool						sys_Status ;//系统状态
};
//FAT
unsigned short FAT1[FATSIZE] = {0}; //主表
unsigned short FAT2[FATSIZE] = {0}; //备用表
superBlk superblk;	//超级块
FILE* disk;	//磁盘模拟文件
inode_list i_head;	//管理链表文件
inode *cur_dir; //当前目录位置
//函数
struct inode* getInode(uid_t uid,gid_t gid,type_t type);	//获取inode节点
void createFile(char * filelname,type_t type, uid_t uid, gid_t gid);
void InitSys();	//初始化系统
#endif
