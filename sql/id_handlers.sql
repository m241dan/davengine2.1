SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `id-handlers`;
CREATE TABLE IF NOT EXISTS `id-handlers` (
   `type` smallint NOT NULL DEFAULT '-1',
   `name` varchar(40) NOT NULL DEFAULT 'unknown',
   `top_id` int NOT NULL DEFAULT '-1',
   `can_recycle` smallint NOT NULL DEFAULT '0',
   PRIMARY KEY (`type`),
   UNIQUE INDEX `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


INSERT INTO `id-handlers` VALUES ( '0', 'Account Handler', '1', '0' );
