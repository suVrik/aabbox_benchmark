#include "shared.h"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>

constexpr size_t TEST_COUNT = 10000000;

int main(int argc, char* argv[]) {
    //
    // Generate data.
    //

    float4 float3_data[8192];
    aabbox aabbox_data[4096];

    for (float4& value : float3_data) {
        value.x = static_cast<float>(rand()) / RAND_MAX;
        value.y = static_cast<float>(rand()) / RAND_MAX;
        value.z = static_cast<float>(rand()) / RAND_MAX;
    }

    for (aabbox& value : aabbox_data) {
#ifdef USE_MINMAX
        value.min.x = static_cast<float>(rand()) / RAND_MAX;
        value.min.y = static_cast<float>(rand()) / RAND_MAX;
        value.min.z = static_cast<float>(rand()) / RAND_MAX;
        value.max.x = static_cast<float>(rand()) / RAND_MAX;
        value.max.y = static_cast<float>(rand()) / RAND_MAX;
        value.max.z = static_cast<float>(rand()) / RAND_MAX;
#else
        value.center.x = static_cast<float>(rand()) / RAND_MAX;
        value.center.y = static_cast<float>(rand()) / RAND_MAX;
        value.center.z = static_cast<float>(rand()) / RAND_MAX;
        value.extent.x = static_cast<float>(rand()) / RAND_MAX;
        value.extent.y = static_cast<float>(rand()) / RAND_MAX;
        value.extent.z = static_cast<float>(rand()) / RAND_MAX;
#endif
    }

    //
    // operator+(float4)
    //

    auto t1 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < TEST_COUNT; i++) {
        size_t float3_idx = i & 8191;
        size_t aabbox_idx = i & 4095;

        aabbox result = aabbox_data[aabbox_idx] + float3_data[float3_idx];

        consume_result(result);
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    
    //
    // operator+(aabbox)
    //

    auto t3 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0, j = 1337; i < TEST_COUNT; i++, j++) {
        size_t aabbox_lhs_idx = i & 4095;
        size_t aabbox_rhs_idx = j & 4095;

        aabbox result = aabbox_data[aabbox_lhs_idx] + aabbox_data[aabbox_rhs_idx];

        consume_result(result);
    }

    auto t4 = std::chrono::high_resolution_clock::now();

    //
    // intersect(float4)
    //

    auto t5 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < TEST_COUNT; i++) {
        size_t float3_idx = i & 8191;
        size_t aabbox_idx = i & 4095;

        bool result = intersect(float3_data[float3_idx], aabbox_data[aabbox_idx]);

        consume_result(result);
    }

    auto t6 = std::chrono::high_resolution_clock::now();

    //
    // intersect(aabbox)
    //

    auto t7 = std::chrono::high_resolution_clock::now();

    for (size_t i = 0, j = 1337; i < TEST_COUNT; i++, j++) {
        size_t aabbox_lhs_idx = i & 4095;
        size_t aabbox_rhs_idx = j & 4095;

        bool result = intersect(aabbox_data[aabbox_lhs_idx], aabbox_data[aabbox_rhs_idx]);

        consume_result(result);
    }

    auto t8 = std::chrono::high_resolution_clock::now();

    //
    // Print results.
    //

    long long t12 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    long long t34 = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count();
    long long t56 = std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count();
    long long t78 = std::chrono::duration_cast<std::chrono::nanoseconds>(t8 - t7).count();

    std::cout << "operator+(float4): " << std::setw(10) << t12 << "ns" << std::endl;
    std::cout << "operator+(aabbox): " << std::setw(10) << t34 << "ns" << std::endl;
    std::cout << "intersect(float4): " << std::setw(10) << t56 << "ns" << std::endl;
    std::cout << "intersect(aabbox): " << std::setw(10) << t78 << "ns" << std::endl;

    return 0;
}
