

bin_PROGRAMS = cameroa cameroa_simulateur

# the application source, library search path, and link libraries

cameroa_SOURCES = main.cpp cameroa.cpp pointscomm.cpp SimulateurApogee.cpp \
		WidgetPoseImage.cpp WidgetPoseCentrage.cpp ApnCamData.cpp\
		ApnCamData_KAI4020MLB.cpp ApnCamera.cpp \
		ApnCamera_Linux.cpp ApnCamera_USB.cpp ApnCamTable.cpp ApogeeUsbLinux.cpp \
		$(BUILT_SOURCES)

# MAEL VALAIS: j'ai ajouté les fichiers WidgetPoseImage.moc.cpp WidgetPoseCentrage.moc.cpp\
# car apparement les moc n'étaient pas pris en compte (d'ou lerreur avec les vtables)
		
cameroa_simulateur_SOURCES = $(cameroa_SOURCES)


# Flags
#cameroa_CXXFLAGS=-std=c++11

cameroa_CXXFLAGS = $(QT_CXXFLAGS) $(AM_CXXFLAGS)
cameroa_CFLAGS = $(QT_CFLAGS) $(AM_CFLAGS)
cameroa_CPPFLAGS = $(QT_CPPFLAGS) $(AM_CPPFLAGS) -I/usr/include/kde
cameroa_LDFLAGS = $(QT_LDFLAGS) $(LDFLAGS) 
cameroa_LDADD = $(QT_LIBS) $(LDADD) -lssl -lcrypto -L/usr/lib/kde -lkdeui -lkdecore
# -lcrypto ajouté car problème de précédence des librairies (-lssl puis -lcrypto...)

cameroa_simulateur_CXXFLAGS = $(cameroa_CXXFLAGS)
cameroa_simulateur_CFLAGS = $(cameroa_CFLAGS)
cameroa_simulateur_CPPFLAGS = $(cameroa_CPPFLAGS) -D_SIMULATEUR_APOGEE
cameroa_simulateur_LDFLAGS= $(cameroa_LDFLAGS) 
cameroa_simulateur_LDADD = $(cameroa_LDADD)



# Fichiers qui devront être compilés par uic ou moc avant la compilation
# Voir CONCERNANT Qt au dessus
BUILT_SOURCES=\
	WidgetPoseImage.moc.cpp WidgetPoseCentrage.moc.cpp
	

# Règle perso car la règle présente dans autotroll.mk (%ui_.h: %.ui) ne marche pas
# pour produire du src/ui_blabla.h (car il faut deux wildcards, genre %ui_%.h)
#src/ui_%.h: src/%.ui
#	$(UIC) $< -o $@

# NOTE sur $(srcdir) et $(includedir)
# il faudrait théoriquement préfixer tous les fichiers source par $(srcdir)
# et tous les fichiers header par $(includedir)