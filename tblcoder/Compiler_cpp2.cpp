#include "StdAfx.h"
#include "Compiler.h"

bool CCompiler::CompileCpp2(const char* pszFile)
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
        if(false == _CompileClassInfo2(m_oParser.m_oVecTableInfo[i], oClassInfo))
        {
            return false;
        }

        m_oVecTableClassInfo.push_back(oClassInfo);
    }

    for(i = 0; i < m_oParser.m_oVecTableInfo.size(); i++)
    {
        if(false == _WriteTblHeaderFile2(m_oVecTableClassInfo[i]))
        {
            return false;
        }
        if(false == _WriteTblCppFile2(m_oVecTableClassInfo[i]))
        {
            return false;
        }
    }

    if(false == _WriteTableMgrFileCpp2())
    {
        return false;
    }

    m_strErrMsg.Format("Compile %s OK", pszFile);
    return true;
}

bool CCompiler::_GetTypeAndPrefix2(INT32 nType, CString& rstrType, CString& rstrPrefix)
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

bool CCompiler::_CompileClassInfo2(const CTableInfo& roTableInfo, CTableClassInfo& roClassInfo)
{
    roClassInfo.m_strName = roTableInfo.m_strName.c_str();
    roClassInfo.m_strTblFile = roTableInfo.m_strFile.c_str();
    roClassInfo.m_nKeyMode = roTableInfo.m_nKeyMode;
    roClassInfo.m_strHFile.Format("%s.h", roTableInfo.m_strName.c_str());
    roClassInfo.m_strHFile.MakeLower();
    roClassInfo.m_strCppFile.Format("%s.cpp", roTableInfo.m_strName.c_str());
    roClassInfo.m_strCppFile.MakeLower();
    roClassInfo.m_strIfDef.Format("%s_H", roTableInfo.m_strName.c_str());
    roClassInfo.m_strIfDef.MakeUpper();
    roClassInfo.m_strClassName.Format("C%s", roTableInfo.m_strName.c_str());
	roClassInfo.m_bServerOnly = roTableInfo.m_bServerOnly;
	roClassInfo.m_bClientSkipLoad = roTableInfo.m_bClientSkipLoad;

    for(size_t i = 0; i < roTableInfo.m_oCols.size(); i++)
    {
        CString strType;
        CString strPreFix;

        if(false == _GetTypeAndPrefix2(roTableInfo.m_oCols[i].m_nType, strType, strPreFix))
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

bool CCompiler::_WriteTblHeaderFile2(CTableClassInfo& roClassInfo)
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
        "    bool Load(const std::string &strConfigDir,\n"
		"              const std::string &strLanguage,\n"
		"              const std::string &strPlatformName);\n",
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
        "\n"
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
		"    std::string m_strFileTemplate;\n"
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

    fprintf(fp,
        "\n"
        "#endif\n");

    fclose(fp);
    return true;
}

bool CCompiler::_WriteTblCppFile2(CTableClassInfo& roClassInfo)
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
		"#include <brickred/string_util.h>\n"
        "#include \"common/log.h\"\n"
        "#include \"common/table_util.h\"\n"
        "\n",
        roClassInfo.m_strHFile);

	roClassInfo.m_strTblFile = roClassInfo.m_strTblFile;
    if(false == roClassInfo.m_strTblFile.IsEmpty())
    {
        fprintf(fp, "%s g_o%s;\n", roClassInfo.m_strClassName, roClassInfo.m_strName);
    }
    
    fprintf(fp,
        "\n"
        "%s::%s()\n"
        "{\n"
        "    m_strFileTemplate = \"%s.txt\";\n"
		"    m_strFile = m_strFileTemplate;\n"
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
        "bool %s::Load(const std::string &strConfigDir,\n"
		"    const std::string &strLanguage, const std::string &strPlatformName)\n"
        "{\n"
		"    m_strFile = brickred::string_util::replace(\n"
		"        strConfigDir + m_strFileTemplate, \"$language$\", strLanguage);\n"
		"    m_strFile = brickred::string_util::replace(\n"
		"        m_strFile, \"$platform_name$\", strPlatformName);\n"
		"    std::vector<std::string> oVecLines;\n"
		"    if (false == common::util::getAllLinesFromTblFile(\n"
		"            m_strFile, &oVecLines)) {\n"
        "        LOG_ERROR(\"Load File %%s failed, can not get all lines\",\n"
		"                  m_strFile.c_str());\n"
        "        return false;\n"
        "    }\n"
        "\n"
		"    std::vector<std::string> oHeaders;\n"
		"    brickred::string_util::split(oVecLines[1].c_str(), \"\\t\", &oHeaders);\n"
        "\n"
		"    if (oHeaders.size() != (size_t)m_nColCount) {\n"
        "        LOG_ERROR(\"Load File %%s failed, header count %%zd is not correct, should be %%d\",\n"
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
        "            LOG_ERROR(\"Load File %%s failed, header %%s is invalid\",\n"
		"                      m_strFile.c_str(), oHeaders[i].c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n");
        
    fprintf(fp, 
		"    for (size_t i = 0; i < %d; ++i) {\n"
		"        if (-1 == anIndex[i]) {\n"
        "            LOG_ERROR(\"Load File %%s failed, not find all headers\",\n"
		"                      m_strFile.c_str());\n"
        "            return false;\n"
        "        }\n"
        "    }\n\n",
        roClassInfo.m_oVecItems.size());

    fprintf(fp,
		"    for (size_t i = 2; i < oVecLines.size(); ++i) {\n"
		"        std::vector<std::string> oSegs;\n"
        "        brickred::string_util::split(oVecLines[i].c_str(), \"\\t\", &oSegs);\n"
        "\n"
		"        if (oSegs.size() != (size_t)m_nColCount) {\n"
		"            LOG_ERROR(\"Load File %%s failed, line %%zd col count %%zd is not correct, should be %%d\",\n"
		"                      m_strFile.c_str(), i + 1, oSegs.size(), m_nColCount);\n"
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
				"            oItem.m_%s = brickred::string_util::trim(oSegs[anIndex[%d]], \"\\\"\");\n"
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
        "    LOG_INFO(\"Load File %%s ok, %%zd lines, %%d cols\",\n"
		"             m_strFile.c_str(), oVecLines.size(), m_nColCount);\n"
        "    return true;\n"
        "}\n"
        "\n");

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
            "        LOG_ERROR(\"Load File %%s failed, %s %%u %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s, (uint32_t)%s);\n"
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
			"bool %s::_AddItem(%s %s,%s %s, CItem *poItem)\n"    //1 
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
            "            LOG_ERROR(\"Load File %%s failed, key of first row is null\", m_strFile.c_str());\n"
            "            return false;\n"
            "        }\n"
            "        poItem->m_%s = %s;\n"      //4.1
            "        Get(%s)->push_back(*poItem);\n"    //4.2
            "        return true;\n"
            "    }\n\n"
            "    if (Get(%s) != NULL) {\n" //5
            "        LOG_ERROR(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s);\n"    //6
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
            "        LOG_ERROR(\"Load File %%s failed, %s %%u repeat\", m_strFile.c_str(), (uint32_t)%s);\n"
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

bool CCompiler::_WriteTableMgrFileCpp2()
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
		"bool LoadAllTables(const std::string &strConfigDir,\n"
		"                   const std::string &strLanguage,\n"
		"                   const std::string &strPlatformName);\n"
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
		"bool LoadAllTables(const std::string &strConfigDir,\n"
		"    const std::string &strLanguage, const std::string &strPlatformName)\n"
        "{\n");

    for(i = 0; i < m_oVecTableClassInfo.size(); i++)
    {
        if(false == m_oVecTableClassInfo[i].m_strTblFile.IsEmpty())
        {
            fprintf(fp, 
                "    if (false == g_o%s.Load(strConfigDir, strLanguage, strPlatformName))\n"
                "        return false;\n\n",
                m_oVecTableClassInfo[i].m_strName);
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