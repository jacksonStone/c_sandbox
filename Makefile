CXX		  := g++
CXX_FLAGS := -g

BIN		:= bin
SRC		:= src
INCLUDE	:= include
LIB		:= lib

LIBRARIES	:=
EXECUTABLE	:= main.out


all: $(EXECUTABLE)

run: ./$(EXECUTABLE)

$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I $(INCLUDE) -L $(LIB) $^ -o $@ $(LIBRARIES)