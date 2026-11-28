#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
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
    kSensorCount,
} SensorId;

typedef struct {
    SensorId sensor;
    uint32_t seq;
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
    [kSensorFirst] = {.sensor = kSensorFirst,
                      .periodMs = kFirstProducerPeriodMs,
                      .priority = kFirstProducerPriority,
                      .name = "ProducerFirst"},
    [kSensorSecond] = {.sensor = kSensorSecond,
                       .periodMs = kSecondProducerPeriodMs,
                       .priority = kSecondProducerPriority,
                       .name = "ProducerSecond"},
};

static_assert(sizeof(kProducerConfigs) / sizeof(kProducerConfigs[0]) == kSensorCount,
              "kProducerConfigs must have one row per sensor");

static QueueHandle_t sensorQueueHandle = NULL;

static void producerTask(void* params) {
    const ProducerConfig* config = params;
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(config->periodMs);
    uint32_t seq = 0;
    while (true) {
        xTaskDelayUntil(&lastWake, period);
        const SensorReading reading = {
            .sensor = config->sensor,
            .seq = seq,
            .value = (uint8_t)esp_random(),
            .timestampTicks = xTaskGetTickCount(),
        };
        ++seq;
        const BaseType_t result = xQueueSend(sensorQueueHandle, &reading, pdMS_TO_TICKS(kProducerTimeoutMs));
        if (result != pdPASS) {
            printf("[%s] Queue full, reading dropped...\n", config->name);
        }
    }
}

static const char* sensorName(SensorId sensorId) {
    switch (sensorId) {
        case kSensorFirst:
            return "First";
        case kSensorSecond:
            return "Second";
        case kSensorCount:
            break;
    }
    return "Unknown";
}

static void consumerTask(void* params) {
    (void)params;
    uint32_t nextSeq[kSensorCount] = {0};
    while (true) {
        SensorReading reading = {0};
        const BaseType_t result = xQueueReceive(sensorQueueHandle, &reading, pdMS_TO_TICKS(kConsumerTimeoutMs));
        if (result != pdPASS) {
            printf("[Consumer] No data for %" PRIu32 " ms...\n",
                   kConsumerTimeoutMs);  // cppcheck-suppress invalidPrintfArgType_uint ; uint32_t is unsigned long on
                                         // Xtensa, PRIu32 is correct
            continue;
        }
        if (reading.sensor >= kSensorCount) {
            printf("[Consumer] Unknown sensor %d, reading dropped...\n",
                   reading.sensor);  // cppcheck-suppress invalidPrintfArgType_uint ; uint32_t is unsigned long on
                                     // Xtensa, PRIu32 is correct
            continue;
        }
        if (reading.seq > nextSeq[reading.sensor]) {
            printf("[Consumer] Sensor %s: %" PRIu32 " readings lost...\n", sensorName(reading.sensor),
                   reading.seq - nextSeq[reading.sensor]);
        }
        nextSeq[reading.sensor] = reading.seq + 1;
        printf("[Consumer] Sensor %s: #%" PRIu32 " value %" PRIu8 ", read at %" PRIu32
               " ms...\n",  // cppcheck-suppress invalidPrintfArgType_uint ; uint32_t is unsigned long on Xtensa, PRIu32
                            // is correct
               sensorName(reading.sensor), reading.seq, reading.value,
               pdTICKS_TO_MS(reading.timestampTicks));  // cppcheck-suppress invalidPrintfArgType_uint ; uint32_t is
                                                        // unsigned long on Xtensa, PRIu32 is correct
    }
}

static void createTask(TaskFunction_t taskFunction, const char* taskName, uint32_t stackSizeBytes, void* params,
                       UBaseType_t priority, TaskHandle_t* taskHandle) {
    const BaseType_t result =
        xTaskCreatePinnedToCore(taskFunction, taskName, stackSizeBytes, params, priority, taskHandle, kAppCore);
    if (result != pdPASS) {
        printf("Failed to create task %s...\n", taskName);
    }
}

void app_main(void) {
    sensorQueueHandle = xQueueCreate(kQueueLength, sizeof(SensorReading));
    if (sensorQueueHandle == NULL) {
        printf("Failed to create sensor queue...\n");
        return;
    }
    createTask(consumerTask, "Consumer", kTaskStackSizeBytes, NULL, kConsumerPriority, NULL);
    const size_t producerCount = sizeof(kProducerConfigs) / sizeof(kProducerConfigs[0]);
    for (size_t i = 0; i < producerCount; ++i) {
        const ProducerConfig* config = &kProducerConfigs[i];
        if (config->sensor != (SensorId)i || config->name == NULL) {
            printf("Invalid producer config at row %zu...\n", i);
            continue;
        }
        createTask(producerTask, config->name, kTaskStackSizeBytes, (void*)config, config->priority, NULL);
    }
}
