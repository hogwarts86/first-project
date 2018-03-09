// This is a part of the Microsoft Foundation Classes C++ library. 
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "CImage.h"
#include "ChildView.h"
#include "InfoDlg.h"
#include "ImageSettings.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CChildView

CChildView::CChildView()
{
	m_nImageSize = SIZE_NONE;
	m_offset = 10;
	m_shift = 10;
	m_amplification = 1;
	isEnhanced = false;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPENIMAGE, OnFileOpenimage)
	ON_COMMAND(ID_FILE_SAVEIMAGE, OnFileSaveImage)
	ON_COMMAND(ID_TOOLS_MAKEBW, OnToolsMakeBW)
	ON_COMMAND(ID_FILE_IMAGEINFO, OnImageInfo)
	ON_COMMAND(ID_FILE_INPUT,&CChildView::OnFileInput)
	ON_UPDATE_COMMAND_UI(ID_SIZE_HALF,OnUpdateSizeHalf)
	ON_UPDATE_COMMAND_UI(ID_SIZE_ORIGINAL,OnUpdateSizeOriginal)
	ON_UPDATE_COMMAND_UI(ID_SIZE_DOUBLE,OnUpdateSizeDouble)
	ON_UPDATE_COMMAND_UI(ID_SIZE_FILL,OnUpdateSizeFill)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEIMAGE,OnUpdateImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMAGEINFO,OnUpdateImage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MAKEBW,OnUpdateImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_INPUT,OnUpdateImage)
	ON_COMMAND_RANGE(ID_SIZE_HALF,ID_SIZE_FILL,OnChangeSize)
	
	
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (!imgOriginal.IsNull()) 
	{
		//Enhance();
		switch (m_nImageSize)
		{
			case SIZE_HALF:
				imgOriginal.StretchBlt(dc,0,0,imgOriginal.GetWidth()/2,imgOriginal.GetHeight()/2,SRCCOPY);
				break;
			case SIZE_ORIGINAL:
				imgOriginal.StretchBlt(dc,0,0,imgOriginal.GetWidth(),imgOriginal.GetHeight(),SRCCOPY);
				break;
			case SIZE_DOUBLE:
				imgOriginal.StretchBlt(dc,0,0,imgOriginal.GetWidth()*2,imgOriginal.GetHeight()*2,SRCCOPY);
				break;
			case SIZE_FILL:
				CRect rctWindowSize;
				GetClientRect(rctWindowSize);
				imgOriginal.StretchBlt(dc,0,0,rctWindowSize.Width(),rctWindowSize.Height(),SRCCOPY);
		};
	}

}


void CChildView::OnFileOpenimage(void)
{
	// TODO: Add your command handler code here
	CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;

	hResult = imgOriginal.GetExporterFilterString(strFilter,aguidFileTypes);
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("GetExporterFilter failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter);
	dlg.m_ofn.nFilterIndex = m_nFilterLoad;
	hResult = (int)dlg.DoModal();
	if(FAILED(hResult) || (hResult == IDCANCEL)) {
		return;
	}

	m_nFilterLoad = dlg.m_ofn.nFilterIndex;
	imageOriginal.empty();
	imgOriginal.Destroy();

	hResult = imgOriginal.Load(dlg.GetPathName());
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("Load image failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}
	int gpp = imgOriginal.GetBPP();
	if( gpp != 8){
		CString fmt;
		fmt.Format("Load image failed:\n %s has %i BPP",dlg.GetFileName(),gpp);
		::AfxMessageBox(fmt);
		return;
	}
	imageOriginal = cvLoadImage(dlg.GetPathName(),0);

	int value = (int)imageOriginal.at<uchar>(2,2);
	COLORREF pixel = (int)imgOriginal.GetPixel(2,2);
	int r,g,b;
	r = GetRValue(pixel);
	g = GetGValue(pixel);
	b = GetBValue(pixel);
	int maxColor = imgOriginal.GetMaxColorTableEntries();
	m_nImageSize = SIZE_ORIGINAL;
	Invalidate();
	UpdateWindow();

}

void CChildView::OnFileSaveImage(void) 
{
	CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;


	strFilter = "Bitmap image|*.bmp|JPEG image|*.jpg|GIF image|*.gif|PNG image|*.png||";

	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,strFilter);
	dlg.m_ofn.nFilterIndex = m_nFilterLoad;
	hResult = (int)dlg.DoModal();
	if (FAILED(hResult)) {
		return;
	}

    // Add the appropriate extension if the user didn't type one

	CString strFileName;
	CString strExtension;
	CString strPath;
	strPath = dlg.GetFolderPath();
	strFileName = dlg.m_ofn.lpstrFile;

	strExtension = dlg.GetFileExt();
	// add the file extension if the user didn't supply one
	if (dlg.GetFileExt().IsEmpty()) 
	{
		switch (dlg.m_ofn.nFilterIndex)
		{
		case 1:
			strExtension = "bmp";
			break;
		case 2:
			strExtension = "jpg";
			break;
		case 3:
			strExtension = "gif";
			break;
		case 4:
			strExtension = "png";
			break;
		default:
			break;
		}

		strFileName = strFileName + '.' + strExtension;

	}

	// the extension on the file name will determine the file type that is saved
	hResult = imgOriginal.Save(strFileName);
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("Save image failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}

}

void CChildView::OnToolsMakeBW(void)
{

	CWaitCursor wait;

	if (!imgOriginal.IsIndexed()) {

		// the image does not use an indexed palette, so we will change each pixel to B&W (slow)
		COLORREF pixel;
		int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();
		byte r,g,b,avg;
		for (int x=0; x<maxX; x++) {
			for (int y=0; y<maxY; y++) {
				pixel = imgOriginal.GetPixel(x,y);
				r = GetRValue(pixel);
				g = GetGValue(pixel);
				b = GetBValue(pixel);
				avg = ((r + g + b)/3);
				imgOriginal.SetPixelRGB(x,y,avg,avg,avg);
			}
		}

	} else {

		// the image uses an indexed palette, so we will just change the palette table entries to
		// their B&W equivalents 
		int MaxColors = imgOriginal.GetMaxColorTableEntries();
		RGBQUAD* ColorTable;
		ColorTable = new RGBQUAD[MaxColors];
		
		imgOriginal.GetColorTable(0,MaxColors,ColorTable);
		for (int i=0; i<MaxColors; i++)
		{
			int avg = (ColorTable[i].rgbBlue + ColorTable[i].rgbGreen + ColorTable[i].rgbRed)/3;
			ColorTable[i].rgbBlue = (BYTE) avg ; 
			ColorTable[i].rgbGreen = (BYTE) avg;
			ColorTable[i].rgbRed = (BYTE) avg ;
		}
		imgOriginal.SetColorTable(0,MaxColors,ColorTable);
	
		delete[] ColorTable;
	}

	Invalidate();
	UpdateWindow();
}


void CChildView::OnImageInfo()
{
	CInfoDlg infoDlg(imgOriginal);
	infoDlg.DoModal();
}


void CChildView::OnChangeSize(UINT nID)
{
	m_nImageSize = (SizesEnum)(nID - ID_SIZE_BASE);
	Invalidate();
	UpdateWindow();
}

void CChildView::OnUpdateSizeHalf(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_HALF));
}

void CChildView::OnUpdateSizeOriginal(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_ORIGINAL));
}

void CChildView::OnUpdateSizeDouble(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_DOUBLE));
}

void CChildView::OnUpdateSizeFill(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_FILL));
}

void CChildView::OnUpdateImage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
}


//======================================== this function to give the 4 input parameters ============================
//Offset, Shift, Gain
void CChildView::OnFileInput()
{

	ImageSettings settingDlg(this);
	HRESULT hResult;
	hResult = (int)settingDlg.DoModal();
	if(FAILED(hResult) || hResult == IDCANCEL ){
		return;
	}
	m_offset = settingDlg.getOffsets();
	m_shift  = settingDlg.getShift();
	m_amplification = settingDlg.getAmplification();
	Enhance();
	// TODO: Add your command handler code here
}


void CChildView::Enhance(void)
{
	CWaitCursor wait;

	//======== 1. Copy Original Image into a buffered image ===============================
	imageBuffer = imageOriginal.clone();
	imshow("bufferedImage",imageBuffer);

	//=======  2. Offset the original image ===============================================
	//This functions commonly used in multiplication using alpha (amplification factor)
	//and beta as addition factor for contrast and brighness adjustment (color image)
	// the formula is f(x) = alpha * g(x) + beta
	// alpha = gain factor
	// beta	 = bias
	imageOriginal.convertTo(imageOriginal,-1,1.0,m_offset);
	
	imshow("Offset Source",imageOriginal);
	//Subtract Original image with vertically Shifted buffered Image
	
	//=================== Shift buffered image using m_shift variable (with zero padding) =========================
	Mat out = Mat::zeros(imageBuffer.size(),imageBuffer.type());
	if( m_shift != 0){
			if(m_shift < 0){
				//shift-down
				m_shift = abs(m_shift);
				imageBuffer(cv::Rect(0, 0, imageBuffer.cols, imageBuffer.rows - m_shift)).copyTo(out(cv::Rect(0, m_shift, imageBuffer.cols, imageBuffer.rows - m_shift)));
			}else{
				//shift-up
				imageBuffer(cv::Rect(0, m_shift, imageBuffer.cols, imageBuffer.rows - m_shift)).copyTo(out(cv::Rect(0,0, imageBuffer.cols, imageBuffer.rows - m_shift))); 
			}
			//copy the shifted image from out to buffered image
			imageBuffer = out;
	}
	imshow("Buffered image with shift",imageBuffer);
	//============== 3.Subtract with shifted buffered image===============================================
	//imageOriginal = imageOriginal - imageBuffer;
	subtract(imageOriginal,imageBuffer,imageOriginal);
	imshow("Subtracted image",imageOriginal);



	//============== 4. Apply amplification of resulted image (in this code is imageOriginal)=============
	imageOriginal.convertTo(imageOriginal,-1,m_amplification,0);
	imshow("Result with amplification",imageOriginal);

	//============== 5. To show in main window ============================================================
		// the image does not use an indexed palette, so we will change each pixel to B&W (slow)
		int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();
		byte r,g,b;
		for (int x=0; x<maxX; x++) {
			for (int y=0; y<maxY; y++) {
				r = (byte)imageOriginal.at<uchar>(y,x);
				g = (byte)imageOriginal.at<uchar>(y,x);
				b = (byte)imageOriginal.at<uchar>(y,x);
				imgOriginal.SetPixelRGB(x,y,r,g,b);
			}
		}
	Invalidate();
	UpdateWindow();
}
