all: server client
%: %.c
	clang -Wall -Wextra -Wconversion -g -o $@ $^