Name:		libstrophe
Version:	1
Release:	1%{?dist}_git
Summary:	xmpp library in C

Group:		Application/System
License:	MIT/GPLv3
URL:		http://strophe.im/libstrophe/
Source0:	libstrophe_git.tar.gz

BuildRequires:	libxml2-devel
Requires:		libxml2

%description
XMPP library in C

%prep
%setup -n libstrophe
cmake

%build
%configure
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%files
%{_libdir}/libstrophe.so
%{_includedir}/strophe.h

%changelog
