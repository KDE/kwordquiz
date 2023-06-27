#! /bin/sh
LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C -o -name \*.qml``
if test -n "$LIST"; then
$XGETTEXT $LIST rc.cpp -o $podir/kwordquiz.pot
fi
