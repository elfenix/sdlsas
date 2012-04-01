#ifndef PLAYERSHIP_HPP_
#define PLAYERSHIP_HPP_

#include "PlayingField.hpp"
#include "GameEntity.hpp"

void CleanUpStuff();

class Player: public GameEntity
{
public:
    Player();

    virtual void draw();
    virtual void tick();
    virtual void SetBitmap();

    void shieldOn();
    void shieldOff();
    void shieldAdd( int a );
    int shielded();

    void set_can_shoot_three( bool p_enabled );
    void set_can_shield_recharge( bool p_enabled );

    bool get_disabled() const;
    virtual void destroy();
    virtual bool destructive_collision( const GameEntity& p_other );
    virtual bool check_bounce( const GameEntity& p_other ) const;
    virtual bool check_collide( const GameEntity& p_other ) const;
    virtual void notify_bounce( const GameEntity& p_other ) const;
    virtual void notify_destroyed( const GameEntity& p_other );
    void reset_score();
    int get_score();
    void death();
    int ships();
    void setships( int sships );
    void addship();
    int isDeadStick() const;
    int shieldPercent() const;
    int weaponPercent() const;
    int weaponPower();

    void Reset();
    void SetDeadStick( int dead );
    void addMaxPower( int a );
    void addRegPower( int a );
    void Thrust( float rRate );
    void Hyper();
    void dischargeWeapon();
    void Brake();
    void Fire();

protected:
    void CreateBullet( int discharge_count, int angle, bool apply_thrust,
            double scale_thrust = 0.1 );

protected:
    // Game play variables
    int canShootThree;
    int canShootBack;
    int canFireShield;
    int canRecharge;
    int shieldMax, shieldTimeLeft, shieldLives;
    int lives, thrustcnt, wPower, deadStick;
    int maxPower, rechargeRate, shieldStatus;
    int score;
};

#endif
