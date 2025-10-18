#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "HashMap.h"

int main(void) {
    HashMap *map = createMap();
    if (map == NULL) {
        printf("create map failed\n");
    }

    putMap(map, "HelloWorld!", "HelloWorld!");
    putMap(map, "HelloWorld", "HelloWorld");
    putMap(map, "HelloWorl", "HelloWorl");
    putMap(map, "HelloWor", "HelloWor");

    putMap(map, "HelloWorld", "");
    putMap(map, "HelloW", "");

    deleteMap(map, "HelloWorld!");
    return 0;
}