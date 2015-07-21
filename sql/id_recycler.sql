SET FOREIGN_KEY_CHECKS=0;

DROP TABLE IF EXISTS `id_recycled`;
CREATE TABLE IF NOT EXISTS `id_recycled` (
   `type` smallint NOT NULL DEFAULT '-1',
   `rec_id` bigint NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

