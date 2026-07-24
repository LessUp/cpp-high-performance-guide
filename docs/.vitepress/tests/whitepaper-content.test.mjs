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

test('Chinese content pages do not contain placeholder markers', () => {
  for (const relativePath of [
    'zh/index.md',
  ]) {
    const content = read(relativePath)
    assert.doesNotMatch(content, /占位/)
    assert.doesNotMatch(content, /Task 3/i)
    assert.doesNotMatch(content, /后续任务/)
    assert.doesNotMatch(content, /第一版参考资料书架/)
  }
})

test('no markdown files reference deleted sections', () => {
  const zhDir = path.join(docsRoot, 'zh')
  const mdFiles = []

  function walk(dir) {
    for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
      const full = path.join(dir, entry.name)
      if (entry.isDirectory()) walk(full)
      else if (entry.name.endsWith('.md')) mdFiles.push(full)
    }
  }

  walk(zhDir)

  const deletedSections = [
    '/zh/algorithms/',
    '/zh/exercises/',
    '/zh/academy/',
    '/zh/architecture/',
    '/zh/research/',
    '/zh/playbook/',
    '/zh/contributing/',
  ]

  for (const file of mdFiles) {
    const content = fs.readFileSync(file, 'utf8')
    const relative = path.relative(docsRoot, file)
    for (const section of deletedSections) {
      assert.doesNotMatch(
        content,
        new RegExp(section.replace(/\//g, '\\/')),
        `${relative} references deleted section ${section}`
      )
    }
  }
})
