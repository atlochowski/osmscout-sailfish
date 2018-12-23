# 
# Do NOT Edit the Auto-generated Part!
# Generated by: spectacle version 0.27
# 

Name:       harbour-osmscout

# >> macros

# ignore installed files that are not packed to rpm
%define _unpackaged_files_terminate_build 0

# don't setup rpm provides
%define __provides_exclude_from ^%{_datadir}/.*$

# don't setup rpm requires
# list here all the libraries your RPM installs
%define __requires_exclude ^ld-linux|libmarisa|libgomp|libosmscout.*$

# dont strip binaries - it causes segfault on Jolla phone (arm)
%global __os_install_post %{nil}

# << macros

Summary:    OSMScout for Sailfish
Version:    1.4
Release:    1
Group:      Qt/Qt
License:    GPLv2
URL:        https://github.com/Karry/osmscout-sailfish
Source0:    %{name}-%{version}.tar.bz2
Source100:  harbour-osmscout.yaml
Requires:   sailfishsilica-qt5 >= 0.10.9
Requires:   qt5-qtpositioning >= 5.2.0
Requires:   qt5-qtdeclarative-import-positioning >= 5.2.0
Requires:   libxml2 >= 2.9.1
BuildRequires:  pkgconfig(sailfishapp) >= 1.0.2
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Positioning)
BuildRequires:  pkgconfig(Qt5Sql)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  cmake
BuildRequires:  chrpath
BuildRequires:  desktop-file-utils

%description
OSM Scout is offline map viewer and routing application.


%prep
%setup -q -n %{name}-%{version}

# >> setup
# << setup

%build
# >> build pre
#rm -rf rpmbuilddir-%{_arch}
mkdir -p rpmbuilddir-%{_arch}

## for production build:
cd rpmbuilddir-%{_arch} && cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_RPATH=%{_datadir}/%{name}/lib/: ..
## for debug build, use these commands instead:
# cd rpmbuilddir-%{_arch} && cmake -DCMAKE_BUILD_TYPE=DEBUG -DQT_QML_DEBUG=yes -DSANITIZE=yes -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_RPATH=%{_datadir}/%{name}/lib/: ..

cd ..
make -C rpmbuilddir-%{_arch} VERBOSE=1 # %{?_smp_mflags}
# << build pre



# >> build post
# << build post

%install
# >> install pre
#rm -rf %{buildroot}
DESTDIR=%{buildroot} make -C rpmbuilddir-%{_arch} install
mkdir -p %{_bindir}
# << install pre

# >> install post
# << install post

desktop-file-install --delete-original       \
  --dir %{buildroot}%{_datadir}/applications             \
   %{buildroot}%{_datadir}/applications/*.desktop

# remove includes, we don't need it on Jolla phone
rm %{buildroot}%{_includedir}/osmscout/*.h
rm %{buildroot}%{_includedir}/osmscout/*/*.h
# remove desktop qml
rm %{buildroot}%{_datadir}/%{name}/qml/desktop.qml

## Jolla harbour rules:

# -- strip symbols app
strip %{buildroot}%{_bindir}/%{name}

# -- ship all shared unallowed libraries with the app
mkdir -p %{buildroot}%{_datadir}/%{name}/lib

# Jolla tablet (i486) build have to be linked to ld-linux.so.2, but it is not allowed 
# in Harbour! Until Jolla fix their rules, we need to ship ld lib with app.
%ifarch i486
  # I am little bit confused by about Jolla tablet x86 architecture. 
  # Cmake setup build with "-march=i686", target rpm package is i486 and rpm %{_arch} is i386
  cp /lib/ld-linux.so.2 %{buildroot}%{_datadir}/%{name}/lib
%endif

# check architecture
# file %{buildroot}%{_bindir}/harbour-osmscout
# file /usr/lib/libgomp.so.1
# file /usr/lib/libgomp.so.1.0.0

# support for OMP
cp /usr/lib/libgomp.so.1      %{buildroot}%{_datadir}/%{name}/lib/
#cp --preserve=links /usr/lib/libgomp.so.1.0.0  %{buildroot}%{_datadir}/%{name}/lib/

# -- shared libraries without executable flag
chmod -x %{buildroot}%{_datadir}/%{name}/lib/*

# -- check app rpath to find its libs
#patchelf --force-rpath --set-rpath '/usr/share/%{name}/lib:' %{buildroot}%{_bindir}/%{name}
chrpath -l %{buildroot}%{_bindir}/%{name}
ls -al     %{buildroot}%{_bindir}/%{name}
sha1sum    %{buildroot}%{_bindir}/%{name}

%files
%defattr(-,root,root,-)
%{_bindir}
%{_datadir}/%{name}
%{_datadir}/%{name}/lib/
%{_datadir}/applications/%{name}.desktop
%{_datadir}/%{name}/map-styles/
%{_datadir}/%{name}/map-icons/
%{_datadir}/icons/hicolor/86x86/apps/%{name}.png
%{_datadir}/icons/hicolor/108x108/apps/%{name}.png
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/icons/hicolor/256x256/apps/%{name}.png
# >> files
# << files
