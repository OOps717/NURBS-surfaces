#include "nurbs.h"
#include <utility>
#include <limits>
#include <stdexcept>

NURBSSurface::NURBSSurface(int uCount,
        int vCount,
        int degreeU,
        int degreeV,
        std::vector<double> knotsU,
        std::vector<double> knotsV,
        std::vector<std::vector<Point3D>> controlPoints,
        std::vector<std::vector<double>> weights)
        : uCount_(uCount),
            vCount_(vCount),
            degreeU_(degreeU),
            degreeV_(degreeV),
            knotsU_(move(knotsU)),
            knotsV_(move(knotsV)),
            controlPoints_(move(controlPoints)),
            weights_(move(weights)) {}

Point3D NURBSSurface::evaluate(double u, double v) const {
    const auto nu = basisFunctions(uCount_ - 1, degreeU_, knotsU_, std::clamp(u, knotsU_.front(), knotsU_.back()));
    const auto nv = basisFunctions(vCount_ - 1, degreeV_, knotsV_, std::clamp(v, knotsV_.front(), knotsV_.back()));

    double sumX = 0.0;
    double sumY = 0.0;
    double sumZ = 0.0;
    double weightSum = 0.0;

    for (int i = 0; i < uCount_; ++i) {
        for (int j = 0; j < vCount_; ++j) {
            const double w = weights_[i][j] * nu[i] * nv[j];
            const Point3D& p = controlPoints_[i][j];

            sumX += w * p.x;
            sumY += w * p.y;
            sumZ += w * p.z;
            weightSum += w;
        }
    }

    if (std::abs(weightSum) < 1e-12) {
        return {0.0, 0.0, 0.0};
    }

    return {sumX / weightSum, sumY / weightSum, sumZ / weightSum};
}

SurfaceMeshData NURBSSurface::generateMesh(int segmentsU, int segmentsV) const {
    if (segmentsU <= 0 || segmentsV <= 0) {
        throw std::invalid_argument("Mesh segment counts must be positive");
    }

    const std::size_t rows = static_cast<std::size_t>(segmentsU) + 1;
    const std::size_t columns = static_cast<std::size_t>(segmentsV) + 1;
    if (rows > std::numeric_limits<unsigned int>::max() / columns) {
        throw std::length_error("Mesh has too many vertices for unsigned int indices");
    }

    SurfaceMeshData mesh;
    mesh.vertices.reserve(rows * columns * 5);
    mesh.indices.reserve((rows - 1) * (columns - 1) * 6);

    const double uMin = knotsU_[degreeU_];
    const double uMax = knotsU_[uCount_];
    const double vMin = knotsV_[degreeV_];
    const double vMax = knotsV_[vCount_];

    for (std::size_t i = 0; i < rows; ++i) {
        const double textureU = static_cast<double>(i) / segmentsU;
        const double u = i == rows - 1 ? uMax : uMin + textureU * (uMax - uMin);
        for (std::size_t j = 0; j < columns; ++j) {
            const double textureV = static_cast<double>(j) / segmentsV;
            const double v = j == columns - 1 ? vMax : vMin + textureV * (vMax - vMin);
            const Point3D point = evaluate(u, v);
            mesh.vertices.insert(mesh.vertices.end(), {
                static_cast<float>(point.x), static_cast<float>(point.y),
                static_cast<float>(point.z), static_cast<float>(textureU),
                static_cast<float>(textureV)
            });
        }
    }

    for (std::size_t i = 0; i + 1 < rows; ++i) {
        for (std::size_t j = 0; j + 1 < columns; ++j) {
            const auto a = static_cast<unsigned int>(i * columns + j);
            const auto b = static_cast<unsigned int>((i + 1) * columns + j);
            const unsigned int c = a + 1;
            const unsigned int d = b + 1;
            mesh.indices.insert(mesh.indices.end(), {a, b, c, b, d, c});
        }
    }
    return mesh;
}

double NURBSSurface::basisFunction(int i, int p, const std::vector<double>& U, double u) {
    if (p == 0) {
        const bool inInterval = U[i] <= u && u < U[i + 1];
        // case if u is equal to the last knot value, we need to check if it is at the right end of the knot vector
        const bool atRightEnd = u == U.back() && U[i] < U[i + 1] && U[i + 1] == U.back();
        return (inInterval || atRightEnd) ? 1.0 : 0.0;
    }

    double value = 0.0;
    const double leftDenominator = U[i + p] - U[i];
    const double rightDenominator = U[i + p + 1] - U[i + 1];

    if (leftDenominator != 0.0) {
        value += (u - U[i]) / leftDenominator * basisFunction(i, p - 1, U, u); // N_{i,p-1}(u)
    }
    if (rightDenominator != 0.0) {
        value += (U[i + p + 1] - u) / rightDenominator * basisFunction(i + 1, p - 1, U, u); // N_{i+1,p-1}(u)
    }
    return value;
}

std::vector<double> NURBSSurface::basisFunctions(int n, int p, const std::vector<double>& U, double u) {
    std::vector<double> N(n + 1, 0.0);
    for (int i = 0; i <= n; ++i) {
        N[i] = NURBSSurface::basisFunction(i, p, U, u); // Compute the basis function N_{i,p}(u)
    }

    return N;
}

void NURBSSurface::setControlPoint(int i, int j, Point3D point)
{
    controlPoints_.at(i).at(j) = point;
}
