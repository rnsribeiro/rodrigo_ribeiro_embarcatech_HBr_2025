/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 
 int main() {
     stdio_init_all();
     int count = 0;
     while (true) {
         printf("Hello, world USB Serial %d!\n", ++count);
         sleep_ms(1000);
     }
 }