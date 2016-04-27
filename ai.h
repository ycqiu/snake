#ifndef AI_H_
#define AI_H_

#include <iostream>
#include <string>
#include <stack>
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


class CCleverAi : public CAi
{
private:
	std::stack<std::pair<int, int> > path_;
	std::map<std::string, std::string> visit_;

public:
	CCleverAi(const CSnake& g) : CAi(g) {}

	virtual int calcul();	
	int bfs(std::pair<int, int> goal);
	int calculPath(const std::string& goal);
};

#endif 
