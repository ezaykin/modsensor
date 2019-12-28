#include "mysqldata.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//функция возвращает порт и скорость порта MODBUS-устройства
int GetModbusSettings(MYSQL *pConnection, char* strModbusDevice, int* nBaud)
{
    int Result = RES_ERROR;
    const char* strQuerySettings = "SELECT `TableSettings`.Device, `TableSettings`.Baud from `TableSettings`;";
    //получаем настройки
    if (mysql_query(pConnection, strQuerySettings)>=0) 
    {
        MYSQL_RES* pQueryResult = mysql_store_result(pConnection);
        if (pQueryResult)
        {
            MYSQL_ROW row = mysql_fetch_row(pQueryResult);
            strncpy(strModbusDevice, row[0], FILENAME_MAX);
            *nBaud = atoi(row[1]);
            mysql_free_result(pQueryResult);
            Result = RES_OK;
        }
    }
    return Result;
}

//функция открывает таблицу регистров MODBUS, которые есть в базе данных
MYSQL_RES* OpenRegistersTable(MYSQL *pConnection)
{
    MYSQL_RES* pQueryResult = NULL;
    char* strQuery = "SELECT `TableModbusRegisters`.Register from `TableModbusRegisters` ORDER BY `TableModbusRegisters`.Register;";
    if (mysql_query(pConnection, strQuery)>=0) 
    {
        pQueryResult = mysql_store_result(pConnection);
    }
    return pQueryResult;
}

//функция открывает таблицу дискретных входов MODBUS, которые есть в базе данных
MYSQL_RES* OpenInputsTable(MYSQL *pConnection)
{
    MYSQL_RES* pQueryResult = NULL;
    const char* strQuery = "SELECT `TableModbusInputs`.ModbusInput from `TableModbusInputs` ORDER BY `TableModbusInputs`.ModbusInput;";
    if (mysql_query(pConnection, strQuery)>=0) 
    {
        pQueryResult = mysql_store_result(pConnection);
    }
    return pQueryResult;
}

//перебирает записи (регистры, дискретные входы) из предварительно открытой таблицы БД
int GetNextRecord(MYSQL* pConnection, MYSQL_RES* pQueryResult)
{
    int nModbusRegister = RES_ERROR;
    MYSQL_ROW row = mysql_fetch_row(pQueryResult);
    if (row != NULL)
    {
        nModbusRegister = atoi(row[0]);
    }
    return nModbusRegister;
}

//функция открывает таблицу дискретных входов MODBUS, которые есть в базе данных
MYSQL_RES* OpenCoilsTable(MYSQL *pConnection)
{
    MYSQL_RES* pQueryResult = NULL;
    char* strQuery = "SELECT `TableCurrentCoilsValues`.ModbusCoil, `TableCurrentCoilsValues`.Value from `TableCurrentCoilsValues` ORDER BY `TableCurrentCoilsValues`.ModbusCoil;";
    if (mysql_query(pConnection, strQuery)>=0) 
    {
        pQueryResult = mysql_store_result(pConnection);
    }
    return pQueryResult;
}

//перебирает записи (дискретные выходы) из предварительно открытой таблицы БД
int GetNextCoil(MYSQL* pConnection, MYSQL_RES* pQueryResult, int* pCoilValue)
{
    int nModbusCoil = RES_ERROR;
    *pCoilValue = 0;
    MYSQL_ROW row = mysql_fetch_row(pQueryResult);
    if (row != NULL)
    {
        nModbusCoil = atoi(row[0]); // в первом столбце - номер регистра
        if(row[1][0] == '1')        // во втором - значение
        {
           *pCoilValue = 1; 
        }
    }
    return nModbusCoil;
}
