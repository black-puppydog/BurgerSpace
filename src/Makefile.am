# $Id: Makefile.am,v 1.20 2007/12/25 07:06:25 sarrazip Exp $

bin_PROGRAMS = burgerspace

burgerspace_SOURCES = \
	BurgerSpaceEngine.cpp \
	BurgerSpaceEngine.h \
	IngredientGroup.cpp \
	IngredientGroup.h \
	IngredientSprite.h \
	EnemySprite.cpp \
	EnemySprite.h \
	ChefController.h \
	util.h \
	sdlmain.cpp

burgerspace_LDADD = \
	$(FLATZEBRA_LIBS)

burgerspace_CXXFLAGS = \
	-DPKGSOUNDDIR=\"$(pkgsounddir)\" \
	-I$(top_srcdir)/src/images \
	$(FLATZEBRA_CFLAGS)

# Only test the --version option if GNU getopt is available:
if HAVE_GETOPT_LONG
check: all
	test "`./$(PACKAGE) --version`" = "$(PACKAGE) $(VERSION)"
	./$(PACKAGE) --version >& /dev/null
	./$(PACKAGE) --help >& /dev/null
endif

run: all check
	PKGSOUNDDIR=$(srcdir)/sounds ./$(PACKAGE)


desktopentrydir = $(datadir)/applications
desktopentry_DATA = $(PACKAGE).desktop

pixmapdir = $(datadir)/pixmaps
pixmap_DATA = images/$(PACKAGE).png


XPM_IMAGES = \
	images/bottom-bun.xpm \
	images/bottom-bun0.xpm \
	images/bottom-bun1.xpm \
	images/bottom-bun2.xpm \
	images/bottom-bun3.xpm \
	images/empty.xpm \
	images/floor-and-ladder.xpm \
	images/floor.xpm \
	images/ladder.xpm \
	images/plate.xpm \
	images/lettuce.xpm \
	images/lettuce0.xpm \
	images/lettuce1.xpm \
	images/lettuce2.xpm \
	images/lettuce3.xpm \
	images/yellowstuff.xpm \
	images/yellowstuff0.xpm \
	images/yellowstuff1.xpm \
	images/yellowstuff2.xpm \
	images/yellowstuff3.xpm \
	images/redstuff.xpm \
	images/redstuff0.xpm \
	images/redstuff1.xpm \
	images/redstuff2.xpm \
	images/redstuff3.xpm \
	images/meat.xpm \
	images/meat0.xpm \
	images/meat1.xpm \
	images/meat2.xpm \
	images/meat3.xpm \
	images/top-bun.xpm \
	images/top-bun0.xpm \
	images/top-bun1.xpm \
	images/top-bun2.xpm \
	images/top-bun3.xpm \
	images/chef-back0.xpm \
	images/chef-back1.xpm \
	images/chef-back2.xpm \
	images/chef-dead0.xpm \
	images/chef-dead1.xpm \
	images/chef-dead2.xpm \
	images/chef-dead3.xpm \
	images/chef-dead4.xpm \
	images/chef-dead5.xpm \
	images/chef-front0.xpm \
	images/chef-front1.xpm \
	images/chef-front2.xpm \
	images/chef-left0.xpm \
	images/chef-left1.xpm \
	images/chef-left2.xpm \
	images/chef-right0.xpm \
	images/chef-right1.xpm \
	images/chef-right2.xpm \
	images/egg-back0.xpm \
	images/egg-back1.xpm \
	images/egg-front0.xpm \
	images/egg-front1.xpm \
	images/egg-left0.xpm \
	images/egg-left1.xpm \
	images/egg-right0.xpm \
	images/egg-right1.xpm \
	images/egg-dead0.xpm \
	images/egg-dead1.xpm \
	images/egg-dead2.xpm \
	images/egg-dead3.xpm \
	images/egg-peppered0.xpm \
	images/egg-peppered1.xpm \
	images/hotdog-back0.xpm \
	images/hotdog-back1.xpm \
	images/hotdog-dead0.xpm \
	images/hotdog-dead1.xpm \
	images/hotdog-dead2.xpm \
	images/hotdog-dead3.xpm \
	images/hotdog-front0.xpm \
	images/hotdog-front1.xpm \
	images/hotdog-left0.xpm \
	images/hotdog-left1.xpm \
	images/hotdog-peppered0.xpm \
	images/hotdog-peppered1.xpm \
	images/hotdog-right0.xpm \
	images/hotdog-right1.xpm \
	images/pickle-back0.xpm \
	images/pickle-back1.xpm \
	images/pickle-dead0.xpm \
	images/pickle-dead1.xpm \
	images/pickle-dead2.xpm \
	images/pickle-dead3.xpm \
	images/pickle-front0.xpm \
	images/pickle-front1.xpm \
	images/pickle-left0.xpm \
	images/pickle-left1.xpm \
	images/pickle-peppered0.xpm \
	images/pickle-peppered1.xpm \
	images/pickle-right0.xpm \
	images/pickle-right1.xpm \
	images/pepper0.xpm \
	images/pepper1.xpm \
	images/icecream.xpm \
	images/coffee.xpm \
	images/fries.xpm \
	images/digit0.xpm \
	images/digit1.xpm \
	images/digit2.xpm \
	images/digit3.xpm \
	images/digit4.xpm \
	images/digit5.xpm \
	images/digit6.xpm \
	images/digit7.xpm \
	images/digit8.xpm \
	images/digit9.xpm

pkgsounddir = $(datadir)/sounds/$(PACKAGE)
pkgsound_DATA = \
	sounds/ingredient-bounces.wav \
	sounds/ingredient-in-plate.wav \
	sounds/ingredient-falls.wav \
	sounds/ingredient-lowered.wav \
	sounds/enemy-catches-chef.wav \
	sounds/enemy-paralyzed.wav \
	sounds/enemy-smashed.wav \
	sounds/chef-throws-pepper.wav \
	sounds/chef-gets-treat.wav \
	sounds/new-game-starts.wav \
	sounds/level-finished.wav \
	sounds/chef-shoots-blanks.wav \
	sounds/treat-appears.wav \
	sounds/treat-disappears.wav

EXTRA_DIST = \
	$(PACKAGE).desktop \
	images/$(PACKAGE).png \
	$(XPM_IMAGES) \
	$(pkgsound_DATA)

MAINTAINERCLEANFILES = Makefile.in
