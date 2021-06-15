#
# Makefile pour le DS PSR
#

#
# Constantes pour la compilation
#

export LD = gcc
export AR = ar
export CFLAGS += -g -Wall
MAKE = make 

#
# Constantes du projet
#

DIRS=Communication Flux Programme

#
# Cible principale
#

all: $(patsubst %, _dir_%, $(DIRS))

$(patsubst %,_dir_%,$(DIRS)):
	cd $(patsubst _dir_%,%,$@) && $(MAKE)

#
# Cible de deverminage
#

debug: CFLAGS += -DDEVERMINE
debug: all

#
# Cible de nettoyage
#

clean: $(patsubst %, _clean_%, $(DIRS))

$(patsubst %,_clean_%,$(DIRS)):
	cd $(patsubst _clean_%,%,$@) && $(MAKE) clean
