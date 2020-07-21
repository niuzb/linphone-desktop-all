# -*- rpm-spec -*-

%define _prefix    @CMAKE_INSTALL_PREFIX@
%define pkg_prefix @BC_PACKAGE_NAME_PREFIX@

%define build_number 5

%define _lib_name soci

%define _is_set() %1 == "YES" || %1 == "ON" || %1 == "TRUE" || %1 == "1" || %1 == "Y"

%if %{_is_set "@WITH_DB2@"}
  %define _with_db2 1
%endif
%if %{_is_set "@WITH_FIREBIRD@"}
  %define _with_firebird 1
%endif
%if %{_is_set "@WITH_MYSQL@"}
  %define _with_mysql 1
%endif
%if %{_is_set "@WITH_ODBC@"}
  %define _with_odbc 1
%endif
%if %{_is_set "@WITH_ORACLE@"}
  %define _with_oracle 1
%endif
%if %{_is_set "@WITH_POSTGRESQL@"}
  %define _with_postgresql 1
%endif
%if %{_is_set "@WITH_SQLITE3@"}
  %define _with_sqlite3 1
%endif

Name:           @CPACK_PACKAGE_NAME@
Version:        @PROJECT_VERSION@
Release:        %{build_number}%{?dist}
Summary:        The database access library for C++ programmers


Group:          System Environment/Libraries
License:        Boost
URL:            http://soci.sourceforge.net/
Source0:        %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-buildroot

%description
%{name} is a C++ database access library that provides the
illusion of embedding SQL in regular C++ code, staying entirely within
the C++ standard.


%{?_with_sqlite3:%package        sqlite3
Summary:        SQLite3 back-end for %{name}
Group:          System Environment/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
BuildRequires:  sqlite-devel

%description    sqlite3
This package contains the SQLite3 back-end for %{name}, i.e.,
dynamic library specific to the SQLite3 database. If you would like to
use %{name} in your programs with SQLite3, you will need to
install %{name}-sqlite3.}


%{?_with_mysql:%package        mysql
Summary:        MySQL back-end for %{name}
Group:          System Environment/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
BuildRequires:  mysql-devel

%description    mysql
This package contains the MySQL back-end for %{name}, i.e.,
dynamic library specific to the MySQL database. If you would like to
use %{name} in your programs with MySQL, you will need to
install %{name}-mysql.}


%{?_with_postgresql:%package        postgresql
Summary:        PostGreSQL back-end for %{name}
Group:          System Environment/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
BuildRequires:  postgresql-devel

%description    postgresql
This package contains the PostGreSQL back-end for %{name}, i.e.,
dynamic library specific to the PostGreSQL database. If you would like
to use %{name} in your programs with PostGreSQL, you will need to
install %{name}-postgresql.}


%{?_with_odbc:%package        odbc
Summary:        ODBC back-end for %{name}
Group:          System Environment/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
BuildRequires:  unixODBC-devel

%description    odbc
This package contains the ODBC back-end for %{name}, i.e.,
dynamic library specific to the ODBC connectors. If you would like to
use %{name} in your programs with ODBC, you will need to
install %{name}-odbc.}


%{?_with_oracle:%package        oracle
Summary:        Oracle back-end for %{name}
Group:          System Environment/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    oracle
This package contains the Oracle back-end for %{name}, i.e.,
dynamic library specific to the Oracle database. If you would like to
use %{name} in your programs with Oracle, you will need to install
%{name}-oracle.}


%package        devel
Summary:        Header files, libraries and development documentation for %{name}
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       pkgconfig

%description    devel
This package contains the header files, dynamic libraries and
development documentation for %{name}. If you would like to develop
programs using %{name}, you will need to install %{name}-devel.


%{?_with_sqlite3:%package        sqlite3-devel
Summary:        SQLite3 back-end for %{name}
Group:          Development/Libraries
Requires:       %{name}-devel = %{version}-%{release}
Requires:       %{name}-sqlite3 = %{version}-%{release}
Requires:       sqlite-devel

%description    sqlite3-devel
This package contains the SQLite3 back-end for %{name}, i.e., header
files and dynamic libraries specific to the SQLite3 database. If you
would like to develop programs using %{name} and SQLite3, you will need
to install %{name}-sqlite3.}


%{?_with_mysql:%package        mysql-devel
Summary:        MySQL back-end for %{name}
Group:          Development/Libraries
Requires:       %{name}-devel = %{version}-%{release}
Requires:       %{name}-mysql = %{version}-%{release}
Requires:       mysql-devel

%description    mysql-devel
This package contains the MySQL back-end for %{name}, i.e., header
files and dynamic libraries specific to the MySQL database. If you
would like to develop programs using %{name} and MySQL, you will need
to install %{name}-mysql.}


%{?_with_postgresql:%package        postgresql-devel
Summary:        PostGreSQL back-end for %{name}
Group:          Development/Libraries
Requires:       %{name}-devel = %{version}-%{release}
Requires:       %{name}-postgresql = %{version}-%{release}
Requires:       postgresql-devel

%description    postgresql-devel
This package contains the PostGreSQL back-end for %{name}, i.e., header
files and dynamic libraries specific to the PostGreSQL database. If
you would like to develop programs using %{name} and PostGreSQL, you
will need to install %{name}-postgresql.}


%{?_with_odbc:%package        odbc-devel
Summary:        ODBC back-end for %{name}
Group:          Development/Libraries
Requires:       %{name}-devel = %{version}-%{release}
Requires:       %{name}-odbc = %{version}-%{release}
Requires:       unixODBC-devel

%description    odbc-devel
This package contains the Odbc back-end for %{name}, i.e., header
files and dynamic libraries specific to the Odbc database. If you
would like to develop programs using %{name} and Odbc, you will need
to install %{name}-odbc.}


%{?_with_oracle:%package        oracle-devel
Summary:        Oracle back-end for %{name}
Group:          Development/Libraries
Requires:       %{name}-devel = %{version}-%{release}
Requires:       %{name}-oracle = %{version}-%{release}

%description    oracle-devel
This package contains the Oracle back-end for %{name}, i.e., header
files and dynamic libraries specific to the Oracle database. If you
would like to develop programs using %{name} and Oracle, you will need
to install %{name}-oracle.}


%package        doc
Summary:        HTML documentation for the %{name} library
Group:          Documentation
%if 0%{?fedora} || 0%{?rhel} > 5
BuildArch:      noarch
%endif
#BuildRequires:  tex(latex)
#BuildRequires:  doxygen, ghostscript

%description    doc
This package contains the documentation in the HTML format of the %{name}
library. The documentation is the same as at the %{name} web page.


%if 0%{?rhel} && 0%{?rhel} <= 7
%global cmake_name cmake3
%else
%global cmake_name cmake
%endif

%prep
%setup -n %{name}-%{version}

mv README.md README
mv CHANGES ChangeLog
echo "2017-12-06:" > NEWS
echo "- Version 4.0.0" >> NEWS
echo "- See the ChangeLog file for more details." >> NEWS

%build
%{expand:%%%cmake_name} . \
  -DCMAKE_BUILD_TYPE=@CMAKE_BUILD_TYPE@ \
  -DCMAKE_PREFIX_PATH:PATH=%{_prefix} \
  -DSOCI_CXX_C11=ON \
  -DSOCI_SHARED=ON \
  -DSOCI_EMPTY=OFF \
  -DSOCI_TESTS=OFF \
  -DWITH_DB2=@WITH_DB2@ \
  -DWITH_FIREBIRD=@WITH_FIREBIRD@ \
  -DWITH_MYSQL=@WITH_MYSQL@ \
  -DWITH_ODBC=@WITH_ODBC@ \
  -DWITH_ORACLE=@WITH_ORACLE@ \
  -DWITH_POSTGRESQL=@WITH_POSTGRESQL@ \
  -DWITH_SQLITE3=@WITH_SQLITE3@
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}
#  Remove unpackaged files from the buildroot
rm -f $RPM_BUILD_ROOT%{_libdir}/*.a
rm -f $RPM_BUILD_ROOT%{_includedir}/soci/soci-config.h.in
rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/empty

%{!?_with_db2:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/db2}
%{!?_with_firebird:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/firebird}
%{!?_with_mysql:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/mysql}
%{!?_with_odbc:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/odbc}
%{!?_with_oracle:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/oracle}
%{!?_with_postgresql:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/postgresql}
%{!?_with_sqlite3:rm -rf $RPM_BUILD_ROOT%{_includedir}/soci/sqlite3}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_core.so.*
%{?_with_empty:%{_libdir}/lib%{_lib_name}_empty.so.*}

%{?_with_sqlite3:%files sqlite3
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_sqlite3.so.*}

%{?_with_mysql:%files mysql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_mysql.so.*}

%{?_with_postgresql:%files postgresql
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_postgresql.so.*}

%{?_with_odbc:%files odbc
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_odbc.so.*}

%{?_with_oracle:%files oracle
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_libdir}/lib%{_lib_name}_oracle.so.*}


%files devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%{_prefix}/cmake/*
%dir %{_includedir}/%{_lib_name}/
%{_includedir}/%{_lib_name}/*.h
%{?_with_empty:%{_includedir}/%{_lib_name}/empty/}
%{_libdir}/lib%{_lib_name}_core.so
%{?_with_empty:%{_libdir}/lib%{_lib_name}_empty.so}

%{?_with_sqlite3:%files sqlite3-devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %{_includedir}/%{_lib_name}
%{_includedir}/%{_lib_name}/sqlite3/
%{_libdir}/lib%{_lib_name}_sqlite3.so}

%{?_with_mysql:%files mysql-devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %{_includedir}/%{_lib_name}
%{_includedir}/%{_lib_name}/mysql
%{_libdir}/lib%{_lib_name}_mysql.so}

%{?_with_postgresql:%files postgresql-devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %{_includedir}/%{_lib_name}
%{_includedir}/%{_lib_name}/postgresql
%{_libdir}/lib%{_lib_name}_postgresql.so}

%{?_with_odbc:%files odbc-devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %{_includedir}/%{_lib_name}
%{_includedir}/%{_lib_name}/odbc/
%{_libdir}/lib%{_lib_name}_odbc.so}

%{?_with_oracle:%files oracle-devel
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README
%dir %{_includedir}/%{_lib_name}
%{_includedir}/%{_lib_name}/oracle
%{_libdir}/lib%{_lib_name}_oracle.so}

%files doc
%defattr(-,root,root,-)
%doc AUTHORS ChangeLog COPYING NEWS README docs

%changelog

* Tue Nov 27 2018 ronan.abhamon <ronan.abhamon@belledonne-communications.com>
- Do not set CMAKE_INSTALL_LIBDIR and never with _libdir!

* Tue Oct 30 2018 ronan.abhamon <ronan.abhamon@belledonne-communications.com>
- Use CPack.

* Wed Dec 6 2017 erwan.croze <erwan.croze@belledonne.communications.com>
- Initial RPM release.
