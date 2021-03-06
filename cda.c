//
//  cda.c
//  A generic circular dynamic array class with corresponding methods to alter the array
//  project0
//
//  Created by Chance Tudor on 9/3/18.
//  Copyright © 2018 Chance Tudor. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "cda.h"

typedef void (*FM)(void * ptr); // typedef declaration to store a freeMethod function pointer in CDA struct
typedef void (*DM)(void * ptr, FILE *fp); // typedef declaration to store a displayMethod function pointer in CDA struct
static int getCapacityCDA(CDA * items);
static int correctIndex(CDA *items, int oldIndex);
static int getStartCDA(CDA * items);
static int getEndCDA(CDA * items);
static int getIndex(CDA * items, int oldIndex);
static bool isFull(CDA * items);
static void doubleArray(CDA * items);
static void halveArray(CDA * items);

struct cda {
  void * (*storage);
  int capacity;
  int size;
  int startIndex;
  int endIndex;
  int debugVal;
  FM freeMethod;
  DM displayMethod;
};

// constructor; returns new, initialized CDA object
extern CDA * newCDA(void) {
  CDA * array = malloc(sizeof(CDA));
  assert(array != 0);
  array->capacity = 1;
  array->storage = malloc(sizeof(void *) * array->capacity);
  assert(array->storage != 0);
  array->size = 0;
  array->startIndex = 0;
  array->endIndex = 0;
  array->debugVal = 0;
  array->freeMethod = 0;
  array->displayMethod = 0;

  return array;
}

extern void setCDAdisplay(CDA *items, void (*displayMeth)(void *ptr, FILE *fp)) { items->displayMethod = displayMeth; }

extern void setCDAfree(CDA *items, void (*freeMeth)(void *ptr)) { items->freeMethod = freeMeth; }

static bool isFull(CDA * items) {
  if (sizeCDA(items) == getCapacityCDA(items)) { return true; }
  else { return false; }
}

static int correctIndex(CDA *items, int oldIndex) {
  int index = (oldIndex + getCapacityCDA(items)) % getCapacityCDA(items);
  return index;
}

// places value in slot named by given index
// previous item at that slot shifts to the next higher slot (and so on)
// if no room for insertion, array grows by doubling
extern void insertCDA(CDA *items, int index, void *value) {
  assert(index >= 0 && index <= sizeCDA(items));

  if (isFull(items) == true) { doubleArray(items); }

  if (index == 0) { // insert at front of CDA
    items->startIndex = correctIndex(items, getStartCDA(items) - 1); // don't decrement
    items->storage[getStartCDA(items)] = value;
    items->size += 1;
  }

  else if (sizeCDA(items) == 0 || index == sizeCDA(items)) {
    items->endIndex = correctIndex(items, getEndCDA(items) + 1);
    items->storage[getEndCDA(items)] = value;
    items->size += 1;
  }

  else { // insert in the middle of the CDA
    int decisionPt = sizeCDA(items) / 2; // determines whether array shifts left or right for insertion
    int trueIndex = correctIndex(items, index);
    if (index <= decisionPt) { // shift left
      memmove(&items->storage[trueIndex], &items->storage[trueIndex + 1], (
      sizeCDA(items) - trueIndex - 1) * sizeof(items));

      items->storage[trueIndex] = value;
    }
    else { // shift right
      memmove(&items->storage[trueIndex + 1], &items->storage[trueIndex],
      (sizeCDA(items) - trueIndex - 1) * sizeof(items));

      items->storage[trueIndex] = value;
    }
    items->size += 1;
  }
}

static void doubleArray(CDA * items) {
  printf("doubling\n");
  int newCap = items->capacity * 2;
  void * (*temp) = malloc(sizeof(void *) * newCap);
  assert(temp != 0);
  printf("Size = %d\n", sizeCDA(items));
  for (int i = 0; i < sizeCDA(items); i++) {
    temp[i] = getCDA(items, i);
  }
  free(items->storage);
  items->storage = temp;
  free(temp);
  items->startIndex = 0;
  items->endIndex = sizeCDA(items);
  items->capacity = newCap;
  printf("New capacity = %d\n", getCapacityCDA(items));
}

static void halveArray(CDA * items) {
  int newCap = items->capacity / 2;
  void * (*temp) = malloc(sizeof(void*) * newCap);
  assert(temp != 0);
  for (int i = 0; i < sizeCDA(items); i++) {
    temp[i] = getCDA(items, i);
  }
  free(items->storage);
  items->storage = temp;
  free(temp);
  items->startIndex = 0;
  items->endIndex = sizeCDA(items);
  items->capacity = newCap;
}

static int getStartCDA(CDA * items) { return items->startIndex; }

static int getEndCDA(CDA * items) { return items->endIndex; }

static int getIndex(CDA * items, int oldIndex) {
  int trueIndex = correctIndex(items, oldIndex);
  return trueIndex;
}

// removes and returns the item named by the given index
// item at the next higher slot shifts to that slot (and so on)
// if ratio of size to capacity < .25 array shrinks by half
// array should never shrink below a capacity of one
extern void *removeCDA(CDA * items, int index) {
  assert(index >= 0 && index <= sizeCDA(items) - 1);
  assert(sizeCDA(items) > 0);
  int trueIndex = correctIndex(items, index);
  void * value = getCDA(items, trueIndex);

  if (trueIndex == getStartCDA(items)) {
    items->startIndex = correctIndex(items, getStartCDA(items) + 1);
    items->size -= 1;
  }

  else if (trueIndex == getEndCDA(items)) {
    items->endIndex = correctIndex(items, getEndCDA(items) - 1);
    items->size -= 1;
  }

  else {
    int decisionPt = sizeCDA(items) / 2; // determines whether array shifts left or right for removal
    if (trueIndex <= decisionPt) { // shift left
      printf("Shifting left in removeCDA()\n");
      for (int i = 0; i < decisionPt; i++) {
        items->storage[getIndex(items, i)] = items->storage[getIndex(items, i + 1)];
      }
    }

    else { // shift right
      printf("shifting right in removeCDA()\n");
      for (int i = sizeCDA(items); i >= decisionPt + 1; i--) {
        items->storage[getIndex(items, i)] = items->storage[getIndex(items, i - 1)];
      }
    }

    items->size -= 1;
  }
  if ((sizeCDA(items)/(double)getCapacityCDA(items)) < .25) {
    halveArray(items);
    if ((sizeCDA(items)/(double)getCapacityCDA(items)) < .25) {
      halveArray(items);
    }
  }

  return value;
}

// takes two arrays and moves all the items in the donor array to the recipient arrays
extern void unionCDA(CDA *recipient, CDA *donor) {
  for (int i = 0; i < sizeCDA(donor); i++) {
    insertCDAback(recipient, getCDA(donor, i));
  }
  donor->size = 0;
  donor->capacity = 1;
  free(donor->storage);
  donor->storage = malloc(sizeof(void *) * 1);
}

// method returns the value at the given index, from user's perspective
// In the user's view, the first item is at index zero, the second item at index 1, and so on
// routine has to translate between the users view
// and the internal view (where the first item can be anywhere in the underlying array)
extern void *getCDA(CDA *items, int index) {
  assert(index >= 0 && index < sizeCDA(items));
  int trueIndex = correctIndex(items, index + getStartCDA(items));
  return items->storage[trueIndex];
}

// updates the value at the given index, from user's perspective
// if given index == size of the array, value is inserted at back of array
// if given index == -1 value is inserted at front of array
extern void *setCDA(CDA *items, int index, void *value) {
  assert(index >= -1 && index <= sizeCDA(items));
  int trueIndex = correctIndex(items, index + getStartCDA(items));
  void * val = getCDA(items, trueIndex);
  if (index == sizeCDA(items)) {
    insertCDAback(items, value);
    return val;
  }
  else if (index == -1) {
    insertCDAfront(items, value);
    return val;
  }
  else {
    items->storage[trueIndex] = value;
  }

  return val;
}

// method returns the size of array
extern int sizeCDA(CDA *items) { return items->size; }

// method returns the capacity of array
static int getCapacityCDA(CDA * items) { return items->capacity; }

// FIXME
extern void displayCDA(CDA *items, FILE *fp) {
  if (sizeCDA(items) == 0) {
    printf("Size = 0\n");
    if (items->debugVal > 0) { fprintf(fp, "((%d))", items->capacity); } // empty array and method should display num. empty indeces
    else { fprintf(fp, "()"); } // empty array and method should not display num. empty indeces
  }

  else if (items->displayMethod == 0) {
    if (items->debugVal > 0) { // no display method set and method should display num. empty indeces
      printf("Display method not set, displaying num. empty indeces\n");
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(items); i++) {
        fprintf(fp, "@%p,", &items->storage[getIndex(items, i)]); // no set display method forces addresses of each item to be printed
      }
      fprintf(fp, "(%d))", (getCapacityCDA(items) - sizeCDA(items)));
    }
    else { // no display method set and method should not display num. empty indeces
      printf("Display method not set, not displaying num. empty indeces\n");
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(items); i++) {
        fprintf(fp, "@%p,", &items->storage[getIndex(items, i)]); // no set display method forces addresses of each item to be printed
      }
      fprintf(fp, ")");
    }
  }
  else {
    if (items->debugVal > 0) { // display method set and method should display num. empty indeces
      printf("Display method set, displaying num. empty indeces\n");
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(items); i++) {
        items->displayMethod(getCDA(items, i), fp);
        if (i != (sizeCDA(items) - 1)) { fprintf(fp, ","); }
      }
      fprintf(fp, "(%d))", (getCapacityCDA(items) - sizeCDA(items)));
    }
    else { // display method set and method should not display num. empty indeces
      printf("Display method set, not displaying num. empty indeces\n");
      printf("Size = %d\n", sizeCDA(items));
      fprintf(fp, "(");
      for (int i = 0; i < sizeCDA(items); i++) {
        items->displayMethod(getCDA(items, i), fp);
        if (i != (sizeCDA(items) - 1)) { fprintf(fp, ","); }
      }
      fprintf(fp, ")");
    }
  }
}

extern int debugCDA(CDA *items, int level) {
  int prevVal = items->debugVal;
  items->debugVal = level;

  return prevVal;
}

extern void freeCDA(CDA *items) {
  if (items->freeMethod != 0) { // individual items are only freed if a freeMethod is set
    for (int i = 0; i < sizeCDA(items); i++) { items->freeMethod(getCDA(items, i)); }
  }
  free(items->storage);
  free(items);
}
