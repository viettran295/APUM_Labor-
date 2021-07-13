################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sml/src/sml_attention_response.c \
../sml/src/sml_boolean.c \
../sml/src/sml_close_request.c \
../sml/src/sml_close_response.c \
../sml/src/sml_crc16.c \
../sml/src/sml_file.c \
../sml/src/sml_get_list_request.c \
../sml/src/sml_get_list_response.c \
../sml/src/sml_get_proc_parameter_request.c \
../sml/src/sml_get_proc_parameter_response.c \
../sml/src/sml_get_profile_list_request.c \
../sml/src/sml_get_profile_list_response.c \
../sml/src/sml_get_profile_pack_request.c \
../sml/src/sml_get_profile_pack_response.c \
../sml/src/sml_list.c \
../sml/src/sml_log.c \
../sml/src/sml_message.c \
../sml/src/sml_number.c \
../sml/src/sml_octet_string.c \
../sml/src/sml_open_request.c \
../sml/src/sml_open_response.c \
../sml/src/sml_set_proc_parameter_request.c \
../sml/src/sml_shared.c \
../sml/src/sml_signature.c \
../sml/src/sml_status.c \
../sml/src/sml_time.c \
../sml/src/sml_transport.c \
../sml/src/sml_tree.c \
../sml/src/sml_v105.c \
../sml/src/sml_value.c 

OBJS += \
./sml/src/sml_attention_response.o \
./sml/src/sml_boolean.o \
./sml/src/sml_close_request.o \
./sml/src/sml_close_response.o \
./sml/src/sml_crc16.o \
./sml/src/sml_file.o \
./sml/src/sml_get_list_request.o \
./sml/src/sml_get_list_response.o \
./sml/src/sml_get_proc_parameter_request.o \
./sml/src/sml_get_proc_parameter_response.o \
./sml/src/sml_get_profile_list_request.o \
./sml/src/sml_get_profile_list_response.o \
./sml/src/sml_get_profile_pack_request.o \
./sml/src/sml_get_profile_pack_response.o \
./sml/src/sml_list.o \
./sml/src/sml_log.o \
./sml/src/sml_message.o \
./sml/src/sml_number.o \
./sml/src/sml_octet_string.o \
./sml/src/sml_open_request.o \
./sml/src/sml_open_response.o \
./sml/src/sml_set_proc_parameter_request.o \
./sml/src/sml_shared.o \
./sml/src/sml_signature.o \
./sml/src/sml_status.o \
./sml/src/sml_time.o \
./sml/src/sml_transport.o \
./sml/src/sml_tree.o \
./sml/src/sml_v105.o \
./sml/src/sml_value.o 

C_DEPS += \
./sml/src/sml_attention_response.d \
./sml/src/sml_boolean.d \
./sml/src/sml_close_request.d \
./sml/src/sml_close_response.d \
./sml/src/sml_crc16.d \
./sml/src/sml_file.d \
./sml/src/sml_get_list_request.d \
./sml/src/sml_get_list_response.d \
./sml/src/sml_get_proc_parameter_request.d \
./sml/src/sml_get_proc_parameter_response.d \
./sml/src/sml_get_profile_list_request.d \
./sml/src/sml_get_profile_list_response.d \
./sml/src/sml_get_profile_pack_request.d \
./sml/src/sml_get_profile_pack_response.d \
./sml/src/sml_list.d \
./sml/src/sml_log.d \
./sml/src/sml_message.d \
./sml/src/sml_number.d \
./sml/src/sml_octet_string.d \
./sml/src/sml_open_request.d \
./sml/src/sml_open_response.d \
./sml/src/sml_set_proc_parameter_request.d \
./sml/src/sml_shared.d \
./sml/src/sml_signature.d \
./sml/src/sml_status.d \
./sml/src/sml_time.d \
./sml/src/sml_transport.d \
./sml/src/sml_tree.d \
./sml/src/sml_v105.d \
./sml/src/sml_value.d 


# Each subdirectory must supply rules for building sources it contributes
sml/src/%.o: ../sml/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/punk/workspace_new/libSML/sml/include/sml -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


