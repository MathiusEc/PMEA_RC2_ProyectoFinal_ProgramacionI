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
 2. Predicción: Estima los niveles de contaminación para las próximas 24 horas    }
    
    // 4. DIAS PROBLEMATICOS:    // 5. RECOMENDACIONES BASADAS EN TENDENCIAS:ndo
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
 #include <time.h>
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
        printf("===========================================\n");
        printf("1. Registrar Datos                          \n");
        printf("2. Monitoreo                                \n");
        printf("3. Tendencias e Histórico                 \n");
        printf("4. Pronóstico 24 Horas                    \n");
        printf("5. Gestión de Datos                       \n");
        printf("6. Historial con Fechas                   \n");
        printf("7. Exportar Reportes                      \n");
        printf("8. Estado del Sistema                     \n");
        printf("0. Salir                                   \n");
        printf("===========================================\n");
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
        printf("Error al guardar datos de la zona %s\n", zona->nombre);
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

// ================= FUNCION DE VALIDACION DE DATOS =================
void funcionValidarDatosdeRegistro(float *valor, char *nombre_dato, float min_val, float max_val) {
    int val;
    do {
        printf("%s: ", nombre_dato);
        fflush(stdin);
        val = scanf("%f", valor);
        fflush(stdin);
        
        if(val != 1) {
            printf("ERROR: Ingrese un numero valido.\n");
        } else if(*valor < min_val || *valor > max_val) {
            printf("ERROR: El valor debe estar entre %.1f y %.1f\n", min_val, max_val);
        }
    } while(val != 1 || *valor < min_val || *valor > max_val);
}

// ================= FUNCION DE REGISTRO DIARIO =================
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
            printf("Opcion invalida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || id_zona < 1 || id_zona > MAX_ZONAS);

    // Mover datos históricos (FIFO - lo más reciente al inicio)
    for(int i = MAX_DIAS_HISTORICOS-1; i > 0; i--) {
        zonas[id_zona - 1].historico[i] = zonas[id_zona - 1].historico[i-1];
        zonas[id_zona - 1].historico_fechas[i] = zonas[id_zona - 1].historico_fechas[i-1];
    }

    // Leer nuevos datos con validación
    printf("Ingrese los niveles de contaminantes para la zona %s:\n", zonas[id_zona - 1].nombre);
    
    // Validar datos de contaminantes con rangos específicos
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].niveles_actuales.co2, 
                                 "CO2 (ppm)", 0.0, 1000.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].niveles_actuales.so2, 
                                 "SO2 (ug/m3)", 0.0, 500.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].niveles_actuales.no2, 
                                 "NO2 (ug/m3)", 0.0, 300.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].niveles_actuales.pm25, 
                                 "PM2.5 (ug/m3)", 0.0, 200.0);

    // Registrar datos climáticos con validación
    printf("\nIngrese los datos climaticos para la zona %s:\n", zonas[id_zona - 1].nombre);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].clima_actual.temperatura, 
                                 "Temperatura (Celsius)", -20.0, 50.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].clima_actual.velocidad_viento, 
                                 "Velocidad del viento (km/h)", 0.0, 120.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].clima_actual.humedad, 
                                 "Humedad (%)", 0.0, 100.0);
    
    funcionValidarDatosdeRegistro(&zonas[id_zona - 1].clima_actual.presion_atmosferica, 
                                 "Presion (hPa)", 900.0, 1100.0);

    // Actualizar niveles actuales
    zonas[id_zona - 1].historico[0] = zonas[id_zona - 1].niveles_actuales;
    
    // Actualizar histórico con fechas (fecha actual)
    time_t tiempo_actual;
    struct tm *info_tiempo;
    time(&tiempo_actual);
    info_tiempo = localtime(&tiempo_actual);
    
    zonas[id_zona - 1].historico_fechas[0].fecha.dia = info_tiempo->tm_mday;
    zonas[id_zona - 1].historico_fechas[0].fecha.mes = info_tiempo->tm_mon + 1;
    zonas[id_zona - 1].historico_fechas[0].fecha.año = info_tiempo->tm_year + 1900;
    zonas[id_zona - 1].historico_fechas[0].niveles = zonas[id_zona - 1].niveles_actuales;
    zonas[id_zona - 1].historico_fechas[0].clima = zonas[id_zona - 1].clima_actual;

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
    // Obtener fecha y hora actual
    time_t tiempo_actual;
    struct tm *info_tiempo;
    time(&tiempo_actual);
    info_tiempo = localtime(&tiempo_actual);
    
    printf("=== MONITOREO ACTUAL DE CALIDAD DEL AIRE ===\n");
    printf("Fecha: %02d/%02d/%04d - Hora: %02d:%02d\n", 
           info_tiempo->tm_mday, 
           info_tiempo->tm_mon + 1, 
           info_tiempo->tm_year + 1900,
           info_tiempo->tm_hour,
           info_tiempo->tm_min);
    printf("=============================================\n\n");
    
    // DASHBOARD GENERAL - Vista rápida de todas las zonas
    printf("ESTADO ACTUAL DEL SISTEMA:\n");
    printf("===========================================\n");
    
    int zonas_activas = 0;
    int alertas_criticas = 0;
    
    for(int i = 0; i < MAX_ZONAS; i++) {
        if(zonas[i].dias_registrados > 0) {
            zonas_activas++;
            
            // Contar excesos críticos
            int excesos = 0;
            if(zonas[i].niveles_actuales.co2 > LIMITE_CO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.so2 > LIMITE_SO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.no2 > LIMITE_NO2_OMS) excesos++;
            if(zonas[i].niveles_actuales.pm25 > LIMITE_PM25_OMS) excesos++;
            
            // Determinar estado visual
            char estado_icono[20];
            if(excesos == 0) {
                strcpy(estado_icono, "BUENO");
            } else if(excesos <= 1) {
                strcpy(estado_icono, "MODERADO");
            } else if(excesos <= 2) {
                strcpy(estado_icono, "DANINO");
                alertas_criticas++;
            } else {
                strcpy(estado_icono, "CRITICO");
                alertas_criticas++;
            }
            
            printf("%-20s | %s\n", zonas[i].nombre, estado_icono);
        } else {
            printf("%-20s | SIN DATOS\n", zonas[i].nombre);
        }
    }
    
    printf("\nRESUMEN: %d/%d zonas activas | %d alertas criticas\n", 
           zonas_activas, MAX_ZONAS, alertas_criticas);
    
    // SELECCIÓN DE ZONA PARA MONITOREO DETALLADO
    printf("\n===========================================\n");
    printf("MONITOREO DETALLADO POR ZONA\n");
    printf("===========================================\n");
    
    int zona_seleccionada, val;
    
    printf("\nZONAS DISPONIBLES:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s", zonas[i].id_zona, zonas[i].nombre);
        if(zonas[i].dias_registrados > 0) {
            // Mostrar fecha del último registro
            printf(" (Ultimo: %02d/%02d/%04d)", 
                   zonas[i].historico_fechas[0].fecha.dia,
                   zonas[i].historico_fechas[0].fecha.mes,
                   zonas[i].historico_fechas[0].fecha.año);
        } else {
            printf(" (Sin datos)");
        }
        printf("\n");
    }
    
    // Seleccionar zona
    do {
        printf("\nSeleccione la zona para monitoreo actual (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &zona_seleccionada);
        fflush(stdin);
        
        if(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS) {
            printf("Opcion invalida. Por favor, intente de nuevo.\n");
        }
    } while(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS);
    
    zona_seleccionada--;
    
    // Verificar si la zona tiene datos
    if(zonas[zona_seleccionada].dias_registrados == 0) {
        printf("\nERROR: La zona '%s' no tiene datos registrados.\n", 
               zonas[zona_seleccionada].nombre);
        printf("   Registre datos primero usando la opcion 1 del menu.\n");
        return;
    }
    
    // ================= MONITOREO ACTUAL =================
    printf("\nMONITOREO ACTUAL: %s\n", zonas[zona_seleccionada].nombre);
    printf("===========================================\n");
    
    // Fecha del último registro
    printf("Ultimo registro: %02d/%02d/%04d\n", 
           zonas[zona_seleccionada].historico_fechas[0].fecha.dia,
           zonas[zona_seleccionada].historico_fechas[0].fecha.mes,
           zonas[zona_seleccionada].historico_fechas[0].fecha.año);
    
    // 1. NIVELES ACTUALES DE CONTAMINANTES
    printf("\nNIVELES DE CONTAMINANTES ACTUALES:\n");
    printf("-------------------------------------------\n");
    
    // CO2
    printf("CO2:   %6.1f ppm   | Limite: %6.1f | ", 
           zonas[zona_seleccionada].niveles_actuales.co2, LIMITE_CO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.co2 > LIMITE_CO2_OMS) {
        printf("EXCEDE (%.1f%%)\n", 
               (zonas[zona_seleccionada].niveles_actuales.co2 / LIMITE_CO2_OMS) * 100 - 100);
    } else {
        printf("NORMAL\n");
    }
    
    // SO2
    printf("SO2:   %6.1f ug/m3 | Limite: %6.1f | ",
           zonas[zona_seleccionada].niveles_actuales.so2, LIMITE_SO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.so2 > LIMITE_SO2_OMS) {
        printf("EXCEDE (%.1f%%)\n", 
               (zonas[zona_seleccionada].niveles_actuales.so2 / LIMITE_SO2_OMS) * 100 - 100);
    } else {
        printf("NORMAL\n");
    }
    
    // NO2
    printf("NO2:   %6.1f ug/m3 | Limite: %6.1f | ",
           zonas[zona_seleccionada].niveles_actuales.no2, LIMITE_NO2_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.no2 > LIMITE_NO2_OMS) {
        printf("EXCEDE (%.1f%%)\n", 
               (zonas[zona_seleccionada].niveles_actuales.no2 / LIMITE_NO2_OMS) * 100 - 100);
    } else {
        printf("NORMAL\n");
    }
    
    // PM2.5
    printf("PM2.5: %6.1f ug/m3 | Limite: %6.1f | ",
           zonas[zona_seleccionada].niveles_actuales.pm25, LIMITE_PM25_OMS);
    if(zonas[zona_seleccionada].niveles_actuales.pm25 > LIMITE_PM25_OMS) {
        printf("EXCEDE (%.1f%%)\n", 
               (zonas[zona_seleccionada].niveles_actuales.pm25 / LIMITE_PM25_OMS) * 100 - 100);
    } else {
        printf("NORMAL\n");
    }
    
    // 2. CONDICIONES CLIMÁTICAS ACTUALES
    printf("\nCONDICIONES CLIMATICAS ACTUALES:\n");
    printf("-------------------------------------------\n");
    printf("Temperatura:       %6.1f°C\n", zonas[zona_seleccionada].clima_actual.temperatura);
    printf("Viento:           %6.1f km/h\n", zonas[zona_seleccionada].clima_actual.velocidad_viento);
    printf("Humedad:          %6.1f%%\n", zonas[zona_seleccionada].clima_actual.humedad);
    printf("Presion:          %6.1f hPa\n", zonas[zona_seleccionada].clima_actual.presion_atmosferica);
    
    // 3. ÍNDICE DE CALIDAD DEL AIRE (ICA)
    printf("\nINDICE DE CALIDAD DEL AIRE:\n");
    printf("-------------------------------------------\n");
    
    int contaminantes_excedidos = 0;
    if(zonas[zona_seleccionada].niveles_actuales.co2 > LIMITE_CO2_OMS) contaminantes_excedidos++;
    if(zonas[zona_seleccionada].niveles_actuales.so2 > LIMITE_SO2_OMS) contaminantes_excedidos++;
    if(zonas[zona_seleccionada].niveles_actuales.no2 > LIMITE_NO2_OMS) contaminantes_excedidos++;
    if(zonas[zona_seleccionada].niveles_actuales.pm25 > LIMITE_PM25_OMS) contaminantes_excedidos++;
    
    if(contaminantes_excedidos == 0) {
        printf("BUENO - Calidad del aire satisfactoria\n");
        printf("   Seguro para actividades al aire libre\n");
    } else if(contaminantes_excedidos == 1) {
        printf("MODERADO - Calidad del aire aceptable\n");
        printf("   Grupos sensibles deben limitar actividades prolongadas\n");
    } else if(contaminantes_excedidos <= 2) {
        printf("DANINO - Calidad del aire no saludable\n");
        printf("   Todos deben reducir actividades al aire libre\n");
    } else {
        printf("CRITICO - Calidad del aire peligrosa\n");
        printf("   Evitar actividades al aire libre\n");
    }
    
    // 4. RECOMENDACIONES INMEDIATAS
    printf("\nRECOMENDACIONES INMEDIATAS:\n");
    printf("-------------------------------------------\n");
    
    if(contaminantes_excedidos == 0) {
        printf("- Condiciones favorables para actividades exteriores\n");
        printf("- Mantener monitoreo de rutina\n");
        printf("- Ventilar espacios interiores\n");
    } else {
        printf("- Limitar tiempo de exposicion al aire libre\n");
        printf("- Usar mascarilla si es necesario salir\n");
        printf("- Mantener ventanas cerradas\n");
        printf("- Activar purificadores de aire si estan disponibles\n");
        
        if(contaminantes_excedidos >= 3) {
            printf("- URGENTE: Grupos vulnerables deben permanecer en interiores\n");
            printf("- Suspender actividades deportivas al aire libre\n");
        }
    }
    
    printf("\n===========================================\n");
}

void mostrarEstadoSistema(ZonaUrbana zonas[])
{
    printf("Sistema de Monitoreo - Quito\n");
    printf("==================================\n\n");
    
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
    printf("======================================================\n");
    printf("GRAFICOS Y ANALISIS DE DATOS HISTORICOS\n");
    printf("======================================================\n\n");
    
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
    printf("========================================================\n");
    
    // 1. HISTORIAL DETALLADO DE DATOS
    printf("\n1. HISTORIAL DETALLADO DE CONTAMINANTES:\n");
    printf("------------------------------------------------------\n");
    
    printf("Fecha      | CO2    | SO2    | NO2    | PM2.5  | Estado General\n");
    printf("-----------|--------|--------|--------|--------|---------------\n");
    
    int dias_mostrar;
    if(zonas[zona_seleccionada].dias_registrados > 10) {
        dias_mostrar = 10;
    } else {
        dias_mostrar = zonas[zona_seleccionada].dias_registrados;
    }
    
    for(int i = 0; i < dias_mostrar; i++) {
        // Contar excesos para determinar estado
        int excesos = 0;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) excesos++;
        
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
        
        printf("%02d/%02d/%04d | %-6.1f | %-6.1f | %-6.1f | %-6.1f | %s",
               zonas[zona_seleccionada].historico_fechas[i].fecha.dia,
               zonas[zona_seleccionada].historico_fechas[i].fecha.mes,
               zonas[zona_seleccionada].historico_fechas[i].fecha.año,
               zonas[zona_seleccionada].historico_fechas[i].niveles.co2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.so2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.no2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.pm25,
               estado);
        
        // Marcar si excede límites OMS
        if(excesos > 0) {
            printf(" (");
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) printf("CO2 ");
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) printf("SO2 ");
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) printf("NO2 ");
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) printf("PM2.5 ");
            printf("exceden)");
        }
        printf("\n");
    }
    
    // 2. ANÁLISIS ESTADÍSTICO
    printf("\n2. ANALISIS ESTADISTICO:\n");
    printf("------------------------------------------------------\n");
    
    // Calcular estadísticas
    float suma_co2 = 0, suma_so2 = 0, suma_no2 = 0, suma_pm25 = 0;
    float max_co2 = 0, max_so2 = 0, max_no2 = 0, max_pm25 = 0;
    float min_co2 = 999999, min_so2 = 999999, min_no2 = 999999, min_pm25 = 999999;
    
    for(int i = 0; i < zonas[zona_seleccionada].dias_registrados; i++) {
        // Sumas para promedio
        suma_co2 += zonas[zona_seleccionada].historico_fechas[i].niveles.co2;
        suma_so2 += zonas[zona_seleccionada].historico_fechas[i].niveles.so2;
        suma_no2 += zonas[zona_seleccionada].historico_fechas[i].niveles.no2;
        suma_pm25 += zonas[zona_seleccionada].historico_fechas[i].niveles.pm25;
        
        // Máximos
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > max_co2) 
            max_co2 = zonas[zona_seleccionada].historico_fechas[i].niveles.co2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > max_so2) 
            max_so2 = zonas[zona_seleccionada].historico_fechas[i].niveles.so2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > max_no2) 
            max_no2 = zonas[zona_seleccionada].historico_fechas[i].niveles.no2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > max_pm25) 
            max_pm25 = zonas[zona_seleccionada].historico_fechas[i].niveles.pm25;
        
        // Mínimos
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 < min_co2) 
            min_co2 = zonas[zona_seleccionada].historico_fechas[i].niveles.co2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 < min_so2) 
            min_so2 = zonas[zona_seleccionada].historico_fechas[i].niveles.so2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 < min_no2) 
            min_no2 = zonas[zona_seleccionada].historico_fechas[i].niveles.no2;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 < min_pm25) 
            min_pm25 = zonas[zona_seleccionada].historico_fechas[i].niveles.pm25;
    }
    
    // Calcular promedios
    float promedio_co2 = suma_co2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_so2 = suma_so2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_no2 = suma_no2 / zonas[zona_seleccionada].dias_registrados;
    float promedio_pm25 = suma_pm25 / zonas[zona_seleccionada].dias_registrados;
    
    printf("ESTADISTICAS GENERALES (%d dias):\n", zonas[zona_seleccionada].dias_registrados);
    printf("                 | Promedio | Maximo  | Minimo  | Limite OMS | Estado\n");
    printf("-----------------|----------|---------|---------|------------|--------\n");
    printf("CO2 (ppm)\t| %.1f\t| %.1f\t| %.1f\t| %.1f\t| ", 
           promedio_co2, max_co2, min_co2, LIMITE_CO2_OMS);
    if(promedio_co2 > LIMITE_CO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("SO2 (ug/m3)\t| %.1f\t| %.1f\t| %.1f\t| %.1f\t| ", 
           promedio_so2, max_so2, min_so2, LIMITE_SO2_OMS);
    if(promedio_so2 > LIMITE_SO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("NO2 (ug/m3)\t| %.1f\t| %.1f\t| %.1f\t| %.1f\t| ", 
           promedio_no2, max_no2, min_no2, LIMITE_NO2_OMS);
    if(promedio_no2 > LIMITE_NO2_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    printf("PM2.5 (ug/m3)\t| %.1f\t| %.1f\t| %.1f\t| %.1f\t| ", 
           promedio_pm25, max_pm25, min_pm25, LIMITE_PM25_OMS);
    if(promedio_pm25 > LIMITE_PM25_OMS) {
        printf("EXCEDE\n");
    } else {
        printf("OK\n");
    }
    
    // 3. ANÁLISIS DE TENDENCIAS DETALLADO
    printf("\n3. ANALISIS DE TENDENCIAS DETALLADO:\n");
    printf("-------------------------------------------------------------\n");
    
    if(zonas[zona_seleccionada].dias_registrados > 1) {
        // Calcular promedios históricos ponderados
        int dias_para_promedio;
        if(zonas[zona_seleccionada].dias_registrados > 7) {
            dias_para_promedio = 7;
        } else {
            dias_para_promedio = zonas[zona_seleccionada].dias_registrados;
        }
        
        float promedio_pond_co2 = 0, promedio_pond_so2 = 0, promedio_pond_no2 = 0, promedio_pond_pm25 = 0;
        
        for(int i = 0; i < dias_para_promedio; i++) {
            promedio_pond_co2 += zonas[zona_seleccionada].historico[i].co2;
            promedio_pond_so2 += zonas[zona_seleccionada].historico[i].so2;
            promedio_pond_no2 += zonas[zona_seleccionada].historico[i].no2;
            promedio_pond_pm25 += zonas[zona_seleccionada].historico[i].pm25;
        }
        
        promedio_pond_co2 /= dias_para_promedio;
        promedio_pond_so2 /= dias_para_promedio;
        promedio_pond_no2 /= dias_para_promedio;
        promedio_pond_pm25 /= dias_para_promedio;
        
        printf("PROMEDIO ULTIMOS %d DIAS vs NIVEL ACTUAL:\n", dias_para_promedio);
        printf("Contaminante\t| Promedio\t| Actual\t| Diferencia\t| Tendencia\n");
        printf("----------------|---------------|---------------|---------------|----------\n");
        
        // Análisis CO2
        float dif_co2 = zonas[zona_seleccionada].niveles_actuales.co2 - promedio_pond_co2;
        float porc_co2 = 0;
        if(promedio_pond_co2 != 0) {
            porc_co2 = absoluto(dif_co2 / promedio_pond_co2 * 100);
        }
        
        printf("CO2 (ppm)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_co2, zonas[zona_seleccionada].niveles_actuales.co2, dif_co2);
        
        if(zonas[zona_seleccionada].niveles_actuales.co2 > promedio_pond_co2) {
            printf("SUBIENDO ");
        } else {
            printf("BAJANDO  ");
        }
        printf("(%.1f%%)\n", porc_co2);
        
        // Análisis SO2
        float dif_so2 = zonas[zona_seleccionada].niveles_actuales.so2 - promedio_pond_so2;
        float porc_so2 = 0;
        if(promedio_pond_so2 != 0) {
            porc_so2 = absoluto(dif_so2 / promedio_pond_so2 * 100);
        }
        
        printf("SO2 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_so2, zonas[zona_seleccionada].niveles_actuales.so2, dif_so2);
        
        if(zonas[zona_seleccionada].niveles_actuales.so2 > promedio_pond_so2) {
            printf("SUBIENDO ");
        } else {
            printf("BAJANDO  ");
        }
        printf("(%.1f%%)\n", porc_so2);
        
        // Análisis NO2
        float dif_no2 = zonas[zona_seleccionada].niveles_actuales.no2 - promedio_pond_no2;
        float porc_no2 = 0;
        if(promedio_pond_no2 != 0) {
            porc_no2 = absoluto(dif_no2 / promedio_pond_no2 * 100);
        }
        
        printf("NO2 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_no2, zonas[zona_seleccionada].niveles_actuales.no2, dif_no2);
        
        if(zonas[zona_seleccionada].niveles_actuales.no2 > promedio_pond_no2) {
            printf("SUBIENDO ");
        } else {
            printf("BAJANDO  ");
        }
        printf("(%.1f%%)\n", porc_no2);
        
        // Análisis PM2.5
        float dif_pm25 = zonas[zona_seleccionada].niveles_actuales.pm25 - promedio_pond_pm25;
        float porc_pm25 = 0;
        if(promedio_pond_pm25 != 0) {
            porc_pm25 = absoluto(dif_pm25 / promedio_pond_pm25 * 100);
        }
        
        printf("PM2.5 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_pm25, zonas[zona_seleccionada].niveles_actuales.pm25, dif_pm25);
        
        if(zonas[zona_seleccionada].niveles_actuales.pm25 > promedio_pond_pm25) {
            printf("SUBIENDO ");
        } else {
            printf("BAJANDO  ");
        }
        printf(" (%.1f%%)\n", porc_pm25);
    }
    
    if(zonas[zona_seleccionada].dias_registrados >= 3) {
        printf("\nCOMPARACION TEMPORAL (Primeros vs Ultimos dias):\n");
        
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
    printf("-----------------------------------------------------------\n");
    
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
    printf("----------------------------------------------------------\n");
    
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
    
    printf("\n=======================================================\n");
}

// ============= FUNCIONES DE PREDICCION 24H =============

// Función principal para predicción de contaminación 24h
void prediccionContaminacion24h(ZonaUrbana zonas[]) {
    int zona_seleccionada, i;
    
    printf("\n=======================================================\n");
    printf("           PREDICCION DE CONTAMINACION 24H            \n");
    printf("=======================================================\n");
    
    // Mostrar zonas disponibles
    printf("\nZonas disponibles para prediccion:\n");
    for(i = 0; i < MAX_ZONAS; i++) {
        if(zonas[i].dias_registrados > 0) {
            printf("%d. %s (ID: %d) - %d dias de datos\n", 
                   i+1, zonas[i].nombre, zonas[i].id_zona, zonas[i].dias_registrados);
        }
    }
    
    int val;
    do {
        printf("\nSeleccione una zona (1-5): ");
        val = scanf("%d", &zona_seleccionada);
        fflush(stdin);
        
        if(val != 1) {
            printf("ERROR: Ingrese un numero valido.\n");
            continue;
        }
        
        zona_seleccionada--; // Ajustar a índice del array
        
        if(zona_seleccionada < 0 || zona_seleccionada >= MAX_ZONAS) {
            printf("ERROR: Zona invalida. Ingrese un numero entre 1 y 5.\n");
            continue;
        }
        
        if(zonas[zona_seleccionada].dias_registrados < 3) {
            printf("ERROR: Se necesitan al menos 3 dias de datos para hacer predicciones.\n");
            printf("Esta zona tiene solo %d dias registrados.\n", 
                   zonas[zona_seleccionada].dias_registrados);
            continue;
        }
        
        break;
    } while(1);
    
    ZonaUrbana *zona = &zonas[zona_seleccionada];
    
    printf("\n=======================================================\n");
    printf("PREDICCION PARA: %s (ID: %d)\n", zona->nombre, zona->id_zona);
    printf("=======================================================\n");
    
    // Calcular predicciones para cada contaminante
    float pred_co2, pred_so2, pred_no2, pred_pm25;
    
    // Extraer datos históricos por contaminante
    float hist_co2[MAX_DIAS_HISTORICOS], hist_so2[MAX_DIAS_HISTORICOS];
    float hist_no2[MAX_DIAS_HISTORICOS], hist_pm25[MAX_DIAS_HISTORICOS];
    
    for(i = 0; i < zona->dias_registrados; i++) {
        hist_co2[i] = zona->historico[i].co2;
        hist_so2[i] = zona->historico[i].so2;
        hist_no2[i] = zona->historico[i].no2;
        hist_pm25[i] = zona->historico[i].pm25;
    }
    
    // Calcular predicciones base
    pred_co2 = calcularPrediccion(hist_co2, zona->dias_registrados);
    pred_so2 = calcularPrediccion(hist_so2, zona->dias_registrados);
    pred_no2 = calcularPrediccion(hist_no2, zona->dias_registrados);
    pred_pm25 = calcularPrediccion(hist_pm25, zona->dias_registrados);
    
    // Predecir condiciones climáticas a 24h
    DatosClimaticos clima_predicho = predecirClima24h(zona);
    
    // Ajustar predicciones por condiciones climáticas predichas
    pred_co2 = ajustarPorClima(pred_co2, clima_predicho);
    pred_so2 = ajustarPorClima(pred_so2, clima_predicho);
    pred_no2 = ajustarPorClima(pred_no2, clima_predicho);
    pred_pm25 = ajustarPorClima(pred_pm25, clima_predicho);
    
    // Mostrar predicciones
    printf("\nPREDICCIONES PARA LAS PROXIMAS 24 HORAS:\n");
    printf("-------------------------------------------------------\n");
    printf("CO2:   %.2f ppm (Limite OMS: %.2f ppm)\n", pred_co2, LIMITE_CO2_OMS);
    printf("SO2:   %.2f ug/m3 (Limite OMS: %.2f ug/m3)\n", pred_so2, LIMITE_SO2_OMS);
    printf("NO2:   %.2f ug/m3 (Limite OMS: %.2f ug/m3)\n", pred_no2, LIMITE_NO2_OMS);
    printf("PM2.5: %.2f ug/m3 (Limite OMS: %.2f ug/m3)\n", pred_pm25, LIMITE_PM25_OMS);
    
    // Determinar niveles de alerta
    int alerta_co2 = determinarNivelAlerta(pred_co2, 0);
    int alerta_so2 = determinarNivelAlerta(pred_so2, 1);
    int alerta_no2 = determinarNivelAlerta(pred_no2, 2);
    int alerta_pm25 = determinarNivelAlerta(pred_pm25, 3);
    
    // Mostrar alertas
    printf("\nNIVELES DE ALERTA PREDICHOS:\n");
    printf("-------------------------------------------------------\n");
    
    char *niveles[] = {"VERDE", "AMARILLO", "NARANJA", "ROJO"};
    printf("CO2:   %s\n", niveles[alerta_co2]);
    printf("SO2:   %s\n", niveles[alerta_so2]);
    printf("NO2:   %s\n", niveles[alerta_no2]);
    printf("PM2.5: %s\n", niveles[alerta_pm25]);
    
    // Determinar nivel de alerta general (el más alto)
    int alerta_general = alerta_co2;
    if(alerta_so2 > alerta_general) alerta_general = alerta_so2;
    if(alerta_no2 > alerta_general) alerta_general = alerta_no2;
    if(alerta_pm25 > alerta_general) alerta_general = alerta_pm25;
    
    printf("\nNIVEL DE ALERTA GENERAL: %s\n", niveles[alerta_general]);
    
    // Mostrar condiciones climáticas predichas
    printf("\nCONDICIONES CLIMATICAS PREDICHAS:\n");
    printf("-------------------------------------------------------\n");
    printf("Temperatura:        %.1f°C\n", clima_predicho.temperatura);
    printf("Viento:             %.1f km/h\n", clima_predicho.velocidad_viento);
    printf("Humedad:            %.1f%%\n", clima_predicho.humedad);
    printf("Presion:            %.1f hPa\n", clima_predicho.presion_atmosferica);
    
    // Mostrar recomendaciones
    printf("\nRECOMENDACIONES:\n");
    printf("-------------------------------------------------------\n");
    if(alerta_general >= ALERTA_ROJA) {
        mostrarRecomendaciones(ALERTA_ROJA, "GENERAL");
    } else if(alerta_general >= ALERTA_NARANJA) {
        mostrarRecomendaciones(ALERTA_NARANJA, "GENERAL");
    } else if(alerta_general >= ALERTA_AMARILLA) {
        mostrarRecomendaciones(ALERTA_AMARILLA, "GENERAL");
    } else {
        mostrarRecomendaciones(ALERTA_VERDE, "GENERAL");
    }
    
    // Mostrar probabilidad de exceder límites
    printf("\nPROBABILIDAD DE EXCEDER LIMITES OMS:\n");
    printf("-------------------------------------------------------\n");
    if(pred_co2 > LIMITE_CO2_OMS) {
        printf("CO2:   85.0%%\n");
    } else {
        printf("CO2:   15.0%%\n");
    }
    
    if(pred_so2 > LIMITE_SO2_OMS) {
        printf("SO2:   80.0%%\n");
    } else {
        printf("SO2:   20.0%%\n");
    }
    
    if(pred_no2 > LIMITE_NO2_OMS) {
        printf("NO2:   75.0%%\n");
    } else {
        printf("NO2:   25.0%%\n");
    }
    
    if(pred_pm25 > LIMITE_PM25_OMS) {
        printf("PM2.5: 70.0%%\n");
    } else {
        printf("PM2.5: 30.0%%\n");
    }
    
    printf("\n=======================================================\n");
}

// Función auxiliar para calcular predicción usando promedio ponderado
float calcularPrediccion(float *historico, int dias_disponibles) {
    if(dias_disponibles < 3) return 0.0;
    
    float prediccion = 0.0;
    
    // Usar promedio ponderado con más peso a los días recientes
    if(dias_disponibles >= 3) {
        // Días más recientes tienen más peso
        prediccion = historico[dias_disponibles-1] * PESO_DIA_1;  // Día más reciente
        prediccion += historico[dias_disponibles-2] * PESO_DIA_2; // Segundo día
        prediccion += historico[dias_disponibles-3] * PESO_DIA_3; // Tercer día
        
        // Si hay más días, usar el peso restante distribuido
        if(dias_disponibles > 3) {
            float suma_resto = 0.0;
            for(int i = 0; i < dias_disponibles - 3; i++) {
                suma_resto += historico[i];
            }
            prediccion += (suma_resto / (dias_disponibles - 3)) * PESO_RESTO;
        }
    }
    
    return prediccion;
}

// Función auxiliar para ajustar predicción por condiciones climáticas
float ajustarPorClima(float prediccion_base, DatosClimaticos clima) {
    float factor_ajuste = 1.0;
    
    // Ajuste por temperatura (temperaturas altas aumentan algunos contaminantes)
    if(clima.temperatura > 30.0) {
        factor_ajuste *= 1.1; // Incremento del 10%
    } else if(clima.temperatura < 10.0) {
        factor_ajuste *= 1.05; // Incremento del 5%
    }
    
    // Ajuste por viento (viento fuerte dispersa contaminantes)
    if(clima.velocidad_viento > 20.0) {
        factor_ajuste *= 0.85; // Reducción del 15%
    } else if(clima.velocidad_viento < 5.0) {
        factor_ajuste *= 1.15; // Incremento del 15%
    }
    
    // Ajuste por humedad (alta humedad puede incrementar ciertos contaminantes)
    if(clima.humedad > 80.0) {
        factor_ajuste *= 1.08; // Incremento del 8%
    }
    
    // Ajuste por presión atmosférica (baja presión dificulta dispersión)
    if(clima.presion_atmosferica < 1000.0) {
        factor_ajuste *= 1.05; // Incremento del 5%
    }
    
    return prediccion_base * factor_ajuste;
}

// Función auxiliar para determinar nivel de alerta
int determinarNivelAlerta(float valor, int tipo_contaminante) {
    float limite_oms;
    
    // Determinar límite OMS según tipo de contaminante
    switch(tipo_contaminante) {
        case 0: limite_oms = LIMITE_CO2_OMS; break;  // CO2
        case 1: limite_oms = LIMITE_SO2_OMS; break;  // SO2
        case 2: limite_oms = LIMITE_NO2_OMS; break;  // NO2
        case 3: limite_oms = LIMITE_PM25_OMS; break; // PM2.5
        default: limite_oms = 100.0; break;
    }
    
    // Determinar nivel de alerta basado en múltiplos del límite OMS
    if(valor <= limite_oms) {
        return ALERTA_VERDE;
    } else if(valor <= limite_oms * 1.5) {
        return ALERTA_AMARILLA;
    } else if(valor <= limite_oms * 2.0) {
        return ALERTA_NARANJA;
    } else {
        return ALERTA_ROJA;
    }
}


// Función para calcular predicción climática usando promedio ponderado
float calcularPrediccionClimatica(float *historico_clima, int dias_disponibles) {
    if(dias_disponibles < 3) return 0.0;
    
    float prediccion = 0.0;
    
    // Usar la misma lógica de promedio ponderado que la contaminación
    if(dias_disponibles >= 3) {
        // Días más recientes tienen más peso
        prediccion = historico_clima[dias_disponibles-1] * PESO_DIA_1;  // Día más reciente
        prediccion += historico_clima[dias_disponibles-2] * PESO_DIA_2; // Segundo día
        prediccion += historico_clima[dias_disponibles-3] * PESO_DIA_3; // Tercer día
        
        // Si hay más días, usar el peso restante distribuido
        if(dias_disponibles > 3) {
            float suma_resto = 0.0;
            for(int i = 0; i < dias_disponibles - 3; i++) {
                suma_resto += historico_clima[i];
            }
            prediccion += (suma_resto / (dias_disponibles - 3)) * PESO_RESTO;
        }
    }
    
    return prediccion;
}

// Función principal para predecir clima a 24h
DatosClimaticos predecirClima24h(ZonaUrbana *zona) {
    DatosClimaticos clima_predicho;
    
    // Generar datos históricos climáticos consistentes basados en el clima actual
    // Usar variaciones determinísticas basadas en el índice del día
    float hist_temperatura[MAX_DIAS_HISTORICOS];
    float hist_viento[MAX_DIAS_HISTORICOS];
    float hist_humedad[MAX_DIAS_HISTORICOS];
    float hist_presion[MAX_DIAS_HISTORICOS];
    
    // Generar histórico climático de forma determinística
    // Usar el día como semilla para variaciones consistentes
    for(int i = 0; i < zona->dias_registrados; i++) {
        // Variación basada en el índice del día (sin aleatoriedad)
        float factor_dia = (float)(i % 7) / 7.0; // Ciclo semanal
        float variacion_temp = (factor_dia - 0.5) * 6.0; // ±3°C
        float variacion_viento = (factor_dia - 0.5) * 10.0; // ±5 km/h
        float variacion_humedad = (factor_dia - 0.5) * 20.0; // ±10%
        float variacion_presion = (factor_dia - 0.5) * 20.0; // ±10 hPa
        
        // Aplicar variaciones estacionales determinísticas
        float factor_estacional = (float)((i % 30) / 30.0); // Ciclo mensual
        variacion_temp += (factor_estacional - 0.5) * 4.0; // Variación estacional
        
        // Calcular valores históricos
        hist_temperatura[i] = zona->clima_actual.temperatura + variacion_temp;
        hist_viento[i] = zona->clima_actual.velocidad_viento + variacion_viento;
        hist_humedad[i] = zona->clima_actual.humedad + variacion_humedad;
        hist_presion[i] = zona->clima_actual.presion_atmosferica + variacion_presion;
        
        // Validar rangos durante la generación
        if(hist_temperatura[i] < -20.0) hist_temperatura[i] = -20.0;
        if(hist_temperatura[i] > 50.0) hist_temperatura[i] = 50.0;
        if(hist_viento[i] < 0.0) hist_viento[i] = 0.0;
        if(hist_viento[i] > 120.0) hist_viento[i] = 120.0;
        if(hist_humedad[i] < 0.0) hist_humedad[i] = 0.0;
        if(hist_humedad[i] > 100.0) hist_humedad[i] = 100.0;
        if(hist_presion[i] < 900.0) hist_presion[i] = 900.0;
        if(hist_presion[i] > 1100.0) hist_presion[i] = 1100.0;
    }
    
    // Calcular predicciones usando promedio ponderado - MISMA LÓGICA QUE CONTAMINANTES
    clima_predicho.temperatura = calcularPrediccionClimatica(hist_temperatura, zona->dias_registrados);
    clima_predicho.velocidad_viento = calcularPrediccionClimatica(hist_viento, zona->dias_registrados);
    clima_predicho.humedad = calcularPrediccionClimatica(hist_humedad, zona->dias_registrados);
    clima_predicho.presion_atmosferica = calcularPrediccionClimatica(hist_presion, zona->dias_registrados);
    
    // Validar rangos finales de las predicciones
    if(clima_predicho.temperatura < -20.0) clima_predicho.temperatura = -20.0;
    if(clima_predicho.temperatura > 50.0) clima_predicho.temperatura = 50.0;
    if(clima_predicho.velocidad_viento < 0.0) clima_predicho.velocidad_viento = 0.0;
    if(clima_predicho.velocidad_viento > 120.0) clima_predicho.velocidad_viento = 120.0;
    if(clima_predicho.humedad < 0.0) clima_predicho.humedad = 0.0;
    if(clima_predicho.humedad > 100.0) clima_predicho.humedad = 100.0;
    if(clima_predicho.presion_atmosferica < 900.0) clima_predicho.presion_atmosferica = 900.0;
    if(clima_predicho.presion_atmosferica > 1100.0) clima_predicho.presion_atmosferica = 1100.0;
    
    return clima_predicho;
}

void mostrarRecomendaciones(int nivel_alerta, char *contaminante) {
    switch(nivel_alerta) {
        case ALERTA_VERDE:
            printf("> Condiciones normales - Calidad del aire buena\n");
            printf("> Actividades al aire libre sin restricciones\n");
            printf("> Ideal para ejercicio y deportes al exterior\n");
            printf("> Mantener monitoreo regular\n");
            printf("> Ventilar espacios cerrados normalmente\n");
            break;
            
        case ALERTA_AMARILLA:
            printf("> Personas sensibles deben limitar actividades prolongadas al aire libre\n");
            printf("> Reducir ejercicio intenso en exteriores\n");
            printf("> Preferir transporte publico o bicicleta\n");
            printf("> Ventilar espacios cerrados en horas de menor contaminacion\n");
            printf("> Grupos vulnerables: ninos, ancianos y personas con asma deben tomar precauciones\n");
            break;
            
        case ALERTA_NARANJA:
            printf("> Evitar actividades fisicas intensas al aire libre\n");
            printf("> Usar mascarilla si es necesario salir por periodos prolongados\n");
            printf("> Mantener ventanas cerradas durante el dia\n");
            printf("> Personas con problemas respiratorios deben evitar exposicion\n");
            printf("> Postponer actividades deportivas al exterior\n");
            printf("> Usar purificadores de aire en espacios cerrados\n");
            break;
            
        case ALERTA_ROJA:
            printf("> EVITAR SALIR SI ES POSIBLE - PERMANECER EN INTERIORES\n");
            printf("> Usar mascarilla N95 obligatoriamente al salir\n");
            printf("> Mantener espacios cerrados y filtrar el aire\n");
            printf("> Personas vulnerables deben quedarse en casa\n");
            printf("> Contactar servicios medicos si hay sintomas respiratorios\n");
            printf("> Suspender todas las actividades al aire libre\n");
            printf("> Considerar evacuar a zonas con mejor calidad del aire\n");
            break;
    }
    
    // Recomendaciones adicionales generales
    printf("\nRECOMENDACIONES ADICIONALES:\n");
    switch(nivel_alerta) {
        case ALERTA_VERDE:
            printf("- Aprovechar para actividades al aire libre\n");
            printf("- Mantener rutinas normales de ejercicio\n");
            break;
        case ALERTA_AMARILLA:
            printf("- Revisar pronostico antes de planificar actividades\n");
            printf("- Tener medicamentos para asma a la mano\n");
            break;
        case ALERTA_NARANJA:
            printf("- Informar a grupos vulnerables sobre riesgos\n");
            printf("- Evitar areas de trafico pesado\n");
            break;
        case ALERTA_ROJA:
            printf("- Contactar autoridades de salud publica\n");
            printf("- Activar protocolos de emergencia ambiental\n");
            break;
    }
}

// ================= FUNCION DE CORRECCION DE DATOS INGRESADOS MEJORADA =================
void corregirDatosIngresados(ZonaUrbana zonas[]) {
    int id_zona, val, dia;
    char continuar_editando = 's';
    int cambios_realizados = 0;

    // Mostrar zonas disponibles
    cargarTodasLasZonas(zonas);
    printf("\n=== EDITOR AVANZADO DE DATOS HISTORICOS ===\n");
    printf("ZONAS DISPONIBLES PARA EDICION:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s (%d dias de datos)\n", zonas[i].id_zona, zonas[i].nombre, zonas[i].dias_registrados);
    }

    // Seleccionar zona
    do {
        printf("\nSeleccione la zona a editar (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &id_zona);
        fflush(stdin);
        if(val != 1 || id_zona < 1 || id_zona > MAX_ZONAS) {
            printf("ERROR: Opcion invalida. Intente de nuevo.\n");
        }
    } while(val != 1 || id_zona < 1 || id_zona > MAX_ZONAS);

    ZonaUrbana *zona = &zonas[id_zona-1];
    if(zona->dias_registrados == 0) {
        printf("ERROR: No hay datos registrados para esta zona.\n");
        return;
    }

    printf("\n=== EDITANDO ZONA: %s ===\n", zona->nombre);
    
    // BUCLE PRINCIPAL: SELECCIÓN DE DÍA Y EDICIÓN
    do {
        // Mostrar días disponibles con vista resumida
        printf("\nDIAS DISPONIBLES PARA EDICION:\n");
        printf("=================================================================\n");
        printf("Dia\tCO2\tSO2\tNO2\tPM2.5\n");
        printf("-----------------------------------------------------------------\n");
        for(int i = 0; i < zona->dias_registrados && i < 10; i++) {
            printf("%d\t%.1f\t%.1f\t%.1f\t%.1f\n", i+1,
                   zona->historico[i].co2, zona->historico[i].so2, 
                   zona->historico[i].no2, zona->historico[i].pm25);
        }
        printf("=================================================================\n");
        
        if(zona->dias_registrados > 10) {
            printf("(Mostrando primeros 10 dias de %d disponibles)\n", zona->dias_registrados);
        }

        // Seleccionar día
        do {
            printf("Seleccione el dia a editar (1-%d): ", zona->dias_registrados);
            val = scanf("%d", &dia);
            fflush(stdin);
            if(val != 1 || dia < 1 || dia > zona->dias_registrados) {
                printf("ERROR: Dia invalido. Intente de nuevo.\n");
            }
        } while(val != 1 || dia < 1 || dia > zona->dias_registrados);
        dia--; // convertir a índice

        // BUCLE DE EDICIÓN MÚLTIPLE PARA EL DÍA SELECCIONADO
        int cambios_dia = 0;
        do {
            int subop;
            float nuevo_valor;
            char confirmacion;
            
            // Mostrar panel de edición
            printf("\nEDITOR - DIA %d de %s\n", dia + 1, zona->nombre);
            printf("=======================================================\n");
            printf("CONTAMINANTES ACTUALES:\n");
            printf("  1. CO2:\t%6.1f ppm\n", zona->historico[dia].co2);
            printf("  2. SO2:\t%6.1f ug/m3\n", zona->historico[dia].so2);
            printf("  3. NO2:\t%6.1f ug/m3\n", zona->historico[dia].no2);
            printf("  4. PM2.5:\t%6.1f ug/m3\n", zona->historico[dia].pm25);
            printf("\nDATOS CLIMATICOS ACTUALES:\n");
            printf("  5. Temperatura:\t%6.1f C\n", zona->clima_actual.temperatura);
            printf("  6. Viento:\t\t%6.1f km/h\n", zona->clima_actual.velocidad_viento);
            printf("  7. Humedad:\t\t%6.1f %%\n", zona->clima_actual.humedad);
            printf("  8. Presion:\t\t%6.1f hPa\n", zona->clima_actual.presion_atmosferica);
            printf("=======================================================\n");
            printf("  9. Edicion rapida (todos los contaminantes)\n");
            printf("  0. Terminar edicion de este dia\n");
            
            do {
                printf("\nSeleccione el campo a editar (0-9): ");
                val = scanf("%d", &subop);
                fflush(stdin);
                if(val != 1 || subop < 0 || subop > 9) {
                    printf("ERROR: Opcion invalida. Intente de nuevo.\n");
                }
            } while(val != 1 || subop < 0 || subop > 9);
            
            if(subop == 0) {
                break; // Terminar edición de este día
            }
            
            // Edición rápida de todos los contaminantes
            if(subop == 9) {
                printf("\nEDICION RAPIDA - TODOS LOS CONTAMINANTES\n");
                printf("Valores actuales: CO2=%.1f, SO2=%.1f, NO2=%.1f, PM2.5=%.1f\n",
                       zona->historico[dia].co2, zona->historico[dia].so2, 
                       zona->historico[dia].no2, zona->historico[dia].pm25);
                       
                float nuevos_valores[4];
                char *nombres_cont[] = {"CO2 (ppm)", "SO2 (ug/m3)", "NO2 (ug/m3)", "PM2.5 (ug/m3)"};
                float rangos_min[] = {0.0, 0.0, 0.0, 0.0};
                float rangos_max[] = {1000.0, 500.0, 300.0, 200.0};
                
                printf("Ingrese los nuevos valores:\n");
                for(int i = 0; i < 4; i++) {
                    funcionValidarDatosdeRegistro(&nuevos_valores[i], nombres_cont[i], 
                                                 rangos_min[i], rangos_max[i]);
                }
                
                // Confirmar cambios masivos
                printf("\nRESUMEN DE CAMBIOS:\n");
                printf("CO2:\t%.1f\t-->\t%.1f ppm\n", zona->historico[dia].co2, nuevos_valores[0]);
                printf("SO2:\t%.1f\t-->\t%.1f ug/m3\n", zona->historico[dia].so2, nuevos_valores[1]);
                printf("NO2:\t%.1f\t-->\t%.1f ug/m3\n", zona->historico[dia].no2, nuevos_valores[2]);
                printf("PM2.5:\t%.1f\t-->\t%.1f ug/m3\n", zona->historico[dia].pm25, nuevos_valores[3]);
                
                do {
                    printf("¿Confirma TODOS estos cambios? (s/n): ");
                    scanf(" %c", &confirmacion);
                    fflush(stdin);
                } while(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N');
                
                if(confirmacion == 's' || confirmacion == 'S') {
                    zona->historico[dia].co2 = nuevos_valores[0];
                    zona->historico[dia].so2 = nuevos_valores[1];
                    zona->historico[dia].no2 = nuevos_valores[2];
                    zona->historico[dia].pm25 = nuevos_valores[3];
                    
                    // Actualizar niveles actuales si es el día más reciente
                    if(dia == 0) {
                        zona->niveles_actuales = zona->historico[0];
                    }
                    
                    printf("EXITO: Todos los contaminantes actualizados exitosamente.\n");
                    cambios_dia += 4;
                } else {
                    printf("ERROR: Cambios cancelados.\n");
                }
                continue;
            }
            
            // Edición individual
            char *nombres[] = {"", "CO2 (ppm)", "SO2 (ug/m3)", "NO2 (ug/m3)", "PM2.5 (ug/m3)",
                              "Temperatura (C)", "Viento (km/h)", "Humedad (%)", "Presion (hPa)"};
            float rangos_min[] = {0, 0.0, 0.0, 0.0, 0.0, -20.0, 0.0, 0.0, 900.0};
            float rangos_max[] = {0, 1000.0, 500.0, 300.0, 200.0, 50.0, 120.0, 100.0, 1100.0};
            
            printf("\nEDITANDO: %s\n", nombres[subop]);
            printf("Valor actual: ");
            switch(subop) {
                case 1: printf("%.1f ppm\n", zona->historico[dia].co2); break;
                case 2: printf("%.1f ug/m3\n", zona->historico[dia].so2); break;
                case 3: printf("%.1f ug/m3\n", zona->historico[dia].no2); break;
                case 4: printf("%.1f ug/m3\n", zona->historico[dia].pm25); break;
                case 5: printf("%.1f C\n", zona->clima_actual.temperatura); break;
                case 6: printf("%.1f km/h\n", zona->clima_actual.velocidad_viento); break;
                case 7: printf("%.1f%%\n", zona->clima_actual.humedad); break;
                case 8: printf("%.1f hPa\n", zona->clima_actual.presion_atmosferica); break;
            }
            
            funcionValidarDatosdeRegistro(&nuevo_valor, nombres[subop], 
                                         rangos_min[subop], rangos_max[subop]);

            // Confirmar cambio individual
            do {
                printf("¿Confirma el cambio de %s? (s/n): ", nombres[subop]);
                scanf(" %c", &confirmacion);
                fflush(stdin);
            } while(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N');

            if(confirmacion == 's' || confirmacion == 'S') {
                switch(subop) {
                    case 1: zona->historico[dia].co2 = nuevo_valor; break;
                    case 2: zona->historico[dia].so2 = nuevo_valor; break;
                    case 3: zona->historico[dia].no2 = nuevo_valor; break;
                    case 4: zona->historico[dia].pm25 = nuevo_valor; break;
                    case 5: zona->clima_actual.temperatura = nuevo_valor; break;
                    case 6: zona->clima_actual.velocidad_viento = nuevo_valor; break;
                    case 7: zona->clima_actual.humedad = nuevo_valor; break;
                    case 8: zona->clima_actual.presion_atmosferica = nuevo_valor; break;
                }
                
                // Si editamos el día más reciente (día 1 = índice 0), actualizar niveles actuales
                if(dia == 0 && subop <= 4) {
                    zona->niveles_actuales = zona->historico[0];
                    printf("INFO: Niveles actuales actualizados (día más reciente modificado).\n");
                }
                
                printf("EXITO: Cambio aplicado exitosamente.\n");
                cambios_dia++;
                
                // Guardar inmediatamente para evitar pérdida de datos
                guardarZona(zona);
            } else {
                printf("ERROR: Cambio cancelado.\n");
            }
                
        } while(1); // El bucle se rompe con subop == 0

        // Resumen de cambios del día
        if(cambios_dia > 0) {
            printf("\nRESUMEN: %d cambio(s) realizado(s) en el dia %d\n", cambios_dia, dia + 1);
            cambios_realizados += cambios_dia;
        }
        
        // Preguntar si quiere editar otro día
        do {
            printf("\n¿Desea editar otro dia de esta zona? (s/n): ");
            scanf(" %c", &continuar_editando);
            fflush(stdin);
        } while(continuar_editando != 's' && continuar_editando != 'S' && 
                continuar_editando != 'n' && continuar_editando != 'N');
                
    } while(continuar_editando == 's' || continuar_editando == 'S');

    // Guardar cambios si se realizaron
    if(cambios_realizados > 0) {
        guardarZona(zona);
        printf("\nDATOS GUARDADOS: %d cambio(s) total(es) en %s\n", 
               cambios_realizados, zona->nombre);
    }
    
    // Preguntar si quiere editar otra zona
    do {
        printf("\n¿Desea editar datos de otra zona? (s/n): ");
        scanf(" %c", &continuar_editando);
        fflush(stdin);
    } while(continuar_editando != 's' && continuar_editando != 'S' && 
            continuar_editando != 'n' && continuar_editando != 'N');
    
    if(continuar_editando == 's' || continuar_editando == 'S') {
        corregirDatosIngresados(zonas);
    }
    
    printf("\nEditor finalizado. Total de cambios realizados: %d\n", cambios_realizados);
}



// ===== FUNCIONES PARA MANEJO DE FECHAS =====

void mostrarFecha(Fecha fecha) {
    printf("%02d/%02d/%04d", fecha.dia, fecha.mes, fecha.año);
}

int compararFechas(Fecha f1, Fecha f2) {
    if (f1.año != f2.año) return f1.año - f2.año;
    if (f1.mes != f2.mes) return f1.mes - f2.mes;
    return f1.dia - f2.dia;
}

void mostrarHistorialConFechas(ZonaUrbana zonas[]) {
    printf("\n");
    printf("=======================================================\n");
    printf("           HISTORIAL DE DATOS CON FECHAS              \n");
    printf("=======================================================\n\n");
    
    int zona_seleccionada, val;
    
    // Mostrar zonas disponibles
    printf("ZONAS DISPONIBLES:\n");
    printf("-------------------------------------------------------\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("  %d. %-25s", zonas[i].id_zona, zonas[i].nombre);
        if(zonas[i].dias_registrados > 0) {
            printf("(%d dias registrados)\n", zonas[i].dias_registrados);
        } else {
            printf("(Sin datos)\n");
        }
    }
    printf("-------------------------------------------------------\n");
    
    // Seleccionar zona
    do {
        printf("\nSeleccione la zona para ver historial (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &zona_seleccionada);
        fflush(stdin);
        
        if(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS) {
            printf("ERROR: Opcion invalida. Por favor, intente de nuevo.\n");
        }
    } while(val != 1 || zona_seleccionada < 1 || zona_seleccionada > MAX_ZONAS);
    
    zona_seleccionada--;
    
    if(zonas[zona_seleccionada].dias_registrados == 0) {
        printf("\nERROR: La zona '%s' no tiene datos registrados.\n", 
               zonas[zona_seleccionada].nombre);
        printf("       Registre datos primero usando la opcion 1 del menu.\n");
        return;
    }
    
    printf("\n");
    printf("=======================================================\n");
    printf("  HISTORIAL DETALLADO: %s\n", zonas[zona_seleccionada].nombre);
    printf("=======================================================\n");
    printf("Total de registros: %d dias\n", zonas[zona_seleccionada].dias_registrados);
    printf("=======================================================\n\n");
    
    // Encabezados de tabla mejorados
    printf("+-----------+--------+--------+--------+--------+---------------+\n");
    printf("| Fecha     |  CO2   |  SO2   |  NO2   | PM2.5  | Estado General|\n");
    printf("| (dd/mm/aa)| (ppm)  |(ug/m3) |(ug/m3) |(ug/m3) |               |\n");
    printf("+-----------+--------+--------+--------+--------+---------------+\n");
    
    // Mostrar todos los días registrados
    for(int i = 0; i < zonas[zona_seleccionada].dias_registrados; i++) {
        // Contar excesos para determinar estado
        int excesos = 0;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) excesos++;
        
        char estado[16];
        if(excesos == 0) {
            strcpy(estado, "Bueno");
        } else if(excesos == 1) {
            strcpy(estado, "Moderado");
        } else if(excesos == 2) {
            strcpy(estado, "Danino");
        } else {
            strcpy(estado, "Peligroso");
        }
        
        // Mostrar fila de datos con formato alineado
        printf("| %02d/%02d/%02d | %6.1f | %6.1f | %6.1f | %6.1f | %-13s |\n",
               zonas[zona_seleccionada].historico_fechas[i].fecha.dia,
               zonas[zona_seleccionada].historico_fechas[i].fecha.mes,
               zonas[zona_seleccionada].historico_fechas[i].fecha.año % 100,
               zonas[zona_seleccionada].historico_fechas[i].niveles.co2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.so2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.no2,
               zonas[zona_seleccionada].historico_fechas[i].niveles.pm25,
               estado);
        
        // Mostrar contaminantes que exceden límites en línea separada
        if(excesos > 0) {
            printf("|           |        |        |        |        | Exceden: ");
            int primero = 1;
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) {
                if(!primero) printf(", ");
                printf("CO2");
                primero = 0;
            }
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) {
                if(!primero) printf(", ");
                printf("SO2");
                primero = 0;
            }
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) {
                if(!primero) printf(", ");
                printf("NO2");
                primero = 0;
            }
            if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) {
                if(!primero) printf(", ");
                printf("PM2.5");
            }
            
            // Completar la línea con espacios
            printf("%*s|\n", (int)(6 - strlen("Exceden: ")), "");
        }
        
        // Separador entre filas cada 5 registros para mejor legibilidad
        if((i + 1) % 5 == 0 && i < zonas[zona_seleccionada].dias_registrados - 1) {
            printf("+-----------+--------+--------+--------+--------+---------------+\n");
        }
    }
    
    printf("+-----------+--------+--------+--------+--------+---------------+\n");
    
    // Resumen estadístico al final
    printf("\nRESUMEN ESTADISTICO:\n");
    printf("-------------------------------------------------------\n");
    
    // Calcular días con problemas
    int dias_buenos = 0, dias_moderados = 0, dias_daninos = 0, dias_peligrosos = 0;
    
    for(int i = 0; i < zonas[zona_seleccionada].dias_registrados; i++) {
        int excesos = 0;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) excesos++;
        if(zonas[zona_seleccionada].historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) excesos++;
        
        if(excesos == 0) dias_buenos++;
        else if(excesos == 1) dias_moderados++;
        else if(excesos == 2) dias_daninos++;
        else dias_peligrosos++;
    }
    
    printf("  Dias buenos:     %2d (%.1f%%)\n", dias_buenos, 
           (float)dias_buenos / zonas[zona_seleccionada].dias_registrados * 100);
    printf("  Dias moderados:  %2d (%.1f%%)\n", dias_moderados,
           (float)dias_moderados / zonas[zona_seleccionada].dias_registrados * 100);
    printf("  Dias daninos:    %2d (%.1f%%)\n", dias_daninos,
           (float)dias_daninos / zonas[zona_seleccionada].dias_registrados * 100);
    printf("  Dias peligrosos: %2d (%.1f%%)\n", dias_peligrosos,
           (float)dias_peligrosos / zonas[zona_seleccionada].dias_registrados * 100);
    
    printf("-------------------------------------------------------\n");
    printf("LIMITES OMS DE REFERENCIA:\n");
    printf("  CO2: %.1f ppm | SO2: %.1f ug/m3 | NO2: %.1f ug/m3 | PM2.5: %.1f ug/m3\n",
           LIMITE_CO2_OMS, LIMITE_SO2_OMS, LIMITE_NO2_OMS, LIMITE_PM25_OMS);
    printf("=======================================================\n");
    
    printf("\nPresione Enter para continuar...");
    getchar();
}

// ===== FUNCIONES PARA EXPORTACIÓN DE REPORTES =====

void exportarReportePorZona(ZonaUrbana zonas[], int zona_id) {
    if (zona_id < 1 || zona_id > MAX_ZONAS) {
        printf("ID de zona invalido.\n");
        return;
    }
    
    ZonaUrbana *zona = &zonas[zona_id - 1];
    
    char nombre_archivo[200];
    sprintf(nombre_archivo, "reporte_zona_%d_%s.txt", zona_id, zona->nombre);
    
    FILE *archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo de reporte.\n");
        return;
    }
    
    /* Obtener fecha actual para el reporte */
    time_t tiempo_actual = time(NULL);
    struct tm *tiempo_local = localtime(&tiempo_actual);
    
    /* Arte ASCII mejorado y encabezado visual - VERSION LIMPIA SIN SOMBRAS */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║      █████  ██ ██████       ██████  ██   ██  █████  ██      ██ ████████ ██   ██  ║\n");
    fprintf(archivo, "║     ██   ██ ██ ██   ██     ██    ██ ██   ██ ██   ██ ██      ██    ██     ██ ██   ║\n");
    fprintf(archivo, "║     ███████ ██ ██████      ██    ██ ██   ██ ███████ ██      ██    ██      ███    ║\n");
    fprintf(archivo, "║     ██   ██ ██ ██   ██     ██ ██ ██ ██   ██ ██   ██ ██      ██    ██      ██     ║\n");
    fprintf(archivo, "║     ██   ██ ██ ██   ██      ██████   █████  ██   ██ ███████ ██    ██      ██     ║\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║            ███    ███  ██████  ███    ██ ██ ████████  ██████  ██████            ║\n");
    fprintf(archivo, "║            ████  ████ ██    ██ ████   ██ ██    ██    ██    ██ ██   ██           ║\n");
    fprintf(archivo, "║            ██ ████ ██ ██    ██ ██ ██  ██ ██    ██    ██    ██ ██████            ║\n");
    fprintf(archivo, "║            ██  ██  ██ ██    ██ ██  ██ ██ ██    ██    ██    ██ ██   ██           ║\n");
    fprintf(archivo, "║            ██      ██  ██████  ██   ████ ██    ██     ██████  ██   ██           ║\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║                   ╔═══════════════════════════════════════════════╗              ║\n");
    fprintf(archivo, "║                   ║          MONITOREO AMBIENTAL QUITO           ║              ║\n");
    fprintf(archivo, "║                   ║           SISTEMA DE CALIDAD DEL AIRE         ║              ║\n");
    fprintf(archivo, "║                   ║               ECUADOR - 2025                  ║              ║\n");
    fprintf(archivo, "║                   ║            REPORTE PROFESIONAL               ║              ║\n");
    fprintf(archivo, "║                   ╚═══════════════════════════════════════════════╝              ║\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n");
    fprintf(archivo, "                                                                                    \n");
    fprintf(archivo, "    ╔═══════════════════════════════════════════════════════════════════════════╗  \n");
    fprintf(archivo, "    ║    SISTEMA PROFESIONAL DE MONITOREO ATMOSFERICO - ESTILO AIRNOW EPA    ║  \n");
    fprintf(archivo, "    ║                          Compatible con Estandares OMS                   ║  \n");
    fprintf(archivo, "    ╚═══════════════════════════════════════════════════════════════════════════╝  \n");
    fprintf(archivo, "                                                                                    \n");
    
    /* Calcular Indice de Calidad del Aire (AQI simplificado) */
    int excesos_actuales = 0;
    if(zona->niveles_actuales.co2 > LIMITE_CO2_OMS) excesos_actuales++;
    if(zona->niveles_actuales.so2 > LIMITE_SO2_OMS) excesos_actuales++;
    if(zona->niveles_actuales.no2 > LIMITE_NO2_OMS) excesos_actuales++;
    if(zona->niveles_actuales.pm25 > LIMITE_PM25_OMS) excesos_actuales++;
    
    char categoria_aqi[30];
    char color_aqi[20];
    int valor_aqi;
    
    if(excesos_actuales == 0) {
        strcpy(categoria_aqi, "BUENO");
        strcpy(color_aqi, "VERDE");
        valor_aqi = 25;
    } else if(excesos_actuales == 1) {
        strcpy(categoria_aqi, "MODERADO");
        strcpy(color_aqi, "AMARILLO");
        valor_aqi = 75;
    } else if(excesos_actuales == 2) {
        strcpy(categoria_aqi, "DANINO PARA SENSIBLES");
        strcpy(color_aqi, "NARANJA");
        valor_aqi = 125;
    } else if(excesos_actuales == 3) {
        strcpy(categoria_aqi, "DANINO");
        strcpy(color_aqi, "ROJO");
        valor_aqi = 175;
    } else {
        strcpy(categoria_aqi, "MUY DANINO");
        strcpy(color_aqi, "MORADO");
        valor_aqi = 225;
    }
    
    // ENCABEZADO DEL REPORTE
    fprintf(archivo, "┌─────────────────────────────────────────────────────────────────────────────┐\n");
    fprintf(archivo, "│                           REPORTE DE CALIDAD DEL AIRE                      │\n");
    fprintf(archivo, "├─────────────────────────────────────────────────────────────────────────────┤\n");
    fprintf(archivo, "│  Zona: %-30s                                      │\n", zona->nombre);
    fprintf(archivo, "│  ID de Zona: %-3d                                                         │\n", zona->id_zona);
    fprintf(archivo, "│  Fecha: %02d/%02d/%04d a las %02d:%02d                                          │\n", 
            tiempo_local->tm_mday, tiempo_local->tm_mon + 1, tiempo_local->tm_year + 1900,
            tiempo_local->tm_hour, tiempo_local->tm_min);
    fprintf(archivo, "│  Dias monitoreados: %-3d                                                   │\n", zona->dias_registrados);
    fprintf(archivo, "└─────────────────────────────────────────────────────────────────────────────┘\n\n");
    
    // ÍNDICE DE CALIDAD DEL AIRE PRINCIPAL
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                            INDICE DE CALIDAD DEL AIRE                           ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║                                 AQI: %3d                                       ║\n", valor_aqi);
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║                               %s                                               ║\n", categoria_aqi);
    fprintf(archivo, "║                                Nivel: %s                                       ║\n", color_aqi);
    fprintf(archivo, "║                                                                                  ║\n");
    
    if(excesos_actuales == 0) {
        fprintf(archivo, "║    ┌─────────────────────────────────────────────────────────────────────────┐    ║\n");
        fprintf(archivo, "║    │ * AIRE LIMPIO Y SALUDABLE - CONDICIONES OPTIMAS PARA TODOS            │    ║\n");
        fprintf(archivo, "║    └─────────────────────────────────────────────────────────────────────────┘    ║\n");
    } else if(excesos_actuales == 1) {
        fprintf(archivo, "║    ┌─────────────────────────────────────────────────────────────────────────┐    ║\n");
        fprintf(archivo, "║    │ ! CALIDAD ACEPTABLE - PRECAUCION PARA GRUPOS SENSIBLES                │    ║\n");
        fprintf(archivo, "║    └─────────────────────────────────────────────────────────────────────────┘    ║\n");
    } else if(excesos_actuales <= 2) {
        fprintf(archivo, "║    ┌─────────────────────────────────────────────────────────────────────────┐    ║\n");
        fprintf(archivo, "║    │ !! DANINO PARA GRUPOS SENSIBLES - LIMITE ACTIVIDADES EXTERIORES        │    ║\n");
        fprintf(archivo, "║    └─────────────────────────────────────────────────────────────────────────┘    ║\n");
    } else if(excesos_actuales == 3) {
        fprintf(archivo, "║    ┌─────────────────────────────────────────────────────────────────────────┐    ║\n");
        fprintf(archivo, "║    │ !!! AIRE DANINO - TODOS DEBEN REDUCIR EXPOSICION EXTERIOR                │    ║\n");
        fprintf(archivo, "║    └─────────────────────────────────────────────────────────────────────────┘    ║\n");
    } else {
        fprintf(archivo, "║    ┌─────────────────────────────────────────────────────────────────────────┐    ║\n");
        fprintf(archivo, "║    │ !!!! PELIGROSO - EVITAR COMPLETAMENTE ACTIVIDADES AL AIRE LIBRE            │    ║\n");
        fprintf(archivo, "║    └─────────────────────────────────────────────────────────────────────────┘    ║\n");
    }
    
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    /* NIVELES ACTUALES */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                       NIVELES ACTUALES DE CONTAMINANTES                         ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    
    fprintf(archivo, "║  CO2:    %6.1f ppm      │ Limite OMS: %6.1f │ ", zona->niveles_actuales.co2, LIMITE_CO2_OMS);
    if(zona->niveles_actuales.co2 <= LIMITE_CO2_OMS) {
        fprintf(archivo, "* NORMAL         ║\n");
    } else {
        fprintf(archivo, "! EXCEDIDO %.1f%%   ║\n", 
                (zona->niveles_actuales.co2 / LIMITE_CO2_OMS - 1) * 100);
    }
    
    fprintf(archivo, "║  SO2:    %6.1f ug/m3   │ Limite OMS: %6.1f │ ", zona->niveles_actuales.so2, LIMITE_SO2_OMS);
    if(zona->niveles_actuales.so2 <= LIMITE_SO2_OMS) {
        fprintf(archivo, "* NORMAL         ║\n");
    } else {
        fprintf(archivo, "! EXCEDIDO %.1f%%   ║\n", 
                (zona->niveles_actuales.so2 / LIMITE_SO2_OMS - 1) * 100);
    }
    
    fprintf(archivo, "║  NO2:    %6.1f ug/m3   │ Limite OMS: %6.1f │ ", zona->niveles_actuales.no2, LIMITE_NO2_OMS);
    if(zona->niveles_actuales.no2 <= LIMITE_NO2_OMS) {
        fprintf(archivo, "* NORMAL         ║\n");
    } else {
        fprintf(archivo, "! EXCEDIDO %.1f%%   ║\n", 
                (zona->niveles_actuales.no2 / LIMITE_NO2_OMS - 1) * 100);
    }
    
    fprintf(archivo, "║  PM2.5:  %6.1f ug/m3   │ Limite OMS: %6.1f │ ", zona->niveles_actuales.pm25, LIMITE_PM25_OMS);
    if(zona->niveles_actuales.pm25 <= LIMITE_PM25_OMS) {
        fprintf(archivo, "* NORMAL         ║\n");
    } else {
        fprintf(archivo, "! EXCEDIDO %.1f%%   ║\n", 
                (zona->niveles_actuales.pm25 / LIMITE_PM25_OMS - 1) * 100);
    }
    
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n");
    fprintf(archivo, "\n");
    
    /* CONDICIONES CLIMATICAS */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                        CONDICIONES CLIMATICAS ACTUALES                          ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║  Temperatura:          %6.1f°C     │   Humedad:             %6.1f%%     ║\n", 
            zona->clima_actual.temperatura, zona->clima_actual.humedad);
    fprintf(archivo, "║                                     │                                       ║\n");
    fprintf(archivo, "║  Velocidad del viento: %6.1f km/h  │   Presion atmosferica: %6.1f hPa  ║\n", 
            zona->clima_actual.velocidad_viento, zona->clima_actual.presion_atmosferica);
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    /* RECOMENDACIONES DE SALUD */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                           RECOMENDACIONES DE SALUD                              ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    
    if(excesos_actuales == 0) {
        fprintf(archivo, "║  POBLACION GENERAL:                                                            ║\n");
        fprintf(archivo, "║    * Excelente dia para actividades al aire libre                              ║\n");
        fprintf(archivo, "║    * Ideal para ejercicio y deportes                                            ║\n");
        fprintf(archivo, "║    * Perfecto para que los ninos jueguen afuera                                ║\n");
        fprintf(archivo, "║                                                                                  ║\n");
        fprintf(archivo, "║  GRUPOS SENSIBLES:                                                             ║\n");
        fprintf(archivo, "║    * Sin restricciones                                                          ║\n");
        fprintf(archivo, "║    * Condiciones optimas para personas con asma                                ║\n");
        fprintf(archivo, "║    * Seguro para adultos mayores y ninos pequenos                              ║\n");
    } else if(excesos_actuales <= 1) {
        fprintf(archivo, "║  POBLACION GENERAL:                                                            ║\n");
        fprintf(archivo, "║    * Actividades al aire libre son aceptables                                  ║\n");
        fprintf(archivo, "║    ! Considere reducir ejercicio intenso prolongado                            ║\n");
        fprintf(archivo, "║    * Condiciones normales para la mayoria de personas                          ║\n");
        fprintf(archivo, "║                                                                                  ║\n");
        fprintf(archivo, "║  GRUPOS SENSIBLES:                                                             ║\n");
        fprintf(archivo, "║    ! Personas con asma: limite actividades prolongadas                         ║\n");
        fprintf(archivo, "║    ! Adultos mayores: prefiera actividades en interiores                       ║\n");
        fprintf(archivo, "║    ! Ninos: reduzca tiempo de juego al aire libre                              ║\n");
    } else if(excesos_actuales <= 2) {
        fprintf(archivo, "║  POBLACION GENERAL:                                                            ║\n");
        fprintf(archivo, "║    ! Reduzca actividades al aire libre prolongadas                             ║\n");
        fprintf(archivo, "║    ! Evite ejercicio intenso al aire libre                                     ║\n");
        fprintf(archivo, "║    ! Use mascarilla si es sensible a la contaminacion                          ║\n");
        fprintf(archivo, "║                                                                                  ║\n");
        fprintf(archivo, "║  GRUPOS SENSIBLES:                                                             ║\n");
        fprintf(archivo, "║    ! Evite actividades al aire libre                                           ║\n");
        fprintf(archivo, "║    ! Permanezca en interiores cuando sea posible                               ║\n");
        fprintf(archivo, "║    ! Tenga medicamentos de emergencia a mano                                   ║\n");
    } else {
        fprintf(archivo, "║  POBLACION GENERAL:                                                            ║\n");
        fprintf(archivo, "║    ! Evite todas las actividades al aire libre                                 ║\n");
        fprintf(archivo, "║    ! Permanezca en interiores                                                   ║\n");
        fprintf(archivo, "║    ! Use mascarilla si debe salir                                               ║\n");
        fprintf(archivo, "║    ! Cancele eventos deportivos al aire libre                                   ║\n");
        fprintf(archivo, "║                                                                                  ║\n");
        fprintf(archivo, "║  GRUPOS SENSIBLES:                                                             ║\n");
        fprintf(archivo, "║    !! ALERTA MEDICA: Permanezca en interiores                                   ║\n");
        fprintf(archivo, "║    !! Tenga medicamentos listos                                                  ║\n");
        fprintf(archivo, "║    !! Contacte a su medico si presenta sintomas                                  ║\n");
        fprintf(archivo, "║    !! Busque atencion medica si tiene dificultad respiratoria                   ║\n");
    }
    
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    /* PRONOSTICO DE 24 HORAS */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                          PRONOSTICO PARA LAS PROXIMAS 24 HORAS                 ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    
    /* Calcular pronostico simple basado en tendencia historica */
    if(zona->dias_registrados >= 3) {
        float tendencia_co2 = (zona->historico_fechas[0].niveles.co2 - zona->historico_fechas[2].niveles.co2) / 2.0;
        float tendencia_so2 = (zona->historico_fechas[0].niveles.so2 - zona->historico_fechas[2].niveles.so2) / 2.0;
        float tendencia_no2 = (zona->historico_fechas[0].niveles.no2 - zona->historico_fechas[2].niveles.no2) / 2.0;
        float tendencia_pm25 = (zona->historico_fechas[0].niveles.pm25 - zona->historico_fechas[2].niveles.pm25) / 2.0;
        
        float pronostico_co2 = zona->niveles_actuales.co2 + tendencia_co2;
        float pronostico_so2 = zona->niveles_actuales.so2 + tendencia_so2;
        float pronostico_no2 = zona->niveles_actuales.no2 + tendencia_no2;
        float pronostico_pm25 = zona->niveles_actuales.pm25 + tendencia_pm25;
        
        fprintf(archivo, "║  NIVELES ESPERADOS:                                                            ║\n");
        fprintf(archivo, "║                                                                                  ║\n");
        
        fprintf(archivo, "║  CO2:   %6.1f ppm     │ Tendencia: ", pronostico_co2);
        if(tendencia_co2 > 0) {
            fprintf(archivo, "* SUBIENDO +%5.1f            ║\n", tendencia_co2);
        } else {
            fprintf(archivo, "* BAJANDO %6.1f            ║\n", tendencia_co2);
        }
        
        fprintf(archivo, "║  SO2:   %6.1f ug/m3  │ Tendencia: ", pronostico_so2);
        if(tendencia_so2 > 0) {
            fprintf(archivo, "* SUBIENDO +%5.1f            ║\n", tendencia_so2);
        } else {
            fprintf(archivo, "* BAJANDO %6.1f            ║\n", tendencia_so2);
        }
        
        fprintf(archivo, "║  NO2:   %6.1f ug/m3  │ Tendencia: ", pronostico_no2);
        if(tendencia_no2 > 0) {
            fprintf(archivo, "* SUBIENDO +%5.1f            ║\n", tendencia_no2);
        } else {
            fprintf(archivo, "* BAJANDO %6.1f            ║\n", tendencia_no2);
        }
        
        fprintf(archivo, "║  PM2.5: %6.1f ug/m3  │ Tendencia: ", pronostico_pm25);
        if(tendencia_pm25 > 0) {
            fprintf(archivo, "* SUBIENDO +%5.1f            ║\n", tendencia_pm25);
        } else {
            fprintf(archivo, "* BAJANDO %6.1f            ║\n", tendencia_pm25);
        }
        
        /* Calcular excesos proyectados */
        int excesos_pronostico = 0;
        if(pronostico_co2 > LIMITE_CO2_OMS) excesos_pronostico++;
        if(pronostico_so2 > LIMITE_SO2_OMS) excesos_pronostico++;
        if(pronostico_no2 > LIMITE_NO2_OMS) excesos_pronostico++;
        if(pronostico_pm25 > LIMITE_PM25_OMS) excesos_pronostico++;
        
        fprintf(archivo, "║                                                                                  ║\n");
        fprintf(archivo, "║  EXPECTATIVA DE CALIDAD:                                                       ║\n");
        if(excesos_pronostico < excesos_actuales) {
            fprintf(archivo, "║    * MEJORANDO - Se espera mejoria en las condiciones                           ║\n");
        } else if(excesos_pronostico > excesos_actuales) {
            fprintf(archivo, "║    ! EMPEORANDO - Se espera deterioro en las condiciones                       ║\n");
        } else {
            fprintf(archivo, "║    = ESTABLE - Condiciones similares esperadas                                 ║\n");
        }
    } else {
        fprintf(archivo, "║ ! Datos insuficientes para generar pronostico confiable                        ║\n");
        fprintf(archivo, "║    (Se requieren al menos 3 dias de datos historicos)                           ║\n");
    }
    
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n\n");
    
    /* RESUMEN ESTADISTICO */
    if (zona->dias_registrados > 0) {
        fprintf(archivo, "RESUMEN ESTADISTICO DEL PERIODO:\n");
        fprintf(archivo, "===============================================================================\n");
        
        /* Calcular valores maximos y dias con excesos */
        float max_co2 = zona->historico_fechas[0].niveles.co2;
        float max_so2 = zona->historico_fechas[0].niveles.so2;
        float max_no2 = zona->historico_fechas[0].niveles.no2;
        float max_pm25 = zona->historico_fechas[0].niveles.pm25;
        
        float min_co2 = zona->historico_fechas[0].niveles.co2;
        float min_so2 = zona->historico_fechas[0].niveles.so2;
        float min_no2 = zona->historico_fechas[0].niveles.no2;
        float min_pm25 = zona->historico_fechas[0].niveles.pm25;
        
        int dias_exceso = 0;
        int dias_buenos = 0;
        
        int i;
        for (i = 0; i < zona->dias_registrados; i++) {
            /* Maximos */
            if (zona->historico_fechas[i].niveles.co2 > max_co2) max_co2 = zona->historico_fechas[i].niveles.co2;
            if (zona->historico_fechas[i].niveles.so2 > max_so2) max_so2 = zona->historico_fechas[i].niveles.so2;
            if (zona->historico_fechas[i].niveles.no2 > max_no2) max_no2 = zona->historico_fechas[i].niveles.no2;
            if (zona->historico_fechas[i].niveles.pm25 > max_pm25) max_pm25 = zona->historico_fechas[i].niveles.pm25;
            
            /* Minimos */
            if (zona->historico_fechas[i].niveles.co2 < min_co2) min_co2 = zona->historico_fechas[i].niveles.co2;
            if (zona->historico_fechas[i].niveles.so2 < min_so2) min_so2 = zona->historico_fechas[i].niveles.so2;
            if (zona->historico_fechas[i].niveles.no2 < min_no2) min_no2 = zona->historico_fechas[i].niveles.no2;
            if (zona->historico_fechas[i].niveles.pm25 < min_pm25) min_pm25 = zona->historico_fechas[i].niveles.pm25;
            
            /* Contar dias con excesos */
            int excesos_dia = 0;
            if (zona->historico_fechas[i].niveles.co2 > LIMITE_CO2_OMS) excesos_dia++;
            if (zona->historico_fechas[i].niveles.so2 > LIMITE_SO2_OMS) excesos_dia++;
            if (zona->historico_fechas[i].niveles.no2 > LIMITE_NO2_OMS) excesos_dia++;
            if (zona->historico_fechas[i].niveles.pm25 > LIMITE_PM25_OMS) excesos_dia++;
            
            if(excesos_dia > 0) dias_exceso++;
            else dias_buenos++;
        }
        
        fprintf(archivo, "PROMEDIOS DEL PERIODO (%d dias):\n", zona->dias_registrados);
        fprintf(archivo, "   CO2:   %.1f ppm\n", zona->promedio_30_dias[0]);
        fprintf(archivo, "   SO2:   %.1f ug/m3\n", zona->promedio_30_dias[1]);
        fprintf(archivo, "   NO2:   %.1f ug/m3\n", zona->promedio_30_dias[2]);
        fprintf(archivo, "   PM2.5: %.1f ug/m3\n\n", zona->promedio_30_dias[3]);
        
        fprintf(archivo, "VALORES MAXIMOS REGISTRADOS:\n");
        fprintf(archivo, "   CO2:   %.1f ppm\n", max_co2);
        fprintf(archivo, "   SO2:   %.1f ug/m3\n", max_so2);
        fprintf(archivo, "   NO2:   %.1f ug/m3\n", max_no2);
        fprintf(archivo, "   PM2.5: %.1f ug/m3\n\n", max_pm25);
        
        fprintf(archivo, "VALORES MINIMOS REGISTRADOS:\n");
        fprintf(archivo, "   CO2:   %.1f ppm\n", min_co2);
        fprintf(archivo, "   SO2:   %.1f ug/m3\n", min_so2);
        fprintf(archivo, "   NO2:   %.1f ug/m3\n", min_no2);
        fprintf(archivo, "   PM2.5: %.1f ug/m3\n\n", min_pm25);
        
        fprintf(archivo, "ANALISIS DE CALIDAD:\n");
        fprintf(archivo, "   Dias con buena calidad:    %2d de %2d (%.1f%%)\n", 
                dias_buenos, zona->dias_registrados, 
                (float)dias_buenos / zona->dias_registrados * 100.0);
        fprintf(archivo, "   Dias con excesos OMS:      %2d de %2d (%.1f%%)\n", 
                dias_exceso, zona->dias_registrados, 
                (float)dias_exceso / zona->dias_registrados * 100.0);
        
        if(dias_exceso == 0) {
            fprintf(archivo, "   EXCELENTE: Sin dias con excesos registrados\n");
        } else if(dias_exceso <= zona->dias_registrados * 0.1) {
            fprintf(archivo, "   BUENO: Muy pocos dias con excesos\n");
        } else if(dias_exceso <= zona->dias_registrados * 0.3) {
            fprintf(archivo, "   MODERADO: Algunos dias con excesos\n");
        } else {
            fprintf(archivo, "   PREOCUPANTE: Muchos dias con excesos\n");
        }
    }
    
    fprintf(archivo, "\n");
    
    /* PIE DEL REPORTE */
    fprintf(archivo, "╔══════════════════════════════════════════════════════════════════════════════════╗\n");
    fprintf(archivo, "║                              INFORMACION ADICIONAL                              ║\n");
    fprintf(archivo, "╠══════════════════════════════════════════════════════════════════════════════════╣\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║  Para actualizaciones en tiempo real:    www.airnow-quito.gov.ec             ║\n");
    fprintf(archivo, "║  Emergencias ambientales:                911                                  ║\n");
    fprintf(archivo, "║  Salud respiratoria MSP:                  171                                  ║\n");
    fprintf(archivo, "║  Los limites utilizados corresponden a las Guias OMS 2021                     ║\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "║                Cuidemos juntos la calidad del aire de Quito                   ║\n");
    fprintf(archivo, "║                                                                                  ║\n");
    fprintf(archivo, "╚══════════════════════════════════════════════════════════════════════════════════╝\n");
    fprintf(archivo, "                                                                                    \n");
    fprintf(archivo, "              ███████ ███  ██ ██████   █████  ███████  ██████  ███████ ██████   █████  ██████  ████████\n");
    fprintf(archivo, "              ██      ████ ██ ██   ██ ██   ██ ██      ██   ██ ██      ██   ██ ██   ██ ██   ██    ██   \n");
    fprintf(archivo, "              █████   ██ ████ ██   ██ ██   ██ █████   ██████  █████   ██████  ██   ██ ██████     ██   \n");
    fprintf(archivo, "              ██      ██  ███ ██   ██ ██   ██ ██      ██   ██ ██      ██      ██   ██ ██   ██    ██   \n");
    fprintf(archivo, "              ███████ ██   ██ ██████   █████  ██      ██   ██ ███████ ██       █████  ██   ██    ██   \n");
    fprintf(archivo, "                                                                                    \n");
    
    fclose(archivo);
    printf("Reporte AirQuality exportado exitosamente: %s\n", nombre_archivo);
}

void menuExportarReportes(ZonaUrbana zonas[]) {
    printf("=== EXPORTAR REPORTES ===\n");
    printf("==========================\n\n");
    
    printf("Zonas disponibles:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s\n", zonas[i].id_zona, zonas[i].nombre);
    }
    
    int zona_id, val;
    do {
        printf("\nIngrese el ID de la zona (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &zona_id);
        fflush(stdin);
        
        if(val != 1 || zona_id < 1 || zona_id > MAX_ZONAS) {
            printf("ID de zona inválido. Por favor, intente de nuevo.\n");
        }
    } while(val != 1 || zona_id < 1 || zona_id > MAX_ZONAS);
    
    exportarReportePorZona(zonas, zona_id);
    
    printf("\nPresione Enter para continuar...");
    getchar();
}

