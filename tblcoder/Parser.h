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
    CColInfo() :
		m_nType(0)
    {
    }

    string  m_strName;
    INT32   m_nType;
};

class CTableInfo
{
public:
    CTableInfo() :
		m_nKeyMode(0),
		m_bServerOnly(false),
		m_bClientSkipLoad(false)
    {
    }

    INT32               m_nKeyMode;
    string              m_strName;
    string              m_strFile;
    string              m_strKey;
    string              m_strKey2;
	bool                m_bServerOnly;
	bool                m_bClientSkipLoad;
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
