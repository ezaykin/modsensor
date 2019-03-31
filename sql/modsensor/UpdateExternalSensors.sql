-- --------------------------------------------------------
-- Хост:                         192.168.5.1
-- Версия сервера:               10.2.19-MariaDB - Source distribution
-- Операционная система:         Linux
-- HeidiSQL Версия:              10.1.0.5464
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;

-- Дамп структуры для процедура modsensor.UpdateExternalSensors
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` PROCEDURE `UpdateExternalSensors`(
	IN `UpdateTime` DATETIME



















)
BEGIN
	#выключаем предупреждение о том, что таблица уже существует
	SET sql_notes = 0;
	CREATE TABLE IF NOT EXISTS `TableExternalSensorsValues` (
		`SensorId` INT NOT NULL,
		`UpdateTime` DATETIME NOT NULL,
		`Temperature` FLOAT NOT NULL,
		`Humidity` INT,
		`Battery` BOOL,
		`Heating` BOOL,
		PRIMARY KEY (`SensorId`)
	)
	COLLATE='utf8_general_ci'
	ENGINE = MEMORY
	;
	SET sql_notes = 1;
	#обновляем значение температуры из соответствующего регистра Modbus
	INSERT INTO `TableExternalSensorsValues` (`SensorId`,`Temperature`,`UpdateTime`) 
	(SELECT `TableModbusRegisters`.`SensorId`,`GetSensorTemperature`(`TableCurrentRegisterValues`.`RegisterValue`), UpdateTime 
	FROM `TableCurrentRegisterValues` INNER JOIN `TableModbusRegisters` ON `TableCurrentRegisterValues`.`ModbusRegister`=`TableModbusRegisters`.`Register`
	WHERE `TableModbusRegisters`.`TypeId`=0 AND `GetSensorTemperature`(`TableCurrentRegisterValues`.`RegisterValue`) IS NOT NULL AND `TableModbusRegisters`.SensorId>1) 
	ON DUPLICATE KEY UPDATE 
		`TableExternalSensorsValues`.`UpdateTime`=UpdateTime,
		`TableExternalSensorsValues`.Temperature=`GetSensorTemperature`(`TableCurrentRegisterValues`.`RegisterValue`)
;
#обновляем показания нужного состояния нагрева, если температура была обновлена
	UPDATE
	`TableExternalSensorsValues` As e
	SET
	e.Heating=`GetCurrentHeatingState`(e.SensorId, e.Temperature,UpdateTime)
	WHERE e.UpdateTime=UpdateTime
;
#обновляем показания влажности тех датчиков, для которых обновилась температура (т.е. значения актуальные)
	UPDATE
	`TableExternalSensorsValues` As e
	INNER JOIN `TableModbusRegisters`  as tmr on tmr.SensorId=e.SensorId
	INNER JOIN `TableCurrentRegisterValues` as tcr on tcr.`ModbusRegister`=tmr.`Register`
	SET
	e.Humidity=tcr.RegisterValue
	WHERE tmr.TypeId=1 and e.UpdateTime=UpdateTime
;
#обновляем показания статуса батарейки тех датчиков, для которых обновилась температура (т.е. значения актуальные)
	UPDATE
	`TableExternalSensorsValues` As e
	INNER JOIN `TableModbusInputs`  as tmi on tmi.SensorId=e.SensorId
	INNER JOIN `TableCurrentInputValues` as tiv on tiv.`ModbusInput`=tmi.ModbusInput
	SET
	e.Battery=tiv.InputValue
	WHERE e.UpdateTime=UpdateTime
;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
