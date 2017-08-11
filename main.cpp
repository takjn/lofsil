#include "mbed.h"
#include "EasyAttach_CameraAndLCD.h"
#include "SdUsbConnect.h"
#include "opencv.hpp"
#include "camera_if.hpp"
#include "face_detector.hpp"
#include "DisplayApp.h"

using namespace cv;

#define DBG_PCMONITOR (1)

/* Application variables */
Mat frame_gray;     // Input frame (in grayscale)

#if (DBG_PCMONITOR == 1)
/* For viewing image on PC */
static DisplayApp  display_app;
#endif

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

int main() {
    printf("Camera Test\r\n");

    // Camera
    camera_start_0();
    camera_start_1();
    led4 = 1;

    // SD & USB
    SdUsbConnect storage("storage");
    printf("Finding a storage...");
    // wait for the storage device to be connected
    storage.wait_connect();
    printf("done\n");
    led3 = 1;

    while (1) {
        // Retrieve a video frame (grayscale)
        create_gray_0(frame_gray);
        if (frame_gray.empty()) {
            printf("ERR: There is no input frame, retry to capture\n");
            continue;
        }

#if (DBG_PCMONITOR == 1)
        size_t jpeg_size = create_jpeg_0();
        display_app.SendJpeg(get_jpeg_adr(), jpeg_size);
#endif

    }
}
