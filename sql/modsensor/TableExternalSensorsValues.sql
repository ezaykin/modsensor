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

-- Дамп структуры для таблица modsensor.TableExternalSensorsValues
CREATE TABLE IF NOT EXISTS `TableExternalSensorsValues` (
  `SensorId` int(11) NOT NULL,
  `UpdateTime` datetime NOT NULL,
  `Temperature` float NOT NULL,
  `Humidity` int(11) DEFAULT NULL,
  `Battery` tinyint(1) DEFAULT NULL,
  `Heating` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`SensorId`)
) ENGINE=MEMORY DEFAULT CHARSET=utf8;

-- Дамп данных таблицы modsensor.TableExternalSensorsValues: 2 rows
DELETE FROM `TableExternalSensorsValues`;
/*!40000 ALTER TABLE `TableExternalSensorsValues` DISABLE KEYS */;
/*!40000 ALTER TABLE `TableExternalSensorsValues` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
