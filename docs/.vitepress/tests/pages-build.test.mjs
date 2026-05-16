import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const packageJsonPath = path.resolve(__dirname, '..', '..', 'package.json')

test('build:pages pins the GitHub Pages base path', () => {
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'))

  assert.match(packageJson.scripts['build:pages'], /VITEPRESS_BASE=\/cpp-high-performance-guide\//)
})
