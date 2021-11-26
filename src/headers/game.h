#ifndef Game_H
#define Game_H

#include "headers/player.h"
#include "headers/screen.h"

class Game
{
public:
	Game(Player *, Screen *, Screen *);
	int fin_score;
	int fin_time;

	void update(void);

	static std::function<void(void)> start(Game *);
	static void exit_game(void);

private:
	std::function<bool(int)> is_solid(void);
	Player *player;
	Screen *scr_game;
	Screen *scr_post_game;

	int cur_score;
	bool game_run;
	std::chrono::system_clock::time_point start_time;

	void update_post_game_screen(void);
	void update_map(void);
	void update_player(void);
	std::string update_score(std::string);
	std::string update_time(std::string);

	void check_end(void);
};

#endif
