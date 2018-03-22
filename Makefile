NAME    := sourcey/libsourcey
VERSION := $$(cat VERSION)

git_release:
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
