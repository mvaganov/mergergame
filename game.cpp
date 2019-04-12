#include "game.h"
#include "state_placetile.h"
#include "state_cursor.h"
#include "state_buystock.h"
#include "state_mergeoptions.h"
#include "state_mergerresolution.h"
#include "vector2.h"
#include "cli.h"
#include "mem.h"
#include <stdio.h>// sprintf_s

void Game::initStateMachine()
{
	queueState(NEWMEM(State_PlaceTile));
	m_stateQueue->peek()->init(this);
}

Game::Game(int numPlayers,int width, int height):m_running(true),userInput(0),turn(0),updates(0),inventoryPlayer(0){
	board = NEWMEM(Board(width, height));
	players = NEWMEM(Queue<Player>());
	players->setSizeUsed(numPlayers);
	Player * p;
	char *playerName;
	// give tiles to players
	for(int a = 0; a < 6; ++a)
	{
		for(int i = 0; i < players->size; ++i)
		{
			p = &players->list[i];
			Tile * nextTile = board->getRandomUnusedTile();
			if(nextTile)
				p->addTile(nextTile);
		}
	}
	// finish initializing players
	for(int i = 0; i < players->size; ++i)
	{
		playerName = NEWMEM_ARR(char, 16);
#ifndef __STDC_WANT_SECURE_LIB__
	sprintf(playerName,
#else
	sprintf_s(playerName,16, 
#endif
			"player%d", i);
		p = &players->list[i];
		p->setName(playerName);
		p->orderTiles();
	}
	companies = NEWMEM(Queue<TileGroup>());
	companies->setSizeUsed(8);
	const char *companyNames[] = {"??????","Xanatos","Stark","Umbrella","Tyrell","Wayne","LexCorp","Shinra"};
	int companyTier[] = {-1,2,2,1,1,1,0,0};
	int colors[] = {
		CLI::COLOR::DARK_GRAY,
		CLI::COLOR::CYAN,
		CLI::COLOR::YELLOW,
		CLI::COLOR::GREEN,
		CLI::COLOR::BRIGHT_YELLOW,
		CLI::COLOR::BRIGHT_CYAN,
		CLI::COLOR::BRIGHT_RED,
		CLI::COLOR::BRIGHT_MAGENTA
	};
	TileGroup * c;
	for(int i = 0; i < companies->size; ++i)
	{
		c = &companies->list[i];
		c->setup(companyNames[i], companyTier[i], colors[i]);
	}
	m_stateQueue = NEWMEM(Queue<GameState*>());
	currentPlayer = 0;
	companySelector = NEWMEM(CompanySelector(this,
		Vector2(board->getWidth()*Tile::DRAW_WIDTH, 0), companies));
	initStateMachine();
}

void Game::release()
{
	if(m_stateQueue)
	{
		// clear all game states that may be enqueued
		GameState * state;
		while(m_stateQueue->size > 0){
			state = m_stateQueue->pop();
			state->release();
			DELMEM(state);
		}
		DELMEM(m_stateQueue);
		m_stateQueue = 0;
	}
	if(players)
	{
		for(int i = 0; i < players->size; ++i)
		{
			DELMEM_ARR(players->list[i].getName());
		}
		DELMEM(players);
		players = 0;
	}
	DELMEM_CLEAN(companies);
	DELMEM_CLEAN(board);
	DELMEM_CLEAN(companySelector);
}

void Game::draw(Graphics * g)
{
	int boardW = board->getWidth()*Tile::DRAW_WIDTH;
	int boardH = board->getHeight()*Tile::DRAW_HEIGHT;
	int companySelectorHeight = 9;
	GameState * state = m_stateQueue->peek();
	Player * p = getCurrentPlayer();
	// clear the screen
	g->resetColor();
	g->fillScreen(' ');
	// draw the board
	board->draw(g);
	// draw the player's inventory
	Player * invPlayer = inventoryPlayer?inventoryPlayer:p;
	invPlayer->drawInventory(g, Vector2(boardW, companySelectorHeight));
	// draw the current state information
	g->move(boardH, 0);
	flashColor(2,g);
	g->printf("%s%s", getCurrentPlayer()->getName(), state->getName());
	// draw the company selector
	companySelector->draw(g);
	if(state)
	{
		state->draw(g);
	}
}

void Game::setInput(int a_input)
{
	switch(a_input){
	case 'w':	a_input = CLI::KEY::UP;		break;	// up
	case 'a':	a_input = CLI::KEY::LEFT;	break;	// left
	case 's':	a_input = CLI::KEY::DOWN;	break;	// down
	case 'd':	a_input = CLI::KEY::RIGHT;	break;	// right
	case CLI::KEY::ESCAPE:	// escape key. user wants to quit.
		m_running = false;
		break;
	}
	userInput = a_input;
}

void Game::processInput()
{
	if(m_stateQueue->size > 0)
		m_stateQueue->peek()->processInput(userInput);
	userInput = 0;
}

int Game::flashColor(int style, Graphics * g){
#define ARR_SIZE(a)		(sizeof(a)/sizeof(a[0]))
	// store a color-pair-int with the given forground and backgorund color
#define F_(f)	(CLI::COLOR::f)
#define FB_(f, b)	(CLI::COLOR::f | (CLI::COLOR::b << 4))
	static int WHITE_TEXT[] = {// white
			F_(DARK_GRAY),
			F_(LIGHT_GRAY),
			F_(LIGHT_GRAY),
			F_(WHITE),
			F_(WHITE),
			F_(WHITE),
			F_(WHITE),
			F_(WHITE),
			F_(LIGHT_GRAY),
			F_(LIGHT_GRAY),
			F_(DARK_GRAY),
		};
	static int WHITE_BACKGROUND[] = {// white background
			FB_(DARK_GRAY,DARK_GRAY),
			FB_(LIGHT_GRAY, DARK_GRAY),
			FB_(LIGHT_GRAY, DARK_GRAY),
			FB_(WHITE, DARK_GRAY),
			FB_(WHITE, DARK_GRAY),
			FB_(WHITE, DARK_GRAY),
			FB_(WHITE, DARK_GRAY),
			FB_(WHITE, DARK_GRAY),
			FB_(LIGHT_GRAY, DARK_GRAY),
			FB_(LIGHT_GRAY, DARK_GRAY),
			FB_(DARK_GRAY,DARK_GRAY),
		};
	static int GREEN_TEXT[] = {// green
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(GREEN, BLACK),
			FB_(BRIGHT_GREEN, DARK_GRAY),
			FB_(BRIGHT_GREEN, DARK_GRAY),
			FB_(BRIGHT_GREEN, DARK_GRAY),
			FB_(BRIGHT_GREEN, DARK_GRAY),
			FB_(BRIGHT_GREEN, DARK_GRAY),
			FB_(BRIGHT_GREEN, DARK_GRAY),
		};
	static int YELLOW_TEXT[] = {// yellow
			F_(DARK_GRAY),
			F_(YELLOW),
			F_(BRIGHT_YELLOW),
			F_(BRIGHT_YELLOW),
			F_(YELLOW),
			F_(DARK_GRAY),
		};
	static int MUTED_TEXT[] = {// muted text
			F_(BLACK),
			F_(BLACK),
			F_(BLACK),
			F_(BLACK),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(DARK_GRAY),
			F_(BLACK),
			F_(BLACK),
			F_(BLACK),
			F_(BLACK),
		};
	static int COLORS_SIZE[] = {
		ARR_SIZE(WHITE_TEXT),ARR_SIZE(WHITE_BACKGROUND),ARR_SIZE(GREEN_TEXT),
		ARR_SIZE(YELLOW_TEXT),ARR_SIZE(MUTED_TEXT)};
	static int * COLORS[] = {WHITE_TEXT,WHITE_BACKGROUND,GREEN_TEXT,YELLOW_TEXT,MUTED_TEXT,};

#undef ARR_SIZE
#undef F_
#undef FB_

	int c = COLORS[style][updates % COLORS_SIZE[style]];
	if(g){
		g->setColor(c & 0xf, (c >> 4) & 0xf);
	}
	return c;
}
