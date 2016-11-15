/* queue.c --- 
 * 
 * Filename: queue.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: 
 * Created: Thu Feb  7 19:49:26 2013 (-0500)
 * Version: 
 * Last-Updated: 11/09/2016
 *           By: Raghavendra Nataraj(natarajr), Srikanth Kanuri(srkanuri)
 *     Update #: 1
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#include "queue.h"

void init_queue(queue_t *buf) {
  buf->head = 0;
  buf->tail = 0;
}

int enqueue (queue_t *buf, int data) {
  if((buf->head+1)%QUEUE_SIZE != buf->tail){
    buf->buffer[buf->head] = data;
    buf->head = (buf->head + 1)%QUEUE_SIZE;
    return 1;
  }
  return 0;
}

int dequeue (queue_t *buf, uint8_t* data) {
  if(buf->head != buf->tail){
    *data = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1)%QUEUE_SIZE;
    return *data;
  }
  return 0;
}

int queue_empty(queue_t *buf) {
  return (buf->head == buf->tail)? 1:0;
}


/* queue.c ends here */
