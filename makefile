# using makefile from Earwig (Ben Kurtovic) and Josh (Hypersonic)

PROGRAM = carve
SOURCES = src
BUILD   = bin

CC     = clang
FLAGS  = -O2 -Wall -Wextra -pedantic -std=c11
CFLAGS = $(shell sdl2-config --cflags)
LIBS   = $(shell sdl2-config --libs) -lSDL2_image -lSDL2_ttf
MKDIR  = mkdir -p
RM     = rm -rf

BNRY = $(PROGRAM)
SDRS = $(shell find $(SOURCES) -type d | xargs echo)
SRCS = $(filter-out %.inc.c,$(foreach d,$(SDRS),$(wildcard $(addprefix $(d)/*,.c))))
OBJS = $(patsubst %.c,%.o,$(addprefix $(BUILD)/$(MODE)/,$(SRCS)))
DEPS = $(OBJS:%.o=%.d)
DIRS = $(sort $(dir $(OBJS)))


.PHONY: all clean run

all: $(BNRY)

clean:
	$(RM) $(BUILD) $(PROGRAM) $(PROGRAM)$(DEVEXT)

run: $(BNRY)
	./$(BNRY)

$(DIRS):
	$(MKDIR) $@

$(BNRY): $(OBJS)
	$(CC) $(FLAGS) $(LIBS) $(OBJS) -o $@

$(OBJS): | $(DIRS)

$(BUILD)/$(MODE)/%.o: %.c
	$(CC) $(FLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
