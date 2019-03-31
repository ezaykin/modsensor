<?php
function ShowProfiles($Profiles)
{
?>
    Редактирование списка профилей
    <br>
    <br>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="profiles-table" style="width: 200px;">
    <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
    <script type="text/javascript">
        var tabledata=[    
        <?php
        if(!empty($Profiles))
        {
            foreach($Profiles as $key =>$value)
            {
                echo '{id:"'.$key.'", desc:"'.$value.'"},';
            }
        }
        ?>
        ];
        var ProfileDesc;
        var table = new Tabulator("#profiles-table", {
            data:tabledata, //load initial data into table
            layout:"fitColumns", //fit columns to width of table (optional)
            selectable:1, //make rows selectable
            addRowPos:"bottom",
            columns:[{title:"Профиль", field:"desc", /*width:200,*/ headerSort:false}],
            rowSelected:function(row){
                ProfileDesc=row.getData()["desc"];
                var ProfileId=row.getData()["id"];
                if(ProfileId>2) {
                    document.getElementById("DeleteRow").disabled = false;
                }
                else {
                    document.getElementById("DeleteRow").disabled = true; 
                    table.deselectRow();
                    ProfileDesc="";
                }
            },
            rowDeselected:function(row){
                document.getElementById("DeleteRow").disabled = true;
            },
        });
    </script>
    </div>
    <button id="DeleteRow" onclick="DelRow()" disabled>Удалить выбранную запись</button>
    <br>
    <br>
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    Новый профиль
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    <br>
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    <input type="text" id="ProfileDesc" value="Новый профиль"></input>
    <button id="AddRow" onClick="AddRow()">Добавить</button>
    <script type="text/javascript">
    function AddRow() {
        document.getElementById("DeleteRow").disabled = true;
        ProfileDesc=document.getElementById("ProfileDesc").value;
        var xhr = new XMLHttpRequest();
        xhr.open("POST", 'Profiles.php', true);
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
            if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                window.location.reload(false); 
            }
        }
        var query="Action=Add&Profile="+ProfileDesc;
        xhr.send(query); 
    }
    function DelRow() {
        document.getElementById("DeleteRow").disabled = true;
        var xhr = new XMLHttpRequest();
        xhr.open("POST", 'Profiles.php', true);
        //Передает правильный заголовок в запросе
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
        if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
            // Запрос завершен. Здесь можно обрабатывать результат.
            window.location.reload(false); 
        }
        }
        var query="Action=Delete&Profile="+ProfileDesc;
        xhr.send(query); 
    }
    </script>
<?php
}
?>
