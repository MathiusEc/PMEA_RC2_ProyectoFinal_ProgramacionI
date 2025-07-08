
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
        printf("6. Estado del Sistema                     \n");
        printf("0. Salir                                   \n");
        printf("===========================================\n");
        printf("Seleccione una opción: ");
        fflush(stdin);
        val = scanf("%d", &opc);
        fflush(stdin);
        if (val != 1 || opc < 0 || opc > 6)
        {
            printf("Opción inválida. Por favor, intente de nuevo.\n");
        }
    } while (val != 1 || opc < 0 || opc > 6);
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
    }

    // Leer nuevos datos
    printf("Ingrese los niveles de contaminantes para la zona %s:\n", zonas[id_zona - 1].nombre);
    printf("CO2 (ppm): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.co2);
    printf("SO2 (ug/m3): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.so2);
    printf("NO2 (ug/m3): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.no2);
    printf("PM2.5 (ug/m3): ");
    scanf("%f", &zonas[id_zona - 1].niveles_actuales.pm25);

    // Registrar datos climáticos
    printf("Ingrese los datos climaticos para la zona %s:\n", zonas[id_zona - 1].nombre);
    printf("Temperatura (Celsius): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.temperatura);
    printf("Velocidad del viento (km/h): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.velocidad_viento);
    printf("Humedad (%%): ");
    scanf("%f", &zonas[id_zona - 1].clima_actual.humedad);
    printf("Presion (hPa): ");
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
    printf("=== MONITOREO DETALLADO POR ZONA ===\n");
    printf("Fecha: %s - Hora: %s\n", __DATE__, __TIME__);
    printf("===============================================\n");
    printf("ANALISIS ACTUAL DE CONTAMINANTES POR ZONA\n");
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
    
    // 3. HISTORIAL RECIENTE (últimos 5 días)
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
    
    // 4. RECOMENDACIONES ESPECÍFICAS
    printf("\nRECOMENDACIONES PARA %s:\n", zonas[zona_seleccionada].nombre);
    printf("-----------------------------------------------\n");
    
    int total_excesos = 0;
    if(zonas[zona_seleccionada].niveles_actuales.co2 > LIMITE_CO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.so2 > LIMITE_SO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.no2 > LIMITE_NO2_OMS) total_excesos++;
    if(zonas[zona_seleccionada].niveles_actuales.pm25 > LIMITE_PM25_OMS) total_excesos++;
    
    // Determinar nivel de alerta y usar función de recomendaciones
    int nivel_alerta_zona;
    if(total_excesos == 0) {
        nivel_alerta_zona = ALERTA_VERDE;
    } else if(total_excesos <= 1) {
        nivel_alerta_zona = ALERTA_AMARILLA;
    } else if(total_excesos <= 2) {
        nivel_alerta_zona = ALERTA_NARANJA;
    } else {
        nivel_alerta_zona = ALERTA_ROJA;
    }
    
    // Reutilizar función de recomendaciones
    mostrarRecomendaciones(nivel_alerta_zona, "ZONA");
    
    printf("\n========================================================\n");
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
    
    printf("Dia  | CO2    | SO2    | NO2    | PM2.5  | Estado General\n");
    printf("-----|--------|--------|--------|--------|---------------\n");
    
    int dias_mostrar;
    if(zonas[zona_seleccionada].dias_registrados > 10) {
        dias_mostrar = 10;
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
        
        printf("%-4d | %-6.1f | %-6.1f | %-6.1f | %-6.1f | %s",
               i+1,
               zonas[zona_seleccionada].historico[i].co2,
               zonas[zona_seleccionada].historico[i].so2,
               zonas[zona_seleccionada].historico[i].no2,
               zonas[zona_seleccionada].historico[i].pm25,
               estado);
        
        // Marcar si excede límites OMS
        if(excesos > 0) {
            printf(" (");
            if(zonas[zona_seleccionada].historico[i].co2 > LIMITE_CO2_OMS) printf("CO2 ");
            if(zonas[zona_seleccionada].historico[i].so2 > LIMITE_SO2_OMS) printf("SO2 ");
            if(zonas[zona_seleccionada].historico[i].no2 > LIMITE_NO2_OMS) printf("NO2 ");
            if(zonas[zona_seleccionada].historico[i].pm25 > LIMITE_PM25_OMS) printf("PM2.5 ");
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
            printf("SUBIENDO");
        } else {
            printf("BAJANDO");
        }
        printf(" (%.1f%%)\n", porc_co2);
        
        // Análisis SO2
        float dif_so2 = zonas[zona_seleccionada].niveles_actuales.so2 - promedio_pond_so2;
        float porc_so2 = 0;
        if(promedio_pond_so2 != 0) {
            porc_so2 = absoluto(dif_so2 / promedio_pond_so2 * 100);
        }
        
        printf("SO2 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_so2, zonas[zona_seleccionada].niveles_actuales.so2, dif_so2);
        
        if(zonas[zona_seleccionada].niveles_actuales.so2 > promedio_pond_so2) {
            printf("SUBIENDO");
        } else {
            printf("BAJANDO");
        }
        printf(" (%.1f%%)\n", porc_so2);
        
        // Análisis NO2
        float dif_no2 = zonas[zona_seleccionada].niveles_actuales.no2 - promedio_pond_no2;
        float porc_no2 = 0;
        if(promedio_pond_no2 != 0) {
            porc_no2 = absoluto(dif_no2 / promedio_pond_no2 * 100);
        }
        
        printf("NO2 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_no2, zonas[zona_seleccionada].niveles_actuales.no2, dif_no2);
        
        if(zonas[zona_seleccionada].niveles_actuales.no2 > promedio_pond_no2) {
            printf("SUBIENDO");
        } else {
            printf("BAJANDO");
        }
        printf(" (%.1f%%)\n", porc_no2);
        
        // Análisis PM2.5
        float dif_pm25 = zonas[zona_seleccionada].niveles_actuales.pm25 - promedio_pond_pm25;
        float porc_pm25 = 0;
        if(promedio_pond_pm25 != 0) {
            porc_pm25 = absoluto(dif_pm25 / promedio_pond_pm25 * 100);
        }
        
        printf("PM2.5 (ug/m3)\t| %.1f\t\t| %.1f\t\t| %.1f\t\t| ", 
               promedio_pond_pm25, zonas[zona_seleccionada].niveles_actuales.pm25, dif_pm25);
        
        if(zonas[zona_seleccionada].niveles_actuales.pm25 > promedio_pond_pm25) {
            printf("SUBIENDO");
        } else {
            printf("BAJANDO");
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
    
    do {
        printf("\nSeleccione una zona (1-5): ");
        scanf("%d", &zona_seleccionada);
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


// ================= FUNCION DE CORRECCION DE DATOS INGRESADOS =================
void corregirDatosIngresados(ZonaUrbana zonas[]) {
    int id_zona, val, dia, tipo, subop;
    float nuevo_valor;
    char confirmacion;

    // Mostrar zonas disponibles
    cargarTodasLasZonas(zonas);
    printf("\nZONAS DISPONIBLES PARA CORRECCION:\n");
    for(int i = 0; i < MAX_ZONAS; i++) {
        printf("%d. %s (%d dias de datos)\n", zonas[i].id_zona, zonas[i].nombre, zonas[i].dias_registrados);
    }

    // Seleccionar zona
    do {
        printf("Seleccione la zona a corregir (1-%d): ", MAX_ZONAS);
        val = scanf("%d", &id_zona);
        // Limpiar buffer de entrada
        fflush(stdin);
        if(val != 1 || id_zona < 1 || id_zona > MAX_ZONAS) {
            printf("Opcion invalida. Intente de nuevo.\n");
        }
    } while(val != 1 || id_zona < 1 || id_zona > MAX_ZONAS);

    ZonaUrbana *zona = &zonas[id_zona-1];
    if(zona->dias_registrados == 0) {
        printf("No hay datos registrados para esta zona.\n");
        return;
    }

    // Mostrar días disponibles
    printf("\nDias disponibles para correccion en %s:\n", zona->nombre);
    for(int i = 0; i < zona->dias_registrados; i++) {
        printf("Dia %d: CO2=%.1f, SO2=%.1f, NO2=%.1f, PM2.5=%.1f\n", i+1, zona->historico[i].co2, zona->historico[i].so2, zona->historico[i].no2, zona->historico[i].pm25);
    }

    // Seleccionar día
    do {
        printf("Seleccione el dia a corregir (1-%d): ", zona->dias_registrados);
        val = scanf("%d", &dia);
        fflush(stdin);
        if(val != 1 || dia < 1 || dia > zona->dias_registrados) {
            printf("Dia invalido. Intente de nuevo.\n");
        }
    } while(val != 1 || dia < 1 || dia > zona->dias_registrados);
    dia--; // índice

    // Elegir tipo de dato a corregir
    printf("\nSeleccione el tipo de dato a corregir:\n");
    printf("1. Contaminantes\n");
    printf("2. Datos climaticos\n");
    do {
        printf("Opcion: ");
        val = scanf("%d", &tipo);
        fflush(stdin);
        if(val != 1 || (tipo != 1 && tipo != 2)) {
            printf("Opcion invalida. Intente de nuevo.\n");
        }
    } while(val != 1 || (tipo != 1 && tipo != 2));

    if(tipo == 1) {
        // Contaminantes
        printf("\nSeleccione el contaminante a corregir:\n");
        printf("1. CO2\n2. SO2\n3. NO2\n4. PM2.5\n");
        do {
            printf("Opcion: ");
            val = scanf("%d", &subop);
            fflush(stdin);
            if(val != 1 || subop < 1 || subop > 4) {
                printf("Opcion invalida. Intente de nuevo.\n");
            }
        } while(val != 1 || subop < 1 || subop > 4);

        printf("Ingrese el nuevo valor: ");
        val = scanf("%f", &nuevo_valor);
        fflush(stdin);
        if(val != 1) {
            printf("Valor invalido. Operacion cancelada.\n");
            return;
        }

            do {
                printf("Confirma la correccion? (s/n): ");
                scanf(" %c", &confirmacion);
                fflush(stdin);
                if(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N') {
                printf("Opcion invalida. Ingrese 's' para si o 'n' para no.\n");
                }
            } while(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N');

            if(confirmacion == 'n' || confirmacion == 'N') {
                printf("Operacion cancelada.\n");
                return;
            }

            switch(subop) {
                case 1: zona->historico[dia].co2 = nuevo_valor; break;
                case 2: zona->historico[dia].so2 = nuevo_valor; break;
                case 3: zona->historico[dia].no2 = nuevo_valor; break;
                case 4: zona->historico[dia].pm25 = nuevo_valor; break;
            }
            // Si es el día más reciente, actualizar niveles actuales
            if(dia == 0) {
                zona->niveles_actuales = zona->historico[0];
            }
            printf("Dato corregido exitosamente.\n");
            } else {
            // Datos climáticos
            printf("\nSeleccione el dato climatico a corregir:\n");
            printf("1. Temperatura\n2. Velocidad del viento\n3. Humedad\n4. Presion atmosferica\n");
            do {
                printf("Opcion: ");
                val = scanf("%d", &subop);
                fflush(stdin);
                if(val != 1 || subop < 1 || subop > 4) {
                printf("Opcion invalida. Intente de nuevo.\n");
                }
            } while(val != 1 || subop < 1 || subop > 4);

            printf("Ingrese el nuevo valor: ");
            val = scanf("%f", &nuevo_valor);
            fflush(stdin);
            if(val != 1) {
                printf("Valor invalido. Operacion cancelada.\n");
                return;
            }

            do {
                printf("Confirma la correccion? (s/n): ");
                scanf(" %c", &confirmacion);
                fflush(stdin);
                if(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N') {
                printf("Opcion invalida. Ingrese 's' para si o 'n' para no.\n");
                }
            } while(confirmacion != 's' && confirmacion != 'S' && confirmacion != 'n' && confirmacion != 'N');

            if(confirmacion == 'n' || confirmacion == 'N') {
                printf("Operacion cancelada.\n");
                return;
            }


        switch(subop) {
            case 1: zona->clima_actual.temperatura = nuevo_valor; break;
            case 2: zona->clima_actual.velocidad_viento = nuevo_valor; break;
            case 3: zona->clima_actual.humedad = nuevo_valor; break;
            case 4: zona->clima_actual.presion_atmosferica = nuevo_valor; break;
        }
        printf("Dato climatico corregido exitosamente.\n");
    }

    // Guardar zona actualizada
    guardarZona(zona);
}

