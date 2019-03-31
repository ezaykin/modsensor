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

-- Дамп структуры для функция modsensor.GetLastHeatingState
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.55` FUNCTION `GetLastHeatingState`(
	`SensorId` INT


,
	`EventTime` DATETIME

) RETURNS bit(1)
    COMMENT 'последнее сохраненное в базе значение на заданное время'
BEGIN
	DECLARE HeatingState BOOL DEFAULT FALSE;
	SET HeatingState=
	(SELECT `TableHeatingLog`.State FROM `TableHeatingLog`
		INNER JOIN `TableTimeLog` ON `TableHeatingLog`.TimeId=`TableTimeLog`.TimeId
		WHERE `TableHeatingLog`.SensorId=SensorId AND `TableTimeLog`.EventTime<=EventTime ORDER BY `TableTimeLog`.EventTime DESC LIMIT 1);
	IF HeatingState IS NULL THEN
		SET HeatingState=FALSE;
	END IF;
	RETURN HeatingState;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
