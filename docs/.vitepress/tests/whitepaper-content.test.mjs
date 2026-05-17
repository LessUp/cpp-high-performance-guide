import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const docsRoot = path.resolve(__dirname, '..', '..')

function read(relativePath) {
  return fs.readFileSync(path.join(docsRoot, relativePath), 'utf8')
}

test('rewritten English whitepaper pages no longer contain stub markers', () => {
  for (const relativePath of [
    'en/index.md',
    'en/academy/index.md',
    'en/academy/module-atlas.md',
    'en/academy/validation-doctrine.md',
    'en/architecture/index.md',
    'en/architecture/repository-topology.md',
    'en/architecture/performance-methodology.md',
    'en/playbook/index.md',
    'en/reference/index.md',
    'en/research/index.md',
    'en/research/related-work.md',
    'en/research/references.md',
    'en/research/evolution.md',
  ]) {
    const content = read(relativePath)
    assert.doesNotMatch(content, /\bstub\b/i)
    assert.doesNotMatch(content, /Task 3/i)
    assert.doesNotMatch(content, /Later tasks will/i)
  }
})

test('landing page states the stronger thesis and validation ladder', () => {
  const content = read('en/index.md')

  assert.match(content, /performance advice as something to compile, test, benchmark, and falsify/i)
  assert.match(content, /debug→ubsan/)
  assert.match(content, /Validation claims/)
  assert.match(content, /Expert reader callouts/)
})

test('architecture pages describe preset-driven validation and profiling methodology', () => {
  const architectureIndex = read('en/architecture/index.md')
  const methodology = read('en/architecture/performance-methodology.md')

  assert.match(architectureIndex, /Preset-driven validation loop/)
  assert.match(architectureIndex, /cmake --preset=tsan && cmake --build build\/tsan && ctest --preset=tsan/)
  assert.match(methodology, /relwithdebinfo/)
  assert.match(methodology, /perf record -g --call-graph dwarf/)
  assert.match(methodology, /Google Benchmark/)
})

test('benchmark command references use real build output paths', () => {
  const methodology = read('en/architecture/performance-methodology.md')
  const doctrine = read('en/academy/validation-doctrine.md')

  for (const content of [methodology, doctrine]) {
    assert.doesNotMatch(content, /\/bench\//)
  }

  assert.match(methodology, /\.\/build\/release\/examples\/02-memory-cache\/aos_vs_soa_bench/)
  assert.match(methodology, /\.\/build\/release\/examples\/04-simd-vectorization\/simd_bench/)
  assert.match(methodology, /\.\/build\/relwithdebinfo\/examples\/04-simd-vectorization\/simd_bench/)
  assert.match(methodology, /\.\/tools\/performance\/generate_flamegraph\.sh \.\/build\/relwithdebinfo\/examples\/02-memory-cache\/aos_vs_soa_bench/)
  assert.match(doctrine, /\.\/build\/release\/examples\/02-memory-cache\/aos_vs_soa_bench/)
})

test('research pages cite concrete external repositories and references', () => {
  const relatedWork = read('en/research/related-work.md')
  const references = read('en/research/references.md')
  const evolution = read('en/research/evolution.md')

  for (const needle of [
    'https://github.com/google/benchmark',
    'https://github.com/brendangregg/FlameGraph',
    'https://github.com/xtensor-stack/xsimd',
    'https://github.com/oneapi-src/oneTBB',
    'https://github.com/facebook/folly',
    'https://github.com/abseil/abseil-cpp',
    'https://www.agner.org/optimize/',
    'https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html',
    'https://vitepress.dev/',
  ]) {
    assert.ok(
      relatedWork.includes(needle) || references.includes(needle) || evolution.includes(needle),
      needle,
    )
  }
})

test('module atlas uses explicit repository paths for supporting headers', () => {
  const moduleAtlas = read('en/academy/module-atlas.md')

  for (const needle of [
    'examples/03-modern-cpp/include/buffer.hpp',
    'examples/03-modern-cpp/include/compile_time.hpp',
    'examples/03-modern-cpp/include/ranges_utils.hpp',
    'examples/03-modern-cpp/include/vector_reserve.hpp',
    'examples/04-simd-vectorization/include/simd_utils.hpp',
    'examples/04-simd-vectorization/include/simd_wrapper.hpp',
    'examples/05-concurrency/include/concurrency_utils.hpp',
    'examples/05-concurrency/include/lock_free_queue.hpp',
  ]) {
    assert.match(moduleAtlas, new RegExp(needle.replaceAll('/', '\\/')))
  }

  for (const bareName of [
    '`buffer.hpp`',
    '`compile_time.hpp`',
    '`ranges_utils.hpp`',
    '`vector_reserve.hpp`',
    '`simd_utils.hpp`',
    '`simd_wrapper.hpp`',
    '`concurrency_utils.hpp`',
    '`lock_free_queue.hpp`',
  ]) {
    assert.ok(!moduleAtlas.includes(bareName), bareName)
  }
})
