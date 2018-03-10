#include <stdio.h>
#include "../util.h"
#include "bat.h"

void
batt_hook(void)
{
	system("ZZZ");
}

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
		if (chfull) {
			if (((chnow*100)/chfull) < 5)
				batt_hook();
			return bprintf("%i", ((chnow*100)/chfull));
		}
	return bprintf("%i", 0);
}
