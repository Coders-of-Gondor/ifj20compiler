#!/usr/bin/env bash

# pouziti: ./t.sh testname

SUPPORT_FILE=ifj20.go
MAIN_FILE=$1
go run ${MAIN_FILE} ${SUPPORT_FILE}
