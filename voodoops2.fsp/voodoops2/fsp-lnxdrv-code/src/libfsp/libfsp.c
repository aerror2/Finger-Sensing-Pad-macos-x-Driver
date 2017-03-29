/**
 * @file libfsp.c Library for communicating with Finger Sensing Pad PS/2
 * kernel driver.
 *
 * Copyright (C) 2008-2010 Tai-hwa Liang, Sentelic Corporation.
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
 * $Id: libfsp.c 47088 2010-07-08 02:19:51Z avatar $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sentelic.h"
#include "fsp.h"

#define	FSP_SYSFS_NODE_VER		"ver"
#define	FSP_SYSFS_NODE_SETREG		"setreg"
#define	FSP_SYSFS_NODE_SETREG_WLEN	(5)
#define	FSP_SYSFS_NODE_GETREG		"getreg"
#define	FSP_SYSFS_NODE_GETREG_WLEN	(2)
#define	FSP_SYSFS_NODE_HSCR		"hscroll"
#define	FSP_SYSFS_NODE_HSCR_WLEN	(2)
#define	FSP_SYSFS_NODE_VSCR		"vscroll"
#define	FSP_SYSFS_NODE_VSCR_WLEN	(2)
#define	FSP_SYSFS_NODE_PKTFMT		"pktfmt"
#define	FSP_SYSFS_NODE_PKTFMT_WLEN	(2)
#define	FSP_SYSFS_NODE_PS2		"ps2"
#define	FSP_SYSFS_NODE_PS2_WLEN		(4)
#define	FSP_SYSFS_NODE_FLAGS		"flags"
#define	FSP_SYSFS_NODE_PAGE		"page"
#define	FSP_SYSFS_NODE_PAGE_WLEN	(2)
#define	FSP_SYSFS_NODE_ACCEL		"accel"

#define	FSP_MAX_PKT_FMT_ID		2

extern int	errno;

static int
fsp_sysfs_write_str(const char *node, const char *in, size_t in_len)
{
	FILE	*fp;

	/* sanity check */
	if ((node == NULL) || (in == NULL)) {
		return (EINVAL);
	}
	if ((fp = fopen(node, "rb+")) == NULL) {
		return (errno);
	}
	fwrite(in, in_len, 1, fp);

	fclose(fp);

	return (0);
}

static int
fsp_sysfs_read_str(const char *node, char *out, size_t out_len)
{
	FILE	*fp;
	int	rlen;

	/* sanity check */
	if ((node == NULL) || (out == NULL)) {
		return (EINVAL);
	}
	if ((fp = fopen(node, "rb")) == NULL) {
		return (errno);
	}
	rlen = fread(out, 1, out_len, fp);
	out[rlen - 1] = '\0';	/* enforce NULL termination */

	return fclose(fp);
}

void *
fsp_open_dev(void)
{
#define	I8042_ROOT_PATH	"/sys/devices/platform/i8042"
	struct stat	st;
	char		buf[128];
	int		i;

	for (i = 0; i < 32; i++) {
		snprintf(buf, sizeof(buf), "%s/serio%d/vscroll",
			I8042_ROOT_PATH, i);
		if (stat(buf, &st) == 0) {
			snprintf(buf, sizeof(buf), "%s/serio%d",
				I8042_ROOT_PATH, i);
			return strdup(buf);
		}
	}
	return (NULL);
#undef	I8042_ROOT_PATH
}

void
fsp_free_dev(void *handle)
{
	free(handle);
}

int
fsp_get_ver(void *handle, char *ver, size_t ver_len)
{
	char	buf[64], tmp[64];
	int	ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_VER, (char *)handle);

	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (ver != NULL)) {
		strncpy(ver, tmp, ver_len);
	}
	return (ret);
}

int
fsp_set_regs(void *handle, int *addrs, int *vals, size_t len)
{
	int	i, rc = 0;

	for (i = 0; i < len; i++) {
		rc = fsp_set_reg(handle, addrs[i], vals[i]);
		if (rc != 0)
			break;
	}
	return (rc);
}

int
fsp_set_reg(void *handle, int addr, int val)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_SETREG_WLEN + 1];

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_SETREG, (char *)handle);
	/* compose value string */
	snprintf(tmp, sizeof(tmp), "%02x %02x", addr, val);

	return fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_SETREG_WLEN);
}

int
fsp_get_reg(void *handle, int addr, int *val)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_SETREG_WLEN + 1];
	int	v1, v2, ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_GETREG, (char *)handle);
	/* compose request string */
	snprintf(tmp, sizeof(tmp), "%02x", addr);

	/* send command */
	ret = fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_GETREG_WLEN);
	if (ret != 0) {
		return (ret);
	}
	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (val != NULL)) {
		sscanf(tmp, "%02x%02x", &v1, &v2);
		*val = v2;			/* ignore address byte */
	}
	return (ret);
}

int
fsp_set_vscr(void *handle, int en)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_VSCR_WLEN + 1];

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_VSCR, (char *)handle);
	/* compose value string */
	snprintf(tmp, sizeof(tmp), "%02x", (en == 0) ? 0 : 1);

	return fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_VSCR_WLEN);
}

int
fsp_get_vscr(void *handle, int *en)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_VSCR_WLEN + 1];
	int	ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_VSCR, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (en != NULL)) {
		*en = atoi(tmp);
	}
	return (ret);
}

int
fsp_set_hscr(void *handle, int en)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_HSCR_WLEN + 1];

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_HSCR, (char *)handle);
	/* compose value string */
	snprintf(tmp, sizeof(tmp), "%02x", (en == 0) ? 0 : 1);

	return fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_HSCR_WLEN);
}

int
fsp_get_hscr(void *handle, int *en)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_HSCR_WLEN + 1];
	int	ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_HSCR, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (en != NULL)) {
		*en = atoi(tmp);
	}
	return (ret);
}

int
fsp_set_pktfmt(void *handle, int mode)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_PKTFMT_WLEN + 1];

	if ((mode < 0) || (mode > FSP_MAX_PKT_FMT_ID)) {
		return (EINVAL);
	}
	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_PKTFMT, (char *)handle);
	/* compose value string */
	snprintf(tmp, sizeof(tmp), "%02x", mode);

	return fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_PKTFMT_WLEN);
}

int
fsp_get_pktfmt(void *handle, int *mode)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_PKTFMT_WLEN + 1];
	int	ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_PKTFMT, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (mode != NULL)) {
		*mode = atoi(tmp);
	}
	return (ret);
}

int
fsp_ps2(void *handle, int cmd, unsigned int res[FSP_RESP_PKT_MAXLEN])
{
	char	buf[64], tmp[64];
	int	ret, i;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_PS2, (char *)handle);
	/* compose request string */
	snprintf(tmp, sizeof(tmp), "%04x", cmd);

	/* send command */
	ret = fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_PS2_WLEN);
	if (ret != 0) {
		return (ret);
	}
	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (res != NULL)) {
		for (i = 0; i < strlen(tmp); i++) {
			sscanf(&tmp[i * 2], "%02x", &res[i]);
		}
	}
	return (ret);
}

int
fsp_set_flags(void *handle, int flags)
{
	char	buf[64], tmp[64], old_flags[64];
	int	ret, wlen, i;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_FLAGS, (char *)handle);

	/* read current flags */
	ret = fsp_sysfs_read_str(buf, old_flags, sizeof(old_flags));
	if (ret != 0)
		return (ret);

	tmp[0] = '\0';	/* initiate a new flag string */

	/* compatibility mode: only apply supported flags */
	for (wlen = strlen(old_flags) - 1, i = 0; wlen >= 0; wlen--) {
		switch (old_flags[wlen]) {
		case 'K': case 'k':
			tmp[i++] = flags & FSPDRV_FLAG_OPICON_KEY ? 'K' : 'k';
			break;
		case 'B': case 'b':
			tmp[i++] = flags & FSPDRV_FLAG_OPICON_BTN ? 'B' : 'b';
			break;
		case 'X': case 'x':
			tmp[i++] = flags & FSPDRV_FLAG_REVERSE_X ? 'X' : 'x';
			break;
		case 'Y': case 'y':
			tmp[i++] = flags & FSPDRV_FLAG_REVERSE_Y ? 'Y' : 'y';
			break;
		case 'A': case 'a':
			tmp[i++] = flags & FSPDRV_FLAG_AUTO_SWITCH ? 'A' : 'a';
			break;
		case 'C': case 'c':
			tmp[i++] = flags & FSPDRV_FLAG_EN_OPC ? 'C' : 'c';
			break;
		default:
			/* unsupported flags */
			break;
		}
	}
	tmp[i++] = '\n';
	tmp[i] = '\0';

	return fsp_sysfs_write_str(buf, tmp, i);
}

int
fsp_get_flags(void *handle, int *flags)
{
	char	buf[64], tmp[64];
	int	ret, i, f;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_FLAGS, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (flags != NULL)) {
		/* parse the result */
		f = 0;
		for (i = 0; i < strlen(tmp); i++) {
			switch (tmp[i]) {
			case 'B':
				f |= FSPDRV_FLAG_OPICON_BTN;
				break;
			case 'b':
				f &= ~FSPDRV_FLAG_OPICON_BTN;
				break;
			case 'K':
				f |= FSPDRV_FLAG_OPICON_KEY;
				break;
			case 'k':
				f &= ~FSPDRV_FLAG_OPICON_KEY;
				break;
			case 'X':
				f |= FSPDRV_FLAG_REVERSE_X;
				break;
			case 'x':
				f &= ~FSPDRV_FLAG_REVERSE_X;
				break;
			case 'Y':
				f |= FSPDRV_FLAG_REVERSE_Y;
				break;
			case 'y':
				f &= ~FSPDRV_FLAG_REVERSE_Y;
				break;
			case 'A':
				f |= FSPDRV_FLAG_AUTO_SWITCH;
				break;
			case 'a':
				f &= ~FSPDRV_FLAG_AUTO_SWITCH;
				break;
			case 'C':
				f |= FSPDRV_FLAG_EN_OPC;
				break;
			case 'c':
				f &= ~FSPDRV_FLAG_EN_OPC;
				break;
			default:
				/* unknown flag */
				break;
			}
		}
		*flags = f;
	}
	return (ret);
}

int
fsp_set_page(void *handle, int page)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_PAGE_WLEN + 1];

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_PAGE, (char *)handle);
	/* compose value string */
	snprintf(tmp, sizeof(tmp), "%02x", page);

	return fsp_sysfs_write_str(buf, tmp, FSP_SYSFS_NODE_PAGE_WLEN);
}

int
fsp_get_page(void *handle, int *page)
{
	char	buf[64], tmp[FSP_SYSFS_NODE_PAGE_WLEN + 1];
	int	ret;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_PAGE, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0 && (page != NULL)) {
		sscanf(tmp, "%02x", page);
	}
	return (ret);
}

int
fsp_set_accel(void *handle, int acc_num, int acc_denom, int thres)
{
	char	buf[64], tmp[64];
	int	wlen;

	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_ACCEL, (char *)handle);
	/* compose value string */
	wlen = snprintf(tmp, sizeof(tmp), "%d %d %d", acc_num, acc_denom,
		thres);

	return fsp_sysfs_write_str(buf, tmp, wlen);
}

int
fsp_get_accel(void *handle, int *acc_num, int *acc_denom, int *thres)
{
	char	buf[64], tmp[64];
	int	ret;

	if ((acc_num == NULL) || (acc_denom == NULL) || (thres == NULL)) {
		return (EINVAL);
	}
	/* compose full sysfs path */
	snprintf(buf, sizeof(buf), "%s/" FSP_SYSFS_NODE_ACCEL, (char *)handle);

	/* read result */
	ret = fsp_sysfs_read_str(buf, tmp, sizeof(tmp));
	if (ret == 0) {
		sscanf(tmp, "%d %d %d", acc_num, acc_denom, thres);
	}
	return (ret);
}
