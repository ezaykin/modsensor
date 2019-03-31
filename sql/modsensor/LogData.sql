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

-- Дамп структуры для процедура modsensor.LogData
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` PROCEDURE `LogData`(
	IN `UpdateTime` DATETIME






















)
BEGIN
	DECLARE TimeId INT;
	#определяем, что показания обновились
	IF (SELECT COUNT(*) FROM `TableInternalSensorValues` WHERE `TableInternalSensorValues`.UpdateTime=UpdateTime)>0 OR
		(SELECT COUNT(*) FROM `TableExternalSensorsValues` WHERE `TableExternalSensorsValues`.UpdateTime=UpdateTime)>0 THEN
		INSERT INTO `TableTimeLog` (`EventTime`) VALUES (UpdateTime); 
		SET TimeId=LAST_INSERT_ID();
		#лог температуры по всем датчикам
		INSERT INTO `TableTemperatureLog` (`TimeId`,`Temperature`,`SensorId`) 
		(SELECT TimeId, `ViewCurrentTemperature`.Temperature,`ViewCurrentTemperature`.SensorId
		FROM `ViewCurrentTemperature` WHERE `ViewCurrentTemperature`.UpdateTime=UpdateTime);
		#лог влажности. Пишем только если значение изменилось
		INSERT INTO `TableHumidityLog` (`TimeId`,`Humidity`,`SensorId`) 
		(SELECT TimeId,`TableExternalSensorsValues`.Humidity,`TableExternalSensorsValues`.SensorId FROM `TableExternalSensorsValues` 
			WHERE `TableExternalSensorsValues`.UpdateTime=UpdateTime
			AND `TableExternalSensorsValues`.Humidity<>`GetLastHumidity`(`TableExternalSensorsValues`.SensorId,UpdateTime));
		#лог состояния батареек. Пишем только если значение изменилось
		INSERT INTO `TableBatteryStatusLog` (`TimeId`,`BatteryStatus`,`SensorId`) 
		(SELECT TimeId,`TableExternalSensorsValues`.Battery,`TableExternalSensorsValues`.SensorId FROM `TableExternalSensorsValues` 
			WHERE `TableExternalSensorsValues`.UpdateTime=UpdateTime 
			AND `TableExternalSensorsValues`.Battery<>`GetLastBatteryStatus`(`TableExternalSensorsValues`.SensorId,UpdateTime));
		#лог давления. Пишем только если значение изменилось
		INSERT INTO `TablePressureLog` (`TimeId`,`Pressure`) 
		(SELECT TimeId,`TableInternalSensorValues`.Pressure FROM `TableInternalSensorValues` 
			WHERE `TableInternalSensorValues`.UpdateTime=UpdateTime AND `TableInternalSensorValues`.Pressure<>`GetLastPressure`(UpdateTime));
		#лог состояния обогрева. Пишем только если значение изменилось
		INSERT INTO `TableHeatingLog` (`TimeId`,`State`,`SensorId`) 
		(SELECT TimeId, `ViewCurrentTemperature`.Heating,`ViewCurrentTemperature`.SensorId FROM `ViewCurrentTemperature` 
			WHERE `ViewCurrentTemperature`.UpdateTime=UpdateTime AND `ViewCurrentTemperature`.Heating IS NOT NULL
			AND `ViewCurrentTemperature`.Heating<>`GetLastHeatingState`(`ViewCurrentTemperature`.SensorId, UpdateTime));
	END IF;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
