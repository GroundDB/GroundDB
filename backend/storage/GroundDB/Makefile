#-------------------------------------------------------------------------
#
# Makefile--
#    Makefile for storage/GroundDB
#
# IDENTIFICATION
#    src/backend/storage/GroundDB/Makefile
#
#-------------------------------------------------------------------------

subdir = src/backend/storage/GroundDB
top_builddir = ../../../..
include $(top_builddir)/src/Makefile.global

OBJS = \
	allocate_page.o \
	mempool_server.o \
	rdma_client.o \
	rdma_server.o \
	read.o \
	util.o \
	write.o

include $(top_srcdir)/src/backend/common.mk
