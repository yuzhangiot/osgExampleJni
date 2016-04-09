/* Math for fisheye camera
*/
#pragma once
#include "projector.h"

class FisheyeSpherical : public Projector
{
public:
    FisheyeSpherical();
    FisheyeSpherical(int image_width, int image_height, float fov, bool shift_tl = false, bool use_hfov = true);
    ~FisheyeSpherical();

    void set_camera_params(int image_width, int image_height, float fov, bool use_hfov);
    void update_focal_length();
    float get_focal_length() { return focal_; }
    void set_scale(float scale);
    void set_output_width(int width);

    void set_thoby_params(float k1, float k2) { k1_ = k1; k2_ = k2; use_thoby_ = true; }

    // for a (x,y), get fisheye => rectilinear 
    void map_forward(float x, float y, float &u, float &v);
    // for a (x,y), get rectilinear => fisheye ratio
    void map_backward(float u, float v, float &x, float &y);

    // detect roi
    void detect_result_roi(int width, int height, int &x0, int &y0, int &x1, int &y1);
    void set_full_roi(int &x0, int &y0, int &x1, int &y1);

    bool is_inited_ ;


    // field of view in angles
    float hfov_, vfov_, theta_max_;
    bool use_hfov_;

    // Thoby fisheye model
    bool use_thoby_ ;
    float k1_, k2_;

    // focal length
    float focal_ ;
    // center of the input image (fisheye distorted)
    int centx_, centy_;
    // image width and height
    int image_width_, image_height_;
    // spherical scale
    float scale_ ;
};
