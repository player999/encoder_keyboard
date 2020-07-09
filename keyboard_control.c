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

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/hidraw.h>
#include <memory.h>
#include <stdio.h>

#include "keyboard_control.h"

#define OMEN_ENCODER_PID 0x3341
#define OMEN_ENCODER_VID 0x03f0
#define DESCRIPTOR_SIZE 30
#define PACKET_SIZE 64
#define RETRIES 5
#define TIMEOUT 20000

int get_keyboard()
{
    int fd;
    int dev_idx = 0;
    while(1)
    {
        static char fname[DESCRIPTOR_SIZE];
        int descriptor_size;
        struct hidraw_devinfo devinfo;

        snprintf(fname, sizeof(fname), "/dev/hidraw%d", dev_idx);
        fd = open(fname, O_RDWR);
        if(-1 == fd)
        {
            return fd;
        }

        if(-1 != ioctl(fd, HIDIOCGRAWINFO, &devinfo))
        {
            if((OMEN_ENCODER_VID == devinfo.vendor) && (OMEN_ENCODER_PID == devinfo.product))
            {
                if(-1 != ioctl(fd, HIDIOCGRDESCSIZE, &descriptor_size))
                {
                    if(DESCRIPTOR_SIZE == descriptor_size)
                    {
                        break;
                    }

                }
            }
        }
        else
        {
            perror("ioctl failed");
        }
        close(fd);
        dev_idx++;
    }
    return fd;
}

void send_data(int fd, char buffer[])
{
    char recv_buffer[PACKET_SIZE];
    int retries = RETRIES;
    memset(recv_buffer, 0, sizeof(recv_buffer));
    write(fd, buffer, PACKET_SIZE);

    while(retries--)
    {
        ssize_t len = read(fd, recv_buffer, sizeof(recv_buffer));
        usleep(TIMEOUT);
        if(len > 2)
        {
            /* Match retcode */
            if( (recv_buffer[0] == buffer[0]) &&
                (recv_buffer[1] == buffer[1]))
            {
                return;
            }
        }
    }
    printf("Command failed\n");
}

void set_brightness(int fd, BRIGHTNESS_LVL brt)
{
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x0C;
    buffer[1] = 0x00;
    buffer[2] = 0x01;
    buffer[3] = 0x00;
    buffer[4] = brt;
    send_data(fd, buffer);
}

void light_keymask(int fd, char mask[17])
{
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x04;
    buffer[1] = 0x01;
    buffer[2] = 0x12;
    buffer[3] = 0x00;
    memcpy(&buffer[4], mask, 17);
    send_data(fd, buffer);
}

void set_lights_on(int fd)
{
    char buffer[64];
    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x04;
    buffer[1] = 0x01;
    buffer[2] = 0x12;
    buffer[3] = 0x00;
    memset(&buffer[4], 0xFF, 16);
    buffer[20] = 0x01;
    send_data(fd, buffer);
}

void set_lights_off(int fd)
{
    char buffer1[64] = {0x03, 0x00, 0x24};
    char buffer2[64] = {0x0A, 0x00, 0x02, 0x00, 0xAC, 0x53};
    send_data(fd, buffer1);
    send_data(fd, buffer2);
}


void set_animation_mode(int fd, ANIMATION_MODE mode, ANIMATION_SPEED speed)
{
    char buffer_breathe[64] =       {0x03, 0x00, 0x24, 0x00, 0x02, 0x00, 0x00, 0x02, 0x00, 0x01};
    char buffer_ripple[64] =        {0x03, 0x00, 0x24, 0x00, 0x09, 0x00, 0x00, 0x02, 0x00, 0x01};
    char buffer_left_wave[64] =     {0x03, 0x00, 0x24, 0x00, 0x0a, 0x00, 0x00, 0x02, 0x00, 0x02};
    char buffer_right_wave[64] =    {0x03, 0x00, 0x24, 0x00, 0x0a, 0x00, 0x00, 0x02, 0x00, 0x03};
    char *buffer;
    char buffer_commit[64] = {0x0a, 0x00, 0x02, 0x00, 0xac, 0x53};

    buffer = buffer_breathe;
    if(ANIMATION_MODE_RIPPLE == mode)
    {
        buffer = buffer_ripple;
    }
    else if(ANIMATION_MODE_LEFT_WAVE == mode)
    {
        buffer = buffer_left_wave;
    }
    else if(ANIMATION_MODE_RIGT_WAVE == mode)
    {
        buffer = buffer_right_wave;
    }
    else if(ANIMATION_MODE_BREATHE == mode)
    {
        buffer = buffer_breathe;
    }

    buffer[7] = speed;
    send_data(fd, buffer);

    send_data(fd, buffer_commit);
}

void light_single_key_by_id(int fd, unsigned int id)
{
    if(id > 128) return;
    unsigned char offset;
    unsigned char bitno;
    char buffer[64];

    memset(buffer, 0, sizeof(buffer));
    buffer[0] = 0x04;
    buffer[1] = 0x01;
    buffer[2] = 0x12;
    buffer[3] = 0x00;
    offset = id / 8 + 4;
    bitno = id % 8;
    buffer[offset] = 1 << bitno;
    send_data(fd, buffer);
}