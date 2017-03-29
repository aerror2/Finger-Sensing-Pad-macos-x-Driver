/**
 * @file fspcInfoFrame.h The information extraction module.
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
 * $Id: fspcInfoFrame.h 28876 2008-10-27 06:17:55Z avatar $
 */

#ifndef	__FSPC_INFO_FRAME_H__
#define	__FSPC_INFO_FRAME_H__

/** the name of info frame in the XRC file */
#define	FSPC_INFO_FRAME_NAME		"frameInfo"

/**
 * @brief Info frame class.
 *
 * InfoFrame contains some settings of visual controls.
 */
class FspcInfoFrame: public wxFrame
{
private:

public:
	void Init(void);

	wxString GetInfoStr(const wxString &key);
	int GetInfoInt(const wxString &key);
};

/* variable exports */
extern FspcInfoFrame *g_infoFrame;

/** macros to get info of different types */
#define	GET_INFO_INT(_key_)	g_infoFrame->GetInfoInt(_key_)
#define	GET_INFO_STR(_key_)	g_infoFrame->GetInfoStr(_key_)

#endif	/* __FSPC_INFO_FRAME_H__ */
