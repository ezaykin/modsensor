<?php
function ShowCurrentInternal($InternalSensorData) 
{
?>
    <script type='text/javascript'>
    var InternalSensorData = [{    
    <?php
        if(!empty($InternalSensorData)) 
        {
            foreach ($InternalSensorData as $key => $value) {
                echo $key.':"'.$value.'", ';
            }
        }
    ?>
    }];
    </script>
    Внутренний датчик
    <p>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="internal-sensor-table" style="width: 500px;">
      <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
      <script type="text/javascript">
        var tableInternal = new Tabulator("#internal-sensor-table", {
          data:InternalSensorData, //load initial data into table
          layout:"fitColumns", //fit columns to width of table (optional)
          columns:[
            {title:"Температура", field:"Temperature", headerSort:false},
            {title:"Давление", field:"Pressure", headerSort:false},
            {title:"Обогрев", field:"Heating", headerSort:false},
            {title:"Данные обновлены", field:"UpdateTime", width:150,headerSort:false},
            ],
        });
      </script>
    </div>
<?php  
}

function ShowCurrentExternal($ExternalSensorData) 
{
?>
    <script type='text/javascript'>
    var ExternalSensorData = [    
    <?php
        if(!empty($ExternalSensorData)) 
        {
            foreach ($ExternalSensorData as $SensorData) {
                echo '{';
                foreach ($SensorData as $key => $value) {
                    echo $key.':"'.$value.'", ';
                }
                echo '},';
            }
        }
    ?>
    ];
    </script>
    Внешние датчики
    <p>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="external-sensor-table" style="width: 650px;">
        <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
        <script type="text/javascript">
            var tableExternal = new Tabulator("#external-sensor-table", {
            data:ExternalSensorData, //load initial data into table
            layout:"fitColumns", //fit columns to width of table (optional)
            columns:[
                {title:"Датчик", field:"SensorDescription", headerSort:false},
                {title:"Температура", field:"Temperature", width:100, headerSort:false},
                {title:"Влажность", field:"Humidity", width:90, headerSort:false},
                {title:"Батарейка", field:"Battery", width:90, headerSort:false},
                {title:"Обогрев", field:"Heating", width:90, headerSort:false},
                {title:"Данные обновлены", field:"UpdateTime", width:150, headerSort:false}
            ],
            });
        </script>
    </div>
    <?php
        foreach ($ExternalSensorData as $SensorData) {
            if(isset($SensorData['BatteryWarning'])) {
                echo 'Статус батарейки "Не норма" для датчика "'.$SensorData['SensorDescription'].' c '.$SensorData['BatteryWarning'].'<br>';
            }
        }
    ?>
<?php  
}
?>
