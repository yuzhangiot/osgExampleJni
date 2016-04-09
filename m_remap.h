#pragma once
#include <vector>
#include <math.h>

struct mColor
{
    unsigned char b, g, r;
    mColor() {}
    mColor(unsigned char b, unsigned char g, unsigned char r) {
        this->b = b;
        this->g = g;
        this->r = r;
    }

    mColor operator = (const mColor src) { 
        this->b = src.b;
        this->g = src.g;
        this->r = src.r;
        return *this;
    }

    friend mColor operator*(const mColor& src, float r) {
        mColor dst;
        dst.b = (unsigned char)src.b * r;
        dst.g = (unsigned char)src.g * r;
        dst.r = (unsigned char)src.r * r;
        return dst;
    }

    friend mColor operator+(const mColor& src0, const mColor& src1) {
        mColor dst;
        dst.b = src0.b + src1.b;
        dst.g = src0.g + src1.g;
        dst.r = src0.r + src1.r;
        return dst;
    }
};

enum INTER_METHOD {
    NEAREST = 0,
    LINEAR
};

void remap_cpu(int width, int height, const std::vector<std::vector<mColor> > &src, std::vector<std::vector<mColor> > &dst, 
    const std::vector<std::vector<float> > &xmap, const std::vector<std::vector<float> > &ymap, int inter = NEAREST);

void remap_cpu(int src_cols, int src_rows, int map_cols, int map_rows, const unsigned char src[], unsigned char dst[],
    const std::vector<std::vector<float> > &xmap, const std::vector<std::vector<float> > &ymap, int inter = NEAREST);
