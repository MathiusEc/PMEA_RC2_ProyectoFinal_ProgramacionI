/*
 Proyecto: Sistema Integral de Gestión y Predicción de Contaminación del Aire en Zonas Urbanas
 
 * Resumen General:
 Este programa en lenguaje C tiene como objetivo principal monitorear, predecir y gestionar
 la contaminación del aire en zonas urbanas. Busca ofrecer una solución tecnológica
 que aborde los desafíos sociales, ambientales y económicos derivados del aumento
 de contaminantes como CO₂, SO₂, NO₂ y PM2.5, especialmente ante la falta de sistemas
 eficientes de monitoreo y predicción.
 
 * Funcionalidades Clave:
 1. Monitoreo Actual: Calcula y compara los niveles de contaminación en al menos 5 zonas
 urbanas con límites aceptables.
 2. Predicción: Estima los niveles de contaminación para las próximas 24 horas usando
 datos históricos y factores climáticos (temperatura, viento, humedad). Se utilizará
 un algoritmo de promedio ponderado, dando más peso a los datos recientes.
 3. Alertas Preventivas: Emite advertencias si se pronostican niveles superiores a los límites.
 4. Promedios y Comparación OMS: Calcula promedios de los últimos 30 días por zona y los
 compara con los estándares de la Organización Mundial de la Salud (OMS).
 5. Recomendaciones: Sugiere medidas de mitigación como reducción de tráfico o suspensión
 de actividades en exteriores.
 6. Exportación de Datos: Genera archivos para almacenar datos históricos y predicciones,
 facilitando análisis posteriores.
 
 * Requerimientos Técnicos:
 - Implementación usando arreglos, estructuras, punteros, funciones y manejo de archivos.
 - Los datos históricos se guardarán en archivos para asegurar su persistencia.
 - Restricción importante: No se permite el uso de librerías externas para manipulación
 de archivos o cálculos avanzados.
 
 * Propósito Ético:
 Contribuir a la salud pública y la protección ambiental mediante la información oportuna
 y recomendaciones que permitan tomar decisiones para mitigar los efectos nocivos de
 la contaminación atmosférica.
 */

 #include <stdio.h>
 #include <string.h>
 #include "funciones.h"
 
int main(int argc, char *argv[]) {
    ZonaUrbana zonas[MAX_ZONAS];
    int opcion;
    int zonas_cargadas = 0;
    
    printf("SISTEMA DE MONITOREO DE CALIDAD DEL AIRE - QUITO\n");
    printf("======================================================\n");
    printf("Inicializando sistema...\n\n");
    
    // Intentar cargar zonas desde archivos separados
    zonas_cargadas = cargarTodasLasZonas(zonas);
    
    // Si no se pudieron cargar las zonas, inicializar por primera vez
    if(zonas_cargadas == 0) {
        printf("No se encontraron datos previos. Inicializando zonas...\n");
        inicializarZonas(zonas);
        zonas_cargadas = MAX_ZONAS;
    }
    
    printf("Sistema listo con %d zonas operativas\n\n", zonas_cargadas);
    
    // Menú principal
    do {
        opcion = menu();
        
        switch(opcion) {
            case 1:
                printf("\n");
                registroDatosDiario(zonas);
                break;
                
            case 2:
                printf("\n");
                monitoreoDetalladoPorZona(zonas);
                break;
                
            case 3:
                printf("\n");
                mostrarTendenciasHistorico(zonas);
                break;
                
            case 4:
                printf("\n");
                prediccionContaminacion24h(zonas);
                break;
                

            case 5:
                printf("\n");
                // Gestión de datos: Corrección de datos ingresados
                corregirDatosIngresados(zonas);
                break;
            
            case 6:
                printf("\n");
                mostrarEstadoSistema(zonas);
                break;
                
            case 0:
                printf("\n");
                printf("\n");
                printf("\n=== SALIENDO DEL SISTEMA ===\n");
                printf("Guardando datos antes de salir...\n");
                guardarTodasLasZonas(zonas);
                printf("Datos guardados correctamente.\n");
                printf("¡Gracias por usar el sistema de monitoreo ambiental!\n");
                break;
                
            default:
                printf("Opción no válida. Por favor, intente de nuevo.\n");
                break;
        }
        
        // Las funciones ya manejan su propia pausa, excepto la gestión de datos
        // que tiene su propio bucle interno
        
    } while(opcion != 0);
    
    return 0;
} 
