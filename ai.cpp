#include <iostream>
#include <queue>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "ai.h"
#include "log.h"

using namespace std;


void printSnake(const std::string& s)
{
	/*
	for(int i = 0; i < s.length(); i += 2)	
	{
		cout << (int)s[i] << " " << (int)s[i + 1] << "| ";
	}
	*/
}

int CAi::calcul()
{
	int dir = game_.getDir();
	std::string snake = game_.getSnake();
	std::pair<int, int> snakeLoc = std::make_pair(snake[0], snake[1]);

	srand(time(NULL));
	for(; ;)
	{
		int t = (dir + rand()) % 4;	
		static const int v[] = {CSnake::DOWN, CSnake::UP, CSnake::RIGHT, CSnake::LEFT};	

		if(v[t] == dir)
		{
			continue;
		}
		
		int r = snakeLoc.first + CSnake::direction[t][0];
		int c = snakeLoc.second + CSnake::direction[t][1];
		int ret = game_.gameOver(r, c);
		if(ret == 0)
		{
			return dir = t;	
		}
	}
	return CSnake::INVALID;
}




/////////////////////
int CCleverAi::bfs(std::pair<int, int> goal)
{
	int ret = -1;
	//int dir = game_.getDir();
	std::string snake = game_.getSnake();
//	std::pair<int, int> snakeLoc = std::make_pair(snake[0], snake[1]);
//	std::pair<int, int> appleLoc = game_.getAppleLoc();

	std::queue<std::string>	q;
	q.push(snake);
	visit_[snake] = snake;
	while(q.empty() == false)
	{
		snake = q.front();  q.pop();
		if(snake[0] == goal.first &&
				snake[1] == goal.second)
		{
			ret = calculPath(snake);
			if(ret < 0)
			{
				ERROR_LOG("bfs ok, but calculPath ret: %d", ret);
				return ret = -2;
			}
			return ret; 
		}

		for(int i = 0; i < 4; ++i)	
		{
			int r = snake[0] + CSnake::direction[i][0];	
			int c = snake[1] + CSnake::direction[i][1];	

			if( !(r >= 1 && c >= 1 && r <= CSnake::H && c <= CSnake::W) )
			{
				continue;
			}

			//检查当前蛇头位置是否与蛇身重合(不包括蛇尾,因为蛇已经移动了)	
			int len = snake.length();
			//蛇的长度为2时,需要检查蛇的第二个节点(因为该节点也是蛇尾),是否与蛇头重合,
			//主要为了排除 上下/左右方向不能变换问题
			len = (len == 4 ? len : len - 2);
			int flag = 1;
			for(std::string::size_type i = 0; i < snake.length() - 2; i += 2)
			{
				if(r == snake[i] && c == snake[i + 1])	
				{
					flag = 0;
					break;
				}
			}
			
			if(flag == 0)
			{
				continue;
			}

			std::string newSnake; 
			newSnake += char(r);
			newSnake += char(c);
			newSnake += snake;
			newSnake.erase(newSnake.length() - 2, 2);

			if(visit_.find(newSnake) == visit_.end())			
			{
				printSnake(newSnake);
				printSnake(snake);
				//cout << endl;
				q.push(newSnake);
				visit_[newSnake] = snake;
			}
		}
	}
	return ret;
}

int CCleverAi::calcul()
{
	if(path_.empty() == false)
	{
		pair<int, int> d = path_.top();  path_.pop();
		for(int i = 0; i < 4; ++i)
		{
			if(d.first == CSnake::direction[i][0] &&
				d.second == CSnake::direction[i][1])
			{
				//cout << "dir " << i << endl;
				return i;
			}
		}
		ERROR_LOG("no this direction: (%d, %d)", d.first, d.second);
		return -1;
	}

	std::pair<int, int> appleLoc = game_.getAppleLoc();
	visit_.clear();
	int ret = bfs(appleLoc);	
	if(ret)
	{
		ERROR_LOG("bfs ret: %d", ret);
		return -2;
	}
	return calcul();
}


int CCleverAi::calculPath(const std::string& g)
{
	cout << "----------------" << endl;
	INFO_LOG("visit_ size: %d", (int)visit_.size());
	std::string goal = g;
	while(1)
	{
		std::map<std::string, std::string>::iterator iter = visit_.find(goal);
		if(iter == visit_.end())
		{
			ERROR_LOG("no find goal: %s", goal.c_str());
			return -1;
		}

		std::string preg = iter->second; 
		printSnake(goal);
		printSnake(preg);
		//cout << endl;
		if(preg == goal)
		{
			INFO_LOG("path len: %d", (int)path_.size());
			return 0;
		}
		
		int x = goal[0] - preg[0];
		int y = goal[1] - preg[1];
		path_.push(make_pair(x, y));	

		goal = preg;
	}		
	return -2;
}
