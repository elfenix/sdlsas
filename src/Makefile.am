# Makefile.am: Sasteroids top src Makefile

bin_PROGRAMS = sasteroids
AM_CPPFLAGS = $(GL_CFLAGS) $(SDL_CFLAGS) -DGAMEDIR=\"$(pkgdatadir)\"

sasteroids_SOURCES = \
    Alien.cpp \
    Asteroid.cpp \
    Bullet.cpp \
    Explosion.cpp \
    fastmath.cpp \
    GameEntity.cpp \
    GraphicsMenu.cpp \
    Player.cpp \
    PlayingField.cpp \
    PowerUp.cpp \
    Resources.cpp \
    sasteroids.cpp \
    ScreenBitmap.cpp \
    Spinner.cpp \
    UserInterfaceManager.cpp

sasteroids_LDADD = $(GL_LIBS) $(SDL_LIBS)

sasteroids_MANS = sasteroids.6
EXTRA_DIST = $(sasteroids_MANS)
