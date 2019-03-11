
TOPSRCDIR   = $$PWD
TOPBUILDDIR = $$shadowed($$PWD)

DESTDIR 	= $$TOPBUILDDIR/output

LIBPATH_QAPPLOGING  = $${TOPSRCDIR}/libs/private/QAppLogging
LIBPATH_LIBQXT      = $${TOPSRCDIR}/libs/3rdparty/libqxt

CONFIG(debug, debug|release) {

} else {

}

