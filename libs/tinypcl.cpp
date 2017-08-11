/*
** tiny Point Cloud Library
**
** Copyright (c) 2017 Jun Takeda
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#include <bitset>
#include <stdio.h>
#include "tinypcl.hpp"

// Constructor: Initializes PointCloud
PointCloud::PointCloud(void) {
    clear();
}

// Returns the value of the point
unsigned char PointCloud::get(unsigned int index) {
    return point_cloud_data[index];
}

// Returns the value of the point
unsigned char PointCloud::get(unsigned int x, unsigned int y, unsigned int z) {
    return point_cloud_data(x,y,z);
}

// Sets the value of the point
void PointCloud::set(unsigned int index, unsigned char val) {
    point_cloud_data[index] = val;
}

// Sets the value of the point
void PointCloud::set(unsigned int x, unsigned int y, unsigned int z, unsigned char val) {
    point_cloud_data(x,y,z) = val;
}

// Clear all points
void PointCloud::clear(void) {
    for (int z=0;z<SIZE;z++) {
        for (int y=0;y<SIZE;y++) {
            for (int x=0;x<SIZE;x++) {
                point_cloud_data(x,y,z) = 1;
            }
        }
    }
}

// Save point clouds as XYZ file
void PointCloud::save_as_xyz(const char* file_name) {
    FILE *fp_xyz = fopen(file_name, "w");

    for (int z=0; z<SIZE; z++) {
        for (int y=0; y<SIZE; y++) {
            for (int x=0; x<SIZE; x++) {
                if (point_cloud_data(x,y,z) == 1) {

                    // Write a 3D point
                    fprintf(fp_xyz,"%g %g %g\n", x*SCALE, y*SCALE, z*SCALE);

                }
            }
        }
    }

    fclose(fp_xyz);
}