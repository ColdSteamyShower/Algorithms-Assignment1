/*
Big Multiplication Assignment
M. Andrew Moshier
February 23, 2021

The main task for this assignment is to implement a big natural num-ber multiplication following the prototypevoid bigmul64(uint64_t a[], int sz_a, uint64_t b[], sz_b, uint64_t c[], int sz_c);
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

//
// Assignment Part 1
//

uint32_t addto32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b) {
  // Assume that sz_b <= sz_a
  // Compute as += bs
  int i;
  uint32_t c = 0;
  uint64_t s;
  for (i=0; i< sz_b; i++) {
    s = (uint64_t) as[i] + (uint64_t) bs[i] + (uint64_t) c; // s is a 33 bit value
    c = s >> 32;
    as[i] = (uint32_t) s;
  }
  for ( ; i< sz_a; i++) {
    s = (uint64_t) as[i] + (uint64_t) c; // s is a 33 bit value
    c = s >> 32;
    as[i] = (uint32_t) s;
  }
  return c;
}


//
// Assignment Part 2
//

uint32_t partialprod32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b, uint32_t d, int offset) {
  // Assume that sz_b <= sz_a
  // Compute as += bs * d

  int i;
  uint64_t pProd;     // partial product
  uint32_t cProd = 0; // carry of partial product
  uint64_t sum;       // sum
  uint32_t cSum = 0;  // carry of sum

  for (i=0; i< sz_b; i++) {
    //caluculate the 64-bit partial product (pProd)
    pProd = (uint64_t) bs[i] * (uint64_t) d;

    // add first 32 bits of partial product and the last carry
    sum = (uint64_t) as[i+offset] + (uint64_t) ((uint32_t)pProd) + (uint64_t) cProd + (uint64_t) cSum; // s is a 33 bit value

    // store the front 32 bits of partial product to carry
    cProd = pProd >> 32;

    // depending on the original value of the destination as[], the sum might have a carry. Store that
    cSum = sum >> 32;

    // set first 32 of sum in the array
    as[i+offset] = (uint32_t) sum;

  }

  // insert final remainder(s)
  for ( ; i<sz_a && cSum != 0; i++) {
    sum = (uint64_t) as[i+offset] + (uint64_t) cProd + (uint64_t) cSum;
    cProd = 0;
    cSum = sum >> 32;
    as[i+offset] = (uint32_t) sum;
  }
  return cSum;
}


//
// Assignment Part 3
//

void bigmul64(uint64_t a[], int sz_a, uint64_t b[], sz_b, uint64_t c[], int sz_c){
  // Assume that sz_a >= sz_b + sz_c
  // Assume that sz_c <= sz_b
  // compute as a = b * c
  
  // convert to 32 bit arrays
  uint32_t as[sz_a*2];
  for (int i=0 ; i<sz_a ; ++i){
    as[2*i] = (uint32_t)(a[i] >> 32);
    as[2*i+1] = (uint32_t)(a[i]);
  }

  uint32_t bs[sz_b*2];
  for (int i=0 ; i<sz_b ; ++i){
    bs[2*i] = (uint32_t)(b[i] >> 32);
    bs[2*i+1] = (uint32_t)(b[i]);
  }

  uint32_t cs[sz_c*2];
  for (int i=0 ; i<sz_a ; ++i){
    cs[2*i] = (uint32_t)(c[i] >> 32);
    cs[2*i+1] = (uint32_t)(c[i]);
  }

  // multiplication part

  uint32_t r = 0;   // remainder for partial product
  uint64_t sum = 0; // sum of current as index and r

  for (int i=0 ; i < sz_c*2 ; ++i){

    // calculate partial product against ith index of c
    r = partialprod32(as, sz_a*2, bs, sz_b*2, cs[i], i);

    // add remainder
    sum = (uint64_t) as[sz_b+i] + (uint64_t) r;
    as[sz_b+i] = sum;
  }

  // submit to a[]
  for (int i=0 ; i<sz_a ; ++i){
    a[i] = (uint64_t)(as[2*i]) + ((uint64_t)as[2*i+1] << 32);
  }
  
}

uint64_t rand_uint64(void) {
  uint64_t r = 0;
  for (int i=0; i<64; i += 15 /*30*/) {
    r = r*((uint64_t)RAND_MAX + 1) + rand();
  }
  return r;
}

int main(void){
  
  // uint64_t original[4];

  // for (int i=0 ; i<4 ; ++i){
  //   original[i] = (uint64_t) rand_uint64();
  //   printf("Unsigned 64-bit Integer %i: 0x%16lx\n", i, (uint64_t)original[i]);
  // }


  // for (int i=0 ; i<4 ; ++i){
  //    printf("Unsigned 32-bit Integer %i (part 1): 0x%08x\n", i, (uint32_t)(original[i] >> 32));
  //    printf("Unsigned 32-bit Integer %i (part 2): 0x%08x\n", i, (uint32_t)original[i]);
  // }

  uint64_t destination[8] = {0,0,0,0,0,0,0,0};
  uint64_t mult1[4] = {rand_uint64(),rand_uint64(),rand_uint64(),rand_uint64()};
  uint64_t mult2[3] = {rand_uint64(),rand_uint64(),rand_uint64()};

  bigmul64(destination, 8, mult1, 4, mult2, 3);

  printf("mult1: 0x%lx 0x%lx 0x%lx 0x%lx\n", mult1[0], mult1[1], mult1[2], mult1[3]);
  printf("mult2: 0x%lx 0x%lx 0x%lx\n", mult2[0], mult2[1], mult2[2]);

  for (int i=0 ; i<8 ; ++i){
    printf("[%i] %lx \n", i, destination[i]);
  }
  
}
  
