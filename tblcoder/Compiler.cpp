#include "StdAfx.h"
#include "Compiler.h"

CCompiler::CCompiler(void)
{
}

CCompiler::~CCompiler(void)
{
}

void CCompiler::SetOutputPath(LPCTSTR pszPath)
{
    m_strOutputPath = pszPath;
}

//////////////////////////////////////////cpp file2////////////////////////////////////////////////////////////////////////

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
        rstrType = "int8_t";
        rstrPrefix = "c";
        break;
    case DATATYPE_UINT8:
        rstrType = "uint8_t";
        rstrPrefix = "by";
        break;
    case DATATYPE_INT16:
        rstrType = "int16_t";
        rstrPrefix = "sh";
        break;
    case DATATYPE_UINT16:
        rstrType = "uint16_t";
        rstrPrefix = "w";
        break;
    case DATATYPE_INT32:
        rstrType = "int32_t";
        rstrPrefix = "n";
        break;
    case DATATYPE_UINT32:
        rstrType = "uint32_t";
        rstrPrefix = "dw";
        break;
    case DATATYPE_INT64:
        rstrType = "int64_t";
        rstrPrefix = "l";
        break;
    case DATATYPE_UINT64:
        rstrType = "uint64_t";
        rstrPrefix = "qw";
        break;
    case DATATYPE_STRING:
        rstrType = "std::string";
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
    roClassInfo.m_strObj1 = roTableInfo.m_strObj1.c_str();
    roClassInfo.m_strFile1 = roTableInfo.m_strFile1.c_str();
    roClassInfo.m_strObj2 = roTableInfo.m_strObj2.c_str();
    roClassInfo.m_strFile2 = roTableInfo.m_strFile2.c_str();
    roClassInfo.m_strObj3 = roTableInfo.m_strObj3.c_str();
    roClassInfo.m_strFile3 = roTableInfo.m_strFile3.c_str();
    roClassInfo.m_strObj4 = roTableInfo.m_strObj4.c_str();
    roClassInfo.m_strFile4 = roTableInfo.m_strFile4.c_str();
    roClassInfo.m_strObj5 = roTableInfo.m_strObj5.c_str();
    roClassInfo.m_strFile5 = roTableInfo.m_strFile5.c_str();
    roClassInfo.m_strObj6 = roTableInfo.m_strObj6.c_str();
    roClassInfo.m_strFile6 = roTableInfo.m_strFile6.c_str();
    roClassInfo.m_nKeyMode = roTableInfo.m_nKeyMode;
    roClassInfo.m_strHFile.Format("%s.h", roTableInfo.m_strName.c_str());
    roClassInfo.m_strHFile.MakeLower();
    roClassInfo.m_strCppFile.Format("%s.cpp", roTableInfo.m_strName.c_str());
    roClassInfo.m_strCppFile.MakeLower();
    roClassInfo.m_strIfDef.Format("%s_H", roTableInfo.m_strName.c_str());
    roClassInfo.m_strIfDef.MakeUpper();
    roClassInfo.m_strClassName.Format("C%s", roTableInfo.m_strName.c_str());

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
        "#ifndef TABLE_%s\n"
        "#define TABLE_%s\n"
        "\n",
        roClassInfo.m_strIfDef, roClassInfo.m_strIfDef);

    fprintf(fp,
		"#include <stdint.h>\n"
		"#include <cstddef>\n"
        "#include <map>\n"
        "#include <string>\n"
		"#include <vector>\n");

    fprintf(fp,
		"\n"
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
        fprintf(fp, "    typedef std::map<uint64_t, CItem> CMapItem;\n");
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, 
			"    typedef std::vector<CItem> CVecItem;\n"
            "    typedef std::map<%s, CVecItem> CMapItem;\n", roClassInfo.m_strKeyType);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, 
			"    typedef std::vector<CItem> CVecItem;\n"
			"    typedef std::map<uint64_t, CVecItem> CMapItem;\n");
	}
    else
    {
        fprintf(fp, "    typedef std::map<%s, CItem> CMapItem;\n", roClassInfo.m_strKeyType);
    }
    fprintf(fp, 
        "\n"
        "    %s();\n"
        "    ~%s();\n"
        "\n"
        "    bool Load(const std::string &strConfigDir);\n"
        "    bool ReLoad(const std::string &strConfigDir);\n",
        roClassInfo.m_strClassName,
        roClassInfo.m_strClassName);

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s::CItem *Get(%s %s, %s %s);\n", 
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, "    %s::CVecItem *Get(%s %s, %s %s);\n", 
			roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
	}
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s::CVecItem *Get(%s %s);\n", 
            roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }
    else
    {
        fprintf(fp, "    %s::CItem *Get(%s %s);\n", roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }

    fprintf(fp,    
        "    %s::CMapItem &GetAllItems() { return m_oMapItem; }\n"
        "    void Clear() { m_oMapItem.clear(); }\n"
        "protected:\n",
        roClassInfo.m_strClassName);

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    bool _AddItem(%s %s, %s %s, CItem *pstItem);\n",
            roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp, "    bool _AddItem(%s %s, CItem *pstItem);\n", roClassInfo.m_strKeyType, roClassInfo.m_strKeyName);
    }
    
    fprintf(fp,
        "\n"
		"    std::string m_strFile;\n"
        "    int32_t m_nColCount;\n");
    
    if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    %s m_%sLastKey;\n", roClassInfo.m_strKeyType, roClassInfo.m_strKeyPrefix);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp, "    uint64_t m_%sLastKey;\n", roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    %s m_%sLastKey1;\n", roClassInfo.m_strKeyType,roClassInfo.m_strKeyPrefix);
		fprintf(fp, "    %s m_%sLastKey2;\n", roClassInfo.m_strKey2Type,roClassInfo.m_strKeyPrefix);
	}

    fprintf(fp,
        "    CMapItem m_oMapItem;\n"
        "};\n"
        "\n");

    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strName);
    }
    if(false == roClassInfo.m_strObj1.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj1);
    }
    if(false == roClassInfo.m_strObj2.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj2);
    }
    if(false == roClassInfo.m_strObj3.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj3);
    }
    if(false == roClassInfo.m_strObj4.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj4);
    }
    if(false == roClassInfo.m_strObj5.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj5);
    }
    if(false == roClassInfo.m_strObj6.IsEmpty())
    {
        fprintf(fp, "extern %s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj6);
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
        "#include \"%s\"\n\n"
		"#include <cstdlib>\n"
		"#include \"string_util.h\"\n"
		"#include \"table_util.h\"\n"
        "#include \"../Zebra.h\"\n"
        "\n",
        roClassInfo.m_strHFile);

	roClassInfo.m_strTblFile = roClassInfo.m_strTblFile;
    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strName);
    }
    if(false == roClassInfo.m_strObj1.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj1);
    }
    if(false == roClassInfo.m_strObj2.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj2);
    }
    if(false == roClassInfo.m_strObj3.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj3);
    }
    if(false == roClassInfo.m_strObj4.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj4);
    }
    if(false == roClassInfo.m_strObj5.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj5);
    }
    if(false == roClassInfo.m_strObj6.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strObj6);
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
        "bool %s::Load(const std::string &strConfigDir)\n"
        "{\n"
	        "    Clear();\n\n"
		"    std::vector<std::string> oVecLines;\n"
		"    if (false == common::util::getAllLinesFromTblFile(\n"
		"            strConfigDir + m_strFile, &oVecLines)) {\n"
        "        Zebra::logger->error(\"Load File %%s failed, can not get all lines\",\n"
		"                  m_strFile.c_str());\n"
        "        return false;\n"
        "    }\n"
        "\n"
		"    std::vector<std::string> oHeaders;\n"
		"    common::string_util::split(oVecLines[1].c_str(), \"\\t\", &oHeaders);\n"
        "\n"
		"    if (oHeaders.size() != (size_t)m_nColCount) {\n"
        "        Zebra::logger->error(\"Load File %%s failed, header count %%zd is not correct, should be %%d\",\n"
		"                  m_strFile.c_str(), oHeaders.size(), m_nColCount);\n"
        "        return false;\n"
        "    }\n"
        "\n"
        "    int32_t anIndex[%d];\n"
		"    for (size_t i = 0; i < %d; ++i) {\n"
        "        anIndex[i] = -1;\n"
        "    }\n"
        "\n"
		"    for (size_t i = 0; i < %d; ++i) {\n",
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
				"        if (oHeaders[i] == \"%s\") {\n"
                "            anIndex[%d] = (int32_t)i;\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
        else
        {
            fprintf(fp,
				"        } else if (oHeaders[i] == \"%s\") {\n"
                "            anIndex[%d] = (int32_t)i;\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
    }

    fprintf(fp,
		"        } else {\n"
        "            Zebra::logger->error(\"Load File %%s failed, header %%s is invalid\",\n"
		"                      m_strFile.c_str(), oHeaders[i].c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n");
        
    fprintf(fp, 
		"    for (size_t i = 0; i < %d; ++i) {\n"
		"        if (-1 == anIndex[i]) {\n"
        "            Zebra::logger->error(\"Load File %%s failed, not find all headers\",\n"
		"                      m_strFile.c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n",
        roClassInfo.m_oVecItems.size());

    fprintf(fp,
		"    for (size_t i = 2; i < oVecLines.size(); ++i) {\n"
		"        std::vector<std::string> oSegs;\n"
        "        common::string_util::split(oVecLines[i].c_str(), \"\\t\", &oSegs);\n"
        "\n"
		"        if (oSegs.size() != (size_t)m_nColCount) {\n"
		"            Zebra::logger->error(\"Load File %%s failed, line %zd col count %%zd is not correct, should be %%d\",\n"
		"                      i + 1, m_strFile.c_str(), oSegs.size(), m_nColCount);\n"
		"            return false;\n"
		"        }\n"
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
				"        if (oSegs.size() > (size_t)anIndex[%d]) {\n"
				"            oItem.m_%s = common::string_util::trim(oSegs[anIndex[%d]], \"\\\"\");\n"
                "        }\n",
                i,
                roClassInfo.m_oVecItems[i].m_strMemberName, i);
        }
        else if(DATATYPE_DOUBLE == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp,
                "        oItem.m_%s = ::atof(oSegs[anIndex[%d]].c_str());\n",
                roClassInfo.m_oVecItems[i].m_strMemberName, i);
        }
        else
        {
            fprintf(fp,
                "        oItem.m_%s = (%s)::atol(oSegs[anIndex[%d]].c_str());\n",
                roClassInfo.m_oVecItems[i].m_strMemberName, roClassInfo.m_oVecItems[i].m_strMemberType, i);
        }
    }

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode|| KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\n"
            "        if (false == _AddItem(oItem.m_%s, oItem.m_%s, &oItem)) {\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp,
            "\n"
            "        if (false == _AddItem(oItem.m_%s, &oItem)) {\n",
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,
        "            return false;\n"
        "        }\n"
        "    }\n"
        "\n"
        "    Zebra::logger->info(\"Load File %%s ok, %%zd lines, %%d cols\",\n"
		"             m_strFile.c_str(), oVecLines.size(), m_nColCount);\n"
        "    return true;\n"
        "}\n"
        "\n");

    fprintf(fp,
        "bool %s::ReLoad(const std::string &strConfigDir)\n"
        "{\n"
	"    CMapItem tempMap;\n"
	"    tempMap = m_oMapItem;\n"
	"    if (Load(strConfigDir) == true) {\n"
	"        return true;\n"
	"    } else {\n"
	"        m_oMapItem = tempMap;\n"
	"    }\n"
        "    return false;\n"
        "}\n"
        "\n",
        roClassInfo.m_strClassName);

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "%s::CItem *%s::Get(%s %s, %s %s)\n"
            "{\n"
            "    uint64_t qwKey = %s;\n"
            "    qwKey = (qwKey << 32) | %s;\n"
            "    CMapItem::iterator it = m_oMapItem.find(qwKey);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name);
    }
	else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
	{
		fprintf(fp,
			"%s::CVecItem *%s::Get(%s %s, %s %s)\n"
			"{\n"
			"    uint64_t qwKey = %s;\n"
			"    qwKey = (qwKey << 32) | %s;\n"
			"    CMapItem::iterator it = m_oMapItem.find(qwKey);\n",
			roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
			roClassInfo.m_strKeyName,
			roClassInfo.m_strKey2Name);
	}  
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "%s::CVecItem *%s::Get(%s %s)\n"
            "{\n"
            "    CMapItem::iterator it = m_oMapItem.find(%s);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName);
    }
    else
    {
        fprintf(fp,
            "%s::CItem *%s::Get(%s %s)\n"
            "{\n"
            "    CMapItem::iterator it = m_oMapItem.find(%s);\n",
            roClassInfo.m_strClassName, roClassInfo.m_strClassName, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,
		"    if (it == m_oMapItem.end()) {\n"
        "        return NULL;\n"
        "    }\n"
        "    return &it->second;\n"
        "}\n"
        "\n");

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "bool %s::_AddItem(%s %s, %s %s, CItem *poItem)\n"
            "{\n"
            "    uint64_t qwKey = %s;\n"
            "    qwKey = (qwKey << 32) | %s;\n"
			"    if (Get(%s, %s) != NULL) {\n"
            "        Zebra::logger->error(\"Load File %%s failed, %s %%u %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s, (uint32_t)%s);\n"
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
			"bool %s::_AddItem(%s %s, %s %s, CItem *poItem)\n"    //1 
			"{\n"
			"    uint64_t qwKey = %s;\n"//2
			"    qwKey = (qwKey << 32) | %s;\n"//3
			"    if (0 == qwKey)\n"     
			"    {\n"
			"        qwKey = m_%sLastKey;\n"    //4
			"        if(0 == qwKey) {\n"     
			"            return false;\n"
			"        }\n"
			"        poItem->m_%s = m_%sLastKey1;\n"      //5
			"        poItem->m_%s = m_%sLastKey2;\n"      //5.1
			"        Get(m_%sLastKey1,m_%sLastKey2)->push_back(*poItem);\n"    //6
			"        return true;\n"
			"    }\n\n"
			"    if (Get(%s ,%s) != NULL) {\n" //7
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
            "bool %s::_AddItem(%s %s, CItem *poItem)\n"     //1
            "{\n"
            "    if (0 == %s) {\n"     //2
            "        %s = m_%sLastKey;\n"    //3
            "        if(0 == %s) {\n"     //4
            "            Zebra::logger->error(\"Load File %%s failed, key of first row is null\", m_strFile.c_str());\n"
            "            return false;\n"
            "        }\n"
            "        poItem->m_%s = %s;\n"      //4.1
            "        Get(%s)->push_back(*poItem);\n"    //4.2
            "        return true;\n"
            "    }\n\n"
            "    if (Get(%s) != NULL) {\n" //5
            "        Zebra::logger->error(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s);\n"    //6
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
            "bool %s::_AddItem(%s %s, CItem *poItem)\n"
            "{\n"
			"    if (Get(%s) != NULL) {\n"
            "        Zebra::logger->error(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s);\n"
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
        "#ifndef TABLE_TABLEMGR_H\n"
        "#define TABLE_TABLEMGR_H\n"
		"\n"
		"#include <string>\n"
        "\n"
		"namespace table {\n"
		"\n"
		"bool LoadAllTables(const std::string &strConfigDir);\n"
        "\n"
		"} // namespace table\n"
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
		"namespace table {\n"
		"\n"
        "bool LoadAllTables(const std::string &strConfigDir)\n"
        "{\n");

    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp, 
                "    if (false == g_o%s.Load(strConfigDir))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strName);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj1.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj1, m_oVecTableClassInfo[i].m_strFile1);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj2.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj2, m_oVecTableClassInfo[i].m_strFile2);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj3.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj3, m_oVecTableClassInfo[i].m_strFile3);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj4.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj4, m_oVecTableClassInfo[i].m_strFile4);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj5.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj5, m_oVecTableClassInfo[i].m_strFile5);
        }
        if(false == m_oVecTableClassInfo[i].m_strObj6.IsEmpty())
        {
            fprintf(fp,
                "    if (false == g_o%s.Load(strConfigDir + \"%s\"))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strObj6, m_oVecTableClassInfo[i].m_strFile6);
        }
    }

    fprintf(fp, 
        "    return true;\n"
        "}\n"
		"\n"
		"} // namespace table\n");
	fclose(fp);

    return true;
}

//////////////////////////////////////////cs file////////////////////////////////////////////////////////////////////////

bool CCompiler::CompileCS(const char* pszFile)
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
        if(false == _WriteTblCSFile(m_oVecTableClassInfo[i]))
        {
            return false;
        }
    }

    if(false == _WriteTableMgrFileCS())
    {
        return false;
    }

    m_strErrMsg.Format("Compile %s OK", pszFile);
    return true;
}

bool CCompiler::_WriteTblCSFile(CTableClassInfo& roClassInfo)
{
    CString strCSFile;
    CString strCSNameSpcae;
    CString strCStruct;
    size_t i;
    strCStruct.Format("Struc%s",roClassInfo.m_strClassName);
    strCSNameSpcae.Format("Tableload");
    strCSFile.Format("%s\\%s", m_strOutputPath, roClassInfo.m_strCSFile);
    FILE* fp = fopen(strCSFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", roClassInfo.m_strCSFile);
        return false;
    }

    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");

    fprintf(fp, 
        "using System;\n"
		"using TSSGClient;\n"
        "using UnityEngine;\n"
        "using System.Collections.Generic;\n"
        "using System.Text;\n"
        "using System.IO;\n"
        "using System.Collections;\n"
        "using INT8 = System.SByte;\n"
        "using UINT8 = System.Byte;\n"
        "using UINT16 = System.UInt16;\n"
        "using INT16 = System.Int16;\n"
        "using INT32 = System.Int32;\n"
        "using UINT32 = System.UInt32;\n"
        "using INT64 = System.Int64;\n"
        "using UINT64 = System.UInt64;\n"
        "\n\n"
        "namespace %s\n"
        "{\n",
        strCSNameSpcae);
    fprintf(fp,
        "\tpublic class %s\n"
        "\t{\n",
        strCStruct);
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            roClassInfo.m_oVecItems[i].m_strMemberType = "string";
        }
        
        fprintf(fp, "    \tpublic %s m_%s;\n", roClassInfo.m_oVecItems[i].m_strMemberType, roClassInfo.m_oVecItems[i].m_strMemberName);
    }
    fprintf(fp,
    "\t}\n\n");
    fprintf(fp,
        "\tpublic class %s\n"
        "\t{\n"
        "    \tpublic static string m_strFile = \"%s\";\n"
        "    \tpublic static INT32 m_nColCount = %d;\n",
        roClassInfo.m_strClassName,
        roClassInfo.m_strTblFile,
        roClassInfo.m_oVecItems.size());

    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
        {
             fprintf(fp, "\t\tpublic Dictionary<%s,List<%s>> m_h%s = new Dictionary<%s,List<%s>>();\n",
                 roClassInfo.m_strKeyType, strCStruct, roClassInfo.m_strName, roClassInfo.m_strKeyType, strCStruct);
        } else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
        {
            fprintf(fp, "\t\tpublic Dictionary<UINT64,List<%s>> m_h%s = new Dictionary<UINT64,List<%s>>();\n", strCStruct, roClassInfo.m_strName, strCStruct);
        } else
            fprintf(fp, "\t\tpublic Dictionary<%s,%s> m_h%s = new Dictionary<%s,%s>();\n",
                roClassInfo.m_strKeyType, strCStruct, roClassInfo.m_strName, roClassInfo.m_strKeyType, strCStruct);
    }
    if(false == roClassInfo.m_strObj1.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj1);
    }
    if(false == roClassInfo.m_strObj2.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj2);
    }
    if(false == roClassInfo.m_strObj3.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj3);
    }
    if(false == roClassInfo.m_strObj4.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj4);
    }
    if(false == roClassInfo.m_strObj5.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj5);
    }
    if(false == roClassInfo.m_strObj6.IsEmpty())
    {
        fprintf(fp, "\t\tpublic Hashtable m_h%s = new Hashtable();\n", roClassInfo.m_strObj6);
    }

    if(KEY_MODE_SET == roClassInfo.m_nKeyMode )
    {
        fprintf(fp, "\t\tpublic static INT32 m_%sLastKey = 0;\n", roClassInfo.m_strKeyPrefix);
    }
    if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "\t\tpublic static UINT64 m_%sLastKey = 0;\n", roClassInfo.m_strKeyPrefix);
        fprintf(fp, "\t\tpublic static %s m_%sLastKey1 = 0;\n", roClassInfo.m_strKeyType,roClassInfo.m_strKeyPrefix);
        fprintf(fp, "\t\tpublic static %s m_%sLastKey2 = 0;\n", roClassInfo.m_strKey2Type,roClassInfo.m_strKeyPrefix);
    }

    fprintf(fp,
        "\t\tpublic bool LoadFromFile(string path = \"\", AssetBundle bundle = null)\n"
        "\t\t{\n"
        //"    \t\tif((object)pszFile != null)\n"
        //"    \t\t{\n"
        //"        \t\tm_strFile = pszFile;\n"
        //"    \t\t}\n\n"
        "    \t\tm_h%s.Clear();\n"
        "    \t\tList<string> oVecLines = new List<string>();\n"
		"	 \t\tstring filename = m_strFile.Substring(9);\n"
		"	 \t\tbyte[] textBytes = null;\n"
		"	 \t\tif(bundle != null)\n"
		"	 \t\t{\n"
		"	 \t\t\tTextAsset oTextAsset = null;\n"
		"	 \t\t\toTextAsset = bundle.Load(filename + \".txt\") as TextAsset;\n"
		"	 \t\t\ttextBytes = oTextAsset.bytes;\n"
		"	 \t\t}\n"
		"	 \t\telse if(path != \"\")\n"
		"	 \t\t{\n"
		"	 \t\t\tWWW tableLoder = new WWW(path + filename + \".txt\");\n"
		"	 \t\t\twhile (!tableLoder.isDone) { };\n"
		"	 \t\t\ttextBytes = tableLoder.bytes;\n"
		"	 \t\t}\n"
		"	 \t\telse\n"
		"	 \t\t{\n"
		"	 \t\t\tTextAsset oTextAsset = Resources.Load(m_strFile) as TextAsset;\n"
		"	 \t\t\ttextBytes = oTextAsset.bytes;\n"
		"	 \t\t}\n"
        "    \t\tStream s = new MemoryStream(textBytes);\n"
        "    \t\tStreamReader sr = new StreamReader(s);\n"
        "    \t\tstring strLine;\n"
        "    \t\tstrLine = sr.ReadLine();\n"
        "    \t\toVecLines.Add(strLine);\n"
        "    \t\twhile (strLine != null)\n"
        "    \t\t{\n"
        "        \t\tstrLine = sr.ReadLine();\n"
        "        \t\toVecLines.Add(strLine);\n"
        "    \t\t}"
        "\n"
        "    \t\tsr.Close();\n"
//        "    \t\tList<string> oHeaders =new List<string>();\n"
        "    \t\tstring[] sHeaders = oVecLines[1].Split(new char[]{'\\t'}); \n"
        "    \t\t"
        "\n"
        "    \t\tif(sHeaders.Length != m_nColCount)\n"
        "    \t\t{\n"
        "        \t\treturn false;\n"
        "    \t\t}\n"
        "\n"
        "    \t\tINT32[] anIndex = new INT32[%d];\n"
        "    \t\tUINT32 i;\n"
        "    \t\tfor(i = 0; i < %d; i++)\n"
        "    \t\t{\n"
        "        \t\tanIndex[i] = -1;\n"
        "    \t\t}\n"
        "\n"
        "    \t\tfor(i = 0; i < %d; i++)\n"
        "    \t\t{\n",
        roClassInfo.m_strName,
        roClassInfo.m_oVecItems.size(),
        roClassInfo.m_oVecItems.size(),
        roClassInfo.m_oVecItems.size());

    
    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(0 == i)
        {
            fprintf(fp,
                "        \t\tif(sHeaders[i] == \"%s\")\n"
                "        \t\t{\n"
                "            \t\tanIndex[%d] = (INT32)i;\n"
                "        \t\t}\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
        else
        {
            fprintf(fp,
                "        \t\telse if(sHeaders[i] == \"%s\")\n"
                "        \t\t{\n"
                "            \t\tanIndex[%d] = (INT32)i;\n"
                "        \t\t}\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
    }

    fprintf(fp,
        "        \t\telse\n"
        "        \t\t{\n" 
		"            \t\tDebugLog.LogError(string.Format(\"Load File {0} failed, header {1} is invalid\", m_strFile, sHeaders[i]));\n"
        "            \t\treturn false;\n"
        "        \t\t}\n"
        "    \t\t}\n\n");
        
    fprintf(fp, 
        "    \t\tfor(i = 0; i < %d; i++)\n"
        "    \t\t{\n"
        "        \t\tif(-1 == anIndex[i])\n"
        "        \t\t{\n"
		"            \t\tDebugLog.LogError(string.Format(\"Load File {0} failed, not find all headers\", m_strFile));\n"
        "            \t\treturn false;\n"
        "        \t\t}\n"
        "    \t\t}\n\n",
        roClassInfo.m_oVecItems.size());

    fprintf(fp,
        "    \t\tfor(i = 2; i < oVecLines.Count-1; i++)\n"
        "    \t\t{\n"
        "        \t\tstring[] sHeader = oVecLines[(INT32)i].Split(new char[]{'\\t'}); \n"

        "\n"
        "        \t\t%s oItem = new %s();\n",strCStruct,strCStruct);
    fprintf(fp,
        "        \t\tif (sHeader[anIndex[0]].Length>0)\n"
        "        \t\t{\n"
        "            \t\toItem.m_%s = (INT32)int.Parse(sHeader[anIndex[0]]);\n"
        "        \t\t}\n"
        "        \t\telse\n"
        "            \t\toItem.m_%s = 0;\n\n",
        roClassInfo.m_oVecItems[0].m_strMemberName,roClassInfo.m_oVecItems[0].m_strMemberName); 
        size_t Limit = 1;  
    if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode){
    fprintf(fp,
        "        \t\tif (sHeader[anIndex[1]].Length>0)\n"
        "        \t\t{\n"
        "            \t\toItem.m_%s = (INT32)int.Parse(sHeader[anIndex[1]]);\n"
        "        \t\t}\n"
        "        \t\telse\n"
        "            \t\toItem.m_%s = 0;\n\n",
        roClassInfo.m_oVecItems[1].m_strMemberName,roClassInfo.m_oVecItems[1].m_strMemberName);   
//     fprintf(fp,
//         "        \t\tif (sHeader[anIndex[2]].Length>0)\n"
//         "        \t\t{\n"
//         "            \t\toItem.m_%s = (INT32)int.Parse(sHeader[anIndex[2]]);\n"
//         "        \t\t}\n"
//         "        \t\telse\n"
//         "            \t\toItem.m_%s = 0;\n\n",
//         roClassInfo.m_oVecItems[2].m_strMemberName,roClassInfo.m_oVecItems[2].m_strMemberName);   
        Limit = 2; 
    }

    for(i =Limit; i < roClassInfo.m_oVecItems.size(); i++)
    {
            
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp, 
                "        \t\tif(sHeaders.Length > anIndex[%d])\n"
                "        \t\t{\n"
                "            \t\toItem.m_%s = sHeader[anIndex[%d]];\n"
                "        \t\t}\n",
                i,
                roClassInfo.m_oVecItems[i].m_strMemberName,i);
        }
        else if(DATATYPE_STRING == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp, 
                "        \t\tif(sHeaders.Length > anIndex[%d])\n"
                "        \t\t{\n"
                "            \t\toItem.m_%s = sHeader[anIndex[%d]];\n"
                "        \t\t}\n",
                i,
                roClassInfo.m_oVecItems[i].m_strMemberName,i);
        }
        else if(DATATYPE_DOUBLE == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp,
				"        \t\tif(sHeader[anIndex[%d]].Length > 0)\n"
                "        \t\t\toItem.m_%s = (%s)float.Parse(sHeader[anIndex[%d]]);\n"
				"        \t\telse\n"
				"        \t\t\toItem.m_%s = 0;\n",
                i, roClassInfo.m_oVecItems[i].m_strMemberName,roClassInfo.m_oVecItems[i].m_strMemberType, i, roClassInfo.m_oVecItems[i].m_strMemberName);
        }
        else
        {
			fprintf(fp,
				"        \t\tif(sHeader[anIndex[%d]].Length > 0)\n"
				"        \t\t\toItem.m_%s = (%s)int.Parse(sHeader[anIndex[%d]]);\n"
				"        \t\telse\n"
				"        \t\t\toItem.m_%s = 0;\n",
				i, roClassInfo.m_oVecItems[i].m_strMemberName,roClassInfo.m_oVecItems[i].m_strMemberType, i, roClassInfo.m_oVecItems[i].m_strMemberName);
        }
    }
    

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode )
    {
        fprintf(fp,
            "\n"
            "        \t\tif(false == _AddItem(oItem.m_%s, oItem.m_%s, oItem))\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp,
            "\n"
            "        \t\tif(false == _AddItem(oItem.m_%s, oItem))\n",
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,
        "        \t\t{\n"
        "            \t\treturn false;\n"
        "        \t\t}\n"
        "    \t\t}\n"
        "\t\t\n");
        
   fprintf(fp,
//	   "	\t\tResources.UnloadAsset(oTextAsset);\n"
	   "    \t\tDebugLog.Log(string.Format(\"Load File{0} ok, {1} lines, {2} cols\", m_strFile, oVecLines.Count, m_nColCount));\n"
       "    \t\treturn true;\n"
       "\t\t}\n\n"
       );
    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic %s Get(%s %s, %s %s)\n"
            "\t\t{\n"
            "    \t\tUINT64 qwKey = (UINT64)%s;\n"
            "    \t\tqwKey = (qwKey << 32) | (UINT64)%s;\n"
            "    \t\tforeach (DictionaryEntry ID in m_h%s)\n"
            "    \t\tfor (INT32 i = 0; i < m_h%s.Count; ++i)\n"
            "    \t\t{\n"
            "        \t\tif((UINT64)ID.Key == qwKey )\n"
            "            \t\treturn (%s)ID.Value;\n"
            "    \t\t}\n"
            "    \t\treturn null;\n"
            "\t\t}\n\n",
            strCStruct,roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
            roClassInfo.m_strName,strCStruct);
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic List<%s> Get(%s %s)\n"
            "\t\t{\n"
            "    \t\tif (m_h%s.ContainsKey(%s))\n"
            "        \t\treturn m_h%s[%s];\n"
            "    \t\treturn null;\n"
            "\t\t}\n\n",
            strCStruct, roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
            roClassInfo.m_strName,roClassInfo.m_strKeyName,
            roClassInfo.m_strName,roClassInfo.m_strKeyName);
    }
    else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic List<%s> Get(%s %s, %s %s)\n"
            "\t\t{\n"
            "    \t\tUINT64 qwKey = (UINT64)%s;\n"
            "    \t\tqwKey = (qwKey << 32) | (UINT64)%s;\n"

            "    \t\tif (m_h%s.ContainsKey(qwKey))\n"
            "        \t\treturn m_h%s[qwKey];\n"
            "    \t\treturn null;\n"
            "\t\t}\n\n",
            strCStruct,roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
            roClassInfo.m_strName,
            roClassInfo.m_strName);
    }
    else
    {
            fprintf(fp,
                "\t\tpublic %s Get(%s %s)\n"
                "\t\t{\n"
                "    \t\tif (m_h%s.ContainsKey(%s))\n"
                "        \t\treturn m_h%s[%s];\n"
                "    \t\treturn null;\n"
                "\t\t}\n\n",
                strCStruct,roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,
                roClassInfo.m_strName,roClassInfo.m_strKeyName,
                roClassInfo.m_strName,roClassInfo.m_strKeyName);
    }

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic bool _AddItem(%s %s, %s %s, %s poItem)\n"
            "\t\t{\n"
            "    \t\tUINT64 qwKey = (UINT64)%s;\n"
            "    \t\tqwKey = (qwKey << 32) | (UINT64)%s;\n"
            "    \t\tif((object)Get(%s, %s) != null)\n"
            "    \t\t{\n"
            "        \t\treturn false;\n"
            "    \t\t}\n"
            "\n"
            "    \t\tm_h%s.Add(qwKey, poItem);\n"
            "    \t\treturn true;"
            "}\n\n",
            roClassInfo.m_strKeyType, roClassInfo.m_strKeyName, roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,strCStruct,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name,roClassInfo.m_strName);
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic bool _AddItem(%s %s, %s poItem)\n"     //1
            "\t\t{\n"
            "    \t\tif(0 == %s)\n"     //2
            "    \t\t{\n"
            "        \t\t%s = m_%sLastKey;\n" //3    
            "        \t\tif(0 == %s)\n"     //4
            "        \t\t{\n"
			"            \t\tDebugLog.LogError(string.Format(\"Load File {0} failed, key of first row is null\", m_strFile));\n"
            "           \t\t return false;\n"
            "        \t\t}\n"
            "        \t\tpoItem.m_%s = %s;\n"      //4.1
            "        \t\tGet(%s).Add(poItem);\n"    //4.2
            "        \t\treturn true;\n"
            "    \t\t}\n\n"
            "    \t\tif(Get(%s) != null)\n" 
            "    \t\t{\n"
            "        \t\treturn false;\n"
            "    \t\t}\n"
            "\n"
            "    \t\tList<%s> oVecItem = new List<%s>();\n"
            "    \t\toVecItem.Add(poItem);\n"
            "    \t\tm_h%s.Add(%s,oVecItem);\n"  //7
            "    \t\tm_%sLastKey = %s;\n"   //8
            "    \t\treturn true;\n"
            "\t\t}\n\n",
            roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,strCStruct,     //1
            roClassInfo.m_strKeyName,   //2
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyPrefix,   //3
            roClassInfo.m_strKeyName,   //4
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName, //4.1
            roClassInfo.m_strKeyName, //4.2
            roClassInfo.m_strKeyName,   //5
            strCStruct,strCStruct,
            roClassInfo.m_strName,roClassInfo.m_strKeyName,    //7
            roClassInfo.m_strKeyPrefix, roClassInfo.m_strKeyName);
    }
    else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "\t\tpublic bool _AddItem(%s %s, %s %s, %s poItem)\n"    //1 
            "\t\t{\n"
            "    \t\tUINT64 qwKey = (UINT64)%s;\n"//2
            "    \t\tqwKey = (qwKey << 32) | (UINT64)%s;\n"//3
            "    \t\tif(0 == qwKey)\n"     
            "    \t\t{\n"
            "        \t\tqwKey = m_%sLastKey;\n"    //4
            "        \t\tif(0 == qwKey)\n"     
            "        \t\t{\n"
			"            \t\tDebugLog.LogError(string.Format(\"Load File {0} failed, key of first row is null\", m_strFile));\n"
            "           \t\t return false;\n"
            "        \t\t}\n"
            "        \t\tpoItem.m_%s = %s;\n"      //5
            "        \t\tpoItem.m_%s = %s;\n"      //5.1
            "        \t\tGet(m_%sLastKey1,m_%sLastKey2).Add(poItem);\n"    //6
            "        \t\treturn true;\n"
            "    \t\t}\n\n"
            "    \t\tif(Get(%s ,%s) != null)\n" //7
            "    \t\t{\n"
            "        \t\treturn false;\n"
            "    \t\t}\n"
            "\n"
            "    \t\tList<%s> oVecItem = new List<%s>();\n" //7.1
            "    \t\toVecItem.Add(poItem);\n"
            "    \t\tm_h%s.Add(qwKey,oVecItem);\n"  //7.2
            "    \t\tm_%sLastKey = qwKey;\n"   //8
            "    \t\tm_%sLastKey1 = %s;\n"   //8.1
            "    \t\tm_%sLastKey2 = %s;\n"   //8.2
            "    \t\treturn true;\n"
            "\t\t}\n\n",
            roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,roClassInfo.m_strKey2Type, roClassInfo.m_strKey2Name,strCStruct,     //1
            roClassInfo.m_strKeyName,//2
            roClassInfo.m_strKey2Name,//3
            roClassInfo.m_strKeyPrefix,   //4
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName,//5
            roClassInfo.m_strKey2Name, roClassInfo.m_strKey2Name,//5.1
            roClassInfo.m_strKeyPrefix,roClassInfo.m_strKeyPrefix,//6
            roClassInfo.m_strKeyName,  roClassInfo.m_strKey2Name, //7
            strCStruct,strCStruct,//7.1
            roClassInfo.m_strName,    //7.2
            roClassInfo.m_strKeyPrefix,
            roClassInfo.m_strKeyPrefix,roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyPrefix,roClassInfo.m_strKey2Name
            );
    }
    else
    {
        fprintf(fp,
            "\t\tpublic bool _AddItem(%s %s, %s poItem)\n"
            "\t\t{\n"
            "    \t\tif((object)Get(%s) != null)\n"
            "    \t\t{\n"
			"        \t\tDebugLog.LogError(string.Format(\"Load File {0} failed, %s {1} repeat\", m_strFile, (UINT32)%s));\n"
            "        \t\treturn false;\n"
            "    \t\t}\n"
            "\n"
            "    \t\tm_h%s.Add(%s, poItem);\n"
            "    \t\treturn true;\n"
            "\t\t}\n"
            "\n",
             roClassInfo.m_strKeyType, roClassInfo.m_strKeyName,strCStruct,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName,
            roClassInfo.m_strName,roClassInfo.m_strKeyName);
    }
    fprintf(fp,"\t}\n}\n");

    fclose(fp);
    return true;
}

bool CCompiler::_WriteTableMgrFileCS()
{
    CString strCSFile;
    strCSFile.Format("%s\\tablemgr.CS", m_strOutputPath);
    size_t i;
        
    FILE* fp = fopen(strCSFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", strCSFile);
        return false;
    }
    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");
    fprintf(fp,"using System;\nusing TSSGClient;\nusing Tableload;using UnityEngine;\n\n\n ");
    
    fprintf(fp,"public class tablemgr\n"
    "{\n");
    fprintf(fp,"\tprivate static tablemgr m_instance = null;\n");
    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp,"\tpublic C%s g_o%s = new C%s();\n",m_oVecTableClassInfo[i].m_strName,m_oVecTableClassInfo[i].m_strName,m_oVecTableClassInfo[i].m_strName);
        }
    }
    fprintf(fp,
    "\tpublic tablemgr ()\n"    
    "\t{\n\t}\n\n");
    
    fprintf(fp,
        "\tpublic static tablemgr Instance {\n"    
        "\t\tget {\n"
        "\t\t\tif (m_instance == null)\n"
        "\t\t\t{\n"
        "\t\t\t\tm_instance = new tablemgr();\n"
        "\t\t\t}\n"
        "\n\t\t\treturn m_instance;\n"
        "\t\t}\n"
        "\t}\n");

	fprintf(fp,
		"\tpublic static void empty()\n"
		"\t{\n");
	fprintf(fp,
		"\t\t m_instance = null; \n"
		"\t}\n");

    fprintf(fp,
    "\tpublic bool LoadAllTables(string path = \"\", AssetBundle bundle = null)\n"
    "\t{\n");
    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp, 
                "    \tif(false == g_o%s.LoadFromFile(path, bundle))\n"
				"    \t{\n"
				"    \t\tDebugLog.LogError(\"Read %s Tabel Error!\");"
                "        \treturn false;\n\n"
				"    \t}\n",
                m_oVecTableClassInfo[i].m_strName, m_oVecTableClassInfo[i].m_strName);
        }
    }
    fprintf(fp, 
        "    \treturn true;\n"
        "\t}\n"
        "}\n");
	fclose(fp);

    return true;
}

//////////////////////////////////////////py file////////////////////////////////////////////////////////////////////////

bool CCompiler::CompilePy(const char* pszFile)
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
		if(false == _WriteTblPyFile(m_oVecTableClassInfo[i]))
		{
			return false;
		}
	}

	m_strErrMsg.Format("Compile %s OK", pszFile);
	return true;
}

bool CCompiler::_WriteTblPyFile( CTableClassInfo& roClassInfo )
{
	CString strPyFile;
    CString strPyStruct;
	roClassInfo.m_strTblFile = "../settings/" + roClassInfo.m_strTblFile;
    size_t i;
    strPyStruct.Format("Struct%s",roClassInfo.m_strClassName);
    strPyFile.Format("%s\\%s", m_strOutputPath, roClassInfo.m_strPyFile);
    FILE* fp = fopen(strPyFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", roClassInfo.m_strCppFile);
        return false;
    }

    fprintf(fp,
		"import codecs\n\n"
		"class %s:\n"
		"    def __init__(self):\n", strPyStruct);

    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {   
        fprintf(fp, "        self.m_%s = None\n", roClassInfo.m_oVecItems[i].m_strMemberName);
    }

    fprintf(fp,
    "\n\n");
    fprintf(fp,
		"class %s:\n"
        "    m_File = \"%s.txt\"\n"
        "    m_ColCount = %d\n",
        roClassInfo.m_strClassName,
        roClassInfo.m_strTblFile,
        roClassInfo.m_oVecItems.size());

    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strName);
    }
    if(false == roClassInfo.m_strObj1.IsEmpty())
    {
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj1);
    }
	if(false == roClassInfo.m_strObj2.IsEmpty())
	{
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj2);
	}
	if(false == roClassInfo.m_strObj3.IsEmpty())
	{
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj3);
	}
	if(false == roClassInfo.m_strObj4.IsEmpty())
	{
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj4);
	}
	if(false == roClassInfo.m_strObj5.IsEmpty())
	{
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj5);
	}
	if(false == roClassInfo.m_strObj6.IsEmpty())
	{
		fprintf(fp, "    m_dic%s = {}\n", roClassInfo.m_strObj6);
	}

    if(KEY_MODE_SET == roClassInfo.m_nKeyMode )
    {
        fprintf(fp, "    m_%sLastKey = 0\n", roClassInfo.m_strKeyPrefix);
    }
    if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp, "    m_%sLastKey = 0\n", roClassInfo.m_strKeyPrefix);
        fprintf(fp, "    m_%sLastKey1 = 0\n", roClassInfo.m_strKeyType,roClassInfo.m_strKeyPrefix);
        fprintf(fp, "    m_%sLastKey2 = 0\n", roClassInfo.m_strKey2Type,roClassInfo.m_strKeyPrefix);
    }

    fprintf(fp,
		"    def LoadFromFile(self, pszFile = None):\n"
		"        if pszFile is not None:"
		"\n"
        "            %s.m_File = pszFile\n"
		""
		"        %s.m_dic%s = {};\n"
		"        oVecLines = []\n"
		"        filename = %s.m_File\n"
        "        file = codecs.open(%s.m_File, 'r', \"UTF-16\")\n"
		"        str = file.read().encode(\"UTF-8\")\n"
        "        oVecLines = str.split('\\n')\n"
        "        file.close()\n"
        "        sHeaders = oVecLines[1].split('\\t')\n\n"
		"        if len(sHeaders) != %s.m_ColCount:\n"
        "            return False\n"
        "\n"
		"        anIndex = []\n"
		"        for i in range(0, %d):\n"
		"            anIndex.append(-1)\n"
        "\n"
		"        for i in range(0,%d):\n",
		roClassInfo.m_strClassName,
		roClassInfo.m_strClassName,
        roClassInfo.m_strName,
		roClassInfo.m_strClassName,
		roClassInfo.m_strClassName,
		roClassInfo.m_strClassName,
        roClassInfo.m_oVecItems.size(),
        roClassInfo.m_oVecItems.size());

    for(i = 0; i < roClassInfo.m_oVecItems.size(); i++)
    {
        if(0 == i)
        {
            fprintf(fp,
				"            if sHeaders[i].strip() == \"%s\":\n"
                "                anIndex[%d] = i\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
        else
        {
            fprintf(fp,
				"            elif sHeaders[i].strip() == \"%s\":\n"
                "                anIndex[%d] = i\n",
                roClassInfo.m_oVecItems[i].m_strRawName,
                i);
        }
    }

    fprintf(fp,
		"            else:\n"
		"                print(\"Load File \" + \"%s\" + \" Header \" + sHeaders[i])\n"
        "                return False\n\n", roClassInfo.m_strTblFile);
        
    fprintf(fp, 
		"        for i in range(0,%d):\n"
		"            if -1 == anIndex[i]:\n"
		"                print(\"Load File \" + \"%s\")\n"
        "                return False\n",
        roClassInfo.m_oVecItems.size(), roClassInfo.m_strTblFile);

    fprintf(fp,
		"        for i in range(2, len(oVecLines) - 1):\n"
        "            sHeader = oVecLines[i].split('\\t')\n"
        "            oItem = %s()\n",strPyStruct);
    fprintf(fp,
		"            if len(sHeader[anIndex[0]].strip()) > 0:\n"
		"                oItem.m_%s = int(sHeader[anIndex[0]].strip())\n"
		"            else:\n"
        "                oItem.m_%s = 0\n\n",
        roClassInfo.m_oVecItems[0].m_strMemberName,roClassInfo.m_oVecItems[0].m_strMemberName); 
        size_t Limit = 1;  
    if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode){
    fprintf(fp,
		"            if len(sHeader[anIndex[1]].strip()) > 0: \n"
		"                oItem.m_%s = int(sHeader[anIndex[1]].strip())\n"
		"            else:\n"
        "                oItem.m_%s = 0\n\n",
        roClassInfo.m_oVecItems[1].m_strMemberName,roClassInfo.m_oVecItems[1].m_strMemberName);   
//     fprintf(fp,
// 		"            if len(sHeader[anIndex[2]].strip())>0):\n"
// 		"                oItem.m_%s = int(sHeader[anIndex[2]].strip())\n"
// 		"            else:\n"
//         "                oItem.m_%s = 0\n\n",
//         roClassInfo.m_oVecItems[2].m_strMemberName,roClassInfo.m_oVecItems[2].m_strMemberName);   
        Limit = 2; 
    }

    for(i =Limit; i < roClassInfo.m_oVecItems.size(); i++)
    {
            
        if(DATATYPE_COMMENT == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp, 
                "            oItem.m_%s = sHeader[anIndex[%d]]\n",
                roClassInfo.m_oVecItems[i].m_strMemberName,i);
        }
        else if(DATATYPE_STRING == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp, 
                "            oItem.m_%s = sHeader[anIndex[%d]]\n",
                roClassInfo.m_oVecItems[i].m_strMemberName,i);
        }
        else if(DATATYPE_DOUBLE == roClassInfo.m_oVecItems[i].m_nDataType)
        {
            fprintf(fp,
				"            if sHeader[anIndex[%d]]:\n"
                "                oItem.m_%s = float(sHeader[anIndex[%d]].strip())\n"
				"            else:\n"
				"                oItem.m_%s = 0\n",
                i, roClassInfo.m_oVecItems[i].m_strMemberName, i, roClassInfo.m_oVecItems[i].m_strMemberName);
        }
        else
        {
			fprintf(fp,
				"            if sHeader[anIndex[%d]]:\n"
				"                oItem.m_%s = int(sHeader[anIndex[%d]].strip())\n"
				"            else:\n"
				"                oItem.m_%s = 0\n",
				i, roClassInfo.m_oVecItems[i].m_strMemberName, i, roClassInfo.m_oVecItems[i].m_strMemberName);
        }
    }
    
    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode || KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode )
    {
        fprintf(fp,
            "\n"
            "            if False == self.AddItem(oItem.m_%s, oItem.m_%s, oItem):\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name);
    }
    else
    {
        fprintf(fp,
            "\n"
            "            if False == self.AddItem(oItem.m_%s,oItem):\n",
            roClassInfo.m_strKeyName);
    }

    fprintf(fp,"                return False\n");
        
   fprintf(fp,
       "        return True\n"
       "\n\n"
       );
    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
			"    def Get(self, %s, %s):\n"
			"        qwKey = int(%s)\n"
            "        qwKey = (qwKey << 32) | %s\n"
			"        for k,v in %s.m_dic%s.iteritems():\n"
			"            if k == qwKey:\n"
            "                return v\n"
            "            return None\n"
            "\n\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
			roClassInfo.m_strClassName,
            roClassInfo.m_strName);
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
			"    def Get(self, %s):\n"
			"        for k,v in %s.m_dic%s.iteritems():\n"
			"            if k == %s:\n"
            "                return v\n"
            "        return None\n"
            "\n\n",
            roClassInfo.m_strKeyName,
			roClassInfo.m_strClassName,
            roClassInfo.m_strName,roClassInfo.m_strKeyName);
    }
    else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
			"    def Get(self, %s, %s):\n"
			"        qwKey = int(%s)\n"
            "        qwKey = (qwKey << 32) | %s\n"
			"        for k,v in %s.m_dic%s.iteritems():\n"
			"            if k == qwKey:\n"
            "                    return v\n"
            "        return None\n"
            "\n\n",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
			roClassInfo.m_strClassName,
            roClassInfo.m_strName);
    }
    else
    {
            fprintf(fp,
				"    def Get(self, %s):\n"
				"        for k,v in %s.m_dic%s.iteritems():\n"
				"            if k == %s: \n"
                "                return v\n"
                "        return None\n"
                "    \n\n",
                roClassInfo.m_strKeyName,
				roClassInfo.m_strClassName,
                roClassInfo.m_strName,roClassInfo.m_strKeyName);
    }

    if(KEY_MODE_MULITI == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
			"    def AddItem(self, %s, %s, poItem):\n"
			"        qwKey = int(%s)\n"
            "        qwKey = (qwKey << 32) | %s\n"
			"        if self.Get(%s, %s) is not None:\n"
            "            return False\n"
            "\n"
			"        %s.m_dic%s[qwKey] = poItem\n"
            "        return True",
            roClassInfo.m_strKeyName, roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKey2Name,
            roClassInfo.m_strKeyName, 
			roClassInfo.m_strClassName,
			roClassInfo.m_strKey2Name);
    }
    else if(KEY_MODE_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "    def AddItem(self, %s, poItem):\n"     //1
            "        if 0 == %s:\n"     //2
            "            %s = %s.m_%sLastKey\n" //3    
            "            if 0 == %s:\n"     //4
			"                print(\"Load File\" + \"%s\" + \" first row is null\")\n"
            "                return False\n"
            "            poItem.m_%s = %s\n"      //4.1
            "            self.Get(%s).append(poItem)\n"    //4.2
            "            return True\n"
			"        if self.Get(%s) != None:\n" 
            "            return False\n"
            "\n"
			"        oVecItem = []\n"
            "        oVecItem.append(poItem)\n"
            "        %s.m_dic%s[%s] = oVecItem\n"  //7
            "        %s.m_%sLastKey = %s\n"   //8
            "        return True\n"
            "\n\n",
            roClassInfo.m_strKeyName,     //1
            roClassInfo.m_strKeyName,   //2
            roClassInfo.m_strKeyName, roClassInfo.m_strClassName, roClassInfo.m_strKeyPrefix,   //3
            roClassInfo.m_strKeyName,   //4
			roClassInfo.m_strTblFile,
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName, //4.1
            roClassInfo.m_strKeyName, //4.2
            roClassInfo.m_strKeyName,   //5
			roClassInfo.m_strClassName,
            roClassInfo.m_strName,roClassInfo.m_strKeyName,    //7
            roClassInfo.m_strClassName, roClassInfo.m_strKeyPrefix, roClassInfo.m_strKeyName);
    }
    else if(KEY_MODE_MULITI_SET == roClassInfo.m_nKeyMode)
    {
        fprintf(fp,
            "    def AddItem(self, %s,%s,poItem):\n"    //1 
            "        qwKey = int(%s)\n"//2
            "        qwKey = (qwKey << 32) | %s\n"//3
			"        if 0 == qwKey:\n"     
            "            qwKey = %s.m_%sLastKey\n"    //4
			"            if 0 == qwKey:\n"
            "                return False\n"
            "            poItem.m_%s = %s\n"      //5
            "            poItem.m_%s = %s\n"      //5.1
            "            self.Get(%s.m_%sLastKey1,%s.m_%sLastKey2).append(poItem)\n"    //6
            "            return True\n"
            "        if self.Get(%s ,%s) is not None:\n" //7
            "            return False\n"
            "\n"
            "        oVecItem = []\n" //7.1
            "        oVecItem.append(poItem)\n"
            "        %s.m_dic%s[qwKey] = oVecItem\n"  //7.2
            "        %s.m_%sLastKey = qwKey\n"   //8
            "        %s.m_%sLastKey1 = %s\n"   //8.1
            "        %s.m_%sLastKey2 = %s\n"   //8.2
            "        return True\n"
            "\n\n",
            roClassInfo.m_strKeyName,roClassInfo.m_strKey2Name,     //1
            roClassInfo.m_strKeyName,//2
            roClassInfo.m_strKey2Name,//3
			roClassInfo.m_strClassName,
            roClassInfo.m_strKeyPrefix,   //4
            roClassInfo.m_strKeyName, roClassInfo.m_strKeyName,//5
            roClassInfo.m_strKey2Name, roClassInfo.m_strKey2Name,//5.1
            roClassInfo.m_strClassName, roClassInfo.m_strKeyPrefix, roClassInfo.m_strClassName, roClassInfo.m_strKeyPrefix,//6
            roClassInfo.m_strKeyName,  roClassInfo.m_strKey2Name, //7
			roClassInfo.m_strClassName,
            roClassInfo.m_strName,    //7.2
			roClassInfo.m_strClassName,
            roClassInfo.m_strKeyPrefix,
			roClassInfo.m_strClassName,
            roClassInfo.m_strKeyPrefix,roClassInfo.m_strKeyName,
			roClassInfo.m_strClassName,
            roClassInfo.m_strKeyPrefix,roClassInfo.m_strKey2Name
            );
    }
    else
    {
        fprintf(fp,
			"    def AddItem(self, %s, poItem):\n"
			"        if self.Get(%s) is not None:\n"
			"            print(\"Load File\"  + \"%s\" + \" failed, \" + %s + \"repeat\")\n"
            "            return False\n"
            "\n"
			"        %s.m_dic%s[%s] = poItem\n"
            "        return True\n"
            "\n",
            roClassInfo.m_strKeyName,
            roClassInfo.m_strKeyName,
			roClassInfo.m_strTblFile,
            roClassInfo.m_strKeyName,
            roClassInfo.m_strClassName,
			roClassInfo.m_strName,
			roClassInfo.m_strKeyName);
    }

	fprintf(fp,"\n");
	fprintf(fp,"g_%s = %s()\n", roClassInfo.m_strName, roClassInfo.m_strClassName);
    fprintf(fp,"g_%s.LoadFromFile()\n", roClassInfo.m_strName);

    fclose(fp);
	return true;
}

bool CCompiler::_WriteTableMgrFilePy()
{
	CString strCSFile;
    strCSFile.Format("%s\\tablemgr.py", m_strOutputPath);
    size_t i;
        
    FILE* fp = fopen(strCSFile, "w");
    if(NULL == fp)
    {
        m_strErrMsg.Format("open file %s for write failed", strCSFile);
        return false;
    }
    fprintf(fp, 
        "/********************************************************************\n"
        "**       This head file is generated by program,                   **\n"
        "**            Please do not change it directly.                    **\n"
        "********************************************************************/\n"
        "\n");
    
    fprintf(fp,"class TableMgr:\n"
    "{\n");
    
    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp,"    %s g_%s = %s();\n",m_oVecTableClassInfo[i].m_strName,m_oVecTableClassInfo[i].m_strName,m_oVecTableClassInfo[i].m_strName);
        }
    }
 
    fprintf(fp,
    "    def LoadAllTables()\n"
    "    {\n");
    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp, 
				"        if False == g_%s.LoadFromFile():\n"
                "            return False\n\n",
                m_oVecTableClassInfo[i].m_strName);
        }
    }
    fprintf(fp, 
        "        return True\n");
    return true;
}
