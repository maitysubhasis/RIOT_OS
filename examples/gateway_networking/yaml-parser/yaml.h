#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef YAML_PARSER_H
#define YAML_PARSER_H

int indent = 2;

// set all characters to a specific character
// start and end are included
char * setAll(char * str, char ch, int start, int end) {
	while(start <= end) {
		*(str + start) = ch;
		start++;
	}
	return str + end;
}

// calculate character count between a and b pointer
int len (char *a, char *b) {
	char * c = a;
	int len = 0;
	while(c != b) {
		c++;
		len++;
	}
	return len;
}

// finds character position as char pointer
// if not found return end null pointer
char * charPos(char * str, char ch) {
	while ((*str) != ch && (*str) != '\0') {
		str++;
	}
	return str;
}

// finds first character distance from current str pointer
int fountAt (char * str, char ch) {
	char *p = charPos(str, ch);
	return len(str, p);
}

void copy (char * dst, char * src, int len) {
	while (len--) {
		(*dst) = (*src);
		dst++;
		src++;
	}
}

void printKeys(char * keys, int count) {
	char * c = keys;
	while(count--) {
		printf("%s.", c);
		c = strchr(c, '\0');
		if (count) c++;
	}
	printf("\n");
}
// mutates
int splitKeys(char * str) {
	int keyCount = 0;
	if (str[0] != '\0') {
		keyCount++;
	}
	int index = -1;
  
	while (str[++index] != '\0') {
		if (str[index] == '.') {
			keyCount++;
			str[index] = '\0';
		}
	}
	return keyCount;
}

char * getKeyFromIndex(char * str, int index) {
	char *c = str;
	while (index) {
		c += 1;
		if (*c == '\0') {
			index--;
			c++;
		}
	}
	return c;
}

void strToBuff (char * buff, char * str) {
	while ((*str) != '\0') {
		*buff = (*str) != '.' ? *str : '\0';
		buff++;
		str++;
	}
}

// int getNextData(char * dataPtr, char * parentIndent) {
// 	// return 1 if next data exists
// }

void readFile (FILE * f, char ** buffer) {
	long length;
	if (f) {
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  *buffer = (char *) malloc(length);
	  if (*buffer) {
	    fread (*buffer, 1, length, f);
	  }
	  fclose (f);
	}
}

// finds the value and sets to val
int getValue(char ** cLine, char *keys, int keyCount, int pos, char * parentIndent, int length) {
	if (length <= 0) return 0;
	// printf("%d\n", length);
	// find the first key with indent parentIndent
	// then recursive ly find the next key val
	// when pos === keyCount return data pointer 
	uint separatorPos;
	int curLineLen;
	char * cPos = *cLine;
	char * endIndent;
	char *key = getKeyFromIndex(keys, pos);
	// printf("Key: %s\n", key);
	if (strncmp(cPos, parentIndent, pos*indent) == 0) {
		// printf("%s %d\n", key, pos*indent);
		cPos += pos*indent;
		// checkif key match
		separatorPos = fountAt(cPos, ':');
		if (strncmp(cPos, key, separatorPos) == 0 && strlen(key) == separatorPos) {
			// printf("%s %d\n", key, pos*indent);
			// printf("%d\n", separatorPos);
			// matched first key
			// go to next line
			pos++;
			if (keyCount == pos) {
				return 1;
			}
			endIndent = setAll(parentIndent, ' ', (pos-1) * indent, pos* indent);
			*endIndent = '\0';
			curLineLen = fountAt(*cLine, '\n');
			length -= curLineLen;
			length--;
			*cLine += curLineLen + 1;
			// printf("%d\n", separatorPos);
			// length -= nextLine(&cLine, length);
			return getValue(cLine, keys, keyCount, pos, parentIndent, length);
		} else {
			curLineLen = fountAt(*cLine, '\n');
			length -= curLineLen;
			length--;
			*cLine += curLineLen + 1;
			// length -= nextLine(&cLine, length);
			// setAll(parentIndent, )
			return getValue(cLine, keys, keyCount, pos, parentIndent, length);
		}
	} else {
			curLineLen = fountAt(*cLine, '\n');
			length -= curLineLen;
			length--;
			*cLine += curLineLen + 1;
			// length -= nextLine(&cLine, length);
			// setAll(parentIndent, )
			return getValue(cLine, keys, keyCount, pos, parentIndent, length);
	}
	// printf("%s\n", "ret 0");
	return 0;
}

void stripSpace (char ** str) {
	while ((**str) == ' ') {
		(*str)++;
	}
}

int getVal(char * dbuff, char * data, int len, char * _keys) {
  char keys[255];
  char parentIndent[100];
  char * fPos = data;
  int f = 0;
  int dataLen = 0;
	copy(keys, _keys, strlen(_keys) + 1);
	int keyCount = splitKeys(keys);
  f = getValue(&fPos, keys, keyCount, 0, parentIndent, len);
	if (f) {
	  data = charPos(fPos, ':');
	  data++;
	  stripSpace(&data);
	  dataLen = fountAt(data, '\n');
      // printf("--%s, %d\n", data, dataLen);
	  copy(dbuff, data, dataLen);
	  dbuff[dataLen] = '\0';
	  return 1;
  } else {
  	// printf("%s\n", _keys);
  	return 0;
  }
  return 0;
}

#endif