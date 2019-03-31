<?php require_once 'view/ShowSettings.php';  ?>
<?php require_once 'view/ShowExternalSensors.php';  ?>
<?php require_once 'view/ShowHeaters.php';  ?>
<?php require_once 'db/SettingsData.php';  ?>
<?php require_once 'db/ProfilesData.php';  ?>
<?php require_once 'db/SensorsData.php';  ?>
<?php require_once 'db/HeatersData.php';  ?>
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
                <?php
                    //если настройки изменены пользователем, они придут в параметрах запроса
                    if(!empty($_POST)) {
                        if($_POST['Action']=='UpdateSettings')
                        {
                            UpdateSettings($_POST);
                        }
                        else if($_POST['Action']=='UpdateSensor')
                        {
                            UpdateExternalSensor($_POST['Channel'],$_POST['Sensor']);
                        }
                        else if($_POST['Action']=='DeleteSensor')
                        {
                            DeleteExternalSensor($_POST['Channel']);
                        }
                        else if($_POST['Action']=='UpdateHeaters')
                        {
                            UpdateHeaters($_POST);
                        }
                    }
                    else //после POST-запроса сразу вызывается обновление страницы, отображение нужно только если запроса не было
                    {
                        $Profiles=GetProfiles();
                        //далее отображение актуальных настроек
                        $SettingsData=GetSettings();
                        ShowSettings($SettingsData, $Profiles);
                ?>
                        <br><br><br>
                        <p>Внешние датчики BALDR</p>
                <?php
                        $ExternalSensors=GetExternalSensors();
                        ShowExternalSensors($ExternalSensors);
                        ?>
                        <br><br><br>
                        <p>Приводы радиаторов отопления</p>
                        <?php
                        $Sensors=GetSensors();
                        $Heaters=GetHeaters();
                        ShowHeaters($Heaters, $Sensors);
                    }
                ?>
                <script type='text/javascript'>
                        window.onload = function() {
                        document.getElementById("DeleteSensor").disabled = true;
                        document.getElementById("SendHeatersParams").disabled = true;
                        document.getElementById("SendSettings").disabled = true;
                    }
                </script>

            </td>
        </tr>
    </table>
</body>
</html>

