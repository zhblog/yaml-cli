#include <stdio.h>
#include <stdlib.h>
#include "include/cJSON.h"

static void test(cJSON *json){
				json = cJSON_GetObjectItem(json,"son");
}

static void doit(const char *text)
{
		cJSON *big;
		cJSON *json;
		char *out;
		big=cJSON_Parse(text);
		if (!big) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
		else
		{
				json = cJSON_GetObjectItem(big,"cmdstree");
				json = cJSON_GetArrayItem(json,1);
				out = cJSON_Print(json);
				//printf("|\n%s\n|\n",out);
				free(out);
				test(json);
				out = cJSON_Print(json);
				//printf("|\n%s\n|\n",out);
				free(out);
				//while(json && json->type != cJSON_NULL){
				//		printf("%s\n",cJSON_GetObjectItem(json,"name")->valuestring);
				//		json = cJSON_GetObjectItem(json,"son");
				//}
				cJSON_Delete(big);
		}
}

static void dofile(char *filename)
{
		FILE *f=fopen(filename,"rb");
		fseek(f,0,SEEK_END);
		long len=ftell(f);
		fseek(f,0,SEEK_SET);
		char *data=malloc(len+1);
		fread(data,1,len,f);
		fclose(f);

		doit(data);
		free(data);
}


int main (int argc, const char ** argv) {

        char *filename;
        filename = (char *)argv[1];
		dofile(filename);
		return 0;
}
