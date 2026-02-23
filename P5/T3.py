import serial
import time

# Configuración del puerto
puerto = 'COM4'
baudrate = 9600

try:
    ser = serial.Serial(puerto, baudrate, timeout=1)
    time.sleep(2)

    # 'w' abre el archivo para escribir (borra lo anterior)
    with open("datos_imu_excel.txt", "w") as archivo:
        print(f"Archivo creado. Guardando datos de {puerto}...")

        # Paso previo: Texto fijo para comprobar generación correcta
        archivo.write("Prueba de cabecera;AccX;AccY;AccZ;GyrX;GyrY;GyrZ;MagX;MagY;MagZ\n")

        while True:
            if ser.in_waiting > 0:
                # Leer línea del Arduino (Tarea 6 de P1) [cite: 166]
                linea = ser.readline().decode('utf-8').strip()

                if linea:
                    # Sustituimos separadores si es necesario para usar ';'
                    # Si tu Arduino ya envía ';' no hace falta el replace
                    datos_para_excel = linea.replace(",", ";")

                    # Escribir con retorno de carro [cite: 173]
                    archivo.write(datos_para_excel + ";\n")

                    print(f"Registrado: {datos_para_excel}")

except serial.SerialException as e:
    print(f"Error de conexión: {e}")
except KeyboardInterrupt:
    print("\nCaptura finalizada. Fichero guardado.")
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()