.PHONY: help build build-image build-image-linux run clean proxmox

help: ## Display this help screen
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'


clean: ## Clean artifacts
	rm -f main
	rm -f board-game-escape.tar

tidy: ## go mod tidy
	pushd server/src && go mod tidy

build: clean ## Build main executable
	pushd server/src && go build -o main .

run: ## go run main.go
	pushd server/src && go run .

run-docker: ## Run docker image
	docker-compose up --build

build-image: ## Builds docker image (local machine)
	docker build -f server/Dockerfile -t board-game-escape:latest server

build-image-linux: ## Builds docker image (linux platform)
	docker buildx build --platform linux/amd64 -f server/Dockerfile -t board-game-escape:latest --load server

build-oci-image: build-image-linux ## Builds Docker image and exports to OCI
	skopeo copy \
	--override-arch amd64 \
	--override-os linux \
	--insecure-policy \
	docker-daemon:board-game-escape:latest \
	oci-archive:board-game-escape.tar

push-oci-image: build-oci-image ## Builds docker image, exports to OCI and uploads to Proxmox
	scp board-game-escape.tar root@192.168.4.4:/var/lib/vz/template/cache/
	make clean
