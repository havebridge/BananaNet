SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

-- -----------------------------------------------------
-- Table `hnet`.`chat_user`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `hnet`.`chat_user` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `username` VARCHAR(255) NOT NULL ,
  `login` VARCHAR(255) NOT NULL ,
  `password` VARCHAR(255) NOT NULL ,
  `ip` VARCHAR(15) NOT NULL ,
  `registered_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL ,
  `last_visited_at` TIMESTAMP NOT NULL) 
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Table `hnet`.`chat_line`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `hnet`.`chat_line` (
  `id` BIGINT AUTO_INCREMENT PRIMARY KEY,
  `sender_id` INT UNSIGNED  NOT NULL ,
  `receiver_id` INT UNSIGNED  NOT NULL ,
  `message_text` TEXT ,
  `send_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ,
  FOREIGN KEY (sender_id) REFERENCES chat_user(id),
  FOREIGN KEY (receiver_id) REFERENCES chat_user(id))
ENGINE = InnoDB;

-- -----------------------------------------------------
-- Table `hnet`.`relationships`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `hnet`.`relationships` (
  `relationship_id` INT AUTO_INCREMENT PRIMARY KEY,
  `user1_id` INT UNSIGNED  NOT NULL,
  `user2_id` INT UNSIGNED  NOT NULL,
  `status` VARCHAR(50),
  FOREIGN KEY (user1_id) REFERENCES chat_user(id),
  FOREIGN KEY (user2_id) REFERENCES chat_user(id))
ENGINE = InnoDB;

SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
