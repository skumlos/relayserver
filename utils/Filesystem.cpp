#include "Filesystem.h"
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <stddef.h>     /* offsetof */

bool Filesystem::deleteDirectory(std::string path) {
        DIR *dir;
        struct dirent *ent = NULL;
	struct dirent *entryp = NULL;

        if ((dir = opendir(path.c_str())) == NULL) {
                printf("Cannot open dir %s\n",path.c_str());
                return false;
        }

	int name_max = pathconf(path.c_str(), _PC_NAME_MAX);
	if (name_max == -1)         /* Limit not defined, or error */
		name_max = 255;         /* Take a guess */
	int len = offsetof(struct dirent, d_name) + name_max + 1;
	entryp = (struct dirent*)malloc(len);

        while (readdir_r(dir,entryp,&ent) == 0 && ent != NULL) {
                std::string entry( ent->d_name );
                if(entry == "." || entry == "..") continue;
                struct stat st;
                stat(std::string(path+"/"+ent->d_name).c_str(),&st);
                if(st.st_mode & S_IFDIR) {
                        deleteDirectory(std::string(path+"/"+ent->d_name));
                } else {
			remove(std::string(path+"/"+ent->d_name).c_str());
		}
        }

	free(entryp);

	closedir(dir);

	rmdir(path.c_str());

	return true;
}

bool Filesystem::deleteFile(const std::string& path) {
	struct stat s;
	bool ret = false;
	int rc = stat(path.c_str(),&s);
	if(rc == 0) {
		ret = (remove(path.c_str()) == 0);
	}
	return ret;
}

bool Filesystem::doesFileExist(const std::string& path)
{
	struct stat s;
	int rc = stat(path.c_str(),&s);
	return (rc == 0);
}

std::list<std::string> Filesystem::getFiles(const std::string& path) {
        DIR *dir;
        struct dirent *ent = NULL;
	struct dirent *entryp = NULL;

	std::list<std::string> r;

        if ((dir = opendir(path.c_str())) == NULL) {
                printf("Cannot open dir %s\n",path.c_str());
                return r;
        }

	int name_max = pathconf(path.c_str(), _PC_NAME_MAX);
	if (name_max == -1)         /* Limit not defined, or error */
		name_max = 255;         /* Take a guess */
	int len = offsetof(struct dirent, d_name) + name_max + 1;
	entryp = (struct dirent*)malloc(len);

	while (readdir_r(dir,entryp,&ent) == 0 && ent != NULL) {
		std::string entry( ent->d_name );
		if(entry == "." || entry == "..") continue;
		struct stat st;
		stat(std::string(path+"/"+ent->d_name).c_str(),&st);
		if(st.st_mode & S_IFDIR) {
			continue;
		} else {
			r.push_back(std::string(ent->d_name));
		}
        }

	free(entryp);

	closedir(dir);

	return r;
}
