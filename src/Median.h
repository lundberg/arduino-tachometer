#ifndef __MEDIAN__
#define __MEDIAN__

typedef long elem_type;

#define ELEM_SWAP(a,b) { register elem_type t=(a);(a)=(b);(b)=t; }

// Method 1 @ 1976
elem_type kth_smallest(elem_type a[], uint16_t n, uint16_t k);
#define wirth_median(a,n) kth_smallest(a,n,(((n)&1)?((n)/2):(((n)/2)-1)))

// Method 2 @ 1992
elem_type quick_select_median(elem_type arr[], uint16_t n);

#endif
