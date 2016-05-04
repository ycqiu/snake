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



//////////////////////
int CEfficientAi::fill(const std::string& snake, std::vector<std::string>& plat)
{
	plat.resize(CSnake::H + 1);	
	for(std::vector<std::string>::size_type i = 0; i < plat.size(); ++i)
	{
		plat[i].assign(CSnake::W + 1, ' ');	
	}
	
	for(std::string::size_type i = 0; i < snake.length(); i += 2)
	{
		int r = snake[i];
		int c = snake[i + 1];
		plat[r][c] = '*';
	}	
	return 0;
}

//int CEfficientAi::bfs(std::string& snk, std::pair<int, int> goal, std::stack<std::pair<int, int> >& path)
template<class T>
int CEfficientAi::bfs(std::string& snk, std::pair<int, int> goal, std::stack<std::pair<int, int> >& path)
{
	int ret = 0;
	std::vector<std::string> plat;
	fill(snk, plat);
	plat[goal.first][goal.second] = ' ';

	//std::priority_queue<CHeadInfo> q;
	T q;
	std::pair<int, int> head(snk[0], snk[1]);
	q.push(CHeadInfo(0, head, goal));	
	plat[head.first][head.second] = '*';

	std::map<std::pair<int, int>, std::pair<int, int> > pathMap;
	pathMap[head] = head;

	while(q.empty() == false)
	{
		CHeadInfo info = q.top();   q.pop();	

		if(info.head_ == goal)
		{
			ret = getPath(goal, pathMap, path);
			if(ret < 0)
			{
				ERROR_LOG("getPath ret: %d", ret);
				return ret = -1;
			}
			return ret;
		}

		for(int i = 0; i < 4; ++i)
		{
			int r = info.head_.first + CSnake::direction[i][0];	
			int c = info.head_.second + CSnake::direction[i][1];	

			if( !(r >= 1 && c >= 1 && r <= CSnake::H && c <= CSnake::W) )
			{
				continue;
			}

			if(plat[r][c] != ' ')
			{
				continue;
			}

			plat[r][c] = '*';
			std::pair<int, int> next = make_pair(r, c);
			pathMap[next] = info.head_;
			q.push(CHeadInfo(info.step_ + 1, next, goal));
		}
	}
	return -2;
}

int CEfficientAi::getPath(std::pair<int, int> goal, const std::map<std::pair<int, int>, std::pair<int, int> >& pathMap,
			std::stack<std::pair<int, int> >& path)
{
	while(1)
	{
		std::map<std::pair<int, int>, std::pair<int, int> >::const_iterator iter = pathMap.find(goal);
		if(iter == pathMap.end())
		{
			ERROR_LOG("not find goal: (%d, %d)", goal.first, goal.second);
			return -1;
		} 
		std::pair<int, int> preg = iter->second; 
		if(preg == goal)
		{
			return 0;
		}
		
		int x = goal.first - preg.first;
		int y = goal.second - preg.second;
		path.push(make_pair(x, y));	

		goal = preg;
	}		
	return -2;
}


int CEfficientAi::calcul()	
{
	int ret = -1;
	if(nearPath_.empty() == false)
	{
		pair<int, int> d = nearPath_.top();  nearPath_.pop();
		int res = getDir(d);
		if(res < 0)
		{
			ERROR_LOG("no this direction: (%d, %d)", d.first, d.second);
			return -4;
		}
		return res;
	}

	std::pair<int, int> appleLoc = game_.getAppleLoc();
	std::string snake = game_.getSnake();
	//ret = bfs<CMaxHeap>(snake, appleLoc, nearPath_);	
	ret = bfs<CMinHeap>(snake, appleLoc, nearPath_);	
	
	if(ret >= 0)
	{
		INFO_LOG("bfs apple succ");
		std::string newSnake = game_.getSnake();
		std::stack<std::pair<int, int> > pathCopy = nearPath_;		
		ret = go(newSnake, appleLoc, pathCopy);
		if(ret < 0)
		{
			ERROR_LOG("go ret: %d", ret);
			return -1;
		}

		int len = newSnake.length();
		INFO_LOG("newSnake len: %d", len);
		std::pair<int, int> tail(newSnake[len -2], newSnake[len -1]);
		ret = bfs<CMinHeap>(newSnake, tail, pathCopy);
		if(ret < 0)
		{
			INFO_LOG("newSnake find tail fial: head(%d, %d), tail(%d, %d)", 
					newSnake[0], newSnake[1], tail.first, tail.second);
		}
		else
		{		
			clearPath(farPath_);
			return calcul();
		}
	}

	if(ret < 0)
	{
		//寻找蛇尾巴
		clearPath(nearPath_);
		snake = game_.getSnake();
		int len = snake.length();
		std::pair<int, int> tail(snake[len -2], snake[len -1]);
		ret = bfs<CMaxHeap>(snake, tail, farPath_);
		DEBUG_LOG("bfs tail");
		if(ret < 0)
		{
			ERROR_LOG("find tail fial: head(%d, %d), tail(%d, %d)", 
					snake[0], snake[1], tail.first, tail.second);
			return -3;
		}
		else
		{
			if(farPath_.empty() == false)
			{
				pair<int, int> d = farPath_.top();  farPath_.pop();
				DEBUG_LOG("far not empty");

				int res = getDir(d);
				if(res < 0)
				{
					ERROR_LOG("no this direction: (%d, %d)", d.first, d.second);
					return -4;
				}
				return res;
			}
		}
	}

	return -5;
}

int CEfficientAi::getDir(std::pair<int, int> d)
{
	for(int i = 0; i < 4; ++i)
	{
		if(d.first == CSnake::direction[i][0] &&
				d.second == CSnake::direction[i][1])
		{
			return i;
		}
	}
	return -1;
}


int CEfficientAi::go(std::string& snk, std::pair<int, int> apple, const std::stack<std::pair<int, int> >& path)
{
	std::stack<std::pair<int, int> > pathCopy = path;
	while(pathCopy.empty() == false)
	{
		std::pair<int, int> dir = pathCopy.top();  pathCopy.pop();
		int r = snk[0] + dir.first;	
		int c = snk[1] + dir.second; 

		if( !(r >= 1 && c >= 1 && r <= CSnake::H && c <= CSnake::W) )
		{
			ERROR_LOG("hit wall");
			return -1;
		}

		for(std::string::size_type i = 0; i < snk.length() - 2; i += 2)
		{
			if(r == snk[i] && c == snk[i + 1])	
			{
				ERROR_LOG("eat self");
				return -2;
			}
		}

		std::string pos; 
		pos += char(r);
		pos += char(c);
		snk.insert(0, pos);
		if(!(r == apple.first && c == apple.second))
		{
			snk.erase(snk.length() - 2, 2);
		}
	} 
	return 0;
}


void CEfficientAi::clearPath(std::stack<std::pair<int, int> >& path)
{
	while(path.empty() == false)
	{
		path.pop();
	}
}

