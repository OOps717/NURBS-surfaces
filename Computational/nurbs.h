#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <vector>

struct Point3D {
    double x;
    double y;
    double z;
};

struct SurfaceMeshData {
    std::vector<float> vertices; // x, y, z, u, v; UV coordinates are in [0, 1].
    std::vector<unsigned int> indices;
};

class NURBSSurface {
public:
    NURBSSurface(int uCount,
                int vCount,
                int degreeU,
                int degreeV,
                std::vector<double> knotsU,
                std::vector<double> knotsV,
                std::vector<std::vector<Point3D>> controlPoints,
                std::vector<std::vector<double>> weights);

    Point3D evaluate(double u, double v) const ;

    SurfaceMeshData generateMesh(int segmentsU, int segmentsV) const;

private:
    static double basisFunction(int i, int p, const std::vector<double>& U, double u);
    static std::vector<double> basisFunctions(int n, int p, const std::vector<double>& U, double u);

    int uCount_;
    int vCount_;
    int degreeU_;
    int degreeV_;
    std::vector<double> knotsU_;
    std::vector<double> knotsV_;
    std::vector<std::vector<Point3D>> controlPoints_;
    std::vector<std::vector<double>> weights_;
};
