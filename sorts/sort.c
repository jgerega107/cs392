#include <stdio.h>
#include "sorts.h"

int main() {
    int nums[] = { 4, -1, 3, 7, 2 };
    const int len = sizeof(nums) / sizeof(int);
    display_array(nums, len);
    bubble_sort(nums, len);
    display_array(nums, len);

    int nums2[] = { 4, -1, 3, 7, 2 };
    const int len2 = sizeof(nums2) / sizeof(int);
    display_array(nums2, len2);
    selection_sort(nums2, len2);
    display_array(nums2, len2);

    int nums3[] = { 4, -1, 3, 7, 2 };
    const int len3 = sizeof(nums3) / sizeof(int);
    display_array(nums3, len3);
    insertion_sort(nums3, len3);
    display_array(nums3, len3);

    return 0;
}
