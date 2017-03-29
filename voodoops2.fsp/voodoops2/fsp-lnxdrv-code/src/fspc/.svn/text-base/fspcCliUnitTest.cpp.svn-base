/**
 * @file fspcCliUnitTest.cpp The unittest program for fspc CLI.
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
 * $Id: fspcCliUnitTest.cpp 28876 2008-10-27 06:17:55Z avatar $
 */

#include <wx/wx.h>
#include <wx/debug.h>
#include <wx/wxchar.h>
#include <wx/apptrait.h>
#include "fspcCli.h"

/**
 * Main function of fspc CLI unit test..
 */
int main(int argc, char **argv)
{
    wxString value;
    FspcIconMode mode;

#ifdef MANUAL_TEST    
    wxInitializer init;
    FspcCli cli(argc, argv);

    if (cli.GetResult() == false)
    {
        // When result is false, the usage will be shown automatically.
        // We just leave the program.
        cli.Usage();
        return (-1);
    }
    else
    {
        // Get the parameters.
        wxPrintf(wxT("-h: %d\n"), (int)cli.GetParamHelp());
        wxPrintf(wxT("-V: %d\n"), (int)cli.GetParamVersion());
        wxPrintf(wxT("-v: %d\n"), (int)cli.GetParamVerbose());
        wxPrintf(wxT("-d: %d\n"), (int)cli.GetParamDebug());
        wxPrintf(wxT("-t: %d\n"), (int)cli.GetParamTextMode());
        if (cli.GetParamConfig(value))
        {
            wxPrintf(wxT("-c: %s\n"), value.c_str());        
        }
        else
        {
            wxPrintf(wxT("-c: <none>\n"));        
        }

        if (cli.GetParamMode(mode))
        {
            wxPrintf(wxT("-m: %d\n"), (int) mode);
        }
        else
        {
            wxPrintf(wxT("-m: <none>\n"));        
        }

        if (cli.GetParamSetProp(value))
        {
            wxPrintf(wxT("-s: %s\n"), value.c_str());
        }
        else
        {
            wxPrintf(wxT("-s: <none>\n"));
        }

        if (cli.GetParamGetProp(value))
        {
            wxPrintf(wxT("-g: %s\n"), value.c_str());
        }
        else
        {   
            wxPrintf(wxT("-g: <none>\n"));
        }

        wxPrintf(wxT("OK\n"));    
    }
    
#else
    wxInitializer init;
    wxString cmdLine(wxT("test"));
    FspcCli cli(cmdLine);
    
    cli.Usage();
    
    // Test the default setting of arguments.
    wxASSERT(cli.GetParamHelp() == false);
    wxASSERT(cli.GetParamVersion() == false);
    wxASSERT(cli.GetParamVerbose() == false);
    wxASSERT(cli.GetParamDebug() == false);
    wxASSERT(cli.GetParamTextMode() == false);
    wxASSERT(cli.GetParamLoadConf(value) == false);    
    wxASSERT(cli.GetParamSaveConf(value) == false);    
    wxASSERT(cli.GetParamMode(mode) == false);
    wxASSERT(cli.GetParamProp(value) == false);

    // Test -c parameters.
    cmdLine = wxString(wxT("test --load=xxx.cfg"));
    cli.DoParse(cmdLine);

    wxASSERT(cli.GetParamHelp() == false);
    wxASSERT(cli.GetParamVersion() == false);
    wxASSERT(cli.GetParamVerbose() == false);
    wxASSERT(cli.GetParamDebug() == false);
    wxASSERT(cli.GetParamTextMode() == false);
    wxASSERT(cli.GetParamLoadConf(value) == true);    
    wxASSERT(value == wxT("xxx.cfg"));
    wxASSERT(cli.GetParamSaveConf(value) == false);        
    wxASSERT(cli.GetParamMode(mode) == false);
    wxASSERT(cli.GetParamProp(value) == false);

    // Test -m parameters.
    cmdLine = wxString(wxT("test"));
    cli.DoParse(cmdLine);    
    wxASSERT(cli.GetParamMode(mode) == false);
    wxASSERT(mode == FSPC_ICON_MODE_DISABLE);
    cmdLine = wxString(wxT("test -mhidden"));
    cli.DoParse(cmdLine);    
    wxASSERT(cli.GetParamMode(mode) == true);
    wxASSERT(mode == FSPC_ICON_MODE_HIDDEN);
    cmdLine = wxString(wxT("test --mode=static"));
    cli.DoParse(cmdLine);
    wxASSERT(cli.GetParamMode(mode) == true);
    wxASSERT(mode == FSPC_ICON_MODE_STATIC);
    cmdLine = wxString(wxT("test -m dynamic"));
    cli.DoParse(cmdLine);
    wxASSERT(cli.GetParamMode(mode) == true);
    wxASSERT(mode == FSPC_ICON_MODE_DYNAMIC);

    // Test Key Value Hash.
    KeyValueHash hash;
    KeyValueHash hash2;
    wxString str;
    hash[wxT("a")] = wxT("1");
    hash[wxT("bc")] = wxT("2");
    hash[wxT("def")] = wxT("34");
    hash[wxT("ghij")] = wxT("567");
    wxASSERT(ConvertHashToKeyValue(hash, str) == true);
    wxASSERT(ConvertKeyValueToHash(str, hash2) == true);
    wxASSERT(hash[wxT("a")] == hash2[wxT("a")]);
    wxASSERT(hash[wxT("bc")] == hash2[wxT("bc")]);
    wxASSERT(hash[wxT("def")] == hash2[wxT("def")]);
    wxASSERT(hash[wxT("ghij")] == hash2[wxT("ghij")]);

    // Test done.
    wxPrintf(wxT("\nAll tests passed..\n"));
#endif
}

