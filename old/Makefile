#OBJS specifies which files to compile as part of the project
OBJS = pong.cpp 
#CC specifies which compiler we're using
CC = g++

DEBUG_FLAG = -g

#COMPILER_FLAGS specifies the additional compilation options we're using
#-w suppresses all warnings lol
COMPILER_FLAGS = -std=c++11 -w

#INCLUDE_FLAGS specifies the additional include file locations
INCLUDE_FLAGS = /usr/include/SDL2
#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#OBJ_NAME specifies the name of our executable
OBJ_NAME = pong 
#This is the target that compiles our executable
debug : $(OBJS)
	$(CC) $(OBJS) $(DEBUG_FLAG) $(COMPILER_FLAGS) -I $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) -I $(INCLUDE_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
