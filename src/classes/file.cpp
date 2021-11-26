#include <iostream>
#include <fstream>
#include <string>
#include "headers/file.h"

std::ifstream File::r(std::string path)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cout << "Error: Opening file '" << path << "' failed!" << std::endl;
		exit(1);
	}
	else
	{
		return file;
	}
}

std::ofstream File::w(std::string path)
{
	std::ofstream file(path);

	if (!file.is_open())
	{
		std::cout << "Error: Opening file '" << path << "' failed!" << std::endl;
		exit(1);
	}
	else
	{
		return file;
	}
}
