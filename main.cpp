#include "game.h"
#include <iostream>
#include "cli.h"
#include "mem.h"
/*
    current player's turn
        |
    [place tile]
        |
    if tile placed next to another tile
        |        \
       false    true    
        |        |
        |        if no other adjacent tile 
        |        is company owned tile 
        |        |                    \
        |       true                 false            
        |        |                       \
        |    if new company placeable    if adjacent tile is company owned
        |    /             |                /                    \
        |false            true          false                    true
        | /                |              /                        \
        |/    [pick new company]        /      if 2 or more adjacent companies are the same size
        |            |                /        /                 |
        |    tile added to company  /      false                true
        |    along with connected /         /                    |
        |    tiles              /      if only 1 company        [user picks largest company]
        |      /              /         /               \            |
        |     /            /         true             false          |
        |    /         /             /                  |            /
        |  /      /                 /    biggest company will absorb smaller companies
        | /  /   tile added to company   determine first and second majority stockholders and pay bonuses
        |/       along with connected    each player with stock in companies being merged, starting with current player
        |        tiles                          |
		|			/						[merger options] x Number of players with stock
        |        /                           1. sell stock for calculated price
        |     /                              2. trade merged stock 2-for-1 for merged company
        |  /                                 3. keep options for later
        |/                                  /
		|                                /
		|                             /
		|                          /
		|                       /
    if companies exist with stock
        |        |
       false    true
        |        |
        |     [buy stock]
        |        |
    [intermission]
        |
    next player turn

	required user states:
	[place tile] - state_placetile
	[pick new company] - state_pickunplacedcompany
	[user picks largest company] - state_pickcontrollingcompany
	[merger options] - state_mergeoptions
	[buy stock] - state_buystock
	[intermission] - state_cursor
*/

int getNumber(const char *prompt, int minInc, int maxEx)
{
	int number;
	do
	{
		if(std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(1024,'\n');
		}
		std::cout << prompt << std::endl;
		std::cin >> number;
	}
	while(std::cin.fail() || number < minInc || number >= maxEx);
	return number;
}

int main(int argc, const char ** argv)
{
	CLI::init();
	CLI::setSize(-1, 24);
	CLI::setDoubleBuffered(true);
	int numPlayers = getNumber("How many players?", 1, 100);
	Game game(numPlayers, 12, 9);// Game(players, width, height)
	while(game.isRunning())
	{
		if(CLI::kbhit()){
			game.setInput(CLI::getcharBlocking());
		}else{
			game.update();
			game.draw(CLI::getCLI());
			CLI::move(CLI::getHeight()-1,0);
			CLI::refresh();
			time_t soon = CLI::upTimeMS()+50;
			while(CLI::upTimeMS() < soon && !CLI::kbhit()){CLI::sleep(1);}	// throttle code for CPU
		}
	}
	game.release();
	CLI::release();
	return 0;
}
