import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

// Ensures source-file paths mentioned in the docs (e.g. `include/hpc/foo.hpp`,
// `examples/02-memory-cache/src/bar.cpp`) still point at real files. This
// catches the documentation drift that occurs when code is moved or renamed
// (for example, when example headers were promoted into include/hpc/). It runs
// as part of `npm test` and therefore in the CI docs job.

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const docsRoot = path.resolve(__dirname, '..', '..') // docs/
const projectRoot = path.resolve(docsRoot, '..') // repo root

const mdFiles = []
function walk(dir) {
  for (const entry of fs.readdirSync(dir, { withFileTypes: true })) {
    const full = path.join(dir, entry.name)
    if (entry.isDirectory()) walk(full)
    else if (entry.name.endsWith('.md')) mdFiles.push(full)
  }
}
walk(path.join(docsRoot, 'zh'))

// Prose path references. Code-fence `<hpc/...>` includes do not match these
// patterns (no leading `include/`), so only explicit file paths are checked.
const PATH_PATTERNS = [
  /examples\/[0-9][0-9]-[a-z-]*\/[a-zA-Z0-9_/]*\.(?:hpp|cpp)/g,
  /include\/hpc\/[a-z_]*\.hpp/g,
]

const missing = []
const checked = new Set()

for (const file of mdFiles) {
  const content = fs.readFileSync(file, 'utf8')
  for (const pattern of PATH_PATTERNS) {
    for (const match of content.matchAll(pattern)) {
      const ref = match[0]
      if (checked.has(ref)) continue
      checked.add(ref)
      if (!fs.existsSync(path.join(projectRoot, ref))) {
        missing.push({ ref, file: path.relative(docsRoot, file) })
      }
    }
  }
}

test('markdown source-path references resolve to real files', () => {
  assert.deepEqual(
    missing,
    [],
    `文档引用了不存在的源码路径（代码已移动/重命名，文档未同步）:\n${missing
      .map((m) => `  ${m.ref}  (in ${m.file})`)
      .join('\n')}`
  )
})
