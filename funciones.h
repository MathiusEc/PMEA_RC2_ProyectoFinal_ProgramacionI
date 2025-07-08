#define MAX_ZONAS 5
#define MAX_DIAS_HISTORICOS 30
#define MAX_NOMBRE 50

// Límites OMS (valores de referencia)
#define LIMITE_CO2_OMS 400.0     // ppm
#define LIMITE_SO2_OMS 20.0      // µg/m³ (24h)
#define LIMITE_NO2_OMS 25.0      // µg/m³ (24h)
#define LIMITE_PM25_OMS 15.0     // µg/m³ (24h)

// Niveles de alerta
#define ALERTA_VERDE 0
#define ALERTA_AMARILLA 1
#define ALERTA_NARANJA 2
#define ALERTA_ROJA 3

// Pesos para promedio ponderado (predicción)
#define PESO_DIA_1 0.4    // Día más reciente
#define PESO_DIA_2 0.3    // Segundo día
#define PESO_DIA_3 0.2    // Tercer día
#define PESO_RESTO 0.1    // Días anteriores

// Estructura para datos climáticos
typedef struct {
    float temperatura;
    float velocidad_viento;
    float humedad;
    float presion_atmosferica;
} DatosClimaticos;

// Estructura para niveles de contaminantes
typedef struct {
    float co2;      // CO₂ en ppm
    float so2;      // SO₂ en µg/m³
    float no2;      // NO₂ en µg/m³
    float pm25;     // PM2.5 en µg/m³
} NivelesContaminacion;

// Estructura para límites OMS
typedef struct {
    float co2_limite;
    float so2_limite;
    float no2_limite;
    float pm25_limite;
} LimitesOMS;

// Estructura para una zona urbana
typedef struct {
    char nombre[MAX_NOMBRE];
    int id_zona;
    NivelesContaminacion niveles_actuales;
    NivelesContaminacion historico[MAX_DIAS_HISTORICOS];
    DatosClimaticos clima_actual;
    float promedio_30_dias[4]; // Para CO₂, SO₂, NO₂, PM2.5
    int dias_registrados;
} ZonaUrbana;

// Estructura para predicciones
typedef struct {
    int zona_id;
    NivelesContaminacion prediccion_24h;
    float probabilidad_alerta;
    int nivel_alerta; // 0=Verde, 1=Amarillo, 2=Naranja, 3=Rojo
} Prediccion;

// Estructura para recomendaciones
typedef struct {
    char mensaje[200];
    int prioridad; // 1=Baja, 2=Media, 3=Alta
    int tipo; // 1=Tráfico, 2=Industrial, 3=Actividades exteriores
} Recomendacion;

//PROTIPADO DE FUNCIONES
void leerCadena(char *cadena, int num);
int menu();

// Función para validar datos de entrada con rangos específicos
void funcionValidarDatosdeRegistro(float *valor, char *nombre_dato, float min_val, float max_val);

// Permite modificar datos históricos de contaminantes y clima para una zona y día específico
void corregirDatosIngresados(ZonaUrbana zonas[]);

// Funciones de inicialización
void inicializarZonas(ZonaUrbana zonas[]);

// Funciones para archivos separados
void guardarZona(ZonaUrbana *zona);
void guardarTodasLasZonas(ZonaUrbana zonas[]);
int cargarZona(ZonaUrbana *zona, int id_zona);
int cargarTodasLasZonas(ZonaUrbana zonas[]);

// Funciones principales del sistema
void registroDatosDiario(ZonaUrbana zonas[]);
void monitoreoDetalladoPorZona(ZonaUrbana zonas[]);
void mostrarTendenciasHistorico(ZonaUrbana zonas[]);
void prediccionContaminacion24h(ZonaUrbana zonas[]);
void mostrarEstadoSistema(ZonaUrbana zonas[]);

// Funciones auxiliares para predicción
float calcularPrediccion(float *historico, int dias_disponibles);
float ajustarPorClima(float prediccion_base, DatosClimaticos clima);
int determinarNivelAlerta(float valor, int tipo_contaminante);
void mostrarRecomendaciones(int nivel_alerta, char *contaminante);

// Funciones para predicción climática
DatosClimaticos predecirClima24h(ZonaUrbana *zona);
float calcularPrediccionClimatica(float *historico_clima, int dias_disponibles);

