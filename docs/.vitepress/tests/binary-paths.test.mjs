import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

// Ensures executable paths referenced in the docs and README (e.g.
// `./build/release/examples/02-memory-cache/aos_vs_soa_bench`) correspond to
// real CMake targets. Executables are emitted flat into
// build/<preset>/examples/<module>/ (there is no src/ or bench/ output
// subdirectory), and benchmark targets are named `<NAME>_bench`. Both facts
// drifted in the docs before after the target-naming refactor; this test runs
// as part of `npm test` (CI docs job) to keep them pinned.

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const docsRoot = path.resolve(__dirname, '..', '..') // docs/
const projectRoot = path.resolve(docsRoot, '..') // repo root

// --- Collect real targets from examples/*/CMakeLists.txt ---------------------

const targets = new Set()
const examplesDir = path.join(projectRoot, 'examples')

for (const entry of fs.readdirSync(examplesDir, { withFileTypes: true })) {
  if (!entry.isDirectory() || !/^[0-9][0-9]-/.test(entry.name)) continue
  const cmakeFile = path.join(examplesDir, entry.name, 'CMakeLists.txt')
  if (!fs.existsSync(cmakeFile)) continue
  const content = fs.readFileSync(cmakeFile, 'utf8')
  // hpc_add_example(NAME foo ...) → `foo`, plus `foo_bench` when
  // BENCHMARK_SOURCES is given. hpc_add_benchmark(NAME foo ...) → `foo`.
  for (const call of content.matchAll(/hpc_add_(example|benchmark)\s*\(([^)]*)\)/g)) {
    const [, kind, body] = call
    const name = /NAME\s+([A-Za-z0-9_]+)/.exec(body)?.[1]
    if (!name) continue
    targets.add(name)
    if (kind === 'example' && /BENCHMARK_SOURCES/.test(body)) {
      targets.add(`${name}_bench`)
    }
  }
}

// --- Collect referenced binaries from docs + README --------------------------

const mdFiles = [path.join(projectRoot, 'README.md')]
function walk(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const full = path.join(dir, entry.name)
    if (entry.isDirectory()) walk(full)
    else if (entry.name.endsWith('.md')) mdFiles.push(full)
  }
}
walk(path.join(docsRoot, 'zh'))

const problems = []
const checked = new Set()

for (const file of mdFiles) {
  const content = fs.readFileSync(file, 'utf8')
  const where = path.relative(projectRoot, file)

  // Binary references: build/<preset>/examples/<module>/<binary>. The binary
  // token stops at whitespace/backticks, so trailing flags are not captured.
  for (const match of content.matchAll(
    /build\/(?:release|debug)\/examples\/([0-9][0-9]-[a-z-]*)\/([A-Za-z0-9_.-]+)/g
  )) {
    const [, module, binary] = match
    const key = `${module}/${binary}`
    if (checked.has(key)) continue
    checked.add(key)
    if (!targets.has(binary)) {
      problems.push({ key, where, reason: '无此 CMake 目标（目标已重命名？）' })
    }
  }

  // Binaries are emitted flat into the module directory; src/ and bench/ are
  // source-tree directories only and never appear in output paths.
  for (const match of content.matchAll(
    /build\/(?:release|debug)\/examples\/[0-9][0-9]-[a-z-]*\/(?:src|bench)\/[A-Za-z0-9_.-]+/g
  )) {
    problems.push({ key: match[0], where, reason: '二进制路径含 src/ 或 bench/ 前缀（输出目录无此层级）' })
  }
}

test('markdown binary-path references resolve to real CMake targets', () => {
  assert.ok(targets.size > 0, '未能从 examples/*/CMakeLists.txt 解析出任何目标——解析器坏了吗？')
  assert.deepEqual(
    problems,
    [],
    `文档引用了不存在的可执行文件路径（目标重命名/移动后文档未同步）:\n${problems
      .map((p) => `  ${p.key}  (in ${p.where}) — ${p.reason}`)
      .join('\n')}`
  )
})
