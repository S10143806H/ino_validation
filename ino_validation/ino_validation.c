#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "dirent.h"	// https://codeyarns.com/tech/2014-06-06-how-to-use-dirent-h-with-visual-studio.html#gsc.tab=0

#define PRINT_DEBUG 1

/* Declear global vairables */
const char* keywordNN = "modelSelect";
const char* keywordVOE = "configVideoChannel";
const char* keyword_header = "#include";
const char* keyword_customized = "CUSTOMIZED";
const char* keyword_default = "DEFAULT";
const char* keyword_default_backup = "Dbackup";
const char* keyword_customized_backup = "Cbackup";
const char* keyword_bypassNN1 = " .modelSelect";
const char* keyword_bypassNN2 = " modelSelect";
const char* keyword_bypassVOE1 = " .configVideoChannel";
const char* keyword_bypassVOE2 = " configVideoChannel";

const char* filename_txt = "ino_validation.txt";


/* Declear function headers */
int isDirExists(const char* path);
void copyFile(const char* sourcePath, const char* destinationPath);
void copyDirectory(const char* sourcePath, const char* destinationPath);
void deleteDirectory(const char* path);
/* Functions below are for txt file manipulation */
int endsWith(const char* str, const char* suffix);
void writeTXT(const char* example_path);


int main(int argc, char* argv[]) {
	// Check if the number of input arguments is correct 
	if (argc != 3) {
#if PRINT_DEBUG
		printf("[Error] Incorrect number of input parameters. Expected 2 parameters.\r\n");
#endif
		exit(1);
	}
	// Retrieve the input parameters 
	const char* build_path = argv[1];
	const char* tools_path = argv[2];

	/* Declear common file paths */
	char root_path[512];
	char arduino15_path[512];
	char* arduino15_add = "\\AppData\\Local\\Arduino15";

	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(arduino15_add)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_DIR) {
#if PRINT_DEBUG
				printf("[%s] Folder:%s\n", __func__, ent->d_name);
#endif
			}
			if (ent->d_type == DT_REG) {
#if PRINT_DEBUG
				printf("[%s] File:%s\n", __func__, ent->d_name);
#endif
			}
		}
	}

	//char* ambpro2_add = "\\AppData\\Local\\Arduino15";
	strcpy(root_path, getenv("USERPROFILE"));
	strcpy(arduino15_path, getenv("USERPROFILE"));
	strcat(arduino15_path, arduino15_add);


	//strcat(str, bar);
#if PRINT_DEBUG
	printf("USERPROFILE = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE   = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH    = %s\n", getenv("HOMEPATH"));
	printf("root_path   = %s\n", root_path);
	printf("arduino15_path   = %s\n", arduino15_path);
	//printf("sdk_version   = %s\n", arduino15_path);
#endif
	exit(1);

	while (1);

#if PRINT_DEBUG
	// Print the input parameters 
	printf("Parameter 1 toolchain p1 path: %s\n", build_path);
	printf("Parameter 2 toolcahin p2 path: %s\n", tools_path);

#endif

	// check whether directory exists
	if (isDirExists(build_path) && isDirExists(tools_path)) {
		copyDirectory(tools_path, build_path);
	}

	deleteDirectory(tools_path);
	mkdir(tools_path, 0755);

	return 0;
}

/**
*/
int isDirExists(const char* path) {
	DIR* dir;
	struct dirent* ent;
	int count = 0;
	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_DIR) {
				count++;
#if PRINT_DEBUG
				printf("[%s] Folder:%s\n", __func__, ent->d_name);
#endif
			}
			if (ent->d_type == DT_REG) {
				count++;
#if PRINT_DEBUG
				printf("[%s] File:%s\n", __func__, ent->d_name);
#endif
			}
		}
		if (count == 2) {
			/* Empty directory that contains "." and ".." only */
#if PRINT_DEBUG
			printf("[%s][Error] Empty directory found\n", __func__);
#endif
			return 0;
		}
		closedir(dir);
		return 1;
	}
	else if (ENOENT == errno) {
#if PRINT_DEBUG
		printf("[%s][Error] Directory does not exist\n", __func__);
#endif
		return 0;
	}
	else {
		/* opendir() failed for some other reason. */
		return 0;
	}
}

void copyFile(const char* sourcePath, const char* destinationPath) {
	FILE* sourceFile = fopen(sourcePath, "rb");
	FILE* destinationFile = fopen(destinationPath, "wb");

	if (sourceFile == NULL || destinationFile == NULL) {
#if PRINT_DEBUG
		printf("[%s][Error] filed to open file\n", __func__);
#endif
		return;
	}

	char buffer[1024];
	size_t bytesRead;

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0) {
		fwrite(buffer, 1, bytesRead, destinationFile);
	}

	fclose(sourceFile);
	fclose(destinationFile);
#if PRINT_DEBUG
	printf("[%s][Error] file copy done\n", __func__);
#endif
}

void copyDirectory(const char* sourcePath, const char* destinationPath) {
	DIR* sourceDir = opendir(sourcePath);

	if (sourceDir == NULL) {
#if PRINT_DEBUG
		printf("[%s][Error] filed to open file\n", __func__);
#endif
		return;
	}
	struct dirent* entry;

	while ((entry = readdir(sourceDir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		char sourceFilePath[1024];
		char destinationFilePath[1024];
		snprintf(sourceFilePath, sizeof(sourceFilePath), "%s/%s", sourcePath, entry->d_name);
		snprintf(destinationFilePath, sizeof(destinationFilePath), "%s/%s", destinationPath, entry->d_name);

		if (entry->d_type == DT_DIR) {
			mkdir(destinationFilePath, 0755);
			copyDirectory(sourceFilePath, destinationFilePath);
		}
		else {
			copyFile(sourceFilePath, destinationFilePath);
		}
	}

	closedir(sourceDir);
}

void deleteDirectory(const char* path) {
	DIR* dir = opendir(path);
	struct dirent* entry;

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}
		char filePath[1024];
		snprintf(filePath, sizeof(filePath), "%s/%s", path, entry->d_name);
		if (entry->d_type == DT_DIR) {
			deleteDirectory(filePath);
#if PRINT_DEBUG
			printf("[%s][Error] Directory delete!\r\n", __func__);
#endif
		}
		else {
			remove(filePath);
#if PRINT_DEBUG
			printf("[%s][Error] File delete!\r\n", __func__);
#endif
		}
	}
	closedir(dir);
	rmdir(path);
}

int endsWith(const char* str, const char* suffix) {
	size_t str_len = strlen(str);
	size_t suffix_len = strlen(suffix);
	if (suffix_len > str_len) {
		return 0;
	}
	return strncmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

void writeTXT(const char* example_path) {

}