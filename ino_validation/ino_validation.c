#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "dirent.h"	// https://codeyarns.com/tech/2014-06-06-how-to-use-dirent-h-with-visual-studio.html#gsc.tab=0
#include "cJSON.h"

#define PRINT_DEBUG 1
#define MAX_PATH_LENGTH 1024

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
//usrmodel_path = os.path.abspath(os.path.dirname(sys.argv[0]))

/* Declear function headers */
int dirExists(const char* directory_path);
const char* dirName(const char* directory_path);


int isDirExists(const char* path);
void copyFile(const char* sourcePath, const char* destinationPath);
void copyDirectory(const char* sourcePath, const char* destinationPath);
void deleteDirectory(const char* path);
/* Functions below are for txt file manipulation */
int endsWith(const char* str, const char* suffix);
void resetTXT(void);
void writeTXT(const char* example_path);
void updateTXT(const char* input);
const char* input2model(const char* input);
const char* input2header(const char* input);
const char* input2filename(const char* dest_path, const char* input);




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
	char ambpro2_path[512];
	char* arduino15_add = "\\AppData\\Local\\Arduino15";
	char* ambpro2_add = "\\packages\\realtek\\hardware\\AmebaPro2";

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
	strcpy(ambpro2_path, arduino15_path);
	strcat(ambpro2_path, ambpro2_add);

#if PRINT_DEBUG
	printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("root_path        = %s\n", root_path);
	printf("arduino15_path   = %s\n", arduino15_path);
	printf("ambpro2_path     = %s\n", ambpro2_path);
	printf("ambpro2_sdkver   = %s\n", dirName(ambpro2_path));
#endif

	// SECTION FOR FUNCTION TEST
	const char* input = "CUSTOMIZED_MOBILEFACENET";
	const char* model = input2model(input);
	printf("Model: %s\n", model);

	updateTXT("Hello, world!");













	// END OF SECTION FOR FUNCTION TEST
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

	// check weather dir is valid
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
		return count;
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

const char* input2model(const char* input) {
	const char* model_mapping[][2] = {
		{"CUSTOMIZED_YOLOV3TINY",    "yolov3_tiny"},
		{"CUSTOMIZED_YOLOV4TINY",    "yolov4_tiny"},
		{"CUSTOMIZED_YOLOV7TINY",    "yolov7_tiny"},
		{"CUSTOMIZED_MOBILEFACENET", "mobilefacenet_i16"},
		{"CUSTOMIZED_SCRFD",         "scrfd640"},
		{"DEFAULT_YOLOV3TINY",       "yolov3_tiny"},
		{"DEFAULT_YOLOV4TINY",       "yolov4_tiny"},
		{"DEFAULT_YOLOV7TINY",       "yolov7_tiny"},
		{"DEFAULT_MOBILEFACENET",    "mobilefacenet_i8"},
		{"DEFAULT_SCRFD",            "scrfd320p"}
	};

	int mapping_size = sizeof(model_mapping) / sizeof(model_mapping[0]);

	for (int i = 0; i < mapping_size; i++) {
		if (strcmp(input, model_mapping[i][0]) == 0) {
			return model_mapping[i][1];
		}
	}

	return NULL;
}

const char* input2header(const char* input) {
	const char* header = NULL;

	if (strcmp(input, "yolov3_tiny") == 0 ||
		strcmp(input, "yolov4_tiny") == 0 ||
		strcmp(input, "yolov7_tiny") == 0) {
		header = "NNObjectDetection.h";
	}
	else if (strcmp(input, "mobilefacenet_i16") == 0 ||
		strcmp(input, "scrfd640") == 0 ||
		strcmp(input, "mobilefacenet_i8") == 0 ||
		strcmp(input, "scrfd320p") == 0) {
		header = "NNFaceDetectionRecognition.h";
	}
	else if (strcmp(input, "None") == 0) {
		header = "NA";
	}

	return header;
}

const char* input2filename(const char* dest_path, const char* input) {
	const char* value_file = NULL;

	if (dirExists(dest_path)) {
		DIR* dir = opendir(dest_path);
		struct dirent* entry;

		while ((entry = readdir(dir)) != NULL) {
			if (endsWith(entry->d_name, ".json")) {
				char file_json_path[1024];
				sprintf(file_json_path, "%s/%s", dest_path, entry->d_name);

				FILE* file = fopen(file_json_path, "r");
				if (file) {
					char line[1024];
					while (fgets(line, sizeof(line), file)) {
						// Assuming the JSON structure is known
						if (strstr(line, input) != NULL && strstr(line, "\"file\"") != NULL) {
							char* start = strchr(line, '\"') + 1;
							char* end = strrchr(line, '\"');
							size_t length = end - start;
							//value_file = malloc(length + 1);
							//strncpy(value_file, start, length);
							//value_file[length] = '\0';
							break;
						}
					}

					fclose(file);
				}
			}
		}

		closedir(dir);
	}

	return value_file;
}

/*
* Function checks wehtther the directory exisits 
* Returns 1 if dir is valid
* Returns 0 if dir is invalid
*/
int dirExists(const char* directory_path) {
	DIR* directory = opendir(directory_path);
	// check weather dir is valid
	if (directory) {
		closedir(directory);
		return 1;
	} 	else {
		printf("[%s][Error] Failed to open directory.\n", __func__);
		return 0;
	}
}

/*
* Function check folder names under current directory 
* Returns NA
*/
const char* dirName(const char* directory_path) {
	int sdk_counter = 0;
	struct dirent* entry;
	DIR* directory = opendir(directory_path);
	const char* sdk_name = "";
	// check dir validation
	if (directory) {
		while ((entry = readdir(directory)) != NULL) {
			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
				continue;
			}
			else {
				sdk_counter++;	
				sdk_name = entry->d_name;
			}
		}
		// non singular SDK validation
		if (sdk_counter > 1) {
			printf("[%s][Error] Current dirctory contains more than 1 SDK!!! \n", __func__);
			exit(1);
		}
		else {
			return sdk_name;			
		}
	}
	else {
		printf("[%s][Error] Failed to open directory.\n", __func__);
	}
	closedir(directory);
}

void resetTXT(void) {
	const char* filepath_txt = "path/to/your/file.txt";
	const char* directory = "path/to/your/directory";

	// 创建目录（如果不存在）
	struct stat st;
	if (stat(directory, &st) == -1) {
		mkdir(directory, 0700);
	}

	// 打开文件并清空内容
	FILE* file = fopen(filepath_txt, "w");
	if (file != NULL) {
		fclose(file);
	}

	printf("[INFO] %s has been reset\n", filepath_txt);
}

void updateTXT(const char* input) {
	const char* filepath_txt = "path/to/your/file.txt"; // 修改为你的文件路径

	//char* directory = strdup(filepath_txt);
	/*	char* dir_path = dirname(directory);

	// 创建目录（如果不存在）
#ifdef _WIN32
	_mkdir(dir_path);
#else
	mkdir(dir_path, 0777);
#endif

	FILE* file = fopen(filepath_txt, "a");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file.\n");
		free(directory);
		return;
	}

	fprintf(file, "%s\n", input);

	fclose(file);*/
	//free(directory);
}



void writeTXT(const char* example_path) {

}