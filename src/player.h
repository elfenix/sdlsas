#ifndef __SHIP_H__
#define __SHIP_H__

#include "obj.h"
#include "screenobject.h"

class Player : public ScreenObject
{
 public:
  Player();

  virtual void draw();
  virtual void tick();
  virtual void SetBitmap();
 
  void shieldOn();
  void shieldOff();
  void shieldAdd(int a);
  int shielded();



  inline void death() 
    { 
      lives--; 
    }

  inline int ships() 
    { 
      return lives; 
    }
  
  inline void setships(int sships)
    { 
      lives = sships; 
    }
  
  inline void addship()	
    { 
      lives++; 
    }

  inline int isDeadStick() 
    { 
      return deadStick; 
    }
   
  inline int shieldPercent() const 
    {
      return (100*(int)shieldTimeLeft)/(int)shieldMax; 
    }

  inline int weaponPercent() const
    {
      return (100*wPower)/(maxPower);
    }

  inline int weaponPower() 
    { 
      if(deadStick) return 0; return wPower; 
    }

  void Reset();
  void SetDeadStick(int dead);
  void addMaxPower(int a);
  void addRegPower(int a);

  void Thrust(float rRate);  
  void Hyper();
  void dischargeWeapon();
  void Brake();
  void Fire();

  // Game play variables
  int canShootThree;
  int canShootBack;
  int canFireShield;
  int canRecharge;

 protected:
  int shieldMax, shieldTimeLeft, shieldLives;
  int lives, thrustcnt, wPower, deadStick;
  int maxPower, rechargeRate, shieldStatus;
};

#endif
