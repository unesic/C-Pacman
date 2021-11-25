#ifndef Player_H
#define Player_H

typedef struct _controller
{
	unsigned short int curr_key_pressed;
	unsigned short int prev_key_pressed;
} controller;

class Player
{
public:
	Player(unsigned short int pos);
	unsigned short int curr_pos;
	unsigned short int prev_pos;
	controller controller;
	void update(void);

private:
	void move_up(void);
	void move_down(void);
	void move_left(void);
	void move_right(void);
	int kbhhit(void);
}
