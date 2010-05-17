# $Id: Makefile.am,v 1.13.2.1 2009/06/21 02:02:34 sarrazip Exp $

SUBDIRS = src

man_MANS = doc/burgerspace.6 doc/burgerspace-server.6

docdir = $(datadir)/doc/$(PACKAGE)-$(VERSION)
doc_DATA = \
	AUTHORS \
	COPYING \
	INSTALL \
	NEWS \
	README \
	THANKS \
	TODO

RPMBUILD = rpmbuild
rpm: dist
	$(RPMBUILD) -ta $(distdir).tar.gz

EXTRA_DIST = \
	bootstrap \
	autogen.sh \
	$(PACKAGE).spec \
	$(man_MANS) \
	macros/ac_compile_warnings.m4 \
	macros/getopt_long.m4

ACLOCAL_AMFLAGS = -I macros

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	configure \
	config.guess \
	config.sub \
	depcomp \
	install-sh \
	missing \
	mkinstalldirs

