#include "m_remap.h"
#include <omp.h>

void remap_cpu(int width, int height, const std::vector<std::vector<mColor> > &src, std::vector<std::vector<mColor> > &dst, 
    const std::vector<std::vector<float> > &xmap, const std::vector<std::vector<float> > &ymap, int inter) {
    if (src.empty() || src[0].empty())
        return;

    int src_rows = src.size(), src_cols = src[0].size();

#pragma omp parallel for
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float dx = xmap[y][x];
            float dy = ymap[y][x];
            if (dx < 0 || dy < 0 || dx > src_cols - 1 || dy > src_rows - 1)
                continue;

            if (inter == NEAREST) {
                // nearest
                //dst[y][x].b = src[dy][dx].b;
                //dst[y][x].g = src[dy][dx].g;
                //dst[y][x].r = src[dy][dx].r;
                dst[y][x] = src[dy][dx];
            } else {
                // linear
                float dx0 = floor(dx), dy0 = floor(dy);
                float dx1 = ceil(dx), dy1 = ceil(dy);
                float rx = dx - dx0, ry = dy - dy0;
                if (rx == 0 && ry == 0) {
                    dst[y][x] = src[dy][dx];
                } else {
                    // faster than operator overload
                    dst[y][x].b = src[dy0][dx0].b * (1 - rx) * (1 - ry) + src[dy0][dx1].b * (rx)* (1 - ry) +
                        src[dy1][dx0].b * (1 - rx) * (ry)+src[dy1][dx1].b * (rx)* (ry);
                    dst[y][x].g = src[dy0][dx0].g * (1 - rx) * (1 - ry) + src[dy0][dx1].g * (rx)* (1 - ry) +
                        src[dy1][dx0].g * (1 - rx) * (ry)+src[dy1][dx1].g* (rx)* (ry);
                    dst[y][x].r = src[dy0][dx0].r * (1 - rx) * (1 - ry) + src[dy0][dx1].r * (rx)* (1 - ry) +
                        src[dy1][dx0].r * (1 - rx) * (ry)+src[dy1][dx1].r * (rx)* (ry);
                }
            }
        }
    }
}

void remap_cpu(int src_cols, int src_rows, int map_cols, int map_rows, const unsigned char src[], unsigned char dst[],
    const std::vector<std::vector<float> > &xmap, const std::vector<std::vector<float> > &ymap, int inter) {
#pragma omp parallel for
    for (int y = 0; y < map_rows; y++) {
        for (int x = 0; x < map_cols; x++) {
            float dx = xmap[y][x];
            float dy = ymap[y][x];
            if (dx < 0 || dy < 0 || dx > src_cols - 1 || dy > src_rows - 1) {
                //if the system is low, check these lines.
                dst[y * map_cols * 3 + x * 3 + 0] = 0;
                dst[y * map_cols * 3 + x * 3 + 1] = 0;
                dst[y * map_cols * 3 + x * 3 + 2] = 0;
                continue;
            }

            if (inter == NEAREST) {
                // nearest
                int dx0 = floor(dx), dy0 = floor(dy);
                dst[y * map_cols * 3 + x * 3 + 0] = src[dy0 * src_cols * 3 + dx0 * 3 + 0];
                dst[y * map_cols * 3 + x * 3 + 1] = src[dy0 * src_cols * 3 + dx0 * 3 + 1];
                dst[y * map_cols * 3 + x * 3 + 2] = src[dy0 * src_cols * 3 + dx0 * 3 + 2];
            } else {
                // linear
                int dx0 = floor(dx), dy0 = floor(dy);
                int dx1 = ceil(dx), dy1 = ceil(dy);
                float rx = dx - dx0, ry = dy - dy0;
                if (rx == 0 && ry == 0) {
                    dst[y * map_cols * 3 + x * 3 + 0] = src[dy0 * src_cols * 3 + dx0 * 3 + 0];
                } else {
                    // faster than operator overload
                    dst[y * map_cols * 3 + x * 3 + 0] = src[dy0 * src_cols * 3 + dx0 * 3 + 0] * (1 - rx) * (1 - ry) 
                        + src[dy0 * src_cols * 3 + dx1 * 3 + 0] * (rx)* (1 - ry)
                        + src[dy1 * src_cols * 3 + dx0 * 3 + 0] * (1 - rx) * (ry)
                        + src[dy1 * src_cols * 3 + dx1 * 3 + 0] * (rx) * (ry);
                    dst[y * map_cols * 3 + x * 3 + 1] = src[dy0 * src_cols * 3 + dx0 * 3 + 0] * (1 - rx) * (1 - ry)
                        + src[dy0 * src_cols * 3 + dx1 * 3 + 1] * (rx)* (1 - ry)
                        + src[dy1 * src_cols * 3 + dx0 * 3 + 1] * (1 - rx) * (ry)
                        +src[dy1 * src_cols * 3 + dx1 * 3 + 1] * (rx)* (ry);
                    dst[y * map_cols * 3 + x * 3 + 2] = src[dy0 * src_cols * 3 + dx0 * 3 + 0] * (1 - rx) * (1 - ry)
                        + src[dy0 * src_cols * 3 + dx1 * 3 + 2] * (rx)* (1 - ry)
                        + src[dy1 * src_cols * 3 + dx0 * 3 + 2] * (1 - rx) * (ry)
                        +src[dy1 * src_cols * 3 + dx1 * 3 + 2] * (rx)* (ry);
                }
            }
        }
    }
}
