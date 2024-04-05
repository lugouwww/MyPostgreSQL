# contrib/contrib-global.mk

NO_PGXS = 1

PG_CFLAGS = -lstdc++ -lz

PG_CXXFLAGS = -mpopcnt -lz

include $(top_srcdir)/src/makefiles/pgxs.mk
