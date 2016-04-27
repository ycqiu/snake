#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include "snake.h"
#include "ai.h"

using namespace std;

const int NB_DISABLE = 0;
const int NB_ENABLE = 1;

#define Sleep(x) usleep(x * 1000)
	
int kbhit() 
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 500 * 1000;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	//int res = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	int res = select(STDIN_FILENO + 1, &fds, NULL, NULL, NULL);
	if(res > 0)
	{
	//	select(0, NULL, NULL, NULL, &tv);	
	}

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
	nonblock(NB_ENABLE);
	CSnake game;
	//CAi ai(game);
	CCleverAi ai(game);
	while(ret == 0)
	{
		game.draw();
/*		if(kbhit())
		{
			int key = game.getkb();
			if(key == -1)
			{
				return 0;
			}
			game.changeDirection(key);	
		}
*/
		int dir = ai.calcul();
		if(dir == CSnake::INVALID)
		{
			cout << "ai error" << endl;
			break;
		}

		//	cout << "dir " << dir << endl;	
		game.changeDirection(dir);

		ret = game.go();
		if(ret < 0)
		{
			cout << "ret " << ret << endl;
		}
		Sleep(300);
	}
	return 0;
}

