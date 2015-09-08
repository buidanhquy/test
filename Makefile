.PHONY: clean all

APP_NAME:=test
SRCS:=test.c
OBJS:=$(SRCS:.c=.o)

all: $(APP_NAME)

$(APP_NAME): $(OBJS)
	gcc -o $@ $^

%.o: %.c
	gcc -c $<

clean:
	rm -fr $(APP_NAME) $(OBJS)
