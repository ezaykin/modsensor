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

-- Дамп структуры для процедура modsensor.UpdateCoilsState
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` PROCEDURE `UpdateCoilsState`(
	IN `UpdateTime` DATETIME





























)
BEGIN
#выключаем предупреждение о том, что таблица уже существует
	SET sql_notes = 0;
	CREATE TABLE IF NOT EXISTS `TableCurrentCoilsValues` (
		`ModbusCoil` INT NOT NULL,
		`UpdateTime` DATETIME NULL,
		`Value` BOOL DEFAULT false,
		PRIMARY KEY (`ModbusCoil`)
	)
	COLLATE='utf8_general_ci'
	ENGINE = MEMORY
	;
	SET sql_notes = 1;

#установим состояние приводов радиаторов в соответствии с установленным значением
	INSERT INTO `TableCurrentCoilsValues` (`ModbusCoil`,`UpdateTime`) 
	(SELECT `TableModbusCoils`.Coil, UpdateTime  FROM `TableModbusCoils`
	INNER JOIN `ViewCurrentTemperature` ON `TableModbusCoils`.PrimarySensor=`ViewCurrentTemperature`.SensorId
	WHERE `ViewCurrentTemperature`.UpdateTime=UpdateTime) 
	ON DUPLICATE KEY UPDATE 
		`TableCurrentCoilsValues`.UpdateTime=UpdateTime,
		`TableCurrentCoilsValues`.Value=`ViewCurrentTemperature`.Heating
;
#если нет актуальных показаний предпочтительного датчика (села батарейка), установим по дублирующему
#сначала дополним таблицу выходами, для которых нет актуальных показаний, пометив их нулевым временем
	INSERT INTO `TableCurrentCoilsValues` (`ModbusCoil`,`UpdateTime`) 
	(SELECT `TableModbusCoils`.Coil, NULL  FROM `TableModbusCoils`
	INNER JOIN `ViewCurrentTemperature` ON `TableModbusCoils`.PrimarySensor=`ViewCurrentTemperature`.SensorId
	WHERE `ViewCurrentTemperature`.UpdateTime<>UpdateTime) 
	ON DUPLICATE KEY UPDATE 
		`TableCurrentCoilsValues`.UpdateTime=NULL,
		`TableCurrentCoilsValues`.Value=FALSE
;
#теперь установим значение выходов по дублирующему датчику
#если нет актуальных данных и от второго датчика, выход останется выключенным
	UPDATE `TableCurrentCoilsValues` AS tcv
	INNER JOIN `TableModbusCoils` AS tmc ON tcv.ModbusCoil=tmc.Coil
	INNER JOIN `ViewCurrentTemperature` AS vct ON tmc.SecondarySensor=vct.SensorId
	SET
	tcv.UpdateTime=UpdateTime,
	tcv.Value=vct.Heating
	WHERE tcv.UpdateTime IS NULL AND vct.UpdateTime=UpdateTime
;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
