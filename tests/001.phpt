--TEST--
Check if my is loaded
--SKIPIF--
<?php
if (!extension_loaded('my')) {
	echo 'skip';
}
?>
--FILE--
<?php 
echo 'The extension "my" is available';
?>
--EXPECT--
The extension "my" is available
