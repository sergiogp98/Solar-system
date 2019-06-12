INC = ./include/
SRC = ./src/
BIN = ./bin/
OS = Linux/
EXE = solar_system

all : $(EXE) execute

$(EXE) : $(SRC)$(EXE).c $(SRC)GL_utilities.c $(SRC)VectorUtils3.c $(SRC)loadobj.c $(SRC)LoadTGA.c $(SRC)$(OS)MicroGlut.c 
	gcc -Wall -Wmisleading-indentation -o $(BIN)$(EXE) -I$(INC) -I$(INC)$(OS) -DGL_GLEXT_PROTOTYPES $(SRC)$(EXE).c $(SRC)GL_utilities.c $(SRC)loadobj.c $(SRC)VectorUtils3.c $(SRC)LoadTGA.c $(SRC)$(OS)MicroGlut.c -lXt -lX11 -lGL -lm

execute : 
	$(BIN)$(EXE)
clean :
	rm $(EXE)
