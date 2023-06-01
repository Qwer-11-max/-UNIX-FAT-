#ifndef FILESYS_H
#define FILESYS_H
#include<stdio.h>
#include<ctime>
#include<stdbool.h>
/*基础属性*/
#define BLOCKSIZE			256		//块大小,单位是BYTE
#define INODESIZE			32		//inode大小
#define FILENAMESIZE		14	//文件名大小
#define FILETYPESIZE		9   //文件后缀最大长度
#define WHOLEFILENAMESIZE   50  //文件全名极限大小
#define CLUSTERSIZE			(4*BLOCKSIZE)	//簇大小   1024BYTE
#define CLUSTERNUM			512		//簇的数量
#define FATSIZE				512		//FAT表的大小
#define BITMAPSIZE			(FATSIZE/16)	//位图的大小   32
#define MAXUSERS			8		//用户数量限定
#define SUBFILENUM			(CLUSTERSIZE/(FILENAMESIZE+2))		//一个目录下文件数量限制  1024/16            64
/*磁盘划分*/
#define SUPERBLKSIZE		2	//超级块占用两个簇
#define INODEBLKSIZE		(INODESIZE*CLUSTERNUM/CLUSTERSIZE) //inode区占用的簇大小        32*512/1024=16
#define SYSCLUSTERSIZE		(SUPERBLKSIZE+INODEBLKSIZE) //系统保留区所占用的磁盘簇大小
/*用户属性*/
#define USERNAMESIZE		38 
#define PWDSIZE				20 
#define ADMIN				0x00	//管理员
#define USER				0x01	//普通用户
/*文件操作*/
#define UNREAD				0x01	//选择时不可读，默认可读
#define UNWIRTE				0x02 //选择时不可写，默认可写
#define UNEXECUTE			0x04 //选择时不可执行，默认可以执行
#define UNEXTEND			0x08 //选择时不可拓展，默认可以拓展
#define UNDELETE			0x10 //选择时不可删除，默认可以删除
#define LIST				0X11 //选择时可以列表
/*文件类型*/
#define FILEERROR			0xffff //文件类型错误
#define C					0x03 //.c文件
#define DEFAULT             0x02 //默认文件类型
#define TXT					0x01 //txt文件
#define DIR					0x00 //目录文件

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
}inode;

typedef struct file {
	char						f_name[FILENAMESIZE];	//文件名
	unsigned short				f_ino;		//文件inode序号
}file;

typedef struct Files {
	file						file[SUBFILENUM]; //子文件信息
	unsigned					size; //子文件数量
}Files;

typedef struct WholeName {
	char						f_name[FILENAMESIZE];	//文件名
	type_t						i_type;   //文件类型
}WholeName;

typedef struct Openqueue {
	char						f_name[FILENAMESIZE];	//文件名
	unsigned short				f_ino;		//文件inode序号
	type_t						i_type;   //文件类型
}Openqueue;

//用户结构体
typedef struct User {
	uid_t						uid;	//用户id
	unsigned short				u_mode;	//用户的类别，如管理员与普通用户
	char						u_name[USERNAMESIZE];	//用户名
	char						u_pwd[PWDSIZE];		//密码
}User;

//超级块，用于组织文件
//一共占用两个块
typedef struct superBlk {
	unsigned int				disk_size;//磁盘大小
	unsigned short				free_blk;//剩余空闲簇数量
	unsigned int				free_disk;//剩余磁盘大小
	unsigned short				root_ino;	//根目录ino编号
	unsigned short				bitmap_inode[BITMAPSIZE];//空闲inode节点位图，0表示空闲，1表示占用
	unsigned short				FAT[FATSIZE];//FAT
	unsigned short				inode_count;//已用inode节点的数量
	unsigned short				inode_free;//空闲inode节点数量
	bool						sys_Status ;//系统状态
	User						users[MAXUSERS];//存储的用户数据
}superBlk;

/*=================通用模块=======================*/
inode* getInode(superBlk* supblk, uid_t uid, gid_t gid, type_t type);	//获取inode节点
unsigned short* getFATList(superBlk* supblk, inode* target); //找到文件占用的块序列
int  setCurPath(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, unsigned short nextDirIno); // 目录跳转
int freeInode(superBlk* supblk, FILE* disk, unsigned short ino); //释放占用的inode
unsigned short getIno(Files* fls, FILE* disk, char* filename, type_t type);//遍历目录子文件列表，找到文件名对应的ino，如果没有则返回-1即0xFFFF
void allocBlk(superBlk* supblk, inode* target, unsigned short size); //为文件分配额外的空间
type_t Split(char* wholefilename);//拆分输入信息函数


/*=================功能模块=======================*/
/*系统类*/
void mainWindows(superBlk* supblk, FILE* disk, inode* curPath, User* curUser, Files* fls, Files* path,Openqueue* queue); //系统子界面
void powerOn(FILE** disk, superBlk** supblk, inode** curPath, Files** files, Files** path); //系统开机
void InitSys(superBlk* supblk, FILE* disk);	//初始化系统
void halt(superBlk* supblk, FILE* disk, inode* curPath, Files* fls); //系统停机

/*目录类*/
void mkdir(superBlk* supblk, FILE* disk, Files* fls);//创建目录
void Ls(Files* fls);//展示当前目录所包含文件名
void chdir(superBlk* supblk, FILE* disk, inode* curPath, Files* fls, Files* path); //目录跳转

/*文件类*/
void creatFile(superBlk* supblk, FILE* disk, Files* fls, uid_t uid, gid_t gid);//创建文件
void OpenFile(Files* fls, FILE* disk, Openqueue* queue);//将特定文件加入打开队列
void CloseFile(char* filename, type_t type, Openqueue* queue);//将特定文件从打开队列删除
void deleteFile(superBlk* supblk, FILE* disk, Files* fls); //删除文件
void readFile();
void writeFile();

/*用户类*/
User* login(superBlk* superBlk1, char* u_name, char* u_pwd, User* act_user);//登录函数,返回用户结构体的指针。
bool create_user(superBlk* superBlk1, char* new_name, char* new_pwd);//创建新用户函数,返回bool类型

#endif
