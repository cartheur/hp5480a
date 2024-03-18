#-------------------------------------------
# INIT

SRCS=cp.c util.c attn.c gpioDummy.c
OBJS=$(SRCS:.c=.o)

CFLAGS = -Wall -DDEBUG -g


#-------------------------------------------
# TOP-LEVEL TARGETS

all: $(OBJS)

clean: 
	rm $(OBJS)

tar:
	tar cvzf util.tgz $(SRCS) *.h Makefile

zip:
	zip util.zip $(SRCS) *.h Makefile


#-------------------------------------------

$(OBJS): util.h cp.h attn.h gpioDummy.h
