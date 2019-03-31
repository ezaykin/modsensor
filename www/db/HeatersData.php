<?php
require_once 'DbAuth.php';

function GetHeaters()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='SELECT * FROM `TableModbusCoils` 
        ORDER BY `TableModbusCoils`.Coil';
        if ($result = mysqli_query($link, $query)) {
            while( $row = mysqli_fetch_assoc($result) ){
                $Heater['description']=$row['Description'];
                $Heater['primary']=$row['PrimarySensor'];
                $Heater['secondary']=$row['SecondarySensor'];
                $Heaters[$row['Coil']]=$Heater;
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $Heaters;
}

function UpdateHeaters($HeatersData)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        array_shift($HeatersData);  //убираем параметр ACTION из массива
        $HeatersCount=count($HeatersData)/3;    //на каждый привод передается 3 параметра
        for ($i = 0; $i<$HeatersCount; $i++) {
            $query='UPDATE `TableModbusCoils`
            SET `TableModbusCoils`.Description="'.$HeatersData['desc'.$i].'",
                `TableModbusCoils`.PrimarySensor=`GetSensorId`("'.$HeatersData['p'.$i].'"),
                `TableModbusCoils`.SecondarySensor=`GetSensorId`("'.$HeatersData['s'.$i].'") 
                WHERE `TableModbusCoils`.Coil="'.$i.'"';    
            if ($result = mysqli_query($link, $query)) {
                    mysqli_free_result($result);
            }
        }
    }
    mysqli_close($link);
}

?>