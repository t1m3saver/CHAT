DROP DATABASE IF EXISTS chat_app;
CREATE DATABASE IF NOT EXISTS chat_app;
USE chat_app;

CREATE USER 'chatDBUser'@'%' IDENTIFIED BY 'dbpasswd123!';
GRANT ALL PRIVILEGES ON chat_app.* TO 'chatDBUser'@'%';
FLUSH PRIVILEGES;


create table user (
 userId int auto_increment primary key,
 userName varchar(50) character set utf8mb4 not null unique,
 phoneNumber varchar(20) unique,
 emailAddress varchar(20) not null unique,
 passwordHash varchar(255) not null,
 createDay TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE userInfo (
    userId INT NOT NULL COMMENT '用户ID，与user表共享主键',
    fullName VARCHAR(100) COMMENT '真实姓名',
    idNumber VARCHAR(30) COMMENT '身份证号码',
    bio TEXT COMMENT '个人简介',
    avatar_url VARCHAR(255) COMMENT '头像URL',
    address VARCHAR(255) COMMENT '地址',
    gender ENUM('male', 'female', 'other') COMMENT '性别',
    birthday DATE COMMENT '出生日期',
    PRIMARY KEY (userId),
    FOREIGN KEY (userId) REFERENCES user(userId) ON DELETE CASCADE
);

CREATE TABLE userFriends (
    userId INT NOT NULL,
    friendId INT NOT NULL,
    status ENUM('normal', 'blocked', 'unknown') DEFAULT 'unknown',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (userId, friendId),
    FOREIGN KEY (userId) REFERENCES user(userId) ON DELETE CASCADE,
    FOREIGN KEY (friendId) REFERENCES user(userId) ON DELETE CASCADE
);

CREATE TABLE userGroups (
    groupId INT AUTO_INCREMENT PRIMARY KEY,       -- 群组ID
    groupName VARCHAR(100) NOT NULL,              -- 群组名称
    usersNum INT,
    description TEXT,                        -- 群组描述
    createdBy INT NOT NULL,                 -- 群组创建者的用户ID
    createdTimeStamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  -- 群组创建时间
    updatedTimeStamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,  -- 群组更新时间
    status ENUM('normal', 'blocked') DEFAULT 'normal',          -- 群组是否有效（是否仍然存在）
    FOREIGN KEY (createdBy) REFERENCES user(userId) ON DELETE CASCADE  -- 外键：创建者
);

CREATE TABLE groupMembers (
    groupId INT NOT NULL,                  -- 群组ID
    userId INT NOT NULL,                   -- 用户ID
    role ENUM('owner', 'member', 'admin') DEFAULT 'member' NOT NULL,  -- 用户在群组中的角色（群主、普通成员、管理员）
    joinedTimeStamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,  -- 加入时间
    leftTimeStamp TIMESTAMP,                      -- 离开群组的时间
    isActive BOOLEAN DEFAULT TRUE,         -- 成员是否仍然在群组中
    PRIMARY KEY (groupId, userId),        -- 群组和用户的唯一组合，确保一个用户只能在一个群组中有一个角色
    FOREIGN KEY (groupId) REFERENCES userGroups(groupId) ON DELETE CASCADE,  -- 外键：群组ID
    FOREIGN KEY (userId) REFERENCES user(userId) ON DELETE CASCADE    -- 外键：用户ID
);




