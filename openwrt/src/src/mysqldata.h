#ifndef MYSQLDATA_H_
#define MYSQLDATA_H_

#include <mysql.h>

#define RES_OK    0
#define RES_ERROR -1


//функция возвращает порт и скорость порта MODBUS-устройства
int GetModbusSettings(MYSQL *con, char* strModbusDevice, int* nBaud);

//функция открывает таблицу регистров MODBUS, которые есть в базе данных
MYSQL_RES* OpenRegistersTable(MYSQL *con);

//функция открывает таблицу дискретных входов MODBUS, которые есть в базе данных
MYSQL_RES* OpenInputsTable(MYSQL *con);

//перебирает записи (регистры, дискретные входы) из предварительно открытой таблицы БД
int GetNextRecord(MYSQL* con, MYSQL_RES* QueryResult);

//функция открывает таблицу дискретных входов MODBUS, которые есть в базе данных
MYSQL_RES* OpenCoilsTable(MYSQL *con);

//перебирает записи (дискретные выходы) из предварительно открытой таблицы БД
int GetNextCoil(MYSQL* pConnection, MYSQL_RES* pQueryResult, int* pCoilValue);

#endif /* MYSQLDATA_H_ */
