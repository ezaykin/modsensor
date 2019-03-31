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

-- Дамп структуры для таблица modsensor.TableModbusCoils
CREATE TABLE IF NOT EXISTS `TableModbusCoils` (
  `Coil` int(10) unsigned NOT NULL,
  `PrimarySensor` int(1) unsigned DEFAULT NULL,
  `SecondarySensor` int(1) unsigned DEFAULT NULL,
  `Description` varchar(50) NOT NULL,
  PRIMARY KEY (`Coil`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Дамп данных таблицы modsensor.TableModbusCoils: ~4 rows (приблизительно)
DELETE FROM `TableModbusCoils`;
/*!40000 ALTER TABLE `TableModbusCoils` DISABLE KEYS */;
INSERT INTO `TableModbusCoils` (`Coil`, `PrimarySensor`, `SecondarySensor`, `Description`) VALUES
	(0, 2, 0, 'Привод батареи гостиной'),
	(1, 2, 0, 'Привод батареи кухни'),
	(2, 3, 2, 'Привод батареи спальни'),
	(3, 0, 0, 'Не используется');
/*!40000 ALTER TABLE `TableModbusCoils` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
