#include <mysql.h>
#include <modbus.h>
#include <modbus-rtu.h>
#include <stdio.h>
#include "mysqldata.h"
#include "sensordata.h"

const char* DBSERVER="127.0.0.1";
const char* DATABASE="modsensor";
const char* DB_USER="modsensor";
const char* DB_PASS="modsensor";


int main(int argc, char **argv)
{
    char strModbusDevice[FILENAME_MAX];
    int nModbusBaud=0;

    MYSQL *pConnection = mysql_init(NULL);
    int Result=1;
    if (pConnection != NULL)
    {
        //подключаемся к БД
        if (mysql_real_connect(pConnection, DBSERVER, DB_USER, DB_PASS, DATABASE, 0, NULL, 0) != NULL)
        {
            if(GetModbusSettings(pConnection, strModbusDevice, &nModbusBaud) != -1)
            {
                modbus_t *pModbusCtx=pModbusCtx = modbus_new_rtu(strModbusDevice, nModbusBaud, 'N', 8, 1);
                if (pModbusCtx != NULL) {
                    //подключаемся к датчику
                    int nModbusResult=ConnectSensor(pModbusCtx);
                    if(nModbusResult!=-1)
                    {
                        //последовательно заполняем таблицы
                        nModbusResult=FillRegistersData(pModbusCtx,pConnection);
                        if(nModbusResult!=-1)
                        {
                            nModbusResult=FillInputsData(pModbusCtx,pConnection);
                            if(nModbusResult!=-1)
                            {
                                //формирование значений выходов
                                nModbusResult=UpdateCurrentState(pConnection);
                                if(nModbusResult!=-1)
                                {
                                    //установка выходов
                                    nModbusResult=UpdateCoilsState(pModbusCtx,pConnection);
                                    if(nModbusResult!=-1)
                                    {
                                        Result=0;
                                    }
                                }
                            }
                        }
                    }
                    modbus_free(pModbusCtx);
                }
            }
        }
        mysql_close(pConnection);
    }
    return Result;
}
