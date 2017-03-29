/**
 * @file fspcMainFrame.cpp The main frame module.
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
 * $Id: fspcMainFrame.cpp 28876 2008-10-27 06:17:55Z avatar $
 */

#include <wx/wx.h>
#include <wx/xrc/xmlres.h>
#include <wx/html/htmlwin.h>
#include <wx/imaglist.h>
#include "fspcInfoFrame.h"
#include "fspcConfig.h"
#include "fspcMainFrame.h"

/** bitmap file settings */
#define	BITMAP_RELATIVE_PATH		"pic/"
#define	BITMAP_FILE_EXT			".png"

/**
 * @brief Default constructor.
 */
FspcMainFrame::FspcMainFrame()
{
	/* initialize FrameInfo */
	g_infoFrame = new FspcInfoFrame();
	g_infoFrame->Init();
}

/**
 * @brief Destructor.
 */
FspcMainFrame::~FspcMainFrame()
{
	if (g_infoFrame)
	{
		delete g_infoFrame;
		g_infoFrame = NULL;
	}
}

/**
 * @brief Initialize all visual controls.
 */
void FspcMainFrame::Init(void)
{
	/* set frame height and width */
	SetSize(GET_INFO_INT(wxT("frameMainWidth")),
	    GET_INFO_INT(wxT("frameMainHeight")));

	/* initialize controls */
	InitListbookMain();
	InitHtmlWin();
	
	/* link controls that are frequently accessed */
	LinkCtrls();

	/* set up control states */
	SetupCtrls();
	
	/* change bitmap for VHScr according to current settings */
	ChangeBmpVHScr();
}

/**
 * @brief Get and save frequently-used controls.
 */
void FspcMainFrame::LinkCtrls(void)
{
	m_btnApply = XRCCTRL(*this, "btnApply", wxButton);
	m_cbEnableOnPadClick = XRCCTRL(*this, "cbEnableOnPadClick", wxCheckBox);
	m_cbEnableVScr = XRCCTRL(*this, "cbEnableVScr", wxCheckBox);
	m_cbEnableHScr = XRCCTRL(*this, "cbEnableHScr", wxCheckBox);
	m_sldrAccel = XRCCTRL(*this, "sldrAccel", wxSlider);
}

/**
 * @brief Set up controls according to configuration.
 */
void FspcMainFrame::SetupCtrls(void)
{
	int cfgInt;
	
	cfgInt = g_fspcConfig->GetConfigInt(FC_ENABLE_ON_PAD_CLICK);
	m_cbEnableOnPadClick->SetValue(cfgInt ? 1:0);

	cfgInt = g_fspcConfig->GetConfigInt(FC_ENABLE_VSCR);
	m_cbEnableVScr->SetValue(cfgInt ? 1:0);

	cfgInt = g_fspcConfig->GetConfigInt(FC_ENABLE_HSCR);
	m_cbEnableHScr->SetValue(cfgInt ? 1:0);

	cfgInt = g_fspcConfig->GetConfigInt(FC_ACCELERATION);
	m_sldrAccel->SetValue(cfgInt);
}

/**
 * @brief Initialize the wxListBook control.
 *
 * Initialize the width, height and page images.
 */
void FspcMainFrame::InitListbookMain(void)
{
	wxListbook *lb;
	wxImageList *il;
	int imgWidth, imgHeight;
	
	imgWidth = GET_INFO_INT(wxT("listbookImgWidth"));
	imgHeight = GET_INFO_INT(wxT("listbookImgHeight"));
	il = new wxImageList(imgWidth, imgHeight);

	lb = XRCCTRL(*this, "listbookMain", wxListbook);
	lb->AssignImageList(il);

	for (unsigned int i = 0; i < lb->GetPageCount(); ++i)
	{
		wxString str;
		wxBitmap bmp;
		wxImage img;

		/* compose bitmap filename */
		str = wxT(BITMAP_RELATIVE_PATH) + lb->GetPage(i)->GetName() +
		    wxT(BITMAP_FILE_EXT);

		/* load bitmap */
		bmp = wxBitmap(str, wxBITMAP_TYPE_ANY);
		if (bmp.Ok())
		{
			/* comvert bitmap to image */
			img = bmp.ConvertToImage();
			/* scale image and add it to image list */
			il->Add(img.Scale(imgWidth, imgHeight));

			/* set image to page */
			lb->SetPageImage(i, i);
		}
	}
}

/**
 * @brief Initialize all wxHtmlWindow controls.
 *
 * Load text and set to wxHtmlWindow controls.
 */
void FspcMainFrame::InitHtmlWin(void)
{
	wxHtmlWindow *hw;

	hw = XRCCTRL(*this, "htmlWinDeclaration", wxHtmlWindow);
	hw->SetPage(GET_INFO_STR(wxT("htmlWinDeclarationText")));

	hw = XRCCTRL(*this, "htmlWinClickOnPad", wxHtmlWindow);
	hw->SetPage(GET_INFO_STR(wxT("htmlWinClickOnPadText")));

	hw = XRCCTRL(*this, "htmlWinScrollOnPad", wxHtmlWindow);
	hw->SetPage(GET_INFO_STR(wxT("htmlWinScrollOnPadText")));

	hw = XRCCTRL(*this, "htmlWinAcceleration", wxHtmlWindow);
	hw->SetPage(GET_INFO_STR(wxT("htmlWinAccelerationText")));

	hw = XRCCTRL(*this, "htmlWinSwInfo", wxHtmlWindow);
	hw->SetPage(GET_INFO_STR(wxT("htmlWinSwInfoText")));
}

/**
 * @brief Change the bitmap of specified wxStaticBitmap control.
 *
 * @param ctrlName The name of wxStaticBitmap control.
 * @param bmpFileName The bitmap filename.
 */
void FspcMainFrame::ChangeStaticBitmap(wxString ctrlName, wxString &bmpFileName)
{
	wxStaticBitmap *sb;
	wxBitmap bmp;
	wxString str;

	str = wxT(BITMAP_RELATIVE_PATH) + bmpFileName + wxT(BITMAP_FILE_EXT);
	bmp = wxBitmap(str, wxBITMAP_TYPE_ANY);

	sb = wxStaticCast((*this).FindWindow(wxXmlResource::Get()->GetXRCID(ctrlName)),
	    wxStaticBitmap);
	sb->SetBitmap(bmp);
}

/**
 * @brief Change the bitmap, which illustrates vertical/horizontal scroll
 *     function, according to control states.
 */
void FspcMainFrame::ChangeBmpVHScr(void)
{
	static wxString bmpNames[] = { wxT("VHDisabled"), wxT("VEnabled"),
	    wxT("HEnabled"), wxT("VHEnabled") };
	wxString str = bmpNames[((int)m_cbEnableHScr->IsChecked() << 1) |
	        ((int)m_cbEnableVScr->IsChecked())];

	ChangeStaticBitmap(wxT("sBmpVHScr"), str);
}

/**
 * @brief Update configuration to current control states.
 */
void FspcMainFrame::UpdateConfig(void)
{
	int cfgInt;
	
	cfgInt = m_cbEnableOnPadClick->GetValue();
	g_fspcConfig->SetConfigInt(FC_ENABLE_ON_PAD_CLICK, cfgInt);

	cfgInt = m_cbEnableVScr->GetValue();
	g_fspcConfig->SetConfigInt(FC_ENABLE_VSCR, cfgInt);

	cfgInt = m_cbEnableHScr->GetValue();
	g_fspcConfig->SetConfigInt(FC_ENABLE_HSCR, cfgInt);

	cfgInt = m_sldrAccel->GetValue();
	g_fspcConfig->SetConfigInt(FC_ACCELERATION, cfgInt);
}

/**
 * @brief Enable or disable 'Apply' button.
 */
void FspcMainFrame::EnableApply(int en)
{
	m_btnApply->Enable(en ? 1:0);
}

/*
 * TABLE OF EVENTS
 */
BEGIN_EVENT_TABLE(FspcMainFrame, wxFrame)
	EVT_CLOSE(FspcMainFrame::OnClose)

	EVT_CHECKBOX(XRCID("cbEnableOnPadClick"),
	    FspcMainFrame::OnCheckBoxCbEnableOnPadClick)
	EVT_CHECKBOX(XRCID("cbEnableVScr"),
	    FspcMainFrame::OnCheckBoxCbEnableVScr)
	EVT_CHECKBOX(XRCID("cbEnableHScr"),
	    FspcMainFrame::OnCheckBoxCbEnableHScr)
	EVT_SCROLL_CHANGED( FspcMainFrame::OnScrollChangedSldrAccel)

	EVT_BUTTON(XRCID("btnOk"), FspcMainFrame::OnButtonClickOk)
	EVT_BUTTON(XRCID("btnCancel"), FspcMainFrame::OnButtonClickCancel)
	EVT_BUTTON(XRCID("btnApply"), FspcMainFrame::OnButtonClickApply)
	EVT_BUTTON(XRCID("btnDefault"), FspcMainFrame::OnButtonClickDefault)

	EVT_HTML_LINK_CLICKED(XRCID("htmlWinSwInfo"),
	    FspcMainFrame::OnHtmlLinkClickSwInfo )

END_EVENT_TABLE()

/**
 * @brief Destroy the frame when closing.
 */
void FspcMainFrame::OnClose(wxCloseEvent &/*evt*/)
{
	/* destroy the window safely */
	Destroy();
}

/**
 * @brief On 'EnableOnPadClick' checkbox click.
 */
void FspcMainFrame::OnCheckBoxCbEnableOnPadClick(wxCommandEvent& event)
{
	EnableApply(true);
}

/**
 * @brief On 'EnableVScr' checkbox click.
 */
void FspcMainFrame::OnCheckBoxCbEnableVScr(wxCommandEvent& event)
{
	ChangeBmpVHScr();

	EnableApply(true);
}

/**
 * @brief On 'EnableHScr' checkbox click.
 */
void FspcMainFrame::OnCheckBoxCbEnableHScr(wxCommandEvent& event)
{
	ChangeBmpVHScr();
	
	EnableApply(true);
}

/**
 * @brief On 'SldrAccel' scroll changed.
 */
void FspcMainFrame::OnScrollChangedSldrAccel(wxScrollEvent& event)
{
	EnableApply(true);
}

/**
 * @brief On 'Ok' button click.
 */
void FspcMainFrame::OnButtonClickOk(wxCommandEvent& event)
{
	int ret;
	
	/* update configuration */
	UpdateConfig();
	/* save user configuration */
	ret = g_fspcConfig->SaveUserConfig();
	if (ret != 0)
	{
		::wxLogError(wxT("Cannot save user configuration file(%d)."),
		    ret);
		/* keep going */
	}
	/* change device configuration */
	ret = g_fspcConfig->SaveDeviceConfig();
	if (ret != 0)
	{
		::wxLogError(wxT("Cannot save device configuration(%d)."),
		    ret);
		/* keep going */
	}

	/* close the window */
	Close(true);
}

/**
 * @brief On 'Cancel' button click.
 */
void FspcMainFrame::OnButtonClickCancel(wxCommandEvent& event)
{
	Close(true);
}

/**
 * @brief On 'Apply' button click.
 */
void FspcMainFrame::OnButtonClickApply(wxCommandEvent& event)
{
	int ret;

	/* update configuration */
	UpdateConfig();
	/* save user configuration */
	ret = g_fspcConfig->SaveUserConfig();
	if (ret != 0)
	{
		::wxLogError(wxT("Cannot save user configuration file(%d)."),
		    ret);
		/* keep going */
	}
	/* change device configuration */
	ret = g_fspcConfig->SaveDeviceConfig();
	if (ret != 0)
	{
		::wxLogError(wxT("Cannot save device configuration(%d)."),
		    ret);
		/* keep going */
	}
	
	EnableApply(false);
}

/**
 * @brief On 'Default' button click.
 */
void FspcMainFrame::OnButtonClickDefault(wxCommandEvent& event)
{
	/* load default configuration */
	g_fspcConfig->LoadDefaultConfig();
	/* set up controls */
	SetupCtrls();
	
	EnableApply(true);
}

/**
 * @brief On 'SwInfo' HTML-link click.
 */
void FspcMainFrame::OnHtmlLinkClickSwInfo(wxHtmlLinkEvent& event)
{
	wxLaunchDefaultBrowser(event.GetLinkInfo().GetHref());
}
