import os
import psycopg2

conn = psycopg2.connect(
    host="db",
    dbname=os.environ["POSTGRES_DB"],
    user=os.environ["POSTGRES_USER"],
    password=os.environ["POSTGRES_PASSWORD"]
)

cur = conn.cursor()
cur.execute("SELECT version();")
version = cur.fetchone()
print("DBVER", version)

cur.close()
conn.close()
