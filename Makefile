.PHONY:

# Targets
all: help

# Show this help message
help:
	@echo "Available commands:"
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_-]+:.*?## / {printf "  make %-10s - %s\n", $$1, $$2}' $(MAKEFILE_LIST)

compile: ## Compile the source code
	clang ./src/main.c -o ./build/build.out

run: ## Run the compiled program
	./build/build.out
