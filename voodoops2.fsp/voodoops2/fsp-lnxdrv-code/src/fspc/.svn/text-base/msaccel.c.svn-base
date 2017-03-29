/**
 * @file msaccel.c The mouse acceleration module.
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
 * $Id: msaccel.c 28876 2008-10-27 06:17:55Z avatar $
 */

#include <stdio.h>
#include <X11/Xlib.h>

/**
 * @brief Get mouse acceleration number.
 *
 * @param acc The integer buffer for acceleration number to be stored.
 *
 * @retval -1 Failed to open display.
 * @retval 0 Success.
 */
int get_mouse_acc_num(int *acc)
{
	char *disp = NULL;
	Display *dpy;
	int acc_num, acc_denom, threshold;

	/* open a connection to the X server that controls a display */
	dpy = XOpenDisplay(disp);
	if (dpy == NULL)
	{
		return (-1);
	}

	/* get acceleration multiplier and acceleration threshold */
	XGetPointerControl(dpy, &acc_num, &acc_denom, &threshold);

	*acc = acc_num;

	return (0);
}

/**
 * @brief Set mouse acceleration number.
 *
 * @param acc The acceleration number.
 *
 * @retval -1 Failed to open display.
 * @retval 0 Success.
 */
int set_mouse_acc_num(int acc)
{
	char *disp = NULL;
	Display *dpy;
	int acc_num, acc_denom, threshold;

	/* open a connection to the X server that controls a display */
	dpy = XOpenDisplay(disp);
	if (dpy == NULL)
	{
		return (-1);
	}

	/* set acceleration multiplier and acceleration threshold */
	XChangePointerControl(dpy, True, False, acc, 1, 0);

	/* get acceleration multiplier and acceleration threshold */
	XGetPointerControl(dpy, &acc_num, &acc_denom, &threshold);

	return (0);
}
