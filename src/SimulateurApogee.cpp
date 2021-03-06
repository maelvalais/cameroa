/* MODULE SIMULANT LE COMPORTEMENT D'UNE CAMERA APOGEE USB

   (d)05/2006 David.Romeuf@univ-lyon1.fr
*/

#include <iostream>
#include <new>
#include <cmath>

#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "SimulateurApogee.h"

SimulateurApogeeUSB::SimulateurApogeeUSB(int pverbeux)
{
	verbeux=pverbeux;

	m_TotalRows=0;
	m_TotalColumns=0;
	m_ImagingColumns=0;
	m_ImagingRows=0;
	m_pvtRoiStartX=0;
	m_pvtRoiPixelsH=0;
	m_pvtRoiStartY=0;
	m_pvtRoiPixelsV=0;
	resolution=Apn_Resolution_TwelveBit;

	CoolerSetPoint=-20.0;
}

SimulateurApogeeUSB::~SimulateurApogeeUSB()
{
}

bool SimulateurApogeeUSB::InitDriver( unsigned long  CamIdA,unsigned short CamIdB,unsigned long Option)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::InitDriver(" << CamIdA << "," << CamIdB << "," << Option << ")" << std::endl;

	m_TotalColumns=2112;
	m_TotalRows=2072;
	m_ImagingColumns=2048;
	m_ImagingRows=2048;
	m_pvtRoiStartX=0;
	m_pvtRoiPixelsH=m_ImagingColumns;
	m_pvtRoiStartY=0;
	m_pvtRoiPixelsV=m_ImagingRows;

	return true;
}

bool SimulateurApogeeUSB::CloseDriver()
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::CloseDriver()" << std::endl;

	return true;
}

bool SimulateurApogeeUSB::ResetSystem()
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::ResetSystem()" << std::endl;

	return true;
}

bool SimulateurApogeeUSB::sensorInfo()
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::sensorInfo()" << std::endl;

	return true;
}

void SimulateurApogeeUSB::write_CameraMode(Apn_CameraMode CameraMode)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_CameraMode(" << CameraMode << ")" << std::endl;
}

void SimulateurApogeeUSB::write_LedMode(Apn_LedMode LedMode)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_LedMode(" << LedMode << ")"<< std::endl;
}

void SimulateurApogeeUSB::write_TestLedBrightness(double TestLedBrightness)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_TestLedBrightness(" << TestLedBrightness << ")" << std::endl;
}

void SimulateurApogeeUSB::write_LedState(unsigned short LedId,Apn_LedState LedState)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_LedState(" << LedId << "," << LedState << ")" << std::endl;
}

void SimulateurApogeeUSB::write_ForceShutterOpen(bool ForceShutterOpen)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_ForceShutterOpen(" << ForceShutterOpen << ")" << std::endl;
}

void SimulateurApogeeUSB::write_CoolerEnable(bool CoolerEnable)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_CoolerEnable(" << CoolerEnable << ")" << std::endl;
}

void SimulateurApogeeUSB::write_ImageCount(unsigned short Count)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_ImageCount(" << Count << ")" << std::endl;
}

double SimulateurApogeeUSB::read_TempCCD()
{
	double valeur=CoolerSetPoint;

	srandom((unsigned int) time(NULL));
	valeur+=((double) (random() % 10))/10.0;

	if( verbeux ) std::cout << "SimulateurApogeeUSB::read_TempCCD(" << valeur << ")" << std::endl;

	return valeur;
}

double SimulateurApogeeUSB::read_TempHeatsink()
{
	double valeur=21.0;

	srandom((unsigned int) time(NULL));
	valeur+=((double) (random() % 10))/10.0;

	if( verbeux ) std::cout << "SimulateurApogeeUSB::read_TempHeatsink(" << valeur << ")" << std::endl;

	return valeur;
}

double SimulateurApogeeUSB::read_CoolerDrive()
{
	double valeur=30.0;

	srandom((unsigned int) time(NULL));
	valeur+=((double) (random() % 300))/100.0;

	if( verbeux ) std::cout << "SimulateurApogeeUSB::read_CoolerDrive(" << valeur << ")" << std::endl;

	return valeur;
}

void SimulateurApogeeUSB::write_FanMode(Apn_FanMode FanMode)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_FanMode(" << FanMode << ")" << std::endl;
}

void SimulateurApogeeUSB::write_CoolerSetPoint(double SetPoint)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_CoolerSetPoint(" << SetPoint << ")" << std::endl;

	CoolerSetPoint=SetPoint;
}

bool SimulateurApogeeUSB::StopExposure(bool DigitizeData)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::StopExposure(" << DigitizeData << ")" << std::endl;

	return true;
}

void SimulateurApogeeUSB::write_RoiBinningH(unsigned short BinningH)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_RoiBinningH(" << BinningH << ")"  << std::endl;

	binningx=BinningH;
}

void SimulateurApogeeUSB::write_RoiBinningV(unsigned short BinningV)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_RoiBinningV(" << BinningV << ")" << std::endl;

	binningy=BinningV;
}

void SimulateurApogeeUSB::write_DataBits(Apn_Resolution BitResolution)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::write_DataBits(" << BitResolution << ")" << std::endl;

	resolution=BitResolution;
}

bool SimulateurApogeeUSB::Expose(double Duration,bool Light)
{
	if( verbeux ) std::cout << "SimulateurApogeeUSB::Expose(" << Duration << "," << Light << ")" << std::endl;

	usleep((__useconds_t) (Duration*1000000.0));

	return true;
}

long SimulateurApogeeUSB::GetImageData(unsigned short *pImageData,unsigned short &Width,unsigned short &Height,unsigned long  &Count)
{
	Width=m_pvtRoiPixelsH;
	Height=m_pvtRoiPixelsV;
	Count=1;

	// Coordonnees du cercle lumineux dans le repere du CCD
	//
	srandom((unsigned int) time(NULL));

	double pxc=(double) (m_ImagingColumns/2-10+(random() % 20));	// Centre du cercle au milieu mais il bouge
	double pyc=(double) (m_ImagingRows/2-10+(random() %20));
	double ep=(double) (10+random() % 20);				// epaisseur du cercle
	double raym=((double) (m_ImagingColumns/4));
	double rayi=raym-ep/2.0;					// rayon interieur	
	double raye=raym+ep/2.0;					// rayon exterieur
	double vmax=(double) (20000+(random() % 20000));		// valeur d'intensite maximale

	// On calcul uniquement la portion a acquerir
	//
	for( int y=0; y < m_pvtRoiPixelsV; y++ )
	{
		for( int x=0; x < m_pvtRoiPixelsH; x++ )
		{
			int ydi=m_pvtRoiStartY+y*binningx;		// Coordonnees dans l'image globale
			int xdi=m_pvtRoiStartX+x*binningy;

			double d=std::sqrt( std::pow((double) (ydi-pyc),2.0) + std::pow((double) (xdi-pxc),2.0) );

			unsigned long offset=y*m_pvtRoiPixelsH+x;

			if( d <= raye && d >= rayi )
			{
				double pourc=std::fabs(raym-d)*100.0/ep/2.0;
				pourc=100.0-pourc;
				pourc=vmax*pourc/100;
				if( pourc < 0 ) pourc=0;

				*(pImageData+offset)=(unsigned short) pourc;
			}
			else
			{
				*(pImageData+offset)=random() % 1000;
			}
		}
	}

	if( verbeux ) std::cout << "SimulateurApogeeUSB::GetImageData(" << pImageData << ")=" << Width << "," << Height << "," << Count << std::endl;

	if( resolution == Apn_Resolution_SixteenBit ) usleep(3000000); else usleep(300000);

	return CAPNCAMERA_SUCCESS;
}
