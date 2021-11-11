#include <stdio.h>
#include <string.h>

#include "sdl_init.h"

#include "game/app.h"

#include <vosk_api.h>

// https://donjon.bin.sh/fantasy/name/#type=set;set=deity
// https://opengameart.org/content/2d-modified-dark-forest-tileset
// https://opengameart.org/content/forest-tileset-for-rpgs
void test() {
    FILE *wavin;
    char buf[3200];
    int nread, final;

    VoskModel *model = vosk_model_new("model");
    VoskRecognizer *recognizer = vosk_recognizer_new(model, 16000.0);

    wavin = fopen("test.wav", "rb");
    fseek(wavin, 44, SEEK_SET);
    while (!feof(wavin)) {
         nread = fread(buf, 1, sizeof(buf), wavin);
         final = vosk_recognizer_accept_waveform(recognizer, buf, nread);
         if (final) {
             printf("%s\n", vosk_recognizer_result(recognizer));
         } /*else {
             printf("%s\n", vosk_recognizer_partial_result(recognizer));
         }*/
    }
    printf("%s\n", vosk_recognizer_final_result(recognizer));

    vosk_recognizer_free(recognizer);
    vosk_model_free(model);
    fclose(wavin);
    exit(0);
    return;
}

int main( int argc, char* args[] ) {
    sdl::init();
    game::app l_app;
    bool l_start_server = false;

    test();

    // check args
    for( uint32_t i = 0; i < argc; i++) {
        if( strcmp( args[i], "server") == 0)
            l_start_server = true;
    }

    // startup
    l_app.begin( l_start_server);
    
    // main loop
    while(l_app.update());

    // deinit the res
    l_app.close();
    sdl::deinit();

    return 0;
}
