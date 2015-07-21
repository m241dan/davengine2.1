SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `vars`;
CREATE TABLE IF NOT EXISTS `vars` (
   `ownertype` smallint NOT NULL DEFAULT '-1',
   `ownerid` bigint NOT NULL DEFAULT '0',
   `name` varchar(60) NOT NULL DEFAULT ' ',
   `indextype` smallint NOT NULL DEFAULT '-1',
   `vindex` varchar(60) NOT NULL DEFAULT ' ',
   `datatype` smallint NOT NULL DEFAULT '-1',
   `data` text NOT NULL DEFAULT '',
   `isscript` tinyint NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `vars` ADD UNIQUE `unique_index` ( `ownerid`, `ownertype`, `name`, `vindex`, `indextype` );
