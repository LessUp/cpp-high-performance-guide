import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const packageJsonPath = path.resolve(__dirname, '..', '..', 'package.json')
const buildPagesScriptPath = path.resolve(__dirname, '..', 'scripts', 'build-pages.mjs')

test('build:pages uses a cross-platform Node wrapper for the GitHub Pages base path', () => {
  const packageJson = JSON.parse(fs.readFileSync(packageJsonPath, 'utf8'))
  const buildPagesScript = fs.readFileSync(buildPagesScriptPath, 'utf8')

  assert.equal(packageJson.scripts['build:pages'], 'node .vitepress/scripts/build-pages.mjs')
  assert.match(buildPagesScript, /VITEPRESS_BASE/)
  assert.match(buildPagesScript, /\/cpp-high-performance-guide\//)
  assert.match(buildPagesScript, /vitepress/)
})
