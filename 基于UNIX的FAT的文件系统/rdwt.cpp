#include"filesys.h"
#include<iostream>
#include <string.h>

using namespace std;
//向文件中输入数据
void writeFile(superBlk* supblk, FILE* disk, Openqueue* queue) {
	//输入文件名
	char filename[FILENAMESIZE] = "\0";
	cout << "请输入文件名:";
	cin >> filename;

	//判断存不存在打开文件列表内
	bool check = false;
	inode* target = NULL;
	for (int i = 0; i < 10; i++) {
		if (!strcmp(filename, queue[i].f_name)) { //如果文件在打开队列中
			check = true;
			target = &queue[i].f_inode;
			break;
		}
	}
	if (!check) { //不在打开队列中的话打印错误信息。退出
		cout << "文件未打开\n";
		return;
	}

	//输入数据以及要写入的基地址
	char buf[BUFSIZE] = "\0";
	int base = 0;

	cout << "请输入要写入的基地址";
	cin >> base;
	cout << "请输入数据：";
	cin >> buf;

	//判断文件大小并分配空间
	int size = strlen(buf) + base;
	if (size > target->i_size) {
		allocBlk(supblk, target, size - target->i_size);
	}
	//向文件中输入数据
	unsigned short* FATList = getFATList(supblk, target);
	int curBlk = base / CLUSTERSIZE;
	int blkRest = CLUSTERSIZE -  base % CLUSTERSIZE;
	int rest = strlen(buf);
	int b = 0;
	while (rest > 0) {
		fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + FATList[curBlk] * CLUSTERSIZE, SEEK_SET);
		fwrite(buf + b, sizeof(char), blkRest, disk);
		curBlk += 1;
		blkRest = CLUSTERSIZE;
		rest -= blkRest;
		b += blkRest;
	}
	//写文件成功
	cout << "文件写入成功";
}