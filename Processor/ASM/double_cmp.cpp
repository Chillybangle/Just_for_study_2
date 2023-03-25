#include "double_cmp.h"
#include <math.h>

int double_more (double num_1, double num_2) //num_1 > num_2
{
    if (fabs (num_1 - num_2) < f_cmp_accuracy)
        return 0;
    
    return (num_1 > num_2);
}

int double_equal (double num_1, double num_2)
{
    if (fabs (num_1 - num_2) < f_cmp_accuracy)
        return 1;
    return 0;
}
