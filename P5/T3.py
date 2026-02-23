import serial
import time

try:
    ser = serial.Serial('COM4', 9600, timeout=1)
    time.sleep(2)

    with open("datos_sensores.txt", "w") as archivo:
        # Cabecera limpia
        archivo.write("AccX;AccY;AccZ;GyrX;GyrY;GyrZ;MagX;MagY;MagZ\n")
        print("Capturando datos puros... Presiona Ctrl+C para parar.")

        while True:
            if ser.in_waiting > 0:
                linea = ser.readline().decode('utf-8').strip()
                if linea:
                    # Guardamos la línea tal cual viene del Arduino (ya trae los ';')
                    archivo.write(linea + "\n")
                    print(linea)

except KeyboardInterrupt:
    print("\nGuardado finalizado.")
finally:
    if 'ser' in locals(): ser.close()