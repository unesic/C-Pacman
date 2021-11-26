#include "includes/defines.cpp"
#include "headers/player.h"
#include "headers/screen.h"
#include "headers/game.h"
#include "headers/file.h"
#include "headers/game.h"

Game::Game(Player *player, Screen *scr_game, Screen *scr_post_game)
{
	this->player = player;
	this->scr_game = scr_game;
	this->scr_post_game = scr_post_game;
	this->game_run = false;
}

std::function<void(void)> Game::start(Game *game)
{
	return [=]() -> void
	{
		game->game_run = true;
		game->cur_score = 1;
		game->fin_score = 0;
		game->fin_time = 0;
		game->start_time = std::chrono::system_clock::now();

		while (game->game_run)
		{
			game->update();
			Screen::draw_screen(game->scr_game);
			game->check_end();
			usleep(1000000 / 10);
		}

		game->update_post_game_screen();
		game->scr_post_game->draw();
	};
}

void Game::update_post_game_screen()
{
	this->scr_post_game->file_r = File::r(this->scr_post_game->path);
	std::string line((std::istreambuf_iterator<char>(this->scr_post_game->file_r)), std::istreambuf_iterator<char>());
	this->scr_post_game->file_r.close();

	std::string score = std::to_string(this->fin_score);
	std::string time = std::to_string(this->fin_time);

	for (int i = 0; i < score.length(); i++)
	{
		line[512 + i] = score[i];
	}
	for (int i = 0; i < time.length(); i++)
	{
		line[572 + i] = time[i];
	}

	this->scr_post_game->file_w = File::w(this->scr_post_game->path);
	this->scr_post_game->file_w << line;
	this->scr_post_game->file_w.close();
}

void Game::check_end()
{
	if (!this->fin_time || this->cur_score == MAX_SCORE)
	{
		this->game_run = false;
	}
}

void Game::update()
{
	this->update_map();
	this->player->update(this->is_solid());
}

void Game::update_map()
{
	this->scr_game->file_r = File::r(this->scr_game->path);
	std::string line((std::istreambuf_iterator<char>(this->scr_game->file_r)), std::istreambuf_iterator<char>());
	this->scr_game->file_r.close();

	this->update_player();

	if (line[this->player->curr_pos] == '&')
	{
		this->cur_score++;
	}

	if (line[this->player->prev_pos] != ' ')
	{
		line[this->player->prev_pos] = ' ';
	}

	line[this->player->curr_pos] = '*';
	line = this->update_score(line);
	line = this->update_time(line);

	this->scr_game->file_w = File::w(this->scr_game->path);
	this->scr_game->file_w << line;
	this->scr_game->file_w.close();
}

void Game::update_player()
{
	if (this->player->curr_pos == PRTL_LEFT)
	{
		this->player->curr_pos = PRTL_RIGHT;
	}
	else if (this->player->curr_pos == PRTL_RIGHT)
	{
		this->player->curr_pos = PRTL_LEFT;
	}
}

std::string Game::update_score(std::string line)
{
	std::string text = "SCORE: " + std::to_string(this->cur_score);

	for (int i = 0; i < text.length(); i++)
	{
		line[13 * MAP_W + MAP_W / 2 - text.length() / 2 + i] = text[i];
	}

	this->fin_score = this->cur_score;

	return line;
}

std::string Game::update_time(std::string line)
{
	std::chrono::system_clock::time_point current_time = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = current_time - start_time;

	unsigned short int elapsed_time = (int)(elapsed.count());
	unsigned short int display_time = MAX_TIME - elapsed_time;

	std::string text = "TIME:  " + std::to_string(display_time);

	for (int i = 0; i < text.length(); i++)
	{
		line[14 * MAP_W + MAP_W / 2 - text.length() / 2 + i] = text[i];
	}

	this->fin_time = MAX_TIME - elapsed_time;

	return line;
}

std::function<bool(int)> Game::is_solid()
{
	this->scr_game->file_r = File::r(this->scr_game->path);
	std::string line((std::istreambuf_iterator<char>(scr_game->file_r)), std::istreambuf_iterator<char>());
	scr_game->file_r.close();

	return [=](int pos)
	{
		if (
				(line[pos] == ' ' && line[pos] == ' ') ||
				(line[pos] == '&' && line[pos] == '&'))
		{
			return false;
		}
		else
		{
			return true;
		}
	};
}

void Game::exit_game()
{
	exit(1);
}
