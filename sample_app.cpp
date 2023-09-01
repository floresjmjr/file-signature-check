#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <botan/hash.h>
#include <botan/hex.h>

// Checks to see if the file signature matches that of the jpg or pdf
std::string checkFileSignature(const std::filesystem::path &path)
{
  std::ifstream inputFile(path, std::ios::binary);
  if (!inputFile.is_open())
  {
    std::cout << "Couldn't open the file: " << path << std::endl;
    return "";
  }

  char magic[0];
  inputFile.read(magic, 4);

  std::ostringstream ss;
  ss << std::hex << std::setfill('0');
  for (int i = 0; i < 4; i++)
  {
    ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(magic[i]));
  }

  std::string signature = ss.str();

  const std::string JPG_SIGNATURE = "ffd8";
  const std::string PDF_SIGNATURE = "25504446";

  if (signature == PDF_SIGNATURE)
  {
    return "PDF";
  }
  else if (signature.starts_with(JPG_SIGNATURE))
  {
    return "JPG";
  }
  else
  {
    return "";
  }
}

// Generates a MD5 hash based on the contents of the file.
std::string calculateMD5(const std::filesystem::path &filename)
{
  std::ifstream file(filename, std::ios::binary);
  if (!file)
  {
    std::cout << "Couldn't open the file" << std::endl;
  }

  std::unique_ptr<Botan::HashFunction> hashFunction(Botan::HashFunction::create("MD5"));

  std::vector<unsigned char> buffer(4096);
  while (file)
  {
    file.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
    std::streamsize bytesRead = file.gcount();
    if (bytesRead > 0)
    {
      hashFunction->update(buffer.data(), bytesRead);
    }
  }

  Botan::secure_vector<uint8_t> hash = hashFunction->final();
  std::string hexHash = Botan::hex_encode(hash);

  return hexHash;
}

// Creates an entry in the output file with the following: Full Path, File Type, MD5 Hash
void processFile(const std::filesystem::path &filePath, std::ofstream &outputFile)
{
  std::string fileType = checkFileSignature(filePath);
  if (fileType == "PDF" || fileType == "JPG")
  {
    try
    {
      std::string hash = calculateMD5(filePath);
      outputFile << std::endl
                 << filePath << "," << fileType << "," << hash;
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
    }
  }
}

// Iterates over the files within a directory or the files in both the current and subdirectories to process them.
void processFiles(const std::string &analyzedDir, bool includeSubdir, std::string &outputPath)
{
  std::ofstream outputFile(outputPath);
  outputFile << "File Path,File Type,MD5 Hash";

  if (includeSubdir)
  {
    for (const std::filesystem::directory_entry &file : std::filesystem::recursive_directory_iterator(analyzedDir))
    {
      if (!file.is_directory())
      {
        processFile(file.path(), outputFile);
      }
    }
  }
  else
  {
    for (const std::filesystem::directory_entry &file : std::filesystem::directory_iterator(analyzedDir))
    {
      if (!file.is_directory())
      {
        processFile(file.path(), outputFile);
      }
    }
  }
  outputFile.close();
}

struct Arguments
{
  std::string analyzedDir;
  std::string outputPath;
  bool includeSubdir;
};

// Assigns and Validates the command line arguments.
Arguments processArguments(int argc, char *argv[])
{

  if (argc != 4)
  {
    throw std::runtime_error("Please provide a analyzedDir, a output path, and a flag (-s) as command line arguments, in that order.");
  }

  std::string analyzedDir = argv[1];
  std::string outputPath = argv[2];
  if (outputPath.find('.') == std::string::npos)
  {
    throw std::runtime_error("Please make sure to include the output file and its extension in the file path.");
  }

  std::string flag = argv[3];

  bool includeSubdir = false;
  if (flag == "-s")
  {
    includeSubdir = true;
  }
  else if (flag.find('=') == std::string::npos)
  {
    throw std::runtime_error("Please provide a flag to indicate whether to include subdirectories or not.");
  }
  else
  {
    std::string flagValue = flag.substr(flag.find('=') + 1);

    for (char &c : flagValue)
    {
      c = std::tolower(c);
    }

    if ((flagValue == "t") || (flagValue == "true"))
    {
      includeSubdir = true;
    }
  }

  Arguments args;
  args.analyzedDir = analyzedDir;
  args.outputPath = outputPath;
  args.includeSubdir = includeSubdir;

  return args;
}

int main(int argc, char *argv[])
{
  // takes in the following args in this order,
  // - a the full directory path that will be processed
  // - a path for the output file that will contain the entries from the processed files
  // - a required flag ( -s=<value> ) that accepts 't' or 'true' else always false, that decides whether the processing will be done recursively.
  Arguments inputArgs = processArguments(argc, argv);
  processFiles(inputArgs.analyzedDir, inputArgs.includeSubdir, inputArgs.outputPath);

  return 0;
}