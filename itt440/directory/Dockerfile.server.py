FROM python:3.9
WORKDIR /app
COPY server.py /app/
RUN pip install psycopg2-binary
CMD ["python", "server.py"]
