# contrib/alex/Makefile

MODULE_big = alex
OBJS = \
	$(WIN32RES) \
	alcost.o \
	alinsert.o \
	alutils.o \
	alvacuum.o \
	alvalidate.o

EXTENSION = alex
DATA = alex--1.0.sql
PGFILEDESC = "alex access method - signature file based index"

REGRESS = alex

TAP_TESTS = 1

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/alex
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
