/*
Navicat MySQL Data Transfer

Source Server         : localhost_3306
Source Server Version : 50537
Source Host           : localhost:3306
Source Database       : dolorsUsr

Target Server Type    : MYSQL
Target Server Version : 50537
File Encoding         : 65001

Date: 2014-09-19 15:57:43
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for usr
-- ----------------------------
DROP TABLE IF EXISTS `usr`;
CREATE TABLE `usr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `pwd` varchar(50) NOT NULL,
  `status` varchar(10) NOT NULL DEFAULT 'offline',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=15 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of usr
-- ----------------------------
INSERT INTO `usr` VALUES ('1', 'aaa', 'aaa', 'offline');
INSERT INTO `usr` VALUES ('2', 'ddd', 'fff', 'offline');
INSERT INTO `usr` VALUES ('10', '的', '941126', 'offline');
INSERT INTO `usr` VALUES ('11', '小明', '941126', 'offline');
INSERT INTO `usr` VALUES ('14', '李华', '941126', 'online');
