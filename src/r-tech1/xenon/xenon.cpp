#ifdef XENON

#ifdef DEBUG
#include <network/network.h>
#include <threads/gdb.h>
#endif

#include <threads/threads.h>
#include <xenos/xenos.h>
#include <diskio/ata.h>
#include <libfat/fat.h>
#include <xenon_sound/sound.h>
#include <console/console.h>
#include <xenon_soc/xenon_power.h>
#include <usb/usbmain.h>

void xenon_init(){
    xenos_init(VIDEO_MODE_AUTO);
    console_init();
    xenon_make_it_faster(XENON_SPEED_FULL);
    usb_init();
    usb_do_poll();
    xenon_ata_init();
    xenon_atapi_init();
    fatInitDefault();
    xenon_sound_init();
    threading_init();

#ifdef DEBUG
    network_init();
    gdb_init();
#endif
}
#endif

