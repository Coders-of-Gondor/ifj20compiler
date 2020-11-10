 #include <filesystem>
 #include "gtest/gtest.h"

int main(int argc, char **argv) {
  std::filesystem::path test_dir = std::filesystem::current_path().concat("/tests");
  if (!std::filesystem::is_directory(test_dir)) {
    std::cerr << "The tests need to be executed from the root project directory.\n";
    std::cerr << "Aborting now.\n";

    return EXIT_FAILURE;
  }

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
