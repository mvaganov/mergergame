CPP_FILES = \
board.cpp \
cli.cpp \
companyselector.cpp \
game.cpp \
main.cpp \
mem.cpp \
player.cpp \
state_buystock.cpp \
state_cursor.cpp \
state_endgame.cpp \
state_mergeoptions.cpp \
state_mergerresolution.cpp \
state_pickcontrollingcompany.cpp \
state_pickunplacedcompany.cpp \
state_placetile.cpp \
tile.cpp \
tilegroup.cpp

OUTPUT = mergergame

all:
	echo "-------------------------------------------------"
	g++ $(CPP_FILES) -o $(OUTPUT)
