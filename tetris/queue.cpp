#include "queue.h"
#define Q_MAX 20

// Source https://www.tutorialspoint.com/data_structures_algorithms/queue_program_in_c.htm

int Array[Q_MAX];
int front = 0;
int rear = -1;
int itemCount = 0;

int queuePeek(void) {
   return Array[front];
}

unsigned char queueIsEmpty(void) {
   if(itemCount == 0){
    return 1;
   }else{
   return 0;
   }
}

unsigned char queueIsFull(void) {
   if(itemCount == Q_MAX){
    return 1;
   }else{
   return 0;
   }
}

int queueSize(void) {
   return itemCount;
}  

void queueInsert(int data) {
   if(!queueIsFull()) {
  
      if(rear == Q_MAX-1) {
         rear = -1;            
      }       

      Array[++rear] = data;
      itemCount++;
   }
}

int queueRemoveData() {
   int data = Array[front++];
  
   if(front == Q_MAX) {
      front = 0;
   }
  
   itemCount--;
   return data;  
}



