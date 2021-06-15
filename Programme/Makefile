#
# Makefile pour generer le commutateur virtuel
#

OBJS = programme.c
LIBS = -lpthread
LOCAL_LIBS  = -L ../Communication -L ../Flux 
LOCAL_LIBS += -lcom -lflux 
CFLAGS     += -I ../Communication -I ../Flux

#
# Cible generale
#

all: programme

#
# La cible de nettoyage
#

clean: 
	rm -f core *.o programme

#
# Les cibles pour l'executable
#

programme: $(OBJS)
	$(CC) $(CFLAGS) -o programme $(OBJS) $(LOCAL_LIBS) $(LIBS)
