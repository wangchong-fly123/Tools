// Markup.cpp: implementation of the CMarkup class.
//
// Markup Release 6.3
// Copyright (C) 1999-2002 First Objective Software, Inc. All rights reserved
// Go to www.firstobject.com for the latest CMarkup and EDOM documentation
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#include "stdafx.h"
#include "afxconv.h"
#include "Markup.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _MBCS
#pragma message( "Note: MBCS build (not UTF-8)" )
// For UTF-8, remove _MBCS from project settings C/C++ preprocessor definitions
#endif

// Defines for Windows CE
#ifndef _tclen
#define _tclen(p) 1
#define _tccpy(p1,p2) *(p1)=*(p2)
#endif

//[CMARKUPDEV
// To add a version tag to new documents, define MARKUP_VERSIONTAG
#ifdef MARKUP_VERSIONTAG
#define x_VERSIONTAG _T("<?xml version=\"1.0\"?>\r\n")
#endif
//]CMARKUPDEV


void CMarkup::operator=( const CMarkup& markup )
{
	m_iPosParent = markup.m_iPosParent;
	m_iPos = markup.m_iPos;
	m_iPosChild = markup.m_iPosChild;
	m_iPosFree = markup.m_iPosFree;
	m_nNodeType = markup.m_nNodeType;
	//[CMARKUPDEV
	m_nNodeOffset = markup.m_nNodeOffset;
	m_nNodeLength = markup.m_nNodeLength;
	//]CMARKUPDEV
	m_aPos.RemoveAll();
	m_aPos.Append( markup.m_aPos );
	m_csDoc = markup.m_csDoc;
	MARKUP_SETDEBUGSTATE;
}

bool CMarkup::SetDoc( LPCTSTR szDoc )
{
	// Reset indexes
	m_iPosFree = 1;
	ResetPos();
	m_mapSavedPos.RemoveAll();

	// Set document text
	if ( szDoc )
		m_csDoc = szDoc;
	else
		m_csDoc.Empty();

	// Starting size of position array: 1 element per 64 bytes of document
	// Tight fit when parsing small doc, only 0 to 2 reallocs when parsing large doc
	// Start at 8 when creating new document
	int nStartSize = m_csDoc.GetLength() / 64 + 8;
	if ( m_aPos.GetSize() < nStartSize )
		m_aPos.SetSize( nStartSize );

	// Parse document
	bool bWellFormed = false;
	if ( m_csDoc.GetLength() )
	{
		m_aPos[0].Clear();
		int iPos = x_ParseElem( 0 );
		if ( iPos > 0 )
		{
			m_aPos[0].iElemChild = iPos;
			bWellFormed = true;
		}
	}

	// Clear indexes if parse failed or empty document
	if ( ! bWellFormed )
	{
		m_aPos[0].Clear();
		m_iPosFree = 1;
	}

	ResetPos();
	return bWellFormed;
};

bool CMarkup::IsWellFormed()
{
	if ( m_aPos.GetSize() && m_aPos[0].iElemChild )
		return true;
	return false;
}

bool CMarkup::Load( LPCTSTR szFileName )
{
	CString csDoc;
	CFile file;
	if ( ! file.Open(szFileName,CFile::modeRead) )
		return false;
	int nLength = file.GetLength();

#if defined(_UNICODE)
	// Allocate Buffer for UTF-8 file data
	unsigned char* pBuffer = new unsigned char[nLength + 1];
	nLength = file.Read( pBuffer, nLength );
	pBuffer[nLength] = '\0';

	// Convert file from UTF-8 to Windows UNICODE (AKA UCS-2)
	int nWideLength = MultiByteToWideChar(CP_UTF8,0,(const char*)pBuffer,nLength,NULL,0);
	nLength = MultiByteToWideChar(CP_UTF8,0,(const char*)pBuffer,nLength,
		csDoc.GetBuffer(nWideLength),nWideLength);
	ASSERT( nLength == nWideLength );
	delete [] pBuffer;
#else
	nLength = file.Read( csDoc.GetBuffer(nLength), nLength );
#endif
	csDoc.ReleaseBuffer(nLength);
	file.Close();

	return SetDoc( csDoc );
}

bool CMarkup::Save( LPCTSTR szFileName )
{
	int nLength = m_csDoc.GetLength();
	CFile file;
	if ( ! file.Open(szFileName,CFile::modeWrite|CFile::modeCreate) )
		return false;
#if defined( _UNICODE )
	int nUTF8Len = WideCharToMultiByte(CP_UTF8,0,m_csDoc,nLength,NULL,0,NULL,NULL);
	char* pBuffer = new char[nUTF8Len+1];
	nLength = WideCharToMultiByte(CP_UTF8,0,m_csDoc,nLength,pBuffer,nUTF8Len+1,NULL,NULL);
	file.Write( pBuffer, nLength );
	delete pBuffer;
#else
	file.Write( (LPCTSTR)m_csDoc, nLength );
#endif
	file.Close();
	return true;
}

bool CMarkup::FindElem( LPCTSTR szName )
{
	// Change current position only if found
	//
	if ( m_aPos.GetSize() )
	{
		int iPos = x_FindElem( m_iPosParent, m_iPos, szName );
		if ( iPos )
		{
			// Assign new position
			x_SetPos( m_aPos[iPos].iElemParent, iPos, 0 );
			return true;
		}
	}
	return false;
}

bool CMarkup::FindChildElem( LPCTSTR szName )
{
	// Change current child position only if found
	//
	// Shorthand: call this with no current main position
	// means find child under root element
	if ( ! m_iPos )
		FindElem();

	int iPosChild = x_FindElem( m_iPos, m_iPosChild, szName );
	if ( iPosChild )
	{
		// Assign new position
		int iPos = m_aPos[iPosChild].iElemParent;
		x_SetPos( m_aPos[iPos].iElemParent, iPos, iPosChild );
		return true;
	}

	return false;
}

//[CMARKUPDEV
int CMarkup::FindNode( int nType )
{
	// Change current node position only if a node is found
	// If nType is 0 find any node, otherwise find node of type nType
	// Return type of node or 0 if not found
	// If found node is an element, change m_iPos

	// Determine where in document to start scanning for node
	int nTypeFound = 0;
	int nNodeOffset = m_nNodeOffset;
	if ( m_nNodeType > 1 )
	{
		// By-pass current node
		nNodeOffset += m_nNodeLength;
	}
	else
	{
		// Set position to begin looking for node
		nNodeOffset = 0; // default to start of document
		if ( m_iPos )
		{
			// After element
			nNodeOffset = m_aPos[m_iPos].nEndR + 1;
		}
		else
		{
			// Immediately after start tag of parent
			if ( m_aPos[m_iPosParent].IsEmptyElement() )
				return 0;
			if ( m_iPosParent )
				nNodeOffset = m_aPos[m_iPosParent].nStartR + 1;
		}
	}

	// Get nodes until we find what we're looking for
	int iPosNew = m_iPos;
	TokenPos token( m_csDoc );
	token.nNext = nNodeOffset;
	do
	{
		nNodeOffset = token.nNext;
		nTypeFound = x_ParseNode( token );
		if ( ! nTypeFound )
			return 0;
		if ( nTypeFound == MNT_ELEMENT )
		{
			if ( iPosNew )
				iPosNew = m_aPos[iPosNew].iElemNext;
			else
				iPosNew = m_aPos[m_iPosParent].iElemChild;
			if ( ! iPosNew )
				return 0;
			if ( ! nType || (nType & nTypeFound) )
			{
				// Found node, move position to this element
				x_SetPos( m_iPosParent, iPosNew, 0 );
				return m_nNodeType;
			}
			token.nNext = m_aPos[iPosNew].nEndR + 1;
		}
	}
	while ( nType && ! (nType & nTypeFound) );

	m_iPos = iPosNew;
	m_iPosChild = 0;
	m_nNodeOffset = nNodeOffset;
	m_nNodeLength = token.nNext - nNodeOffset;
	m_nNodeType = nTypeFound;
	MARKUP_SETDEBUGSTATE;
	return m_nNodeType;
}

bool CMarkup::RemoveNode()
{
	if ( m_iPos || m_nNodeLength )
	{
		x_RemoveNode( m_iPosParent, m_iPos, m_nNodeType, m_nNodeOffset, m_nNodeLength );
		m_iPosChild = 0;
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	return false;
}
//]CMARKUPDEV

CString CMarkup::GetTagName() const
{
	// Return the tag name at the current main position
	CString csTagName;

	//[CMARKUPDEV
	// This method is primarily for elements, however
	// it does return something for certain other nodes
	if ( m_nNodeLength )
	{
		switch ( m_nNodeType )
		{
		case MNT_PROCESSING_INSTRUCTION:
			{
				// Processing instruction returns target
				// For the document version it returns "xml"
				TokenPos token( m_csDoc );
				token.nNext = m_nNodeOffset + 2;
				if ( x_FindToken(token) )
					csTagName = x_GetToken( token );
			}
			break;
		case MNT_COMMENT:
			csTagName = _T("#comment");
			break;
		case MNT_CDATA_SECTION:
			csTagName = _T("#cdata-section");
			break;
		case MNT_DOCUMENT_TYPE:
			{
				// Document type returns document type name
				TokenPos token( m_csDoc );
				token.nNext = m_nNodeOffset + 2;
				// Find second token
				if ( x_FindToken(token) && x_FindToken(token) )
					csTagName = x_GetToken( token );
			}
			break;
		case MNT_TEXT:
		case MNT_WHITESPACE:
			csTagName = _T("#text");
			break;
		}
		return csTagName;
	}
	//]CMARKUPDEV

	if ( m_iPos )
		csTagName = x_GetTagName( m_iPos );
	return csTagName;
}

bool CMarkup::IntoElem()
{
	// If there is no child position and IntoElem is called it will succeed in release 6.3
	// (A subsequent call to FindElem will find the first element)
	// The following short-hand behavior was never part of EDOM and was misleading
	// It would find a child element if there was no current child element position and go into it
	// It is removed in release 6.3, this change is NOT backwards compatible!
	// if ( ! m_iPosChild )
	//	FindChildElem();

	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		x_SetPos( m_iPos, m_iPosChild, 0 );
		return true;
	}
	return false;
}

bool CMarkup::OutOfElem()
{
	// Go to parent element
	if ( m_iPosParent )
	{
		x_SetPos( m_aPos[m_iPosParent].iElemParent, m_iPosParent, m_iPos );
		return true;
	}
	return false;
}

CString CMarkup::GetAttribName( int n ) const
{
	// Return nth attribute name of main position
	if ( ! m_iPos || m_nNodeType != MNT_ELEMENT )
		return _T("");

	TokenPos token( m_csDoc );
	token.nNext = m_aPos[m_iPos].nStartL + 1;
	for ( int nAttrib=0; nAttrib<=n; ++nAttrib )
		if ( ! x_FindAttrib(token) )
			return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::SavePos( LPCTSTR szPosName )
{
	// Save current element position in saved position map
	if ( szPosName )
	{
		SavedPos savedpos;
		savedpos.iPosParent = m_iPosParent;
		savedpos.iPos = m_iPos;
		savedpos.iPosChild = m_iPosChild;
		m_mapSavedPos.SetAt( szPosName, savedpos );
		return true;
	}
	return false;
}

bool CMarkup::RestorePos( LPCTSTR szPosName )
{
	// Restore element position if found in saved position map
	SavedPos savedpos;
	if ( szPosName && m_mapSavedPos.Lookup( szPosName, savedpos ) )
	{
		x_SetPos( savedpos.iPosParent, savedpos.iPos, savedpos.iPosChild );
		return true;
	}
	return false;
}

bool CMarkup::GetOffsets( int& nStart, int& nEnd ) const
{
	// Return document offsets of current main position element
	// This is not part of EDOM but is used by the Markup project
	if ( m_iPos )
	{
		nStart = m_aPos[m_iPos].nStartL;
		nEnd = m_aPos[m_iPos].nEndR;
		return true;
	}
	return false;
}

CString CMarkup::GetChildSubDoc() const
{
	if ( m_iPosChild )
	{
		int nL = m_aPos[m_iPosChild].nStartL;
		int nR = m_aPos[m_iPosChild].nEndR + 1;
		TokenPos token( m_csDoc );
		token.nNext = nR;
		if ( ! x_FindToken(token) || m_csDoc[token.nL] == _T('<') )
			nR = token.nL;
		return m_csDoc.Mid( nL, nR - nL );
	}
	return _T("");
}

bool CMarkup::RemoveElem()
{
	// Remove current main position element
	if ( m_iPos && m_nNodeType == MNT_ELEMENT )
	{
		int iPos = x_RemoveElem( m_iPos );
		x_SetPos( m_iPosParent, iPos, 0 );
		return true;
	}
	return false;
}

bool CMarkup::RemoveChildElem()
{
	// Remove current child position element
	if ( m_iPosChild )
	{
		int iPosChild = x_RemoveElem( m_iPosChild );
		x_SetPos( m_iPosParent, m_iPos, iPosChild );
		return true;
	}
	return false;
}

//[CMARKUPDEV
CString CMarkup::FindGetData( LPCTSTR szName )
{
	if ( FindElem(szName) )
		return x_GetData( m_iPos );
	return _T("");
}

bool CMarkup::FindSetData( LPCTSTR szName, LPCTSTR szData, int nCDATA )
{
	if ( FindElem(szName) )
		return x_SetData( m_iPos, szData, nCDATA );
	return false;
}

// Base64 methods
CString CMarkup::EncodeBase64( const BYTE* pBuffer, int nBufferLen )
{
	// Return Base64 string
	// 1 byte takes 2, 2:3, 3:4, 4:6, 5:7, 6:8, 7:10....
	int nLenBase64 =  nBufferLen + nBufferLen/3 + 3;
	nLenBase64 += (nLenBase64 / 64) * 2 + 4; // CRLFs
	CString csBase64;
	LPTSTR pBase64 = csBase64.GetBuffer( nLenBase64 );

	// Start with CRLF
	int nOffsetBase64 = 0;
	pBase64[nOffsetBase64++] = _T('\r');
	pBase64[nOffsetBase64++] = _T('\n');

	// Loop through pBuffer 3 bytes at a time
	LPCTSTR pCodes = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	int nOffsetBuffer = 0;
	UINT n3BufferBytes;
	int nTop;
	int nPad = 0;
	while ( nOffsetBuffer < nBufferLen )
	{
		// Set n3BufferBytes
		n3BufferBytes = pBuffer[nOffsetBuffer++];
		n3BufferBytes = n3BufferBytes << 8;
		if ( nOffsetBuffer < nBufferLen )
			n3BufferBytes |= pBuffer[nOffsetBuffer++];
		else
			++nPad;
		n3BufferBytes = n3BufferBytes << 8;
		if ( nOffsetBuffer < nBufferLen )
			n3BufferBytes |= pBuffer[nOffsetBuffer++];
		else
			++nPad;

		// Fill pBase64
		nTop = nOffsetBase64 + 4;
		while ( nTop != nOffsetBase64 )
		{
			--nTop;
			if ( nPad == 0 || nTop < nOffsetBase64 + 4 - nPad )
				pBase64[nTop] = pCodes[n3BufferBytes & 0x3f];
			else
				pBase64[nTop] = (_TCHAR)_T('=');
			n3BufferBytes = n3BufferBytes >> 6;
		}
		nOffsetBase64 += 4;

		if ( ! (nOffsetBase64 % 66) || nPad )
		{
			pBase64[nOffsetBase64++] = _T('\r');
			pBase64[nOffsetBase64++] = _T('\n');
		}
	}

	csBase64.ReleaseBuffer( nOffsetBase64 );
	return csBase64;
}

int CMarkup::DecodeBase64( const CString& csBase64, BYTE* pBuffer, int nBufferLen )
{
	// Return length of binary buffer after decoding
	// If nBufferLen is 0, returns long enough length without decoding
	// so that you can allocate a large enough buffer
	// 1 byte takes 2, 2:3, 3:4, 4:6, 5:7, 6:8, 7:10....
	// Otherwise, returns actual decoded length as long as it fits inside nBufferLen
	int nLenBase64 = csBase64.GetLength();
	if ( ! nBufferLen )
		return (nLenBase64/4)*3 + 3;

	// Loop through Base64, 4 bytes at a time
	const BYTE* pBase64 = (const BYTE*)(LPCTSTR)csBase64;
	int nOffsetBase64 = 0;
	int nOffsetBuffer = 0;
	UINT n3BufferBytes;
	int nTop;
	int nCount;
	int nCode;
	int nPad = 0;
	while ( nOffsetBase64 < nLenBase64 )
	{
		// Set n3BufferBytes
		nCount = 0;
		n3BufferBytes = 0;
		while ( nCount != 4 && nOffsetBase64 < nLenBase64 )
		{
			// Check encoded character and bypass if whitespace
			nCode = pBase64[nOffsetBase64++];
			if ( nCode == _T('\r') || nCode == _T('\n') )
				continue;

			// Shift previous bits up before OR-ing 6 lower sig bits
			n3BufferBytes = n3BufferBytes << 6;

			// Use ASCII codes for fast conversion
			if ( nCode > 96 ) //(a-z)
				n3BufferBytes |= (nCode-71);
			else if ( nCode > 64 ) //(A-Z)
				n3BufferBytes |= (nCode-65);
			else if ( nCode == 61 ) //(=)
				++nPad;
			else if ( nCode > 47 ) //(0-9)
				n3BufferBytes |= (nCode+4);
			else if ( nCode == 47 ) //(/)
				n3BufferBytes |= 63;
			else if ( nCode == 43 ) //(+)
				n3BufferBytes |= 62;
			++nCount;
		}

		if ( nCount == 4 )
		{
			// Fill pBuffer
			nTop = nOffsetBuffer + 3;
			if ( nTop - nPad > nBufferLen )
				return 0;
			while ( nTop != nOffsetBuffer )
			{
				--nTop;
				if ( nPad == 0 || nTop < nOffsetBuffer+3-nPad )
					pBuffer[nTop] = (BYTE)(n3BufferBytes & 0xff);
				n3BufferBytes = n3BufferBytes >> 8;
			}
			nOffsetBuffer += 3-nPad;
		}
	}

	return nOffsetBuffer;
}
//]CMARKUPDEV

//////////////////////////////////////////////////////////////////////
// Private Methods
//////////////////////////////////////////////////////////////////////

int CMarkup::x_GetFreePos()
{
	//
	// This returns the index of the next unused ElemPos in the array
	//
	if ( m_iPosFree == m_aPos.GetSize() )
		m_aPos.SetSize( m_iPosFree + m_iPosFree / 2 );
	++m_iPosFree;
	return m_iPosFree - 1;
}

int CMarkup::x_ReleasePos()
{
	//
	// This decrements the index of the next unused ElemPos in the array
	// allowing the element index returned by GetFreePos() to be reused
	//
	--m_iPosFree;
	return 0;
}

int CMarkup::x_ParseError( LPCTSTR szError, LPCTSTR szName )
{
	if ( szName )
		m_csError.Format( szError, szName );
	else
		m_csError = szError;
	x_ReleasePos();
	return -1;
}

int CMarkup::x_ParseElem( int iPosParent )
{
	// This is either called by SetDoc, x_AddSubDoc, or itself recursively
	// m_aPos[iPosParent].nEndL is where to start parsing for the child element
	// This returns the new position if a tag is found, otherwise zero
	// In all cases we need to get a new ElemPos, but release it if unused
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = m_aPos[iPosParent].nEndL;
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;

	// Start Tag
	// A loop is used to ignore all remarks tags and special tags
	// i.e. <?xml version="1.0"?>, and <!-- comment here -->
	// So any tag beginning with ? or ! is ignored
	// Loop past ignored tags
	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPosParent].nEndL;
	CString csName;
	while ( csName.IsEmpty() )
	{
		// Look for left angle bracket of start tag
		m_aPos[iPos].nStartL = token.nNext;
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nStartL, _T('<') ) )
			return x_ParseError( _T("Element tag not found") );

		// Set parent's End tag to start looking from here (or later)
		m_aPos[iPosParent].nEndL = m_aPos[iPos].nStartL;

		// Determine whether this is an element, or bypass other type of node
		token.nNext = m_aPos[iPos].nStartL + 1;
		if ( x_FindToken( token ) )
		{
			if ( token.bIsString )
				return x_ParseError( _T("Tag starts with quote") );
			_TCHAR cFirstChar = m_csDoc[token.nL];
			if ( cFirstChar == _T('?') || cFirstChar == _T('!') )
			{
				token.nNext = m_aPos[iPos].nStartL;
				if ( ! x_ParseNode(token) )
					return x_ParseError( _T("Invalid node") );
			}
			else if ( cFirstChar != _T('/') )
			{
				csName = x_GetToken( token );
				// Look for end of tag
				if ( ! x_FindChar(token.szDoc, token.nNext, _T('>')) )
					return x_ParseError( _T("End of tag not found") );
			}
			else
				return x_ReleasePos(); // probably end tag of parent
		}
		else
			return x_ParseError( _T("Abrupt end within tag") );
	}
	m_aPos[iPos].nStartR = token.nNext;

	// Is ending mark within start tag, i.e. empty element?
	if ( m_csDoc[m_aPos[iPos].nStartR-1] == _T('/') )
	{
		// Empty element
		// Close tag left is set to ending mark, and right to open tag right
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR-1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nStartR;
	}
	else // look for end tag
	{
		// Element probably has contents
		// Determine where to start looking for left angle bracket of end tag
		// This is done by recursively parsing the contents of this element
		int iInner, iInnerPrev = 0;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + 1;
		while ( (iInner = x_ParseElem( iPos )) > 0 )
		{
			// Set links to iInner
			if ( iInnerPrev )
				m_aPos[iInnerPrev].iElemNext = iInner;
			else
				m_aPos[iPos].iElemChild = iInner;
			iInnerPrev = iInner;

			// Set offset to reflect child
			m_aPos[iPos].nEndL = m_aPos[iInner].nEndR + 1;
		}
		if ( iInner == -1 )
			return -1;

		// Look for left angle bracket of end tag
		if ( ! x_FindChar( token.szDoc, m_aPos[iPos].nEndL, _T('<') ) )
			return x_ParseError( _T("End tag of %s element not found"), csName );

		// Look through tokens of end tag
		token.nNext = m_aPos[iPos].nEndL + 1;
		int nTokenCount = 0;
		while ( x_FindToken( token ) )
		{
			++nTokenCount;
			if ( ! token.bIsString )
			{
				// Is first token not an end slash mark?
				if ( nTokenCount == 1 && m_csDoc[token.nL] != _T('/') )
					return x_ParseError( _T("Expecting end tag of element %s"), csName );

				else if ( nTokenCount == 2 && ! token.Match(csName) )
					return x_ParseError( _T("End tag does not correspond to %s"), csName );

				// Else is it a right angle bracket?
				else if ( m_csDoc[token.nL] == _T('>') )
					break;
			}
		}

		// Was a right angle bracket not found?
		if ( ! token.szDoc[token.nL] || nTokenCount < 2 )
			return x_ParseError( _T("End tag not completed for element %s"), csName );
		m_aPos[iPos].nEndR = token.nL;
	}

	// Successfully parsed element (and contained elements)
	return iPos;
}

bool CMarkup::x_FindChar( LPCTSTR szDoc, int& nChar, _TCHAR c )
{
	// static function
	LPCTSTR pChar = &szDoc[nChar];
	while ( *pChar && *pChar != c )
		pChar += _tclen( pChar );
	nChar = pChar - szDoc;
	if ( ! *pChar )
		return false;
	/*
	while ( szDoc[nChar] && szDoc[nChar] != c )
		nChar += _tclen( &szDoc[nChar] );
	if ( ! szDoc[nChar] )
		return false;
	*/
	return true;
}

bool CMarkup::x_FindToken( CMarkup::TokenPos& token )
{
	// Starting at token.nNext, bypass whitespace and find the next token
	// returns true on success, members of token point to token
	// returns false on end of document, members point to end of document
	LPCTSTR szDoc = token.szDoc;
	int nChar = token.nNext;
	token.bIsString = false;

	// By-pass leading whitespace
	while ( szDoc[nChar] && _tcschr(_T(" \t\n\r"),szDoc[nChar]) )
		++nChar;
	if ( ! szDoc[nChar] )
	{
		// No token was found before end of document
		token.nL = nChar;
		token.nR = nChar;
		token.nNext = nChar;
		return false;
	}

	// Is it an opening quote?
	_TCHAR cFirstChar = szDoc[nChar];
	if ( cFirstChar == _T('\"') || cFirstChar == _T('\'') )
	{
		token.bIsString = true;

		// Move past opening quote
		++nChar;
		token.nL = nChar;

		// Look for closing quote
		x_FindChar( token.szDoc, nChar, cFirstChar );

		// Set right to before closing quote
		token.nR = nChar - 1;

		// Set nChar past closing quote unless at end of document
		if ( szDoc[nChar] )
			++nChar;
	}
	else
	{
		// Go until special char or whitespace
		token.nL = nChar;
		while ( szDoc[nChar] && ! _tcschr(_T(" \t\n\r<>=\\/?!"),szDoc[nChar]) )
			nChar += _tclen(&szDoc[nChar]);

		// Adjust end position if it is one special char
		if ( nChar == token.nL )
			++nChar; // it is a special char
		token.nR = nChar - 1;
	}

	// nNext points to one past last char of token
	token.nNext = nChar;
	return true;
}

CString CMarkup::x_GetToken( const CMarkup::TokenPos& token ) const
{
	// The token contains indexes into the document identifying a small substring
	// Build the substring from those indexes and return it
	if ( token.nL > token.nR )
		return _T("");
	return m_csDoc.Mid( token.nL,
		token.nR - token.nL + ((token.nR<m_csDoc.GetLength())? 1:0) );
}

int CMarkup::x_FindElem( int iPosParent, int iPos, LPCTSTR szPath )
{
	// If szPath is NULL or empty, go to next sibling element
	// Otherwise go to next sibling element with matching path
	//
	/*
	if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[0] )
		return iPos;

	// Search
	TokenPos token( m_csDoc );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStartL + 1;
		x_FindToken( token ); // Locate tag name
		if ( token.Match(szPath) )
			return iPos;
		iPos = m_aPos[iPos].iElemNext;
	}
	return 0;
	*/

	//[CMARKUPDEV
	//
	// Example relative paths:
	//
	// "ITEM/QTY" first QTY child element of next ITEM element
	// "ITEM/*" first child element of next ITEM element
	//
	// Example absolute paths:
	//
	// "/"  root
	// "/ORDER"  root must be an ORDER element
	// "/*/ITEM" first ITEM child element of root
	//
	int iPathOffset = 0;
	if ( szPath && szPath[0] == _T('/') )
	{
		// Go to root
		iPos = m_aPos[0].iElemChild;
		iPosParent = 0;
		++iPathOffset;
	}
	else if ( iPos )
		iPos = m_aPos[iPos].iElemNext;
	else
		iPos = m_aPos[iPosParent].iElemChild;

	// Finished here if szPath not specified
	if ( szPath == NULL || !szPath[iPathOffset] )
		return iPos;

	// Search
	TokenPos token( m_csDoc );
	while ( iPos )
	{
		// Compare tag name
		token.nNext = m_aPos[iPos].nStartL + 1;
		x_FindToken( token ); // Locate tag name
		if ( szPath[iPathOffset] == _T('*') )
		{
			// Wildcard
			if ( szPath[iPathOffset+1] == _T('/') )
			{
				iPathOffset += 2;
				iPosParent = iPos;
				iPos = m_aPos[iPosParent].iElemChild;
			}
			else
				return iPos;
		}
		else if ( token.Match(&szPath[iPathOffset]) )
		{
			// Matched tag name
			int nLen = token.nR - token.nL + 1;
			if ( szPath[iPathOffset+nLen] == _T('/') )
			{
				iPathOffset += nLen + 1;
				iPosParent = iPos;
				iPos = m_aPos[iPosParent].iElemChild;
			}
			else
				return iPos;
		}
		else
			iPos = m_aPos[iPos].iElemNext;
	}
	return 0;
	//]CMARKUPDEV
}

int CMarkup::x_ParseNode( CMarkup::TokenPos& token )
{
	// Call this with token.nNext set to the start of the node
	// This returns the node type and token.nNext set to the char after the node
	// If the node is not found or an element, token.nR is not determined
	// White space between elements is a text node
	int nTypeFound = 0;
	LPCTSTR szDoc = token.szDoc;
	token.nL = token.nNext;
	if ( szDoc[token.nL] == '<' )
	{
		// Started with <, could be:
		// <!--...--> comment
		// <!DOCTYPE ...> dtd
		// <?target ...?> processing instruction
		// <![CDATA[...]]> cdata section
		// <NAME ...> element
		//
		if ( ! szDoc[token.nL+1] || ! szDoc[token.nL+2] )
			return 0;
		_TCHAR cFirstChar = szDoc[token.nL+1];
		LPCTSTR szEndOfNode = NULL;
		if ( cFirstChar == _T('?') )
		{
			nTypeFound = MNT_PROCESSING_INSTRUCTION; // processing instruction
			szEndOfNode = _T("?>");
		}
		else if ( cFirstChar == _T('!') )
		{
			_TCHAR cSecondChar = szDoc[token.nL+2];
			if ( cSecondChar == _T('[') )
			{
				nTypeFound = MNT_CDATA_SECTION;
				szEndOfNode = _T("]]>");
			}
			else if ( cSecondChar == _T('-') )
			{
				nTypeFound = MNT_COMMENT;
				szEndOfNode = _T("-->");
			}
			else
			{
				// Document type requires tokenizing because of strings and brackets
				nTypeFound = 0;
				int nBrackets = 0;
				while ( x_FindToken(token) )
				{
					if ( ! token.bIsString )
					{
						_TCHAR cChar = szDoc[token.nL];
						if ( cChar == _T('[') )
							++nBrackets;
						else if ( cChar == _T(']') )
							--nBrackets;
						else if ( nBrackets == 0 && cChar == _T('>') )
						{
							nTypeFound = MNT_DOCUMENT_TYPE;
							break;
						}
					}
				}
				if ( ! nTypeFound )
					return 0;
			}
		}
		else if ( cFirstChar == _T('/') )
		{
			// End tag means no node found within parent element
			return 0;
		}
		else
		{
			nTypeFound = MNT_ELEMENT;
		}

		// Search for end of node if not found yet
		if ( szEndOfNode )
		{
			LPCTSTR pEnd = _tcsstr( &szDoc[token.nNext], szEndOfNode );
			if ( ! pEnd )
				return 0; // not well-formed
			token.nNext = (pEnd - szDoc) + _tcslen(szEndOfNode);
		}
	}
	else if ( szDoc[token.nL] )
	{
		// It is text or whitespace because it did not start with <
		nTypeFound = MNT_WHITESPACE;
		if ( x_FindToken(token) )
		{
			if ( szDoc[token.nL] == _T('<') )
				token.nNext = token.nL;
			else
			{
				nTypeFound = MNT_TEXT;
				x_FindChar( token.szDoc, token.nNext, _T('<') );
			}
		}
	}
	return nTypeFound;
}

CString CMarkup::x_GetTagName( int iPos ) const
{
	// Return the tag name at specified element
	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPos].nStartL + 1;
	if ( ! iPos || ! x_FindToken( token ) )
		return _T("");

	// Return substring of document
	return x_GetToken( token );
}

bool CMarkup::x_FindAttrib( CMarkup::TokenPos& token, LPCTSTR szAttrib ) const
{
	// If szAttrib is NULL find next attrib, otherwise find named attrib
	// Return true if found
	int nAttrib = 0;
	for ( int nCount = 0; x_FindToken(token); ++nCount )
	{
		if ( ! token.bIsString )
		{
			// Is it the right angle bracket?
			if ( m_csDoc[token.nL] == _T('>') || m_csDoc[token.nL] == _T('/') )
				break; // attrib not found

			// Equal sign
			if ( m_csDoc[token.nL] == _T('=') )
				continue;

			// Potential attribute
			if ( ! nAttrib && nCount )
			{
				// Attribute name search?
				if ( ! szAttrib || ! szAttrib[0] )
					return true; // return with token at attrib name

				// Compare szAttrib
				if ( token.Match(szAttrib) )
					nAttrib = nCount;
			}
		}
		else if ( nAttrib && nCount == nAttrib + 2 )
		{
			return true;
		}
	}

	// Not found
	return false;
}

CString CMarkup::x_GetAttrib( int iPos, LPCTSTR szAttrib ) const
{
	// Return the value of the attrib at specified element
	if ( ! iPos || m_nNodeType != MNT_ELEMENT )
		return _T("");

	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPos].nStartL + 1;
	if ( szAttrib && x_FindAttrib( token, szAttrib ) )
		return x_TextFromDoc( token.nL, token.nR - ((token.nR<m_csDoc.GetLength())?0:1) );
	return _T("");
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, int nValue )
{
	// Convert integer to string and call SetChildAttrib
	_TCHAR szVal[25];
	_stprintf( szVal, _T("%d"), nValue );
	return x_SetAttrib( iPos, szAttrib, szVal );
}

bool CMarkup::x_SetAttrib( int iPos, LPCTSTR szAttrib, LPCTSTR szValue )
{
	// Set attribute in iPos element
	if ( ! iPos || m_nNodeType != MNT_ELEMENT )
		return false;

	TokenPos token( m_csDoc );
	token.nNext = m_aPos[iPos].nStartL + 1;
	int nInsertAt, nReplace = 0;
	CString csInsert;
	if ( x_FindAttrib( token, szAttrib ) )
	{
		// Decision: for empty value leaving attrib="" instead of removing attrib
		// Replace value only
		csInsert = x_TextToDoc( szValue, true );
		nInsertAt = token.nL;
		nReplace = token.nR-token.nL+1;
	}
	else
	{
		// Insert string name value pair
		CString csFormat;
		csFormat = _T(" ");
		csFormat += szAttrib;
		csFormat += _T("=\"");
		csFormat += x_TextToDoc( szValue, true );
		csFormat += _T("\"");
		csInsert = csFormat;

		// take into account whether it is an empty element
		nInsertAt = m_aPos[iPos].nStartR - (m_aPos[iPos].IsEmptyElement()?1:0);
	}

	x_DocChange( nInsertAt, nReplace, csInsert );
	int nAdjust = csInsert.GetLength() - nReplace;
	m_aPos[iPos].nStartR += nAdjust;
	m_aPos[iPos].AdjustEnd( nAdjust );
	x_Adjust( iPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

//[CMARKUPDEV
bool CMarkup::x_RemoveAttrib( int iPos, LPCTSTR szAttrib )
{
	// Find attribute in iPos element and remove it
	if ( iPos && m_nNodeType == MNT_ELEMENT )
	{
		TokenPos token( m_csDoc );
		token.nNext = m_aPos[iPos].nStartL + 1;
		while ( x_FindAttrib(token) )
		{
			// Compare szAttrib
			if ( token.Match(szAttrib) )
			{
				int nInsertAt = token.nL - 1; // preceding whitespace
				if ( x_FindToken(token) && m_csDoc[token.nL] == _T('=') && x_FindToken(token) && token.bIsString )
				{
					int nReplace = token.nR - nInsertAt + 2;
					x_DocChange( nInsertAt, nReplace, _T("") );
					m_aPos[iPos].nStartR -= nReplace;
					m_aPos[iPos].AdjustEnd( - nReplace );
					x_Adjust( iPos, - nReplace );
					MARKUP_SETDEBUGSTATE;
					return true;
				}
			}
		}
	}
	return false;
}
//]CMARKUPDEV

bool CMarkup::x_CreateNode( CString& csNode, int nNodeType, LPCTSTR szText )
{
	// Set csNode based on nNodeType and szData
	// Return false if szData would jeopardize well-formed document
	//
	switch ( nNodeType )
	{
	//[CMARKUPDEV
	case MNT_PROCESSING_INSTRUCTION:
		csNode = "<?";
		csNode += szText;
		csNode += "?>";
		break;
	case MNT_COMMENT:
		csNode = "<!--";
		csNode += szText;
		csNode += "-->";
		break;
	case MNT_ELEMENT:
		csNode = "<";
		csNode += szText;
		csNode += "/>";
		break;
	case MNT_TEXT:
	case MNT_WHITESPACE:
		csNode = x_TextToDoc( szText );
		break;
	case MNT_DOCUMENT_TYPE:
		csNode = szText;
		break;
	//]CMARKUPDEV
	case MNT_CDATA_SECTION:
		if ( _tcsstr(szText,_T("]]>")) != NULL )
			return false;
		csNode = "<![CDATA[";
		csNode += szText;
		csNode += "]]>";
		break;
	}
	return true;
}

bool CMarkup::x_SetData( int iPos, LPCTSTR szData, int nCDATA )
{
	// Set data at specified position
	// if nCDATA==1, set content of element to a CDATA Section
	CString csInsert;

	//[CMARKUPDEV
	if ( iPos == m_iPos && m_nNodeLength )
	{
		// Not an element
		if ( ! x_CreateNode(csInsert, m_nNodeType, szData) )
			return false;
		x_DocChange( m_nNodeOffset, m_nNodeLength, csInsert );
		x_AdjustForNode( m_iPosParent, iPos, csInsert.GetLength() - m_nNodeLength );
		m_nNodeLength = csInsert.GetLength();
		MARKUP_SETDEBUGSTATE;
		return true;
	}
	//]CMARKUPDEV

	// Set data in iPos element
	if ( ! iPos || m_aPos[iPos].iElemChild )
		return false;

	// Build csInsert from szData based on nCDATA
	// If CDATA section not valid, use parsed text (PCDATA) instead
	if ( nCDATA != 0 )
		if ( ! x_CreateNode(csInsert, MNT_CDATA_SECTION, szData) )
			nCDATA = 0;
	if ( nCDATA == 0 )
		csInsert = x_TextToDoc( szData );

	// Decide where to insert
	int nInsertAt, nReplace;
	if ( m_aPos[iPos].IsEmptyElement() )
	{
		nInsertAt = m_aPos[iPos].nEndL;
		nReplace = 1;

		// Pre-adjust since <NAME/> becomes <NAME>data</NAME>
		CString csTagName = x_GetTagName( iPos );
		m_aPos[iPos].nStartR -= 1;
		m_aPos[iPos].nEndL -= (1 + csTagName.GetLength());
		CString csFormat;
		csFormat = _T(">");
		csFormat += csInsert;
		csFormat += _T("</");
		csFormat += csTagName;
		csInsert = csFormat;
	}
	else
	{
		nInsertAt = m_aPos[iPos].nStartR+1;
		nReplace = m_aPos[iPos].nEndL - m_aPos[iPos].nStartR - 1;
	}
	x_DocChange( nInsertAt, nReplace, csInsert );
	int nAdjust = csInsert.GetLength() - nReplace;
	x_Adjust( iPos, nAdjust );
	m_aPos[iPos].AdjustEnd( nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

CString CMarkup::x_GetData( int iPos ) const
{
	//[CMARKUPDEV
	if ( iPos == m_iPos && m_nNodeLength )
	{
		if ( m_nNodeType == MNT_COMMENT )
			return m_csDoc.Mid( m_nNodeOffset+4, m_nNodeLength-7 );
		else if ( m_nNodeType == MNT_PROCESSING_INSTRUCTION )
			return m_csDoc.Mid( m_nNodeOffset+2, m_nNodeLength-4 );
		else if ( m_nNodeType == MNT_CDATA_SECTION )
			return m_csDoc.Mid( m_nNodeOffset+8, m_nNodeLength-11 );
		else if ( m_nNodeType == MNT_TEXT )
			return x_TextFromDoc( m_nNodeOffset, m_nNodeOffset+m_nNodeLength-1 );
		else
			return m_csDoc.Mid( m_nNodeOffset, m_nNodeLength );
	}
	//]CMARKUPDEV

	// Return a string representing data between start and end tag
	// Return empty string if there are any children elements
	if ( ! m_aPos[iPos].iElemChild && ! m_aPos[iPos].IsEmptyElement() )
	{
		// See if it is a CDATA section
		TokenPos token( m_csDoc );
		token.nNext = m_aPos[iPos].nStartR+1;
		if ( x_FindToken( token ) && m_csDoc[token.nL] == _T('<')
				&& token.nL + 11 < m_aPos[iPos].nEndL
				&& _tcsncmp( &token.szDoc[token.nL+1], _T("![CDATA["), 8 ) == 0 )
		{
			int nEndCDATA = m_csDoc.Find( _T("]]>"), token.nNext );
			if ( nEndCDATA != -1 && nEndCDATA < m_aPos[iPos].nEndL )
			{
				return m_csDoc.Mid( token.nL+9, nEndCDATA-token.nL-9 );
			}
		}
		return x_TextFromDoc( m_aPos[iPos].nStartR+1, m_aPos[iPos].nEndL-1 );
	}
	return _T("");
}

CString CMarkup::x_TextToDoc( LPCTSTR szText, bool bAttrib ) const
{
	// Convert text as seen outside XML document to XML friendly
	// replacing special characters with ampersand escape codes
	// E.g. convert "6>7" to "6&gt;7"
	//
	// &lt;   less than
	// &amp;  ampersand
	// &gt;   greater than
	//
	// and for attributes:
	//
	// &apos; apostrophe or single quote
	// &quot; double quote
	//
	static _TCHAR* szaReplace[] = { _T("&lt;"),_T("&amp;"),_T("&gt;"),_T("&apos;"),_T("&quot;") };
	const _TCHAR* pFind = bAttrib?_T("<&>\'\""):_T("<&>");
	CString csText;
	const _TCHAR* pSource = szText;
	int nDestSize = _tcslen(pSource);
	nDestSize += nDestSize / 10 + 7;
	_TCHAR* pDest = csText.GetBuffer(nDestSize);
	int nLen = 0;
	_TCHAR cSource = *pSource;
	_TCHAR* pFound;
	while ( cSource )
	{
		if ( nLen > nDestSize - 6 )
		{
			csText.ReleaseBuffer(nLen);
			nDestSize *= 2;
			pDest = csText.GetBuffer(nDestSize);
		}
		if ( (pFound=(_TCHAR *)_tcschr(pFind,cSource)) != NULL )
		{
			pFound = szaReplace[pFound-pFind];
			_tcscpy(&pDest[nLen],pFound);
			nLen += _tcslen(pFound);
		}
		else
		{
			_tccpy( &pDest[nLen], pSource );
			++nLen;
		}
		pSource += _tclen( pSource );
		cSource = *pSource;
	}
	csText.ReleaseBuffer(nLen);
	return csText;
}

CString CMarkup::x_TextFromDoc( int nLeft, int nRight ) const
{
	// Convert XML friendly text to text as seen outside XML document
	// replacing ampersand escape codes with special characters
	// E.g. convert "6&gt;7" to "6>7"
	//
	// Conveniently the result is always the same or shorter in length
	//
	static _TCHAR* szaCode[] = { _T("lt;"),_T("amp;"),_T("gt;"),_T("apos;"),_T("quot;") };
	static int anCodeLen[] = { 3,4,3,5,5 };
	static _TCHAR* szSymbol = _T("<&>\'\"");
	CString csText;
	const _TCHAR* pSource = m_csDoc;
	int nDestSize = nRight - nLeft + 1;
	_TCHAR* pDest = csText.GetBuffer(nDestSize);
	int nLen = 0;
	int nCharLen;
	int nChar = nLeft;
	while ( nChar <= nRight )
	{
		if ( pSource[nChar] == _T('&') )
		{
			// Look for matching &code;
			int nMatch;
			for ( nMatch = 0; nMatch < 5; ++nMatch )
			{
				if ( nChar <= nRight - anCodeLen[nMatch]
					&& _tcsncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
				{
					pDest[nLen++] = szSymbol[nMatch];
					nChar += anCodeLen[nMatch] + 1;
					break;
				}
			}

			// If no match is found it means XML doc is invalid
			// no devastating harm done, ampersand code will just be left in result
			if ( nMatch == 5 )
			{
				pDest[nLen++] = _T('&');
				++nChar;
			}
		}
		else
		{
			nCharLen = _tclen(&pSource[nChar]);
			_tccpy( &pDest[nLen], &pSource[nChar] );
			nLen += nCharLen;
			nChar += nCharLen;
		}
	}
	csText.ReleaseBuffer(nLen);
	return csText;
}

void CMarkup::x_DocChange( int nLeft, int nReplace, const CString& csInsert )
{
	// Insert csInsert int m_csDoc at nLeft replacing nReplace chars
	// Do this with only one buffer reallocation if it grows
	//
	int nDocLength = m_csDoc.GetLength();
	int nInsLength = csInsert.GetLength();

	// Make sure nLeft and nReplace are within bounds
	nLeft = max( 0, min( nLeft, nDocLength ) );
	nReplace = max( 0, min( nReplace, nDocLength-nLeft ) );

	// Get pointer to buffer with enough room
	int nNewLength = nInsLength + nDocLength - nReplace;
	int nBufferLen = nNewLength;
	//[CMARKUPDEV
	// When creating a document, reduce reallocs by reserving string space
	// Allow for 1.5 times the current allocation, with minimum of 200
	int nAllocLen = ((CStringData*)((LPCTSTR)m_csDoc)-1)->nAllocLength;
	if ( nNewLength > nAllocLen )
		nBufferLen = max( nAllocLen+nAllocLen/2, max(nNewLength,200) );
	//]CMARKUPDEV
	_TCHAR* pDoc = m_csDoc.GetBuffer( nBufferLen );

	// Move part of old doc that goes after insert
	if ( nLeft+nReplace < nDocLength )
		memmove( &pDoc[nLeft+nInsLength], &pDoc[nLeft+nReplace], (nDocLength-nLeft-nReplace)*sizeof(_TCHAR) );

	// Copy insert
	memcpy( &pDoc[nLeft], csInsert, nInsLength*sizeof(_TCHAR) );

	// Release
	m_csDoc.ReleaseBuffer( nNewLength );
}

void CMarkup::x_Adjust( int iPos, int nShift, bool bAfterPos )
{
	// Loop through affected elements and adjust indexes
	// Algorithm:
	// 1. update children unless bAfterPos
	//    (if no children or bAfterPos is true, end tag of iPos not affected)
	// 2. update next siblings and their children
	// 3. go up until there is a next sibling of a parent and update end tags
	// 4. step 2
	int iPosTop = m_aPos[iPos].iElemParent;
	bool bPosFirst = bAfterPos; // mark as first to skip its children
	while ( iPos )
	{
		// Were we at containing parent of affected position?
		bool bPosTop = false;
		if ( iPos == iPosTop )
		{
			// Move iPosTop up one towards root
			iPosTop = m_aPos[iPos].iElemParent;
			bPosTop = true;
		}

		// Traverse to the next update position
		if ( ! bPosTop && ! bPosFirst && m_aPos[iPos].iElemChild )
		{
			// Depth first
			iPos = m_aPos[iPos].iElemChild;
		}
		else if ( m_aPos[iPos].iElemNext )
		{
			iPos = m_aPos[iPos].iElemNext;
		}
		else
		{
			// Look for next sibling of a parent of iPos
			// When going back up, parents have already been done except iPosTop
			while ( (iPos=m_aPos[iPos].iElemParent) != 0 && iPos != iPosTop )
				if ( m_aPos[iPos].iElemNext )
				{
					iPos = m_aPos[iPos].iElemNext;
					break;
				}
		}
		bPosFirst = false;

		// Shift indexes at iPos
		if ( iPos != iPosTop )
			m_aPos[iPos].AdjustStart( nShift );
		m_aPos[iPos].AdjustEnd( nShift );
	}
}

void CMarkup::x_LocateNew( int iPosParent, int& iPosRel, int& nOffset, int nLength, int nFlags )
{
	// Determine where to insert new element or node
	//
	bool bInsert = (nFlags&1)?true:false;
	bool bHonorWhitespace = (nFlags&2)?true:false;

	int nStartL;
	if ( nLength )
	{
		// Located at a non-element node
		if ( bInsert )
			nStartL = nOffset;
		else
			nStartL = nOffset + nLength;
	}
	else if ( iPosRel )
	{
		// Located at an element
		if ( bInsert ) // precede iPosRel
			nStartL = m_aPos[iPosRel].nStartL;
		else // follow iPosRel
			nStartL = m_aPos[iPosRel].nEndR + 1;
	}
	else if ( m_aPos[iPosParent].IsEmptyElement() )
	{
		// Parent has no separate end tag, so split empty element
		nStartL = m_aPos[iPosParent].nStartR;
	}
	else
	{
		if ( bInsert ) // after start tag
			nStartL = m_aPos[iPosParent].nStartR + 1;
		else // before end tag
			nStartL = m_aPos[iPosParent].nEndL;
	}

	// Go up to start of next node, unless its splitting an empty element
	if ( ! bHonorWhitespace && ! m_aPos[iPosParent].IsEmptyElement() )
	{
		TokenPos token( m_csDoc );
		token.nNext = nStartL;
		if ( ! x_FindToken(token) || m_csDoc[token.nL] == _T('<') )
			nStartL = token.nL;
	}

	// Determine iPosBefore
	int iPosBefore = 0;
	if ( iPosRel )
	{
		if ( bInsert )
		{
			// Is iPosRel past first sibling?
			int iPosPrev = m_aPos[iPosParent].iElemChild;
			if ( iPosPrev != iPosRel )
			{
				// Find previous sibling of iPosRel
				while ( m_aPos[iPosPrev].iElemNext != iPosRel )
					iPosPrev = m_aPos[iPosPrev].iElemNext;
				iPosBefore = iPosPrev;
			}
		}
		else
		{
			iPosBefore = iPosRel;
		}
	}
	else if ( m_aPos[iPosParent].iElemChild )
	{
		if ( ! bInsert )
		{
			// Find last element under iPosParent
			int iPosLast = m_aPos[iPosParent].iElemChild;
			int iPosNext = iPosLast;
			while ( iPosNext )
			{
				iPosLast = iPosNext;
				iPosNext = m_aPos[iPosNext].iElemNext;
			}
			iPosBefore = iPosLast;
		}
	}

	nOffset = nStartL;
	iPosRel = iPosBefore;
}

bool CMarkup::x_AddElem( LPCTSTR szName, LPCTSTR szValue, bool bInsert, bool bAddChild )
{
	if ( bAddChild )
	{
		// Adding a child element under main position
		if ( ! m_iPos )
			return false;
	}
	else if ( m_iPosParent == 0 )
	{
		// Adding root element
		if ( IsWellFormed() )
			return false;

//[CMARKUPDEV
#if defined( x_VERSIONTAG )
		if ( m_csDoc.IsEmpty() )
			m_csDoc = x_VERSIONTAG;
#endif
//]CMARKUPDEV

		// Locate after any version and DTD
		m_aPos[0].nEndL = m_csDoc.GetLength();
	}

	// Locate where to add element relative to current node
	int iPosParent, iPosBefore, nOffset = 0, nLength = 0;
	if ( bAddChild )
	{
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
		//[CMARKUPDEV
		nOffset = m_nNodeOffset;
		nLength = m_nNodeLength;
		//]CMARKUPDEV
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, nLength, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent )
		nOffset += 2; // include CRLF

	// Create element and modify positions of affected elements
	// If no szValue is specified, an empty element is created
	// i.e. either <NAME>value</NAME> or <NAME/>
	//
	int iPos = x_GetFreePos();
	m_aPos[iPos].nStartL = nOffset;

	// Set links
	m_aPos[iPos].iElemParent = iPosParent;
	m_aPos[iPos].iElemChild = 0;
	m_aPos[iPos].iElemNext = 0;
	if ( iPosBefore )
	{
		// Link in after iPosBefore
		m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
		m_aPos[iPosBefore].iElemNext = iPos;
	}
	else
	{
		// First child
		m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
		m_aPos[iPosParent].iElemChild = iPos;
	}

	// Create string for insert
	CString csInsert;
	int nLenName = _tcslen(szName);
	int nLenValue = szValue? _tcslen(szValue) : 0;
	if ( ! nLenValue )
	{
		// <NAME/> empty element
		csInsert = _T("<");
		csInsert += szName;
		csInsert += _T("/>\r\n");
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 2;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR - 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + 1;
	}
	else
	{
		// <NAME>value</NAME>
		CString csValue = x_TextToDoc( szValue );
		nLenValue = csValue.GetLength();
		csInsert = _T("<");
		csInsert += szName;
		csInsert += _T(">");
		csInsert += csValue;
		csInsert += _T("</");
		csInsert += szName;
		csInsert += _T(">\r\n");
		m_aPos[iPos].nStartR = m_aPos[iPos].nStartL + nLenName + 1;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR + nLenValue + 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nEndL + nLenName + 2;
	}

	// Insert
	int nReplace = 0, nLeft = m_aPos[iPos].nStartL;
	if ( bEmptyParent )
	{
		CString csParentTagName = x_GetTagName(iPosParent);
		CString csFormat;
		csFormat = _T(">\r\n");
		csFormat += csInsert;
		csFormat += _T("</");
		csFormat += csParentTagName;
		csInsert = csFormat;
		nLeft -= 3;
		nReplace = 1;
		// x_Adjust is going to update all affected indexes by one amount
		// This will satisfy all except the empty parent
		// Here we pre-adjust for the empty parent
		// The empty tag slash is removed
		m_aPos[iPosParent].nStartR -= 1;
		// For the newly created end tag, see the following example:
		// <A/> (len 4) becomes <A><B/></A> (len 11)
		// In x_Adjust everything will be adjusted 11 - 4 = 7
		// But the nEndL of element A should only be adjusted 5
		m_aPos[iPosParent].nEndL -= (csParentTagName.GetLength() + 1);
	}
	x_DocChange( nLeft, nReplace, csInsert );
	x_Adjust( iPos, csInsert.GetLength() - nReplace );

	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else
		x_SetPos( iPosParent, iPos, 0 );
	return true;
}

bool CMarkup::x_AddSubDoc( LPCTSTR szSubDoc, bool bInsert, bool bAddChild )
{
	// Add subdocument, parse, and modify positions of affected elements
	//
	int nOffset = 0, iPosParent, iPosBefore;
	if ( bAddChild )
	{
		// Add a subdocument under main position, after current child position
		if ( ! m_iPos )
			return false;
		iPosParent = m_iPos;
		iPosBefore = m_iPosChild;
	}
	else
	{
		iPosParent = m_iPosParent;
		iPosBefore = m_iPos;
	}
	int nFlags = bInsert?1:0;
	x_LocateNew( iPosParent, iPosBefore, nOffset, 0, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	if ( bEmptyParent )
		nOffset += 2; // include CRLF

	// if iPosBefore is NULL, insert as first element under parent
	int nParentEndLBeforeAdd = m_aPos[iPosParent].nEndL;
	int iPosFreeBeforeAdd = m_iPosFree;

	// Skip version tag or DTD at start of subdocument
	TokenPos token( szSubDoc );
	int nNodeType = x_ParseNode( token );
	while ( nNodeType && nNodeType != MNT_ELEMENT )
	{
		token.szDoc = &szSubDoc[token.nNext];
		token.nNext = 0;
		nNodeType = x_ParseNode( token );
	}
	CString csInsert = token.szDoc;

	// Insert subdocument
	m_aPos[iPosParent].nEndL = nOffset;
	int nReplace = 0, nLeft = nOffset;
	CString csParentTagName;
	if ( bEmptyParent )
	{
		csParentTagName = x_GetTagName(iPosParent);
		CString csFormat;
		csFormat = _T(">\r\n");
		csFormat += csInsert;
		csFormat += _T("</");
		csFormat += csParentTagName;
		csInsert = csFormat;
		m_aPos[iPosParent].nEndL = m_aPos[iPosParent].nStartR + 2;
		nLeft = m_aPos[iPosParent].nStartR - 1;
		nReplace = 1;
	}
	x_DocChange( nLeft, nReplace, csInsert );

	// Parse subdocument
	int iPos = x_ParseElem(iPosParent);
	m_aPos[iPosParent].nEndL = nParentEndLBeforeAdd;
	if ( iPos <= 0 )
	{
		// Abort because not well-formed
		CString csRevert = bEmptyParent?_T("/"):_T("");
		x_DocChange( nLeft, csInsert.GetLength(), csRevert );
		m_iPosFree = iPosFreeBeforeAdd;
		return false;
	}
	else
	{
		// Link in parent and siblings
		m_aPos[iPos].iElemParent = iPosParent;
		if ( iPosBefore )
		{
			m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
			m_aPos[iPosBefore].iElemNext = iPos;
		}
		else
		{
			m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
			m_aPos[iPosParent].iElemChild = iPos;
		}

		// Make empty parent pre-adjustment
		if ( bEmptyParent )
		{
			m_aPos[iPosParent].nStartR -= 1;
			m_aPos[iPosParent].nEndL -= (csParentTagName.GetLength() + 1);
		}

		// Adjust, but don't adjust children of iPos (bAfterPos=true)
		x_Adjust( iPos, csInsert.GetLength() - nReplace, true );
	}

	// Set position to top element of subdocument
	if ( bAddChild )
		x_SetPos( m_iPosParent, iPosParent, iPos );
	else // Main
		x_SetPos( m_iPosParent, iPos, 0 );
	return true;
}

int CMarkup::x_RemoveElem( int iPos )
{
	// Remove element and all contained elements
	// Return new position
	//
	int iPosParent = m_aPos[iPos].iElemParent;

	// Find previous sibling and bypass removed element
	// This leaves orphan positions in m_aPos array
	int iPosLook = m_aPos[iPosParent].iElemChild;
	int iPosPrev = 0;
	while ( iPosLook != iPos )
	{
		iPosPrev = iPosLook;
		iPosLook = m_aPos[iPosLook].iElemNext;
	}
	if ( iPosPrev )
		m_aPos[iPosPrev].iElemNext = m_aPos[iPos].iElemNext;
	else
		m_aPos[iPosParent].iElemChild = m_aPos[iPos].iElemNext;

	// Remove from document
	// Links have been changed to go around removed element
	// But element position and links are still valid
	int nAfterEnd = m_aPos[iPos].nEndR + 1;
	TokenPos token( m_csDoc );
	token.nNext = nAfterEnd;
	if ( ! x_FindToken(token) || token.szDoc[token.nL] == _T('<') )
		nAfterEnd = token.nL;
	int nLen = nAfterEnd - m_aPos[iPos].nStartL;
	x_DocChange( m_aPos[iPos].nStartL, nLen, CString() );
	x_Adjust( iPos, - nLen, true );
	return iPosPrev;
}

//[CMARKUPDEV
void CMarkup::x_AdjustForNode( int iPosParent, int iPos, int nShift )
{
	// Adjust affected indexes
	if ( ! iPos )
	{
		// Adjust end tag of parent, and adjust from there
		int iPosChild = m_aPos[iPosParent].iElemChild;
		if ( ! iPosParent )
		{
			if ( ! iPosChild )
				return; // no elements in this document yet

			// Change happened before root element
			m_aPos[iPosChild].AdjustStart( nShift );
			iPos = m_aPos[iPosChild].iElemChild;
			if ( iPos )
				m_aPos[iPos].AdjustStart( nShift );
			else
				iPos = iPosChild;
		}
		else
		{
			// Change happened before or at first element under iPosParent
			iPos = iPosChild;
			if ( iPos )
				m_aPos[iPos].AdjustStart( nShift );
			else
				iPos = iPosParent;
		}
		m_aPos[iPos].AdjustEnd( nShift );
	}
	x_Adjust( iPos, nShift, true );
}

bool CMarkup::x_AddNode( int nNodeType, LPCTSTR szText, bool bInsert )
{
	// Add node of nNodeType after current node position
	CString csInsert;
	if ( ! x_CreateNode(csInsert, nNodeType, szText) )
		return false;

	// Only comments, DTDs, and processing instructions are followed by CRLF
	// Other nodes are usually concerned with mixed content, so no CRLF
	bool bNewline = false;
	if ( nNodeType == MNT_PROCESSING_INSTRUCTION
			|| nNodeType == MNT_COMMENT
			|| nNodeType == MNT_DOCUMENT_TYPE )
		bNewline = true;

	// Locate where to add node relative to current node
	int iPosBefore = m_iPos;
	int iPosParent = m_iPosParent;
	int nNodeOffset = m_nNodeOffset;
	int nNodeLength = m_nNodeLength;
	int nFlags = bNewline?0:2;
	if ( bInsert )
		nFlags |= 1;
	x_LocateNew( iPosParent, iPosBefore, nNodeOffset, nNodeLength, nFlags );
	bool bEmptyParent = m_aPos[iPosParent].IsEmptyElement();
	int nInsertAt = nNodeOffset;

	// Get node length before adding CRLF
	nNodeLength = csInsert.GetLength();
	if ( bNewline )
		csInsert += _T("\r\n");

	// If its a new element, link it in
	int iPos = iPosBefore;
	if ( nNodeType == MNT_ELEMENT )
	{
		// Set indexes
		iPos = x_GetFreePos();
		m_aPos[iPos].nStartL = nNodeOffset;
		m_aPos[iPos].nStartR = nNodeOffset + nNodeLength - 1;
		m_aPos[iPos].nEndL = m_aPos[iPos].nStartR - 1;
		m_aPos[iPos].nEndR = m_aPos[iPos].nStartR;
		nNodeOffset = 0;
		nNodeLength = 0;

		// Set links
		m_aPos[iPos].iElemParent = iPosParent;
		m_aPos[iPos].iElemChild = 0;
		m_aPos[iPos].iElemNext = 0;
		if ( iPosBefore )
		{
			// Link in after iPosBefore
			m_aPos[iPos].iElemNext = m_aPos[iPosBefore].iElemNext;
			m_aPos[iPosBefore].iElemNext = iPos;
		}
		else
		{
			// First child
			m_aPos[iPos].iElemNext = m_aPos[iPosParent].iElemChild;
			m_aPos[iPosParent].iElemChild = iPos;
		}
	}

	// Insert
	int nReplace = 0;
	if ( bEmptyParent )
	{
		nReplace = 1;
		--nInsertAt;
		CString csParentTagName = x_GetTagName(iPosParent);
		CString csFormat;
		csFormat = _T(">");
		if ( bNewline )
			csFormat += _T("\r\n");
		csFormat += csInsert;
		csFormat += _T("</");
		csFormat += csParentTagName;
		csInsert = csFormat;
		m_aPos[iPosParent].nStartR -= 1;
		m_aPos[iPosParent].nEndL -= (csParentTagName.GetLength() + 1);
	}
	x_DocChange( nInsertAt, nReplace, csInsert );

	// Need to adjust element positions after iPos
	x_AdjustForNode( iPosParent, iPos, csInsert.GetLength() - nReplace );

	// Set current position
	m_iPos = iPos;
	m_iPosChild = 0;
	m_nNodeOffset = nNodeOffset;
	m_nNodeLength = nNodeLength;
	m_nNodeType = nNodeType;
	MARKUP_SETDEBUGSTATE;
	return true;
}

void CMarkup::x_RemoveNode( int iPosParent, int& iPos, int& nNodeType, int& nNodeOffset, int& nNodeLength )
{
	// Remove node and return new position
	//
	int iPosPrev = iPos;

	// Removing an element?
	if ( nNodeType == MNT_ELEMENT )
	{
		nNodeOffset = m_aPos[iPos].nStartL;
		nNodeLength = m_aPos[iPos].nEndR - m_aPos[iPos].nStartL + 1;

		// Deleting iPos element, so find previous element if any
		iPosPrev = m_aPos[iPosParent].iElemChild;
		if ( iPosPrev == iPos )
		{
			iPosPrev = 0;
			m_aPos[iPosParent].iElemChild = m_aPos[iPos].iElemNext;
		}
		else
		{
			while ( m_aPos[iPosPrev].iElemNext != iPos )
				iPosPrev = m_aPos[iPosPrev].iElemNext;
			m_aPos[iPosPrev].iElemNext = m_aPos[iPos].iElemNext;
		}
	}

	// Find previous node type, offset and length
	int nPrevOffset = 0;
	if ( iPosPrev )
		nPrevOffset = m_aPos[iPosPrev].nEndR + 1;
	else if ( iPosParent )
		nPrevOffset = m_aPos[iPosParent].nStartR + 1;
	TokenPos token( m_csDoc );
	token.nNext = nPrevOffset;
	int nPrevType = 0;
	while ( token.nNext < nNodeOffset )
	{
		nPrevOffset = token.nNext;
		nPrevType = x_ParseNode( token );
	}
	int nPrevLength = nNodeOffset - nPrevOffset;
	if ( ! nPrevLength )
	{
		// Previous node is iPosPrev element
		nPrevOffset = 0;
		if ( iPosPrev )
			nPrevType = MNT_ELEMENT;
	}

	// Remove node from document
 	x_DocChange( nNodeOffset, nNodeLength, CString() );
	x_AdjustForNode( iPosParent, iPos, - nNodeLength );

	nNodeType = nPrevType;
	nNodeOffset = nPrevOffset;
	nNodeLength = nPrevLength;
	iPos = iPosPrev;
}
//]CMARKUPDEV

