PROGRAM   := anti_mtp_demo

SRCDIRS   := ./src ../src/C++

DEBUG=1
CXXFLAGS  += -I./src  -I../include/C++ -Wall -g -Wno-deprecated -fno-strict-aliasing -std=c++11  -pthread

ifeq ($(DEBUG), 0)
	CXXFLAGS += -O2
endif


LDFLAGS   += -ldl -lrt

CC      = gcc

CXX=g++

CC      = $(CXX)

CFLAGS  = $(CXXFLAGS)

RM        = rm -f

SRCEXTS   := .c .cpp

SHELL   = /bin/sh

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))

OBJS    = $(foreach x,$(SRCEXTS), \
      $(patsubst %$(x),%.o,$(filter %$(x),$(SOURCES))))

DEPS    = $(patsubst %.o,%.d,$(OBJS))

.PHONY : all objs clean cleanall rebuild

all : $(PROGRAM)

# Rules for creating the dependency files (.d).

#---------------------------------------------------

%.d : %.c

	@$(CC) -MM -MD $(CFLAGS) $<


%.d : %.C

	@$(CC) -MM -MD $(CXXFLAGS) $<


%.d : %.cc

	@$(CC) -MM -MD $(CXXFLAGS) $<


%.d : %.cpp

	@$(CC) -MM -MD $(CXXFLAGS) $< -o $@


%.d : %.CPP

	@$(CC) -MM -MD $(CXXFLAGS) $<


%.d : %.c++

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cp

	@$(CC) -MM -MD $(CXXFLAGS) $<



%.d : %.cxx

	@$(CC) -MM -MD $(CXXFLAGS) $<



# Rules for producing the objects.

#---------------------------------------------------

objs : $(OBJS)

%.o : %.c

	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@


%.o : %.cpp

	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@


# Rules for producing the executable.

#----------------------------------------------

$(PROGRAM) : $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)




-include $(DEPS)



rebuild: clean all



clean :

	@$(RM) *.o src/*.o src/*.d *.d $(OBJS)



cleanall: clean

	@$(RM) $(PROGRAM) $(PROGRAM).exe



### End of the Makefile ##  Suggestions are welcome  ## All rights reserved ###

###############################################################################
