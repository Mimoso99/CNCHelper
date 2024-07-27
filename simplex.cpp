/**
 * This file contains the following function definitions for linear programming problems:
 * - is_feasible
 * - Simplex
 * - Midpoint
 */

// Include headers & libraries
#include <iostream>   // for standard C++ library input/output
#include <cfloat>     // for constants related to floating point values
#include <cmath>      // for mathematical functions
#include <vector>     // for using std::vector
#include "chipload.h" // for external user defined functions

/**
 * Function: Check if a Point p is feasible within given bounds and constraints.
 *
 * Parameters:
 * @param p The point to be checked for feasibility.
 * @param x_min The minimum x value allowed.
 * @param x_max The maximum x value allowed.
 * @param y_max The maximum y value allowed.
 * @param a The slope of the upper bound straight, a constraint of the feasible region
 * @param b The slope of the lower bound straight, a constraint of the feasible region
 * 
 * Return:
 * @return true if the point is feasible, false otherwise.
 */
bool is_feasible(const Point &p, int x_min, int x_max, int y_max, float a, float b) {
    return (x_min <= p.x && p.x <= x_max &&
            0 <= p.y && p.y <= y_max &&
            p.y >= b * p.x &&
            p.y <= a * p.x);
}

/**
 * Function: Implements the Simplex algorithm to find the optimal point based on given constraints.
 * It iterates over possible x or y values within bounds to maximize the objective function value.
 *
 * Parameters:
 * @param x_min The minimum x value allowed.
 * @param x_max The maximum x value allowed.
 * @param y_max The maximum y value allowed.
 * @param a The slope of the upper bound straight, a constraint of the feasible region
 * @param b The slope of the lower bound straight, a constraint of the feasible region
 * @param maximize_y Flag to indicate whether to maximize y (true) or x (false).
 * 
 * Return:
 * @return The optimal point that maximizes the objective function value within the constraints.
 */
Point Simplex(int x_min, int x_max, int y_max, float a, float b, bool maximize_y) {
    Point best_point = {0, 0};
    float best_value = -FLT_MAX; // Initialize to the lowest possible negative float to ensure maximization

    if (maximize_y) {
        // Maximize y by iterating over possible x values
        for (int x = x_min; x <= x_max; x += 100) {
            int y_min = static_cast<int>(b * x);
            int y_max_limit = static_cast<int>(a * x);

            // Adjust y_min and y_max_limit to be within bounds
            y_min = std::max(0, y_min);
            y_max_limit = std::min(y_max, y_max_limit);

            for (int y = y_min; y <= y_max_limit; y += 50) {
                Point p = {x, y};
                if (is_feasible(p, x_min, x_max, y_max, a, b) && p.y > best_value) {
                    best_point = p;
                    best_value = p.y;
                }
            }
        }
    } else {
        // Maximize x by iterating over possible y values
        for (int y = 0; y <= y_max; y += 50) {
            int x_min_limit = static_cast<int>(y / b);
            int x_max_limit = static_cast<int>(y / a);

            // Adjust x_min_limit and x_max_limit to be within bounds
            x_min_limit = std::max(x_min, x_min_limit);
            x_max_limit = std::min(x_max, x_max_limit);

            for (int x = x_min_limit; x <= x_max_limit; x += 100) {
                Point p = {x, y};
                if (is_feasible(p, x_min, x_max, y_max, a, b) && p.x > best_value) {
                    best_point = p;
                    best_value = p.x;
                }
            }
        }
    }

    return best_point;
}

/**
 * Function: Calculate the midpoint of a given range based on the provided constraints.
 *
 * Parameters:
 * @param x_min The minimum x value allowed.
 * @param x_max The maximum x value allowed.
 * @param y_max The maximum y value allowed.
 * @param c The slope of the straight-segment for the midpoint
 * 
 * Return:
 * @return The midpoint point that satisfies the constraints.
 */
Point Midpoint(int x_min, int x_max, int y_max, float c) {
    int mid_x = (x_min + x_max) / 2;
    int y_min_at_mid_x = static_cast<int>(c * mid_x);

    // Adjust y_min_at_mid_x to be within bounds
    y_min_at_mid_x = std::max(MIN_Y, y_min_at_mid_x);

    int y_max_at_mid_x = y_max;
    int mid_y = (y_min_at_mid_x + y_max_at_mid_x) / 2;

    Point midpoint = {mid_x, mid_y};

    // Check feasibility of the midpoint
    if (!is_feasible(midpoint, x_min, x_max, y_max, c, 0)) {
        // Adjust to the nearest feasible point if not feasible
        if (mid_y < c * mid_x) {
            mid_y = static_cast<int>(c * mid_x);
        }
        midpoint.y = mid_y;
    }

    return midpoint;
}
