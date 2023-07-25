import os
import socket

def main():
    host = os.getenv("SERVER_HOST", "server-python")
    port = int(os.getenv("SERVER_PORT", "15001"))
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))

    try:
        user = input("Enter user name: ")
        while True:
            client_socket.sendall(f"{user}:0".encode('utf-8'))
            data = client_socket.recv(1024).decode('utf-8')
            print(f"User: {user}, Points: {data}")
    except KeyboardInterrupt:
        print("Client stopped.")
    except Exception as e:
        print("Error:", e)
    finally:
        client_socket.close()

if __name__ == "__main__":
    main()
