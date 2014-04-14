Name:           harbour-puzzle-master
Version:        2.5.2
Release:        1%{?dist}
Summary:        Fun jigsaw puzzle game

Group:          Amusements/Games
License:        GPLv2+
URL:            http://puzzle-master.colorful.hu/

Source0:        %{name}-%{version}.tar.gz

BuildRequires: pkgconfig(Qt5Core), pkgconfig(Qt5Gui), pkgconfig(Qt5Quick)
BuildRequires: desktop-file-utils

# >> macros
%{!?qtc_qmake5:%define qtc_qmake5 qmake}
%{!?qtc_qmake:%define qtc_qmake qmake}
%{!?qtc_make:%define qtc_make make}
%{?qtc_builddir:%define _builddir %qtc_builddir}
# << macros

%description
%{name} is a jigsaw puzzle game that lets you use your own
images (and contains some built-in ones) for generating puzzles.
You can decide the size and the difficulty of the puzzle.

%prep
%setup -q

%build
%qtc_qmake5 DEFINES+=PUZZLE_MASTER_SAILFISH
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
%defattr(-,root,root,-)
%{_bindir}/%{name}
%attr(644,root,root) %{_datadir}/applications/%{name}.desktop
%attr(644,root,root) %{_datadir}/icons/hicolor/86x86/apps/%{name}.png

%changelog
* Mon Apr 14 2014 Timur Kristóf <venemo@fedoraproject.org> 2.5.1-1
- Added advanced difficulty mode
* Tue Dec 10 2013 Timur Kristóf <venemo@fedoraproject.org> 2.5.0-1
- Sailfish release

