EXTRA_DIST=autogen.sh mssilk.iss.in CMakeLists.txt
SILK_VERSION = 1.0.9
#SILK_DIR=$(srcdir)/sdk/SILK_SDK_SRC_v$(SILK_VERSION)/SILK_SDK_SRC_$(SILK_FLAVOUR)_v$(SILK_VERSION)
SILK_DIR=$(top_builddir)/sdk/SILK_SDK_SRC_v$(SILK_VERSION)/SILK_SDK_SRC_$(SILK_FLAVOUR)_v$(SILK_VERSION)

SUBDIRS = sdk


pluginsdir = $(libdir)/mediastreamer/plugins

plugins_LTLIBRARIES=libmssilk.la

libmssilk_la_SOURCES=silk_dec.c silk_enc.c


SILK_LIBS = -L${SILK_DIR} -lSKP_SILK_SDK
libmssilk_la_LIBADD=\
	$(ORTP_LIBS) \
        $(MEDIASTREAMER_LIBS) \
        $(SILK_LIBS)

libmssilk_la_LDFLAGS=-module -no-undefined

SILK_CFLAGS = -I$(SILK_DIR)/interface 
AM_CFLAGS=      $(SILK_CFLAGS) \
                $(MEDIASTREAMER_CFLAGS) \
				$(STRICT_CFLAGS)

INSTALLDIR=$(shell cd $(top_builddir) && pwd)/$(PACKAGE)-install
INSTALLDIR_WITH_PREFIX=$(INSTALLDIR)/$(prefix)
ZIPFILE=$(shell cd $(top_builddir) && pwd)/$(PACKAGE)-win32-$(VERSION).zip
ZIP_EXCLUDED=include 

ISS_SCRIPT=mssilk.iss
ISS_SCRIPT_PATH=$(shell cd $(top_srcdir) && pwd)/$(ISS_SCRIPT)
#path to Inno Setup 5 compiler
ISCC=ISCC.exe
PACKAGE_WIN32_FILELIST=$(PACKAGE)-win32.filelist

setup.exe: zip
	cd $(INSTALLDIR_WITH_PREFIX) && \
	rm -f $(PACKAGE_WIN32_FILELIST) && \
	for file in `find` ; do \
		if ! test -d $$file ; then \
			echo "Source: $$file; Destdir: {app}\\`dirname $$file`; Flags: ignoreversion" \
			>> $(PACKAGE_WIN32_FILELIST) ;\
		fi \
	done
	cp $(ISS_SCRIPT) $(INSTALLDIR_WITH_PREFIX)/.
	cd $(INSTALLDIR_WITH_PREFIX) && \
	$(ISCC) $(ISS_SCRIPT) 
	mv $(INSTALLDIR_WITH_PREFIX)/Output/setup.exe $(PACKAGE)-$(VERSION)-setup.exe
	rm -rf $(INSTALLDIR_WITH_PREFIX)/Output
	rm -f $(INSTALLDIR_WITH_PREFIX)/$(PACKAGE_WIN32_FILELIST)
	rm -f $(INSTALLDIR_WITH_PREFIX)/$(ISS_SCRIPT)

zip:
	rm -f $(ZIPFILE)
	rm -rf $(INSTALLDIR)
	mkdir -p $(INSTALLDIR)
	make install DESTDIR=$(INSTALLDIR)
	cp -f $(top_srcdir)/README $(INSTALLDIR_WITH_PREFIX)/.
	cp -f $(top_srcdir)/COPYING $(INSTALLDIR_WITH_PREFIX)/.
	cd $(INSTALLDIR)/$(prefix) && rm -rf $(ZIP_EXCLUDED) && \
	zip -r $(ZIPFILE) *

