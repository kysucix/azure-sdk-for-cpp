# Copyright(c) Microsoft Corporation.All rights reserved.
# SPDX - License - Identifier : MIT

cmake_minimum_required(VERSION 3.13)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(eventhub_sas_writer_sample)

add_executable(eventhub_token_writer_sample
    eventhub_token_writer_sample.cpp
)
add_dependencies(eventhub_token_writer_sample azure-core-amqp)

target_include_directories(eventhub_token_writer_sample PRIVATE ${azure-core-amqp_SOURCE_DIR} Azure::azure-identity)

target_link_libraries(eventhub_token_writer_sample PRIVATE azure-core-amqp Azure::azure-identity)
