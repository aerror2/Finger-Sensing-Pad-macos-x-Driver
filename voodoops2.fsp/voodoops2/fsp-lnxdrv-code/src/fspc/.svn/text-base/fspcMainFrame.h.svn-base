/**
 * @file fspcMainFrame.h The main frame module.
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
 * $Id: fspcMainFrame.h 28876 2008-10-27 06:17:55Z avatar $
 */

#ifndef	__FSPC_MAIN_FRAME_H__
#define	__FSPC_MAIN_FRAME_H__

#include <wx/listbook.h>
#include <wx/html/htmlwin.h>

/** the name of main frame in the XRC file */
#define	FSPC_MAIN_FRAME_NAME		"frameMain"

/**
 * @brief Main frame class.
 */
class FspcMainFrame: public wxFrame
{
private:
	wxButton *m_btnApply;
	wxCheckBox *m_cbEnableOnPadClick;
	wxCheckBox *m_cbEnableVScr;
	wxCheckBox *m_cbEnableHScr;
	wxSlider *m_sldrAccel;

	void LinkCtrls(void);
	void SetupCtrls(void);

	void InitListbookMain(void);
	void InitHtmlWin(void);

	void ChangeStaticBitmap(wxString ctrlName, wxString &bmpFileName);
	void ChangeBmpVHScr(void);

	void UpdateConfig(void);

	void EnableApply(int en);

	/* event handlers */
	void OnClose(wxCloseEvent &evt);
	void OnCheckBoxCbEnableOnPadClick(wxCommandEvent& event);
	void OnCheckBoxCbEnableVScr(wxCommandEvent& event);
	void OnCheckBoxCbEnableHScr(wxCommandEvent& event);
	void OnScrollChangedSldrAccel(wxScrollEvent& event);
	void OnButtonClickOk(wxCommandEvent& event);
	void OnButtonClickCancel(wxCommandEvent& event);
	void OnButtonClickApply(wxCommandEvent& event);
	void OnButtonClickDefault(wxCommandEvent& event);
	void OnHtmlLinkClickSwInfo(wxHtmlLinkEvent& event);
	DECLARE_EVENT_TABLE()

protected:

public:
	FspcMainFrame();
	~FspcMainFrame();

	void Init(void);
};

#endif	/* __FSPC_MAIN_FRAME_H__ */
