C++ = g++
# uncomment and place here the EasyLocal++ installation path.
 EASYLOCALDIR = ../miniEasyLocal++

#OPTOPTS = -DNO_MINI
COMPOPTS = -I$(EASYLOCALDIR)/src $(OPTOPTS)
LINKOPTS = -L$(EASYLOCALDIR)/lib -lEasyLocal
WARNOPTS = -Wall 
SRCFILES = faculty.cpp solver.cpp main.cpp
OBJFILES = $(SRCFILES:%.cpp=%.o)

all: course_tt

course_tt: $(OBJFILES)
	$(C++) $(OBJFILES) $(LINKOPTS) -o $@

make.depend: $(SRCFILES)
	$(C++) -MM $(COMPOPTS) $^ > $@

%.o:%.cpp
	$(C++) $(COMPOPTS) $(WARNOPTS) -c $<

include make.depend

clean:
	rm -f $(OBJFILES) course_tt make.depend

