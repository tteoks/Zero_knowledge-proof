#!/usr/bin/python3
from pymysql import connect
from common import *


# Edit these values to your server env
DB_HOST = "127.0.0.1"
DB_USER = "admin"
DB_PASS = "suwon"
DB_NAME = "KCDC"
DB_TABLE = "_PrivateSetY"

if __name__ == "__main__":
    dataset = make_dataset(5, preset_values)

    conn = connect(
        host=DB_HOST, user=DB_USER, password=DB_PASS, db=DB_NAME, charset="utf8"
    )
    INSERT_DB(dataset, conn, DB_TABLE)

    res = SELECT_DB(conn, DB_TABLE)
    conn.close()

    for i in res:
        print(i)
