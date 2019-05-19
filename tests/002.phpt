--TEST--
my_test1() Basic test
--SKIPIF--
<?php
if (!extension_loaded('my')) {
	echo 'skip';
}
?>
--FILE--
<?php 
$ret = my_test1();

var_dump($ret);
?>
--EXPECT--
The extension my is loaded and working!
NULL
