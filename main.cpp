#include "mbed.h"
#include "EasyAttach_CameraAndLCD.h"
#include "SdUsbConnect.h"
#include "opencv.hpp"
#include "camera_if.hpp"
#include "DisplayApp.h"

#include "EthernetInterface.h"
#include "Websocket.h"

using namespace cv;

#define DBG_PCMONITOR (1)

//  3D grid size
#define GRID_SIZE_X 50     // size
#define GRID_SIZE_Y 50     // size
#define GRID_SIZE_Z 45     // size
#define GRID_SCALE  50.0    // resolution(mm/grid)

// Intrinsic parameters of the camera (cf. OpenCV's Camera Calibration)
#define CAMERA_0_DISTANCE 1350
#define CAMERA_0_RADIAN 0.0    // 0 degree
#define CAMERA_0_CENTER_U 87   // Optical centers (cx)
#define CAMERA_0_CENTER_V 58   // Optical centers (cy)
#define CAMERA_0_FX 121.0      // Focal length(fx)
#define CAMERA_0_FY 120.0      // Focal length(fy)

#define CAMERA_1_DISTANCE 1120
#define CAMERA_1_RADIAN 1.5708 // 90 degree
#define CAMERA_1_CENTER_U 78   // Optical centers (cx)
#define CAMERA_1_CENTER_V 58   // Optical centers (cy)
#define CAMERA_1_FX 119.2      // Focal length(fx)
#define CAMERA_1_FY 122.5      // Focal length(fy)

// pre-calculation table
int camera_area_table[GRID_SIZE_X][GRID_SIZE_Y][GRID_SIZE_Z];
int camera_0_table[GRID_SIZE_X][GRID_SIZE_Y][GRID_SIZE_Z][2];
int camera_1_table[GRID_SIZE_X][GRID_SIZE_Y][GRID_SIZE_Z][2];

/* Application variables */
Mat img_background_0;   // background image for NTSC-1A
Mat img_background_1;   // background image for NTSC-2A

Mat img_silhouette_0;
Mat img_silhouette_1;
char data_buf[128];

int reconst_index = 1;
int file_name_index = 1;
char file_name[32];

#if (DBG_PCMONITOR == 1)
/* For viewing image on PC */
static DisplayApp  display_app;
#endif

DigitalIn  button0(USER_BUTTON);
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

void set_background() {
    // 背景画像の取得
    create_gray_0(img_background_0);
    create_gray_1(img_background_1);
}

// Projects a 3D point into camera0 coordinates
int projection_0(double Xw, double Yw,double Zw, int &u, int &v)
{
    // rotate around the Y axis
    double Xc = cos(CAMERA_0_RADIAN)*Xw + sin(CAMERA_0_RADIAN)*Zw;
    double Yc = -Yw;
    double Zc =-sin(CAMERA_0_RADIAN)*Xw + cos(CAMERA_0_RADIAN)*Zw;

    // Perspective projection
    Zc -= CAMERA_0_DISTANCE;
  
    u = CAMERA_0_CENTER_U - (int)((Xc/Zc)*(CAMERA_0_FX));
    v = CAMERA_0_CENTER_V - (int)((Yc/Zc)*(CAMERA_0_FY));

    return (u>0 && u<VIDEO_PIXEL_HW && v>0 && v<VIDEO_PIXEL_VW);
}

// Projects a 3D point into camera1 coordinates
int projection_1(double Xw, double Yw,double Zw, int &u, int &v)
{
    // rotate around the Y axis
    double Xc = cos(CAMERA_1_RADIAN)*Xw + sin(CAMERA_1_RADIAN)*Zw;
    double Yc = -Yw;
    double Zc =-sin(CAMERA_1_RADIAN)*Xw + cos(CAMERA_1_RADIAN)*Zw;

    // Perspective projection
    Zc -= CAMERA_1_DISTANCE;
  
    u = CAMERA_1_CENTER_U - (int)((Xc/Zc)*(CAMERA_1_FX));
    v = CAMERA_1_CENTER_V - (int)((Yc/Zc)*(CAMERA_1_FY));

    return (u>0 && u<VIDEO_PIXEL_HW && v>0 && v<VIDEO_PIXEL_VW);
}

// Pre-caluation
void precalc() {
    double xx,yy,zz;    // 3D point(x,y,z)
    int u,v;            // camera coordinates(x,y)
    int ret;

    zz = (-GRID_SIZE_Z / 2) * GRID_SCALE;
    for (int z=0; z<GRID_SIZE_Z; z++, zz += GRID_SCALE) {

        yy = (-GRID_SIZE_Y / 2) * GRID_SCALE;
        for (int y=0; y<GRID_SIZE_Y; y++, yy += GRID_SCALE) {

            xx = (-GRID_SIZE_X / 2) * GRID_SCALE;
            for (int x=0; x<GRID_SIZE_X; x++, xx += GRID_SCALE) {

                ret = projection_0(xx, yy, zz, u, v);
                camera_0_table[x][y][z][0] = u;
                camera_0_table[x][y][z][1] = v;
                
                ret = ret & projection_1(xx, yy, zz, u, v);
                camera_1_table[x][y][z][0] = u;
                camera_1_table[x][y][z][1] = v;

                camera_area_table[x][y][z] = ret;

            }
        }
    }
}

// Voxel based "Shape from silhouette"
// Only voxels that lie inside all silhouette volumes remain part of the final shape.
void shape_from_silhouette(Websocket *ws) {

    // Take a silhouette
    create_gray_0(img_silhouette_0);
    create_gray_1(img_silhouette_1);

    // Background subtraction
    cv::absdiff(img_silhouette_0, img_background_0, img_silhouette_0);
    cv::absdiff(img_silhouette_1, img_background_1, img_silhouette_1);

    // Get a silhouette
    cv::threshold(img_silhouette_0, img_silhouette_0, 50, 255, cv::THRESH_BINARY);
    cv::threshold(img_silhouette_1, img_silhouette_1, 50, 255, cv::THRESH_BINARY);

    // Check each voxels
    for (int z=0; z<GRID_SIZE_Z; z++) {
        for (int y=0; y<GRID_SIZE_Y; y++) {
            for (int x=0; x<GRID_SIZE_X; x++) {
                    
                // Project a 3D point into camera0 coordinates
                if (camera_area_table[x][y][z]) {
                    
                    unsigned char *src0 = img_silhouette_0.ptr<unsigned char>(camera_0_table[x][y][z][1]);
                    if (src0[camera_0_table[x][y][z][0]]) {

                        // Project a 3D point into camera1 coordinates
                        unsigned char *src1 = img_silhouette_1.ptr<unsigned char>(camera_1_table[x][y][z][1]);
                        if (src1[camera_1_table[x][y][z][0]]) {
                            // Keep the point because it is inside the shilhouette
                            sprintf(data_buf, "%d %d %d", x, y, z);
                            int error_c = ws->send(data_buf);
                        }
                    }
                }
            }
        }
    }

    int error_c = ws->send("end");
}

int main() {

    // announce
    printf("Websocket Example v1.0.0\r\n");
    
    // Create a network interface and connect
    EthernetInterface eth;
    eth.connect();
    printf("IP Address is %s\r\n", eth.get_ip_address());

    // Create a websocket instance
    Websocket ws("ws://192.168.0.10:1880/ws/pointcloud", &eth);
    int connect_error = ws.connect();
    
    printf("Camera Test\r\n");

    // Camera
    camera_start_0();
    camera_start_1();
    led4 = 1;

    // SD & USB
    SdUsbConnect storage("storage");
    led3 = 1;

    set_background();
    precalc();

    while (1) {
        if (button0 == 0) {
            printf("Finding a storage...");
            // wait for the storage device to be connected
            storage.wait_connect();
            printf("done\r\n");
        
            // 背景画像の保存
            printf("Saving background images...");
            cv::imwrite("/storage/img_background_0.bmp", img_background_0);
            cv::imwrite("/storage/img_background_1.bmp", img_background_1);
            printf("done\r\n");

            printf("Saving silhouette images...");
            cv::imwrite("/storage/img_silhouette_0.bmp", img_silhouette_0);
            cv::imwrite("/storage/img_silhouette_1.bmp", img_silhouette_1);
            printf("done\r\n");
        }

        shape_from_silhouette(&ws);

        led2 = 1 - led2;

#if (DBG_PCMONITOR == 1)
        size_t jpeg_size = create_jpeg_0();
        display_app.SendJpeg(get_jpeg_adr(), jpeg_size);
#endif

    }
}
