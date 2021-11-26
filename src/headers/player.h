#ifndef Player_H
#define Player_H

typedef struct _controller
{
	int curr_key_pressed;
	int prev_key_pressed;
} controller;

class Player
{
public:
	unsigned short int curr_pos;
	unsigned short int prev_pos;
	controller controller;
	void update(std::function<bool(int)>);
	Player(int, int, int, int);

private:
	void move_up(std::function<bool(int)>);
	void move_down(std::function<bool(int)>);
	void move_left(std::function<bool(int)>);
	void move_right(std::function<bool(int)>);
	int kbhit();
};

#endif
