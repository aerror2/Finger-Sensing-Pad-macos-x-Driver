/**
 * @file fspcCli.cpp The source code of FSPC command line interface.
 *
 * Copyright (C) 2008 Gary Lee, Sentelic Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $Id: fspcCli.cpp 28876 2008-10-27 06:17:55Z avatar $
 */

#include <wx/wx.h>
#include <wx/wxchar.h>
#include <wx/tokenzr.h>
#include "fspcCli.h"
#include "fspcConfig.h"

/**
 * The static array to describe the command line usage of fspc.
 */
static const wxCmdLineEntryDesc g_fspcCmdLineDesc[] =
{
    { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("displays the usage of command line parameters") },
    { wxCMD_LINE_SWITCH, wxT("V"), wxT("version"), wxT("shows the version of program.") },
    { wxCMD_LINE_SWITCH, wxT("v"), wxT("verbose"), wxT("enable verbose mode") },
    { wxCMD_LINE_SWITCH, wxT("d"), wxT("debug"), wxT("enable debug mode") },
    { wxCMD_LINE_SWITCH, wxT("t"), wxT("text"), wxT("enter text mode") },
    { wxCMD_LINE_OPTION, wxT("l"), wxT("load"), wxT("load user preference.") },
    { wxCMD_LINE_OPTION, wxT("s"), wxT("save"), wxT("save user preference.") },
    { wxCMD_LINE_OPTION, wxT("m"), wxT("mode"), wxT("specify the icon mode (disable|hidden|static|dynamic)") },
    { wxCMD_LINE_OPTION, wxT("p"), wxT("prop"), wxT("get the specified property") },
    { wxCMD_LINE_PARAM, NULL, NULL, wxT("other parameters"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

/**
 * Default constructor of FspcCli class.
 */
FspcCli::FspcCli()
{
    DoInit();
}

/**
 * Constructor of FspcCli class.
 */
FspcCli::FspcCli(
    int argc,   ///< The number of arguments in argv.
    char **argv, ///< The array of argument.
    const char *logo ///< The logo of usage.
    )
{    
    DoInit();
    if (logo != NULL)
    {
        wxString logoText(logo, wxConvUTF8);        
        m_parser.SetLogo(logoText);
    }

    if (!DoParse(argc, argv))
    {
        return;
    }    
}

/**
 * Constructor of FspcCli class.
 */
FspcCli::FspcCli(
    wxString &cmdLine,   ///< The number of arguments in argv.
    wxString &logo ///< The logo of usage.
    )
{    
    DoInit();
    m_parser.SetLogo(logo);    
    if (!DoParse(cmdLine))
    {
        return;
    }   
}

/**
 * Constructor of FspcCli class.
 */
FspcCli::FspcCli(
    wxString &cmdLine   ///< The command line
    )
{    
    DoInit();
    if (!DoParse(cmdLine))
    {
        return;
    }    
}

/**
 * Initialize and reset all object and variable.
 */
bool FspcCli::DoInit()
{
    m_parseResult = false;    
    m_parser.SetDesc(g_fspcCmdLineDesc);   
    return true;
}

/**
 * Parse the command line.
 *
 * @retval true OK.
 * @retval false Failed.
 */
bool FspcCli::DoParse()
{
    int ret;

    ret = m_parser.Parse();
    if (ret != 0)
    {
        m_parseResult = false;
    }
    else
    {
        m_parseResult = true;
    }
    return m_parseResult;
}

/**
 * Parse the command line.
 *
 * @retval true OK.
 * @retval false Failed.
 */
bool FspcCli::DoParse(
    int argc,   ///< The number of arguments in argv.
    char **argv ///< The array of argument.
    )
{
    m_parser.SetCmdLine(argc, argv);
    return DoParse();
}

/**
 * Parse the command line.
 *
 * @retval true OK.
 * @retval false Failed.
 */
bool FspcCli::DoParse(
    wxString &cmdLine   ///< The command line
    )
{
    m_parser.SetCmdLine(cmdLine);
    return DoParse();
}

/**
 * Get the parsing result.
 *
 * @retval true OK.
 * @retval false Failed.
 */
bool FspcCli::GetResult()
{
    return m_parseResult;
}

/**
 * Show the usage to standard output.
 */
void FspcCli::Usage()
{
    m_parser.Usage();
}

/**
 * Get the value of -h/--help command line switch.
 *
 * @retval true switch on.
 * @retval false switch off.
 */
bool FspcCli::GetParamHelp()
{
    return m_parser.Found(wxT("h"));
}

/**
 * Get the value of -V/--version command line switch. 
 *
 * @retval true switch on.
 * @retval false switch off.
 */
bool FspcCli::GetParamVersion()
{
    return m_parser.Found(wxT("V"));
}

/**
 * Get the value of -v/--verbose command line switch.
 *
 * @retval true switch on.
 * @retval false switch off.
 */
bool FspcCli::GetParamVerbose()
{
    return m_parser.Found(wxT("v"));
}

/**
 * Get the value of -d/--debug command line switch.
 *
 * @retval true switch on.
 * @retval false switch off.
 */ 
bool FspcCli::GetParamDebug()
{
    return m_parser.Found(wxT("d"));
}

/**
 * Get the value of -t/--text command line switch.
 *
 * @retval true switch on.
 * @retval false switch off.
 */
bool FspcCli::GetParamTextMode()
{
    return m_parser.Found(wxT("t"));    
}

/**
 * Get the value of -l/--load command line option.
 *
 * @return The path of configuration file. If this option is not specified, 
 *         a empty string will be returned.
 */
bool FspcCli::GetParamLoadConf(wxString& value)
{
    return m_parser.Found(wxT("l"), &value);
}

/**
 * Get the value of -s/--save command line option.
 *
 * @return The path of configuration file. If this option is not specified, 
 *         a empty string will be returned.
 */
bool FspcCli::GetParamSaveConf(wxString& value)
{
    return m_parser.Found(wxT("s"), &value);
}

/**
 * Get the value of -m/--mode command line option.
 *
 * @return The mode of system tray icon.
 */
bool FspcCli::GetParamMode(FspcIconMode& mode)
{
    bool ret;
    wxString value;

    ret = m_parser.Found(wxT("m"), &value);
    if (!ret)
    {
        // User doesn't specify the mode of tray icon. Use disable as default.
        mode = FSPC_ICON_MODE_DISABLE; 
    }
    else if (value.CmpNoCase(FSPC_ICON_MODE_SYMBOL_DISABLE) == 0)
    {
        mode = FSPC_ICON_MODE_DISABLE;
    }
    else if (value.CmpNoCase(FSPC_ICON_MODE_SYMBOL_HIDDEN) == 0)
    {
        mode = FSPC_ICON_MODE_HIDDEN;
    }
    else if (value.CmpNoCase(FSPC_ICON_MODE_SYMBOL_STATIC) == 0)
    {
        mode = FSPC_ICON_MODE_STATIC;
    }
    else if (value.CmpNoCase(FSPC_ICON_MODE_SYMBOL_DYNAMIC) == 0)
    {
        mode = FSPC_ICON_MODE_DYNAMIC;
    }
    else
    {
        // User doesn't specify the mode of tray icon. Use disable as default.
        mode = FSPC_ICON_MODE_DISABLE; 
        ret = false;
    }

    return ret;
}

/**
 * Get the value of -p/--prop command line option.
 *
 * @param value The wxString object which used to store the result.
 * @retval true The set property option is specified.
 * @retval false The set property option is not specified.
 */
bool FspcCli::GetParamProp(wxString& value)
{
    return m_parser.Found(wxT("s"), &value);
}

/**
 * The CLI module handler.
 *
 * @retval 0 OK.
 * @retval !0 failed.
 */
int FspcCli::Execute(void)
{
    int ret = 0;
    wxString value;
    
    if (GetResult() == false)
    {
        return (-1);   
    }
    
    if (GetParamHelp())
    {
        Usage();
        return (0);
    }
    
    if (GetParamVersion())
    {
        wxPrintf(wxT("%s\n"), m_ver.c_str());
    }
    
    m_verbose = GetParamVerbose();
    m_debug = GetParamDebug();
    
    if (GetParamLoadConf(value))
    {
        VerboseOutput(wxString::Format(wxT("LoadConf: Use %s as configuration file.\n"), value.c_str()));
        FspcConfig fspcConfig(value);
        fspcConfig.LoadDefaultConfig();
        fspcConfig.LoadUserConfig();
        fspcConfig.SaveDeviceConfig();
    }
    
    if (GetParamSaveConf(value))
    {
        VerboseOutput(wxString::Format(wxT("SaveConf: Use %s as configuration file.\n"), value.c_str()));
        FspcConfig fspcConfig(value);
        fspcConfig.LoadDeviceConfig();
        fspcConfig.SaveUserConfig();
    }
    return ret;   
}

/**
 * Only output message when verbose mode is set.
 *
 * @param str The text to be printed.
 */
void FspcCli::VerboseOutput(wxString str)
{
    if (!m_verbose)
    {
        return;   
    }
    wxPrintf(str);
}

/**
 * Only output message when debug mode is set.
 *
 * @param str The text to be printed.
 */
void FspcCli::DebugOutput(wxString str)
{
    if (!m_debug)
    {
        return;   
    }
    wxPrintf(str);
}

/**
 * Check if arguments contain text mode switch.
 *
 * @param argc the counter of arguments.
 * @param argv the arguments.
 * 
 * @retval true Argument contains text mode switch.
 * @retval false No text mode switch in arguments.
 */
const bool FspcCli::CheckTextMode(int argc, char **argv)
{
    int i;
    wxString textSwShort(wxT("-t"));
    wxString textSwLong(wxT("--text"));
    wxString helpSwShort(wxT("-h"));
    wxString helpSwLong(wxT("--help"));
    
    wxString argToken;
    
    for (i = 0; i < argc; ++i)
    {
        argToken = wxString::FromAscii(argv[i]);
        if (textSwShort.Cmp(argToken) == 0 || 
            textSwLong.Cmp(argToken) == 0 ||
            helpSwShort.Cmp(argToken) == 0 ||
            helpSwLong.Cmp(argToken) == 0 )
        {
            return true;
        }
    }
    return false;
}

/**
 * Set the version informaion which will be displayed by "-V" switch.
 *
 * @param ver The vesion to be displayed.
 */
void FspcCli::SetVersion(wxString ver)
{
    m_ver = ver;   
}


/**
 * Convert key/value string to a hash table of wxString.
 *
 * @param str the string to be converted.
 * @param hash the hash table of string which used to store the result.
 * @param comma the symbol which used to seperate the key and value.
 * @param assign the symbol which used to seperate the key/value pairs.
 *
 * @retval true OK.
 * @retval false Failed.
 */
bool ConvertKeyValueToHash(wxString &str, KeyValueHash &hash, const wxChar *comma, const wxChar *assign)
{
    wxStringTokenizer tkz(str, comma);
    while ( tkz.HasMoreTokens() )
    {
        wxString key, value;
        wxString token = tkz.GetNextToken();
        wxStringTokenizer tkzKeyVal(token, assign);
        if (!tkzKeyVal.HasMoreTokens())
        {
            continue;
        }
        key = tkzKeyVal.GetNextToken();
        value = tkzKeyVal.GetNextToken();
        hash[key] = value;
    }
    return true;
}

/**
 * Convert key/value string to a hash table of wxString.
 *
 * @param hash the hash table of string which used to store the result.
 * @param str the string to be converted.
 * @param comma the symbol which used to seperate the key and value.
 * @param assign the symbol which used to seperate the key/value pairs.
 *
 * @retval true OK.
 * @retval false Failed. 
 */
bool ConvertHashToKeyValue(KeyValueHash &hash, wxString &str, const wxChar *comma, const wxChar *assign)
{
    wxString keyVal;
    int cnt;

    // iterate over all the elements in the hash.
    KeyValueHash::iterator it;
    for( it = hash.begin(), cnt = 0; it != hash.end(); ++it, ++cnt )
    {
        if (it->second.IsEmpty())
        {
            keyVal = it->first;
        }
        else
        {   
            keyVal = it->first + assign + it->second;
        }

        if (cnt > 0)
        {
            str += comma + keyVal;
        }
        else
        {
            str += keyVal;
        }
    }
    return true;
}



