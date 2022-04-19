/*
Copyright 2021-2022 Dotz0cat

This file is part of walld.

    walld is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    walld is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with walld.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "list.h"

linked_node* add_node_to_list(linked_node* prev, char* data) {
	if (prev == NULL) {
		linked_node* node = malloc(sizeof(linked_node));

		node->image = data;

		node->next = NULL;

		return node;
	}
	else {
		if (prev->image == NULL) {
			prev->image = data;
			prev->next = NULL;

			return prev;
		}
		else {
			linked_node* node = malloc(sizeof(linked_node));

			prev->next = node;

			node->image = data;

			node->next = NULL;

			return node;
		}
	}
}

linked_node* wind_to_tail(linked_node* node) {
	while (node->next != NULL) {
		node = node->next;
	}

	return node;
}

linked_node* wind_to_x(linked_node* node, int x) {
	linked_node* head = node;

	for (int i = 0; i < x; i++) {
		head = head->next;		
	}

	return head;
}

void free_list(linked_node* head) {
	linked_node* free_head;
	linked_node* temp;

	free_head = head;

	while (free_head != NULL) {
		if (free_head->image != NULL) {
			free(free_head->image);
		}
		temp = free_head;
		free_head = free_head->next;
		free(temp);
	}
}

void free_circular_list(linked_node* head) {
	linked_node* free_head;
	linked_node* temp;

	free_head = head;

	do {
		if (free_head->image != NULL) {
			free(free_head->image);
		}
		temp = free_head;
		free_head = free_head->next;
		free(temp);
	} while (free_head != NULL && free_head != head);
}

linked_node* shuffle(linked_node* head) {
	int count = 0;
	linked_node* count_head = head;
	do {
		count++;
		count_head = count_head->next;
	} while (count_head != NULL && count_head != head);

	linked_node** array = malloc(count * sizeof(linked_node*));

	int i = 0;

	linked_node* add_head = head;

	for (i = 0; i < count; i++) {
		array[i] = add_head;
		add_head = add_head->next;
	}

	srand48(time(NULL));



	for (int j = count - 1; j > 0; j--) {
		int random = lrand48() % (j+1);

		array_swap(&array[j], &array[random]);
	}

	for (int k = 0; k < count - 1; k++) {
		relink(array[k], array[k + 1]);
	}

	linked_node* new_head = array[0];

	array[count - 1]->next = new_head;

	free(array);

	return new_head;
}

static inline void relink(linked_node* prev, linked_node* next) {
	if (prev != NULL && next != NULL) {
		prev->next = next;
	}
}

void array_swap(linked_node** a, linked_node** b) {
	linked_node* temp = *a;
	*a = *b;
	*b = temp;
}

linked_node* get_images(linked_node* source) {
	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	do {
		file_type type = get_file_type(source->image);

		switch(type) {
			case (DIRECTORY): {
				linked_node* dir_list;
				dir_list = list_files_full(source->image);

				if (dir_list != NULL) {
					linked_node* typed = get_images(dir_list);

					if (typed != NULL) {
						if (current == entry_point) {
							free(entry_point);

							entry_point = typed;

							current = entry_point;

							current = wind_to_tail(current);
						}
						else {
							current->next = typed;

							current = wind_to_tail(current);
						}
					}
					free_list(dir_list);
				}
				break;
			}	
			case(LIST): {
				linked_node* file_contents = list_file_parse(source->image);

				if (file_contents != NULL) {
					linked_node* typed = get_images(file_contents);

					if (typed != NULL) {
						if (current == entry_point) {
							free(entry_point);

							entry_point = typed;

							current = entry_point;

							current = wind_to_tail(current);
						}
						else {
							current->next = typed;

							current = wind_to_tail(current);
						}
					}
					free_list(file_contents);
				}
				break;
			}
			case(IMAGE): {
				current = add_node_to_list(current, strdup(source->image));

				break;
			}
			case(ERROR):
			default: {
			break;
			}
		}
	} while ((source = source->next) != NULL);

	if (entry_point->image == NULL) {
		free(entry_point);
		return NULL;
	}

	return entry_point;
}

file_type get_file_type(const char* path) {
	if (path == NULL) {
		return ERROR;
	}
	else if (strcmp(path, "") == 0) {
		return ERROR;
	}

	DIR* dir = opendir(path);

	if (dir != NULL) {
		closedir(dir);
		return DIRECTORY;
	}

	if (errno == ENOENT) {
		regex_t regex;
		if (regcomp(&regex, "^\\(http\\|https\\|ftp\\):\\/\\/\\(www\\.\\)\\?[a-z0-9A-z\\.:]\\{2,256\\}", REG_ICASE) != 0) {
			return ERROR;
		}

		if (regexec(&regex, path, 0, NULL, 0) == 0) {
			regfree(&regex);
			return IMAGE;
		}

		regfree(&regex);
		
		return ERROR;
	}
	else if (errno == ENOTDIR) {
		return image_or_text(path);
	}
	else {
		return ERROR;
	}
}

linked_node* list_files_full(const char* directory) {
	struct dirent* d;

	DIR* dir;

	dir = opendir(directory);

	linked_node* entry_point = add_node_to_list(NULL, NULL);

	linked_node* current = entry_point;

	if (dir != NULL) {
		while ((d = readdir(dir)) != NULL) {
			if (strcmp(d->d_name, ".") != 0) {
				if (strcmp(d->d_name, "..") != 0) {
					char* real = realpath_wrap(d->d_name, directory);

					if (real != NULL) {
						current = add_node_to_list(current, real);
					}
				}
			}
		}
	}
	else {
		free(entry_point);
		return NULL;
	}

	closedir(dir);

	if (entry_point->image == NULL) {
		free(entry_point);
		return NULL;
	}
	

	return entry_point;
}

char* realpath_wrap(const char* path, const char* dir) {
	int count = snprintf(NULL, 0, "%s%s%s", dir, "/", path);

	if (count <= 0) {
		return NULL;
	}

	char* longer_path = malloc(count + 1U);

	if (longer_path == NULL) {
		return NULL;
	}

	snprintf(longer_path, count + 1U, "%s%s%s", dir, "/", path);

	char* res = realpath(longer_path, NULL);

#if defined(PATH_MAX) && PATH_MAX > 0

	if (res == NULL && longer_path != NULL && errno == EINVAL) {

		char* resolved_path = malloc(PATH_MAX);

		if (resolved_path != NULL) {
			char* tmp = realpath(longer_path, resolved_path);

			if (tmp != NULL) {
				res = strdup(tmp);
				free(tmp);
			}

			free(resolved_path);
		}
	}

#endif

	free(longer_path);

	return res;
}

char** list_to_null_termed_string_array(linked_node* head, size_t* len) {
	int count = 0;
	linked_node* count_head = head;
	do {
		count++;
		count_head = count_head->next;
	} while (count_head != NULL && count_head != head);

	count++;

	char** array = malloc(count * sizeof(char*));

	*len = count;

	int i = 0;

	linked_node* add_head = head;

	for (i = 0; i < count - 1; i++) {
		array[i] = strdup(add_head->image);
		add_head = add_head->next;
	}

	array[count - 1] = NULL;

	return array;
}
