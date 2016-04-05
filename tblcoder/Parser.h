#pragma once

#include <string>
#include <vector>
#include "markupstl.h"

using namespace std;

#define KEY_MODE_SINGLE     0
#define KEY_MODE_MULITI     1
#define KEY_MODE_SET        2
#define KEY_MODE_MULITI_SET 3

class CColInfo
{
public:
    CColInfo()
    {

    }

    CColInfo(const CColInfo& o)
    {
        m_strName   = o.m_strName;
        m_nType     = o.m_nType;
    }

    CColInfo& operator=(const CColInfo& o)
    {
        m_strName   = o.m_strName;
        m_nType     = o.m_nType;
        return *this;
    }

    string  m_strName;
    INT32   m_nType;
};

class CTableInfo
{
public:
    CTableInfo()
    {

    }

    CTableInfo(const CTableInfo& o)
    {
        m_nKeyMode  = o.m_nKeyMode;
        m_strName   = o.m_strName;
        m_strFile   = o.m_strFile;
        m_strKey    = o.m_strKey;
        m_strKey2   = o.m_strKey2;
        m_strObj1   = o.m_strObj1;
        m_strFile1  = o.m_strFile1;
        m_strObj2   = o.m_strObj2;
        m_strFile2  = o.m_strFile2;
        m_strObj3   = o.m_strObj3;
        m_strFile3  = o.m_strFile3;
        m_strObj4   = o.m_strObj4;
        m_strFile4  = o.m_strFile4;
        m_strObj5   = o.m_strObj5;
        m_strFile5  = o.m_strFile5;
        m_strObj6   = o.m_strObj6;
        m_strFile6  = o.m_strFile6;
        m_oCols     = o.m_oCols;
    }

    CTableInfo& operator=(const CTableInfo& o)
    {
        m_nKeyMode  = o.m_nKeyMode;
        m_strName   = o.m_strName;
        m_strFile   = o.m_strFile;
        m_strKey    = o.m_strKey;
        m_strKey2   = o.m_strKey2;
        m_strObj1   = o.m_strObj1;
        m_strFile1  = o.m_strFile1;
        m_strObj2   = o.m_strObj2;
        m_strFile2  = o.m_strFile2;
        m_strObj3   = o.m_strObj3;
        m_strFile3  = o.m_strFile3;
        m_strObj4   = o.m_strObj4;
        m_strFile4  = o.m_strFile4;
        m_strObj5   = o.m_strObj5;
        m_strFile5  = o.m_strFile5;
        m_strObj6   = o.m_strObj6;
        m_strFile6  = o.m_strFile6;
        m_oCols     = o.m_oCols;
        return *this;
    }

    INT32               m_nKeyMode;
    string              m_strName;
    string              m_strFile;
    string              m_strKey;
    string              m_strKey2;
    string              m_strObj1;
    string              m_strFile1;
    string              m_strObj2;
    string              m_strFile2;
    string              m_strObj3;
    string              m_strFile3;
    string              m_strObj4;
    string              m_strFile4;
    string              m_strObj5;
    string              m_strFile5;
    string              m_strObj6;
    string              m_strFile6;
    vector<CColInfo>    m_oCols;
};

enum DataType
{
    DATATYPE_INVALID=0,
    DATATYPE_INT8   = 1,
    DATATYPE_UINT8  = 2,
    DATATYPE_INT16  = 3,
    DATATYPE_UINT16 = 4,
    DATATYPE_INT32  = 5,
    DATATYPE_UINT32 = 6,
    DATATYPE_INT64  = 7,
    DATATYPE_UINT64 = 8,
    DATATYPE_STRING = 9,
    DATATYPE_COMMENT = 10,
    DATATYPE_DOUBLE = 11
};

class CParser
{
public:
    CParser(void);
    ~CParser(void);

    bool Parse(const char* pszFile);


protected:
    bool _ParseTable(CMarkupSTL& roXml, CTableInfo& roTableInfo);
    INT32 _GetDataType(const string& strType);

    bool _CheckAllTables();
    bool _CheckTableNameRepeat();
    bool _CheckTable(CTableInfo& roTableInfo);

public:
    CString                 m_strErrMsg;
    vector<CTableInfo>      m_oVecTableInfo;
};
