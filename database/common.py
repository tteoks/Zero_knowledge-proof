import pymysql.cursors
import random
import string


def make_dataset(num, pre_dataset):
    dataset = []
    temp = []
    num_10 = [10, 100, 1000, 10000]
    len_preset = len(pre_dataset)

    for i in range(len_preset):
        frequency = random.randrange(1, 20)
        amount = random.random() * random.choice(num_10)
        dataset.append((frequency, amount, pre_dataset[i].rstrip()))

    for i in range(num - len_preset):
        frequency = random.randrange(1, 20)
        amount = random.random() * random.choice(num_10)
        value = "".join([random.choice(string.ascii_letters) for _ in range(30)])
        while value in dataset:
            value = "".join([random.choice(string.ascii_letters) for _ in range(30)])
        dataset.append((frequency, amount, value))

    return dataset


def INSERT_DB(dataset, conn, table):
    curs = conn.cursor()

    query = "INSERT INTO " + table + "(frequency, amount, value) VALUES(%s, %s, %s)"
    print(query, dataset)
    for i in range(len(dataset)):
        curs.execute(query, dataset[i])

    conn.commit()


def SELECT_DB(conn, table):
    curs = conn.cursor()

    query = "SELECT * FROM " + table
    curs.execute(query)

    rows = curs.fetchall()

    return rows


# Preset value, these values may be match between client and server inspect
preset_values = [
    "Normal Person 1",
    "Normal Person 2",
    "Normal Person 3",
    "Normal Person 4",
    "Normal Person 5",
    "Normal Person 6",
    "Normal Person 7",
    "Normal Person 8",
    "Normal Person 9",
    "Normal Person 10",
    "Infected Person 1",
    "Infected Person 2",
    "Infected Person 3",
    "Infected Person 4",
    "Infected Person 5",
]
