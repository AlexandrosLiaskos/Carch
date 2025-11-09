#!/bin/bash
# Run performance benchmarks and generate report

set -e

ITERATIONS=${1:-5}
BASELINE=${2:-""}
OUTPUT=${3:-"benchmark-results.md"}

echo "=== Carch Performance Benchmarks ==="
echo "Iterations: $ITERATIONS"
echo ""

# Build with optimizations
if [ ! -f "build/tests/performance_tests" ]; then
    echo "Building performance tests..."
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
    cmake --build build --target performance_tests
fi

# Run benchmarks multiple times
echo "Running benchmarks..."
RESULTS=()

for i in $(seq 1 $ITERATIONS); do
    echo "  Run $i/$ITERATIONS..."
    ./build/tests/performance_tests > "benchmark_run_$i.txt"
    RESULTS+=("benchmark_run_$i.txt")
done

# Generate report
echo ""
echo "Generating report: $OUTPUT"

cat > "$OUTPUT" << 'EOF'
# Carch Performance Benchmark Results

## Test Environment

- Date: $(date)
- Platform: $(uname -a)
- Compiler: $(c++ --version | head -n1)
- CMake: $(cmake --version | head -n1)

## Results

EOF

# Add results from first run (could average across runs)
cat "${RESULTS[0]}" >> "$OUTPUT"

# Compare with baseline if provided
if [ -n "$BASELINE" ] && [ -f "$BASELINE" ]; then
    echo "" >> "$OUTPUT"
    echo "## Comparison with Baseline" >> "$OUTPUT"
    echo "" >> "$OUTPUT"
    echo "Baseline: $BASELINE" >> "$OUTPUT"
    # Could add diff logic here
fi

# Cleanup
rm -f benchmark_run_*.txt

echo ""
echo "✓ Benchmark complete. Report: $OUTPUT"
