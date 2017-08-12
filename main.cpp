#include "mbed.h"
#include "EasyAttach_CameraAndLCD.h"
#include "SdUsbConnect.h"
#include "opencv.hpp"
#include "camera_if.hpp"
#include "DisplayApp.h"
#include "tinypcl.hpp"

using namespace cv;

#define DBG_PCMONITOR (1)

// Intrinsic parameters of the camera (cf. OpenCV's Camera Calibration)
#define CAMERA_0_DISTANCE 130
#define CAMERA_0_CENTER_U 173   // Optical centers (cx)
#define CAMERA_0_CENTER_V 116   // Optical centers (cy)
#define CAMERA_0_FX 242.0       // Focal length(fx)
#define CAMERA_0_FY 240.0       // Focal length(fy)

#define CAMERA_1_DISTANCE 110
#define CAMERA_1_CENTER_U 156   // Optical centers (cx)
#define CAMERA_1_CENTER_V 115   // Optical centers (cy)
#define CAMERA_1_FX 239.4       // Focal length(fx)
#define CAMERA_1_FY 245.0       // Focal length(fy)

/* Application variables */
PointCloud point_cloud; // Point cloud (3D reconstruction result)

Mat img_background_0;   // background image for NTSC-1A
Mat img_background_1;   // background image for NTSC-2A

Mat img_silhouette_0;
Mat img_silhouette_1;

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
    // // rotate around the Z axis
    // double Xc = cos(rad)*Xw + sin(rad)*Yw;
    // double Yc =-sin(rad)*Xw + cos(rad)*Yw;
    // double Zc = Zw;

    // camera0 : rad = 0
    double Xc = Xw;
    double Yc = Yw;
    double Zc = Zw;

    // Perspective projection
    Yc -= CAMERA_0_DISTANCE;
  
    u = CAMERA_0_CENTER_U - (int)((Xc/Yc)*(CAMERA_0_FX));
    v = CAMERA_0_CENTER_V - (int)((Zc/Yc)*(CAMERA_0_FY));

    return (u>0 && u<VIDEO_PIXEL_HW && v>0 && v<VIDEO_PIXEL_VW);
}

// Projects a 3D point into camera1 coordinates
int projection_1(double Xw, double Yw,double Zw, int &u, int &v)
{
    // // rotate around the Z axis
    // double Xc = cos(rad)*Xw + sin(rad)*Yw;
    // double Yc =-sin(rad)*Xw + cos(rad)*Yw;
    // double Zc = Zw;

    // camera0 : rad = 0
    double Xc = Yw;
    double Yc = -Xw;
    double Zc = Zw;

    // Perspective projection
    Yc -= CAMERA_1_DISTANCE;
  
    u = CAMERA_1_CENTER_U - (int)((Xc/Yc)*(CAMERA_1_FX));
    v = CAMERA_1_CENTER_V - (int)((Zc/Yc)*(CAMERA_1_FY));

    return (u>0 && u<VIDEO_PIXEL_HW && v>0 && v<VIDEO_PIXEL_VW);
}

// Voxel based "Shape from silhouette"
// Only voxels that lie inside all silhouette volumes remain part of the final shape.
void shape_from_silhouette() {

    // Take a silhouette
    create_gray_0(img_silhouette_0);
    create_gray_1(img_silhouette_1);

    // Background subtraction
    cv::absdiff(img_silhouette_0, img_background_0, img_silhouette_0);
    cv::absdiff(img_silhouette_1, img_background_1, img_silhouette_1);

    // Get a silhouette
    // #define SILHOUETTE_THRESH_BINARY 30     // threshold value for silhouette detection
    // cv::threshold(img_silhouette_0, img_silhouette_0, SILHOUETTE_THRESH_BINARY, 255, cv::THRESH_BINARY);
    // cv::threshold(img_silhouette_1, img_silhouette_1, SILHOUETTE_THRESH_BINARY, 255, cv::THRESH_BINARY);
    cv::threshold(img_silhouette_0, img_silhouette_0, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    cv::threshold(img_silhouette_1, img_silhouette_1, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Check each voxels
    double xx,yy,zz;    // 3D point(x,y,z)
    int u,v;            // camera coordinates(x,y)
    int pcd_index=0;

    zz = (-point_cloud.SIZE_Z / 2) * point_cloud.SCALE;
    for (int z=0; z<point_cloud.SIZE_Z; z++, zz += point_cloud.SCALE) {

        yy = (-point_cloud.SIZE_Y / 2) * point_cloud.SCALE;
        for (int y=0; y<point_cloud.SIZE_Y; y++, yy += point_cloud.SCALE) {

            xx = (-point_cloud.SIZE_X / 2) * point_cloud.SCALE;
            for (int x=0; x<point_cloud.SIZE_X; x++, xx += point_cloud.SCALE, pcd_index++) {
                    
                // Project a 3D point into camera0 coordinates
                if (projection_0(xx, yy, zz, u, v)) {
                    
                    // if (img_silhouette_0.at<unsigned char>(v, u)) {
                    unsigned char *src0 = img_silhouette_0.ptr<unsigned char>(v);
                    if (src0[u]) {

                        // Project a 3D point into camera1 coordinates
                        if (projection_1(xx, yy, zz, u, v)) {

                            // if (img_silhouette_1.at<unsigned char>(v, u)) {
                            unsigned char *src1 = img_silhouette_1.ptr<unsigned char>(v);
                            if (src1[u]) {
                                // Keep the point because it is inside the shilhouette
                                point_cloud.set(pcd_index, 1);
                            }
                            else {
                                // Delete the point because it is outside the shilhouette
                                point_cloud.set(pcd_index, 0);
                            }
                        }
                        else {
                            // Delete the point because it is outside the camera image
                            point_cloud.set(pcd_index, 0);
                        }
                    }
                    else {
                        // Delete the point because it is outside the shilhouette
                        point_cloud.set(pcd_index, 0);
                    }
                } else {
                    // Delete the point because it is outside the camera image
                    point_cloud.set(pcd_index, 0);
                }
            }
        }
    }
}

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

    set_background();

    while (1) {
        if (button0 == 0) {
            storage.wait_connect();

            // 背景画像の保存
            printf("Saving background images...");
            cv::imwrite("/storage/img_background_0.bmp", img_background_0);
            cv::imwrite("/storage/img_background_1.bmp", img_background_1);
            printf("done\r\n");

            printf("Saving silhouette images...");
            cv::imwrite("/storage/img_silhouette_0.bmp", img_silhouette_0);
            cv::imwrite("/storage/img_silhouette_1.bmp", img_silhouette_1);
            printf("done\r\n");

            printf("Saving a xyz file...\r\n");
            sprintf(file_name, "/storage/result_%d.xyz", reconst_index);
            point_cloud.save_as_xyz(file_name);
            printf("done\r\n");
            reconst_index++;
        }

        shape_from_silhouette();

        led2 = 1 - led2;

#if (DBG_PCMONITOR == 1)
        size_t jpeg_size = create_jpeg_0();
        display_app.SendJpeg(get_jpeg_adr(), jpeg_size);
#endif

    }
}
