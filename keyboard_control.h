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

typedef enum {
    BRIGHTNESS_LVL_0 = 3,
    BRIGHTNESS_LVL_1 = 2,
    BRIGHTNESS_LVL_2 = 1,
    BRIGHTNESS_LVL_3 = 0,
    BRIGHTNESS_LVL_MAX = 0,
} BRIGHTNESS_LVL;

typedef enum {
    ANIMATION_SPEED_SLOW = 0,
    ANIMATION_SPEED_MEDIUM = 1,
    ANIMATION_SPEED_FAST = 2,
} ANIMATION_SPEED;

typedef enum {
    ANIMATION_MODE_RIPPLE,
    ANIMATION_MODE_LEFT_WAVE,
    ANIMATION_MODE_RIGT_WAVE,
    ANIMATION_MODE_BREATHE,
} ANIMATION_MODE;

int get_keyboard();
void send_data(int fd, char buffer[]);

void set_brightness(int fd, BRIGHTNESS_LVL brt);
void light_keymask(int fd, char mask[17]);
void set_lights_on(int fd);
void set_lights_off(int fd);
void set_animation_mode(int fd, ANIMATION_MODE mode, ANIMATION_SPEED speed);
void light_single_key_by_id(int fd, unsigned int id);
