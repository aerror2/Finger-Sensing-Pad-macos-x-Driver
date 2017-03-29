/**
 * @file fspcCli.h The include file of FspcCli module.
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
 * $Id: fspcCli.h 28876 2008-10-27 06:17:55Z avatar $
 */

#ifndef __FSPCCLI_H__
#define __FSPCCLI_H__

#include <wx/cmdline.h>

const wxString FSPC_ICON_MODE_SYMBOL_DISABLE (wxT("disable"));
const wxString FSPC_ICON_MODE_SYMBOL_HIDDEN (wxT("hidden"));
const wxString FSPC_ICON_MODE_SYMBOL_STATIC (wxT("static"));
const wxString FSPC_ICON_MODE_SYMBOL_DYNAMIC (wxT("dynamic"));

/// The enumeration of tray icon mode.
enum FspcIconMode {
    FSPC_ICON_MODE_DISABLE = 0,
    FSPC_ICON_MODE_HIDDEN = 1,
    FSPC_ICON_MODE_STATIC = 2,
    FSPC_ICON_MODE_DYNAMIC = 3
    };

/// The wxString key/value pair hash object.
WX_DECLARE_STRING_HASH_MAP( wxString, KeyValueHash );

extern bool ConvertKeyValueToHash(wxString &str, KeyValueHash &hash,
    const wxChar *comma=wxT(","), const wxChar *assign=wxT("="));
extern bool ConvertHashToKeyValue(KeyValueHash &hash, wxString &str,
    const wxChar *comma=wxT(","), const wxChar *assign=wxT("="));

/// The class of command line interface(CLI) for fspc program.
class FspcCli
{
private:
    /// The result of command line parser.
    bool m_parseResult;
    /// The command line parser object for internal use.
    wxCmdLineParser m_parser;
    wxString m_ver;
    bool m_verbose;
    bool m_debug;
protected:

public:
    FspcCli();
    FspcCli(int argc, char **argv, const char *logo=NULL);
    FspcCli(wxString &cmdLine, wxString &logo);
    FspcCli(wxString &cmdLine);
    static const bool CheckTextMode(int argc, char **argv);
    void SetLogo(const wxString& logo);
    bool DoParse();
    bool DoParse(int argc, char **argv);
    bool DoParse(wxString &cmdLine);
    bool DoInit();
    bool GetResult();
    void Usage();
    void SetVersion(wxString ver);
    bool GetParamHelp();
    bool GetParamVersion();
    bool GetParamVerbose();
    bool GetParamDebug();
    bool GetParamTextMode();
    bool GetParamLoadConf(wxString& value);
    bool GetParamSaveConf(wxString& value);
    bool GetParamMode(FspcIconMode& mode);
    bool GetParamProp(wxString& value);
    void VerboseOutput(wxString str);
    void DebugOutput(wxString str);
    int Execute(void);
};

#endif
