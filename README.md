

# 🌍 Proyecto: Sistema Integral de Gestión y Predicción de Contaminación del Aire en Zonas Urbanas

## 🎯 Objetivos Propuestos

- Diseñar una solución de software utilizando técnicas y métodos óptimos, considerando las restricciones del problema en los ámbitos **sociales**, **ambientales** y **económicos**.
- Realizar un análisis integral del problema, identificando sus principales variables y restricciones.
- Plantear diversas alternativas de solución y evaluar la más efectiva para abordar el problema de manera holística.

## 🛠️ Descripción del Problema

El crecimiento urbano descontrolado y el aumento del tráfico vehicular han elevado los niveles de contaminación atmosférica, afectando la salud pública y el medio ambiente. Contaminantes como **CO₂**, **SO₂**, **NO₂** y **PM2.5** representan serias amenazas.

Un reto crucial es la **falta de sistemas eficientes de monitoreo y predicción**, lo que limita la toma de decisiones a tiempo por parte de las autoridades.

## 🧩 Solución Propuesta

Desarrollar un **programa en lenguaje C** que:

- Monitoree la calidad del aire en al menos 5 zonas de la ciudad.
- Prediga los niveles de contaminación a partir de datos históricos y condiciones climáticas actuales.
- Genere alertas preventivas.
- Ofrezca recomendaciones para mitigar los efectos nocivos de la contaminación.

## ✅ Requerimientos Funcionales

1. **Monitoreo Actual**
   - Calcular niveles actuales de contaminación.
   - Compararlos con los límites aceptables establecidos.

2. **Predicción de Contaminación**
   - Usar datos históricos y factores climáticos (temperatura, viento, humedad) para estimar niveles futuros (próximas 24h).

3. **Alertas Preventivas**
   - Emitir advertencias si se prevé que los niveles excedan los límites aceptables.

4. **Cálculo de Promedios**
   - Promediar datos de los últimos 30 días por zona y comparar con los estándares de la **OMS**.

5. **Recomendaciones de Mitigación**
   - Sugerir medidas como reducción de tráfico, suspensión de actividades industriales o al aire libre.

6. **Exportación de Datos**
   - Generar archivos con los datos históricos y predicciones para análisis posterior.

## 💾 Requerimientos Técnicos

- Uso de:
  - **Arreglos**
  - **Estructuras**
  - **Punteros**
  - **Funciones**
  - **Archivos**
  
- Algoritmo de predicción:
  - Se permite el uso de **promedio ponderado**, dando mayor peso a los días más recientes.

- Persistencia de datos:
  - Los registros deben guardarse en archivos para mantener datos históricos.

- Restricciones:
  - **No usar librerías externas** para archivos o cálculos avanzados.

