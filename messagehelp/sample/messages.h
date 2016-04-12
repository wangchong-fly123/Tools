#ifndef _MESSAGE_HERO_CARD_CMD_H
#define _MESSAGE_HERO_CARD_CMD_H
#include "CmdType.h"
#include "zType.h"
#include <sstream>
///////////////////////////////////////////////
//
//code[messages.h] 由 messagehelp 生成,请勿修改
//
///////////////////////////////////////////////


#pragma pack(1)
namespace Cmd
{
    struct stHeroCardCmd : public stNullUserCmd
    {
        stHeroCardCmd()
        {
            byCmd = HERO_CARD_USERCMD;
        }
    };


    struct t_Group_List
    {
        t_Group_List()
        {
            index = 0;
            occupation = 0;
            cardNum = 0;
            memset(name, 0, sizeof(name));
        }
        DWORD index;
        DWORD occupation;
        DWORD cardNum;
        char name[32];
        std::string debugString()
        {
            std::string str = ""
            std::stringstream stream;
            stream<<index;
            str += stream.str() + ",";
            stream.clear();
            stream<<occupation;
            str += stream.str() + ",";
            stream.clear();
            stream<<cardNum;
            str += stream.str() + ",";
            stream.clear();
            stream<<name;
            str += stream.str() + ",";
            stream.clear();
            return str;
        }
    };

    struct t_Tujian
    {
        t_Tujian()
        {
            id = 0;
            num = 0;
        }
        DWORD id;
        WORD num;
        std::string debugString()
        {
            std::string str = ""
            std::stringstream stream;
            stream<<id;
            str += stream.str() + ",";
            stream.clear();
            stream<<num;
            str += stream.str() + ",";
            stream.clear();
            return str;
        }
    };

    const BYTE NOTIFY_ALL_CARD_TUJIAN_INFO_CMD = 1;
    struct stNotifyAllCardTujianInfoCmd : public stHeroCardCmd
    {
        stNotifyAllCardTujianInfoCmd()
        {
            byParam = NOTIFY_ALL_CARD_TUJIAN_INFO_CMD;
            count = 0;
        }
        WORD count;
        t_Tujian info[0];
        DWORD getSize() { return sizeof(*this) + count*sizeof(t_Tujian); }
        std::string debugString()
        {
            std::string str = ""
            std::stringstream stream;
            stream<<count;
            str += stream.str() + ",";
            stream.clear();
            return str;
        }
    };

    const BYTE NOTIFY_ONE_CARD_TUJIAN_INFO_CMD = 2;
    struct stNotifyOneCardTujianInfoCmd : public stHeroCardCmd
    {
        stNotifyOneCardTujianInfoCmd()
        {
            byParam = NOTIFY_ONE_CARD_TUJIAN_INFO_CMD;
            id = 0;
            num = 0;
            memset(desc, 0, sizeof(desc));
            count = 0;
        }
        DWORD id;
        DWORD num;
        char desc[32];
        WORD count;
        WORD state[0];
        DWORD getSize() { return sizeof(*this) + count*sizeof(WORD); }
        std::string debugString()
        {
            std::string str = ""
            std::stringstream stream;
            stream<<id;
            str += stream.str() + ",";
            stream.clear();
            stream<<num;
            str += stream.str() + ",";
            stream.clear();
            stream<<desc;
            str += stream.str() + ",";
            stream.clear();
            stream<<count;
            str += stream.str() + ",";
            stream.clear();
            return str;
        }
    };

    const BYTE REQ_ALL_CARD_TUJIAN_DATA_USER_CMD = 4;
    struct stReqAllCardTujianDataUserCmd : public stHeroCardCmd
    {
        stReqAllCardTujianDataUserCmd()
        {
            byParam = REQ_ALL_CARD_TUJIAN_DATA_USER_CMD;
        }
        std::string debugString()
        {
            std::string str = ""
            return str;
        }
    };

    const BYTE RET_CARD_GROUP_LIST_INFO_USER_CMD = 6;
    struct stRetCardGroupListInfoUserCmd : public stHeroCardCmd
    {
        stRetCardGroupListInfoUserCmd()
        {
            byParam = RET_CARD_GROUP_LIST_INFO_USER_CMD;
            count = 0;
        }
        WORD count;
        t_Group_List info[0];
        DWORD getSize() { return sizeof(*this) + count*sizeof(t_Group_List); }
        std::string debugString()
        {
            std::string str = ""
            std::stringstream stream;
            stream<<count;
            str += stream.str() + ",";
            stream.clear();
            return str;
        }
    };

} //end of namespace Cmd
#pragma pack()
#endif
