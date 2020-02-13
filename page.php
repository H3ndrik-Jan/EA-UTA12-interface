    <!DOCTYPE html>
    <html>
    <head>
   <!-- <meta http-equiv="refresh" content="10" > -->
    <?php $limit = 50;?>

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
	</meta>
    </head>

    <body>
	<form action="page.php" method="post">
		# of results: <br>
		<input type="text" name = "+1" value="50" />
	</form>

		<button type="button" id= "stdby" name = "standby">Standby</button> 
		<button type="button" id= "extern" name = "external">External</button> 



    <?php
$voltage = array();
$time = array();
$current = array();
	
	if(isset($_POST['+1'])){
$limit = $_POST['+1'];
}


    $conn = mysqli_connect("localhost", "page", "page", "labpsu");
    // Check connection
    if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
    }
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

echo "Last measured voltage: ". $voltage[0]. "V<br>";
 echo '<meter value ="'.$voltage[0].'" min = "0" max = "150">Voltage</meter><br>';
echo "Last measured Current: ". $current[0]. "A<br>";
 echo '<meter value ="'.$current[0].'" min = "0" max = "4">Current</meter><br>';
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
