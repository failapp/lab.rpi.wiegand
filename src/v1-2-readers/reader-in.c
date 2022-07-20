
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>
#include <mosquitto.h>

#define MAXWIEGANDBITS 32
#define READERTIMEOUT 3000000
#define LEN 256

static unsigned char __wiegandData[MAXWIEGANDBITS];
static unsigned long __wiegandBitCount;
static struct timespec __wiegandBitTime;

void getData0(void) {
    if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

void getData1(void) {
    if (__wiegandBitCount / 8 < MAXWIEGANDBITS) {
        __wiegandData[__wiegandBitCount / 8] <<= 1;
        __wiegandData[__wiegandBitCount / 8] |= 1;
        __wiegandBitCount++;
    }
    clock_gettime(CLOCK_MONOTONIC, &__wiegandBitTime);
}

int wiegandInit(int d0pin, int d1pin) {

    //wiringPiSetup();

    printf("wiegand init .. d0pin -> %d, d1pin -> %d", d0pin, d1pin);

    pinMode(d0pin, INPUT);
    pinMode(d1pin, INPUT);
    wiringPiISR(d0pin, INT_EDGE_FALLING, getData0);
    wiringPiISR(d1pin, INT_EDGE_FALLING, getData1);

}

void wiegandReset() {
    memset((void *)__wiegandData, 0, MAXWIEGANDBITS);
    __wiegandBitCount = 0;
}

int wiegandGetPendingBitCount() {
    struct timespec now, delta;
    clock_gettime(CLOCK_MONOTONIC, &now);
    delta.tv_sec = now.tv_sec - __wiegandBitTime.tv_sec;
    delta.tv_nsec = now.tv_nsec - __wiegandBitTime.tv_nsec;

    if ((delta.tv_sec > 1) || (delta.tv_nsec > READERTIMEOUT))
        return __wiegandBitCount;

    return 0;
}

int wiegandReadData(void* data, int dataMaxLen) {
    if (wiegandGetPendingBitCount() > 0) {
        int bitCount = __wiegandBitCount;
        int byteCount = (__wiegandBitCount / 8) + 1;
        memcpy(data, (void *)__wiegandData, ((byteCount > dataMaxLen) ? dataMaxLen : byteCount));

        wiegandReset();
        return bitCount;
    }
    return 0;
}


void *handle_reader_in (void *args) {

    const char *gpio_data0_in = getenv("GPIO_DATA0_IN");
    const char *gpio_data1_in = getenv("GPIO_DATA1_IN");
    const char *mqtt_host = getenv("MQTT_HOST");
    const char *mqtt_topic = getenv("MQTT_TOPIC");

    
    int i;
    wiegandInit(atoi(gpio_data0_in), atoi(gpio_data1_in));

    while(1) {
        int bitLen = wiegandGetPendingBitCount();
        if (bitLen == 0) {
            usleep(5000);
        } else {

            //size_t len2 = strlen(_buf2);
            //if(len + len2 > tam) ..

            char data[100];
            bitLen = wiegandReadData((void *)data, 100);
            int bytes = bitLen / 8 + 1;

            char hexa[8];
            char card[8] = "";
            for (i = 0; i < bytes; i++) {
                //printf("%02X", (int)data[i]);
                snprintf(hexa, sizeof(hexa), "%02X", (int)data[i]);
                strcat(card, hexa);
            }
            printf("reader in -> card data hexa -> %s\n", card);

            int rc;
            struct mosquitto *mosq;
            mosquitto_lib_init();
            mosq = mosquitto_new("publisher-lab", true, NULL);
            rc = mosquitto_connect(mosq, mqtt_host, 1883, 60);
            if (rc != 0) {
                printf("Client could not connect to broker! Error code: %d\n", rc);
                mosquitto_destroy(mosq);
            } else {

                //mosquitto_publish(mosq, NULL, mqtt_topic, strlen(card), card, 0, 0);

                char msg[128] = "";
                strcat(msg, "{\"card\":\"");
                strcat(msg, card);
                strcat(msg, "\", \"relay_mqtt\":\"");
                strcat(msg, mqtt_topic);
                strcat(msg, "\"}");

                mosquitto_publish(mosq, NULL, "sync/reader/in", strlen(msg), msg, 0, 0);
                mosquitto_disconnect(mosq);
                mosquitto_destroy(mosq);
                mosquitto_lib_cleanup();
            }
            
        }
    } // fin while ..
}
