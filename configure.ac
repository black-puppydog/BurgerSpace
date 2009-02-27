# $Id: configure.ac,v 1.12 2009/02/27 02:50:42 sarrazip Exp $

AC_PREREQ(2.50)
AC_INIT(src/BurgerSpaceEngine.h)
AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE(burgerspace, 1.8.3)


# Checks for programs.

AC_PROG_CXX
#AC_COMPILE_WARNINGS
AC_LANG_CPLUSPLUS


# Checks for libraries.

MINVER_FLATZEBRA=0.1.2;		AC_SUBST(MINVER_FLATZEBRA)
PKG_CHECK_MODULES(FLATZEBRA, flatzebra-0.1 >= $MINVER_FLATZEBRA)


# Check for getopt_long() but don't fail if it is not available:
GETOPT_LONG


AC_OUTPUT([
	Makefile
	burgerspace.spec
	src/Makefile
])
