<?php
function ShowHeaters($Heaters, $Sensors) 
{
?>
    <script type='text/javascript'>
    var Sensors={values:{
    <?php
        //чтобы была возможность не выбирать ни одного датчика, добавляем датчик 'Не задано'
        $Sensors['0'] = 'Не задано';
        foreach ($Sensors as $value) {
            echo '"'.$value.'":"'.$value.'", ';
        }
    ?>
    }};
    var HeatersData = [    
    <?php
        if(!empty($Heaters)) 
        {
            foreach ($Heaters as $key => $value) {
                echo '{id:"'.$key.'", desc:"'.$value['description'].'", primary:"'.
                    $Sensors[$value['primary']].'", secondary:"'.$Sensors[$value['secondary']].'"},';
            }
        }
    ?>
    ];
    </script>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="heaters-table" style="width: 500px;">
      <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
      <script type="text/javascript">
        var tableHeaters = new Tabulator("#heaters-table", {
          data:HeatersData, //load initial data into table
          layout:"fitColumns", //fit columns to width of table (optional)
          columns:[
            {title:"Привод", field:"desc", editor:"input", headerSort:false},
            {title:"Основной датчик", field:"primary", editor:"select", editorParams:Sensors, headerSort:false},
            {title:"Резервный датчик", field:"secondary", editor:"select", editorParams:Sensors, headerSort:false},
            ],
            dataEdited:function(data) {
                document.getElementById("SendHeatersParams").disabled = false;
            },
        });
      </script>
    </div>
 
    <script type='text/javascript'>
    function SendHeatersParams() {
        var tableData = tableHeaters.getData();
        var xhr = new XMLHttpRequest();
        xhr.open("POST", 'Settings.php', true);
        xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
            if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
                window.location.reload(false); 
            }
        }
        var SerialData="Action=UpdateHeaters";
        tableData.forEach(function(item) {
            SerialData=SerialData+"&desc"+item["id"]+"="+item["desc"]+"&p"+item["id"]+"="+item["primary"]+"&s"+item["id"]+"="+item["secondary"];
        });
        xhr.send(SerialData);
        document.getElementById("SendHeatersParams").disabled = true;
    }
    </script>
    <button id="SendHeatersParams" onClick="SendHeatersParams()" disabled=true>Сохранить</button>
<?php  
}
?>
