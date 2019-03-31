#ifndef SENSORDATA_H_
#define SENSORDATA_H_

#include <modbus.h>
#include <mysql.h>

//подключение к датчику
int ConnectSensor(modbus_t *pModbusCtx);

//заполнение таблицы в БД значениями регистров MODBUS
int FillRegistersData(modbus_t *pModbusCtx, MYSQL *pConnection);

//заполнение таблицы в БД значениями дискретных входов MODBUS
int FillInputsData(modbus_t *pModbusCtx, MYSQL *pConnection);

//обновление таблиц в БД и формирование значений выходов
int UpdateCurrentState(MYSQL *pConnection);

//установка дискретных выходов MODBUS значениями из таблицы БД
int UpdateCoilsState(modbus_t *pModbusCtx, MYSQL *pConnection);


#endif
