FROM python:3.9
WORKDIR /app
COPY client.py /app/
CMD ["python", "client.py"]
