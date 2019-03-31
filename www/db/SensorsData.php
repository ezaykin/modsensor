<?php
require_once 'DbAuth.php';

function GetSensors()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        if ($result = mysqli_query($link, 'SELECT * FROM `TableSensors` ORDER BY `TableSensors`.SensorId')) {
            while( $row = mysqli_fetch_assoc($result) ){
                $Sensors[$row['SensorId']]=$row['SensorDescription'];
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $Sensors;
}

//для отображения нужен номер канала и описание датчика. Номер канала можно получить из связанной таблицы
function GetExternalSensors()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        //у внутреннего датчика зарезервированный ID, поэтому возвращаем все, для которых назначены ModbusInputs
        //номер канала на 1 больше номера ModbusInputs
        $query='SELECT `TableSensors`.SensorDescription, (`TableModbusInputs`.ModbusInput+1) AS `Channel`
            FROM `TableSensors` INNER JOIN `TableModbusInputs` 
            ON `TableSensors`.SensorId=`TableModbusInputs`.SensorId WHERE (`TableSensors`.SensorId > 1)';
        if ($result = mysqli_query($link, $query)) {
            while( $row = mysqli_fetch_assoc($result) ){
                $ExternalSensors[$row['Channel']]=$row['SensorDescription'];
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $ExternalSensors;
}

function UpdateExternalSensor($Channel, $Sensor)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='CALL `AddBaldrSensor`("'.$Channel.'", "'.$Sensor.'")';
        if ($result = mysqli_query($link, $query)) {
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
}

function DeleteExternalSensor($Channel)
{
    if(($Channel>0)&&($Channel<4))
    {
        $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
        if($link)
        {
            $query='CALL `DeleteBaldrSensor`("'.$Channel.'")';
            if ($result = mysqli_query($link, $query)) {
                mysqli_free_result($result);
            }
            mysqli_close($link);
        }
    }
}

?>