<?php
require_once 'DbAuth.php';

function GetCurrentInternal()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if ($link) {
        $query='select `TableInternalSensorValues`.`SensorId` AS `SensorId`,`TableInternalSensorValues`.`UpdateTime` AS `UpdateTime`,
        `TableInternalSensorValues`.`Temperature` AS `Temperature`,`TableInternalSensorValues`.`Pressure` AS `Pressure`,
        if(`TableInternalSensorValues`.`Heating` is null,"Не задано",
        if(`TableInternalSensorValues`.`Heating`,"Включен","Выключен")) AS `Heating`
         from `TableInternalSensorValues`';
        if ($result = mysqli_query($link, $query)) {
            if($row = mysqli_fetch_assoc($result) ){
                $InternalSensorData=$row;
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $InternalSensorData;
}

function GetCurrentExternal()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if ($link) {
        $query='select `TableExternalSensorsValues`.`SensorId` AS `SensorId`,`TableExternalSensorsValues`.`UpdateTime` AS `UpdateTime`,
        `TableExternalSensorsValues`.`Temperature` AS `Temperature`,`TableExternalSensorsValues`.`Humidity` AS `Humidity`,
        if(`TableExternalSensorsValues`.`Battery` = "0","Норма","Не норма") AS `Battery`,
        if(`TableExternalSensorsValues`.`Battery` = "0",NULL,`GetBatteryWarning`(`TableExternalSensorsValues`.`SensorId`)) AS `BatteryWarning`,
        if(`TableExternalSensorsValues`.`Heating` is null,"Не задано",
        if(`TableExternalSensorsValues`.`Heating`,"Включен","Выключен")) AS `Heating`,
        `TableSensors`.`SensorDescription` AS `SensorDescription` 
        from (`TableExternalSensorsValues` join `TableSensors` 
        on(`TableExternalSensorsValues`.`SensorId` = `TableSensors`.`SensorId`))';
        if ($result = mysqli_query($link, $query)) {
                while( $row = mysqli_fetch_assoc($result) ){
                $ExternalSensorData[]=$row;
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $ExternalSensorData;
}

?>