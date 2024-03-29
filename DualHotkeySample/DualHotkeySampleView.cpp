
// DualHotkeySampleView.cpp : implementation of the CDualHotkeySampleView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "DualHotkeySample.h"
#endif

#include "DualHotkeySampleDoc.h"
#include "DualHotkeySampleView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDualHotkeySampleView

IMPLEMENT_DYNCREATE(CDualHotkeySampleView, CView)

BEGIN_MESSAGE_MAP(CDualHotkeySampleView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDualHotkeySampleView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CDualHotkeySampleView construction/destruction

CDualHotkeySampleView::CDualHotkeySampleView()
{
	// TODO: add construction code here

}

CDualHotkeySampleView::~CDualHotkeySampleView()
{
}

BOOL CDualHotkeySampleView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDualHotkeySampleView drawing

void CDualHotkeySampleView::OnDraw(CDC* pDC)
{
	CDualHotkeySampleDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDC->SetTextColor(RGB(192, 64, 64));
	pDC->DrawText(_T("Try the following hotkeys:"), CRect(20, 10, 800, 40), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->DrawText(_T("Ctrl+M              New File"), CRect(20, 50, 800, 80), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->DrawText(_T("Ctrl+K, Ctrl+A      About"), CRect(20, 80, 800, 110), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->DrawText(_T("Ctrl+K, Ctrl+P      Print"), CRect(20, 110, 800, 140), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	pDC->DrawText(_T("Ctrl+K, Ctrl+M      Copy (Disabled, won't trigger. Please check status bar)"), CRect(20, 140, 800, 170), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	pDC->DrawText(_T("For MFC users, you may need to hack into the menus in order to display dual hotkeys."), CRect(20, 280, 800, 310), DT_LEFT | DT_SINGLELINE | DT_VCENTER);
}


// CDualHotkeySampleView printing


void CDualHotkeySampleView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CDualHotkeySampleView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDualHotkeySampleView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDualHotkeySampleView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CDualHotkeySampleView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDualHotkeySampleView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDualHotkeySampleView diagnostics

#ifdef _DEBUG
void CDualHotkeySampleView::AssertValid() const
{
	CView::AssertValid();
}

void CDualHotkeySampleView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDualHotkeySampleDoc* CDualHotkeySampleView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDualHotkeySampleDoc)));
	return (CDualHotkeySampleDoc*)m_pDocument;
}
#endif //_DEBUG


// CDualHotkeySampleView message handlers
