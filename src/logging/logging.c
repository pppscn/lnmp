/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include <syslog.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_logging.h"

/* If you declare any globals in php_logging.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(logging)
*/

/* True global resources - no need for thread safety here */
//static int le_logging;

/* {{{ logging_functions[]
 *
 * Every user visible function must have an entry in logging_functions[].
 */
const zend_function_entry logging_functions[] = {
	PHP_FE(confirm_logging_compiled,	NULL)		/* For testing, remove later. */
    PHP_FE(php_log_info,	NULL)
    PHP_FE(php_dump_data,	NULL)

	{NULL,NULL,NULL}
};
/* }}} */

/* {{{ logging_module_entry
 */
zend_module_entry logging_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"logging",
	logging_functions,
	PHP_MINIT(logging),
	PHP_MSHUTDOWN(logging),
	PHP_RINIT(logging),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(logging),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(logging),
#if ZEND_MODULE_API_NO >= 20010901
	"0.2", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LOGGING
ZEND_GET_MODULE(logging)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("logging.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_logging_globals, logging_globals)
    STD_PHP_INI_ENTRY("logging.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_logging_globals, logging_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_logging_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_logging_init_globals(zend_logging_globals *logging_globals)
{
	logging_globals->global_value = 0;
	logging_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(logging)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(logging)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(logging)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(logging)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(logging)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "logging support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_logging_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_logging_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "logging", arg);
	RETURN_STRINGL(strg, len);

}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
//static char lastfile[1200]={ 0x00 };
static char hostname[128];
static char ident[128];

static char *get_now_time_str()
{
    //time_t tm_t;
    struct timeval tv;
    static char timestr[64];

    gettimeofday(&tv, NULL);
    strftime(timestr,sizeof(timestr),"%m.%d %T",localtime(&tv.tv_sec));
    sprintf(timestr+strlen(timestr), ".%03lu", tv.tv_usec/1000);
    return timestr;
}
static int open_log_file()
{
    char *log_id, *log_env;
    char fname[1024];

    log_env=getenv("ER_PHP_LOG_FILE");
    strncpy(fname, log_env?log_env:"php_debug", 512);
    log_id=strrchr(fname,'/');
    if( log_id == NULL) log_id=fname;
    else log_id++;

	gethostname(hostname, sizeof(hostname));
	snprintf(ident, sizeof(ident), "ZCPT_PHP_%s", log_id);
	
	openlog(ident, LOG_PID, LOG_LOCAL5);

    return 0;
}

static void log_info(const char *fmt, ...)
{
    va_list         ap;
	FILE *fp;
	char *mbuf;
	size_t mlen;

    if(open_log_file() < 0) return;

	fp = open_memstream(&mbuf, &mlen);	

    fprintf(fp, "[%s][pid:%d][%s]",hostname, getpid(), get_now_time_str());

    va_start (ap, fmt);
    vfprintf (fp, fmt, ap);
    va_end (ap);

    //fputc('\n',fp);
    //fputc('\n',fp);

	fclose(fp);

	syslog(LOG_INFO, "%s", mbuf);
	free(mbuf);

    return;
}
static void dump_data (char *title,const unsigned char *buffer,int len)
{
    int     i, j, line=0, count;
	FILE *fp;
	char *mbuf;
	size_t mlen;

    if(open_log_file() < 0) return;

	fp = open_memstream(&mbuf, &mlen);	

    if(title) fprintf(fp,"[%s][pid:%d][%s] %s:\n",hostname, getpid(), get_now_time_str(),title);
    if(!buffer||!len)
    {
		fclose(fp);
        return; //check if buffer validate
    }

    for (i=0; i<len; i++) {
        if (i == 0) {
            if (len >= 16) {
                line = 1;
                count = line * 16;
            }
            else    count = len;
            fprintf(fp, "%6.6d   ", count);
        }
        if (!(i%8) && (i!=0)) {
            if (i%16)   fprintf(fp, " ");
            if (i%16)   fprintf(fp, " ");
            else {
                fprintf(fp, " |  ");
                for (j=i-16; j<i; j++) {
                    if ((buffer[j] >= 0x20) && (buffer[j] < 0x7F))
                        fprintf(fp, "%c", buffer[j]);
                    else    fprintf(fp, ".");
                }
                fputc('\n', fp);
                if ((len-i) >= 16) {
                    line ++;
                    count = line * 16;
                }
                else    count = line * 16 + len - i;
                fprintf(fp , "%6.6d   ", count);
            }
        }
        fprintf(fp, "%02X ", buffer[i]);
    }

    if ((i%16) != 0)
        for (j=0; j<(((i%16)>8) ? (3*(16-i%16)) : (3*(16-i%16)+1)); j++)
            fputc(' ', fp);
    fprintf(fp, " |  ");
    for (j=(i - (((i % 16) == 0) ? 16 : (i % 16))); j<i; j++) {
        if ((buffer[j] >= 0x20) && (buffer[j] < 0x7F))
            fprintf(fp, "%c", buffer[j]);
        else    fprintf(fp, ".");
    }
    //fputc('\n', fp);
    //fputc('\n', fp);
	fclose(fp);
	syslog(LOG_DEBUG, "%s", mbuf);
	free(mbuf);

	return;
}
PHP_FUNCTION(php_log_info)
{
    char *msg;
    size_t msglen;

    if(open_log_file() < 0) return;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &msg, &msglen) == FAILURE)
    {
        log_info("php_log_info2 parameters parse fail");
        return;
    }

    log_info("%s", msg);
}
PHP_FUNCTION(php_dump_data)
{
    char *title, *buf;
    size_t title_len, buf_len;

    if(open_log_file() < 0) return;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &title, &title_len, &buf, &buf_len) == FAILURE) 
    {
        log_info("php_dump_data2 parameters parse fail");
        return;
    }

    dump_data(title, buf, buf_len);
}
static void asc2bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int len)
{
    int i;
    unsigned char  ch;

    for (i=0; i<len; i++)
    {

        ch = ascii_buf[i];
        if (ch>='a')  ch -= 'a' - 10;
        else if (ch>='A')  ch -= 'A' - 10;
        else ch -= '0';
        if (i & 1)  *(bcd_buf++) |= ch & 0x0f;        else  *bcd_buf = ch << 4;
    }
}   

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
