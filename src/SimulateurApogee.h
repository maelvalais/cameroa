/* HEADER DU MODULE SIMULANT LE COMPORTEMENT D'UNE CAMERA APOGEE USB

   (d)05/2006 David.Romeuf@univ-lyon1.fr
*/

#include "Apogee.h"
#include "ApnCamera.h"

#ifndef _SIMULATEUR_APOGEE_USB
#define _SIMULATEUR_APOGEE_USB

class SimulateurApogeeUSB
{
public:
	int verbeux;

	unsigned short m_TotalRows;
	unsigned short m_TotalColumns;
	unsigned short m_ImagingColumns;
	unsigned short m_ImagingRows;
	unsigned short m_pvtRoiStartX;
	unsigned short m_pvtRoiPixelsH;
	unsigned short m_pvtRoiStartY;
	unsigned short m_pvtRoiPixelsV;
	unsigned short binningx;
	unsigned short binningy;
	Apn_Resolution	resolution;

	double CoolerSetPoint;

	SimulateurApogeeUSB(int pverbeux);
	~SimulateurApogeeUSB();

	bool InitDriver( unsigned long  CamIdA,unsigned short CamIdB,unsigned long Option);

	bool CloseDriver();

	bool ResetSystem();

	bool sensorInfo();

	void write_CameraMode(Apn_CameraMode CameraMode);

	void write_LedMode(Apn_LedMode LedMode);

	void write_TestLedBrightness(double TestLedBrightness);

	void write_LedState(unsigned short LedId,Apn_LedState LedState);

	void write_ForceShutterOpen(bool ForceShutterOpen);

	void write_CoolerEnable(bool CoolerEnable);

	void write_ImageCount(unsigned short Count);

	double read_TempCCD();

	double read_TempHeatsink();

	double read_CoolerDrive();

	void write_FanMode(Apn_FanMode FanMode);

	void write_CoolerSetPoint(double SetPoint);

	bool StopExposure(bool DigitizeData);

	void write_RoiBinningH(unsigned short BinningH);

	void write_RoiBinningV(unsigned short BinningV);

	void write_DataBits(Apn_Resolution BitResolution);

	bool Expose(double Duration,bool Light);

	long GetImageData(unsigned short *pImageData,unsigned short &Width,unsigned short &Height,unsigned long  &Count);
};

#endif
