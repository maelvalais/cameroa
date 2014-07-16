# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = cameroa.h ListesCommandesReponsesCamerOA.h pointscomm.h \
			SimulateurApogee.h WidgetPoseImage.h WidgetPoseCentrage.h ApnCamData.h\
			ApnCamData_KAI4020MLB.h\
			ApnCamera.h ApnCamera_USB.h ApnCamTable.h Apn.h Apogee.h ApogeeLinux.h \
			ApogeeUsbErr.h ApogeeUsb.h stdafx.h usb.h trameimage.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/cameroa.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = cameroa cameroa_simulateur

# the application source, library search path, and link libraries

cameroa_SOURCES = main.cpp cameroa.cpp pointscomm.cpp SimulateurApogee.cpp \
		WidgetPoseImage.cpp WidgetPoseCentrage.cpp ApnCamData.cpp\
		ApnCamData_KAI4020MLB.cpp ApnCamera.cpp \
		ApnCamera_Linux.cpp ApnCamera_USB.cpp ApnCamTable.cpp ApogeeUsbLinux.cpp \
		WidgetPoseImage.moc.cpp WidgetPoseCentrage.moc.cpp

# MAEL VALAIS: j'ai ajouté les fichiers WidgetPoseImage.moc.cpp WidgetPoseCentrage.moc.cpp\
# car apparement les moc n'étaient pas pris en compte (d'ou lerreur avec les vtables)
		
cameroa_simulateur_SOURCES = $(cameroa_SOURCES)


# Flags
#cameroa_CXXFLAGS=-std=c++11
cameroa_LDFLAGS = $(KDE_RPATH) $(all_libraries)
# MAEL VALAIS: -lcrypto ajouté car problème de précédence des librairies (-lssl puis -lcrypto...)
cameroa_LDADD = -lssl -lusb $(LIB_KDEUI) -lcrypto

cameroa_simulateur_LDFLAGS=$(cameroa_LDFLAGS) -D_SIMULATEUR_APOGEE
cameroa_simulateur_CPPFLAGS=-D_SIMULATEUR_APOGEE
cameroa_simulateur_LDADD = $(cameroa_LDADD)


# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Utilities
shelldesktop_DATA = cameroa.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/cameroa
shellrc_DATA = cameroaui.rc

