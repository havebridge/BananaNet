SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';


-- -----------------------------------------------------
-- Table `hnet`.`chat`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `hnet`.`chat` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `hnet`.`chat_user`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `hnet`.`chat_user` (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `username` VARCHAR(20) NOT NULL ,
  `login` VARCHAR(20) NOT NULL ,
  `password` VARCHAR(20) NOT NULL ,
  `ip` VARCHAR(15) NOT NULL ,
  `registered_at` TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL ,
  `last_visited_at` TIMESTAMP NOT NULL ,
  PRIMARY KEY (`id`) )
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `hnet`.`chat_line`
-- -----------------------------------------------------
CREATE  TABLE IF NOT EXISTS `hnet`.`chat_line` (
  `id` BIGINT UNSIGNED NOT NULL AUTO_INCREMENT ,
  `chat_id` INT UNSIGNED NOT NULL ,
  `user_id` INT UNSIGNED NOT NULL ,
  `text` TEXT NOT NULL ,
  `created_at` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ,
  PRIMARY KEY (`id`) ,
  INDEX `fk_chat_line_chat` (`chat_id` ASC) ,
  INDEX `fk_chat_line_chat_user1` (`user_id` ASC) ,
  CONSTRAINT `fk_chat_line_chat`
    FOREIGN KEY (`chat_id` )
    REFERENCES `hnet`.`chat` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_chat_line_chat_user1`
    FOREIGN KEY (`user_id` )
    REFERENCES `hnet`.`chat_user` (`id` )
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;



SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
