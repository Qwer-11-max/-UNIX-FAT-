#include<iostream>
#include"filesys.h"

int main() {
	std::cout << sizeof(inode)<<std::endl;
	std::cout << sizeof(short) << std::endl;
	std::cout << sizeof(char) << std::endl;
	std::cout << sizeof(long long) << std::endl;
	std::cout << sizeof(int*) << std::endl;
}