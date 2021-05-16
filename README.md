# AABBox Benchmark

For proper benchmark source must be compiled in release mode with whole program optimization turned off.
The center+extent AABBox layout performs better in AABBox-Frustum intersection test (not included here), which speeds up the frustum culling.
The min+max AABBox layout performs better in constructing AABBox from bunch of points or other AABBoxes.

## Center+extent AABBox layout, without SSE

| Function | Time |
|-|-|
| operator+(float4): | 106053100ns |
| operator+(aabbox): | 145008400ns |
| intersect(float4): | 86422800ns |
| intersect(aabbox): | 58851000ns |

## Center+extent AABBox layout, with SSE

| Function | Time | Improvement* |
|-|-|-|
| operator+(float4): | 40955700ns | 2.59 |
| operator+(aabbox): | 42973200ns | 3.37 |
| intersect(float4): | 16630500ns | 5.20 |
| intersect(aabbox): | 16431800ns | 3.58 |

*\* over center+extent AABBox layout, without SSE*

## Min+max AABBox layout, without SSE

| Function | Time | Improvement* |
|-|-|-|
| operator+(float4): | 61488100ns | 1.72 |
| operator+(aabbox): | 67009900ns | 2.16 |
| intersect(float4): | 72047100ns | 1.20 |
| intersect(aabbox): | 72235600ns | 0.81 |

*\* over center+extent AABBox layout, without SSE*

## Min+max AABBox layout, with SSE

| Function | Time | Improvement* |
|-|-|-|
| operator+(float4): | 15794400ns | 2.59 |
| operator+(aabbox): | 14395800ns | 2.99 |
| intersect(float4): | 16317700ns | 1.02 |
| intersect(aabbox): | 15112000ns | 1.09 |

*\* over center+extent AABBox layout, with SSE*
