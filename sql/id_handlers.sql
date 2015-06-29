SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `id_handlers`;
CREATE TABLE IF NOT EXISTS `id_handlers` (
   `type` smallint NOT NULL DEFAULT '-1',
   `name` varchar(40) NOT NULL DEFAULT 'unknown',
   `top_id` int NOT NULL DEFAULT '-1',
   `can_recycle` smallint NOT NULL DEFAULT '0',
   PRIMARY KEY (`type`),
   UNIQUE INDEX `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


INSERT INTO `id_handlers` VALUES ( '0', 'Account Handler', '3', '0' );
INSERT INTO `id_handlers` VALUES ( '1', 'Nanny Handler', '1', '1' );
INSERT INTO `id_handlers` VALUES ( '2', 'Global Vars Handler', '1', '1' );
