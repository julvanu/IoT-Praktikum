FROM python:3.14-slim-bookworm
WORKDIR /app
COPY requirements.txt /app/requirements.txt
RUN pip install --no-cache-dir -v -r requirements.txt
COPY coarsen_data.py /app/coarsen_data.py
CMD ["python", "/app/coarsen_data.py"]