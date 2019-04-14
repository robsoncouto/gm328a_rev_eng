#include "queue.h"
#define Q_MAX 20

int Array[Q_MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

int queue_peek(void) {
   return Array[front];
}

unsigned char queue_isEmpty(void) {
   if(itemCount == 0){
    return 1;
   }else{
   return 0;
   }
}

unsigned char queue_isFull(void) {
   if(itemCount == Q_MAX){
    return 1;
   }else{
   return 0;
   }
}

int queue_size(void) {
   return itemCount;
}  

void queue_insert(int data) {

   if(!queue_isFull()) {
  
      if(rear == Q_MAX-1) {
         rear = -1;            
      }       

      Array[++rear] = data;
      itemCount++;
   }
}

int queue_removeData() {
   int data = Array[front++];
  
   if(front == Q_MAX) {
      front = 0;
   }
  
   itemCount--;
   return data;  
}



