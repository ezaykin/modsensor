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

-- Дамп структуры для таблица modsensor.TablePressureLog
CREATE TABLE IF NOT EXISTS `TablePressureLog` (
  `TimeId` int(10) unsigned NOT NULL,
  `Pressure` float NOT NULL,
  PRIMARY KEY (`TimeId`),
  CONSTRAINT `FK_TablePressureLog_TableTimeLog` FOREIGN KEY (`TimeId`) REFERENCES `TableTimeLog` (`TimeId`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Дамп данных таблицы modsensor.TablePressureLog: ~0 rows (приблизительно)
DELETE FROM `TablePressureLog`;
/*!40000 ALTER TABLE `TablePressureLog` DISABLE KEYS */;
/*!40000 ALTER TABLE `TablePressureLog` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
