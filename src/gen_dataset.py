#!/usr/bin/python3

import pymysql.cursors
import random
import string

DB_HOST = "127.0.0.1"
DB_USER = "admin"
DB_PASS = "suwon"
DB_NAME = "airline"
DB_TABLE = "_PrivateSetX"

def make_dataset(num):
	dataset = []
	temp = []
	num_10 = [10, 100, 1000, 10000]
	for i in range(num):
		frequnecy = random.randrange(1, 20)
		amount = random.random() * random.choice(num_10)
		value = "".join([random.choice(string.ascii_letters) for _ in range(30)])
		while value in temp:
			value = "".join([random.choice(string.ascii_letters) for _ in range(30)])
		
		temp.append(value)
		dataset.append((frequnecy, amount, value))
	
	return dataset
	

def INSERT_DB(dataset):
	conn = pymysql.connect(
						host = DB_HOST,
						user = DB_USER,
						password = DB_PASS,
						db = DB_NAME, charset = 'utf8')
	curs = conn.cursor()

	query = "INSERT INTO "+DB_TABLE+"(frequency, amount, value) VALUES(%s, %s, %s)"
	for i in range(len(dataset)):
		curs.execute(query, dataset[i])
	
	conn.commit()
	conn.close()
		

def SELECT_DB():
	conn = pymysql.connect(
						host=DB_HOST, 
						user=DB_USER, 
						password=DB_PASS, 
						db=DB_NAME, charset = 'utf8')

	curs = conn.cursor()

	query = "SELECT Value FROM " + DB_TABLE
	curs.execute(query)
	
	rows = curs.fetchall()

	conn.close()
	return rows


if __name__=="__main__":

    dataset = make_dataset(3000)
#	INSERT_DB(dataset)
    
    res = SELECT_DB()
    result = ""
    for i in range(len(res)):
        result+= "'%s',\n"%res[i]
    print(result)
