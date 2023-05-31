#include"filesys.h"
#include<IOSTREAM>
#include<string.h>
using namespace std;
void Split(WholeName* wholename, char* wholefilename)//需求参数为结构体指针，以及存放整个文件名的字符数组指针           //注明，未对非法状态下数据结构内的值仔细推敲
{
	WholeName* temp_ptr = wholename;//建立临时指针承载传入的参数指针
	int length;//输入的文件全名长度
	int location;//文件名中最后一个‘.’的位置
	bool check = false;//布尔变量表示是否搜索到'.'
	char temp_type[FILETYPESIZE] = {NULL};//临时存放文件后缀设定文件后缀长度为9，超过此长度非法，
	length = strlen(wholefilename);//检测输入的字符数组长度；                 

	for (int i = length - 1;i >= 0;i--)//倒循环用于判断最后一个“.”的位置
	{
		if (wholefilename[i] == '.')
		{
			location = i;
			check = true;
			break;
		}
		else continue;
	}
	if (check == true)//如果搜索到了‘，’
	{
		if (location == 0)//如果输入中的‘.’在第一个字符，那么非法
		{
			cout << "输入文件名非法" << endl;
			return;//此处是否有改进空间？让函数外知晓是否非法？
		}
		else if (location >13)//如果'.'在第13个字符之后，即文件名长于13
		{
			cout << "输入文件名过长" << endl;
			return;
		}
		else if (length - location - 1 > 8)//此处计算的是后缀的真实长度
		{
			cout << "输入文件后缀过长" << endl;
			return;
		}
		else //当有‘.’且长度都合法时
		{
			for (int i = 0;i < location;i++)//拆分存储
				temp_ptr->f_name[i] = wholefilename[i];
			temp_ptr->f_name[location] = '\0';
			for (int i = location + 1;i < length;i++)
				temp_type[i] = wholefilename[i];
		//接下来是比较字符串并且给结构体赋值
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
			else//如果上述四种后缀皆不是，那么输入的后缀就是非法的
			{
				cout << "输入的文件后缀非法" << endl;
				return;
			}
		}
	}
	else//如果没搜索到‘.’,将后缀设为默认后缀
	{
		if (length <= 13)
		{
			for (int i = 0;i < length;i++)
				temp_ptr->f_name[i] = wholefilename[i];
			temp_ptr->f_name[length] = '\0';
			temp_ptr->i_type = DEFAULT;
			return;
		}
		else//过长非法
		{
			cout << "文件名过长" << endl;
			return;

		}
	}

}