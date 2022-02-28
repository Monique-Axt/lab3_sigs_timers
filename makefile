#   File:	makefile
#   Builds:	Lab3
#   Date:	September 2017
						
CCX = g++

CCFLAGS		=  -ansi -pedantic -std=c++11
#LDFLAGS		= -lrt
LDFLAGS	=

PROGS		= lab3
OBJS		= lab3.o TestTask.o Restart.o Random.o psignal.o
HDRS		= psignal.h Random.h TestTask.h
TEXT 		= tmp*.txt
.cc.o:
	@echo "Compiling " $<
	$(CCX) -c $(CCFLAGS) $<
	@echo $@ "done"

all:	$(PROGS)


lab3:	$(OBJS)
	@echo "Linking " $@
	$(CCX)  -o $@ $(OBJS) $(LDFLAGS)
	@echo $@ "done"

clean:
	$(RM) $(PROGS) $(OBJS) $(TEXT) core *~
