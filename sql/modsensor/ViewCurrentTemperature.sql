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

-- Дамп структуры для представление modsensor.ViewCurrentTemperature
-- Удаление временной таблицы и создание окончательной структуры представления
DROP TABLE IF EXISTS `ViewCurrentTemperature`;
CREATE ALGORITHM=UNDEFINED DEFINER=`root`@`192.168.5.55` SQL SECURITY DEFINER VIEW `ViewCurrentTemperature` AS (select `TableInternalSensorValues`.`UpdateTime` AS `UpdateTime`,`TableInternalSensorValues`.`Temperature` AS `Temperature`,`TableInternalSensorValues`.`SensorId` AS `SensorId`,`TableInternalSensorValues`.`Heating` AS `Heating` from `TableInternalSensorValues`) union (select `TableExternalSensorsValues`.`UpdateTime` AS `UpdateTime`,`TableExternalSensorsValues`.`Temperature` AS `Temperature`,`TableExternalSensorsValues`.`SensorId` AS `SensorId`,`TableExternalSensorsValues`.`Heating` AS `Heating` from `TableExternalSensorsValues`);

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
