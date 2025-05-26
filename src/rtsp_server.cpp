#include "rtsp_server.hpp"
#include <gst/rtsp-server/rtsp-server.h>

void start_rtsp_server()
{
    GMainLoop *loop;
    GstRTSPServer *server;
    GstRTSPMountPoints *mounts;
    GstRTSPMediaFactory *factory;

    gst_init(nullptr, nullptr);

    loop = g_main_loop_new(nullptr, FALSE);

    server = gst_rtsp_server_new();
    mounts = gst_rtsp_server_get_mount_points(server);

    factory = gst_rtsp_media_factory_new();
    /*To play default Video file RTSP*/
    // gst_rtsp_media_factory_set_launch(factory,
    //     "( videotestsrc ! x264enc ! rtph264pay name=pay0 pt=96 )");
    
    /*To play custom file in VLC*/
    gst_rtsp_media_factory_set_launch(factory,
    "( filesrc location=/home/somik/GStreamer/IVI_Project/assets/test2_AOT.mp4 ! decodebin ! x264enc tune=zerolatency ! rtph264pay name=pay0 pt=96 )");

    // gst_rtsp_media_factory_set_launch(factory,
    // "( rtspsrc location=rtsp://172.21.176.1:8554/webcam latency=0 ! decodebin ! videoconvert ! x264enc tune=zerolatency ! rtph264pay name=pay0 pt=96 )");


    gst_rtsp_mount_points_add_factory(mounts, "/test", factory);
    g_object_unref(mounts);

    gst_rtsp_server_attach(server, nullptr);

    g_print("RTSP stream ready at rtsp://127.0.0.1:8554/test\n");
    g_main_loop_run(loop);
}
