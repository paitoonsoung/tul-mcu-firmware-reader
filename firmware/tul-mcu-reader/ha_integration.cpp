#include "ha_integration.h"

#include <WiFi.h>
#include "ha_config.h"
#include "network_manager.h"
#include "storage_manager.h"

#if __has_include(<PubSubClient.h>)
#include <PubSubClient.h>
#define TUL_HAS_MQTT 1
#else
#define TUL_HAS_MQTT 0
#endif

namespace {
#if TUL_HAS_MQTT
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
#endif
bool connected = false;
unsigned long lastPublish = 0;
}

void haBegin() {
#if TUL_HAS_MQTT
    if (strlen(TUL_HA_MQTT_HOST) == 0 || WiFi.status() != WL_CONNECTED) {
        connected = false;
        return;
    }
    mqtt.setServer(TUL_HA_MQTT_HOST, TUL_HA_MQTT_PORT);
#else
    connected = false;
#endif
}

#if TUL_HAS_MQTT
static void publishDiscovery() {
    const char *topic = "homeassistant/sensor/tul_mcu_reader/config";
    const char *payload =
        "{\"name\":\"TUL MCU Reader\",\"uniq_id\":\"tul_mcu_reader_status\","
        "\"stat_t\":\"tul/mcu_reader/state\",\"val_tpl\":\"{{ value_json.status }}\","
        "\"device\":{\"identifiers\":[\"tul_mcu_reader\"],\"name\":\"TUL MCU Firmware Reader\",\"manufacturer\":\"TUL\"}}";
    mqtt.publish(topic, payload, true);
}
#endif

void haLoop() {
#if TUL_HAS_MQTT
    if (WiFi.status() != WL_CONNECTED || strlen(TUL_HA_MQTT_HOST) == 0) {
        connected = false;
        return;
    }
    if (!mqtt.connected()) {
        connected = mqtt.connect(TUL_HA_DEVICE_ID, TUL_HA_MQTT_USER, TUL_HA_MQTT_PASS);
        if (connected) {
            publishDiscovery();
            haPublishState();
        }
    } else {
        mqtt.loop();
    }
    if (connected && millis() - lastPublish > 10000UL) {
        haPublishState();
    }
#else
    connected = false;
#endif
}

void haPublishState() {
#if TUL_HAS_MQTT
    if (!mqtt.connected()) return;
    char payload[256];
    const char *wifi = networkSSID();
    snprintf(payload, sizeof(payload),
             "{\"status\":\"online\",\"wifi\":\"%s\",\"ip\":\"%s\",\"rssi\":%d,\"sd_free_mb\":%llu,\"ram_free_kb\":%lu}",
             wifi, networkIP(), networkRSSI(),
             storageFreeBytes() / (1024ULL * 1024ULL),
             ESP.getFreeHeap() / 1024UL);
    mqtt.publish("tul/mcu_reader/state", payload, true);
    lastPublish = millis();
#endif
}

bool haConnected() {
    return connected;
}
