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

// Función para calcular valor absoluto sin usar math.h
float absoluto(float numero) {
    if (numero < 0) {
        return -numero;
    }
    return numero;
}

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
        printf("==============================================\n");
        printf("1. Registrar Datos                          \n");
        printf("2. Monitoreo                                \n");
        printf("3. Tendencias e Histórico                 \n");
        printf("4. Pronóstico 24 Horas                    \n");
        printf("5. Alertas y Notificaciones               \n");
        printf("6. Gestión de Datos                       \n");
        printf("7. Estado del Sistema                     \n");
        printf("0. Salir                                   \n");
        printf("==============================================\n");
        printf("Seleccione una opción: ");
        fflush(stdin);
        val = scanf("%d", &opc);
        fflush(stdin);
        if (val != 1 || opc < 0 || opc > 7)
        {
            printf("Opción inválida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || opc < 0 || opc > 7);
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
    fflush(stdin);

}

void monitoreoDetalladoPorZona(ZonaUrbana zonas[]) {
    // Mostrar fecha y hora al principio
    printf("=== CALIDAD DEL AIRE - QUITO ===\n");
    printf("Fecha: %s - Hora: %s\n", __DATE__, __TIME__);
    printf("===============================================\n\n");
    
    // RESUMEN GENERAL INICIAL
    printf("RESUMEN GENERAL DEL SISTEMA:\n");
    printf("-----------------------------------------------\n");
    
    int zonas_con_datos = 0;
    int zonas_buenas = 0;
    int zonas_moderadas = 0;
    int zonas_problematicas = 0;
    
    // Mostrar resumen por zona de forma compacta
    for(int i = 0; i < MAX_ZONAS; i++) {
        if(zonas[i].dias_registrados > 0) {
            zonas_con_datos++;
            
            // Calcular estado general de la zona
            int excesos = 0;
            if(zonas[i].niveles_actuales.co2 > LIMITE_CO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.so2 > LIMITE_SO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.no2 > LIMITE_NO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.pm25 > LIMITE_PM25_OMS) excesos++;
            
            // Mostrar zona con estado resumido
            printf("%s: ", zonas[i].nombre);
            if(excesos == 0) {
                printf("BUENO\n");
                zonas_buenas++;
            } else if(excesos == 1) {
                printf("MODERADO\n");
                zonas_moderadas++;
            } else {
                printf("DANINO\n");
                zonas_problematicas++;
            }
        } else {
            printf("%s: SIN DATOS\n", zonas[i].nombre);
        }
    }
    
    printf("\nEstado: %d/%d zonas monitoreadas | Buenas: %d | Moderadas: %d | Problemas: %d\n",
           zonas_con_datos, MAX_ZONAS, zonas_buenas, zonas_moderadas, zonas_problematicas);
    
    // SELECCIÓN DE ZONA PARA ANÁLISIS DETALLADO
    printf("\n===============================================\n");
    printf("ANALISIS DETALLADO POR ZONA\n");
    printf("===============================================\n");
    
    int zona_seleccionada, val;
    
    // Mostrar zonas disponibles para análisis detallado
    printf("\nZONAS DISPONIBLES:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s", zonas[i].id_zona, zonas[i].nombre);
        if(zonas[i].dias_registrados > 0) {
            printf(" (%d dias de datos)\n", zonas[i].dias_registrados);
        } else {
            printf(" (Sin datos)\n");
        }
    }
    
    // Seleccionar zona
    do {
        printf("\nSeleccione la zona a analizar (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &zona_seleccionada);
        fflush(stdin);
        
        if(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS) {
            printf("Opcion invalida. Por favor, intente de nuevo.\n");
        }
    } while(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS);
    
    // Ajustar índice (resta 1 porque arrays empiezan en 0)
    zona_seleccionada--;
    
    // Verificar si la zona tiene datos
    if(zonas[zona_seleccionada].dias_registrados == 0) {
        printf("\nERROR: La zona '%s' no tiene datos registrados.\n", 
               zonas[zona_seleccionada].nombre);
        printf("Registre datos primero usando la opcion 1 del menu.\n");
        return;
    }
    
    // ================= INFORMACIÓN DETALLADA =================
    printf("\nANALISIS DETALLADO: %s\n", zonas[zona_seleccionada].nombre);
    printf("===============================================\n");
    
    // 1. DATOS ACTUALES (MÁS RECIENTES)
    printf("\nDATOS ACTUALES (Ultimo registro):\n");
    printf("-----------------------------------------------\n");
    printf("  CO2:   %.1f ppm     | Limite OMS: %.1f | Estado: ", 
           zonas[zona_seleccionada].niveles_actuales.co2, LIMITE_CO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.co2 > LIMITE_CO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("  SO2:   %.1f ug/m3   | Limite OMS: %.1f | Estado: ",
           zonas[zona_seleccionada].niveles_actuales.so2, LIMITE_SO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.so2 > LIMITE_SO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("  NO2:   %.1f ug/m3   | Limite OMS: %.1f | Estado: ",
           zonas[zona_seleccionada].niveles_actuales.no2, LIMITE_NO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.no2 > LIMITE_NO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("  PM2.5: %.1f ug/m3   | Limite OMS: %.1f | Estado: ",
           zonas[zona_seleccionada].niveles_actuales.pm25, LIMITE_PM25_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.pm25 > LIMITE_PM25_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    // 2. CONDICIONES CLIMÁTICAS
    printf("\nCONDICIONES CLIMATICAS ACTUALES:\n");
    printf("-----------------------------------------------\n");
    printf("  Temperatura:        %.1f C\n", zonas[zona_seleccionada].clima_actual.temperatura);
    printf("  Velocidad del viento: %.1f km/h\n", zonas[zona_seleccionada].clima_actual.velocidad_viento);
    printf("  Humedad:            %.1f%%\n", zonas[zona_seleccionada].clima_actual.humedad);
    printf("  Presion atmosferica: %.1f hPa\n", zonas[zona_seleccionada].clima_actual.presion_atmosferica);
    
    // 3. ANÁLISIS DE TENDENCIAS (si hay más de 1 día)
    if(zonas[zona_seleccionada].dias_registrados > 1) {
        printf("\nANALISIS DE TENDENCIAS:\n");
        printf("-----------------------------------------------\n");
        
        // Calcular promedios históricos
        float promedio_co2 = 0, promedio_so2 = 0, promedio_no2 = 0, promedio_pm25 = 0;
        int dias_para_promedio;
        if(zonas[zona_seleccionada].dias_registrados > 7) {
            dias_para_promedio = 7;
        } else {
            dias_para_promedio = zonas[zona_seleccionada].dias_registrados;
        }
        
        for(int i = 0; i < dias_para_promedio; i++) {
            promedio_co2 += zonas[zona_seleccionada].historico[i].co2;
            promedio_so2 += zonas[zona_seleccionada].historico[i].so2;
            promedio_no2 += zonas[zona_seleccionada].historico[i].no2;
            promedio_pm25 += zonas[zona_seleccionada].historico[i].pm25;
        }
        
        promedio_co2 /= dias_para_promedio;
        promedio_so2 /= dias_para_promedio;
        promedio_no2 /= dias_para_promedio;
        promedio_pm25 /= dias_para_promedio;
        
        printf("  Promedio ultimos %d dias:\n", dias_para_promedio);
        printf("    CO2:   %.1f ppm\n", promedio_co2);
        printf("    SO2:   %.1f ug/m3\n", promedio_so2);
        printf("    NO2:   %.1f ug/m3\n", promedio_no2);
        printf("    PM2.5: %.1f ug/m3\n", promedio_pm25);
        
        // Comparar con nivel actual para mostrar tendencia
        printf("\n  Tendencia respecto al promedio:\n");
        if(promedio_co2 != 0) {
            float diferencia = zonas[zona_seleccionada].niveles_actuales.co2 - promedio_co2;
            float porcentaje = absoluto(diferencia / promedio_co2 * 100);
            if(zonas[zona_seleccionada].niveles_actuales.co2 > promedio_co2) {
                printf("    CO2:   SUBIENDO (%.1f%% mas alto)\n", porcentaje);
            } else {
                printf("    CO2:   BAJANDO (%.1f%% mas bajo)\n", porcentaje);
            }
        }
    }
    
    // 4. HISTORIAL RECIENTE (últimos 5 días)
    printf("\nHISTORIAL RECIENTE:\n");
    printf("-----------------------------------------------\n");
    printf("Dia  | CO2    | SO2    | NO2    | PM2.5  | Estado\n");
    printf("-----|--------|--------|--------|--------|--------\n");
    
    int dias_mostrar;
    if(zonas[zona_seleccionada].dias_registrados > 5) {
        dias_mostrar = 5;
    } else {
        dias_mostrar = zonas[zona_seleccionada].dias_registrados;
    }
    
    for(int i = 0; i < dias_mostrar; i++) {
        // Contar excesos para determinar estado
        int excesos = 0;
        if(zonas[zona_seleccionada].historico[i].co2 > LIMITE_CO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico[i].so2 > LIMITE_SO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico[i].no2 > LIMITE_NO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico[i].pm25 > LIMITE_PM25_OMS) excesos++;
        
        char estado[16];
        if(excesos == 0) {
            strcpy(estado, "Bueno");
        } else if(excesos <= 1) {
            strcpy(estado, "Moderado");
        } else if(excesos <= 2) {
            strcpy(estado, "Danino");
        } else {
            strcpy(estado, "Peligroso");
        }
        
        printf("%-4d | %-6.1f | %-6.1f | %-6.1f | %-6.1f | %s\n",
               i+1,
               zonas[zona_seleccionada].historico[i].co2,
               zonas[zona_seleccionada].historico[i].so2,
               zonas[zona_seleccionada].historico[i].no2,
               zonas[zona_seleccionada].historico[i].pm25,
               estado);
    }
    
    // 5. RECOMENDACIONES ESPECÍFICAS
    printf("\nRECOMENDACIONES PARA %s:\n", zonas[zona_seleccionada].nombre);
    printf("-----------------------------------------------\n");
    
    int total_excesos = 0;
    if(zonas[zona_seleccionada].niveles_actuales.co2 > LIMITE_CO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.so2 > LIMITE_SO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.no2 > LIMITE_NO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.pm25 > LIMITE_PM25_OMS) total_excesos++;
    
    if(total_excesos == 0) {
        printf("  Calidad del aire aceptable\n");
        printf("  Seguro para actividades al aire libre\n");
        printf("  Ideal para ejercicio exterior\n");
    } else if(total_excesos <= 1) {
        printf("  Calidad del aire moderada\n");
        printf("  Grupos sensibles deben limitar actividades prolongadas\n");
        printf("  Mantener ventanas cerradas en horas pico\n");
    } else if(total_excesos <= 2) {
        printf("  Calidad del aire danina para grupos sensibles\n");
        printf("  Evitar ejercicio al aire libre\n");
        printf("  Considerar uso de mascarillas\n");
    } else {
        printf("  Calidad del aire peligrosa\n");
        printf("  Permanecer en interiores\n");
        printf("  Uso obligatorio de mascarillas\n");
        printf("  Contactar autoridades de salud\n");
    }
    
    printf("\n════════════════════════════════════════════════════════\n");
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

void mostrarTendenciasHistorico(ZonaUrbana zonas[]) {
    printf("=== TENDENCIAS E HISTORICO ===\n");
    printf("Fecha: %s - Hora: %s\n", __DATE__, __TIME__);
    printf("════════════════════════════════════════════════════════\n\n");
    
    int zona_seleccionada, val;
    
    // Mostrar zonas disponibles
    printf("ZONAS DISPONIBLES:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s", zonas[i].id_zona, zonas[i].nombre);
        if(zonas[i].dias_registrados > 0) {
            printf(" (%d dias de datos)\n", zonas[i].dias_registrados);
        } else {
            printf(" (Sin datos)\n");
        }
    }
    
    // Seleccionar zona
    do {
        printf("\nSeleccione la zona para analizar tendencias (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &zona_seleccionada);
        fflush(stdin);
        
        if(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS) {
            printf("Opcion invalida. Por favor, intente de nuevo.\n");
        }
    } while(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS);
    
    zona_seleccionada--;
    
    if(zonas[zona_seleccionada].dias_registrados == 0) {
        printf("\nERROR: La zona '%s' no tiene datos registrados.\n", 
               zonas[zona_seleccionada].nombre);
        return;
    }
    
    printf("\n=== ANALISIS DE TENDENCIAS: %s ===\n", zonas[zona_seleccionada].nombre);
    printf("════════════════════════════════════════════════════════\n");
    
    // 1. GRÁFICO DE TENDENCIAS (ASCII)
    printf("\n1. GRAFICOS DE TENDENCIAS (ultimos %d dias):\n", 
           zonas[zona_seleccionada].dias_registrados);
    printf("──────────────────────────────────────────────────────\n");
    
    // Gráfico de CO2
    printf("\nCO2 (ppm) - Limite OMS: %.1f\n", LIMITE_CO2_OMS);
    printf("Dia  | Valor  | Grafico\n");
    printf("-----|--------|");
    for(int i = 0; i < 40; i++) printf("-");
    printf("\n");
    
    int dias_grafico = (zonas[zona_seleccionada].dias_registrados > 10) ? 10 : zonas[zona_seleccionada].dias_registrados;
    
    for(int i = 0; i < dias_grafico; i++) {
        printf("%-4d | %-6.1f | ", i+1, zonas[zona_seleccionada].historico[i].co2);
        
        // Crear gráfico ASCII
        int barras = (int)(zonas[zona_seleccionada].historico[i].co2 / LIMITE_CO2_OMS * 20);
        if(barras > 40) barras = 40;
        if(barras < 0) barras = 0;
        
        for(int j = 0; j < barras; j++) {
            if(zonas[zona_seleccionada].historico[i].co2 > LIMITE_CO2_OMS) {
                printf("█"); // Excede límite
            } else {
                printf("▓"); // Normal
            }
        }
        printf(" %.1f\n", zonas[zona_seleccionada].historico[i].co2);
    }
    
    // Gráfico de PM2.5
    printf("\nPM2.5 (ug/m3) - Limite OMS: %.1f\n", LIMITE_PM25_OMS);
    printf("Dia  | Valor  | Grafico\n");
    printf("-----|--------|");
    for(int i = 0; i < 40; i++) printf("-");
    printf("\n");
    
    for(int i = 0; i < dias_grafico; i++) {
        printf("%-4d | %-6.1f | ", i+1, zonas[zona_seleccionada].historico[i].pm25);
        
        int barras = (int)(zonas[zona_seleccionada].historico[i].pm25 / LIMITE_PM25_OMS * 20);
        if(barras > 40) barras = 40;
        if(barras < 0) barras = 0;
        
        for(int j = 0; j < barras; j++) {
            if(zonas[zona_seleccionada].historico[i].pm25 > LIMITE_PM25_OMS) {
                printf("█");
            } else {
                printf("▓");
            }
        }
        printf(" %.1f\n", zonas[zona_seleccionada].historico[i].pm25);
    }
    
    // 2. ANÁLISIS ESTADÍSTICO
    printf("\n2. ANALISIS ESTADISTICO:\n");
    printf("──────────────────────────────────────────────────────\n");
    
    // Calcular estadísticas
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    float max_co2 = 0, max_so2 = 0, max_no2 = 0, max_pm25 = 0;
    float min_co2 = 999999, min_so2 = 999999, min_no2 = 999999, min_pm25 = 999999;
    
    for(int i = 0; i < zonas[zona_seleccionada].dias_registrados; i++) {
        // Sumas para promedio
        suma_co2 += zonas[zona_seleccionada].historico[i].co2;
        suma_so2 += zonas[zona_seleccionada].historico[i].so2;
        suma_no2 += zonas[zona_seleccionada].historico[i].no2;
        suma_pm25 += zonas[zona_seleccionada].historico[i].pm25;
        
        // Máximos
        if(zonas[zona_seleccionada].historico[i].co2 > max_co2) 
            max_co2 = zonas[zona_seleccionada].historico[i].co2;
        if(zonas[zona_seleccionada].historico[i].so2 > max_so2) 
            max_so2 = zonas[zona_seleccionada].historico[i].so2;
        if(zonas[zona_seleccionada].historico[i].no2 > max_no2) 
            max_no2 = zonas[zona_seleccionada].historico[i].no2;
        if(zonas[zona_seleccionada].historico[i].pm25 > max_pm25) 
            max_pm25 = zonas[zona_seleccionada].historico[i].pm25;
        
        // Mínimos
        if(zonas[zona_seleccionada].historico[i].co2 < min_co2) 
            min_co2 = zonas[zona_seleccionada].historico[i].co2;
        if(zonas[zona_seleccionada].historico[i].so2 < min_so2) 
            min_so2 = zonas[zona_seleccionada].historico[i].so2;
        if(zonas[zona_seleccionada].historico[i].no2 < min_no2) 
            min_no2 = zonas[zona_seleccionada].historico[i].no2;
        if(zonas[zona_seleccionada].historico[i].pm25 < min_pm25) 
            min_pm25 = zonas[zona_seleccionada].historico[i].pm25;
    }
    
    // Calcular promedios
    float promedio_co2 = suma_co2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_so2 = suma_so2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_no2 = suma_no2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_pm25 = suma_pm25 / zonas[zona_seleccionada].dias_registrados;
    
    printf("ESTADISTICAS GENERALES (%d dias):\n", zonas[zona_seleccionada].dias_registrados);
    printf("                 | Promedio | Maximo  | Minimo  | Limite OMS | Estado\n");
    printf("-----------------|----------|---------|---------|------------|--------\n");
    printf("CO2 (ppm)        | %-8.1f | %-7.1f | %-7.1f | %-10.1f | %s\n", 
           promedio_co2, max_co2, min_co2, LIMITE_CO2_OMS,
           (promedio_co2 > LIMITE_CO2_OMS) ? "EXCEDE" : "OK");
    printf("SO2 (ug/m3)      | %-8.1f | %-7.1f | %-7.1f | %-10.1f | %s\n", 
           promedio_so2, max_so2, min_so2, LIMITE_SO2_OMS,
           (promedio_so2 > LIMITE_SO2_OMS) ? "EXCEDE" : "OK");
    printf("NO2 (ug/m3)      | %-8.1f | %-7.1f | %-7.1f | %-10.1f | %s\n", 
           promedio_no2, max_no2, min_no2, LIMITE_NO2_OMS,
           (promedio_no2 > LIMITE_NO2_OMS) ? "EXCEDE" : "OK");
    printf("PM2.5 (ug/m3)    | %-8.1f | %-7.1f | %-7.1f | %-10.1f | %s\n", 
           promedio_pm25, max_pm25, min_pm25, LIMITE_PM25_OMS,
           (promedio_pm25 > LIMITE_PM25_OMS) ? "EXCEDE" : "OK");
    
    // 3. ANÁLISIS DE TENDENCIAS
    printf("\n3. ANALISIS DE TENDENCIAS:\n");
    printf("──────────────────────────────────────────────────────\n");
    
    if(zonas[zona_seleccionada].dias_registrados >= 3) {
        // Comparar primeros 3 días vs últimos 3 días
        float promedio_reciente = (zonas[zona_seleccionada].historico[0].co2 + 
                                  zonas[zona_seleccionada].historico[1].co2 + 
                                  zonas[zona_seleccionada].historico[2].co2) / 3;
        
        int dias_antiguos = zonas[zona_seleccionada].dias_registrados - 1;
        float promedio_antiguo = (zonas[zona_seleccionada].historico[dias_antiguos].co2 + 
                                 zonas[zona_seleccionada].historico[dias_antiguos-1].co2 + 
                                 zonas[zona_seleccionada].historico[dias_antiguos-2].co2) / 3;
        
        printf("CO2 - Tendencia:\n");
        printf("  Promedio reciente (3 dias): %.1f ppm\n", promedio_reciente);
        printf("  Promedio anterior (3 dias): %.1f ppm\n", promedio_antiguo);
        
        if(promedio_reciente > promedio_antiguo) {
            float incremento = ((promedio_reciente - promedio_antiguo) / promedio_antiguo) * 100;
            printf("  Tendencia: EMPEORANDO (%.1f%% mas alto)\n", incremento);
        } else {
            float mejora = ((promedio_antiguo - promedio_reciente) / promedio_antiguo) * 100;
            printf("  Tendencia: MEJORANDO (%.1f%% mas bajo)\n", mejora);
        }
    }
    
    // 4. DÍAS PROBLEMÁTICOS
    printf("\n4. DIAS PROBLEMATICOS:\n");
    printf("──────────────────────────────────────────────────────\n");
    
    int dias_exceso = 0;
    printf("Dias con excesos de limites OMS:\n");
    
    for(int i = 0; i < zonas[zona_seleccionada].dias_registrados; i++) {
        int excesos_dia = 0;
        char problemas[200] = "";
        
        if(zonas[zona_seleccionada].historico[i].co2 > LIMITE_CO2_OMS) {
            excesos_dia++;
            strcat(problemas, "CO2 ");
        }
        if(zonas[zona_seleccionada].historico[i].so2 > LIMITE_SO2_OMS) {
            excesos_dia++;
            strcat(problemas, "SO2 ");
        }
        if(zonas[zona_seleccionada].historico[i].no2 > LIMITE_NO2_OMS) {
            excesos_dia++;
            strcat(problemas, "NO2 ");
        }
        if(zonas[zona_seleccionada].historico[i].pm25 > LIMITE_PM25_OMS) {
            excesos_dia++;
            strcat(problemas, "PM2.5 ");
        }
        
        if(excesos_dia > 0) {
            dias_exceso++;
            printf("  Dia %d: %d exceso(s) - %s\n", i+1, excesos_dia, problemas);
        }
    }
    
    printf("\nRESUMEN: %d de %d dias con excesos (%.1f%%)\n", 
           dias_exceso, zonas[zona_seleccionada].dias_registrados,
           (float)dias_exceso / zonas[zona_seleccionada].dias_registrados * 100);
    
    // 5. RECOMENDACIONES BASADAS EN TENDENCIAS
    printf("\n5. RECOMENDACIONES BASADAS EN TENDENCIAS:\n");
    printf("──────────────────────────────────────────────────────\n");
    
    float porcentaje_exceso = (float)dias_exceso / zonas[zona_seleccionada].dias_registrados * 100;
    
    if(porcentaje_exceso > 50) {
        printf("CRITICO: Mas del 50%% de dias con excesos\n");
        printf("- Implementar medidas de emergencia\n");
        printf("- Restricciones de trafico permanentes\n");
        printf("- Monitoreo continuo\n");
    } else if(porcentaje_exceso > 25) {
        printf("PREOCUPANTE: Mas del 25%% de dias con excesos\n");
        printf("- Reforzar medidas preventivas\n");
        printf("- Alertas tempranas\n");
        printf("- Revision de politicas ambientales\n");
    } else if(porcentaje_exceso > 0) {
        printf("MODERADO: Algunos dias con excesos\n");
        printf("- Mantener monitoreo regular\n");
        printf("- Medidas preventivas ocasionales\n");
    } else {
        printf("BUENO: Sin excesos registrados\n");
        printf("- Mantener politicas actuales\n");
        printf("- Continuar monitoreo preventivo\n");
    }
    
    printf("\n════════════════════════════════════════════════════════\n");
}
