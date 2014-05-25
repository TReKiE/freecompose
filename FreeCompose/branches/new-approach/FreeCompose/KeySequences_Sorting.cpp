#include "stdafx.h"

#include "KeySequences.h"

//
// CKeySequences static member variables
//

CKeySequences::sortcallbackfunc* CKeySequences::ResultColumnsSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Result,
	&CKeySequences::_ListComparer_Descending_Result,
};

CKeySequences::sortcallbackfunc* CKeySequences::SequenceColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Sequence,
	&CKeySequences::_ListComparer_Descending_Sequence,
};

CKeySequences::sortcallbackfunc* CKeySequences::EnabledColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Enabled,
	&CKeySequences::_ListComparer_Descending_Enabled,
};

CKeySequences::sortcallbackfunc* CKeySequences::CaseInsensitiveColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_CaseInsensitive,
	&CKeySequences::_ListComparer_Descending_CaseInsensitive,
};

CKeySequences::sortcallbackfunc* CKeySequences::ReversibleColumnSortFuncMap[] = {
	&CKeySequences::_ListComparer_Unsorted,
	&CKeySequences::_ListComparer_Ascending_Reversible,
	&CKeySequences::_ListComparer_Descending_Reversible,
};

CKeySequences::sortcallbackfunc** CKeySequences::ColumnSortFuncMap[] = {
	EnabledColumnSortFuncMap,
	ResultColumnsSortFuncMap,
	ResultColumnsSortFuncMap,
	SequenceColumnSortFuncMap,
	CaseInsensitiveColumnSortFuncMap,
	ReversibleColumnSortFuncMap,
};

int const CKeySequences::ColumnHeaderFormatFlagsMap[] = {
	0,
	HDF_SORTUP,
	HDF_SORTDOWN
};

//
// CKeySequences comparison functions, for the list control.
// Huh. Subtraction is so much easier, for certain kinds of comparisons.
//

int CKeySequences::_ListComparer_Unsorted( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	index1 = self->m_ListIndexMap[index1];
	index2 = self->m_ListIndexMap[index2];

	return static_cast<int>( index1 - index2 );
}

int CKeySequences::_ListComparer_Ascending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& result1 = self->_GetComposeSequenceFromListIndex( index1 ).Result;
	CString& result2 = self->_GetComposeSequenceFromListIndex( index2 ).Result;

	return result1.Compare( result2 );
}

int CKeySequences::_ListComparer_Descending_Result( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& result1 = self->_GetComposeSequenceFromListIndex( index1 ).Result;
	CString& result2 = self->_GetComposeSequenceFromListIndex( index2 ).Result;

	return result2.Compare( result1 );
}

int CKeySequences::_ListComparer_Ascending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& sequence1 = self->_GetComposeSequenceFromListIndex( index1 ).Sequence;
	CString& sequence2 = self->_GetComposeSequenceFromListIndex( index2 ).Sequence;

	return sequence1.CompareNoCase( sequence2 );
}

int CKeySequences::_ListComparer_Descending_Sequence( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	CString& sequence1 = self->_GetComposeSequenceFromListIndex( index1 ).Sequence;
	CString& sequence2 = self->_GetComposeSequenceFromListIndex( index2 ).Sequence;

	return sequence2.CompareNoCase( sequence1 );
}

int CKeySequences::_ListComparer_Ascending_Enabled( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool enabled1 = !self->_GetComposeSequenceFromListIndex( index1 ).Disabled;
	bool enabled2 = !self->_GetComposeSequenceFromListIndex( index2 ).Disabled;

	return enabled1 > enabled2;
}

int CKeySequences::_ListComparer_Descending_Enabled( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool enabled1 = !self->_GetComposeSequenceFromListIndex( index1 ).Disabled;
	bool enabled2 = !self->_GetComposeSequenceFromListIndex( index2 ).Disabled;

	return enabled2 > enabled1;
}

int CKeySequences::_ListComparer_Ascending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool caseInsensitive1 = !self->_GetComposeSequenceFromListIndex( index1 ).CaseInsensitive;
	bool caseInsensitive2 = !self->_GetComposeSequenceFromListIndex( index2 ).CaseInsensitive;

	return caseInsensitive1 > caseInsensitive2;
}

int CKeySequences::_ListComparer_Descending_CaseInsensitive( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool caseInsensitive1 = !self->_GetComposeSequenceFromListIndex( index1 ).CaseInsensitive;
	bool caseInsensitive2 = !self->_GetComposeSequenceFromListIndex( index2 ).CaseInsensitive;

	return caseInsensitive2 > caseInsensitive1;
}

int CKeySequences::_ListComparer_Ascending_Reversible( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool reversible1 = !self->_GetComposeSequenceFromListIndex( index1 ).Reversible;
	bool reversible2 = !self->_GetComposeSequenceFromListIndex( index2 ).Reversible;

	return reversible1 > reversible2;
}

int CKeySequences::_ListComparer_Descending_Reversible( LPARAM index1, LPARAM index2, LPARAM lparamSelf ) {
	CKeySequences* self = reinterpret_cast<CKeySequences*>( lparamSelf );
	bool reversible1 = !self->_GetComposeSequenceFromListIndex( index1 ).Reversible;
	bool reversible2 = !self->_GetComposeSequenceFromListIndex( index2 ).Reversible;

	return reversible2 > reversible1;
}
