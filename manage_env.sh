#!/bin/bash

# Verificación de argumentos
if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
  echo "Uso: $0 [ -p (push) | -u (update) | -c (create) ] [cpu|gpu (opcional, default=cpu)]"
  exit 1
fi

ACTION="$1"
MODE="$2"

# Validaciones
if [[ "$MODE" != "cpu" && "$MODE" != "gpu" ]]; then
  MODE="cpu"
fi

if [[ "$ACTION" != "-p" && "$ACTION" != "-u" && "$ACTION" != "-c" ]]; then
  echo "Acción inválida: $ACTION. Debe ser '-p', '-u' o '-c'."
  exit 1
fi

# Archivos
ENV_NAME="ProyectoLIDI-TinyML-Autito-UNLP"
COMMON_FILE="env_common.yml"
EXTRA_FILE="env_${MODE}_extra.yml"

# Acciones
if [[ "$ACTION" == "-p" ]]; then
  echo "Exportando entorno '$ENV_NAME'..."

  # Exportar dependencias completas del entorno (sin builds)
  conda env export -n "$ENV_NAME" --no-builds > temp_full.yml

  # Extraer en un temporal las líneas de dependencias (solo las que empiezan con "- ")
  grep '^[[:space:]]*-[[:space:]]torch*' "$EXTRA_FILE" > extra_lines.tmp

  # Mostrar en pantalla su contenido
  #echo "=== Contenido de extra_lines.tmp ==="
  #cat extra_lines.tmp
  #echo "==================================="

  # Filtrar temp_full.yml: cualquier línea que coincida EXACTAMENTE con alguna de extra_lines.tmp
  grep -Fv -f extra_lines.tmp temp_full.yml > "$COMMON_FILE"

  # Limpiar temporales
  rm extra_lines.tmp

  # Limpiar archivo temporal
  rm temp_full.yml


  echo "Accion completada."
  echo "Nota: Si se quieren actualizar archivos especificos de cpu o gpu que no sean de torch esto no funciona."

elif [[ "$ACTION" == "-u" ]]; then
  echo "Actualizando entorno '$ENV_NAME' con $MODE..."

  conda env update -n "$ENV_NAME" -f "$COMMON_FILE" --prune
  conda env update -n "$ENV_NAME" -f "$EXTRA_FILE"

  echo "Entorno actualizado."

elif [[ "$ACTION" == "-c" ]]; then
  echo "Creando entorno '$ENV_NAME' con $MODE..."

  conda env create -n "$ENV_NAME" -f "$COMMON_FILE"
  conda env update -n "$ENV_NAME" -f "$EXTRA_FILE"

  echo "Entorno creado."
fi

# Eliminar las clausulas prefix
sed -i '/^prefix:/d' "$COMMON_FILE" "$EXTRA_FILE"

