<?php
require_once 'DbAuth.php';

function GetSettings()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if ($link) {
        $query='SELECT `TableSettings`.Device,`TableSettings`.Baud,`TableSettings`.LogData,`TableSettings`.AutoSelect,`TableProfiles`.Description
        FROM `TableSettings` INNER JOIN `TableProfiles` ON `TableSettings`.ManualProfile=`TableProfiles`.ProfileId';
        
        if ($result = mysqli_query($link, $query)) {
            if($row = mysqli_fetch_assoc($result) ){
                $Settings['Порт устройства']=$row['Device'];
                $Settings['Скорость порта']=$row['Baud'];
                $Settings['Запись журнала показаний в БД']=($row['LogData']=='1')?'Включена':'Выключена';
                $Settings['Автовыбор профиля (будни/выходные)']=($row['AutoSelect']=='1')?'Включен':'Выключен';
                $Settings['Выбранный профиль']=$row['Description'];
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $Settings;
}

function UpdateSettings($NewSettings) {
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    $strdata=$NewSettings['id2'];
    if ($link) {
        $query='UPDATE `TableSettings` SET `Device`="'.$NewSettings['id0'].
        '", '.'`Baud`="'.$NewSettings['id1'].
        '", `LogData`='.(($NewSettings['id2']=='Включена')?'1':'0').', ';
        //если включен автовыбор профиля, обновляем только значение
        if($NewSettings ['id3']=='Включен') {
            $query.='`Autoselect`=1';
        }
        //если автовыбор выключен, то сформируем подзапрос чтобы определить ID по описанию
        else {
            $query.='`Autoselect`=0, `ManualProfile`=(SELECT `TableProfiles`.ProfileId FROM `TableProfiles` WHERE `TableProfiles`.Description="'.
            $NewSettings ['id4'].'")';
        }
        if ($result = mysqli_query($link, $query)) {
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
}

?>