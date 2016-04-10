TOP=.
RSLIBS=utils
LIBS=$(foreach i, $(RSLIBS), $(TOP)/$(i)/lib$(notdir $(i)).a)
LDLIBS=-lpthread -lrt -lcrypto -lssl -lwiringPi
EXEC=relayserver
SRCS=$(shell ls *.cpp)
OBJS=$(SRCS:%.cpp=%.o)
CPPFLAGS+=-g -Wall -W

all: $(LIBS) $(OBJS)
	g++ -o $(EXEC) $(OBJS) $(LIBS) $(LDLIBS)

$(LIBS):
	make -C $(dir $@) lib

clean:
	$(foreach i, $(RSLIBS), rm -f $(TOP)/$(i)/*.o)
	rm -f $(LIBS)
	rm -f *.o
	rm -f $(EXEC)

