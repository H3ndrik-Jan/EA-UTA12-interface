<?php
$stdbyq = "UPDATE labpsu.settings SET standby= NOT standby WHERE 1";

$conn = mysqli_connect("servername", "username", "password", "database");
	$result = $conn->query($stdbyq);
  $conn->close();

?>
