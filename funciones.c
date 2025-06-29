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

void leerCadena(char *cadena, int num)
{
    fflush(stdin);
    fgets(cadena, num, stdin);

    int len = strlen(cadena) - 1;
    cadena[len] = '\0';
}

int menu()
{
    int opc, val;
    do
    {
        printf("==========MENU PRINCIPAL==========\n");
        printf("1. Monitoreo Actual\n");
        printf("2. Prediccion de Contaminacion\n");
        printf("3. Alertas Preventivas\n");
        printf("4. Promedios y Comparacion OMS\n");
        printf("5. Recomendaciones\n");
        printf("6. Exportacion de Datos\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");
        fflush(stdin);
        int val = scanf("%d", &opc);
        fflush(stdin);
        if (val != 1 || opc < 0 || opc > 6)
        {
            printf("Opción invalida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || opc < 0 || opc > 6);
    return opc;
}

void guardarZonas(ZonaUrbana *zonas, int contZonas)
{
    FILE *f;
    f = fopen("zonas.dat", "wb+"); // wb+ es para escribir y leer en binario
    if (f != NULL) {
        fwrite(&contZonas, sizeof(int), 1, f); // Guardar la cantidad de zonas
        fwrite(zonas, sizeof(ZonaUrbana), contZonas, f); // Guardar las zonas urbanas
        fclose(f);
    }
}

void inicializarZonas(ZonaUrbana zonas[])
{
    // Inicializar todas las zonas en cero primero
    for(int i = 0; i < MAX_ZONAS; i++) {
        // Inicializar todo en cero
        zonas[i].id_zona = 0;
        zonas[i].dias_registrados = 0;
        
        // Inicializar niveles actuales en cero
        zonas[i].niveles_actuales.co2 = 0.0;
        zonas[i].niveles_actuales.so2 = 0.0;
        zonas[i].niveles_actuales.no2 = 0.0;
        zonas[i].niveles_actuales.pm25 = 0.0;
        
        // Inicializar clima actual en cero
        zonas[i].clima_actual.temperatura = 0.0;
        zonas[i].clima_actual.velocidad_viento = 0.0;
        zonas[i].clima_actual.humedad = 0.0;
        zonas[i].clima_actual.presion_atmosferica = 0.0;
        
        // Inicializar promedios de 30 días en cero
        for(int j = 0; j < 4; j++) {
            zonas[i].promedio_30_dias[j] = 0.0;
        }
        
        // Inicializar histórico en cero
        for(int k = 0; k < MAX_DIAS_HISTORICOS; k++) {
            zonas[i].historico[k].co2 = 0.0;
            zonas[i].historico[k].so2 = 0.0;
            zonas[i].historico[k].no2 = 0.0;
            zonas[i].historico[k].pm25 = 0.0;
        }
    }
    
    // Solo asignar nombres y IDs a las 5 zonas de Quito
    strcpy(zonas[0].nombre, "Centro Historico");
    zonas[0].id_zona = 1;
    
    strcpy(zonas[1].nombre, "Norte - La Carolina");
    zonas[1].id_zona = 2;
    
    strcpy(zonas[2].nombre, "Sur - Quitumbe");
    zonas[2].id_zona = 3;
    
    strcpy(zonas[3].nombre, "Valle Los Chillos");
    zonas[3].id_zona = 4;
    
    strcpy(zonas[4].nombre, "Cumbaya - Tumbaco");
    zonas[4].id_zona = 5;
    
    guardarZonas(zonas, 5); // Guardar las zonas inicializadas en el archivo
}

int leerZonas(ZonaUrbana *zonas, int *contZonas)
{
    FILE *f;
    f = fopen("zonas.dat", "rb+"); // rb+ es para leer y escribir en binario
    if (f == NULL)
    {
        *contZonas = 0; // Si no existe el archivo, no hay zonas
        return 0; // Retornar 0 si no se pudo abrir el archivo
    }
    
    fread(contZonas, sizeof(int), 1, f); // Leer la cantidad de zonas
    fread(zonas, sizeof(ZonaUrbana), *contZonas, f); // Leer las zonas del archivo
    fclose(f);
    return 1; // Retornar 1 si se leyeron las zonas correctamente
}
