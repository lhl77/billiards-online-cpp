-- MySQL dump 10.13  Distrib 5.7.44, for Linux (x86_64)
--
-- Host: localhost    Database: c-class-billboards
-- ------------------------------------------------------
-- Server version	5.7.44-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Users`
--

DROP TABLE IF EXISTS `Users`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `win_count` int(11) DEFAULT '0',
  `total_count` int(11) DEFAULT '0',
  `coins` int(11) DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `username` (`username`)
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;



DROP TABLE IF EXISTS `characters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `characters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  `description` text,
  `price` int(11) DEFAULT '0',
  `url_big` varchar(255) NOT NULL,
  `url_small` varchar(255) NOT NULL,
  `url_bg` varchar(255) NOT NULL,
  `url_game_bg` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `characters`
--

LOCK TABLES `characters` WRITE;
/*!40000 ALTER TABLE `characters` DISABLE KEYS */;
INSERT INTO `characters` VALUES (1,'小白','台球小白，天真无邪，动作笨拙，长得很别致。',0,'https://cdn.sa.net/2025/03/09/ACqfzEWPeBM3Jbx.png','https://cdn.sa.net/2025/03/09/BGaHlsypMo8qkDT.png','https://cdn.sa.net/2025/03/13/hIBMYG38vl6nFKQ.png','https://cdn.sa.net/2025/03/28/ine9MolpWCTtQ8y.png'),(2,'暗鸦','传闻他曾在赌局中输掉一切，如今只为复仇而战。',2,'https://cdn.sa.net/2025/03/09/8Mo1uxiHDELtk3b.png','https://cdn.sa.net/2025/03/09/MdVFIpxbqClXKet.png','https://cdn.sa.net/2025/03/13/zH6oUCvsPnRMSib.png','https://cdn.sa.net/2025/03/28/7nT3uPzeRLKUbQ9.png'),(3,'莉莉安','红唇卷发的高跟鞋御姐，球风优雅却致命，隐藏实力只为享受碾压快感。',5,'https://cdn.sa.net/2025/03/13/fvMUT3Qo4kxNFWm.png','https://cdn.sa.net/2025/03/13/9Uh1KEVdrOmzlsv.png','https://cdn.sa.net/2025/03/27/ytSeRcqYl5nDkrJ.png','https://cdn.sa.net/2025/03/28/XlWydIE6KgF3Rcz.png'),(4,'扳手','改造右臂为机械义肢的暴躁工科男，穿着沾满油污的背带裤。',10,'https://cdn.sa.net/2025/03/14/3bBi4kueIdHh5RQ.png','https://cdn.sa.net/2025/03/14/XZtp4v9TOGw1rlQ.png','https://cdn.sa.net/2025/03/14/IOGm4kDqBtwCcJ1.png','https://cdn.sa.net/2025/03/28/SLcGeEYpACxVryH.png'),(5,'星野','科技宅少年，球桌在他眼中是实时演算的网格。',35,'https://cdn.sa.net/2025/03/14/apijc4Buoqx6Kv5.png','https://cdn.sa.net/2025/03/14/BKTwlthkUIPWZy5.png','https://cdn.sa.net/2025/03/15/XLytVD9roxCW8sd.png','https://cdn.sa.net/2025/03/28/3tvWNxPFwKSd6Vl.png'),(6,'老K','披着破旧风衣的邋遢大叔，手握一根掉漆的老式球杆。',70,'https://cdn.sa.net/2025/03/15/or7Ys8U39VIdyJB.png','https://cdn.sa.net/2025/03/15/bp5yNKuswZPrOAR.png','https://cdn.sa.net/2025/03/20/g3Q1vEt7CRxb4au.png','https://cdn.sa.net/2025/03/28/KlNbf1F7HMTYoZh.png'),(7,'教授','打球前必抛硬币决定策略，擅长制造看似混乱实则精妙的连锁反应。',120,'https://cdn.sa.net/2025/03/20/uDKw9ozOaW6S8rU.png','https://cdn.sa.net/2025/03/20/qnacmdRtQj2ClbN.png','https://cdn.sa.net/2025/03/20/jskdcuGL8TKixFg.png','https://cdn.sa.net/2025/03/28/L2aDmbJRFQWUMrz.png'),(8,'闪电','染荧光黄短发，以闪电标志闻名，经常气鼓鼓地踹球桌。',200,'https://cdn.sa.net/2025/03/20/JZ6KCPOzQn2Hqhb.png','https://cdn.sa.net/2025/03/20/MLQl8UtYkWneusR.png','https://cdn.sa.net/2025/03/20/7uziFSXhG6Nltaw.png','https://cdn.sa.net/2025/03/28/IopBMQt631xHJny.png');
/*!40000 ALTER TABLE `characters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `recharge_shop`
--

DROP TABLE IF EXISTS `recharge_shop`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `recharge_shop` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `coin_amount` int(11) NOT NULL,
  `price` decimal(10,2) NOT NULL,
  `image_url` varchar(255) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `recharge_shop`
--

LOCK TABLES `recharge_shop` WRITE;
/*!40000 ALTER TABLE `recharge_shop` DISABLE KEYS */;
INSERT INTO `recharge_shop` VALUES (1,20,2.00,'https://cdn.sa.net/2025/03/03/F5KjtHs8nfIkeJO.png'),(2,100,10.00,'https://cdn.sa.net/2025/03/03/HT6DBlWcmUk1iSh.png'),(3,300,30.00,'https://cdn.sa.net/2025/03/03/2CybUdQf6rMXzSt.png');
/*!40000 ALTER TABLE `recharge_shop` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user_characters`
--

DROP TABLE IF EXISTS `user_characters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `user_characters` (
  `user_id` int(11) NOT NULL,
  `user_character_set` int(11) NOT NULL,
  `user_characters` varchar(255) NOT NULL,
  PRIMARY KEY (`user_id`,`user_character_set`),
  KEY `character_id` (`user_character_set`),
  CONSTRAINT `user_characters_ibfk_1` FOREIGN KEY (`user_id`) REFERENCES `Users` (`id`),
  CONSTRAINT `user_characters_ibfk_2` FOREIGN KEY (`user_character_set`) REFERENCES `characters` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `user_characters`
--



--
-- Dumping events for database 'c-class-billboards'
--

--
-- Dumping routines for database 'c-class-billboards'
--
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2025-04-17  1:17:46
