
#include "../include/lapacke_utils_reimpl.h" // Se puede evitar poner la ruta completa si se configura el compilador con algo como gcc -I./include -o programa utils/lapacke_ssy_nancheck_reimpl.c

float slamch_reimpl(const char *cmach) {
    float rnd = 1.0f, eps = FLT_EPSILON * 0.5f, result; 
    char cmach_upper = toupper(cmach[0]);

    switch (cmach_upper) {
        case 'E':
            {
                result = eps; 
                break;
            }
        case 'S': 
            {
                float sfmin = FLT_MIN;
                float small = 1.0f / FLT_MAX;
                if (small >= sfmin) {  
                    sfmin = small * (1.0f + eps);
                }
                result = sfmin;
                break;
            }
        case 'B':
            result = FLT_RADIX;
            break;
        case 'P':
            result = eps * FLT_RADIX; 
            break;
        case 'N':
            result = (float)FLT_MANT_DIG;
            break;
        case 'R':
            result = rnd;
            break;
        case 'M':
            result = (float)FLT_MIN_EXP;
            break;
        case 'U':
            result = FLT_MIN;
            break;
        case 'L':
            result = (float)FLT_MAX_EXP;
            break;
        case 'O':
            result = FLT_MAX;
            break;
        default:
            result = 0.0f;
    }
    return result;
}

float slacm3_reimpl(float a, float b) {
    volatile float va = a; 
    volatile float vb = b;
    return va + vb; 
}