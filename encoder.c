/*
Copyright 2020 Taras Zakharchenko

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1.  Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
3.  Neither the name of the copyright holder nor the names of its contributors
    may be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "keyboard_control.h"

extern char *optarg;
extern int optind, opterr, optopt;

const char help_text[] =
"HELP TEXT\n"
"-s static lightning\n"
"-o turn off lightning\n"
"-h show this help\n"
"-b <0|1|2> brightness\n"
"-v <slow|medium|fast> animation speed\n"
"-a <ripple|left_wave|right_wave|breathe> animation mode\n"
;

int main(int argc, char *argv[])
{
    int fd;
    int dev_idx = 0;
    char buffer[64];
    int option;
    ANIMATION_SPEED speed = ANIMATION_SPEED_FAST;
    BRIGHTNESS_LVL brightness = BRIGHTNESS_LVL_3;
    ANIMATION_MODE animode;
    int ani_set = 0;
    int off_set = 0;
    int sta_set = 0;
    int spd_set = 0;
    int need_help = 0;

    fd = get_keyboard();
    if(-1 == fd)
    {
        printf("Did not find OMEN Encoder keyboard\n");
        exit(-1);
    }

    speed = ANIMATION_SPEED_SLOW;
    while((option = getopt(argc, argv, ":sohm:b:v:a:")) != -1)
    {
        switch(option){
            case 's':
                sta_set = 1;
                break;
            case 'o':
                off_set = 1;
                break;
            case 'h':
                need_help = 1;
                break;
            case 'b':
                if(strcmp(optarg, "0") == 0)        brightness = BRIGHTNESS_LVL_0;
                else if(strcmp(optarg, "1") == 0)   brightness = BRIGHTNESS_LVL_1;
                else if(strcmp(optarg, "2") == 0)   brightness = BRIGHTNESS_LVL_2;
                else if(strcmp(optarg, "3") == 0)   brightness = BRIGHTNESS_LVL_3;
                break;
            case 'v':
                if(strcmp(optarg, "slow") == 0)         { speed = ANIMATION_SPEED_SLOW; spd_set = 1; }
                else if(strcmp(optarg, "medium") == 0)  { speed = ANIMATION_SPEED_MEDIUM; spd_set = 1; }
                else if(strcmp(optarg, "fast") == 0)    { speed = ANIMATION_SPEED_FAST; spd_set = 1; }
                break;
            case 'a':
                if(strcmp(optarg, "ripple") == 0)           { animode = ANIMATION_MODE_RIPPLE; ani_set = 1; }
                else if(strcmp(optarg, "left_wave") == 0)   { animode = ANIMATION_MODE_LEFT_WAVE; ani_set = 1; }
                else if(strcmp(optarg, "right_wave") == 0)  { animode = ANIMATION_MODE_RIGT_WAVE; ani_set = 1; }
                else if(strcmp(optarg, "breathe") == 0)     { animode = ANIMATION_MODE_BREATHE; ani_set = 1; }
                break;
            case ':':
                printf("Option needs a value\n");
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }

    set_brightness(fd, brightness);
    if(need_help)
    {
        printf(help_text);
    }
    else if(off_set)
    {
        set_lights_off(fd);
    }
    else if(sta_set)
    {
        set_lights_on(fd);
    }
    else if(ani_set)
    {
        set_animation_mode(fd, animode, speed);
    }

    close(fd);
}