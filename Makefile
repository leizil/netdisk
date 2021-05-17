OBJS:=$(wildcard *.c)
main:$(OBJS)
	gcc $^ -o $@ -lcrypt 
clean:
	rm main
