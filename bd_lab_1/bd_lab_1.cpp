#include <iostream>
#include <string>
#include <mysql.h>

MYSQL* conn;
MYSQL_FIELD* field;
MYSQL_RES* res;
MYSQL_ROW row;
std::string query;
int num_fields;
int num_rows;
int* LensFields = nullptr;

int main()
{
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "127.0.0.1", "root", "", "lab_1", 3306, NULL, 0);

	if (conn) {
		std::cout << "successful connection to database" << std::endl;
		std::cout << "enter your query:" << std::endl;
		std::getline(std::cin, query);
		while (query != "Maxim") {
			if (mysql_query(conn, query.c_str())) {
				std::cerr << "Query error: " << mysql_error(conn) << std::endl;
				std::getline(std::cin, query);
				continue;
			}
			if (query.compare(0, 6, "SELECT") != 0) {
				std::cout << "Done!" << std::endl;
			}
			else {
				res = mysql_store_result(conn);
				if (!res) {
					std::cerr << "Result storrage error: " << mysql_error(conn) << std::endl;
					std::getline(std::cin, query);
					continue;
				}

				//получим максимальные длины строк каждого атрибута, чтобы вывести таблицу красиво - создадим массив с длинами для каждого столбца
				//LensFields[i] - это максимальная длина строки i-го атрибута

				num_fields = mysql_num_fields(res);
				int* LensFields = new int[num_fields];
				for (int i = 0; i < num_fields; i++) {
					field = mysql_fetch_field(res);
					LensFields[i] = strlen((field->name));
				}
				num_rows = mysql_num_rows(res);
				for (int j = 0; j < num_rows; j++) {
					row = mysql_fetch_row(res);
					for (int i = 0; i < num_fields; i++) {
						if (strlen(row[i]) > LensFields[i]) {
							LensFields[i] = strlen(row[i]);
						}
					}
				}
				
				res = nullptr;
				field = nullptr;
				row = nullptr;
				mysql_query(conn, query.c_str());
				res = mysql_store_result(conn);

				//вывод шапочки (Hr) - имен атрибутов!
				
				for (int i = 0; i < num_fields; i++) {
					field = mysql_fetch_field(res);
					std::cout << field->name << " ";
					for (int j = 0; j < LensFields[i] - strlen(field->name); j++) {
						std::cout << " ";
					}
					std::cout << "|";
				}
				std::cout << std::endl;

				//вывод тела (Br) - кортежей!
				for (int k = 0; k < num_rows; k++) {
					row = mysql_fetch_row(res);
					for (int i = 0; i < num_fields; i++) {
						std::cout << row[i] << " ";
						for (int j = 0; j < LensFields[i] - strlen(row[i]); j++) {
							std::cout << " ";
						}
						std::cout << "|";
					}
					std::cout << std::endl;
				}
			}
			std::cout << "enter query:" << std::endl;
			std::getline(std::cin, query);
		}
		mysql_close(conn);
	}
	else {
		std::cerr << "Connection to database has failed: " << mysql_error(conn) << std::endl;
		mysql_close(conn);
		return 1;
	}
	delete[] LensFields;
}