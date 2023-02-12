#include <rsx/rsx.h>
#include <sysutil/video.h>
#include <malloc.h>

#define GCM_LABEL_INDEX 255
#define CB_SIZE         0x100000
#define HOST_SIZE       (32*1024*1024)

namespace {
videoResolution res;
gcmContextData *context = NULL;

u32 curr_fb = 0;
u32 first_fb = 1;

u32 display_width;
u32 display_height;

u32 depth_pitch;
u32 depth_offset;
u32 *depth_buffer;

u32 color_pitch;
u32 color_offset[2];
u32 *color_buffer[2];

static u32 sLabelVal = 1;

static void wait_finish(){
    rsxSetWriteBackendLabel(context,GCM_LABEL_INDEX,sLabelVal);

    rsxFlushBuffer(context);

    while(*(vu32*)gcmGetLabelAddress(GCM_LABEL_INDEX)!=sLabelVal)
        usleep(30);

    ++sLabelVal;
}

static void wait_rsx_idle(){
    rsxSetWriteBackendLabel(context,GCM_LABEL_INDEX,sLabelVal);
    rsxSetWaitLabel(context,GCM_LABEL_INDEX,sLabelVal);

    ++sLabelVal;

    wait_finish();
}

void init_screen2(){
    videoState state;
    videoGetState(0,0,&state);

    videoGetResolution(state.displayMode.resolution,&res);

    videoConfiguration vconfig;
    memset(&vconfig,0,sizeof(videoConfiguration));

    vconfig.resolution = state.displayMode.resolution;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = res.width*sizeof(u32);

    videoConfigure(0,&vconfig,NULL,0);
}

void init_screen(void *host_addr,u32 size){
    context = rsxInit(CB_SIZE,size,host_addr);

    videoState state;
    videoGetState(0,0,&state);

    videoGetResolution(state.displayMode.resolution,&res);

    videoConfiguration vconfig;
    memset(&vconfig,0,sizeof(videoConfiguration));

    vconfig.resolution = state.displayMode.resolution;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = res.width*sizeof(u32);

    wait_rsx_idle();

    videoConfigure(0,&vconfig,NULL,0);
    videoGetState(0,0,&state);

    gcmSetFlipMode(GCM_FLIP_VSYNC);

    display_width = res.width;
    display_height = res.height;

    color_pitch = display_width*sizeof(u32);
    color_buffer[0] = (u32*)rsxMemalign(64,(display_height*color_pitch));
    color_buffer[1] = (u32*)rsxMemalign(64,(display_height*color_pitch));

    rsxAddressToOffset(color_buffer[0],&color_offset[0]);
    rsxAddressToOffset(color_buffer[1],&color_offset[1]);

    gcmSetDisplayBuffer(0,color_offset[0],color_pitch,display_width,display_height);
    gcmSetDisplayBuffer(1,color_offset[1],color_pitch,display_width,display_height);

    depth_pitch = display_width*sizeof(u32);
    depth_buffer = (u32*)rsxMemalign(64,(display_height*depth_pitch)*2);
    rsxAddressToOffset(depth_buffer,&depth_offset);
}

}
