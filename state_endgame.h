#include "gamestate.h"
#include "graphics.h"
#include "queue.h"
#include "player.h"

// TODO generate interesting metrics about player...
struct EndGameStats
{
	Player * p;
	int cash;
	int stockWorth;
	int worth;
	EndGameStats(Player * p, int cash, int stockWorth)
		:p(p),cash(cash),stockWorth(stockWorth)
	{worth = cash+stockWorth;}
	EndGameStats(){worth=-1;}
	bool operator<(EndGameStats & other){
		return worth < other.worth;
	}
};

// TODO draw stock ownership graph, determine primary stockholdership, count mergers initiated
class State_EndGame : public GameState
{
	const char *reasonTheGameEnded, * detail;
	Queue<EndGameStats> stats;
public:
	State_EndGame(const char *reason, const char *detail):reasonTheGameEnded(reason),detail(detail){}
	virtual const char *getName(){return "End Game";}
	virtual void init(Game * g);
	virtual void draw(Graphics * g);
	virtual void processInput(int key);
	virtual void release(){stats.release();}
};