CC = gcc

PRGNAME = mock3d
IDIR = include 
BUILDDIR = build
TESTDIR = src/tests

LIBRARY_SRC = 
SOURCE_DIRS = $(LIBRARY_SRC) src 
TESTS = 

CFLAGS = -std=c2x -Wall -Wextra -fstack-protector -I $(IDIR) -lSDL2 -lSDL2_image
LDFLAGS = -I $(IDIR) -L$(BUILDDIR) -lSDL2 -lSDL2_image -lm
OPTIMIZATION = -Og

TESTPATH = $(BUILDDIR)/$(TESTDIR)
SOURCES = $(foreach dir, $(SOURCE_DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c, $(BUILDDIR)/%.o, $(SOURCES))

all: tree exec

tree:
	@printf "Building directory tree\n"
	@mkdir -p $(TESTPATH)/$$dir;
	@for dir in $(SOURCE_DIRS); do \
		mkdir -p $(BUILDDIR)/$$dir; \
	done

release: OPTIMIZATION=-O3
release: tree exec
test: tree $(TESTS)

exec: $(OBJS)
	@printf "Building Executable\n"
	@printf "\e[1;35m(LD)\e[m $(BUILDDIR)/$(PRGNAME)"
	@$(CC) -o $(BUILDDIR)/$(PRGNAME) $^ $(OPIMIZATION) $(LDFLAGS)
	@printf " [DONE]\n"


$(TESTS): $(OBJS)
	@printf "(CC) $(TESTPATH)/$@.o\n"
	@$(CC) -c -o $(TESTPATH)/$@.o $(TESTDIR)/$@.c $(OPTIMIZATION) $(CFLAGS)
	@printf "\e[1;32m(LD)\e[m $(TESTPATH)/$@\n"
	@$(CC) -o $(TESTPATH)/$@ $(TESTPATH)/$@.o $(OPIMIZATION) $(LDFLAGS)

$(BUILDDIR)/%.o: %.c
	@printf "(CC) $@"
	@$(CC) -c -o $@ $^ $(OPTIMIZATION) $(CFLAGS)
	@printf " [DONE]\n"


.PHONY: clean tree library static dynamic

clean:
	rm -rf $(BUILDDIR)
