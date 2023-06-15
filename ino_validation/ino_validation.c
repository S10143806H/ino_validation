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

/* Declear function headers */
int dirExists(const char* directory_path);
const char* dirName(const char* directory_path);
int isDirExists(const char* path);
void copyFile(const char* sourcePath, const char* destinationPath);
void copyDirectory(const char* sourcePath, const char* destinationPath);
void deleteDirectory(const char* path);
const char* validateINO(const char* directory_path);

/* Functions below are for txt file manipulation */
int endsWith(const char* str, const char* suffix);
void resetTXT(const char* directory_path);
void writeTXT(const char* example_path);
void updateTXT(const char* input);
const char* input2model(const char* input);
const char* input2header(const char* input);
const char* input2filename(const char* dest_path, const char* input);
cJSON* loadJSONFile(const char* directory_path);
void removeChar(char* str, char c);
const char* pathTempJSON(const char* directory_path);


/* Declear common file paths */
char* arduino15_add = "\\AppData\\Local\\Arduino15";
char* ambpro2_add = "\\packages\\realtek\\hardware\\AmebaPro2\\";
char* model_add = "\\variants\\common_nn_models";
char* txtfile_add = "\\misc\\";

char root_path[MAX_PATH_LENGTH];
char arduino15_path[MAX_PATH_LENGTH];
char ambpro2_path[MAX_PATH_LENGTH];
char model_path[MAX_PATH_LENGTH];
char txtfile_path[MAX_PATH_LENGTH];

const char* example_path;
const char* path_build_options_json;


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

	strcpy(root_path, getenv("USERPROFILE"));
	strcpy(arduino15_path, getenv("USERPROFILE"));
	strcat(arduino15_path, arduino15_add);
	strcpy(ambpro2_path, arduino15_path);
	strcat(ambpro2_path, ambpro2_add);
	strcpy(model_path, ambpro2_path);
	strcat(model_path, dirName(ambpro2_path));
	strcat(model_path, model_add);
	strcpy(txtfile_path, argv[2]);
	strcat(txtfile_path, txtfile_add);
	//strcat(txtfile_path, filename_txt);

#if PRINT_DEBUG
	// Print the input parameters 
	printf("Parameter 1      = %s\n", build_path);
	printf("Parameter 2      = %s\n", tools_path);
	printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("root_path        = %s\n", root_path);
	printf("arduino15_path   = %s\n", arduino15_path);
	printf("ambpro2_path     = %s\n", ambpro2_path);
	printf("ambpro2_sdkver   = %s\n", dirName(ambpro2_path));
	printf("model_path       = %s\n", model_path);
	printf("txtfile_path     = %s\n", txtfile_path);
#endif

	resetTXT(txtfile_path);
	// generate path
	path_build_options_json = pathTempJSON(build_path);
	printf("[%s][INFO] %s\n", __func__, path_build_options_json);

	example_path = validateINO(build_path);

	// SECTION FOR FUNCTION TEST
	//writeTXT(example_path);

	exit(1);
	while (1);

	// END OF SECTION FOR FUNCTION TEST

	const char* input = "CUSTOMIZED_MOBILEFACENET";
	const char* model = input2model(input);
	printf("Model: %s\n", model);
	updateTXT("Hello, world!");

#if PRINT_DEBUG
	

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
	const char* filepath_txt=""; // 修改为你的文件路径
	//strcpy(filepath_txt, input);
	//printf("%s\n", input);
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

	printf("[%s][INFO] Load json file \"%s\"\n", __func__, example_path);
	
	
}

const char* validateINO(const char* directory_path) {
	DIR* dir;
	struct dirent* ent;
	const char* extension = ".json";
	const char* json_keyword = "build";

	cJSON* data = loadJSONFile(path_build_options_json);
	cJSON* example_path = cJSON_GetObjectItem(data, "sketchLocation");

	printf("[%s][INFO] example_path %s \n", __func__, example_path->valuestring);


	// check weather dir is valid
	if ((dir = opendir(directory_path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {

			if (ent->d_type == DT_REG) {
#if PRINT_DEBUG
				//printf("[%s] File:%s\n", __func__, ent->d_name);
				size_t filesize = strlen(ent->d_name);
				size_t extensionsize = strlen(extension);
				const char* jsonfilename = strstr(ent->d_name, json_keyword);
				const char* json_data = NULL;
				const char* keyword = "Arduino15";
				const char* keyword2 = ".ino";
				const char* keyword3 = "Arduino15";
				long length;

				if (filesize >= extensionsize && strcmp(ent->d_name + filesize - extensionsize, extension)== 0) {
					if (strlen(jsonfilename)!=0 && strlen(jsonfilename) == strlen(ent->d_name)) {
						strcat(directory_path, "\\");
						strcat(directory_path, jsonfilename);
						printf("[%s][INFO] Load json file \"%s\"\n", __func__, directory_path);
					}
					// Open the JSON file and retrive the data 
					cJSON* data = loadJSONFile(directory_path);

					// Access and process the parsed JSON data
					// Arduino IDE1.0 
					cJSON* example_path = cJSON_GetObjectItem(data, "sketchLocation");
					
					// Arduino IDE2.0	
					if (strstr(example_path->valuestring, keyword) == NULL) {
						// Extract example name from file path
						char* example_name = strrchr(example_path->valuestring, '\\');
						removeChar(example_name, '\\');

						

						if (strstr(example_path->valuestring, keyword2) == NULL && strstr(example_name, keyword2) == NULL) {
							// rename json extracted example filename
							strcat(example_name, keyword2);
							printf("123\n");
							//printf("[%s][INFO] example_path %s \n", __func__, example_path->valuestring);
							//printf("[%s][INFO] example_name %s \n", __func__, example_name);
							// find filepath in includes.cache
						}
						printf("[%s][INFO] example_path %s \n", __func__, example_path->valuestring);
						printf("[%s][INFO] example_name %s \n", __func__, example_name);
					}

					

					// Clean up cJSON object and allocated memory
					cJSON_Delete(data);
					free(json_data);

					return example_path->valuestring;
				}
#endif
			}
		}
	}
}

/*
* Load JSON file from the directory path into cJSON library
*/
cJSON* loadJSONFile(const char* directory_path) {
	// Open the JSON file
	FILE* file = fopen(directory_path, "r");
	if (file == NULL) {
		printf("[%s][Error] Failed to open the JSON file.\n", __func__);
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
		printf("[%s][Error] Failed to read the JSON file.\n", __func__);
		fclose(file);
		free(json_data);
		return 1;
	}
	json_data[file_size] = '\0';  // Null-terminate the string

	// Close the file
	fclose(file);

	// Parse the JSON data
	cJSON* data = cJSON_Parse(json_data);
	return data;
}


/*
* Return path of the JSON file under the given input directory_path
*/
const char* pathTempJSON(const char* directory_path) {
	DIR* dir;
	struct dirent* ent;
	const char* extension = ".json";
	const char* json_keyword = "build";


	// check weather dir is valid
	if ((dir = opendir(directory_path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {

			if (ent->d_type == DT_REG) {

				printf("[%s] File:%s\n", __func__, ent->d_name);
				size_t filesize = strlen(ent->d_name);
				size_t extensionsize = strlen(extension);
				const char* jsonfilename = strstr(ent->d_name, json_keyword);
				const char* json_data = NULL;
				const char* keyword = "Arduino15";
				const char* keyword2 = ".ino";
				long length;

				if (filesize >= extensionsize && strcmp(ent->d_name + filesize - extensionsize, extension) == 0) {
					if (strlen(jsonfilename) != 0 && strlen(jsonfilename) == strlen(ent->d_name)) {
						strcat(directory_path, "\\");
						strcat(directory_path, jsonfilename);
#if PRINT_DEBUG
						printf("[%s][INFO] Load json file \"%s\"\n", __func__, directory_path);
#endif
					}
					return directory_path;
				}

			}
		}
	}
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
