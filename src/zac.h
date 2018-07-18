//zach lib, random functions and data structures for use wherever
//Inspired by the stb library and talks by Sean Barrett

// Copyright (C) 2018 Zachary Wells
//
// This software may be modified and distributed under the terms
// of the MIT license. See the LICENSE file for details.

#ifndef __ZAC_INCLUDE_H
#define __ZAC_INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#define zac_max(a, b) ((a) > (b) ? (a) : (b))
#define zac_min(a, b) ((a) < (b) ? (a) : (b))

typedef unsigned char zac_byte;
typedef unsigned int  zac_uint;

#ifdef ZAC_IMPLEMENTATION

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#ifndef zac_assert
#define zac_assert(e, r) assert(e)
#endif
#ifndef zac_malloc
#define zac_malloc(s) malloc(s)
#endif
#ifndef zac_realloc
#define zac_realloc(p, s) realloc(p, s)
#endif
#ifndef zac_free
#define zac_free(p) free(p)
#endif

#endif

#ifdef ZAC_IMPLEMENTATION
#define Z(...) __VA_ARGS__
#else
#define Z(...)
#endif

//Resizeable Arrays
//std::vector replacement
typedef struct zac_array {
	int len, cap;
} zac_array;

#define zac_array_head(a)      (((zac_array*)(a))[-1])
#define zac_array_len(a)       ((a) ? zac_array_head(a).len : 0)
#define zac_array_cap(a)       ((a) ? zac_array_head(a).cap : 0)
#define zac_array_elem_size(a) (sizeof(*(a)))
#define zac_array_end(a) ((a) + zac_array_len(a) - 1)

#define zac_array_resize(a, c) ((a) = zac_array__alloc((zac_array*)(a), zac_array_elem_size(a), c))
#define zac_array_shrink(a)    (zac_array_resize(a, zac_array_len(a)))
#define zac_array_empty(a)     (zac_array_head(a).len = 0)
#define zac_array_free(a)      (zac_array_resize(a, 0))

#define zac_array_hasSpace(a, n) (zac_array_cap(a) >= (zac_array_len(a) + (n)))
#define zac_array_getSpace(a, n) ((zac_array_hasSpace(a, n) ? 0 : zac_array_resize(a, zac_array_len(a) + (n))), \
								zac_array_head(a).len += (n), zac_array_end(a) - (n) + 1)

#define zac_array_push(a, ...) ((zac_array_hasSpace(a, 1) ? 0 : zac_array_resize(a, zac_array_len(a)*3/2+1)), \
								zac_array_head(a).len++, *zac_array_end(a) = (__VA_ARGS__))

void *zac_array__alloc(zac_array *arr, int size, int cap) Z({
	if (cap == 0) {
		zac_free(arr ? (arr-1) : arr);
		return NULL;
	}
	
	int len = zac_array_len(arr);
	
	zac_array *out = zac_realloc(arr ? (arr-1) : arr, sizeof(zac_array) + (size * cap));
	zac_assert(out != NULL, "Allocation failed!");
	out->len = zac_min(len, cap);
	out->cap = cap;
	return out + 1;
});


//Hash Map (tbd)
typedef unsigned long zac_hash;

zac_hash zac_hash_str(const char *str) Z({
	zac_hash hash = 5381, c;
	
	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c;
	}
	
	return hash;
});


//File IO helper functions
typedef void zac_file; //Wrapper for C FILE*

int zac_io_getSize(const zac_file *file) Z({
	FILE *fp = (FILE*)file;
	int pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, pos, SEEK_SET);
	return len;
});

//Read entire file as ascii char array
char *zac_io_reads(const char *fileName, int *length) Z({
	FILE *file = fopen(fileName, "rb");
	int len = -1;
	char *out = NULL;
	
	if (file != NULL) {
		len = zac_io_getSize(file);
		
		out = zac_malloc(sizeof(char) * len + 1);
		fread(out, sizeof(char), len, file);
		out[len] = '\0';
		
		fclose(file);
	}
	
	if (length != NULL) {
		*length = len;
	}
	
	return out;
});
int zac_io_readsb(char *buffer, int length, const char *fileName) Z({
	FILE *file = fopen(fileName, "rb");
	int len = -1;
	
	if (file != NULL) {
		len = zac_min(zac_io_getSize(file), length - 1);
		
		fread(buffer, sizeof(char), len, file);
		buffer[len++] = '\0';
		
		fclose(file);
	}
	
	return len;
});
//TODO: make array parameter wrapper
// int zac_io_readsa(char *array, const char *fileName) Z({
// 	FILE *file = fopen(fileName, "rb");
// 	int len = -1;
	
// 	if (file != NULL) {
// 		len = zac_io_getSize(file);
		
// 		fread(zac_array_getSpace(array, len + 1), sizeof(char), len, file);
// 		array[len++] = '\0';
		
// 		fclose(file);
// 	}
	
// 	return len;
// });

//GL Utility Functions
#ifdef ZAC_GL_UTIL

typedef struct zac_vec2 {
	float x, y;
} zac_vec2;

typedef struct zac_vec3 {
	float x, y, z;
} zac_vec3;

typedef struct zac_gl_obj {
	char *name;
	zac_vec3 *vertices;
	zac_vec2 *uvs;
	zac_vec3 *normals;
} zac_gl_obj;

zac_gl_obj *zac_gl_parseObjb(const char *buffer, int length) Z({
	
	zac_gl_obj *out = NULL;
	zac_gl_obj *curObj = zac_array_getSpace(out, 1);
	curObj->name     = NULL;
	curObj->vertices = NULL;
	curObj->uvs      = NULL;
	curObj->normals  = NULL;
	
	zac_vec3 *vertices = NULL;
	zac_vec2 *uvs      = NULL;
	zac_vec3 *normals  = NULL;
	int *indices = NULL;
	
	int i = 0;
	
	while (i < length) {
		switch (buffer[i]) {
			case 'o': {
				//push object to return array
				//Actually split by material, not object
				if (curObj->name != NULL) {
					curObj = zac_array_getSpace(out, 1);
					curObj->name     = NULL;
					curObj->vertices = NULL;
					curObj->uvs      = NULL;
					curObj->normals  = NULL;
				}
				int begin, end;
				while (isspace(buffer[++i]));
				begin = i;
				while (buffer[i++] != '\n');
				end = i - 1;
				int length = end - begin;
				
				curObj->name = zac_malloc(sizeof(char) * length);
				
				if (curObj->name != NULL) {
					strncpy(curObj->name, buffer + begin, length);
					curObj->name[length] = '\0';
				}
				i--;
			}
			break;
			
			case 'v': {
				i++;
				switch (buffer[i]) {
					case ' ': { //vertex
						i++;
						zac_vec3 vec;
						char *nextFloat;
						vec.x = strtof(buffer + i, &nextFloat);
						vec.y = strtof(nextFloat, &nextFloat);
						vec.z = strtof(nextFloat, &nextFloat);
						zac_array_push(vertices, vec);
						i = nextFloat - buffer;
					}
					break;
					
					case 't': { //uv
						i++;
						zac_vec2 vec;
						char *nextFloat;
						vec.x = strtof(buffer + i, &nextFloat);
						vec.y = 1 - strtof(nextFloat, &nextFloat);
						zac_array_push(uvs, vec);
						i = nextFloat - buffer;
					}
					break;
					
					case 'n': { //normal
						i++;
						zac_vec3 vec;
						char *nextFloat;
						vec.x = strtof(buffer + i, &nextFloat);
						vec.y = strtof(nextFloat, &nextFloat);
						vec.z = strtof(nextFloat, &nextFloat);
						zac_array_push(normals, vec);
						i = nextFloat - buffer;
					}
					break;
				}
			}
			break;
			
			case 'f': {
				i++;
				char *nextInt;
				int temp, current = 0;
				while ((temp = strtol(buffer + i, &nextInt, 10))) {
					i = nextInt - buffer + 1;
					
					if (current >= 3) {
						current = 0;
					}
					
					temp = abs(temp);
					
					zac_array_push(indices, temp);
					current++;
					
					if (isspace(nextInt[0]) || nextInt[0] == '\0') {
						for (;current < 3; current++) {
							zac_array_push(indices, 0);
						}
					} else if (nextInt[0] == '/' && nextInt[1] == '/') {
						zac_array_push(indices, 0);
						current++;
						i++;
					}
				}
				i--;
				
				int count = zac_array_len(indices) / 3;
				
				if (count == 4) {
					struct _index {
						int vert, uv, norm;
					};
					
					struct _index int_0 = ((struct _index*)indices)[0];
					struct _index int_2 = ((struct _index*)indices)[2];
					zac_array_push(indices, int_0.vert);
					zac_array_push(indices, int_0.uv);
					zac_array_push(indices, int_0.norm);
					
					zac_array_push(indices, int_2.vert);
					zac_array_push(indices, int_2.uv);
					zac_array_push(indices, int_2.norm);
					
				} else if (count > 4) {
					//TODO: triangulate ngons
					zac_array_empty(indices);
					break;
				}
				
				for (int j = 0; j < zac_array_len(indices); j+=3) {
					int vert_int = indices[j + 0];
					int uv_int   = indices[j + 1];
					int norm_int = indices[j + 2];
					
					zac_array_push(curObj->vertices, vertices[vert_int - 1]);
					
					if (uv_int == 0) {
						zac_array_push(curObj->uvs, (zac_vec2){0, 0});
					} else {
						zac_array_push(curObj->uvs, uvs[uv_int - 1]);
					}
					
					if (norm_int == 0) {
						//TODO: calculate normals
						zac_array_push(curObj->normals, (zac_vec3){0, 0, 0});
					} else {
						zac_array_push(curObj->normals, normals[norm_int - 1]);
					}
				}
				zac_array_empty(indices);
			}
			break;
		}
		while (buffer[i++] != '\n');
	}
	
	zac_array_free(vertices);
	zac_array_free(uvs);
	zac_array_free(normals);
	zac_array_free(indices);
	
	return out;
});
zac_gl_obj *zac_gl_parseObj(const char *fileName) Z({
	int length;
	char *file = zac_io_reads(fileName, &length);
	if (length == -1) {
		return NULL;
	}
	
	zac_gl_obj *out = zac_gl_parseObjb(file, length);
	zac_free(file);
	
	return out;
});
void zac_gl_freeObj(zac_gl_obj *objs) Z({
	for (int i = 0; i < zac_array_len(objs); i++) {
		zac_free(objs[i].name);
		zac_array_free(objs[i].vertices);
		zac_array_free(objs[i].uvs);
		zac_array_free(objs[i].normals);
	}
	zac_array_free(objs);
});

#endif

#undef Z

//Undefine just incase
#ifdef ZAC_IMPLEMENTATION
#undef ZAC_IMPLEMENTATION
#endif

#ifdef __cplusplus
}
#endif
#endif