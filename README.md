# ProyectoLIDI-TinyML-Autito-UNLP
Este repositorio contiene los archivos y la documentación necesaria para el proyecto **"TinyML: Integrando Inteligencia Artificial en Sistemas Embebidos"**, subproyecto: Autito que se maneja solo. 

El objetivo de este proyecto es explorar y demostrar cómo la inteligencia artificial y el aprendizaje automático pueden ser implementados en sistemas embebidos de recursos limitados, utilizando tecnologías tales como EmbedIA y microcontroladores como el ESP32.

Frente a la creciente demanda de sistemas inteligentes y autónomos, la integración de inteligencia artificial en sistemas embebidos se ha vuelto una necesidad. La inteligencia artificial en sistemas embebidos, también conocida como TinyML, permite que los dispositivos sean capaces de realizar tareas de aprendizaje automático en tiempo real, sin necesidad de conectividad a la nube. Por otro lado, la utilización de microcontroladores de bajo costo y bajo consumo energético, como el ESP32, permite que estos sistemas sean **accesibles** , **económicos** y **ambientalmente amigables**.

## Integrantes
- [César Estrebou]() : Docente a cargo del proyecto.
- [Marcos Saavedra]() : Ingeniero en Computación y Becario de Investigación.
- [Nicolás Ricciardi]() : Estudiante de Licenciatura en Informática.
- [Santiago Chaves]() : Estudiante de Licenciatura en Sistemas.

## Descripción del Proyecto: Autito
El objetivo principal de este proyecto es lograr que un autito autónomo aprenda a conducirse por sí solo, siguiendo una calle. Para ello, se emplean técnicas de inteligencia artificial, en particular aprendizaje por refuerzo (Reinforcement Learning, RL), permitiendo que el agente aprenda a partir de la interacción con el entorno y la retroalimentación que recibe.

El entorno de entenamiento está desarrollado en Unity, una plataforma ampliamente utilizada en videojuegos y simulaciones 3D, lo cual permite recrear escenarios realistas para el entrenamiento del modelo. Para la integración entre Unity y los algoritmos de IA, se utiliza ML-Agents Toolkit, una herramienta de código abierto proporcionada por Unity que facilita la implementación de agentes inteligentes en entornos simulados.


## Requisitos del Proyecto

- **GB necesarios**: Al menos **16 GB**  
- **Unity**: Versión **6.0 (6000.0.49f1) LTS**  
- **Miniconda**: [Descargar desde aquí](https://www.anaconda.com/download/success)  

---

# Guía rápida para instalar y ejecutar el proyecto

### 1. Clonar el repositorio

### 2. Crear y activar el entorno Conda

- Usá el script `manage_env.sh` para manejar las dependencias. Formato del comando:
   ```bash
   bash manage_env.sh [ -p | -u | -c ] [cpu|gpu (opcional, default=cpu)]
   ```
   - `-c`: Crear el entorno (solo una vez)
   - `-p`: Subir/pushear cambios en las dependencias
   - `-u`: Actualizar tus dependencias locales luego de hacer pull

   **Ejemplos usando CPU** (por defecto se usa `cpu` si no se especifica):
   ```bash
   bash manage_env.sh -c         # Crear entorno
   bash manage_env.sh -p         # Pushear dependencias nuevas
   bash manage_env.sh -u         # Actualizar dependencias locales
   ```

   > *Nota:* Actualmente el script `manage_env.sh` solo diferencia entre CPU y GPU para `torch`. Si en el futuro hay más paquetes con esta distinción, se deberá ajustar el script.


- Para activar el entorno:
   ```bash
   conda activate ProyectoLIDI-TinyML-Autito-UNLP
   ```
---

### 3. Instalación de ML-Agents

Luego de crear el entorno, es necesario clonar el repositorio oficial de ML-Agents:
```bash
git clone --branch release_20 https://github.com/Unity-Technologies/ml-agents.git
```

## Guía para entrenar y usar un modelo

### 1. Entrenar el modelo

Para entrenar el modelo se debe usar el siguiente comando
```bash
mlagents-learn [direccion al archivo de configuracion yaml]  --run-id=[Nombre que tendra la run (debe ser unico)]
```
Example: 

```bash
mlagents-learn config/ppo/3DBall.yaml --run-id=first3DBallRun 
```

> *Nota:* Usar la flag `--resume` si el entrenamiento habia sido parado con Ctrl+C

Luego de ejecutar el comando se debera poner el boton de "Play" en Unity. Para una guia mas detallada mirar [Getting Started Guide](https://github.com/Unity-Technologies/ml-agents/blob/release_20_docs/docs/Getting-Started.md). 


En simultaneo, abrir otra terminal y ejecutar para ver un analisis y graficas de la ejecucucion (Se tiene que recargar la pagina para actualizarla)
```bash
tensorboard --logdir results
```

### 2. Probar el modelo

- Luego de entrenar el modelo, este deberia de estar ubicado en results/<run-identifier>/<model_name>.onnx
- Arrastrar el modelo al proyecto unity para copiarlo.
- Por ultimo settear el modelo para su uso como se explica en [Getting Started Guide](https://github.com/Unity-Technologies/ml-agents/blob/release_20_docs/docs/Getting-Started.md)

