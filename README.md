# Tools
some useful tool

##SkillParser
```
```
##createEffect
```
```
##tblcoder
```
Code Generator
1. xml---->parser code
2. add code to your project
3. compile your project
4. call "Load" when app start
   call "Get" when you do something
   call "Reload" when table file changed
```
##messagehelp
```
From xml to cpp code, so easy.
```
```
1. source xml
<?xml version="1.0" encoding="GB2312"?>
<messages file="message_type.xml" basename="HeroCardCmd" cmdId="162" namespace="Cmd" secondnamespace="">
    <message id="2" name="NotifyOneCardTujianInfoCmd">
        <col name="id" type="DWORD"/>
        <col name="num" type="DWORD"/>
        <col name="desc" type="string" size="32"/>
        <col name="state" type="WORD" repeated="1"/>
    </message>
</messages>
2. .h file
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
            std::string str = ""; 
            std::stringstream stream;
            stream<<(int)byCmd;
            str += "byCmd:" + stream.str() + ",";
            stream.clear();
            stream.str("");
            stream<<(int)byParam;
            str += "byParam:" + stream.str() + ",";
            stream.clear();
            stream.str("");
            stream<<id;
            str += "id:" + stream.str() + ",";
            stream.clear();
            stream.str("");
            stream<<num;
            str += "num:" + stream.str() + ",";
            stream.clear();
            stream.str("");
            stream<<desc;
            str += "desc:" + stream.str() + ",";
            stream.clear();
            stream.str("");
            stream<<count;
            str += "count:"+ stream.str() + ",";
            stream.clear();
            stream.str("");
            return str;
        }   
    };
3. sample
  stNotifyOneCardTujianInfoCmd cmd;
  std::string debug_str = cmd.debugString();
4. after run sample
  160413-14:08:10 ScenesServer[21] DEBUG: debugstring:byCmd:162,byParam:2,id:0,num:0,desc:,count:0,
```
