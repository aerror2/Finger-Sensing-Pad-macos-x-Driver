/**
 * @file fspcInfoFrame.cpp The information extraction module.
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
 * $Id: fspcInfoFrame.cpp 28876 2008-10-27 06:17:55Z avatar $
 */

#include <wx/wx.h>
#include <wx/frame.h>
#include <wx/xrc/xmlres.h>
#include "fspcInfoFrame.h"

/** object pointer */
FspcInfoFrame *g_infoFrame = NULL;

/**
 * @brief Initialize info frame.
 */
void FspcInfoFrame::Init(void)
{
	wxXmlResource::Get()->LoadFrame(this, NULL,
	    wxT(FSPC_INFO_FRAME_NAME));
}

/**
 * @brief Get the info of type string.
 *
 * @param key Info key.
 */
wxString FspcInfoFrame::GetInfoStr(const wxString &key)
{
	wxStaticText *st;
	wxString str;
	
	st = wxStaticCast((*this).FindWindow(wxXmlResource::Get()->GetXRCID(key)),
	    wxStaticText);
	str = st->GetLabel();
	
	return (str);
}

/**
 * @brief Get the info of type integer.
 *
 * @param key Info key.
 */
int FspcInfoFrame::GetInfoInt(const wxString &key)
{	
	long res;
	
	GetInfoStr(key).ToLong(&res);
	
	return (res);
}
