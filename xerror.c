
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "php_xerror.h"

ZEND_DECLARE_MODULE_GLOBALS(xerror)

void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void xerror_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
zend_bool php_xerror_set_global(char *name, uint name_len TSRMLS_DC);

// xerror_module_entry
zend_module_entry xerror_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"XError",
	NULL,
	PHP_MINIT(xerror),
	PHP_MSHUTDOWN(xerror),
	PHP_RINIT(xerror),		
	PHP_RSHUTDOWN(xerror),	
	PHP_MINFO(xerror),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", 
#endif
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_XERROR
ZEND_GET_MODULE(xerror)
#endif

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("xerror.file", "none", PHP_INI_ALL, OnUpdateString, file, zend_xerror_globals, xerror_globals)
	STD_PHP_INI_ENTRY("xerror.reporting",DEFAULT_XERROR_REPORTING,	 PHP_INI_ALL, OnUpdateLong, reporting, zend_xerror_globals, xerror_globals)
	STD_PHP_INI_BOOLEAN("xerror.force", "1", PHP_INI_ALL, OnUpdateBool, force, zend_xerror_globals, xerror_globals)
	STD_PHP_INI_BOOLEAN("xerror.enable_cli", "0", PHP_INI_ALL, OnUpdateBool, enable_cli, zend_xerror_globals, xerror_globals)
PHP_INI_END()


static void php_xerror_init_globals(zend_xerror_globals *xerror_globals)
{
	xerror_globals->error_count	=	0;

}


static void php_xerror_shutdown_globals(zend_xerror_globals *xerror_globals TSRMLS_DC)
{
	//do nothing here
}


PHP_MINIT_FUNCTION(xerror)
{
	ZEND_INIT_MODULE_GLOBALS(xerror, php_xerror_init_globals, php_xerror_shutdown_globals);
	REGISTER_INI_ENTRIES();
	old_error_cb = zend_error_cb;
	zend_error_cb = xerror_error_cb;
	// Register $_ERROR
	zend_register_auto_global("_ERROR", sizeof("_ERROR") - 1, php_xerror_set_global TSRMLS_CC);
	return SUCCESS;
}


zend_bool php_xerror_set_global(char *name, uint name_len TSRMLS_DC) 
{
	if (XERROR_G(error_count)>0){
		ZEND_SET_SYMBOL(&EG(symbol_table),"_ERROR",XERROR_G(error_info));
		return 0;
	}else{
		return 1;
	}
}


//  New error callback
void xerror_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args) 
{
	int error_flag = 0;
	const char *default_html = "<html><title>System is busy,please try again later.</title><body style=\"background-color:#EDEDEF;text-align:center;\"><div  style=\"width:500px;margin:100px auto;margin-bottom:30px;font-size:60px;color:#444;font-weight:bold;text-align:left;\">System is busy</div><div style=\"width:500px;margin:0px auto;text-align:right;font-size:30px;\"><i> Please try again later</i></div></body></html>";
	const char *status_line = "HTTP/1.1 503 Service Temporarily Unavailable"; 
	sapi_header_line header_line = {0};
	char *message;
	TSRMLS_FETCH();
	
	error_flag = type&XERROR_G(reporting);
	if (!XERROR_G(force)) {
		error_flag &= EG(error_reporting);
	} 
	//Use default error handle
	if (!error_flag || !XERROR_G(enable_cli) && strcmp(sapi_module.name, "cli")==0) {
		old_error_cb(type, error_filename, error_lineno, format, args);
		return;
	}
	//Set header line
	header_line.line = status_line;
	header_line.line_len = strlen(status_line);
	// Avoid recursion
	if (++XERROR_G(error_count)>1) {
		if (XERROR_G(error_count)<3) {
			sapi_header_op(SAPI_HEADER_ADD, &header_line TSRMLS_CC);
		}
		php_printf(default_html);
	} else {
		zend_file_handle script;
		if (XERROR_G(file)=="none") {
			sapi_header_op(SAPI_HEADER_ADD, &header_line TSRMLS_CC);
			php_printf(default_html);
		} else {
			script.type=ZEND_HANDLE_FP;
			script.filename = XERROR_G(file);
			script.opened_path = NULL;
			script.free_filename =0;
			if (!(script.handle.fp=fopen(script.filename, "rb"))) {
				sapi_header_op(SAPI_HEADER_ADD, &header_line TSRMLS_CC);
				php_printf(default_html);
			} else {
				// Collect error information
				vspprintf(&message, PG(log_errors_max_len), format, args);
				MAKE_STD_ZVAL(XERROR_G(error_info));
				array_init(XERROR_G(error_info));
				add_assoc_long(XERROR_G(error_info),	"type", type);
				add_assoc_string(XERROR_G(error_info), "message", message,	1);
				add_assoc_string(XERROR_G(error_info),	"file",	error_filename,	1);
				add_assoc_long(XERROR_G(error_info), "line",	error_lineno);

				// Run custom script
				php_execute_script(&script TSRMLS_CC);
			}
		}
	}
	zend_bailout();
}


PHP_MSHUTDOWN_FUNCTION(xerror)
{
	UNREGISTER_INI_ENTRIES();
	zend_error_cb = old_error_cb;

#ifdef ZTS
	ts_free_id(xerror_globals_id);
#else
	php_xerror_shutdown_globals(&xerror_globals TSRMLS_CC);
#endif

	return SUCCESS;
}


PHP_RINIT_FUNCTION(xerror)
{
	XERROR_G(error_count)	=	0;
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(xerror)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(xerror)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XError support", "enabled");
	php_info_print_table_row(2, "XError author", "Showsun Liu");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();

}
