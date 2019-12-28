#include <mysql.h>
#include <modbus.h>
#include <stdio.h>

#include "mysqldata.h"
#include "sensordata.h"

static const char* DBSERVER = "127.0.0.1";
static const char* DATABASE = "modsensor";
static const char* DB_USER  = "modsensor";
static const char* DB_PASS  = "modsensor";

static int UpdateDB(modbus_t*pModbusCtx, MYSQL* pConnection)
{
    //заполняем таблицы БД аналоговыми параметрами
    int nModbusResult = FillRegistersData(pModbusCtx, pConnection);
    if(nModbusResult != RES_ERROR)
    {
        //заполняем таблицы БД дискретными параметрами
        nModbusResult = FillInputsData(pModbusCtx, pConnection);
        if(nModbusResult != RES_ERROR)
        {
            //запуск запроса формирования значений выходов
            nModbusResult = UpdateCurrentState(pConnection);
        }
    }
    return nModbusResult;
}

static int UpdateSensorData(MYSQL* pConnection, char* strModbusDevice, int nModbusBaud)
{
    int nModbusResult = RES_ERROR;
    
    modbus_t* pModbusCtx = modbus_new_rtu(strModbusDevice, nModbusBaud, 'N', 8, 1);
    if (pModbusCtx != NULL)
    {
        //подключаемся к датчику
        nModbusResult = ConnectSensor(pModbusCtx);
        if(nModbusResult != RES_ERROR)
        {
            //последовательно заполняем таблицы
            nModbusResult = UpdateDB(pModbusCtx, pConnection);
            if(nModbusResult != RES_ERROR)
            {
                //установка выходов значениями из БД
                nModbusResult = UpdateCoilsState(pModbusCtx, pConnection);
            }
        }
        modbus_free(pModbusCtx);
    }
    return nModbusResult;

}

int main(int argc, char **argv)
{
    MYSQL *pConnection = mysql_init(NULL);
    int Result = 1;     //Linux return code for general error 
    if (pConnection != NULL)
    {
        //подключаемся к БД
        if (mysql_real_connect(pConnection, DBSERVER, DB_USER, DB_PASS, DATABASE, 0, NULL, 0) != NULL)
        {
            char strModbusDevice[FILENAME_MAX];
            int nModbusBaud = 0;
            if(GetModbusSettings(pConnection, strModbusDevice, &nModbusBaud) != RES_ERROR)
            {
                int nModbusResult = UpdateSensorData(pConnection, strModbusDevice, nModbusBaud);
                if (nModbusResult == RES_OK)
                {
                    Result = 0; //Linux return code for OK 
                }
                
            }
        }
        mysql_close(pConnection);
    }
    return Result;
}


