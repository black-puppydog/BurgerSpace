# $Id: configure.ac,v 1.11.2.11 2010/05/17 01:17:23 sarrazip Exp $
# configure.ac for burgerspace

AC_PREREQ(2.50)
AC_INIT(src/BurgerSpaceClient.h)
AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(burgerspace, 1.9.0)

MANUAL_DATE_EN="May 16th, 2010"; AC_SUBST(MANUAL_DATE_EN)
MANUAL_DATE_FR="16 mai 2010"; AC_SUBST(MANUAL_DATE_FR)  # UTF-8

PACKAGE_FULL_NAME="BurgerSpace"
PACKAGE_SUMMARY_EN="A hamburger-smashing video game"
PACKAGE_SUMMARY_FR="Un jeu de hamburgers qu'on écrase"  # UTF-8
AC_SUBST(PACKAGE_FULL_NAME)
AC_SUBST(PACKAGE_SUMMARY_EN)
AC_SUBST(PACKAGE_SUMMARY_FR)


AC_DISABLE_STATIC
AC_PROG_LIBTOOL  dnl This must come after AC_DISABLE_STATIC.


# Checks for programs.

AC_PROG_CXX
AC_COMPILE_WARNINGS
AC_LANG_CPLUSPLUS


# Checks for libraries.

MINVER_FLATZEBRA=0.1.5; AC_SUBST(MINVER_FLATZEBRA)
PKG_CHECK_MODULES(FLATZEBRA, flatzebra-0.1 >= $MINVER_FLATZEBRA)


# Check for getopt_long() but don't fail if it is not available:
GETOPT_LONG


# Configuration options.

build_network=no

AC_ARG_WITH([network],
[  --with-network          build network (client-server) version (default: yes)],
[
	if test "$withval" = no; then
		build_network=no
	else
		build_network=yes
	fi
],
[
	build_network=yes
])


# Allow specification of default UDP port.

AC_MSG_RESULT([building network version: $build_network])
AM_CONDITIONAL(BUILD_NETWORK, [test "$build_network" = yes])

DEFAULT_UDP_SERVER_PORT=49152  # private as per IANA
AC_ARG_WITH([default-udp-port],
[  --with-default-udp-port  default UDP port number on which to accept clients (default: $DEFAULT_UDP_SERVER_PORT)],
[
	if test "$withval" = no; then
		true
	else
		DEFAULT_UDP_SERVER_PORT="$withval"
	fi
]
[
	true
])

AC_SUBST(DEFAULT_UDP_SERVER_PORT)
AC_MSG_RESULT([using $DEFAULT_UDP_SERVER_PORT as default UDP listening port])


# Generate files.

AC_OUTPUT([
	Makefile
	burgerspace.spec
	src/Makefile
	src/burgerspace.desktop
	doc/burgerspace.6
	doc/burgerspace-server.6
])
