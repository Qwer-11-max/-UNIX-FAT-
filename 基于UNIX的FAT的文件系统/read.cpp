/*刘振铭*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;

void readFile(superBlk* supblk, FILE* disk, Openqueue* queue) {
	char filename[FILENAMESIZE];
	cout << "请输入文件名:";
	cin >> filename;

	bool check = false;
	inode* target = NULL;
	for (int i = 0; i < 10; i++) {
		if (!strcmp(filename, queue[i].f_name)) { 
			check = true;
			target = &queue[i].f_inode;
			break;
		}
	}
	if (!check) { 
		cout << "该文件未打开\n";
		return;
	}

	char* buf = new char[target->i_size] {'\0'};
	int blknum=0;
	unsigned short* FATList = getFATList(supblk, target);
	for(int i=0;i<target->i_blkCnt;i++){
		fseek(disk, SYSCLUSTERSIZE * CLUSTERSIZE + FATList[i] * CLUSTERSIZE, SEEK_SET);
		fread(buf+blknum*CLUSTERSIZE,CLUSTERSIZE,1,disk);
		blknum++;
	}
	cout<<buf<<endl;;
	delete[] buf;
}
