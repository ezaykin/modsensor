<?php
function ShowSelectProfileParams($Profiles, $Sensors, $CurrentProfile, $CurrentSensor)
{
?>
<form action="ProfileParams.php" method="get">
  <select name="Profile" size="1">
<?php
  if (!empty($Profiles)){
    foreach ($Profiles as $value) {
        echo '<option ';
        if($value == $CurrentProfile){
          echo 'selected ';
        }
        echo 'value="'.$value.'">'.$value.'</option>';
    }
  }
?>
</select>
<select name="Sensor" size="1">
<?php
  if (!empty($Sensors)){
    foreach ($Sensors as $value) {
        echo '<option ';
        if($value == $CurrentSensor){
          echo 'selected ';
        }
        echo 'value="'.$value.'">'.$value.'</option>';
    }
  }
?>
</select>
<input type="submit"value="Отобразить параметры" />
</form>
<p>
<?php
}

function ShowProfileParams($ProfileData)
{
?>
<link href="dist/css/tabulator.min.css" rel="stylesheet">
<div id="params-table" style="width: 500px;">
  <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
  <script type="text/javascript">
    var tabledata = [
    <?php
        if(!empty($ProfileData)) {
            foreach($ProfileData as $key => $value)
            {
                echo '{time:"'.$key.'", temp:"'.$value.'"},';
            }
        }
    ?>
    ];
    var SelTime;
    var table = new Tabulator("#params-table", {
      data:tabledata, //load initial data into table
      layout:"fitColumns", //fit columns to width of table (optional)
      selectable:1, //make rows selectable
      addRowPos:"bottom",
      columns:[ //Define Table Columns
        {title:"Время", field:"time", /*width:150, */headerSort:false},
        {title:"Температура", field:"temp", /*width:150,*/ align:"left", headerSort:false},
      ],
      rowSelected:function(row){
        SelTime=row.getData()["time"];
        document.getElementById("inputTime").value = SelTime;
        var RowTemp=row.getData()["temp"];
        document.getElementById("inputTemp").value = RowTemp;
        document.getElementById("DeleteRow").disabled = false;
      },
      rowDeselected:function(row){
            document.getElementById("DeleteRow").disabled = true;
        },
    });
  </script>
</div>
<?php
}

function ShowUpdateButtons($CurrentProfile, $CurrentSensor)
{
?>    
    <script type="text/javascript">
        function AddRow() {
            document.getElementById("DeleteRow").disabled = true;
            var StrTime=document.getElementById("inputTime").value;
            var StrTemp=document.getElementById("inputTemp").value;
            var xhr = new XMLHttpRequest();
            xhr.open("POST", 'ProfileParams.php', true);
            //Передает правильный заголовок в запросе
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
                if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                    // Запрос завершен. Здесь можно обрабатывать результат.
                    window.location.reload(false); 
                }
            }
            var query="Action=Update&Profile=<?php echo $CurrentProfile.'&Sensor='.$CurrentSensor.'&RecTime='; ?>"+StrTime+"&Temperature="+StrTemp;
            xhr.send(query); 
        }
    
        function DelRow() {
            document.getElementById("DeleteRow").disabled = true;
            var xhr = new XMLHttpRequest();
            xhr.open("POST", 'ProfileParams.php', true);
            //Передает правильный заголовок в запросе
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
                if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                    // Запрос завершен. Здесь можно обрабатывать результат.
                    window.location.reload(false); 
                }
            }
            var query="Action=Delete&Profile=<?php echo $CurrentProfile.'&Sensor='.$CurrentSensor.'&RecTime='; ?>"+SelTime;
            xhr.send(query); 
        }
    </script>
    <button id="DeleteRow" onclick="DelRow()" disabled>Удалить выбранную запись</button>
    <br>
    <br>
    Время начала интервала

    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp

    Температура 

    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    <br>
    <input type="time" id="inputTime" value="00:00"></input>

    &nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp

    <input type="number" id="inputTemp" name="inputTemp" size="2" value="20"></input>
    <button id="AddRow" onClick="AddRow()">Добавить/изменить</button>

<?php
}
?>
