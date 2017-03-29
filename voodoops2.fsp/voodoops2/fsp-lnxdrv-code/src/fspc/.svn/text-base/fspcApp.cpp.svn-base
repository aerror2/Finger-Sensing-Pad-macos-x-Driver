/**
 * @file fspcApp.cpp The application module.
 *
 * Copyright (C) 2008 Yu-chung Tsao, Sentelic Corporation.
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
 * $Id: fspcApp.cpp 28876 2008-10-27 06:17:55Z avatar $
 */

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/stdpaths.h>
#include <wx/file.h>
#include "fspcMainFrame.h"
#include "fspcInfoFrame.h"
#include "fspcConfig.h"
#include "fspcCli.h"
#if !defined(_WIN32)
#include <sentelic.h>
#include <fsp.h>
#endif
#include "fspcApp.h"

/** FSPC version string */
#define	FSPC_VERSION		"FSPC 1.0.0"

#ifndef	FSPC_USE_PNG
#define	FSPC_USE_PNG		(1)
#endif

/** XRC filename format */
#define	FSPC_XRC_FILE		"fspcui%s.xrc"
/** application instance name */
#define	FSPC_INSTANCE		"fspc"

/* a handle for determining if device exists */
void *g_hTestDev = NULL;

/* implement application without a main function because we are gonna
    implement our own one */
IMPLEMENT_APP_NO_MAIN(FspcApp);

/**
 * @brief Application initialization.
 *
 * @note See wxApp::OnInit().
 */
bool FspcApp::OnInit(void)
{
	wxString xrcFile;
	int ret;

	/* see if device exists */
	if (g_hTestDev == NULL)
	{
		::wxLogError(_T("FSP not detected."));
		return (false);
	}

	/* get XRC filename */
	xrcFile = GetXrcFile();
	if (xrcFile.IsEmpty())
	{
		::wxLogError(_T("Missing resource file."));
		return (false);
	}

	/* one instance checking */
	m_checker = new wxSingleInstanceChecker(
	    wxT(FSPC_INSTANCE) + wxGetUserId());
	if (m_checker->IsAnotherRunning())
	{
		delete m_checker;
		::wxLogError(_T("Program already running, aborting."));
		return (false);
	}

#if	(FSPC_USE_PNG == 1)
	/* add PNG handler */
	wxImage::AddHandler(new wxPNGHandler);
#endif

	/* initialize configuration */
	g_fspcConfig = new FspcConfig();
	g_fspcConfig->LoadDefaultConfig();
	ret = g_fspcConfig->LoadDeviceConfig();
	if (ret != 0)
	{
		::wxLogError(wxT("Cannot load device configuration(%d)."),
		    ret);
		/* keep going */
	}

	/* load XRC file */
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(xrcFile);

	/* load FrameMain */
	m_mainFrame = new FspcMainFrame();
	wxXmlResource::Get()->LoadFrame(m_mainFrame, NULL,
	    wxT(FSPC_MAIN_FRAME_NAME));

	/* initialize FrameMain controls */
	m_mainFrame->Init();
	
	/* show FrameMain */
	m_mainFrame->Show(true);
	SetTopWindow(m_mainFrame);
	
	/* return true to continue processing */
	return (true);
}

/**
 * @brief Application finalization.
 *
 * @note See wxApp::OnExit().
 */
int FspcApp::OnExit(void)
{
	/* destroy configuration object */
	delete g_fspcConfig;
	/* destroy single-instance checker */
	delete m_checker;
	
	return (0);
}

/**
 * @brief Get the XRC filename according to the locale.
 *
 * @return The XRC filename.
 *
 * @ntoe If the localized XRC file doesn't exist, default XRC file will be used.
 *     If both files don't exist, an empty filename is returned.
 */
wxString FspcApp::GetXrcFile(void)
{
	wxLocale l;
	wxString name;
	wxStandardPaths sp;

	/* initialize locale */
	l.Init();

	/* set working directory to resource directory */
	::wxSetWorkingDirectory(sp.GetResourcesDir());

	/* compose XRC filename with locale canonical name */
	name.Printf(wxT(FSPC_XRC_FILE),
	    (wxT("_") + l.GetCanonicalName()).c_str());
	/* use default XRC file if localized XRC file doesn't exist */
	if (wxFile::Exists(name) == false)
	{
		name.Printf(wxT(FSPC_XRC_FILE), wxT(""));

		if (wxFile::Exists(name) == false)
		{
			name.Clear();
		}
	}

	return (name);
}

/**
 * @brief Program entry.
 */
#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
#if defined(_WIN32)
	/* just set non-NULL value to let GUI work in Win32 */
	g_hTestDev = (void *)1;
	
	return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
#else
	/* get and free device handle */
	/* just see if the device exists */
	g_hTestDev = fsp_open_dev();
	if (g_hTestDev != NULL)
	{
		fsp_free_dev(g_hTestDev);
	}

	bool textMode = FspcCli::CheckTextMode(argc, argv);

	if (!textMode)
	{
		/* GUI */

		return wxEntry(argc, argv); 
	}
	else
	{
		/* console */

		/* initialize wx libraries */
		wxInitializer initObj;

		/* add message standard error output handler */
		wxMessageOutput::Set(new wxMessageOutputStderr);

		if (g_hTestDev == NULL)
		{
			wxPrintf(_T("FSP not detected.\n"));
			return (-1);
		}

		/* create console module */
		FspcCli cli(argc, argv);
		cli.SetVersion(wxT(FSPC_VERSION));

		return cli.Execute();
	}
#endif
}
