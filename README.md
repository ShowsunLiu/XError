PHP Extension XError
======

##Description
XError is a simple php extension for rewriting the error page by yourself.

##Installation
Like other PHP extensions ,Add extension=[.so|.dll] to your php.ini. 


##Related settings in php.ini

- **1 xerror.file**

   Specify a php file for running when error happens.The default value is "none".
if you set like this xerror.file="/etc/error.php", you can retrieve  the error message by the super global $_ERROR.


- **2 xerror.reporting**

  Specify the error levels that you want to rewrite. it's usage is as same as error_reporting. The default value is all the fetal errors(E_ERROR|E_PARSE|E_CORE_ERROR|E_COMPILE_ERROR|E_USER_ERROR|E_RECOVERABLE_ERROR).

- **3 xerror.force**

  When On, xerror will not be affected by error_reporting.

- **4 xerror.enable_cli**

  Whether enable xerror in cli.

  


