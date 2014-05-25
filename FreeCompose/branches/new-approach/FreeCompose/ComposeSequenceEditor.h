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
	enum { IDD = IDD_COMPOSE_SEQUENCE_EDITOR };

	// Results
	CArray<ComposeSequence> m_ComposeSequences;

protected:
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog( );
	virtual void OnOK( );
	virtual void OnOKAddAnother( );

	void OnDrawItem( int nID, LPDRAWITEMSTRUCT lpDrawItemStruct );
	void OnUpdateComposeSequence( );
	void OnUpdateComposeResult( );
	void OnResultModeClicked( UINT uID );
	void OnCheckboxClicked( UINT uID );

private:
	ComposeSequence& m_sequence;
	SEQUENCE_EDITOR_MODE m_mode;
	CString m_strResultInput;
	CFont* m_pFont;
	bool m_fModified;

	CompositeCharacter m_CompositeCharacter;

	CButton m_buttonAddAnother;
	CEdit   m_editComposeSequence;
	CEdit   m_editComposeResult;
	CStatic m_staticPreview;

	CString m_strComposeSequence;
	CString m_strComposeResult;
	int     m_nResultMode;
	BOOL    m_fEnabled;
	BOOL    m_fCaseInsensitive;
	BOOL    m_fReversible;

	bool _ParseCodePointList( UChar32 const* pqzInput, int const cchInput, int const base, UChar32*& pqzOutput, int& cchOutput );
	void _SetResultFromInput( void );
	void _SetInputFromResult( void );

	void _DDV_MinMaxCompositeCharacters( CDataExchange* pDX, unsigned const uID, CString const& strComposeSequence, int const cchMin, int const cchMax );

};
