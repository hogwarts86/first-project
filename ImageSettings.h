#pragma once

#include "resource.h"
#include <afxstr.h>
#include <atlimage.h>
// ImageSettings dialog
class ImageSettings : public CDialog
{
	DECLARE_DYNAMIC(ImageSettings)

public:
	ImageSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~ImageSettings();

// Dialog Data
	enum { IDD = IDD_IMAGESETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_offset;

public:
	int getOffsets(void) const;
	int getShift(void);
	double getAmplification(void);
	afx_msg void OnBnClickedOk();

private:
	// Shift buffered image
	int m_shift;
	double m_amplification;
};
