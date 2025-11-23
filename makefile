NAME = CLOCK
DESCRIPTION = "Dashboard for the Ti-84 Plus CE"

SOURCES = src/main.c src/weather.c src/scene.c src/feature.c

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

include $(shell cedev-config --makefile)
