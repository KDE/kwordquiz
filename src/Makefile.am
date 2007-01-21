####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kwordquiz

## INCLUDES were found outside kdevelop specific part

kwordquiz_SOURCES = kwordquizprefs.cpp kwordquizview.cpp kwordquizdoc.cpp \
			kwordquiz.cpp main.cpp dlglanguagebase.ui flashviewbase.ui multipleviewbase.ui \
			qaviewbase.ui qaview.cpp flashview.cpp multipleview.cpp wqquiz.cpp wqlistitem.cpp \
			wqscore.cpp keduvocdata.cpp prefeditorbase.ui prefquizbase.ui prefeditor.cpp \
			prefquiz.cpp dlgsortbase.ui dlgsort.cpp dlgrcbase.ui dlgrc.cpp dlgspecchar.cpp \
			kvtmlwriter.cpp dlglanguage.cpp wqprintdialogpage.cpp prefcharacter.cpp \
			prefcharacterbase.ui paukerreader.cpp wqlreader.cpp wqlwriter.cpp wqundo.cpp prefs.kcfgc \
		prefcardappearance.cpp prefcardappearancebase.ui kwqnewstuff.cpp kwqnewstuff.h
kwordquiz_LDADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE) \
	$(LIBSOCKET) $(LIB_KDEPRINT) -lknewstuff


EXTRA_DIST = main.cpp kwordquiz.cpp kwordquiz.h kwordquizdoc.cpp kwordquizdoc.h kwordquizview.cpp kwordquizview.h kwordquizui.rc kwordquizprefs.cpp kwordquizprefs.h

kde_kcfg_DATA = kwordquiz.kcfg
kde_conf_DATA = kwordquizrc
mime_DATA = x-kwordquiz.desktop
mimedir = $(kde_mimedir)/application

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

xdg_apps_DATA = kwordquiz.desktop

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kwordquiz.pot; \
	fi


noinst_HEADERS = qaview.h flashview.h multipleview.h wqquiz.h wqlistitem.h \
		wqscore.h prefeditor.h prefquiz.h dlgsort.h dlgrc.h dlgspecchar.h kvtmlwriter.h \
		dlglanguage.h dlglanguage.h wqprintdialogpage.h prefcharacter.h wqlreader.h \
		wqlwriter.h wqundo.h prefcardappearance.h
