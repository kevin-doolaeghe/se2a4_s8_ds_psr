#
# Makefile pour generer la bibliotheque des flux d'execution
#

all: libflux.a

#
# La cible de nettoyage
#

clean: 
	rm -f core *.o libflux.a

#
# Les cibles pour la bibliotheque
#

libflux.o: libflux.c libflux.h
 
libflux.a: libflux.o
	rm -f $@
	$(AR) rs $@ $+
