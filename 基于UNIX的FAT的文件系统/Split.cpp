#include"filesys.h"
#include<IOSTREAM>
#include<string.h>
using namespace std;
void Split(WholeName* wholename, char* wholefilename)//�������Ϊ�ṹ��ָ�룬�Լ���������ļ������ַ�����ָ��           //ע����δ�ԷǷ�״̬�����ݽṹ�ڵ�ֵ��ϸ����
{
	WholeName* temp_ptr = wholename;//������ʱָ����ش���Ĳ���ָ��
	int length;//������ļ�ȫ������
	int location;//�ļ��������һ����.����λ��
	bool check = false;//����������ʾ�Ƿ�������'.'
	char temp_type[FILETYPESIZE] = {NULL};//��ʱ����ļ���׺�趨�ļ���׺����Ϊ9�������˳��ȷǷ���
	length = strlen(wholefilename);//���������ַ����鳤�ȣ�                 

	for (int i = length - 1;i >= 0;i--)//��ѭ�������ж����һ����.����λ��
	{
		if (wholefilename[i] == '.')
		{
			location = i;
			check = true;
			break;
		}
		else continue;
	}
	if (check == true)//����������ˡ�����
	{
		if (location == 0)//��������еġ�.���ڵ�һ���ַ�����ô�Ƿ�
		{
			cout << "�����ļ����Ƿ�" << endl;
			return;//�˴��Ƿ��иĽ��ռ䣿�ú�����֪���Ƿ�Ƿ���
		}
		else if (location >13)//���'.'�ڵ�13���ַ�֮�󣬼��ļ�������13
		{
			cout << "�����ļ�������" << endl;
			return;
		}
		else if (length - location - 1 > 8)//�˴�������Ǻ�׺����ʵ����
		{
			cout << "�����ļ���׺����" << endl;
			return;
		}
		else //���С�.���ҳ��ȶ��Ϸ�ʱ
		{
			for (int i = 0;i < location;i++)//��ִ洢
				temp_ptr->f_name[i] = wholefilename[i];
			temp_ptr->f_name[location] = '\0';
			for (int i = location + 1;i < length;i++)
				temp_type[i] = wholefilename[i];
		//�������ǱȽ��ַ������Ҹ��ṹ�帳ֵ
			char type0[9] = "dir";
			char type1[9] = "txt";
			char type2[9] = "default";
			char type3[9] = "c";
			if(strcmp(temp_type, type1)==0)
				temp_ptr->i_type = TXT;
			else if(strcmp(temp_type, type3)==0)
				temp_ptr->i_type = C;
			else if (strcmp(temp_type, type0) == 0)
				temp_ptr->i_type = DIR;
			else if (strcmp(temp_type, type2) == 0)
				temp_ptr->i_type = DEFAULT;
			else//����������ֺ�׺�Բ��ǣ���ô����ĺ�׺���ǷǷ���
			{
				cout << "������ļ���׺�Ƿ�" << endl;
				return;
			}
		}
	}
	else//���û��������.��,����׺��ΪĬ�Ϻ�׺
	{
		if (length <= 13)
		{
			for (int i = 0;i < length;i++)
				temp_ptr->f_name[i] = wholefilename[i];
			temp_ptr->f_name[length] = '\0';
			temp_ptr->i_type = DEFAULT;
			return;
		}
		else//�����Ƿ�
		{
			cout << "�ļ�������" << endl;
			return;

		}
	}

}