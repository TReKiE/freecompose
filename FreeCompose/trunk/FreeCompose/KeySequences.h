#pragma once


// CKeySequences dialog

class CKeySequences : public CPropertyPage
{
	DECLARE_DYNAMIC(CKeySequences)

public:
	CKeySequences();
	virtual ~CKeySequences();

// Dialog Data
	enum { IDD = IDD_KEYSEQUENCES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
