####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kwordquiz

## INCLUDES were found outside kdevelop specific part

kwordquiz_SOURCES = kwordquizprefs.cpp kwordquizview.cpp kwordquizdoc.cpp kwordquiz.cpp main.cpp wqviewbase.ui dlglanguagebase.ui  flashviewbase.ui multipleviewbase.ui qaviewbase.ui qaview.cpp flashview.cpp multipleview.cpp wqquiz.cpp wqlistitem.cpp wqscore.cpp keduvocdata.cpp prefgeneralbase.ui prefeditorbase.ui prefflashbase.ui prefmultiplebase.ui prefqabase.ui prefquizbase.ui prefeditor.cpp prefquiz.cpp configuration.cpp wqsyntax.cpp dlgsortbase.ui dlgsort.cpp dlgrcbase.ui dlgrc.cpp dlgspecchar.cpp kvtmlwriter.cpp dlglanguage.cpp wqprintdialogpage.cpp
kwordquiz_LDADD   =  $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE)  $(LIBSOCKET) $(LIB_KDEPRINT)


EXTRA_DIST = main.cpp kwordquiz.cpp kwordquiz.h kwordquizdoc.cpp kwordquizdoc.h kwordquizview.cpp kwordquizview.h kwordquizui.rc kwordquiz.desktop kwordquizprefs.cpp kwordquizprefs.h

install-data-local:
	$(mkinstalldirs) $(kde_mimedir)/application
	$(INSTALL_DATA) $(srcdir)/x-kwordquiz.desktop $(kde_mimedir)/application/x-kwordquiz.desktop
	$(mkinstalldirs) $(kde_appsdir)/Edutainment/Miscellaneous/
	$(INSTALL_DATA) $(srcdir)/kwordquiz.desktop $(kde_appsdir)/Edutainment/Miscellaneous/kwordquiz.desktop

uninstall-local:
	-rm -f $(kde_mimedir)/application/x-kwordquiz.desktop
	-rm -f $(kde_appsdir)/Applications/kwordquiz.desktop


####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/kwordquiz/icons $(all_includes)
SUBDIRS = . pics examples
METASOURCES = AUTO

# the library search path. 
kwordquiz_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/kwordquiz
rc_DATA = kwordquizui.rc eventsrc

apps_DATA = kwordquiz.desktop
appsdir = $(kde_appsdir)/Edutainment/Miscellaneous

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kwordquiz.pot; \
	fi


noinst_HEADERS = qaview.h flashview.h multipleview.h wqquiz.h wqlistitem.h wqpos.h wqscore.h prefeditor.h prefquiz.h configuration.h wqsyntax.h dlgsort.h dlgrc.h dlgspecchar.h kvtmlwriter.h dlglanguage.h dlglanguage.h wqprintdialogpage.h
