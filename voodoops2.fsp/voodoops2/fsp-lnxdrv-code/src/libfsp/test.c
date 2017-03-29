/**
 * @file Testing drive for the Finger Sensing Pad kernel communication
 * library.
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
 * $Id: test.c 28876 2008-10-27 06:17:55Z avatar $
 */

#include <stdio.h>
#include <stdlib.h>

#include "sentelic.h"
#include "fsp.h"

int
main(void)
{
	void	*h = fsp_open_dev();
	char	ver[64];
	int	page, flags, pktfmt, hscr, vscr, val;

	if (h == NULL) {
		printf("FSP not found!\n");
		return (-1);
	}
	fsp_get_ver(h, ver, sizeof(ver));
	printf("ver: %s\n", ver);

	fsp_get_page(h, &page);
	printf("page: 0x%x\n", page);

	fsp_set_page(h, 0);
	printf("set page to 0\n");

	fsp_get_flags(h, &flags);
	printf("flags: 0x%x\n", flags);

	/* flipping FSPDRV_FLAG_EN_OPC setting */
	if ((flags & FSPDRV_FLAG_EN_OPC) != 0)
		flags &= ~FSPDRV_FLAG_EN_OPC;
	else
		flags |= FSPDRV_FLAG_EN_OPC;
	fsp_set_flags(h, flags);
	printf("set flags to 0x%x\n", flags);

	fsp_get_pktfmt(h, &pktfmt);
	printf("pktfmt: 0x%x\n", pktfmt);

	/* flip pktfmt settings */
	pktfmt = (pktfmt + 1) % 3;
	fsp_set_pktfmt(h, pktfmt);
	printf("set pktfmt to %d\n", pktfmt);

	fsp_get_hscr(h, &hscr);
	printf("hscr: 0x%x\n", hscr);

	/* flip hscr settings */
	hscr = hscr ? 0 : 1;
	fsp_set_hscr(h, hscr);
	printf("set hscr to %d\n", hscr);

	fsp_get_vscr(h, &vscr);
	printf("vscr: 0x%x\n", vscr);

	/* flip vscr settings */
	hscr = vscr ? 0 : 1;
	fsp_set_vscr(h, vscr);
	printf("set vscr to %d\n", vscr);

	fsp_get_reg(h, 1, &val);
	printf("reg[1] = 0x%x\n", val);

	/* flip reg[1] settings */
	val = ((val & 2) == 0) ? val | 2 : val & ~2;
	fsp_set_reg(h, 1, val);
	printf("set reg[1] to 0x%x\n", val);

	fsp_free_dev(h);

	return (0);
}
