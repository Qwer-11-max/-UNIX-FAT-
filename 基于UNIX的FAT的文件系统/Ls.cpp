/*作者：马文轩*/
#include"filesys.h"
#include<IOSTREAM>
#include<string.h>
using namespace std;
void Ls(Files* fls)//展示当前目录所包含文件名
{	
	int i = 0;
	Files* temp_ptr = fls;//临时指针接收参数
	for (i = 0;i < SUBFILENUM;i++){//循环
		if (temp_ptr->file[i].f_name[0] != '\0')
			cout << temp_ptr->file[i].f_name << "  ";//文件名有内容则打印之
	}
	cout << endl;
}
