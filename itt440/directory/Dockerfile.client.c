FROM gcc:latest
WORKDIR /app
COPY client.c /app/
RUN gcc -o client client.c
CMD ["./client"]
