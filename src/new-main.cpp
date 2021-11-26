
#include <iostream>
#include <fstream>
#include <sys/uio.h>
#include <unistd.h>
#include <ncurses.h>
#include <ctime>

#include "includes/defines.cpp"
#include "classes/screen.cpp"
#include "classes/file.cpp"
#include "classes/game.cpp"
#include "classes/player.cpp"

Screen *screen_home;
Screen *screen_info_1;
Screen *screen_info_2;
Screen *screen_post_game;
Screen *screen_exit;
Screen *screen_game;

Player *player;
Game *game;

void initialize(void);
void configure(void);

int main()
{
	initscr();
	raw();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);

	initialize();
	configure();

	screen_home->draw();
	endwin();

	return 0;
}

void initialize()
{
	screen_home = new Screen("screens/home.txt", 0, 2);
	screen_info_1 = new Screen("screens/info_1.txt", 0, 2);
	screen_info_2 = new Screen("screens/info_2.txt", 0, 2);
	screen_post_game = new Screen("screens/post_game_1.txt", 0, 2);
	screen_exit = new Screen("screens/exit.txt", 0, 2);
	screen_game = new Screen("screens/game.txt", 0, 2);

	player = new Player(23 * MAP_W + MAP_W / 2 - 1, 0, 0, 0);
	game = new Game(player, screen_game, screen_post_game);

	std::ifstream game_map = File::r("screens/level.txt");
	screen_game->file_w = File::w(screen_game->path);
	std::string line1((std::istreambuf_iterator<char>(game_map)), std::istreambuf_iterator<char>());
	screen_game->file_w << line1;

	game_map.close();
	screen_game->file_w.close();

	std::ifstream post_game = File::r("screens/post_game.txt");
	screen_post_game->file_w = File::w(screen_post_game->path);
	std::string line2((std::istreambuf_iterator<char>(post_game)), std::istreambuf_iterator<char>());
	screen_post_game->file_w << line2;

	post_game.close();
	screen_post_game->file_w.close();
}

void configure()
{
	screen_home->opts.opt_1.tag = Screen::Tag::FUN;
	screen_home->opts.opt_1.fun = Game::start(game);
	screen_home->opts.opt_2.tag = Screen::Tag::SCR;
	screen_home->opts.opt_2.scr = screen_info_1;
	screen_home->opts.opt_3.tag = Screen::Tag::SCR;
	screen_home->opts.opt_3.scr = screen_exit;

	screen_info_1->opts.opt_1.tag = Screen::Tag::SCR;
	screen_info_1->opts.opt_1.scr = screen_info_2;
	screen_info_1->opts.opt_2.tag = Screen::Tag::SCR;
	screen_info_1->opts.opt_2.scr = screen_home;
	screen_info_1->opts.opt_3.tag = Screen::Tag::NUL;

	screen_info_2->opts.opt_1.tag = Screen::Tag::SCR;
	screen_info_2->opts.opt_1.scr = screen_info_1;
	screen_info_2->opts.opt_2.tag = Screen::Tag::SCR;
	screen_info_2->opts.opt_2.scr = screen_home;
	screen_info_2->opts.opt_3.tag = Screen::Tag::NUL;

	screen_post_game->opts.opt_1.tag = Screen::Tag::FUN;
	screen_post_game->opts.opt_1.fun = []() -> void
	{
		initialize();
		Game::start(game)();
	};
	screen_post_game->opts.opt_2.tag = Screen::Tag::SCR;
	screen_post_game->opts.opt_2.scr = screen_info_1;
	screen_post_game->opts.opt_3.tag = Screen::Tag::SCR;
	screen_post_game->opts.opt_3.scr = screen_exit;

	screen_exit->opts.opt_1.tag = Screen::Tag::FUN;
	screen_exit->opts.opt_1.fun = Game::exit_game;
	screen_exit->opts.opt_2.tag = Screen::Tag::SCR;
	screen_exit->opts.opt_2.scr = screen_home;
	screen_exit->opts.opt_3.tag = Screen::Tag::NUL;
}
