# -*- rpm-spec -*-

%define _prefix    @CMAKE_INSTALL_PREFIX@
%define pkg_prefix @BC_PACKAGE_NAME_PREFIX@

%define build_number 1


Name:           @CPACK_PACKAGE_NAME@
Version:        @PROJECT_VERSION@
Release:        %{build_number}%{?dist}
Summary:        BroadVoice(R)16 (BV16)


Group:          Applications/Audio
License:        LGPL
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
BroadVoice(R)16 (BV16)


%package        devel
Summary:        Headers, libraries and docs for the BV16 library
Group:          Development/Libraries

Requires:       %{name} = %{version}

%description    devel
BroadVoice(R)16 (BV16)

This package contains header files and development libraries needed to
develop programs using the BV16 library.


%if 0%{?rhel} && 0%{?rhel} <= 7
%global cmake_name cmake3
%define ctest_name ctest3
%else
%global cmake_name cmake
%define ctest_name ctest
%endif

# This is for debian builds where debug_package has to be manually specified, whereas in centos it does not
%define custom_debug_package %{!?_enable_debug_packages:%debug_package}%{?_enable_debug_package:%{nil}}
%custom_debug_package

%prep
%setup -n %{name}-%{version}

%build
%{expand:%%%cmake_name} . -DCMAKE_BUILD_TYPE=@CMAKE_BUILD_TYPE@ -DCMAKE_PREFIX_PATH:PATH=%{_prefix}
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%check
%{ctest_name} -V %{?_smp_mflags}

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root,-)
%if @ENABLE_SHARED@
%{_libdir}/*.so
%endif

%files devel
%defattr(-,root,root,-)
%{_includedir}/*/*/*.h
%if @ENABLE_STATIC@
%{_libdir}/*.a
%endif

%changelog

* Tue Nov 27 2018 ronan.abhamon <ronan.abhamon@belledonne-communications.com>
- Do not set CMAKE_INSTALL_LIBDIR and never with _libdir!

* Tue Mar 13 2018 ronan.abhamon <ronan.abhamon@belledonne-communications.com>
- Initial RPM release.
