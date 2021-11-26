#include <ncurses.h>
#include "includes/defines.cpp"
#include "headers/player.h"

Player::Player(int c_pos, int p_pos, int c_key, int p_key)
{
	curr_pos = c_pos;
	prev_pos = p_pos;
	controller.curr_key_pressed = c_key;
	controller.prev_key_pressed = p_key;
}

void Player::move_up(std::function<bool(int)> func)
{
	if (!func(curr_pos - MAP_W - 1) && !func(curr_pos - MAP_W) && !func(curr_pos - MAP_W + 1))
	{
		prev_pos = curr_pos;
		curr_pos -= MAP_W;
		controller.prev_key_pressed = controller.curr_key_pressed;
		controller.curr_key_pressed = ARR_UP;
	}
	else
	{
		if (controller.prev_key_pressed == ARR_LEFT)
		{
			Player::move_left(func);
		}
		else if (controller.prev_key_pressed == ARR_RIGHT)
		{
			Player::move_right(func);
		}
	}
}

void Player::move_down(std::function<bool(int)> func)
{
	if (!func(curr_pos + MAP_W - 1) && !func(curr_pos + MAP_W) && !func(curr_pos + MAP_W + 1))
	{
		prev_pos = curr_pos;
		curr_pos += MAP_W;
		controller.prev_key_pressed = controller.curr_key_pressed;
		controller.curr_key_pressed = ARR_DOWN;
	}
	else
	{
		if (controller.prev_key_pressed == ARR_LEFT)
		{
			Player::move_left(func);
		}
		else if (controller.prev_key_pressed == ARR_RIGHT)
		{
			Player::move_right(func);
		}
	}
}

void Player::move_left(std::function<bool(int)> func)
{
	if (!func(curr_pos - 2))
	{
		prev_pos = curr_pos;
		curr_pos -= 1;
		controller.prev_key_pressed = controller.curr_key_pressed;
		controller.curr_key_pressed = ARR_LEFT;
	}
	else
	{
		if (controller.prev_key_pressed == ARR_UP)
		{
			Player::move_up(func);
		}
		else if (controller.prev_key_pressed == ARR_DOWN)
		{
			Player::move_down(func);
		}
	}
}

void Player::move_right(std::function<bool(int)> func)
{
	if (!func(curr_pos + 2))
	{
		prev_pos = curr_pos;
		curr_pos += 1;
		controller.prev_key_pressed = controller.curr_key_pressed;
		controller.curr_key_pressed = ARR_RIGHT;
	}
	else
	{
		if (controller.prev_key_pressed == ARR_UP)
		{
			Player::move_up(func);
		}
		else if (controller.prev_key_pressed == ARR_DOWN)
		{
			Player::move_down(func);
		}
	}
}

void Player::update(std::function<bool(int)> func)
{
	if (Player::kbhit())
		controller.curr_key_pressed = getch();
	switch (controller.curr_key_pressed)
	{
	case ARR_UP:
		Player::move_up(func);
		break;

	case ARR_DOWN:
		Player::move_down(func);
		break;

	case ARR_LEFT:
		Player::move_left(func);
		break;

	case ARR_RIGHT:
		Player::move_right(func);
		break;
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
