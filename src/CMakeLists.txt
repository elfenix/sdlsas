
target_sources(sdlsas PUBLIC
	"Alien.cpp"	"Alien.hpp"
	"Asteroid.cpp" "Asteroid.hpp"
	"Bullet.cpp" "Bullet.hpp"
	"Explosion.cpp" "Explosion.hpp"
	"fastmath.cpp" "fastmath.h"
	"gameconf.h"
	"GameEntity.cpp" "GameEntity.hpp"
	"GraphicsMenu.cpp" "GraphicsMenu.hpp"
	"Player.cpp" "Player.hpp"
	"PlayingField.cpp" "PlayingField.hpp"
	"PowerUp.cpp" "PowerUp.hpp"
	"Resources.cpp" "Resources.hpp"
	"sasteroids.cpp" "sasteroids.h"
	"ScreenBitmap.cpp" "ScreenBitmap.hpp"
	"Spinner.cpp" "Spinner.hpp"
	"UserInterfaceManager.cpp" "UserInterfaceManager.hpp"
	"vector.h")

target_include_directories(sdlsas PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
