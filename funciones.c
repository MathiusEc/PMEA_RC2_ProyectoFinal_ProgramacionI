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
        printf("\nSISTEMA DE CALIDAD DEL AIRE - QUITO\n");
        printf("══════════════════════════════════════════════\n");
        printf("1. Registrar Datos                          \n");
        printf("2. Calidad del Aire Ahora (Resumen)       \n");
        printf("3. Monitoreo Detallado por Zona           \n");  
        printf("4. Mapa de Calidad del Aire              \n");
        printf("5. Tendencias y Histórico                 \n");
        printf("6. Pronóstico 24 Horas                    \n");
        printf("7. Alertas y Notificaciones               \n");
        printf("8. Gestión de Datos                       \n");
        printf("9. Estado del Sistema                      \n");
        printf("0. Salir                                   \n");
        printf("══════════════════════════════════════════════\n");
        printf("Seleccione una opción: ");
        fflush(stdin);
        val = scanf("%d", &opc);
        fflush(stdin);
        if (val != 1 || opc < 0 || opc > 8)
        {
            printf("Opción inválida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || opc < 0 || opc > 8);
    return opc;
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
    
    // Guardar cada zona en archivo separado
    guardarTodasLasZonas(zonas);
}

// =================== FUNCIONES PARA ARCHIVOS SEPARADOS ===================

void guardarZona(ZonaUrbana *zona) {
    char nombre_archivo[100];
    
    // Crear nombre de archivo basado en ID
    sprintf(nombre_archivo, "zona_%d.dat", zona->id_zona);
    
    FILE *f = fopen(nombre_archivo, "wb");
    if(f != NULL) {
        fwrite(zona, sizeof(ZonaUrbana), 1, f);
        fclose(f);
        // Guardado silencioso para no interrumpir la experiencia del usuario
    } else {
        printf("⚠ Error al guardar datos de la zona %s\n", zona->nombre);
    }
}

void guardarTodasLasZonas(ZonaUrbana zonas[]) {
    printf("Guardando zonas en archivos separados...\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        guardarZona(&zonas[i]);
    }
}

int cargarZona(ZonaUrbana *zona, int id_zona) {
    char nombre_archivo[100];
    sprintf(nombre_archivo, "zona_%d.dat", id_zona);
    
    FILE *f = fopen(nombre_archivo, "rb+");
    if(f != NULL) {
        if(fread(zona, sizeof(ZonaUrbana), 1, f) == 1) {
            fclose(f);
            printf("Zona %s cargada desde %s\n", zona->nombre, nombre_archivo);
            return 1; // Éxito
        }
        fclose(f);
    }
    return 0; // No se pudo cargar
}

int cargarTodasLasZonas(ZonaUrbana zonas[]) {
    int zonas_cargadas = 0;
    
    printf("Cargando zonas desde archivos separados...\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        if(cargarZona(&zonas[i], i+1)) {
            zonas_cargadas++;
        }
    }
    
    printf("Total de zonas cargadas: %d/%d\n", zonas_cargadas, MAX_ZONAS);
    return zonas_cargadas;
}

void registroDatosDiario(ZonaUrbana zonas[]) {
    int id_zona, val;
    // Mostrar zonas disponibles
    cargarTodasLasZonas(zonas);
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s\n", zonas[i].id_zona, zonas[i].nombre);
    }

    do
    {
        printf("Seleccione la zona para registrar datos (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &id_zona);
        fflush(stdin);

        if (val != 1 || id_zona < 1 || id_zona > MAX_ZONAS){
            printf("Opción inválida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || id_zona < 1 || id_zona > MAX_ZONAS);

    // Mover datos históricos (FIFO - lo más reciente al inicio)
    for(int i = MAX_DIAS_HISTORICOS-1; i > 0; i--) {
        zonas[id_zona - 1].historico[i] = zonas[id_zona - 1].historico[i-1];
    }

    // Leer nuevos datos
    printf("Ingrese los niveles de contaminantes para la zona %s:\n", zonas[id_zona - 1].nombre);
    printf("CO₂ (ppm): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.co2);
    printf("SO₂ (µg/m³): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.so2);
    printf("NO₂ (µg/m³): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.no2);
    printf("PM₂.₅ (µg/m³): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.pm25);

    // Registrar datos climáticos
    printf("Ingrese los datos climáticos para la zona %s:\n", zonas[id_zona - 1].nombre);
    printf("Temperatura (°C): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.temperatura);
    printf("Velocidad del viento (km/h): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.velocidad_viento);
    printf("Humedad (%%): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.humedad);
    printf("Presión (hPa): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.presion_atmosferica);

    // Actualizar niveles actuales
    zonas[id_zona - 1].historico[0] = zonas[id_zona - 1].niveles_actuales;

    // Incrementar contador de días registrados
    if (zonas[id_zona - 1].dias_registrados < MAX_DIAS_HISTORICOS) {
        zonas[id_zona - 1].dias_registrados++;
    }

    printf("Datos registrados correctamente para la zona %s.\n", zonas[id_zona - 1].nombre);
    
    // Guardar zona actualizada automáticamente
    guardarZona(&zonas[id_zona - 1]);
    
    // Limpiar buffer de entrada
    while(getchar() != '\n');

}

void monitoreoActual(ZonaUrbana zonas[]) {
    printf("=== MONITOREO ACTUAL DE CALIDAD DEL AIRE ===\n");
    printf("Fecha: %s - Hora: %s\n", __DATE__, __TIME__);
    printf("═══════════════════════════════════════════════════\n\n");
    
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("ZONA: %s\n", zonas[i].nombre);
        printf("────────────────────────────────────────────\n");
        
        // Solo mostrar datos si hay registros
        if(zonas[i].dias_registrados > 0) {
            // Mostrar contaminantes actuales
            printf("NIVELES ACTUALES:\n");
            printf("  CO₂:   %.1f ppm     (Límite OMS: %.1f) ", 
                   zonas[i].niveles_actuales.co2, LIMITE_CO2_OMS);
            if(zonas[i].niveles_actuales.co2 > LIMITE_CO2_OMS) {
                printf("EXCEDE\n");
            } else {
                printf("OK\n");
            }
            
            printf("  SO₂:   %.1f µg/m³   (Límite OMS: %.1f) ", 
                   zonas[i].niveles_actuales.so2, LIMITE_SO2_OMS);
            if(zonas[i].niveles_actuales.so2 > LIMITE_SO2_OMS) {
                printf("EXCEDE\n");
            } else {
                printf("OK\n");
            }
            
            printf("  NO₂:   %.1f µg/m³   (Límite OMS: %.1f) ", 
                   zonas[i].niveles_actuales.no2, LIMITE_NO2_OMS);
            if(zonas[i].niveles_actuales.no2 > LIMITE_NO2_OMS) {
                printf("EXCEDE\n");
            } else {
                printf("OK\n");
            }
            
            printf("  PM₂.₅: %.1f µg/m³   (Límite OMS: %.1f) ", 
                   zonas[i].niveles_actuales.pm25, LIMITE_PM25_OMS);
            if(zonas[i].niveles_actuales.pm25 > LIMITE_PM25_OMS) {
                printf("EXCEDE\n");
            } else {
                printf("OK\n");
            }
            
            // Calcular índice de calidad del aire general
            int excesos = 0;
            if(zonas[i].niveles_actuales.co2 > LIMITE_CO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.so2 > LIMITE_SO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.no2 > LIMITE_NO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.pm25 > LIMITE_PM25_OMS) excesos++;
            
            // Mostrar estado general
            printf("\n ESTADO GENERAL: ");
            if(excesos == 0) {
                printf("BUENO - Aire saludable para todos\n");
            } else if(excesos == 1) {
                printf("MODERADO - Aceptable para la mayoría\n");
            } else if(excesos == 2) {
                printf("DAÑINO - Grupos sensibles en riesgo\n");
            } else {
                printf("PELIGROSO - Todos los grupos en riesgo\n");
            }
            
            // Mostrar condiciones climáticas actuales
            printf("\n CONDICIONES CLIMÁTICAS:\n");
            printf("  Temperatura: %.1f°C\n", zonas[i].clima_actual.temperatura);
            printf("  Viento: %.1f km/h\n", zonas[i].clima_actual.velocidad_viento);
            printf("  Humedad: %.1f%%\n", zonas[i].clima_actual.humedad);
            printf("  Presión: %.1f hPa\n", zonas[i].clima_actual.presion_atmosferica);
            
        } else {
            printf("Sin datos registrados para esta zona\n");
            printf("Registre datos primero usando la opción 1\n");
        }
        
        printf("\n");
    }
    
    // Resumen general del sistema
    printf("═══════════════════════════════════════════════════\n");
    printf("RESUMEN GENERAL DEL SISTEMA:\n");
    
    int zonas_con_datos = 0;
    int zonas_buenas = 0;
    int zonas_problematicas = 0;
    
    for(int i = 0; i < MAX_ZONAS; i++) {
        if(zonas[i].dias_registrados > 0) {
            zonas_con_datos++;
            
            int excesos = 0;
            if(zonas[i].niveles_actuales.co2 > LIMITE_CO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.so2 > LIMITE_SO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.no2 > LIMITE_NO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.pm25 > LIMITE_PM25_OMS) excesos++;
            
            if(excesos <= 1) {
                zonas_buenas++;
            } else {
                zonas_problematicas++;
            }
        }
    }
    
    printf("  Zonas monitoreadas: %d/%d\n", zonas_con_datos, MAX_ZONAS);
    printf("  Zonas con calidad aceptable: %d\n", zonas_buenas);
    printf("  Zonas con problemas: %d\n", zonas_problematicas);
    
    if(zonas_problematicas > 0) {
        printf("Se recomienda revisar alertas preventivas\n");
    }
}

void mostrarEstadoSistema(ZonaUrbana zonas[])
{
    printf("Sistema de Monitoreo - Quito\n");
    printf("═══════════════════════════════\n\n");
    
    // Resumen general
    int zonas_activas = 0;
    int total_registros = 0;
    
    for(int i = 0; i < MAX_ZONAS; i++) {
        if(zonas[i].dias_registrados > 0) {
            zonas_activas++;
            total_registros += zonas[i].dias_registrados;
        }
    }
    
    printf("RESUMEN GENERAL:\n");
    printf("  Zonas configuradas: %d\n", MAX_ZONAS);
    printf("  Zonas con datos: %d\n", zonas_activas);
    printf("  Total de registros: %d\n", total_registros);
    if(zonas_activas > 0) {
        printf("  Estado: OPERATIVO\n");
    } else {
        printf("  Estado: SIN DATOS\n");
    }
    
    // Estado por zona (resumido)
    printf("\nESTADO POR ZONA:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("  %s: %d días registrados", 
               zonas[i].nombre, zonas[i].dias_registrados);
        
        if(zonas[i].dias_registrados > 0) {
            printf(" OK\n");
        } else {
            printf(" SIN DATOS\n");
        }
    }
}
