#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const uint32_t kTaskStackSizeBytes = 3072;
static const uint32_t kFirstProducerPeriodMs = 100;
static const uint32_t kSecondProducerPeriodMs = 200;
static const uint32_t kConsumerTimeoutMs = 500;
static const uint32_t kProducerTimeoutMs = 50;

static const UBaseType_t kSecondProducerPriority = tskIDLE_PRIORITY + 2;
static const UBaseType_t kFirstProducerPriority = tskIDLE_PRIORITY + 3;
static const UBaseType_t kConsumerPriority = tskIDLE_PRIORITY + 4;

static const UBaseType_t kQueueLength = 10;

static const BaseType_t kAppCore = 1;

typedef enum {
    kSensorFirst,
    kSensorSecond,
} SensorId;

typedef struct {
    SensorId sensor;
    uint8_t value;
    TickType_t timestampTicks;
} SensorReading;

void app_main(void) {}
