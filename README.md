PHP Extension XError
======

##Installation
Like other PHP extensions ,Add extension=[.so|.dll] to your php.ini

##Related settings in php.ini

- **1 xerror.file**
   Specify a php file for running when error happens.Default value is "none".
if you set like this xerror.file="/etc/error.php", you can retrieve  the error message by the super global $_ERROR.


- **2 xerror.reporting**
  Specify the error levels 




