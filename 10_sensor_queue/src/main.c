#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
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

typedef struct {
    SensorId sensor;
    uint32_t periodMs;
    UBaseType_t priority;
    const char* name;
} ProducerConfig;

static const ProducerConfig kProducerConfigs[] = {
    {.sensor = kSensorFirst,
     .periodMs = kFirstProducerPeriodMs,
     .priority = kFirstProducerPriority,
     .name = "ProducerFirst"},
    {.sensor = kSensorSecond,
     .periodMs = kSecondProducerPeriodMs,
     .priority = kSecondProducerPriority,
     .name = "ProducerSecond"},
};

static QueueHandle_t sensorQueueHandle = NULL;

static void producerTask(void* params) {
    const ProducerConfig* config = params;
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(config->periodMs);
    while (true) {
        xTaskDelayUntil(&lastWake, period);
        const SensorReading reading = {
            .sensor = config->sensor, .value = (uint8_t)esp_random(), .timestampTicks = xTaskGetTickCount()};
        const BaseType_t result = xQueueSend(sensorQueueHandle, &reading, pdMS_TO_TICKS(kProducerTimeoutMs));
        if (result != pdPASS) {
            printf("[%s] Queue full, reading dropped...\n", config->name);
        }
    }
}

static const char* sensorName(SensorId id) {
    switch (id) {
        case kSensorFirst:
            return "First";
        case kSensorSecond:
            return "Second";
    }
    return "Unknown";
}

static void consumerTask(void* params) {
    (void)params;
    while (true) {
        SensorReading reading = {0};
        const BaseType_t result = xQueueReceive(sensorQueueHandle, &reading, pdMS_TO_TICKS(kConsumerTimeoutMs));
        if (result != pdPASS) {
            printf("[Consumer] No data for %" PRIu32 " ms...\n", kConsumerTimeoutMs);
            continue;
        }
        printf("[Consumer] Sensor %s: value %" PRIu8 ", read at %" PRIu32 " ms\n", sensorName(reading.sensor),
               reading.value, pdTICKS_TO_MS(reading.timestampTicks));
    }
}

void app_main(void) {}
