// GeoSysMATView.cpp : Implementierung der Klasse CGeoSysView
//
#include "stdafx.h"
#include "GeoSys.h"
#include "GeoSysDoc.h"
#include "GeoSysMATView.h"
#include "OGLEnabledView.h"
#include "MainFrm.h"
// GUI
#include "gs_graphics.h"
#include "rf_p.h"
#include "gs_output.h"
// RF Objects
// Dialogs
#include "gs_mat_fp.h"
#include "gs_mat_sp.h"
// GeoLib
// MshLib
//zoom
#include "GeoSysZoomView.h"
#include "afxpriv.h"    // For WM_SETMESSAGESTRING

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGeoSysMATView

IMPLEMENT_DYNCREATE(CGeoSysMATView, CGeoSysZoomView)

BEGIN_MESSAGE_MAP(CGeoSysMATView, CGeoSysZoomView)
    ON_COMMAND(ID_VIEW_MAT_PROPERTIES, OnProperties)
    ON_COMMAND(ID_SOLIDPROPERTIES_DENSITY, OnSolidPropertiesDensity)
    ON_COMMAND(ID_MSP_EDITOR, OnMatSolidProperties)
    ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_COMMAND(ID_VIEW_ZOOMFULL, OnViewZoomfull)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeoSysMATView Konstruktion/Destruktion

/**************************************************************************
GeoSys-GUI: CGeoSysView()
Programing:
09/2003 OK Implementation
05/2005 OK modeless dialog
**************************************************************************/
CGeoSysMATView::CGeoSysMATView()
{
  m_iDisplayMATProperties = 0;
  m_dXmin = 0.0;
  m_dXmax = 1.0;
  m_dYmin = 0.0;
  m_dYmax = 1.0;
  m_iFluidPhase = 1;
//OK_MODELESS
  //----------------------------------------------------------------------
  m_dlg_modeless = new COUTMaterialProperties;
  m_dlg_modeless->Create(IDD_OUT_MAT);
  m_dlg_modeless->ShowWindow(SW_SHOW);
  //----------------------------------------------------------------------
}

CGeoSysMATView::~CGeoSysMATView()
{
//OK_MODELESS
  m_dlg_modeless->CloseWindow();
  delete m_dlg_modeless;
}

/**************************************************************************
GeoSys-GUI Function OnInitialUpdate
Programing:
05/2004 CC Implementation
09/2004 OK Properties Dialog
**************************************************************************/
void CGeoSysMATView::OnInitialUpdate()
{
  CGeoSysZoomView::OnInitialUpdate();
  CGeoSysDoc* pDoc = GetDocument();
  // Initialize the CGeoSysZoomView class
  SetZoomSizes(pDoc->GetDocSize());
  // Properties dialog
//OK  OnProperties();
	// Create the LOGICAL font to draw with (only once!)
	/*int height = (m_totalLog.cy / 10) / 5;  // 1/5 of box size
	m_font = new CFont();
   m_font->CreateFont(height, 0, 0, 0,
         FW_NORMAL, FALSE, FALSE,
         FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
         DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE, "Arial");*/
} // OnInitialUpdate

BOOL CGeoSysMATView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: �ndern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Zeichnen

/**************************************************************************
GeoSys-GUI: CGeoSysView()
Programing:
12/2003 OK Implementation
05/2004 CC Modification
05/2005 OK MMP
01/2009 OK MFP
**************************************************************************/
void CGeoSysMATView::OnDraw(CDC* pDC)
{
  GetWindowAttributes(this->m_hWnd,&width,&height);
  //-----------------------------------------------------------------------
  CString m_strViewTitle;
  m_strViewTitle = " Material function: ";
  m_strViewTitle += m_dlg_modeless->m_strMATPropertyName;
  m_strViewTitle += " -> ";
  m_strViewTitle += m_dlg_modeless->m_strMATGroupName;
  pDC->TextOut(0,0,m_strViewTitle);
  //----------------------------------------------------------------------
  m_dXmin = m_dlg_modeless->m_dXmin;
  m_dXmax = m_dlg_modeless->m_dXmax;
  m_dYmin = m_dlg_modeless->m_dUmin;
  m_dYmax = m_dlg_modeless->m_dUmax;
  //----------------------------------------------------------------------
  CGraphics m_graphics;
  m_graphics.width = width;
  m_graphics.height = height;
  m_graphics.m_dXmin = m_dXmin;
  m_graphics.m_dXmax = m_dXmax;
  m_graphics.m_dDX = m_graphics.m_dXStep*(m_graphics.m_dXmax - m_graphics.m_dXmin);
  m_graphics.m_dYmin = m_dYmin;
  m_graphics.m_dYmax = m_dYmax;
  m_graphics.m_dDY = m_graphics.m_dYStep*(m_graphics.m_dYmax - m_graphics.m_dYmin);
  //-----------------------------------------------------------------------
  // Koordinatenachsen zeichnen und beschriften
  m_graphics.DrawCoordinateAxes(pDC);
  m_graphics.DrawGridAxes(pDC);
  //-----------------------------------------------------------------------
  // Medium properties
  switch(m_dlg_modeless->m_iMATType)
  {
    case 0: //MMP
      if(!m_dlg_modeless->m_strMATPropertyName.IsEmpty())
      {
        m_graphics.DrawMATFunction(pDC,m_dlg_modeless->m_mmp,m_dlg_modeless->m_strMATPropertyName);
      }
      break;
    case 1: //MFP
        m_graphics.m_iMATType = m_dlg_modeless->m_iMATType;
        m_graphics.m_fct = m_dlg_modeless->m_fct;
        m_graphics.DrawMATFunction(pDC,m_dlg_modeless->m_mmp,m_dlg_modeless->m_strMATPropertyName);
      break;
  }
  //-----------------------------------------------------------------------
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Drucken

BOOL CGeoSysMATView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Standardvorbereitung
	return DoPreparePrinting(pInfo);
}

void CGeoSysMATView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Zus�tzliche Initialisierung vor dem Drucken hier einf�gen
}

void CGeoSysMATView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// ZU ERLEDIGEN: Hier Bereinigungsarbeiten nach dem Drucken einf�gen
}

/////////////////////////////////////////////////////////////////////////////
// CGeoSysView Diagnose

#ifdef _DEBUG
void CGeoSysMATView::AssertValid() const
{

	CView::AssertValid();
}

void CGeoSysMATView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGeoSysDoc* CGeoSysMATView::GetDocument() // Die endg�ltige (nicht zur Fehlersuche kompilierte) Version ist Inline
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

void CGeoSysMATView::OnProperties()
{
  COUTMaterialProperties m_out_materials;

  m_out_materials.m_dXmin = m_dXmin;
  m_out_materials.m_dXmax = m_dXmax;
  m_out_materials.m_dYmin = m_dYmin;
  m_out_materials.m_dYmax = m_dYmax;
  m_out_materials.m_iDisplayMATPropertiesType = m_iDisplayMATProperties;

  m_out_materials.DoModal();

  m_dXmin = m_out_materials.m_dXmin;
  m_dXmax = m_out_materials.m_dXmax;
  m_dYmin = m_out_materials.m_dYmin;
  m_dYmax = m_out_materials.m_dYmax;
  m_iDisplayMATProperties = m_out_materials.m_iDisplayMATPropertiesType;
  m_iFluidPhase = m_out_materials.m_iFluidPhase;

  Invalidate(TRUE);
}

void CGeoSysMATView::OnSolidPropertiesDensity()
{
  if(1) {
    AfxMessageBox("No PCS data ! Create in -> Simulator -> Processes");
    return;
  }
  CMaterialFluidDensity m_mat_fp_density;
  m_mat_fp_density.m_dDensityMax = m_dYmax;
  if (m_mat_fp_density.DoModal()==IDOK){
    m_dYmax = m_mat_fp_density.m_dDensityMax;
  }
}

/**************************************************************************
GeoSys-GUI: CGeoSysMATView()
Programing:
01/2004 WW Implementation
**************************************************************************/
void CGeoSysMATView::OnMatSolidProperties()
{
  MAT_Mech_dlg MMech_dlg;
  MMech_dlg.DoModal();
}

//#########################################################################
// Zoom
//#########################################################################

/**************************************************************************
GeoSys-GUI: OnViewZoomin
Programing:User pressed the ZOOM IN toolbutton or menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysMATView::OnViewZoomin()
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
void CGeoSysMATView::OnViewZoomout()
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
void CGeoSysMATView::OnViewZoomfull()
{
	DoZoomFull();  // Call CGeoSysZoomView member to zoom full scale
} // OnViewZoomfull


/**************************************************************************
GeoSys-GUI: OnUpdateViewZoomin
Programing:Tell MFC whether to depress the button or check the menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysMATView::OnUpdateViewZoomin(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetZoomMode() == MODE_ZOOMIN);
} // OnUpdateViewZoomin


/**************************************************************************
GeoSys-GUI: OnUpdateViewZoomout
Programing:Tell MFC whether to depress the button or check the menu
05/2004 CC Implementation
last modified: 
**************************************************************************/
void CGeoSysMATView::OnUpdateViewZoomout(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetZoomMode() == MODE_ZOOMOUT);
}

//#########################################################################
// Mouse Events
//#########################################################################

void CGeoSysMATView::OnMouseMove(UINT nFlags, CPoint point)
{
  Invalidate();
  CView::OnMouseMove(nFlags, point);
}
