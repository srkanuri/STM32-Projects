/* screen.h --- 
 * 
 * Filename: screen.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Wed Aug 13 15:21:36 2014
 * Last-Updated: 
 *           By: 
 *     Update #: 0
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */

#include "c335sim.h"

void fillScreen(uint16_t);
void drawChar(uint8_t, uint8_t, unsigned char, uint16_t, uint16_t);
void drawString(uint8_t, uint8_t, char *, uint16_t, uint16_t);
void drawRect(uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);

/* screen.h ends here */
