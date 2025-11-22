NAME = CLOCK
DESCRIPTION = "Dashboard for the Ti-84 Plus CE"

SOURCES = dashboard.cpp


CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

include $(shell cedev-config --makefile)
