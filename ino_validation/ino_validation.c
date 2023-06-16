#define _GNU_SOURCE
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "dirent.h"	// https://codeyarns.com/tech/2014-06-06-how-to-use-dirent-h-with-visual-studio.html#gsc.tab=0
#include "cJSON.h"
#include <locale.h>

#define PRINT_DEBUG 1
#define MAX_PATH_LENGTH 1024

/* Declear global vairables */
const char* key_ambNN = "modelSelect";
const char* key_ambVOE = "configVideoChannel";
const char* key_amb_header = "#include";
const char* key_amb_customized = "CUSTOMIZED";
const char* key_amb_default = "DEFAULT";
const char* key_amb_default_backup = "Dbackup";
const char* key_amb_customized_backup = "Cbackup";
const char* key_amb_bypassNN1 = " .modelSelect";
const char* key_amb_bypassNN2 = " modelSelect";
const char* key_amb_bypassVOE1 = " .configVideoChannel";
const char* key_amb_bypassVOE2 = " configVideoChannel";
const char* filename_txt = "ino_validation.txt";

/* Declear function headers */
int dirExists(const char* directory_path);
const char* dirName(const char* directory_path);
int isDirExists(const char* path);
void copyFile(const char* sourcePath, const char* destinationPath);
void copyDirectory(const char* sourcePath, const char* destinationPath);
void deleteDirectory(const char* path);

/* Functions below are for txt file manipulation */
int endsWith(const char* str, const char* suffix);
void writeTXT(const char* path_example);

/* Returns example file path inside the temp JSON file */
const char* pathTempJSON(const char* directory_path, const char* ext, const char* key);
/* Load JSON file from the directory and parse into cJSON data format */
cJSON* loadJSONFile(const char* directory_path);
/* Remove char c from string str */
void removeChar(char* str, char c);
/* Validate example in directory_path and returns example path */
const char* validateINO(const char* directory_path);
/* Clear all content in the TXT file */
void resetTXT(const char* directory_path);
// -------------------------------------------------------------
/* Conveert model input type to model filename*/
const char* input2filename(const char* dest_path, const char* input);
/* Conveert model input type to model name*/
const char* input2model(const char* input);
/* Conveert model input type to model header file*/
const char* input2header(const char* input);
/* Update content in the input to TXT file */
void updateNATXT(const char* filepath, const char* start_line, const char* end_line);
/* Update content in the input to TXT file */
void updateTXT(const char* input);

/* Declear common file paths */
char* path_arduino15_add = "\\AppData\\Local\\Arduino15";
char* ambpro2_add = "\\packages\\realtek\\hardware\\AmebaPro2\\";
char* model_add = "\\variants\\common_nn_models";
char* txtfile_add = "\\misc\\";

char path_root[MAX_PATH_LENGTH];
char arduino15_path[MAX_PATH_LENGTH];
char ambpro2_path[MAX_PATH_LENGTH];
char path_model[MAX_PATH_LENGTH];
char path_txtfile[MAX_PATH_LENGTH];

const char* path_build_options_json = NULL;
const char* path_example = "";
const char* name_example = NULL;
const char* ext_json = ".json";
const char* key_json = "build";

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "en_US.UTF-8");
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

	strcpy(path_root, getenv("USERPROFILE"));
	strcpy(arduino15_path, getenv("USERPROFILE"));
	strcat(arduino15_path, path_arduino15_add);
	strcpy(ambpro2_path, arduino15_path);
	strcat(ambpro2_path, ambpro2_add);
	strcpy(path_model, ambpro2_path);
	strcat(path_model, dirName(ambpro2_path));
	strcat(path_model, model_add);
	strcpy(path_txtfile, argv[2]);
	strcat(path_txtfile, txtfile_add);
	//strcat(path_txtfile, filename_txt);

#if PRINT_DEBUG
	// Print the input parameters 
	printf("Parameter 1      = %s\n", build_path);
	printf("Parameter 2      = %s\n", tools_path);
	printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("path_root        = %s\n", path_root);
	printf("arduino15_path   = %s\n", arduino15_path);
	printf("ambpro2_path     = %s\n", ambpro2_path);
	printf("ambpro2_sdkver   = %s\n", dirName(ambpro2_path));
	printf("path_model       = %s\n", path_model);
	printf("path_txtfile     = %s\n", path_txtfile);
#endif

	resetTXT(path_txtfile);
	// generate path
	path_build_options_json = pathTempJSON(build_path, ext_json, key_json);
	path_example = validateINO(build_path);

	printf("[%s][INFO] path_build_options_json = %s\n", __func__, path_build_options_json);
	printf("[%s][INFO] path_example            = %s\n", __func__, path_example);
	printf("[%s][INFO] name_example            = %s\n", __func__, name_example);

	// SECTION FOR FUNCTION TEST
	writeTXT(path_example);

	
	
	
	
	exit(1);
	while (1);
	printf("111\n");
printf("222\n");

	// END OF SECTION FOR FUNCTION TEST

	const char* input = "CUSTOMIZED_MOBILEFACENET";
	const char* model = input2model(input);
	printf("Model: %s\n", model);
	updateTXT("Hello, world!");


	// check whether directory exists
	if (isDirExists(build_path) && isDirExists(tools_path)) {
		copyDirectory(tools_path, build_path);
	}

	deleteDirectory(tools_path);
	mkdir(tools_path, 0755);

	return 0;
}

/**
* Function returns nuber of content under this path
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
		char filePath[MAX_PATH_LENGTH];
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

/*
* Create empty txt file and renamed as ino_validation.txt
*/
void resetTXT(const char* directory_path) {
	DIR* dir = opendir(directory_path);
	struct stat st;

	// create directory if not exists
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0700);
	}
	strcat(directory_path, filename_txt);

	// open txt file and clear everything
	FILE* file = fopen(directory_path, "w");
	if (file != NULL) {
        
	}
}

void updateTXT(const char* input) {
	//const char* filepath_txt = path_txtfile; // 修改为你的文件路径
	//strcpy(filepath_txt, input);
	printf("%s\n", input);
	DIR* directory = opendir(path_txtfile);
	if (directory) {
	}
	else {
		printf("[%s][Error] Failed to open directory.\n", __func__);
	}
	closedir(directory);
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



const char* pathTempJSON(const char* directory_path, const char* ext, const char* key) {
	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(directory_path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
#if PRINT_DEBUG
				printf("[%s] [INFO] File:%s\n", __func__, ent->d_name);
				size_t size_file = strlen(ent->d_name);
				size_t size_json = strlen(ext);
				const char* jsonfilename = strstr(ent->d_name, key);

				if (size_file >= size_json && strcmp(ent->d_name + size_file - size_json, ext) == 0) {
					if (strlen(jsonfilename) != 0 && strlen(jsonfilename) == strlen(ent->d_name)) {
						strcat(directory_path, "\\");
						strcat(directory_path, jsonfilename);
						printf("[%s][INFO] Load json file \"%s\"\n", __func__, directory_path);
#endif
					}
					return directory_path;
				}
			}
		}
	}
}

cJSON* loadJSONFile(const char* directory_path) {
	// Open file
	FILE* file = fopen(directory_path, "r");
	if (file == NULL) {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		return 1;
	}

	// Get the file size
	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	rewind(file);

	// Allocate memory to hold the JSON data
	char* json_data = (char*)malloc(file_size + 1);
	if (json_data == NULL) {
		printf("[%s][Error] Failed to allocate memory.\n", __func__);
		fclose(file);
		return 1;
	}

	// Read the JSON data from the file
	size_t read_size = fread(json_data, 1, file_size, file);
	if (read_size != file_size) {
		printf("[%s][Error] Failed to read the file.\n", __func__);
		fclose(file);
		free(json_data);
		return 1;
	}
	json_data[file_size] = '\0';  // Null-terminate the string

	// Close the file
	fclose(file);

	// Parse the JSON data
	cJSON* data = cJSON_Parse(json_data);

	// Clean up cJSON object and allocated memory
	//cJSON_Delete(data);
	//free(json_data);

	return data;
}

void removeChar(char* str, char c) {
	int i, j;
	int len = strlen(str);
	for (i = j = 0; i < len; i++) {
		if (str[i] != c) {
			str[j++] = str[i];
		}
	}
	str[j] = '\0';
}

const char* validateINO(const char* directory_path) {
	DIR* dir;
	struct dirent* ent;
	const char* key_amb = "Arduino15";
	const char* key_ino = ".ino";

	// Open the JSON file and retrive the data
	cJSON* data = loadJSONFile(path_build_options_json);
	// Arduino IDE1.0 
	cJSON* path_example = cJSON_GetObjectItem(data, "sketchLocation");
	path_example = path_example->valuestring;
	// Arduino IDE2.0	

	if (strstr(path_example, key_amb) == NULL) {
		//name_example = strrchr(path_example, '\\');
		//removeChar(name_example, '\\');
		//if (strstr(path_example, key_ino) == NULL && strstr(name_example, key_ino) == NULL) {
			// rename json extracted example filename
			//strcat(name_example, key_ino);
			// find filepath in includes.cache
		//}
		printf("[%s][INFO] path_example %s \n", __func__, path_example);
		//printf("[%s][INFO] name_example = %s\n", __func__, name_example);	
	}
	// Clean up cJSON object and allocated memory
	cJSON_Delete(data);
	//free(data);
	
	return path_example;
}

void writeTXT(const char* path) {
	DIR* dir;
	struct dirent* ent;
	const char* key_ino = ".ino";
	const char buf[MAX_PATH_LENGTH] = "";
	const char backslash[] = "\\";
	char line[MAX_PATH_LENGTH] = {0};
	unsigned int line_count = 0;

	path = path_example;

#if PRINT_DEBUG
	printf("[%s][INFO] Load json file \"%s\"\n", __func__, path);
	// check weather dir is valid
	if ((dir = opendir(path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				strcpy(buf, ent->d_name);
				if (strstr(buf, key_ino) != 0) {
					strcat(path, backslash);
					strcat(path, buf);
					printf("[%s][INFO] path_example            = %s\n", __func__, path);
#endif
				}
			}
		}
	}
	
	// Open file
	FILE* file = fopen(path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");
	if (file) {
		char line[1024];
		while (fgets(line, sizeof(line), file)) {
			printf("%s\n",line);
		}
		updateTXT("----------------------------------");
		fclose(file);
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}

}

