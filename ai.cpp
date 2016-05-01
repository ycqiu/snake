#include <iostream>
#include <queue>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "ai.h"
#include "log.h"

using namespace std;


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
int CCleverAi::bfs(std::pair<int, int> goal, const std::string& snk,
	   	std::map<std::string, std::string>& visit, std::vector<std::vector<int> >& lvMp, int num)
{
	int ret = 0;
	initLevelMap(lvMp);
	std::priority_queue<CSnakeInfo> q;
	CSnakeInfo info(snk, 1, 0);
	q.push(info);
	visit[info.snake_] = info.snake_;
	++lvMp[info.snake_[0]][info.snake_[1]];
	while(q.empty() == false)
	{
		info = q.top();  q.pop();
		//DEBUG_LOG("level: %d, step: %d", info.level_, info.step_);
		const std::string snake = info.snake_;
		if(snake[0] == goal.first &&
				snake[1] == goal.second)
		{
			if(num == 1)
			{
				INFO_LOG("sec succ");
				return ret = 0;
			}
			
			//DEBUG_LOG("first succ");
			if(num == 0)
			{
				std::map<std::string, std::string> vs;
				std::vector<std::vector<int> > lv;

				std::string nextSnake;
			    nextSnake += char(goal.first); 
				nextSnake += char(goal.second);
				nextSnake += visit[snake];
				
				int len = nextSnake.length();
				if(len == 2)
				{
					ERROR_LOG("not find nextSnake");
					return -1;	
				}
				std::pair<int, int> tail(nextSnake[len - 2], nextSnake[len - 1]); 
				//CSnake::printSnake(nextSnake, "nextSnake");
				//INFO_LOG("sec begin");
				INFO_LOG("tail pos: (%d, %d)", tail.first, tail.second);
				ret = bfs(tail, nextSnake, vs, lv, num + 1);
				INFO_LOG("sec ret: %d", ret);
				if(ret < 0)
				{
					//INFO_LOG("sec fail");
					continue;	
				}
			}
			
			if(num == 1)
			{
				//sleep(2);
			}

			ret = calculPath(snake, visit);
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
			for(int i = 0; i < len; i += 2)
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

			if(visit.find(newSnake) == visit.end())			
			{
				//CSnake::printSnake(newSnake);
				//CSnake::printSnake(snake);
				//cout << endl;
				q.push(CSnakeInfo(newSnake, ++lvMp[r][c], info.step_ + 1));
				visit[newSnake] = snake;
			}
		}
	}
	return ret = -2;
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
	int ret = bfs(appleLoc, game_.getSnake(), visit_, levelMap_, 0);	
	if(ret)
	{
		ERROR_LOG("bfs ret: %d", ret);
		return -2;
	}
	return calcul();
}


int CCleverAi::calculPath(const std::string& g, std::map<std::string, std::string>& vis)
{
//	cout << "----------------" << endl;
	INFO_LOG("visite size: %d", (int)vis.size());
	std::string goal = g;
	while(1)
	{
		std::map<std::string, std::string>::iterator iter = vis.find(goal);
		if(iter == vis.end())
		{
			ERROR_LOG("no find goal: %s", goal.c_str());
			return -1;
		} 
		std::string preg = iter->second; 
		//CSnake::printSnake(goal);
		//CSnake::printSnake(preg);
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

void CCleverAi::initLevelMap(std::vector<std::vector<int> >& lvMp)
{
	lvMp.clear();
	lvMp.resize(CSnake::H + 1);
	for(int i = 0; i < CSnake::H + 1; ++i)
	{
		lvMp[i].resize(CSnake::W + 1);	
	}
}
	
