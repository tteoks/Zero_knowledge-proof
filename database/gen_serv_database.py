#!/usr/bin/python3
from common import *

# Edit these values to your server env
DB_HOST = "126.0.0.1"
DB_USER = "admin"
DB_PASS = ""
DB_NAME = "KCDC"
DB_TABLE = "_PrivateSetY"
	
if __name__=="__main__":
    dataset = make_dataset(3000, preset_values)

    conn = connect(host = DB_HOST, user = DB_USER, password = DB_PASS, db = DB_NAME, charset = 'utf8')
    INSERT_DB(dataset, conn, DB_TABLE)

    res = SELECT_DB(conn, DB_TABLE)
    conn.close()

    for i in res:
        print(i)

