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

-- Дамп структуры для функция modsensor.GetCurrentHeatingState
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` FUNCTION `GetCurrentHeatingState`(
	`SensorId` INT

,
	`CurrentTemperature` FLOAT






,
	`UpdateTime` DATETIME

) RETURNS bit(1)
    COMMENT 'возвращает требуемое состояние нагрева по показаниям датчика'
BEGIN
	DECLARE HeatingState BOOL;
	DECLARE SetTemp FLOAT;
	#для внутреннего датчика
	IF SensorId=1 THEN
		SET HeatingState=(SELECT `TableInternalSensorValues`.Heating FROM `TableInternalSensorValues` WHERE
			`TableInternalSensorValues`.SensorId=SensorId);
	ELSE
	#для внешнего датчика
		SET HeatingState=(SELECT `TableExternalSensorsValues`.Heating FROM `TableExternalSensorsValues` WHERE
			`TableExternalSensorsValues`.SensorId=SensorId);
	END IF;
	
	SET SetTemp=`GetRegTemperature`(UpdateTime,SensorId);
	IF SetTemp<CurrentTemperature THEN
		SET HeatingState=FALSE;
	ELSE 
		IF SetTemp>CurrentTemperature THEN
			SET HeatingState=TRUE;
		END IF;
	END IF;
	RETURN HeatingState;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
