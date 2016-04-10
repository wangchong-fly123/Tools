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
    struct stHeroCardCmd : public stNullUserCmd
    {
        stHeroCardCmd()
        {
            byCmd = HERO_CARD_CMD;
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
        DWORD getSize() { sizeof(this) + count*sizeof(WORD)};
    };

    const BYTE REQ_ALL_CARD_TUJIAN_DATA_USER_CMD = 4;
    struct stReqAllCardTujianDataUserCmd : public stHeroCardCmd
    {
        stReqAllCardTujianDataUserCmd()
        {
            byParam = REQ_ALL_CARD_TUJIAN_DATA_USER_CMD;
        }
    };

} //end of namespace Cmd
#pragma pack()
#endif
