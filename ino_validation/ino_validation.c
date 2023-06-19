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

#define PRINT_DEBUG 1
#define MAX_PATH_LENGTH 1024

/* Declear global vairables */
const char* key_ambNN					= "modelSelect";
const char* key_ambVOE					= "configVideoChannel";
const char* key_amb_header				= "#include";
const char* key_amb_customized			= "CUSTOMIZED";
const char* key_amb_default				= "DEFAULT";
const char* key_amb_default_backup		= "Dbackup";
const char* key_amb_customized_backup	= "Cbackup";
const char* key_amb_bypassNN1			= " .modelSelect";
const char* key_amb_bypassNN2			= " modelSelect"; // duplicated with key_ambNN
const char* key_amb_bypassVOE1			= " .configVideoChannel";
const char* key_amb_bypassVOE2			= " configVideoChannel";
const char* filename_txt				= "ino_validation.txt";

/* Declear function headers */
int dirExists(const char* directory_path);
const char* dirName(const char* directory_path);
int isDirExists(const char* path);

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
/* Update content in the input to TXT file */
void updateTXT(const char* input);
/* Repalce the single backslash to double in Windows*/
void replaceBackslash(char* str);
/* Similar function as REGEX*/
void extractParam(char* line, char* param);
/* Conveert model input type to model name*/
const char* input2model(const char* input);
// -------------------------------------------------------------

/* Conveert model input type to model filename*/
const char* input2filename(const char* dest_path, const char* input);

/* Conveert model input type to model header file*/
const char* input2header(const char* input);
/* Update content in the input to TXT file */
void updateNATXT(const char* filepath, const char* start_line, const char* end_line);


/* Declear common file paths */
char* path_arduino15_add	= "\\AppData\\Local\\Arduino15";
char* ambpro2_add			= "\\packages\\realtek\\hardware\\AmebaPro2\\";
char* model_add				= "\\variants\\common_nn_models";
char* txtfile_add			= "\\misc\\";
const char* path_build_options_json = NULL;
const char* path_example = NULL;
const char* name_example = NULL;
const char* ext_json = ".json";
const char* key_json = "build";
const char* key_amb = "Arduino15";
const char* key_ino = ".ino";

char path_root[MAX_PATH_LENGTH];
char arduino15_path[MAX_PATH_LENGTH];
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

	strcpy(path_root, getenv("USERPROFILE"));
	strcpy(arduino15_path, getenv("USERPROFILE"));
	strcat(arduino15_path, path_arduino15_add);
	strcpy(path_pro2, arduino15_path);
	strcat(path_pro2, ambpro2_add);
	strcpy(path_model, path_pro2);
	strcat(path_model, dirName(path_pro2));
	strcat(path_model, model_add);
	strcpy(path_txtfile, argv[2]);
	strcat(path_txtfile, txtfile_add);

	// Print the input parameters 
	printf("Parameter 1      = %s\n", path_build);
	printf("Parameter 2      = %s\n", path_tools);
	printf("USERPROFILE      = %s\n", getenv("USERPROFILE"));
	printf("HOMEDRIVE        = %s\n", getenv("HOMEDRIVE"));
	printf("HOMEPATH         = %s\n", getenv("HOMEPATH"));
	printf("path_root        = %s\n", path_root);
	printf("arduino15_path   = %s\n", arduino15_path);
	printf("path_pro2        = %s\n", path_pro2);
	printf("ver_pro2         = %s\n", dirName(path_pro2));
	printf("path_model       = %s\n", path_model);
	printf("path_txtfile     = %s\n", path_txtfile);

	resetTXT(path_txtfile);
	printf("[%s][INFO] resetTXT done\n", __func__);
	// generate path
	path_build_options_json = pathTempJSON(path_build, ext_json, key_json);
	printf("[%s][INFO] path_build_options_json = %s\n", __func__, path_build_options_json);

	path_example = validateINO(path_build);
	printf("[%s][INFO] path_example            = %s\n", __func__, path_example);

	// SECTION FOR FUNCTION TEST =======================================================
	writeTXT(path_example);


	exit(1);
	while (1);
	printf("111\n");
printf("222\n");

	// END OF SECTION FOR FUNCTION TEST =======================================================

	const char* input = "CUSTOMIZED_MOBILEFACENET";
	const char* model = input2model(input);
	printf("Model: %s\n", model);
	updateTXT("Hello, world!");


	// check whether directory exists
	if (isDirExists(path_build) && isDirExists(path_tools)) {
		
	}

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
	FILE* file = fopen(path_txtfile, "a");

	if (file) {
		fprintf(file, "%s\n", input);
		fclose(file);
	}
	else {
#if PRINT_DEBUG
		printf("[%s][Error] Failed to open the file.\n", __func__);
#endif	
		perror(path_txtfile);
		return EXIT_FAILURE;
	}
}

const char* pathTempJSON(const char* directory_path, const char* ext, const char* key) {
	DIR* dir;
	struct dirent* ent;

#if PRINT_DEBUG
	printf("[%s][INFO] Load json file \"%s\"\n", __func__, directory_path);
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

	// Open the JSON file and retrive the data
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

void writeTXT(const char* path) {
	DIR* dir;
	struct dirent* ent;
	const char buf[MAX_PATH_LENGTH] = "";
	const char backslash[] = "\\";
	char line[MAX_PATH_LENGTH] = {0};
	unsigned int line_count = 0;
	char model_type[100] = "";
	char model_name_od[100] = "";
	char model_name_fd[100] = "";
	char model_name_fr[100] = "";
	path = path_example;
	// replaceBackslash(path);

#if PRINT_DEBUG
	printf("[%s][INFO] Load example: \"%s\"\n", __func__, path);
#endif

	FILE* file = fopen(path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");
	char param[100];

	updateTXT("----------------------------------");
	updateTXT("Current ino contains model(s):");

	if(file) {
		char line[1024];
		while (fgets(line, sizeof(line), file)) {
			/* check whether keywordNN in file content */ 
			if (strstr(line, key_ambNN) != NULL && strstr(line, "//") == NULL && strstr(line, key_amb_bypassNN1) == NULL && strstr(line, key_amb_bypassNN2) == NULL){
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
						}
						// check customized model
						if (strstr(token, key_amb_customized) != NULL) {
							printf("key_amb_customized\n");
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
							}
							// check customized model
							if (strstr(token, key_amb_customized) != NULL) {
								printf("key_amb_customized\n");
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
							}
							// check customized model
							if (strstr(token, key_amb_customized) != NULL) {
								printf("key_amb_customized\n");
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
				updateTXT(input2model(model_name_od));
				updateTXT(input2model(model_name_fd));
				updateTXT(input2model(model_name_fr));
			}
		}
		fclose(file);
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}
			
	// update NA for non-NN examples
	if (strlen(model_name_od) == 0 && strlen(model_name_fd) == 0 && strlen(model_name_fr) == 0) {
		for (int i = 0; i < 3; i++) {
				updateTXT("NA");
			}
	}
		
	updateTXT("----------------------------------");
	updateTXT("Current NN header file(s): ");

	FILE* file2 = fopen(path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");
	if (file2) {
		char line[1024];
		printf("Extracted parameter: %s\n", line);
		while (fgets(line, sizeof(line), file2)) {
			/* check whether keywordNN in file content */
			if (strstr(line, key_amb_header) != NULL && strstr(line, "NN") != NULL) {
				printf("Extracted parameter: %s\n", line);
				updateTXT(line);
			}
		}
		fclose(file2);
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}

	updateTXT("----------------------------------");
	updateTXT("Current ino video status:");




	updateTXT("----------------------------------");
	updateTXT("Current ino contains header file(s): ");

	FILE* file4 = fopen(path, "r");  //FILE* file = fopen(path, "r, ccs=UTF-8");
	if (file4) {
		char line[1024];
		printf("Extracted parameter: %s\n", line);
		while (fgets(line, sizeof(line), file4)) {
			/* check whether keywordNN in file content */
			if (strstr(line, key_amb_header) != NULL) {
				printf("Extracted parameter: %s\n", line);
				updateTXT(line);
			}
		}
		fclose(file4);
	}
	else {
		printf("[%s][Error] Failed to open the file.\n", __func__);
		perror(path);
		return EXIT_FAILURE;
	}


	return 0;


	
error_combination:
	error_handler("Model mismatch. Please check modelSelect() again.");

error_customized:
	error_handler("Model not found. Please check your sketch folder again.");
}

