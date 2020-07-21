#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([mssilk],[1.1.0])
AC_PREREQ(2.63)
AC_CONFIG_SRCDIR([sdk/known_checksums/SILK_SDK_SRC_v1.0.8.zip.md5])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([tar-ustar --warnings=no-portability])

AC_CANONICAL_SYSTEM

AC_CONFIG_HEADERS(config.h)

AC_ARG_ENABLE(strict,
[  --enable-strict       Enable error on compilation warning [default=no]],
[wall_werror=$enableval],
[wall_werror=yes]
)


CFLAGS="$CFLAGS -Wall"

if test "$wall_werror" = "yes" ; then
	STRICT_CFLAGS="-Werror"
fi

AC_SUBST(STRICT_CFLAGS)

case $host in
	arm*)
		case $CC in
			*clang*) # clang doesn't like the old ARM assembly
				SILK_FLAVOUR=FIX
			;;
			*)
				SILK_FLAVOUR=ARM
			;;
		esac
	;;
	*)
		SILK_FLAVOUR=FIX
	;;
esac

case $host in
	armv6-apple*)
		SILK_MAKE_OPTIONS="ADDED_DEFINES+=IPHONE TOOLCHAIN_PREFIX=XXXX"
		CFLAGS="$CFLAGS -mno-thumb"
	;;
	armv7-apple*)
		SILK_MAKE_OPTIONS="USE_NEON=yes ADDED_DEFINES+=IPHONE TOOLCHAIN_PREFIX=XXXX"
		CFLAGS="$CFLAGS -mno-thumb"
	;;
	armv7*)
		SILK_MAKE_OPTIONS="USE_NEON=yes TOOLCHAIN_PREFIX=XXXX"
	;;
	arm*)
		dnl override flavour for case of old arm, as a workaround of a bug in the Skype's Makefile resulting in neon functions not being compiled while requested,
		dnl because the compiler says it supports armv7 and neon.
		SILK_FLAVOUR=FIX
	;;
	*)
	;;
esac



AC_SUBST([SILK_FLAVOUR])
AC_SUBST([SILK_MAKE_OPTIONS])

LT_INIT([disable-static])

AC_ARG_WITH([mediastreamer], AS_HELP_STRING([--without-mediastreamer], [Permits to deactivate mediastreamer check for download of SILK code only]))

if test "x$with_mediastreamer" != "xno"; then
    # check for libraries that have pkg-config files installed
    PKG_CHECK_MODULES(MEDIASTREAMER, mediastreamer >= 2.0.0)
fi


# Checks for libraries.
AC_CHECK_LIB([m],[cosf])

# initialize libtool
AC_LIBTOOL_WIN32_DLL
LT_INIT([disable-static])

case $target_os in 
	*mingw*)
		mingw_found=yes
	;;
esac

AM_CONDITIONAL(BUILD_WIN32, test x$mingw_found = xyes)

AC_CONFIG_COMMANDS([libtool-hacking],[
if test "$mingw_found" = "yes" ; then
	AC_MSG_NOTICE([Hacking libtool to work with mingw...])
	sed -e 's/\*\" \$a_deplib \"\*/\*/' < ./libtool > libtool.tmp
	cp -f ./libtool.tmp ./libtool
	rm -f ./libtool.tmp
fi
],[mingw_found=$mingw_found])

# Checks for library functions.

# Create the following files from their .in counterparts
AC_CONFIG_FILES([
  Makefile
  sdk/Makefile
  mssilk.iss
])

#  src/Makefile
#  pkgconfig/Makefile
#  pkgconfig/silk.pc
#  man/Makefile
AC_OUTPUT
