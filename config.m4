dnl
dnl $Id$
dnl

PHP_ARG_ENABLE(xerror, whether to enable custom error support,
[  --enable-showsun          Disable custom error support], yes)

if test "$PHP_xerror" != "no"; then
  PHP_NEW_EXTENSION(xerror, xerror.c, $ext_shared)
fi
