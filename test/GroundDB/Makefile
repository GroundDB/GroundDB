#-------------------------------------------------------------------------
#
# Makefile for GroundDB
#
# Copyright (c) 2003-2020, PostgreSQL Global Development Group
#
# src/test/GroundDB/Makefile
#
#-------------------------------------------------------------------------

subdir = src/test/GroundDB
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global

BACKEND_SUBDIRS = storage/DSMEngine storage/GroundDB utils/GroundDB

SUBDIRS = $(BACKEND_SUBDIRS:%=$(top_builddir)/src/backend/%)

include $(top_srcdir)/src/backend/common.mk

OBJS = \
	$(SUBDIROBJS) \
	$(top_srcdir)/src/backend/port/port_posix.o \
	GroundDB_test.o

all: GroundDB_test

GroundDB_test: $(OBJS)
	$(CC) $(CFLAGS) $(call expand_subsys,$^) $(LDFLAGS) $(LDFLAGS_EX) $(LIBS) -o $@
