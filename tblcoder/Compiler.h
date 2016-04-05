#pragma once

#include <vector>
#include "Parser.h"

using namespace std;

class CStructItemInfo
{
public:
    CStructItemInfo()
    {

    }

    CStructItemInfo(const CStructItemInfo& o)
    {
        m_nDataType     = o.m_nDataType;
        m_strMemberType = o.m_strMemberType;
        m_strMemberName = o.m_strMemberName;
        m_strRawName    = o.m_strRawName;
    }

    CStructItemInfo& operator=(const CStructItemInfo& o)
    {
        m_nDataType     = o.m_nDataType;
        m_strMemberType = o.m_strMemberType;
        m_strMemberName = o.m_strMemberName;
        m_strRawName    = o.m_strRawName;

        return *this;
    }

    INT32   m_nDataType;
    CString m_strMemberType;
    CString m_strMemberName;
    CString m_strRawName;
};

class CTableClassInfo
{
public:
    CTableClassInfo()
    {

    }

    CTableClassInfo(const CTableClassInfo& o)
    {
        m_nKeyMode = o.m_nKeyMode;
        m_strName = o.m_strName;
        m_strTblFile = o.m_strTblFile;
        m_strHFile = o.m_strHFile;
        m_strCppFile = o.m_strCppFile;
        m_strCSFile = o.m_strCSFile;
		m_strPyFile = o.m_strPyFile;
        m_strIfDef = o.m_strIfDef;
        m_strClassName = o.m_strClassName;
        m_strKeyType = o.m_strKeyType;
        m_strKeyName = o.m_strKeyName;
        m_strKeyPrefix = o.m_strKeyPrefix;
        m_strKey2Type = o.m_strKey2Type;
        m_strKey2Name = o.m_strKey2Name;
        m_strObj1 = o.m_strObj1;
        m_strFile1 = o.m_strFile1;
        m_strObj2 = o.m_strObj2;
        m_strFile2 = o.m_strFile2;
        m_strObj3 = o.m_strObj3;
        m_strFile3 = o.m_strFile3;
        m_strObj4 = o.m_strObj4;
        m_strFile4 = o.m_strFile4;
        m_strObj5 = o.m_strObj5;
        m_strFile5 = o.m_strFile5;
        m_strObj6 = o.m_strObj6;
        m_strFile6 = o.m_strFile6;

        m_oVecItems = o.m_oVecItems;
    }

    CTableClassInfo& operator=(const CTableClassInfo& o)
    {
        m_nKeyMode = o.m_nKeyMode;
        m_strName = o.m_strName;
        m_strTblFile = o.m_strTblFile;
        m_strHFile = o.m_strHFile;
        m_strCppFile = o.m_strCppFile;
        m_strCSFile = o.m_strCSFile;
		m_strPyFile = o.m_strPyFile;
        m_strIfDef = o.m_strIfDef;
        m_strClassName = o.m_strClassName;
        m_strKeyType = o.m_strKeyType;
        m_strKeyName = o.m_strKeyName;
        m_strKeyPrefix = o.m_strKeyPrefix;
        m_strKey2Type = o.m_strKey2Type;
        m_strKey2Name = o.m_strKey2Name;
        m_strObj1 = o.m_strObj1;
        m_strFile1 = o.m_strFile1;
        m_strObj2 = o.m_strObj2;
        m_strFile2 = o.m_strFile2;
        m_strObj3 = o.m_strObj3;
        m_strFile3 = o.m_strFile3;
        m_strObj4 = o.m_strObj4;
        m_strFile4 = o.m_strFile4;
        m_strObj5 = o.m_strObj5;
        m_strFile5 = o.m_strFile5;
        m_strObj6 = o.m_strObj6;
        m_strFile6 = o.m_strFile6;
        m_oVecItems = o.m_oVecItems;

        return *this;
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
    CString m_strObj1;
    CString m_strFile1;
    CString m_strObj2;
    CString m_strFile2;
    CString m_strObj3;
    CString m_strFile3;
    CString m_strObj4;
    CString m_strFile4;
    CString m_strObj5;
    CString m_strFile5;
    CString m_strObj6;
    CString m_strFile6;

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
