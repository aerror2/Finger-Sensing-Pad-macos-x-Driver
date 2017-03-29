/*-
 * Finger Sensing Pad PS/2 mouse driver.
 *
 * Copyright (C) 2005-2007 Asia Vital Components Co., Ltd.
 * Copyright (C) 2005-2009 Tai-hwa Liang, Sentelic Corporation.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: sentelic.c 35958 2009-07-02 07:32:39Z avatar $
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/input.h>
#include <linux/ctype.h>
#include <linux/libps2.h>
#include <linux/serio.h>
#include <linux/jiffies.h>

#include "psmouse.h"
#include "sentelic.h"

/**
 * Timeout for FSP PS/2 command only (in milliseconds).
 */
#define	FSP_CMD_TIMEOUT		200
#define	FSP_CMD_TIMEOUT2	30

/** Driver version. */
static const char fsp_drv_ver[] = "1.0.0";

int fsp_reset(struct psmouse *psmouse);

/*
 * Make sure that the value being sent to FSP will not conflict with
 * possible sample rate values.
 */
static unsigned char fsp_test_swap_cmd(unsigned char reg_val)
{
	switch (reg_val) {
	case 10: case 20: case 40: case 60: case 80: case 100: case 200:
		/*
		 * The requested value being sent to FSP matched to possible
		 * sample rates, swap the given value such that the hardware
		 * wouldn't get confused.
		 */
		return (reg_val >> 4) | (reg_val << 4);
	default:
		return reg_val;	/* swap isn't necessary */
	}
}

/*
 * Make sure that the value being sent to FSP will not conflict with certain
 * commands.
 */
static unsigned char fsp_test_invert_cmd(unsigned char reg_val)
{
	switch (reg_val) {
	case 0xe9: case 0xee: case 0xf2: case 0xff:
		/*
		 * The requested value being sent to FSP matched to certain
		 * commands, inverse the given value such that the hardware
		 * wouldn't get confused.
		 */
		return ~reg_val;
	default:
		return reg_val;	/* inversion isn't necessary */
	}
}

static int fsp_reg_read(struct psmouse *psmouse, int reg_addr, int *reg_val)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char param[3];
	unsigned char addr;
	int rc = -1;

	/*
	 * We need to shut off the device and switch it into command
	 * mode so we don't confuse our protocol handler. We don't need
	 * to do that for writes because sysfs set helper does this for
	 * us.
	 */
	ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE);
	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);
	mutex_lock(&ps2dev->cmd_mutex);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	/* should return 0xfe(request for resending) */
	ps2_sendbyte(ps2dev, 0x66, FSP_CMD_TIMEOUT2);
	/* should return 0xfc(failed) */
	ps2_sendbyte(ps2dev, 0x88, FSP_CMD_TIMEOUT2);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	if ((addr = fsp_test_invert_cmd(reg_addr)) != reg_addr) {
		ps2_sendbyte(ps2dev, 0x68, FSP_CMD_TIMEOUT2);
	} else if ((addr = fsp_test_swap_cmd(reg_addr)) != reg_addr) {
		/* swapping is required */
		ps2_sendbyte(ps2dev, 0xcc, FSP_CMD_TIMEOUT2);
		/* expect 0xfe */
	} else {
		/* swapping isn't necessary */
		ps2_sendbyte(ps2dev, 0x66, FSP_CMD_TIMEOUT2);
		/* expect 0xfe */
	}
	/* should return 0xfc(failed) */
	ps2_sendbyte(ps2dev, addr, FSP_CMD_TIMEOUT);

	if (__ps2_command(ps2dev, param, PSMOUSE_CMD_GETINFO) < 0)
		goto out;

	*reg_val = param[2];
	rc = 0;

 out:
	mutex_unlock(&ps2dev->cmd_mutex);
	ps2_command(ps2dev, NULL, PSMOUSE_CMD_ENABLE);
	psmouse_set_state(psmouse, PSMOUSE_ACTIVATED);
	dev_dbg(&ps2dev->serio->dev, "READ REG: 0x%02x is 0x%02x (rc = %d)\n",
		reg_addr, *reg_val, rc);
	return rc;
}

static int fsp_reg_write(struct psmouse *psmouse, int reg_addr, int reg_val)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char v;
	int rc = -1;

	mutex_lock(&ps2dev->cmd_mutex);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	if ((v = fsp_test_invert_cmd(reg_addr)) != reg_addr) {
		/* inversion is required */
		ps2_sendbyte(ps2dev, 0x74, FSP_CMD_TIMEOUT2);
	} else {
		if ((v = fsp_test_swap_cmd(reg_addr)) != reg_addr) {
			/* swapping is required */
			ps2_sendbyte(ps2dev, 0x77, FSP_CMD_TIMEOUT2);
		} else {
			/* swapping isn't necessary */
			ps2_sendbyte(ps2dev, 0x55, FSP_CMD_TIMEOUT2);
		}
	}
	/* write the register address in correct order */
	ps2_sendbyte(ps2dev, v, FSP_CMD_TIMEOUT2);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		return -1;

	if ((v = fsp_test_invert_cmd(reg_val)) != reg_val) {
		/* inversion is required */
		ps2_sendbyte(ps2dev, 0x47, FSP_CMD_TIMEOUT2);
	} else if ((v = fsp_test_swap_cmd(reg_val)) != reg_val) {
		/* swapping is required */
		ps2_sendbyte(ps2dev, 0x44, FSP_CMD_TIMEOUT2);
	} else {
		/* swapping isn't necessary */
		ps2_sendbyte(ps2dev, 0x33, FSP_CMD_TIMEOUT2);
	}

	/* write the register value in correct order */
	ps2_sendbyte(ps2dev, v, FSP_CMD_TIMEOUT2);
	rc = 0;

 out:
	mutex_unlock(&ps2dev->cmd_mutex);
	dev_dbg(&ps2dev->serio->dev, "WRITE REG: 0x%02x to 0x%02x (rc = %d)\n",
		reg_addr, reg_val, rc);
	return rc;
}

/* enable register clock gating for writing certain registers */
static int fsp_reg_write_enable(struct psmouse *psmouse, int en)
{
	int v, nv;

	if (fsp_reg_read(psmouse, FSP_REG_SYSCTL1, &v) == -1)
		return -1;

	if (en)
		nv = v | FSP_BIT_EN_REG_CLK;
	else
		nv = v & ~FSP_BIT_EN_REG_CLK;

	/* only write if necessary */
	if (nv != v)
		if (fsp_reg_write(psmouse, FSP_REG_SYSCTL1, nv) == -1)
			return -1;

	return 0;
}

static int fsp_page_reg_read(struct psmouse *psmouse, int *reg_val)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char param[3];
	int rc = -1;

	ps2_command(ps2dev, NULL, PSMOUSE_CMD_DISABLE);
	psmouse_set_state(psmouse, PSMOUSE_CMD_MODE);
	mutex_lock(&ps2dev->cmd_mutex);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	ps2_sendbyte(ps2dev, 0x66, FSP_CMD_TIMEOUT2);
	ps2_sendbyte(ps2dev, 0x88, FSP_CMD_TIMEOUT2);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	ps2_sendbyte(ps2dev, 0x83, FSP_CMD_TIMEOUT2);
	ps2_sendbyte(ps2dev, 0x88, FSP_CMD_TIMEOUT2);

	/* get the returned result */
	if (__ps2_command(ps2dev, param, PSMOUSE_CMD_GETINFO))
		goto out;

	*reg_val = param[2];
	rc = 0;

 out:
	mutex_unlock(&ps2dev->cmd_mutex);
	ps2_command(ps2dev, NULL, PSMOUSE_CMD_ENABLE);
	psmouse_set_state(psmouse, PSMOUSE_ACTIVATED);
	dev_dbg(&ps2dev->serio->dev, "READ PAGE REG: 0x%02x (rc = %d)\n",
		*reg_val, rc);
	return rc;
}

static int fsp_page_reg_write(struct psmouse *psmouse, int reg_val)
{
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char v;
	int rc = -1;

	mutex_lock(&ps2dev->cmd_mutex);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		goto out;

	ps2_sendbyte(ps2dev, 0x38, FSP_CMD_TIMEOUT2);
	ps2_sendbyte(ps2dev, 0x88, FSP_CMD_TIMEOUT2);

	if (ps2_sendbyte(ps2dev, 0xf3, FSP_CMD_TIMEOUT) < 0)
		return -1;

	if ((v = fsp_test_invert_cmd(reg_val)) != reg_val) {
		ps2_sendbyte(ps2dev, 0x47, FSP_CMD_TIMEOUT2);
	} else if ((v = fsp_test_swap_cmd(reg_val)) != reg_val) {
		/* swapping is required */
		ps2_sendbyte(ps2dev, 0x44, FSP_CMD_TIMEOUT2);
	} else {
		/* swapping isn't necessary */
		ps2_sendbyte(ps2dev, 0x33, FSP_CMD_TIMEOUT2);
	}

	ps2_sendbyte(ps2dev, v, FSP_CMD_TIMEOUT2);
	rc = 0;

 out:
	mutex_unlock(&ps2dev->cmd_mutex);
	dev_dbg(&ps2dev->serio->dev, "WRITE PAGE REG: to 0x%02x (rc = %d)\n",
		reg_val, rc);
	return rc;
}

static int fsp_device_id(struct psmouse *psmouse)
{
	int id;

	if (fsp_reg_read(psmouse, FSP_REG_DEVICE_ID, &id))
		return -1;

	return id;
}

static int fsp_get_version(struct psmouse *psmouse)
{
	int ver;

	if (fsp_reg_read(psmouse, FSP_REG_VERSION, &ver))
		return -1;

	return ver;
}

static int fsp_get_revision(struct psmouse *psmouse)
{
	int rev;

	if (fsp_reg_read(psmouse, FSP_REG_REVISION, &rev))
		return -1;

	return rev;
}

static int fsp_get_buttons(struct psmouse *psmouse)
{
	static const int buttons[] = {
		0x16, /* Left/Middle/Right/Forward/Backward & Scroll Up/Down */
		0x06, /* Left/Middle/Right & Scroll Up/Down/Right/Left */
		0x04, /* Left/Middle/Right & Scroll Up/Down */
		0x02, /* Left/Middle/Right */
	};
	int val;

	if (fsp_reg_read(psmouse, FSP_REG_TMOD_STATUS1, &val) == -1)
		return -1;

	return buttons[(val & 0x30) >> 4];
}

/** enable on-pad command tag output */
static int fsp_opc_tag_enable(struct psmouse *psmouse, int en)
{
	int v, nv;
	int res = 0;

	if (fsp_reg_read(psmouse, FSP_REG_OPC_QDOWN, &v) == -1) {
		dev_err(&psmouse->ps2dev.serio->dev, "Unable get OPC state.\n");
		return -1;
	}

	if (en)
		nv = v | FSP_BIT_EN_OPC_TAG;
	else
		nv = v & ~FSP_BIT_EN_OPC_TAG;

	/* only write if necessary */
	if (nv != v) {
		fsp_reg_write_enable(psmouse, 1);
		res = fsp_reg_write(psmouse, FSP_REG_OPC_QDOWN, nv);
		fsp_reg_write_enable(psmouse, 0);
	}

	if (res != 0)
		dev_err(&psmouse->ps2dev.serio->dev,
			"Unable to enable OPC tag.\n");

	return res;
}

/**
 * set packet format based on the number of buttons current device has
 */
static int fsp_set_packet_format(struct psmouse *psmouse)
{
	struct fsp_data *ad = psmouse->private;
	struct ps2dev *ps2dev = &psmouse->ps2dev;
	unsigned char param[2];
	int val;

	/*
	 * standard procedure to enter FSP Intellimouse mode
	 * (scrolling wheel, 4th and 5th buttons)
	 */
	param[0] = 200;
	ps2_command(ps2dev, param, PSMOUSE_CMD_SETRATE);
	param[0] = 200;
	ps2_command(ps2dev, param, PSMOUSE_CMD_SETRATE);
	param[0] =  80;
	ps2_command(ps2dev, param, PSMOUSE_CMD_SETRATE);
	ps2_command(ps2dev, param, PSMOUSE_CMD_GETID);
	if (param[0] != 0x04) {
		dev_err(&psmouse->ps2dev.serio->dev,
			"Unable to enable 4 bytes packet.\n");
		psmouse->pktsize = 3;
		return -1;
	}
	psmouse->pktsize = 4;

	if (ad->buttons == 0x06) {
		/* Left/Middle/Right & Scroll Up/Down/Right/Left */
		fsp_reg_read(psmouse, FSP_REG_SYSCTL5, &val);
		val &= ~(FSP_BIT_EN_MSID7 | FSP_BIT_EN_MSID8 | FSP_BIT_EN_AUTO_MSID8);
		val |= FSP_BIT_EN_MSID6;
		if (fsp_reg_write(psmouse, FSP_REG_SYSCTL5, val)) {
			dev_err(&psmouse->ps2dev.serio->dev,
				"Unable to enable MSID6 mode.\n");
			return -1;
		}
	}

	/*
	 * enable OPC tags such that driver can tell the difference between
	 * on-pad and real button click
	 */
	return fsp_opc_tag_enable(psmouse, 1);
}

static int fsp_onpad_vscr(struct psmouse *psmouse, int enable)
{
	struct fsp_data *ad = psmouse->private;
	struct fsp_hw_state *state = &ad->hw_state;
	int val;

	if (fsp_reg_read(psmouse, FSP_REG_ONPAD_CTL, &val))
		return -1;

	state->onpad_vscroll = (enable == 0) ? 0 : 1;

	if (enable)
		val |= (FSP_BIT_FIX_VSCR | FSP_BIT_ONPAD_ENABLE);
	else
		val &= (0xff ^ FSP_BIT_FIX_VSCR);

	if (fsp_reg_write(psmouse, FSP_REG_ONPAD_CTL, val))
		return -1;

	return 0;
}

static int fsp_onpad_hscr(struct psmouse *psmouse, int enable)
{
	struct fsp_data *ad = psmouse->private;
	struct fsp_hw_state *state = &ad->hw_state;
	int val, v2;

	if (fsp_reg_read(psmouse, FSP_REG_ONPAD_CTL, &val))
		return -1;

	if (fsp_reg_read(psmouse, FSP_REG_SYSCTL5, &v2))
		return -1;

	state->onpad_hscroll = (enable == 0) ? 0 : 1;

	if (enable) {
		val |= (FSP_BIT_FIX_HSCR | FSP_BIT_ONPAD_ENABLE);
		v2 |= FSP_BIT_EN_MSID6;
	} else {
		val &= (0xff ^ FSP_BIT_FIX_HSCR);
		v2 &= ~(FSP_BIT_EN_MSID6 | FSP_BIT_EN_MSID7 | FSP_BIT_EN_MSID8);
	}

	if (fsp_reg_write(psmouse, FSP_REG_ONPAD_CTL, val))
		return -1;

	/* reconfigure horizontal scrolling packet output */
	if (fsp_reg_write(psmouse, FSP_REG_SYSCTL5, v2))
		return -1;

	return 0;
}

static int fsp_onpad_icon(struct psmouse *psmouse, int enable)
{
	struct fsp_data *ad = psmouse->private;
	struct fsp_hw_state *state = &ad->hw_state;
	int val;

#ifdef	notyet
	/* switch to register page 1, where icon switch button registers are */
	fsp_reg_read(psmouse, FSP_REG_PAGE_CTRL, &val);
	val |= 0x01;
	if (fsp_reg_write(psmouse, FSP_REG_PAGE_CTRL, val))
		return -1;

	fsp_reg_write_enable(psmouse, 1);
	/* set position of icon switch button */
	/*
	 *   XL		XH
	 *   YL+--------+
	 *     | sw_btn |	POSITION UNIT IS IN 0.5 SCANLINE
	 *   YH+--------+
	 */
	if (fsp_reg_write(psmouse, FSP_REG_OPTZ_YHI, 0))
		val = -1;
	if (fsp_reg_write(psmouse, FSP_REG_OPTZ_YLO, 6))
		val = -1;
	if (fsp_reg_write(psmouse, FSP_REG_OPTZ_XHI, 0))
		val = -1;
	if (fsp_reg_write(psmouse, FSP_REG_OPTZ_XLO, 6 | 0x80))
		val = -1;
	fsp_reg_write_enable(psmouse, 0);
	if (val == -1)
		return -1;

	/* switch back to register page 0 */
	fsp_reg_read(psmouse, FSP_REG_PAGE_CTRL, &val);
	val &= ~0x01;
	if (fsp_reg_write(psmouse, FSP_REG_PAGE_CTRL, val))
		return -1;
#endif
	/* enable icon switch button and absolute packet */
	fsp_reg_read(psmouse, FSP_REG_SYSCTL5, &val);
	val &= ~(FSP_BIT_EN_MSID7 | FSP_BIT_EN_MSID8 | FSP_BIT_EN_AUTO_MSID8);

	if (enable) {
		val |= (FSP_BIT_EN_MSID8 | FSP_BIT_EN_PKT_G0);
		state->onpad_icon = 1;
		state->abs_pkt = 1;
	} else {
		state->onpad_icon = 0;
		state->abs_pkt = 0;
	}

	if (fsp_reg_write(psmouse, FSP_REG_SYSCTL5, val))
		return -1;

	return 0;
}

static ssize_t fsp_attr_show_setreg(struct psmouse *psmouse,
					void *data, char *buf)
{
	/* do nothing */
	return 0;
}

/*
 * Write device specific initial parameters.
 *
 * ex: 0xab 0xcd - write oxcd into register 0xab
 */
static ssize_t fsp_attr_set_setreg(struct psmouse *psmouse, void *data,
				   const char *buf, size_t count)
{
	unsigned long reg, val;
	char *rest;
	ssize_t retval;

	reg = simple_strtoul(buf, &rest, 16);
	if (rest == buf || *rest != ' ' || reg > 0xff)
		return -EINVAL;

	if (strict_strtoul(rest + 1, 16, &val) || val > 0xff)
		return -EINVAL;

	if (fsp_reg_write_enable(psmouse, 1))
		return -EIO;

	retval = fsp_reg_write(psmouse, reg, val) < 0 ? -EIO : count;

	fsp_reg_write_enable(psmouse, 0);

	return count;
}

PSMOUSE_DEFINE_ATTR(setreg, S_IWUSR, NULL,
		fsp_attr_show_setreg, fsp_attr_set_setreg);

static ssize_t fsp_attr_show_getreg(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%02x%02x\n", ad->last_reg, ad->last_val);
}

/*
 * Read a register from device.
 *
 * ex: 0xab -- read content from register 0xab
 */
static ssize_t fsp_attr_set_getreg(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	unsigned long reg;
	int val;

	if (strict_strtoul(buf, 16, &reg) || reg > 0xff)
		return -EINVAL;

	if (fsp_reg_read(psmouse, reg, &val))
		return -EIO;

	ad->last_reg = reg;
	ad->last_val = val;

	return count;
}

PSMOUSE_DEFINE_ATTR(getreg, S_IWUSR | S_IRUGO, NULL,
			fsp_attr_show_getreg, fsp_attr_set_getreg);

static ssize_t fsp_attr_show_pagereg(struct psmouse *psmouse,
					void *data, char *buf)
{
	int val = 0;

	if (fsp_page_reg_read(psmouse, &val))
		return -EIO;

	return sprintf(buf, "%02x\n", val);
}

static ssize_t fsp_attr_set_pagereg(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	unsigned long val;

	if (strict_strtoul(buf, 16, &val) || val > 0xff)
		return -EINVAL;

	if (fsp_page_reg_write(psmouse, val))
		return -EIO;

	return count;
}

PSMOUSE_DEFINE_ATTR(page, S_IWUSR | S_IRUGO, NULL,
			fsp_attr_show_pagereg, fsp_attr_set_pagereg);

static ssize_t fsp_attr_show_vscroll(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%d\n", ad->hw_state.onpad_vscroll ? 1 : 0);
}

static ssize_t fsp_attr_set_vscroll(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	unsigned long val;

	if (strict_strtoul(buf, 10, &val) || val > 1)
		return -EINVAL;

	ad->hw_state.onpad_vscroll = val;
	fsp_onpad_vscr(psmouse, val);

	return count;
}

PSMOUSE_DEFINE_ATTR(vscroll, S_IWUSR | S_IRUGO | S_IWUGO, NULL,
			fsp_attr_show_vscroll, fsp_attr_set_vscroll);

static ssize_t fsp_attr_show_hscroll(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%d\n", ad->hw_state.onpad_hscroll ? 1 : 0);
}

static ssize_t fsp_attr_set_hscroll(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	unsigned long val;

	if (strict_strtoul(buf, 10, &val) || val > 1)
		return -EINVAL;

	ad->hw_state.onpad_hscroll = val;
	fsp_onpad_hscr(psmouse, val);

	return count;
}

PSMOUSE_DEFINE_ATTR(hscroll, S_IWUSR | S_IRUGO | S_IWUGO, NULL,
			fsp_attr_show_hscroll, fsp_attr_set_hscroll);

static ssize_t fsp_attr_show_onpadicon(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%d\n", ad->hw_state.onpad_icon ? 1 : 0);
}

static ssize_t fsp_attr_set_onpadicon(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	unsigned long val;

	if (strict_strtoul(buf, 10, &val) || val > 1)
		return -EINVAL;

	ad->hw_state.onpad_icon = val;
	fsp_onpad_icon(psmouse, val);

	return count;
}

PSMOUSE_DEFINE_ATTR(onpadicon, S_IWUSR | S_IRUGO, NULL,
			fsp_attr_show_onpadicon, fsp_attr_set_onpadicon);

static ssize_t fsp_attr_show_pktfmt(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%d\n", ad->hw_state.pkt_fmt);
}

static ssize_t fsp_attr_set_pktfmt(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	unsigned long val;

	if (strict_strtoul(buf, 10, &val) || val > 2)
		return -EINVAL;

	ad->hw_state.pkt_fmt = val;
	/* TODO need full G0/A0 abs. packet setup */

	return count;
}

PSMOUSE_DEFINE_ATTR(pktfmt, S_IWUSR | S_IRUGO, NULL,
			fsp_attr_show_pktfmt, fsp_attr_set_pktfmt);

static ssize_t fsp_attr_show_flags(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%c%c%c%c%c%c\n",
		ad->flags & FSPDRV_FLAG_OPICON_KEY ? 'K' : 'k',
		ad->flags & FSPDRV_FLAG_OPICON_BTN ? 'B' : 'b',
		ad->flags & FSPDRV_FLAG_REVERSE_X ? 'X' : 'x',
		ad->flags & FSPDRV_FLAG_REVERSE_Y ? 'Y' : 'y',
		ad->flags & FSPDRV_FLAG_AUTO_SWITCH ? 'A' : 'a',
		ad->flags & FSPDRV_FLAG_EN_OPC ? 'C' : 'c');
}

static ssize_t fsp_attr_set_flags(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;
	size_t i;

	for (i = 0; i < count; i++) {
		switch (buf[i]) {
		case 'B':
			ad->flags |= FSPDRV_FLAG_OPICON_BTN;
			break;
		case 'b':
			ad->flags &= ~FSPDRV_FLAG_OPICON_BTN;
			break;
		case 'K':
			ad->flags |= FSPDRV_FLAG_OPICON_KEY;
			break;
		case 'k':
			ad->flags &= ~FSPDRV_FLAG_OPICON_KEY;
			break;
		case 'X':
			ad->flags |= FSPDRV_FLAG_REVERSE_X;
			break;
		case 'x':
			ad->flags &= ~FSPDRV_FLAG_REVERSE_X;
			break;
		case 'Y':
			ad->flags |= FSPDRV_FLAG_REVERSE_Y;
			break;
		case 'y':
			ad->flags &= ~FSPDRV_FLAG_REVERSE_Y;
			break;
		case 'A':
			ad->flags |= FSPDRV_FLAG_AUTO_SWITCH;
			break;
		case 'a':
			ad->flags &= ~FSPDRV_FLAG_AUTO_SWITCH;
			break;
		case 'C':
			ad->flags |= FSPDRV_FLAG_EN_OPC;
			break;
		case 'c':
			ad->flags &= ~FSPDRV_FLAG_EN_OPC;
			break;
		case 'R':
		case 'r':
			/* hack: reset pad */
#ifdef	FSP_DEBUG
			dev_dbg(&psmouse->ps2dev.serio->dev,
				"Resetting FSP...\n");
#endif
			/* disable on-pad vertical scrolling */
			fsp_onpad_vscr(psmouse, 0);
			/* disable on-pad horizontal scrolling */
			fsp_onpad_hscr(psmouse, 0);

			/*
			 * disable on-pad switching icon and absolute packet
			 * mode
			 */
			fsp_onpad_icon(psmouse, 0);
			/* reload custom initial parameters */
			fsp_reset(psmouse);
			/* re-initialise output packet format */
			fsp_set_packet_format(psmouse);
			break;
		default:
			return -EINVAL;
		}
	}
	return count;
}

PSMOUSE_DEFINE_ATTR(flags, S_IWUSR | S_IRUGO | S_IWUGO, NULL,
			fsp_attr_show_flags, fsp_attr_set_flags);

static ssize_t fsp_attr_show_ver(struct psmouse *psmouse,
					void *data, char *buf)
{
	return sprintf(buf, "Sentelic FSP kernel module %s\n", fsp_drv_ver);
}

static ssize_t fsp_attr_set_ver(struct psmouse *psmouse, void *data,
				const char *buf, size_t count)
{
	/* do nothing */
	return 0;
}

PSMOUSE_DEFINE_ATTR(ver, S_IRUGO, NULL,
			fsp_attr_show_ver, fsp_attr_set_ver);

static ssize_t fsp_attr_show_accel(struct psmouse *psmouse,
					void *data, char *buf)
{
	struct fsp_data *ad = psmouse->private;

	return sprintf(buf, "%d %d %d\n",
		ad->accel_num, ad->accel_denom, ad->accel_threshold);
}

static ssize_t fsp_attr_set_accel(struct psmouse *psmouse, void *data,
					const char *buf, size_t count)
{
	struct fsp_data *ad = psmouse->private;

	sscanf(buf, "%d %d %d",
		&ad->accel_num, &ad->accel_denom, &ad->accel_threshold);

	/* sanity check */
	if (ad->accel_num <= 0)
		ad->accel_num = DEFAULT_ACCEL_NUM;

	/* prevent dividing by zero */
	if (ad->accel_denom <= 0)
		ad->accel_denom = DEFAULT_ACCEL_DENOM;

	if (ad->accel_threshold <= 0)
		ad->accel_threshold = DEFAULT_ACCEL_THRESHOLD;

	return count;
}

PSMOUSE_DEFINE_ATTR(accel, S_IWUSR | S_IRUGO | S_IWUGO, NULL,
			fsp_attr_show_accel, fsp_attr_set_accel);

int fsp_reset(struct psmouse *psmouse)
{
	/* TODO: reset initial parameters */
	return 0;
}

static int fsp_reconnect(struct psmouse *psmouse)
{
	int version;

	if (fsp_detect(psmouse, 0) < 0)
		return -1;

	if ((version = fsp_get_version(psmouse)) < 0)
		return -1;

	fsp_reset(psmouse);

	return 0;
}

static struct attribute *fsp_attributes[] = {
	&psmouse_attr_setreg.dattr.attr,
	&psmouse_attr_getreg.dattr.attr,
	&psmouse_attr_page.dattr.attr,
	&psmouse_attr_vscroll.dattr.attr,
	&psmouse_attr_hscroll.dattr.attr,
	&psmouse_attr_onpadicon.dattr.attr,
	&psmouse_attr_pktfmt.dattr.attr,
	&psmouse_attr_flags.dattr.attr,
	&psmouse_attr_ver.dattr.attr,
	&psmouse_attr_accel.dattr.attr,
	NULL
};

static struct attribute_group fsp_attribute_group = {
	.attrs = fsp_attributes,
};

static void fsp_disconnect(struct psmouse *psmouse)
{
	sysfs_remove_group(&psmouse->ps2dev.serio->dev.kobj,
			   &fsp_attribute_group);
	fsp_opc_tag_enable(psmouse, 0);
	fsp_reset(psmouse);
	kfree(psmouse->private);
}

int fsp_detect(struct psmouse *psmouse, int set_properties)
{
	int error;

	if (fsp_device_id(psmouse) != 0x01)
		return -1;

	if (set_properties) {
		psmouse->vendor = "Sentelic";
		psmouse->name = "FingerSensingPad";

		error = sysfs_create_group(&psmouse->ps2dev.serio->dev.kobj,
					   &fsp_attribute_group);
		if (error) {
			dev_err(&psmouse->ps2dev.serio->dev,
				"Failed to create sysfs attributes (%d)",
				error);
			return -1;
		}
	}

	return 0;
}

static void fsp_set_input_params(struct psmouse *psmouse)
{
	struct fsp_data *ad = psmouse->private;
	struct fsp_hw_state *state = &ad->hw_state;

	if (state->abs_pkt == 0) {
		__set_bit(BTN_MIDDLE, psmouse->dev->keybit);
		__set_bit(REL_WHEEL, psmouse->dev->relbit);
		__set_bit(REL_HWHEEL, psmouse->dev->relbit);

		__set_bit(EV_REL, psmouse->dev->evbit);
		__set_bit(REL_X, psmouse->dev->relbit);
		__set_bit(REL_Y, psmouse->dev->relbit);

		__set_bit(BTN_BACK, psmouse->dev->keybit);
		__set_bit(BTN_FORWARD, psmouse->dev->keybit);

		__clear_bit(EV_ABS, psmouse->dev->evbit);
		__clear_bit(BTN_SIDE, psmouse->dev->keybit);
		__clear_bit(BTN_EXTRA, psmouse->dev->keybit);
	} else {
		/* enable absolute packet mode */
		__set_bit(EV_ABS, psmouse->dev->evbit);

		input_set_abs_params(psmouse->dev, ABS_X, 0, 1023, 0, 0);
		input_set_abs_params(psmouse->dev, ABS_Y, 0, 767, 0, 0);

		/* no more relative coordinates */
		__clear_bit(EV_REL, psmouse->dev->evbit);
		__clear_bit(REL_X, psmouse->dev->relbit);
		__clear_bit(REL_Y, psmouse->dev->relbit);
	}
}

#ifdef FSP_DEBUG
static void fsp_packet_debug(unsigned char packet[])
{
	static unsigned int ps2_packet_cnt;
	static unsigned int ps2_last_second;
	unsigned int jiffies_msec;

	ps2_packet_cnt++;
	jiffies_msec = jiffies_to_msecs(jiffies);
	printk(KERN_DEBUG "%08dms PS/2 packets: %02x, %02x, %02x, %02x\n",
		jiffies_msec, packet[0], packet[1], packet[2], packet[3]);

	if (jiffies_msec - ps2_last_second > 1000) {
		printk(KERN_DEBUG "PS/2 packets/sec = %d\n", ps2_packet_cnt);
		ps2_packet_cnt = 0;
		ps2_last_second = jiffies_msec;
	}
}
#else
static void fsp_packet_debug(unsigned char packet[])
{
}
#endif

#if	LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
static psmouse_ret_t fsp_process_byte(struct psmouse *psmouse,
				      struct pt_regs *regs)
#else
static psmouse_ret_t fsp_process_byte(struct psmouse *psmouse)
#endif
{
	struct input_dev *dev = psmouse->dev;
	struct fsp_data *ad = psmouse->private;
	unsigned char *packet = psmouse->packet;
	unsigned char button_status = 0, lscroll = 0, rscroll = 0;
	static unsigned short prev_abs_x, prev_abs_y;
	unsigned short abs_x, abs_y;
	int rel_x, rel_y;

	if (psmouse->pktcnt < 4)
		return PSMOUSE_GOOD_DATA;

	/*
	 * Full packet accumulated, process it
	 */

	switch (psmouse->packet[0] >> FSP_PKT_TYPE_SHIFT) {
	case FSP_PKT_TYPE_ABS:
		abs_x = (packet[1] << 2) | ((packet[3] >> 2) & 0x03);
		abs_y = (packet[2] << 2) | (packet[3] & 0x03);

		if (abs_x && abs_y) {
			/* no X/Y directional reversal when finger is up */
			if (ad->flags & FSPDRV_FLAG_REVERSE_X)
				abs_x = 1023 - abs_x;
			if (ad->flags & FSPDRV_FLAG_REVERSE_Y)
				abs_y = 767 - abs_y;
			prev_abs_x = abs_x;
			prev_abs_y = abs_y;
		}

		if (ad->flags & (FSPDRV_FLAG_OPICON_BTN | FSPDRV_FLAG_OPICON_KEY)) {
			/* do nothing */
		} else {
			input_report_key(dev, BTN_LEFT, packet[0] & 1);
			input_report_key(dev, BTN_MIDDLE, (packet[0] >> 2) & 1);
			input_report_key(dev, BTN_RIGHT, (packet[0] >> 1) & 1);
#if	0
			/* vscroll down */
			input_report_key(dev, BTN_BACK, (packet[3] >> 5) & 1);
			/* vscroll up */
			input_report_key(dev, BTN_FORWARD, (packet[3] >> 4) & 1);
			/* rscroll */
			input_report_key(dev, BTN_RIGHT, (packet[3] >> 7) & 1);
			/* lscroll */
			input_report_key(dev, BTN_LEFT, (packet[3] >> 6) & 1);
#endif
		}
		input_report_abs(dev, ABS_X, prev_abs_x);
		input_report_abs(dev, ABS_Y, prev_abs_y);
		break;

	case FSP_PKT_TYPE_NORMAL_OPC:
		/* on-pad click, filter it if necessary */
		if ((ad->flags & FSPDRV_FLAG_EN_OPC) != FSPDRV_FLAG_EN_OPC)
			packet[0] &= ~BIT(0);
		/* fall through */

	case FSP_PKT_TYPE_NORMAL:
		/* normal packet */
		/* special packet data translation from on-pad packets */
		if (packet[3] != 0) {
			if (packet[3] & BIT(0))
				button_status |= 0x01;	/* wheel down */
			if (packet[3] & BIT(1))
				button_status |= 0x0f;	/* wheel up */
			if (packet[3] & BIT(2))
				button_status |= BIT(5);/* horizontal left */
			if (packet[3] & BIT(3))
				button_status |= BIT(4);/* horizontal right */
			/* push back to packet queue */
			if (button_status != 0)
				packet[3] = button_status;
			rscroll = (packet[3] >> 4) & 1;
			lscroll = (packet[3] >> 5) & 1;
		}
		/*
		 * Processing wheel up/down and extra button events
		 */
		input_report_rel(dev, REL_WHEEL,
				 (int)(packet[3] & 8) - (int)(packet[3] & 7));
		input_report_rel(dev, REL_HWHEEL, lscroll - rscroll);
		input_report_key(dev, BTN_BACK, lscroll);
		input_report_key(dev, BTN_FORWARD, rscroll);

		/*
		 * Generic PS/2 Mouse
		 */
		input_report_key(dev, BTN_LEFT, packet[0] & 1);
		input_report_key(dev, BTN_MIDDLE, (packet[0] >> 2) & 1);
		input_report_key(dev, BTN_RIGHT, (packet[0] >> 1) & 1);

		/* perform acceleration */
		rel_x = packet[1] ? (int)packet[1] - (int)((packet[0] << 4) & 0x100) : 0;
		rel_y = packet[2] ? (int)((packet[0] << 3) & 0x100) - (int)packet[2] : 0;

		if (abs(rel_x) > ad->accel_threshold)
			rel_x = rel_x * ad->accel_num / ad->accel_denom;
		if (abs(rel_y) > ad->accel_threshold)
			rel_y = rel_y * ad->accel_num / ad->accel_denom;
		input_report_rel(dev, REL_X, rel_x);
		input_report_rel(dev, REL_Y, rel_y);
		break;
	}

	input_sync(dev);

	fsp_packet_debug(packet);

	return PSMOUSE_FULL_PACKET;
}

int fsp_init(struct psmouse *psmouse)
{
	struct fsp_data *priv;
	int ver;

	if ((ver = fsp_get_version(psmouse)) < 0)
		return -1;

	priv = kzalloc(sizeof(struct fsp_data), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	psmouse->private = priv;

	priv->ver = ver;
	priv->rev = fsp_get_revision(psmouse);
	priv->buttons = fsp_get_buttons(psmouse);

	/* enable on-pad click by default */
	priv->flags |= FSPDRV_FLAG_EN_OPC;

	switch (priv->buttons) {
	case 0x06:
		priv->hw_state.btn_fbbb = 0;
		priv->hw_state.btn_slsr = 1;
		break;
	case 0x16:
		priv->hw_state.btn_fbbb = 1;
		priv->hw_state.btn_slsr = 0;
		break;
	default:
		priv->hw_state.btn_fbbb = 0;
		priv->hw_state.btn_slsr = 0;
		break;
	}
	psmouse->protocol_handler = fsp_process_byte;
	psmouse->disconnect = fsp_disconnect;
	psmouse->reconnect = fsp_reconnect;

	/* report hardware information */
	printk(KERN_INFO
		"Finger Sensing Pad, hw: %d.%d.%d, sw: %s, buttons: %d\n",
		(priv->ver >> 4), (priv->ver & 0x0F), priv->rev,
		fsp_drv_ver, priv->buttons & 7);

	/* set default acceleration parameters */
	priv->accel_num = DEFAULT_ACCEL_NUM;
	priv->accel_denom = DEFAULT_ACCEL_DENOM;
	priv->accel_threshold = DEFAULT_ACCEL_THRESHOLD;

	/* set default packet output based on number of buttons we found */
	fsp_set_packet_format(psmouse);

	/* disable on-pad vertical scrolling */
	fsp_onpad_vscr(psmouse, 0);

	/* disable on-pad horizontal scrolling */
	fsp_onpad_hscr(psmouse, 0);

	/* disable on-pad switching icon and absolute packet mode */
	fsp_onpad_icon(psmouse, 0);

	/* set various supported input event bits */
	fsp_set_input_params(psmouse);

	return 0;
}
