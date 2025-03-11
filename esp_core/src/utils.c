#include "utils.h"

// quadratic ease in out derivative
double ease_in_out(double x) {
	return x < 0.5 ? 4 * x : 4 * (1 - x);
}

// 4th power smoothing function
double ease_p4(double x) {
    double p4_term = (2 *x - 1);
    p4_term = p4_term*p4_term*p4_term*p4_term;
    return 1 - p4_term;
}

double ease(double min, double max, double x) {
    return min + (max - min) * ease_p4(x);
}

double lerp(double min, double max, double x) {
    return min + (max - min)*x;
}

double clamp(double min, double max, double num) {
    num = num < max ? num : max;
    num = num > min ? num : min;
    return num;
}
