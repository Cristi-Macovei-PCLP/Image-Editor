EXE = image_editor
FLAGS = -lm -Wall -Wextra
VAL_FLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes

FILES = src/main.c src/defines.h src/cmd_utils.h src/cmd_utils.c src/file_utils.h src/file_utils.c src/histograms.h src/histograms.c src/crop.h src/crop.c src/equalize.h src/equalize.c src/math_utils.h src/math_utils.c src/apply_param.h src/apply_param.c src/rotate.h src/rotate.c

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

rund1:
	make buildd
	./$(EXE) < tasks/image_editor/tests/27-image_editor/27-image_editor.in 2>debug.log

runval: $(FILES)
	make buildd
	valgrind $(VAL_FLAGS) ./$(EXE)

runval1: $(FILES)
	make buildd
	valgrind $(VAL_FLAGS) ./$(EXE) < tasks/image_editor/tests/13-image_editor/13-image_editor.in

clean:
	rm -f image_editor
