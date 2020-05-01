<?php
$stdbyq = "UPDATE labpsu.settings SET extern= NOT extern WHERE 1";

$conn = mysqli_connect("servername", "username", "password", "database");
	$result = $conn->query($stdbyq);
  $conn->close();
?>
