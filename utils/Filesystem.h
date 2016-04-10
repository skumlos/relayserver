#include <string>
#include <list>

namespace Filesystem {
	bool deleteDirectory(std::string path);
	bool deleteFile(const std::string& path);
	bool doesFileExist(const std::string& path);
	std::list<std::string> getFiles(const std::string& path);
}
