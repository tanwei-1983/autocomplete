/* my extension for PHP */

#ifndef PHP_MY_H
# define PHP_MY_H

extern zend_module_entry my_module_entry;
# define phpext_my_ptr &my_module_entry

# define PHP_MY_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_MY)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_MY_H */
