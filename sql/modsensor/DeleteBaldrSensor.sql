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

-- Дамп структуры для процедура modsensor.DeleteBaldrSensor
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` PROCEDURE `DeleteBaldrSensor`(
	IN `ChannelNumber` INT



)
BEGIN
	DECLARE SensorId INT DEFAULT NULL;
	IF ChannelNumber>0 AND ChannelNumber<4 THEN
		#сначала определим, есть ли датчик с таким номером канала в системе
		SET SensorId=(SELECT `TableModbusInputs`.SensorId 
		FROM `TableModbusInputs` WHERE `TableModbusInputs`.ModbusInput=(ChannelNumber-1));
		IF SensorId IS NOT NULL THEN
			#Обновим таблицу приводов радиаторов, установив значение "Не задано" для удаляемого датчика
			UPDATE `TableModbusCoils`
			SET `TableModbusCoils`.PrimarySensor=0
			WHERE `TableModbusCoils`.PrimarySensor=SensorId;
			UPDATE `TableModbusCoils`
			SET `TableModbusCoils`.SecondarySensor=0
			WHERE `TableModbusCoils`.SecondarySensor=SensorId;
			DELETE FROM `TableSensors` WHERE `TableSensors`.SensorId=SensorId;
		END IF;
	END IF;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
