/* Image projectors
   Fisheye, Radial Distortion, Equirectangular, etc.
*/
#pragma once

// base class of projectors
class Projector
{
public:
    Projector();
    ~Projector();

    // src => dst
    virtual void map_forward(float x, float y, float &u, float &v) {};
    // dst => src
    virtual void map_backward(float u, float v, float &x, float &y) {};

    // detect roi
    virtual void detect_result_roi(int width, int height, int &x0, int &y0, int &x1, int &y1) {};
    virtual void set_full_roi(int &x0, int &y0, int &x1, int &y1) {};

    void detectResultRoiByBorder(int width, int height, float &tl_uf, float &tl_vf, float &br_uf, float &br_vf);

    // shift top-left point in output space or not (use false when followed by warping,
    // which shifted the center already in it's intrinsic matrix)
    bool shift_tl() { return shift_tl_; }

    virtual float get_focal_length() { return -1; }

protected:
    bool shift_tl_ ;
};

//#pragma comment(lib, "projectors.lib")
