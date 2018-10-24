CC=g++
app=odb

SRC_DIR=./src
INCLUDE_DIR=-I./src

CPPFLAGS=-g -std=c++17 $(INCLUDE_DIR)
OBJ_DIR=./obj

LIBS=-lodbc

OBJS=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(wildcard $(SRC_DIR)/*.cpp)))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CPPFLAGS)

$(app):$(OBJS)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LIBS)

clean:
	rm -rf $(OBJS) $(app)
