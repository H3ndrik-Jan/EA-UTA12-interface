<?php
$stdbyq = "UPDATE labpsu.settings SET standby= NOT standby WHERE 1";

$conn = mysqli_connect("localhost", "page", "page", "labpsu");
	$result = $conn->query($stdbyq);
  $conn->close();

?>
