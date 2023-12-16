EXE = image_editor
FLAGS = -Wall -Wextra
VAL_FLAGS = --leak-check=full --show-leak-kinds=all --track-origin=yes

build:
	gcc src/main.c -o $(EXE) $(FLAGS)

buildd:
	gcc -g src/main.c -o $(EXE) $(FLAGS) -D MODE_DEBUG

run:
	make build
	./$(EXE)

rund:
	make buildd
	./$(EXE)

runval:
	make buildd
	valgrind $(VAL_FLAGS) ./$(EXE)
