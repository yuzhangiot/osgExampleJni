#include "projector.h"
#include <limits>
#include <algorithm>

using namespace std;

Projector::Projector():shift_tl_ (true)
{}

Projector::~Projector()
{}

void Projector::detectResultRoiByBorder(int width, int height, float &tl_uf, float &tl_vf, float &br_uf, float &br_vf) {
    tl_uf = std::numeric_limits<float>::max();
    tl_vf = std::numeric_limits<float>::max();
    br_uf = -std::numeric_limits<float>::max();
    br_vf = -std::numeric_limits<float>::max();

    // by border
    float u, v;
    for (float x = 0; x < width; ++x)
    {
        map_forward(static_cast<float>(x), 0, u, v);
        tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
        br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);

        map_forward(static_cast<float>(x), static_cast<float>(height - 1), u, v);
        tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
        br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);
    }
    for (int y = 0; y < height; ++y)
    {
        map_forward(0, static_cast<float>(y), u, v);
        tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
        br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);

        map_forward(static_cast<float>(width - 1), static_cast<float>(y), u, v);
        tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
        br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);
    }

    //// by diagonal
    //float d = sqrt(width * width + height * height);
    //float dx = d / width, dy = d / height;
    //float x0 = 0, y0 = 0, y1 = height - 1;
    //for (int i = 0; i < d; i++) {
    //    map_forward(x0, y0, u, v);
    //    tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
    //    br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);

    //    map_forward(x0, y1, u, v);
    //    tl_uf = std::min(tl_uf, u); tl_vf = std::min(tl_vf, v);
    //    br_uf = std::max(br_uf, u); br_vf = std::max(br_vf, v);

    //    x0 += dx;
    //    y0 += dy;
    //    y1 -= dy;
    //}
}
