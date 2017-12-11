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