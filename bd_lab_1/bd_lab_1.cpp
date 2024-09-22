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
				//вывод шапочки (Hr) - имен атрибутов!
				num_fields = mysql_num_fields(res);
				for (int i = 0; i < num_fields; i++) {
					field = mysql_fetch_field(res);
					std::cout << field->name << "\t| ";
				}
				std::cout << std::endl;

				//вывод тела (Br) - кортежей!
				num_rows = mysql_num_rows(res);
				for (int j = 0; j < num_rows; j++) {
					row = mysql_fetch_row(res);
					for (int i = 0; i < num_fields; i++) {
						std::cout << row[i] << "\t| ";
					}
					std::cout << std::endl;
				}
			}
			std::getline(std::cin, query);
		}
		mysql_close(conn);
	}
	else {
		std::cerr << "Connection to database has failed: " << mysql_error(conn) << std::endl;
		mysql_close(conn);
		return 1;
	}
}