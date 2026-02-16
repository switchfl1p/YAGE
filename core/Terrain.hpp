/* switchfl1p 2025-2026 */
#pragma once
#include <vector>

// Perlin noise - smooth, natural looking
// Simplex noise - faster than Perlin, less directional artifacts
// Fractional Brownian Motion (fBM) - layered noise for detail

struct TerrainData{
    std::vector<float> heights;
    int width;
    int depth;
    float scale;
};

TerrainData generateTerrain(int width, int depth, float amplitute, float frequency);