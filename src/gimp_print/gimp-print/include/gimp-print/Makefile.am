## $Id: Makefile.am,v 1.1.1.3 2004/07/23 06:26:27 jlovell Exp $
## Copyright (C) 2000 Roger Leigh
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

## Process this file with automake to produce Makefile.in.

AUTOMAKE_OPTIONS = 1.4 gnu

@SET_MAKE@

MAINT_CHARSET = latin1

include $(top_srcdir)/scripts/global.mk


## Data

noinst_HEADERS = gimp-print-intl.h gimp-print-intl-internal.h

pkginclude_HEADERS = \
	gimp-print.h \
	gimp-print-module.h \
	array.h \
	bit-ops.h \
	channel.h \
	color.h \
	curve-cache.h \
	curve.h \
	dither.h \
	sequence.h \
	image.h \
	list.h \
	module.h \
	mxml.h \
	paper.h \
	path.h \
	printers.h \
	sequence.h \
	string-list.h \
	util.h \
	vars.h \
	weave.h \
	xml.h

nodist_pkginclude_HEADERS = gimp-print-version.h


## Clean

DISTCLEANFILES = gimp-print-version.h
MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = gimp-print-version.h.in

TAGS_FILES = gimp-print-version.h.in
