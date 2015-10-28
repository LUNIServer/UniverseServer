-- phpMyAdmin SQL Dump
-- version 4.0.10deb1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Erstellungszeit: 14. Okt 2015 um 09:46
-- Server Version: 5.5.44-0ubuntu0.14.04.1
-- PHP-Version: 5.5.9-1ubuntu4.13

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Datenbank: `luni`
--
CREATE DATABASE IF NOT EXISTS `luni` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `luni`;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `accounts`
--

CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(64) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '127.0.0.1',
  `rank` smallint(1) NOT NULL DEFAULT '0',
  `numChars` tinyint(4) NOT NULL,
  `frontChar` bigint(20) NOT NULL,
  `lastLog` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `activeSub` smallint(1) NOT NULL DEFAULT '0',
  `subTime` int(32) NOT NULL,
  `legoClub` smallint(1) NOT NULL,
  `locked` tinyint(4) NOT NULL,
  `banned` tinyint(4) NOT NULL,
  `loginTries` int(1) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=327 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `auth_tickets`
--

CREATE TABLE IF NOT EXISTS `auth_tickets` (
  `id` int(20) NOT NULL AUTO_INCREMENT,
  `status` enum('REQUEST','ACCEPTED','DECLINED') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'REQUEST',
  `sessionid` int(20) NOT NULL,
  `message` text COLLATE utf8_unicode_ci,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `availability`
--

CREATE TABLE IF NOT EXISTS `availability` (
  `server_address` varchar(22) NOT NULL,
  `available` tinyint(1) NOT NULL,
  `message` varchar(255) CHARACTER SET utf8 NOT NULL DEFAULT 'Universe not available!',
  UNIQUE KEY `available` (`available`),
  UNIQUE KEY `server_address` (`server_address`),
  UNIQUE KEY `available_2` (`available`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `characters`
--

CREATE TABLE IF NOT EXISTS `characters` (
  `id` bigint(20) unsigned NOT NULL DEFAULT '0',
  `accountID` int(10) unsigned NOT NULL,
  `objectID` bigint(20) NOT NULL AUTO_INCREMENT,
  `name` varchar(25) COLLATE utf8_unicode_ci NOT NULL,
  `unapprovedName` varchar(66) COLLATE utf8_unicode_ci NOT NULL,
  `nameRejected` tinyint(4) NOT NULL DEFAULT '0',
  `freeToPlay` tinyint(4) NOT NULL DEFAULT '0',
  `gmlevel` mediumint(9) NOT NULL DEFAULT '0',
  `shirtColor` int(11) NOT NULL DEFAULT '0',
  `shirtStyle` int(11) NOT NULL DEFAULT '0',
  `pantsColor` int(11) NOT NULL DEFAULT '0',
  `hairStyle` int(11) NOT NULL DEFAULT '0',
  `hairColor` int(11) NOT NULL DEFAULT '0',
  `lh` int(11) NOT NULL DEFAULT '0',
  `rh` int(11) NOT NULL DEFAULT '0',
  `eyebrows` int(11) NOT NULL DEFAULT '0',
  `eyes` int(11) NOT NULL DEFAULT '0',
  `mouth` int(11) NOT NULL DEFAULT '0',
  `headID` int(11) NOT NULL DEFAULT '0',
  `neckID` int(11) NOT NULL DEFAULT '0',
  `shirtID` int(11) NOT NULL DEFAULT '0',
  `pantsID` int(11) NOT NULL DEFAULT '0',
  `leftHandID` int(11) NOT NULL DEFAULT '0',
  `rightHandID` int(11) NOT NULL DEFAULT '0',
  `faction` tinyint(1) NOT NULL DEFAULT '0',
  `worldID` int(11) NOT NULL DEFAULT '0',
  `lastZoneId` int(11) NOT NULL,
  `mapInstance` int(11) NOT NULL,
  `mapClone` int(11) NOT NULL,
  `x` double NOT NULL DEFAULT '0',
  `y` double NOT NULL DEFAULT '0',
  `z` double NOT NULL DEFAULT '0',
  `friends` varchar(100) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `bestFriends` varchar(50) COLLATE utf8_unicode_ci NOT NULL DEFAULT '',
  `level` int(3) NOT NULL DEFAULT '1',
  `uScore` int(32) NOT NULL DEFAULT '0',
  PRIMARY KEY (`objectID`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1152921504606847274 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `equipment`
--

CREATE TABLE IF NOT EXISTS `equipment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `owner` bigint(20) NOT NULL COMMENT 'objid of equiping player',
  `object` bigint(20) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=6138 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `friends`
--

CREATE TABLE IF NOT EXISTS `friends` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `charida` bigint(20) NOT NULL,
  `charidb` bigint(20) NOT NULL,
  `status` enum('REQUEST','ACCEPTED','DECLINED','FRIENDS','BEST_FRIEND_REQUEST','BEST_FRIENDS') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'REQUEST',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=88 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `instances`
--

CREATE TABLE IF NOT EXISTS `instances` (
  `instanceid` int(11) NOT NULL AUTO_INCREMENT,
  `server_address` varchar(22) NOT NULL,
  PRIMARY KEY (`instanceid`),
  FULLTEXT KEY `server_address` (`server_address`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=7 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `inventory`
--

CREATE TABLE IF NOT EXISTS `inventory` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `owner` bigint(64) NOT NULL,
  `object` bigint(64) NOT NULL,
  `qnt` smallint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'quantity',
  `slot` smallint(3) unsigned NOT NULL COMMENT 'bag slot',
  `linked` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=2190 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `mails`
--

CREATE TABLE IF NOT EXISTS `mails` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `subject` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `text` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `sender` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `recipient_id` bigint(20) NOT NULL,
  `attachment` bigint(20) NOT NULL DEFAULT '0',
  `attachment_count` int(11) NOT NULL DEFAULT '0',
  `sent_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `is_read` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=519 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `missions`
--

CREATE TABLE IF NOT EXISTS `missions` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `character` bigint(20) NOT NULL COMMENT 'Character ObjectID',
  `missionid` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `count` smallint(6) NOT NULL DEFAULT '1',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=205 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `objects`
--

CREATE TABLE IF NOT EXISTS `objects` (
  `objectid` bigint(64) NOT NULL AUTO_INCREMENT,
  `template` int(32) unsigned NOT NULL COMMENT 'LOT',
  `spawnid` bigint(20) DEFAULT NULL COMMENT 'objectID of in-world object',
  `nose_cone_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)',
  `cockpit_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)',
  `engine_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)',
  PRIMARY KEY (`objectid`),
  KEY `objectid` (`objectid`),
  KEY `objectid_2` (`objectid`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1152921510794157320 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `sessions`
--

CREATE TABLE IF NOT EXISTS `sessions` (
  `sessionid` int(20) NOT NULL AUTO_INCREMENT,
  `ipaddress` varchar(21) NOT NULL DEFAULT '',
  `phase` smallint(6) NOT NULL DEFAULT '1' COMMENT '1 = CONNECTED, 2 = AUTHENTIFIED, 3 = PLAYING, 4 = INWORLD',
  `sessionkey` varchar(33) NOT NULL DEFAULT '',
  `instanceid` int(11) DEFAULT NULL,
  `accountid` int(10) unsigned NOT NULL DEFAULT '0',
  `login_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `charid` bigint(20) DEFAULT NULL,
  `zoneid` int(11) NOT NULL DEFAULT '0',
  `cloneid` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`sessionid`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1 AUTO_INCREMENT=817 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `topics`
--

CREATE TABLE IF NOT EXISTS `topics` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(128) COLLATE utf8_unicode_ci NOT NULL,
  `min_rank` smallint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=54 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `worlds`
--

CREATE TABLE IF NOT EXISTS `worlds` (
  `id` int(50) unsigned NOT NULL AUTO_INCREMENT,
  `zone` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'the zone, 0 avant gardens etc',
  `owner` int(5) NOT NULL,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `desc` text COLLATE utf8_unicode_ci NOT NULL COMMENT 'description',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '0 need to be cleaned, 1: private, 2: friends, 3: public',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `world_objects`
--

CREATE TABLE IF NOT EXISTS `world_objects` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `objectID` bigint(64) NOT NULL,
  `world` int(10) unsigned NOT NULL,
  `posX` double NOT NULL,
  `posY` double NOT NULL,
  `posZ` double NOT NULL,
  `rotX` double NOT NULL,
  `rotY` double NOT NULL,
  `rotZ` double NOT NULL,
  `rotW` double NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci AUTO_INCREMENT=161 ;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
