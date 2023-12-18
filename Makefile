EXE = image_editor
FLAGS = -Wall -Wextra
VAL_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes

FILES = src/main.c src/defines.h src/cmd_utils.h src/cmd_utils.c src/file_utils.h src/file_utils.c src/histograms.h src/histograms.c src/crop.h src/crop.c

build: $(FILES)
	gcc $(FILES) -o $(EXE) $(FLAGS)

buildd: $(FILES)
	gcc -g $(FILES) -o $(EXE) $(FLAGS) -D MODE_DEBUG

run: $(FILES)
	make build
	./$(EXE)

rund: $(FILES)
	make buildd
	./$(EXE)

runval: $(FILES)
	make buildd
	valgrind $(VAL_FLAGS) ./$(EXE)
