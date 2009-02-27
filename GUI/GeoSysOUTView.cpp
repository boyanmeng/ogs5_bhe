// GeoSysOUTView.cpp : Implementierung der Klasse CGeoSysView
//

#include "stdafx.h"
#include "GeoSys.h"
#include <algorithm>
#include "math.h"

#include "GeoSysDoc.h"
#include "GeoSysOUTView.h"
#include "OGLEnabledView.h"
#include "MainFrm.h"

// GUI
#include "gs_graphics.h"
#include "gs_output.h"
#include "rf_p.h"
#include ".\geosysoutview.h"
#include "out_dlg.h"
#include "dlg_GHDB.h"

// GeoSys-FEM
#include "femlib.h"
#include "files.h"
#include "elements.h"
#include "nodes.h"
#include "rf_out_new.h"
#include "rf_pcs.h"

// For stess path visualization
#include "pcs_dm.h"

//zoom
#include ".\geosyszoomview.h"
#include "afxpriv.h"    // For WM_SETMESSAGESTRING

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGeoSysOUTView

IMPLEMENT_DYNCREATE(CGeoSysOUTView, CGeoSysZoomView)

BEGIN_MESSAGE_MAP(CGeoSysOUTView, CGeoSysZoomView)
    ON_COMMAND(ID_VIEW_TEMPORALPROFILES_PROPERTIES, OnProperties)

     //05/2004
    ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_COMMAND(ID_VIEW_ZOOMFULL, OnViewZoomfull)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoSysOUTView Konstruktion/Destruktion

/**************************************************************************
GeoSys-GUI: CGeoSysView()
Programing:
09/2003 OK Implementation
**************************************************************************/
CGeoSysOUTView::CGeoSysOUTView()
{
  // FS m_iDisplayOUTProperties = 0;
  m_dXmin = 0.0;
  m_dXmax = 1.0;
  m_dYmin = 0.0;
  m_dYmax = 1.0;
}

CGeoSysOUTView::~CGeoSysOUTView()
{

}

/**************************************************************************
GeoSys-GUI Function OnInitialUpdate
Programing:
05/2004 CC Implementation
09/2004 OK Properties Dialog
**************************************************************************/
void CGeoSysOUTView::OnInitialUpdate()
{
  CGeoSysZoomView::OnInitialUpdate();
  CGeoSysDoc* pDoc = GetDocument();
  // Initialize the CGeoSysZoomView class
  SetZoomSizes(pDoc->GetDocSize());
  // Properties dialog
 /* FS if(GHDBvector_x.size()==0)
 OnProperties();*/
}

BOOL CGeoSysOUTView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: �ndern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Zeichnen
//###########################################################################
// Draw
//###########################################################################

/**************************************************************************
GeoSys-GUI: CGeoSysOUTView()
Programing:
12/2003 OK Implementation
05/2004 CC Modification
10/2004 OK LOAD_PATH_ANALYSIS
01/2006 OK OUT,MSH,PCS concept
02/2009 FS GHDB data view
**************************************************************************/
void CGeoSysOUTView::OnDraw(CDC* pDC)
{
//----------------------------------------------------------------------- 
	CMainFrame* m_frame = (CMainFrame*)AfxGetMainWnd();
	if(m_frame->GHDBvector_x.size()>0)
	  {
		vector<double> xx_vector;
		vector<double> yy_vector;
		
		int j=0;
		int i;
		j = (int)m_frame->GHDBvector_y.size();
		xx_vector.clear(); 
		yy_vector.clear();
		xx_vector.resize(m_frame->GHDBvector_x.size());
		yy_vector.resize(m_frame->GHDBvector_y.size());
		for(i=0;i<(int)m_frame->GHDBvector_x.size();i++)
			{
				xx_vector[i]=m_frame->GHDBvector_x[i];
				yy_vector[i]=m_frame->GHDBvector_y[i];
			}
		m_dYmin= *min_element(m_frame->GHDBvector_y.begin(),m_frame->GHDBvector_y.end());
		m_dYmax= *max_element(m_frame->GHDBvector_y.begin(),m_frame->GHDBvector_y.end());
		GetWindowAttributes(this->m_hWnd,&width,&height);
		pDC->TextOut(0,0,m_frame->GHDBViewTitle);
		CGraphics m_graphics;
		  m_graphics.width = width;
		  m_graphics.height = height;
		  m_graphics.m_dXmin = 1;
		  m_graphics.m_dXmax = j;
		  m_graphics.m_dDX = m_graphics.m_dXStep*(m_graphics.m_dXmax - m_graphics.m_dXmin);
		  m_graphics.m_dYmin = m_dYmin;
		  m_graphics.m_dYmax = m_dYmax;
		  m_graphics.m_dDY = m_graphics.m_dYStep*(m_graphics.m_dYmax - m_graphics.m_dYmin);
		  //m_graphics.m_strQuantityName = m_strQuantityName;
		  //-----------------------------------------------------------------------
		  // Axis
		  m_graphics.DrawGridAxes(pDC);
		  m_graphics.DrawCoordinateAxes(pDC);
		  //----------------------------------------------------------------------
		  // Draw x-y plot
		  int max_dim0 = max_dim;
		  max_dim = 0;
		  m_graphics.DrawXYPlot(pDC,xx_vector,yy_vector,0);
          max_dim = max_dim0; 
		}
		  //----------------------------------------------------------------------
		  // Tests
		  //......................................................................
else
{
  vector<double>x_vector;
  vector<double>y_vector;
  CGLPoint* m_pnt = GEOGetPointByName(m_out->geo_name);
  if(!m_pnt)
  {
    AfxMessageBox("Warning in CGeoSysOUTView::OnDraw - no GEO data");
    return;
  }
  //......................................................................
  CFEMesh* m_msh = m_out->GetMSH();
  if(!m_msh)
  {
    AfxMessageBox("Warning in CGeoSysOUTView::OnDraw - no MSH data");
    return;
  }
  //......................................................................
  CRFProcess* m_pcs = m_out->GetPCS((string)m_strQuantityName);
  if(!m_pcs)
  {
    AfxMessageBox("Warning in CGeoSysOUTView::OnDraw - no PCS data");
    return;
  }
  //......................................................................
  CTimeDiscretization* m_tim = NULL;
  m_tim = TIMGet(m_pcs->pcs_type_name);
  if(!m_tim){
    AfxMessageBox("Warning in CGeoSysOUTView::OnDraw - no TIM data");
    return;
  }
  //----------------------------------------------------------------------
  GetWindowAttributes(this->m_hWnd,&width,&height);
  //----------------------------------------------------------------------
  // View title
  CString m_strViewInfo = "Temporal Profiles in Observation Points: ";
  m_strViewInfo += m_out->nod_value_name.c_str();//m_strQuantityName;
  pDC->TextOut(0,0,m_strViewInfo);
  //----------------------------------------------------------------------
  // OUT->GEO->MSH (x data)
  if(aktuelle_zeit==0.0)
    x_vector.clear(); 
  x_vector.push_back(aktuelle_zeit);
  //----------------------------------------------------------------------
  // OUT->GEO->MSH (y data)
  if(aktuelle_zeit==0.0)
    y_vector.clear(); //OK
  double nod_value;
  int nidx1;
  nidx1 = m_pcs->GetNodeValueIndex(m_out->nod_value_name);
  if(nidx1<0)
  {
    AfxMessageBox("Warning in CGeoSysOUTView::OnDraw - no PCS data");
    return;
  }
  nidx1++; // new time
  long msh_node_number;
  msh_node_number = m_msh->GetNODOnPNT(m_pnt);
  nod_value = m_pcs->GetNodeValue(msh_node_number,nidx1);
  y_vector.push_back(nod_value);
  //-----------------------------------------------------------------------
  // X-Y-Range
  CGraphics m_graphics;
  m_graphics.width = width;
  m_graphics.height = height;
  m_graphics.m_dXmin = m_tim->time_start;
  m_graphics.m_dXmax = m_tim->time_end;
  m_graphics.m_dDX = m_graphics.m_dXStep*(m_graphics.m_dXmax - m_graphics.m_dXmin);
  m_graphics.m_dYmin = m_dYmin;
  m_graphics.m_dYmax = m_dYmax;
  m_graphics.m_dDY = m_graphics.m_dYStep*(m_graphics.m_dYmax - m_graphics.m_dYmin);
  m_graphics.m_strQuantityName = m_strQuantityName;
  //-----------------------------------------------------------------------
  // Axis
  m_graphics.DrawGridAxes(pDC);
  m_graphics.DrawCoordinateAxes(pDC);
  //----------------------------------------------------------------------
  // Draw x-y plot
  m_graphics.DrawXYPlot(pDC,x_vector,y_vector,0);
  }
  //-----------------------------------------------------------------------
/*
  //-----------------------------------------------------------------------
  // Doc
  CGeoSysDoc* m_pDoc = GetDocument();
  CTimeDiscretization* m_out_tim = NULL;
  m_out_tim = m_pDoc->m_doc_tim;
  //-----------------------------------------------------------------------
  // Fluid properties
  if(m_iDisplayOUTProperties){
    if(m_strQuantityName=="LOAD_PATH_ANALYSIS"){
      double stressInv[3];
      stressInv[0] = stressInv[1] = stressInv[2] = 0.0;
      //WW_LOAD_PATH calculation on I/II min,max values
      //OK CalMaxiumStressInvariants(stressInv);
      if(stressInv[0]==0.0) stressInv[0] = 1.0; 
      if(stressInv[1]==0.0) stressInv[1] = 1.0; 
      m_dXmax = stressInv[0];
      m_dYmax = stressInv[1];
      m_graphics.m_dXmin = m_dXmin;
      m_graphics.m_dXmax = m_dXmax;
      m_graphics.m_dDX = m_graphics.m_dXStep*(m_dXmax - m_dXmin);
      m_graphics.m_dYmin = m_dYmin;
      m_graphics.m_dYmax = m_dYmax;
      m_graphics.m_dDY = m_graphics.m_dYStep*(m_dYmax - m_dYmin);
      m_graphics.DrawCoordinateAxes(pDC);
      m_graphics.DrawGridAxes(pDC);
      m_graphics.DrawLoadPathAnalysis(pDC);
    }
    else{
      m_graphics.DrawTemporalBreakthroughCurves(pDC,m_out_tim);
    }
  }
*/
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Drucken

BOOL CGeoSysOUTView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CGeoSysOUTView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Zus�tzliche Initialisierung vor dem Drucken hier einf�gen
}

void CGeoSysOUTView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Hier Bereinigungsarbeiten nach dem Drucken einf�gen
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Diagnose

#ifdef _DEBUG
void CGeoSysOUTView::AssertValid() const
{

	CView::AssertValid();
}

void CGeoSysOUTView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGeoSysDoc* CGeoSysOUTView::GetDocument() // Die endg�ltige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGeoSysDoc)));
	return (CGeoSysDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Nachrichten-Handler
//###########################################################################
// Menus
//###########################################################################


void CGeoSysOUTView::OnProperties()
{
  CDialogOUT m_dlg;
  m_dlg.DoModal();
  m_out = m_dlg.m_obj;
/*
  COutputObservation m_output;

  m_output.m_dXmin = m_dXmin;
  m_output.m_dXmax = m_dXmax;
  m_output.m_dYmin = m_dYmin;
  m_output.m_dYmax = m_dYmax;
  m_output.m_strQuantityName = m_strQuantityName;

  m_output.DoModal();

  m_dXmin = m_output.m_dXmin;
  m_dXmax = m_output.m_dXmax;
  m_dYmin = m_output.m_dYmin;
  m_dYmax = m_output.m_dYmax;
  m_strQuantityName = m_output.m_strQuantityName;
  m_iDisplayOUTProperties = 1;
*/
  Invalidate(TRUE);

}

//////////////////////////////////////////////////////////////////////
/////////////////// CC 05/2004 zoom function


/**************************************************************************
GeoSys-GUI: OnViewZoomin
Programing:User pressed the ZOOM IN toolbutton or menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysOUTView::OnViewZoomin()
{
	// Toggle zoomin mode
	CWnd *pWin = ((CWinApp *) AfxGetApp())->m_pMainWnd;
	if (GetZoomMode() == MODE_ZOOMIN) {
		SetZoomMode(MODE_ZOOMOFF);
		// Clear the statusbar
		pWin->SendMessage(WM_SETMESSAGESTRING, 0, 
			(LPARAM)(LPCSTR)"");
	} else {
		SetZoomMode(MODE_ZOOMIN);
		// Give instructions in the statusbar
		pWin->SendMessage(WM_SETMESSAGESTRING, 0, 
			(LPARAM)(LPCSTR)"Click to zoom in on point or drag a zoom box.");
	}
} // OnViewZoomin


/**************************************************************************
GeoSys-GUI: OnViewZoomout
Programing:User pressed the ZOOM OUT toolbutton or menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysOUTView::OnViewZoomout()
{
	// Toggle zoomout mode
	CWnd *pWin = ((CWinApp *) AfxGetApp())->m_pMainWnd;
	if (GetZoomMode() == MODE_ZOOMOUT) {
		SetZoomMode(MODE_ZOOMOFF);
		// Clear the statusbar
		pWin->SendMessage(WM_SETMESSAGESTRING, 0, 
			(LPARAM)(LPCSTR)"");
	} else {
		SetZoomMode(MODE_ZOOMOUT);
		// Give instructions in the statusbar
		pWin->SendMessage(WM_SETMESSAGESTRING, 0, 
			(LPARAM)(LPCSTR)"Click to zoom out on point.");
	}
} // OnViewZoomout


/**************************************************************************
GeoSys-GUI: OnViewZoomfull
Programing:User pressed the ZOOM FULL toolbutton or menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysOUTView::OnViewZoomfull()
{
	DoZoomFull();  // Call CGeoSysZoomView member to zoom full scale
} // OnViewZoomfull


/**************************************************************************
GeoSys-GUI: OnUpdateViewZoomin
Programing:Tell MFC whether to depress the button or check the menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysOUTView::OnUpdateViewZoomin(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetZoomMode() == MODE_ZOOMIN);
} // OnUpdateViewZoomin


/**************************************************************************
GeoSys-GUI: OnUpdateViewZoomout
Programing:Tell MFC whether to depress the button or check the menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysOUTView::OnUpdateViewZoomout(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetZoomMode() == MODE_ZOOMOUT);
}

