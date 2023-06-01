#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void writeFile(superBlk* supblk, FILE* disk, Openqueue* queue) {
	//输入文件名
	char filename[FILENAMESIZE];
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
	char* buf=new char[INODESIZE];
  int blknum=0;
  for(int i=0;i<target->i_blkCnt;i++){
    fread(buf,CLUSTERSIZE,1,disk+blknum*CLUSTERSIZE);
    blknum++;
  }
  cout<<"文件读取成功"<<endl;
  delete buf;
}
