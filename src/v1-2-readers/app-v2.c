#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <wiringPi.h>

#include "reader-in.h"
#include "reader-out.h"
#include "relay.h"


int main(int argc, char const *argv[] ) {

    int rc, id=12;
    pthread_t reader_in;
    pthread_t reader_out;
    pthread_t relay;

    wiringPiSetup();

    pthread_create(&reader_in, NULL, handle_reader_in, NULL);
    pthread_create(&reader_out, NULL, handle_reader_out, NULL);        
    pthread_create(&relay, NULL, handle_relay, NULL);

    pthread_join(reader_in, NULL);
    pthread_join(reader_out, NULL);
    pthread_join(relay, NULL);
    
    return 0;
}
