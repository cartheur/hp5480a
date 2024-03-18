#-------------------------------------------
# INIT

ifeq ($(mode),standalone)
  GPIO_OBJ = ../util/gpioDummy.o
else
  GPIO_OBJ = -l wiringPi -lrt
endif

SRCS=main.c ifDSA.c
OBJS=$(SRCS:.c=.o)

UTIL_OBJS=../util/cp.o ../util/util.o ../util/attn.o

CFLAGS = -Wall -DDEBUG -g


#-------------------------------------------
# TOP-LEVEL TARGETS

all: ctl5480

install: all
	mv ctl5480 ~/bin/

clean: 
	rm  $(OBJS) ctl5480

tar:
	tar cvzf ctl5480.tgz $(SRCS) *.h Makefile


#-------------------------------------------

ctl5480: $(OBJS)
	$(CC) $(CFLAGS) -o ctl5480 main.o ifDSA.o $(UTIL_OBJS) $(GPIO_OBJ)

$(OBJS): ifDSA.h ../util/cp.h ../util/util.h
