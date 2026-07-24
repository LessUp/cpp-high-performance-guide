import test from 'node:test'
import assert from 'node:assert/strict'
import fs from 'node:fs'
import path from 'node:path'
import { fileURLToPath } from 'node:url'

const __dirname = path.dirname(fileURLToPath(import.meta.url))
const themeDir = path.resolve(__dirname, '..', 'theme')

function read(relativePath) {
  return fs.readFileSync(path.join(themeDir, relativePath), 'utf8')
}

test('style.css defines VitePress brand variables', () => {
  const css = read('style.css')

  for (const token of [
    '--vp-c-brand-1',
    '--vp-c-brand-2',
    '--vp-c-brand-3',
    '--vp-c-brand-soft',
    '--vp-c-bg',
    '--vp-c-text-1',
    '--vp-c-border',
  ]) {
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('style.css provides dark mode overrides', () => {
  const css = read('style.css')
  assert.match(css, /\.dark/)
})

test('theme index extends DefaultTheme without custom components', () => {
  const themeIndex = read('index.ts')
  assert.match(themeIndex, /DefaultTheme/)
  assert.doesNotMatch(themeIndex, /Citation/)
  assert.doesNotMatch(themeIndex, /ComplexityBadge/)
  assert.doesNotMatch(themeIndex, /DiagramCanvas/)
  assert.doesNotMatch(themeIndex, /SectionIndex/)
  assert.doesNotMatch(themeIndex, /BaseAwareLink/)
})
