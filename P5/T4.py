import serial
import time
import numpy as np
import matplotlib

# Forzamos el backend antes de importar pyplot
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# 1. Configuración del puerto serie
try:
    ser = serial.Serial('COM4', 9600, timeout=0.1)
    time.sleep(2)
    print("Conexión establecida en COM4. Mostrando gráfica...")
except Exception as e:
    print(f"Error al abrir el puerto: {e}")
    exit()

# 2. Buffers para 5 segundos (50 muestras)
max_muestras = 50
datos_x = [0.0] * max_muestras
datos_y = [0.0] * max_muestras
datos_z = [0.0] * max_muestras

# 3. Configuración de la gráfica
plt.style.use('bmh')
fig, ax = plt.subplots(figsize=(10, 6))
linea_x, = ax.plot(datos_x, label='Acc X', color='red', linewidth=1.5)
linea_y, = ax.plot(datos_y, label='Acc Y', color='green', linewidth=1.5)
linea_z, = ax.plot(datos_z, label='Acc Z', color='blue', linewidth=1.5)

ax.set_ylim(-2, 2)
ax.set_title("Muestreo IMU en Tiempo Real - Ventana 5s")
ax.set_ylabel("Aceleración (g)")
ax.legend(loc='upper right')


def update(frame):
    global datos_x, datos_y, datos_z

    while ser.in_waiting > 0:
        try:
            linea = ser.readline().decode('utf-8').strip()
            if not linea: continue

            valores = linea.split(';')
            if len(valores) >= 3:
                # Insertar nuevos datos y eliminar antiguos
                datos_x.append(float(valores[0]))
                datos_y.append(float(valores[1]))
                datos_z.append(float(valores[2]))

                datos_x.pop(0)
                datos_y.pop(0)
                datos_z.pop(0)

                # Reporte estadístico cada 10 frames
                if frame % 10 == 0:
                    prom = np.mean(datos_x)
                    desv = np.std(datos_x)
                    print(f"Estadísticas X -> Promedio: {prom:.3f} | Desviación: {desv:.3f}")

        except:
            pass

    linea_x.set_ydata(datos_x)
    linea_y.set_ydata(datos_y)
    linea_z.set_ydata(datos_z)

    return linea_x, linea_y, linea_z


# 4. Ejecución
ani = FuncAnimation(fig, update, interval=100, blit=True)

# plt.show() con ventana persistente
plt.show()

# Al cerrar la ventana manual se cierra el puerto
ser.close()
print("Puerto serie liberado correctamente.")