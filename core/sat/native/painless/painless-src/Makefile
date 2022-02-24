SRCS = $(shell find . -name "*.cpp")
LIB_SRCS = $(shell find . -name "*.cpp" -and -not -name "painless.cpp")

OBJS = $(addsuffix .o, $(basename $(SRCS)))
LIB_OBJS = $(addsuffix .o, $(basename $(LIB_SRCS)))

EXEC = painless
LIB = libpainless-lib-static.a

LIBS = -lmapleCOMSPS -L../mapleCOMSPS/build/release/lib/ \
       -lm4ri -L../mapleCOMSPS/m4ri-20140914/.libs \
       -lmapleChronoBT -L../mapleChronoBT/build/release/lib/ \
       -lpthread -lz -lm -static -flto

CXXFLAGS = -I../mapleCOMSPS -I../mapleCOMSPS/m4ri-20140914 \
           -I../mapleChronoBT -I. \
           -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS \
           -std=c++11 -O3 -D NDEBUG -flto

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(LIB): $(LIB_OBJS)
	ar rcs $(LIB) $(LIB_OBJS)

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LIBS)

all: $(EXEC) $(LIB)

clean:
	rm -f $(OBJS) $(EXEC) $(LIB)
