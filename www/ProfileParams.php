<?php require_once 'view/ShowProfilesParams.php';  ?>
<?php require_once 'db/ProfilesData.php';  ?>
<?php require_once 'db/SensorsData.php';  ?>
<?php require_once 'db/ProfileParamsData.php';  ?>
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
                Настройка параметров профиля
                <p>
                <?php
                    //если была команда обновления параметров профиля, отображать остальную страницу не нужно (после запроса будет обновление)
                    if(!empty($_POST)) {
                        if($_POST['Action']=='Update')
                        {
                            UpdateProfileParams($_POST['Profile'],$_POST['Sensor'],$_POST['RecTime'],$_POST['Temperature']);
                        }
                        else if($_POST['Action']=='Delete') 
                        {
                            DeleteProfileParams($_POST['Profile'],$_POST['Sensor'],$_POST['RecTime']);
                        }
                    }
                    else
                    {
                        $Profiles=GetProfiles();
                        if(!empty($_GET['Profile'])) {
                            $CurrentProfile=$_GET['Profile'];    
                        }
                        else {
                            $CurrentProfile=GetCurrentProfile();
                        }
                        $Sensors=GetSensors();
                        if(!empty($_GET['Sensor'])) {
                            $CurrentSensor=$_GET['Sensor'];    
                            $ProfileParams=GetProfileParams($CurrentProfile,$CurrentSensor);
                        }
                        else {
                            //если не задан датчик и профиль, выбираем первый не пустой
                            foreach($Sensors as $value) {
                                $ProfileParams=GetProfileParams($CurrentProfile,$value);
                                if(!empty($ProfileParams)) {
                                    $CurrentSensor=$value;
                                    break;
                                }
                            }
                        }
                        //сначала отображается форма выбора профиля и датчика
                        ShowSelectProfileParams($Profiles, $Sensors, $CurrentProfile, $CurrentSensor);
                ?>
                        <p><a href="Profiles.php">Редактировать список профилей</a></p>
                <?php
                        //дальше отображается таблица параметров выбранного профиля
                        echo '<p>Параметры профиля "'.$CurrentProfile.'" для датчика "'.$CurrentSensor.'"<p>';
                        ShowProfileParams($ProfileParams);
                        //кнопки добавить/удалить
                        ShowUpdateButtons($CurrentProfile, $CurrentSensor);
                    }
                ?>
                <script type="text/javascript">
                    window.onload = function() {
                        document.getElementById("DeleteRow").disabled = true;
                    }
                </script>
            </td>
        </tr>
    </table>
</body>
</html>

