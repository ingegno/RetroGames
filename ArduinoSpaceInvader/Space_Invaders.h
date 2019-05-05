#ifndef Space_Invaders_H
#define Space_Invaders_H

#include "Arduino.h"

class Ship { //my ship
private:
    uint8_t x, y; //location
    uint8_t speed; //the speed of the movement
    uint8_t lives; //the number of lives left
public:
    Ship(uint8_t newX,  uint8_t newY);
    ~Ship();
    void move(uint8_t direction);
    void setX(uint8_t newX);
    void setY(uint8_t newY);
    uint8_t getX();
    uint8_t getY();
    void setLives(uint8_t newLives);
    uint8_t getLives();
};

class Enemy { //the enemy ships
private:
    uint8_t x, y; //location
    uint8_t speed; //the speed of the movement
    bool alive; //true - it's alive, false - it is not
public:
	Enemy(uint8_t newX,  uint8_t newY, uint8_t newSpeed);
	Enemy();
	~Enemy();
	char move();
	void update(char ch);
	void setDead();
	uint8_t getX();
    uint8_t getY();
    bool Alive();
    bool Defeat();
};

class Bullet {
private:
    uint8_t x, y;
    uint8_t speed;
    char exist;
public:
    Bullet(uint8_t newX, uint8_t newY, uint8_t newSpeed);
    Bullet();
    ~Bullet();
    void move();
    uint8_t getSpeed();
    uint8_t getX();
    uint8_t getY();
    char getExist();
    char collisionShip(Ship* ship);
    char collisionEnemy(Enemy* enemy);
};

#endif
