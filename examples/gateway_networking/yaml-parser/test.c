#include "./yaml.h"

void test() {
	// YAML
	FILE *fp;
 	fp = fopen("./sample.yaml", "r");
	char * buff = 0;
	readFile(fp, &buff);
	int len = fountAt(buff, '\0');
  int got = 0;
  char dbuff[500];
  // check if more data is available 
  setbuf(stdout, NULL);
  // got = getVal(dbuff, buff, len, "response.data");
  // if (got) printf("%s\n", dbuff);

  // printf("%s\n","======================");
  // printf("%s\n", buff);
  // printf("%s\n","-------------------");

  // printf("%s\n","-------------------");
  // got = getVal(dbuff, buff, len, "src.addr");
  // if (got) printf("%s\n", dbuff);
  // printf("%s\n","======================");
//   printf("%s\n", buff);
//   printf("%s\n","-------------------");

  // printf("%s\n","-------------------");
  // got = getVal(dbuff, buff, len, "src.port");
  // if (got) printf("%s\n", dbuff);
  // printf("%s\n","======================");
//   printf("%s\n", buff);
//   printf("%s\n","-------------------");

  // printf("%s\n","-------------------");
  got = getVal(dbuff, buff, len, "type");
  if (got) printf("%s\n", dbuff);
  
  got = getVal(dbuff, buff, len, "proto");
  if (got) printf("%s\n", dbuff);
  // printf("%s\n","======================");
  // printf("%s\n", buff);
  // printf("%s\n","-------------------");
}

int main(int argc, char const *argv[])
{
	test();
	return 0;
}