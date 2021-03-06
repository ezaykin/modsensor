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

-- Дамп структуры для таблица modsensor.TableSettings
CREATE TABLE IF NOT EXISTS `TableSettings` (
  `Device` varchar(50) DEFAULT NULL,
  `Baud` int(10) unsigned DEFAULT NULL,
  `AutoSelect` bit(1) DEFAULT NULL,
  `ManualProfile` int(1) unsigned DEFAULT NULL,
  `LogData` bit(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Дамп данных таблицы modsensor.TableSettings: ~0 rows (приблизительно)
DELETE FROM `TableSettings`;
/*!40000 ALTER TABLE `TableSettings` DISABLE KEYS */;
INSERT INTO `TableSettings` (`Device`, `Baud`, `AutoSelect`, `ManualProfile`, `LogData`) VALUES
	('/dev/ttyUSB0', 38400, b'1', 1, b'1');
/*!40000 ALTER TABLE `TableSettings` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
