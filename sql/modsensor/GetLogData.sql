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

-- Дамп структуры для процедура modsensor.GetLogData
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.52` PROCEDURE `GetLogData`(
	IN `SensorDesc` VARCHAR(50),
	IN `Period` INT
)
    COMMENT 'Возвращает данные для отображения журнала показаний'
BEGIN
	DECLARE SensorId INT;
	DECLARE StartTime DATETIME;
	DECLARE AvgInterval INT;

	#значения давления, влажности и обогрева на момент начала выборки
	SET SensorId=(SELECT `TableSensors`.SensorId FROM `TableSensors` WHERE `TableSensors`.SensorDescription=SensorDesc);
	#получаем время начала выборки
	SET StartTime=NOW()-interval Period day;
	#интервал усреднения, если период больше суток
	SET AvgInterval=Period;
	SET @LastHeating=NULL;
	SET @LastPressure=NULL;
	SET @LastHumidity=NULL;
	
	CREATE TEMPORARY TABLE IF NOT EXISTS `TableLogData` (
		`TimeId` INT DEFAULT NULL,
		`Temperature` FLOAT NOT NULL,
		`Humidity` FLOAT,
		`Pressure` FLOAT,
		`Heating` FLOAT
	)
	COLLATE='utf8_general_ci';
	TRUNCATE `TableLogData`;
	
	#заполним первую строку таблицы результата
	INSERT INTO `TableLogData` (`TimeId`, `Temperature`) 
	(SELECT `TableTimeLog`.TimeId, `TableTemperatureLog`.Temperature FROM `TableTimeLog` 
		INNER JOIN `TableTemperatureLog` ON `TableTimeLog`.TimeId=`TableTemperatureLog`.TimeId 
		WHERE `TableTemperatureLog`.SensorId=SensorId AND `TableTimeLog`.EventTime>=StartTime
		ORDER BY `TableTimeLog`.EventTime LIMIT 1);

	#обновляем фактическое время начала выборки
	SET StartTime=(SELECT `TableTimeLog`.EventTime FROM `TableTimeLog` WHERE `TableTimeLog`.TimeId=(SELECT `TableLogData`.TimeId FROM `TableLogData`));
	#если датчик используется для управления отоплением, то выведем шкалу включения обогрева
	IF (select COUNT(*) FROM `TableModbusCoils` 
		WHERE `TableModbusCoils`.PrimarySensor=SensorId 
		OR `TableModbusCoils`.SecondarySensor=SensorId)>0 THEN
			SET @LastHeating=IF(`GetLastHeatingState`(SensorId,StartTime),1.0,0);
			#помещаем в первую строку состояние обогрева
			UPDATE `TableLogData`
			SET `TableLogData`.Heating=@LastHeating;
	END IF;
	UPDATE `TableLogData`
	SET `TableLogData`.TimeId=ROUND(`TableLogData`.TimeId/AvgInterval);
	
	IF SensorId=1 THEN
		#помещаем в первую строку атмосферное давление на начало выборки
		UPDATE `TableLogData`
		SET `TableLogData`.Pressure=`GetLastPressure`(StartTime);
		#заполняем остальные данные таблицы результата для внутреннего датчика
		INSERT INTO `TableLogData` (`TimeId`, `Temperature`, `Pressure`, `Heating`) 
			(SELECT `TableTimeLog`.TimeId/AvgInterval, `TableTemperatureLog`.Temperature,
			@LastPressure:=COALESCE(`TablePressureLog`.Pressure,@LastPressure) AS Pressure,
			@LastHeating:=COALESCE(`TableHeatingLog`.State,@LastHeating) AS `Heating` FROM `TableTimeLog`
			INNER JOIN `TableTemperatureLog` ON `TableTimeLog`.TimeId=`TableTemperatureLog`.TimeId
			LEFT JOIN `TablePressureLog` ON `TableTimeLog`.TimeId=`TablePressureLog`.TimeId
			LEFT JOIN `TableHeatingLog` ON `TableTimeLog`.TimeId=`TableHeatingLog`.TimeId AND `TableTemperatureLog`.SensorId=`TableHeatingLog`.SensorId
		WHERE `TableTemperatureLog`.SensorId=1 AND `TableTimeLog`.EventTime>StartTime
		ORDER BY `TableTimeLog`.EventTime);
	ELSE
		SET @LastHumidity=`GetLastHumidity`(SensorId,StartTime);
		#помещаем в первую строку влажность на начало выборки
		UPDATE `TableLogData`
		SET `TableLogData`.Humidity=@LastHumidity,
		`TableLogData`.Heating=@LastHeating;
		#заполняем остальные данные таблицы результата для внешних датчиков
		INSERT INTO `TableLogData` (`TimeId`, `Temperature`, `Humidity`, `Heating`) 
			(SELECT `TableTimeLog`.TimeId/AvgInterval, `TableTemperatureLog`.Temperature, 
			@LastHumidity:=COALESCE(`TableHumidityLog`.Humidity, @LastHumidity) AS Humidity,
			@LastHeating:=COALESCE(`TableHeatingLog`.State,@LastHeating) AS `Heating` FROM `TableTimeLog`
			INNER JOIN `TableTemperatureLog` ON `TableTimeLog`.TimeId=`TableTemperatureLog`.TimeId
			LEFT JOIN `TableHumidityLog` ON `TableTimeLog`.TimeId=`TableHumidityLog`.TimeId AND `TableTemperatureLog`.SensorId=`TableHumidityLog`.SensorId
			LEFT JOIN `TableHeatingLog` ON `TableTimeLog`.TimeId=`TableHeatingLog`.TimeId AND `TableTemperatureLog`.SensorId=`TableHeatingLog`.SensorId
		WHERE `TableTemperatureLog`.SensorId=SensorId AND `TableTimeLog`.EventTime>StartTime
		ORDER BY `TableTimeLog`.EventTime);
	END IF;

#возврат результата
#если нужны данные за сутки, усреднение не производим
	IF Period=1 THEN
		SELECT `TableTimeLog`.EventTime, `TableTimeLog`.TimeId, 
		`TableLogData`.Temperature,
		`TableLogData`.Pressure,
		`TableLogData`.Humidity,
		`TableLogData`.Heating
		 FROM `TableLogData` INNER JOIN `TableTimeLog` ON
		`TableLogData`.TimeId =`TableTimeLog`.TimeId;
	ELSE
		SELECT `TableTimeLog`.EventTime, `TableTimeLog`.TimeId, 
		ROUND(AVG(`TableLogData`.Temperature),1) AS Temperature,
		ROUND(AVG(`TableLogData`.Pressure),1) AS Pressure,
		ROUND(AVG(`TableLogData`.Humidity),1) AS Humidity,
		ROUND(AVG(`TableLogData`.Heating),1) AS Heating
		 FROM `TableLogData` INNER JOIN `TableTimeLog` ON
		`TableLogData`.TimeId*AvgInterval =`TableTimeLog`.TimeId
		GROUP BY (`TableLogData`.TimeId);
	END IF;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
