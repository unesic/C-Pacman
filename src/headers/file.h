#ifndef File_H
#define File_H

class File
{
public:
	static std::ifstream r(std::string);
	static std::ofstream w(std::string);
};

#endif
