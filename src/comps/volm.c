#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

#include "../util.h"

extern const char *alsa_device;

const char *
getvolm()
{
	snd_mixer_t*          handle;
	snd_mixer_elem_t*     elem;
	snd_mixer_selem_id_t* sid;

	static const char* mix_name  = "Master";
//	static const char* card      = "hw:0";
	static       int   mix_index = 0;

	long pmin, pmax;
	long get_vol, set_vol;
	float f_multi;
	int ret;

	snd_mixer_selem_id_alloca(&sid);

	//sets simple-mixer index and name
	snd_mixer_selem_id_set_index(sid, mix_index);
	snd_mixer_selem_id_set_name(sid, mix_name);

	if ((snd_mixer_open(&handle,0)) < 0 )
		return bprintf("-1");
	if ((snd_mixer_attach(handle, alsa_device)) < 0) {
		snd_mixer_close(handle);
		return bprintf("-2");
	}
	if ((snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
		snd_mixer_close(handle);
		return bprintf("-3");
	}
	ret = snd_mixer_load(handle);
	if (ret < 0) {
		snd_mixer_close(handle);
		return bprintf("-4");
	}
	elem = snd_mixer_find_selem(handle, sid);
	if (!elem) {
		snd_mixer_close(handle);
		return bprintf("-5");
	}

	long minv, maxv, outvol;

	snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv);

	if (snd_mixer_selem_get_playback_volume(elem, 0, &outvol) < 0) {
		snd_mixer_close(handle);
		return bprintf("-6");
	}
	outvol -= minv;
	maxv -= minv;
	minv = 0;
	outvol = 100 * outvol / maxv;
	snd_mixer_close(handle);
	return bprintf("%i", outvol);
}
