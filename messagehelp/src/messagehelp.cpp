#include "zXMLParser.h"
#include "zString.h"
#include <argp.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <map>
#include <vector>
using namespace std;

const char* argp_program_version = "messagehelp - 1.0";
const char* argp_program_bug_address = "<test@163.com>";
static char doc[] =
"this is a Description for the messagehelp!\n"
"	\n"
"	\n"
"	\n"
"	\n"
"	\n"
"	\n"
"	\n"
"	\n"
"	\n";

static char args_doc[] = "FILE";
static struct argp_option options[] =
{
    {0,		0,	0,	0,	"options:\n"},
    {"output",	'o',	"FILE", 0,	"create the cpp file"},
    {"memfunc", 'm',	NULL,	0,	"member function "},
    {"indent",	'i',	"N",	0,	"default is 0 "},
    {"parse",	'p',	NULL,	0,	"just parse.for check xml"},
    {"dump",	'd',	NULL,	0,	"parse and dump "},
    {"nodename",'n',	"NODE",	0,	"rootnode name(default is node messagehelp_)  "},
    {"wholefile", 'f',	0,	0,	"xxxxxxxxxxxxxxxxxxxxxxx "},
    {0, 0,	0,	0,	
	"example:\n\n"
	    "	1. ./messagehelp cfg.xml\n"
	    "	2. ./messagehelp cfg.xml -o cfg.h\n"
	    "	3. ./messagehelp cfg.xml -m \n"
	    "	4. ./messagehelp cfg.xml -d\n"
	    "	5. ./messagehelp cfg.xml -n\n"
	    "	6. ./messagehelp cfg.xml -f\n"
    },

    {0,		0,	0,	0,	0}
};

struct Args
{
    Args()
    {
	xmlfile = NULL;
	outputfile = NULL;
	layout = 0;
	indent = 0;
	parse_flag = 0;
	dump_flag = 0;
	generator_root_nodename = NULL;
	wholefile_flag = 0;
    }
    char* xmlfile;
    char* outputfile;
    int layout;
    int indent;
    int parse_flag;
    int dump_flag;
    char* generator_root_nodename;
    int wholefile_flag;
};

static error_t parse_opt(int key, char* arg, struct argp_state* state)
{
    struct Args* args = (struct Args*)state->input;
    switch(key)
    {
	case 'o':
	    args ->outputfile = arg;
	    break;
	case 'm':
	    args->layout = 1;
	    break;
	case 'i':
	    args->indent = atol(arg);
	    break;
	case 'p':
	    args->parse_flag = 1;
	    break;
	case 'd':
	    args->dump_flag = 1;
	    break;
	case 'n':
	    args->generator_root_nodename = arg;
	    break;
	case 'f':
	    args->wholefile_flag = 1;
	    break;
	case ARGP_KEY_ARG:
	    args->xmlfile = arg;
	    break;
	case ARGP_KEY_END:
	    if(state->arg_num < 1)
		argp_usage(state);
	    break;
	default:
	    return 0;

    }
    return 0;
}

static struct argp messagehelp_argp = 
{
    options, parse_opt, args_doc, doc
};

struct colInfo
{
    char name[64];
    char type[64];
    int size;
    int repeated;
    colInfo()
    {
	bzero(this, sizeof(*this));
    }
};

struct messageInfo
{
    int id;
    char name[64];
    char bigName[64];
    std::vector<colInfo> colVec;
    messageInfo()
    {
	bzero(this, sizeof(*this));
    }
};

struct structInfo
{
    char name[64];
    std::vector<colInfo> colVec;
    structInfo()
    {
	bzero(this, sizeof(*this));
    }
};

/**
 * \brief
 * \param HelloWorld-->HELLO_WORLD
 * \return
*/
void nameToBigName(char *name, std::string &realBigName)
{
    std::vector<char> temp;
    for (int i = 0; name[i] != '\0'; ++i) {
	if (isupper(name[i])) {
	    vector<char>::iterator result = find(temp.begin(), temp.end(), name[i]);
	    if (result == temp.end()) {
		temp.push_back(name[i]);
		//std::cout<<name[i]<<std::endl;
	    }
	}
    }
    std::string bigName(name);
    //std::cout<<bigName<<std::endl;
    for (std::vector<char>::iterator iter = temp.begin();
	    iter != temp.end(); ++iter) {
	//std::cout<<*iter<<std::endl;
	std::stringstream stream;
	stream<<*iter;
	std::string old_str = stream.str();
	std::string new_str = old_str;
	Zebra::to_lower(new_str);
	Zebra::replace_all(bigName, old_str, "_" + new_str);
    }
    bigName = bigName.substr(1, bigName.size()-1);
    Zebra::to_upper(bigName);
    realBigName = bigName;
}

bool run(int argc, char** argv)
{
    Args args;
    argp_parse(&messagehelp_argp, argc, argv, 0, 0, &args);
    if(args.xmlfile)
    {
	zXMLParser xml;
	if(!xml.initFile(args.xmlfile))
	{
	    std::cout<<"file name :"<<args.xmlfile<<" maybe wrong!!"<<endl;
	    return false;
	}
	xmlNodePtr root = xml.getRootNode("messages");
	if(!root)
	{
	    std::cout<<"root node error!!!"<<endl;
	    return false;
	}
	char baseName[64];
	char nameSpace[64];
	char secondNameSpace[64];
	xml.getNodePropStr(root, "basename", baseName, sizeof(baseName));
	xml.getNodePropStr(root, "namespace", nameSpace, sizeof(nameSpace));
	xml.getNodePropStr(root, "secondnamespace", secondNameSpace, sizeof(secondNameSpace));
	std::string baseBigName = "";
	nameToBigName(baseName, baseBigName);
	std::string strSecondNameSpace(secondNameSpace);
	bool hasSecondNameSpace = false;
	if (strSecondNameSpace != "") {
	    hasSecondNameSpace = true;
	}

	std::map<std::string, structInfo> structMap;
	xmlNodePtr structNode = xml.getChildNode(root, "struct");
	while (structNode) {
	    structInfo message;
	    xml.getNodePropStr(structNode, "name", message.name, sizeof(message.name));
	    std::string structName(message.name);

	    std::vector<colInfo> colInfos;
	    xmlNodePtr colNode = xml.getChildNode(structNode, "col");
	    while (colNode) {
		colInfo col;

		xml.getNodePropStr(colNode, "name", col.name, sizeof(col.name));
		xml.getNodePropStr(colNode, "type", col.type, sizeof(col.type));
		xml.getNodePropNum(colNode, "size", &col.size, sizeof(col.size));
		xml.getNodePropNum(colNode, "repeated", &col.repeated, sizeof(col.repeated));
		if (col.repeated > 0) {
		    std::cout<<"repeated col can only use in message!!!"<<std::endl;
		    return false;
		}
		std::string colname(col.name);
		if (colname == "count" ||
		    colname == "friend") {
		    std::cout<<"col name must be not count,count is a keywords!!!"<<std::endl;
		    return false;
		}
		colInfos.push_back(col);
		colNode = xml.getNextNode(colNode, "col");
	    }

	    std::map<std::string, structInfo>::iterator iter = structMap.find(structName);
	    if (iter != structMap.end()) {
		std::cout<<"structName repeated!!!"<<structName<<endl;
		return false;
	    }
	    message.colVec = colInfos;
	    structMap[structName] = message;
	    structNode = xml.getNextNode(structNode, "struct");
	}

	std::map<DWORD, messageInfo> messageMap;
	xmlNodePtr messageNode = xml.getChildNode(root, "message");
	while (messageNode) {
	    messageInfo message;
	    xml.getNodePropNum(messageNode, "id", &message.id, sizeof(message.id));
	    xml.getNodePropStr(messageNode, "name", message.name, sizeof(message.name));

	    std::vector<colInfo> colInfos;
	    xmlNodePtr colNode = xml.getChildNode(messageNode, "col");
	    while (colNode) {
		colInfo col;

		xml.getNodePropStr(colNode, "name", col.name, sizeof(col.name));
		xml.getNodePropStr(colNode, "type", col.type, sizeof(col.type));
		xml.getNodePropNum(colNode, "size", &col.size, sizeof(col.size));
		xml.getNodePropNum(colNode, "repeated", &col.repeated, sizeof(col.repeated));
		std::string coltype(col.type);
		if (coltype == "string" && col.repeated > 0) {
		    std::cout<<"repeated col can not be string!!!"<<std::endl;
		    return false;
		}
		std::string colname(col.name);
		if (colname == "count" ||
		    colname == "friend") {
		    std::cout<<"col name must be not count,count is a keywords!!!"<<std::endl;
		    return false;
		}
		colInfos.push_back(col);
		colNode = xml.getNextNode(colNode, "col");
	    }

	    std::map<DWORD, messageInfo>::iterator iter = messageMap.find(message.id);
	    if (iter != messageMap.end()) {
		std::cout<<"id repeated!!!"<<message.id<<endl;
		return false;
	    }
	    message.colVec = colInfos;
	    messageMap[message.id] = message;
	    messageNode = xml.getNextNode(messageNode, "message");
	}

	std::string tmp = args.xmlfile;
	std::string outsql = args.xmlfile;
	Zebra::replace_all(tmp, ".xml","");
	Zebra::replace_all(outsql, ".xml",".sql");

	//h
	std::string outh = args.xmlfile;
	Zebra::replace_all(outh, ".xml",".h");

	std::ofstream ofh(outh.c_str());
	ofh<<"#ifndef _MESSAGE_"<<baseBigName<<"_H"<<std::endl;
	ofh<<"#define _MESSAGE_"<<baseBigName<<"_H"<<std::endl;
	ofh<<"#include \"CmdType.h\""<<std::endl;
	ofh<<"#include \"zType.h\""<<std::endl;
	ofh<<"#include <vector>"<<std::endl;
	ofh<< "///////////////////////////////////////////////"<<std::endl;
	ofh<< "//"<<std::endl;
	ofh<< "//"<<"code["<<outh<<"] 由 messagehelp 生成,请勿修改"<<std::endl;
	ofh<< "//"<<std::endl;
	ofh<< "///////////////////////////////////////////////"<<std::endl;
	ofh<<std::endl;
	ofh<<std::endl;

	ofh<<"#pragma pack(1)"<<std::endl;
	ofh<<"namespace "<<nameSpace<<std::endl;
	ofh<<"{"<<std::endl;
	if (hasSecondNameSpace) {
	    ofh<<"namespace "<<secondNameSpace<<std::endl;
	    ofh<<"{"<<std::endl;
	}
	ofh<<"    struct st"<<baseName<<" : public stNullUserCmd"<<std::endl;
	ofh<<"    {"<<std::endl;
	ofh<<"        st"<<baseName<<"()"<<std::endl;
	ofh<<"        {"<<std::endl;
	ofh<<"            byCmd = "<<baseBigName<<";"<<std::endl;
	ofh<<"        }"<<std::endl;
	ofh<<"    };"<<std::endl;
	ofh<<std::endl;
	ofh<<std::endl;

	// struct
	for (std::map<std::string, structInfo>::iterator iter = structMap.begin();
	     iter != structMap.end(); ++iter) {
	    std::string strcutName = iter->first;
	    std::vector<colInfo> &colInfos = iter->second.colVec;
	    ofh<<"    struct "<<iter->second.name<<std::endl;
	    ofh<<"    {"<<std::endl;
	    ofh<<"        "<<iter->second.name<<"()"<<std::endl;
	    ofh<<"        {"<<std::endl;
	    for (std::vector<colInfo>::iterator it = colInfos.begin();
		 it != colInfos.end(); ++it) {
		std::string str(it->type);
		if (str == "string") {
		    ofh<<"            memset("<<it->name<<", 0, "<<"sizeof("<<it->name<<")"<<");"<<std::endl;
		} else {
		    ofh<<"            "<<it->name<<" = 0;"<<std::endl;
		}
	    }
	    ofh<<"        }"<<std::endl;
	    for (std::vector<colInfo>::iterator it = colInfos.begin();
		    it != colInfos.end(); ++it) {
		std::string str(it->type);
		if (str == "string") {
		    ofh<<"        "<<"char"<<" "<<it->name<<"["<<it->size<<"]"<<";"<<std::endl;
		} else {
		    ofh<<"        "<<it->type<<" "<<it->name<<";"<<std::endl;
		}
	    }
	    ofh<<"    };"<<std::endl;
	    ofh<<std::endl;
	}

	// message
	for (std::map<DWORD, messageInfo>::iterator iter = messageMap.begin();
	     iter != messageMap.end(); ++iter) {
	    DWORD message_id = iter->first;
	    std::string bigName = "";
	    nameToBigName(iter->second.name, bigName);
	    std::vector<colInfo> &colInfos = iter->second.colVec;
	    ofh<<"    const BYTE "<<bigName<<" = "<<message_id<<";"<<std::endl;
	    ofh<<"    struct st"<<iter->second.name<<" : public st"<<baseName<<std::endl;
	    ofh<<"    {"<<std::endl;
	    ofh<<"        st"<<iter->second.name<<"()"<<std::endl;
	    ofh<<"        {"<<std::endl;
	    ofh<<"            byParam = "<<bigName<<";"<<std::endl;
	    for (std::vector<colInfo>::iterator it = colInfos.begin();
		 it != colInfos.end(); ++it) {
		if (it->repeated) {
		    ofh<<"            count = 0;"<<std::endl;
		    continue;
		}
		std::string str(it->type);
		if (str == "string") {
		    ofh<<"            memset("<<it->name<<", 0, "<<"sizeof("<<it->name<<")"<<");"<<std::endl;
		} else {
		    ofh<<"            "<<it->name<<" = 0;"<<std::endl;
		}
	    }
	    ofh<<"        }"<<std::endl;
	    for (std::vector<colInfo>::iterator it = colInfos.begin();
		 it != colInfos.end(); ++it) {
		std::string str(it->type);
		if (it->repeated) {
			ofh<<"        WORD count;"<<std::endl;
			ofh<<"        "<<it->type<<" "<<it->name<<"[0]"<<";"<<std::endl;
			ofh<<"        DWORD getSize() { return sizeof(*this) + count*sizeof("<<it->type<<"); }"<<std::endl;
		} else {
		    if (str == "string") {
			ofh<<"        "<<"char"<<" "<<it->name<<"["<<it->size<<"]"<<";"<<std::endl;
		    } else {
			ofh<<"        "<<it->type<<" "<<it->name<<";"<<std::endl;
		    }
		}
	    }
	    ofh<<"    };"<<std::endl;
	    ofh<<std::endl;
	}
	if (hasSecondNameSpace) {
	    ofh<<"} //end of namespace "<<secondNameSpace<<std::endl;
	}
	ofh<<"} //end of namespace "<<nameSpace<<std::endl;
	ofh<<"#pragma pack()"<<std::endl;
	ofh<<"#endif"<<std::endl;

    }
    return true;
}

int main(int argc, char* argv[])
{
    return run(argc, argv);
}
