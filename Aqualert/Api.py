from flask import Flask, request, jsonify
import psycopg2
from credenciais import DB_CONFIG
from psycopg2 import sql

app = Flask(__name__)

# Rota para verificar se a API está online
@app.route("/", methods=["GET"])
def status():
    return jsonify({"status": "online"}), 200

@app.route("/add_leitura", methods=["POST"])
def add_leitura():
    try:
        data = request.get_json()
        velocidade = data.get("velocidade")

        with psycopg2.connect(**DB_CONFIG) as conn:
            with conn.cursor() as cur:
                query = sql.SQL("INSERT INTO leituras (velocidade) VALUES (%s)")
                cur.execute(query, (velocidade,))
                conn.commit()

        return jsonify({"status": "ok", "velocidade": velocidade}), 201

    except Exception as e:
        return jsonify({"status": "erro", "mensagem": str(e)}), 500


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
