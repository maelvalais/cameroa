# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = cameroa.h ListesCommandesReponsesCamerOA.h pointscomm.h \
			SimulateurApogee.h WidgetPoseImage.h WidgetPoseCentrage.h ApnCamData_CCD3011B.h \
			ApnCamData_CCD3011.h ApnCamData_CCD4240B.h ApnCamData_CCD4240.h ApnCamData_CCD4710ALT.h \
			ApnCamData_CCD4710B.h ApnCamData_CCD4710.h ApnCamData_CCD4720B.h ApnCamData_CCD4720.h \
			ApnCamData_CCD5520B.h ApnCamData_CCD5520.h ApnCamData_CCD5710B.h ApnCamData_CCD5710F.h \
			ApnCamData_CCD5710.h ApnCamData_CCD7700B.h ApnCamData_CCD7700.h ApnCamData.h \
			ApnCamData_KAF0261EB.h ApnCamData_KAF0261E.h ApnCamData_KAF0401EB.h ApnCamData_KAF0401E.h \
			ApnCamData_KAF09000.h ApnCamData_KAF10011105.h ApnCamData_KAF1001EB.h ApnCamData_KAF1001E.h \
			ApnCamData_KAF1001ENS.h ApnCamData_KAF1301EB.h ApnCamData_KAF1301E.h ApnCamData_KAF1401E.h \
			ApnCamData_KAF1602EB.h ApnCamData_KAF1602E.h ApnCamData_KAF16801E.h ApnCamData_KAF3200EB.h \
			ApnCamData_KAF3200E.h ApnCamData_KAF4202.h ApnCamData_KAF6303EB.h ApnCamData_KAF6303E.h \
			ApnCamData_KAI11000C.h ApnCamData_KAI11000CL.h ApnCamData_KAI11000M.h ApnCamData_KAI11000ML.h \
			ApnCamData_KAI2001CL.h ApnCamData_KAI2001M.h ApnCamData_KAI2001ML.h ApnCamData_KAI2020C.h \
			ApnCamData_KAI2020CLB.h ApnCamData_KAI2020CL.h ApnCamData_KAI2020M.h ApnCamData_KAI2020MLB.h \
			ApnCamData_KAI2020ML.h ApnCamData_KAI4020C.h ApnCamData_KAI4020CLB.h ApnCamData_KAI4020CL.h \
			ApnCamData_KAI4020M.h ApnCamData_KAI4020MLB.h ApnCamData_KAI4020ML.h ApnCamData_TH7899.h \
			ApnCamera.h ApnCamera_USB.h ApnCamTable.h Apn.h Apogee.h ApogeeLinux.h \
			ApogeeUsbErr.h ApogeeUsb.h stdafx.h usb.h trameimage.h ApnCamData_KAF16803.h

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
		WidgetPoseImage.cpp WidgetPoseCentrage.cpp ApnCamData_CCD3011B.cpp ApnCamData_CCD3011.cpp \
		ApnCamData_CCD4240B.cpp ApnCamData_CCD4240.cpp ApnCamData_CCD4710ALT.cpp \
		ApnCamData_CCD4710B.cpp ApnCamData_CCD4710.cpp ApnCamData_CCD4720B.cpp ApnCamData_CCD4720.cpp \
		ApnCamData_CCD5520B.cpp ApnCamData_CCD5520.cpp ApnCamData_CCD5710B.cpp ApnCamData_CCD5710.cpp \
		ApnCamData_CCD5710F.cpp ApnCamData_CCD7700B.cpp ApnCamData_CCD7700.cpp ApnCamData.cpp \
		ApnCamData_KAF0261EB.cpp ApnCamData_KAF0261E.cpp ApnCamData_KAF0401EB.cpp \
		ApnCamData_KAF0401E.cpp ApnCamData_KAF09000.cpp ApnCamData_KAF10011105.cpp \
		ApnCamData_KAF1001EB.cpp ApnCamData_KAF1001E.cpp ApnCamData_KAF1001ENS.cpp \
		ApnCamData_KAF1301EB.cpp ApnCamData_KAF1301E.cpp ApnCamData_KAF1401E.cpp \
		ApnCamData_KAF1602EB.cpp ApnCamData_KAF1602E.cpp ApnCamData_KAF16801E.cpp \
		ApnCamData_KAF3200EB.cpp ApnCamData_KAF3200E.cpp ApnCamData_KAF4202.cpp ApnCamData_KAF6303EB.cpp \
		ApnCamData_KAF6303E.cpp ApnCamData_KAI11000C.cpp ApnCamData_KAI11000CL.cpp \
		ApnCamData_KAI11000M.cpp ApnCamData_KAI11000ML.cpp ApnCamData_KAI2001CL.cpp \
		ApnCamData_KAI2001M.cpp ApnCamData_KAI2001ML.cpp ApnCamData_KAI2020C.cpp \
		ApnCamData_KAI2020CLB.cpp ApnCamData_KAI2020CL.cpp ApnCamData_KAI2020M.cpp \
		ApnCamData_KAI2020MLB.cpp ApnCamData_KAI2020ML.cpp ApnCamData_KAI4020C.cpp \
		ApnCamData_KAI4020CLB.cpp ApnCamData_KAI4020CL.cpp ApnCamData_KAI4020M.cpp \
		ApnCamData_KAI4020MLB.cpp ApnCamData_KAI4020ML.cpp ApnCamData_TH7899.cpp ApnCamera.cpp \
		ApnCamera_Linux.cpp ApnCamera_USB.cpp ApnCamTable.cpp ApogeeUsbLinux.cpp \
		ApnCamData_KAF16803.cpp\
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

