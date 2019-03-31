<?php require_once 'view/ShowSensorsLog.php';  ?>
<?php require_once 'db/SensorsData.php';  ?>
<?php require_once 'db/SensorsLogData.php';  ?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Система управления отоплением</title>
</head>
<body>
    <table width="100%" cellspacing="0" cellpadding="5">
        <tr> 
            <td width="100" valign="top">
                <?php include 'MainMenu.php' ?>
            </td>
            <td valign="top" align="center">
                <p>Журнал показаний датчиков</p>
                <p>
                <a href="SensorsLog.php">За сутки</a>&nbsp&nbsp
                <a href="SensorsLog.php?Period=7">За неделю</a>&nbsp&nbsp
                <a href="SensorsLog.php?Period=30">За месяц</a>
                <?php
                    $Sensors=GetSensors();
                    $Period=1;
                    if(!empty($_GET['Period'])) {
                        $Period=$_GET['Period'];
                    }
                    foreach($Sensors as $value)
                    {
                        $SensorData=GetSensorLog($value, $Period);
                        ShowSensorLog($value, $SensorData);
                    }
                ?>
            </td>
        </tr>
    </table>
</body>
</html>