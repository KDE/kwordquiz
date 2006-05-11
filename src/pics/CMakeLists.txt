kde4_header()




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kwordquiz/icons FILES  )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



#original Makefile.am contents follow:

#kwordquiziconsdir = $(kde_datadir)/kwordquiz/icons
#
#kwordquizicons_ICON = AUTO
#
#KDE_ICON = kwordquiz kwordquiz_doc
