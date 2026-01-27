#!/bin/bash

# Ruta origen (tu proyecto STM32)
<<<<<<< HEAD
DESTINO="/home/leo/Documentos/DAC-IO/stm32/DAC-IO-STM32
"
=======
DESTINO="/home/leo/Documentos/DAC-IO/stm32/DAC-IO-STM32"
>>>>>>> 5dde6223d7ae7ed3d4b7384dd3cb06522d3aa4b0

# Ruta destino (tu carpeta dentro del proyecto)
ORIGEN="/home/leo/STM32CubeIDE/workspace_1.15.0/CANBUSTESTING"

# Crear destino si no existe
mkdir -p "$DESTINO"

# Copiar archivos clave
cp -v "$ORIGEN"/*.ioc "$DESTINO" 2>/dev/null
cp -v "$ORIGEN"/.project "$DESTINO" 2>/dev/null
cp -v "$ORIGEN"/.cproject "$DESTINO" 2>/dev/null
cp -v "$ORIGEN"/.stm32cubemx "$DESTINO" 2>/dev/null

# # Copiar carpetas clave
cp -rv "$ORIGEN"/Core "$DESTINO" 2>/dev/null
# cp -rv "$ORIGEN"/Drivers "$DESTINO" 2>/dev/null
# cp -rv "$ORIGEN"/Middlewares "$DESTINO" 2>/dev/null

echo "Copia completa. Revisa la carpeta destino."
