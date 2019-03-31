<?php require_once 'view/ShowProfiles.php';  ?>
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
                    if(!empty($_POST)) {
                        if($_POST['Action']=='Add')
                        {
                            AddProfile($_POST['Profile']);
                        }
                        else if($_POST['Action']=='Delete') 
                        {
                            DeleteProfile($_POST['Profile']);
                        }
                    }
                    else {
                        //сначала отображается форма выбора профиля и датчика
                        $Profiles=GetProfiles();
                        ShowProfiles($Profiles);
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

