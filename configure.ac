AC_INIT([gmerlin-effectv],[2.0.0])
AC_CONFIG_SRCDIR([include/gmerlin_effectv.h])

AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS(include/config.h)

AC_DISABLE_STATIC

CFLAGS_SAVE=$CFLAGS
AC_LANG_SOURCE(C)
LT_INIT
CFLAGS=$CFLAGS_SAVE


dnl
dnl Features
dnl

AC_CHECK_SIZEOF(float,4)
AC_C_BIGENDIAN(,,AC_MSG_ERROR("Cannot detect endianess"))

dnl
dnl Required libraries
dnl

GMERLIN_REQUIRED="2.0.0"
PKG_CHECK_MODULES(GMERLIN, gmerlin >= $GMERLIN_REQUIRED, , AC_MSG_ERROR("gmerlin not found"))

gmerlin_plugindir='$(libdir)/gmerlin/plugins'

dnl LDFLAGS for plugins
GMERLIN_PLUGIN_LDFLAGS="-export-symbols "`pkg-config --variable=prefix gmerlin`"/share/gmerlin/plugin.sym"

AC_SUBST(GMERLIN_PLUGIN_LDFLAGS)
AC_SUBST(gmerlin_plugindir)
AC_SUBST(GMERLIN_REQUIRED)

AM_GNU_GETTEXT([external])

dnl
dnl CFLAGS
dnl

LQT_OPT_CFLAGS(["-O3 -fomit-frame-pointer -ffast-math"])

CFLAGS="$CFLAGS -D_REENTRANT -D_FILE_OFFSET_BITS=64 $OPT_CFLAGS $GMERLIN_CFLAGS"

LQT_TRY_CFLAGS(-Wall, CFLAGS="$CFLAGS -Wall")
LQT_TRY_CFLAGS(-Wmissing-declarations, CFLAGS="$CFLAGS -Wmissing-declarations" )
LQT_TRY_CFLAGS(-Wdeclaration-after-statement, CFLAGS="$CFLAGS -Wdeclaration-after-statement")

dnl
dnl LIBS
dnl

LIBS="$GMERLIN_LIBS"

if test "x$prefix" = xNONE; then
   prefix="${ac_default_prefix}"
fi
if test "x$exec_prefix" = xNONE; then
   exec_prefix="${prefix}"
fi

AC_CONFIG_FILES([Makefile  po/Makefile.in \
include/Makefile \
lib/Makefile \
m4/Makefile \
utils/Makefile \
plugins/Makefile \
])

AC_OUTPUT
