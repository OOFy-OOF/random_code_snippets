SRC_FILES=main.c
CFLAGS += -std=c99 -g -Wall -Wextra -Wno-missing-field-initializers

all: main

main: $(SRC_FILES)
	gcc $(CFLAGS) $(SRC_FILES) -lm -o main.out

clean:
	rm -f main.out

run: main
	# Running our main function from file main.c
	./main.out
