/*----------------------------------------------------
--------------------------------------------------*/

/*
 * Copyright 2002 Red Hat Inc., Durham, North Carolina.
 *
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation on the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  IN NO EVENT SHALL RED HAT AND/OR THEIR SUPPLIERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This is a simple test program that reads from /dev/input/event*,
 * decoding events into a human readable form.
 */

/*
 * Authors:
 *   Rickard E. (Rik) Faith <faith@redhat.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <linux/input.h>
#include <stdbool.h>

//+++++ Mouse key to contrl mplayer----------------------------
#define  KEY_F1 59
#define  KEY_F2 60
#define  KEY_F3 61
#define  KEY_F4 62
#define  KEY_F5 63

static const unsigned char my_kbdmap_normal[256] = {
// key number 0-11 
0,0x1b,'1','2','3','4','5','6','7','8','9','0',
// key number 12-27  0-BS, 9-Tab
'-','=',0,9,'q','w','e','r','t','y','u','i','o','p','[',']',
// key number 28-41  13-ENTER, 0 - Left CTRL  0x27-'
13,0, 'a','s','d','f','g','h','j','k','l',';',0x27,'`',
//key number 42-53   0-Left SHIFT   0x5c-\   0x2f-/
0,0x5c,'z','x','c','v','b','n','m',',','.',0x2f,
//key number 54-68
0,'*',0,0x20,0,0,0,0,0,0,0,
};


struct input_event event;

int main(int argc, char **argv)
{
    char          name[64];           /* RATS: Use ok, but could be better */
    char          buf[256] = { 0, };  /* RATS: Use ok */
    unsigned char mask[EV_MAX/8 + 1]; /* RATS: Use ok */
    int           version;
    int           fd = 0;
    int           rc;
    int           i, j;
    char          *tmp;
//-------------
    char 	  *ckey;
//+++++ Mouse key to contrl mplayer----------------------------
   unsigned int volume_val=100;
   char strCMD[50];
   //feature for USB keyboard
   bool USB_KEYBOARD=false;
   bool FEATURE_REPEAT=false;
   int evnum_kbd; // event number for keyborad input
//---------------+++end
#define test_bit(bit) (mask[(bit)/8] & (1 << ((bit)%8)))

//+++++ ------------- set stdIO buf ----
   setvbuf(stdout,NULL,_IONBF,0);
//---------------------------+++end

    for (i = 0; i < 32; i++) {
        sprintf(name, "/dev/input/event%d", i);
        if ((fd = open(name, O_RDONLY, 0)) >= 0) {
            ioctl(fd, EVIOCGVERSION, &version);
            ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
            ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask);
            printf("%s\n", name);
            printf("    evdev version: %d.%d.%d\n",
                   version >> 16, (version >> 8) & 0xff, version & 0xff);
            printf("    name: %s\n", buf);
//+++++ -------check if it's USB_Keyboard ---------
	    if(strstr(buf,"USB Keyboard") == 0)
			USB_KEYBOARD=true;
//------------------------------------------+++end
            printf("    features:");
            for (j = 0; j < EV_MAX; j++) {
                if (test_bit(j)) {
                    const char *type = "unknown";
                    switch(j) {
                    case EV_KEY: type = "keys/buttons"; break;
                    case EV_REL: type = "relative";     break;
                    case EV_ABS: type = "absolute";     break;
                    case EV_MSC: type = "reserved";     break;
                    case EV_LED: type = "leds";         break;
                    case EV_SND: type = "sound";        break;
                    case EV_REP: type = "repeat";
//+++++ -------check if it has feature 'repeat' ---------
				FEATURE_REPEAT=true;
				evnum_kbd=i;
//------------------------------------------+++end
			        break;
                    case EV_FF:  type = "feedback";     break;
                    }
                   printf(" %s", type);
                }
            }
            printf("\n");
            close(fd);
        }
    }

//+++++ ------- if it's USB keyboard input interface ---------
//    if (argc > 1) {
//        sprintf(name, "/dev/input/event%d", atoi(argv[1]));
    if ( USB_KEYBOARD && FEATURE_REPEAT ){
        sprintf(name, "/dev/input/event%d", evnum_kbd);
//--------------------------------------------------------+++end
        if ((fd = open(name, O_RDWR, 0)) >= 0) {
            printf("%s: open, fd = %d\n", name, fd);
            for (i = 0; i < LED_MAX; i++) {
                event.time.tv_sec  = time(0);
                event.time.tv_usec = 0;
                event.type         = EV_LED;
                event.code         = i;
                event.value        = 1;
                write(fd, &event, sizeof(event));
            }

            while ((rc = read(fd, &event, sizeof(event))) > 0) {
/*
                printf("%-24.24s.%06lu type 0x%04x; code 0x%04x;"
                       " value 0x%08x; ",
                       ctime(&event.time.tv_sec),
                       event.time.tv_usec,
                       event.type, event.code, event.value);
*/
                switch (event.type) {
                case EV_KEY:
                    if (event.code > BTN_MISC) { //----is Button
/*
                        printf("Button %d %s",
                               event.code & 0xff,
                               event.value ? "press" : "release");
*/

                    } else {   //----- is Key
/*
	                       printf("Key %d (0x%x) %s",
                               event.code & 0xff,
                               event.code & 0xff,
                               event.value ? "press" : "release");
*/
//----------------- print key letter ----------
			if(event.value==1){ //----if key_pressed
				ckey=my_kbdmap_normal+(event.code & 0xff);
				if(*ckey == '\r') //---return to next line
					printf("\r\n");
				else
					printf("%c",*ckey);
			}
                    }
                    break;
                case EV_REL:
                    switch (event.code) {
                    case REL_X:      tmp = "X";       break;
                    case REL_Y:      tmp = "Y";       break;
                    case REL_HWHEEL:
			 tmp = "HWHEEL";
			 break;
                    case REL_DIAL:   tmp = "DIAL";    break;
                    case REL_WHEEL:
			 tmp = "WHEEL";
			 break;
                    case REL_MISC:   tmp = "MISC";    break;
                    default:         tmp = "UNKNOWN"; break;
                    }
//                    printf("Relative %s %d", tmp, event.value);
                    break;
                case EV_ABS:
                    switch (event.code) {
                    case ABS_X:        tmp = "X";        break;
                    case ABS_Y:        tmp = "Y";        break;
                    case ABS_Z:        tmp = "Z";        break;
                    case ABS_RX:       tmp = "RX";       break;
                    case ABS_RY:       tmp = "RY";       break;
                    case ABS_RZ:       tmp = "RZ";       break;
                    case ABS_THROTTLE: tmp = "THROTTLE"; break;
                    case ABS_RUDDER:   tmp = "RUDDER";   break;
                    case ABS_WHEEL:    tmp = "WHEEL";    break;
                    case ABS_GAS:      tmp = "GAS";      break;
                    case ABS_BRAKE:    tmp = "BRAKE";    break;
                    case ABS_HAT0X:    tmp = "HAT0X";    break;
                    case ABS_HAT0Y:    tmp = "HAT0Y";    break;
                    case ABS_HAT1X:    tmp = "HAT1X";    break;
                    case ABS_HAT1Y:    tmp = "HAT1Y";    break;
                    case ABS_HAT2X:    tmp = "HAT2X";    break;
                    case ABS_HAT2Y:    tmp = "HAT2Y";    break;
                    case ABS_HAT3X:    tmp = "HAT3X";    break;
                    case ABS_HAT3Y:    tmp = "HAT3Y";    break;
                    case ABS_PRESSURE: tmp = "PRESSURE"; break;
                    case ABS_DISTANCE: tmp = "DISTANCE"; break;
                    case ABS_TILT_X:   tmp = "TILT_X";   break;
                    case ABS_TILT_Y:   tmp = "TILT_Y";   break;
                    case ABS_MISC:     tmp = "MISC";     break;
                    default:           tmp = "UNKNOWN";  break;
                    }
//                    printf("Absolute %s %d", tmp, event.value);
                    break;
//              case EV_MSC: printf("Misc"); break;
                case EV_LED: printf("Led");  break;
                case EV_SND: printf("Snd");  break;
                case EV_REP: printf("Rep");  break;
                case EV_FF:  printf("FF");   break;
                    break;
                }
//                printf("\n");
            }
//            printf("rc = %d, (%s)\n", rc, strerror(errno));
            close(fd);
        }
    }
    return 0;
}
