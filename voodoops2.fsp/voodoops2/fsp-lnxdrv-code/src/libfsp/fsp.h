/**
 * @file fsp.h Library for communicating with Finger Sensing Pad PS/2 kernel
 * driver.
 *
 * Copyright (C) 2008 Tai-hwa Liang, Sentelic Corporation.
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
 * $Id: fsp.h 28876 2008-10-27 06:17:55Z avatar $
 */

#ifndef	__FSP_LIB_H__
#define	__FSP_LIB_H__

#ifndef	FSP_RESP_PKT_MAXLEN
/** maximum number of bytes a PS/2 repsond would have */
#define	FSP_RESP_PKT_MAXLEN		(8)
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Lookup and open an existing FSP device
 *
 * @return NULL if there is no FSP found; otherwise, it returns the device
 * handle
 */
void *	fsp_open_dev(void);

/**
 * Releasing an opened FSP handle
 *
 * @see fsp_open_dev
 */
void	fsp_free_dev(void *handle);

/**
 * Get FSP driver version information.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param ver buffer to hold the returned version information
 * @param ver_len max number of byte ver can hold
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_ver(void *handle, char *ver, size_t ver_len);

/**
 * Write a series of FSP registers.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param addrs an array of register addresses to be written to
 * @param vals an array of register values to be written to
 * @param len how many pairs of registers in addrs and vals
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_regs(void *handle, int *addrs, int *vals, size_t len);

/**
 * Write a FSP register.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param addr register address
 * @param val register value
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_reg(void *handle, int addr, int val);

/**
 * Read a FSP register.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param addr register address
 * @param val buffer to hold the returned register value
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_reg(void *handle, int addr, int *val);

/**
 * Enable/Disable vertical scrolling.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param en 1 to enable, 0 to disable
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_vscr(void *handle, int en);

/**
 * Get current vertical scrolling state.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param en buffer to store scrolling enabled/disabled state
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_vscr(void *handle, int *en);

/**
 * Enable/Disable horizontal scrolling.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param en 1 to enable, 0 to disable
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_hscr(void *handle, int en);

/**
 * Get current horizontal scrolling state.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param en buffer to store scrolling enabled/disabled state
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_hscr(void *handle, int *en);

/**
 * Set current packet format.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param mode 0: normal, 1: G0 absolute packet mode, 2: A0 absolute packet
 * mode
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_pktfmt(void *handle, int mode);

/**
 * Get current packet format.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param mode buffer to store current packet format
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_pktfmt(void *handle, int *mode);

/**
 * Sending single PS/2 command to FSP.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param cmd command to send; higher 8 bits are expected response length,
 * lower 8 bits are the command itself
 * @param res buffer to store the returned response
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_ps2(void *handle, int cmd, unsigned int res[FSP_RESP_PKT_MAXLEN]);

/**
 * Set driver flags.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param flags driver flags to be passed to kernel driver
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_flags(void *handle, int flags);

/**
 * Get driver flags.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param flags buffer to store the returned driver flags
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_flags(void *handle, int *flags);

/**
 * Set current working register page.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param page register page to be switched to
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_set_page(void *handle, int page);

/**
 * Get current working register page.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param page buffer to store the returned register page
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 */
int	fsp_get_page(void *handle, int *page);

/**
 * Set current acceleration settings.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param acc_num the numerator for the acceleration multiplier
 * @param acc_denom the denominator for the acceleration multiplier
 * @param thres the acceleration threshold
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 *
 * @sa XChangePointerControl
 */
int	fsp_set_accel(void *handle, int acc_num, int acc_denom, int thres);

/**
 * Get current acceleration settings.
 *
 * @param handle FSP handle returned by fsp_open_dev()
 * @param acc_num buffer to store the returned acceleration numerator
 * @param acc_denom buffer to store the returned acceleration denominator
 * @param thres buffer to store the returned acceleration threshold
 *
 * @return On success, 0 is returned.  On error, an non-zero value is
 * returned(refer to standard errno).
 *
 * @sa XChangePointerControl
 */
int	fsp_get_accel(void *handle, int *acc_num, int *acc_denom, int *thres);

#ifdef	__cplusplus
}	/* extern "C" */
#endif

#endif	/* __FSP_LIB_H__ */
