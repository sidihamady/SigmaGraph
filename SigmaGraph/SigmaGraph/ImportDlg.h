// ----------------------------------------------------------------------------------------------
// SigmaGraph
// Data Plotting and Analysis Software
// Copyright(C) 1997-2020  Pr. Sidi HAMADY
// http://www.hamady.org
// sidi@hamady.org
// Released under the MIT License:
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and / or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions :
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
// AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ----------------------------------------------------------------------------------------------

#pragma once
#include "afxwin.h"

#include "ReadonlyEdit.h"
#include "afxcmn.h"

// CImportDlg dialog

class CImportDlg : public CDialog
{
    DECLARE_DYNAMIC(CImportDlg)

public:
    CImportDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CImportDlg();

// Dialog Data
    enum { IDD = IDD_IMPORT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_MESSAGE_MAP()

public:
    CString m_strFilename;
    CReadonlyEdit m_edtFileContent;
    CReadonlyEdit m_edtFilename;
    CReadonlyEdit m_edtFileInfo;
    int_t m_nRowCount;
    CComboBox m_cbSeparator;
    int_t m_nSeparator;
    char_t m_cSeparator;
    int_t m_nSkip;
    BOOL m_bReadTitle;
    int_t m_nColCount;
    char_t *m_pszNote;
    CEdit m_edtSelectedColumns;
    CString m_strSelectedColumns;

    int_t *m_piColumn;
    vector_t *m_pColumn;
    int_t *m_pColumnCount;
    long_t *m_pCurrentID;
    BOOL *m_pbIsModified;
    BOOL *m_pbRunning;
    BOOL m_bOK;

    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    BOOL GetFileContent(CString strFilename);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnFileBrowse();
    afx_msg void OnOK();
    afx_msg void OnCancel();
};
