#include "StdAfx.h"
#include "Parser.h"

const char* ELEM_TABLES     = "tables";
const char* ELEM_TABLE      = "table";

const char* ATTR_NAME       = "name";
const char* ATTR_FILE       = "file";
const char* ATTR_OBJ1       = "obj1";
const char* ATTR_FILE1      = "file1";
const char* ATTR_OBJ2       = "obj2";
const char* ATTR_FILE2      = "file2";
const char* ATTR_OBJ3       = "obj3";
const char* ATTR_FILE3      = "file3";
const char* ATTR_OBJ4       = "obj4";
const char* ATTR_FILE4      = "file4";
const char* ATTR_OBJ5       = "obj5";
const char* ATTR_FILE5      = "file5";
const char* ATTR_OBJ6       = "obj6";
const char* ATTR_FILE6      = "file6";
const char* ATTR_KEY        = "key";
const char* ATTR_MULTYKEY   = "multikey";
const char* ATTR_SETKEY     = "setkey";
const char* ATTR_MULTYSETKEY = "multisetkey";

const char* ELEM_COL        = "col";

const char* ATTR_TYPE       = "type";

void _SplitString(const char* pszString, char chSep, vector<string>& roVecSegs)
{
    if(NULL == pszString)
    {
        return;
    }

    const char* pStart = pszString;
    const char* pEnd = NULL;

    while(*pStart)
    {
        string strSeg;
        bool bEnd = false;
        pEnd = strchr(pStart, chSep);
        if(NULL == pEnd)
        {
            bEnd = true;
            strSeg = pStart;
        }
        else
        {
            strSeg.assign(pStart, pEnd);
        }

        roVecSegs.push_back(strSeg);

        if(bEnd)
        {
            break;
        }
        pStart = pEnd+1;
    }
}

CParser::CParser(void)
{
}

CParser::~CParser(void)
{
}

bool CParser::Parse(const char* pszFile)
{
    CMarkupSTL oXml;
    if( false == oXml.Load(pszFile))
    {
        m_strErrMsg.Format("Load %s failed: %s", pszFile, oXml.GetError().c_str());
        return false;
    }

    if(false == oXml.FindElem("tables"))
    {
        m_strErrMsg.Format("Can not find element tables");
        return false;
    }
    if(false == oXml.IntoElem())
    {
        m_strErrMsg.Format("Can not into element tables");
        return false;
    }

    while(oXml.FindElem("table"))
    {
        CTableInfo oTableInfo;
        if(false == _ParseTable(oXml, oTableInfo))
        {
            return false;
        }

        m_oVecTableInfo.push_back(oTableInfo);
    }

    if(false == _CheckAllTables())
    {
        return false;
    }

    return true;
}

bool CParser::_ParseTable(CMarkupSTL& roXml, CTableInfo& roTableInfo)
{
    roTableInfo.m_strName = roXml.GetAttrib(ATTR_NAME);
    if(roTableInfo.m_strName.empty())
    {
        m_strErrMsg.Format("can not find attr %s in element %s", ATTR_NAME, ELEM_TABLE);
        return false;
    }

    roTableInfo.m_strFile = roXml.GetAttrib(ATTR_FILE);
    roTableInfo.m_strObj1 = roXml.GetAttrib(ATTR_OBJ1);
    roTableInfo.m_strFile1 = roXml.GetAttrib(ATTR_FILE1);
    roTableInfo.m_strObj2 = roXml.GetAttrib(ATTR_OBJ2);
    roTableInfo.m_strFile2 = roXml.GetAttrib(ATTR_FILE2);
    roTableInfo.m_strObj3 = roXml.GetAttrib(ATTR_OBJ3);
    roTableInfo.m_strFile3 = roXml.GetAttrib(ATTR_FILE3);
    roTableInfo.m_strObj4 = roXml.GetAttrib(ATTR_OBJ4);
    roTableInfo.m_strFile4 = roXml.GetAttrib(ATTR_FILE4);
    roTableInfo.m_strObj5 = roXml.GetAttrib(ATTR_OBJ5);
    roTableInfo.m_strFile5 = roXml.GetAttrib(ATTR_FILE5);
    roTableInfo.m_strObj6 = roXml.GetAttrib(ATTR_OBJ6);
    roTableInfo.m_strFile6 = roXml.GetAttrib(ATTR_FILE6);

    roTableInfo.m_strKey = roXml.GetAttrib(ATTR_KEY);
    string strMultiKey = roXml.GetAttrib(ATTR_MULTYKEY);
    string strSetKey = roXml.GetAttrib(ATTR_SETKEY);
    string strMultiSetKey = roXml.GetAttrib(ATTR_MULTYSETKEY);

    if(false == roTableInfo.m_strKey.empty())
    {
        roTableInfo.m_nKeyMode = KEY_MODE_SINGLE;
    }
    else if(false == strMultiKey.empty())
    {
        vector<string> oKeys;
        _SplitString(strMultiKey.c_str(), ',', oKeys);
        if(oKeys.size() != 2)
        {
            m_strErrMsg.Format("multi keys %s is not correct", strMultiKey.c_str());
            return false;
        }

        roTableInfo.m_strKey = oKeys[0];
        roTableInfo.m_strKey2 = oKeys[1];

        if(roTableInfo.m_strKey.empty() || roTableInfo.m_strKey2.empty())
        {
            m_strErrMsg.Format("multi key is null");
            return false;
        }

        roTableInfo.m_nKeyMode = KEY_MODE_MULITI;
    }
    else if(false == strSetKey.empty())
    {
        roTableInfo.m_strKey = strSetKey;
        roTableInfo.m_nKeyMode = KEY_MODE_SET;
    }
    else if(false == strMultiSetKey.empty())
    {
        vector<string> oKeys;
        _SplitString(strMultiSetKey.c_str(), ',', oKeys);
        if(oKeys.size() != 2)
        {
            m_strErrMsg.Format("multi keys %s is not correct", strMultiKey.c_str());
            return false;
        }
        roTableInfo.m_strKey = oKeys[0];
        roTableInfo.m_strKey2 = oKeys[1];
        if(roTableInfo.m_strKey.empty() || roTableInfo.m_strKey2.empty())
        {
            m_strErrMsg.Format("multi key is null");
            return false;
        }
        roTableInfo.m_nKeyMode = KEY_MODE_MULITI_SET;
    }
    else
    {
        m_strErrMsg.Format("can not find attr %s or %s or %s in element %s", ATTR_KEY, ATTR_MULTYKEY, ATTR_SETKEY, ELEM_TABLE);
        return false;
    }

    if(false == roXml.IntoElem())
    {
        m_strErrMsg.Format("can not into elem %s", ELEM_TABLE);
        return false;
    }
    
    while(roXml.FindElem(ELEM_COL))
    {
        CColInfo oCol;
        oCol.m_strName = roXml.GetAttrib(ATTR_NAME);
        if(oCol.m_strName.empty())
        {
            m_strErrMsg.Format("can not find attr %s in elem %s", ATTR_NAME, ELEM_COL);
            return false;
        }

        string strType = roXml.GetAttrib(ATTR_TYPE);
        if(strType.empty())
        {
            m_strErrMsg.Format("can not find attr %s in elem %s", ATTR_TYPE, ELEM_COL);
            return false;
        }

        oCol.m_nType = _GetDataType(strType);
        if(DATATYPE_INVALID == oCol.m_nType)
        {
            m_strErrMsg.Format("invalid type %s in elem %s", strType.c_str(), ELEM_COL);
            return false;
        }

        roTableInfo.m_oCols.push_back(oCol);
    }

    roXml.OutOfElem();

    return true;
}

INT32 CParser::_GetDataType(const string& strType)
{
    if(strType == "INT8")
    {
        return DATATYPE_INT8;
    }
    else if(strType == "UINT8")
    {
        return DATATYPE_UINT8;
    }
    else if(strType == "INT16")
    {
        return DATATYPE_INT16;
    }
    else if(strType == "UINT16")
    {
        return DATATYPE_UINT16;
    }
    else if(strType == "INT32")
    {
        return DATATYPE_INT32;
    }
    else if(strType == "UINT32")
    {
        return DATATYPE_UINT32;
    }
    else if(strType == "INT64")
    {
        return DATATYPE_INT64;
    }
    else if(strType == "UINT64")
    {
        return DATATYPE_UINT64;
    }
    else if(strType == "string")
    {
        return DATATYPE_STRING;
    }
    else if(strType == "comment")
    {
        return DATATYPE_COMMENT;
    }
    else if(strType == "double")
    {
        return DATATYPE_DOUBLE;
    }
    else
    {
        return DATATYPE_INVALID;
    }
}

bool CParser::_CheckAllTables()
{
    if(false == _CheckTableNameRepeat())
        return false;

    for(size_t i = 0; i < m_oVecTableInfo.size(); i++)
    {
        if(false == _CheckTable(m_oVecTableInfo[i]))
        {
            return false;
        }
    }

    return true;
}

bool CParser::_CheckTableNameRepeat()
{
    for(size_t i = 0; i < m_oVecTableInfo.size(); i++)
    {
        for(size_t n = 0; n < m_oVecTableInfo.size(); n++)
        {
            if(i == n)
            {
                continue;
            }

            if(m_oVecTableInfo[i].m_strName == m_oVecTableInfo[n].m_strName)
            {
                m_strErrMsg.Format("table name %s repeat define", m_oVecTableInfo[n].m_strName.c_str());
                return false;
            }

            if(m_oVecTableInfo[i].m_strFile.empty())
            {
                continue;
            }
            if(m_oVecTableInfo[i].m_strFile == m_oVecTableInfo[n].m_strFile)
            {
                m_strErrMsg.Format("table file %s repeat used by two tables", m_oVecTableInfo[n].m_strFile.c_str());
                return false;
            }
        }
    }

    return true;
}

bool CParser::_CheckTable(CTableInfo& roTableInfo)
{
    bool bFindKey = false;
    bool bFindKey2 = false;

    for(size_t i = 0; i < roTableInfo.m_oCols.size(); i++)
    {
        for(size_t n = 0; n < roTableInfo.m_oCols.size(); n++)
        {
            if(i == n)
            {
                continue;
            }

            if(roTableInfo.m_oCols[i].m_strName == roTableInfo.m_oCols[n].m_strName)
            {
                m_strErrMsg.Format("col name %s repeat defined in table %s", roTableInfo.m_oCols[n].m_strName.c_str(), roTableInfo.m_strName.c_str());
                return false;
            }
        }

        if(roTableInfo.m_strKey == roTableInfo.m_oCols[i].m_strName)
        {
            bFindKey = true;
        }

        if(roTableInfo.m_strKey2 == roTableInfo.m_oCols[i].m_strName)
        {
            bFindKey2 = true;
        }
    }

    if(KEY_MODE_SINGLE == roTableInfo.m_nKeyMode || KEY_MODE_SET == roTableInfo.m_nKeyMode)
    {
        if(false == bFindKey)
        {
            m_strErrMsg.Format("key %s not defined in table %s", roTableInfo.m_strKey.c_str(), roTableInfo.m_strName.c_str());
            return false;
        }
    }
    else
    {
        if(false == bFindKey)
        {
            m_strErrMsg.Format("multi key %s not defined in table %s", roTableInfo.m_strKey.c_str(), roTableInfo.m_strName.c_str());
            return false;
        }

        if(false == bFindKey2)
        {
            m_strErrMsg.Format("multi key %s not defined in table %s", roTableInfo.m_strKey2.c_str(), roTableInfo.m_strName.c_str());
            return false;
        }
    }

    return true;
}
