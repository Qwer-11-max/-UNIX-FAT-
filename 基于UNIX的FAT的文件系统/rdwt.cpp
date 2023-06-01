#include"filesys.h"
#include<iostream>
#include <string.h>

using namespace std;
//���ļ�����������
void writeFile(superBlk* supblk, FILE* disk, Openqueue* queue) {
	//�����ļ���
	char filename[FILENAMESIZE] = "\0";
	cout << "�������ļ���:";
	cin >> filename;

	//�жϴ治���ڴ��ļ��б���
	bool check = false;
	inode* target = NULL;
	for (int i = 0; i < 10; i++) {
		if (!strcmp(filename, queue[i].f_name)) { //����ļ��ڴ򿪶�����
			check = true;
			target = &queue[i].f_inode;
			break;
		}
	}
	if (!check) { //���ڴ򿪶����еĻ���ӡ������Ϣ���˳�
		cout << "�ļ�δ��\n";
		return;
	}

	//���������Լ�Ҫд��Ļ���ַ
	char buf[BUFSIZE] = "\0";
	int base = 0;

	cout << "������Ҫд��Ļ���ַ";
	cin >> base;
	cout << "���������ݣ�";
	cin >> buf;

	//�ж��ļ���С������ռ�
	int size = strlen(buf) + base;
	if (size > target->i_size) {
		allocBlk(supblk, target, size - target->i_size);
	}
	//���ļ�����������
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
	//д�ļ��ɹ�
	cout << "�ļ�д��ɹ�";
}