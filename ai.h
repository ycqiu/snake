#ifndef AI_H_
#define AI_H_

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <cmath>
#include "snake.h"

using namespace std;


class CAi
{
protected:
	const CSnake& game_;

public:
	CAi(const CSnake& g) : game_(g) {}
	virtual int calcul();	
};


struct CSnakeInfo
{
	std::string snake_;
	int level_;
	int step_;
	int curve_;   //转弯数
	int preDir_;  //上一次移动的方向
	std::pair<int, int> goal_;

	CSnakeInfo(const std::string& a, int l, int s, int c, int d, std::pair<int, int> g) :
		snake_(a), level_(l), step_(s), curve_(c), preDir_(d), goal_(g) {}

	int dist() const
	{
		return std::abs(snake_[0] - goal_.first) + std::abs(snake_[1] - goal_.second);	
	}

	bool operator<(const CSnakeInfo& b) const
	{
		if(curve_ == b.curve_)
		{
			if(level_ == b.level_)
			{
				//return step_ >= b.step_;
				return step_ + dist() >= b.step_ + b.dist();
			}
			return  level_ > b.level_;
		}

		return curve_ > b.curve_;	
	}
};


class CCleverAi : public CAi
{
private:
	std::stack<std::pair<int, int> > path_;
	std::map<std::string, std::string > visit_;
	std::vector<std::vector<int> > levelMap_;

public:
	CCleverAi(const CSnake& g) : CAi(g) {}

	virtual int calcul();	
	void initLevelMap(std::vector<std::vector<int> >&);
	int bfs(std::pair<int, int> goal, const std::string& snake,
		   	std::map<std::string, std::string>&, std::vector<std::vector<int> >&, int num);
	int calculPath(const std::string& goal, std::map<std::string, std::string>&);
	int check(const std::string& snake);
};

//仅仅是个演示而已
class CNormalAi : public CAi
{
public:
	CNormalAi(const CSnake& g) : CAi(g) {}
	virtual int calcul();	
};


struct CHeadInfo
{
	int step_;	
	std::pair<int, int> head_;
	std::pair<int, int> goal_;

	CHeadInfo(int s, std::pair<int, int> h, std::pair<int, int> g) :
		step_(s), head_(h), goal_(g) {}

	int dist() const
	{
		return std::abs(head_.first - goal_.first) + std::abs(head_.second - goal_.second);	
	}


	bool operator<(const CHeadInfo& b) const
	{
		return step_ + dist() >= b.step_ + b.dist();
	}

	bool operator>(const CHeadInfo& b) const
	{
		return step_ + dist() <= b.step_ + b.dist();
	}

};


//静态搜索,减少状态空间,提高效率
class CEfficientAi : public CAi
{
private:
	std::stack<std::pair<int, int> > path_;
	
public:
	CEfficientAi(const CSnake& g) : CAi(g) {}
	virtual int calcul();	
	int fill(const std::string& snake, std::vector<std::string>& plat);
	int getPath(std::pair<int, int> goal, const std::map<std::pair<int, int>, std::pair<int, int> >&,
			std::stack<std::pair<int, int> >& path);
	int getDir(std::pair<int, int>);
	int go(std::string& snk, std::pair<int, int> apple, const std::stack<std::pair<int, int> >& path);

	//template <class T, class Container, class Compare > 
	int bfs(std::string& snk, std::pair<int, int> goal, std::stack<std::pair<int, int> >& path);
};

#endif 
