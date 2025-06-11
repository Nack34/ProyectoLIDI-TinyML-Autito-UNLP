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

  conda env export -n "$ENV_NAME" --no-builds > temp_full.yml

  grep '^[[:space:]]*-[[:space:]]torch*' "$EXTRA_FILE" > extra_lines.tmp

  # Filtrar temp_full.yml: cualquier línea que coincida EXACTAMENTE con alguna de extra_lines.tmp
  grep -Fv -f extra_lines.tmp temp_full.yml > "$COMMON_FILE"

  rm extra_lines.tmp
  rm temp_full.yml


  echo "Accion completada."
  echo "Nota: Si se quieren actualizar archivos especificos de cpu o gpu que no sean de torch esto no funciona."

elif [[ "$ACTION" == "-u" ]]; then
  echo "Actualizando entorno '$ENV_NAME' con $MODE..."
  
  cp "$COMMON_FILE" temp_env.yml
  YML_FILE="temp_env.yml"

  sed -i '/- "--index-url https:\/\/download.pytorch.org\/whl\/cu126"/d' "$YML_FILE"
  sed -i '/  - pip:/a \      - "--extra-index-url https:\/\/download.pytorch.org\/whl\/cu126"  # Changed to EXTRA-index \& moved to TOP' "$YML_FILE"
  sed -i 's/# Changed to EXTRA-index \& moved to TOP//' "$YML_FILE"
  sed 's/^/      /' "$EXTRA_FILE" >> temp_env.yml

  conda env update -n "$ENV_NAME" -f temp_env.yml --prune

  rm temp_env.yml

  echo "Entorno actualizado."

elif [[ "$ACTION" == "-c" ]]; then
  echo "Creando entorno '$ENV_NAME' con $MODE..."
  conda remove --name "$ENV_NAME" --all -y
  
  cp "$COMMON_FILE" temp_env.yml
  YML_FILE="temp_env.yml"

  sed -i '/- "--index-url https:\/\/download.pytorch.org\/whl\/cu126"/d' "$YML_FILE"
  if [[ "$ACTION" == "-p" ]]; then
    sed -i '/  - pip:/a \      - "--extra-index-url https:\/\/download.pytorch.org\/whl\/cu126"  # Changed to EXTRA-index \& moved to TOP' "$YML_FILE"
    sed -i 's/# Changed to EXTRA-index \& moved to TOP//' "$YML_FILE"
  fi
  sed 's/^/      /' "$EXTRA_FILE" >> temp_env.yml

  conda env create -n "$ENV_NAME" -f temp_env.yml

  rm temp_env.yml

  echo "Entorno creado."
fi

# Eliminar las clausulas prefix
sed -i '/^prefix:/d' "$COMMON_FILE" "$EXTRA_FILE"

