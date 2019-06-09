#include <mysql.h>
#include <modbus.h>
#include <stdio.h>

#include "mysqldata.h"
#include "sensordata.h"

static const char* DBSERVER = "127.0.0.1";
static const char* DATABASE = "modsensor";
static const char* DB_USER  = "modsensor";
static const char* DB_PASS  = "modsensor";


int main(int argc, char **argv)
{
    char strModbusDevice[FILENAME_MAX];
    int nModbusBaud = 0;

    MYSQL *pConnection = mysql_init(NULL);
    int Result = 1;     //Linux return code for general error 
    if (pConnection != NULL)
    {
        //подключаемся к БД
        if (mysql_real_connect(pConnection, DBSERVER, DB_USER, DB_PASS, DATABASE, 0, NULL, 0) != NULL)
        {
            if(GetModbusSettings(pConnection, strModbusDevice, &nModbusBaud) != RES_ERROR)
            {
                modbus_t* pModbusCtx = modbus_new_rtu(strModbusDevice, nModbusBaud, 'N', 8, 1);
                if (pModbusCtx != NULL) {
                    //подключаемся к датчику
                    int nModbusResult = ConnectSensor(pModbusCtx);
                    if(nModbusResult != RES_ERROR)
                    {
                        //последовательно заполняем таблицы
                        nModbusResult = FillRegistersData(pModbusCtx, pConnection);
                        if(nModbusResult != RES_ERROR)
                        {
                            nModbusResult = FillInputsData(pModbusCtx, pConnection);
                            if(nModbusResult != RES_ERROR)
                            {
                                //формирование значений выходов
                                nModbusResult = UpdateCurrentState(pConnection);
                                if(nModbusResult != RES_ERROR)
                                {
                                    //установка выходов
                                    nModbusResult = UpdateCoilsState(pModbusCtx, pConnection);
                                    if(nModbusResult != RES_ERROR)
                                    {
                                        Result = 0;     // linux return code for OK
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
