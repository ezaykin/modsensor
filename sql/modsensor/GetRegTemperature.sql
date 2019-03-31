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

-- Дамп структуры для функция modsensor.GetRegTemperature
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` FUNCTION `GetRegTemperature`(
	`UpdateTime` DATETIME,
	`SensorId` INT






) RETURNS float
    COMMENT 'возвращает установленную температуру на заданное время с учетом дня недели'
BEGIN
	DECLARE Result FLOAT DEFAULT NULL;
	DECLARE SetTime TIME DEFAULT NULL;
	DECLARE CurWeekday INT;
	DECLARE ProfileId INT;
	DECLARE Autoselect BIT DEFAULT 1;
	SET SetTime=TIME(UpdateTime);
	SET CurWeekday=WEEKDAY(UpdateTime);
	SET ProfileId=`GetCurrentProfile`();
	SET Result=(SELECT `GetProfileTemperature`(SetTime, ProfileId, SensorId));
	#если в таблице нет температуры для текущего времени, берем значение температуры на предыдущий день
	IF(Result IS NULL) THEN
		SET SetTime='23:59:59';
		#если профили будни/выходные переключаются автоматически, учтем это при выборе профиля
		SET Autoselect=(SELECT `TableSettings`.AutoSelect from `TableSettings`);
		IF Autoselect THEN
			#если предыдущий день выходной, берем температуру из профиля "Будни"
			IF((ProfileId=1) AND (CurWeekday=0)) THEN
				SET ProfileId=2;
			ELSE 
				IF ((ProfileId=2) AND (CurWeekday=5)) THEN
					SET ProfileId=1;
				END IF;
			END IF;
		END IF;
		SET Result=(SELECT `GetProfileTemperature`(SetTime, ProfileId, SensorId));
	END IF;
	RETURN Result;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
