#pragma once

#include <vector>
#include "Parser.h"

using namespace std;

class CStructItemInfo
{
public:
    CStructItemInfo() :
		m_nDataType(0)
    {
    }

    INT32   m_nDataType;
    CString m_strMemberType;
    CString m_strMemberName;
    CString m_strRawName;
};

class CTableClassInfo
{
public:
    CTableClassInfo() :
		m_nKeyMode(0),
		m_bServerOnly(false),
		m_bClientSkipLoad(false)
    {
    }

    INT32   m_nKeyMode;
    CString m_strName;
    CString m_strTblFile;
    CString m_strHFile;
    CString m_strCppFile;
    CString m_strCSFile;
	CString m_strPyFile;
    CString m_strIfDef;
    CString m_strClassName;
    CString m_strKeyType;
    CString m_strKeyName;
    CString m_strKeyPrefix;
    CString m_strKey2Type;
    CString m_strKey2Name;
	bool m_bServerOnly;
	bool m_bClientSkipLoad;

    vector<CStructItemInfo> m_oVecItems;
};

class CCompiler
{
public:
    CCompiler(void);
    ~CCompiler(void);

    bool CompileCpp(const char* pszFile);
	bool CompileCpp2(const char* pszFile);
    bool CompileCS(const char* pszFile);
	bool CompilePy(const char* pszFile);
    
    void SetOutputPath(LPCTSTR pszPath);

protected:
	// Cpp
	bool _CompileClassInfo(const CTableInfo& roTableInfo, CTableClassInfo& roClassInfo);
	bool _GetTypeAndPrefix(INT32 nType, CString& rstrType, CString& rstrPrefix);
    bool _WriteTblHeaderFile(CTableClassInfo& roClassInfo);
    bool _WriteTblCppFile(CTableClassInfo& roClassInfo);
	bool _WriteTableMgrFileCpp();

	// Cpp2
	bool _CompileClassInfo2(const CTableInfo& roTableInfo, CTableClassInfo& roClassInfo);
	bool _GetTypeAndPrefix2(INT32 nType, CString& rstrType, CString& rstrPrefix);
    bool _WriteTblHeaderFile2(CTableClassInfo& roClassInfo);
    bool _WriteTblCppFile2(CTableClassInfo& roClassInfo);
	bool _WriteTableMgrFileCpp2();

	// CSharp
    bool _WriteTblCSFile(CTableClassInfo& roClassInfo);
	bool _WriteTableMgrFileCS();
	
	// Python
	bool _WriteTblPyFile(CTableClassInfo& roClassInfo);
	bool _WriteTableMgrFilePy();

public:
    CString     m_strErrMsg;

protected:
    CParser     m_oParser;
    vector<CTableClassInfo> m_oVecTableClassInfo;
    
    CString m_strOutputPath;
};
