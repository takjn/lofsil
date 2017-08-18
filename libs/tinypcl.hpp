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
#ifndef TINYPCL_HPP
#define TINYPCL_HPP

#include <bitset>
using std::bitset;

//  3D grid size
#define PCD_SIZE_X 25     // size
#define PCD_SIZE_Y 10     // size
#define PCD_SIZE_Z 25     // size
#define PCD_SCALE  10.0    // resolution(mm/grid)

// Syntax sugar to access voxels
#define point_cloud_data(x,y,z)  point_cloud_data[(x) + ((y)*PCD_SIZE_X) + (PCD_SIZE_X*PCD_SIZE_Y*(z))]

class PointCloud {
public:
    const static int SIZE_X = PCD_SIZE_X;
    const static int SIZE_Y = PCD_SIZE_Y;
    const static int SIZE_Z = PCD_SIZE_Z;
    const static float SCALE = PCD_SCALE;

    PointCloud(void);

    unsigned char get(unsigned int index);
    unsigned char get(unsigned int x, unsigned int y, unsigned int z);
    void set(unsigned int index, unsigned char val);
    void set(unsigned int x, unsigned int y, unsigned int z, unsigned char val);
    void clear();
    void save_as_xyz(const char*);
private:
    // 3D grid representing object space
    bitset<PCD_SIZE_X*PCD_SIZE_Y*PCD_SIZE_Z> point_cloud_data;
};

#endif