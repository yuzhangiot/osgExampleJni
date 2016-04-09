#include "fisheye_spherical.h"
#include <stdio.h>
#include <math.h>

FisheyeSpherical::FisheyeSpherical()
{}

FisheyeSpherical::FisheyeSpherical(int image_width, int image_height, float fov, bool shift_tl, bool use_hfov) {
    shift_tl_ = shift_tl;
    set_camera_params(image_width, image_height, fov, use_hfov);
    use_thoby_ = false;
    focal_ = -1;
    scale_ = 100;
    is_inited_ = false;
}

FisheyeSpherical::~FisheyeSpherical()
{}

void FisheyeSpherical::set_output_width(int width) {
    set_scale(width / M_PI / 2);
}

void FisheyeSpherical::set_camera_params(int image_width, int image_height, float fov, bool use_hfov) {
    image_width_ = image_width;
    image_height_ = image_height;
    use_hfov_ = use_hfov;
    if (use_hfov)
        hfov_ = fov / 180 * M_PI;
    else
        vfov_ = fov / 180 * M_PI;
    update_focal_length();
}

void FisheyeSpherical::set_scale(float scale) { 
    scale_ = scale; 
}

void FisheyeSpherical::update_focal_length() {
    is_inited_ = true;

    // center of the image
    centx_ = image_width_ / 2, centy_ = image_height_ / 2;

    // maximum theta/radius from center to the edge of the image (usually vertically)
    float r_max, fov;
    if (use_hfov_) {
        fov = hfov_ / M_PI * 180;
        theta_max_ = hfov_ / 2;
        r_max = image_width_ / 2;
    } else {
        fov = vfov_ / M_PI * 180;
        theta_max_ = vfov_ / 2;
        r_max = image_height_ / 2;
    }

    if (use_thoby_) {
        // R = k1 * f * sin(k2 * theta)
        focal_ = r_max / (k1_ * sin(k2_ * theta_max_));
    } else {
        // focal length based on equidistant fisheye model: R = f * theta 
        focal_ = r_max / theta_max_;
    }

    printf("FoV is %.2f degree, Focal Length is %.2f\n", fov, focal_);
}

// https://en.wikipedia.org/wiki/Spherical_coordinate_system
// for a (x,y), get fisheye => rectilinear 
void FisheyeSpherical::map_forward(float x, float y, float &u, float &v) {
    // (x_, y_) w.r.t. principal point
    float dx = x - centx_;
    float dy = y - centy_;

    float R = sqrt(dx * dx + dy * dy);
    float phi = atan2f(dy, dx);

    // get x_, y_, z_ on sphere
    float x_, y_, z_, theta;

    if (use_thoby_) {
        // R = k1 * f * sin(k2 * theta)
        theta = asinf(R / (k1_ * focal_)) / k2_;
    } else {        
        // get theta by (fisheye) R = f * theta
        theta = R / focal_;
    }

    x_ = sinf(theta) * cosf(phi);
    y_ = sinf(theta) * sinf(phi);
    z_ = cosf(theta);

    u = scale_ * atan2f(x_, z_);
    float w = y_ / sqrtf(x_ * x_ + y_ * y_ + z_ * z_);
    v = scale_ * (static_cast<float>(M_PI)-acosf(w == w ? w : 0));
}

// for a (x,y), get rectilinear => fisheye ratio
void FisheyeSpherical::map_backward(float u, float v, float &x, float &y) {
    u /= scale_;
    v /= scale_;

    float sinv = sinf(v);
    float x_ = sinv * sinf(u);
    float y_ = -::cosf(v);
    float z_ = sinv * ::cosf(u);

    float r = sqrtf(x_ * x_ + y_ * y_ + z_ * z_);
    float theta = acosf(z_ / r);
    theta = theta ? theta : 0;
    float phi = atan2f(y_, x_);
    phi = phi ? phi : 0;

    if ((use_hfov_ && theta > theta_max_) ||
        (!use_hfov_ && theta > theta_max_)) {
        x = -1;
        y = -1;
        return;
    }

    float R;
    if (use_thoby_) {
        // R = k1 * f * sin(k2 * theta)
        R = k1_ * focal_ * sinf(k2_ * theta);
    } else {
        // get theta by (fisheye) R = f * theta
        R = focal_ * theta;
    }

    float dx = R * cosf(phi);
    float dy = R * sinf(phi);
    x = centx_ + dx;
    y = centy_ + dy;
}

// detect roi
void FisheyeSpherical::detect_result_roi(int width, int height, int &x0, int &y0, int &x1, int &y1) {
    float tl_uf, tl_vf, br_uf, br_vf;

    // default by border
    detectResultRoiByBorder(width, height, tl_uf, tl_vf, br_uf, br_vf);

    // set roi
    x0 = floor(tl_uf);
    y0 = floor(tl_vf);
    x1 = ceil(br_uf);
    y1 = ceil(br_vf);
}

void FisheyeSpherical::set_full_roi(int &x0, int &y0, int &x1, int &y1) {
    x0 = -ceil(scale_ * M_PI);
    y0 = 0;
    x1 = ceil(scale_ * M_PI) - 1;
    y1 = ceil(scale_ * M_PI) - 1;
}
