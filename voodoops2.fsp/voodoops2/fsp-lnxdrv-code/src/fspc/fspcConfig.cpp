/**
 * @file fspcConfig.cpp The configuration module.
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
 * $Id: fspcConfig.cpp 47109 2010-07-08 07:20:52Z avatar $
 */

#include <wx/wx.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/fileconf.h>
#if !defined(_WIN32)
#include <sentelic.h>
#include <fsp.h>
#include "msaccel.h"
#endif
#include "fspcConfig.h"

#ifndef	TEST_CONFIG_FILE
#define	TEST_CONFIG_FILE		(0)
#endif

#ifndef	USE_X_MOUSE_ACCELERATION
#define	USE_X_MOUSE_ACCELERATION	(1)
#endif

/** directory permission settings of user configuration file */
#define	USER_CONFIG_DIR_PERM		(0700)

/** configuration filename */
#define	FSPC_CONFIG_FILENAME		wxT("fspc.ini")

/** object pointer */
FspcConfig *g_fspcConfig = NULL;

/** table of configuration items */
FSP_CONFIG_ITEM g_configItems[] =
{
/*	key				val		defVal		*/
	{ wxT("EnableOnPadClick"),	wxT("1"),	wxT("") },
	{ wxT("EnableVScr"),		wxT("1"),	wxT("") },
	{ wxT("EnableHScr"),		wxT("0"),	wxT("") },
	{ wxT("Acceleration"),		wxT("5"),	wxT("") }
};

/** table of cursor acceleration settings */
#define	MAX_NUM_ACCEL_SETTINGS		(10)
#define	DEFAULT_ACCEL_SETTING_IDX	(4)
int g_accelSettings[MAX_NUM_ACCEL_SETTINGS][3] =
{
/*	num	denom	thres	*/
	{1,	4,	1},
	{1,	2,	1},
	{1,	1,	1},
	{3,	2,	3},
	{2,	1,	3},
	{5,	2,	3},
	{3,	1,	4},
	{7,	2,	4},
	{4,	1,	4},
	{9,	2,	4}
};

/**
 * @brief Get the index in the table of cursor acceleration settings by given
 *     settings.
 *
 * @param acc_num The numerator of acceleration multiplier.
 * @param acc_denum The denominator of acceleration multiplier.
 * @param thres The threshold for cursor acceleration.
 *
 * @return The index in the table of cursor acceleration settings. If none
 *     matches, return the default index.
 */
int GetAccelSettingIdx(int acc_num, int acc_denom, int thres)
{
	int i;
	
	for (i = 0; i < MAX_NUM_ACCEL_SETTINGS; ++i)
	{
		if ((g_accelSettings[i][0] == acc_num) &&
		    (g_accelSettings[i][1] == acc_denom) &&
		    (g_accelSettings[i][2] == thres) )
		{
			return (i);
		}
	}
	
	return (DEFAULT_ACCEL_SETTING_IDX);
}

/**
 * @brief Default constructor.
 *
 * @param userConfigFile Given filename of non-zero length to overwrite default
 *     user configuration file.
 */
FspcConfig::FspcConfig(wxString userConfigFile)
{
#if	(TEST_CONFIG_FILE == 0)
	wxStandardPaths sp;

	/* default configuration file */
	m_defaultConfigFile = sp.GetResourcesDir() +
	    wxFileName::GetPathSeparator() + FSPC_CONFIG_FILENAME;
	if (userConfigFile.IsEmpty())
	{
		/* default user configuration file */
		m_userConfigFile = sp.GetUserDataDir() +
		    wxFileName::GetPathSeparator() + FSPC_CONFIG_FILENAME;
	}
#else
	m_defaultConfigFile = wxT("fspc_default.ini");
	if (userConfigFile.IsEmpty())
	{
		m_userConfigFile = wxT("fspc_user.ini");
	}
#endif
	else
	{
		/* overwrite default user configuration file */
		m_userConfigFile = userConfigFile;
	}
}

/**
 * @brief Destructor.
 */
FspcConfig::~FspcConfig()
{

}

/**
 * @brief Load default configuration and store it as current configuration.
 *
 * Load default configuration from file or use hard-coded settings if
 *     configuration file doesn't exist.
 */
void FspcConfig::LoadDefaultConfig(void)
{
	/* see if default configuration has been set */
	if (g_configItems[0].defVal.IsEmpty() == false)
	{
		/* default configuration has been set */
	
		/* copy default configuration to current configuration */
		for (int i = 0; i < FC_MAX; ++i)
		{
			g_configItems[i].val = g_configItems[i].defVal;
		}
	}

	/* see if the file of default configuration exists */
	if (wxFile::Exists(m_defaultConfigFile) == true)
	{
		/* file of default configuration exists */
		
		/* open input stream */
		wxFileInputStream fis(m_defaultConfigFile);

		/* see if input stream is ready */
		if (fis.Ok() == true)
		{
			/* input stream is read */

			/* attach input stream to wxFileConfig */
			wxFileConfig fc(fis);

			/* load and set configuration items */
			for (int i = 0; i < FC_MAX; ++i)
			{
				g_configItems[i].val =
				    fc.Read(g_configItems[i].key,
				    g_configItems[i].val);
			}
		}
	}
	
	/* make a copy of default configuration */
	for (int i = 0; i < FC_MAX; ++i)
	{
		g_configItems[i].defVal = g_configItems[i].val;
	}
}

/**
 * @brief Get current device configuration and store it as current
 *     configuration.
 *
 * @retval -1 Cannot open device.
 * @retval -2 Cannot load device configuration.
 * @retval -3 Cannot get mouse acceleration.
 * @retval 0 Success.
 */
int FspcConfig::LoadDeviceConfig(void)
{
#if !defined(_WIN32)
	int cfgInt, ret;
	int acc_denom, thres;	
	void *h;

	/* get device handle */
	h = fsp_open_dev();
	if (h == NULL)
	{
		return (-1);
	}

	/* get driver flags */
	ret = fsp_get_flags(h, &cfgInt);
	if (ret != 0)
	{
		goto l_free_dev;
	}
	/* FC_ENABLE_ON_PAD_CLICK */
	SetConfigInt(FC_ENABLE_ON_PAD_CLICK,
	    (cfgInt & FSPDRV_FLAG_EN_OPC) ? 1:0);

	/* get vertical scrolling state */
	ret = fsp_get_vscr(h, &cfgInt);
	if (ret != 0)
	{
		goto l_free_dev;
	}
	/* FC_ENABLE_VSCR */
	SetConfigInt(FC_ENABLE_VSCR, cfgInt);
	
	/* get horizontal scrolling state */
	ret = fsp_get_hscr(h, &cfgInt);
	if (ret != 0)
	{
		goto l_free_dev;
	}
	/* FC_ENABLE_HSCR */
	SetConfigInt(FC_ENABLE_HSCR, cfgInt);

	/* get acceleration values */
	ret = fsp_get_accel(h, &cfgInt, &acc_denom, &thres);
	if (ret != 0)
	{
#if	(USE_X_MOUSE_ACCELERATION == 1)
		/* fallback to X based mouse acceleration */
		ret = get_mouse_acc_num(&cfgInt);
		if (ret == 0)
			SetConfigInt(FC_ACCELERATION, cfgInt);
		else
			ret = -3;
		goto l_free_dev;
#endif
	}
	/* FC_ACCELERATION */
	SetConfigInt(FC_ACCELERATION, GetAccelSettingIdx(cfgInt, acc_denom,
	    thres) + 1);

l_free_dev:
	fsp_free_dev(h);

	/* any error so far ? */
	if (ret != 0)
	{
		return (-2);
	}
#endif
	
	return (0);
}

/**
 * @brief Change current device configuration.
 *
 * Change device configuration only when current configuration is different
 *     from device configuration.
 *
 * @retval -1 Cannot open device.
 * @retval -2 Cannot save device configuration.
 * @retval -3 Cannot set X mouse acceleration.
 * @retval 0 Success.
 */
int FspcConfig::SaveDeviceConfig(void)
{
#if !defined(_WIN32)
	int cfgInt, itemVal, newFlags, ret;
	int acc_denom, thres;
	void *h;

	/* get device handle */
	h = fsp_open_dev();
	if (h == NULL)
	{
		return (-1);
	}

	/* get driver flags */
	ret = fsp_get_flags(h, &cfgInt);
	if (ret != 0)
	{
		goto s_free_dev;
	}
	newFlags = cfgInt;
	/* FC_ENABLE_ON_PAD_CLICK */
	itemVal = GetConfigInt(FC_ENABLE_ON_PAD_CLICK) ? 1:0;
	if (itemVal == 1)
	{
		newFlags |= FSPDRV_FLAG_EN_OPC;
	}
	else
	{
		newFlags &= ~FSPDRV_FLAG_EN_OPC;
	}
	/* XXX: more items to be added here */

	/* see if flags are different */
	if (newFlags != cfgInt)
	{
		/* different */

		/* set driver flags */
		ret = fsp_set_flags(h, newFlags);
		if (ret != 0)
		{
			goto s_free_dev;
		}
	}
		
	/* get vertical scrolling state */
	ret = fsp_get_vscr(h, &cfgInt);
	if (ret != 0)
	{
		goto s_free_dev;
	}
	/* FC_ENABLE_VSCR */
	itemVal = GetConfigInt(FC_ENABLE_VSCR) ? 1:0;
	/* see if the settings are different */
	if (itemVal != cfgInt)
	{
		/* set vertical scrolling state */
		ret = fsp_set_vscr(h, itemVal);
		if (ret != 0)
		{
			goto s_free_dev;
		}
	}
	
	/* get horizontal scrolling state */
	ret = fsp_get_hscr(h, &cfgInt);
	if (ret != 0)
	{
		goto s_free_dev;
	}
	/* FC_ENABLE_HSCR */
	itemVal = GetConfigInt(FC_ENABLE_HSCR) ? 1:0;
	/* see if the settings are different */
	if (itemVal != cfgInt)
	{
		/* set horizontal scrolling state */
		ret = fsp_set_hscr(h, itemVal);
		if (ret != 0)
		{
			goto s_free_dev;
		}
	}

	/* get acceleration values */
	ret = fsp_get_accel(h, &cfgInt, &acc_denom, &thres);
	if (ret != 0)
	{
#if	(USE_X_MOUSE_ACCELERATION == 1)
		/* fallback to X based mouse acceleration */
		ret = get_mouse_acc_num(&cfgInt);
		if (ret != 0)
		{
			ret = -3;
		}
		else
		{
			itemVal = GetConfigInt(FC_ACCELERATION);
			if (itemVal != cfgInt)
			{
				ret = set_mouse_acc_num(itemVal);
				if (ret != 0)
				{
					ret = -3;
				}
			}
		}
#endif
		goto s_free_dev;
	}
	/* FC_ACCELERATION */
	itemVal = GetConfigInt(FC_ACCELERATION);
	/* see if the settings are different */
	if (itemVal != (GetAccelSettingIdx(cfgInt, acc_denom, thres) + 1))
	{
		/* get 0-based index */
		itemVal -= 1;

		/* set acceleration values  */
		ret = fsp_set_accel(h, g_accelSettings[itemVal][0],
		    g_accelSettings[itemVal][1], g_accelSettings[itemVal][2]);
		if (ret != 0)
		{
			goto s_free_dev;
		}
	}

s_free_dev:
	fsp_free_dev(h);

	/* any error so far ? */
	if (ret != 0)
	{
		return (-2);
	}
#endif

	return (0);
}

/**
 * @brief Load user configuration from file.
 *
 * @retval -1 File doesn't exist.
 * @retval -2 Input stream is not ready.
 * @retval 0 Success.
 */
int FspcConfig::LoadUserConfig(void)
{
	/* see if file exists */
	if (wxFile::Exists(m_userConfigFile) == false)
	{
		/* file doesn't exist */
		
		return (-1);
	}

	/* open input stream */
	wxFileInputStream fis(m_userConfigFile);

	/* see if input stream is ready */
	if (fis.Ok() == false)
	{
		/* input stream is not ready */

		return (-2);
	}

	/* attach input stream to wxFileConfig */
	wxFileConfig fc(fis);

	/* load and set configuration items */
	for (int i = 0; i < FC_MAX; ++i)
	{
		g_configItems[i].val = fc.Read(g_configItems[i].key,
		    g_configItems[i].defVal);
	}

	return (0);
}

/**
 * @brief Save user configuration to file.
 *
 * @retval -1 Output stream is not ready.
 * @retval -2 Failed to save configuration.
 * @retval -3 Failed to create directories.
 * @retval 0 Success.
 */
int FspcConfig::SaveUserConfig(void)
{
	wxFileName fn(m_userConfigFile);
	
	/* see if directory exists */
	if (wxFileName::DirExists(fn.GetPath()) == false)
	{
		/* directory doesn't exist */
	
		/* create directory */
		if (wxFileName::Mkdir(fn.GetPath(), USER_CONFIG_DIR_PERM,
		    wxPATH_MKDIR_FULL) == false)
		{
			return (-3);
		}
	}

	/* open output stream */
	wxFileOutputStream fos(m_userConfigFile);

	/* see if output stream is ready */
	if (fos.Ok() == false)
	{
		/* output stream is not ready */
		
		return (-1);
	}

	/* attach output stream to wxFileConfig */
	wxFileConfig fc;

	/* write configuration */
	for (int i = 0; i < FC_MAX; ++i)
	{
		fc.Write(g_configItems[i].key, g_configItems[i].val);
	}

	/* save configuration */
	if (fc.Save(fos) == false)
	{
		return (-2);
	}

	return (0);
}

/**
 * @brief Get configuration of type integer.
 *
 * @param idx The configuration item index.
 *
 * @return Data of type integer.
 */
int FspcConfig::GetConfigInt(FSPC_CONFIG_ITEM_INDEX idx)
{
	long val;
	
	g_configItems[idx].val.ToLong(&val);
	
	return (val);
}

/**
 * @brief Set configuration of type integer.
 *
 * @param idx The configuration item index.
 * @param val Data of type integer.
 */
void FspcConfig::SetConfigInt(FSPC_CONFIG_ITEM_INDEX idx, int val)
{
	g_configItems[idx].val.Printf(wxT("%d"), val);
}

/**
 * @brief Get configuration of type string.
 *
 * @param idx The configuration item index.
 *
 * @return Data of type string.
 */
wxString FspcConfig::GetConfigStr(FSPC_CONFIG_ITEM_INDEX idx)
{
	return (g_configItems[idx].val);
}

/**
 * @brief Set configuration of type string.
 *
 * @param idx The configuration item index.
 * @param val Data of type string.
 */
void FspcConfig::SetConfigStr(FSPC_CONFIG_ITEM_INDEX idx, wxString &val)
{
	g_configItems[idx].val = val;
}

