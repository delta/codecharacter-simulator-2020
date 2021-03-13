#!/bin/bash
# Needed by circleci local build tool
exec < /dev/tty

echo "## Running circleci config validation ##"
# Referred:  https://circleci.com/blog/circleci-hacks-validate-circleci-config-on-every-commit-with-a-git-hook/
# Validates circleci config file
if ! msg=$(circleci config validate -c .circleci/config.yml); then
	echo "Failed circleci config validation"
	exit 1
else
	echo $msg
	exit 0
fi
