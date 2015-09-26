-- phpMyAdmin SQL Dump
-- version 4.3.11
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Erstellungszeit: 15. Aug 2015 um 00:37
-- Server-Version: 5.6.24
-- PHP-Version: 5.6.8

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Datenbank: `luni`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `accounts`
--

CREATE TABLE IF NOT EXISTS `accounts` (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(64) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `ip` varchar(32) COLLATE utf8_unicode_ci NOT NULL DEFAULT '127.0.0.1',
  `rank` smallint(1) NOT NULL DEFAULT '0',
  `numChars` tinyint(4) NOT NULL,
  `frontChar` tinyint(4) NOT NULL,
  `lastLog` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `activeSub` smallint(1) NOT NULL DEFAULT '0',
  `subTime` int(32) NOT NULL,
  `legoClub` smallint(1) NOT NULL,
  `locked` tinyint(4) NOT NULL,
  `banned` tinyint(4) NOT NULL,
  `loginTries` int(1) NOT NULL
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `characters`
--

CREATE TABLE IF NOT EXISTS `characters` (
  `id` bigint(20) unsigned NOT NULL,
  `accountID` int(10) unsigned NOT NULL,
  `objectID` bigint(20) NOT NULL,
  `name` varchar(25) COLLATE utf8_unicode_ci NOT NULL,
  `unapprovedName` varchar(66) COLLATE utf8_unicode_ci NOT NULL,
  `nameRejected` tinyint(4) NOT NULL DEFAULT '0',
  `freeToPlay` tinyint(4) NOT NULL DEFAULT '0',
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
  `uScore` int(32) NOT NULL DEFAULT '0'
) ENGINE=MyISAM AUTO_INCREMENT=1152921504606846994 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `equipment`
--

CREATE TABLE IF NOT EXISTS `equipment` (
  `id` int(11) NOT NULL,
  `owner` bigint(20) NOT NULL COMMENT 'objid of equiping player',
  `object` bigint(20) NOT NULL
) ENGINE=MyISAM AUTO_INCREMENT=171 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `friends`
--

CREATE TABLE IF NOT EXISTS `friends` (
  `id` int(11) NOT NULL,
  `charida` bigint(20) NOT NULL,
  `charidb` bigint(20) NOT NULL,
  `status` enum('REQUEST','ACCEPTED','DECLINED','FRIENDS','BEST_FRIEND_REQUEST','BEST_FRIENDS') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'REQUEST'
) ENGINE=MyISAM AUTO_INCREMENT=6 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `inventory`
--

CREATE TABLE IF NOT EXISTS `inventory` (
  `id` int(11) NOT NULL,
  `owner` bigint(64) NOT NULL,
  `object` bigint(64) NOT NULL,
  `qnt` smallint(1) unsigned NOT NULL DEFAULT '0' COMMENT 'quantity',
  `slot` smallint(3) unsigned NOT NULL COMMENT 'bag slot',
  `linked` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=MyISAM AUTO_INCREMENT=46 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `mails`
--

CREATE TABLE IF NOT EXISTS `mails` (
  `id` bigint(20) NOT NULL,
  `subject` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `text` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `sender` text CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `recipient_id` bigint(20) NOT NULL,
  `attachment` bigint(20) NOT NULL DEFAULT '0',
  `attachment_count` int(11) NOT NULL DEFAULT '0',
  `sent_time` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `is_read` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=MyISAM AUTO_INCREMENT=7 DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `missions`
--

CREATE TABLE IF NOT EXISTS `missions` (
  `id` bigint(20) NOT NULL,
  `character` bigint(20) NOT NULL COMMENT 'Character ObjectID',
  `missionid` int(11) NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `count` smallint(6) NOT NULL DEFAULT '1'
) ENGINE=MyISAM AUTO_INCREMENT=11 DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `npcs`
--

CREATE TABLE IF NOT EXISTS `npcs` (
  `id` int(10) unsigned NOT NULL,
  `name` varchar(30) COLLATE utf8_unicode_ci NOT NULL,
  `world` int(10) unsigned NOT NULL,
  `x` double NOT NULL,
  `y` double NOT NULL,
  `z` double NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `objects`
--

CREATE TABLE IF NOT EXISTS `objects` (
  `objectid` bigint(64) NOT NULL,
  `template` int(32) unsigned NOT NULL COMMENT 'LOT',
  `spawnid` bigint(20) DEFAULT NULL COMMENT 'objectID of in-world object',
  `nose_cone_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)',
  `cockpit_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)',
  `engine_template` int(11) DEFAULT NULL COMMENT '(for LOT 6416)'
) ENGINE=MyISAM AUTO_INCREMENT=1152921510794154825 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `sessions`
--

CREATE TABLE IF NOT EXISTS `sessions` (
  `sessionid` int(20) NOT NULL,
  `ipaddress` varchar(21) NOT NULL,
  `phase` smallint(6) NOT NULL DEFAULT '1' COMMENT '1 = CONNECTED, 2 = AUTHENTIFIED, 3 = PLAYING, 4 = INWORLD',
  `sessionkey` varchar(33) NOT NULL,
  `accountid` int(10) unsigned NOT NULL,
  `login_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `charid` bigint(20) DEFAULT NULL,
  `zoneid` int(11) NOT NULL DEFAULT '0'
) ENGINE=MyISAM AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `worlds`
--

CREATE TABLE IF NOT EXISTS `worlds` (
  `id` int(50) unsigned NOT NULL,
  `zone` tinyint(1) NOT NULL DEFAULT '0' COMMENT 'the zone, 0 avant gardens etc',
  `owner` int(5) NOT NULL,
  `name` varchar(35) COLLATE utf8_unicode_ci NOT NULL,
  `desc` text COLLATE utf8_unicode_ci NOT NULL COMMENT 'description',
  `status` tinyint(1) NOT NULL DEFAULT '0' COMMENT '0 need to be cleaned, 1: private, 2: friends, 3: public'
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `accounts`
--
ALTER TABLE `accounts`
  ADD PRIMARY KEY (`id`), ADD UNIQUE KEY `name` (`name`);

--
-- Indizes für die Tabelle `characters`
--
ALTER TABLE `characters`
  ADD PRIMARY KEY (`objectID`);

--
-- Indizes für die Tabelle `equipment`
--
ALTER TABLE `equipment`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `friends`
--
ALTER TABLE `friends`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `inventory`
--
ALTER TABLE `inventory`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `mails`
--
ALTER TABLE `mails`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `missions`
--
ALTER TABLE `missions`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `npcs`
--
ALTER TABLE `npcs`
  ADD PRIMARY KEY (`id`);

--
-- Indizes für die Tabelle `objects`
--
ALTER TABLE `objects`
  ADD PRIMARY KEY (`objectid`), ADD KEY `objectid` (`objectid`), ADD KEY `objectid_2` (`objectid`);

--
-- Indizes für die Tabelle `sessions`
--
ALTER TABLE `sessions`
  ADD PRIMARY KEY (`sessionid`);

--
-- Indizes für die Tabelle `worlds`
--
ALTER TABLE `worlds`
  ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `accounts`
--
ALTER TABLE `accounts`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=11;
--
-- AUTO_INCREMENT für Tabelle `characters`
--
ALTER TABLE `characters`
  MODIFY `objectID` bigint(20) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=1152921504606846994;
--
-- AUTO_INCREMENT für Tabelle `equipment`
--
ALTER TABLE `equipment`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=171;
--
-- AUTO_INCREMENT für Tabelle `friends`
--
ALTER TABLE `friends`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=6;
--
-- AUTO_INCREMENT für Tabelle `inventory`
--
ALTER TABLE `inventory`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=46;
--
-- AUTO_INCREMENT für Tabelle `mails`
--
ALTER TABLE `mails`
  MODIFY `id` bigint(20) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=7;
--
-- AUTO_INCREMENT für Tabelle `missions`
--
ALTER TABLE `missions`
  MODIFY `id` bigint(20) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=11;
--
-- AUTO_INCREMENT für Tabelle `npcs`
--
ALTER TABLE `npcs`
  MODIFY `id` int(10) unsigned NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT für Tabelle `objects`
--
ALTER TABLE `objects`
  MODIFY `objectid` bigint(64) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=1152921510794154825;
--
-- AUTO_INCREMENT für Tabelle `sessions`
--
ALTER TABLE `sessions`
  MODIFY `sessionid` int(20) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=10;
--
-- AUTO_INCREMENT für Tabelle `worlds`
--
ALTER TABLE `worlds`
  MODIFY `id` int(50) unsigned NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
