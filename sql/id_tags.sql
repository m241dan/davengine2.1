SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `id_tags`;
CREATE TABLE IF NOT EXISTS `id_tags` (
   `type` smallint NOT NULL DEFAULT '-1',
   `id` bigint NOT NULL DEFAULT '0',
   `can_recycle` smallint NOT NULL DEFAULT '0',
   `created_by` varchar(256) NOT NULL DEFAULT 'system',
   `created_on` varchar(256) NOT NULL DEFAULT ' ',
   `modified_by` varchar(256) NOT NULL DEFAULT 'system',
   `modified_on` varchar(256) NOT NULL DEFAULT ' '
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `id_tags` ADD UNIQUE `unique_index` ( `type`, `id` );

INSERT INTO `id_tags` VALUES ( '0', '1', '0', 'init_script', ' ', 'init_script', ' ' );
INSERT INTO `id_tags` VALUES ( '0', '2', '0', 'init_script', ' ', 'init_script', ' ' );
