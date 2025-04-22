#include "utils.h"
#include "config.h"
#include <math.h>

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

double ease_exp(double x) {
    return exp(0.7*x) - 1;
}

double ease(double min, double max, double x) {
    return min + (max - min) * ease_exp(x);
}

double lerp(double min, double max, double x) {
    return min + (max - min)*x;
}

double clamp(double min, double max, double num) {
    num = num < max ? num : max;
    num = num > min ? num : min;
    return num;
}

void log_sizes() {
	StepMessage_t smsg;
	RacketMessage_t rmsg;
	Packet_t p;

	ESP_LOGI(pcTaskGetName(NULL), "packet size %d", sizeof p);
	ESP_LOGI(pcTaskGetName(NULL), "stepmsg size %d", sizeof smsg);
	ESP_LOGI(pcTaskGetName(NULL), "racketmsg size %d", sizeof rmsg);
	ESP_LOGI(pcTaskGetName(NULL), "bool size %d", sizeof(bool));
	ESP_LOGI(pcTaskGetName(NULL), "double size %d", sizeof(double));
	ESP_LOGI(pcTaskGetName(NULL), "float size %d", sizeof(float));
	ESP_LOGI(pcTaskGetName(NULL), "stepmsgtypet size %d", sizeof(StepMessageType_t));

	ESP_LOGI(pcTaskGetName(NULL), "&s %p", &smsg);
	ESP_LOGI(pcTaskGetName(NULL), "&s.type %p", &smsg.type);
	ESP_LOGI(pcTaskGetName(NULL), "&s.pos %p", &smsg.position);

	ESP_LOGI(pcTaskGetName(NULL), "&r %p", &rmsg);
	ESP_LOGI(pcTaskGetName(NULL), "&r.angle %p", &rmsg.angle);
	ESP_LOGI(pcTaskGetName(NULL), "&r.fir %p", &rmsg.fire);

	ESP_LOGI(pcTaskGetName(NULL), "&p %p", &p);
	ESP_LOGI(pcTaskGetName(NULL), "&p.h %p", &p.hMsg);
	ESP_LOGI(pcTaskGetName(NULL), "&p.v %p", &p.vMsg);
	ESP_LOGI(pcTaskGetName(NULL), "&p.r %p", &p.rMsg);
}
