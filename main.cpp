#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include "log.h"
#include "snake.h"
#include "ai.h"

using namespace std;

const int NB_DISABLE = 0;
const int NB_ENABLE = 1;

#define Sleep(x) usleep(x * 1000)
	
int kbhit() 
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state) 
{
	struct termios ttystate;
	tcgetattr(STDIN_FILENO, &ttystate);
	if (state == NB_ENABLE) 
	{
		ttystate.c_lflag &= ~ICANON;
		ttystate.c_cc[VMIN] = 1; // Minimum of number input read.
	} else if (state == NB_DISABLE) 
	{
		ttystate.c_lflag |= ICANON;
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

int main()
{
	int ret = 0;
	INIT_LOG2("snake", "log.conf");
	nonblock(NB_ENABLE);
	CSnake game;
	while(ret >= 0)
	{
		ret = game.draw();
		if(ret < 0)
		{
			ERROR_LOG("draw ret: %d", ret);
			break;
		}

		if(kbhit())
		{
			int dir = game.getkb();
			if(dir == -1)
			{
				return 0;
			}

			ret = game.changeDirection(dir);	
			if(ret < 0)
			{
				ERROR_LOG("changeDirection ret: %d", ret);	
				break;
			}
			else if(ret == 1)
			{
				continue;
			}
		}

		ret = game.go();
		if(ret < 0)
		{
			ERROR_LOG("go ret: %d", ret);
			break;
		}
		else if(ret == 1)
		{
			cout << "you win" << endl;	
			INFO_LOG("go ret: %d, snake eat all apple", ret);
			break;
		}
	}
	return 0;
}

