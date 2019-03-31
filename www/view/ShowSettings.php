<?php
function ShowSettings($SettingsData, $Profiles) 
{
?>
    <script type='text/javascript'>
        var SettingsData = [
        <?php
            if(!empty($SettingsData)) 
            {
                //подготавливаем данные для отображения в таблице
                $id=0;
                foreach ($SettingsData as $key => $value) {
                    if(($key!='Выбранный профиль') || ($SettingsData['Автовыбор профиля (будни/выходные)']!='Включен')) {
                        echo '{id:"'.$id.'", param:"'.$key.'", value:"'.$value.'"},';
                        $id++;
                    }
                }
            }
        ?>
        ];
    </script>
    Настройки системы
    <p>
    <link href="dist/css/tabulator.min.css" rel="stylesheet">
    <div id="settings-table" style="width: 500px;">
    <script type="text/javascript" src="dist/js/tabulator.min.js"></script>
    <script type="text/javascript">
    const LOGDATA_POS=2;
    const AUTOSELECT_POS=3;
    const PROFILE_POS=4;
    var editCheck = function(cell){
        var pos = cell.getRow().getPosition(true);
        return (pos!=LOGDATA_POS)&&(pos!=AUTOSELECT_POS);
    }
    <?php echo 'var selValue="'.$SettingsData['Выбранный профиль'].'";'; ?>
    var cellEditor = function(cell, onRendered, success, cancel, editorParams){
        var pos = cell.getRow().getPosition(true);
        if(pos==PROFILE_POS) {
            var editor=document.createElement("select");
            <?php foreach ($Profiles as $Profile) { ?>
                var opt=document.createElement("option");
                <?php echo 'opt.value="'.$Profile.'";'; ?>
                <?php echo 'opt.text="'.$Profile.'";'; ?>
                editor.appendChild(opt);
            <?php } ?>                  
            editor.value=selValue;
        }
        else {
            var editor=document.createElement("input");
        }
        editor.style.padding = "3px";
        editor.style.width = "100%";
        editor.style.boxSizing = "border-box";
        var pos=cell.getRow().getPosition(true);
        //Set value of editor to the current value of the cell
        if(pos!=PROFILE_POS) {
            editor.value = cell.getValue();
        }
        //set focus on the select box when the editor is selected (timeout allows for editor to be added to DOM)
        onRendered(function(){
            editor.focus();
            editor.style.css = "100%";
        });
        //when the value has been set, trigger the cell to update
        function successFunc(){
            if(pos==PROFILE_POS) {  //чтобы сохранялось выбранное значение
                selValue=editor.value;
            }
            success(editor.value);
        }
        editor.addEventListener("change", successFunc);
        editor.addEventListener("blur", successFunc);
        return editor;
    };
    var tableSettings = new Tabulator("#settings-table", {
        data:SettingsData, //load initial data into table
        layout:"fitColumns", //fit columns to width of table (optional)
        columns:[
            {title:"Параметр", field:"param", headerSort:false},
            {title:"Значение", field:"value", headerSort:false, editor:cellEditor, editable:editCheck, cellClick:function(e, cell){
                    //e - the click event object
                    //cell - cell component
                    var pos=cell.getRow().getPosition(true);
                    if(pos==LOGDATA_POS) {
                        var val=cell.getValue();
                        if(val=='Включена') {
                            cell.setValue('Выключена');
                        }
                        else {
                            cell.setValue('Включена');
                        }
                        EnableSendButton();
                    }
                    else if(pos==AUTOSELECT_POS) {
                        var val=cell.getValue();
                        if(val=='Включен') {
                            cell.setValue('Выключен');
                            tableSettings.addRow({id:AUTOSELECT_POS+1,param:"Выбранный профиль",value:selValue});
                        }
                        else {
                            cell.setValue('Включен');
                            tableSettings.deleteRow(PROFILE_POS);
                        }
                    }
                },
            },
        ],
        dataEdited:function(data){
            document.getElementById("SendSettings").disabled = false;  
        },

    });
  function SendSettings() {
    var SettingsData = tableSettings.getData();
    var xhr = new XMLHttpRequest();
    xhr.open("POST", 'Settings.php', true);
    xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function() {//Вызывает функцию при смене состояния.
      if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
          window.location.reload(false); 
      }
    }
    var SerialData="Action=UpdateSettings";
    SettingsData.forEach(function(item, i, SettingsData) {
        SerialData=SerialData+"&id"+item["id"]+"="+item["value"];
    });
    xhr.send(SerialData);
    document.getElementById("SendSettings").disabled = true;
  }
  </script>
</div>
<button id="SendSettings" onClick="SendSettings()" disabled=true>Сохранить настройки</button>

<?php
}
?>