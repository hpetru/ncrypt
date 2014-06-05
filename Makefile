CC=c++
CFLAGS=-std=c++11 -g

PR=ncrypt_public

all: clean compile run

clean:
	rm -rf $(PR)

compile:
	$(CC) $(CFLAGS) $(PR).cpp -o $(PR)

run:
	./$(PR)

