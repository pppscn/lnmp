dnl $Id$
dnl config.m4 for extension logging

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(logging, for logging support,
dnl Make sure that the comment is aligned:
dnl [  --with-logging             Include logging support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(logging, whether to enable logging support,
dnl Make sure that the comment is aligned:
[  --enable-logging           Enable logging support])

if test "$PHP_LOGGING" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-logging -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/logging.h"  # you most likely want to change this
  dnl if test -r $PHP_LOGGING/$SEARCH_FOR; then # path given as parameter
  dnl   LOGGING_DIR=$PHP_LOGGING
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for logging files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       LOGGING_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$LOGGING_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the logging distribution])
  dnl fi

  dnl # --with-logging -> add include path
  dnl PHP_ADD_INCLUDE($LOGGING_DIR/include)

  dnl # --with-logging -> check for lib and symbol presence
  dnl LIBNAME=logging # you may want to change this
  dnl LIBSYMBOL=logging # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LOGGING_DIR/lib, LOGGING_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_LOGGINGLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong logging lib version or lib not found])
  dnl ],[
  dnl   -L$LOGGING_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(LOGGING_SHARED_LIBADD)

  PHP_NEW_EXTENSION(logging, logging.c, $ext_shared)
fi
