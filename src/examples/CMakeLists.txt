kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kwordquiz/examples FILES example.kvtml french_verbs.kvtml fill_in_the_blank.kvtml us_states_and_capitals.kvtml )

kde4_footer()



#original Makefile.am contents follow:

#kwqdatadir = $(kde_datadir)/kwordquiz/examples
#
#kwqdata_DATA = example.kvtml french_verbs.kvtml fill_in_the_blank.kvtml us_states_and_capitals.kvtml
