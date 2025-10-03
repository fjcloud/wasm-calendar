.PHONY: build clean serve

IMAGE_NAME = wasm-calendar-builder

build:
	@echo "Building WASM Calendar..."
	@mkdir -p docs
	podman build -t $(IMAGE_NAME):latest .
	podman run --rm -v $(PWD)/docs:/output:Z $(IMAGE_NAME):latest sh -c "cp -r /app/dist/* /output/"
	@echo ""
	@echo "✓ Build complete! Files in docs/"
	@echo ""
	@echo "To serve: make serve"
	@echo "Or: cd docs && python3 -m http.server 8080"

clean:
	@echo "Cleaning up..."
	rm -rf docs/*
	-podman rmi $(IMAGE_NAME):latest 2>/dev/null || true
	@echo "Cleanup complete."

serve:
	@echo "Starting server at http://localhost:8000"
	@python3 -m http.server -d docs

