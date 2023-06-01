/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>

using namespace std;
//�ļ���������������ļ�������,���ļ������ļ����ͣ�
void creatFile(superBlk* supblk,FILE* disk,Files* fls,uid_t uid,gid_t gid)//������
{
	char filename[FILENAMESIZE];//������ʱ�ļ�������
	unsigned short check;//������ʱ��־λ�������ж��ļ����Ƿ�Ϸ�

	//cout << "starting creatfile, please putin filename" << endl;//�����ʾ
	cin >> filename;//ͨ�����������ļ���
	type_t type = Split(filename);
	if (type == FILEERROR) {
		return;
	}
	
	check = getIno( fls,  disk,  filename, type);//���ò�ѯ�����������ͬ��ͬ�����ļ����򷵻ظ��ļ�ino�������ͬ���ļ��򷵻� -1
	if (check != 0xffff)//�����־λ����-1������ͬ���ļ�
	{
		cout << "there's something wrong,FileName has been used" << endl;//���������Ϣ
		return;
	}
	else //���򴴽����ļ�
	{
		//��ȡ��inode
		inode* temp = getInode(supblk,disk, uid, gid, type);
		//���ļ�����inoд��fls
		for (int i = 0; i < SUBFILENUM; i++) {
			if (fls->file[i].f_name[0] == '\0') {
				strcpy(fls->file[i].f_name, filename);
				fls->file[i].f_ino = temp->i_ino;
				break;
			}
		}
		free(temp);
		cout << "creating success" << endl;
	}
	return;

}
