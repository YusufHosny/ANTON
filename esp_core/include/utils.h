#ifndef EASING_H
#define EASING_H

typedef double (*EasingFunction)(double, double, double);

double ease(double min, double max, double x);
double lerp(double min, double max, double x);
double clamp(double min, double max, double num);


void log_sizes();


// half period in terms of freq
// divided by 2 since this number is used to flip the step pin, 2 flips per period
#define freq_to_usec(freq) (1000000. / freq) / 2


#endif // EASING_H