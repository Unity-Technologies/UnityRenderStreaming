#!/bin/bash

#
# BOKKEN_DEVICE_IP: 
# TEMPLATE_FILE: 
# TEST_TARGET:
# TEST_PLATFORM:
# SCRIPTING_BACKEND:
# EXTRA_UTR_ARG:
# PACKAGE_DIR:
# TEST_PROJECT_DIR:
# TEST_RESULT_DIR:
# EDITOR_VERSION:
#
# 
# brew install gettext
#

export IDENTITY=~/.ssh/id_rsa_macmini

# install envsubst command
brew install gettext

# render template
envsubst '                                    \
  $WEBAPP_DIR                                 \
  $WEBAPP_NAME                                \
  $SCRIPTING_BACKEND                          \
  $EXTRA_UTR_ARG                              \
  $TEST_PROJECT_DIR                           \
  $TEST_TARGET                                \
  $TEST_PLATFORM                              \
  $EDITOR_VERSION'                            \
  < ${TEMPLATE_FILE}                          \
  > ~/remote.sh
chmod +x ~/remote.sh

# copy package to remote machine
scp -i ${IDENTITY} -r ${YAMATO_SOURCE_DIR} bokken@${BOKKEN_DEVICE_IP}:~/${PACKAGE_DIR}

# copy shell script to remote machine
scp -i ${IDENTITY} -r ~/remote.sh bokken@${BOKKEN_DEVICE_IP}:~/remote.sh

# run remote.sh on the remote machine
ssh -i ${IDENTITY} bokken@${BOKKEN_DEVICE_IP} ~/remote.sh
result=$?
if [ $result -ne 0 ]; then
  exit $result
fi

mkdir -p ${TEST_RESULT_DIR}
scp -i ${IDENTITY} -r bokken@${BOKKEN_DEVICE_IP}:~/test-results ${TEST_RESULT_DIR}