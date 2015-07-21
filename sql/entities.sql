SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `entities`;
CREATE TABLE IF NOT EXISTS `entities` (
   `entityID` bigint NOT NULL DEFAULT '0',
   `script` varchar(256) NOT NULL DEFAULT '',
   `type` varchar(256) NOT NULL DEFAULT '',
   `subtype` varchar(256) NOT NULL DEFAULT '',
   `containedBy` bigint NOT NULL DEFAULT '0',
   `ismapped` tinyint NOT NULL DEFAULT '0',
   `coord_x` int NOT NULL DEFAULT '0',
   `coord_y` int NOT NULL DEFAULT '0',
   `coord_z` int NOT NULL DEFAULT '0',
   PRIMARY KEY (`entityID`),
   UNIQUE INDEX `entityID` (`entityID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

