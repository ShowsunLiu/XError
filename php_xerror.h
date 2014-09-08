
#ifndef PHP_XERROR_H
#define PHP_XERROR_H

extern zend_module_entry xerror_module_entry;
#define phpext_xerror_ptr &xerror_module_entry

#ifdef PHP_WIN32
#	define PHP_XERROR_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_XERROR_API __attribute__ ((visibility("default")))
#else
#	define PHP_XERROR_API
#endif

/* Predefined a constant  which contains all the levels of fetal errors. */ 
#if PHP_VERSION_ID<50200
# define DEFAULT_XERROR_REPORTING "341"
#else
# define DEFAULT_XERROR_REPORTING "4437"
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(xerror);
PHP_MSHUTDOWN_FUNCTION(xerror);
PHP_RINIT_FUNCTION(xerror);
PHP_RSHUTDOWN_FUNCTION(xerror);
PHP_MINFO_FUNCTION(xerror);

ZEND_BEGIN_MODULE_GLOBALS(xerror)
	int	error_count;
	char *file;
	long reporting;
	zval	*error_info;
ZEND_END_MODULE_GLOBALS(xerror)

#ifdef ZTS
#define XERROR_G(v) TSRMG(xerror_globals_id, zend_xerror_globals *, v)
#else
#define XERROR_G(v) (xerror_globals.v)
#endif

#endif	/* PHP_XERROR_H */
