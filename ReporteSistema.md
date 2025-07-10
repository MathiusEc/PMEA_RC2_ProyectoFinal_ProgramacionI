# Documentación Técnica: Sistema Integral de Gestión y Predicción de Contaminación del Aire

## Índice
1. [Arquitectura General](#arquitectura-general)
2. [Estructuras de Datos](#estructuras-de-datos)
3. [Algoritmos Clave](#algoritmos-clave)
4. [Persistencia de Datos](#persistencia-de-datos)
5. [Flujo de Datos](#flujo-de-datos)
6. [Análisis de Complejidad](#análisis-de-complejidad)

---

## Arquitectura General

El sistema está diseñado con una arquitectura modular que separa claramente:
- **Interfaz de usuario** (menús interactivos)
- **Lógica de negocio** (algoritmos de predicción y validación)
- **Persistencia** (manejo de archivos binarios)
- **Presentación** (reportes y visualización)

### Archivos del Sistema
- `main.c`: Punto de entrada y menú principal
- `funciones.c`: Implementación de todos los algoritmos y funcionalidades
- `funciones.h`: Definiciones de estructuras, constantes y prototipos
- `migrador_30_a_365.c`: Herramienta de migración de datos

---

## Estructuras de Datos

### 1. Estructura Principal: `ZonaUrbana`

```c
typedef struct {
    char nombre[MAX_NOMBRE];                                    // 50 bytes
    int id_zona;                                               // 4 bytes
    NivelesContaminacion niveles_actuales;                     // 16 bytes
    NivelesContaminacion historico[MAX_DIAS_HISTORICOS];       // 16 * 365 = 5,840 bytes
    RegistroHistorico historico_fechas[MAX_DIAS_HISTORICOS];   // 40 * 365 = 14,600 bytes
    DatosClimaticos clima_actual;                              // 16 bytes
    float promedio_30_dias[4];                                 // 16 bytes
    int dias_registrados;                                      // 4 bytes
} ZonaUrbana;
```

**Tamaño total por zona: ~20,546 bytes (≈20KB)**

### 2. Estructuras Anidadas

#### `NivelesContaminacion`
```c
typedef struct {
    float co2;      // CO₂ en ppm
    float so2;      // SO₂ en µg/m³
    float no2;      // NO₂ en µg/m³
    float pm25;     // PM2.5 en µg/m³
} NivelesContaminacion;  // 16 bytes
```

#### `RegistroHistorico`
```c
typedef struct {
    Fecha fecha;                    // 12 bytes (3 int)
    NivelesContaminacion niveles;   // 16 bytes
    DatosClimaticos clima;          // 16 bytes
} RegistroHistorico;                // 44 bytes total
```

#### `DatosClimaticos`
```c
typedef struct {
    float temperatura;              // °C
    float velocidad_viento;         // km/h
    float humedad;                  // %
    float presion_atmosferica;      // hPa
} DatosClimaticos;                  // 16 bytes
```

### 3. Jerarquía de Anidación

```
ZonaUrbana
├── nombre[50]
├── id_zona
├── niveles_actuales → NivelesContaminacion
│   ├── co2, so2, no2, pm25
├── historico[365] → Array de NivelesContaminacion
├── historico_fechas[365] → Array de RegistroHistorico
│   ├── fecha → Fecha {dia, mes, año}
│   ├── niveles → NivelesContaminacion
│   └── clima → DatosClimaticos
├── clima_actual → DatosClimaticos
├── promedio_30_dias[4]
└── dias_registrados
```

---

## Algoritmos Clave

### 1. Algoritmo de Promedio Ponderado

**Ubicación**: `calcularPrediccion()` en `funciones.c:1087`

#### Descripción
Implementa un algoritmo de predicción que asigna diferentes pesos a los datos históricos, priorizando los registros más recientes.

#### Implementación
```c
float calcularPrediccion(float *historico, int dias_disponibles) {
    if(dias_disponibles < 3) return 0.0;
    
    float prediccion = 0.0;
    
    // Pesos definidos en funciones.h
    prediccion = historico[dias_disponibles-1] * PESO_DIA_1;  // 0.4
    prediccion += historico[dias_disponibles-2] * PESO_DIA_2; // 0.3
    prediccion += historico[dias_disponibles-3] * PESO_DIA_3; // 0.2
    
    // Distribución del peso restante para días anteriores
    if(dias_disponibles > 3) {
        float suma_resto = 0.0;
        for(int i = 0; i < dias_disponibles - 3; i++) {
            suma_resto += historico[i];
        }
        prediccion += (suma_resto / (dias_disponibles - 3)) * PESO_RESTO; // 0.1
    }
    
    return prediccion;
}
```

#### Configuración de Pesos
```c
#define PESO_DIA_1 0.4    // Día más reciente (40%)
#define PESO_DIA_2 0.3    // Segundo día (30%)
#define PESO_DIA_3 0.2    // Tercer día (20%)
#define PESO_RESTO 0.1    // Días anteriores distribuidos (10%)
```

#### Complejidad
- **Temporal**: O(n) donde n = días_disponibles
- **Espacial**: O(1)

### 2. Algoritmo de Ajuste Climático

**Ubicación**: `ajustarPorClima()` en `funciones.c:1113`

#### Descripción
Modifica las predicciones base aplicando factores de corrección basados en condiciones climáticas actuales.

#### Factores de Ajuste
```c
float ajustarPorClima(float prediccion_base, DatosClimaticos clima) {
    float factor_ajuste = 1.0;
    
    // Temperatura
    if(clima.temperatura > 30.0)      factor_ajuste *= 1.10;  // +10%
    else if(clima.temperatura < 10.0) factor_ajuste *= 1.05;  // +5%
    
    // Viento (dispersión)
    if(clima.velocidad_viento > 20.0)    factor_ajuste *= 0.85;  // -15%
    else if(clima.velocidad_viento < 5.0) factor_ajuste *= 1.15;  // +15%
    
    // Humedad
    if(clima.humedad > 80.0) factor_ajuste *= 1.08;  // +8%
    
    // Presión atmosférica
    if(clima.presion_atmosferica < 1000.0) factor_ajuste *= 1.05;  // +5%
    
    return prediccion_base * factor_ajuste;
}
```

#### Base Científica
- **Temperatura alta**: Acelera reacciones fotoquímicas
- **Viento fuerte**: Dispersa contaminantes
- **Alta humedad**: Facilita formación de aerosoles
- **Baja presión**: Reduce dispersión vertical

### 3. Algoritmo FIFO (First In, First Out)

**Ubicación**: `registroDatosDiario()` en `funciones.c:237`

#### Descripción
Implementa un sistema de cola circular para mantener un historial fijo de 365 días, donde los datos más antiguos se eliminan automáticamente.

#### Implementación
```c
// Mover datos históricos (FIFO - lo más reciente al inicio)
for(int i = MAX_DIAS_HISTORICOS-1; i > 0; i--) {
    zonas[id_zona - 1].historico[i] = zonas[id_zona - 1].historico[i-1];
    zonas[id_zona - 1].historico_fechas[i] = zonas[id_zona - 1].historico_fechas[i-1];
}

// Insertar nuevo dato en posición 0
zonas[id_zona - 1].historico[0] = zonas[id_zona - 1].niveles_actuales;
```

#### Características
- **Posición 0**: Datos más recientes
- **Posición n-1**: Datos más antiguos
- **Capacidad máxima**: 365 días
- **Comportamiento**: Al superar la capacidad, elimina automáticamente el dato más antiguo

### 4. Sistema de Alertas por Umbrales

**Ubicación**: `determinarNivelAlerta()` en `funciones.c:1141`

#### Descripción
Clasifica los niveles de contaminación en 4 categorías basadas en múltiplos de los límites OMS.

#### Implementación
```c
int determinarNivelAlerta(float valor, int tipo_contaminante) {
    float limite_oms = obtenerLimiteOMS(tipo_contaminante);
    
    if(valor <= limite_oms)        return ALERTA_VERDE;     // ≤ 100% OMS
    else if(valor <= limite_oms * 1.5) return ALERTA_AMARILLA;  // ≤ 150% OMS
    else if(valor <= limite_oms * 2.0) return ALERTA_NARANJA;   // ≤ 200% OMS
    else                           return ALERTA_ROJA;      // > 200% OMS
}
```

#### Umbrales OMS Implementados
```c
#define LIMITE_CO2_OMS 1000.0    // ppm
#define LIMITE_SO2_OMS 40.0      // µg/m³
#define LIMITE_NO2_OMS 25.0      // µg/m³
#define LIMITE_PM25_OMS 15.0     // µg/m³
```

### 5. Validación de Datos por Rangos

**Ubicación**: `funcionValidarDatosdeRegistro()` en `funciones.c:200`

#### Descripción
Implementa validación robusta de entrada con rangos específicos para cada tipo de dato.

#### Rangos de Validación
| Contaminante | Mínimo | Máximo | Unidad |
|--------------|--------|--------|---------|
| CO₂          | 0.0    | 1000.0 | ppm     |
| SO₂          | 0.0    | 500.0  | µg/m³   |
| NO₂          | 0.0    | 300.0  | µg/m³   |
| PM2.5        | 0.0    | 200.0  | µg/m³   |
| Temperatura  | -20.0  | 50.0   | °C      |
| Viento       | 0.0    | 120.0  | km/h    |
| Humedad      | 0.0    | 100.0  | %       |
| Presión      | 900.0  | 1100.0 | hPa     |

---

## Persistencia de Datos

### 1. Arquitectura de Archivos

El sistema utiliza **archivos binarios separados** para cada zona:
- `zona_1.dat` - Centro Histórico
- `zona_2.dat` - Norte - La Carolina
- `zona_3.dat` - Sur - Quitumbe
- `zona_4.dat` - Valle Los Chillos
- `zona_5.dat` - Cumbayá - Tumbaco

### 2. Formato de Almacenamiento

Cada archivo contiene **una estructura `ZonaUrbana` completa** serializada en binario:

```c
void guardarZona(ZonaUrbana *zona) {
    char nombre_archivo[100];
    sprintf(nombre_archivo, "zona_%d.dat", zona->id_zona);
    
    FILE *f = fopen(nombre_archivo, "wb");
    if(f != NULL) {
        fwrite(zona, sizeof(ZonaUrbana), 1, f);
        fclose(f);
    }
}
```

### 3. Migración de Datos

**Problema**: Cambiar `MAX_DIAS_HISTORICOS` de 30 a 365 cambia el tamaño de la estructura.

**Solución**: `migrador_30_a_365.c`
1. **Backup automático** de archivos originales
2. **Lectura** con estructura antigua (30 días)
3. **Conversión** a estructura nueva (365 días)
4. **Verificación** de integridad de datos
5. **Reemplazo** seguro de archivos

---

## Flujo de Datos

### 1. Registro de Nuevos Datos

```
Input Usuario → Validación → FIFO Shift → Actualización → Persistencia
     ↓              ↓            ↓           ↓              ↓
 [Formulario] → [Rangos OMS] → [Array] → [Estructura] → [Archivo.dat]
```

### 2. Predicción 24 Horas

```
Datos Históricos → Promedio Ponderado → Ajuste Climático → Clasificación
       ↓                   ↓                ↓                  ↓
  [365 días]        [Algoritmo 40:30:20:10] → [Factores] → [Nivel Alerta]
```

### 3. Exportación de Reportes

```
Datos Binarios → Procesamiento → Formato Texto → Archivo TXT
      ↓              ↓              ↓              ↓
 [zona_X.dat] → [Cálculos] → [Tablas ASCII] → [reporte_X.txt]
```

---

## Análisis de Complejidad

### 1. Complejidad Temporal

| Operación | Complejidad | Descripción |
|-----------|-------------|-------------|
| Insertar dato | O(n) | FIFO shift, n = MAX_DIAS_HISTORICOS |
| Búsqueda por fecha | O(n) | Búsqueda lineal en histórico |
| Predicción | O(n) | Promedio ponderado |
| Validación | O(1) | Verificación de rangos |
| Exportar reporte | O(n) | Iteración sobre histórico |

### 2. Complejidad Espacial

| Componente | Memoria | Descripción |
|------------|---------|-------------|
| Una zona | ~20 KB | Estructura completa |
| Sistema completo | ~100 KB | 5 zonas en memoria |
| Archivo por zona | ~20 KB | Persistencia binaria |

### 3. Escalabilidad

#### Limitaciones Actuales
- **Zonas**: Fijo a 5 (configurable con `MAX_ZONAS`)
- **Historial**: Fijo a 365 días (configurable con `MAX_DIAS_HISTORICOS`)
- **Memoria**: Lineal con número de zonas y días

#### Optimizaciones Posibles
1. **Paginación** de datos históricos
2. **Índices** para búsquedas por fecha
3. **Compresión** de datos antiguos
4. **Base de datos** para grandes volúmenes

---

## Conclusiones

El sistema implementa un diseño robusto que combina:

1. **Algoritmos predictivos** basados en evidencia científica
2. **Estructuras de datos** optimizadas para el dominio específico
3. **Persistencia confiable** con migración segura
4. **Validación exhaustiva** de entrada de datos
5. **Escalabilidad controlada** dentro de los límites establecidos

La arquitectura permite extensiones futuras manteniendo la compatibilidad hacia atrás y garantizando la integridad de los datos históricos acumulados.

---

**Fecha de documentación**: Julio 2025  
**Versión del sistema**: 1.0 (365 días)  
**Autor**: Castillo Mathías, Benítez Isaac.
