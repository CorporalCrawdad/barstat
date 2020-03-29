#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <libvirt/libvirt.h>

#include "../util.h"

virConnectPtr  conn = NULL;
virDomainPtr   dom  = NULL;

const char *vmname;
const char *vmconn;

void
dest_vmst(void)
{
	virConnectClose(conn);
}

void
initvmst(void)
{
	conn = virConnectOpen(vmconn);
	if (conn)
		dom = virDomainLookupByName(conn, vmname);
	atexit(dest_vmst);
	assert(dom);
}

const char*
getvmst(void)
{
	int dstate;
	if (!conn)
		initvmst();

	if (virDomainGetState(dom, &dstate, NULL, 0))
		return bprintf("ERR");
	switch (dstate) {
	case VIR_DOMAIN_NOSTATE:
		return bprintf("No State");
	case VIR_DOMAIN_RUNNING:
		return bprintf("Running");
	case VIR_DOMAIN_PAUSED:
		return bprintf("Paused");
	case VIR_DOMAIN_SHUTDOWN:
		return bprintf("Closing");
	case VIR_DOMAIN_SHUTOFF:
		return bprintf("Pwr Off");
	case VIR_DOMAIN_CRASHED:
		return bprintf("Crashed");
	default:
		return bprintf("Unknown");
	}
}

