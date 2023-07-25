FROM gcc:latest
WORKDIR /app
COPY server.c /app/
RUN apt-get update && apt-get install -y libpq-dev
RUN gcc -o server server.c -lpq
CMD ["./server"]
