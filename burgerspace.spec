# $Id: burgerspace.spec.in,v 1.17 2007/12/22 23:03:35 sarrazip Exp $
# Spec file for burgerspace

# Release number can be specified with rpm --define 'rel SOMETHING' ...
# If no such --define is used, the release number is 1.
#
# Source archive's extension can be specified with rpm --define 'srcext .foo'
# where .foo is the source archive's actual extension.
# To compile an RPM from a .bz2 source archive, give the command
#   rpmbuild -ta --define 'srcext .bz2' burgerspace-1.8.4.tar.bz2
#
%if %{?rel:0}%{!?rel:1}
%define rel 1
%endif
%if %{?srcext:0}%{!?srcext:1}
%define srcext .gz
%endif

Summary: A Burgertime(TM) clone
Name: burgerspace
Version: 1.8.4
Release: %{rel}
License: GPL
Group: Amusements/Games
Source: %{name}-%{version}.tar%{srcext}
URL: http://sarrazip.com/dev/%{name}.html
Prefix: /usr
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires:	flatzebra	>= 0.1.4
BuildRequires:	flatzebra-devel	>= 0.1.4

%description
BurgerSpace is a game in which you are a chef, and must walk over
hamburger ingredients (buns, meat, lettuce, etc) to make them fall from
floor to floor, until they end up on the plates at the bottom of the
screen. It requires the SDL multimedia library. It is a clone of the
1982 BurgerTime video game by Bally Midway.

%description -l fr
BurgerSpace est un jeu où vous êtes un chef qui doit marcher sur des
ingrédients de hamburger (pain, viande, lettue, etc) pour les faire
tomber d'étage en étage, jusqu'à ce qu'ils se retrouvent dans les
assiettes au bas de l'écran.  Il s'agit d'un clone du jeu BurgerTime
de Bally Midmay datant de 1982.

%prep
%setup -q

%build
%configure
make %{?_smp_mflags}

%install
rm -fR $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="%{__install} -p"

%clean
rm -fR $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%{_bindir}/*
%{_datadir}/pixmaps/*
%{_datadir}/sounds/*
%{_datadir}/applications/*
%{_mandir}/man*/*
%doc %{_defaultdocdir}/*
