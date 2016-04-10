#ifndef _MESSAGE_HERO_CARD_CMD_H
#define _MESSAGE_HERO_CARD_CMD_H
#include "zType.h"
#include <vector>
///////////////////////////////////////////////
//
//code[messages.h] 由 messagehelp 生成,请勿修改
//
///////////////////////////////////////////////


#pragma pack(1)
namespace Cmd
{
namespace Test
{
    struct stHeroCardCmd : public stNullUserCmd
    {
        stHeroCardCmd()
        {
            byCmd = HERO_CARD_CMD;
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
    };

    const BYTE REQ_ALL_CARD_TUJIAN_DATA_USER_CMD = 4;
    struct stReqAllCardTujianDataUserCmd : public stHeroCardCmd
    {
        stReqAllCardTujianDataUserCmd()
        {
            byParam = REQ_ALL_CARD_TUJIAN_DATA_USER_CMD;
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
    };

} //end of namespace Test
} //end of namespace Cmd
#pragma pack()
#endif
