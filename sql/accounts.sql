SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `accounts`;
CREATE TABLE IF NOT EXISTS `accounts` (
   `accountID` bigint NOT NULL DEFAULT '0',
   `name` varchar(25) NOT NULL DEFAULT '',
   `password` varchar(30) NOT NULL DEFAULT '',
   `level` smallint NOT NULL DEFAULT '0',
   PRIMARY KEY (`accountID`),
   UNIQUE INDEX `name` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

INSERT INTO `accounts` VALUES ( '1', 'Davenge', 'DaPxnNCnzgTfs', '5' );
INSERT INTO `accounts` VALUES ( '2', 'Pain',    'PavIfmXaIsKfQ', '5' );

