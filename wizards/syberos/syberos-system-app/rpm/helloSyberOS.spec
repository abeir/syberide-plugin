Name:	    helloSyberOS	
Version:	1.0
Release:	1%{?dist}
Summary:	a example application for SyberOS.
Group:		Applications/System
License:	GPL
URL:		https://syberos.dev.com/
Source0:	%{name}-%{version}.tar.gz

BuildRequires:  pkgconfig(Qt5Core)


%description
A example application for SyberOS\

%prep
%setup -q -n %{name}-%{version}


%build
%if "%{BUILD_TYPE}" == "release"
%qmake5 CONFIG+=release
%else
%qmake5 CONFIG+=debug
%endif

make

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin/

# >> install pre
# << install pre
install -D -m 744 helloSyberOS %{buildroot}/usr/bin/


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%{_bindir}/helloSyberOS


