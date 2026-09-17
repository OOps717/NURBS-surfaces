#include "nurbs.h"

#include <iomanip>
#include <iostream>
#include <vector>

int main()
{
    const std::vector<double> knots{0.0, 0.0, 1.0, 1.0};
    const std::vector<std::vector<Point3D>> controlPoints{
        {{0.0, 0.0, 0.0}, {0.0, 2.0, 1.0}},
        {{2.0, 0.0, 1.0}, {2.0, 2.0, 0.0}}
    };
    const std::vector<std::vector<double>> weights{
        {1.0, 1.0},
        {1.0, 1.0}
    };

    NURBSSurface surface(2, 2, 1, 1, knots, knots, controlPoints, weights);

    std::cout << std::fixed << std::setprecision(3);
    for (int i = 0; i <= 4; ++i) {
        for (int j = 0; j <= 4; ++j) {
            const double u = i / 4.0;
            const double v = j / 4.0;
            const Point3D point = surface.evaluate(u, v);
            std::cout << "u=" << u << ", v=" << v << " -> ("
                        << point.x << ", " << point.y << ", " << point.z << ")\n";
        }
    }

    return 0;
}
