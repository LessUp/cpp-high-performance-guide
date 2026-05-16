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

test('style.css keeps tokenized selectors for the live homepage, Mermaid, and SVG surfaces', () => {
  const css = read('style.css')

  for (const token of [
    '--wp-paper-1',
    '--wp-ink-1',
    '--wp-panel-bg',
    '--wp-figure-bg',
    '--wp-meta-bg',
    '--wp-section-index-bg',
    '--wp-section-index-border',
    '--wp-surface-1',
    '--wp-surface-2',
    '--wp-surface-section-index',
    '--wp-pill-bg',
    '--wp-diagram-stroke',
    '--wp-icon-muted',
  ]) {
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
    '.wp-hero',
    '.wp-metric-strip',
    '.wp-section-index',
    '.wp-figure-shell',
    '.wp-meta-strip',
    '.wp-reference-list',
    '.vp-doc .mermaid',
    ".vp-doc :where(svg [stroke='currentColor']",
  ]) {
    assert.match(css, new RegExp(selector.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('style.css suppresses VitePress built-in translation widgets in favor of the custom switcher', () => {
  const css = read('style.css')

  for (const selector of [
    '.VPNavBarTranslations',
    '.VPNavBarExtra .translations',
    '.VPNavScreenTranslations',
  ]) {
    assert.match(css, new RegExp(selector.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }
})

test('language switcher markup keeps native navigation when JavaScript is unavailable', () => {
  const switcher = read('LanguageSwitcher.vue')

  assert.match(switcher, /<details\b/)
  assert.match(switcher, /<a\b/)
  assert.match(switcher, /:href=/)
})

test('theme index wires only the active language chrome', () => {
  const themeIndex = read('index.ts')

  for (const componentName of ['LanguageRedirect', 'LanguageSwitcher', 'SectionHero', 'MetricStrip', 'SectionIndex']) {
    assert.match(themeIndex, new RegExp(componentName))
  }
})

test('bilingual landing pages preserve copy while using shared whitepaper primitives', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  for (const content of [enIndex, zhIndex]) {
    assert.match(content, /<SectionHero\b/)
    assert.match(content, /<MetricStrip\b/)
    assert.match(content, /<SectionIndex\b/)
    assert.doesNotMatch(content, /class="home-header"/)
    assert.doesNotMatch(content, /class="home-intro-row"/)
    assert.doesNotMatch(content, /class="feature-map"/)
  }

  assert.match(enIndex, /title="C\+\+ High Performance Guide"/)
  assert.match(enIndex, /A practical C\+\+20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling\./)
  assert.match(enIndex, /title="Quick Start"/)
  assert.match(zhIndex, /title="C\+\+ 高性能指南"/)
  assert.match(zhIndex, /一份实用的 C\+\+20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。/)
  assert.match(zhIndex, /title="快速开始"/)
})
