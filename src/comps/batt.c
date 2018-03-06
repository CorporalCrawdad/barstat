#include <stdio.h>
#include "../util.h"
#include "bat.h"

const char *
getbatlvl(void)
{
	int chnow, chfull;

	if (BT_NOW)
		if (BT_FULL) {
			pscanf(BT_NOW, "%i", &chnow);
			pscanf(BT_FULL, "%i", &chfull);
		}

	if (chnow)
		if (chfull)
			return bprintf("%i", ((chnow*100)/chfull));
	return bprintf("%i", 0);
}
