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

-- Дамп структуры для процедура modsensor.UpdateModbusInputValue
DELIMITER //
CREATE DEFINER=`root`@`192.168.5.52` PROCEDURE `UpdateModbusInputValue`(
	IN `ModbusInput` INT,
	IN `InputValue` BIT

)
BEGIN
SET sql_notes = 0;
CREATE TABLE IF NOT EXISTS `TableCurrentInputValues` (
	`ModbusInput` INT NOT NULL,
	`InputValue` INT(11) NOT NULL,
	PRIMARY KEY (`ModbusInput`)
)
COLLATE='utf8_general_ci'
ENGINE = MEMORY
;
SET sql_notes = 1;
INSERT INTO `TableCurrentInputValues` (`ModbusInput`,`InputValue`) VALUES(ModbusInput,InputValue) ON DUPLICATE KEY UPDATE 
`InputValue`=InputValue
;
END//
DELIMITER ;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
