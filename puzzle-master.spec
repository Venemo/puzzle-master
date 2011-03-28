Name:           puzzle-master
Version:        1.2.2
Release:        1%{?dist}
Summary:        Fun and addictive jigsaw puzzle game

Group:          Amusements/Games
License:        EUPL 1.1
URL:            http://gitorious.org/colorful-apps/puzzle-master
Source0:        http://sources.venemo.net/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: qt-devel       
Requires: qt

%description 
This is a jigsaw puzzle game that lets you use your own
images (and contains some built-in ones) for generating puzzles.
You can decide the size and thus the difficulty of the puzzle.

This application is licensed under the terms of the
European Union Public License (EUPL) version 1.1
http://www.osor.eu/eupl

%prep
%setup -q

%build
qmake-qt4
make

%install
rm -rf $RPM_BUILD_ROOT
make INSTALL_ROOT=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_bindir}/puzzle-master
%{_datadir}/applications/puzzle-master.desktop
%{_datadir}/pixmaps/puzzle-master.png

%changelog
* Sun Mar 27 2011 Timur Kristóf <venemo@msn.com> 1.2.2-1
- Initial version of the package in RPM
