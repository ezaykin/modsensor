<?php
require_once 'DbAuth.php';

function GetSensorLog($Sensor, $Period)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='CALL `GetLogData`("'.$Sensor.'", "'.$Period.'")';
        if ($result = mysqli_query($link, $query)) {
            while( $row = mysqli_fetch_assoc($result) ){
                $LogData['temperature']=$row['Temperature'];
                $LogData['pressure']=$row['Pressure'];
                $LogData['humidity']=$row['Humidity'];
                if(isset($row['Heating'])) {
                    $LogData['heating']=(string)($row['Heating']);
                }
                if($Period==1) {
                    //в качестве ключа будет время - получаем из строки даты-времени
                    $SensorLog[substr($row['EventTime'],11,5)]=$LogData;
                }
                else {
                    //в качестве ключа будут дата и время - получаем из строки даты-времени
                    $SensorLog[substr($row['EventTime'],5,11)]=$LogData;
                }
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
        return $SensorLog;
    }
}

?>