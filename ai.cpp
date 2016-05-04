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
	visit.clear();
	initLevelMap(lvMp);
	std::priority_queue<CSnakeInfo> q;
	CSnakeInfo info(snk, 1, 0, -1, -1, goal);
	q.push(info);
	visit[info.snake_] = info.snake_;
	++lvMp[info.snake_[0]][info.snake_[1]];
	int result = -1;
	while(q.empty() == false)
	{
		info = q.top();  q.pop();
		const std::string& snake = info.snake_;
		if(snake[0] == goal.first &&
				snake[1] == goal.second)
		{
			if(num != 0)
			{
				return 0;
			}

			if(num == 0)  
			{
				std::string nextSnake;
			    nextSnake += char(goal.first); 
				nextSnake += char(goal.second);
				nextSnake += visit[snake];
				
				ret = check(nextSnake);
				if(ret == 0)
				{
					result = 2;	
				}
				else if(ret < 0)
				{
					if(result >= 1)
					{
						continue;
					}

					std::map<std::string, std::string> vs;
					std::vector<std::vector<int> > lv;
					int len = nextSnake.length();
					std::pair<int, int> tail(nextSnake[len - 2], nextSnake[len - 1]); 
					ret = bfs(tail, nextSnake, vs, lv, num + 1);
					if(ret >= 0)
					{
						result = 1;
					}
					else
					{
						if(result >= 0)
						{
							continue;
						}
						result = 0;
					}
				}
			}

			ret = calculPath(snake, visit);
			if(ret < 0)
			{
				ERROR_LOG("bfs ok, but calculPath ret: %d", ret);
				return ret = -1;
			}

			if(result != 2)
			{
				continue;
			}

			return result; 
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
			for(int j = 0; j < len; j += 2)
			{
				if(r == snake[j] && c == snake[j + 1])	
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
				int curve = (info.preDir_ != i ? info.curve_ + 1 : info.curve_);
				q.push(CSnakeInfo(newSnake, ++lvMp[r][c], info.step_ + 1, curve, i, goal));
				visit[newSnake] = snake;
			}
		}
	}
	
	if(result >= 0)
	{
		INFO_LOG("bfs result: %d", result);
		return result;
	}
	return ret = -2;
}


int CCleverAi::check(const std::string& snake)
{
	int len = snake.length();
	if(len == 2)
	{
		ERROR_LOG("not find nextSnake");
		return -1;	
	}
	
	std::pair<int, int> head(snake[0], snake[1]);
	std::pair<int, int> tail(snake[len - 2], snake[len - 1]); 
	std::vector<std::vector<int> > plat;
	initLevelMap(plat);
	for(int i = 2; i < len - 2; i += 2)
	{
		int r = snake[i];
		int c = snake[i + 1];
		plat[r][c] = 1; 
	}

	std::queue<std::pair<int, int> > q;
	q.push(head);
	plat[head.first][head.second] = 1;
	while(q.empty() == false)
	{
		head = q.front();  q.pop();
		if(head == tail)	
		{
			return 0;
		}

		for(int i = 0; i < 4; ++i)	
		{
			int r = head.first + CSnake::direction[i][0];	
			int c = head.second + CSnake::direction[i][1];	

			if( !(r >= 1 && c >= 1 && r <= CSnake::H && c <= CSnake::W) )
			{
				continue;
			}
	
			if(plat[r][c] != 0) 	
			{
				continue;
			}
		
			plat[r][c] = 1;
			q.push(make_pair(r, c));
		}
	}

	return -2;
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
				return i;
			}
		}
		ERROR_LOG("no this direction: (%d, %d)", d.first, d.second);
		return -1;
	}

	std::pair<int, int> appleLoc = game_.getAppleLoc();
	int ret = bfs(appleLoc, game_.getSnake(), visit_, levelMap_, 0);	
	if(ret < 0)
	{
		ERROR_LOG("bfs ret: %d", ret);
		return -2;
	}
	return calcul();
}


int CCleverAi::calculPath(const std::string& g, std::map<std::string, std::string>& vis)
{
	while(path_.empty() == false)
	{
		path_.pop();	
	}

	INFO_LOG("visite size: %d", (int)vis.size());
	std::string goal = g;
	while(1)
	{
		std::map<std::string, std::string>::iterator iter = vis.find(goal);
		if(iter == vis.end())
		{
			ERROR_LOG("not find goal: %s", goal.c_str());
			return -1;
		} 
		std::string preg = iter->second; 
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
	



/////////////////
int CNormalAi::calcul()	
{
	int dir = game_.getDir();
	std::string snake = game_.getSnake();
	std::pair<int, int> snakeLoc = std::make_pair(snake[0], snake[1]);
	if(snakeLoc.first == 1)	
	{
		if(snakeLoc.second == CSnake::W)
		{
			return CSnake::DOWN;
		}
		return CSnake::RIGHT;			
	}
		
	if(dir == CSnake::DOWN)
	{
		if(snakeLoc.first == CSnake::H)
		{
			return CSnake::LEFT;
		}
		return  dir;
	}

	if(dir == CSnake::LEFT)
	{
		if(snakeLoc.first == 2)
		{
			return  CSnake::DOWN;
		}
		return  CSnake::UP;
	}

	if(dir == CSnake::UP)	
	{
		if(snakeLoc.first == 2)	
		{
			if(snakeLoc.second != 1)	
			{
				return CSnake::LEFT;
			}
		}
		return dir;
	}
	return -1;
}

