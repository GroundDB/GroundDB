#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for utils/GroundDB
#
# IDENTIFICATION
#    src/backend/utils/GroundDB/Makefile
#
#-------------------------------------------------------------------------

subdir = src/backend/utils/GroundDB
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

OBJS = \
	hash.o \
	mutexlock.o

override CPPFLAGS += -DDLSUFFIX=\"$(DLSUFFIX)\"

include $(top_srcdir)/src/backend/common.mk
