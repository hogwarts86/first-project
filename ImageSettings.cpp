// ImageSettings.cpp : implementation file
//

#include "stdafx.h"
#include "ImageSettings.h"
#include "afxdialogex.h"


// ImageSettings dialog

IMPLEMENT_DYNAMIC(ImageSettings, CDialog)

ImageSettings::ImageSettings(CWnd* pParent /*=NULL*/)
	: CDialog(ImageSettings::IDD, pParent)
	, m_shift(0)
	, m_amplification(0)
{

	m_offset = 0;
}

ImageSettings::~ImageSettings()
{
}

void ImageSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT1, m_offset);
	DDV_MinMaxInt(pDX, m_offset, -100, 100);
	DDX_Text(pDX, IDC_EDIT2, m_shift);
	DDV_MinMaxInt(pDX, m_shift, -100, 100);
	DDX_Text(pDX, IDC_EDIT3, m_amplification);
	DDV_MinMaxDouble(pDX, m_amplification, 0,3.0);
}


BEGIN_MESSAGE_MAP(ImageSettings, CDialog)

	ON_BN_CLICKED(IDOK, &ImageSettings::OnBnClickedOk)
END_MESSAGE_MAP()


// ImageSettings message handlers


int ImageSettings::getOffsets(void) const
{
	return m_offset;
}


int ImageSettings::getShift(void)
{
	return m_shift;
}


double ImageSettings::getAmplification(void)
{
	return m_amplification;
}



void ImageSettings::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
