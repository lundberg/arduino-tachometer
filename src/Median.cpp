#include <stdio.h>
#include <stdlib.h>
#include "Median.h"

elem_type kth_smallest(elem_type a[], uint16_t n, uint16_t k) {
    /*
     * Method 1:
     * Algorithm from N. Wirth’s book 
     * Algorithms + data structures = programs of 1976
     */
    uint64_t i,j,l,m ;
    elem_type x ;
    l=0 ; m=n-1 ;
    while (l<m) {
        x=a[k] ;
        i=l ;
        j=m ;
        do {
            while (a[i]<x) i++ ;
            while (x<a[j]) j-- ;
            if (i<=j) {
                ELEM_SWAP(a[i],a[j]) ;
                i++ ; j-- ;
            }
        } while (i<=j) ;
        if (j<k) l=i ;
        if (k<i) m=j ;
    }
    return a[k] ;
}

elem_type quick_select_median(elem_type arr[], uint16_t n) {
    /*
     * Method 2:
     * This is the faster median determination method.
     * Algorithm from Numerical recipes in C of 1992
     */
    uint16_t low, high ;
    uint16_t median;
    uint16_t middle, ll, hh;
    low = 0 ; high = n-1 ; median = (low + high) / 2;
    for (;;) {
        if (high <= low) /* One element only */
            return arr[median] ;
        if (high == low + 1) { /* Two elements only */
            if (arr[low] > arr[high])
                ELEM_SWAP(arr[low], arr[high]) ;
            return arr[median] ;
        }
        /* Find median of low, middle and high items; swap into position low */
        middle = (low + high) / 2;
        if (arr[middle] > arr[high])
            ELEM_SWAP(arr[middle], arr[high]) ;
        if (arr[low] > arr[high])
            ELEM_SWAP(arr[low], arr[high]) ;
        if (arr[middle] > arr[low])
            ELEM_SWAP(arr[middle], arr[low]) ;
        /* Swap low item (now in position middle) into position (low+1) */
        ELEM_SWAP(arr[middle], arr[low+1]) ;
        /* Nibble from each end towards middle, swapping items when stuck */
        ll = low + 1;
        hh = high;
        for (;;) {
            do ll++; while (arr[low] > arr[ll]) ;
            do hh--; while (arr[hh] > arr[low]) ;
            if (hh < ll)
                break;
            ELEM_SWAP(arr[ll], arr[hh]) ;
        }
        /* Swap middle item (in position low) back into correct position */
        ELEM_SWAP(arr[low], arr[hh]) ;
        /* Re-set active partition */
        if (hh <= median)
            low = ll;
        if (hh >= median)
            high = hh - 1;
    }
    return arr[median] ;
}

