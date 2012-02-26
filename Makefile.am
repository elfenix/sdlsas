# Makefile.am: creates toplevel makefile
SUBDIRS = src

# Install all data files and game data
fontsdir=$(pkgdatadir)/fonts
graphicsdir=$(pkgdatadir)/graphics
soundsdir=$(pkgdatadir)/sounds

fonts_DATA = \
    fonts/nicefont.ttf \
    fonts/Wargames.ttf

graphics_DATA = \
    graphics/back1.png \
    graphics/back.png \
    graphics/title2.png \
    graphics/title.png \
    graphics/backdrops/back1.jpg \
    graphics/backdrops/back2.jpg \
    graphics/backdrops/back3.jpg \
    graphics/sprite/bigast.png \
    graphics/sprite/bullet2.png \
    graphics/sprite/bullet.png \
    graphics/sprite/enemy.png \
    graphics/sprite/eship1.png \
    graphics/sprite/esmallast.png \
    graphics/sprite/medast.png \
    graphics/sprite/mouse.png \
    graphics/sprite/shieldC.png \
    graphics/sprite/shield.png \
    graphics/sprite/ship0.png \
    graphics/sprite/ship1.png \
    graphics/sprite/smallast.png \
    graphics/sprite/spinner.png \
    graphics/sprite/wMaxPowerUp.png \
    graphics/sprite/wRechargePowerUp.png \
    graphics/sprite/wShieldPowerUp.png \
    graphics/sprite/wShieldRecharge.png \
    graphics/sprite/wThreePowerUp.png


sounds_DATA = \
    sounds/boom1.wav \
    sounds/boom2.wav \
    sounds/engine.wav \
    sounds/flash.wav \
    sounds/powerup.wav \
    sounds/shipexplode.wav \
    sounds/warp.wav \
    sounds/zap.wav

EXTRA_DIST = $(fonts_DATA) $(graphics_DATA) $(sounds_DATA)
