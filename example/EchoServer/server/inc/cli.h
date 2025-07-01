#if 1

#ifndef __COMMAND_LINE_INTERFACE_H__
#define __COMMAND_LINE_INTERFACE_H__

//#include "aiz_send_order/Retcode.h"
//#include "aiz_logger/AizonaLogAsyncPool.h"

#include <map>
#include <string>
#include <cstring>
#include <iomanip>
#include <iostream>

#include <readline/readline.h>
#include <readline/history.h>



/** @brief Command : CmdMap Return/Error code list
 *
 *
*/

enum class CLI_RETCODE
{
    NA                =  0,
    SUCCESS           =  1,
    ERROR_INIT        = -1,
    ERROR_QUIT        = -2,
    ERROR_NOTFOUND    = -3,
};

#ifndef LOG_INFO_P
#define LOG_INFO_P std::cout
#endif



/** @brief Command : Individual Command Line Command Item
 *
 * @param _label : Command label for "help"
 * @param _desc : brief Command description for "help"
 * @param _fptr : Function entry point of the command
*/

template <typename TCmd>
struct Command
{
    std::string _label;
    std::string _desc;
    CLI_RETCODE (*_fptr)(TCmd &lock, const char *cline) = nullptr;
};


/** @brief CmdMapBase : Base Map
 *
 * This base std::map matches the command with its functionality (CommandItem).
*/

template<typename TCmd>
using CmdMapBase = std::map<std::string, Command<TCmd>>;


/** @brief CmdHelp : Displays Help
 *
 * List all available commands.
*/

template<typename TCmd>
CLI_RETCODE CmdHelp (CmdMapBase<TCmd> &map, const std::string &name);



/** @brief CmdMap : Command Map with its executer.
 *
 *
*/

template<typename TCmd>
class CmdMap : public CmdMapBase<TCmd>
{
    public:
        CmdMap(const std::string &name);
        CLI_RETCODE ExecuteCommand(TCmd &obj, const char *cline);

        std::string Name() const { return _name; }

    private :
        std::string _name;
};




/** @brief CmdMapSet : This std::map holds multiple CmdMap.
 *
 *
*/
/*
template<typename TCmd>
class CmdMapSet
{
    private:
        std::map<std::string, CmdMap<TCmd>> _set;
};
*/





template<typename TCmd>
CmdMap<TCmd>::CmdMap(const std::string &name)
    : _name(name)
{
}


template<typename TCmd>
CLI_RETCODE CmdMap<TCmd>::ExecuteCommand(TCmd &obj, const char *cline)
{
    if (0 == std::strcmp(cline, "help"))
    {
        CmdHelp<TCmd>(*this, Name());
        return CLI_RETCODE::ERROR_NOTFOUND;
    }

    CLI_RETCODE res = CLI_RETCODE::NA;
    auto it    = CmdMapBase<TCmd>::find(cline);
    auto itEnd = CmdMapBase<TCmd>::end(); //Get the end iterator from the parameter.

    if (it != itEnd)
    {
        Command<TCmd> &cmd = it->second;

//      LOG_INFO_P << "----> " << cmd._label << std::endl;
        res = cmd._fptr(obj, cline);
//      LOG_INFO_P << "----< " << cmd._label << std::endl;

        switch (res)
        {
            case CLI_RETCODE::ERROR_INIT :
                LOG_INFO_P << "ERROR : Need Init" << std::endl;
                break;
            case CLI_RETCODE::ERROR_QUIT :
                LOG_INFO_P << "Quiting" << std::endl;
                break;
            default:
                break;
        }
    }
    else
    {
        res = CLI_RETCODE::ERROR_NOTFOUND;
    }

    return res;
}


template<typename TCmd>
CLI_RETCODE CmdHelp (CmdMapBase<TCmd> &map, const std::string &name)
{
    LOG_INFO_P << "---- " << name << " ----" << std::endl;
    for (const auto &it : map)
        LOG_INFO_P << std::setw(8)  << it.first << " : " << it.second._label << "  (" <<  it.second._desc << ")" << std::endl;
    return CLI_RETCODE::NA;
}


template <typename FN>
void cli(FN fn)
{
    bool quit = false;
    while (false == quit)
    {
        char *cline = readline("Idle:");
        if ((nullptr == cline) || ('\0' == *cline))
            continue;

        for (char *ch = cline; '\0' != *ch; ++ch)
            *ch = (char)std::tolower(*ch);


        CLI_RETCODE ret = fn(cline);
        //CLI_RETCODE ret = CLI_RETCODE:NA;
        //ret = mapCmdOSenderAll.ExecuteCommand(sorderServers, cline);
        //if (CLI_RETCODE::ERROR_NOTFOUND == ret)
        //    ret = mapCmdOSenderCur.ExecuteCommand(*sorderServers.Current(), cline);

        switch (ret)
        {
            case CLI_RETCODE::NA :  //DO NOT insert cline to history
                break;

            case CLI_RETCODE::ERROR_NOTFOUND :
                //LOG_INFO_P << "ERROR : Invalid Command" << std::endl;
                break;

            case CLI_RETCODE::ERROR_INIT :
                LOG_INFO_P << "ERROR : Need Init" << std::endl;
                break;

            case CLI_RETCODE::ERROR_QUIT :
                quit = true;
                break;

            default:
                add_history(cline);
                break;
        }

        free(cline);
    }

}


#endif // __COMMAND_LINE_INTERFACE_H__

#endif