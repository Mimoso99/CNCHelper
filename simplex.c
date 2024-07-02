/**
 * This file contains the following function definitions for linear programming problems:
 * - is_feasible
 * - Simplex
 * - Midpoint
 */

// Include headers & libraries (<stdbool.h> is included in the header "chipload.h")
#include <float.h>      // for constants related to floating point values
#include <stdio.h>      // for standard input/output operations
#include <stdlib.h>     // for memory allocation
#include "chipload.h"   // for external user defined functions


/**
 * Function: check if a Point p is feasible within given bounds and constraints.
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
 * @return 1 if the point is feasible, 0 otherwise (could be a bool, was implemented before knowing about stdbool.h).
 */
int is_feasible(Point p, int x_min, int x_max, int y_max, float a, float b) {
    return (x_min <= p.x && p.x <= x_max &&
            0 <= p.y && p.y <= y_max &&
            p.y >= b * p.x &&
            p.y <= a * p.x);
}


/**
 * Function: implements the Simplex algorithm to find the optimal point based on given constraints.
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
    float best_value = -FLT_MAX; // Initialize best_value to the lowest possible negative float. This ensures the maximization process runs as intended

    // maximizing y is to go for the highest possible feedrate
    if (maximize_y) {
        // Enumerate over possible x values in 100 increments to find the best y
        for (int x = x_min; x <= x_max; x += 100) {
            int y_min = (int)(b * x);
            int y_max_limit = (int)(a * x);

            // Consider the y values within bounds in 50 increments
            if (y_min < 0) y_min = 0;
            if (y_max_limit > y_max) y_max_limit = y_max;

            // Check feasibility and update best point if necessary
            for (int y = y_min; y <= y_max_limit; y += 50) {
                Point p = {x, y};
                if (is_feasible(p, x_min, x_max, y_max, a, b) && p.y > best_value) {
                    best_point = p;
                    best_value = p.y;
                }
            }
        }
    // maximizing x is to go for the highest possible rotational speed
    } else {
        // Enumerate over possible y values in 50 increments to find the best x
        for (int y = 0; y <= y_max; y += 50) {
            int x_min_limit = (int)(y / a);
            int x_max_limit = (int)(y / b);

            // Consider the x values within bounds in 100 increments
            if (x_min_limit < x_min) x_min_limit = x_min;
            if (x_max_limit > x_max) x_max_limit = x_max;

            // Check feasibility and update best point if necessary
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
 * Function: calculate the Midpoint of a given range based on the provided constraints.
 *
 * Parameters:
 * @param x_min The minimum x value allowed.
 * @param x_max The maximum x value allowed.
 * @param y_max The maximum y value allowed.
 * @param c The slope of the straight-segment for the midpoint
 * 
 * Return:
 * @return The Midpoint point that satisfies the constraints.
 */
Point Midpoint(int x_min, int x_max, int y_max, float c) {
    int mid_x = (x_min + x_max) / 2;
    int y_min_at_mid_x = (int)(c * mid_x);

    // Adjust y_min_at_mid_x to be within bounds
    if (y_min_at_mid_x < MIN_Y) y_min_at_mid_x = MIN_Y;

    int y_max_at_mid_x = y_max;
    int mid_y = (y_min_at_mid_x + y_max_at_mid_x) / 2;

    Point Midpoint = {mid_x, mid_y};

    // Check feasibility of the Midpoint
    if (!is_feasible(Midpoint, x_min, x_max, y_max, c, 0)) {
        // If the Midpoint is not feasible, adjust to the nearest feasible point
        if (mid_y < c * mid_x) {
            mid_y = (int)(c * mid_x);
        }
        Midpoint.y = mid_y;
    }

    return Midpoint;
}