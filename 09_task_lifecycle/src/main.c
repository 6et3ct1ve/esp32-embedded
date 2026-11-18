#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const uint32_t kTaskStackSizeBytes = 2048;
static const uint32_t kSensorPeriodMs = 500;
static const uint32_t kMaintenancePeriodMs = 10000;
static const uint32_t kMaintenancePauseMs = 2000;
static const uint32_t kEventTriggerDelayMs = 3000;

static const UBaseType_t kLowPriority = tskIDLE_PRIORITY + 2;
static const UBaseType_t kNormalPriority = tskIDLE_PRIORITY + 3;
static const UBaseType_t kHighPriority = tskIDLE_PRIORITY + 4;
static const UBaseType_t kRealtimePriority = tskIDLE_PRIORITY + 5;

void app_main(void) {}
