<?php
function GetProfileParams($Profile, $Sensor)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        if ($result = mysqli_query($link, 'SELECT `TableProfileParams`.Time, `TableProfileParams`.Temperature 
            FROM `TableProfileParams` 
            INNER JOIN `TableSensors` ON `TableProfileParams`.SensorId=`TableSensors`.SensorId 
            INNER JOIN `TableProfiles` ON `TableProfileParams`.ProfileId=`TableProfiles`.ProfileId
            WHERE `TableProfiles`.Description="'.$Profile.
            '" AND `TableSensors`.SensorDescription="'.$Sensor.'"')) {
            while( $row = mysqli_fetch_assoc($result) ){
                $ProfileParams[$row['Time']]=$row['Temperature'];
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $ProfileParams;
}

function UpdateProfileParams($Profile, $Sensor, $Time, $Temperature)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='INSERT INTO `TableProfileParams` (`ProfileId`,`SensorId`,`Time`,`Temperature`) 
            VALUES (
            (SELECT `TableProfiles`.ProfileId FROM `TableProfiles` WHERE `TableProfiles`.Description="'.$Profile.'"),
            (SELECT `TableSensors`.SensorId FROM `TableSensors` WHERE `TableSensors`.SensorDescription="'.$Sensor.'"), 
            "'.$Time.'", "'.$Temperature.'") ON DUPLICATE KEY UPDATE 
            `TableProfileParams`.Temperature="'.$Temperature.'"';
        if ($result = mysqli_query($link,$query)) {
            mysqli_free_result($result);
        }
        
        $query='INSERT INTO `TableTemp` (`Test`) 
        VALUES ("'.$Sensor.'")';
        if ($result = mysqli_query($link,$query)) {
            mysqli_free_result($result);
        }

        mysqli_close($link);
   }
}

function DeleteProfileParams($Profile, $Sensor, $Time)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='DELETE FROM `TableProfileParams`
            WHERE  `TableProfileParams`.ProfileId=
            (SELECT `TableProfiles`.ProfileId FROM `TableProfiles` WHERE `TableProfiles`.Description="'.$Profile.'")
            AND `TableProfileParams`.SensorId=
            (SELECT `TableSensors`.SensorId FROM `TableSensors` WHERE `TableSensors`.SensorDescription="'.$Sensor.'") 
            AND `TableProfileParams`.Time="'.$Time.'"';
        if ($result = mysqli_query($link,$query)) {
            mysqli_free_result($result);
        }
        mysqli_close($link);
   }
}

?>