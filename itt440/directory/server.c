#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdbool.h>
#include <postgresql/libpq-fe.h>
#include <unistd.h>

#define DB_HOST "192.168.80.2"
#define DB_NAME "mydatabase"
#define DB_USER "myuser"
#define DB_PASSWORD "mypassword"
#define DB_PORT "5432"

void create_table() {
    PGconn *conn = PQconnectdb("host=" DB_HOST " dbname=" DB_NAME " user=" DB_USER " password=" DB_PASSWORD " port=" DB_PORT);
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }
    const char *sql = "CREATE TABLE IF NOT EXISTS user_data (user TEXT PRIMARY KEY, points INTEGER, datetime_stamp TIMESTAMP)";
    PGresult *res = PQexec(conn, sql);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "SQL error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }
    PQclear(res);
    PQfinish(conn);
}

void update_user_data(const char *user, int points) {
    PGconn *conn = PQconnectdb("host=" DB_HOST " dbname=" DB_NAME " user=" DB_USER " password=" DB_PASSWORD " port=" DB_PORT);
    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    const char *paramValues[3];
    paramValues[0] = user;
    char points_str[12];
    snprintf(points_str, sizeof(points_str), "%d", points);
    paramValues[1] = points_str;
    char timestamp_str[12];
    snprintf(timestamp_str, sizeof(timestamp_str), "%ld", (long)time(NULL));
    paramValues[2] = timestamp_str;

    const char *sql = "INSERT INTO user_data (user, points, datetime_stamp) VALUES ($1, $2, to_timestamp($3)) "
                      "ON CONFLICT (user) DO UPDATE SET points = EXCLUDED.points, datetime_stamp = EXCLUDED.datetime_stamp";
    PGresult *res = PQexecParams(conn, sql, 3, NULL, paramValues, NULL, NULL, 0);
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        fprintf(stderr, "SQL error: %s\n", PQerrorMessage(conn));
    }
    PQclear(res);
    PQfinish(conn);
}

int main() {
    create_table();
    // Server configuration
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    // Socket setup
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(16001);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("C Server listening on port 16001\n");
    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char buffer[1024] = {0};
        valread = read(new_socket, buffer, 1024);
        char *user = strtok(buffer, ":");
        char *points_str = strtok(NULL, ":");
        int current_points = 0;
        if (points_str != NULL) {
            current_points = atoi(points_str);
        }
        current_points++;
        update_user_data(user, current_points);
        char response[1024];
        snprintf(response, sizeof(response), "%d", current_points);
        send(new_socket, response, strlen(response), 0);
        close(new_socket);
    }
    return 0;
}
