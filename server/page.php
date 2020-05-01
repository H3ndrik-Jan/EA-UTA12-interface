  <!DOCTYPE html>
    <html>
    <head>
   <!-- <meta http-equiv="refresh" content="10" > -->
    <?php 
	$limit = 0;
	$setLimit = false;
	$volSet = 0;
	$setVol = false;
	$curSet = 0;
	$setCur = false;
	?>

    <title>UTA-12 output</title>
	<script src="https://cdn.jsdelivr.net/npm/chart.js@2.8.0"></script>
	<script src="../../utils.js"></script>
	<script src="http://ajax.googleapis.com/ajax/libs/jquery/1.11.2/jquery.min.js"></script>
    <style>
    table {
    border-collapse: separate;
    width: 70%;
    color: #588c7e;
    font-family: monospace;
    font-size: 25px;
    text-align: left;
    }
    th {
    background-color: #588c7e;
    color: white;
    }
    tr:nth-child(even) {background-color: #f2f2f2}

    </style>

	<style type="text/css">
    div {
float: left;
width:250px;
}
	</style>

	</meta>
	<h2>Settings menu</h2>
    </head>

    <body>
	<ul id="menu">
	<div><form action="page.php" method="post">
		<h3>Number of drawn results:</h3>
		<input type="text" name = "+1" />
	</form>
</br>

	<h3>Enable/disable:</h3>
	<button type="button" id= "stdby" name = "standby">Standby</button> 
	<button type="button" id= "extern" name = "external">External</button> </div>

	<div>
	<h3>Set voltage/current:</h3>
	<form action="page.php" method="post">
		<h5>Voltage:</h5>
		<input type="text" name = "vols" value="0" />
	</form>
	<form action="page.php" method="post">
		<h5>Current:</h5>
		<input type="text" name = "curs" value="0" />	
	</form>
	</div>

<div>

    <?php
$voltage = array();
$time = array();
$current = array();
	
if(isset($_POST['+1'])){
$limit = $_POST['+1'];
$setLimit = true;
}
if(isset($_REQUEST['vols'])){
$volSet = $_REQUEST['vols'];
$setVol = true;
}
if(isset($_REQUEST['curs'])){
$curSet = $_REQUEST['curs'];
$setCur = true;
}

if($setVol == true){
 $conn = mysqli_connect("servername", "username", "password", "database");
    if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
    }
    $sql = "UPDATE settings SET vset= IF($volSet<=150.0, $volSet, 0) WHERE 1";	//max voltage is 150 volts
$conn->query($sql);
    $conn->close();
}

if($setCur == true){
 $conn = mysqli_connect("servername", "username", "password", "database");
    if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
    }
    $sql = "UPDATE settings SET cset= IF($curSet<=4.0, $curSet, 0) WHERE 1";	//max current is 4 Ampere
$conn->query($sql);
    $conn->close();
}

    $conn = mysqli_connect("servername", "username", "password", "database");
    if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
    }
    if($setLimit == true)
    {
    $sql = "UPDATE settings SET res_num=$limit WHERE 1";
$conn->query($sql);
    }
    $sql = "SELECT res_num FROM settings WHERE 1";
    $result = $conn->query($sql);
    $limit = $result->fetch_object()->res_num;
    

    $sql = "SELECT num, voltage, current, recorded FROM log ORDER BY num DESC LIMIT $limit";
    $result = $conn->query($sql);
    if ($result->num_rows > 0) {
    // output data of each row

    while($row = $result->fetch_assoc()) {
	$voltage[] = $row["voltage"];
	$time[] = $row["recorded"];
	$current[] = $row["current"];
    }

    } else { echo "0 results"; }

echo "<h5>Last measured voltage: </h5>". $voltage[0]. "V<br>";
 echo '<meter value ="'.$voltage[0].'" min = "0" max = "150">Voltage</meter><br>';
echo "<h5>Last measured Current: </h5>". $current[0]. "A<br>";
 echo '<meter value ="'.$current[0].'" min = "0" max = "4">Current</meter><br></div></ul>';
    $conn->close();
    ?>

    <canvas id="myChart" width="50" height="20"></canvas>
    <script>
var voltage = [];
var current = [];
var time = [];
var x = 0;

voltage = <?php echo json_encode($voltage); ?>;
current = <?php echo json_encode($current); ?>;
time = <?php echo json_encode($time); ?>;

var ctx = document.getElementById('myChart').getContext('2d');
var myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: time,
        datasets: [{	//voltage line:
            label: 'Voltage',
            data: voltage,
		fill: false,
            borderColor: [
                'rgba(39, 39, 255, 1)'
            ],
		yAxisID: 'left-y-axis',
            borderWidth: 1,
	lineTension: 0.1
	},{	//Current line:
 label: 'Current',
            data: current,
		fill: false,

            borderColor: [
                'rgba(255, 39, 39, 1)'
            ],

	yAxisID: 'right-y-axis',
            borderWidth: 1,
lineTension: 0.5

        }]
    },

    options: {
	animation: false,
        scales: {
            yAxes: [{
                ticks: {
                    beginAtZero: true
                },
		id: 'left-y-axis',
		position: 'left'
		},{

                ticks: {
                    beginAtZero: true
                },
		id: 'right-y-axis',
		type: 'linear',
		position: 'right'
            }]
        }
    }

});
</script>


<script>
$(document).ready(function(){
$("#stdby").click(function(){
$.ajax({
url:"query.php",
type: "POST",
succes:function(result){
alert(result);
}
});
});
})
</script>

<script>
$(document).ready(function(){
$("#extern").click(function(){
$.ajax({
url:"query2.php",
type: "POST",
succes:function(result){
alert(result);
}
});
});
})
</script>
    </body>
    </html>
