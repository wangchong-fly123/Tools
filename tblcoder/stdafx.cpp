
// stdafx.cpp : source file that includes just the standard includes
// tblcoder.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


const char*	GetRootPath()
{
    static char s_szRootPath[MAX_PATH];
    static bool s_bFirstTime = true;

    if(s_bFirstTime)
    {
        GetModuleFileName(NULL, s_szRootPath, MAX_PATH);
        char* p = strrchr(s_szRootPath, '\\');
        if(p != NULL)
        {
            *p = '\0';
        }

        s_bFirstTime = false;
    }

    return s_szRootPath;
}
