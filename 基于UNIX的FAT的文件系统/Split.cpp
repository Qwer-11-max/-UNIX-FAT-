/*���ߣ�������*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>
using namespace std;
type_t Split(char* wholefilename)//�������Ϊ�ṹ��ָ�룬�Լ���������ļ������ַ�����ָ��           //ע����δ�ԷǷ�״̬�����ݽṹ�ڵ�ֵ��ϸ����
{
	int length;//������ļ�ȫ������
	int location;//�ļ��������һ����.����λ��
	bool check = false;//����������ʾ�Ƿ�������'.'
	length = strlen(wholefilename);//���������ַ����鳤�ȣ�                 

	for (int i = length - 1;i >= 0;i--)//��ѭ�������ж����һ����.����λ��
	{
		if (wholefilename[i] == '.')
		{
			location = i;
			check = true;
			break;
		}
	}

	if (check == true)//����������ˡ�.��
	{
		if (location == 0)//��������еġ�.���ڵ�һ���ַ�����ô�Ƿ�
		{
			cout << "�����ļ����Ƿ�" << endl;
			return FILEERROR;//�˴��Ƿ��иĽ��ռ䣿�ú�����֪���Ƿ�Ƿ���
		}
		else if (location >13)//���'.'�ڵ�13���ַ�֮�󣬼��ļ�������13
		{
			cout << "�����ļ�������" << endl;
			return FILEERROR;
		}
		else if (length - location - 1 > 8)//�˴�������Ǻ�׺����ʵ����
		{
			cout << "�����ļ���׺����" << endl;
			return FILEERROR;
		}
		else //���С�.���ҳ��ȶ��Ϸ�ʱ
		{
			char temp_type[FILENAMESIZE] = "\0";
			int j = 0;
			for (int i = location + 1; i < length; i++,j++) 
				temp_type[j] = wholefilename[i];
		//�������ǱȽ��ַ������Ҹ��ṹ�帳ֵ
			char type0[9] = "dir";
			char type1[9] = "txt";
			char type2[9] = "default";
			char type3[9] = "c";
			if(strcmp(temp_type, type1) == 0)
				return TXT;
			else if(strcmp(temp_type, type3)==0)
				return  C;
			else if (strcmp(temp_type, type0) == 0)
				return  DIR;
			else if (strcmp(temp_type, type2) == 0)
				return  DEFAULT;
			else//����������ֺ�׺�Բ��ǣ���ô����ĺ�׺���ǷǷ���
			{
				cout << "������ļ���׺�Ƿ�" << endl;
				return FILEERROR;
			}
		}
	}
	else//���û��������.��,����׺��ΪĬ�Ϻ�׺
	{
		if (length <= 13)
		{
			return DEFAULT;
		}
		else//�����Ƿ�
		{
			cout << "�ļ�������" << endl;
			return FILEERROR;

		}
	}

}