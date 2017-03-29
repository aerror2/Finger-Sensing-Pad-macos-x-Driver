/**
 * @file fspcConfig.h The configuration module.
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
 * $Id: fspcConfig.h 28876 2008-10-27 06:17:55Z avatar $
 */

#ifndef	__FSPC_CONFIG_H__
#define	__FSPC_CONFIG_H__

/** configuration item indexes */
typedef enum _FSPC_CONFIG_ITEM_INDEX
{
	FC_ENABLE_ON_PAD_CLICK,
	FC_ENABLE_VSCR,
	FC_ENABLE_HSCR,
	FC_ACCELERATION,

	FC_MAX
} FSPC_CONFIG_ITEM_INDEX;

/** configuration item attributes */
typedef struct _FSP_CONFIG_ITEM
{
	wxString key;
	wxString val;
	wxString defVal;

} FSP_CONFIG_ITEM;

/**
 * @brief Configuration class.
 *
 * Default configuration, user configuration and device configuration.
 */
class FspcConfig
{
private:
	wxString m_defaultConfigFile;
	wxString m_userConfigFile;

public:
	FspcConfig(wxString userConfigFile = wxT(""));
	~FspcConfig();

	void LoadDefaultConfig(void);
	int LoadDeviceConfig(void);
	int SaveDeviceConfig(void);
	int LoadUserConfig(void);
	int SaveUserConfig(void);

	int GetConfigInt(FSPC_CONFIG_ITEM_INDEX idx);
	void SetConfigInt(FSPC_CONFIG_ITEM_INDEX idx, int val);
	wxString GetConfigStr(FSPC_CONFIG_ITEM_INDEX idx);
	void SetConfigStr(FSPC_CONFIG_ITEM_INDEX idx, wxString &val);
};

/* variable exports */
extern FspcConfig *g_fspcConfig;

#endif	/* __FSPC_CONFIG_H__ */
