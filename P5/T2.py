import serial
import time

# Configuración del puerto - Asegúrate de que el Monitor de Arduino esté cerrado
try:
    ser = serial.Serial('COM4', 9600, timeout=1)
    time.sleep(2)  # Espera a que la conexión se estabilice
    print("Leyendo datos del puerto serie... (Presiona Ctrl+C para detener)")

    while True:
        # El método correcto es in_waiting, no in_available
        if ser.in_waiting > 0:
            # Lee la línea, la decodifica y elimina espacios/saltos de línea
            linea = ser.readline().decode('utf-8').strip()
            if linea:
                print(f"Datos recibidos: {linea}")

except serial.SerialException as e:
    print(f"Error: No se pudo abrir el puerto. {e}")
except KeyboardInterrupt:
    print("\nLectura detenida por el usuario.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Puerto cerrado.")