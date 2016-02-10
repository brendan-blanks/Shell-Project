PROGRAMS = bbsh
CFLAGS = -Wall -Wextra -std=gnu99
CC = gcc

# Already proving their worth!
all: $(PROGRAMS)

%.o : %.c
	$(CC) $(CFLAGS) -O3 $<

% : %.o
	$(CC) -o $@ $^

clean:
	rm -f *.o $(PROGRAMS) tmp


