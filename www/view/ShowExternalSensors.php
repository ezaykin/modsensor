<?php
function ShowExternalSensors($ExternalSensors) 
{
?>
    <script type='text/javascript'>
    var ExternalSensors = [    
    <?php
        if(!empty($ExternalSensors)) 
        {
            foreach ($ExternalSensors as $key => $value) {
                echo '{channel:"'.$key.'", desc:"'.$value.'"}, ';
            }
        }
    ?>
    ];
    </script>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="external-sensor-table" style="width: 500px;">
        <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
        <script type="text/javascript">
            var SelChannel;
            var tableExternal = new Tabulator("#external-sensor-table", {
            data:ExternalSensors, //load initial data into table
            layout:"fitColumns", //fit columns to width of table (optional)
            selectable:1, //make rows selectable
            columns:[
                {title:"Датчик", field:"desc", headerSort:false},
                {title:"Канал", field:"channel", width:100, headerSort:false},
            ],
            rowSelected:function(row){
                SelChannel=row.getData()["channel"];
                var StrSensor=row.getData()["desc"];
                document.getElementById("inputChannel").value = SelChannel;
                document.getElementById("inputSensor").value = StrSensor;
                document.getElementById("DeleteSensor").disabled = false;
            },
            rowDeselected:function(row){
                document.getElementById("DeleteSensor").disabled = true;
            },
            });
        </script>
    </div>
    <script type='text/javascript'>
    function UpdateSensor() {
            document.getElementById("DeleteSensor").disabled = true;
            var StrChannel=document.getElementById("inputChannel").value;
            var StrSensor=document.getElementById("inputSensor").value;
            var xhr = new XMLHttpRequest();
            xhr.open("POST", 'Settings.php', true);
            //Передает правильный заголовок в запросе
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
                if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                    // Запрос завершен. Здесь можно обрабатывать результат.
                    window.location.reload(false); 
                }
            }
            var query="Action=UpdateSensor&Channel="+StrChannel+"&Sensor="+StrSensor;
            xhr.send(query); 
        }
    
        function DeleteSensor() {
            document.getElementById("DeleteSensor").disabled = true;
            var xhr = new XMLHttpRequest();
            xhr.open("POST", 'Settings.php', true);
            //Передает правильный заголовок в запросе
            xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
                if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                    // Запрос завершен. Здесь можно обрабатывать результат.
                    window.location.reload(false); 
                }
            }
            var query="Action=DeleteSensor&Channel="+SelChannel;
            xhr.send(query); 
        }
    </script>
    <button id="DeleteSensor" onclick="DeleteSensor()" disabled>Удалить датчик</button>
    <p>
    <p>
    Место установки
    &nbsp&nbsp&nbsp&nbsp
    Номер канала
    &nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp
    &nbsp&nbsp&nbsp&nbsp
    <br>
    <input type="text" id="inputSensor" value="Новый датчик"></input>
    <select id="inputChannel" size="1">
    <option value="1">1</option>
    <option value="2">2</option>
    <option value="3">3</option>
    </select>
    <button id="UpdateSensor" onClick="UpdateSensor()">Добавить/изменить</button>
<?php  
}
?>
