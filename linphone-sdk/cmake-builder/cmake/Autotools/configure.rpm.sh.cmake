#!/bin/sh

if [ -n "@AUTOTOOLS_AS_COMPILER@" ]
then
	export AS="@AUTOTOOLS_AS_COMPILER@"
fi
export CC="@AUTOTOOLS_C_COMPILER@"
export CXX="@AUTOTOOLS_CXX_COMPILER@"
export OBJC="@AUTOTOOLS_OBJC_COMPILER@"
export LD="@AUTOTOOLS_LINKER@"
export AR="@AUTOTOOLS_AR@"
export RANLIB="@AUTOTOOLS_RANLIB@"
export STRIP="@AUTOTOOLS_STRIP@"
export NM="@AUTOTOOLS_NM@"

export ASFLAGS="@ep_asflags@"
export CPPFLAGS="@ep_cppflags@"
export CFLAGS="@ep_cflags@"
export CXXFLAGS="@ep_cxxflags@"
export OBJCFLAGS="@ep_objcflags@"
export LDFLAGS="@ep_ldflags@"

export PKG_CONFIG="@LINPHONE_BUILDER_PKG_CONFIG@"
export PKG_CONFIG_PATH="@LINPHONE_BUILDER_PKG_CONFIG_PATH@"
export PKG_CONFIG_LIBDIR="@LINPHONE_BUILDER_PKG_CONFIG_LIBDIR@"

export RPM_TOPDIR="@LINPHONE_BUILDER_WORK_DIR@/rpmbuild"


cd @ep_build@

# create rpmbuild workdir if needed
for dir in BUILDROOT RPMS SOURCES SPECS SRPMS; do
	mkdir -p "$RPM_TOPDIR/$dir"
done

if [ ! -f @ep_config_h_file@ ]
then
 	@ep_autogen_command@ @ep_autogen_redirect_to_file@
	@ep_configure_env@ @ep_configure_command@ @ep_configure_redirect_to_file@
	make dist distdir='@LINPHONE_BUILDER_RPMBUILD_PACKAGE_PREFIX@$(PACKAGE)-$(VERSION)' V=@AUTOTOOLS_VERBOSE_MAKEFILE@ @ep_redirect_to_file@
	cp -v *.tar.gz "$RPM_TOPDIR/SOURCES/"
	@LINPHONE_BUILDER_CONFIGURE_EXTRA_CMD@
fi
