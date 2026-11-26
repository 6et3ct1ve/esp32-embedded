#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const uint32_t kTaskStackSizeBytes = 2048;
static const uint32_t kSensorPeriodMs = 500;
static const uint32_t kMaintenancePeriodMs = 10000;
static const uint32_t kMaintenancePauseMs = 2000;
static const uint32_t kEventTriggerDelayMs = 3000;
static const uint32_t kSafePointWaitMs = 100;

static const UBaseType_t kLowPriority = tskIDLE_PRIORITY + 2;
static const UBaseType_t kNormalPriority = tskIDLE_PRIORITY + 3;
static const UBaseType_t kHighPriority = tskIDLE_PRIORITY + 4;
static const UBaseType_t kRealtimePriority = tskIDLE_PRIORITY + 5;

static const BaseType_t kAppCore = 1;

static TaskHandle_t sensorTaskHandle = NULL;

static void sensorReaderTask(void *params) {
    (void)params;
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(kSensorPeriodMs);
    while (true) {
        xTaskDelayUntil(&lastWake, period);
        const TickType_t late = xTaskGetTickCount() - lastWake;
        if (late >= period) {
            lastWake = xTaskGetTickCount() - (late % period);
            continue;
        }
        printf("[Sensor] Data read...\n");
    }
}

static void workerTask(void *params) {
    (void)params;
    printf("[Worker] Processing event...\n");
    vTaskDelete(NULL);
}

static void createTask(TaskFunction_t taskFunction, const char *taskName, uint32_t stackSizeBytes, void *params,
                       UBaseType_t priority, TaskHandle_t *taskHandle) {
    const BaseType_t result =
        xTaskCreatePinnedToCore(taskFunction, taskName, stackSizeBytes, params, priority, taskHandle, kAppCore);
    if (result != pdPASS) {
        printf("Failed to create task %s\n", taskName);
    }
}

static void eventTriggerTask(void *params) {
    (void)params;
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(kEventTriggerDelayMs));
        printf("[Event] Triggered...\n");
        createTask(workerTask, "Worker", kTaskStackSizeBytes, NULL, kHighPriority, NULL);
    }
}

static bool waitSensorIdle(void) {
    const TickType_t start = xTaskGetTickCount();
    while (eTaskGetState(sensorTaskHandle) != eBlocked) {
        if (xTaskGetTickCount() - start >= pdMS_TO_TICKS(kSafePointWaitMs)) {
            return false;
        }
        vTaskDelay(1);
    }
    return true;
}

static void maintenanceModeTask(void *params) {
    (void)params;
    if (sensorTaskHandle == NULL) {
        printf("[Maintenance] Sensor task missing, stopping...\n");
        vTaskDelete(NULL);
    }
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(kMaintenancePeriodMs);
    while (true) {
        vTaskDelayUntil(&lastWake, period);
        if (!waitSensorIdle()) {
            printf("[Maintenance] Sensor busy, maintenance skipped...\n");
            continue;
        }
        vTaskSuspend(sensorTaskHandle);
        printf("[Maintenance] System paused for 2 seconds...\n");
        vTaskDelay(pdMS_TO_TICKS(kMaintenancePauseMs));
        printf("[Maintenance] Resuming sensor task...\n");
        vTaskResume(sensorTaskHandle);
    }
}

void app_main(void) {
    createTask(eventTriggerTask, "Event", kTaskStackSizeBytes, NULL, kLowPriority, NULL);
    createTask(sensorReaderTask, "Sensor", kTaskStackSizeBytes, NULL, kNormalPriority, &sensorTaskHandle);
    createTask(maintenanceModeTask, "Maintenance", kTaskStackSizeBytes, NULL, kRealtimePriority, NULL);
}
