#pragma once

#include "tile.h"
#include "queue.h"
#include "graphics.h"

class Board
{
private:
	int width, height;
	Tile * map;
	Queue<Tile*> * unusedTiles;
public:
	int getWidth(){return width;}
	int getHeight(){return height;}
	int countUnusuedTiles(){return unusedTiles->size;}

	Tile * getTile(int x, int y)
	{
		if(x < 0 || x >= width || y < 0 || y >= height)
			return 0;
		return &map[x+y*width];
	}
	Board(int width, int height);

	Tile * getRandomUnusedTile();

	~Board();

	void draw(Graphics * g);

	/**
	 * @param x
	 * @param y
	 * @param target which company to change
	 * @param replacement which company to change it to
	 * @param output if given, this will revert the fill after completing it, and output will be a list of all locations modified
	 * @return number of tiles changed by this fill
	 * algorithm snatched from wikipedia, with some adjustments
	 */
	int floodFill(int x, int y, TileGroup * target, TileGroup * replacement, Queue<Step> * output)
	{
	//Flood-fill (node, target-color, replacement-color):
		int changes = 0;
	// 1. Set Q to the empty queue.
		Queue<Step> Q;
	// 2. Add node to the end of Q.
		Q.enqueue(Step(x,y,0));
	// 4. While Q is not empty:
		while(Q.size > 0)
		{
	// 5.     Set n equal to the last element of Q.
	// 7.     Remove last element from Q.
			Step n = Q.pop();
	// 8.     If the color of n is equal to target-color:
			// if the node is on the map
			if(n.y >= 0 && n.y < height && n.x >= 0 && n.x < width
			// and the node is the mark expected
			&& getTile(n.x, n.y)->getCo() == target
			// and the mark has not been found yet
			//&& results->indexOf(n) < 0
			)
			{
				changes++;
	// 9.         Set the color of n to replacement-color.
				getTile(n.x, n.y)->setCo(replacement);
				//map[n.y][n.x] = replacement;
				if(output)
					output->enqueue(n);
	// 10.        Add west node to end of Q.
				Q.enqueue(Step(n.x-1, n.y, n.step+1));
	// 11.        Add east node to end of Q.
				Q.enqueue(Step(n.x+1, n.y, n.step+1));
	// 12.        Add north node to end of Q.
				Q.enqueue(Step(n.x, n.y-1, n.step+1));
	// 13.        Add south node to end of Q.
				Q.enqueue(Step(n.x, n.y+1, n.step+1));
			}
		}
		if(output)
		{
			for(int i = 0; i < output->size; ++i)
			{
				getTile(output->list[i].x, output->list[i].y)->setCo(target);
			}
		}
	// 14. Return.
		return changes;
	}
};

