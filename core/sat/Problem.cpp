#include "core/sat/Problem.h"

#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/core/Dimacs.h"

namespace {

using namespace std;

void read_clauses_to(
    std::filesystem::path const& path, std::vector<std::vector<int>>& clauses) {
  FILE* f = fopen(path.c_str(), "r");
  IPS_VERIFY_S(f != nullptr, "Failed to read CNF from " << path);
  int c = 0;
  bool neg = false;
  vector<int> cls;
  while (c != EOF) {
    c = fgetc(f);
    if (c == 'c' || c == 'p') {
      while (c != '\n') {
        c = fgetc(f);
      }
      continue;
    }
    if (isspace(c)) {
      continue;
    }
    if (c == '-') {
      neg = true;
      continue;
    }
    if (isdigit(c)) {
      int num = c - '0';
      c = fgetc(f);
      while (isdigit(c)) {
        num = num * 10 + (c - '0');
        c = fgetc(f);
      }
      if (neg) {
        num *= -1;
      }
      neg = false;
      if (num != 0) {
        cls.push_back(num);
      } else {
        clauses.push_back(vector<int>(cls));
        cls.clear();
      }
    }
  }
  fclose(f);
}

}  // namespace

namespace core::sat {

Problem::Problem(std::filesystem::path const& path) : _path(path) {}

std::filesystem::path const& Problem::path() const noexcept {
  return _path;
}

std::vector<std::vector<int>> const& Problem::clauses() const noexcept {
  if (_clauses.empty()) {
    std::lock_guard<std::mutex> lg(_load_mutex);
    if (_clauses.empty()) {
      read_clauses_to(path(), _clauses);
    }
  }
  return _clauses;
}

}  // namespace core::sat