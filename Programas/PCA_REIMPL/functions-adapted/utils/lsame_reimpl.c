
#include "../include/lapacke_utils_reimpl.h" 

/**
 * \brief Verifica si dos caracteres son iguales, ignorando mayúsculas/minúsculas
 * 
 * \details Esta función realiza una comparación case-insensitive entre dos caracteres.
 * Requiere que el sistema use codificación ASCII (verifica que 'Z' == 90).
 *
 * \param[in] ca Primer carácter a comparar
 * \param[in] cb Segundo carácter a comparar
 * \return lapack_logical 1 (true) si los caracteres son iguales (ignorando caso), 0 (false) en otro caso
 *
 * \note La función incluye una verificación de compatibilidad con ASCII mediante assert().
 *       Si la comprobación falla, el programa abortirá con un mensaje de error.
 * \warning El comportamiento es indefinido en sistemas que no usen codificación ASCII.
 */

lapack_logical lsame_reimpl(char ca, char cb) {
    // Verifica si el sistema es ASCII
    assert((unsigned char)'Z' == 90 && "Sistema no-ASCII no soportado");
    // Convierte los caracteres a mayúsculas y los compara
    return (toupper(ca) == toupper(cb)) ? 1 : 0;
}
