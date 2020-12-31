/*
    Module : ScintillaCtrl.cpp
    Purpose: Defines the interface for an MFC wrapper class for the Scintilla edit control (www.scintilla.org)
    Created: PJN / 19-03-2004

    Copyright(C) 2004 - 2013 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

    All rights reserved.

    Copyright / Usage Details:

    You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise)
    when your product is released in binary form. You are allowed to modify the source code in any way you want
    except you cannot modify the copyright details at the top of each module. If you want to distribute source
    code with your application, then you are only allowed to distribute versions released by the author. This is
    to maintain a single distribution point for the source code.

*/

/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "ScintillaCtrl.h"

#include "resource.h"

//////////////////////////////// Statics / Macros /////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif


////////////////////////////////// Implementation /////////////////////////////

IMPLEMENT_DYNAMIC(CScintillaCtrl, CWnd)

CScintillaCtrl::CScintillaCtrl() : m_DirectFunction(0), m_DirectPointer(0)
{

}

BOOL CScintillaCtrl::Create(DWORD dwStyle, const RECT& rcT, CWnd* pParentWnd, UINT nID, DWORD dwExStyle, LPVOID lpParam)
{
    //CallScintilla our base class implementation of CWnd::CreateEx
    if (!CreateEx(dwExStyle, _T("scintilla"), NULL, dwStyle, rcT, pParentWnd, nID, lpParam)) {
        return FALSE;
    }

    //Setup the direct access data
    SetupDirectAccess();

    //If we are running as Unicode, then use the UTF8 codepage
    SetCodePage(SC_CP_UTF8);

    return TRUE;
}

void CScintillaCtrl::SetupDirectAccess()
{
    //Setup the direct access data
    m_DirectFunction = GetDirectFunction();
    m_DirectPointer = GetDirectPointer();
}

CScintillaCtrl::~CScintillaCtrl()
{
    DestroyWindow();
}

inline LRESULT CScintillaCtrl::CallScintilla(UINT tMessage, WPARAM wParam, LPARAM lParam, BOOL bDirect)
{
    ASSERT(::IsWindow(m_hWnd)); //Window must be valid

    if (bDirect)
    {
        ASSERT(m_DirectFunction); //Direct function must be valid
        return (reinterpret_cast<SciFnDirect>(m_DirectFunction))(m_DirectPointer, tMessage, wParam, lParam);
    }
    else
        return SendMessage(tMessage, wParam, lParam);
}

LRESULT CScintillaCtrl::GetDirectFunction()
{
    return SendMessage(SCI_GETDIRECTFUNCTION, 0, 0);
}

LRESULT CScintillaCtrl::GetDirectPointer()
{
    return SendMessage(SCI_GETDIRECTPOINTER, 0, 0);
}

CStringA CScintillaCtrl::W2UTF8(const wchar_t* pszText, int nLength)
{
    //First call the function to determine how much space we need to allocate
    int nUTF8Length = WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, NULL, 0, NULL, NULL);

    //If the calculated length is zero, then ensure we have at least room for a NULL terminator
    if (nUTF8Length == 0) {
        nUTF8Length = 1;
    }

    //Now recall with the buffer to get the converted text
    CStringA sUTF;
    char* pszUTF8Text = sUTF.GetBuffer(nUTF8Length + 1); //include an extra byte because we may be null terminating the string ourselves
    int nCharsWritten = WideCharToMultiByte(CP_UTF8, 0, pszText, nLength, pszUTF8Text, nUTF8Length, NULL, NULL);

    //Ensure we NULL terminate the text if WideCharToMultiByte doesn't do it for us
    if (nLength != -1) {
        AFXASSUME(nCharsWritten <= nUTF8Length);
        pszUTF8Text[nCharsWritten] = '\0';
    }

    sUTF.ReleaseBuffer();

    return sUTF;
}

CStringW CScintillaCtrl::UTF82W(const char* pszText, int nLength)
{
    //First call the function to determine how much space we need to allocate
    int nWideLength = MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, NULL, 0);

    //If the calculated length is zero, then ensure we have at least room for a NULL terminator
    if (nWideLength == 0) {
        nWideLength = 1;
    }

    //Now recall with the buffer to get the converted text
    CStringW sWideString;
    wchar_t* pszWText = sWideString.GetBuffer(nWideLength + 1); //include an extra byte because we may be null terminating the string ourselves
    int nCharsWritten = MultiByteToWideChar(CP_UTF8, 0, pszText, nLength, pszWText, nWideLength);

    //Ensure we NULL terminate the text if MultiByteToWideChar doesn't do it for us
    if (nLength != -1) {
        AFXASSUME(nCharsWritten <= nWideLength);
        pszWText[nCharsWritten] = '\0';
    }

    sWideString.ReleaseBuffer();

    return sWideString;
}

void CScintillaCtrl::AddText(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AddText(sUTF8.GetLength(), sUTF8, bDirect);
}

void CScintillaCtrl::InsertText(long pos, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    InsertText(pos, sUTF8, bDirect);
}

CStringW CScintillaCtrl::GetSelText(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = GetSelText(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetSelText(sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetCurLine(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = GetCurLine(0, NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetCurLine(nUTF8Length, sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

void CScintillaCtrl::StyleSetFont(int style, const wchar_t* fontName, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(fontName, -1));

    StyleSetFont(style, sUTF8, bDirect);
}

void CScintillaCtrl::SetWordChars(const wchar_t* characters, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(characters, -1));

    SetWordChars(sUTF8, bDirect);
}

void CScintillaCtrl::AutoCShow(int lenEntered, const wchar_t* itemList, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(itemList, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AutoCShow(lenEntered, sUTF8, bDirect);
}

void CScintillaCtrl::AutoCStops(const wchar_t* characterSet, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(characterSet, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AutoCStops(sUTF8, bDirect);
}

void CScintillaCtrl::AutoCSelect(const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AutoCSelect(sUTF8, bDirect);
}

void CScintillaCtrl::AutoCSetFillUps(const wchar_t* characterSet, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(characterSet, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AutoCSetFillUps(sUTF8, bDirect);
}

void CScintillaCtrl::UserListShow(int listType, const wchar_t* itemList, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(itemList, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    UserListShow(listType, sUTF8, bDirect);
}

CStringW CScintillaCtrl::GetLine(int line, BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = LineLength(line, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetLine(line, sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

void CScintillaCtrl::ReplaceSel(const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    ReplaceSel(sUTF8, bDirect);
}

void CScintillaCtrl::SetText(const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetText(sUTF8, bDirect);
}

CStringW CScintillaCtrl::GetText(int length, BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = length * 4; //A Unicode character can take up to 4 octets when expressed as UTF8

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetText(nUTF8Length, sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    CStringW sWideText(UTF82W(sUTF8, -1));

    return sWideText.Left(length - 1);
}

int CScintillaCtrl::ReplaceTarget(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return ReplaceTarget(sUTF8.GetLength(), sUTF8, bDirect);
}

int CScintillaCtrl::ReplaceTargetRE(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return ReplaceTargetRE(sUTF8.GetLength(), sUTF8, bDirect);
}

int CScintillaCtrl::SearchInTarget(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return SearchInTarget(sUTF8.GetLength(), sUTF8, bDirect);
}

void CScintillaCtrl::CallTipShow(long pos, const wchar_t* definition, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(definition, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    CallTipShow(pos, sUTF8, bDirect);
}

int CScintillaCtrl::TextWidth(int style, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return TextWidth(style, sUTF8, bDirect);
}

void CScintillaCtrl::AppendText(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AppendText(sUTF8.GetLength(), sUTF8, bDirect);
}

int CScintillaCtrl::SearchNext(int flags, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return SearchNext(flags, sUTF8, bDirect);
}

int CScintillaCtrl::SearchPrev(int flags, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return SearchPrev(flags, sUTF8, bDirect);
}

void CScintillaCtrl::CopyText(int length, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, length));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    CopyText(sUTF8.GetLength(), sUTF8, bDirect);
}

void CScintillaCtrl::SetWhitespaceChars(const wchar_t* characters, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(characters, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetWhitespaceChars(sUTF8, bDirect);
}

void CScintillaCtrl::SetProperty(const wchar_t* key, const wchar_t* value, BOOL bDirect)
{
    //Convert the unicode texts to UTF8
    CStringA sUTF8Key(W2UTF8(key, -1));
    CStringA sUTF8Value(W2UTF8(value, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetProperty(sUTF8Key, sUTF8Value, bDirect);
}

void CScintillaCtrl::SetKeyWords(int keywordSet, const wchar_t* keyWords, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(keyWords, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetKeyWords(keywordSet, sUTF8, bDirect);
}

void CScintillaCtrl::SetIdentifiers(int style, const wchar_t* identifiers, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(identifiers, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetIdentifiers(style, sUTF8, bDirect);
}

void CScintillaCtrl::SetLexerLanguage(const wchar_t* language, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(language, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    SetLexerLanguage(sUTF8, bDirect);
}

void CScintillaCtrl::LoadLexerLibrary(const wchar_t* path, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(path, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    LoadLexerLibrary(sUTF8, bDirect);
}

CStringW CScintillaCtrl::GetProperty(const wchar_t* key, BOOL bDirect)
{
    //Validate our parameters
    ASSERT(key);

    //Convert the Key value to UTF8
    CStringA sUTF8Key(W2UTF8(key, -1));

    //Work out the length of string to allocate
    int nUTF8ValueLength = GetProperty(sUTF8Key, 0, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8Value;
    GetProperty(sUTF8Key, sUTF8Value.GetBufferSetLength(nUTF8ValueLength), bDirect);
    sUTF8Value.ReleaseBuffer();

    return UTF82W(sUTF8Value, -1);
}

CStringW CScintillaCtrl::GetPropertyExpanded(const wchar_t* key, BOOL bDirect)
{
    //Validate our parameters
    ASSERT(key);

    //Convert the Key value to UTF8
    CStringA sUTF8Key(W2UTF8(key, -1));

    //Work out the length of string to allocate
    int nUTF8ValueLength = GetPropertyExpanded(sUTF8Key, 0, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8Value;
    GetPropertyExpanded(sUTF8Key, sUTF8Value.GetBufferSetLength(nUTF8ValueLength), bDirect);
    sUTF8Value.ReleaseBuffer();

    return UTF82W(sUTF8Value, -1);
}

int CScintillaCtrl::GetPropertyInt(const wchar_t* key, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(key, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return GetPropertyInt(sUTF8, bDirect);
}

CStringW CScintillaCtrl::StyleGetFont(int style, BOOL bDirect)
{
    //Allocate a UTF8 buffer to contain the font name. See the notes for
    //SCI_STYLEGETFONT / SCI_STYLESETFONT on the reasons why we can use
    //a statically sized buffer of 32 characters in size. Note it is 33 below
    //to include space for the NULL terminator
    char szUTF8FontName[33 * 4]; //A Unicode character can take up to 4 octets when expressed as UTF8
    szUTF8FontName[0] = '\0';

    //CallScintilla the native scintilla version of the function with a UTF8 text buffer
    StyleGetFont(style, szUTF8FontName, bDirect);

    return UTF82W(szUTF8FontName, -1);
}

void CScintillaCtrl::MarginSetText(int line, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    MarginSetText(line, sUTF8, bDirect);
}

void CScintillaCtrl::MarginSetStyles(int line, const wchar_t* styles, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(styles, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    MarginSetStyles(line, sUTF8, bDirect);
}

void CScintillaCtrl::AnnotationSetText(int line, const wchar_t* text, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(text, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AnnotationSetText(line, sUTF8, bDirect);
}

void CScintillaCtrl::AnnotationSetStyles(int line, const wchar_t* styles, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(styles, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    AnnotationSetStyles(line, sUTF8, bDirect);
}

CStringW CScintillaCtrl::AutoCGetCurrentText(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = AutoCGetCurrentText(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    AutoCGetCurrentText(sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetLexerLanguage(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = GetLexerLanguage(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetLexerLanguage(sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::PropertyNames(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = PropertyNames(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    PropertyNames(sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

int CScintillaCtrl::PropertyType(const wchar_t* name, BOOL bDirect)
{
    //Convert the unicode text to UTF8
    CStringA sUTF8(W2UTF8(name, -1));

    //CallScintilla the native scintilla version of the function with the UTF8 text
    return PropertyType(sUTF8, bDirect);
}

CStringW CScintillaCtrl::DescribeProperty(const wchar_t* name, BOOL bDirect)
{
    //Convert the name value to UTF8
    CStringA sUTF8KName(W2UTF8(name, -1));

    //Work out the length of string to allocate
    int nUTF8Length = DescribeProperty(sUTF8KName, NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    DescribeProperty(sUTF8KName, sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::DescribeKeyWordSets(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = DescribeKeyWordSets(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    DescribeKeyWordSets(sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    //Now convert the UTF8 text back to Unicode
    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetTag(int tagNumber, BOOL bDirect)
{
    //Work out the length of string to allocate
    int nUTF8Length = GetTag(tagNumber, NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetTag(tagNumber, sUTF8.GetBufferSetLength(nUTF8Length), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetWordChars(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nValueLength = GetWordChars(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetWordChars(sUTF8.GetBufferSetLength(nValueLength), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetWhitespaceChars(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nValueLength = GetWhitespaceChars(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetWhitespaceChars(sUTF8.GetBufferSetLength(nValueLength), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

CStringW CScintillaCtrl::GetPunctuationChars(BOOL bDirect)
{
    //Work out the length of string to allocate
    int nValueLength = GetPunctuationChars(NULL, bDirect);

    //CallScintilla the function which does the work
    CStringA sUTF8;
    GetPunctuationChars(sUTF8.GetBufferSetLength(nValueLength), bDirect);
    sUTF8.ReleaseBuffer();

    return UTF82W(sUTF8, -1);
}

//Everything else after this point was auto generated using the "ConvertScintillaiface.js" script

void CScintillaCtrl::AddText(int length, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_ADDTEXT, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::AddStyledText(int length, char* c, BOOL bDirect)
{
    CallScintilla(SCI_ADDSTYLEDTEXT, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(c), bDirect);
}

void CScintillaCtrl::InsertText(long pos, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_INSERTTEXT, static_cast<WPARAM>(pos), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::ClearAll(BOOL bDirect)
{
    CallScintilla(SCI_CLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::DeleteRange(long pos, int deleteLength, BOOL bDirect)
{
    CallScintilla(SCI_DELETERANGE, static_cast<WPARAM>(pos), static_cast<LPARAM>(deleteLength), bDirect);
}

void CScintillaCtrl::ClearDocumentStyle(BOOL bDirect)
{
    CallScintilla(SCI_CLEARDOCUMENTSTYLE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLength(BOOL bDirect)
{
    return CallScintilla(SCI_GETLENGTH, 0, 0, bDirect);
}

int CScintillaCtrl::GetCharAt(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_GETCHARAT, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetCurrentPos(BOOL bDirect)
{
    return CallScintilla(SCI_GETCURRENTPOS, 0, 0, bDirect);
}

int CScintillaCtrl::GetCurLineIndex(BOOL bDirect)
{
    int lineIndex = LineFromPosition(GetCurrentPos(bDirect), bDirect);
    return lineIndex;
}

long CScintillaCtrl::GetAnchor(BOOL bDirect)
{
    return CallScintilla(SCI_GETANCHOR, 0, 0, bDirect);
}

int CScintillaCtrl::GetStyleAt(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_GETSTYLEAT, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::Redo(BOOL bDirect)
{
    CallScintilla(SCI_REDO, 0, 0, bDirect);
}

void CScintillaCtrl::SetUndoCollection(BOOL collectUndo, BOOL bDirect)
{
    CallScintilla(SCI_SETUNDOCOLLECTION, static_cast<WPARAM>(collectUndo), 0, bDirect);
}

void CScintillaCtrl::SelectAll(BOOL bDirect)
{
    CallScintilla(SCI_SELECTALL, 0, 0, bDirect);
}

void CScintillaCtrl::SetSavePoint(BOOL bDirect)
{
    CallScintilla(SCI_SETSAVEPOINT, 0, 0, bDirect);
}

int CScintillaCtrl::GetStyledText(TextRange* tr, BOOL bDirect)
{
    return CallScintilla(SCI_GETSTYLEDTEXT, 0, reinterpret_cast<LPARAM>(tr), bDirect);
}

BOOL CScintillaCtrl::CanRedo(BOOL bDirect)
{
    return CallScintilla(SCI_CANREDO, 0, 0, bDirect);
}

int CScintillaCtrl::MarkerLineFromHandle(int handle, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERLINEFROMHANDLE, static_cast<WPARAM>(handle), 0, bDirect);
}

void CScintillaCtrl::MarkerDeleteHandle(int handle, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDELETEHANDLE, static_cast<WPARAM>(handle), 0, bDirect);
}

BOOL CScintillaCtrl::GetUndoCollection(BOOL bDirect)
{
    return CallScintilla(SCI_GETUNDOCOLLECTION, 0, 0, bDirect);
}

int CScintillaCtrl::GetViewWS(BOOL bDirect)
{
    return CallScintilla(SCI_GETVIEWWS, 0, 0, bDirect);
}

void CScintillaCtrl::SetViewWS(int viewWS, BOOL bDirect)
{
    CallScintilla(SCI_SETVIEWWS, static_cast<WPARAM>(viewWS), 0, bDirect);
}

long CScintillaCtrl::PositionFromPoint(int x, int y, BOOL bDirect)
{
    return CallScintilla(SCI_POSITIONFROMPOINT, static_cast<WPARAM>(x), static_cast<LPARAM>(y), bDirect);
}

long CScintillaCtrl::PositionFromPointClose(int x, int y, BOOL bDirect)
{
    return CallScintilla(SCI_POSITIONFROMPOINTCLOSE, static_cast<WPARAM>(x), static_cast<LPARAM>(y), bDirect);
}

void CScintillaCtrl::GotoLine(int line, BOOL bDirect)
{
    CallScintilla(SCI_GOTOLINE, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::GotoPos(long pos, BOOL bDirect)
{
    CallScintilla(SCI_GOTOPOS, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::SetAnchor(long posAnchor, BOOL bDirect)
{
    CallScintilla(SCI_SETANCHOR, static_cast<WPARAM>(posAnchor), 0, bDirect);
}

int CScintillaCtrl::GetCurLine(int length, char* text, BOOL bDirect)
{
    return CallScintilla(SCI_GETCURLINE, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

long CScintillaCtrl::GetEndStyled(BOOL bDirect)
{
    return CallScintilla(SCI_GETENDSTYLED, 0, 0, bDirect);
}

void CScintillaCtrl::ConvertEOLs(int eolMode, BOOL bDirect)
{
    CallScintilla(SCI_CONVERTEOLS, static_cast<WPARAM>(eolMode), 0, bDirect);
}

int CScintillaCtrl::GetEOLMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETEOLMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetEOLMode(int eolMode, BOOL bDirect)
{
    CallScintilla(SCI_SETEOLMODE, static_cast<WPARAM>(eolMode), 0, bDirect);
}

void CScintillaCtrl::StartStyling(long pos, int mask, BOOL bDirect)
{
    CallScintilla(SCI_STARTSTYLING, static_cast<WPARAM>(pos), static_cast<LPARAM>(mask), bDirect);
}

void CScintillaCtrl::SetStyling(int length, int style, BOOL bDirect)
{
    CallScintilla(SCI_SETSTYLING, static_cast<WPARAM>(length), static_cast<LPARAM>(style), bDirect);
}

BOOL CScintillaCtrl::GetBufferedDraw(BOOL bDirect)
{
    return CallScintilla(SCI_GETBUFFEREDDRAW, 0, 0, bDirect);
}

void CScintillaCtrl::SetBufferedDraw(BOOL buffered, BOOL bDirect)
{
    CallScintilla(SCI_SETBUFFEREDDRAW, static_cast<WPARAM>(buffered), 0, bDirect);
}

void CScintillaCtrl::SetTabWidth(int tabWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETTABWIDTH, static_cast<WPARAM>(tabWidth), 0, bDirect);
}

int CScintillaCtrl::GetTabWidth(BOOL bDirect)
{
    return CallScintilla(SCI_GETTABWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetCodePage(int codePage, BOOL bDirect)
{
    CallScintilla(SCI_SETCODEPAGE, static_cast<WPARAM>(codePage), 0, bDirect);
}

void CScintillaCtrl::MarkerDefine(int markerNumber, int markerSymbol, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDEFINE, static_cast<WPARAM>(markerNumber), static_cast<LPARAM>(markerSymbol), bDirect);
}

void CScintillaCtrl::MarkerSetFore(int markerNumber, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_MARKERSETFORE, static_cast<WPARAM>(markerNumber), static_cast<LPARAM>(fore), bDirect);
}

void CScintillaCtrl::MarkerSetBack(int markerNumber, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_MARKERSETBACK, static_cast<WPARAM>(markerNumber), static_cast<LPARAM>(back), bDirect);
}

void CScintillaCtrl::MarkerSetBackSelected(int markerNumber, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_MARKERSETBACKSELECTED, static_cast<WPARAM>(markerNumber), static_cast<LPARAM>(back), bDirect);
}

void CScintillaCtrl::MarkerEnableHighlight(BOOL enabled, BOOL bDirect)
{
    CallScintilla(SCI_MARKERENABLEHIGHLIGHT, static_cast<WPARAM>(enabled), 0, bDirect);
}

int CScintillaCtrl::MarkerAdd(int line, int markerNumber, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERADD, static_cast<WPARAM>(line), static_cast<LPARAM>(markerNumber), bDirect);
}

void CScintillaCtrl::MarkerDelete(int line, int markerNumber, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDELETE, static_cast<WPARAM>(line), static_cast<LPARAM>(markerNumber), bDirect);
}

void CScintillaCtrl::MarkerDeleteAll(int markerNumber, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDELETEALL, static_cast<WPARAM>(markerNumber), 0, bDirect);
}

int CScintillaCtrl::MarkerGet(int line, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERGET, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::MarkerNext(int lineStart, int markerMask, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERNEXT, static_cast<WPARAM>(lineStart), static_cast<LPARAM>(markerMask), bDirect);
}

int CScintillaCtrl::MarkerPrevious(int lineStart, int markerMask, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERPREVIOUS, static_cast<WPARAM>(lineStart), static_cast<LPARAM>(markerMask), bDirect);
}

void CScintillaCtrl::MarkerDefinePixmap(int markerNumber, const char* pixmap, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDEFINEPIXMAP, static_cast<WPARAM>(markerNumber), reinterpret_cast<LPARAM>(pixmap), bDirect);
}

void CScintillaCtrl::MarkerAddSet(int line, int set, BOOL bDirect)
{
    CallScintilla(SCI_MARKERADDSET, static_cast<WPARAM>(line), static_cast<LPARAM>(set), bDirect);
}

void CScintillaCtrl::MarkerSetAlpha(int markerNumber, int alpha, BOOL bDirect)
{
    CallScintilla(SCI_MARKERSETALPHA, static_cast<WPARAM>(markerNumber), static_cast<LPARAM>(alpha), bDirect);
}

void CScintillaCtrl::SetMarginTypeN(int margin, int marginType, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINTYPEN, static_cast<WPARAM>(margin), static_cast<LPARAM>(marginType), bDirect);
}

int CScintillaCtrl::GetMarginTypeN(int margin, BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINTYPEN, static_cast<WPARAM>(margin), 0, bDirect);
}

void CScintillaCtrl::SetMarginWidthN(int margin, int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINWIDTHN, static_cast<WPARAM>(margin), static_cast<LPARAM>(pixelWidth), bDirect);
}

int CScintillaCtrl::GetMarginWidthN(int margin, BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINWIDTHN, static_cast<WPARAM>(margin), 0, bDirect);
}

void CScintillaCtrl::SetMarginMaskN(int margin, int mask, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINMASKN, static_cast<WPARAM>(margin), static_cast<LPARAM>(mask), bDirect);
}

int CScintillaCtrl::GetMarginMaskN(int margin, BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINMASKN, static_cast<WPARAM>(margin), 0, bDirect);
}

void CScintillaCtrl::SetMarginSensitiveN(int margin, BOOL sensitive, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINSENSITIVEN, static_cast<WPARAM>(margin), static_cast<LPARAM>(sensitive), bDirect);
}

BOOL CScintillaCtrl::GetMarginSensitiveN(int margin, BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINSENSITIVEN, static_cast<WPARAM>(margin), 0, bDirect);
}

void CScintillaCtrl::SetMarginCursorN(int margin, int cursor, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINCURSORN, static_cast<WPARAM>(margin), static_cast<LPARAM>(cursor), bDirect);
}

int CScintillaCtrl::GetMarginCursorN(int margin, BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINCURSORN, static_cast<WPARAM>(margin), 0, bDirect);
}

void CScintillaCtrl::StyleClearAll(BOOL bDirect)
{
    CallScintilla(SCI_STYLECLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::StyleSetFore(int style, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETFORE, static_cast<WPARAM>(style), static_cast<LPARAM>(fore), bDirect);
}

void CScintillaCtrl::StyleSetBack(int style, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETBACK, static_cast<WPARAM>(style), static_cast<LPARAM>(back), bDirect);
}

void CScintillaCtrl::StyleSetBold(int style, BOOL bold, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETBOLD, static_cast<WPARAM>(style), static_cast<LPARAM>(bold), bDirect);
}

void CScintillaCtrl::StyleSetItalic(int style, BOOL italic, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETITALIC, static_cast<WPARAM>(style), static_cast<LPARAM>(italic), bDirect);
}

void CScintillaCtrl::StyleSetSize(int style, int sizePoints, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETSIZE, static_cast<WPARAM>(style), static_cast<LPARAM>(sizePoints), bDirect);
}

void CScintillaCtrl::StyleSetFont(int style, const char* fontName, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETFONT, static_cast<WPARAM>(style), reinterpret_cast<LPARAM>(fontName), bDirect);
}

void CScintillaCtrl::StyleSetEOLFilled(int style, BOOL filled, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETEOLFILLED, static_cast<WPARAM>(style), static_cast<LPARAM>(filled), bDirect);
}

void CScintillaCtrl::StyleResetDefault(BOOL bDirect)
{
    CallScintilla(SCI_STYLERESETDEFAULT, 0, 0, bDirect);
}

void CScintillaCtrl::StyleSetUnderline(int style, BOOL underline, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETUNDERLINE, static_cast<WPARAM>(style), static_cast<LPARAM>(underline), bDirect);
}

COLORREF CScintillaCtrl::StyleGetFore(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETFORE, static_cast<WPARAM>(style), 0, bDirect);
}

COLORREF CScintillaCtrl::StyleGetBack(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETBACK, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetBold(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETBOLD, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetItalic(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETITALIC, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::StyleGetSize(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETSIZE, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::StyleGetFont(int style, char* fontName, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETFONT, static_cast<WPARAM>(style), reinterpret_cast<LPARAM>(fontName), bDirect);
}

BOOL CScintillaCtrl::StyleGetEOLFilled(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETEOLFILLED, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetUnderline(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETUNDERLINE, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::StyleGetCase(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETCASE, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::StyleGetCharacterSet(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETCHARACTERSET, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetVisible(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETVISIBLE, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetChangeable(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETCHANGEABLE, static_cast<WPARAM>(style), 0, bDirect);
}

BOOL CScintillaCtrl::StyleGetHotSpot(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETHOTSPOT, static_cast<WPARAM>(style), 0, bDirect);
}

void CScintillaCtrl::StyleSetCase(int style, int caseForce, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETCASE, static_cast<WPARAM>(style), static_cast<LPARAM>(caseForce), bDirect);
}

void CScintillaCtrl::StyleSetSizeFractional(int style, int sizeInHundredthPoints, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETSIZEFRACTIONAL, static_cast<WPARAM>(style), static_cast<LPARAM>(sizeInHundredthPoints), bDirect);
}

int CScintillaCtrl::StyleGetSizeFractional(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETSIZEFRACTIONAL, static_cast<WPARAM>(style), 0, bDirect);
}

void CScintillaCtrl::StyleSetWeight(int style, int weight, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETWEIGHT, static_cast<WPARAM>(style), static_cast<LPARAM>(weight), bDirect);
}

int CScintillaCtrl::StyleGetWeight(int style, BOOL bDirect)
{
    return CallScintilla(SCI_STYLEGETWEIGHT, static_cast<WPARAM>(style), 0, bDirect);
}

void CScintillaCtrl::StyleSetCharacterSet(int style, int characterSet, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETCHARACTERSET, static_cast<WPARAM>(style), static_cast<LPARAM>(characterSet), bDirect);
}

void CScintillaCtrl::StyleSetHotSpot(int style, BOOL hotspot, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETHOTSPOT, static_cast<WPARAM>(style), static_cast<LPARAM>(hotspot), bDirect);
}

void CScintillaCtrl::SetSelFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETSELFORE, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(fore), bDirect);
}

void CScintillaCtrl::SetSelBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETSELBACK, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(back), bDirect);
}

int CScintillaCtrl::GetSelAlpha(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELALPHA, 0, 0, bDirect);
}

void CScintillaCtrl::SetSelAlpha(int alpha, BOOL bDirect)
{
    CallScintilla(SCI_SETSELALPHA, static_cast<WPARAM>(alpha), 0, bDirect);
}

BOOL CScintillaCtrl::GetSelEOLFilled(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELEOLFILLED, 0, 0, bDirect);
}

void CScintillaCtrl::SetSelEOLFilled(BOOL filled, BOOL bDirect)
{
    CallScintilla(SCI_SETSELEOLFILLED, static_cast<WPARAM>(filled), 0, bDirect);
}

void CScintillaCtrl::SetCaretFore(COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETFORE, static_cast<WPARAM>(fore), 0, bDirect);
}

void CScintillaCtrl::AssignCmdKey(DWORD km, int msg, BOOL bDirect)
{
    CallScintilla(SCI_ASSIGNCMDKEY, static_cast<WPARAM>(km), static_cast<LPARAM>(msg), bDirect);
}

void CScintillaCtrl::ClearCmdKey(DWORD km, BOOL bDirect)
{
    CallScintilla(SCI_CLEARCMDKEY, static_cast<WPARAM>(km), 0, bDirect);
}

void CScintillaCtrl::ClearAllCmdKeys(BOOL bDirect)
{
    CallScintilla(SCI_CLEARALLCMDKEYS, 0, 0, bDirect);
}

void CScintillaCtrl::SetStylingEx(int length, const char* styles, BOOL bDirect)
{
    CallScintilla(SCI_SETSTYLINGEX, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(styles), bDirect);
}

void CScintillaCtrl::StyleSetVisible(int style, BOOL visible, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETVISIBLE, static_cast<WPARAM>(style), static_cast<LPARAM>(visible), bDirect);
}

int CScintillaCtrl::GetCaretPeriod(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETPERIOD, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretPeriod(int periodMilliseconds, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETPERIOD, static_cast<WPARAM>(periodMilliseconds), 0, bDirect);
}

void CScintillaCtrl::SetWordChars(const char* characters, BOOL bDirect)
{
    CallScintilla(SCI_SETWORDCHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

int CScintillaCtrl::GetWordChars(char* characters, BOOL bDirect)
{
    return CallScintilla(SCI_GETWORDCHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

void CScintillaCtrl::BeginUndoAction(BOOL bDirect)
{
    CallScintilla(SCI_BEGINUNDOACTION, 0, 0, bDirect);
}

void CScintillaCtrl::EndUndoAction(BOOL bDirect)
{
    CallScintilla(SCI_ENDUNDOACTION, 0, 0, bDirect);
}

void CScintillaCtrl::IndicSetStyle(int indic, int style, BOOL bDirect)
{
    CallScintilla(SCI_INDICSETSTYLE, static_cast<WPARAM>(indic), static_cast<LPARAM>(style), bDirect);
}

int CScintillaCtrl::IndicGetStyle(int indic, BOOL bDirect)
{
    return CallScintilla(SCI_INDICGETSTYLE, static_cast<WPARAM>(indic), 0, bDirect);
}

void CScintillaCtrl::IndicSetFore(int indic, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_INDICSETFORE, static_cast<WPARAM>(indic), static_cast<LPARAM>(fore), bDirect);
}

COLORREF CScintillaCtrl::IndicGetFore(int indic, BOOL bDirect)
{
    return CallScintilla(SCI_INDICGETFORE, static_cast<WPARAM>(indic), 0, bDirect);
}

void CScintillaCtrl::IndicSetUnder(int indic, BOOL under, BOOL bDirect)
{
    CallScintilla(SCI_INDICSETUNDER, static_cast<WPARAM>(indic), static_cast<LPARAM>(under), bDirect);
}

BOOL CScintillaCtrl::IndicGetUnder(int indic, BOOL bDirect)
{
    return CallScintilla(SCI_INDICGETUNDER, static_cast<WPARAM>(indic), 0, bDirect);
}

void CScintillaCtrl::SetWhitespaceFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETWHITESPACEFORE, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(fore), bDirect);
}

void CScintillaCtrl::SetWhitespaceBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETWHITESPACEBACK, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(back), bDirect);
}

void CScintillaCtrl::SetWhitespaceSize(int size, BOOL bDirect)
{
    CallScintilla(SCI_SETWHITESPACESIZE, static_cast<WPARAM>(size), 0, bDirect);
}

int CScintillaCtrl::GetWhitespaceSize(BOOL bDirect)
{
    return CallScintilla(SCI_GETWHITESPACESIZE, 0, 0, bDirect);
}

void CScintillaCtrl::SetStyleBits(int bits, BOOL bDirect)
{
    CallScintilla(SCI_SETSTYLEBITS, static_cast<WPARAM>(bits), 0, bDirect);
}

int CScintillaCtrl::GetStyleBits(BOOL bDirect)
{
    return CallScintilla(SCI_GETSTYLEBITS, 0, 0, bDirect);
}

void CScintillaCtrl::SetLineState(int line, int state, BOOL bDirect)
{
    CallScintilla(SCI_SETLINESTATE, static_cast<WPARAM>(line), static_cast<LPARAM>(state), bDirect);
}

int CScintillaCtrl::GetLineState(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINESTATE, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::GetMaxLineState(BOOL bDirect)
{
    return CallScintilla(SCI_GETMAXLINESTATE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetCaretLineVisible(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETLINEVISIBLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineVisible(BOOL show, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETLINEVISIBLE, static_cast<WPARAM>(show), 0, bDirect);
}

COLORREF CScintillaCtrl::GetCaretLineBack(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETLINEBACK, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineBack(COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETLINEBACK, static_cast<WPARAM>(back), 0, bDirect);
}

void CScintillaCtrl::StyleSetChangeable(int style, BOOL changeable, BOOL bDirect)
{
    CallScintilla(SCI_STYLESETCHANGEABLE, static_cast<WPARAM>(style), static_cast<LPARAM>(changeable), bDirect);
}

void CScintillaCtrl::AutoCShow(int lenEntered, const char* itemList, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSHOW, static_cast<WPARAM>(lenEntered), reinterpret_cast<LPARAM>(itemList), bDirect);
}

void CScintillaCtrl::AutoCCancel(BOOL bDirect)
{
    CallScintilla(SCI_AUTOCCANCEL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::AutoCActive(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCACTIVE, 0, 0, bDirect);
}

long CScintillaCtrl::AutoCPosStart(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCPOSSTART, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCComplete(BOOL bDirect)
{
    CallScintilla(SCI_AUTOCCOMPLETE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCStops(const char* characterSet, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSTOPS, 0, reinterpret_cast<LPARAM>(characterSet), bDirect);
}

void CScintillaCtrl::AutoCSetSeparator(int separatorCharacter, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETSEPARATOR, static_cast<WPARAM>(separatorCharacter), 0, bDirect);
}

int CScintillaCtrl::AutoCGetSeparator(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETSEPARATOR, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSelect(const char* text, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSELECT, 0, reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::AutoCSetCancelAtStart(BOOL cancel, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETCANCELATSTART, static_cast<WPARAM>(cancel), 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetCancelAtStart(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETCANCELATSTART, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetFillUps(const char* characterSet, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETFILLUPS, 0, reinterpret_cast<LPARAM>(characterSet), bDirect);
}

void CScintillaCtrl::AutoCSetChooseSingle(BOOL chooseSingle, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETCHOOSESINGLE, static_cast<WPARAM>(chooseSingle), 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetChooseSingle(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETCHOOSESINGLE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetIgnoreCase(BOOL ignoreCase, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETIGNORECASE, static_cast<WPARAM>(ignoreCase), 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetIgnoreCase(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETIGNORECASE, 0, 0, bDirect);
}

void CScintillaCtrl::UserListShow(int listType, const char* itemList, BOOL bDirect)
{
    CallScintilla(SCI_USERLISTSHOW, static_cast<WPARAM>(listType), reinterpret_cast<LPARAM>(itemList), bDirect);
}

void CScintillaCtrl::AutoCSetAutoHide(BOOL autoHide, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETAUTOHIDE, static_cast<WPARAM>(autoHide), 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetAutoHide(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETAUTOHIDE, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetDropRestOfWord(BOOL dropRestOfWord, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETDROPRESTOFWORD, static_cast<WPARAM>(dropRestOfWord), 0, bDirect);
}

BOOL CScintillaCtrl::AutoCGetDropRestOfWord(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETDROPRESTOFWORD, 0, 0, bDirect);
}

void CScintillaCtrl::RegisterImage(int type, const char* xpmData, BOOL bDirect)
{
    CallScintilla(SCI_REGISTERIMAGE, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(xpmData), bDirect);
}

void CScintillaCtrl::ClearRegisteredImages(BOOL bDirect)
{
    CallScintilla(SCI_CLEARREGISTEREDIMAGES, 0, 0, bDirect);
}

int CScintillaCtrl::AutoCGetTypeSeparator(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETTYPESEPARATOR, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetTypeSeparator(int separatorCharacter, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETTYPESEPARATOR, static_cast<WPARAM>(separatorCharacter), 0, bDirect);
}

void CScintillaCtrl::AutoCSetMaxWidth(int characterCount, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETMAXWIDTH, static_cast<WPARAM>(characterCount), 0, bDirect);
}

int CScintillaCtrl::AutoCGetMaxWidth(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETMAXWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetMaxHeight(int rowCount, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETMAXHEIGHT, static_cast<WPARAM>(rowCount), 0, bDirect);
}

int CScintillaCtrl::AutoCGetMaxHeight(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETMAXHEIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::SetIndent(int indentSize, BOOL bDirect)
{
    CallScintilla(SCI_SETINDENT, static_cast<WPARAM>(indentSize), 0, bDirect);
}

int CScintillaCtrl::GetIndent(BOOL bDirect)
{
    return CallScintilla(SCI_GETINDENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetUseTabs(BOOL useTabs, BOOL bDirect)
{
    CallScintilla(SCI_SETUSETABS, static_cast<WPARAM>(useTabs), 0, bDirect);
}

BOOL CScintillaCtrl::GetUseTabs(BOOL bDirect)
{
    return CallScintilla(SCI_GETUSETABS, 0, 0, bDirect);
}

void CScintillaCtrl::SetLineIndentation(int line, int indentSize, BOOL bDirect)
{
    CallScintilla(SCI_SETLINEINDENTATION, static_cast<WPARAM>(line), static_cast<LPARAM>(indentSize), bDirect);
}

int CScintillaCtrl::GetLineIndentation(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEINDENTATION, static_cast<WPARAM>(line), 0, bDirect);
}

long CScintillaCtrl::GetLineIndentPosition(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEINDENTPOSITION, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::GetColumn(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_GETCOLUMN, static_cast<WPARAM>(pos), 0, bDirect);
}

int CScintillaCtrl::CountCharacters(int startPos, int endPos, BOOL bDirect)
{
    return CallScintilla(SCI_COUNTCHARACTERS, static_cast<WPARAM>(startPos), static_cast<LPARAM>(endPos), bDirect);
}

void CScintillaCtrl::SetHScrollBar(BOOL show, BOOL bDirect)
{
    CallScintilla(SCI_SETHSCROLLBAR, static_cast<WPARAM>(show), 0, bDirect);
}

BOOL CScintillaCtrl::GetHScrollBar(BOOL bDirect)
{
    return CallScintilla(SCI_GETHSCROLLBAR, 0, 0, bDirect);
}

void CScintillaCtrl::SetIndentationGuides(int indentView, BOOL bDirect)
{
    CallScintilla(SCI_SETINDENTATIONGUIDES, static_cast<WPARAM>(indentView), 0, bDirect);
}

int CScintillaCtrl::GetIndentationGuides(BOOL bDirect)
{
    return CallScintilla(SCI_GETINDENTATIONGUIDES, 0, 0, bDirect);
}

void CScintillaCtrl::SetHighlightGuide(int column, BOOL bDirect)
{
    CallScintilla(SCI_SETHIGHLIGHTGUIDE, static_cast<WPARAM>(column), 0, bDirect);
}

int CScintillaCtrl::GetHighlightGuide(BOOL bDirect)
{
    return CallScintilla(SCI_GETHIGHLIGHTGUIDE, 0, 0, bDirect);
}

long CScintillaCtrl::GetLineEndPosition(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEENDPOSITION, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::GetCodePage(BOOL bDirect)
{
    return CallScintilla(SCI_GETCODEPAGE, 0, 0, bDirect);
}

COLORREF CScintillaCtrl::GetCaretFore(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETFORE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetReadOnly(BOOL bDirect)
{
    return CallScintilla(SCI_GETREADONLY, 0, 0, bDirect);
}

void CScintillaCtrl::SetCurrentPos(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETCURRENTPOS, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::SetSelectionStart(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONSTART, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetSelectionStart(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONSTART, 0, 0, bDirect);
}

void CScintillaCtrl::SetSelectionEnd(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONEND, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetSelectionEnd(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetEmptySelection(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETEMPTYSELECTION, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::SetPrintMagnification(int magnification, BOOL bDirect)
{
    CallScintilla(SCI_SETPRINTMAGNIFICATION, static_cast<WPARAM>(magnification), 0, bDirect);
}

int CScintillaCtrl::GetPrintMagnification(BOOL bDirect)
{
    return CallScintilla(SCI_GETPRINTMAGNIFICATION, 0, 0, bDirect);
}

void CScintillaCtrl::SetPrintColourMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETPRINTCOLOURMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetPrintColourMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETPRINTCOLOURMODE, 0, 0, bDirect);
}

long CScintillaCtrl::FindText(int flags, TextToFind* ft, BOOL bDirect)
{
    return CallScintilla(SCI_FINDTEXT, static_cast<WPARAM>(flags), reinterpret_cast<LPARAM>(ft), bDirect);
}

long CScintillaCtrl::FormatRange(BOOL draw, RangeToFormat* fr, BOOL bDirect)
{
    return CallScintilla(SCI_FORMATRANGE, static_cast<WPARAM>(draw), reinterpret_cast<LPARAM>(fr), bDirect);
}

int CScintillaCtrl::GetFirstVisibleLine(BOOL bDirect)
{
    return CallScintilla(SCI_GETFIRSTVISIBLELINE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLine(int line, char* text, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINE, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::GetLineCount(BOOL bDirect)
{
    return CallScintilla(SCI_GETLINECOUNT, 0, 0, bDirect);
}

void CScintillaCtrl::SetMarginLeft(int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINLEFT, 0, static_cast<LPARAM>(pixelWidth), bDirect);
}

int CScintillaCtrl::GetMarginLeft(BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::SetMarginRight(int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINRIGHT, 0, static_cast<LPARAM>(pixelWidth), bDirect);
}

int CScintillaCtrl::GetMarginRight(BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINRIGHT, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetModify(BOOL bDirect)
{
    return CallScintilla(SCI_GETMODIFY, 0, 0, bDirect);
}

void CScintillaCtrl::SetSel(long start, long end, BOOL bDirect)
{
    CallScintilla(SCI_SETSEL, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

int CScintillaCtrl::GetSelText(char* text, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELTEXT, 0, reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::GetTextRange(TextRange* tr, BOOL bDirect)
{
    return CallScintilla(SCI_GETTEXTRANGE, 0, reinterpret_cast<LPARAM>(tr), bDirect);
}

void CScintillaCtrl::HideSelection(BOOL normal, BOOL bDirect)
{
    CallScintilla(SCI_HIDESELECTION, static_cast<WPARAM>(normal), 0, bDirect);
}

int CScintillaCtrl::PointXFromPosition(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_POINTXFROMPOSITION, 0, static_cast<LPARAM>(pos), bDirect);
}

int CScintillaCtrl::PointYFromPosition(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_POINTYFROMPOSITION, 0, static_cast<LPARAM>(pos), bDirect);
}

int CScintillaCtrl::LineFromPosition(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_LINEFROMPOSITION, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::PositionFromLine(int line, BOOL bDirect)
{
    return CallScintilla(SCI_POSITIONFROMLINE, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::LineScroll(int columns, int lines, BOOL bDirect)
{
    CallScintilla(SCI_LINESCROLL, static_cast<WPARAM>(columns), static_cast<LPARAM>(lines), bDirect);
}

void CScintillaCtrl::ScrollCaret(BOOL bDirect)
{
    CallScintilla(SCI_SCROLLCARET, 0, 0, bDirect);
}

void CScintillaCtrl::ScrollRange(long secondary, long primary, BOOL bDirect)
{
    CallScintilla(SCI_SCROLLRANGE, static_cast<WPARAM>(secondary), static_cast<LPARAM>(primary), bDirect);
}

void CScintillaCtrl::ReplaceSel(const char* text, BOOL bDirect)
{
    CallScintilla(SCI_REPLACESEL, 0, reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::SetReadOnly(BOOL readOnly, BOOL bDirect)
{
    CallScintilla(SCI_SETREADONLY, static_cast<WPARAM>(readOnly), 0, bDirect);
}

void CScintillaCtrl::Null(BOOL bDirect)
{
    CallScintilla(SCI_NULL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CanPaste(BOOL bDirect)
{
    return CallScintilla(SCI_CANPASTE, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CanUndo(BOOL bDirect)
{
    return CallScintilla(SCI_CANUNDO, 0, 0, bDirect);
}

void CScintillaCtrl::EmptyUndoBuffer(BOOL bDirect)
{
    CallScintilla(SCI_EMPTYUNDOBUFFER, 0, 0, bDirect);
}

void CScintillaCtrl::Undo(BOOL bDirect)
{
    CallScintilla(SCI_UNDO, 0, 0, bDirect);
}

void CScintillaCtrl::Cut(BOOL bDirect)
{
    CallScintilla(SCI_CUT, 0, 0, bDirect);
}

void CScintillaCtrl::Copy(BOOL bDirect)
{
    CallScintilla(SCI_COPY, 0, 0, bDirect);
}

void CScintillaCtrl::Paste(BOOL bDirect)
{
    CallScintilla(SCI_PASTE, 0, 0, bDirect);
}

void CScintillaCtrl::Clear(BOOL bDirect)
{
    CallScintilla(SCI_CLEAR, 0, 0, bDirect);
}

void CScintillaCtrl::SetText(const char* text, BOOL bDirect)
{
    CallScintilla(SCI_SETTEXT, 0, reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::GetText(int length, char* text, BOOL bDirect)
{
    return CallScintilla(SCI_GETTEXT, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::GetTextLength(BOOL bDirect)
{
    return CallScintilla(SCI_GETTEXTLENGTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetOvertype(BOOL overtype, BOOL bDirect)
{
    CallScintilla(SCI_SETOVERTYPE, static_cast<WPARAM>(overtype), 0, bDirect);
}

BOOL CScintillaCtrl::GetOvertype(BOOL bDirect)
{
    return CallScintilla(SCI_GETOVERTYPE, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretWidth(int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETWIDTH, static_cast<WPARAM>(pixelWidth), 0, bDirect);
}

int CScintillaCtrl::GetCaretWidth(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetTargetStart(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETTARGETSTART, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetTargetStart(BOOL bDirect)
{
    return CallScintilla(SCI_GETTARGETSTART, 0, 0, bDirect);
}

void CScintillaCtrl::SetTargetEnd(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETTARGETEND, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetTargetEnd(BOOL bDirect)
{
    return CallScintilla(SCI_GETTARGETEND, 0, 0, bDirect);
}

int CScintillaCtrl::ReplaceTarget(int length, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_REPLACETARGET, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::ReplaceTargetRE(int length, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_REPLACETARGETRE, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::SearchInTarget(int length, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_SEARCHINTARGET, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::SetSearchFlags(int flags, BOOL bDirect)
{
    CallScintilla(SCI_SETSEARCHFLAGS, static_cast<WPARAM>(flags), 0, bDirect);
}

int CScintillaCtrl::GetSearchFlags(BOOL bDirect)
{
    return CallScintilla(SCI_GETSEARCHFLAGS, 0, 0, bDirect);
}

void CScintillaCtrl::CallTipShow(long pos, const char* definition, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSHOW, static_cast<WPARAM>(pos), reinterpret_cast<LPARAM>(definition), bDirect);
}

void CScintillaCtrl::CallTipCancel(BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPCANCEL, 0, 0, bDirect);
}

BOOL CScintillaCtrl::CallTipActive(BOOL bDirect)
{
    return CallScintilla(SCI_CALLTIPACTIVE, 0, 0, bDirect);
}

long CScintillaCtrl::CallTipPosStart(BOOL bDirect)
{
    return CallScintilla(SCI_CALLTIPPOSSTART, 0, 0, bDirect);
}

void CScintillaCtrl::CallTipSetHlt(int start, int end, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSETHLT, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

void CScintillaCtrl::CallTipSetBack(COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSETBACK, static_cast<WPARAM>(back), 0, bDirect);
}

void CScintillaCtrl::CallTipSetFore(COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSETFORE, static_cast<WPARAM>(fore), 0, bDirect);
}

void CScintillaCtrl::CallTipSetForeHlt(COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSETFOREHLT, static_cast<WPARAM>(fore), 0, bDirect);
}

void CScintillaCtrl::CallTipUseStyle(int tabSize, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPUSESTYLE, static_cast<WPARAM>(tabSize), 0, bDirect);
}

void CScintillaCtrl::CallTipSetPosition(BOOL above, BOOL bDirect)
{
    CallScintilla(SCI_CALLTIPSETPOSITION, static_cast<WPARAM>(above), 0, bDirect);
}

int CScintillaCtrl::VisibleFromDocLine(int line, BOOL bDirect)
{
    return CallScintilla(SCI_VISIBLEFROMDOCLINE, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::DocLineFromVisible(int lineDisplay, BOOL bDirect)
{
    return CallScintilla(SCI_DOCLINEFROMVISIBLE, static_cast<WPARAM>(lineDisplay), 0, bDirect);
}

int CScintillaCtrl::WrapCount(int line, BOOL bDirect)
{
    return CallScintilla(SCI_WRAPCOUNT, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::SetFoldLevel(int line, int level, BOOL bDirect)
{
    CallScintilla(SCI_SETFOLDLEVEL, static_cast<WPARAM>(line), static_cast<LPARAM>(level), bDirect);
}

int CScintillaCtrl::GetFoldLevel(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETFOLDLEVEL, static_cast<WPARAM>(line), 0, bDirect);
}

int CScintillaCtrl::GetLastChild(int line, int level, BOOL bDirect)
{
    return CallScintilla(SCI_GETLASTCHILD, static_cast<WPARAM>(line), static_cast<LPARAM>(level), bDirect);
}

int CScintillaCtrl::GetFoldParent(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETFOLDPARENT, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::ShowLines(int lineStart, int lineEnd, BOOL bDirect)
{
    CallScintilla(SCI_SHOWLINES, static_cast<WPARAM>(lineStart), static_cast<LPARAM>(lineEnd), bDirect);
}

void CScintillaCtrl::HideLines(int lineStart, int lineEnd, BOOL bDirect)
{
    CallScintilla(SCI_HIDELINES, static_cast<WPARAM>(lineStart), static_cast<LPARAM>(lineEnd), bDirect);
}

BOOL CScintillaCtrl::GetLineVisible(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEVISIBLE, static_cast<WPARAM>(line), 0, bDirect);
}

BOOL CScintillaCtrl::GetAllLinesVisible(BOOL bDirect)
{
    return CallScintilla(SCI_GETALLLINESVISIBLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetFoldExpanded(int line, BOOL expanded, BOOL bDirect)
{
    CallScintilla(SCI_SETFOLDEXPANDED, static_cast<WPARAM>(line), static_cast<LPARAM>(expanded), bDirect);
}

BOOL CScintillaCtrl::GetFoldExpanded(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETFOLDEXPANDED, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::ToggleFold(int line, BOOL bDirect)
{
    CallScintilla(SCI_TOGGLEFOLD, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::FoldLine(int line, int action, BOOL bDirect)
{
    CallScintilla(SCI_FOLDLINE, static_cast<WPARAM>(line), static_cast<LPARAM>(action), bDirect);
}

void CScintillaCtrl::FoldChildren(int line, int action, BOOL bDirect)
{
    CallScintilla(SCI_FOLDCHILDREN, static_cast<WPARAM>(line), static_cast<LPARAM>(action), bDirect);
}

void CScintillaCtrl::ExpandChildren(int line, int level, BOOL bDirect)
{
    CallScintilla(SCI_EXPANDCHILDREN, static_cast<WPARAM>(line), static_cast<LPARAM>(level), bDirect);
}

void CScintillaCtrl::FoldAll(int action, BOOL bDirect)
{
    CallScintilla(SCI_FOLDALL, static_cast<WPARAM>(action), 0, bDirect);
}

void CScintillaCtrl::EnsureVisible(int line, BOOL bDirect)
{
    CallScintilla(SCI_ENSUREVISIBLE, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::SetAutomaticFold(int automaticFold, BOOL bDirect)
{
    CallScintilla(SCI_SETAUTOMATICFOLD, static_cast<WPARAM>(automaticFold), 0, bDirect);
}

int CScintillaCtrl::GetAutomaticFold(BOOL bDirect)
{
    return CallScintilla(SCI_GETAUTOMATICFOLD, 0, 0, bDirect);
}

void CScintillaCtrl::SetFoldFlags(int flags, BOOL bDirect)
{
    CallScintilla(SCI_SETFOLDFLAGS, static_cast<WPARAM>(flags), 0, bDirect);
}

void CScintillaCtrl::EnsureVisibleEnforcePolicy(int line, BOOL bDirect)
{
    CallScintilla(SCI_ENSUREVISIBLEENFORCEPOLICY, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::SetTabIndents(BOOL tabIndents, BOOL bDirect)
{
    CallScintilla(SCI_SETTABINDENTS, static_cast<WPARAM>(tabIndents), 0, bDirect);
}

BOOL CScintillaCtrl::GetTabIndents(BOOL bDirect)
{
    return CallScintilla(SCI_GETTABINDENTS, 0, 0, bDirect);
}

void CScintillaCtrl::SetBackSpaceUnIndents(BOOL bsUnIndents, BOOL bDirect)
{
    CallScintilla(SCI_SETBACKSPACEUNINDENTS, static_cast<WPARAM>(bsUnIndents), 0, bDirect);
}

BOOL CScintillaCtrl::GetBackSpaceUnIndents(BOOL bDirect)
{
    return CallScintilla(SCI_GETBACKSPACEUNINDENTS, 0, 0, bDirect);
}

void CScintillaCtrl::SetMouseDwellTime(int periodMilliseconds, BOOL bDirect)
{
    CallScintilla(SCI_SETMOUSEDWELLTIME, static_cast<WPARAM>(periodMilliseconds), 0, bDirect);
}

int CScintillaCtrl::GetMouseDwellTime(BOOL bDirect)
{
    return CallScintilla(SCI_GETMOUSEDWELLTIME, 0, 0, bDirect);
}

int CScintillaCtrl::WordStartPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect)
{
    return CallScintilla(SCI_WORDSTARTPOSITION, static_cast<WPARAM>(pos), static_cast<LPARAM>(onlyWordCharacters), bDirect);
}

int CScintillaCtrl::WordEndPosition(long pos, BOOL onlyWordCharacters, BOOL bDirect)
{
    return CallScintilla(SCI_WORDENDPOSITION, static_cast<WPARAM>(pos), static_cast<LPARAM>(onlyWordCharacters), bDirect);
}

void CScintillaCtrl::SetWrapMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETWRAPMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetWrapMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETWRAPMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapVisualFlags(int wrapVisualFlags, BOOL bDirect)
{
    CallScintilla(SCI_SETWRAPVISUALFLAGS, static_cast<WPARAM>(wrapVisualFlags), 0, bDirect);
}

int CScintillaCtrl::GetWrapVisualFlags(BOOL bDirect)
{
    return CallScintilla(SCI_GETWRAPVISUALFLAGS, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapVisualFlagsLocation(int wrapVisualFlagsLocation, BOOL bDirect)
{
    CallScintilla(SCI_SETWRAPVISUALFLAGSLOCATION, static_cast<WPARAM>(wrapVisualFlagsLocation), 0, bDirect);
}

int CScintillaCtrl::GetWrapVisualFlagsLocation(BOOL bDirect)
{
    return CallScintilla(SCI_GETWRAPVISUALFLAGSLOCATION, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapStartIndent(int indent, BOOL bDirect)
{
    CallScintilla(SCI_SETWRAPSTARTINDENT, static_cast<WPARAM>(indent), 0, bDirect);
}

int CScintillaCtrl::GetWrapStartIndent(BOOL bDirect)
{
    return CallScintilla(SCI_GETWRAPSTARTINDENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetWrapIndentMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETWRAPINDENTMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetWrapIndentMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETWRAPINDENTMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetLayoutCache(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETLAYOUTCACHE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetLayoutCache(BOOL bDirect)
{
    return CallScintilla(SCI_GETLAYOUTCACHE, 0, 0, bDirect);
}

void CScintillaCtrl::SetScrollWidth(int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_SETSCROLLWIDTH, static_cast<WPARAM>(pixelWidth), 0, bDirect);
}

int CScintillaCtrl::GetScrollWidth(BOOL bDirect)
{
    return CallScintilla(SCI_GETSCROLLWIDTH, 0, 0, bDirect);
}

void CScintillaCtrl::SetScrollWidthTracking(BOOL tracking, BOOL bDirect)
{
    CallScintilla(SCI_SETSCROLLWIDTHTRACKING, static_cast<WPARAM>(tracking), 0, bDirect);
}

BOOL CScintillaCtrl::GetScrollWidthTracking(BOOL bDirect)
{
    return CallScintilla(SCI_GETSCROLLWIDTHTRACKING, 0, 0, bDirect);
}

int CScintillaCtrl::TextWidth(int style, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_TEXTWIDTH, static_cast<WPARAM>(style), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::SetEndAtLastLine(BOOL endAtLastLine, BOOL bDirect)
{
    CallScintilla(SCI_SETENDATLASTLINE, static_cast<WPARAM>(endAtLastLine), 0, bDirect);
}

BOOL CScintillaCtrl::GetEndAtLastLine(BOOL bDirect)
{
    return CallScintilla(SCI_GETENDATLASTLINE, 0, 0, bDirect);
}

int CScintillaCtrl::TextHeight(int line, BOOL bDirect)
{
    return CallScintilla(SCI_TEXTHEIGHT, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::SetVScrollBar(BOOL show, BOOL bDirect)
{
    CallScintilla(SCI_SETVSCROLLBAR, static_cast<WPARAM>(show), 0, bDirect);
}

BOOL CScintillaCtrl::GetVScrollBar(BOOL bDirect)
{
    return CallScintilla(SCI_GETVSCROLLBAR, 0, 0, bDirect);
}

void CScintillaCtrl::AppendText(int length, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_APPENDTEXT, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

BOOL CScintillaCtrl::GetTwoPhaseDraw(BOOL bDirect)
{
    return CallScintilla(SCI_GETTWOPHASEDRAW, 0, 0, bDirect);
}

void CScintillaCtrl::SetTwoPhaseDraw(BOOL twoPhase, BOOL bDirect)
{
    CallScintilla(SCI_SETTWOPHASEDRAW, static_cast<WPARAM>(twoPhase), 0, bDirect);
}

void CScintillaCtrl::SetFontQuality(int fontQuality, BOOL bDirect)
{
    CallScintilla(SCI_SETFONTQUALITY, static_cast<WPARAM>(fontQuality), 0, bDirect);
}

int CScintillaCtrl::GetFontQuality(BOOL bDirect)
{
    return CallScintilla(SCI_GETFONTQUALITY, 0, 0, bDirect);
}

void CScintillaCtrl::SetFirstVisibleLine(int lineDisplay, BOOL bDirect)
{
    CallScintilla(SCI_SETFIRSTVISIBLELINE, static_cast<WPARAM>(lineDisplay), 0, bDirect);
}

void CScintillaCtrl::SetMultiPaste(int multiPaste, BOOL bDirect)
{
    CallScintilla(SCI_SETMULTIPASTE, static_cast<WPARAM>(multiPaste), 0, bDirect);
}

int CScintillaCtrl::GetMultiPaste(BOOL bDirect)
{
    return CallScintilla(SCI_GETMULTIPASTE, 0, 0, bDirect);
}

int CScintillaCtrl::GetTag(int tagNumber, char* tagValue, BOOL bDirect)
{
    return CallScintilla(SCI_GETTAG, static_cast<WPARAM>(tagNumber), reinterpret_cast<LPARAM>(tagValue), bDirect);
}

void CScintillaCtrl::TargetFromSelection(BOOL bDirect)
{
    CallScintilla(SCI_TARGETFROMSELECTION, 0, 0, bDirect);
}

void CScintillaCtrl::LinesJoin(BOOL bDirect)
{
    CallScintilla(SCI_LINESJOIN, 0, 0, bDirect);
}

void CScintillaCtrl::LinesSplit(int pixelWidth, BOOL bDirect)
{
    CallScintilla(SCI_LINESSPLIT, static_cast<WPARAM>(pixelWidth), 0, bDirect);
}

void CScintillaCtrl::SetFoldMarginColour(BOOL useSetting, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETFOLDMARGINCOLOUR, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(back), bDirect);
}

void CScintillaCtrl::SetFoldMarginHiColour(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETFOLDMARGINHICOLOUR, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(fore), bDirect);
}

void CScintillaCtrl::LineDown(BOOL bDirect)
{
    CallScintilla(SCI_LINEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::LineDownExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineUp(BOOL bDirect)
{
    CallScintilla(SCI_LINEUP, 0, 0, bDirect);
}

void CScintillaCtrl::LineUpExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeft(BOOL bDirect)
{
    CallScintilla(SCI_CHARLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeftExtend(BOOL bDirect)
{
    CallScintilla(SCI_CHARLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharRight(BOOL bDirect)
{
    CallScintilla(SCI_CHARRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::CharRightExtend(BOOL bDirect)
{
    CallScintilla(SCI_CHARRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeft(BOOL bDirect)
{
    CallScintilla(SCI_WORDLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRight(BOOL bDirect)
{
    CallScintilla(SCI_WORDRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::Home(BOOL bDirect)
{
    CallScintilla(SCI_HOME, 0, 0, bDirect);
}

void CScintillaCtrl::HomeExtend(BOOL bDirect)
{
    CallScintilla(SCI_HOMEEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEnd(BOOL bDirect)
{
    CallScintilla(SCI_LINEEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentStart(BOOL bDirect)
{
    CallScintilla(SCI_DOCUMENTSTART, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentStartExtend(BOOL bDirect)
{
    CallScintilla(SCI_DOCUMENTSTARTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentEnd(BOOL bDirect)
{
    CallScintilla(SCI_DOCUMENTEND, 0, 0, bDirect);
}

void CScintillaCtrl::DocumentEndExtend(BOOL bDirect)
{
    CallScintilla(SCI_DOCUMENTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageUp(BOOL bDirect)
{
    CallScintilla(SCI_PAGEUP, 0, 0, bDirect);
}

void CScintillaCtrl::PageUpExtend(BOOL bDirect)
{
    CallScintilla(SCI_PAGEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageDown(BOOL bDirect)
{
    CallScintilla(SCI_PAGEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::PageDownExtend(BOOL bDirect)
{
    CallScintilla(SCI_PAGEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::EditToggleOvertype(BOOL bDirect)
{
    CallScintilla(SCI_EDITTOGGLEOVERTYPE, 0, 0, bDirect);
}

void CScintillaCtrl::Cancel(BOOL bDirect)
{
    CallScintilla(SCI_CANCEL, 0, 0, bDirect);
}

void CScintillaCtrl::DeleteBack(BOOL bDirect)
{
    CallScintilla(SCI_DELETEBACK, 0, 0, bDirect);
}

void CScintillaCtrl::Tab(BOOL bDirect)
{
    CallScintilla(SCI_TAB, 0, 0, bDirect);
}

void CScintillaCtrl::BackTab(BOOL bDirect)
{
    CallScintilla(SCI_BACKTAB, 0, 0, bDirect);
}

void CScintillaCtrl::NewLine(BOOL bDirect)
{
    CallScintilla(SCI_NEWLINE, 0, 0, bDirect);
}

void CScintillaCtrl::FormFeed(BOOL bDirect)
{
    CallScintilla(SCI_FORMFEED, 0, 0, bDirect);
}

void CScintillaCtrl::VCHome(BOOL bDirect)
{
    CallScintilla(SCI_VCHOME, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeExtend(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMEEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::ZoomIn(BOOL bDirect)
{
    CallScintilla(SCI_ZOOMIN, 0, 0, bDirect);
}

void CScintillaCtrl::ZoomOut(BOOL bDirect)
{
    CallScintilla(SCI_ZOOMOUT, 0, 0, bDirect);
}

void CScintillaCtrl::DelWordLeft(BOOL bDirect)
{
    CallScintilla(SCI_DELWORDLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::DelWordRight(BOOL bDirect)
{
    CallScintilla(SCI_DELWORDRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::DelWordRightEnd(BOOL bDirect)
{
    CallScintilla(SCI_DELWORDRIGHTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineCut(BOOL bDirect)
{
    CallScintilla(SCI_LINECUT, 0, 0, bDirect);
}

void CScintillaCtrl::LineDelete(BOOL bDirect)
{
    CallScintilla(SCI_LINEDELETE, 0, 0, bDirect);
}

void CScintillaCtrl::LineTranspose(BOOL bDirect)
{
    CallScintilla(SCI_LINETRANSPOSE, 0, 0, bDirect);
}

void CScintillaCtrl::LineDuplicate(BOOL bDirect)
{
    CallScintilla(SCI_LINEDUPLICATE, 0, 0, bDirect);
}

void CScintillaCtrl::LowerCase(BOOL bDirect)
{
    CallScintilla(SCI_LOWERCASE, 0, 0, bDirect);
}

void CScintillaCtrl::UpperCase(BOOL bDirect)
{
    CallScintilla(SCI_UPPERCASE, 0, 0, bDirect);
}

void CScintillaCtrl::LineScrollDown(BOOL bDirect)
{
    CallScintilla(SCI_LINESCROLLDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::LineScrollUp(BOOL bDirect)
{
    CallScintilla(SCI_LINESCROLLUP, 0, 0, bDirect);
}

void CScintillaCtrl::DeleteBackNotLine(BOOL bDirect)
{
    CallScintilla(SCI_DELETEBACKNOTLINE, 0, 0, bDirect);
}

void CScintillaCtrl::HomeDisplay(BOOL bDirect)
{
    CallScintilla(SCI_HOMEDISPLAY, 0, 0, bDirect);
}

void CScintillaCtrl::HomeDisplayExtend(BOOL bDirect)
{
    CallScintilla(SCI_HOMEDISPLAYEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndDisplay(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDDISPLAY, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndDisplayExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDDISPLAYEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::HomeWrap(BOOL bDirect)
{
    CallScintilla(SCI_HOMEWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::HomeWrapExtend(BOOL bDirect)
{
    CallScintilla(SCI_HOMEWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndWrap(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndWrapExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeWrap(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMEWRAP, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeWrapExtend(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMEWRAPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineCopy(BOOL bDirect)
{
    CallScintilla(SCI_LINECOPY, 0, 0, bDirect);
}

void CScintillaCtrl::MoveCaretInsideView(BOOL bDirect)
{
    CallScintilla(SCI_MOVECARETINSIDEVIEW, 0, 0, bDirect);
}

int CScintillaCtrl::LineLength(int line, BOOL bDirect)
{
    return CallScintilla(SCI_LINELENGTH, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::BraceHighlight(long pos1, long pos2, BOOL bDirect)
{
    CallScintilla(SCI_BRACEHIGHLIGHT, static_cast<WPARAM>(pos1), static_cast<LPARAM>(pos2), bDirect);
}

void CScintillaCtrl::BraceHighlightIndicator(BOOL useBraceHighlightIndicator, int indicator, BOOL bDirect)
{
    CallScintilla(SCI_BRACEHIGHLIGHTINDICATOR, static_cast<WPARAM>(useBraceHighlightIndicator), static_cast<LPARAM>(indicator), bDirect);
}

void CScintillaCtrl::BraceBadLight(long pos, BOOL bDirect)
{
    CallScintilla(SCI_BRACEBADLIGHT, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::BraceBadLightIndicator(BOOL useBraceBadLightIndicator, int indicator, BOOL bDirect)
{
    CallScintilla(SCI_BRACEBADLIGHTINDICATOR, static_cast<WPARAM>(useBraceBadLightIndicator), static_cast<LPARAM>(indicator), bDirect);
}

long CScintillaCtrl::BraceMatch(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_BRACEMATCH, static_cast<WPARAM>(pos), 0, bDirect);
}

BOOL CScintillaCtrl::GetViewEOL(BOOL bDirect)
{
    return CallScintilla(SCI_GETVIEWEOL, 0, 0, bDirect);
}

void CScintillaCtrl::SetViewEOL(BOOL visible, BOOL bDirect)
{
    CallScintilla(SCI_SETVIEWEOL, static_cast<WPARAM>(visible), 0, bDirect);
}

void* CScintillaCtrl::GetDocPointer(BOOL bDirect)
{
    return reinterpret_cast<void*>(CallScintilla(SCI_GETDOCPOINTER, 0, 0, bDirect));
}

void CScintillaCtrl::SetDocPointer(void* pointer, BOOL bDirect)
{
    CallScintilla(SCI_SETDOCPOINTER, 0, reinterpret_cast<LPARAM>(pointer), bDirect);
}

void CScintillaCtrl::SetModEventMask(int mask, BOOL bDirect)
{
    CallScintilla(SCI_SETMODEVENTMASK, static_cast<WPARAM>(mask), 0, bDirect);
}

int CScintillaCtrl::GetEdgeColumn(BOOL bDirect)
{
    return CallScintilla(SCI_GETEDGECOLUMN, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeColumn(int column, BOOL bDirect)
{
    CallScintilla(SCI_SETEDGECOLUMN, static_cast<WPARAM>(column), 0, bDirect);
}

int CScintillaCtrl::GetEdgeMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETEDGEMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETEDGEMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

COLORREF CScintillaCtrl::GetEdgeColour(BOOL bDirect)
{
    return CallScintilla(SCI_GETEDGECOLOUR, 0, 0, bDirect);
}

void CScintillaCtrl::SetEdgeColour(COLORREF edgeColour, BOOL bDirect)
{
    CallScintilla(SCI_SETEDGECOLOUR, static_cast<WPARAM>(edgeColour), 0, bDirect);
}

void CScintillaCtrl::SearchAnchor(BOOL bDirect)
{
    CallScintilla(SCI_SEARCHANCHOR, 0, 0, bDirect);
}

int CScintillaCtrl::SearchNext(int flags, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_SEARCHNEXT, static_cast<WPARAM>(flags), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::SearchPrev(int flags, const char* text, BOOL bDirect)
{
    return CallScintilla(SCI_SEARCHPREV, static_cast<WPARAM>(flags), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::LinesOnScreen(BOOL bDirect)
{
    return CallScintilla(SCI_LINESONSCREEN, 0, 0, bDirect);
}

void CScintillaCtrl::UsePopUp(BOOL allowPopUp, BOOL bDirect)
{
    CallScintilla(SCI_USEPOPUP, static_cast<WPARAM>(allowPopUp), 0, bDirect);
}

BOOL CScintillaCtrl::SelectionIsRectangle(BOOL bDirect)
{
    return CallScintilla(SCI_SELECTIONISRECTANGLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetZoom(int zoom, BOOL bDirect)
{
    CallScintilla(SCI_SETZOOM, static_cast<WPARAM>(zoom), 0, bDirect);
}

int CScintillaCtrl::GetZoom(BOOL bDirect)
{
    return CallScintilla(SCI_GETZOOM, 0, 0, bDirect);
}

int CScintillaCtrl::CreateDocument(BOOL bDirect)
{
    return CallScintilla(SCI_CREATEDOCUMENT, 0, 0, bDirect);
}

void CScintillaCtrl::AddRefDocument(int doc, BOOL bDirect)
{
    CallScintilla(SCI_ADDREFDOCUMENT, 0, static_cast<LPARAM>(doc), bDirect);
}

void CScintillaCtrl::ReleaseDocument(int doc, BOOL bDirect)
{
    CallScintilla(SCI_RELEASEDOCUMENT, 0, static_cast<LPARAM>(doc), bDirect);
}

int CScintillaCtrl::GetModEventMask(BOOL bDirect)
{
    return CallScintilla(SCI_GETMODEVENTMASK, 0, 0, bDirect);
}

void CScintillaCtrl::SCISetFocus(BOOL focus, BOOL bDirect)
{
    CallScintilla(SCI_SETFOCUS, static_cast<WPARAM>(focus), 0, bDirect);
}

BOOL CScintillaCtrl::GetFocus(BOOL bDirect)
{
    return CallScintilla(SCI_GETFOCUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetStatus(int statusCode, BOOL bDirect)
{
    CallScintilla(SCI_SETSTATUS, static_cast<WPARAM>(statusCode), 0, bDirect);
}

int CScintillaCtrl::GetStatus(BOOL bDirect)
{
    return CallScintilla(SCI_GETSTATUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetMouseDownCaptures(BOOL captures, BOOL bDirect)
{
    CallScintilla(SCI_SETMOUSEDOWNCAPTURES, static_cast<WPARAM>(captures), 0, bDirect);
}

BOOL CScintillaCtrl::GetMouseDownCaptures(BOOL bDirect)
{
    return CallScintilla(SCI_GETMOUSEDOWNCAPTURES, 0, 0, bDirect);
}

void CScintillaCtrl::SetCursor(int cursorType, BOOL bDirect)
{
    CallScintilla(SCI_SETCURSOR, static_cast<WPARAM>(cursorType), 0, bDirect);
}

int CScintillaCtrl::GetCursor(BOOL bDirect)
{
    return CallScintilla(SCI_GETCURSOR, 0, 0, bDirect);
}

void CScintillaCtrl::SetControlCharSymbol(int symbol, BOOL bDirect)
{
    CallScintilla(SCI_SETCONTROLCHARSYMBOL, static_cast<WPARAM>(symbol), 0, bDirect);
}

int CScintillaCtrl::GetControlCharSymbol(BOOL bDirect)
{
    return CallScintilla(SCI_GETCONTROLCHARSYMBOL, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartLeft(BOOL bDirect)
{
    CallScintilla(SCI_WORDPARTLEFT, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartLeftExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDPARTLEFTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartRight(BOOL bDirect)
{
    CallScintilla(SCI_WORDPARTRIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::WordPartRightExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDPARTRIGHTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetVisiblePolicy(int visiblePolicy, int visibleSlop, BOOL bDirect)
{
    CallScintilla(SCI_SETVISIBLEPOLICY, static_cast<WPARAM>(visiblePolicy), static_cast<LPARAM>(visibleSlop), bDirect);
}

void CScintillaCtrl::DelLineLeft(BOOL bDirect)
{
    CallScintilla(SCI_DELLINELEFT, 0, 0, bDirect);
}

void CScintillaCtrl::DelLineRight(BOOL bDirect)
{
    CallScintilla(SCI_DELLINERIGHT, 0, 0, bDirect);
}

void CScintillaCtrl::SetXOffset(int newOffset, BOOL bDirect)
{
    CallScintilla(SCI_SETXOFFSET, static_cast<WPARAM>(newOffset), 0, bDirect);
}

int CScintillaCtrl::GetXOffset(BOOL bDirect)
{
    return CallScintilla(SCI_GETXOFFSET, 0, 0, bDirect);
}

void CScintillaCtrl::ChooseCaretX(BOOL bDirect)
{
    CallScintilla(SCI_CHOOSECARETX, 0, 0, bDirect);
}

void CScintillaCtrl::GrabFocus(BOOL bDirect)
{
    CallScintilla(SCI_GRABFOCUS, 0, 0, bDirect);
}

void CScintillaCtrl::SetXCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect)
{
    CallScintilla(SCI_SETXCARETPOLICY, static_cast<WPARAM>(caretPolicy), static_cast<LPARAM>(caretSlop), bDirect);
}

void CScintillaCtrl::SetYCaretPolicy(int caretPolicy, int caretSlop, BOOL bDirect)
{
    CallScintilla(SCI_SETYCARETPOLICY, static_cast<WPARAM>(caretPolicy), static_cast<LPARAM>(caretSlop), bDirect);
}

void CScintillaCtrl::SetPrintWrapMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETPRINTWRAPMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetPrintWrapMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETPRINTWRAPMODE, 0, 0, bDirect);
}

void CScintillaCtrl::SetHotspotActiveFore(BOOL useSetting, COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETHOTSPOTACTIVEFORE, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(fore), bDirect);
}

COLORREF CScintillaCtrl::GetHotspotActiveFore(BOOL bDirect)
{
    return CallScintilla(SCI_GETHOTSPOTACTIVEFORE, 0, 0, bDirect);
}

void CScintillaCtrl::SetHotspotActiveBack(BOOL useSetting, COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETHOTSPOTACTIVEBACK, static_cast<WPARAM>(useSetting), static_cast<LPARAM>(back), bDirect);
}

COLORREF CScintillaCtrl::GetHotspotActiveBack(BOOL bDirect)
{
    return CallScintilla(SCI_GETHOTSPOTACTIVEBACK, 0, 0, bDirect);
}

void CScintillaCtrl::SetHotspotActiveUnderline(BOOL underline, BOOL bDirect)
{
    CallScintilla(SCI_SETHOTSPOTACTIVEUNDERLINE, static_cast<WPARAM>(underline), 0, bDirect);
}

BOOL CScintillaCtrl::GetHotspotActiveUnderline(BOOL bDirect)
{
    return CallScintilla(SCI_GETHOTSPOTACTIVEUNDERLINE, 0, 0, bDirect);
}

void CScintillaCtrl::SetHotspotSingleLine(BOOL singleLine, BOOL bDirect)
{
    CallScintilla(SCI_SETHOTSPOTSINGLELINE, static_cast<WPARAM>(singleLine), 0, bDirect);
}

BOOL CScintillaCtrl::GetHotspotSingleLine(BOOL bDirect)
{
    return CallScintilla(SCI_GETHOTSPOTSINGLELINE, 0, 0, bDirect);
}

void CScintillaCtrl::ParaDown(BOOL bDirect)
{
    CallScintilla(SCI_PARADOWN, 0, 0, bDirect);
}

void CScintillaCtrl::ParaDownExtend(BOOL bDirect)
{
    CallScintilla(SCI_PARADOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::ParaUp(BOOL bDirect)
{
    CallScintilla(SCI_PARAUP, 0, 0, bDirect);
}

void CScintillaCtrl::ParaUpExtend(BOOL bDirect)
{
    CallScintilla(SCI_PARAUPEXTEND, 0, 0, bDirect);
}

long CScintillaCtrl::PositionBefore(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_POSITIONBEFORE, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::PositionAfter(long pos, BOOL bDirect)
{
    return CallScintilla(SCI_POSITIONAFTER, static_cast<WPARAM>(pos), 0, bDirect);
}

void CScintillaCtrl::CopyRange(long start, long end, BOOL bDirect)
{
    CallScintilla(SCI_COPYRANGE, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

void CScintillaCtrl::CopyText(int length, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_COPYTEXT, static_cast<WPARAM>(length), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::SetSelectionMode(int mode, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONMODE, static_cast<WPARAM>(mode), 0, bDirect);
}

int CScintillaCtrl::GetSelectionMode(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONMODE, 0, 0, bDirect);
}

long CScintillaCtrl::GetLineSelStartPosition(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINESELSTARTPOSITION, static_cast<WPARAM>(line), 0, bDirect);
}

long CScintillaCtrl::GetLineSelEndPosition(int line, BOOL bDirect)
{
    return CallScintilla(SCI_GETLINESELENDPOSITION, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::LineDownRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEDOWNRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineUpRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEUPRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharLeftRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_CHARLEFTRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::CharRightRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_CHARRIGHTRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::HomeRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_HOMERECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMERECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::LineEndRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_LINEENDRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageUpRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_PAGEUPRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::PageDownRectExtend(BOOL bDirect)
{
    CallScintilla(SCI_PAGEDOWNRECTEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageUp(BOOL bDirect)
{
    CallScintilla(SCI_STUTTEREDPAGEUP, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageUpExtend(BOOL bDirect)
{
    CallScintilla(SCI_STUTTEREDPAGEUPEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageDown(BOOL bDirect)
{
    CallScintilla(SCI_STUTTEREDPAGEDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::StutteredPageDownExtend(BOOL bDirect)
{
    CallScintilla(SCI_STUTTEREDPAGEDOWNEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftEnd(BOOL bDirect)
{
    CallScintilla(SCI_WORDLEFTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordLeftEndExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDLEFTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightEnd(BOOL bDirect)
{
    CallScintilla(SCI_WORDRIGHTEND, 0, 0, bDirect);
}

void CScintillaCtrl::WordRightEndExtend(BOOL bDirect)
{
    CallScintilla(SCI_WORDRIGHTENDEXTEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetWhitespaceChars(const char* characters, BOOL bDirect)
{
    CallScintilla(SCI_SETWHITESPACECHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

int CScintillaCtrl::GetWhitespaceChars(char* characters, BOOL bDirect)
{
    return CallScintilla(SCI_GETWHITESPACECHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

void CScintillaCtrl::SetPunctuationChars(const char* characters, BOOL bDirect)
{
    CallScintilla(SCI_SETPUNCTUATIONCHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

int CScintillaCtrl::GetPunctuationChars(char* characters, BOOL bDirect)
{
    return CallScintilla(SCI_GETPUNCTUATIONCHARS, 0, reinterpret_cast<LPARAM>(characters), bDirect);
}

void CScintillaCtrl::SetCharsDefault(BOOL bDirect)
{
    CallScintilla(SCI_SETCHARSDEFAULT, 0, 0, bDirect);
}

int CScintillaCtrl::AutoCGetCurrent(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETCURRENT, 0, 0, bDirect);
}

int CScintillaCtrl::AutoCGetCurrentText(char* s, BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETCURRENTTEXT, 0, reinterpret_cast<LPARAM>(s), bDirect);
}

void CScintillaCtrl::AutoCSetCaseInsensitiveBehaviour(int behaviour, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETCASEINSENSITIVEBEHAVIOUR, static_cast<WPARAM>(behaviour), 0, bDirect);
}

int CScintillaCtrl::AutoCGetCaseInsensitiveBehaviour(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETCASEINSENSITIVEBEHAVIOUR, 0, 0, bDirect);
}

void CScintillaCtrl::AutoCSetOrder(int order, BOOL bDirect)
{
    CallScintilla(SCI_AUTOCSETORDER, static_cast<WPARAM>(order), 0, bDirect);
}

int CScintillaCtrl::AutoCGetOrder(BOOL bDirect)
{
    return CallScintilla(SCI_AUTOCGETORDER, 0, 0, bDirect);
}

void CScintillaCtrl::Allocate(int bytes, BOOL bDirect)
{
    CallScintilla(SCI_ALLOCATE, static_cast<WPARAM>(bytes), 0, bDirect);
}

int CScintillaCtrl::TargetAsUTF8(char* s, BOOL bDirect)
{
    return CallScintilla(SCI_TARGETASUTF8, 0, reinterpret_cast<LPARAM>(s), bDirect);
}

void CScintillaCtrl::SetLengthForEncode(int bytes, BOOL bDirect)
{
    CallScintilla(SCI_SETLENGTHFORENCODE, static_cast<WPARAM>(bytes), 0, bDirect);
}

int CScintillaCtrl::EncodedFromUTF8(const char* utf8, char* encoded, BOOL bDirect)
{
    return CallScintilla(SCI_ENCODEDFROMUTF8, reinterpret_cast<WPARAM>(utf8), reinterpret_cast<LPARAM>(encoded), bDirect);
}

int CScintillaCtrl::FindColumn(int line, int column, BOOL bDirect)
{
    return CallScintilla(SCI_FINDCOLUMN, static_cast<WPARAM>(line), static_cast<LPARAM>(column), bDirect);
}

int CScintillaCtrl::GetCaretSticky(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETSTICKY, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretSticky(int useCaretStickyBehaviour, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETSTICKY, static_cast<WPARAM>(useCaretStickyBehaviour), 0, bDirect);
}

void CScintillaCtrl::ToggleCaretSticky(BOOL bDirect)
{
    CallScintilla(SCI_TOGGLECARETSTICKY, 0, 0, bDirect);
}

void CScintillaCtrl::SetPasteConvertEndings(BOOL convert, BOOL bDirect)
{
    CallScintilla(SCI_SETPASTECONVERTENDINGS, static_cast<WPARAM>(convert), 0, bDirect);
}

BOOL CScintillaCtrl::GetPasteConvertEndings(BOOL bDirect)
{
    return CallScintilla(SCI_GETPASTECONVERTENDINGS, 0, 0, bDirect);
}

void CScintillaCtrl::SelectionDuplicate(BOOL bDirect)
{
    CallScintilla(SCI_SELECTIONDUPLICATE, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineBackAlpha(int alpha, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETLINEBACKALPHA, static_cast<WPARAM>(alpha), 0, bDirect);
}

int CScintillaCtrl::GetCaretLineBackAlpha(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETLINEBACKALPHA, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretStyle(int caretStyle, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETSTYLE, static_cast<WPARAM>(caretStyle), 0, bDirect);
}

int CScintillaCtrl::GetCaretStyle(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETSTYLE, 0, 0, bDirect);
}

void CScintillaCtrl::SetIndicatorCurrent(int indicator, BOOL bDirect)
{
    CallScintilla(SCI_SETINDICATORCURRENT, static_cast<WPARAM>(indicator), 0, bDirect);
}

int CScintillaCtrl::GetIndicatorCurrent(BOOL bDirect)
{
    return CallScintilla(SCI_GETINDICATORCURRENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetIndicatorValue(int value, BOOL bDirect)
{
    CallScintilla(SCI_SETINDICATORVALUE, static_cast<WPARAM>(value), 0, bDirect);
}

int CScintillaCtrl::GetIndicatorValue(BOOL bDirect)
{
    return CallScintilla(SCI_GETINDICATORVALUE, 0, 0, bDirect);
}

void CScintillaCtrl::IndicatorFillRange(int position, int fillLength, BOOL bDirect)
{
    CallScintilla(SCI_INDICATORFILLRANGE, static_cast<WPARAM>(position), static_cast<LPARAM>(fillLength), bDirect);
}

void CScintillaCtrl::IndicatorClearRange(int position, int clearLength, BOOL bDirect)
{
    CallScintilla(SCI_INDICATORCLEARRANGE, static_cast<WPARAM>(position), static_cast<LPARAM>(clearLength), bDirect);
}

int CScintillaCtrl::IndicatorAllOnFor(int position, BOOL bDirect)
{
    return CallScintilla(SCI_INDICATORALLONFOR, static_cast<WPARAM>(position), 0, bDirect);
}

int CScintillaCtrl::IndicatorValueAt(int indicator, int position, BOOL bDirect)
{
    return CallScintilla(SCI_INDICATORVALUEAT, static_cast<WPARAM>(indicator), static_cast<LPARAM>(position), bDirect);
}

int CScintillaCtrl::IndicatorStart(int indicator, int position, BOOL bDirect)
{
    return CallScintilla(SCI_INDICATORSTART, static_cast<WPARAM>(indicator), static_cast<LPARAM>(position), bDirect);
}

int CScintillaCtrl::IndicatorEnd(int indicator, int position, BOOL bDirect)
{
    return CallScintilla(SCI_INDICATOREND, static_cast<WPARAM>(indicator), static_cast<LPARAM>(position), bDirect);
}

void CScintillaCtrl::SetPositionCache(int size, BOOL bDirect)
{
    CallScintilla(SCI_SETPOSITIONCACHE, static_cast<WPARAM>(size), 0, bDirect);
}

int CScintillaCtrl::GetPositionCache(BOOL bDirect)
{
    return CallScintilla(SCI_GETPOSITIONCACHE, 0, 0, bDirect);
}

void CScintillaCtrl::CopyAllowLine(BOOL bDirect)
{
    CallScintilla(SCI_COPYALLOWLINE, 0, 0, bDirect);
}

const char* CScintillaCtrl::GetCharacterPointer(BOOL bDirect)
{
    return reinterpret_cast<const char*>(CallScintilla(SCI_GETCHARACTERPOINTER, 0, 0, bDirect));
}

void* CScintillaCtrl::GetRangePointer(int position, int rangeLength, BOOL bDirect)
{
    return reinterpret_cast<void*>(CallScintilla(SCI_GETRANGEPOINTER, static_cast<WPARAM>(position), static_cast<LPARAM>(rangeLength), bDirect));
}

long CScintillaCtrl::GetGapPosition(BOOL bDirect)
{
    return CallScintilla(SCI_GETGAPPOSITION, 0, 0, bDirect);
}

void CScintillaCtrl::SetKeysUnicode(BOOL keysUnicode, BOOL bDirect)
{
    CallScintilla(SCI_SETKEYSUNICODE, static_cast<WPARAM>(keysUnicode), 0, bDirect);
}

BOOL CScintillaCtrl::GetKeysUnicode(BOOL bDirect)
{
    return CallScintilla(SCI_GETKEYSUNICODE, 0, 0, bDirect);
}

void CScintillaCtrl::IndicSetAlpha(int indicator, int alpha, BOOL bDirect)
{
    CallScintilla(SCI_INDICSETALPHA, static_cast<WPARAM>(indicator), static_cast<LPARAM>(alpha), bDirect);
}

int CScintillaCtrl::IndicGetAlpha(int indicator, BOOL bDirect)
{
    return CallScintilla(SCI_INDICGETALPHA, static_cast<WPARAM>(indicator), 0, bDirect);
}

void CScintillaCtrl::IndicSetOutlineAlpha(int indicator, int alpha, BOOL bDirect)
{
    CallScintilla(SCI_INDICSETOUTLINEALPHA, static_cast<WPARAM>(indicator), static_cast<LPARAM>(alpha), bDirect);
}

int CScintillaCtrl::IndicGetOutlineAlpha(int indicator, BOOL bDirect)
{
    return CallScintilla(SCI_INDICGETOUTLINEALPHA, static_cast<WPARAM>(indicator), 0, bDirect);
}

void CScintillaCtrl::SetExtraAscent(int extraAscent, BOOL bDirect)
{
    CallScintilla(SCI_SETEXTRAASCENT, static_cast<WPARAM>(extraAscent), 0, bDirect);
}

int CScintillaCtrl::GetExtraAscent(BOOL bDirect)
{
    return CallScintilla(SCI_GETEXTRAASCENT, 0, 0, bDirect);
}

void CScintillaCtrl::SetExtraDescent(int extraDescent, BOOL bDirect)
{
    CallScintilla(SCI_SETEXTRADESCENT, static_cast<WPARAM>(extraDescent), 0, bDirect);
}

int CScintillaCtrl::GetExtraDescent(BOOL bDirect)
{
    return CallScintilla(SCI_GETEXTRADESCENT, 0, 0, bDirect);
}

int CScintillaCtrl::MarkerSymbolDefined(int markerNumber, BOOL bDirect)
{
    return CallScintilla(SCI_MARKERSYMBOLDEFINED, static_cast<WPARAM>(markerNumber), 0, bDirect);
}

void CScintillaCtrl::MarginSetText(int line, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_MARGINSETTEXT, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::MarginGetText(int line, char* text, BOOL bDirect)
{
    return CallScintilla(SCI_MARGINGETTEXT, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::MarginSetStyle(int line, int style, BOOL bDirect)
{
    CallScintilla(SCI_MARGINSETSTYLE, static_cast<WPARAM>(line), static_cast<LPARAM>(style), bDirect);
}

int CScintillaCtrl::MarginGetStyle(int line, BOOL bDirect)
{
    return CallScintilla(SCI_MARGINGETSTYLE, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::MarginSetStyles(int line, const char* styles, BOOL bDirect)
{
    CallScintilla(SCI_MARGINSETSTYLES, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(styles), bDirect);
}

int CScintillaCtrl::MarginGetStyles(int line, char* styles, BOOL bDirect)
{
    return CallScintilla(SCI_MARGINGETSTYLES, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(styles), bDirect);
}

void CScintillaCtrl::MarginTextClearAll(BOOL bDirect)
{
    CallScintilla(SCI_MARGINTEXTCLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::MarginSetStyleOffset(int style, BOOL bDirect)
{
    CallScintilla(SCI_MARGINSETSTYLEOFFSET, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::MarginGetStyleOffset(BOOL bDirect)
{
    return CallScintilla(SCI_MARGINGETSTYLEOFFSET, 0, 0, bDirect);
}

void CScintillaCtrl::SetMarginOptions(int marginOptions, BOOL bDirect)
{
    CallScintilla(SCI_SETMARGINOPTIONS, static_cast<WPARAM>(marginOptions), 0, bDirect);
}

int CScintillaCtrl::GetMarginOptions(BOOL bDirect)
{
    return CallScintilla(SCI_GETMARGINOPTIONS, 0, 0, bDirect);
}

void CScintillaCtrl::AnnotationSetText(int line, const char* text, BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONSETTEXT, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(text), bDirect);
}

int CScintillaCtrl::AnnotationGetText(int line, char* text, BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETTEXT, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(text), bDirect);
}

void CScintillaCtrl::AnnotationSetStyle(int line, int style, BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONSETSTYLE, static_cast<WPARAM>(line), static_cast<LPARAM>(style), bDirect);
}

int CScintillaCtrl::AnnotationGetStyle(int line, BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETSTYLE, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::AnnotationSetStyles(int line, const char* styles, BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONSETSTYLES, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(styles), bDirect);
}

int CScintillaCtrl::AnnotationGetStyles(int line, char* styles, BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETSTYLES, static_cast<WPARAM>(line), reinterpret_cast<LPARAM>(styles), bDirect);
}

int CScintillaCtrl::AnnotationGetLines(int line, BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETLINES, static_cast<WPARAM>(line), 0, bDirect);
}

void CScintillaCtrl::AnnotationClearAll(BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONCLEARALL, 0, 0, bDirect);
}

void CScintillaCtrl::AnnotationSetVisible(int visible, BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONSETVISIBLE, static_cast<WPARAM>(visible), 0, bDirect);
}

int CScintillaCtrl::AnnotationGetVisible(BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETVISIBLE, 0, 0, bDirect);
}

void CScintillaCtrl::AnnotationSetStyleOffset(int style, BOOL bDirect)
{
    CallScintilla(SCI_ANNOTATIONSETSTYLEOFFSET, static_cast<WPARAM>(style), 0, bDirect);
}

int CScintillaCtrl::AnnotationGetStyleOffset(BOOL bDirect)
{
    return CallScintilla(SCI_ANNOTATIONGETSTYLEOFFSET, 0, 0, bDirect);
}

void CScintillaCtrl::ReleaseAllExtendedStyles(BOOL bDirect)
{
    CallScintilla(SCI_RELEASEALLEXTENDEDSTYLES, 0, 0, bDirect);
}

int CScintillaCtrl::AllocateExtendedStyles(int numberStyles, BOOL bDirect)
{
    return CallScintilla(SCI_ALLOCATEEXTENDEDSTYLES, static_cast<WPARAM>(numberStyles), 0, bDirect);
}

void CScintillaCtrl::AddUndoAction(int token, int flags, BOOL bDirect)
{
    CallScintilla(SCI_ADDUNDOACTION, static_cast<WPARAM>(token), static_cast<LPARAM>(flags), bDirect);
}

long CScintillaCtrl::CharPositionFromPoint(int x, int y, BOOL bDirect)
{
    return CallScintilla(SCI_CHARPOSITIONFROMPOINT, static_cast<WPARAM>(x), static_cast<LPARAM>(y), bDirect);
}

long CScintillaCtrl::CharPositionFromPointClose(int x, int y, BOOL bDirect)
{
    return CallScintilla(SCI_CHARPOSITIONFROMPOINTCLOSE, static_cast<WPARAM>(x), static_cast<LPARAM>(y), bDirect);
}

void CScintillaCtrl::SetMultipleSelection(BOOL multipleSelection, BOOL bDirect)
{
    CallScintilla(SCI_SETMULTIPLESELECTION, static_cast<WPARAM>(multipleSelection), 0, bDirect);
}

BOOL CScintillaCtrl::GetMultipleSelection(BOOL bDirect)
{
    return CallScintilla(SCI_GETMULTIPLESELECTION, 0, 0, bDirect);
}

void CScintillaCtrl::SetAdditionalSelectionTyping(BOOL additionalSelectionTyping, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALSELECTIONTYPING, static_cast<WPARAM>(additionalSelectionTyping), 0, bDirect);
}

BOOL CScintillaCtrl::GetAdditionalSelectionTyping(BOOL bDirect)
{
    return CallScintilla(SCI_GETADDITIONALSELECTIONTYPING, 0, 0, bDirect);
}

void CScintillaCtrl::SetAdditionalCaretsBlink(BOOL additionalCaretsBlink, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALCARETSBLINK, static_cast<WPARAM>(additionalCaretsBlink), 0, bDirect);
}

BOOL CScintillaCtrl::GetAdditionalCaretsBlink(BOOL bDirect)
{
    return CallScintilla(SCI_GETADDITIONALCARETSBLINK, 0, 0, bDirect);
}

void CScintillaCtrl::SetAdditionalCaretsVisible(BOOL additionalCaretsBlink, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALCARETSVISIBLE, static_cast<WPARAM>(additionalCaretsBlink), 0, bDirect);
}

BOOL CScintillaCtrl::GetAdditionalCaretsVisible(BOOL bDirect)
{
    return CallScintilla(SCI_GETADDITIONALCARETSVISIBLE, 0, 0, bDirect);
}

int CScintillaCtrl::GetSelections(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONS, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetSelectionEmpty(BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONEMPTY, 0, 0, bDirect);
}

void CScintillaCtrl::ClearSelections(BOOL bDirect)
{
    CallScintilla(SCI_CLEARSELECTIONS, 0, 0, bDirect);
}

int CScintillaCtrl::SetSelection(int caret, int anchor, BOOL bDirect)
{
    return CallScintilla(SCI_SETSELECTION, static_cast<WPARAM>(caret), static_cast<LPARAM>(anchor), bDirect);
}

int CScintillaCtrl::AddSelection(int caret, int anchor, BOOL bDirect)
{
    return CallScintilla(SCI_ADDSELECTION, static_cast<WPARAM>(caret), static_cast<LPARAM>(anchor), bDirect);
}

void CScintillaCtrl::SetMainSelection(int selection, BOOL bDirect)
{
    CallScintilla(SCI_SETMAINSELECTION, static_cast<WPARAM>(selection), 0, bDirect);
}

int CScintillaCtrl::GetMainSelection(BOOL bDirect)
{
    return CallScintilla(SCI_GETMAINSELECTION, 0, 0, bDirect);
}

void CScintillaCtrl::SetSelectionNCaret(int selection, long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNCARET, static_cast<WPARAM>(selection), static_cast<LPARAM>(pos), bDirect);
}

long CScintillaCtrl::GetSelectionNCaret(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNCARET, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetSelectionNAnchor(int selection, long posAnchor, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNANCHOR, static_cast<WPARAM>(selection), static_cast<LPARAM>(posAnchor), bDirect);
}

long CScintillaCtrl::GetSelectionNAnchor(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNANCHOR, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetSelectionNCaretVirtualSpace(int selection, int space, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNCARETVIRTUALSPACE, static_cast<WPARAM>(selection), static_cast<LPARAM>(space), bDirect);
}

int CScintillaCtrl::GetSelectionNCaretVirtualSpace(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNCARETVIRTUALSPACE, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetSelectionNAnchorVirtualSpace(int selection, int space, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNANCHORVIRTUALSPACE, static_cast<WPARAM>(selection), static_cast<LPARAM>(space), bDirect);
}

int CScintillaCtrl::GetSelectionNAnchorVirtualSpace(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNANCHORVIRTUALSPACE, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetSelectionNStart(int selection, long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNSTART, static_cast<WPARAM>(selection), static_cast<LPARAM>(pos), bDirect);
}

long CScintillaCtrl::GetSelectionNStart(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNSTART, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetSelectionNEnd(int selection, long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETSELECTIONNEND, static_cast<WPARAM>(selection), static_cast<LPARAM>(pos), bDirect);
}

long CScintillaCtrl::GetSelectionNEnd(int selection, BOOL bDirect)
{
    return CallScintilla(SCI_GETSELECTIONNEND, static_cast<WPARAM>(selection), 0, bDirect);
}

void CScintillaCtrl::SetRectangularSelectionCaret(long pos, BOOL bDirect)
{
    CallScintilla(SCI_SETRECTANGULARSELECTIONCARET, static_cast<WPARAM>(pos), 0, bDirect);
}

long CScintillaCtrl::GetRectangularSelectionCaret(BOOL bDirect)
{
    return CallScintilla(SCI_GETRECTANGULARSELECTIONCARET, 0, 0, bDirect);
}

void CScintillaCtrl::SetRectangularSelectionAnchor(long posAnchor, BOOL bDirect)
{
    CallScintilla(SCI_SETRECTANGULARSELECTIONANCHOR, static_cast<WPARAM>(posAnchor), 0, bDirect);
}

long CScintillaCtrl::GetRectangularSelectionAnchor(BOOL bDirect)
{
    return CallScintilla(SCI_GETRECTANGULARSELECTIONANCHOR, 0, 0, bDirect);
}

void CScintillaCtrl::SetRectangularSelectionCaretVirtualSpace(int space, BOOL bDirect)
{
    CallScintilla(SCI_SETRECTANGULARSELECTIONCARETVIRTUALSPACE, static_cast<WPARAM>(space), 0, bDirect);
}

int CScintillaCtrl::GetRectangularSelectionCaretVirtualSpace(BOOL bDirect)
{
    return CallScintilla(SCI_GETRECTANGULARSELECTIONCARETVIRTUALSPACE, 0, 0, bDirect);
}

void CScintillaCtrl::SetRectangularSelectionAnchorVirtualSpace(int space, BOOL bDirect)
{
    CallScintilla(SCI_SETRECTANGULARSELECTIONANCHORVIRTUALSPACE, static_cast<WPARAM>(space), 0, bDirect);
}

int CScintillaCtrl::GetRectangularSelectionAnchorVirtualSpace(BOOL bDirect)
{
    return CallScintilla(SCI_GETRECTANGULARSELECTIONANCHORVIRTUALSPACE, 0, 0, bDirect);
}

void CScintillaCtrl::SetVirtualSpaceOptions(int virtualSpaceOptions, BOOL bDirect)
{
    CallScintilla(SCI_SETVIRTUALSPACEOPTIONS, static_cast<WPARAM>(virtualSpaceOptions), 0, bDirect);
}

int CScintillaCtrl::GetVirtualSpaceOptions(BOOL bDirect)
{
    return CallScintilla(SCI_GETVIRTUALSPACEOPTIONS, 0, 0, bDirect);
}

void CScintillaCtrl::SetRectangularSelectionModifier(int modifier, BOOL bDirect)
{
    CallScintilla(SCI_SETRECTANGULARSELECTIONMODIFIER, static_cast<WPARAM>(modifier), 0, bDirect);
}

int CScintillaCtrl::GetRectangularSelectionModifier(BOOL bDirect)
{
    return CallScintilla(SCI_GETRECTANGULARSELECTIONMODIFIER, 0, 0, bDirect);
}

void CScintillaCtrl::SetAdditionalSelFore(COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALSELFORE, static_cast<WPARAM>(fore), 0, bDirect);
}

void CScintillaCtrl::SetAdditionalSelBack(COLORREF back, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALSELBACK, static_cast<WPARAM>(back), 0, bDirect);
}

void CScintillaCtrl::SetAdditionalSelAlpha(int alpha, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALSELALPHA, static_cast<WPARAM>(alpha), 0, bDirect);
}

int CScintillaCtrl::GetAdditionalSelAlpha(BOOL bDirect)
{
    return CallScintilla(SCI_GETADDITIONALSELALPHA, 0, 0, bDirect);
}

void CScintillaCtrl::SetAdditionalCaretFore(COLORREF fore, BOOL bDirect)
{
    CallScintilla(SCI_SETADDITIONALCARETFORE, static_cast<WPARAM>(fore), 0, bDirect);
}

COLORREF CScintillaCtrl::GetAdditionalCaretFore(BOOL bDirect)
{
    return CallScintilla(SCI_GETADDITIONALCARETFORE, 0, 0, bDirect);
}

void CScintillaCtrl::RotateSelection(BOOL bDirect)
{
    CallScintilla(SCI_ROTATESELECTION, 0, 0, bDirect);
}

void CScintillaCtrl::SwapMainAnchorCaret(BOOL bDirect)
{
    CallScintilla(SCI_SWAPMAINANCHORCARET, 0, 0, bDirect);
}

int CScintillaCtrl::ChangeLexerState(long start, long end, BOOL bDirect)
{
    return CallScintilla(SCI_CHANGELEXERSTATE, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

int CScintillaCtrl::ContractedFoldNext(int lineStart, BOOL bDirect)
{
    return CallScintilla(SCI_CONTRACTEDFOLDNEXT, static_cast<WPARAM>(lineStart), 0, bDirect);
}

void CScintillaCtrl::VerticalCentreCaret(BOOL bDirect)
{
    CallScintilla(SCI_VERTICALCENTRECARET, 0, 0, bDirect);
}

void CScintillaCtrl::MoveSelectedLinesUp(BOOL bDirect)
{
    CallScintilla(SCI_MOVESELECTEDLINESUP, 0, 0, bDirect);
}

void CScintillaCtrl::MoveSelectedLinesDown(BOOL bDirect)
{
    CallScintilla(SCI_MOVESELECTEDLINESDOWN, 0, 0, bDirect);
}

void CScintillaCtrl::SetIdentifier(int identifier, BOOL bDirect)
{
    CallScintilla(SCI_SETIDENTIFIER, static_cast<WPARAM>(identifier), 0, bDirect);
}

int CScintillaCtrl::GetIdentifier(BOOL bDirect)
{
    return CallScintilla(SCI_GETIDENTIFIER, 0, 0, bDirect);
}

void CScintillaCtrl::RGBAImageSetWidth(int width, BOOL bDirect)
{
    CallScintilla(SCI_RGBAIMAGESETWIDTH, static_cast<WPARAM>(width), 0, bDirect);
}

void CScintillaCtrl::RGBAImageSetHeight(int height, BOOL bDirect)
{
    CallScintilla(SCI_RGBAIMAGESETHEIGHT, static_cast<WPARAM>(height), 0, bDirect);
}

void CScintillaCtrl::RGBAImageSetScale(int scalePercent, BOOL bDirect)
{
    CallScintilla(SCI_RGBAIMAGESETSCALE, static_cast<WPARAM>(scalePercent), 0, bDirect);
}

void CScintillaCtrl::MarkerDefineRGBAImage(int markerNumber, const char* pixels, BOOL bDirect)
{
    CallScintilla(SCI_MARKERDEFINERGBAIMAGE, static_cast<WPARAM>(markerNumber), reinterpret_cast<LPARAM>(pixels), bDirect);
}

void CScintillaCtrl::RegisterRGBAImage(int type, const char* pixels, BOOL bDirect)
{
    CallScintilla(SCI_REGISTERRGBAIMAGE, static_cast<WPARAM>(type), reinterpret_cast<LPARAM>(pixels), bDirect);
}

void CScintillaCtrl::ScrollToStart(BOOL bDirect)
{
    CallScintilla(SCI_SCROLLTOSTART, 0, 0, bDirect);
}

void CScintillaCtrl::ScrollToEnd(BOOL bDirect)
{
    CallScintilla(SCI_SCROLLTOEND, 0, 0, bDirect);
}

void CScintillaCtrl::SetTechnology(int technology, BOOL bDirect)
{
    CallScintilla(SCI_SETTECHNOLOGY, static_cast<WPARAM>(technology), 0, bDirect);
}

int CScintillaCtrl::GetTechnology(BOOL bDirect)
{
    return CallScintilla(SCI_GETTECHNOLOGY, 0, 0, bDirect);
}

int CScintillaCtrl::CreateLoader(int bytes, BOOL bDirect)
{
    return CallScintilla(SCI_CREATELOADER, static_cast<WPARAM>(bytes), 0, bDirect);
}

void CScintillaCtrl::FindIndicatorShow(long start, long end, BOOL bDirect)
{
    CallScintilla(SCI_FINDINDICATORSHOW, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

void CScintillaCtrl::FindIndicatorFlash(long start, long end, BOOL bDirect)
{
    CallScintilla(SCI_FINDINDICATORFLASH, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

void CScintillaCtrl::FindIndicatorHide(BOOL bDirect)
{
    CallScintilla(SCI_FINDINDICATORHIDE, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeDisplay(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMEDISPLAY, 0, 0, bDirect);
}

void CScintillaCtrl::VCHomeDisplayExtend(BOOL bDirect)
{
    CallScintilla(SCI_VCHOMEDISPLAYEXTEND, 0, 0, bDirect);
}

BOOL CScintillaCtrl::GetCaretLineVisibleAlways(BOOL bDirect)
{
    return CallScintilla(SCI_GETCARETLINEVISIBLEALWAYS, 0, 0, bDirect);
}

void CScintillaCtrl::SetCaretLineVisibleAlways(BOOL bAlwaysVisible, BOOL bDirect)
{
    CallScintilla(SCI_SETCARETLINEVISIBLEALWAYS, static_cast<WPARAM>(bAlwaysVisible), 0, bDirect);
}

void CScintillaCtrl::StartRecord(BOOL bDirect)
{
    CallScintilla(SCI_STARTRECORD, 0, 0, bDirect);
}

void CScintillaCtrl::StopRecord(BOOL bDirect)
{
    CallScintilla(SCI_STOPRECORD, 0, 0, bDirect);
}

void CScintillaCtrl::SetLexer(int lexer, BOOL bDirect)
{
    CallScintilla(SCI_SETLEXER, static_cast<WPARAM>(lexer), 0, bDirect);
}

int CScintillaCtrl::GetLexer(BOOL bDirect)
{
    return CallScintilla(SCI_GETLEXER, 0, 0, bDirect);
}

void CScintillaCtrl::Colourise(long start, long end, BOOL bDirect)
{
    CallScintilla(SCI_COLOURISE, static_cast<WPARAM>(start), static_cast<LPARAM>(end), bDirect);
}

void CScintillaCtrl::SetProperty(const char* key, const char* value, BOOL bDirect)
{
    CallScintilla(SCI_SETPROPERTY, reinterpret_cast<WPARAM>(key), reinterpret_cast<LPARAM>(value), bDirect);
}

void CScintillaCtrl::SetKeyWords(int keywordSet, const char* keyWords, BOOL bDirect)
{
    CallScintilla(SCI_SETKEYWORDS, static_cast<WPARAM>(keywordSet), reinterpret_cast<LPARAM>(keyWords), bDirect);
}

void CScintillaCtrl::SetLexerLanguage(const char* language, BOOL bDirect)
{
    CallScintilla(SCI_SETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(language), bDirect);
}

void CScintillaCtrl::LoadLexerLibrary(const char* path, BOOL bDirect)
{
    CallScintilla(SCI_LOADLEXERLIBRARY, 0, reinterpret_cast<LPARAM>(path), bDirect);
}

int CScintillaCtrl::GetProperty(const char* key, char* buf, BOOL bDirect)
{
    return CallScintilla(SCI_GETPROPERTY, reinterpret_cast<WPARAM>(key), reinterpret_cast<LPARAM>(buf), bDirect);
}

int CScintillaCtrl::GetPropertyExpanded(const char* key, char* buf, BOOL bDirect)
{
    return CallScintilla(SCI_GETPROPERTYEXPANDED, reinterpret_cast<WPARAM>(key), reinterpret_cast<LPARAM>(buf), bDirect);
}

int CScintillaCtrl::GetPropertyInt(const char* key, BOOL bDirect)
{
    return CallScintilla(SCI_GETPROPERTYINT, reinterpret_cast<WPARAM>(key), 0, bDirect);
}

int CScintillaCtrl::GetStyleBitsNeeded(BOOL bDirect)
{
    return CallScintilla(SCI_GETSTYLEBITSNEEDED, 0, 0, bDirect);
}

int CScintillaCtrl::GetLexerLanguage(char* text, BOOL bDirect)
{
    return CallScintilla(SCI_GETLEXERLANGUAGE, 0, reinterpret_cast<LPARAM>(text), bDirect);
}

void* CScintillaCtrl::PrivateLexerCall(int operation, void* pointer, BOOL bDirect)
{
    return reinterpret_cast<void*>(CallScintilla(SCI_PRIVATELEXERCALL, static_cast<WPARAM>(operation), reinterpret_cast<LPARAM>(pointer), bDirect));
}

int CScintillaCtrl::PropertyNames(char* names, BOOL bDirect)
{
    return CallScintilla(SCI_PROPERTYNAMES, 0, reinterpret_cast<LPARAM>(names), bDirect);
}

int CScintillaCtrl::PropertyType(const char* name, BOOL bDirect)
{
    return CallScintilla(SCI_PROPERTYTYPE, reinterpret_cast<WPARAM>(name), 0, bDirect);
}

int CScintillaCtrl::DescribeProperty(const char* name, char* description, BOOL bDirect)
{
    return CallScintilla(SCI_DESCRIBEPROPERTY, reinterpret_cast<WPARAM>(name), reinterpret_cast<LPARAM>(description), bDirect);
}

int CScintillaCtrl::DescribeKeyWordSets(char* descriptions, BOOL bDirect)
{
    return CallScintilla(SCI_DESCRIBEKEYWORDSETS, 0, reinterpret_cast<LPARAM>(descriptions), bDirect);
}

void CScintillaCtrl::SetLineEndTypesAllowed(int lineEndBitSet, BOOL bDirect)
{
    CallScintilla(SCI_SETLINEENDTYPESALLOWED, static_cast<WPARAM>(lineEndBitSet), 0, bDirect);
}

int CScintillaCtrl::GetLineEndTypesAllowed(BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEENDTYPESALLOWED, 0, 0, bDirect);
}

int CScintillaCtrl::GetLineEndTypesActive(BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEENDTYPESACTIVE, 0, 0, bDirect);
}

int CScintillaCtrl::GetLineEndTypesSupported(BOOL bDirect)
{
    return CallScintilla(SCI_GETLINEENDTYPESSUPPORTED, 0, 0, bDirect);
}

int CScintillaCtrl::AllocateSubStyles(int styleBase, int numberStyles, BOOL bDirect)
{
    return CallScintilla(SCI_ALLOCATESUBSTYLES, static_cast<WPARAM>(styleBase), static_cast<LPARAM>(numberStyles), bDirect);
}

int CScintillaCtrl::GetSubStylesStart(int styleBase, BOOL bDirect)
{
    return CallScintilla(SCI_GETSUBSTYLESSTART, static_cast<WPARAM>(styleBase), 0, bDirect);
}

int CScintillaCtrl::GetSubStylesLength(int styleBase, BOOL bDirect)
{
    return CallScintilla(SCI_GETSUBSTYLESLENGTH, static_cast<WPARAM>(styleBase), 0, bDirect);
}

void CScintillaCtrl::FreeSubStyles(BOOL bDirect)
{
    CallScintilla(SCI_FREESUBSTYLES, 0, 0, bDirect);
}

void CScintillaCtrl::SetIdentifiers(int style, const char* identifiers, BOOL bDirect)
{
    CallScintilla(SCI_SETIDENTIFIERS, static_cast<WPARAM>(style), reinterpret_cast<LPARAM>(identifiers), bDirect);
}

int CScintillaCtrl::DistanceToSecondaryStyles(BOOL bDirect)
{
    return CallScintilla(SCI_DISTANCETOSECONDARYSTYLES, 0, 0, bDirect);
}

int CScintillaCtrl::GetSubStyleBases(char* styles, BOOL bDirect)
{
    return CallScintilla(SCI_GETSUBSTYLEBASES, 0, reinterpret_cast<LPARAM>(styles), bDirect);
}

CScintillaEditState::CScintillaEditState() : bRegularExpression(FALSE),
    pFindReplaceDlg(NULL),
    bWord(FALSE),
    bFindOnly(TRUE),
    bCase(FALSE),
    bNext(TRUE)
{

}

BEGIN_MESSAGE_MAP(CScintillaFindReplaceDlg, CFindReplaceDialog)
    ON_BN_CLICKED(IDC_REGULAR_EXPRESSION, &CScintillaFindReplaceDlg::OnRegularExpression)
END_MESSAGE_MAP()

CScintillaFindReplaceDlg::CScintillaFindReplaceDlg() : m_bRegularExpression(FALSE)
{

}

BOOL CScintillaFindReplaceDlg::Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat, LPCTSTR lpszReplaceWith, DWORD dwFlags, CWnd* pParentWnd)
{
    // Tell Windows to use our dialog instead of the standard one
    m_fr.Flags |= FR_ENABLETEMPLATE;
    if (bFindDialogOnly) {
        m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_SCINTILLA_FINDDLGORD);
    }
    else {
        m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_SCINTILLA_REPLACEDLGORD);
    }
    m_fr.hInstance = ::AfxFindResourceHandle(m_fr.lpTemplateName, RT_DIALOG);

    //Let the base class do its thing
    return CFindReplaceDialog::Create(bFindDialogOnly, lpszFindWhat, lpszReplaceWith, dwFlags, pParentWnd);
}

BOOL CScintillaFindReplaceDlg::OnInitDialog()
{
    //let the base class do its thing
    BOOL bReturn = CFindReplaceDialog::OnInitDialog();

    //Should we check the regular expression check box
    CButton* pCtrl = static_cast<CButton*>(GetDlgItem(IDC_REGULAR_EXPRESSION));
    AFXASSUME(pCtrl != NULL);
    pCtrl->SetCheck(m_bRegularExpression);

    return bReturn;
}

void CScintillaFindReplaceDlg::OnRegularExpression()
{
    //Save the state of the Regular expression checkbox into a member variable
    CButton* pCtrl = static_cast<CButton*>(GetDlgItem(IDC_REGULAR_EXPRESSION));
    AFXASSUME(pCtrl != NULL);
    m_bRegularExpression = (pCtrl->GetCheck() == 1);
}

