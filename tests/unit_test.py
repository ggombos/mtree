import psycopg2
import os
import heapq

THRESHOLD = 0.0001
KNN_CENTER_POINTS = [3, 8, 10, 23, 45, 56, 67, 87, 99]
KNN_NEIGHBOURS = 10
DELIMITER = ','

def connect_to_database():
    try:
        conn = psycopg2.connect(
            host="localhost",
            port=5432,
            database="postgres",
            user="postgres",
            password="Katona01")

        curs = conn.cursor()
        return curs

    except Exception as e:
        print(e)
        return None


def create_extensions(curs):
    query = f'''CREATE EXTENSION IF NOT EXISTS postgis;
                CREATE EXTENSION IF NOT EXISTS pointcloud;
                CREATE EXTENSION IF NOT EXISTS pointcloud_postgis;
                CREATE EXTENSION IF NOT EXISTS cube;
                CREATE EXTENSION mtree_gist;'''
    curs.execute(query)


def create_table(curs, table_name, point_type):
    query = f'DROP TABLE IF EXISTS public.{table_name};'
    curs.execute(query)

    query = f'''CREATE TABLE public.{table_name} (
        id serial primary key,
        point {point_type}
    );'''
    curs.execute(query)


def read_csv_into_table(curs, file_name, table_name):
    query = f"COPY public.{table_name} (point) FROM '/home/data/mtree/{file_name}' DELIMITER '''' CSV;"
    curs.execute(query)


def create_index(curs, structure, index_name, table_name, type=None):
    if structure == "mtree":
        query = f'DROP INDEX IF EXISTS {index_name};'
        curs.execute(query)

        query = f"""CREATE INDEX {index_name} ON public.{table_name} USING gist (
            point gist_{type}_ops (
                picksplit_strategy    = 'SamplingMinOverlapArea',
                union_strategy        = 'MinMaxDistance'
            )
        );"""
        curs.execute(query)
    
    elif structure == "rtree":
        query = f'DROP INDEX IF EXISTS {index_name};'
        curs.execute(query)

        query = f'CREATE INDEX {index_name} ON public.{table_name} USING gist (point);'
        curs.execute(query)


def knn_test(curs, table_name, center_point_id, neighbour_count):
    query = f"""SELECT c.id, c.point, (c.point <-> (SELECT ic.point FROM public.{table_name} ic WHERE ic.id = {center_point_id})) dist
                FROM public.{table_name} c
                ORDER BY c.point <-> (SELECT ic.point FROM public.{table_name} ic WHERE ic.id = {center_point_id}) LIMIT {neighbour_count};"""
    curs.execute(query)
    return curs.fetchall()


def cleanup(curs, tables, indexes):
    query = ''
    for table_name in tables:
        query += f'DROP TABLE IF EXISTS {table_name};'

    for index_name in indexes:
        query += f'DROP INDEX IF EXISTS {index_name};'

    query += f'''DROP EXTENSION IF EXISTS pointcloud_postgis;
                DROP EXTENSION IF EXISTS pointcloud;
                DROP EXTENSION IF EXISTS postgis;
                DROP EXTENSION IF EXISTS cube;
                DROP EXTENSION IF EXISTS mtree_gist;'''
    curs.execute(query)


def assert_equal(result1, result2, check_id) -> bool:
    if len(result1) != len(result2):
        return False

    for i in range(len(result1)):
        id1 = result1[i][0]
        id2 = result2[i][0]

        if check_id:
            # If the points are equidistant, they may end up being swapped.
            if ((id1 not in [r_id2 for r_id2, _, _ in result2]) or
                (id2 not in [r_id1 for r_id1, _, _ in result1])):
                return False
        
        dist1 = float(result1[i][2])
        dist2 = float(result2[i][2])
        diff = abs(dist1 - dist2)
        if diff > THRESHOLD:
            return False
    
    return True


def text_distance(str1, str2):
    x_axis_len = len(str1) + 1
    y_axis_len = len(str2) + 1
    cost = 1

    m = [[0 for _ in range(y_axis_len)] for _ in range(x_axis_len)]
    for i in range(x_axis_len):
        for j in range(y_axis_len):
            if i == 0:
                m[i][j] = j
            elif j == 0:
                m[i][j] = i
            else:
                
                if str1[i-1] == str2[j-1]:
                    c_val = 0
                else:
                    c_val = cost
                
                m[i][j] = min(m[i-1][j] + cost, m[i][j-1] + cost, m[i-1][j-1] + c_val)

    return float(m[x_axis_len - 1][y_axis_len - 1])


def select_center_point(file, cp):
    with open(file, 'r') as f:
        idx = 1
        line = f.readline()
        while idx < cp:
            idx += 1
            line = f.readline()

        return (idx, line.strip('\n'))


def calculate_knn(file, center):
    neighbours_heap = []
    nearest_neighbours = []

    with open(file, 'r') as f:
        for idx, line in enumerate(f):
            line = line.strip('\n')
            
            if DELIMITER in line:
                texts = line.split(DELIMITER)
                cp_texts = center[1].split(DELIMITER)
                dist = 0.0
                for i in range(len(cp_texts)): # Assume that arrays have the same amount of elements.
                    dist += text_distance(cp_texts[i], texts[i])
            else:
                dist = text_distance(center[1], line)
            
            heapq.heappush(neighbours_heap, (dist, (idx + 1), line))

        for dist, idx, line in heapq.nsmallest(KNN_NEIGHBOURS, neighbours_heap):
            nearest_neighbours.append((idx, line, dist))
    
    return nearest_neighbours


def print_result(result, mtree_res, rtree_res):
    print("✅" if result else "❌")
    if not result:
        print()
        print("\t\tOutput:")
        print("\t\t\tMtree")
        for item in mtree_res:
            print("\t\t\t", item)
        print("\t\t\tRtree")
        for item in rtree_res:
            print("\t\t\t", item)
        print()


def main():
    curs = connect_to_database()
    if curs == None:
        return 1

    types = [name for name in os.listdir('tests') if os.path.isdir(os.path.join('tests', name))]
    final_result = True
    tables = []
    indexes = []

    try:
        create_extensions(curs)
        
        for type in types:
            print("────────────────────────────────")
            print(f"Type: {type}")
            print("────────────────────────────────")
            
            try:
                csv_files = set([f.replace("_cube.csv", "").replace("_mtree.csv", "") for f in os.listdir(f"tests/{type}") if f.endswith('.csv')])
                csv_files = sorted(csv_files, key=lambda x: int(x.split('_')[-1]))
            except:
                print("\tNo such file or directory.")
                continue

            for file in csv_files:
                print(f"\t[{file}]")
                mtree_table = f'{file}_mtree'
                rtree_table = f'{file}_cube'
                mtree_index = f'{file}_mtree_index'
                rtree_index = f'{file}_cube_index'

                if 'text' in type:
                    create_table(curs=curs, table_name=mtree_table, point_type=f"mtree_{type}")
                    read_csv_into_table(curs=curs, file_name=f"tests/{type}/{file}_mtree.csv", table_name=mtree_table)
                    create_index(curs=curs, structure="mtree", index_name=mtree_index, table_name=mtree_table, type=f"mtree_{type}")
                    tables.append(mtree_table)
                    indexes.append(mtree_index)

                    for center_point in KNN_CENTER_POINTS:
                        print(f"\t\tCenter point of KNN is {center_point}:", end="", flush=True)

                        mtree_res = knn_test(curs=curs, table_name=mtree_table, center_point_id=center_point, neighbour_count=KNN_NEIGHBOURS)
                        cp = select_center_point(f"tests/{type}/{file}_mtree.csv", center_point)
                        rtree_res = calculate_knn(f"tests/{type}/{file}_mtree.csv", cp)

                        result = assert_equal(mtree_res, rtree_res, check_id=False)
                        if not result:
                            final_result = False
                        print_result(result, mtree_res, rtree_res)
                
                else:
                    create_table(curs=curs, table_name=mtree_table, point_type=f"mtree_{type}")
                    create_table(curs=curs, table_name=rtree_table, point_type="cube")

                    read_csv_into_table(curs=curs, file_name=f"tests/{type}/{file}_mtree.csv", table_name=mtree_table)
                    read_csv_into_table(curs=curs, file_name=f"tests/{type}/{file}_cube.csv", table_name=rtree_table)

                    create_index(curs=curs, structure="mtree", index_name=mtree_index, table_name=mtree_table, type=f"mtree_{type}")
                    create_index(curs=curs, structure="rtree", index_name=rtree_index, table_name=rtree_table)
                    
                    tables.append(mtree_table)
                    tables.append(rtree_table)
                    indexes.append(mtree_index)
                    indexes.append(rtree_index)

                    for center_point in KNN_CENTER_POINTS:
                        print(f"\t\tCenter point of KNN is {center_point}:", end="", flush=True)

                        mtree_res = knn_test(curs=curs, table_name=mtree_table, center_point_id=center_point, neighbour_count=KNN_NEIGHBOURS)
                        rtree_res = knn_test(curs=curs, table_name=rtree_table, center_point_id=center_point, neighbour_count=KNN_NEIGHBOURS)
                        
                        result = assert_equal(mtree_res, rtree_res, check_id=True)
                        if not result:
                            final_result = False
                        print_result(result, mtree_res, rtree_res)
                    
                print()
        
        cleanup(curs, tables, indexes)

        if final_result:
            return 0
        else:
            return 1

    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    exit(main())