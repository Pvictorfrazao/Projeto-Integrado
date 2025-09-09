import serial
import psycopg2

ser = serial.Serial('COM3', 9600, timeout=1)

db = psycopg2.connect(
    host="localhost",
    user="postgres",
    password="2508",
    dbname="Aqualert"
)

cursor = db.cursor()

while True:
    linha = ser.readline().decode(errors="ignore").strip()
    if linha.startswith("VEL:"):
        try:
            velocidade = float(linha.replace("VEL:", ""))
            cursor.execute(
                "INSERT INTO leituras (velocidade, data_hora) VALUES (%s, NOW())",
                (velocidade,)
            )
            db.commit()
            print(f"Salvo: {velocidade}")
        except ValueError:
            print("Linha inválida:", linha)