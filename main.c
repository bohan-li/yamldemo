
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cyaml/cyaml.h>

struct parameters {
	unsigned int *windowSize;

	int exposureTime;
};

struct top {
	const char *commandName;
	unsigned long time;
	
	struct parameters *params;
};

static const cyaml_schema_value_t intListValueSchema = {
	CYAML_VALUE_INT(CYAML_FLAG_DEFAULT, int),
};

/* CYAML mapping schema fields array for the top level mapping. */
static const cyaml_schema_field_t parametersSchema[] = {
	CYAML_FIELD_UINT(
			"ExposureTime", CYAML_FLAG_OPTIONAL,
			struct parameters, exposureTime),
	CYAML_FIELD_SEQUENCE_FIXED("WindowSize", CYAML_FLAG_POINTER,
			struct parameters, windowSize, &intListValueSchema,
			2),
	CYAML_FIELD_END
};

/* CYAML mapping schema fields array for the top level mapping. */
static const cyaml_schema_field_t topMappingSchema[] = {
	CYAML_FIELD_STRING_PTR("Command", CYAML_FLAG_POINTER,
			struct top, commandName,
			0, CYAML_UNLIMITED),
	CYAML_FIELD_UINT(
			"Time", CYAML_FLAG_DEFAULT,
			struct top, time),
	CYAML_FIELD_MAPPING_PTR("Parameters", CYAML_FLAG_POINTER,
			struct top, params, parametersSchema),
	CYAML_FIELD_END
};

static const cyaml_schema_value_t topSchema = {
	CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER,
			struct top, topMappingSchema),
};

/******************************************************************************
 * Actual code to load and save YAML doc using libcyaml.
 ******************************************************************************/

/* Our CYAML config.
 *
 * If you want to change it between calls, don't make it const.
 *
 * Here we have a very basic config.
 */
static const cyaml_config_t config = {
	.log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
	.log_fn = cyaml_log,            /* Use the default logging function. */
	.mem_fn = cyaml_mem,            /* Use the default memory allocator. */
};

/* Main entry point from OS. */
int main(int argc, char *argv[])
{
	char *string = "Command: CameraSnapshot\nTime: 1585215144500\nParameters:\n  WindowSize:\n    - 1280\n    - 1024\n  ExposureTime: 10000\n BADDD";
	printf(string);

	cyaml_err_t err;
	struct top *n;
	enum {
		ARG_PROG_NAME,
		ARG_PATH_IN,
		ARG__COUNT,
	};
/*
	
	if (argc != ARG__COUNT) {
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "  %s <INPUT>\n", argv[ARG_PROG_NAME]);
		return EXIT_FAILURE;
	}
*/
	/* Load input file. */
	err = cyaml_load_data((uint8_t*) string, strlen(string), &config,
			&topSchema, (cyaml_data_t **)&n, NULL);
	if (err != CYAML_OK) {
		fprintf(stderr, "ERROR: %s\n", cyaml_strerror(err));
		return EXIT_FAILURE;
	}

	printf("My command name is %s\n", n->commandName);
	printf("My time is %lu\n", n->time);
	printf("My window size is %ux%u\n", n->params->windowSize[0], n->params->windowSize[1]);
	printf("My exposure time is %d\n", n->params->exposureTime);
	/* Free the data */
	cyaml_free(&config, &topSchema, n, 0);

	return EXIT_SUCCESS;
}
