#include "state_cursor.h"
#include "state_placetile.h"
#include "game.h"
#include "mem.h"
#include "cli.h"

void State_Cursor::triggerNextTurn()
{
	m_game->nextTurn();
	m_game->queueState(NEWMEM(State_PlaceTile));
	setDone(true);
}

void State_Cursor::init(Game * g)
{
	GameState::init(g);

	// TODO remove this hacky feature disable, and write code so that 
	// state_cursor to provide rich board data
	triggerNextTurn();	// not using state_cursor. 
}

void State_Cursor::processInput(int key){
	Vector2 old = cursor;
	switch(key)
	{
	case CLI::KEY::UP:		cursor.y--;	break;
	case CLI::KEY::LEFT:	cursor.x--;	break;
	case CLI::KEY::DOWN:	cursor.y++;	break;
	case CLI::KEY::RIGHT:	cursor.x++;	break;
	case ' ':	triggerNextTurn();	return;
	}
	Board * b = m_game->getBoard();
	Vector2 c = cursor;
	if(c.x < 0 || c.x >= b->getWidth() || c.y < 0 || c.y >= b->getHeight())
	{
		cursor = old;
	}
}

void State_Cursor::draw(Graphics * g)
{
	if(!isDone())
	{
		Tile * t = m_game->getBoard()->getTile(cursor.x, cursor.y);
		const char *text = t->getCo()?t->getCo()->getName():Tile::DEFAULT_UNPLACED;
		t->draw(g, text, m_game->flashColor(1,g));
	}
}