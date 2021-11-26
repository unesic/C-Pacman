#include <ncurses.h>
#include "headers/screen.h"
#include "headers/file.h"

Screen::Screen(std::string path, int cx, int cy)
{
	this->path = path;
	this->crd.X = cx;
	this->crd.Y = cy;
}

void Screen::draw()
{
	Screen::draw_screen(this);

	char c;
	std::cin >> c;

	if (c == '1' && this->opts.opt_1.tag != Tag::NUL)
	{
		Screen::execute_option(this->opts.opt_1);
	}
	else if (c == '2' && this->opts.opt_2.tag != Tag::NUL)
	{
		Screen::execute_option(this->opts.opt_2);
	}
	else if (c == '3' && this->opts.opt_3.tag != Tag::NUL)
	{
		Screen::execute_option(this->opts.opt_3);
	}
	else
	{
		this->draw();
	}
}

void Screen::draw_screen(Screen *screen)
{
	screen->file_r = File::r(screen->path);
	Screen::set_cursor(screen->crd);
	std::wcout.imbue(std::locale("en_US.UTF-8"));

	char c;
	while (screen->file_r.get(c))
	{
		switch (c)
		{
		case '!':
			std::wcout << L'\u2554';
			break;
		case '@':
			std::wcout << L'\u2557';
			break;
		case '#':
			std::wcout << L'\u255A';
			break;
		case '$':
			std::wcout << L'\u255D';
			break;
		case '%':
			std::wcout << L'\u2550';
			break;
		case '^':
			std::wcout << L'\u2551';
			break;
		case '&':
			std::wcout << L'\u2022';
			break;
		case ' ':
			std::wcout << L'\u00A0';
			break;
		case '-':
			std::wcout << '-';
			break;
		case '*':
			std::wcout << L'\u2588';
			break;
		case '\n':
			screen->crd.Y++;
			Screen::set_cursor(screen->crd);
			break;
		default:
			std::wcout << c;
			break;
		}
	}

	screen->crd.Y = 2;
	screen->file_r.close();
}

void Screen::set_cursor(COORD c)
{
	printf("%c[%d;%df", 0x1B, c.Y, c.X);
}

void Screen::execute_option(OPTION opt)
{
	if (opt.tag == Tag::SCR)
	{
		opt.scr->draw();
	}
	else if (opt.tag == Tag::FUN)
	{
		opt.fun();
	}
}
