#pragma once

#include "CompositeCharacter.h"

enum SEQUENCE_EDITOR_MODE {
	semAdd,
	semEdit,
};

class CComposeSequenceEditor: public CDialog {
	DECLARE_DYNAMIC( CComposeSequenceEditor )
	DECLARE_MESSAGE_MAP( )

public:
	CComposeSequenceEditor( ComposeSequence& sequence, SEQUENCE_EDITOR_MODE mode, CWnd* pParent = nullptr );
	virtual ~CComposeSequenceEditor( );

	// Dialog Data
	enum { IDD = IDD_EDITKEYSEQUENCE };

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void DDX_Result( CDataExchange* pDX, int nIDC, CString& result );

	virtual BOOL OnInitDialog( );

private:
	ComposeSequence& m_sequence;
	SEQUENCE_EDITOR_MODE m_mode;
	CString m_strTitle;

	CompositeCharacter m_CompositeCharacter;

	CEdit m_editComposeSequence;
	CEdit m_editComposeResult;

	CButton m_radioResultAsCharacter;
	CButton m_radioResultAsHexCodePoint;
	CButton m_radioResultAsDecCodePoint;

	CButton m_checkEnabled;
	CButton m_checkCaseInsensitive;
	CButton m_checkReversible;

	CString m_strComposeSequence;
	CString m_strComposeResult;

	int m_nResultMode;

	BOOL m_fEnabled;
	BOOL m_fCaseInsensitive;
	BOOL m_fReversible;
};
