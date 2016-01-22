.PHONY: all clean
#APP:= test_port
#APP:=log_file
APP:=rsa
#APP:ansi_log_file

MAIN_DIR:= .
MAIN_SRCS:= $(APP).c

LIBS := $(shell pkg-config libpjproject --libs) -lcrypto

CFLAGS := $(shell pkg-config libpjproject --cflags) -I$(C_DIR)/include -I$(OPOOL_DIR)/include

all: $(APP)

$(APP): $(C_SRCS:.c=.o) $(MAIN_SRCS:.c=.o)
	gcc -o $@ $^ $(LIBS)

$(C_SRCS:.c=.o): %.o: $(C_DIR)/src/%.c
	gcc -o $@ -c $< $(CFLAGS)

$(MAIN_SRCS:.c=.o): %.o: $(MAIN_DIR)/%.c
	gcc -o $@ -c $< $(CFLAGS)

clean:
	rm -fr *.o $(APP)
