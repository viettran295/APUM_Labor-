// libsmlTest.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#ifdef WIN32
	#include <crtdbg.h>
#endif

#include <sml/sml_file.h>
#include <sml/sml_transport_file.h>
#include <sml/sml_log.h>
#include <sml/sml_signature.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int VerifySmlSignatures(sml_file *smlFile)
{
	int i;

	for (i = 0; i < smlFile->messages_len; i++)
	{
		sml_message *smlMessage = smlFile->messages[i];
		
		if (*(smlMessage->message_body->tag) == SML_MESSAGE_GET_PROC_PARAMETER_RESPONSE)
		{
			sml_get_proc_parameter_response *smlGetProcParameterResponse = smlMessage->message_body->d.get_proc_parameter_response;
			if (smlGetProcParameterResponse && smlGetProcParameterResponse->parameter_tree)
			{
				if (smlGetProcParameterResponse->parameter_tree->parameter_value)
				{
					sml_proc_par_value *smlProcParValue = smlGetProcParameterResponse->parameter_tree->parameter_value;
					if (*(smlProcParValue->tag) == SML_PROC_PAR_VALUE_TAG_TUPEL_ENTRY)
					{
						// SYM2
						unsigned char buf_pA[24] = {0};
						unsigned char buf_mA[24] = {0};

						sml_tupel_entry_build_signed_content(smlProcParValue->data.tupel_entry, buf_pA, sizeof(buf_pA), buf_mA, sizeof(buf_mA));
					}
					else if (*(smlProcParValue->tag) == SML_PROC_PAR_VALUE_TAG_LIST_ENTRY)
					{
						sml_list_entry *smlListEntry = smlProcParValue->data.list_entry;
						if (smlListEntry->value_signature)
						{
							octet_string *valueSignature = NULL;
							octet_string *logBook = NULL;
							octet_string *signedContent = NULL;

							if (smlListEntry->value_signature->len == 50)
							{
								// EDL
								int len = 48;

								valueSignature = sml_octet_string_init(smlListEntry->value_signature->str, len);
								logBook = sml_octet_string_init(smlListEntry->value_signature->str + 48, 2);

								signedContent = sml_list_entry_build_signed_content(SML_MESSAGE_TYPE_EDL, smlGetProcParameterResponse->server_id, smlListEntry, logBook);
							}
							else if (smlListEntry->value_signature->len == 96)
							{
								// BZ ... first approach ... obviously wrong!!!
								int len = 96;

								valueSignature = sml_octet_string_init(smlListEntry->value_signature->str, len);

								signedContent = sml_list_entry_build_signed_content(SML_MESSAGE_TYPE_BZ_SLP, smlGetProcParameterResponse->server_id, smlListEntry, NULL);
							}

							if (valueSignature)
							{
								sml_octet_string_free(valueSignature);
							}
							if (logBook)
							{
								sml_octet_string_free(logBook);
							}
							if (signedContent)
							{
								sml_octet_string_free(signedContent);
							}
						}
					}
				}
				else if (smlGetProcParameterResponse->parameter_tree->child_list)
				{
					// BZ ... EMH compliant
					for (int i = 0; i < smlGetProcParameterResponse->parameter_tree->child_list_len; i++)
					{
						sml_tree *smlTree = smlGetProcParameterResponse->parameter_tree->child_list[i];
						if (smlTree && smlTree->parameter_name)
						{
							unsigned char attribute[2] = { 0x00, 0x06 };
							if (memcmp(smlTree->parameter_name->str, attribute, sizeof(attribute)) == 0)
							{
								sml_proc_par_value *smlProcParValue = smlTree->parameter_value;
								if (smlProcParValue && (*(smlProcParValue->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
								{
									sml_value *smlValue = smlProcParValue->data.value;
									if (smlValue->type == SML_TYPE_OCTET_STRING)
									{
										octet_string *signature = NULL;
										octet_string *signedContent = NULL;

										signature = sml_octet_string_init(smlValue->data.bytes->str, smlValue->data.bytes->len);
										sml_octet_string_free(signature);

										signedContent = sml_tree_build_signed_content(SML_MESSAGE_TYPE_BZ_SLP, smlGetProcParameterResponse->server_id, smlGetProcParameterResponse->parameter_tree);

										break;
									}
								}
							}
						}
					}
				}

				// public key for EDL and BZ(?)
				for (int i = 0; i < smlGetProcParameterResponse->parameter_tree->child_list_len; i++)
				{
					sml_tree *smlTree = smlGetProcParameterResponse->parameter_tree->child_list[i];
					if (smlTree && smlTree->parameter_name)
					{
						unsigned char obis[6] = { 0x81, 0x81, 0xc7, 0x82, 0x05, 0xff };
						if (memcmp(smlTree->parameter_name->str, obis, sizeof(obis)) == 0)
						{
							sml_proc_par_value *smlProcParValue = smlTree->parameter_value;
							if (smlProcParValue && (*(smlProcParValue->tag) == SML_PROC_PAR_VALUE_TAG_VALUE))
							{
								sml_value *smlValue = smlProcParValue->data.value;
								if (smlValue->type == SML_TYPE_OCTET_STRING)
								{
									octet_string *pubKey = NULL;

									pubKey = sml_octet_string_init(smlValue->data.bytes->str, smlValue->data.bytes->len);
									sml_octet_string_free(pubKey);
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
sml_file *ReadSml(char *fileName, int flags)
{
	sml_file *smlFile = NULL;
	int rv, size;
	unsigned char *buf = NULL;
	unsigned char *mem = NULL;

	FILE* fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		printf("Could not open file: %s\r\n", fileName);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	buf = (unsigned char *)malloc(size);
	if (!buf)
	{
		printf("Could not allocate memory: %i\r\n", size);
		return NULL;
	}

	//rv = fread(buf, 1, size, fp);
	//sml_file *smlFile = sml_file_parse(buf + 8, rv - 16);

	//rv = sml_transport_read_file(fp, buf, size);

	mem = (unsigned char *)malloc(size);
	if (!mem)
	{
		printf("Could not allocate memory: %i\r\n", size);
		return NULL;
	}
	rv = fread(mem, 1, size, fp);
	rv = sml_transport_read_mem(mem, buf, size);
	if (rv > 0)
	{
		smlFile = sml_file_parse(buf, rv, flags);
		if (smlFile)
		{
			sml_file_print(smlFile);

			printf("ReadSml: %s (%i)\n", sml_get_errorstring(sml_file_get_error(smlFile)), sml_file_get_error(smlFile));

			VerifySmlSignatures(smlFile);
		}
	}

	if (buf)
		free(buf);
	if (mem)
		free(mem);

	fclose(fp);

	return smlFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WriteSml(sml_file *smlFile, char *fileName, int flags)
{
	int rv;

	if (!smlFile || !fileName)
	{
		printf("Bad parameter: %s,%s\r\n", smlFile, fileName);
		return -1;
	}

	FILE* fp = fopen(fileName, "wb");
	if (fp == NULL)
	{
		printf("Could not open file: %s\r\n", fileName);
		return -1;
	}

	sml_file_add_flags(smlFile, flags);
	rv = sml_transport_write_file(NULL, smlFile);
	if (rv > 0)
	{
		fwrite(sml_file_get_buffer(smlFile), 1, sml_file_get_buffer_len(smlFile), fp);
	}
	printf("WriteSml: %s (%i)\n", sml_get_errorstring(sml_file_get_error(smlFile)), sml_file_get_error(smlFile));

	fclose(fp);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
sml_file *GenerateSmlCryptoReset(int flags)
{
	sml_file *smlFile = sml_file_init(flags);
	if (smlFile)
	{
		unsigned char server_id[] = {0x09, 0x01, 0x44, 0x4E, 0x54, 0x11, 0x02, 0x63, 0x6D, 0x5C};
		unsigned char client_id[] = {0x45, 0x53, 0x2D, 0x48, 0x44, 0x4C, 0x43, 0x2D, 0x42, 0x72, 0x69, 0x64, 0x67, 0x65};

		// build unique transaction id
		octet_string *transactionId = sml_octet_string_generate_uuid();

		// build SML_MESSAGE_OPEN_REQUEST
		sml_message *smlMessageOpenRequest = sml_message_init();
		if (smlMessageOpenRequest->transaction_id)
		{
			sml_octet_string_free(smlMessageOpenRequest->transaction_id);
		}
		smlMessageOpenRequest->transaction_id = sml_octet_string_init(transactionId->str, transactionId->len);

		sml_open_request *smlOpenRequest = sml_open_request_init();
		smlOpenRequest->server_id = sml_octet_string_init(server_id, sizeof(server_id));
		smlOpenRequest->client_id = sml_octet_string_init(client_id, sizeof(client_id));
		smlOpenRequest->sml_version = sml_u8_init(1);

		smlMessageOpenRequest->message_body = sml_message_body_init(SML_MESSAGE_OPEN_REQUEST, smlOpenRequest);

		sml_file_add_message(smlFile, smlMessageOpenRequest);

		// build SML_MESSAGE_SET_PROC_PARAMETER_REQUEST
		sml_message *smlMessageSetProcParameterRequest = sml_message_init();
		if (smlMessageSetProcParameterRequest->transaction_id)
		{
			sml_octet_string_free(smlMessageSetProcParameterRequest->transaction_id);
		}
		smlMessageSetProcParameterRequest->transaction_id = sml_octet_string_init(transactionId->str, transactionId->len);

		sml_set_proc_parameter_request *smlSetProcParameterRequest = sml_set_proc_parameter_request_init();
		smlSetProcParameterRequest->server_id = sml_octet_string_init(server_id, sizeof(server_id));

		smlSetProcParameterRequest->parameter_tree_path = sml_tree_path_init();

		sml_tree *smlTree = sml_tree_init();
		unsigned char uc[] = {0x01, 0x00, 0x5E, 0x31, 0x00, 0x07};
		smlTree->parameter_name = sml_octet_string_init(uc, sizeof(uc));

		sml_proc_par_value *procParValue = sml_proc_par_value_init();
		sml_value *val = sml_value_init();

		val->type = SML_TYPE_BOOLEAN;
		val->data.boolean = sml_boolean_init(SML_BOOLEAN_TRUE);

		procParValue->tag = sml_u8_init(SML_PROC_PAR_VALUE_TAG_VALUE);
		procParValue->data.value = val;
		smlTree->parameter_value = procParValue;

		smlSetProcParameterRequest->parameter_tree = smlTree;

		smlMessageSetProcParameterRequest->message_body = sml_message_body_init(SML_MESSAGE_SET_PROC_PARAMETER_REQUEST, smlSetProcParameterRequest);

		sml_file_add_message(smlFile, smlMessageSetProcParameterRequest);

		// build SML_MESSAGE_CLOSE_REQUEST
		sml_message *smlMessageCloseRequest = sml_message_init();
		if (smlMessageCloseRequest->transaction_id)
		{
			sml_octet_string_free(smlMessageCloseRequest->transaction_id);
		}
		smlMessageCloseRequest->transaction_id = sml_octet_string_init(transactionId->str, transactionId->len);

		sml_close_request *smlCloseRequest = sml_close_request_init();

		smlMessageCloseRequest->message_body = sml_message_body_init(SML_MESSAGE_CLOSE_REQUEST, smlCloseRequest);

		sml_file_add_message(smlFile, smlMessageCloseRequest);

		// final stuff
		sml_file_print(smlFile);

		sml_octet_string_free(transactionId);

		printf("GenerateSmlCryptoReset: %s (%i)\n", sml_get_errorstring(sml_file_get_error(smlFile)), sml_file_get_error(smlFile));
	}

	return smlFile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int trace_cb(char *format, va_list arguments)
{
	int ret = 0;

	if (format)
	{
		ret = vprintf(format, arguments);
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, char* argv[])
{
	int badarg = 0;
	int verbose = 0;
	int cr = 0;
	char *fileNameIn = NULL;
	char *fileNameOut = NULL;
	sml_file *smlFile = NULL;
	int flags = SML_FLAGS_DEBUG_OUTPUT | SML_FLAGS_ISKRA_BZ_IGNORE_BUGS;

#ifdef WIN32
	_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF|_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
	//_CrtSetBreakAlloc(98);
#endif

	argv++;
	while (!badarg && *argv && *argv[0] == '-')
	{
		if (!strcmp (*argv, "-v"))
		{
			verbose = 1;
		}
		else if (!strcmp (*argv, "-cr"))
		{
			cr = 1;
		}
		else if (!strcmp(*argv,"-r"))
		{
			if (!argv[1])
			{
				badarg = 1;
				break;
			}
			argv++;
			fileNameIn = *argv;
		}
		else if (!strcmp(*argv,"-w"))
		{
			if (!argv[1])
			{
				badarg = 1;
				break;
			}
			argv++;
			fileNameOut = *argv;
		}
		else if (!strcmp(*argv,"-f"))
		{
			if (!argv[1])
			{
				badarg = 1;
				break;
			}
			argv++;
			flags |= atoi(*argv);
		}
		argv++;
	}

	if (badarg)
	{
		printf("Usage libsmlTest [options] file.sml ...\r\n");
		printf("where options are\n");
		printf("-v          verbose\n");
		printf("-cr         generate crypto reset message\r\n");
		printf("-r file     read sml messages from file\r\n");
		printf("-w file     write sml messages to file\r\n");
		printf("-f flags    control flags\r\n");
	}

	//sml_set_trace_cb(SML_TRACE_PRINTF, NULL);
	sml_set_trace_cb(SML_TRACE_CB, trace_cb);

	if (cr)
	{
		smlFile = GenerateSmlCryptoReset(flags);
	}
	else if (fileNameIn)
	{
		smlFile = ReadSml(fileNameIn, flags);
	}

	if (fileNameOut)
	{
		WriteSml(smlFile, fileNameOut, flags);
	}

	if (smlFile)
	{
		sml_file_free(smlFile);
	}

	//getchar();

	return 0;
}

