
#include <stdio.h>

#include "common/util.h"

bool isHexChar(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int hexCharToInt(char c) {
    if(c >= '0' && c <= '9') {
        return (int)(c - '0');
    } else if(c >= 'a' && c <= 'f') {
        return (int)(c - 'a') + 10;
    } else if(c >= 'A' && c <= 'F') {
        return (int)(c - 'A') + 10;
    }
    return -1;
}

char intToHexChar(int n) {
    if(n >= 0 && n < 10) {
        return '0' + n;
    } else if(n >= 10 && n < 16) {
        return 'a' + n - 10;
    }
    return 0;
}

int parseUTF8(char* c, int* length) {
    if ((c[0] & 0x80) == 0) {
        *length = 1;
        return c[0];
    } else {
        int ret = 0;
        int i = 0;
        while (i < 8 && (c[0] & (1 << (7 - i))) != 0) {
            i++;
        }
        *length = i;
        ret |= (c[0] & (0xff >> (6 - i))) << ((i - 1) * 6);
        int j = 0;
        while(i > 1) {
            i--;
            j++;
            if((c[j] & 0xc0) != 0x80) {
                return -1;
            }
            ret |= (c[j] & 0x3f) << ((i - 1) * 6);
        }
        return ret;
    }
}

int printUTF8(int c, char* out) {
    if(c <= 0x7f) {
        out[0] = (char)c;
        return 1;
    } else {
        int i = 2;
        while (c > (1 << ((i - 1) * 6 + (6 - i)))) {
            i++;
        }
        out[0] = 0;
        for(int j = 0; j < i; j++) {
            out[0] |= (1 << (7 - j));
        }
        out[0] |= (c >> ((i - 1) * 6)); 
        for(int j = 1; j < i; j++) {
            out[j] = 0x80 | ((c >> ((i - j - 1) * 6)) & 0x3f);
        }
        return i;
    }
}

void uuidv4(char* ret) {
    FILE* file = fopen("/dev/urandom", "rb");
    uint8_t random[16];
    fread(&random, 1, 16, file);
    fclose(file);
    for(int i = 0; i < 16; i++) {
        ret[2*i] = intToHexChar(random[i] >> 4);
        ret[2*i + 1] = intToHexChar(random[i] & 0x0F);
    }
}
