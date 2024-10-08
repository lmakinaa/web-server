#!/bin/bash

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

if [ -z "$1" ]
then
  echo -e "${RED}Usage: ./server_tester.sh <number of requests>${NC}"
  exit 1
fi

# Remove the responses file if it exists
rm -f responses.txt

for i in $(seq 1 $1)
do
  curl -s -H 'Cache-Control: no-cache' localhost:8080 >> responses.txt &
done

wait

echo "Finished sendind requests, now evaluating results";

success=0

while IFS= read -r line
do
  echo $line | grep -q "<h1>Hi!</h1>"
  if [ $? -eq 0 ]
  then
    ((success++))
  fi
done < responses.txt

echo -e "${GREEN}$success out of $1 requests were successful.${NC}"

# Remove the responses file
# rm -f responses.txt