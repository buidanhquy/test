.PHONY: all clean
include custom.mk

APP_DIR:=.

APP:=extract_file

SRCS:=$(APP).c

HT_DIR:=../hash-table
HT_SRCS:=hash-table.c

CMM_DIR:=../common
CMM_SRCS:=ansi-utils.c my-pjlib-utils.c

LIBUT_DIR:=../libut

CNT_DIR:=../group-man

HT_DIR:=../hash-table
HT_SRCS:=hash-table.c

LIBUT_DIR:=../libut

CONST_DIR:=../group-man

O_DIR:=../object-pool
O_SRCS:=object-pool.c

PER_DIR:=../permission
PER_SRCS:=permission.c

CFLAGS:=-DPJ_AUTOCONF=1 -O2 -DPJ_IS_BIG_ENDIAN=0 -DPJ_IS_LITTLE_ENDIAN=1 -fms-extensions
CFLAGS+=-I$(CMM_DIR)/include
CFLAGS+=-I$(LIBS_DIR)/include
CFLAGS+=-I$(LIBS_DIR)/include/json-c
CFLAGS+=-I$(APP_DIR)/include
CFLAGS+=-I$(CNT_DIR)/include
CFLAGS+=-I$(O_DIR)/include
CFLAGS+=-I$(HT_DIR)/include
CFLAGS+=-I$(LIBUT_DIR)/include
CFLAGS+=-I$(CONST_DIR)/include
CFLAGS+=-I$(PER_DIR)/include

CFLAGS+=-D__ICS_INTEL__

all: $(APP)

$(APP): $(SRCS:.c=.o) $(CMM_SRCS:.c=.o) $(HT_SRCS:.c=.o) $(O_SRCS:.c=.o) $(PER_SRCS:.c=.o)
	$(CROSS_TOOL) -o $@ $^ $(LIBS)

$(SRCS:.c=.o): %.o: $(APP_DIR)/%.c
	$(CROSS_TOOL) -c -o $@ $^ $(CFLAGS)
$(CMM_SRCS:.c=.o): %.o: $(CMM_DIR)/src/%.c
	$(CROSS_TOOL) -c -o $@ $^ $(CFLAGS)
$(HT_SRCS:.c=.o) : %.o : $(HT_DIR)/src/%.c
	$(CROSS_TOOL) -o $@ -c $< $(CFLAGS)

$(O_SRCS:.c=.o): %.o: $(O_DIR)/src/%.c
	gcc -c -o $@ $^ $(CFLAGS)

$(PER_SRCS:.c=.o): %.o: $(PER_DIR)/src/%.c
	gcc -c -o $@ $^ $(CFLAGS)

clean:
	rm -fr *.o gen gen-pri $(C_APP) $(S_APP)
