#pragma once
#include "queue.h"
#include "vector2.h"
#include "graphics.h"

//#define RIGGED_GAME

class TileGroup;

class Tile
{
public:
	static const int DRAW_WIDTH = 3, DRAW_HEIGHT = 2;
	static const char *DEFAULT_UNPLACED;
	static const char *DEFAULT_PLACEMENT;
private:
	Vector2 position;
	int turnPlaced;
	TileGroup * company;
public:
	Tile():turnPlaced(-1),company(0){}

	bool isPlaced(){return turnPlaced >= 0 || company;}

	int getTurnPlaced(){return turnPlaced;}
	void setPlacement(int turn){
		turnPlaced = turn;
	}

	Vector2 getPosition(){return position;}
	void setPosition(Vector2 const & columnAndRow){position = columnAndRow;}

	int getX(){return position.x;}
	int getY(){return position.y;}

	void setCo(TileGroup * co){company=co;}
	/** @return the company at this tile */
	TileGroup * getCo(){return company;}

	bool operator<(Tile & other){
		return position.x <  other.position.x
			|| position.x == other.position.x && position.y < other.position.y;
	}
	void draw(Graphics * g, const char *text, int color);
	void draw(Graphics * g);
};
