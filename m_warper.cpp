#include "m_warper.h"
#include <omp.h>

using namespace std;

mWarper::mWarper():inter_ (NEAREST)
{}

mWarper::~mWarper()
{}

void project_maps(Projector *projector, int width, int height, int x0, int y0, int x1, int y1,
    vector<vector<float> > &xmap, vector<vector<float> > &ymap) {
#pragma omp parallel for
    for (int v = y0; v <= y1; v++) {
        for (int u = x0; u <= x1; u++) {
            float uf = u, vf = v, xf, yf;
            projector->map_backward(uf, vf, xf, yf);
            if (xf >= 0 && yf >= 0 && xf < width && yf < height) {
                // map from original image
                xmap[v - y0][u - x0] = xf;
                ymap[v - y0][u - x0] = yf;
            } else {
                xmap[v - y0][u - x0] = -1;
                ymap[v - y0][u - x0] = -1;
            }
        }
    }
}

void mWarper::build_maps(int src_cols, int src_rows, bool use_src_range) {
    src_cols_ = src_cols;
    src_rows_ = src_rows;

    if (use_src_range) {
        // detect result roi
        projector_->detect_result_roi(src_cols, src_rows, dx0_, dy0_, dx1_, dy1_);
    } else {
        projector_->set_full_roi(dx0_, dy0_, dx1_, dy1_);
    }

    // init output map
    map_cols_ = dx1_ - dx0_ + 1;
    map_rows_ = dy1_ - dy0_ + 1;
    xmap_ = vector<vector<float> >(map_rows_, vector<float>(map_cols_, -1));
    ymap_ = vector<vector<float> >(map_rows_, vector<float>(map_cols_, -1));
    project_maps(projector_, src_cols, src_rows, dx0_, dy0_, dx1_, dy1_, xmap_, ymap_);
}

// warp image
void mWarper::warp_image(const vector<vector<mColor> > &src, vector<vector<mColor> > &dst) {
    dst.resize(map_rows_);
    for (int i = 0; i < map_rows_; i++)
        dst[i].resize(map_cols_);

    remap_cpu(map_cols_, map_rows_, src, dst, xmap_, ymap_, inter_);
}

void mWarper::warp_image(const unsigned char src[], unsigned char dst[]) {
    remap_cpu(src_cols_, src_rows_, map_cols_, map_rows_, src, dst, xmap_, ymap_, inter_);
}
