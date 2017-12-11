#pragma once

#include <string>
#include <vector>

using namespace std;

#define PROPTYPE_INT32  1
#define PROPTYPE_DOUBLE 2

struct SPropInfo
{
    string  m_strID;
    int     m_nType;
    string  m_strMin;
    string  m_strMax;
};

class CPropCompiler
{
public:
    CPropCompiler(void);
    ~CPropCompiler(void);

    bool Compile(const char* pszDir);
    bool Parse(const char* pszFile);
    
    bool CompileUnitInc(const char* pszFile);
    bool CompileUnitPropBagInc(const char* pszFile);
    bool CompilePropCode(const char* pszFile);

    void SetOutputPath(LPCTSTR pszPath);

    vector<SPropInfo>  m_oVecProps;
    CString         m_strErrMsg;
    
    CString         m_strOutputPath;
};
