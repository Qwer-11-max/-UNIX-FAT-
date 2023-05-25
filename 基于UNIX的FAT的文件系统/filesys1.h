#ifndef FILESYS_H
#define FILESYS_H

#include<ctime>

/*基础属性*/
#define BLOCKSIZE 256 //块大小
#define FILENAMESIZE 16 //文件名大小
#define CLUSTERSIZE 4 //簇大小
#define FATSIZE 512 //FAT表的大小
#define BITMAPSIZE (FATSIZE/16) //位图的大小
#define NADDR 10	//
#define DIRSIZE 14 //目录名大小
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

/**/
#define SEEK_SET 0	//文件起始地址
#define DESKFULL 65535

/*重新定义一些变量，用于快速标识*/
typedef unsigned short			uid_t;
typedef unsigned short			gid_t;
typedef unsigned short			mode_t;
//文件索引节点结点，总大小为32字节
typedef struct inode{
	/*用链表管理inode节点*/
	struct inode				* i_forwd;	//前向节点 
	struct inode				* i_bak; //后一个节点
	unsigned short				i_ino; // inode序号
	uid_t						i_uid; // 文件所属用户序号
	gid_t						i_gid; // 文件所在组号
	unsigned short				i_addr; // FAT表中的第一个块序号
	unsigned short				i_mode;	//文件打开模式
	unsigned short				i_acl; // 文件权限
	unsigned short				i_size; //文件大小
	unsigned short				i_blkCnt; //块数
	time_t						i_atime; // 最近访问时间
	time_t						i_mtime; // 最近修改时间
};
//目录索引节点
typedef struct dinode {
	unsigned short				di_dino; //dinode序号
	unsigned short				di_mode; //目录模式
	unsigned short				di_uid;	//目录所属用户的id
	unsigned short				di_gid; //目录所属的组id
	unsigned short				di_size; // 目录的大小
	unsigned short				di_addr; // 目录的FAT第一个块号
};
//目录
typedef struct direct {
	char dirname[DIRSIZE];
	unsigned short dno;
};
//用户结构体
typedef struct user {
	uid_t						uid;	//用户id
	char						u_name[USERNAMESIZE];	//用户名
	char						u_pwd[PWDSIZE];		//密码
};

//FAT
unsigned short FAT1[FATSIZE]; //主表
unsigned short FAT2[FATSIZE]; //备用表
//位图
unsigned short bitmap[BITMAPSIZE];
#endif
