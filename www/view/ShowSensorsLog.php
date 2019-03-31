<?php

const ID_TEMPERATURE='temperature';
const ID_PRESSURE='pressure';
const ID_HUMIDITY='humidity';
const ID_HEATING='heating';

//вывоводит данные массива в формате JavaScript-переменной
function CreateJsVar($SensorData, $key)
{
    echo 'var '.$key.' = [';
    foreach($SensorData as $value)
    {
        echo '"'.$value[$key].'",';
    }
    echo '];';
}

function CreateDatasetItem($Index, $ItemId)
{
?>
    {
<?php
    switch ($ItemId) {
        case ID_TEMPERATURE:
            echo 'label: "Температура",';
            echo 'borderColor: window.chartColors.red,';
            echo 'backgroundColor: window.chartColors.red,';
        break;
        case ID_PRESSURE:
            echo 'label: "Давление",';
            echo 'borderColor: window.chartColors.green,';
            echo 'backgroundColor: window.chartColors.green,';
        break;
        case ID_HUMIDITY:
            echo 'label: "Влажность",';
            echo 'borderColor: window.chartColors.blue,';
            echo 'backgroundColor: window.chartColors.blue,';
        break;
        case ID_HEATING:
            echo 'label: "Обогрев",';
            echo 'borderColor: window.chartColors.yellow,';
            echo 'backgroundColor: window.chartColors.yellow,';
            echo 'steppedLine: "before",';
            break;
    }
    ?>
        fill: false,
        spanGaps: true,
        lineTension: 0,
        yAxisID: "y-axis-<?php echo $Index; ?>",
    }, 
<?php
}

//отображаемы данные отличаются диапазоном шкал
function CreateOptionsItem($Index, $ItemId)
{
?>
    {
        type: 'linear',
<?php
    switch ($ItemId) {
        case ID_TEMPERATURE:
            echo 'display: true, ';
            echo 'position: "left", ';
            echo 'ticks: {';
                echo 'min: 0,';
                echo 'max: 40,';
        break;
        case ID_PRESSURE:
            echo 'display: true, ';
            echo 'position: "right", ';
            echo 'ticks: {';
                echo 'min: 700,';
                echo 'max: 800,';
        break;
        case ID_HUMIDITY:
            echo 'display: true, ';
            echo 'position: "right", ';
            echo 'ticks: {';
                echo 'min: 0,';
                echo 'max: 100,';
        break;
        case ID_HEATING:
            echo 'display: false, ';
            echo 'position: "left", ';
            echo 'ticks: {';
                echo 'min: -1,';
                echo 'max: 25,';
        break;
    }
    echo '}, ';
    echo 'id: "y-axis-'.$Index.'", ';
?>
        gridLines: {
            drawOnChartArea: false, 
        },
    },
<?php
}

function ShowSensorLog($Sensor, $SensorData) 
{
    //var_dump($SensorData);
    //return;

    $timelabels=array_keys($SensorData);
?>

<div style="width:100%;">
<style>
canvas {
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
}
</style>
    <?php echo $Sensor;?>
    <canvas id="<?php echo $Sensor;?>"></canvas>
    <script src="./dist/Chart.bundle.js"></script>
   	<script>
    window.chartColors = {
    	red: 'rgb(255, 99, 132)',
    	orange: 'rgb(255, 159, 64)',
    	yellow: 'rgb(255, 205, 86)',
    	green: 'rgb(75, 192, 192)',
    	blue: 'rgb(54, 162, 235)',
    	purple: 'rgb(153, 102, 255)',
    	grey: 'rgb(201, 203, 207)'
    };
    var timelabels = [
    <?php
        foreach($timelabels as $value)
        {
            echo '"'.$value.'",';
        }
    ?>
    ];
    <?php
        $FirstRec=reset($SensorData);
        $SensorIndex=1;
        //определяем, какие данные содержатся в массиве. Ключ входного массива будет являться именем JS-переменной
        //количество наборов данных определит количество линий на графике
        foreach($FirstRec as $key=>$value)
        {
            if(isset($FirstRec[$key]))
            {
                $Sensors[$SensorIndex++]=$key;
                CreateJsVar($SensorData, $key);
            }
        }
    ?>
    var ChartConfig = {
				data: {
					datasets: [
                            <?php
                            foreach($Sensors as $key=>$value)
                            {
                                CreateDatasetItem($key, $value);
                            }
                            ?>
					]
				},
				options: {
					responsive: true,
					hoverMode: 'index',
					stacked: false,
					title: {
						display: true,
					},
					scales: {
						yAxes: [
                            <?php
                            foreach($Sensors as $key=>$value)
                            {
                                CreateOptionsItem($key, $value);
                            }
                            ?>
                        ],
					},
					tooltips: {
						mode: 'index'
					}
				}
			};	
            ChartConfig.data.labels = timelabels;		
            <?php 
            foreach($Sensors as $key=>$value)
            {
                echo 'ChartConfig.data.datasets['.(int)($key-1).'].data = '.$value.';';
            }
            ?>
    		var ctx = document.getElementById("<?php echo $Sensor;?>").getContext('2d');
	    	window.myLine = Chart.Line(ctx, ChartConfig);
	</script>	
    </div>
    <p>
<?php  
}
?>
