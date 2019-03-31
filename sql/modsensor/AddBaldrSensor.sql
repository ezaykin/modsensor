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

-- Дамп структуры для процедура modsensor.AddBaldrSensor
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.52` PROCEDURE `AddBaldrSensor`(
	IN `ChannelNumber` INT,
	IN `Description` VARCHAR(50)






)
BEGIN
	DECLARE SensorId INT DEFAULT NULL;
	IF ChannelNumber>=1 AND ChannelNumber<=3 THEN
		#сначала определим, есть ли датчик с таким номером канала в системе
		SET SensorId=(SELECT `TableModbusInputs`.SensorId 
		FROM `TableModbusInputs` WHERE `TableModbusInputs`.ModbusInput=(ChannelNumber-1));
		IF SensorId IS NOT NULL THEN
			UPDATE `TableSensors`
			SET `TableSensors`.SensorDescription=Description
			WHERE `TableSensors`.SensorId=SensorId;
		ELSE
		#такого датчика нет в системе - надо завести новый, определив для него регистры Modbus
			INSERT INTO `TableSensors` (`SensorDescription`) VALUES (Description);
			SET SensorId=LAST_INSERT_ID();
			#добавляем регистр статуса батарейки
			INSERT INTO `TableModbusInputs` (`ModbusInput`,`SensorId`) VALUES (ChannelNumber-1,SensorId);
			#добавляем регистры температуры и влажности. TypeId=0 - температура
			INSERT INTO `TableModbusRegisters` (`Register`,`TypeId`,`SensorId`) VALUES (ChannelNumber*2,0,SensorId);
			#TypeId=1 - влажность
			INSERT INTO `TableModbusRegisters` (`Register`,`TypeId`,`SensorId`) VALUES (ChannelNumber*2+1,1,SensorId);
		END IF;
	END IF;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
