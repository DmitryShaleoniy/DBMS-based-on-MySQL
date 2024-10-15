#include <iostream>
#include <string>
#include <mysql.h>
#include <algorithm>

MYSQL* conn;
MYSQL_FIELD* field;
MYSQL_RES* res;
MYSQL_ROW row;
std::string query;
std::string product_name;
std::string database_name;
int num_fields;
int num_rows;
int* LensFields = nullptr;

std::string ToUpper(std::string query) {
	std::transform(query.begin(), query.end(), query.begin(), ::towupper);
	return query;
}

int main()
{
	std::cout << "Enter database name: " << std::endl;
	std::getline(std::cin, database_name);
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "127.0.0.1", "root", "", database_name.c_str(), 3306, NULL, 0);

	if (conn) {
		std::cout << "successful connection to database" << std::endl;
		std::cout << "enter your query.\nenter \"availability\" to check avability.\nenter \"daily product\" to set product of the day:" << std::endl;
		std::getline(std::cin, query);
		while (query != "Maxim") {
			if (query == "availability") {
				query = "SELECT menu_n.dish_name, CASE WHEN MIN(CASE WHEN warehouse_n.quantity >= prod_n.quantity THEN 1 ELSE 0 END) = 1 THEN \"AVAILABLE\" ELSE \"NOT AVAILABLE\" END AS availability FROM menu_n JOIN prod_n ON menu_n.dish_name = prod_n.dish_name JOIN warehouse_n ON warehouse_n.prod_name = prod_n.prod_name GROUP BY menu_n.dish_name ORDER BY menu_n.dish_name;";
				//query = "SELECT dish_name, CASE WHEN MIN(CASE WHEN warehouse.quantity >= prod.quantity THEN 1 ELSE 0 END) = 1 THEN \"AVAILABLE\" ELSE \"NOT AVAILABLE\" END AS availability FROM menu JOIN prod ON menu.dish_id = prod.dish_id JOIN warehouse ON warehouse.prod_id = prod.prod_id GROUP BY menu.dish_name ORDER BY dish_name;";
			}
			if (query == "daily product") {
				std::cout << "enter product name: " << std::endl;
				std::getline(std::cin, product_name);
				query = "SELECT j_table.dish_name, CONCAT(j_table.price, \" -> \", (j_table.price * 0.75)) AS new_price, j_table.availability FROM (SELECT menu_n.dish_name, menu_n.price, menu_n.category, CASE WHEN MIN(CASE WHEN warehouse_n.quantity >= prod_n.quantity THEN 1 ELSE 0 END) = 1 THEN \"AVAILABLE\" ELSE \"NOT AVAILABLE\" END AS availability FROM menu_n JOIN prod_n ON menu_n.dish_name = prod_n.dish_name JOIN warehouse_n ON warehouse_n.prod_name = prod_n.prod_name GROUP BY menu_n.dish_name) AS j_table JOIN (SELECT menu_n.dish_name, warehouse_n.prod_name FROM menu_n JOIN prod_n ON menu_n.dish_name = prod_n.dish_name JOIN warehouse_n ON prod_n.prod_name = warehouse_n.prod_name WHERE warehouse_n.prod_name = \"" + product_name + "\") AS a_table ON a_table.dish_name = j_table.dish_name;";
				//query = "SELECT j_table.dish_name, CONCAT(j_table.price, \" -> \", (j_table.price * 0.75)) AS new_price, j_table.availability FROM (SELECT dish_name, price, category, CASE WHEN MIN(CASE WHEN warehouse.quantity >= prod.quantity THEN 1 ELSE 0 END) = 1 THEN \"AVAILABLE\" ELSE \"NOT AVAILABLE\" END AS availability FROM menu JOIN prod ON menu.dish_id = prod.dish_id JOIN warehouse ON warehouse.prod_id = prod.prod_id GROUP BY menu.dish_name) AS j_table JOIN (SELECT menu.dish_name, warehouse.prod_name FROM menu JOIN prod ON menu.dish_id = prod.dish_id JOIN warehouse ON prod.prod_id = warehouse.prod_id WHERE warehouse.prod_name = \"" + product_name + "\") AS a_table ON a_table.dish_name = j_table.dish_name;";
				//query = "SELECT dish_name, price, category, CASE WHEN MIN(CASE WHEN warehouse.quantity >= prod.quantity THEN 1 ELSE 0 END) = 1 THEN \"AVAILABLE\" ELSE \"NOT AVAILABLE\" END AS availability FROM menu JOIN prod ON menu.dish_id = prod.dish_id JOIN warehouse ON warehouse.prod_id = prod.prod_id WHERE warehouse.prod_name = \"" + product_name + "\" GROUP BY menu.dish_name;";
			}
			if (mysql_query(conn, query.c_str())) {
				std::cerr << "Query error: " << mysql_error(conn) << std::endl;
				std::getline(std::cin, query);
				continue;
			}
			if (ToUpper(query).compare(0, 6, "SELECT") != 0) {
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
						if (row[i] == nullptr) {
							if (4 > LensFields[i]) {
								LensFields[i] = 4;
							}
						}
						else if (strlen(row[i]) > LensFields[i]) {
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
				std::cout << std::endl;

				for (int i = 0; i < num_fields; i++) {
					field = mysql_fetch_field(res);
					std::cout << field->name << " ";
					for (int j = 0; j < LensFields[i] - strlen(field->name); j++) {
						std::cout << " ";
					}
					std::cout << "|";
				}
				std::cout << std::endl;

				for (int i = 0; i < num_fields; i++) {
					for (int j = 0; j <= LensFields[i]; j++) {
						std::cout << "-";
					}
					std::cout << "+";
				}
				std::cout << std::endl;


				//вывод тела (Br) - кортежей!
				for (int k = 0; k < num_rows; k++) {
					row = mysql_fetch_row(res);
					
					for (int i = 0; i < num_fields; i++) {
						if (row[i] == nullptr) {
							std::cout << "NULL" << " ";
							for (int j = 0; j < LensFields[i] - 4; j++) {
								std::cout << " ";
							}
						}
						else {
							std::cout << row[i] << " ";
							for (int j = 0; j < LensFields[i] - strlen(row[i]); j++) {
								std::cout << " ";
							}
						}
						std::cout << "|";
					}
					std::cout << std::endl;
				}
			}
			std::cout << "\nenter query:" << std::endl;
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