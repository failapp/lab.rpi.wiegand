#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <wiringPi.h>

#include "reader.h"
#include "relay.h"
#include "button.h"

int main(int argc, char const *argv[] ) {

    pthread_t reader;
    pthread_t relay;
    pthread_t button;

    wiringPiSetup();

    pthread_create(&reader, NULL, handle_reader, NULL);
    pthread_create(&relay, NULL, handle_relay, NULL);
    pthread_create(&button, NULL, handle_button, NULL);

    pthread_join(reader, NULL);
    pthread_join(relay, NULL);
    pthread_join(button, NULL);

    return 0;
}
