#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

struct Point3D {
    double x;
    double y;
    double z;
};

class NURBSSurface {
public:
    NURBSSurface(int uCount,
                int vCount,
                int degreeU,
                int degreeV,
                vector<double> knotsU,
                vector<double> knotsV,
                vector<vector<Point3D>> controlPoints,
                vector<vector<double>> weights)
        : uCount_(uCount),
            vCount_(vCount),
            degreeU_(degreeU),
            degreeV_(degreeV),
            knotsU_(move(knotsU)),
            knotsV_(move(knotsV)),
            controlPoints_(move(controlPoints)),
            weights_(move(weights)) {}

    Point3D evaluate(double u, double v) const {
        const auto nu = basisFunctions(uCount_ - 1, degreeU_, knotsU_, clamp(u, knotsU_.front(), knotsU_.back()));
        const auto nv = basisFunctions(vCount_ - 1, degreeV_, knotsV_, clamp(v, knotsV_.front(), knotsV_.back()));

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

        if (abs(weightSum) < 1e-12) {
            return {0.0, 0.0, 0.0};
        }

        return {sumX / weightSum, sumY / weightSum, sumZ / weightSum};
    }

private:
    static double basisFunction(int i, int p, const vector<double>& U, double u) {
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

    static vector<double> basisFunctions(int n, int p, const vector<double>& U, double u) {
        vector<double> N(n + 1, 0.0);
        for (int i = 0; i <= n; ++i) {
            N[i] = basisFunction(i, p, U, u); // Compute the basis function N_{i,p}(u)
        }

        return N;
    }

    int uCount_;
    int vCount_;
    int degreeU_;
    int degreeV_;
    vector<double> knotsU_;
    vector<double> knotsV_;
    vector<vector<Point3D>> controlPoints_;
    vector<vector<double>> weights_;
};

int main() {
    const int uCount = 4;
    const int vCount = 4;
    const int degreeU = 2;
    const int degreeV = 2;

    vector<double> knotsU = {0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 1.0};
    vector<double> knotsV = {0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 1.0};

    vector<vector<Point3D>> controlPoints(uCount, vector<Point3D>(vCount));
    vector<vector<double>> weights(uCount, vector<double>(vCount, 1.0));

    for (int i = 0; i < uCount; ++i) {
        for (int j = 0; j < vCount; ++j) {
            controlPoints[i][j].x = static_cast<double>(i) * 2.0;
            controlPoints[i][j].y = static_cast<double>(j) * 2.0;
            controlPoints[i][j].z = sin((i + 1) * 0.8) * cos((j + 1) * 0.9);
            weights[i][j] = 1.0 + 0.40 * sin((i + 1) * 0.9) * cos((j + 1) * 0.7);
        }
    }

    NURBSSurface surface(uCount, vCount, degreeU, degreeV, knotsU, knotsV, controlPoints, weights);

    cout << fixed << setprecision(6);
    cout << "NURBS surface sample points:\n";

    for (double u = 0.0; u <= 1.0 + 1e-9; u += 0.25) {
        for (double v = 0.0; v <= 1.0 + 1e-9; v += 0.25) {
            const Point3D p = surface.evaluate(clamp(u, 0.0, 1.0), clamp(v, 0.0, 1.0));
            cout << "u=" << u << ", v=" << v << " -> ("<< p.x << ", " << p.y << ", " << p.z << ")\n";
        }
    }

    return 0;
}
