<?php
$stdbyq = "UPDATE labpsu.settings SET extern= NOT extern WHERE 1";

$conn = mysqli_connect("localhost", "page", "page", "labpsu");
	$result = $conn->query($stdbyq);
  $conn->close();
?>
