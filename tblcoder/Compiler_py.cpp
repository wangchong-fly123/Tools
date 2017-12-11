#include "StdAfx.h"
#include "Compiler.h"

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