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
    '--wp-surface-1',
    '--wp-surface-2',
    '--wp-pill-bg',
    '--wp-diagram-stroke',
    '--wp-icon-muted',
  ]) {
    assert.match(css, new RegExp(token.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))
  }

  for (const selector of [
    '.home-header',
    '.home-header-left',
    '.home-logo',
    '.home-title',
    '.home-subtitle',
    '.home-nav a',
    '.home-intro-row',
    '.home-intro',
    '.home-stats',
    '.feature-card',
    '.quick-start',
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

test('theme index wires only the active language chrome', () => {
  const themeIndex = read('index.ts')

  for (const componentName of ['LanguageRedirect', 'LanguageSwitcher']) {
    assert.match(themeIndex, new RegExp(componentName))
  }

  assert.doesNotMatch(themeIndex, /FigureFrame|MetricStrip|ReferenceList|SectionIndex|SectionHero/)
})

test('bilingual landing pages retain the pre-theme-system content while theme primitives remain optional', () => {
  const docsRoot = path.resolve(themeDir, '..', '..')
  const enIndex = fs.readFileSync(path.join(docsRoot, 'en', 'index.md'), 'utf8')
  const zhIndex = fs.readFileSync(path.join(docsRoot, 'zh', 'index.md'), 'utf8')

  for (const content of [enIndex, zhIndex]) {
    assert.equal(/<SectionHero\b/.test(content), false)
    assert.equal(/<MetricStrip\b/.test(content), false)
    assert.equal(/<SectionIndex\b/.test(content), false)
  }

  assert.match(enIndex, /<div class="home-header">/)
  assert.match(enIndex, /A practical C\+\+20 guide to builds, memory layout, SIMD, concurrency, benchmarking, and profiling\./)
  assert.match(enIndex, /<div class="quick-start">/)
  assert.match(zhIndex, /<div class="home-header">/)
  assert.match(zhIndex, /一份实用的 C\+\+20 指南，涵盖构建系统、内存布局、SIMD、并发、基准测试和性能分析。/)
  assert.match(zhIndex, /<div class="quick-start">/)
})
