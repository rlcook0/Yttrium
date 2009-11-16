include ../Make.defaults

# Indicates compilator to use
CC      = g++

# Specifies compilator options
CFLAGS  = -O3 -Wall -I$(OPENCV_INC_DIR) -D LINUX
LDFLAGS = 
LDLIBS  = -L$(OPENCV_LIB_DIR)

#MORE_LIBS=-ltiff -lpthread -lpng -lm
#GTK_LIBS=$(shell pkg-config gtk+ --libs)
#GT_LIBS=$(shell pkg-config gthread-2.0 --libs)
CV_LIBS=-lcxcore -lcv -lhighgui -lcvaux -lml
LIBS=$(MORE_LIBS) $(GT_LIBS) $(GTK_LIBS) $(CV_LIBS) $(XML_LIB)



# Files extensions .cpp, .o
SUFFIXES = .cpp .o 
.SUFFIXES: $(SUFFIXES) .

# Name of the main program
PROG  = surf

# Object files .o necessary to build the main program
OBJS  = fasthessian.o integral.o main.o surf.o utils.o ipoint.o
 
all: $(PROG)

# Compilation and link
$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS) $(LDLIBS) $(LIBS)

.cpp.o:
	$(CC)   $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	-rm -f $(PROG)
	-rm -f *.o
