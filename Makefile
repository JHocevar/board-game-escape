.PHONY: help build build-image build-image-linux run clean proxmox frontend-install frontend-build

help: ## Display this help screen
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'


clean: ## Clean artifacts
	rm -f main
	rm -f board-game-escape.tar
	rm -rf frontend/build frontend/.svelte-kit

tidy: ## go mod tidy
	pushd server/src && go mod tidy

frontend-install: ## Install frontend dependencies
	cd frontend && npm install

frontend-build: frontend-install ## Build the Svelte SPA
	cd frontend && npm run build

build: clean frontend-build ## Build the application and frontend assets
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
