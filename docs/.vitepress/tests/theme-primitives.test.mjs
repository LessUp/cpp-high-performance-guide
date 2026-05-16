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

test('style.css defines reusable whitepaper primitives for panels, figures, metadata, references, and section indexes', () => {
  const css = read('style.css')

  for (const token of [
    '--wp-paper-1',
    '--wp-ink-1',
    '--wp-panel-bg',
    '--wp-figure-bg',
    '--wp-meta-bg',
  ]) {
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
    '.wp-panel',
    '.wp-figure',
    '.wp-meta-strip',
    '.wp-reference-list',
    '.wp-section-index',
    '.vp-doc .mermaid',
  ]) {
    assert.match(css, new RegExp(selector.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('theme index registers reusable whitepaper components', () => {
  const themeIndex = read('index.ts')

  for (const componentName of [
    'SectionHero',
    'MetricStrip',
    'FigureFrame',
    'ReferenceList',
  ]) {
    assert.match(themeIndex, new RegExp(componentName))
  }
})

test('bilingual landing pages use shared whitepaper theme components', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  for (const content of [enIndex, zhIndex]) {
    assert.match(content, /<SectionHero\b/)
    assert.match(content, /<MetricStrip\b/)
  }
})
