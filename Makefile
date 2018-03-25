# This file automates the release process adhering to semantic versioning.
# Tasks supported are git tagging and docker image building and pushing.
# Before calling `make release` update the version number in the VERSION file.

NAME    := sourcey/libsourcey
VERSION := $$(cat VERSION)

release: | git_tag docker_build docker_push

git_tag:
	git add VERSION
	git add . --update
	git commit --message="Release ${VERSION}"
	git tag --annotate --message="Release ${VERSION}" ${VERSION}
	git push --follow-tags

docker_build:
	docker build -t ${NAME}:latest -t ${NAME}:v${VERSION} .

docker_push:
	docker push ${NAME}

docker_login:
	docker log -u ${DOCKER_USER} -p ${DOCKER_PASS}
