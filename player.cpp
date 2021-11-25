#include <ncurses.h>
#include "player.hpp"

#define KEY_UP 119
#define KEY_DOWN 115
#define KEY_LEFT 97
#define KEY_RIGHT 100

Player::Player(unsigned short int pos)
{
	curr_pos = pos;
}

void Player::move_up(void)
{
	prev_pos = curr_pos;
	curr_pos -= 60;
}

void Player::move_down(void)
{
	prev_pos = curr_pos;
	curr_pos += 60;
}

void Player::move_left(void)
{
	prev_pos = curr_pos;
	curr_pos -= 1;
}

void Player::move_down(void)
{
	prev_pos = curr_pos;
	curr_pos += 1;
}

void Player::update(void)
{
	controller.curr_key_pressed = getch();
	if (kbhit() && controller.curr_key_pressed != controller.curr_key_pressed)
	{
		switch (controller.curr_key_pressed)
		{
		case KEY_UP:
			Player::move_up();
			break;

		case KEY_DOWN:
			Player::move_down();
			break;

		case KEY_LEFT:
			Player::move_left();
			break;

		case KEY_RIGHT:
			Player::move_right();
			break;
		}
		refresh();
	}
	else
	{
		switch (controller.curr_key_pressed)
		{
		case KEY_UP:
			Player::move_up();
			break;

		case KEY_DOWN:
			Player::move_down();
			break;

		case KEY_LEFT:
			Player::move_left();
			break;

		case KEY_RIGHT:
			Player::move_right();
			break;
		}
		refresh();
	}
}

int Player::kbhit(void)
{
	int ch = getch();

	if (ch != ERR)
	{
		ungetch(ch);
		return 1;
	}
	else
	{
		return 0;
	}
}
