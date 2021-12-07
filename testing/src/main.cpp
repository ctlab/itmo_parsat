#include <iostream>

#include <pqxx/pqxx>

int main(int argc, char** argv) {
  try {
    // Connect to the database.
    pqxx::connection C;
    std::cout << "Connected to " << C.dbname() << '\n';

    // Start a transaction.
    pqxx::work W{C};

    // Perform a query and retrieve all results.
    pqxx::result R{W.exec("SELECT name FROM employee")};

    // Iterate over results.
    std::cout << "Found " << R.size() << "employees:\n";
    for (auto row: R)
      std::cout << row[0].c_str() << '\n';

    // Perform a query and check that it returns no result.
    std::cout << "Doubling all employees' salaries...\n";
    W.exec0("UPDATE employee SET salary = salary*2");

    // Commit the transaction.
    std::cout << "Making changes definite: ";
    W.commit();
    std::cout << "OK.\n";
  } catch (std::exception const &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
  return 0;
}
