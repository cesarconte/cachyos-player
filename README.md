# CachyOS Player 🎬

Reproductor multimedia de ultra alta fidelidad para **CachyOS** y **KDE Plasma 6**, impulsado por el motor nativo **libmpv** y diseñado con los componentes y la estética macOS Golden Gate de **CachyOS.UI**.

## ✨ Características Principales

- **Motor libmpv Puro:** Acceso sin restricciones a todo el ecosistema de decodificación de `mpv` sin capas intermedias degradantes.
- **Pipeline de Máxima Fidelidad (Zero-Copy):**
  - Renderizado acelerado por hardware con **Vulkan** (`vo=gpu-next`, `gpu-api=vulkan`) y fallback OpenGL.
  - Decodificación hardware VAAPI nativa para Intel/AMD/Nvidia.
  - Perfil `high-quality` con algoritmos de reescalado EWA Lanczos y dithering dinámico.
  - Audio Bit-Perfect integrado con **PipeWire** (`ao=pipewire`).
- **Diseño CachyOS.UI & macOS Golden Gate:**
  - Ventana `CachyWindow` translúcida con bordes especulares y radio de curvatura de 20px.
  - Cabecera de 44px con semáforos integrados (`CachyTrafficLights`) que se auto-ocultan suavemente.
  - Controles de transporte flotantes tipo cápsula estilo QuickTime / uosc.
  - Popovers y menús flotantes para selección de pistas de audio, subtítulos, capítulos y velocidad.
- **Preferencias Gráficas Integradas:**
  - Panel completo de configuración sin necesidad de editar archivos `.conf` manualmente.
  - Ajustes de motor, escalado de vídeo, salida de audio, subtítulos y atajos de teclado.
- **Extensiones Nativas:**
  - **Autoload:** Carga automática y secuencial de archivos en el mismo directorio.
  - **Coverart:** Visualización de carátulas embebidas y locales para archivos de música.
  - **Autosub / Autosubsync:** Búsqueda y sincronización de subtítulos integrada.

## 🛠️ Compilación e Instalación

> **Regla de instalación:** Exclusivamente en `/usr` para el ecosistema CachyOS.

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j$(nproc)
sudo -n cmake --install build --prefix /usr
```
