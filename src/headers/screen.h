#ifndef Screen_H
#define Screen_H

typedef struct _COORD
{
	short int X;
	short int Y;
} COORD, *PCOORD;

class Screen
{

public:
	Screen(std::string, int, int);
	std::string path;
	std::ifstream file_r;
	std::ofstream file_w;
	COORD crd;

	enum Tag
	{
		SCR,
		FUN,
		NUL,
	};

	typedef struct _OPTION
	{
		Tag tag;
		Screen *scr;
		std::function<void(void)> fun;
	} OPTION;

	struct OPTIONS
	{
		OPTION opt_1;
		OPTION opt_2;
		OPTION opt_3;
	} opts;
	void draw();
	static void draw_screen(Screen *);
	static void set_cursor(COORD);
	static void execute_option(OPTION);
};

#endif
