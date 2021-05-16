//#define USE_MINMAX
#define USE_SSE

#ifdef USE_SSE
#include <emmintrin.h>
#else
#include <algorithm>
#include <cmath>
#endif

union alignas(16) float4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    float data[4];

#ifdef USE_SSE
    __m128 xmm;
#endif
};

inline float4 operator+(const float4& lhs, const float4& rhs) {
#ifdef USE_SSE
    float4 result;
    result.xmm = _mm_add_ps(lhs.xmm, rhs.xmm);
    return result;
#else
    float4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
#endif
}

inline float4 operator-(const float4& lhs, const float4& rhs) {
#ifdef USE_SSE
    float4 result;
    result.xmm = _mm_sub_ps(lhs.xmm, rhs.xmm);
    return result;
#else
    float4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
#endif
}

inline float4 operator*(const float4& lhs, float rhs) {
#ifdef USE_SSE
    float4 result;
    result.xmm = _mm_mul_ps(lhs.xmm, _mm_set_ps1(rhs));
    return result;
#else
    float4 result;
    result.x = lhs.x * rhs;
    result.y = lhs.y * rhs;
    result.z = lhs.z * rhs;
    return result;
#endif
}

inline float4 min(const float4& lhs, const float4& rhs) {
#ifdef USE_SSE
    float4 result;
    result.xmm = _mm_min_ps(lhs.xmm, rhs.xmm);
    return result;
#else
    float4 result;
    result.x = std::min(lhs.x, rhs.x);
    result.y = std::min(lhs.y, rhs.y);
    result.z = std::min(lhs.z, rhs.z);
    return result;
#endif
}

inline float4 max(const float4& lhs, const float4& rhs) {
#ifdef USE_SSE
    float4 result;
    result.xmm = _mm_max_ps(lhs.xmm, rhs.xmm);
    return result;
#else
    float4 result;
    result.x = std::max(lhs.x, rhs.x);
    result.y = std::max(lhs.y, rhs.y);
    result.z = std::max(lhs.z, rhs.z);
    return result;
#endif
}

union aabbox {
    struct {
#ifdef USE_MINMAX
        float4 min;
        float4 max;
#else
        float4 center;
        float4 extent;
#endif
    };

    float data[8];
};

inline aabbox operator+(const aabbox& lhs, const float4& rhs) {
#ifdef USE_MINMAX
#ifdef USE_SSE
    aabbox result;
    result.min.xmm = _mm_min_ps(lhs.min.xmm, rhs.xmm);
    result.max.xmm = _mm_max_ps(lhs.max.xmm, rhs.xmm);
    return result;
#else
    aabbox result;
    result.min = min(lhs.min, rhs);
    result.max = max(lhs.max, rhs);
    return result;
#endif // USE_SSE
#else
#ifdef USE_SSE
    __m128 min_xmm = _mm_min_ps(_mm_sub_ps(lhs.center.xmm, lhs.extent.xmm), rhs.xmm);
    __m128 max_xmm = _mm_max_ps(_mm_add_ps(lhs.center.xmm, lhs.extent.xmm), rhs.xmm);

    aabbox result;
    result.center.xmm = _mm_mul_ps(_mm_add_ps(min_xmm, max_xmm), _mm_set_ps1(0.5f));
    result.extent.xmm = _mm_mul_ps(_mm_sub_ps(min_xmm, max_xmm), _mm_set_ps1(0.5f));
    return result;
#else
    float4 min_ = min(lhs.center - lhs.extent, rhs);
    float4 max_ = max(lhs.center + lhs.extent, rhs);

    aabbox result;
    result.center = (min_ + max_) * 0.5f;
    result.extent = (max_ - min_) * 0.5f;
    return result;
#endif // USE_SSE
#endif // USE_MINMAX
}

inline aabbox operator+(const aabbox& lhs, const aabbox& rhs) {
#ifdef USE_MINMAX
#ifdef USE_SSE
    aabbox result;
    result.min.xmm = _mm_min_ps(lhs.min.xmm, rhs.min.xmm);
    result.max.xmm = _mm_max_ps(lhs.max.xmm, rhs.max.xmm);
    return result;
#else
    aabbox result;
    result.min = min(lhs.min, rhs.min);
    result.max = max(lhs.max, rhs.max);
    return result;
#endif // USE_SSE
#else
#ifdef USE_SSE
    __m128 min_xmm = _mm_min_ps(_mm_sub_ps(lhs.center.xmm, lhs.extent.xmm), _mm_sub_ps(rhs.center.xmm, rhs.extent.xmm));
    __m128 max_xmm = _mm_max_ps(_mm_add_ps(lhs.center.xmm, lhs.extent.xmm), _mm_add_ps(rhs.center.xmm, rhs.extent.xmm));

    aabbox result;
    result.center.xmm = _mm_mul_ps(_mm_add_ps(min_xmm, max_xmm), _mm_set_ps1(0.5f));
    result.extent.xmm = _mm_mul_ps(_mm_sub_ps(min_xmm, max_xmm), _mm_set_ps1(0.5f));
    return result;
#else
    float4 min_ = min(lhs.center - lhs.extent, rhs.center - rhs.extent);
    float4 max_ = max(lhs.center + lhs.extent, rhs.center + rhs.extent);

    aabbox result;
    result.center = (min_ + max_) * 0.5f;
    result.extent = (max_ - min_) * 0.5f;
    return result;
#endif // USE_SSE
#endif // USE_MINMAX
}

inline bool intersect(const float4& lhs, const aabbox& rhs) {
#ifdef USE_MINMAX
#ifdef USE_SSE
    return _mm_movemask_ps(_mm_and_ps(_mm_cmpge_ps(lhs.xmm, rhs.min.xmm), _mm_cmple_ps(lhs.xmm, rhs.max.xmm))) == 0xFFFFFF00;
#else
    return lhs.x >= rhs.min.x && lhs.y >= rhs.min.y && lhs.z >= rhs.min.z &&
           lhs.x <= rhs.max.x && lhs.y <= rhs.max.y && lhs.z <= rhs.max.z;
#endif // USE_SSE
#else
#ifdef USE_SSE
    __m128 abs_xmm = _mm_andnot_ps(_mm_set_ps1(-0.f), _mm_sub_ps(rhs.center.xmm, lhs.xmm));
    return _mm_movemask_ps(_mm_cmple_ps(abs_xmm, rhs.extent.xmm)) == 0xFFFFFF00;
#else
    return std::abs(rhs.center.x - lhs.x) <= rhs.extent.x &&
           std::abs(rhs.center.y - lhs.y) <= rhs.extent.y &&
           std::abs(rhs.center.z - lhs.z) <= rhs.extent.z;
#endif // USE_SSE
#endif // USE_MINMAX
}

inline bool intersect(const aabbox& lhs, const aabbox& rhs) {
#ifdef USE_MINMAX
#ifdef USE_SSE
    return _mm_movemask_ps(_mm_and_ps(_mm_cmple_ps(lhs.min.xmm, rhs.max.xmm), _mm_cmpge_ps(lhs.max.xmm, rhs.min.xmm))) == 0xFFFFFF00;
#else
    return lhs.min.x <= rhs.max.x && lhs.min.y <= rhs.max.y && lhs.min.z <= rhs.max.z &&
           lhs.max.x >= rhs.min.x && lhs.max.y >= rhs.min.y && lhs.max.z >= rhs.min.z;
#endif // USE_SSE
#else
#ifdef USE_SSE
    __m128 abs_xmm = _mm_andnot_ps(_mm_set_ps1(-0.f), _mm_sub_ps(lhs.center.xmm, rhs.center.xmm));
    return _mm_movemask_ps(_mm_cmple_ps(abs_xmm, _mm_add_ps(lhs.extent.xmm, rhs.extent.xmm))) == 0xFFFFFF00;
#else
    return std::abs(lhs.center.x - rhs.center.x) <= lhs.extent.x + rhs.extent.x &&
           std::abs(lhs.center.y - rhs.center.y) <= lhs.extent.y + rhs.extent.y &&
           std::abs(lhs.center.z - rhs.center.z) <= lhs.extent.z + rhs.extent.z;
#endif // USE_SSE
#endif // USE_MINMAX
}

void consume_result(const aabbox& box);
void consume_result(bool flag);
