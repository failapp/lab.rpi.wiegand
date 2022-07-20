#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {

    //printf("Hello, o_0!!!!\n");    
    
    const char *mqtt_topic = getenv("MQTT_TOPIC");

    char hex[8];
    char card[8] = "";

    //char bytes[] = {97, 98, 99, 100};
    unsigned char data[100] = { 233, 7, 80, 255};

    for (int i = 0; i < 4; i++) {
        //printf("%02X", (int)data[i]);
        snprintf(hex, sizeof(hex), "%02X", (int)data[i]);
        strcat(card, hex);
    }

    char msg[128] = "";

    strcat(msg, "{\"card\":\"");
    strcat(msg, card);
    strcat(msg, "\", \"relay_mqtt\":\"");
    strcat(msg, mqtt_topic);
    strcat(msg, "\"}");

    printf("%s\n", msg);

    

    return 0;
}


