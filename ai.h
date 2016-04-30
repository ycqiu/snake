#ifndef AI_H_
#define AI_H_

#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include "snake.h"

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

	CSnakeInfo(const std::string& a = "", int l = 0, int s = 0) :
		snake_(a), level_(l), step_(s) {}

	bool operator<(const CSnakeInfo& b) const
	{
		if(level_ != b.level_)
		{
			return  level_ > b.level_;
		}
		//return step_ <= b.step_;
		return step_ >= b.step_;
	}
};


class CCleverAi : public CAi
{
private:
	std::stack<std::pair<int, int> > path_;
	std::map<std::string, std::string> visit_;
	std::vector<std::vector<int> > levelMap_;

public:
	CCleverAi(const CSnake& g) : CAi(g) {}

	virtual int calcul();	
	void initLevelMap(std::vector<std::vector<int> >&);
	int bfs(std::pair<int, int> goal, const std::string& snake,
		   	std::map<std::string, std::string>&, std::vector<std::vector<int> >&, int num);
	int calculPath(const std::string& goal, std::map<std::string, std::string>&);
};

#endif 
