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

test('research pages cite concrete external repositories and references', () => {
  const relatedWork = read('en/research/related-work.md')
  const references = read('en/research/references.md')

  for (const needle of [
    'https://github.com/google/benchmark',
    'https://github.com/brendangregg/FlameGraph',
    'https://github.com/xtensor-stack/xsimd',
    'https://github.com/oneapi-src/oneTBB',
    'https://github.com/facebook/folly',
    'https://github.com/abseil/abseil-cpp',
    'https://www.agner.org/optimize/',
    'https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html',
  ]) {
    assert.ok(relatedWork.includes(needle) || references.includes(needle), needle)
  }
})
