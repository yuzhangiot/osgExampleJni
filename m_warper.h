#pragma once
#include "m_remap.h"
#include "projector.h"

class mWarper
{
public:
    mWarper();
    ~mWarper();

    void set_projector(Projector *projector) { projector_ = projector; }

    // build maps (use_src_range or use_full_roi)
    void build_maps(int src_cols, int src_rows, bool use_src_range = true);
    //void build_maps(int width, int height, int x0, int y0, int x1, int y1, cv::Mat_<float> &xmap, cv::Mat_<float> &ymap,
    //    cv::Mat_<float> &src_xmap = cv::Mat_<float>(), cv::Mat_<float> &src_ymap = cv::Mat_<float>());
    void get_maps(std::vector<std::vector<float> > &xmap, std::vector<std::vector<float> > &ymap) {
        xmap = xmap_; ymap = ymap_;
    }

    // warp image
    void warp_image(const std::vector<std::vector<mColor> > &src, std::vector<std::vector<mColor> > &dst);
    void warp_image(const unsigned char src[], unsigned char dst[]);

    // helper functions
    void set_roi(int dx0, int dy0, int dx1, int dy1) { dx0_ = dx0; dy0_ = dy0; dx1_ = dx1; dy1_ = dy1; }
    void get_roi(int &dx0, int &dy0, int &dx1, int &dy1) { dx0 = dx0_; dy0 = dy0_; dx1 = dx1_; dy1 = dy1_; }
    void get_map_size(int &map_cols, int &map_rows) { map_cols = map_cols_; map_rows = map_rows_; }
    void set_inter(int inter) { inter_ = inter; }

private:
    Projector *projector_;
    int dx0_, dy0_, dx1_, dy1_;
    std::vector<std::vector<float> > xmap_, ymap_;
    int src_cols_, src_rows_, map_cols_, map_rows_;
    int inter_ ;
};
