#pragma once
#include <stdio.h>
#include "bdaqctrl.h"
using namespace Automation::BDaq;

enum TimeUnit { Microsecond ,Millisecond, Second };
enum FrequencyUnit { Hz, KHz, MHz };
class SimpleGraph : public CStatic
{
	DECLARE_DYNAMIC(SimpleGraph)
public:
	SimpleGraph();
	virtual ~SimpleGraph();
	DECLARE_MESSAGE_MAP()
public:
   DOUBLE       m_XCordTimeDiv;
   DOUBLE       m_XCordTimeOffset;
   DOUBLE       m_YCordRangeMax;
   DOUBLE       m_YCordRangeMin;
   static COLORREF LinesColor[16];
   static void GetXCordRangeLabels(CString * rangeStrs,DOUBLE rangeMax, DOUBLE rangeMin, TimeUnit unit);
   static void GetYCordRangeLabels(CString * rangeStrs,DOUBLE rangeMax, DOUBLE rangeMin, FrequencyUnit unit);
   static void GetYCordRangeLabels(CString * rangeStrs,DOUBLE rangeMax, DOUBLE rangeMin, ValueUnit unit);  
public:
   void Chart(DOUBLE* dataScaled, int channelCount, int dataCountOfPerChan, double xIncBySec);
   void Clear(void);
   void Shift(int shiftTime);
   void Div(int divTime);
   void InitGraph();

private:
	CDC			m_memDc;
	CRect		   m_rect;
   CBitmap		m_bitmap;
	int			m_plotCount;
	int			m_pointCountPerScreen;
   int         m_copyDataCountPerChan;	
	DOUBLE      m_xIncByTime;
   DOUBLE      m_XCordDividedRate;
	DOUBLE*		m_drawDataBuffer;
	POINT*		m_dataPointBuffer;
   DOUBLE      m_shiftCount;
	int         m_drawDataBufferLength;
	int         m_dataCountCachePerPlot;
	int         m_mapDataIndexPerPlot;
   int         m_dataCountPerPlot;
	CRITICAL_SECTION criticalSection;
   void CalcDrawParams(double XIncBySec,int dataCountPerPlot);
   void MapDataPoints( );
   void SaveData(DOUBLE * data, int plotCount, int dataCountPerPlot);
   void Draw();
protected:
   afx_msg void OnPaint();
};


