#include "easing.h"

// quadratic ease in out
double ease_in_out(double min, double max, double x) {
	return x < 0.5 ? 2 * x*x : x * (4 - 2*x) - 1;
}

double ease(double min, double max, double x) {
    return ease_in_out(min, max, x);
}

