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

test('Chinese whitepaper pages do not contain placeholder markers', () => {
  for (const relativePath of [
    'zh/index.md',
    'zh/academy/index.md',
    'zh/academy/module-atlas.md',
    'zh/academy/validation-doctrine.md',
    'zh/architecture/index.md',
    'zh/architecture/repository-topology.md',
    'zh/architecture/performance-methodology.md',
    'zh/playbook/index.md',
    'zh/reference/index.md',
    'zh/research/index.md',
    'zh/research/related-work.md',
    'zh/research/references.md',
    'zh/research/evolution.md',
  ]) {
    const content = read(relativePath)
    assert.doesNotMatch(content, /占位/)
    assert.doesNotMatch(content, /Task 3/i)
    assert.doesNotMatch(content, /后续任务/)
    assert.doesNotMatch(content, /第一版参考资料书架/)
  }
})
