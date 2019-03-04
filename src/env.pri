
TOPSRCDIR   = $$PWD
TOPBUILDDIR = $$shadowed($$PWD)

DESTDIR 	= $$TOPBUILDDIR/output

LIBPATH_QAPPLOGING  = $${TOPSRCDIR}/libs/private/QAppLogging

CONFIG(debug, debug|release) {

} else {

}

