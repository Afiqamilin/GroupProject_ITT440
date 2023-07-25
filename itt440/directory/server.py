import socket
import psycopg2
import time
import os

def create_table():
    try:
        conn = psycopg2.connect(
            host=os.environ.get("DB_HOST"),
            database=os.environ.get("DB_NAME"),
            user=os.environ.get("DB_USER"),
            password=os.environ.get("DB_PASSWORD"),
            port=os.environ.get("DB_PORT")
        )
        c = conn.cursor()
        c.execute('''CREATE TABLE IF NOT EXISTS user_data
                     (user TEXT, points INTEGER, datetime_stamp TIMESTAMP)''')
        conn.commit()
    except psycopg2.Error as e:
        print("Error creating table:", e)
    finally:
        if conn:
            conn.close()

def update_user_data(user, points):
    try:
        conn = psycopg2.connect(
            host=os.environ.get("DB_HOST"),
            database=os.environ.get("DB_NAME"),
            user=os.environ.get("DB_USER"),
            password=os.environ.get("DB_PASSWORD"),
            port=os.environ.get("DB_PORT")
        )
        c = conn.cursor()
        c.execute('''INSERT INTO user_data (user, points, datetime_stamp)
                     VALUES (%s, %s, %s)
                     ON CONFLICT (user) DO UPDATE
                     SET points = EXCLUDED.points, datetime_stamp = EXCLUDED.datetime_stamp''',
                  (user, points, int(time.time())))
        conn.commit()
    except psycopg2.Error as e:
        print("Error updating user data:", e)
    finally:
        if conn:
            conn.close()

def main():
    create_table()
    # Server configuration
    host = '0.0.0.0'
    port = 15001
    backlog = 5
    # Socket setup
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(backlog)
    print(f"Python Server listening on {host}:{port}")
    while True:
        try:
            conn, address = server_socket.accept()
            print(f"Connection from {address}")
            data = conn.recv(1024).decode('utf-8')
            user, points = data.split(':')
            current_points = 0
            if points.isdigit():
                current_points = int(points)
            current_points += 1
            update_user_data(user, current_points)
            conn.sendall(str(current_points).encode('utf-8'))
            conn.close()
        except Exception as e:
            print("Error handling client request:", e)

if __name__ == "__main__":
    main()
