#include <boost/filesystem.hpp>
#include <vector>
#include <string>

bool files_in_dir(std::vector<std::string>& out, const char* dir) {
  using namespace boost::filesystem;
  path p (dir);

  // validate checks
  if (!exists(p)) {
    std::cout << p << " not exists" << std::endl;
    return false;
  } else if (!is_directory(p)) {
    std::cout << p << " not a directory" << std::endl;
    return false;
  }

  // get all files
  for (auto file : directory_iterator(p)) {
    out.push_back(file.path().native());
  }

  return true;
}
