﻿/*
LINUX: 
gcc -c cJSON.c
ar rcs libcjson.a cJSON.o
gcc ino_validation.c -o ino_validation -L -ljson -static

*/
#define _GNU_SOURCE

#ifdef _WIN32
	#include <io.h>
	#include "dirent.h"	// https://codeyarns.com/tech/2014-06-06-how-to-use-dirent-h-with-visual-studio.html#gsc.tab=0
#else // #elif __linux__
	#include <inttypes.h>
	#include <unistd.h>
	#include <dirent.h>
	#define __int64 int64_t
	#define _close close
	#define _read read
	#define _lseek64 lseek64
	#define _O_RDONLY O_RDONLY
	#define _open open
	#define _lseeki64 lseek64
	#define _lseek lseek
	#define stricmp strcasecmp
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "cJSON.h"
#include <locale.h>

#define PRINT_DEBUG 0
#define MAX_PATH_LENGTH 1024

/* Declear function headers */
int dirExists(const char* directory_path);
/* Return folder names under current directory */
const char* dirName(const char* directory_path);
/* Function returns nuber of content under this path */
int isDirExists(const char* path);
/* Functions below are for txt f_model manipulation */
int endsWith(const char* str, const char* suffix);
/* Function checks whether the directory exisits */
void writeTXT(const char* path_example);
/* Returns example f_model path inside the temp JSON f_model */
const char* pathTempJSON(const char* directory_path, const char* ext, const char* key);
/* Load JSON f_model from the directory and parse into cJSON data format */
cJSON* loadJSONFile(const char* directory_path);
/* Remove char c from string str */
void removeChar(char* str, char c);
/* Validate example in directory_path and returns example path */
const char* validateINO(const char* directory_path);
/* Clear all content in the ino_validation.txt file */
void resetTXT(const char* directory_path);
/* Update content in the input to TXT f_model */
void updateTXT(const char* input);
/* Repalce the single backslash to double in Windows*/
void replaceBackslash(char* str);
/* Similar function as REGEX*/
void extractParam(char* line, char* param);
/* Extract header from quote symbols*/
void extractString(char* source, char* result);
/* Handler to print error message in the Arduino IDE */
void error_handler(const char* message);
/* Convert model input type to model name*/
const char* input2model(const char* input);
/* Find the dir name of input source file path*/
void extractRootDirectory(char* source, char* result);
// -------------------------------------------------------------
/* Conveert model input type to model filename*/
const char* input2filename(const char* dest_path, const char* input);
/* Conveert model input type to model header f_model*/
const char* input2header(const char* input);

void convertPath(char* path);

/* Declear global vairables */
const char* key_amb_NN					= "modelSelect";
const char* key_amb_bypassNN1			= " .modelSelect";
const char* key_amb_bypassNN2			= " modelSelect";
const char* key_amb_VOE					= "configVideoChannel";
const char* key_amb_bypassVOE1			= " .configVideoChannel";
const char* key_amb_bypassVOE2			= " configVideoChannel";
const char* key_amb_header				= "#include";
const char* key_amb_customized			= "CUSTOMIZED";
const char* key_json 					= "build";
const char* key_amb 					= "Arduino15";
const char* key_ino 					= ".ino";
const char* ext_json 					= ".json";
// const char* key_amb_default				= "DEFAULT";
// const char* key_amb_default_backup		= "Dbackup";
// const char* key_amb_customized_backup	= "Cbackup";
const char* filename_txt				= "ino_validation.txt";

/* Declear common file paths */
#ifdef _WIN32
char* path_arduino15_add				= "\\AppData\\Local\\Arduino15";
char* path_ambpro2_add					= "\\packages\\realtek\\hardware\\AmebaPro2\\";
char* path_model_add					= "\\variants\\common_nn_models";
char* path_txtfile_add					= "\\misc\\";
#else
char* path_arduino15_add				= "/.arduino15";
char* path_ambpro2_add					= "/packages/realtek/hardware/AmebaPro2/";
char* path_model_add					= "/variants/common_nn_models";
char* path_txtfile_add					= "/misc/";
#endif

const char* path_build_options_json = NULL;
const char* path_example = NULL;
const char* name_example = NULL;

char path_root[MAX_PATH_LENGTH];
char path_arduino15[MAX_PATH_LENGTH];
char path_pro2[MAX_PATH_LENGTH];
char path_model[MAX_PATH_LENGTH];
char path_txtfile[MAX_PATH_LENGTH];

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
	const char* path_build = argv[1];
	const char* path_tools = argv[2];

	// Retrive root path
#ifdef _WIN32
	strcpy(path_root, getenv("USERPROFILE"));
	strcpy(path_arduino15, getenv("USERPROFILE"));
#else
	strcpy(path_root, getenv("HOME"));
	strcpy(path_arduino15, getenv("HOME"));
#endif
	printf("%s\n",path_root);
	printf("%s\n",path_arduino15);
	
	// Retrive user name 
#ifdef linux
	char login[256];
	if (getlogin_r(login, sizeof(login)) == 0) {
		printf("Login name: %s \n", login);
	}
#endif 

	strcat(path_arduino15, path_arduino15_add);
	strcpy(path_pro2, path_arduino15);
	strcat(path_pro2, path_ambpro2_add);
	strcpy(path_model, path_pro2);
	strcat(path_model, dirName(path_pro2));
	strcat(path_model, path_model_add);
	strcpy(path_txtfile, argv[2]);
	strcat(path_txtfile, path_txtfile_add);

	// convert path according to system
	convertPath(path_build);
	convertPath(path_tools);
	convertPath(getenv("USERPROFILE"));
	convertPath(getenv("HOMEDRIVE"));
	convertPath(getenv("HOMEPATH"));
	convertPath(path_root);
	convertPath(path_arduino15);
	convertPath(path_pro2);
	convertPath(dirName(path_pro2));
	convertPath(path_model);
	convertPath(path_txtfile);

	// Print the input parameters 
	printf("Parameter 1      = %s\n", path_build);
	printf("Parameter 2      = %s\n", path_tools);
	printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("path_root        = %s\n", path_root);
	printf("path_arduino15   = %s\n", path_arduino15);
	printf("path_pro2        = %s\n", path_pro2);
	printf("ver_pro2         = %s\n", dirName(path_pro2));
	printf("path_model       = %s\n", path_model);
	printf("path_txtfile     = %s\n", path_txtfile);

	resetTXT(path_txtfile);
	printf("[%s][INFO] resetTXT done\n", __func__);
	path_build_options_json = pathTempJSON(path_build, ext_json, key_json);
	printf("[%s][INFO] path_build_options_json = %s\n", __func__, path_build_options_json);
	path_example = validateINO(path_build);
	printf("[%s][INFO] path_example            = %s\n", __func__, path_example);

	// SECTION FOR FUNCTION TEST =======================================================
	writeTXT(path_example);

	exit(1);
	while (1);

	// END OF SECTION FOR FUNCTION TEST =======================================================
	return 0;
}

void convertPath(char* path) {
    #ifdef _WIN32
        const char* separator = "\\";
		const char* replacement = "\\";
    #else
		const char* separator = "\\";
		const char* replacement = "/";
    #endif

	const char* position = strstr(path, separator); // 查找路径中的斜杠
    while (position != NULL) {
        memcpy(position, replacement, strlen(replacement)); // 替换斜杠
        position = strstr(position + strlen(replacement), separator); // 继续查找下一个斜杠
    }
}

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

	return "NA";
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

				FILE* f_model = fopen(file_json_path, "r");
				if (f_model) {
					char line[1024];
					while (fgets(line, sizeof(line), f_model)) {
						// Assuming the JSON structure is known
						if (strstr(line, input) != NULL && strstr(line, "\"f_model\"") != NULL) {
							char* start = strchr(line, '\"') + 1;
							char* end = strrchr(line, '\"');
							size_t length = end - start;
							//value_file = malloc(length + 1);
							//strncpy(value_file, start, length);
							//value_file[length] = '\0';
							break;
						}
					}

					fclose(f_model);
				}
			}
		}

		closedir(dir);
	}

	return value_file;
}

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

void resetTXT(const char* directory_path) {
	DIR* dir = opendir(directory_path);
	struct stat st;

	// create directory if not exists
	if (stat(dir, &st) == -1) {
		mkdir(dir, 0700);
	}
	strcat(directory_path, filename_txt);

	// open txt f_model and clear everything
	FILE* f_model = fopen(directory_path, "w");
	if (f_model != NULL) {
        
	}
}

void updateTXT(const char* input) {
	FILE* f_model = fopen(path_txtfile, "a");

	if (f_model) {
		fprintf(f_model, "%s\n", input);
		fclose(f_model);
	}
	else {
#if PRINT_DEBUG
		printf("[%s][Error] Failed to open the f_model.\n", __func__);
#endif	
		perror(path_txtfile);
		return EXIT_FAILURE;
	}
}

const char* pathTempJSON(const char* directory_path, const char* ext, const char* key) {
	DIR* dir;
	struct dirent* ent;

#if PRINT_DEBUG
	printf("[%s][INFO] Load json f_model \"%s\"\n", __func__, directory_path);
#endif
	if ((dir = opendir(directory_path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			if (ent->d_type == DT_REG) {
				size_t size_file = strlen(ent->d_name);
				size_t size_json = strlen(ext);
				const char* jsonfilename = strstr(ent->d_name, key);

				if (size_file >= size_json && strcmp(ent->d_name + size_file - size_json, ext) == 0) {
					if (strlen(jsonfilename) != 0 && strlen(jsonfilename) == strlen(ent->d_name)) {
#if PRINT_DEBUG
						printf("[%s][INFO] File:%s\n", __func__, ent->d_name);
#endif
						strcat(directory_path, "\\");
						strcat(directory_path, jsonfilename);

					}
					return directory_path;
				}
			}
		}
	}
}

cJSON* loadJSONFile(const char* directory_path) {
	// Open f_model
	FILE* f_model = fopen(directory_path, "r");
	if (f_model == NULL) {
		printf("[%s][Error] Failed to open the f_model.\n", __func__);
		return 1;
	}

	// Get the f_model size
	fseek(f_model, 0, SEEK_END);
	long file_size = ftell(f_model);
	rewind(f_model);

	// Allocate memory to hold the JSON data
	char* json_data = (char*)malloc(file_size + 1);
	if (json_data == NULL) {
		printf("[%s][Error] Failed to allocate memory.\n", __func__);
		fclose(f_model);
		return 1;
	}

	// Read the JSON data from the f_model
	size_t read_size = fread(json_data, 1, file_size, f_model);
	if (read_size != file_size) {
		printf("[%s][Error] Failed to read the f_model.\n", __func__);
		fclose(f_model);
		free(json_data);
		return 1;
	}
	json_data[file_size] = '\0';  // Null-terminate the string

	// Close the f_model
	fclose(f_model);

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

	// Open the JSON f_model and retrive the data
	cJSON* data = loadJSONFile(path_build_options_json);
	// Arduino IDE1.0 
	cJSON* path_example = cJSON_GetObjectItem(data, "sketchLocation");

	/*
	// Arduino IDE2.0	
	if (strstr(path_example, key_amb) == NULL) {
		name_example = strrchr(path_example, '\\');
		//removeChar(name_example, '\\');
		//if (strstr(path_example, key_ino) == NULL && strstr(name_example, key_ino) == NULL) {
			// rename json extracted example filename
			//strcat(name_example, key_ino);
			// find filepath in includes.cache
		//}
		//printf("[%s][INFO] name_example = %s\n", __func__, name_example);	
	}
	*/
#ifdef PRINT_DEBUG
	printf("[%s][INFO] Current example path: %s \n", __func__, path_example->valuestring);
#endif	

	return path_example->valuestring;
}

void replaceBackslash(char* str) {
	char* found = NULL; 
	// search for the first occurance of `\`
	found = strchr(str, '\\'); 
	while (found != NULL) { 
		// replace as `\\`
		memmove(found + 1, found, strlen(found) + 1); 
		*found = '\\'; 
		// find next
		found = strchr(found + 2, '\\\\'); 
	} 
}

void extractParam(char* line, char* param) {
	char* start = strchr(line, '(');
	char* end = strchr(line, ')');
	if (start != NULL && end != NULL && end > start) {
		size_t length = end - start - 1;
		strncpy(param, start + 1, length);
		param[length] = '\0';
	}
}

void error_handler(const char *message) {
	fprintf(stderr, "[Error] %s\n", message);
	exit(1);
}

void extractString(char* source, char* result) {
	char* start = strchr(source, '\"'); // find the 1st "
	if (start == NULL) {
		strcpy(result, ""); // set as empty string if not found
		return;
	}

	start++; // skip the first "

	char* end = strchr(start, '\"'); // find the 2nd "
	if (end == NULL) {
		strcpy(result, ""); // set as empty string if not found
		return;
	}

	int length = end - start; 
	strncpy(result, start, length); 
	result[length] = '\0'; // add ending param at EOL
}

void extractRootDirectory(char* filepath, char* rootDir) {
	char* lastSeparator = strrchr(filepath, '\\'); // find last occurance of backspace
	if (lastSeparator == NULL) {
		strcpy(rootDir, ""); // set as empty string if not found
		return;
	}
	int length = lastSeparator - filepath + 1; 
	strncpy(rootDir, filepath, length); 
	rootDir[length] = '\0'; // add ending param at EOL
}

void writeTXT(const char* path) {
	DIR* dir;
	struct dirent* ent;
	const char buf[MAX_PATH_LENGTH] = "";
	const char backslash[] = "\\";
	char line[MAX_PATH_LENGTH] = {0};
	unsigned int line_count = 0;
	char voe_status[100] = "NA";
	char model_type[100] = "";
	char model_name_od[100] = "";
	char model_name_fd[100] = "";
	char model_name_fr[100] = "";
	char line_strip_header[100] = "NA";
	char line_strip_headerNN[100] = "NA";
	char dir_example[100] = "NA";

	path = path_example;
	// replaceBackslash(path);

#if PRINT_DEBUG
	printf("[%s][INFO] Load example: \"%s\"\n", __func__, path);
#endif

	updateTXT("----------------------------------");
	updateTXT("Current ino contains model(s):");

	// check path format: IDE1 file path, IDE2 dir path
	if (strstr(path, ".ino") == NULL) {
		printf("IDE2\n");
		//																	update path 
		DIR* dir;
		struct dirent* ent;
		int count = 0;

		// check weather dir is valid
		if ((dir = opendir(path)) != NULL) {
			/* print all the files and directories within directory */
			while ((ent = readdir(dir)) != NULL) {
				if (ent->d_type == DT_REG) {
					count++;
#if PRINT_DEBUG
					printf("[%s] File:%s\n", __func__, ent->d_name);
#endif
					if (strstr(ent->d_name, ".ino") == NULL) {
#ifdef _WIN32
					strcat(path, "\\");
#else
					strcat(path, "/");
#endif
					strcat(path, ent->d_name);
					printf("[%s] path:%s\n", __func__, path);
					}
				}
			}
		}
		else {
			/* opendir() failed for some other reason. */
			return 0;
		}
	}


	FILE* f_model = fopen(path, "r");  //FILE* f_model = fopen(path, "r, ccs=UTF-8");
	char param[100];
	if(f_model) {
		char line[1024];
		while (fgets(line, sizeof(line), f_model)) {
			/* check whether keywordNN in f_model content */ 
			if (strstr(line, key_amb_NN) != NULL && strstr(line, "//") == NULL && strstr(line, key_amb_bypassNN1) == NULL && strstr(line, key_amb_bypassNN2) == NULL){
				extractParam(line, param);
				printf("Extracted parameter: %s\n", param);
				char* token;
				token = strtok(param, ", ");
				if (token != NULL) { 
					strcpy(model_type, token);
					printf("Model Type: %s\n", model_type);
					/* ------------------ object detection ------------------*/
					token = strtok(NULL, ", ");
					printf("1 token: %s\n", token);
					if (token != NULL) {
						// check model combination rules
						if (strcmp(model_type, "OBJECT_DETECTION") == 0) {
							if (strcmp(token, "NA_MODEL") == 0 || strstr(token, "YOLO") == NULL) {
								goto error_combination;
							}
							// check customized od model
							if (strstr(token, key_amb_customized) != NULL) {
#if PRINT_DEBUG
								printf("od key_amb_customized\n");
								printf("customized od: %s\n", input2model(token));
#endif
								extractRootDirectory(path_example, dir_example);
#if PRINT_DEBUG
								printf("customized dir: %s\n", dir_example);
#endif
								DIR* dir;
								struct dirent* ent;
								int count = 0;

								// check weather dir is valid
								if ((dir = opendir(dir_example)) != NULL) {
									/* print all the files and directories within directory */
									while ((ent = readdir(dir)) != NULL) {
										if (ent->d_type == DT_REG) {
											count++;
										}
										if (strstr(ent->d_name, ".nb") != NULL) {
											if (strstr(ent->d_name, "yolo") != NULL) {
#if PRINT_DEBUG
												printf("%s\n", ent->d_name);
#endif
											}
											else {
												goto error_customized_mismatch;
											}
										}
									}
								}
								if (count <= 1) {
									goto error_customized_missing;
								}
							}
						}
						strcpy(model_name_od, token);
						/* ----------------- face detection -----------------*/
						token = strtok(NULL, ", ");	
						printf("2 token: %s\n", token);
						if (token != NULL) {
							// check model combination rules
							if (strcmp(model_type, "FACE_DETECTION") == 0) {
								if (strcmp(token, "NA_MODEL") == 0 || strstr(token, "SCRFD") == NULL) {
									goto error_combination;
								}
								// check customized fd model
								if (strstr(token, key_amb_customized) != NULL) {
									printf("fd key_amb_customized\n");
									printf("customized fd: %s\n", input2model(token));
									extractRootDirectory(path_example, dir_example);
									printf("customized dir: %s\n", dir_example);

									DIR* dir;
									struct dirent* ent;
									int count = 0;

									// check weather dir is valid
									if ((dir = opendir(dir_example)) != NULL) {
										/* print all the files and directories within directory */
										while ((ent = readdir(dir)) != NULL) {
											if (ent->d_type == DT_REG) {
												count++;
											}
											if (strstr(ent->d_name, ".nb") != NULL) {
												if (strstr(ent->d_name, "scrfd") != NULL) {
#if PRINT_DEBUG
													printf("%s\n", ent->d_name);
#endif
												}
												else {
													goto error_customized_mismatch;
												}
											}
										}
									}
									if (count <= 1) {
										goto error_customized_missing;
									}
								}
							}
							strcpy(model_name_fd, token);
							/*-------------- face recognition --------------*/
							token = strtok(NULL, ", "); 
							printf("3 token: %s\n", token);
							// check model combination rules
							if (strcmp(model_type, "FACE_RECOGNITION") == 0) {
								if (strcmp(model_name_fd, "NA_MODEL") == 0 || strstr(model_name_fd, "SCRFD") == NULL || strcmp(token, "NA_MODEL") == 0 || strstr(token, "MOBILEFACENET") == NULL) {
									goto error_combination;
								}
								// check customized fr model
								if (strstr(token, key_amb_customized) != NULL) {
									printf("fr key_amb_customized\n");
									printf("customized fr: %s\n", input2model(token));
									extractRootDirectory(path_example, dir_example);
									printf("customized dir: %s\n", dir_example);

									DIR* dir;
									struct dirent* ent;
									int count = 0;

									// check weather dir is valid
									if ((dir = opendir(dir_example)) != NULL) {
										/* print all the files and directories within directory */
										while ((ent = readdir(dir)) != NULL) {
											if (ent->d_type == DT_REG) {
												count++;
											}
											if (strstr(ent->d_name, ".nb") != NULL) {
												if (strstr(ent->d_name, "mobilefacenet") != NULL) {
#if PRINT_DEBUG
													printf("%s\n", ent->d_name);
#endif
												}
												else {
													goto error_customized_mismatch;
												}
												}
											}
										}
									if (count <= 1) {
										goto error_customized_missing;
									}
								}
							}			
							if (token != NULL) {		 
								strcpy(model_name_fr, token);
							}
						}
					}

					/* default settings for all models */
					else {
						/* provide default settings for all models if user never provide any selections*/
						if (strcmp(model_type, "OBJECT_DETECTION") == 0 && strcmp(input2model(model_name_od), "NA") == 0) {
							printf("111\n");
							strcpy(model_name_od, "DEFAULT_YOLOV4TINY");
							strcpy(model_name_fd, "NA_MODEL");
							strcpy(model_name_fr, "NA_MODEL");
						}
						if (strcmp(model_type, "FACE_DETECTION") == 0 && strcmp(input2model(model_name_od), "NA") == 0) {
							printf("222\n");
							strcpy(model_name_od, "NA_MODEL");
							strcpy(model_name_fd, "DEFAULT_SCRFD");
							strcpy(model_name_fr, "NA_MODEL");
						}
						if (strcmp(model_type, "FACE_RECOGNITION") == 0 && strcmp(input2model(model_name_od), "NA") == 0) {
							printf("333\n");
							strcpy(model_name_od, "NA_MODEL");
							strcpy(model_name_fd, "DEFAULT_SCRFD");
							strcpy(model_name_fr, "DEFAULT_MOBILEFACENET");
						}
					}
				}
				printf("-------------------------------------\n");
				printf("Model Name OD: %s\n", input2model(model_name_od));
				printf("Model Name FD: %s\n", input2model(model_name_fd));
				printf("Model Name FR: %s\n", input2model(model_name_fr));
				printf("-------------------------------------\n");
				updateTXT(input2model(model_name_od));
				updateTXT(input2model(model_name_fd));
				updateTXT(input2model(model_name_fr));
			}
		}
		fclose(f_model);
	}
	else {
		printf("[%s][Error] 1 Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}		
	// update NA for non-NN examples
	if (strlen(model_name_od) == 0 && strlen(model_name_fd) == 0 && strlen(model_name_fr) == 0) {
		for (int i = 0; i < 3; i++) {
				updateTXT("NA");
			}
	}
		
	updateTXT("-----------------------------------");
	updateTXT("Current NN header file(s): ");

	FILE* f_headerNN = fopen(path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");
	if (f_headerNN) {
		char line[1024];
		while (fgets(line, sizeof(line), f_headerNN)) {
			/* check whether keywordNN in f_model content */
			if (strstr(line, key_amb_header) != NULL && strstr(line, "NN") != NULL) {
				extractString(line, line_strip_headerNN);	// remove unnecessary part in the header
#if PRINT_DEBUG
				printf("Extracted string: %s\n", line_strip_headerNN);
#endif
			}
		}
		fclose(f_headerNN);
	}
	else {
		printf("[%s][Error] 2 Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}
	updateTXT(line_strip_headerNN);

	updateTXT("----------------------------------");
	updateTXT("Current ino video status:");

	FILE* f_VOE = fopen(path, "r");  //FILE* f_model = fopen(path, "r, ccs=UTF-8");
	if (f_VOE) {
		char line[1024];
		while (fgets(line, sizeof(line), f_VOE)) {
			/* check whether keywordNN in f_model content */
			if (strstr(line, key_amb_VOE) != NULL && strstr(line, "//") == NULL && strstr(line, key_amb_bypassVOE1) == NULL && strstr(line, key_amb_bypassVOE2) == NULL) {
#if PRINT_DEBUG
				printf("f_VOE Extracted string: %s\n", line);
#endif
				strcpy(voe_status,"ON");		
			}	
		}
		fclose(f_VOE);					// update NA for video status
	}
	else {
		printf("[%s][Error] Failed to open the f_model.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}		
	updateTXT(voe_status);
	
	updateTXT("-------------------------------------");
	updateTXT("Current ino contains header file(s): ");

	FILE* f_header = fopen(path, "r");  //FILE* f_model = fopen(path, "r, ccs=UTF-8");
	if (f_header) {
		char line[1024];
		while (fgets(line, sizeof(line), f_header)) {
			/* check whether keywordNN in f_model content */
			if (strstr(line, key_amb_header) != NULL) {
				// remove unnecessary part in the header
				//char line_strip[100];
				extractString(line, line_strip_header);
#if PRINT_DEBUG
				printf("Extracted string: %s\n", line_strip);
#endif
				updateTXT(line_strip_header);
			}
		}
		fclose(f_header);
	}
	else {
		printf("[%s][Error] Failed to open the f_model.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}
	if (strcmp(line_strip_header,"NA") == NULL) {
		updateTXT(line_strip_header);
	}
	updateTXT("--------------------------------------");

	return 0;

error_combination:
	error_handler("Model combination mismatch. Please check modelSelect() again.");

error_customized_missing:
	error_handler("Model missing. Please check your sketch folder again.");

error_customized_mismatch:
	error_handler("Customized model mismatch. Please check your sketch folder again.");
}