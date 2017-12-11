#include "StdAfx.h"
#include "Compiler.h"

bool CCompiler::CompileCpp(const char* pszFile)
{
    if(false == m_oParser.Parse(pszFile))
    {
        m_strErrMsg = m_oParser.m_strErrMsg;
        return false;
    }

    size_t i;
    for(i = 0; i < m_oParser.m_oVecTableInfo.size(); i++)
    {
        CTableClassInfo oClassInfo;
        if(false == _CompileClassInfo(m_oParser.m_oVecTableInfo[i], oClassInfo))
        {
            return false;
        }

        m_oVecTableClassInfo.push_back(oClassInfo);
    }

    for(i = 0; i < m_oParser.m_oVecTableInfo.size(); i++)
    {
        if(false == _WriteTblHeaderFile(m_oVecTableClassInfo[i]))
        {
            return false;
        }
        if(false == _WriteTblCppFile(m_oVecTableClassInfo[i]))
        {
            return false;
        }
    }

    if(false == _WriteTableMgrFileCpp())
    {
        return false;
    }

    m_strErrMsg.Format("Compile %s OK", pszFile);
    return true;
}

bool CCompiler::_GetTypeAndPrefix(INT32 nType, CString& rstrType, CString& rstrPrefix)
{
    switch(nType)
    {
    case DATATYPE_INT8:
        rstrType = "INT8";
        rstrPrefix = "c";
        break;
    case DATATYPE_UINT8:
        rstrType = "UINT8";
        rstrPrefix = "by";
        break;
    case DATATYPE_INT16:
        rstrType = "INT16";
        rstrPrefix = "sh";
        break;
    case DATATYPE_UINT16:
        rstrType = "UINT16";
        rstrPrefix = "w";
        break;
    case DATATYPE_INT32:
        rstrType = "INT32";
        rstrPrefix = "n";
        break;
    case DATATYPE_UINT32:
        rstrType = "UINT32";
        rstrPrefix = "dw";
        break;
    case DATATYPE_INT64:
        rstrType = "INT64";
        rstrPrefix = "l";
        break;
    case DATATYPE_UINT64:
        rstrType = "UINT64";
        rstrPrefix = "qw";
        break;
    case DATATYPE_STRING:
        rstrType = "string";
        rstrPrefix = "str";
        break;
    case DATATYPE_COMMENT:
        rstrType = "comment";
        rstrPrefix = "str";
        break;
    case DATATYPE_DOUBLE:
        rstrType = "double";
        rstrPrefix = "d";
        break;
    default:
        return false;
    }

    return true;
}

bool CCompiler::_CompileClassInfo(const CTableInfo& roTableInfo, CTableClassInfo& roClassInfo)
{
    roClassInfo.m_strName = roTableInfo.m_strName.c_str();
    roClassInfo.m_strTblFile = roTableInfo.m_strFile.c_str();
    roClassInfo.m_nKeyMode = roTableInfo.m_nKeyMode;
    roClassInfo.m_strHFile.Format("%s.h", roTableInfo.m_strName.c_str());
    roClassInfo.m_strHFile.MakeLower();
    roClassInfo.m_strCppFile.Format("%s.cpp", roTableInfo.m_strName.c_str());
    roClassInfo.m_strCppFile.MakeLower();
    roClassInfo.m_strCSFile.Format("%s.cs", roTableInfo.m_strName.c_str());
    roClassInfo.m_strCSFile.MakeLower();
	roClassInfo.m_strPyFile.Format("%s.py", roTableInfo.m_strName.c_str());
	roClassInfo.m_strPyFile.MakeLower();
    roClassInfo.m_strIfDef.Format("%s_H", roTableInfo.m_strName.c_str());
    roClassInfo.m_strIfDef.MakeUpper();
    roClassInfo.m_strClassName.Format("C%s", roTableInfo.m_strName.c_str());
	roClassInfo.m_bServerOnly = roTableInfo.m_bServerOnly;
	roClassInfo.m_bClientSkipLoad = roTableInfo.m_bClientSkipLoad;

    for(size_t i = 0; i < roTableInfo.m_oCols.size(); i++)
    {
        CString strType;
        CString strPreFix;

        if(false == _GetTypeAndPrefix(roTableInfo.m_oCols[i].m_nType, strType, strPreFix))
        {
            m_strErrMsg.Format("_GetTypeAndPrefix failed, type %d unknown", roTableInfo.m_oCols[i].m_nType);
            return false;
        }

        CStructItemInfo oItem;
        oItem.m_nDataType = roTableInfo.m_oCols[i].m_nType;
        oItem.m_strMemberType = strType;
        oItem.m_strMemberName.Format("%s%s", strPreFix, roTableInfo.m_oCols[i].m_strName.c_str());
        oItem.m_strRawName = roTableInfo.m_oCols[i].m_strName.c_str();

        if(roTableInfo.m_strKey == roTableInfo.m_oCols[i].m_strName.c_str())
        {
            roClassInfo.m_strKeyType = oItem.m_strMemberType;
            roClassInfo.m_strKeyName = oItem.m_strMemberName;
            roClassInfo.m_strKeyPrefix = strPreFix;
        }

        if(KEY_MODE_MULITI == roTableInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roTableInfo.m_nKeyMode)
        {
            if(roTableInfo.m_strKey2 == roTableInfo.m_oCols[i].m_strName.c_str())
            {
                roClassInfo.m_strKey2Type = oItem.m_strMemberType;
                roClassInfo.m_strKey2Name = oItem.m_strMemberName;
            }
        }

        roClassInfo.m_oVecItems.push_back(oItem);
    }
    
    return true;
}

bool CCompiler::_WriteTblHeaderFile(CTableClassInfo& roClassInfo)
{
    CString strHFile;
    strHFile.Format("%s\\%s", m_strOutputPath, roClassInfo.m_strHFile);
    FILE* fp = fopen(strHFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", roClassInfo.m_strHFile);
        return false;
    }

    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");

    fprintf(fp,
        "#ifndef %s_H\n"
        "#define %s_H\n"
        "\n",
        roClassInfo.m_strIfDef, roClassInfo.m_strIfDef);

    fprintf(fp,
        "#include <map>\n"
        "#include <string>\n");

    if(KEY_MODE_SET == roClassInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "#include <vector>\n");
    }

    fprintf(fp,
        "#include \"lvtype.h\"\n"
        "\n"
        "using namespace std;\n"
        "\n");

    fprintf(fp,
        "class %s\n"
        "{\n"
        "public:\n"
        "    class CItem\n"
        "    {\n"
        "    public:\n"
        "        CItem():\n",
        roClassInfo.m_strClassName);

    size_t i;
    bool bFirstConstruct = true;
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType || DATATYPE_STRING == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            continue;
        }
        if(true == bFirstConstruct)
        {
            bFirstConstruct = false;
            fprintf(fp, "        m_%s(0)", roClassInfo.m_oVecItems[i].m_strMemberName);
        }
        else
        {
            fprintf(fp, ",\n        m_%s(0)", roClassInfo.m_oVecItems[i].m_strMemberName);
        }
    }

    fprintf(fp,
        "\n"
        "        {\n"
        "        }\n\n"
        "        CItem(const CItem& o)\n"
        "        {\n");
    
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            continue;
        }
        fprintf(fp, "            m_%s = o.m_%s;\n", roClassInfo.m_oVecItems[i].m_strMemberName, roClassInfo.m_oVecItems[i].m_strMemberName);
    }

    fprintf(fp,
        "        }\n\n"
        "        CItem& operator=(const CItem& o)\n"
        "        {\n");

    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            continue;
        }
        fprintf(fp, "            m_%s = o.m_%s;\n", roClassInfo.m_oVecItems[i].m_strMemberName, roClassInfo.m_oVecItems[i].m_strMemberName);
    }

    fprintf(fp,
        "            return *this;\n"
        "        }\n\n");

    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            continue;
        }
        fprintf(fp, "        %s m_%s;\n", roClassInfo.m_oVecItems[i].m_strMemberType, roClassInfo.m_oVecItems[i].m_strMemberName);
    }
    fprintf(fp, "    };\n\n");

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    typedef map<UINT64, CItem> CMapItem;\n");
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, 
            "    typedef vector<CItem> CVecItem;\n"
           "    typedef map<UINT16, CVecItem> CMapItem;\n");
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, 
			"    typedef vector<CItem> CVecItem;\n"
			"    typedef map<UINT64, CVecItem> CMapItem;\n");
	}
    else
    {
        fprintf(fp, "    typedef map<%s, CItem> CMapItem;\n", roClassInfo.m_strKeyType);
    }
    fprintf(fp, 
        "\n"
        "    %s();\n"
        "    ~%s();\n"
        "\n"
        "    bool LoadFromFile(const std::string &strConfigDir= \"\");\n",
        roClassInfo.m_strClassName,
        roClassInfo.m_strClassName);

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s::CItem* Get(%s %s, %s %s);\n", 
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, "    %s::CVecItem* Get(%s %s, %s %s);\n", 
			roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
	}
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s::CVecItem* Get(%s %s);\n", 
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }
    else
    {
        fprintf(fp, "    %s::CItem* Get(%s %s);\n", roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }

    fprintf(fp,    
        "    %s::CMapItem& GetAllItems() { return m_oMapItem; }\n"
        "\n"
        "protected:\n",
        roClassInfo.m_strClassName);

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    bool _AddItem(%s %s, %s %s, CItem* pstItem);\n",
            roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp, "    bool _AddItem(%s %s, CItem* pstItem);\n", roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }
    
    fprintf(fp,
        "\n"
        "    string      m_strFile;\n"
        "    INT32       m_nColCount;\n");
    
    if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s m_%sLastKey;\n", roClassInfo.m_strKeyType, roClassInfo.m_strKeyPrefix);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, "    UINT64 m_%sLastKey;\n", roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    %s m_%sLastKey1;\n", roClassInfo.m_strKeyType,roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    %s m_%sLastKey2;\n", roClassInfo.m_strKey2Type,roClassInfo.m_strKeyPrefix);
	}

    fprintf(fp,
        "    CMapItem    m_oMapItem;\n"
        "};\n"
        "\n");

    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strName);
    }

    fprintf(fp,
        "\n"
        "#endif\n");

    fclose(fp);
    return true;
}

bool CCompiler::_WriteTblCppFile(CTableClassInfo& roClassInfo)
{
    CString strCppFile;
    strCppFile.Format("%s\\%s", m_strOutputPath, roClassInfo.m_strCppFile);
    FILE* fp = fopen(strCppFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", roClassInfo.m_strCppFile);
        return false;
    }

    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");

    fprintf(fp, 
        "#include \"%s\"\n"
        "#include <vector>\n"
        "#include <cstdlib>\n"
        "#include <brickred/string_util.h>\n"
        "#include \"applogger.h\"\n"
        "#include \"common/table_util.h\"\n"
        "#include \"tblhelper.h\"\n"
        "\n",
        roClassInfo.m_strHFile);

	roClassInfo.m_strTblFile = "../settings/" + roClassInfo.m_strTblFile;
    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strName);
    }
    
    fprintf(fp,
        "\n"
        "%s::%s()\n"
        "{\n"
        "    m_strFile = \"%s.txt\";\n"
        "    m_nColCount = %d;\n",
        roClassInfo.m_strClassName, roClassInfo.m_strClassName,
        roClassInfo.m_strTblFile,
        roClassInfo.m_oVecItems.size());

    if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    m_%sLastKey = 0;\n", roClassInfo.m_strKeyPrefix);
    }

	if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, "    m_%sLastKey = 0;\n", roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    m_%sLastKey1 = 0;\n", roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    m_%sLastKey2 = 0;\n", roClassInfo.m_strKeyPrefix);
	}
    fprintf(fp,
        "}\n"
        "\n"
        "%s::~%s()\n"
        "{\n"
        "}\n"
        "\n",
        roClassInfo.m_strClassName, roClassInfo.m_strClassName);

    fprintf(fp,
        "bool %s::LoadFromFile(const std::string &strConfigDir)\n"
        "{\n"
        "    vector<string> oVecLines;\n"
        "    if(false == common::util::getAllLinesFromTblFile(strConfigDir + m_strFile, &oVecLines))\n"
        "    {\n"
        "        CRI(\"Load File %%s failed, can not get all lines\", m_strFile.c_str());\n"
        "        return false;\n"
        "    }\n"
        "\n"
        "    vector<string> oHeaders;\n"
        "    SplitString(oVecLines[1].c_str(), \'\\t\', oHeaders);\n"
        "\n"
        "    if(oHeaders.size() != (size_t)m_nColCount)\n"
        "    {\n"
        "        CRI(\"Load File %%s failed, header count %%zd is not correct, should be %%d\", m_strFile.c_str(), oHeaders.size(), m_nColCount);\n"
        "        return false;\n"
        "    }\n"
        "\n"
        "    INT32 anIndex[%d];\n"
        "    size_t i;\n"
        "    for(i = 0; i < %d; i++)\n"
        "    {\n"
        "        anIndex[i] = -1;\n"
        "    }\n"
        "\n"
        "    for(i = 0; i < %d; i++)\n"
        "    {\n",
        roClassInfo.m_strClassName,
        roClassInfo.m_oVecItems.size(),
        roClassInfo.m_oVecItems.size(),
        roClassInfo.m_oVecItems.size());

    size_t i;
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(0 == i)
        {
            fprintf(fp,
                "        if(oHeaders[i] == \"%s\")\n"
                "        {\n"
                "            anIndex[%d] = (INT32)i;\n"
                "        }\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
        else
        {
            fprintf(fp,
                "        else if(oHeaders[i] == \"%s\")\n"
                "        {\n"
                "            anIndex[%d] = (INT32)i;\n"
                "        }\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
    }

    fprintf(fp,
        "        else\n"
        "        {\n"
        "            CRI(\"Load File %%s failed, header %%s is invalid\", m_strFile.c_str(), oHeaders[i].c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n");
        
    fprintf(fp, 
        "    for(i = 0; i < %d; i++)\n"
        "    {\n"
        "        if(-1 == anIndex[i])\n"
        "        {\n"
        "            CRI(\"Load File %%s failed, not find all headers\", m_strFile.c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n",
        roClassInfo.m_oVecItems.size());

    fprintf(fp,
        "    for(size_t i = 2; i < oVecLines.size(); i++)\n"
        "    {\n"
        "        vector<string> oSegs;\n"
        "        SplitString(oVecLines[i].c_str(), \'\\t\', oSegs);\n"
        "\n"
        "        CItem oItem;\n");
        
    
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            continue;
        }
        else if(DATATYPE_STRING == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp, 
                "        if(oSegs.size() > (size_t)anIndex[%d])\n"
                "        {\n"
                "            oItem.m_%s = oSegs[anIndex[%d]];\n"
                "        }\n",
                i,
                roClassInfo.m_oVecItems[i].m_strMemberName, i);
        }
        else if(DATATYPE_DOUBLE == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp,
                "        oItem.m_%s = GetDoubleFromVectorString(oSegs, anIndex[%d]);\n",
                roClassInfo.m_oVecItems[i].m_strMemberName, i);
        }
        else
        {
            fprintf(fp,
                "        oItem.m_%s = (%s)GetInt32FromVectorString(oSegs, anIndex[%d]);\n",
                roClassInfo.m_oVecItems[i].m_strMemberName, roClassInfo.m_oVecItems[i].m_strMemberType, i);
        }
    }

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode|| KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\n"
            "        if(false == _AddItem(oItem.m_%s, oItem.m_%s, &oItem))\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp,
            "\n"
            "        if(false == _AddItem(oItem.m_%s, &oItem))\n",
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,
        "        {\n"
        "            return false;\n"
        "        }\n"
        "    }\n"
        "\n"
        "    INF(\"Load File %%s ok, %%zd lines, %%d cols\", m_strFile.c_str(), oVecLines.size(), m_nColCount);\n"
        "    return true;\n"
        "}\n"
        "\n");

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "%s::CItem* %s::Get(%s %s, %s %s)\n"
            "{\n"
            "    UINT64 qwKey = %s;\n"
            "    qwKey = (qwKey << 32) | %s;\n"
            "    CMapItem::iterator it = m_oMapItem.find(qwKey);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp,
			"%s::CVecItem* %s::Get(%s %s, %s %s)\n"
			"{\n"
			"    UINT64 qwKey = %s;\n"
			"    qwKey = (qwKey << 32) | %s;\n"
			"    CMapItem::iterator it = m_oMapItem.find(qwKey);\n",
			roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
			roClassInfo.m_strKeyName,
			roClassInfo.m_strKey2Name);
	}  
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "%s::CVecItem* %s::Get(%s %s)\n"
            "{\n"
            "    CMapItem::iterator it = m_oMapItem.find(%s);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName);
    }
    else
    {
        fprintf(fp,
            "%s::CItem* %s::Get(%s %s)\n"
            "{\n"
            "    CMapItem::iterator it = m_oMapItem.find(%s);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,
        "    if(it == m_oMapItem.end())\n"
        "    {\n"
        "        return NULL;\n"
        "    }\n"
        "    return &it->second;\n"
        "}\n"
        "\n");

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "bool %s::_AddItem(%s %s, %s %s, CItem* poItem)\n"
            "{\n"
            "    UINT64 qwKey = %s;\n"
            "    qwKey = (qwKey << 32) | %s;\n"
            "    if(Get(%s, %s) != NULL)\n"
            "    {\n"
            "        CRI(\"Load File %%s failed, %s %%u %s %%u repeat\", m_strFile.c_str(), (UINT32)%s, (UINT32)%s);\n"
            "        return false;\n"
            "    }\n"
            "\n"
            "    m_oMapItem[qwKey] = *poItem;\n"
            "    return true;\n"
            "}\n\n",
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp,
			"bool %s::_AddItem(%s %s,%s %s, CItem* poItem)\n"    //1 
			"{\n"
			"    UINT64 qwKey = %s;\n"//2
			"    qwKey = (qwKey << 32) | %s;\n"//3
			"    if(0 == qwKey)\n"     
			"    {\n"
			"        qwKey = m_%sLastKey;\n"    //4
			"        if(0 == qwKey)\n"     
			"        {\n"
			"            return false;\n"
			"        }\n"
			"        poItem->m_%s = m_%sLastKey1;\n"      //5
			"        poItem->m_%s = m_%sLastKey2;\n"      //5.1
			"        Get(m_%sLastKey1,m_%sLastKey2)->push_back(*poItem);\n"    //6
			"        return true;\n"
			"    }\n\n"
			"    if(Get(%s ,%s) != NULL)\n" //7
			"    {\n"
			"        return false;\n"
			"    }\n"
			"\n"
			"    CVecItem oVecItem;\n"
			"    oVecItem.push_back(*poItem);\n"
			"    m_oMapItem[qwKey] = oVecItem;\n"  //7
			"    m_%sLastKey = qwKey;\n"   //8
			"    m_%sLastKey1 = %s;\n"   //8.1
			"    m_%sLastKey2 = %s;\n"   //8.2
			"    return true;\n"
			"}\n\n",
			roClassInfo.m_strClassName,roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,     //1
			roClassInfo.m_strKeyName,//2
			roClassInfo.m_strKey2Name,//3

			roClassInfo.m_strKeyPrefix,   //4
			roClassInfo.m_strKeyName, roClassInfo.m_strKeyPrefix,//5
			roClassInfo.m_strKey2Name, roClassInfo.m_strKeyPrefix,//5.1
			roClassInfo.m_strKeyPrefix,roClassInfo.m_strKeyPrefix,//6
			roClassInfo.m_strKeyName,  roClassInfo.m_strKey2Name, //7
			roClassInfo.m_strKeyPrefix,
			roClassInfo.m_strKeyPrefix,roClassInfo.m_strKeyName,
			roClassInfo.m_strKeyPrefix,roClassInfo.m_strKey2Name
			);
	}
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "bool %s::_AddItem(%s %s, CItem* poItem)\n"     //1
            "{\n"
            "    if(0 == %s)\n"     //2
            "    {\n"
            "        %s = m_%sLastKey;\n"    //3
            "        if(0 == %s)\n"     //4
            "        {\n"
            "            CRI(\"Load File %%s failed, key of first row is null\", m_strFile.c_str());\n"
            "            return false;\n"
            "        }\n"
            "        poItem->m_%s = %s;\n"      //4.1
            "        Get(%s)->push_back(*poItem);\n"    //4.2
            "        return true;\n"
            "    }\n\n"
            "    if(Get(%s) != NULL)\n" //5
            "    {\n"
            "        CRI(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (UINT32)%s);\n"    //6
            "        return false;\n"
            "    }\n"
            "\n"
            "    CVecItem oVecItem;\n"
            "    oVecItem.push_back(*poItem);\n"
            "    m_oMapItem[%s] = oVecItem;\n"  //7
            "    m_%sLastKey = %s;\n"   //8
            "    return true;\n"
            "}\n\n",
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,     //1
            roClassInfo.m_strKeyName,   //2
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyPrefix,   //3
            roClassInfo.m_strKeyName,   //4
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName, //4.1
            roClassInfo.m_strKeyName, //4.2
            roClassInfo.m_strKeyName,   //5
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName, //6
            roClassInfo.m_strKeyName,    //7
            roClassInfo.m_strKeyPrefix, roClassInfo.m_strKeyName);
    }
    else
    {
        fprintf(fp,
            "bool %s::_AddItem(%s %s, CItem* poItem)\n"
            "{\n"
            "    if(Get(%s) != NULL)\n"
            "    {\n"
            "        CRI(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (UINT32)%s);\n"
            "        return false;\n"
            "    }\n"
            "\n"
            "    m_oMapItem[%s] = *poItem;\n"
            "    return true;\n"
            "}\n"
            "\n",
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName);
    }

    fclose(fp);
    return true;
}

bool CCompiler::_WriteTableMgrFileCpp()
{
    CString strHFile;
    strHFile.Format("%s\\tablemgr.h", m_strOutputPath);

    FILE* fp = fopen(strHFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", strHFile);
        return false;
    }

    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");

    fprintf(fp, 
        "#ifndef TABLEMGR_H\n"
        "#define TABLEMGR_H\n"
        "\n"
        "bool LoadAllTables();\n"
        "\n"
        "#endif\n");

    fclose(fp);

    CString strCppFile;
    strCppFile.Format("%s\\tablemgr.cpp", m_strOutputPath);
    fp = fopen(strCppFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", strCppFile);
        return false;
    }

    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");

    fprintf(fp, "#include \"tablemgr.h\"\n");
    size_t i;
    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        fprintf(fp, "#include \"%s\"\n", m_oVecTableClassInfo[i].m_strHFile);
    }

    fprintf(fp,
        "\n"
        "bool LoadAllTables()\n"
        "{\n");

    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp, 
                "    if(false == g_o%s.LoadFromFile())\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strName);
        }
    }

    fprintf(fp, 
        "    return true;\n"
        "}\n");
	fclose(fp);

    return true;
}