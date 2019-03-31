<?php require_once 'view/ShowStatus.php';  ?>
<?php require_once 'db/CurrentSensorData.php';  ?>
<?php require_once 'db/SettingsData.php';  ?>
<?php require_once 'db/ProfilesData.php';  ?>
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
                    $InternalSensorData=GetCurrentInternal();
                    $ExternalSensorData=GetCurrentExternal();
                    $CurrentProfile=GetCurrentProfile();
                    $Settings=GetSettings();
                ?>
                Текущее состояние системы
                <p>
                <?php ShowCurrentInternal($InternalSensorData); ?>
                <p>
                <?php ShowCurrentExternal($ExternalSensorData); ?>
                <p>
                <?php echo 'Автовыбор профиля (Будни/Выходные) '.
                mb_convert_case($Settings['Автовыбор профиля (будни/выходные)'], MB_CASE_LOWER, "UTF-8").
                ', '.'текущий профиль: "'.$CurrentProfile.'"'; ?>
                <p>
            </td>
        </tr>
    </table>
</body>
</html>